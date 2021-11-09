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

#include "FeederThread.h"

#include <QFile>

#include <string>
#include <utility>

#include "src/helpers/PlatformHelper.h"

#include <ITunesLibraryParser.hpp>

#include "src/_i18n/trad.hpp"

FeederThread::FeederThread(const AppSettings::ConnectivityInfos connectivityInfos) : ITNZThread(connectivityInfos) {}

void FeederThread::run() {
    //
    emit printLog(
        tr("WARNING ! Make sure you activated the XML file sharing in %1 > Preferences > Advanced.")
            .arg(musicAppName())
    );

    try {
        //
        emit printLog(tr("Getting XML file location..."));

            const auto musicAppLibPath = PlatformHelper::getMusicAppLibLocation().toStdString();
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
            auto [libFile, results] = parser.getStoragedResults();
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
                        .arg(warningPath.c_str()),
                    MessageType::WARNING
                );

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
                    MessageType::ISSUE
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
            const auto output_view = outputParser.dataBuffer().view();
            auto qb_array = QByteArray::fromRawData(output_view.data(), output_view.size());

            //
            UploadHelper uploader;

            auto response = uploader.uploadDataToPlatform({
                this->_connectivityInfos,
                AppSettings::getFeederUploadInfos(),
                qb_array
            });

        emit printLog(tr("Sending... Waiting for response."));

            // on finished
            QObject::connect(
                response, &QNetworkReply::finished,
                [this, response]() {
                    // if an error occured, nothing else to do
                    if(response->error()) return;

                    //
                    auto rOutput = response->readAll();

                    //
                    if (!rOutput.isEmpty()) {
                        emit printLog(
                            tr("Server responded: %1")
                                .arg(rOutput)
                        );
                    } else {
                        emit printLog(
                            tr("No feedback from the server ? Strange... Please check the targeted host."),
                            MessageType::WARNING
                        );
                    }

                    // back to exec
                    this->quit();
                }
            );

            // on error
            QObject::connect(
                response, &QNetworkReply::errorOccurred,
                [this, response](QNetworkReply::NetworkError) {
                    //
                    emit printLog(
                        tr("An error occured while sending tracks infos to %1 platform : %2")
                            .arg(DEST_PLATFORM_PRODUCT_NAME)
                            .arg(response->errorString()),
                        MessageType::ISSUE
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
            MessageType::ISSUE
        );
    }
}
