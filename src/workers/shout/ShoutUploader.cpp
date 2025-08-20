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

#include "ShoutUploader.h"

#include <functional>

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QTimer>
#include <QEventLoop>

ShoutUploader::ShoutUploader(const AppSettings::ConnectivityInfos connectivityInfos) : 
    _connectivityInfos(connectivityInfos), _uploader(new UploadHelper) {}

//
void ShoutUploader::waitForDrain(unsigned long timeout) {

    //
    _drainTriggered = true;
    if (this->_drain) {
        this->_drain->deleteLater();
    }
    this->_drain = new QEventLoop;


    // note: if not setting this as context (`this` as 2cd argument), QTimer will execute whenever ShoutUploader still exists, resulting in segfault
    QTimer::singleShot(timeout, this, [this]() {
        if (this->_drain && this->_drain->isRunning()) {
            qDebug() << "[ShoutUploader] Drain timed out. Force exiting.";
            this->_drain->exit();
        }
    });

    //
    this->_drain->exec();

    //
    endDrain:
        qDebug() << "[ShoutUploader] Successfully drained.";
    
    //
    _drainTriggered = false;
}

//
void ShoutUploader::uploadAsShout(const QJsonObject &parsedShout) {
    //
    qDebug() << "[ShoutUploader] Shout received, uploading...";

    //
    try {
        //
        UploadHelper::UploadInstructions instr {
            _connectivityInfos,
            AppSettings::getShoutUploadInfos(),
            QJsonDocument{parsedShout}.toJson()
        };

        //
        this->_pendingUploads++;
        auto response = _uploader->uploadDataToPlatform(instr, false);

        // on error
        QObject::connect(
            response, &QNetworkReply::errorOccurred,
            [this, response](QNetworkReply::NetworkError) {
                emit forwardMessage(
                    tr("An error occured while shouting tracks infos to %1 platform : %2")
                        .arg(DEST_PLATFORM_PRODUCT_NAME)
                        .arg(IMessenger::prettyPrintErrorNetworkMessage(response)),
                    MessageType::ISSUE
                );
            }
        );

        // on finished (WITH or WITHOUT error)
        QObject::connect(
            response, &QNetworkReply::finished,
            [this, response]() {
                //
                QVariant statusCode = response->attribute(QNetworkRequest::HttpStatusCodeAttribute);
                if (statusCode.isValid()) {
                    int code = statusCode.toInt();
                    qDebug() << "[ShoutUploader] Uploaded with HTTP code " << code;
                }

                //
                response->deleteLater();
                
                //
                this->_pendingUploads--;

                //
                if (_drainTriggered) {
                    qDebug() << "[ShoutUploader] Drained HTTP request, remaining:" << this->_pendingUploads;

                    //
                    if (this->_pendingUploads <= 0 && this->_drain && this->_drain->isRunning()) {
                        qDebug() << "[ShoutUploader] No more requests to drain, exiting.";
                        this->_drain->exit();
                    } 
                }
            }
        );

    } catch(const std::exception& e) {
        emit forwardMessage(e.what(), MessageType::ISSUE);
    }
}
