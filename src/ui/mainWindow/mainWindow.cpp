#include "mainWindow.h"

MainWindow::MainWindow() : aHelper(), cHelper(), owHelper(WARNINGS_FILE_PATH) {     
    
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
    auto filesToWatch = QStringList(configFilePath.toStdString().c_str());
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
    PlatformHelper::openUrlInBrowser(this->wtnzUrl.toStdString().c_str());
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

    if(MAINTENANCE_TOOL_LOCATION == "") {
        this->updater = new QtAutoUpdater::Updater(this);
    }
    else {
         this->updater = new QtAutoUpdater::Updater(MAINTENANCE_TOOL_LOCATION, this);
    }
    
	QObject::connect(this->updater, &QtAutoUpdater::Updater::checkUpdatesDone, 
                     this, &MainWindow::onUpdateChecked);

    //start the update check
    this->checkForAppUpdates();
};

void MainWindow::onUpdateChecked(bool hasUpdate, bool hasError) {

    //if the user asks directly to check updates
    if(this->userNotificationOnUpdateCheck) {
        this->userNotificationOnUpdateCheck = false;
        
        QString title = (QString)APP_NAME + " - " + I18n::tr()->Menu_CheckForUpgrades();
        QString content = this->updater->errorLog();

        if(!hasUpdate && !hasError) {
            QMessageBox::information(this, 
                QString(title.toStdString().c_str()), 
                QString(content.toStdString().c_str()), 
                QMessageBox::Ok, QMessageBox::Ok);
        } else if (hasError) {
            QMessageBox::warning(this, 
                QString(title.toStdString().c_str()), 
                QString(content.toStdString().c_str()), 
                QMessageBox::Ok, QMessageBox::Ok);
        }
    }

    //no update, no go
    if(!hasUpdate) {
        this->UpdateSearch_switchUI(false);
        return;
    }

    //if has update
    QString title = (QString)APP_NAME + " - " + I18n::tr()->Alert_UpdateAvailable_Title();
    QString content = I18n::tr()->Alert_UpdateAvailable_Text();

    auto msgboxRslt = QMessageBox::information(this, 
                QString(title.toStdString().c_str()), 
                QString(content.toStdString().c_str()), 
                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes
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
    this->statusLabel->setText(QString(message.toStdString().c_str()));
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
        [=]() {
            delete this->sw;
            this->sw = nullptr;
        }
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
        [=]() {
            delete this->fw;
            this->fw = nullptr;
        });

    this->fw->start();
}