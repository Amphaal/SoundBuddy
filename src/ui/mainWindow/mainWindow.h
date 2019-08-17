#pragma once

#include <QCoreApplication>

#include <QString>
#include <QSize>
#include <QObject>
#include <QDebug>
#include <QFileSystemWatcher>
#include <QTabWidget>
#include <QMainWindow>
#include <QWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QCloseEvent>
#include <QHideEvent>
#include <QIcon>
#include <QStatusBar>

#include <rapidjson/document.h>

#include "src/version.h"
#include "src/localization/i18n.cpp"

#include "src/_libs/autoupdatercore/updater.h"

#include "src/helpers/_const.cpp"
#include "src/helpers/configHelper/authHelper.cpp"
#include "src/helpers/platformHelper/platformHelper.h"

#include "src/ui/tabs/ShoutTab.cpp"
#include "src/ui/tabs/FeederTab.cpp"
#include "src/workers/connectivity/ConnectivityThread.h"

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
        std::vector<QAction*> myWTNZActions;
        std::vector<QAction*> warningsfileActions;
        AuthHelper aHelper;
        ConfigHelper cHelper;
        OutputHelper owHelper;
        std::string wtnzUrl;
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
        ShoutThread *sw = 0;
        FeederThread *fw = 0;
        ConnectivityThread *cw = 0;
        void startupConnectivityThread();
        void startupShoutThread();
        void startupFeederThread();
};