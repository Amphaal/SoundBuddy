#include "mainWindow.h"

MainWindow::MainWindow() {
    // generate the UI
    this->_initUI();

    //
    this->onAppSettingsChanged();
    this->startupMBeatThread();
    this->updateWarningsMenuItem();
    this->setupAutoUpdate();
}

void MainWindow::informWarningPresence() {
    this->updateWarningsMenuItem();
}

// updates the menu depending on the config values filled or not
void MainWindow::onAppSettingsChanged() {
    bool shouldActivateLink = !this->appSettings.getConnectivityInfos().getPlaformHomeUrl().isEmpty();
    this->myPlatformAction->setEnabled(shouldActivateLink);
}

void MainWindow::updateWarningsMenuItem() {
    bool hasFeederWarnings = QFileInfo(this->owHelper.getOutputPath()).exists();
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
    PlatformHelper::openFileInOS(this->owHelper.getOutputPath());
}

void MainWindow::setupAutoUpdate() {
    QObject::connect(
        &this->updateChecker, &UpdaterThread::isNewerVersionAvailable,
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
                tr("%1 - Error while checking updates").arg(APP_NAME),
                errMsg,
                QMessageBox::Ok,
                QMessageBox::Ok);

        // no updates
        } else if (!checkResults.hasNewerVersion) {
            QMessageBox::information(this,
                tr("%1 - Checking updates").arg(APP_NAME),
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
        tr("%1 - Update Available").arg(APP_NAME),
        tr("An update is available for %1. Would you like to install it now ?").arg(APP_NAME),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::Yes);

    if (msgboxRslt == QMessageBox::Yes) {
        UpdateChecker::tryToLaunchUpdater();
        this->forcedClose();
    }

    this->UpdateSearch_switchUI(false);
}

void MainWindow::requireUpdateCheckFromUser() {
    this->userNotificationOnUpdateCheck = true;
    this->checkForAppUpdates();
}

void MainWindow::checkForAppUpdates() {
    this->UpdateSearch_switchUI(true);
    this->updateChecker.start();
}

void MainWindow::updateStatusBar(const QString &message, const TLW_Colors &color) {
    this->statusLabel->setText(message);
    this->statusLight->setCurrentIndex(static_cast<int>(color));
}

void MainWindow::startupMBeatThread() {
    //
    this->cw = new MBeatThread(this->appSettings.getConnectivityInfos());

    QObject::connect(
        this->cw, &MBeatThread::updateSIOStatus,
        this, &MainWindow::updateStatusBar
    );

    this->cw->start();
}

void MainWindow::startupShoutThread() {
    this->sw = new ShoutThread(this->appSettings.getConnectivityInfos());
    this->shoutTab->bindWithWorker(this->sw);

    QObject::connect(
        this->sw, &QThread::finished,
        this->sw, &QObject::deleteLater
    );

    this->sw->start();
}


void MainWindow::startupFeederThread() {
    this->fw = new FeederThread(this->appSettings.getConnectivityInfos());
    this->feederTab->bindWithWorker(this->fw);

    QObject::connect(
        this->fw, &ITNZThread::operationFinished,
        this, &MainWindow::updateWarningsMenuItem
    );

    QObject::connect(
        this->fw, &QThread::finished,
        this->fw, &QObject::deleteLater
    );

    this->fw->start();
}

//
// UI
//

// main initialization
void MainWindow::_initUI() {
    // values specific to this
    QString stdTitle = _DEBUG ? (QString)"DEBUG - " + APP_NAME : APP_NAME;
    this->setWindowTitle(stdTitle);
    this->setMinimumSize(QSize(480, 400));
    this->setWindowIcon(QIcon(":/icons/app.png"));

    // helpers
    this->_initUITabs();
    this->_initUITray();
    this->_initUIMenu();
    this->_initStatusBar();

    if (_DEBUG) this->trueShow();
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

    #ifdef _WIN32
        auto cMenu = this->_getFileMenu();
    #endif

    #ifdef APPLE
        auto cMenu = new QMenu("");
        cMenu->addMenu(this->_getFileMenu());
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
        this, &MainWindow::startupShoutThread
    );

    QObject::connect(
        this->feederTab->tButton, &QPushButton::clicked,
        this, &MainWindow::startupFeederThread
    );

    tabs->addTab(this->shoutTab, "Shout!");
    tabs->addTab(this->feederTab, "Feeder");
    this->setCentralWidget(tabs);

    // autostart shout thread ?
    if (this->appSettings.value(AppSettings::MUST_AUTORUN_SHOUT).toBool()) {
        this->shoutTab->tButton->click();
    }
}

void MainWindow::_initUIMenu() {
    auto menuBar = new QMenuBar;
    menuBar->addMenu(this->_getFileMenu());
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

QMenu* MainWindow::_getFileMenu() {
    //
    auto fileMenuItem = new QMenu(tr("File"));

    // monitorAction
    auto monitorAction = new QAction(tr("Open monitor..."), fileMenuItem);
    QObject::connect(
        monitorAction, &QAction::triggered,
        this, &MainWindow::trueShow
    );

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
        PlatformHelper::openUrlInBrowser(PlatformHelper::getDataStorageDirectory());
    });

    // quit
    auto quitAction = new QAction(tr("Quit"), fileMenuItem);
    QObject::connect(
        quitAction, &QAction::triggered,
        this, &MainWindow::forcedClose
    );

    fileMenuItem->addAction(monitorAction);
    fileMenuItem->addSeparator();
    fileMenuItem->addAction(myPlatformAction);
    fileMenuItem->addAction(accessPreferencesAction);
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
    #ifdef APPLE
        if (!this->forceQuitOnMacOS) {
            return this->trueHide(event);
        }
    #endif

    // if running shout thread
    if (this->sw && this->sw->isRunning()) {
        auto msgboxRslt = QMessageBox::warning(this,
            tr("Shout worker running !"),
            tr("Shout worker is actually running : Are you sure you want to exit ?"),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No);

        if (msgboxRslt == QMessageBox::Yes) {
            // makes sure to wait for shoutThread to end. Limits COM app retention on Windows
            this->sw->quit();
            this->sw->wait();
        } else {
            event->ignore();
            return;
        }
    }

    // hide trayicon on shutdown for Windows, refereshes the UI frames of system tray
    this->trayIcon->hide();
    QCoreApplication::quit();
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
