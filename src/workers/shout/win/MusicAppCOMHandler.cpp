// FeedTNZ
// Small companion app for desktop to feed or stream ITunes / Music library informations
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

MusicAppCOMHandler::MusicAppCOMHandler(QAxObject *MusicAppObj, ShoutThread *worker) : MusicAppObj(MusicAppObj), worker(worker) {
    this->shoutHelper();
}

void MusicAppCOMHandler::OnAboutToPromptUserToQuitEvent() {
    this->musicAppShutdownRequested = true;
}

void MusicAppCOMHandler::OnPlayerPlayEvent(QVariant iTrack) {
    this->shoutHelper(iTrack);
}

void MusicAppCOMHandler::OnPlayerStopEvent(QVariant iTrack) {
    this->shoutHelper(iTrack);
}

void MusicAppCOMHandler::shoutHelper(QVariant iTrack) {
    //
    QAxObject* weilder = nullptr;

    // find value
    if(iTrack.isNull()) {
        //
        weilder = this->MusicAppObj->querySubObject("CurrentTrack");

        // shout empty
        if(!weilder) return this->worker->shoutEmpty();

    } else {
        //
        weilder = new QAxObject(iTrack.value<IDispatch*>());

        if(weilder->isNull()) {
            weilder->clear();
            delete weilder;
            weilder = this->MusicAppObj->querySubObject("CurrentTrack");
        }
    }

    // get values for shout
    auto tName = weilder->property("Name").value<QString>();
    auto tAlbum = weilder->property("Album").toString();
    auto tArtist = weilder->property("Artist").toString();
    auto tGenre = weilder->property("Genre").toString();
    auto iDuration = weilder->property("Duration").toInt();
    auto iPlayerPos = this->MusicAppObj->property("PlayerPosition").value<int>();
    auto iPlayerState = this->MusicAppObj->property("PlayerState").value<bool>();
    auto tDatePlayed = weilder->property("PlayedDate").toDateTime().toString(Qt::ISODate);
    auto tDateSkipped = weilder->property("SkippedDate").toDateTime().toString(Qt::ISODate);
    auto tYear = weilder->property("Year").toInt();

    // clear
    weilder->clear();
    delete weilder;

    // compare with old shout, if equivalent, don't reshout
    if(this->worker->shouldUpload(iPlayerState, tName, tAlbum, tArtist, tDatePlayed, tDateSkipped)) {
        // shout !
        this->worker->shoutFilled(tName, tAlbum, tArtist, tGenre, iDuration, iPlayerPos, iPlayerState, tYear);
    }
}

#endif
