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
    this->shoutTrackAsVariant();
}

void MusicAppCOMHandler::shoutTrackAsVariant(QVariant iTrack) {
    //
    QAxObject* weilder = nullptr;

    // find value
    if(iTrack.isNull()) {
        //
        weilder = this->_musicAppObj->querySubObject("CurrentTrack");

        // shout empty
        if(!weilder) return this->_worker->shoutEmpty();

    } else {
        //
        weilder = new QAxObject(iTrack.value<IUnknown*>());

        if(weilder->isNull()) {
            weilder->clear();
            delete weilder;
            weilder = this->_musicAppObj->querySubObject("CurrentTrack");
        }
    }

    // get values for shout
    auto tName = weilder->property("Name").value<QString>();
    auto tAlbum = weilder->property("Album").toString();
    auto tArtist = weilder->property("Artist").toString();
    auto tGenre = weilder->property("Genre").toString();
    auto iDuration = weilder->property("Duration").toInt();
    auto iPlayerPos = this->_musicAppObj->property("PlayerPosition").value<int>();
    auto iPlayerState = this->_musicAppObj->property("PlayerState").value<bool>();
    auto tDatePlayed = weilder->property("PlayedDate").toDateTime().toString(Qt::ISODate);
    auto tDateSkipped = weilder->property("SkippedDate").toDateTime().toString(Qt::ISODate);
    auto tYear = weilder->property("Year").toInt();

    // clear
    weilder->clear();
    delete weilder;

    // compare with old shout, if equivalent, don't reshout
    if(this->_worker->shouldUpload(iPlayerState, tName, tAlbum, tArtist, tDatePlayed, tDateSkipped)) {
        // shout !
        this->_worker->shoutFilled(tName, tAlbum, tArtist, tGenre, iDuration, iPlayerPos, iPlayerState, tYear);
    }
}

void MusicAppCOMHandler::listenUntilShutdown() {
    this->_evtLoop.exec();
}

void MusicAppCOMHandler::stopListening() {
    this->_evtLoop.quit();
}

#endif
