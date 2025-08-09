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

#include "iTunesCOMHandler.h"

#include "src/workers/shout/ShoutWatcher.h"

iTunesCOMHandler::iTunesCOMHandler(QAxObject *musicAppObj, ShoutWatcher *worker) : _musicAppObj(musicAppObj), _worker(worker) {
    this->_processPayloadFromCurrentTrack();
}

void iTunesCOMHandler::onPlayerStateChanged(QVariant currentTrackAsCOM) {
    _processPayloadFromCurrentTrack();
}

void iTunesCOMHandler::listenUntilShutdown() {
    this->_evtLoop.exec();
}

void iTunesCOMHandler::stopListening() {
    this->_evtLoop.quit();
}

bool iTunesCOMHandler::_isMusicAppPlaying() const {
    const auto prop = this->_musicAppObj->property("PlayerState");
    auto value = prop.data_ptr().data.data[0];
    return value > 0;
}

void iTunesCOMHandler::_processPayloadFromCurrentTrack() {

    ShoutPayload payload;

    {
        //
        auto currentTrack = this->_musicAppObj->querySubObject("CurrentTrack");
        
        // no current track, shout empty
        if(!currentTrack || currentTrack->isNull()) {
            return this->_worker->processPayload(ShoutPayload{});
        }
        
        // get values for shout from Track
        payload.tName = currentTrack->property("Name").toString();
        payload.tAlbum = currentTrack->property("Album").toString();
        payload.tArtist = currentTrack->property("Artist").toString();
        payload.tGenre = currentTrack->property("Genre").toString();
        payload.iDuration = currentTrack->property("Duration").toInt();
        payload.tDatePlayed = currentTrack->property("PlayedDate").toDateTime().toString(Qt::ISODate);
        payload.tDateSkipped = currentTrack->property("SkippedDate").toDateTime().toString(Qt::ISODate);
        payload.tYear = currentTrack->property("Year").toInt();
        payload.tFileLocation = currentTrack->property("Location").toString();

        // clear asap
        currentTrack->clear();
        delete currentTrack;
    }

    // get values for shout from Player state
    payload.iPlayerPosMS = this->_musicAppObj->property("PlayerPositionMS").toInt();
    payload.iPlayerState = this->_isMusicAppPlaying();

    // compare with old shout, if equivalent, don't reshout
    this->_worker->processPayload(payload);
}

void iTunesCOMHandler::onPeriodicalCheckJumpingTrack() {
    // THEN, AND ONLY THEN, extract location from current track
    auto currentTrack = this->_musicAppObj->querySubObject("CurrentTrack");
    if (!currentTrack) {
        return;
    }

    // IMPORTANT NOTE: FOR SOME REASON, we have to check for pos first; if not, we'll end up locking both iTunes and SoundBuddy !
    // TODO: find a way to not trigger "log errors-only" when attempting to check player pos when no tune is played...
    const auto posMS = this->_musicAppObj->property("PlayerPositionMS").toInt();

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
        _processPayloadFromCurrentTrack();
    };
    
    //
    _jTracker.lPosMS = posMS;
}

#endif
