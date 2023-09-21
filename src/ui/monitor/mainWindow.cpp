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

#include "mainWindow.h"
#include "version.h"

MainWindow::MainWindow() {
    // generate the UI
    this->_initUI();

    //
    this->onAppSettingsChanged();
    this->updateWarningsMenuItem();
    this->setupAutoUpdate();

    // autostart shout thread ?
    if (this->appSettings.value(AppSettings::MUST_AUTORUN_SHOUT).toBool()) {
        this->shoutTab->tButton->click();
    }
}

void MainWindow::informWarningPresence() {
    this->updateWarningsMenuItem();
}

// updates the menu depending on the config values filled or not
void MainWindow::onAppSettingsChanged() {
    //
    bool shouldActivateLink = !this->appSettings.getConnectivityInfos().getPlaformHomeUrl().isEmpty();
    this->myPlatformAction->setEnabled(shouldActivateLink);

    // restart threads ?
    runMBeat();
    if(this->shoutWorker && this->shoutWorker->isRunning()) runShouts();
    if(this->feederWorker && this->feederWorker->isRunning()) runFeeder();
}

void MainWindow::updateWarningsMenuItem() {
    bool hasFeederWarnings = QFileInfo(AppSettings::getFeedWarningFilePath()).exists();
    this->openWarningsAction->setEnabled(hasFeederWarnings);
}

///
/// Functionnalities helpers calls
///

void MainWindow::accessPlatform() {
    const auto platformHomeUrl = this->appSettings.getConnectivityInfos().getPlaformHomeUrl();

    //
    if(platformHomeUrl.isEmpty()) {
        QMessageBox::critical(this,
            tr("Cannot access %1 plaform").arg(DEST_PLATFORM_PRODUCT_NAME),
            tr("%1 Plaform URL is not valid. Please check Preferences connectivity parameters.").arg(DEST_PLATFORM_PRODUCT_NAME)
        );
        return;
    }

    //
    PlatformHelper::openUrlInBrowser(platformHomeUrl);
}

void MainWindow::accessPreferences() {
    PreferencesDialog(&this->appSettings, this).exec();
    this->onAppSettingsChanged();
}

// open the warnings file on the OS
void MainWindow::openWarnings() {
    PlatformHelper::openFileInOS(AppSettings::getFeedWarningFilePath());
}

void MainWindow::setupAutoUpdate() {
    QObject::connect(
        &this->updateCheckerWorker, &UpdaterThread::isNewerVersionAvailable,
        this, &MainWindow::onUpdateChecked
    );

    // start the update check
    this->checkForAppUpdates();
}

void MainWindow::onUpdateChecked(const UpdateChecker::CheckResults checkResults) {
    // if the user asks directly to check updates
    if (this->userNotificationOnUpdateCheck) {
        // next check will go un-notified
        this->userNotificationOnUpdateCheck = false;

        // error
        if (checkResults.result != UpdateChecker::CheckCode::Succeeded) {
            //
            QString errMsg;
            switch (checkResults.result) {
                case UpdateChecker::CheckCode::NoRemoteURL:
                    errMsg = tr("No remote URL given, probably because of an issue with the executable. Contact the developpers.");
                break;
                case UpdateChecker::CheckCode::LocalManifestFetch:
                    errMsg = tr("Issue while finding the local package manifest.");
                break;
                case UpdateChecker::CheckCode::LocalManifestRead:
                    errMsg = tr("Issue while reading the local package manifest.");
                break;
                case UpdateChecker::CheckCode::RemoteManifestFetch:
                    errMsg = tr("Issue while finding the remote package update manifest.");
                break;
                case UpdateChecker::CheckCode::RemoteManifestRead:
                    errMsg = tr("Issue while reading the remote package update manifest.");
                break;
                default:
                case UpdateChecker::CheckCode::UnspecifiedFail:
                    errMsg = tr("No specified code linked to the error happening. Contact the developpers.");
                break;
            }

            //
            QMessageBox::critical(this,
                tr("Error while checking updates"),
                errMsg,
                QMessageBox::Ok,
                QMessageBox::Ok);

        // no updates
        } else if (!checkResults.hasNewerVersion) {
            QMessageBox::information(this,
                tr("Checking updates"),
                tr("No updates available at the time."),
                QMessageBox::Ok,
                QMessageBox::Ok);
        }
    }

    // no update, no go
    if (!checkResults.hasNewerVersion) {
        this->UpdateSearch_switchUI(false);
        return;
    }

    // if has update
    auto msgboxRslt = QMessageBox::information(this,
        tr("Update Available"),
        tr("An update is available for %1. Would you like to install it now ?").arg(APP_NAME),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::Yes);

    if (msgboxRslt == QMessageBox::Yes) {
        switch (checkResults.source) {
        case UpdateChecker::CheckSource::GithubCheck:
            QDesktopServices::openUrl(QUrl(APP_PATCHNOTE_URL));
        break;
        
        case UpdateChecker::CheckSource::IFWCheck:
            UpdateChecker::tryToLaunchUpdater();
            this->forcedClose();
        break;
        }
    }

    this->UpdateSearch_switchUI(false);
}

