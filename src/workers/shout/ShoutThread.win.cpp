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

#include "src/i18n/trad.hpp"

#include "src/workers/shout/win/MusicAppCOMHandler.h"

#include <QTimer>
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

        timer.deleteLater();

        // disconnect events
            QObject::disconnect(oatputqe);
            QObject::disconnect(oppe);
            QObject::disconnect(opse);

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
