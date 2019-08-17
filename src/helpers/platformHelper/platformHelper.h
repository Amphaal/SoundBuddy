#pragma once
#include <string>
#include <fstream>
#include <map>

#include <rapidjson/document.h>

#include "src/helpers/_const.cpp"
#include "src/version.h"

#include <QSettings>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>

class PlatformHelper
{
    public:
        static void openFileInOS(const std::string &cpURL);
        static void openUrlInBrowser(const std::string &cpURL);
        static std::string getITunesPrefFileProbableLocation();
        static std::string getEnvironmentVariable(const char* variable);
        static std::string extractItunesLibLocation(const std::string &pathToParamFile);
        static bool isLaunchingAtStartup();
        static void switchStartupLaunch();
        static bool fileExists(const std::string &filename);
        static std::string getAppDirectory();
        static std::string getDataStorageDirectory();

    private:
        static QSettings* getStartupSettingsHandler();
        static std::string getPathToApp();
        static std::string getPathToAppFromStartupSettings(QSettings *settings);
        static std::string prepareStandardPath(QStandardPaths::StandardLocation pathType);
};