void MainWindow::requireUpdateCheckFromUser() {
    this->userNotificationOnUpdateCheck = true;
    this->checkForAppUpdates();
}

void MainWindow::checkForAppUpdates() {
    this->UpdateSearch_switchUI(true);
    this->updateCheckerWorker.start();
}

void MainWindow::updateStatusBar(const QString &message, const ConnectivityIndicator &indic) {
    this->statusLabel->setText(message);
    this->statusLight->setCurrentIndex(static_cast<int>(indic));
}

void MainWindow::runMBeat() {
    //
    auto &thread = this->mbeatWorker;

    //
    if(thread && thread->isRunning()) {
        thread->quit();
        thread->wait();
    }

    //
    thread = new MBeatThread(this->appSettings.getConnectivityInfos());

        QObject::connect(
            thread, &MBeatThread::updateConnectivityStatus,
            this, &MainWindow::updateStatusBar
        );

        QObject::connect(
            thread, &QThread::finished,
            thread, &QObject::deleteLater
        );

        QObject::connect(
            thread, &QObject::destroyed,
            [&thread]() { 
                thread = nullptr; 
            }
        );

    thread->start();
}

void MainWindow::runShouts() {
    auto &thread = this->shoutWorker;

    //
    if(thread && thread->isRunning()) {
        thread->quit();
        thread->wait();
    }

    thread = new ShoutThread(this->appSettings.getConnectivityInfos());

        this->shoutTab->bindWithWorker(thread);
        QObject::connect(
            thread, &QThread::finished,
            thread, &QObject::deleteLater
        );

        QObject::connect(
            thread, &QObject::destroyed,
            [&thread]() { 
                thread = nullptr; 
            }
        );

    thread->start();
}

void MainWindow::runFeeder() {
    //
    auto &thread = this->feederWorker;

    //
    if(thread && thread->isRunning()) {
        thread->quit();
        thread->wait();
    }

    thread = new FeederThread(this->appSettings.getConnectivityInfos());

        QObject::connect(
            thread, &FeederThread::filesGenerated,
            this, &MainWindow::updateWarningsMenuItem
        );

        this->feederTab->bindWithWorker(thread);

        QObject::connect(
            thread, &QThread::finished,
            thread, &QObject::deleteLater
        );

        QObject::connect(
            thread, &QObject::destroyed,
            [&thread]() { 
                thread = nullptr;
            }
        );

    thread->start();
}

//
// UI
//

