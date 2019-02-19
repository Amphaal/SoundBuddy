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

#include "../../libs/socketiocpp_custom/sio_client.h"
#include <rapidjson/document.h>

#include "../helpers/_const.cpp"
#include "../helpers/configHelper.cpp"
#include "../helpers/platformHelper/platformHelper.h"
#include "tabs/ShoutTab.cpp"
#include "tabs/FeederTab.cpp"
#include "../localization/i18n.cpp"

#include "./widgets/TraficLight.cpp"

#include "../../libs/qtautoupdater/autoupdatercore/updater.h"
#include "../version.h"

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
        ConfigHelper cHelper;
        PlatformHelper pHelper;
        OutputHelper owHelper;
        rapidjson::Document config;
        string wtnzUrl;
        ShoutTab *shoutTab;
        QtAutoUpdater::Updater *updater;
        sio::client sioClient;
    
        ///
        ///UI instanciation
        ///

        void _initUI();
        void _initUITabs();
        void _initUIMenu();
        void _initUITray();

        QMenu* _getFileMenu();
        QMenu* _getOptionsMenu();

        void updateConfigValues();
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

        //hide window on minimize, only triggered on windows
        void hideEvent(QHideEvent *event);
        void closeEvent(QCloseEvent *event);
        void trueShow();
        void trueHide(QEvent* event);
        void forcedClose();
        void iconActivated(QSystemTrayIcon::ActivationReason reason);

        //update handling
        void setupAutoUpdate();
        void onUpdateChecked(bool hasUpdate, bool hasError);
        void requireUpdateCheckFromUser();

        void startupWS();
};