#include "ShoutThread.h"

#include <string>

ShoutThread::ShoutThread() {}

ShoutThread::~ShoutThread() {
    if (this->_helper)
        delete this->_helper;
}

void ShoutThread::_inst() {
    this->_helper = new OutputHelper(SHOUT_FILE_PATH, "uploadShout", "shout_file");
}

void ShoutThread::quit() {
    this->_mustListen = false;
}

rapidjson::Document ShoutThread::_createBasicShout() {
    // get iso date
    time_t now;
    time(&now);
    char buf[sizeof "2011-10-08T07:07:09Z"];
    strftime(buf, sizeof buf, "%FT%TZ", gmtime(&now));

    // return json obj
    rapidjson::Document obj;
    obj.Parse("{}");
    rapidjson::Document::AllocatorType &alloc = obj.GetAllocator();
    auto dateAsJSONVal = rapidjson::Value(buf, alloc);
    obj.AddMember("date", dateAsJSONVal, alloc);
    return obj;
}

void ShoutThread::shoutEmpty() {
    auto obj = this->_createBasicShout();
    const auto date = obj["date"].GetString();

    emit printLog(
        tr("%1: Shouting -> Nothing").arg(date)
    );

    this->_shoutToServer(obj);
}

void ShoutThread::shoutFilled(
        const QString &name,
        const QString &album,
        const QString &artist,
        const QString &genre,
        int duration,
        int playerPosition,
        bool playerState,
        int year
    ) {
    // fill obj
    auto obj = this->_createBasicShout();
    rapidjson::Document::AllocatorType &alloc = obj.GetAllocator();

    // factory for value generation
    auto valGen = [&alloc](QString defVal) {
        rapidjson::Value p(defVal.toStdString().c_str(), alloc);
        return p;
    };

    obj.AddMember("name", valGen(name), alloc);
    obj.AddMember("album", valGen(album), alloc);
    obj.AddMember("artist", valGen(artist), alloc);
    obj.AddMember("genre", valGen(genre), alloc);
    obj.AddMember("duration", duration, alloc);
    obj.AddMember("playerPosition", playerPosition, alloc);
    obj.AddMember("playerState", playerState, alloc);
    obj.AddMember("year", year, alloc);

    auto pState = obj["playerState"].GetBool() ? tr("playing") : tr("paused");

    // log...
    auto logMessage =
        tr("%1: Shouting -> %2 - %3 - %4 (%5)")
            .arg(obj["date"].GetString())
            .arg(obj["name"].GetString())
            .arg(obj["album"].GetString())
            .arg(obj["artist"].GetString())
            .arg(pState);

    emit printLog(logMessage);

    this->_shoutToServer(obj);
}

void ShoutThread::_shoutToServer(rapidjson::Document &incoming) {
    try {
        this->_helper->writeAsJsonFile(incoming);
        this->_helper->uploadFile();
    } catch(const std::exception& e) {
        emit printLog(e.what(), false, true);
    }
}

// compare with old shout, if equivalent, don't reshout
bool ShoutThread::shouldUpload(
        bool iPlayerState,
        const QString &tName,
        const QString &tAlbum,
        const QString &tArtist,
        const QString &tDatePlayed,
        const QString &tDateSkipped
    ) {
    // hash blueprint
    const auto currHash = QString::number(iPlayerState) + tName + tAlbum + tArtist + (tDatePlayed >= tDateSkipped ? tDatePlayed : tDateSkipped);

    // check if strings are identical
    bool isHashIdentical = (this->_lastTrackHash == currHash);

    // replace old hash with new
    this->_lastTrackHash = currHash;

    // if not identical, shout !
    return !isHashIdentical;
}


#ifdef APPLE

#include <unistd.h>
#include <QProcess>

void ShoutThread::run() {
    //
    this->_inst();

    emit printLog(tr("Waiting for iTunes to launch..."));

    // define applescript to get shout values
    Q_INIT_RESOURCE(resources);
    const auto scriptContent = QFile(":/mac/CurrentlyPlaying.applescript").readAll();

    // prepare script exec
    QProcess p;
    p.setProgram("/usr/bin/osascript");
    p.setArguments({ "-l", "AppleScript", "-s", "s"});
    p.write(scriptContent);
    p.closeWriteChannel();

    // loop until user said not to
    while (this->_mustListen) {
        // get shout results
        p.start();
        p.waitForReadyRead();
        auto result = p.readAll();
        p.waitForFinished();

        // default values and inst
        QString tName;
        QString tAlbum;
        QString tArtist;
        QString tGenre;
        int iDuration;
        int iPlayerPos;
        bool iPlayerState = false;
        // bool iRepeatMode;
        QString tDateSkipped;
        QString tDatePlayed;
        int tYear;

        // if has result
        if (result.size()) {
            // turn results into array
            result[0] = '[';
            result[result.size() - 1] = ']';

            // cast to json
            rapidjson::Document trackObj;
            trackObj.Parse(result.data());

            // get values for shout
            tName = trackObj[0].GetString();
            tAlbum = trackObj[1].GetString();
            tArtist = trackObj[2].GetString();
            tGenre = trackObj[3].GetString();
            iDuration = trackObj[4].GetInt();
            tYear = trackObj[5].GetInt();
            iPlayerPos = trackObj[6].GetInt();
            iPlayerState = trackObj[7].GetString() == "paused" ? 0 : 1;
            tDateSkipped = trackObj[8].GetString();
            tDatePlayed = trackObj[9].GetString();
        }

        // compare with old shout, if equivalent, don't reshout
        if (this->shouldUpload(iPlayerState, tName, tAlbum, tArtist, tDatePlayed, tDateSkipped)) {
            // if had results
            if (result.size()) {
                // say track infos
                this->shoutFilled(tName, tAlbum, tArtist, tGenre, iDuration, iPlayerPos, iPlayerState, tYear);
            } else {
                // say nothing happens
                this->shoutEmpty();
            }
        }

       // wait a bit before re-asking
       this->sleep(1);
    }

    this->shoutEmpty();
    emit printLog(tr("Stopped listening to iTunes."));
}
#endif

