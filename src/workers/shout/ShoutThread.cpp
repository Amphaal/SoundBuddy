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

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkReply>

#include "src/_i18n/trad.hpp"

#ifdef _WIN32
#include "src/workers/shout/win/MusicAppCOMHandler.h"
#endif

ShoutThread::ShoutThread(const AppSettings::ConnectivityInfos connectivityInfos) : ITNZThread(connectivityInfos) {}

void ShoutThread::quit() {
    this->_mustListen = false;
    #ifdef _WIN32
        if(this->_handler) this->_handler->stopListening();
    #endif
    ITNZThread::quit();
}

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

void ShoutThread::shoutEmpty(bool waitForResponse) {
    //
    emit printLog(
        tr("%1: Shouting -> Nothing")
            .arg(QDateTime::currentDateTime().toString())
    );

    // send...
    auto shout = this->_createBasicShout();
    this->_shoutToServer(shout, waitForResponse);
}

void ShoutThread::run() {
    _uploader = new UploadHelper;
    _startShouting();
    delete _uploader;
}

void ShoutThread::_shoutToServer(const QJsonObject &incoming, bool waitForResponse) {
    try {
        //
        UploadHelper::UploadInstructions instr {
            _connectivityInfos,
            AppSettings::getShoutUploadInfos(),
            QJsonDocument{incoming}.toJson()
        };

        //
        auto response = _uploader->uploadDataToPlatform(instr);

        // on error
        QObject::connect(
            response, &QNetworkReply::errorOccurred,
            [this, response, waitForResponse](QNetworkReply::NetworkError) {
                //
                emit printLog(
                    tr("An error occured while shouting tracks infos to %1 platform : %2")
                        .arg(DEST_PLATFORM_PRODUCT_NAME)
                        .arg(response->errorString()),
                    MessageType::ISSUE
                );

                // ask for deletion
                response->deleteLater();

                // if must be sync, quit loop
                if(waitForResponse) this->_syncLp->quit();
            }
        );

        // on finished
        QObject::connect(
            response, &QNetworkReply::finished,
            [this, response, waitForResponse]() {
                // delete
                response->deleteLater();

                // if must be sync, quit loop
                if(waitForResponse) this->_syncLp->quit();
            }
        );

        // wait for response
        if(waitForResponse) {
            _syncLp = new QEventLoop;
            this->_syncLp->exec();
            delete _syncLp;
            _syncLp = nullptr;
        }

    //
    } catch(const std::exception& e) {
        // emit error
        emit printLog(e.what(), MessageType::ISSUE);
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
        int year,
        bool waitForResponse
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

    this->_shoutToServer(obj, waitForResponse);
}

#ifdef __APPLE__

#include <unistd.h>
#include <QProcess>

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

    QEventLoop q;

    QObject::connect(&p, &QProcess::readyRead, [&q]() {
        q.exit();
    });

    p.start();
    q.exec();

    // loop until user said not to
    // while (this->_mustListen) {
    //     // get shout results
    //     p.start();

    //     emit printLog(
    //         "nique poop"
    //     );

    //     p.waitForReadyRead(1000);

    //     emit printLog(
    //         "nique pep"
    //     );

    //     auto result = p.readAll();
    //     p.waitForFinished(1000);

    //     emit printLog(
    //         "nique OK"
    //     );

    //     // default values and inst
    //     QString tName;
    //     QString tAlbum;
    //     QString tArtist;
    //     QString tGenre;
    //     int iDuration;
    //     int iPlayerPos;
    //     bool iPlayerState = false;
    //     QString tDateSkipped;
    //     QString tDatePlayed;
    //     int tYear;

    //     // if has result
    //     if (result.size()) {
    //         // turn results into array
    //         result[0] = '[';
    //         result[result.size() - 1] = ']';

    //         // cast to json
    //         const auto trackData = QJsonDocument::fromJson(result.data()).array();

    //         // get values for shout
    //         tName = trackData[0].toString();
    //         tAlbum = trackData[1].toString();
    //         tArtist = trackData[2].toString();
    //         tGenre = trackData[3].toString();
    //         iDuration = trackData[4].toInt();
    //         tYear = trackData[5].toInt();
    //         iPlayerPos = trackData[6].toInt();
    //         iPlayerState = trackData[7].toString() == "paused" ? 0 : 1;
    //         tDateSkipped = trackData[8].toString();
    //         tDatePlayed = trackData[9].toString();
    //     }

    //     // compare with old shout, if equivalent, don't reshout
    //     if (this->shouldUpload(iPlayerState, tName, tAlbum, tArtist, tDatePlayed, tDateSkipped)) {
    //         // if had results
    //         if (result.size()) {
    //             // say track infos
    //             this->shoutFilled(
    //                 tName, tAlbum, tArtist, tGenre, iDuration, iPlayerPos, iPlayerState, tYear,
    //                 true
    //             );
    //         } else {
    //             // say nothing happens
    //             this->shoutEmpty(true);
    //         }
    //     }

    //    // wait a bit before re-asking
    //    this->sleep(1);
    // }

    this->shoutEmpty();
    emit printLog(
        tr("Stopped listening to %1.")
            .arg(musicAppName())
    );
}
#endif

