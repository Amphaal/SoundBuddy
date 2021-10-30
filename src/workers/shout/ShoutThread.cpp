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

#include "ShoutThread.h"

#include <QJsonDocument>
#include <QJsonObject>

#include "src/_i18n/trad.hpp"

ShoutThread::ShoutThread(const UploadHelper* uploder, const AppSettings::ConnectivityInfos connectivityInfos) : ITNZThread(uploder, connectivityInfos) {}

QJsonObject ShoutThread::_createBasicShout() const {
    // get iso date
    time_t now;
    time(&now);
    char buf[sizeof "2011-10-08T07:07:09Z"];
    strftime(buf, sizeof buf, "%FT%TZ", gmtime(&now));

    // return json obj
    QJsonObject obj;
    obj["date"] = buf;
    return obj;
}

void ShoutThread::shoutEmpty() {
    //
    emit printLog(
        tr("%1: Shouting -> Nothing")
            .arg(QDateTime::currentDateTime().toString())
    );

    // send...
    auto shout = this->_createBasicShout();
    this->_shoutToServer(shout);
}


void ShoutThread::_shoutToServer(const QJsonObject &incoming) {
    try {
        //
        UploadHelper::UploadInstructions instr {
            _connectivityInfos,
            AppSettings::getShoutUploadInfos(),
            QJsonDocument{incoming}.toJson()
        };

        //
        this->_uploder->uploadDataToPlatform(instr);
    } catch(const std::exception& e) {
        // emit error
        emit printLog(e.what(), false, true);
    }
}

// compare with old shout, if equivalent, don't reshout
bool ShoutThread::shouldUpload(
        bool iPlayerState,
        const QString &tName,
        const QString &tAlbum,
        const QString &tArtist,
        const QString &tDatePlayed,
        const QString &tDateSkipped
    ) {
    // hash blueprint
    const auto currHash = QString::number(iPlayerState) + tName + tAlbum + tArtist + (tDatePlayed >= tDateSkipped ? tDatePlayed : tDateSkipped);

    // check if strings are identical
    bool isHashIdentical = (this->_lastTrackHash == currHash);

    // replace old hash with new
    this->_lastTrackHash = currHash;

    // if not identical, shout !
    return !isHashIdentical;
}

void ShoutThread::shoutFilled(
        const QString &name,
        const QString &album,
        const QString &artist,
        const QString &genre,
        int duration,
        int playerPosition,
        bool playerState,
        int year
    ) {
    // fill obj
    auto obj = this->_createBasicShout();
    obj["name"] = name;
    obj["album"] = album;
    obj["artist"] = artist;
    obj["genre"] = genre;
    obj["duration"] = duration;
    obj["playerPosition"] = playerPosition;
    obj["playerState"] = playerState;
    obj["year"] = year;

    auto pState = playerState ? tr("playing") : tr("paused");

    // log...
    auto logMessage =
        tr("%1: Shouting -> %2 - %3 - %4 (%5)")
            .arg(QDateTime::currentDateTime().toString())
            .arg(name)
            .arg(album)
            .arg(artist)
            .arg(pState);

    emit printLog(logMessage);

    this->_shoutToServer(obj);
}

#ifdef APPLE

#include <unistd.h>
#include <QProcess>

void ShoutThread::run() {
    emit printLog(tr("Waiting for %1 to launch...").arg(musicAppName()));

    // define applescript to get shout values
    Q_INIT_RESOURCE(resources);
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
        p.waitForReadyRead();
        auto result = p.readAll();
        p.waitForFinished();

        // default values and inst
        QString tName;
        QString tAlbum;
        QString tArtist;
        QString tGenre;
        int iDuration;
        int iPlayerPos;
        bool iPlayerState = false;
        // bool iRepeatMode;
        QString tDateSkipped;
        QString tDatePlayed;
        int tYear;

        // if has result
        if (result.size()) {
            // turn results into array
            result[0] = '[';
            result[result.size() - 1] = ']';

            // cast to json
            QJsonDocument trackObj;
            trackObj.Parse(result.data());

            // get values for shout
            tName = trackObj[0].GetString();
            tAlbum = trackObj[1].GetString();
            tArtist = trackObj[2].GetString();
            tGenre = trackObj[3].GetString();
            iDuration = trackObj[4].GetInt();
            tYear = trackObj[5].GetInt();
            iPlayerPos = trackObj[6].GetInt();
            iPlayerState = trackObj[7].GetString() == "paused" ? 0 : 1;
            tDateSkipped = trackObj[8].GetString();
            tDatePlayed = trackObj[9].GetString();
        }

        // compare with old shout, if equivalent, don't reshout
        if (this->shouldUpload(iPlayerState, tName, tAlbum, tArtist, tDatePlayed, tDateSkipped)) {
            // if had results
            if (result.size()) {
                // say track infos
                this->shoutFilled(tName, tAlbum, tArtist, tGenre, iDuration, iPlayerPos, iPlayerState, tYear);
            } else {
                // say nothing happens
                this->shoutEmpty();
            }
        }

       // wait a bit before re-asking
       this->sleep(1);
    }

    this->shoutEmpty();
    emit printLog(tr("Stopped listening to %1.").arg(musicAppName()));
}
#endif

