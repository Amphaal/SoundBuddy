#define _HAS_STD_BYTE 0 //prevent build error on mac (byte type overriding)
#include "mainWindow.h"

MainWindow::MainWindow(QString *title) : 
    title(title), 
    helper(ConfigHelper()), 
    pHelper(PlatformHelper()), 
    owHelper("output\\warnings.json") {     
    this->updateConfigValues();
    this->setWindowTitle(*title);
    this->_initUI();
    this->setupConfigFileWatcher();
    this->updateWarningsMenuItem();
};

void MainWindow::informWarningPresence() {
    this->updateWarningsMenuItem();
};

void MainWindow::_initUI() {
    this->setMinimumSize(QSize(480, 400));
    this->_initUITabs();
    this->_initUITray();
    this->_initUIMenu();
};

void MainWindow::_initUITabs() {
    QTabWidget *tabs = new QTabWidget;
    this->shoutTab = new ShoutTab(tabs, &this->helper, this->config);
    FeederTab *feedTab = new FeederTab(tabs);

    tabs->addTab(shoutTab, "Shout!");
    tabs->addTab(feedTab, "Feeder");

    this->setCentralWidget(tabs);
};

void MainWindow::_initUIMenu() {
    QMenuBar *menuBar = new QMenuBar;
    menuBar->addMenu(this->_getMenu());
    this->setMenuWidget(menuBar);
};

void MainWindow::_initUITray() {
    QSystemTrayIcon *trayIcon = new QSystemTrayIcon;
    this->trayIcon = trayIcon;
    trayIcon->setIcon(QIcon(":/icons/feedtnz.png"));
    trayIcon->setToolTip(*this->title);

    //double it to the tray icon
    this->trayIcon->setContextMenu(this->_getMenu());

    trayIcon->show();
};

QMenu* MainWindow::_getMenu() {

    QMenu *fileMenuItem = new QMenu(I18n::tr()->Menu_File().c_str());

    //monitorAction
    QAction *monitorAction = new QAction(I18n::tr()->Menu_OpenMonitor().c_str(), fileMenuItem);
    QObject::connect(
        monitorAction, &QAction::triggered,
        this, &MainWindow::trueShow
    );

    //myWTNZAction
    QAction *myWTNZAction = new QAction(I18n::tr()->Menu_MyWTNZ().c_str(), fileMenuItem);
    myWTNZAction->setEnabled(false);
    QObject::connect(
        myWTNZAction, &QAction::triggered,
        this, &MainWindow::accessWTNZ
    );
    this->myWTNZActions.push_back(myWTNZAction);

    //updateConfigAction
    QAction *updateConfigAction = new QAction(I18n::tr()->Menu_UpdateConfig().c_str(), fileMenuItem);
    QObject::connect(
        updateConfigAction, &QAction::triggered,
        this, &MainWindow::openConfigFile
    );

    //openWarningsAction
    QAction *openWarningsAction = new QAction(I18n::tr()->Menu_OpenWarnings().c_str(), fileMenuItem);
    openWarningsAction->setEnabled(false);
    QObject::connect(
        openWarningsAction, &QAction::triggered,
        this, &MainWindow::openWarnings
    );
    this->warningsfileActions.push_back(openWarningsAction);

    //quit
    QAction *quitAction = new QAction(I18n::tr()->Menu_Quit().c_str(), fileMenuItem);
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
    this->config = this->helper.accessConfig();
};

//set watcher on config file
void MainWindow::setupConfigFileWatcher() {
    this->updateMenuItemsFromConfigValues();

    std::string f = this->helper.getConfigFileFullPath();
    this->configWatcher = new QFileSystemWatcher(QStringList(f.c_str()), this);
    
    QObject::connect(this->configWatcher, &QFileSystemWatcher::fileChanged,
            this, &MainWindow::updateMenuItemsFromConfigValues);
};

//updates the menu depending on the config values filled or not
void MainWindow::updateMenuItemsFromConfigValues(const QString &path) {
    this->updateConfigValues();

    bool WTNZUrlAvailable = !this->config["targetUrl"].empty() && !this->config["user"].empty();
    if (WTNZUrlAvailable) {
        
        //set new WTNZ Url
        this->wtnzUrl = this->config["targetUrl"].get<std::string>();
        this->wtnzUrl += "/";
        this->wtnzUrl += this->config["user"].get<std::string>();

        //update action state
        for (QAction *action: this->myWTNZActions){action->setEnabled(true);}
    } else {
        //update action state
        for (QAction *action: this->myWTNZActions){action->setEnabled(false);}
    }
}; 

void MainWindow::updateWarningsMenuItem() {    
    bool doEnable = this->helper.fileExists(this->owHelper.getOutputPath());

    for (QAction *action: this->warningsfileActions){action->setEnabled(doEnable);}
};

///
/// Functionnalities helpers calls
///


void MainWindow::accessWTNZ() {
    this->pHelper.openUrlInBrowser(this->wtnzUrl.c_str());
};

//open the config file into the OS browser
void MainWindow::openConfigFile() {
    this->helper.openConfigFile();
};


//open the warnings file on the OS
void MainWindow::openWarnings() {
    this->pHelper.openFileInOS(this->owHelper.getOutputPath());
};

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
