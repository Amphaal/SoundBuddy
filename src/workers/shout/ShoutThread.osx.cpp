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

#include "ShoutThread.h"

#include "src/i18n/trad.hpp"

#include <unistd.h>
#include <QProcess>
#include <QJsonDocument>

void ShoutThread::_startShouting() {
    emit printLog(
        tr("Waiting for %1 to launch...")
            .arg(musicAppName())
    );

    // define applescript to get shout values
    const auto scriptContent = QFile(":/mac/CurrentlyPlaying.applescript").readAll();

    // prepare script exec
    QProcess p;
    p.setProgram("/usr/bin/osascript");
    p.setArguments({ "-l", "AppleScript", "-s", "s"});
    p.write(scriptContent);
    p.closeWriteChannel();

    // loop until user said not to
    while (this->_mustListen) {
        // get shout results
        p.start();

        auto isReady = p.waitForReadyRead(200);
        if (!isReady || !this->_mustListen) {
            // wait a bit before re-asking
            if(this->_mustListen) this->sleep(1);
            continue;
        }

        auto result = p.readAll();
        auto readFinished = p.waitForFinished(100);
        if (!readFinished || !this->_mustListen) {
            // wait a bit before re-asking
            if(this->_mustListen) this->sleep(1);
            continue;
        }

        // default values and inst
        QString tName;
        QString tAlbum;
        QString tArtist;
        QString tGenre;
        int iDuration;
        int iPlayerPos;
        bool iPlayerState = false;
        QString tDateSkipped;
        QString tDatePlayed;
        int tYear;

        // if has result
        if (result.size()) {
            // turn results into array
            result[0] = '[';
            result[result.size() - 1] = ']';

            // cast to json
            const auto trackData = QJsonDocument::fromJson(result.data()).array();

            // get values for shout
            tName = trackData[0].toString();
            tAlbum = trackData[1].toString();
            tArtist = trackData[2].toString();
            tGenre = trackData[3].toString();
            iDuration = trackData[4].toInt();
            tYear = trackData[5].toInt();
            iPlayerPos = trackData[6].toInt();
            iPlayerState = trackData[7].toString() == "paused" ? 0 : 1;
            tDateSkipped = trackData[8].toString();
            tDatePlayed = trackData[9].toString();
        }

        // compare with old shout, if equivalent, don't reshout
        if (this->shouldUpload(iPlayerState, tName, tAlbum, tArtist, tDatePlayed, tDateSkipped)) {
            // if had results
            if (result.size()) {
                // say track infos
                this->shoutFilled(
                    tName, tAlbum, tArtist, tGenre, iDuration, iPlayerPos, iPlayerState, tYear,
                    true
                );
            } else {
                // say nothing happens
                this->shoutEmpty(true);
            }
        }

       // wait a bit before re-asking
       this->sleep(1);
    }

    this->shoutEmpty();
    emit printLog(
        tr("Stopped listening to %1.")
            .arg(musicAppName())
    );
}
