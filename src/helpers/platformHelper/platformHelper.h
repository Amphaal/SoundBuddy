#pragma once
#include <string>
#include <fstream>
#include <map>
#include <boost/filesystem.hpp>
#include <rapidjson/document.h>

#include "../_const.cpp"
#include "../../version.h"
#include <QtCore/QSettings>
#include <QCoreApplication>

class PlatformHelper
{
    public:
        void openFileInOS(std::string cpURL);
        void openUrlInBrowser(std::string cpURL);
        std::string getITunesPrefFileProbableLocation();
        std::string getEnvironmentVariable(const char* variable);
        std::string extractItunesLibLocation(std::string pathToParamFile);
        bool isLaunchingAtStartup();
        void switchStartupLaunch();
        bool fileExists(std::string filename);

    private:
        QSettings* getStartupSettingsHandler();
        std::string getPathToApp();
        std::string getPathToAppFromStartupSettings(QSettings *settings);
};