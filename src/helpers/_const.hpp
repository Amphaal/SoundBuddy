#pragma once

#include <QString>
#include <QVector>
#include "src/version.h"

static const QString LOCAL_ICON_PNG_PATH = ":/icons/app.png";
static const QString LOCAL_REVERSE_ICON_PNG_PATH = ":/icons/app_reverse.png";
static const QString WARNINGS_FILE_PATH = "output\\warnings.json";
static const QString OUTPUT_FILE_PATH = "output\\output.json";
static const QString SHOUT_FILE_PATH = "output\\shout.json";
static const QString WINDOWS_REG_STARTUP_LAUNCH_PATH = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
static const QString MAC_REG_STARTUP_LAUNCH_PATH = QString("/Library/LaunchAgents/%1.plist").arg(APP_NAME);
static const QString AUTH_FILE_PATH = "auth.json";
static const QString APP_CONFIG_FILE_PATH = "config.json";
static const QVector<QString> REQUIRED_CONFIG_FIELDS {"username", "password"};
static const QString AUTO_RUN_SHOUT_PARAM_NAME = "autoLaunchShout";
static constexpr int SIO_PORT = 3000;
static constexpr int MAX_LOG_MESSAGES = 100;
