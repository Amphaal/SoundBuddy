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

#include "ShoutThread.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QTimeZone>

#include "src/i18n/trad.hpp"

ShoutThread::ShoutThread(const AppSettings::ConnectivityInfos connectivityInfos) : ITNZThread(connectivityInfos) {}

void ShoutThread::quit() {
    //
    this->_mustListen = false;

    //
    #ifdef _WIN32
        if(this->_handler) {
            this->_handler->stopListening();
        }
    #endif

    //
    ITNZThread::quit();
}

QJsonObject ShoutThread::_createBasicShout() const {
    // get ISO date, JS compatible (2011-10-08T07:07:09Z)
    auto currentTime = QDateTime::currentDateTimeUtc(); // Important ! we should not use local time, only UTC+0 !

    // return json obj
    QJsonObject obj;
    obj["date"] = currentTime.toString(Qt::DateFormat::ISODate);
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
    Defer defer;
    _uploader = new UploadHelper;
    defer.defer([&](){ delete _uploader; });

    //
    _startShouting();
}

/** careful wait for response would only work if master QEventLoop is still running ! If shutting down, exits with -1 instantly at */
void ShoutThread::_shoutToServer(const QJsonObject &incoming, bool waitForResponse) {
    try {
        //
        UploadHelper::UploadInstructions instr {
            _connectivityInfos,
            AppSettings::getShoutUploadInfos(),
            QJsonDocument{incoming}.toJson()
        };

        //
        auto response = _uploader->uploadDataToPlatform(instr, false);

        //
        QEventLoop _syncLp;

        // will exit event loop on response discard
        if (waitForResponse) {
             QObject::connect(
                response, &QObject::destroyed,
                [&_syncLp]() {
                    _syncLp.quit();
                }
            );
        }

        // on error
        QObject::connect(
            response, &QNetworkReply::errorOccurred,
            [this, response, waitForResponse, &_syncLp](QNetworkReply::NetworkError) {
                //
                emit printLog(
                    tr("An error occured while shouting tracks infos to %1 platform : %2")
                        .arg(DEST_PLATFORM_PRODUCT_NAME)
                        .arg(prettyPrintErrorNetworkMessage(response)),
                    MessageType::ISSUE
                );

                // ask for deletion
                response->deleteLater();
            }
        );

        // on finished (WITH or WITHOUT error)
        QObject::connect(
            response, &QNetworkReply::finished,
            [response]() {
                response->deleteLater();
            }
        );


        // wait for response
        if(waitForResponse) {
            const auto exec = _syncLp.exec();
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
        const QString &location,
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
    //
    emit newFileLocationShout(location);

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
