// SoundBuddy
// Small companion app that feeds SoundVitrine with ITunes / Music library metadata
// Copyright (C) 2019-2023 Guillaume Vara <guillaume.vara@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Any graphical or audio resources available within the source code may
// use a different license and copyright : please refer to their metadata
// for further details. Resources without explicit references to a
// different license and copyright still refer to this GPL.

#ifdef _WIN32

#include <QAxObject>

#include "MusicAppCOMHandler.h"

#include "src/workers/shout/ShoutThread.h"

MusicAppCOMHandler::MusicAppCOMHandler(QAxObject *musicAppObj, ShoutThread *worker) : _musicAppObj(musicAppObj), _worker(worker) {
    this->_shoutCurrentTrack();
}

void MusicAppCOMHandler::_shoutCurrentTrack() {
    //
    auto currentTrack = this->_musicAppObj->querySubObject("CurrentTrack");
    
    // no current track, shout empty
    if(!currentTrack || currentTrack->isNull()) 
        return this->_worker->shoutEmpty();
    
    //
    this->_shoutFromCOMObj(currentTrack);
}

bool MusicAppCOMHandler::_isMusicAppPlaying() const {
    const auto prop = this->_musicAppObj->property("PlayerState");
    auto value = prop.data_ptr().data.data[0];
    return value > 0;
}

void MusicAppCOMHandler::_shoutFromCOMObj(QAxObject* obj) {
    ShoutPayload payload;

    // get values for shout
    payload.tName = obj->property("Name").toString();
    payload.tAlbum = obj->property("Album").toString();
    payload.tArtist = obj->property("Artist").toString();
    payload.tGenre = obj->property("Genre").toString();
    payload.iDuration = obj->property("Duration").toInt();
    payload.iPlayerPos = this->_musicAppObj->property("PlayerPosition").toInt();
    payload.iPlayerState = this->_isMusicAppPlaying();
    payload.tDatePlayed = obj->property("PlayedDate").toDateTime().toString(Qt::ISODate);
    payload.tDateSkipped = obj->property("SkippedDate").toDateTime().toString(Qt::ISODate);
    payload.tYear = obj->property("Year").toInt();
    payload.tFileLocation = obj->property("Location").toString();

    // clear
    obj->clear();
    delete obj;

    // compare with old shout, if equivalent, don't reshout
    if(!this->_worker->shouldUpload(payload)) 
        return;

    // shout !
    this->_worker->shoutFilled(payload);
}

void MusicAppCOMHandler::onCurrentTrackStateChanged(QVariant trackAsCOM) {
    _shoutCurrentTrack();
}

void MusicAppCOMHandler::listenUntilShutdown() {
    this->_evtLoop.exec();
}

void MusicAppCOMHandler::stopListening() {
    // send last shout before shutting down QThread's QEventLoop, which will prevent waiting for last shout to be sent
    // Only on Windows, since OSX "scan" behavior is "poll" like and does not require QEventLoop thread-blocking
    this->_worker->shoutEmpty(true);

    //
    this->_evtLoop.quit();
}
#endif
