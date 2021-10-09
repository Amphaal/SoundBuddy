#include "platformHelper.h"

#include <QFileInfo>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>

bool PlatformHelper::isLaunchingAtStartup() {
    auto settings = PlatformHelper::getStartupSettingsHandler();
    return PlatformHelper::getPathToApp() == PlatformHelper::getPathToAppFromStartupSettings(settings);  // compare paths
}

// ensure a file exists
bool PlatformHelper::fileExists(const QString &outputFileName) {
    QFileInfo confP(outputFileName);
    return confP.exists();
}

/* STD PATHS */

QString PlatformHelper::getAppDirectory() {
    return QCoreApplication::applicationDirPath();
}
QString PlatformHelper::getDataStorageDirectory() {
    return PlatformHelper::prepareStandardPath(QStandardPaths::AppLocalDataLocation);
}

QString PlatformHelper::prepareStandardPath(QStandardPaths::StandardLocation pathType) {
    auto path = QStandardPaths::writableLocation(pathType);
    QDir destDir(path.toStdString().c_str());
    if (!destDir.exists()) destDir.mkpath(".");
    return path;
}