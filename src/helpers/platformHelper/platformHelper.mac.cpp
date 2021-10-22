#ifdef Q_OS_OSX

    #include <string>
    #include <unistd.h>
    #include <stdlib.h>
    #include <map>
    #include <pwd.h>
    #include <QtCore/QSettings>
    #include <QtCore/QCoreApplication>

    #include "platformHelper.h"
    #include "src/helpers/stringHelper/stringHelper.hpp"


    void PlatformHelper::openFileInOS(const QString &cpURL) {
        QString command = "open \"" + cpURL + "\"";
        system(command.toStdString().c_str());
    };

    void PlatformHelper::openUrlInBrowser(const QString &cpURL) {
        PlatformHelper::openFileInOS(cpURL);
    };

    QString PlatformHelper::getEnvironmentVariable(const char* variable) {
    
        const char *homeDir = getenv(variable);

        if (!homeDir) {
            struct passwd* pwd = getpwuid(getuid());
            if (pwd) homeDir = pwd->pw_dir;
        }

        return homeDir;
    };

    QString PlatformHelper::getITunesPrefFileProbableLocation() {
        return PlatformHelper::getEnvironmentVariable("HOME") + QString("/Library/Preferences/com.apple.iTunes.plist");
    };

    QString PlatformHelper::extractItunesLibLocation(const QString &pathToParamFile) {
        //get path to iTunes as string
        QSettings plist(pathToParamFile, QSettings::NativeFormat);
        auto pathToLib = plist.value("NSNavLastRootDirectory").toString();

        //get parent directory
        QFileInfo mapP(pathToLib);
        auto pre = mapP.dir().absolutePath();

        //replace tilda with full path
        StringHelper::replaceFirstOccurrence(pre, "~", PlatformHelper::getEnvironmentVariable("HOME"));

        return pre + "/iTunes Music Library.xml";
    };

    QSettings* PlatformHelper::getStartupSettingsHandler() {
        if(!_settings) {
            auto cPath = PlatformHelper::getEnvironmentVariable("HOME") + MAC_REG_STARTUP_LAUNCH_PATH; //computed path
            _settings = new QSettings(cPath, QSettings::NativeFormat);
        }

        return _settings;
    }

    QString PlatformHelper::getPathToApp() {
        return QCoreApplication::applicationFilePath();
    }

    QString PlatformHelper::getPathToAppFromStartupSettings(QSettings *settings) {
        QStringList c = settings->value("ProgramArguments").toStringList();
        if(!c.size()) return ""; //if file not exists
        return c.takeFirst();
    }

    void PlatformHelper::switchStartupLaunch() {

        auto settings = PlatformHelper::getStartupSettingsHandler();

        if (!PlatformHelper::isLaunchingAtStartup()) {
            settings->setValue("Label", APP_NAME);
            settings->setValue("ProcessType", "Interactive");
            settings->setValue("ExitTimeOut", 0);
            settings->setValue("RunAtLoad", true);
            settings->setValue("LimitLoadToSessionType", "Aqua");
            QStringList args(PlatformHelper::getPathToApp());
            settings->setValue("ProgramArguments", args);
        } else {
            auto cPath = PlatformHelper::getEnvironmentVariable("HOME") + MAC_REG_STARTUP_LAUNCH_PATH; //computed path
            remove(cPath.toStdString().c_str());
        }
    }

#endif