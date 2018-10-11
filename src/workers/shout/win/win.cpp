#include <windows.h>

// #include "../../../libs/itunescom/iTunesEventHandler.cpp"
#include "../../../libs/itunescom/iTunesCOMInterface.h"

#include <ActiveQt/QAxBase>
#include <ActiveQt/QAxObject>

#include "../shout.h" 
#include "iTunesEventsSink.h"
#include "../../../helpers/stringHelper.cpp"

#include <QMetaObject>
#include <QMetaMethod>
#include <QObject>
#include <QCoreApplication>

ITunesEventsSink::ITunesEventsSink(QAxObject *iTunesObj, ShoutWorker *worker) : iTunesObj(iTunesObj), worker(worker) {
    this->shoutHelper();
};

void ITunesEventsSink::OnAboutToPromptUserToQuitEvent() {
    this->worker->exit();
};

void ITunesEventsSink::OnPlayerPlayEvent(QVariant iTrack) {
    this->shoutHelper(iTrack);
};

void ITunesEventsSink::OnPlayerStopEvent(QVariant iTrack) {
    this->shoutHelper(iTrack);
}; 

void ITunesEventsSink::shoutHelper(QVariant iTrack) {
    
    //inst as COM obj
    QAxObject *trackObj = new QAxObject(iTrack.value<IDispatch*>());

    //if empty
    if(trackObj->isNull()) {
        //get current track
        trackObj = iTunesObj->querySubObject("CurrentTrack");
    }

    //if still empty, shout nothing
    if (trackObj == NULL) return this->worker->shoutEmpty();

    //get values for shout
    auto tName = trackObj->property("Name").value<QString>().toStdString();
    auto tAlbum = trackObj->property("Album").value<QString>().toStdString();
    auto tArtist = trackObj->property("Artist").value<QString>().toStdString();
    auto tGenre = trackObj->property("Genre").value<QString>().toStdString();
    auto tDuration = trackObj->property("Duration").value<QString>().toStdString();
    auto iPlayerPos = this->iTunesObj->property("PlayerPosition").value<int>();
    auto iPlayerState = this->iTunesObj->property("PlayerState").value<bool>();

    //compare with old shout, if equivalent, don't reshout
    size_t currHash = std::hash<std::string>{}(StringHelper::boolToString(iPlayerState) + tName + tAlbum + tArtist);
    if (this->lastTrackHash == currHash) {
        return;
    }

    //shout !
    this->worker->shoutFilled(tName, tAlbum, tArtist, tGenre, tDuration, iPlayerPos, iPlayerState);
    
    //clear
    trackObj->clear();
    this->lastTrackHash = currHash;
};

void ShoutWorker::run() {

    emit this->printLog("Waiting for iTunes to launch...");
    
    //initiate COM object
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    wchar_t* wch = nullptr;
    HRESULT hr = ::StringFromCLSID(CLSID_iTunesApp, &wch);
    QAxObject *iITunes = new QAxObject(QString::fromWCharArray(wch));
    
    //iITunes->blockSignals(false);
    ITunesEventsSink *sink = new ITunesEventsSink(iITunes, this);

    //bind events to sink handler
    bool oatputqe = QObject::connect(iITunes, iITunes->metaObject()->method(5), sink, sink->metaObject()->method(5));
    bool oppe = QObject::connect(iITunes, iITunes->metaObject()->method(9), sink, sink->metaObject()->method(6));
    bool opse = QObject::connect(iITunes, iITunes->metaObject()->method(11), sink, sink->metaObject()->method(7));

    //process events
    while(this->mustListen) {
        QCoreApplication::processEvents();
        Sleep(200);
    }

    //clear COM usage
    sink->shoutHelper();
    iITunes->clear(); 
    CoUninitialize();

    emit this->printLog("Stopped listening to iTunes.");
};