// main initialization
void MainWindow::_initUI() {
    // values specific to this
    #ifdef _DEBUG
        QString stdTitle = (QString)"DEBUG - " + APP_NAME;
    #else
        QString stdTitle = APP_NAME;
    #endif
    
    this->setWindowTitle(stdTitle);
    this->setMinimumSize(QSize(480, 400));
    this->setWindowIcon(QIcon(":/icons/app.png"));

    // helpers
    this->_initUITabs();
    this->_initUITray();
    this->_initUIMenu();
    this->_initStatusBar();

    #ifdef _DEBUG
        this->trueShow();
    #endif
}


//////////
// Tray //
//////////

void MainWindow::_initUITray() {
    auto trayIcon = new QSystemTrayIcon;
    this->trayIcon = trayIcon;
    trayIcon->setIcon(QIcon(":/icons/app_reverse.png"));
    trayIcon->setToolTip(this->windowTitle());

    QObject::connect(
        this->trayIcon, &QSystemTrayIcon::activated,
        this, &MainWindow::iconActivated
    );

    QMenu* cMenu;

    #ifdef _WIN32
        cMenu = this->_getFileMenu(true);
    #endif

    #ifdef __APPLE__
        cMenu = new QMenu("");
        cMenu->addMenu(this->_getFileMenu(true));
        cMenu->addMenu(this->_getOptionsMenu());
    #endif

    this->trayIcon->setContextMenu(cMenu);

    trayIcon->show();
}

void MainWindow::_initStatusBar() {
    //
    auto statusBar = new QStatusBar(this);

    auto sb_widget = new QWidget;
    this->statusLight = new TrafficLightWidget;
    this->statusLabel = new QLabel;

    // define statusbar content
    sb_widget->setLayout(new QHBoxLayout);
    sb_widget->layout()->addWidget(this->statusLight);
    sb_widget->layout()->addWidget(this->statusLabel);

    // define statusbar
    statusBar->addWidget(sb_widget);
    this->setStatusBar(statusBar);
}

void MainWindow::_initUITabs() {
    auto tabs = new QTabWidget(this);
    this->shoutTab = new ShoutTab(this, &this->appSettings);
    this->feederTab = new FeederTab(this);

    QObject::connect(
        this->shoutTab->tButton, &QPushButton::clicked,
        this, &MainWindow::runShouts
    );

    QObject::connect(
        this->feederTab->tButton, &QPushButton::clicked,
        this, &MainWindow::runFeeder
    );

    tabs->addTab(this->shoutTab, "Shout!");
    tabs->addTab(this->feederTab, "Feeder");
    this->setCentralWidget(tabs);
}

void MainWindow::_initUIMenu() {
    auto menuBar = new QMenuBar;
    menuBar->addMenu(this->_getFileMenu(false));
    menuBar->addMenu(this->_getOptionsMenu());
    this->setMenuWidget(menuBar);
}

//////////////
// End Tray //
//////////////

//////////////////////
/// Menu components //
//////////////////////

QMenu* MainWindow::_getOptionsMenu() {
    QMenu *optionsMenuItem = new QMenu(tr("Options"));

    // for checking available updates
    this->cfugAction = new QAction(tr("Check for updates"), optionsMenuItem);
        QObject::connect(
        this->cfugAction, &QAction::triggered,
        this, &MainWindow::requireUpdateCheckFromUser
    );

    this->versionAction = new QAction(APP_FULL_DENOM, optionsMenuItem);
    this->versionAction->setEnabled(false);

    optionsMenuItem->addAction(this->cfugAction);
    optionsMenuItem->addSeparator();
    optionsMenuItem->addAction(this->versionAction);

    return optionsMenuItem;
}

void MainWindow::UpdateSearch_switchUI(bool isSearching) {
    //
    this->cfugAction->setEnabled(!isSearching);

    //
    QString descr = APP_FULL_DENOM;
    if (isSearching) descr += " - " + tr("Searching for updates...");

    //
    this->versionAction->setText(descr);
}

