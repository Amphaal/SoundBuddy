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

#include "FeederThread.h"

#include <QFile>

#include <string>
#include <utility>

#include "src/helpers/PlatformHelper.h"

#include <ITunesLibraryParser.hpp>

#include "src/_i18n/trad.hpp"

FeederThread::FeederThread(const UploadHelper* uploder, const AppSettings::ConnectivityInfos connectivityInfos) : ITNZThread(uploder, connectivityInfos) {}

void FeederThread::run() {
    //
    emit printLog(tr("WARNING ! Make sure you activated the XML file sharing in %1 > Preferences > Advanced.").arg(musicAppName()));

    try {
        //
        const auto musicAppLibPath = this->_getMusicAppLibLocation().toStdString();
        const auto outputPath = AppSettings::getFeedOutputFilePath().toStdString();
        const auto warningPath = AppSettings::getFeedWarningFilePath().toStdString();

        //
        ITunesLibraryParser parser (
            musicAppLibPath.c_str(),
            outputPath.c_str(),
            warningPath.c_str()
        );

        //
        emit printLog(tr("Collecting tracks infos..."));
            auto results = parser.getStoragedResults();
        emit printLog(tr("Collection done !"));

        //
        emit printLog(tr("Parsing infos into JSON..."));

            // if has missing fields tracks
            if(results.missingFieldsTracks.size()) {
                //
                emit printLog(
                    tr("WARNING ! %1 files in your library are missing important "
                    "metadata and consequently were removed from the output file ! "
                    "Please check the \"%2\" file for more informations.")
                        .arg(results.missingFieldsTracks.size())
                        .arg(warningPath.c_str()));
                //
                MissingFieldsJSONParser { std::move(results.missingFieldsTracks) }
                    .copyToFile(warningPath.c_str());
            } else {
                // else remove warning file
                QFile::remove(warningPath.c_str());
            }

            //
            if(!results.allTracksCount()) {
                //
                emit printLog(
                    tr("No music found in your %1 library. Please feed it some.")
                        .arg(musicAppName()),
                    false,
                    true
                );

                //
                return;
            }

            //
            SuccessfulJSONParser outputParser { std::move(results.OKTracks) };
            outputParser.copyToFile(outputPath.c_str());

        emit printLog(tr("Parsing done !"));

        // send results
        emit printLog(tr("OK, output file is ready for breakfast !"));

        emit filesGenerated();

        emit printLog(tr("Let's try to send now !"));
            //
            auto buffer_str_view = outputParser.dataBuffer().str();
            auto qb_array =  QByteArray::fromRawData(buffer_str_view.data(), buffer_str_view.size());

            //
            auto response = this->_uploder->uploadDataToPlatform({
                this->_connectivityInfos,
                AppSettings::getFeederUploadInfos(),
                qb_array
            });

        emit printLog(tr("Sending... Waiting for response."));

            // on finished
            QObject::connect(
                response, &QNetworkReply::finished,
                [this, response]() {
                    //
                    auto rOutput = response->readAll();

                    //
                    if (!rOutput.isEmpty()) {
                        emit printLog(tr("Server responded: %1").arg(rOutput));
                    } else {
                        emit printLog(tr("No feedback from the server ? Strange... Please check the targeted host."));
                    }

                    // back to exec
                    this->quit();
                }
            );

            // on error
            QObject::connect(
                response, &QNetworkReply::errorOccurred,
                [this](QNetworkReply::NetworkError) {
                    //
                    emit printLog(
                        tr("An error occured while sending tracks infos to %1 platform.")
                            .arg(DEST_PLATFORM_PRODUCT_NAME),
                        false,
                        true
                    );

                    // back to exec
                    this->quit();
                }
            );

        // wait for network reply
        exec();

        // ask for deletion
        response->deleteLater();

    //
    } catch (const std::exception& e) {
        // emit error as log
        emit printLog(
            QString(e.what()),
            false,
            true
        );
    }
}

///
/// Other Helpers
///

// seek in Music App preference file the library location
const QString FeederThread::_getMusicAppLibLocation() {
    emit printLog(tr("Getting XML file location..."));
    try {
        return PlatformHelper::getMusicAppLibLocation();
    } catch(...) {
        throw std::logic_error(
            tr("An issue happened while fetching %1's XML file location. Have you installed %1 ?")
                .arg(musicAppName())
                .toStdString()
        );
    }
}
