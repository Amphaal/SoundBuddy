#pragma once

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QUrl>
#include <QFileInfo>
#include <QLocale>
#include <QFile>

#include "src/helpers/AppSettings.hpp"

class UploadHelper {
 public:
    struct UploadInstructions {
        const AppSettings::ConnectivityInfos &connectivityInfos;
        const AppSettings::UploadInfos &uploadInfos;
        const QByteArray &dataToUpload;

        const QUrl getUploadUrl() const {
            return QUrl(connectivityInfos.getPlaformHomeUrl() + uploadInfos.path);
        }
    };

    explicit UploadHelper(QObject* parent = nullptr) : _manager(new QNetworkAccessManager(parent)) {}
    ~UploadHelper() { _manager->deleteLater(); }

    QNetworkReply* uploadDataToPlatform(const UploadInstructions &instructions) const {
        //
        auto postData = new QHttpMultiPart(QHttpMultiPart::MixedType);

            QHttpPart filePart;
            filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
            filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"" + instructions.uploadInfos.outputFileName + "\""));
            filePart.setBody(instructions.dataToUpload);

            // password...
            QHttpPart passwordPart;
            passwordPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"password\""));
            passwordPart.setBody(instructions.connectivityInfos.password.toUtf8());

            // headless flag
            QHttpPart headlessPart;
            headlessPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"headless\""));
            headlessPart.setBody("1");

        postData->append(filePart);
        postData->append(passwordPart);
        postData->append(headlessPart);

        // build request
        QNetworkRequest request;
        request.setUrl(instructions.getUploadUrl());
        request.setRawHeader("Accept-Language", QLocale::system().name().toUtf8());

        // handle reply and send
        auto reply = _manager->post(request, postData);
        postData->setParent(reply);  // delete the multiPart with the reply

        return reply;
    }

 private:
    QNetworkAccessManager* _manager;
};
