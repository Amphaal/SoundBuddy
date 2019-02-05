#ifdef _WIN32

    #include <windows.h>
    #include <stdlib.h>  

    #include "platformHelper.h"
    #include <string>
    #include <map>
    #include <boost/any.hpp>
    #include <boost/locale.hpp>
	#include <vector>

    using namespace boost;
    using namespace std;

    void PlatformHelper::openFileInOS(string cpURL) {
        ShellExecuteA(NULL, "open", "notepad", cpURL.c_str(), NULL, SW_SHOWNORMAL);
    };

    void PlatformHelper::openUrlInBrowser(string cpURL) {
        ShellExecuteA(NULL, "open", cpURL.c_str(), NULL, NULL, SW_SHOWNORMAL);
    };

    string PlatformHelper::getEnvironmentVariable(const char* variable) {
        char* buf = nullptr;
        size_t sz = 0;
        if (_dupenv_s(&buf, &sz, variable) == 0 && buf != nullptr)
        {
            string ret = buf;
            free(buf);
            return ret;
        } else {
            return NULL;
        }
    };
    
    string PlatformHelper::getITunesPrefFileProbableLocation() {
        return this->getEnvironmentVariable("APPDATA") + string("\\Apple Computer\\Preferences\\com.apple.iTunes.plist");
    };

    string PlatformHelper::extractItunesLibLocationFromMap(map<string, any> *pListAsMap) {
        auto rAsAny = pListAsMap->at("LXML:1:iTunes Library XML Location");
        auto rAsVector = any_cast<vector<char>>(rAsAny);
        
        //reformat for source UTF-16
        string rAsString;
        for (int b = 0; b < rAsVector.size() ; b++)
        {
            if(b % 2 == 0) rAsString += rAsVector[b];
        }
        return rAsString;
    };

    QSettings* PlatformHelper::getStartupSettingsHandler() {
        auto settings = new QSettings(WINDOWS_REG_STARTUP_LAUNCH_PATH.c_str(), QSettings::NativeFormat);
        return settings;
    }

    std::string PlatformHelper::getPathToApp() {
        return QCoreApplication::applicationFilePath().replace('/', '\\').toStdString();
    }

    std::string PlatformHelper::getPathToAppFromStartupSettings(QSettings *settings) {
        return settings->value(APP_NAME.c_str(), "").toString().toStdString();
    }

    void PlatformHelper::switchStartupLaunch() {

        auto settings = this->getStartupSettingsHandler();

        if (!this->isLaunchingAtStartup()) {
            settings->setValue(APP_NAME.c_str(), this->getPathToApp().c_str());
        } else {
            settings->remove(APP_NAME.c_str());
        }
    }

#endif