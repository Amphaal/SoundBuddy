#ifdef __APPLE__

    #include <string>
    #include <unistd.h>
    #include <stdlib.h>
    #include <map>
    #include <pwd.h>
    #include <QtCore/QSettings>
    #include <QtCore/QCoreApplication>

    #include "platformHelper.h"
    #include "src/helpers/stringHelper/stringHelper.cpp"


    void PlatformHelper::openFileInOS(std::string cpURL) {
        std::string command = "open \"" + cpURL + "\"";
        system(command.c_str());
    };

    void PlatformHelper::openUrlInBrowser(std::string cpURL) {
        PlatformHelper::openFileInOS(cpURL);
    };

    std::string PlatformHelper::getEnvironmentVariable(const char* variable) {
    
        const char *homeDir = getenv(variable);

        if (!homeDir) {
            struct passwd* pwd = getpwuid(getuid());
            if (pwd) homeDir = pwd->pw_dir;
        }

        return homeDir;
    };

    std::string PlatformHelper::getITunesPrefFileProbableLocation() {
        return PlatformHelper::getEnvironmentVariable("HOME") + std::string("/Library/Preferences/com.apple.iTunes.plist");
    };

    std::string PlatformHelper::extractItunesLibLocation(std::string pathToParamFile) {
        //get path to iTunes as string
        QSettings plist(pathToParamFile.c_str(), QSettings::NativeFormat);
        auto pathToLib = plist.value("NSNavLastRootDirectory").toString().toStdString();

        //get parent directory
        boost::filesystem::path mapP(pathToLib);
        auto pre = mapP.parent_path().string();

        //replace tilda with full path
        StringHelper::replaceFirstOccurrence(pre, "~", PlatformHelper::getEnvironmentVariable("HOME"));

        return pre + "/iTunes Music Library.xml";
    };

    QSettings* PlatformHelper::getStartupSettingsHandler() {
        auto cPath = PlatformHelper::getEnvironmentVariable("HOME") + MAC_REG_STARTUP_LAUNCH_PATH; //computed path
        auto settings = new QSettings(cPath.c_str(), QSettings::NativeFormat);
        return settings;
    }

    std::string PlatformHelper::getPathToApp() {
        return QCoreApplication::applicationFilePath().toStdString();
    }

    std::string PlatformHelper::getPathToAppFromStartupSettings(QSettings *settings) {
        QStringList c = settings->value("ProgramArguments").toStringList();
        if(!c.size()) return ""; //if file not exists
        return c.takeFirst().toStdString();
    }

    void PlatformHelper::switchStartupLaunch() {

        auto settings = PlatformHelper::getStartupSettingsHandler();

        if (!PlatformHelper::isLaunchingAtStartup()) {
            settings->setValue("Label", APP_NAME);
            settings->setValue("ProcessType", "Interactive");
            settings->setValue("ExitTimeOut", 0);
            settings->setValue("RunAtLoad", true);
            settings->setValue("LimitLoadToSessionType", "Aqua");
            QStringList args(PlatformHelper::getPathToApp().c_str());
            settings->setValue("ProgramArguments", args);
        } else {
            auto cPath = PlatformHelper::getEnvironmentVariable("HOME") + MAC_REG_STARTUP_LAUNCH_PATH; //computed path
            remove(cPath.c_str());
        }
    }

#endif