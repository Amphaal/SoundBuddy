#include <windows.h>

#include "../../../libs/itunescom/iTunesCOMInterface.h"

#include <ActiveQt/QAxBase>
#include <ActiveQt/QAxObject>

#include "../shout.h" 
#include "iTunesCOMHandler.h"
#include "../../../helpers/stringHelper.cpp"

#include <QMetaObject>
#include <QMetaMethod>
#include <QObject>
#include <QCoreApplication>

iTunesCOMHandler::iTunesCOMHandler(QAxObject *iTunesObj, ShoutWorker *worker) : iTunesObj(iTunesObj), worker(worker) {
    this->shoutHelper();
};

void iTunesCOMHandler::OnAboutToPromptUserToQuitEvent() {
    this->iTunesShutdownRequested = true;
};

void iTunesCOMHandler::OnPlayerPlayEvent(QVariant iTrack) {
    this->shoutHelper(iTrack);
};

void iTunesCOMHandler::OnPlayerStopEvent(QVariant iTrack) {
    this->shoutHelper(iTrack);
}; 

void iTunesCOMHandler::shoutHelper(QVariant iTrack) {
    
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