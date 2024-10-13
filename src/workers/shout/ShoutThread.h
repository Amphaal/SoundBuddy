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

#pragma once

#include <QString>
#include <QEventLoop>

#include <functional>

#include "src/workers/base/ITNZThread.hpp"

#ifdef _WIN32
    #include "src/workers/shout/win/MusicAppCOMHandler.h"
#endif
#include <src/workers/BashThread.hpp>

class MusicAppCOMHandler;

class ShoutThread : public ITNZThread {
    Q_OBJECT

 signals:
    void newFileLocationShout(const QString &filePath);

 public:
    ShoutThread(const AppSettings::ConnectivityInfos connectivityInfos);

    void run() override;
    void quit() override;

    void shoutEmpty(bool waitForResponse = false);

    void shoutFilled(
        const QString &location,
        const QString &name,
        const QString &album,
        const QString &artist,
        const QString &genre,
        int duration,
        int playerPosition,
        bool playerState,
        int year,
        bool waitForResponse = false
    );

    bool shouldUpload(
        bool iPlayerState,
        const QString &tName,
        const QString &tAlbum,
        const QString &tArtist,
        const QString &tDatePlayed,
        const QString &tDateSkipped
    );

 private:
    QString _lastTrackHash;
    bool _mustListen = true;

    #ifdef _WIN32
        MusicAppCOMHandler* _handler = nullptr;
    #endif

    UploadHelper* _uploader;
    void _startShouting();

    QJsonObject _createBasicShout() const;

    void _shoutToServer(const QJsonObject &incoming, bool waitForResponse = false);
};
