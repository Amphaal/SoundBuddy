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

#pragma once

#include <QThread>

#include "src/helpers/AppSettings.hpp"
#include "src/helpers/UploadHelper.hpp"

enum class MessageType {
   STANDARD,
   ISSUE,
   WARNING
};

Q_DECLARE_METATYPE(MessageType)

class ITNZThread : public QThread {
    Q_OBJECT

 public:
    ITNZThread(const AppSettings::ConnectivityInfos connectivityInfos) : _connectivityInfos(connectivityInfos) {}

    virtual void quit() { QThread::quit(); }

 protected:
    const AppSettings::ConnectivityInfos _connectivityInfos;

 signals:
    void printLog(const QString &message, const MessageType msgType = MessageType::STANDARD, const bool replacePreviousLine = false);
};