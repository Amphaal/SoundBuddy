#pragma once

#include <string>
#include <vector>

static const std::string LOCAL_ICON_PNG_PATH = ":/icons/feedtnz.png";
static const std::string WARNINGS_FILE_PATH = "output\\warnings.json";
static const std::string OUTPUT_FILE_PATH = "output\\output.json";
static const std::string SHOUT_FILE_PATH = "output\\shout.json";
static const std::string APP_NAME = "FeedTNZ";
static const std::string WINDOWS_REG_STARTUP_LAUNCH_PATH = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
static const std::string MAC_REG_STARTUP_LAUNCH_PATH = "/Library/LaunchAgents/FeedTNZ.plist";
static const std::string CONFIG_FILE_PATH = "config.json";
static const std::vector<std::string> REQUIRED_CONFIG_FIELDS{"targetUrl", "user", "password"};