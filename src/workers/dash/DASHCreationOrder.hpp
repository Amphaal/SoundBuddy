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

#pragma once

#include <QString>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

class DASHCreationOrder {
    private:
        /* path to original audio file */
        QString _path;

        /* hash of original audio file */
        QString _hash;

        /** */
        QString _dirtyFilePath;

    public:
        DASHCreationOrder() {}
        
        void replaceOrder(const QString& path, const QString& hash) {
            this->_path = path;
            this->_hash = hash;
            this->_dirtyFilePath = getPathWithDestFolder("dirty");
        }
        
        const QString& path() {
            return _path;
        }

        /* checks if same */
        bool isSame(const QString &anotherHash) {
            return _hash == anotherHash;
        }

        const QString getPathWithDestFolder(const QString &filename) {
            const auto path = QDir::toNativeSeparators(
                getPathToDashTempFolder() +
                QDir::separator() + _hash +
                QDir::separator() + filename
            );
            return path;
        }

        /* */
        void mayGenerateDirtyFile() {
            try {
                // create dir if not existing
                QDir destDir(_dirtyFilePath);
                if (!destDir.exists()) destDir.mkpath("..");

                //
                QFile file(_dirtyFilePath);
                if (file.open(QIODevice::ReadWrite)) {
                    file.close();
                }
            } catch(...) { }
        }

        void mayRemoveDirtyFile() {
            // remove file if terminating normally
            try {QFile::remove(_dirtyFilePath);}
            catch (...) { }
        }

        static QString getPathToDashTempFolder() {
            return QDir::toNativeSeparators(
                QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + 
                QDir::separator() + "DASH"
            );
        }
};
