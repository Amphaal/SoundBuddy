#pragma once

#include <QSettings>

class AppSettings {
 public:
    static inline const char * MUST_AUTORUN_SHOUT = "AutoRunShout";
    static inline const char * PLATFORM_USERNAME = "PlatformUsername";
    static inline const char * PLATFORM_PASSWORD = "PlatformPassword";
    static inline const char * PLATFORM_HOST_URL = "PlatformHostUrl";
    static inline const char * MUST_RUN_AT_STARTUP = "MustRunAtStartup";
};
