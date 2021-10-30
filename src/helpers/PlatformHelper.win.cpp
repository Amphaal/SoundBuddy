#ifdef _WIN32

#include <QCoreApplication>

#include <windows.h>
#include <shellapi.h>
#include <stdlib.h>

#include "PlatformHelper.h"
#include "src/version.h"

const QString PlatformHelper::_getMusicAppPrefFileProbableLocation() {
    return _getEnvironmentVariable("APPDATA") + "\\Apple Computer\\Preferences\\com.apple.iTunes.plist";
}

const QString PlatformHelper::getMusicAppLibLocation() {
    // get a copy of converted binary plist
    auto pathTo_plutil = _getEnvironmentVariable("PROGRAMFILES") + "\\Common Files\\Apple\\Apple Application Support\\plutil.exe";
    auto destPath = getDataStorageDirectory() + "/temp.plist";
    QString command = "-convert xml1 -o ";
            command += "\"" + destPath +"\" ";
            command +="\"" + _getMusicAppPrefFileProbableLocation()  +"\"";
    ShellExecuteA(NULL, "open", pathTo_plutil.toUtf8(), command.toUtf8(), NULL, SW_HIDE);

    // read it into JSON obj
    MusicAppLibParser musicAppParams(destPath);
    auto xmlAsJSONString = musicAppParams.ToJSON();
    QJsonDocument d;
    d.Parse(xmlAsJSONString.toUtf8());

    // decode path
    auto encodedPath = QString::fromStdString(d["LXML:1:iTunes Library XML Location"].GetString());
    auto decodedData = encodedPath.toUtf8().toBase64();

    return decodedData;
}

QSettings* PlatformHelper::_getStartupSettingsHandler() {
    if (!_settings) {
        _settings = new QSettings(R"(HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run)", QSettings::NativeFormat);
    }

    return _settings;
}

const QString PlatformHelper::_getPathToAppFromStartupSettings(QSettings *settings) {
    return settings->value(APP_NAME, "").toString();
}

void PlatformHelper::switchStartupLaunch() {
    //
    auto settings = _getStartupSettingsHandler();

    if (!isLaunchingAtStartup()) {
        settings->setValue(APP_NAME, _getPathToApp());
    } else {
        settings->remove(APP_NAME);
    }
}

#endif
