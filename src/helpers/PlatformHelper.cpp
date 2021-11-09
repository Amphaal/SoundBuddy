#include "PlatformHelper.h"

#include <QFileInfo>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QProcessEnvironment>

bool PlatformHelper::isLaunchingAtStartup() {
    auto settings = _getStartupSettingsHandler();
    return _getPathToApp() == _getPathToAppFromStartupSettings(settings);  // compare paths
}

void PlatformHelper::openUrlInBrowser(const QString &url) {
    QDesktopServices::openUrl({url});
}

void PlatformHelper::openFileInOS(const QString &filePath) {
    QUrl url(filePath);
    url.setScheme("file");
    QDesktopServices::openUrl(
        QDir::fromNativeSeparators(filePath) // takes URI, Windows backslashes are not welcomed
    );
}

const QString PlatformHelper::_getEnvironmentVariable(const char* variable) {
    return QProcessEnvironment::systemEnvironment().value(variable);
}

const QString PlatformHelper::getDataStorageDirectory() {
    //
    auto path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

    // create dir if not existing
    QDir destDir(path);
    if (!destDir.exists()) destDir.mkpath(".");
    
    //
    return QDir::toNativeSeparators(path);
}

const QString PlatformHelper::_getPathToApp() {
    return QCoreApplication::applicationFilePath();
}
