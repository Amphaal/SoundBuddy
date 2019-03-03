#ifdef _WIN32
    #include <windows.h>
    #include <stdlib.h>  

    #include "platformHelper.h"
    #include <string>
    #include <map>
	#include <vector>
    #include "src/helpers/iTunesLibParser/iTunesLibParser.h"
    #include "libs/base64/base64.h"

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
        return PlatformHelper::getEnvironmentVariable("APPDATA") + string("\\Apple Computer\\Preferences\\com.apple.iTunes.plist");
    };

    string PlatformHelper::extractItunesLibLocation(std::string pathToParamFile) {

        //get a copy of converted binary plist 
        auto pathTo_plutil = PlatformHelper::getEnvironmentVariable("PROGRAMFILES") + string("\\Common Files\\Apple\\Apple Application Support\\plutil.exe");
        auto destPath = PlatformHelper::getDataStorageDirectory() + "/temp.plist";
        std::string command = "-convert xml1 -o ";
                    command += "\"" + destPath +"\" ";
                    command +="\"" + pathToParamFile  +"\"";
        ShellExecuteA(NULL, "open", pathTo_plutil.c_str(), command.c_str(), NULL, SW_HIDE);

        //read it into JSON obj
        iTunesLibParser iTunesParams(destPath.c_str());
        auto xmlAsJSONString = iTunesParams.ToJSON();
        rapidjson::Document d;
        d.Parse(xmlAsJSONString.c_str());

        //decode path
        auto encodedPath = (std::string)d["LXML:1:iTunes Library XML Location"].GetString();
        std::vector<BYTE> decodedData = base64_decode(encodedPath);
        
        //reformat from source UTF-16
        string rAsString;
        for (int b = 0; b < decodedData.size() ; b++)
        {
            if(b % 2 == 0) rAsString += decodedData[b];
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
        return settings->value(APP_NAME, "").toString().toStdString();
    }

    void PlatformHelper::switchStartupLaunch() {

        auto settings = PlatformHelper::getStartupSettingsHandler();

        if (!PlatformHelper::isLaunchingAtStartup()) {
            settings->setValue(APP_NAME, PlatformHelper::getPathToApp().c_str());
        } else {
            settings->remove(APP_NAME);
        }
    }

#endif