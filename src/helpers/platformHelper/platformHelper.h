#pragma once

#include <QString>
#include <QSettings>
#include <QStandardPaths>

class PlatformHelper {
 public:
    static void openFileInOS(const QString &cpURL);
    static void openUrlInBrowser(const QString &cpURL);
    static QString getMusicAppPrefFileProbableLocation();
    static QString getEnvironmentVariable(const char* variable);
    static QString extractMusicAppLibLocation(const QString &pathToParamFile);
    static bool isLaunchingAtStartup();
    static void switchStartupLaunch();
    static QString getDataStorageDirectory();

 private:
    static inline QSettings* _settings = nullptr;
    static QSettings* getStartupSettingsHandler();

    static QString getPathToApp();
    static QString getPathToAppFromStartupSettings(QSettings *settings);
    static QString prepareStandardPath(QStandardPaths::StandardLocation pathType);
};
