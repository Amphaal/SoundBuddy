#pragma once

#include <string>
#include <vector>
#include "src/version.h"

static const std::string LOCAL_ICON_PNG_PATH = ":/icons/app.png";
static const std::string LOCAL_REVERSE_ICON_PNG_PATH = ":/icons/app_reverse.png";
static const std::string WARNINGS_FILE_PATH = "output\\warnings.json";
static const std::string OUTPUT_FILE_PATH = "output\\output.json";
static const std::string SHOUT_FILE_PATH = "output\\shout.json";
static const std::string WINDOWS_REG_STARTUP_LAUNCH_PATH = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
static const std::string MAC_REG_STARTUP_LAUNCH_PATH = "/Library/LaunchAgents/" + (std::string)APP_NAME + ".plist";
static const std::string AUTH_FILE_PATH = "auth.json";
static const std::string APP_CONFIG_FILE_PATH = "config.json";
static const std::vector<std::string> REQUIRED_CONFIG_FIELDS{"username", "password"};
static const std::string AUTO_RUN_SHOUT_PARAM_NAME = "autoLaunchShout";
static const int SIO_PORT = 3000;
static const int MAX_LOG_MESSAGES = 100;