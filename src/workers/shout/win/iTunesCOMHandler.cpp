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

    //find value
    if(iTrack.isNull()) {
        iTrack = this->iTunesObj->querySubObject("CurrentTrack")->asVariant();
    }

    //if still empty, shout nothing
    if (iTrack.isNull()) return this->worker->shoutEmpty();

    auto dataList = iTrack.toHash();

    //get values for shout
    auto tName = dataList.value("Name").toString();
    auto tAlbum = dataList.value("Album").toString();
    auto tArtist = dataList.value("Artist").toString();
    auto tGenre = dataList.value("Genre").toString();
    auto iDuration = dataList.value("Duration").toInt();
    auto iPlayerPos = this->iTunesObj->property("PlayerPosition").value<int>();
    auto iPlayerState = this->iTunesObj->property("PlayerState").value<bool>();
    auto tDatePlayed = dataList.value("PlayedDate").toDateTime().toString(Qt::ISODate);
    auto tDateSkipped = dataList.value("SkippedDate").toDateTime().toString(Qt::ISODate);
    auto tYear = dataList.value("Year").toInt();

    //compare with old shout, if equivalent, don't reshout
    if(this->worker->shouldUpload(iPlayerState, tName, tAlbum, tArtist, tDatePlayed, tDateSkipped)) {
        //shout !
        this->worker->shoutFilled(tName, tAlbum, tArtist, tGenre, iDuration, iPlayerPos, iPlayerState, tYear);
    } 
};

#endif