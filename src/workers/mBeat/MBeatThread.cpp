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

#include "MBeatThread.h"

MBeatThread::MBeatThread(const AppSettings::ConnectivityInfos &connectivityInfos) : _connectivityInfos(connectivityInfos) {}

void MBeatThread::run() {
    //
    if(!_connectivityInfos.areOK) {
        emit updateConnectivityStatus(tr("Waiting for appropriate credentials."), TLW_Colors::RED);
        return;
    }

    //
    QWebSocket socket;
    QUrl url(this->_connectivityInfos.getPlaformHomeUrl());
    url.setPort(3000);
    url.setScheme("wss");
    socket.open(url.toString(QUrl::RemovePath));

    //
    emit updateConnectivityStatus(tr("Connecting to server..."), TLW_Colors::YELLOW);

    // when response to pings are received
    QObject::connect(
        &socket, &QWebSocket::pong,
        [](quint64 elapsedTime, const QByteArray &payload) {
            // TODO
        }
    );

    // handling errors
    QObject::connect(
        &socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
        [this](QAbstractSocket::SocketError error) {
            emit updateConnectivityStatus(tr("An error occured while connecting with %1 platform !"), TLW_Colors::RED);
        }
    );

    // handling disconnect
    QObject::connect(
        &socket, &QWebSocket::disconnected,
        [this]() {
            // TODO
        }
    );

    // handling messages
    QObject::connect(
        &socket, &QWebSocket::textMessageReceived,
        [this](const QString &message) {
            // TODO
        }
    );

    // on connection !
    QObject::connect(
        &socket, &QWebSocket::connected,
        [this]() {
            //
            emit updateConnectivityStatus(tr("Asking for credentials validation..."), TLW_Colors::YELLOW);

            //
            sioClient->socket("/login")->emit("checkCredentials", p);
        }
    );

    /// ping/pong feature
    QTimer pingTimer;
    pingTimer.setInterval(10000);  // ping every 10 seconds
    QObject::connect(
        &pingTimer, &QTimer::timeout,
        [&socket]() {
            socket.ping();
        }
    );
    pingTimer.start();

    // tell sio is trying to reconnect
    this->_sioClient->set_reconnect_listener([&](unsigned int a, unsigned int b) {
        emit updateConnectivityStatus(tr("Reconnecting to server..."), TLW_Colors::YELLOW);
    });

    // once server checked the credentials
    this->_sioClient->socket("/login")->on("credentialsChecked", [&](sio::event& ev) {
        // extract response
        auto response = ev.get_messages()[0]->get_map();
        auto isOk = response["isLoginOk"]->get_bool();
        auto extraInfo = QString::fromStdString(response["accomp"]->get_string());

        if(isOk) {
            this->_loggedInUser = extraInfo;
            _emitLoggedUserMsg();
        } else {
            emit updateConnectivityStatus(_validationErrorTr(extraInfo), TLW_Colors::RED);
        }

        // toggle flag
        this->_requestOngoing = false;
    });

    // when server tell us the database has been updated, ask for revalidation
    this->_sioClient->socket("/login")->on("databaseUpdated", [&](sio::event& ev) {
        this->_checkCredentials(true);
    });

    this->exec();
}

void MBeatThread::_emitLoggedUserMsg() {
    emit updateConnectivityStatus(tr("Logged as \"%1\"").arg(_connectivityInfos.username), TLW_Colors::GREEN);
}

const QString MBeatThread::_validationErrorTr(const QString& returnCode) const {
    QString msg;

    if(returnCode == "cdm") {
        msg = tr("Credential data missing");
    } else if(returnCode == "eud") {
        msg = tr("Empty users database");
    } else if(returnCode == "unfid") {
        msg = tr("Username not found in database");
    } else if(returnCode == "nopass") {
        msg = tr("Password for the user not found in database");
    } else if(returnCode == "pmiss") {
        msg = tr("Password missmatch");
    } else {
        return tr("Unknown error from the validation request");
    }

    return tr("Server responded with : \"%1\"").arg(msg);
}
