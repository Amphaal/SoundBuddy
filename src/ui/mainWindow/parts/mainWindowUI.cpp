#include "src/ui/mainWindow/mainWindow.h"

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
