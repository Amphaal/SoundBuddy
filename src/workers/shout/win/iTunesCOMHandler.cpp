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

    QAxObject* weilder = nullptr;

    //find value
    if(iTrack.isNull()) {

        weilder = this->iTunesObj->querySubObject("CurrentTrack");
        
        //shout empty
        if(!weilder) return this->worker->shoutEmpty();

    } else {

        weilder = new QAxObject(iTrack.value<IDispatch*>());

        if(weilder->isNull()) {
            weilder->clear(); delete weilder;
            weilder = this->iTunesObj->querySubObject("CurrentTrack");
        }

    }

    //get values for shout
    auto tName = weilder->property("Name").value<QString>();
    auto tAlbum = weilder->property("Album").toString();
    auto tArtist = weilder->property("Artist").toString();
    auto tGenre = weilder->property("Genre").toString();
    auto iDuration = weilder->property("Duration").toInt();
    auto iPlayerPos = this->iTunesObj->property("PlayerPosition").value<int>();
    auto iPlayerState = this->iTunesObj->property("PlayerState").value<bool>();
    auto tDatePlayed = weilder->property("PlayedDate").toDateTime().toString(Qt::ISODate);
    auto tDateSkipped = weilder->property("SkippedDate").toDateTime().toString(Qt::ISODate);
    auto tYear = weilder->property("Year").toInt();
    
    //clear
    weilder->clear();
    delete weilder;

    //compare with old shout, if equivalent, don't reshout
    if(this->worker->shouldUpload(iPlayerState, tName, tAlbum, tArtist, tDatePlayed, tDateSkipped)) {
        //shout !
        this->worker->shoutFilled(tName, tAlbum, tArtist, tGenre, iDuration, iPlayerPos, iPlayerState, tYear);
    } 
};

#endif