#include "platformHelper.h"

#include <QFileInfo>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>

bool PlatformHelper::isLaunchingAtStartup() {
    auto settings = PlatformHelper::getStartupSettingsHandler();
    return PlatformHelper::getPathToApp() == PlatformHelper::getPathToAppFromStartupSettings(settings);  // compare paths
}

/* STD PATHS */
QString PlatformHelper::getDataStorageDirectory() {
    return PlatformHelper::prepareStandardPath(QStandardPaths::AppLocalDataLocation);
}

QString PlatformHelper::prepareStandardPath(QStandardPaths::StandardLocation pathType) {
    auto path = QStandardPaths::writableLocation(pathType);
    QDir destDir(path);
    if (!destDir.exists()) destDir.mkpath(".");
    return path;
}