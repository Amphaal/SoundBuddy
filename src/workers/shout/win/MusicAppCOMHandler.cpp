// SoundBuddy
// Small companion app that feeds SoundVitrine with ITunes / Music library metadata
// Copyright (C) 2019-2021 Guillaume Vara <guillaume.vara@gmail.com>

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
    // get values for shout
    auto tName = obj->property("Name").toString();
    auto tAlbum = obj->property("Album").toString();
    auto tArtist = obj->property("Artist").toString();
    auto tGenre = obj->property("Genre").toString();
    auto iDuration = obj->property("Duration").toInt();
    auto iPlayerPos = this->_musicAppObj->property("PlayerPosition").toInt();
    auto iPlayerState = this->_isMusicAppPlaying();
    auto tDatePlayed = obj->property("PlayedDate").toDateTime().toString(Qt::ISODate);
    auto tDateSkipped = obj->property("SkippedDate").toDateTime().toString(Qt::ISODate);
    auto tYear = obj->property("Year").toInt();

    // clear
    obj->clear();
    delete obj;

    // compare with old shout, if equivalent, don't reshout
    if(!this->_worker->shouldUpload(iPlayerState, tName, tAlbum, tArtist, tDatePlayed, tDateSkipped)) 
        return;

    // shout !
    this->_worker->shoutFilled(tName, tAlbum, tArtist, tGenre, iDuration, iPlayerPos, iPlayerState, tYear);
}

void MusicAppCOMHandler::onCurrentTrackStateChanged(QVariant trackAsCOM) {
    _shoutCurrentTrack();
}

void MusicAppCOMHandler::listenUntilShutdown() {
    this->_evtLoop.exec();
}

void MusicAppCOMHandler::stopListening() {
    this->_evtLoop.quit();
}

#endif
