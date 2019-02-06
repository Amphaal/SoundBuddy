#pragma once
#include <string>
#include <fstream>
#include <map>
#include <any>

#include "../const.cpp"
#include "QtCore/QSettings"
#include "QCoreApplication"

class PlatformHelper
{
    public:
        void openFileInOS(std::string cpURL);
        void openUrlInBrowser(std::string cpURL);
        std::string getITunesPrefFileProbableLocation();
        std::string getEnvironmentVariable(const char* variable);
        std::string extractItunesLibLocationFromMap(std::map<std::string, std::any> *pListAsMap);
        bool isLaunchingAtStartup();
        void switchStartupLaunch();

    private:
        QSettings* getStartupSettingsHandler();
        std::string getPathToApp();
        std::string getPathToAppFromStartupSettings(QSettings *settings);
};