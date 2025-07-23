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

#include "MBeatThread.h"

#include <QJsonObject>
#include <QJsonDocument>

#include "version.h"

MBeatThread::MBeatThread(const AppSettings::ConnectivityInfos &connectivityInfos) : _connectivityInfos(connectivityInfos) {}

void MBeatThread::run() {
    //
    qDebug("MBeat: Start");

    //
    // Check connectivity infos
    //

    if(!_connectivityInfos.areOK) {
        //
        emit updateConnectivityStatus(
            tr("Waiting for appropriate credentials."),
            ConnectivityIndicator::NOK
        );

        //
        return;
    }

    //
    // Setup
    //

    //
    QNetworkAccessManager _manager;
    auto request = this->_createPOSTRequest();

    //
    QTimer pingTimer;
    pingTimer.setSingleShot(true); // will require manual re-trigger

    // on heartbeat intervals...
    QObject::connect(
        &pingTimer, &QTimer::timeout,
        [this, &pingTimer, request, &_manager]() {
            //
            auto reply = this->_checkCredentials(request, &_manager);

            // on finished
            QObject::connect(
                reply, &QNetworkReply::finished,
                [this, &pingTimer, reply]() {
                    // get HTTP status code
                    const auto httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

                    //
                    // handle status bar update
                    //

                    if (httpCode != 200) {
                        // get body
                        auto rOutput = reply->readAll();

                        //
                        emit updateConnectivityStatus(
                            tr("An error occured while connecting with %1 platform : %2")
                                .arg(DEST_PLATFORM_PRODUCT_NAME)
                                .arg(rOutput.isEmpty() ? reply->errorString() : rOutput),
                            ConnectivityIndicator::NOK
                        );

                        //
                        qDebug("MBeat: Failure !");

                    } else {
                        //
                        emit updateConnectivityStatus(
                            tr("Logged as \"%1\"")
                                .arg(_connectivityInfos.username),
                            ConnectivityIndicator::OK
                        );

                        //
                        qDebug("MBeat: Credentials OK !");
                    }

                    //
                    // handle thread lifecycle
                    //

                    // if credentials are not accepted, no need to loop back again, user needs to update them.
                    if (httpCode == 403) {
                        this->quit(); // exit loop and thread
                    } else {
                        // get rid of this reply
                        reply->deleteLater();

                        // will retry to confirm 
                        pingTimer.setInterval(HEARTBEAT_INTERVAL_MS);
                        pingTimer.start();
                        qDebug("MBeat: Will retry soon.");
                    }
                }
            );
        }
    );

    // configure timer to trigger immediately after loop starts
    pingTimer.setInterval(0);
    pingTimer.start();

    // start loop
    this->exec();

    // loop has exited
    qDebug("MBeat: Shutdown");
}

QNetworkReply* MBeatThread::_checkCredentials(QNetworkRequest* request, QNetworkAccessManager* manager) {
    //
    qDebug("MBeat: Checking credentials...");
    emit updateConnectivityStatus(
        tr("Asking for credentials validation..."),
        ConnectivityIndicator::ONGOING
    );

    //
    auto postData = this->_createPOSTData(manager);

    // reply will be deleted along QNetworkAccessManager (it is his parent by default)
    auto reply = manager->post(*request, postData);
    return reply;
}

QNetworkRequest* MBeatThread::_createPOSTRequest() {
    //
    QUrl url(this->_connectivityInfos.getSoundHeartbeatUrl() + "/creds");
    auto url_host = url.host();
    const auto wantsUnsecure = url_host == "localhost" || url_host == "127.0.0.1";
    url.setScheme(wantsUnsecure ? "http" : "https");
    
    // build request
    auto request = new QNetworkRequest(url);
    request->setTransferTimeout(REQUEST_TIMEOUT_MS); // setup an earlier timeout
    request->setRawHeader("Accept-Language", QLocale::system().name().toUtf8());
    request->setRawHeader("X-Client-Id", APP_NAME);
    request->setRawHeader("X-Client-Version", APP_CURRENT_VERSION);

    //
    return request;
}

// note: cannot be reused; will be drained after any request attempt using it
QHttpMultiPart* MBeatThread::_createPOSTData(QNetworkAccessManager* manager) {
    //
    auto postData = new QHttpMultiPart(QHttpMultiPart::ContentType::FormDataType, manager);

        // username...
        QHttpPart usernamePart;
        usernamePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"username\""));
        usernamePart.setBody(this->_connectivityInfos.username.toUtf8());

        // password...
        QHttpPart passwordPart;
        passwordPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"password\""));
        passwordPart.setBody(this->_connectivityInfos.password.toUtf8());

    postData->append(usernamePart);
    postData->append(passwordPart);

    return postData;
}
