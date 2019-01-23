#pragma once

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
#include "QtWidgets/QMessageBox"
#include "QtGui/QCloseEvent"
#include "QtGui/QHideEvent"
#include "QtGui/QIcon"

#include "../helpers/const.cpp"
#include "../helpers/configHelper.cpp"
#include "../helpers/platformHelper/platformHelper.h"
#include "tabs/ShoutTab.cpp"
#include "tabs/FeederTab.cpp"
#include "../localization/i18n.cpp"

class MainWindow : public QMainWindow {
   
    public:
        MainWindow(QString *title);
        void informWarningPresence();

    private:
        bool forceQuitOnMacOS = false;
        QString *title;
        QSystemTrayIcon *trayIcon;
        QFileSystemWatcher *configWatcher;
        vector<QAction*> myWTNZActions;
        vector<QAction*> warningsfileActions;
        PlatformHelper pHelper;
        OutputHelper owHelper;
        ConfigHelper helper;
        nlohmann::json config;
        string wtnzUrl;
        ShoutTab *shoutTab;
    
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
};