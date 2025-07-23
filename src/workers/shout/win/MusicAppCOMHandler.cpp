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
    if(!currentTrack || currentTrack->isNull()) {
        return this->_worker->shoutEmpty();
    }
    
    //
    this->_shoutFromCOMObj(currentTrack);
}

bool MusicAppCOMHandler::_isMusicAppPlaying() const {
    const auto prop = this->_musicAppObj->property("PlayerState");
    auto value = prop.data_ptr().data.data[0];
    return value > 0;
}

void MusicAppCOMHandler::_shoutFromCOMObj(QAxObject* obj) {
    // get values for shout from Track
    ShoutPayload payload;
        payload.tName = obj->property("Name").toString();
        payload.tAlbum = obj->property("Album").toString();
        payload.tArtist = obj->property("Artist").toString();
        payload.tGenre = obj->property("Genre").toString();
        payload.iDuration = obj->property("Duration").toInt();
        payload.tDatePlayed = obj->property("PlayedDate").toDateTime().toString(Qt::ISODate);
        payload.tDateSkipped = obj->property("SkippedDate").toDateTime().toString(Qt::ISODate);
        payload.tYear = obj->property("Year").toInt();
        payload.tFileLocation = obj->property("Location").toString();

    // clear asap
    obj->clear();
    delete obj;

        // get values for shout from Player state
        payload.iPlayerPosMS = this->_musicAppObj->property("PlayerPositionMS").toInt();
        payload.iPlayerState = this->_isMusicAppPlaying();

    // compare with old shout, if equivalent, don't reshout
    const auto shouldUpload = this->_worker->shouldUpload(payload);
    if(!shouldUpload) {
        return;
    }

    // shout !
    this->_worker->shoutFilled(payload);
}

void MusicAppCOMHandler::onPlayerStateChanged(QVariant currentTrackAsCOM) {
    _shoutCurrentTrack();
}

void MusicAppCOMHandler::onPeriodicalCheckJumpingTrack() {
    // IMPORTANT NOTE: FOR SOME REASON, we have to check for pos first; if not, we'll end up locking both iTunes and SoundBuddy !
    // TODO: find a way to not trigger "log errors-only" when attempting to check player pos when no tune is played...
    const auto posMS = this->_musicAppObj->property("PlayerPositionMS").toInt();

    // THEN, AND ONLY THEN, extract location from current track
    auto currentTrack = this->_musicAppObj->querySubObject("CurrentTrack");
    if (!currentTrack) {
        return;
    }
    const auto location = currentTrack->property("Location").toString();
    currentTrack->clear();
    delete currentTrack;

    //
    if (location != _jTracker.lLocation) {
        _jTracker.lLocation = location;
        _jTracker.lPosMS = posMS;
        return;
    }

    //
    const auto diff = ((double)_jTracker.lPosMS) - posMS;
    if (qFabs(diff) > 2000) {
        _shoutCurrentTrack();
    };
    
    //
    _jTracker.lPosMS = posMS;
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
