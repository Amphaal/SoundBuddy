#pragma once

#include <QtCore/QCoreApplication>

#include <QtCore/QString>
#include <QtCore/QSize>
#include <QtCore/QObject>
#include <QtCore/QDebug>
#include <QtCore/QFileSystemWatcher>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QtWidgets/QSystemTrayIcon>
#include <QtWidgets/QMessageBox>
#include <QtGui/QCloseEvent>
#include <QtGui/QHideEvent>
#include <QtGui/QIcon>
#include <QtWidgets/QStatusBar>

#include <rapidjson/document.h>

#include "src/version.h"
#include "src/localization/i18n.cpp"

#include "libs/qtautoupdater/autoupdatercore/updater.h"

#include "src/helpers/_const.cpp"
#include "src/helpers/configHelper/authHelper.cpp"
#include "src/helpers/platformHelper/platformHelper.h"

#include "src/ui/tabs/ShoutTab.cpp"
#include "src/ui/tabs/FeederTab.cpp"
#include "src/workers/connectivity/sio.cpp"

#include "src/ui/widgets/TraficLight.cpp"


class MainWindow : public QMainWindow {
   
    public:
        MainWindow();
        void informWarningPresence();

    private:
        bool forceQuitOnMacOS = false;
        bool userNotificationOnUpdateCheck = false;
        QSystemTrayIcon *trayIcon;
        QFileSystemWatcher *configWatcher;
        vector<QAction*> myWTNZActions;
        vector<QAction*> warningsfileActions;
        AuthHelper aHelper;
        ConfigHelper cHelper;
        OutputHelper owHelper;
        string wtnzUrl;
        QtAutoUpdater::Updater *updater;
        
        //statusbar
        void _initStatusBar();
        void updateStatusBar(const std::string &message, const TLW_Colors &color);
        QLabel *statusLabel;
        TrafficLightWidget *statusLight;


        ///
        ///UI instanciation
        ///

        void _initUI();
        void _initUIMenu();
        void _initUITray();

        QMenu* _getFileMenu();
        QMenu* _getOptionsMenu();

        void setupConfigFileWatcher();
        void updateMenuItemsFromConfigValues(const QString &path = NULL);
        void updateWarningsMenuItem();

        ///
        /// Functionnalities helpers calls
        ///

        void accessWTNZ();
        void openConfigFile();
        void openWarnings();
        void addToStartupSwitch(bool checked);

        ///
        /// Events handling
        ///

        //visibility
        void hideEvent(QHideEvent *event);
        void closeEvent(QCloseEvent *event);
        void trueShow();
        void trueHide(QEvent* event);
        void forcedClose();
        void iconActivated(QSystemTrayIcon::ActivationReason reason);

        //update handling
        QAction *versionAction;
        QAction *cfugAction;
        void setupAutoUpdate();
        void onUpdateChecked(bool hasUpdate, bool hasError);
        void requireUpdateCheckFromUser();
        void checkForAppUpdates();
        void UpdateSearch_switchUI(bool isSearching);

        //tabs
        ShoutTab *st = 0; 
        FeederTab *ft = 0;
        void _initUITabs();

        //workers
        ShoutWorker *sw = 0;
        FeederWorker *fw = 0;
        ConnectivityWorker *cw = 0;
        void startupConnectivityWorker();
        void startupShoutWorker();
        void startupFeederWorker();
};