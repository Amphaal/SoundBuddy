#include "QtCore/QString"
#include "QtCore/QSize"
#include "QtCore/QObject"
#include "QtCore/QFileSystemWatcher"
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

#include "nlohmann/json.hpp"

#include "feedtnzTab.cpp"
#include "../helpers/configHelper.cpp"
#include "../helpers/platformHelper/platformHelper.h"

class MainWindow : public QMainWindow {
   
    public:
        MainWindow(QString *title) : title(title), helper(ConfigHelper()), pHelper(PlatformHelper()) {     
            this->setWindowTitle(*title);
            this->_initUI();
            this->setupConfigFile();
        }
    
    private:
        bool forceQuitOnMacOS = false;
        QString *title;
        ConfigHelper helper;
        QSystemTrayIcon *trayIcon;
        vector<QAction*> myWTNZActions;
        nlohmann::json *config;
        QFileSystemWatcher *watcher;
        string wtnzUrl;
        PlatformHelper pHelper;
    
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
            this->myWTNZActions.push_back(myWTNZAction);

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


        void setupConfigFile() {
            this->updateMenuItems();

            std::string f = this->helper.getConfigFileFullPath();
            QFileSystemWatcher *watcher = new QFileSystemWatcher(QStringList(f.c_str()), this);
            this->watcher = watcher;
            
            connect(watcher, &QFileSystemWatcher::fileChanged,
                    this, &MainWindow::updateMenuItems);
        }

        void updateMenuItems(const QString &path = NULL) {
            auto lconfig = this->helper.accessConfig();
            this->config = &lconfig;
            bool WTNZUrlAvailable = !lconfig["targetUrl"].empty() && !lconfig["user"].empty();
            if (WTNZUrlAvailable) {
                
                //set new WTNZ Url
                this->wtnzUrl = lconfig["targetUrl"];
                this->wtnzUrl +=  + "/";
                this->wtnzUrl += lconfig["user"];

                //update action state
                for (QAction *action: this->myWTNZActions){action->setEnabled(true);}
            } else {
                //update action state
                for (QAction *action: this->myWTNZActions){action->setEnabled(false);}
            }
        }      

        ///
        /// Functionnalities helpers calls
        ///


        void accessWTNZ() {
            this->pHelper.openUrlInBrowser(this->wtnzUrl.c_str());
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