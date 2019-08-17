#ifdef _WIN32

#include "iTunesCOMHandler.h"

iTunesCOMHandler::iTunesCOMHandler(QAxObject *iTunesObj, ShoutThread *worker) : iTunesObj(iTunesObj), worker(worker) {
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
        trackObj = this->iTunesObj->querySubObject("CurrentTrack");
    }

    //if still empty, shout nothing
    if (trackObj == NULL) return this->worker->_shoutEmpty();

    //get values for shout
    auto tName = trackObj->property("Name").value<QString>().toStdString();
    auto tAlbum = trackObj->property("Album").value<QString>().toStdString();
    auto tArtist = trackObj->property("Artist").value<QString>().toStdString();
    auto tGenre = trackObj->property("Genre").value<QString>().toStdString();
    auto iDuration = trackObj->property("Duration").value<int>();
    auto iPlayerPos = this->iTunesObj->property("PlayerPosition").value<int>();
    auto iPlayerState = this->iTunesObj->property("PlayerState").value<bool>();
    auto tDatePlayed = trackObj->property("PlayedDate").value<QDateTime>().toString(Qt::ISODate).toStdString();
    auto tDateSkipped = trackObj->property("SkippedDate").value<QDateTime>().toString(Qt::ISODate).toStdString();
    auto tYear = trackObj->property("Year").value<int>();

    //clear
    trackObj->clear();
    delete trackObj;

    //compare with old shout, if equivalent, don't reshout
    if(this->worker->_shouldUpload(iPlayerState, tName, tAlbum, tArtist, tDatePlayed, tDateSkipped)) {
        //shout !
        this->worker->_shoutFilled(tName, tAlbum, tArtist, tGenre, iDuration, iPlayerPos, iPlayerState, tYear);
    } 
};

#endif