#ifdef _WIN32

#include "src/workers/shout/ShoutThread.h" 
#include "win/iTunesCOMHandler.h"

#include <windows.h>
#include <combaseapi.h>

#include <QMetaObject>
#include <QMetaMethod>
#include <QObject>
#include <QCoreApplication>
#include <QDebug>

void ShoutThread::run() {
    
    this->_inst();
    
    //start with log
    emit printLog(tr("Waiting for iTunes to launch..."));
    
    //prepare CLID
    HWND currentITunesWindowsHandler;
    DWORD currentProcessID;

    //iTunes IID extracted from Apple API
    wchar_t* wch = nullptr;
    HRESULT hr = ::StringFromCLSID({0xDC0C2640,0x1415,0x4644,{0x87,0x5C,0x6F,0x4D,0x76,0x98,0x39,0xBA}}, &wch);
    auto iTunesCLID = QString::fromWCharArray(wch);

    do {
        
        //search for iTunes...
        currentITunesWindowsHandler = FindWindowA(0, "iTunes");
        
        //if not found, wait and retry
        if(!currentITunesWindowsHandler) {
            this->sleep(1);
            continue;
        } 

        //iTunes found, store the associated PID
        GetWindowThreadProcessId(currentITunesWindowsHandler, &currentProcessID);
        
        //log..
        emit printLog(tr("Listening to iTunes !"));

        //initiate COM object
        CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
        wchar_t* wch = nullptr;

        //iTunes IID extracted from Apple API
        QAxObject *iITunes = new QAxObject(iTunesCLID);
        iTunesCOMHandler *handler = new iTunesCOMHandler(iITunes, this);      

        //bind events to sink handler
        auto oatputqe = QObject::connect(
            iITunes, SIGNAL(OnAboutToPromptUserToQuitEvent()), 
            handler, SLOT(OnAboutToPromptUserToQuitEvent())
        );

        auto oppe = QObject::connect(
            iITunes, SIGNAL(OnPlayerPlayEvent(QVariant)), 
            handler, SLOT(OnPlayerPlayEvent(QVariant))
        );

        auto opse = QObject::connect(
            iITunes, SIGNAL(OnPlayerStopEvent(QVariant)), 
            handler, SLOT(OnPlayerStopEvent(QVariant))
        );

        //iITunes->dumpObjectInfo();

        //process events
        while(this->_mustListen && !handler->iTunesShutdownRequested) {
            QCoreApplication::processEvents();
            this->msleep(20);
        }

        //disconnect events
        QObject::disconnect(oatputqe);
        QObject::disconnect(oppe);
        QObject::disconnect(opse);

        //clear COM related Obj
        this->shoutEmpty();
        delete handler;
        iITunes->clear(); 
        delete iITunes;

        //uninitialize COM
        CoUninitialize();

        //if iTunes is shutting down...
        if(this->_mustListen && handler->iTunesShutdownRequested) {
            
            //say we acknoledge iTunes shutting down...
            emit printLog(tr("iTunes shutting down !"));

            //wait for old iTunes window to finally shutdown
            do {
                
                //check if window still exists 
                HWND checkHandler = FindWindowA(0, "iTunes");
                if(checkHandler) {

                    //if it exists, check the PID (shutting down window...)
                    DWORD checkProcessID;
                    DWORD checkProcessID_worked = GetWindowThreadProcessId(checkHandler, &checkProcessID);
                    if(checkProcessID_worked) {
                        
                        //if old ID <> checked ID, means a new window has been opened, so we can close
                        if(checkProcessID != currentProcessID) {
                            break;
                        }
                    }

                //if no window found, break...
                } else {
                    break;
                } 
                
                //finally, sleep
                this->sleep(1);

            } while (this->_mustListen);
            
            //say we relooped
            emit printLog(tr("Waiting for iTunes to launch again..."));
        }

    } while (this->_mustListen);

    //end with log
    emit printLog(tr("Stopped listening to iTunes."));

};

#endif