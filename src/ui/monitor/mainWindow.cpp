#include "mainWindow.h"

#include <IFWUpdateChecker.hpp>

MainWindow::MainWindow() : aHelper(), cHelper(), owHelper(WARNINGS_FILE_PATH), updateChecker(APP_REMOTE_MANIFEST_URL) {
    
    //generate the UI
    this->_initUI();

    this->setupConfigFileWatcher();
    this->startupConnectivityThread();
    this->updateWarningsMenuItem();
    this->setupAutoUpdate();
    
    
};

void MainWindow::informWarningPresence() {
    this->updateWarningsMenuItem();
};


//set watcher on config file
void MainWindow::setupConfigFileWatcher() {
    this->updateMenuItemsFromConfigValues();

    auto configFilePath = this->aHelper.getConfigFileFullPath();
    auto filesToWatch = QStringList(configFilePath);
    this->configWatcher = new QFileSystemWatcher(filesToWatch);
    
    QObject::connect(
        this->configWatcher, &QFileSystemWatcher::fileChanged,
        this, &MainWindow::updateMenuItemsFromConfigValues
    );
};

//updates the menu depending on the config values filled or not
void MainWindow::updateMenuItemsFromConfigValues(const QString &path) {

    //check then save
    auto myWtnzUrl = this->aHelper.getUsersHomeUrl();
    bool shouldActivateLink = (myWtnzUrl != "");
    if(shouldActivateLink) {
        this->wtnzUrl = myWtnzUrl;
    }
    
    //update action state
    for (auto action: this->myWTNZActions){action->setEnabled(shouldActivateLink);}
    
}; 

void MainWindow::updateWarningsMenuItem() {    
    bool doEnable = PlatformHelper::fileExists(this->owHelper.getOutputPath());

    for (auto action: this->warningsfileActions){action->setEnabled(doEnable);}
};

///
/// Functionnalities helpers calls
///


void MainWindow::accessWTNZ() {
    PlatformHelper::openUrlInBrowser(this->wtnzUrl);
};

//open the config file into the OS browser
void MainWindow::openConfigFile() {
    this->aHelper.openConfigFile();
};


//open the warnings file on the OS
void MainWindow::openWarnings() {
    PlatformHelper::openFileInOS(this->owHelper.getOutputPath());
};

//change startup
void MainWindow::addToStartupSwitch(bool checked) {
    PlatformHelper::switchStartupLaunch();
};


void MainWindow::setupAutoUpdate() {   
	QObject::connect(this->updateChecker, &UpdateChecker::isNewerVersionAvailable, 
                     this, &MainWindow::onUpdateChecked);

    //start the update check
    this->checkForAppUpdates();
};

void MainWindow::onUpdateChecked(const UpdateChecker::CheckResults checkResults) {

    //if the user asks directly to check updates
    if(this->userNotificationOnUpdateCheck) {
        // next check will go un-notified
        this->userNotificationOnUpdateCheck = false;

        // error 
        if(checkResults.result != UpdateChecker::CheckCode::Succeeded) {
            //
            QString errMsg;
            switch(checkResults.result) {
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
                QMessageBox::Ok
            );

        // no updates
        } else if(!checkResults.hasNewerVersion) { 
            QMessageBox::information(this, 
                tr("%1 - Checking updates").arg(APP_NAME), 
                tr("No updates available at the time."), 
                QMessageBox::Ok, 
                QMessageBox::Ok
            );
        }
    }

    //no update, no go
    if(!checkResults.hasNewerVersion) {
        this->UpdateSearch_switchUI(false);
        return;
    }

    //if has update
    auto msgboxRslt = QMessageBox::information(this, 
        tr("%1 - Update Available").arg(APP_NAME), 
        tr("An update is available for %1. Would you like to install it now ?").arg(APP_NAME), 
        QMessageBox::Yes | QMessageBox::No, 
        QMessageBox::Yes
    );
    
    if(msgboxRslt == QMessageBox::Yes) {
        this->updater->runUpdaterOnExit();
        this->forcedClose();
    }

    this->UpdateSearch_switchUI(false);
};

void MainWindow::requireUpdateCheckFromUser() {

    this->userNotificationOnUpdateCheck = true;

    if (!this->updater->isRunning()) {
        this->checkForAppUpdates();
    }
};

void MainWindow::checkForAppUpdates() {
    this->UpdateSearch_switchUI(true);
    this->updater->checkForUpdates();
}

