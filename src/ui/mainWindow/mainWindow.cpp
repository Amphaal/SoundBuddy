#define _HAS_STD_BYTE 0 //prevent build error on mac (byte type overriding)
#include "mainWindow.h"

MainWindow::MainWindow() : aHelper(), cHelper(), owHelper(WARNINGS_FILE_PATH) {     
    
    //generate the UI
    this->_initUI();

    this->startupWS();
    this->setupConfigFileWatcher();
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
    auto filesToWatch = QStringList(configFilePath.c_str());
    this->configWatcher = new QFileSystemWatcher(filesToWatch);
    
    QObject::connect(this->configWatcher, &QFileSystemWatcher::fileChanged,
            this, &MainWindow::updateMenuItemsFromConfigValues);
};

//updates the menu depending on the config values filled or not
void MainWindow::updateMenuItemsFromConfigValues(const QString &path) {

    // force rechecking credentials
    this->sio_loggedInUser = "";
    this->checkCredentials(true);

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
    PlatformHelper::openUrlInBrowser(this->wtnzUrl.c_str());
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
}


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
        auto t_qurl = this->aHelper.getTargetUrl();
        t_qurl->setPort(SIO_PORT);
        auto turl = t_qurl->toString(QUrl::RemovePath).toStdString();
        delete t_qurl;
        
        ////////////////////
        // Event Handlers //
        ////////////////////

        //tell sio is trying to reconnect
        this->sioClient.set_reconnect_listener([&](unsigned int a, unsigned int b) {
            this->updateSIOStatus(I18n::tr()->SIOReconnecting(), TLW_Colors::YELLOW);
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
                this->updateSIOStatus(I18n::tr()->SIOLoggedAs(accomp), TLW_Colors::GREEN);
            } else {
                this->updateSIOStatus(I18n::tr()->SIOErrorOnValidation(accomp), TLW_Colors::RED);
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

        
        //declare waiting for connection
        this->updateSIOStatus(I18n::tr()->SIOWaitingConnection(), TLW_Colors::YELLOW);
        
        //connect...
        this->sioClient.connect(turl);
}

void MainWindow::updateSIOStatus(std::string newMessage, TLW_Colors colorToApply) {
    this->statusLabel->setText(QString(newMessage.c_str()));
    this->statusLight->setCurrentIndex(colorToApply);
}

//ask credentials
void MainWindow::checkCredentials(bool forceRecheck) {
    
    if(forceRecheck) {
        this->sio_requestOngoing = false;
    }

    if(this->sio_loggedInUser != "" && !forceRecheck) {
        this->updateSIOStatus(I18n::tr()->SIOLoggedAs(this->sio_loggedInUser), TLW_Colors::GREEN);
        return;
    }

    auto prerequisitesOK = this->aHelper.ensureConfigFileIsReadyForUpload(false);
    
    if (!prerequisitesOK) {
        this->updateSIOStatus(I18n::tr()->SIOWaitingCredentials(), TLW_Colors::RED);
    }
    else if(prerequisitesOK && !this->sio_requestOngoing) {
        
        //start check
        this->sio_requestOngoing = true;
        this->updateSIOStatus(I18n::tr()->SIOAskingCredentialValidation(), TLW_Colors::YELLOW);
        
        sio::message::list p;
        auto username = this->aHelper.getParamValue("username");
        auto password = this->aHelper.getParamValue("password");

        p.push(sio::string_message::create(username.c_str()));
        p.push(sio::string_message::create(password.c_str()));
        this->sioClient.socket("/login")->emit_socket("checkCredentials", p);
        
    }
}