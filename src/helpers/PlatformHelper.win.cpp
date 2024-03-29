#ifdef _WIN32

#include <QString>
#include <QDir>
#include <QFile>

#include <windows.h>
#include <shellapi.h>
#include <stdlib.h>

#include "PlatformHelper.h"
#include "version.h"
#include "src/i18n/trad.hpp"

const QString PlatformHelper::_getMusicAppPrefFileProbableLocation() {
    const auto ePath = QString (R"(\Apple Computer\Preferences\com.apple.iTunes.plist)");
    
    // check the WindowsApp install path first
    const auto packagesPath = _getEnvironmentVariable("LOCALAPPDATA") + R"(\Packages)";

    const auto foundItunesPackage = QDir(packagesPath).entryInfoList({"AppleInc.iTunes*"}, QDir::Dirs | QDir::NoDotAndDotDot | QDir::CaseSensitive);
    
    // if found...
    if(foundItunesPackage.length()) {
        const auto foundPath = QDir::toNativeSeparators(foundItunesPackage[0].filePath());
        return foundPath + R"(\LocalCache\Roaming)" + ePath;
    }

    // else, return standard expected path
    const auto standardPath = _getEnvironmentVariable("APPDATA") + ePath;
    return standardPath;
}

const QString PlatformHelper::getMusicAppLibLocation() {
    // try to open preferences file
    QFile file(_getMusicAppPrefFileProbableLocation());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::logic_error(
            QObject::tr("Could not open %1 preferences file.")
                .arg(musicAppName())
                .toStdString()
        );
    }

    // read its content
    const auto pListContent = file.readAll();
    
    // prepare for url search
    qsizetype pathStartI = 0;
    const char* hint = "file://localhost/";

    // search for db path
    while((pathStartI = pListContent.indexOf(hint, pathStartI)) != -1) {   
        // hint found, update pathStart index
        pathStartI = pathStartI + strlen(hint);

        // find tab that marks the end of path
        auto pathEndI = pListContent.indexOf('\t', pathStartI);

        // should not happen, means missformated url
        if(pathEndI == -1) {
            break;
        }

        // get path
        const auto path = pListContent.mid(pathStartI, pathEndI - pathStartI);

        // search for db
        auto isDbPath = path.indexOf("iTunes%20Library.itl");
        
        //
        if(isDbPath > -1) {
            // remove db Path and append music library file name
            return path.mid(0, isDbPath) + "iTunes Music Library.xml";
        }
    }

    // no hint found at all, just sadly return nothing
    throw std::logic_error(
        QObject::tr("%1 XML library location could not be deduced.")
            .arg(musicAppName())
            .toStdString()
    );
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

QString PlatformHelper::_switchStartupLaunch() {
    //
    auto settings = _getStartupSettingsHandler();
    const auto pathToApp = _getPathToApp();

    //
    if (!isLaunchingAtStartup()) {
        settings->setValue(APP_NAME, QString('"') + pathToApp + '"'); // we add quotation marks to sticks to Windows stadards (OneDrive does it...)
    } else {
        settings->remove(APP_NAME);
    }

    //
    return pathToApp;
}

#endif
