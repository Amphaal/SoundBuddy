#pragma once

#include <QString>
#include <QSettings>

class PlatformHelper {
 public:
    static void openFileInOS(const QString &cpURL);
    static void openUrlInBrowser(const QString &cpURL);
    static QString getITunesPrefFileProbableLocation();
    static QString getEnvironmentVariable(const char* variable);
    static QString extractItunesLibLocation(const QString &pathToParamFile);
    static bool isLaunchingAtStartup();
    static void switchStartupLaunch();
    static bool fileExists(const QString &filename);
    static QString getAppDirectory();
    static QString getDataStorageDirectory();

 private:
    static QSettings* getStartupSettingsHandler();
    static QString getPathToApp();
    static QString getPathToAppFromStartupSettings(QSettings *settings);
    static QString prepareStandardPath(QStandardPaths::StandardLocation pathType);
};
