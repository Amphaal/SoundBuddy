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

#include "ShoutWatcher.h"

#include "src/i18n/trad.hpp"

#include "src/workers/shout/win/iTunesCOMHandler.h"

#include <QTimer>
#include <QAxObject>
#include <windows.h>
#include <combaseapi.h>

#include "win/iTunesCOMHandler.h"
#include "win/iTunesCOMInterface_i.c"

//
bool ShoutWatcher::_waitForAvailableSession() {
    while (this->_keepWatching) {
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

        // if has already been ran and iTunes is still the same instance, do not proceed, since it means that the window is shutting down right now.
        if(this->_oldProcessID != NULL && this->_oldProcessID == currentProcessID) {
            this->sleep(1);
            continue;
        }

        // store process ID
        this->_oldProcessID = currentProcessID;

        //
        return this->_keepWatching;
    };

    //
    return false;
}


void ShoutWatcher::_execWatchSession() {
    // log..
    emit forwardMessage(
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
    emit forwardMessage(tr("Initializing communication..."));

    // Music App IID extracted from Apple API
    auto musicAppObj = new QAxObject(ComCLID);
    this->_handler = new iTunesCOMHandler(musicAppObj, this);

    // bind events to sink handler
        auto oatputqe = QObject::connect(
            musicAppObj, SIGNAL(OnAboutToPromptUserToQuitEvent()),
            this->_handler, SLOT(stopListening())
        );

        auto oppe = QObject::connect(
            musicAppObj, SIGNAL(OnPlayerPlayEvent(QVariant)),
            this->_handler, SLOT(onPlayerStateChanged(QVariant))
        );

        auto opse = QObject::connect(
            musicAppObj, SIGNAL(OnPlayerStopEvent(QVariant)),
            this->_handler, SLOT(onPlayerStateChanged(QVariant))
        );

    QTimer timer;
    timer.setInterval(1000);
    QObject::connect(
        &timer, SIGNAL(timeout()),
        this->_handler, SLOT(onPeriodicalCheckJumpingTrack())
    );
    timer.start();

    // log..
    emit forwardMessage(
        tr("Listening to %1 !")
            .arg(musicAppName())
    );

        // process events
        this->_handler->listenUntilShutdown();

    timer.deleteLater();

    // disconnect events
        QObject::disconnect(oatputqe);
        QObject::disconnect(oppe);
        QObject::disconnect(opse);

    // clear COM related Obj
        //
        this->_handler->deleteLater();
        
        //
        musicAppObj->clear();
        musicAppObj->deleteLater();

    // uninitialize COM
    CoUninitialize();
}
