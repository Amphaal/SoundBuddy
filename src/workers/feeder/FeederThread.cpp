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

#include "FeederThread.h"

#include <QFile>

#include <string>
#include <utility>
#include <exception>

#include <zlib.h>
#include <stdlib.h>

#include "src/helpers/PlatformHelper.h"
#include "src/helpers/UploadHelper.hpp"

#include <ITunesLibraryParser.hpp>

#include "src/i18n/trad.hpp"

FeederThread::FeederThread(const AppSettings::ConnectivityInfos connectivityInfos) : _connectivityInfos(connectivityInfos) {}

void FeederThread::run() {
    //
    emit forwardMessage(
        tr("WARNING ! Make sure you activated the XML file sharing in %1 > Preferences > Advanced.")
            .arg(musicAppName())
    );

    try {
        //
        emit forwardMessage(tr("Getting XML file location..."));

            const auto musicAppLibPath = PlatformHelper::getMusicAppLibLocation().toStdString();
            const auto outputPath = AppSettings::getFeedOutputFilePath(false).toStdString();
            const auto warningPath = AppSettings::getFeedWarningFilePath().toStdString();

            //
            ITunesLibraryParser parser (
                musicAppLibPath.c_str(),
                outputPath.c_str(),
                warningPath.c_str()
            );

        //
        emit forwardMessage(tr("Collecting tracks infos..."));
            auto [libFile, results] = parser.getStoragedResults();
        emit forwardMessage(tr("Collection done !"));

        //
        emit forwardMessage(tr("Parsing infos into JSON..."));

            // if has missing fields tracks
            if(results.missingFieldsTracks.size()) {
                //
                emit forwardMessage(
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
                emit forwardMessage(
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

        emit forwardMessage(tr("Parsing done !"));

        // send results
        emit forwardMessage(tr("OK, output file is ready for breakfast !"));

        emit filesGenerated();

        emit forwardMessage(tr("Now we try to compress it..."));

            const auto output_view = outputParser.dataBuffer().view();
            auto compressedDataSize = compressBound(output_view.size()); // must be at least source len for starters, may be updated by zlib after sucessful compression
            auto compressedData = (unsigned char *)malloc(compressedDataSize);

            const auto compressionResult = compress(
                compressedData, 
                &compressedDataSize, 
                reinterpret_cast<const unsigned char *>(output_view.data()), 
                output_view.size()
            );

            if (compressionResult != Z_OK) {
                throw std::domain_error("zLib failed");
            }

            // write into file
            QFile file(AppSettings::getFeedOutputFilePath(true));
            file.open(QIODevice::WriteOnly);
            file.write(reinterpret_cast<const char *>(compressedData), compressedDataSize);
            file.close();

        emit forwardMessage(
            tr("Went from %1 to %2")
            .arg(QLocale::system().formattedDataSize(output_view.size()))
            .arg(QLocale::system().formattedDataSize(compressedDataSize))
        );

        emit forwardMessage(tr("Let's try to send now !"));
            //
            auto qb_array = QByteArray::fromRawData(reinterpret_cast<const char *>(compressedData), compressedDataSize);

            //
            UploadHelper uploader;

            auto response = uploader.uploadDataToPlatform({
                this->_connectivityInfos,
                AppSettings::getFeederUploadInfos(),
                qb_array,
            }, true);

        emit forwardMessage(tr("Sending... Waiting for response."));

            // on finished
            QObject::connect(
                response, &QNetworkReply::finished,
                [this, response, &compressedData]() {

                    // if an error occured, nothing else to do
                    if(response->error()) return;

                    //
                    auto rOutput = response->readAll();

                    //
                    if (!rOutput.isEmpty()) {
                        emit forwardMessage(
                            tr("Server responded: %1")
                                .arg(rOutput)
                        );
                    } else {
                        emit forwardMessage(
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
                    emit forwardMessage(
                        tr("An error occured while sending tracks infos to %1 platform : %2")
                            .arg(DEST_PLATFORM_PRODUCT_NAME)
                            .arg(IMessenger::prettyPrintErrorNetworkMessage(response)),
                        MessageType::ISSUE
                    );

                    // back to exec
                    this->quit();
                }
            );

        // wait for network reply
        exec();

        // cleanup compressed data
        free(compressedData);

        // ask for deletion
        response->deleteLater();

    //
    } catch (const std::exception& e) {
        // emit error as log
        emit forwardMessage(
            QString(e.what()),
            MessageType::ISSUE
        );
    }
}
