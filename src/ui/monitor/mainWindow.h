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

#include <IFWUpdateChecker.hpp>

#include <QCoreApplication>
#include <QString>
#include <QSize>
#include <QObject>
#include <QDebug>
#include <QTabWidget>
#include <QMainWindow>
#include <QWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QCloseEvent>
#include <QHideEvent>
#include <QIcon>
#include <QStatusBar>

#include "src/helpers/PlatformHelper.h"

#include "src/ui/tabs/ShoutTab.hpp"
#include "src/ui/tabs/FeederTab.hpp"
#include "src/workers/mBeat/MBeatThread.h"
#include "src/workers/base/UpdaterThread.hpp"

#include "src/ui/widgets/TraficLight.hpp"

#include "src/helpers/AppSettings.hpp"
#include "src/ui/monitor/PreferencesDialog.hpp"

class MainWindow : public QMainWindow {
   Q_OBJECT

 public:
    MainWindow();
    void informWarningPresence();

 private:
    bool forceQuitOnMacOS = false;
    bool userNotificationOnUpdateCheck = false;
    QSystemTrayIcon* trayIcon;
    QAction* myPlatformAction;
    QAction* openWarningsAction;
    AppSettings appSettings;

    // statusbar
    void _initStatusBar();
    void updateStatusBar(const QString &message, const ConnectivityIndicator &indic);
    QLabel* statusLabel;
    TrafficLightWidget* statusLight;

    ///
    /// UI instanciation
    ///

    void _initUI();
    void _initUIMenu();
    void _initUITray();

    QMenu* _getFileMenu(bool withMonitor);
    QMenu* _getOptionsMenu();

    void onAppSettingsChanged();
    void updateWarningsMenuItem();

    ///
    /// Functionnalities helpers calls
    ///

    void accessPlatform();
    void accessPreferences();
    void openWarnings();

    ///
    /// Events handling
    ///

    // visibility
    void hideEvent(QHideEvent *event);
    void closeEvent(QCloseEvent *event);
    void trueShow();
    void trueHide(QEvent* event);
    void forcedClose();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    // update handling
    QAction *versionAction = nullptr;
    QAction *cfugAction = nullptr;
    void setupAutoUpdate();
    void onUpdateChecked(const UpdateChecker::CheckResults checkResults);
    void requireUpdateCheckFromUser();
    void checkForAppUpdates();
    void UpdateSearch_switchUI(bool isSearching);

    // tabs
    ShoutTab* shoutTab = nullptr;
    FeederTab* feederTab = nullptr;
    void _initUITabs();

    // workers
    ShoutThread* shoutWorker = nullptr;
    FeederThread* feederWorker = nullptr;
    MBeatThread* mbeatWorker = nullptr;
    BashThread* bashWorker = nullptr;
    UpdaterThread updateCheckerWorker;

    void runMBeat();
    void runShouts();
    void runFeeder();
    void _runBash();
};
