#ifdef __APPLE__

#include <QSettings>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>

#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>

#include "PlatformHelper.h"
#include "src/version.h"

static const QString MAC_REG_STARTUP_LAUNCH_PATH = "/Library/LaunchAgents/" APP_NAME ".plist";

const QString PlatformHelper::_getMusicAppPrefFileProbableLocation() {
    return _getEnvironmentVariable("HOME") + QString("/Library/Preferences/com.apple.Music.plist");
}

const QString PlatformHelper::getMusicAppLibLocation() {
    // get path to Music App as string
    QSettings plist(_getMusicAppPrefFileProbableLocation(), QSettings::NativeFormat);
    auto pathToLib = plist.value("NSNavLastRootDirectory").toString();

    // get parent directory
    QFileInfo mapP(pathToLib);
    auto pre = mapP.dir().absolutePath();

    // replace tilda with full path
    pre.replace('~', _getEnvironmentVariable("HOME"));

    //
    return pre + "/iTunes Music Library.xml";
}

QSettings* PlatformHelper::_getStartupSettingsHandler() {
    if (!_settings) {
        auto cPath = _getEnvironmentVariable("HOME") + MAC_REG_STARTUP_LAUNCH_PATH;  // computed path
        _settings = new QSettings(cPath, QSettings::NativeFormat);
    }

    return _settings;
}

const QString PlatformHelper::_getPathToAppFromStartupSettings(QSettings *settings) {
    QStringList c = settings->value("ProgramArguments").toStringList();
    if (!c.size()) return QString();  // if file not exists
    return c.takeFirst();
}

void PlatformHelper::switchStartupLaunch() {
    //
    auto settings = _getStartupSettingsHandler();

    if (!isLaunchingAtStartup()) {
        settings->setValue("Label", APP_NAME);
        settings->setValue("ProcessType", "Interactive");
        settings->setValue("ExitTimeOut", 0);
        settings->setValue("RunAtLoad", true);
        settings->setValue("LimitLoadToSessionType", "Aqua");
        settings->setValue("ProgramArguments", QStringList{ _getPathToApp() });
    } else {
        auto cPath = _getEnvironmentVariable("HOME") + MAC_REG_STARTUP_LAUNCH_PATH;  // computed path
        remove(cPath.toUtf8());
    }
}

#endif