QMenu* MainWindow::_getFileMenu(bool withMonitor) {
    //
    auto fileMenuItem = new QMenu(tr("File"));

    // myPlatformAction
    this->myPlatformAction = new QAction(tr("My %1").arg(DEST_PLATFORM_PRODUCT_NAME), fileMenuItem);
    myPlatformAction->setEnabled(false);
    QObject::connect(
        myPlatformAction, &QAction::triggered,
        this, &MainWindow::accessPlatform
    );

    // accessPreferencesAction
    auto accessPreferencesAction = new QAction(tr("Preferences"), fileMenuItem);
    QObject::connect(
        accessPreferencesAction, &QAction::triggered,
        this, &MainWindow::accessPreferences
    );

    // openWarningsAction
    this->openWarningsAction = new QAction(tr("Read latest upload warnings report"), fileMenuItem);
    this->openWarningsAction->setEnabled(false);
    QObject::connect(
        this->openWarningsAction, &QAction::triggered,
        this, &MainWindow::openWarnings
    );

    // openData
    auto openDataFolder = new QAction(tr("Access upload data folder"), fileMenuItem);
    QObject::connect(openDataFolder, &QAction::triggered, [=]() {
        auto directory = PlatformHelper::getDataStorageDirectory();
        PlatformHelper::openFileInOS(directory);
    });

    // quit
    auto quitAction = new QAction(tr("Quit"), fileMenuItem);
    QObject::connect(
        quitAction, &QAction::triggered,
        this, &MainWindow::forcedClose
    );

    if(withMonitor) {
        // monitorAction
        auto monitorAction = new QAction(tr("Open monitor..."), fileMenuItem);
        QObject::connect(
            monitorAction, &QAction::triggered,
            this, &MainWindow::trueShow
        );

        fileMenuItem->addAction(monitorAction);
        fileMenuItem->addSeparator();
    }

    fileMenuItem->addAction(accessPreferencesAction);
    fileMenuItem->addAction(myPlatformAction);
    fileMenuItem->addSeparator();
    fileMenuItem->addAction(openWarningsAction);
    fileMenuItem->addAction(openDataFolder);
    fileMenuItem->addSeparator();
    fileMenuItem->addAction(quitAction);

    return fileMenuItem;
}

//////////////////////////
/// End Menu components //
//////////////////////////

//////////////////////
/// Events handling //
//////////////////////

void MainWindow::closeEvent(QCloseEvent *event) {
    // apple specific behaviour, prevent closing
    #ifdef __APPLE__
        if (!this->forceQuitOnMacOS) {
            return this->trueHide(event);
        }
    #endif

    // if running shout thread
    if (this->shoutWorker && this->shoutWorker->isRunning()) {
        auto msgboxRslt = QMessageBox::warning(this,
            tr("Shout worker running !"),
            tr("Shout worker is actually running : Are you sure you want to exit ?"),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No);

        if (msgboxRslt == QMessageBox::Yes) {
            // makes sure to wait for shoutThread to end. Limits COM app retention on Windows
            this->shoutWorker->quit();
            this->shoutWorker->wait();
        } else {
            event->ignore();
            return;
        }
    }

    // if running 
    if (this->mbeatWorker && this->mbeatWorker->isRunning()) {
        this->mbeatWorker->quit();   
    }
    
    // hide trayicon on shutdown for Windows, refereshes the UI frames of system tray
    this->trayIcon->hide();
    QApplication::quit();
}

void MainWindow::trueShow() {
    this->showNormal();
    this->activateWindow();
    this->raise();
}

// hide window on minimize, only triggered on windows
void MainWindow::hideEvent(QHideEvent *event) {
    this->trueHide(event);
}

void MainWindow::trueHide(QEvent* event) {
    event->ignore();
    this->hide();
}

void MainWindow::forcedClose() {
    this->forceQuitOnMacOS = true;
    this->close();
}

// tray click handling
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        this->trueShow();
        break;
    default:
        break;
    }
}

//////////////////////////
/// End Events handling //
//////////////////////////
