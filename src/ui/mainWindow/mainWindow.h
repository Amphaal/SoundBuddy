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

#include <IFWUpdateChecker.hpp>

#include "src/helpers/_const.hpp"
#include "src/helpers/configHelper/authHelper.hpp"
#include "src/helpers/platformHelper/platformHelper.h"

#include "src/ui/tabs/ShoutTab.hpp"
#include "src/ui/tabs/FeederTab.hpp"
#include "src/workers/connectivity/ConnectivityThread.h"

#include "src/ui/widgets/TraficLight.hpp"


class MainWindow : public QMainWindow {
 Q_OBJECT   
 public:
    MainWindow();
    void informWarningPresence();

 private:
    bool forceQuitOnMacOS = false;
    bool userNotificationOnUpdateCheck = false;
    QSystemTrayIcon* trayIcon;
    QFileSystemWatcher* configWatcher;
    QVector<QAction*> myWTNZActions;
    QVector<QAction*> warningsfileActions;
    AuthHelper aHelper;
    ConfigHelper cHelper;
    OutputHelper owHelper;
    QString wtnzUrl;
    UpdateChecker updateChecker;
    
    //statusbar
    void _initStatusBar();
    void updateStatusBar(const QString &message, const TLW_Colors &color);
    QLabel* statusLabel;
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
    QAction *versionAction = nullptr;
    QAction *cfugAction = nullptr;
    void setupAutoUpdate();
    void onUpdateChecked(bool hasUpdate, bool hasError);
    void requireUpdateCheckFromUser();
    void checkForAppUpdates();
    void UpdateSearch_switchUI(bool isSearching);

    //tabs
    ShoutTab* st = nullptr; 
    FeederTab* ft = nullptr;
    void _initUITabs();

    //workers
    ShoutThread *sw = nullptr;
    FeederThread *fw = nullptr;
    ConnectivityThread *cw = nullptr;
    void startupConnectivityThread();
    void startupShoutThread();
    void startupFeederThread();
};