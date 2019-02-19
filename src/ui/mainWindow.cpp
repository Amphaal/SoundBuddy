#define _HAS_STD_BYTE 0 //prevent build error on mac (byte type overriding)
#include "mainWindow.h"

MainWindow::MainWindow() : 
    cHelper(ConfigHelper()), 
    pHelper(PlatformHelper()), 
    owHelper(WARNINGS_FILE_PATH) {     
    
    this->updateConfigValues();

    std::string stdTitle = IS_DEBUG_APP ? (std::string)"DEBUG - " + APP_NAME : APP_NAME;

    this->setWindowTitle(QString(stdTitle.c_str()));
    this->_initUI();
    this->setupConfigFileWatcher();
    this->updateWarningsMenuItem();
    this->setupAutoUpdate();
    
};

void MainWindow::informWarningPresence() {
    this->updateWarningsMenuItem();
};

void MainWindow::_initUI() {
    this->setMinimumSize(QSize(480, 400));
    this->setWindowIcon(QIcon(LOCAL_ICON_PNG_PATH.c_str()));
    this->_initUITabs();
    this->_initUITray();
    this->_initUIMenu();
    this->_initStatusBar();
    this->startupWS();
};

void MainWindow::_initUITabs() {
    auto tabs = new QTabWidget;
    this->shoutTab = new ShoutTab(tabs, &this->cHelper);
    auto feedTab = new FeederTab(tabs);

    tabs->addTab(shoutTab, "Shout!");
    tabs->addTab(feedTab, "Feeder");

    this->setCentralWidget(tabs);
};

void MainWindow::_initUIMenu() {
    auto menuBar = new QMenuBar;
    menuBar->addMenu(this->_getFileMenu());
    menuBar->addMenu(this->_getOptionsMenu());
    this->setMenuWidget(menuBar);
};

void MainWindow::_initStatusBar() {
    
    //this->statusLight = new LightWidget(Qt::red);
    this->statusLabel = new QLabel;

    //auto sb_layout = new QHBoxLayout;
    //sb_layout->addWidget(this->statusLight, 0);
    //sb_layout->addWidget(this->statusLabel, 0);

    //auto sb_widget = new QWidget;
    //sb_widget->setLayout(sb_layout);

    auto statusBar = new QStatusBar;
    //statusBar->addWidget(sb_widget, 1);
    statusBar->addPermanentWidget(this->statusLabel, 1);
    this->setStatusBar(statusBar);

    //declare waiting for connection
    this->updateSIOStatus(I18n::tr()->SIOWaitingConnection());
}

void MainWindow::_initUITray() {
    auto trayIcon = new QSystemTrayIcon;
    this->trayIcon = trayIcon;
    trayIcon->setIcon(QIcon(LOCAL_REVERSE_ICON_PNG_PATH.c_str()));
    trayIcon->setToolTip(this->windowTitle());
    
    QObject::connect(
        this->trayIcon, &QSystemTrayIcon::activated,
        this, &MainWindow::iconActivated
    );

    #ifdef _WIN32
        auto cMenu = this->_getFileMenu();
    #endif

    #ifdef __APPLE__
        auto cMenu = new QMenu("");
        cMenu->addMenu(this->_getFileMenu());
        cMenu->addMenu(this->_getOptionsMenu());
    #endif

    this->trayIcon->setContextMenu(cMenu);

    trayIcon->show();
};

QMenu* MainWindow::_getOptionsMenu() {

    QMenu *optionsMenuItem = new QMenu(I18n::tr()->Menu_Options().c_str());

    //add to system startup Action
    auto atssAction = new QAction(I18n::tr()->Menu_AddToStartup().c_str(), optionsMenuItem);
    atssAction->setCheckable(true);
    QObject::connect(
        atssAction, &QAction::triggered,
        this, &MainWindow::addToStartupSwitch
    );
    if (this->pHelper.isLaunchingAtStartup()) {
        atssAction->setChecked(true);
    }

    //for checking the upgrades available
    auto cfugAction = new QAction(I18n::tr()->Menu_CheckForUpgrades().c_str(), optionsMenuItem);
        QObject::connect(
        cfugAction, &QAction::triggered,
        this, &MainWindow::requireUpdateCheckFromUser
    );
    
    auto versionAction = new QAction(APP_FULL_DENOM, optionsMenuItem);
    versionAction->setEnabled(false);

    optionsMenuItem->addAction(atssAction);
    optionsMenuItem->addAction(cfugAction);
    optionsMenuItem->addSeparator();
    optionsMenuItem->addAction(versionAction);

    return optionsMenuItem;
}

