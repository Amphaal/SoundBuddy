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

#include <QString>
#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QLockFile>

#include "ui/monitor/mainWindow.h"

int main(int argc, char** argv) {
    // prevent multiples instances
    QLockFile lockFile(QDir::tempPath() + QDir::separator() + APP_NAME + ".lock");
    if (!lockFile.tryLock(100)) {
        return 1;
    }

    // setup app
    QApplication app(argc, argv);

    // activate translations...
        auto translationsPath = app.applicationDirPath() + QDir::separator() + "translations";
        auto locale = QLocale::system();

        // Qt
        auto _qtTranslator = new QTranslator(&app);
        if (_qtTranslator->load(locale, "qt", "_", translationsPath)) {
            app.installTranslator(_qtTranslator);
        }

        // app
        auto _appTranslator = new QTranslator(&app);
        if (_appTranslator->load(locale, "", "", translationsPath)) {
            app.installTranslator(_appTranslator);
        }

    //
    app.setQuitOnLastWindowClosed(false);
    app.setApplicationName(APP_NAME);
    app.setOrganizationName(APP_PUBLISHER);
    app.setApplicationDisplayName(APP_NAME);

    // fetch main window
    MainWindow mw;

    // wait for the app to close
    return app.exec();
}
