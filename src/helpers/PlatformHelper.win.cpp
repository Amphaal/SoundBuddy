#ifdef _WIN32

#include <QString>
#include <QDir>
#include <QFile>

#include <windows.h>
#include <shellapi.h>
#include <stdlib.h>

#include "PlatformHelper.h"
#include "src/version.h"

const QString PlatformHelper::_getMusicAppPrefFileProbableLocation() {
    const auto ePath = QString ("\\Apple Computer\\Preferences\\com.apple.iTunes.plist");
    
    // check the WindowsApp install path first
    const auto packagesPath = _getEnvironmentVariable("LOCALAPPDATA") + "\\Packages";
    const auto foundItunesPackage = QDir(packagesPath).entryInfoList({"AppleInc.iTunes*"}, QDir::Dirs | QDir::NoDotAndDotDot | QDir::CaseSensitive);
    
    // if found...
    if(foundItunesPackage.length()) {
        return foundItunesPackage[0].filePath() + "\\LocalCache\\Roaming" + ePath;
    }

    // else, return standard expected path
    const auto standardPath = _getEnvironmentVariable("APPDATA") + ePath;
    return standardPath;

}

const QString PlatformHelper::getMusicAppLibLocation() {
    const auto pListContent = QFile(_getMusicAppPrefFileProbableLocation()).readAll();
    
    //
    qsizetype pathStartI = 0;
    const char* hint = "file://localhost/";

    // search for db path
    while((pathStartI = pListContent.indexOf(hint)) != -1) {   
        // hint found, update pathStart index
        pathStartI = pathStartI + strlen(hint);

        // find tab that marks the end of path
        auto pathEndI = pListContent.indexOf('\t', pathStartI);

        // should not happen, means missformated url
        if(pathEndI == -1) break;

        // get path
        const auto path = pListContent.mid(pathStartI, pathEndI - pathStartI);

        // search for db
        auto isDbPath = path.indexOf("iTunes%20Library.itl");
        
        //
        if(isDbPath > -1) {
            // remove db Path and append music library file name
            return path.mid(0, isDbPath + 1) + "iTunes Music Library.xml";
        }
    }

    // no hint found at all, just sadly return nothing
    return {};
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