QMenu* MainWindow::_getFileMenu() {

    auto fileMenuItem = new QMenu(I18n::tr()->Menu_File().c_str());

    //monitorAction
    auto monitorAction = new QAction(I18n::tr()->Menu_OpenMonitor().c_str(), fileMenuItem);
    QObject::connect(
        monitorAction, &QAction::triggered,
        this, &MainWindow::trueShow
    );

    //myWTNZAction
    auto myWTNZAction = new QAction(I18n::tr()->Menu_MyWTNZ().c_str(), fileMenuItem);
    myWTNZAction->setEnabled(false);
    QObject::connect(
        myWTNZAction, &QAction::triggered,
        this, &MainWindow::accessWTNZ
    );
    this->myWTNZActions.push_back(myWTNZAction);

    //updateConfigAction
    auto updateConfigAction = new QAction(I18n::tr()->Menu_UpdateConfig().c_str(), fileMenuItem);
    QObject::connect(
        updateConfigAction, &QAction::triggered,
        this, &MainWindow::openConfigFile
    );

    //openWarningsAction
    auto openWarningsAction = new QAction(I18n::tr()->Menu_OpenWarnings().c_str(), fileMenuItem);
    openWarningsAction->setEnabled(false);
    QObject::connect(
        openWarningsAction, &QAction::triggered,
        this, &MainWindow::openWarnings
    );
    this->warningsfileActions.push_back(openWarningsAction);

    //quit
    auto quitAction = new QAction(I18n::tr()->Menu_Quit().c_str(), fileMenuItem);
    QObject::connect(
        quitAction, &QAction::triggered,
        this, &MainWindow::forcedClose
    );

    fileMenuItem->addAction(monitorAction);
    fileMenuItem->addSeparator();
    fileMenuItem->addAction(myWTNZAction);
    fileMenuItem->addAction(updateConfigAction);
    fileMenuItem->addAction(openWarningsAction);
    fileMenuItem->addSeparator();
    fileMenuItem->addAction(quitAction);

    return fileMenuItem;
};

void MainWindow::updateConfigValues() {
    this->config = this->cHelper.accessConfig();
};

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        this->trueShow();
        break;
    }
};

//set watcher on config file
void MainWindow::setupConfigFileWatcher() {
    this->updateMenuItemsFromConfigValues();

    auto configFilePath = this->cHelper.getFullPath();
    auto filesToWatch = QStringList(configFilePath.c_str());
    this->configWatcher = new QFileSystemWatcher(filesToWatch);
    
    QObject::connect(this->configWatcher, &QFileSystemWatcher::fileChanged,
            this, &MainWindow::updateMenuItemsFromConfigValues);
};

//updates the menu depending on the config values filled or not
void MainWindow::updateMenuItemsFromConfigValues(const QString &path) {
    
    //update config state from file
    this->updateConfigValues();

    // force rechecking credentials
    this->sio_loggedInUser = "";
    this->checkCredentials(true);

    //check then save
    auto myWtnzUrl = this->cHelper.getUsersHomeUrl(this->config);
    bool shouldActivateLink = (myWtnzUrl != "");
    if(shouldActivateLink) {
        this->wtnzUrl = myWtnzUrl;
    }
    
    //update action state
    for (auto action: this->myWTNZActions){action->setEnabled(shouldActivateLink);}
    
}; 

void MainWindow::updateWarningsMenuItem() {    
    bool doEnable = this->pHelper.fileExists(this->owHelper.getOutputPath());

    for (auto action: this->warningsfileActions){action->setEnabled(doEnable);}
};

///
/// Functionnalities helpers calls
///


void MainWindow::accessWTNZ() {
    this->pHelper.openUrlInBrowser(this->wtnzUrl.c_str());
};

//open the config file into the OS browser
void MainWindow::openConfigFile() {
    this->cHelper.openConfigFile();
};


//open the warnings file on the OS
void MainWindow::openWarnings() {
    this->pHelper.openFileInOS(this->owHelper.getOutputPath());
};

//change startup
void MainWindow::addToStartupSwitch(bool checked) {
    this->pHelper.switchStartupLaunch();
}

///
/// Events handling
///

//hide window on minimize, only triggered on windows
void MainWindow::hideEvent(QHideEvent *event) {
    this->trueHide(event);
};