void MainWindow::updateStatusBar(const QString &message, const TLW_Colors &color) {
    this->statusLabel->setText(message);
    this->statusLight->setCurrentIndex(color);
}

void MainWindow::startupConnectivityThread() {

    this->cw = new ConnectivityThread(&this->aHelper, this->configWatcher);
    
    QObject::connect(
        this->cw, &ConnectivityThread::updateSIOStatus,
        this, &MainWindow::updateStatusBar
    );
    
    this->cw->start();
};

void MainWindow::startupShoutThread() {
    this->sw = new ShoutThread;
    this->st->bindWithWorker(this->sw);

    QObject::connect(
        this->sw, &QThread::finished,
        this->sw, &QObject::deleteLater
    );

    this->sw->start();
}


void MainWindow::startupFeederThread() {
    this->fw = new FeederThread;
    this->ft->bindWithWorker(this->fw);

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

//main initialization
void MainWindow::_initUI() {

    //values specific to this
    QString stdTitle = _DEBUG ? (QString)"DEBUG - " + APP_NAME : APP_NAME;
    this->setWindowTitle(stdTitle);
    this->setMinimumSize(QSize(480, 400));
    this->setWindowIcon(QIcon(LOCAL_ICON_PNG_PATH));

    //helpers
    this->_initUITabs();
    this->_initUITray();
    this->_initUIMenu();
    this->_initStatusBar();

    if(_DEBUG) this->trueShow();
};


//////////
// Tray //
//////////

void MainWindow::_initUITray() {
    auto trayIcon = new QSystemTrayIcon;
    this->trayIcon = trayIcon;
    trayIcon->setIcon(QIcon(LOCAL_REVERSE_ICON_PNG_PATH));
    trayIcon->setToolTip(this->windowTitle());
    
    QObject::connect(
        this->trayIcon, &QSystemTrayIcon::activated,
        this, &MainWindow::iconActivated
    );

    #ifdef _WIN32
        auto cMenu = this->_getFileMenu();
    #endif

    #ifdef Q_OS_OSX
        auto cMenu = new QMenu("");
        cMenu->addMenu(this->_getFileMenu());
        cMenu->addMenu(this->_getOptionsMenu());
    #endif

    this->trayIcon->setContextMenu(cMenu);

    trayIcon->show();
};

void MainWindow::_initStatusBar() {
    
    auto statusBar = new QStatusBar(this);

    auto sb_widget = new QWidget;
    this->statusLight = new TrafficLightWidget;
    this->statusLabel = new QLabel;

    //define statusbar content
    sb_widget->setLayout(new QHBoxLayout);
    sb_widget->layout()->addWidget(this->statusLight);
    sb_widget->layout()->addWidget(this->statusLabel);

    //define statusbar
    statusBar->addWidget(sb_widget);
    this->setStatusBar(statusBar);
}

void MainWindow::_initUITabs() {
    auto tabs = new QTabWidget;
    this->st = new ShoutTab;
    this->ft = new FeederTab; 

    QObject::connect(
        this->st->tButton, &QPushButton::clicked, 
        this, &MainWindow::startupShoutThread
    );

    QObject::connect(
        this->ft->tButton, &QPushButton::clicked, 
        this, &MainWindow::startupFeederThread
    );

    tabs->addTab(this->st, "Shout!");
    tabs->addTab(this->ft, "Feeder");
    this->setCentralWidget(tabs);

    //autostart
    if (this->cHelper.getParamValue(AUTO_RUN_SHOUT_PARAM_NAME) == "true") {
        this->st->tButton->click();
    }

};

void MainWindow::_initUIMenu() {
    auto menuBar = new QMenuBar;
    menuBar->addMenu(this->_getFileMenu());
    menuBar->addMenu(this->_getOptionsMenu());
    this->setMenuWidget(menuBar);
};

//////////////
// End Tray //
//////////////

//////////////////////
/// Menu components //
//////////////////////

QMenu* MainWindow::_getOptionsMenu() {

    QMenu *optionsMenuItem = new QMenu(tr("Options"));

    //add to system startup Action
    auto atssAction = new QAction(tr("Launch at system boot"), optionsMenuItem);
    atssAction->setCheckable(true);
    QObject::connect(
        atssAction, &QAction::triggered,
        this, &MainWindow::addToStartupSwitch
    );
    if (PlatformHelper::isLaunchingAtStartup()) {
        atssAction->setChecked(true);
    }

    //for checking available updates
    this->cfugAction = new QAction(tr("Check for updates"), optionsMenuItem);
        QObject::connect(
        this->cfugAction, &QAction::triggered,
        this, &MainWindow::requireUpdateCheckFromUser
    );
    
    this->versionAction = new QAction(APP_FULL_DENOM, optionsMenuItem);
    this->versionAction->setEnabled(false);

    optionsMenuItem->addAction(atssAction);
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
    if(isSearching) descr += " - " + tr("Searching for updates...");
    
    //
    this->versionAction->setText(descr);
}

QMenu* MainWindow::_getFileMenu() {
    //
    auto fileMenuItem = new QMenu(tr("File"));

    //monitorAction
    auto monitorAction = new QAction(tr("Open monitor..."), fileMenuItem);
    QObject::connect(
        monitorAction, &QAction::triggered,
        this, &MainWindow::trueShow
    );

    //myWTNZAction
    auto myWTNZAction = new QAction(tr("My WTNZ"), fileMenuItem);
    myWTNZAction->setEnabled(false);
    QObject::connect(
        myWTNZAction, &QAction::triggered,
        this, &MainWindow::accessWTNZ
    );
    this->myWTNZActions.push_back(myWTNZAction);

    //updateConfigAction
    auto updateConfigAction = new QAction(tr("Update configuration file"), fileMenuItem);
    QObject::connect(
        updateConfigAction, &QAction::triggered,
        this, &MainWindow::openConfigFile
    );

    //openWarningsAction
    auto openWarningsAction = new QAction(tr("Read latest upload warnings report"), fileMenuItem);
    openWarningsAction->setEnabled(false);
    QObject::connect(
        openWarningsAction, &QAction::triggered,
        this, &MainWindow::openWarnings
    );
    this->warningsfileActions.push_back(openWarningsAction);

    //openData
    auto openDataFolder = new QAction(tr("Access upload data folder"), fileMenuItem);
    QObject::connect(
        openDataFolder, &QAction::triggered,
        [=]() {
            PlatformHelper::openUrlInBrowser(PlatformHelper::getDataStorageDirectory());
        }
    );

    //quit
    auto quitAction = new QAction(tr("Quit"), fileMenuItem);
    QObject::connect(
        quitAction, &QAction::triggered,
        this, &MainWindow::forcedClose
    );

    fileMenuItem->addAction(monitorAction);
    fileMenuItem->addSeparator();
    fileMenuItem->addAction(myWTNZAction);
    fileMenuItem->addAction(updateConfigAction);
    fileMenuItem->addAction(openWarningsAction);
    fileMenuItem->addAction(openDataFolder);
    fileMenuItem->addSeparator();
    fileMenuItem->addAction(quitAction);

    return fileMenuItem;
};

//////////////////////////
/// End Menu components //
//////////////////////////

//////////////////////
/// Events handling //
//////////////////////

void MainWindow::closeEvent(QCloseEvent *event) {

    //apple specific behaviour, prevent closing
    #ifdef Q_OS_OSX
        if(!this->forceQuitOnMacOS) {
            return this->trueHide(event);
        }
    #endif

    //if running shout thread
    if(this->sw && this->sw->isRunning()) {
        auto msgboxRslt = QMessageBox::warning(this, 
            tr("Shout worker running !"), 
            tr("Shout worker is actually running : Are you sure you want to exit ?"), 
            QMessageBox::Yes | QMessageBox::No, 
            QMessageBox::No
        );
        
        if(msgboxRslt == QMessageBox::Yes) {
            //makes sure to wait for shoutThread to end. Limits COM app retention on Windows
            this->sw->quit();
            this->sw->wait();
        } else {
            event->ignore();
            return;
        }
    }

    //hide trayicon on shutdown for Windows, refereshes the UI frames of system tray
    this->trayIcon->hide();
    QCoreApplication::quit();
};

void MainWindow::trueShow() {
    this->showNormal();
    this->activateWindow();
    this->raise();
};

//hide window on minimize, only triggered on windows
void MainWindow::hideEvent(QHideEvent *event) {
    this->trueHide(event);
};

void MainWindow::trueHide(QEvent* event) {
    event->ignore();
    this->hide();
};

void MainWindow::forcedClose() {
    this->forceQuitOnMacOS = true;
    this->close();
};

//tray click handling
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        this->trueShow();
        break;
    default:
        break;
    }
};

//////////////////////////
/// End Events handling //
//////////////////////////