#ifdef _WIN32

#include <QMetaObject>
#include <QMetaMethod>
#include <QObject>
#include <QCoreApplication>
#include <QDebug>

#include <windows.h>
#include <combaseapi.h>

#include "src/workers/shout/ShoutThread.h"
#include "win/MusicAppCOMHandler.h"

void ShoutThread::run() {
    // start with log
    emit printLog(tr("Waiting for %1 to launch...").arg(musicAppName()));

    // prepare CLID
    HWND windowsHandler;
    DWORD currentProcessID;

    // Music app IID extracted from Apple API
    wchar_t* wch = nullptr;
    HRESULT hr = ::StringFromCLSID({0xDC0C2640, 0x1415, 0x4644, {0x87, 0x5C, 0x6F, 0x4D, 0x76, 0x98, 0x39, 0xBA}}, &wch);
    auto ComCLID = QString::fromWCharArray(wch);

    do {
        // search for music app...
        windowsHandler = FindWindowA(0, musicAppName().toUtf8());

        // if not found, wait and retry
        if(!windowsHandler) {
            this->sleep(1);
            continue;
        }

        // Music App found, store the associated PID
        GetWindowThreadProcessId(windowsHandler, &currentProcessID);

        // log..
        emit printLog(tr("Listening to %1 !").arg(musicAppName()));

        // initiate COM object
        CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
        wchar_t* wch = nullptr;

        // Music App IID extracted from Apple API
        QAxObject *comObj = new QAxObject(ComCLID);
        MusicAppCOMHandler *handler = new MusicAppCOMHandler(comObj, this);

        // bind events to sink handler
        auto oatputqe = QObject::connect(
            comObj, SIGNAL(OnAboutToPromptUserToQuitEvent()),
            handler, SLOT(OnAboutToPromptUserToQuitEvent())
        );

        auto oppe = QObject::connect(
            comObj, SIGNAL(OnPlayerPlayEvent(QVariant)),
            handler, SLOT(OnPlayerPlayEvent(QVariant))
        );

        auto opse = QObject::connect(
            comObj, SIGNAL(OnPlayerStopEvent(QVariant)),
            handler, SLOT(OnPlayerStopEvent(QVariant))
        );

        // comObj->dumpObjectInfo();

        // process events
        while(this->_mustListen && !handler->musicAppShutdownRequested) {
            QCoreApplication::processEvents();
            this->msleep(20);
        }

        // disconnect events
        QObject::disconnect(oatputqe);
        QObject::disconnect(oppe);
        QObject::disconnect(opse);

        // clear COM related Obj
        this->shoutEmpty();
        delete handler;
        comObj->clear();
        delete comObj;

        // uninitialize COM
        CoUninitialize();

        // if Music App is shutting down...
        if(this->stop && handler->musicAppShutdownRequested) {
            // say we acknoledge Music App shutting down...
            emit printLog(tr("%1 shutting down !").arg(musicAppName()));

            // wait for old Music App window to finally shutdown
            do {
                // check if window still exists
                HWND checkHandler = FindWindowA(0, musicAppName().toUtf8());
                if(checkHandler) {
                    // if it exists, check the PID (shutting down window...)
                    DWORD checkProcessID;
                    DWORD checkProcessID_worked = GetWindowThreadProcessId(checkHandler, &checkProcessID);
                    if(checkProcessID_worked) {
                        // if old ID <> checked ID, means a new window has been opened, so we can close
                        if(checkProcessID != currentProcessID) {
                            break;
                        }
                    }

                // if no window found, break...
                } else {
                    break;
                }

                // finally, sleep
                this->sleep(1);
            } while (this->_mustListen);

            // say we relooped
            emit printLog(tr("Waiting for %1 to launch again...").arg(musicAppName()));
        }
    } while (this->_mustListen);

    // end with log
    emit printLog(tr("Stopped listening to %1.").arg(musicAppName()));
}

#endif
