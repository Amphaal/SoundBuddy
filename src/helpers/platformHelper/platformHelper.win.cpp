#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#include <stdlib.h>  
#include <rapidjson/document.h>

#include "platformHelper.h"
#include "src/version.h"
#include "src/helpers/_const.hpp"
#include "src/helpers/iTunesLibParser/iTunesLibParser.h"

#include <string>
#include <map>
#include <vector>

#include <QCoreApplication>

void PlatformHelper::openFileInOS(const QString &cpURL) {
    ShellExecuteA(NULL, "open", "notepad", cpURL.toStdString().c_str(), NULL, SW_SHOWNORMAL);
};

void PlatformHelper::openUrlInBrowser(const QString &cpURL) {
    ShellExecuteA(NULL, "open", cpURL.toStdString().c_str(), NULL, NULL, SW_SHOWNORMAL);
};

QString PlatformHelper::getEnvironmentVariable(const char* variable) {
    char* buf = nullptr;
    size_t sz = 0;
    if (_dupenv_s(&buf, &sz, variable) == 0 && buf != nullptr) {
        QString ret(buf);
        free(buf);
        return ret;
    } else {
        return NULL;
    }
};

QString PlatformHelper::getITunesPrefFileProbableLocation() {
    return PlatformHelper::getEnvironmentVariable("APPDATA") + QString("\\Apple Computer\\Preferences\\com.apple.iTunes.plist");
};

QString PlatformHelper::extractItunesLibLocation(const QString &pathToParamFile) {
    //get a copy of converted binary plist 
    auto pathTo_plutil = PlatformHelper::getEnvironmentVariable("PROGRAMFILES") + QString("\\Common Files\\Apple\\Apple Application Support\\plutil.exe");
    auto destPath = PlatformHelper::getDataStorageDirectory() + "/temp.plist";
    QString command = "-convert xml1 -o ";
            command += "\"" + destPath +"\" ";
            command +="\"" + pathToParamFile  +"\"";
    ShellExecuteA(NULL, "open", pathTo_plutil.toStdString().c_str(), command.toStdString().c_str(), NULL, SW_HIDE);

    //read it into JSON obj
    iTunesLibParser iTunesParams(destPath);
    auto xmlAsJSONString = iTunesParams.ToJSON();
    rapidjson::Document d;
    d.Parse(xmlAsJSONString.toStdString().c_str());

    //decode path
    auto encodedPath = QString::fromStdString(d["LXML:1:iTunes Library XML Location"].GetString());
    auto decodedData = encodedPath.toUtf8().toBase64();
    
    return decodedData;
};

QSettings* PlatformHelper::getStartupSettingsHandler() {
    if(!_settings) {
        _settings = new QSettings(WINDOWS_REG_STARTUP_LAUNCH_PATH.toStdString().c_str(), QSettings::NativeFormat);
    }

    return _settings;
}

QString PlatformHelper::getPathToApp() {
    return QCoreApplication::applicationFilePath().replace('/', '\\');
}

QString PlatformHelper::getPathToAppFromStartupSettings(QSettings *settings) {
    return settings->value(APP_NAME, "").toString();
}

void PlatformHelper::switchStartupLaunch() {

    auto settings = PlatformHelper::getStartupSettingsHandler();

    if (!PlatformHelper::isLaunchingAtStartup()) {
        settings->setValue(APP_NAME, PlatformHelper::getPathToApp());
    } else {
        settings->remove(APP_NAME);
    }
}

#endif