void MainWindow::closeEvent(QCloseEvent *event) {
    
    //apple specific behaviour, prevent closing
    #ifdef __APPLE__
        if(!this->forceQuitOnMacOS) {
            return this->trueHide(event);
        }
    #endif

    //if running shout thread
    if(this->shoutTab->isWorkerRunning()) {
        auto msgboxRslt = QMessageBox::warning(this, QString(I18n::tr()->Alert_RunningWorker_Title().c_str()), 
                    QString(I18n::tr()->Alert_RunningWorker_Text().c_str()), 
                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        
        if(msgboxRslt == QMessageBox::Yes) {
            //makes sure to wait for shoutThread to end. Limits COM app retention on Windows
            this->shoutTab->endThread();
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

void MainWindow::trueHide(QEvent* event) {
    event->ignore();
    this->hide();
};

void MainWindow::forcedClose() {
    this->forceQuitOnMacOS = true;
    this->close();
};

void MainWindow::setupAutoUpdate() {

    if(MAINTENANCE_TOOL_LOCATION == "") {
        this->updater = new QtAutoUpdater::Updater(this);
    }
    else {
         this->updater = new QtAutoUpdater::Updater(MAINTENANCE_TOOL_LOCATION, this);
    }
    

	QObject::connect(this->updater, &QtAutoUpdater::Updater::checkUpdatesDone, 
                     this, &MainWindow::onUpdateChecked);

    //start the update check
    this->updater->checkForUpdates();
}

void MainWindow::onUpdateChecked(bool hasUpdate, bool hasError) {

    //if the user asks directly to check updates
    if(this->userNotificationOnUpdateCheck) {
        this->userNotificationOnUpdateCheck = false;
        
        std::string title = (std::string)APP_NAME + " - " + I18n::tr()->Menu_CheckForUpgrades();
        std::string content = this->updater->errorLog().toStdString();

        if(!hasUpdate && !hasError) {
            QMessageBox::information(this, 
                QString(title.c_str()), 
                QString(content.c_str()), 
                QMessageBox::Ok, QMessageBox::Ok);
        } else if (hasError) {
            QMessageBox::warning(this, 
                QString(title.c_str()), 
                QString(content.c_str()), 
                QMessageBox::Ok, QMessageBox::Ok);
        }
    }

    //no update, no go
    if(!hasUpdate) return;

    //if has update
    std::string title = (std::string)APP_NAME + " - " + I18n::tr()->Alert_UpdateAvailable_Title();
    std::string content = I18n::tr()->Alert_UpdateAvailable_Text();

    auto msgboxRslt = QMessageBox::information(this, 
                QString(title.c_str()), 
                QString(content.c_str()), 
                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes
    );
    
    if(msgboxRslt == QMessageBox::Yes) {
        this->updater->runUpdaterOnExit();
        this->forcedClose();
    }
}

void MainWindow::requireUpdateCheckFromUser() {

    this->userNotificationOnUpdateCheck = true;

    if (!this->updater->isRunning()) {
        this->updater->checkForUpdates();
    }
}

void MainWindow::startupWS() {

        //extract destination url for sio connection
        auto t_qurl = this->cHelper.getTargetUrl(this->config);
        t_qurl->setPort(SIO_PORT);
        auto turl = t_qurl->toString(QUrl::RemovePath).toStdString();
        delete t_qurl;
        
        ////////////////////
        // Event Handlers //
        ////////////////////

        //tell sio is trying to reconnect
        this->sioClient.set_reconnect_listener([&](unsigned int a, unsigned int b) {
            this->updateSIOStatus(I18n::tr()->SIOReconnecting());
        });

        //on connect, check credentials
        this->sioClient.set_open_listener([&]() {
            checkCredentials();
        });

        //once server checked the credentials
        this->sioClient.socket("/login")->on("credentialsChecked", [&](sio::event& ev) {
            
            //extract response
            auto response = ev.get_messages()[0]->get_map();
            auto isOk = response["isLoginOk"]->get_bool();
            std::string accomp = response["accomp"]->get_string();

            if(isOk) {
                this->sio_loggedInUser = accomp;
                this->updateSIOStatus(I18n::tr()->SIOLoggedAs(accomp));
            } else {
                this->updateSIOStatus(I18n::tr()->SIOErrorOnValidation(accomp));
            }

            //toggle flag
            this->sio_requestOngoing = false;
        });

        //when server tell us the database has been updated, ask for revalidation
        this->sioClient.socket("/login")->on("databaseUpdated", [&](sio::event& ev) {
            checkCredentials(true);
        });

        ////////////////////////
        // End Event Handlers //
        ////////////////////////

        //connect...
        this->sioClient.connect(turl);
    
}

void MainWindow::updateSIOStatus(std::string newMessage) {
    this->statusBar()->clearMessage();
    this->statusLabel->setText(QString(newMessage.c_str()));
}

//ask credentials
void MainWindow::checkCredentials(bool forceRecheck) {
    
    if(forceRecheck) {
        this->sio_requestOngoing = false;
    }

    if(this->sio_loggedInUser != "" && !forceRecheck) {
        this->updateSIOStatus(I18n::tr()->SIOLoggedAs(this->sio_loggedInUser));
        return;
    }

    auto prerequisitesOK = this->cHelper.ensureConfigFileIsReadyForUpload(this->config, false);
    
    if (!prerequisitesOK) {
        this->updateSIOStatus(I18n::tr()->SIOWaitingCredentials());
    }
    else if(prerequisitesOK && !this->sio_requestOngoing) {
        
        //start check
        this->sio_requestOngoing = true;
        this->updateSIOStatus(I18n::tr()->SIOAskingCredentialValidation());
        
        sio::message::list p;
        auto username = this->cHelper.getParamValue(this->config, "username");
        auto password = this->cHelper.getParamValue(this->config, "password");

        p.push(sio::string_message::create(username.c_str()));
        p.push(sio::string_message::create(password.c_str()));
        this->sioClient.socket("/login")->emit_socket("checkCredentials", p);
        
    }
}