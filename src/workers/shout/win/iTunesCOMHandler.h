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

#pragma once

#include <QAxObject>
#include <QEventLoop>
#include <QVariant>

class ShoutWatcher;

class iTunesCOMHandler : public QObject {
    Q_OBJECT

   struct JumpTracker {
      QString lLocation = QString();
      qint64 lPosMS = 0;
   };

 private:
    QAxObject* _musicAppObj;
    ShoutWatcher* _worker;
    QEventLoop _evtLoop;
    JumpTracker _jTracker;

 public:
    iTunesCOMHandler(QAxObject* musicAppObj, ShoutWatcher* worker);
    
    //
    void listenUntilShutdown();

 public slots:
    void onPlayerStateChanged(QVariant currentTrackAsCOM);
    void onPeriodicalCheckJumpingTrack();
    void stopListening();
 
 private:
    void _processPayloadFromCurrentTrack();
    bool _isMusicAppPlaying() const;
};

#endif
