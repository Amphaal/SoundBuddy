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

#include "MBeatThread.h"

#include <QJsonObject>
#include <QJsonDocument>

#include "version.h"

MBeatThread::MBeatThread(const AppSettings::ConnectivityInfos &connectivityInfos) : _connectivityInfos(connectivityInfos) {}

void MBeatThread::run() {
    //
    if(!_connectivityInfos.areOK) {
        emit updateConnectivityStatus(
            tr("Waiting for appropriate credentials."),
            ConnectivityIndicator::NOK
        );
        return;
    }

    //
    QWebSocket socket;
    QUrl url(this->_connectivityInfos.getPlaformHomeUrl());
    auto url_host = url.host();
    if(url_host == "localhost" || url_host == '127.0.0.1') {
        url.setPort(80);
        url.setScheme("ws");
    } else {
        url.setPort(443);
        url.setScheme("wss");
    }

    //
    socket.open(url);

    //
    emit updateConnectivityStatus(
        tr("Connecting to server..."),
        ConnectivityIndicator::ONGOING
    );

    // handling errors
    QObject::connect(
        &socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred),
        [this, &socket](QAbstractSocket::SocketError error) {
            emit updateConnectivityStatus(
                tr("An error occured while connecting with %1 platform : %2")
                    .arg(DEST_PLATFORM_PRODUCT_NAME)
                    .arg(socket.errorString()),
                ConnectivityIndicator::NOK
            );
        }
    );

    // handling messages
    QObject::connect(
        &socket, &QWebSocket::textMessageReceived,
        [this, &socket](const QString &message) {
            //
            QJsonParseError parseError;
            auto document = QJsonDocument::fromJson(message.toUtf8(), &parseError);

            auto parseErr = [this]() {
                emit updateConnectivityStatus(
                    tr("Issue while reading response from %1 platform.")
                        .arg(DEST_PLATFORM_PRODUCT_NAME),
                    ConnectivityIndicator::NOK
                );
            };

            //
            if (parseError.error != QJsonParseError::ParseError::NoError || !document.isObject()) {
                parseErr();
                return;
            }

            const auto jsonObj = document.object();
            const auto msgType = jsonObj.value("id").toString();
            const auto msgContent = jsonObj.value("r").toString();

            if(msgType.isEmpty() || msgContent.isEmpty()) {
                parseErr();
                return;
            }

            //
            if(msgType == "credentialsChecked") {
                if(msgContent == "ok") {
                    emit updateConnectivityStatus(
                        tr("Logged as \"%1\"")
                            .arg(_connectivityInfos.username),
                        ConnectivityIndicator::OK
                    );
                } else {
                    emit updateConnectivityStatus(
                        _onCredentialsErrorMsg(msgContent),
                        ConnectivityIndicator::NOK
                    );
                }
            } else if (msgType == "databaseUpdated") {
                this->_checkCredentials(socket);
            } else {
                parseErr();
                return;
            }
        }
    );

    // ping/pong feature
    QTimer pingTimer;
    pingTimer.setInterval(HEARTBEAT_INTERVAL);

        // on heartbeat intervals...
        QObject::connect(
            &pingTimer, &QTimer::timeout,
            [this, &socket]() {
                // whenever pong has been received in the meantime
                if(!_pongReceived) {
                    // pong missed, tell we lost connection
                    emit updateConnectivityStatus(
                        tr("Reconnecting to server..."),
                        ConnectivityIndicator::ONGOING
                    );
                    _pongMissed = true;
                } else {
                    // reset pong flag
                    _pongReceived = false;
                }

                // ping every time
                socket.ping();
            }
        );

        // when response to pings are received
        QObject::connect(
            &socket, &QWebSocket::pong,
            [this, &socket](quint64 elapsedTime, const QByteArray &payload) {
                // ack pong
                _pongReceived = true;

                // if heartbeat failed somehow, recheck credentials
                if(_pongMissed) {
                    _pongMissed = false;
                    this->_checkCredentials(socket);
                }
            }
        );

    // on connection !
    QObject::connect(
        &socket, &QWebSocket::connected,
        [this, &socket, &pingTimer]() {
            pingTimer.start(); // can start tracking for heartbeats
            this->_checkCredentials(socket);
        }
    );


    this->exec();
}

void MBeatThread::_checkCredentials(QWebSocket &socket) {
    //
    emit updateConnectivityStatus(
        tr("Asking for credentials validation..."),
        ConnectivityIndicator::ONGOING
    );

    //
    QJsonObject payload;
    payload["id"] = "checkCredentials";
    payload["r"] = this->_connectivityInfos.password;
    socket.sendTextMessage(QJsonDocument{payload}.toJson());
}

const QString MBeatThread::_onCredentialsErrorMsg(const QString& returnCode) const {
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
