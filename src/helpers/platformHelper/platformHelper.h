#pragma once
#include <string>
#include <fstream>
#include <map>

#include <rapidjson/document.h>
#include <filesystem>

#include "src/helpers/_const.cpp"
#include "src/version.h"

#include <QSettings>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>

class PlatformHelper
{
    public:
        static void openFileInOS(std::string cpURL);
        static void openUrlInBrowser(std::string cpURL);
        static std::string getITunesPrefFileProbableLocation();
        static std::string getEnvironmentVariable(const char* variable);
        static std::string extractItunesLibLocation(std::string pathToParamFile);
        static bool isLaunchingAtStartup();
        static void switchStartupLaunch();
        static bool fileExists(std::string filename);
        static std::string getAppDirectory();
        static std::string getDataStorageDirectory();

    private:
        static QSettings* getStartupSettingsHandler();
        static std::string getPathToApp();
        static std::string getPathToAppFromStartupSettings(QSettings *settings);
        static std::string prepareStandardPath(QStandardPaths::StandardLocation pathType);
};