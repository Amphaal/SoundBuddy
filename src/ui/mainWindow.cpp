#include "QtCore/QString"
#include "QtCore/QSize"
#include "QtCore/QObject"
#include "QtWidgets/QTabWidget"
#include "QtWidgets/QMainWindow"
#include "QtWidgets/QWidget"
#include "QtWidgets/QMenuBar"
#include "QtWidgets/QMenu"
#include "QtWidgets/QAction"
#include "QtWidgets/QSystemTrayIcon"
#include "QtGui/QCloseEvent"
#include "QtGui/QHideEvent"
#include "QtGui/QIcon"

#include "feedtnzTab.cpp"
#include "../helpers/configHelper.cpp"

class MainWindow : public QMainWindow {

    QString *title;
    ConfigHelper helper;
    QSystemTrayIcon *trayIcon;
    bool forceQuitOnMacOS = false;
    vector<QAction> myWTNZActions;
    
    public:
    MainWindow(QString *title) : title(title), helper(ConfigHelper()) {     
        this->helper.accessConfig();
        this->setWindowTitle(*title);
        this->_initUI();
    }
    
    private:
    
    ///
    ///UI instanciation
    ///

    void _initUI() {
        this->setMinimumSize(QSize(480, 400));
        this->_initUITabs();
        this->_initUITray();
        this->_initUIMenu();
    }

    void _initUITabs() {
        QTabWidget *tabs = new QTabWidget;
        FeedTNZTab *shoutTab = new FeedTNZTab(tabs);
        FeedTNZTab *feedTab = new FeedTNZTab(tabs);

        tabs->addTab(shoutTab, "Shout!");
        tabs->addTab(feedTab, "Feeder");

        this->setCentralWidget(tabs);
    }

    void _initUIMenu() {
        QMenuBar *menuBar = new QMenuBar;
        menuBar->addMenu(this->_getMenu());
        this->setMenuWidget(menuBar);
    }

    void _initUITray() {
        QSystemTrayIcon *trayIcon = new QSystemTrayIcon;
        this->trayIcon = trayIcon;
        trayIcon->setIcon(QIcon(":/icons/feedtnz.png"));
        trayIcon->setToolTip(*this->title);

        //double it to the tray icon
        this->trayIcon->setContextMenu(this->_getMenu());

        trayIcon->show();
    }

    QMenu* _getMenu() {

        QMenu *fileMenuItem = new QMenu("File");

        //monitorAction
        QAction *monitorAction = new QAction("Open monitor...", fileMenuItem);
        QObject::connect(
            monitorAction, &QAction::triggered,
            this, &MainWindow::trueShow
        );

        //myWTNZAction
        QAction *myWTNZAction = new QAction("My WTNZ", fileMenuItem);
        myWTNZAction->setEnabled(false);
        QObject::connect(
            myWTNZAction, &QAction::triggered,
            this, &MainWindow::accessWTNZ
        );

        //updateConfigAction
        QAction *updateConfigAction = new QAction("Update configuration file", fileMenuItem);
        QObject::connect(
            updateConfigAction, &QAction::triggered,
            this, &MainWindow::openConfigFile
        );

        //quit
        QAction *quitAction = new QAction("Quit", fileMenuItem);
        QObject::connect(
            quitAction, &QAction::triggered,
            this, &MainWindow::forcedClose
        );

        fileMenuItem->addAction(monitorAction);
        fileMenuItem->addSeparator();
        fileMenuItem->addAction(myWTNZAction);
        fileMenuItem->addAction(updateConfigAction);
        fileMenuItem->addSeparator();
        fileMenuItem->addAction(quitAction);

        return fileMenuItem;
    }

    ///
    /// Functionnalities helpers calls
    ///


    void accessWTNZ() {

    }

    //open the config file into the OS browser
    void openConfigFile() {
        this->helper.openConfigFile();
    }

    ///
    /// Events handling
    ///

    //hide window on minimize, only triggered on windows
    void hideEvent(QEvent* event)
    {
        this->trueHide(event);
    }

    void closeEvent(QCloseEvent *event) {
        
        //apple specific behaviour, prevent closing
        #ifdef __APPLE__
            if(!this->forceQuitOnMacOS) {
                return this->trueHide(event);
            }
        #endif

        //hide trayicon on shutdown for Windows, refereshes the UI frames of system tray
        this->trayIcon->hide();
    }

    void trueShow() {
        this->showNormal();
        this->activateWindow();
        this->raise();
    }

    void trueHide(QEvent* event) {
        event->ignore();
        this->hide();
    }

    void forcedClose() {
        this->forceQuitOnMacOS = true;
        this->close();
    }
};