#ifdef _WIN32

#include <QAxObject>
#include <windows.h>
#include <combaseapi.h>

#include "win/MusicAppCOMHandler.h"
#include "win/iTunesCOMInterface_i.c"

void ShoutThread::_startShouting() {
    // start with log
    emit printLog(
        tr("Waiting for %1 to launch...")
            .arg(musicAppName())
    );

    // prepare
    DWORD oldProcessID = NULL;

    //
    //
    //

    // bind method
    auto bindWithMusicApp = [this]() {
        // log..
        emit printLog(
            tr("Connecting to %1 ...")
                .arg(musicAppName())
        );

        // Music app IID extracted from Apple API
        wchar_t* wch;
        HRESULT hr = ::StringFromCLSID(CLSID_iTunesApp, &wch);
        const auto ComCLID = QString::fromWCharArray(wch);

        // initiate COM object
        CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

        // log..
        emit printLog(tr("Initializing communication..."));

        // Music App IID extracted from Apple API
        auto musicAppObj = new QAxObject(ComCLID);
        this->_handler = new MusicAppCOMHandler(musicAppObj, this);

        // bind events to sink handler
            auto oatputqe = QObject::connect(
                musicAppObj, SIGNAL(OnAboutToPromptUserToQuitEvent()),
                this->_handler, SLOT(stopListening())
            );

            auto oppe = QObject::connect(
                musicAppObj, SIGNAL(OnPlayerPlayEvent(QVariant)),
                this->_handler, SLOT(onCurrentTrackStateChanged(QVariant))
            );

            auto opse = QObject::connect(
                musicAppObj, SIGNAL(OnPlayerStopEvent(QVariant)),
                this->_handler, SLOT(onCurrentTrackStateChanged(QVariant))
            );

        // log..
        emit printLog(
            tr("Listening to %1 !")
                .arg(musicAppName())
        );

            // process events
            this->_handler->listenUntilShutdown();

        // say we acknoledge Music App shutting down...
        emit printLog(
            tr("Stopped listening to %1.")
                .arg(musicAppName())
        );

        // disconnect events
            QObject::disconnect(oatputqe);
            QObject::disconnect(oppe);
            QObject::disconnect(opse);

        // send last shout
        this->shoutEmpty(true);

        // clear COM related Obj
            //
            delete this->_handler;
            this->_handler = nullptr;
            
            //
            musicAppObj->clear();
            delete musicAppObj;

        // uninitialize COM
        CoUninitialize();
    };

    // search for music app method
    auto waitForMusicAppRunning = [this, &oldProcessID, bindWithMusicApp]() {
        do {
            // search for music app...
            auto windowHandler = FindWindowA(0, musicAppName().toUtf8());

            // if not found, wait and retry
            if(!windowHandler) {
                this->sleep(1);
                continue;
            }

            // Music App found, store the associated PID
            DWORD currentProcessID;
            auto success = GetWindowThreadProcessId(windowHandler, &currentProcessID);
            
            // if cannot fetch, wait and retry
            if(!success) {
                this->sleep(1);
                continue;
            }

            // if has already been ran and iTunes is still the same instance...
            if(oldProcessID != NULL && currentProcessID == oldProcessID) {
                this->sleep(1);
                continue;
            }

            // store process ID
            oldProcessID = currentProcessID;

            // start to listen
            bindWithMusicApp();

            // if must still listen, means user prompted for app quit 
            if(this->_mustListen) {
                emit printLog(
                    tr("Waiting for %1 to launch again...")
                        .arg(musicAppName())
                );
            }

        //
        } while (this->_mustListen);
    };

    //
    //
    //

    waitForMusicAppRunning();
}

#endif
