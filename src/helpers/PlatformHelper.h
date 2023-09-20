#pragma once

#include <QString>
#include <QSettings>

class PlatformHelper {
 public:
    static void openFileInOS(const QString &filePath);
    static void openUrlInBrowser(const QString &url);
    static const QString getDataStorageDirectory();

    /* Platform specific, unhandled by Qt */
      static void switchStartupLaunch();
      static QString _switchStartupLaunch();
      static bool isLaunchingAtStartup();

      static const QString getMusicAppLibLocation();

 private:
    static inline QSettings* _settings = nullptr;
    static QSettings* _getStartupSettingsHandler();

    static const QString _getPathToApp();
    static const QString _getPathToAppFromStartupSettings(QSettings *settings);

    static const QString _getEnvironmentVariable(const char* variable);
    
    /* Platform specific, unhandled by Qt */
    static const QString _getMusicAppPrefFileProbableLocation();
};
