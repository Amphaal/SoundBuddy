#ifdef Q_OS_OSX

#include <unistd.h>
#include <QProcess>

#include <rapidjson/document.h>

#include "src/workers/shout/ShoutThread.h" 
#include "src/helpers/stringHelper/stringHelper.hpp"

void ShoutThread::run() { 

    this->_inst();

    emit printLog(tr("Waiting for iTunes to launch..."));

    //define applescript to get shout values
    QString script = "tell application \"iTunes\" \n ";
    script += "if skipped date of current track is not missing value then \n "
              "set SkpDt to skipped date of current track as «class isot» as string \n "
              "else \n "
              "set SkpDt to \"\" \n "
              "end if \n ";
    script += "if played date of current track is not missing value then \n "
              "set PlyDt to played date of current track as «class isot» as string \n "
              "else \n "
              "set PlyDt to \"\" \n "
              "end if \n ";
    script += "get {name, album, artist, genre, duration, year} of current track & ";
    script += "player position & ";
    script += "(player state as string) & ";
    script += "SkpDt & ";
    script += "PlyDt ";
    script += "\n end tell";

    //prepare script exec
    QString osascript = "/usr/bin/osascript";
    QStringList processArguments;
    processArguments << "-l" << "AppleScript" << "-s" << "s";

    while (this->_mustListen) {

        //get shout results
        QProcess p;
        p.start(osascript, processArguments);
        p.write(script.toStdString().c_str());
        p.closeWriteChannel();
        p.waitForReadyRead();
        auto result = p.readAll();
        p.waitForFinished(-1);
        p.deleteLater();

        //default values and inst
        QString tName;
        QString tAlbum;
        QString tArtist;
        QString tGenre;
        int iDuration;
        int iPlayerPos;
        bool iPlayerState = false;
        //bool iRepeatMode;
        QString tDateSkipped;
        QString tDatePlayed;
        int tYear;

        //if has result
        if (result.size()) {
            
            //turn results into array
            result = result.substr(0, result.size()-2);
            result = result.substr(1, result.size()-1);
            result = "[" + result + "]";
            
            //cast to json
            rapidjson::Document trackObj;
            trackObj.Parse(result.toStdString().c_str());
            
            //get values for shout
            tName = trackObj[0].GetString();
            tAlbum = trackObj[1].GetString();
            tArtist = trackObj[2].GetString();
            tGenre = trackObj[3].GetString();
            iDuration = trackObj[4].GetFloat();
            tYear = trackObj[5].GetFloat();
            iPlayerPos = trackObj[6].GetFloat();
            iPlayerState = QString(trackObj[7].GetString()) == "paused" ? 0 : 1;
            tDateSkipped = trackObj[8].GetString();
            tDatePlayed = trackObj[9].GetString();
            
        }

        //compare with old shout, if equivalent, don't reshout
        if(this->shouldUpload(iPlayerState, tName, tAlbum, tArtist, tDatePlayed, tDateSkipped)) {
            if(result.size()) {
                //shout !
                this->shoutFilled(tName, tAlbum, tArtist, tGenre, iDuration, iPlayerPos, iPlayerState, tYear);
            }
            else {
                this->shoutEmpty();
            }
        } 

       this->sleep(1);
    }

    this->shoutEmpty();
    emit printLog(tr("Stopped listening to iTunes."));
}
#endif