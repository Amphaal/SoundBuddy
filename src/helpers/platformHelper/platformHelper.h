#pragma once
#include <string>
#include <fstream>
#include <map>
#include <boost/filesystem.hpp>
#include <rapidjson/document.h>

#include "src/helpers/_const.cpp"
#include "src/version.h"
#include <QtCore/QSettings>
#include <QCoreApplication>

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

    private:
        static QSettings* getStartupSettingsHandler();
        static std::string getPathToApp();
        static std::string getPathToAppFromStartupSettings(QSettings *settings);
};