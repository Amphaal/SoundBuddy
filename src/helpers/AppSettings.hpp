#pragma once

#include <QSettings>

class AppSettings {
 public:
    static inline const char * MUST_AUTORUN_SHOUT = "AutoRunShout";
    static inline const char * WTNZ_USERNAME = "WtnzUsername";
    static inline const char * WTNZ_PASSWORD = "WtnzPassword";
    static inline const char * WTNZ_HOST_URL = "WtnzHostUrl";
    static inline const char * MUST_RUN_AT_STARTUP = "MustRunAtStartup";
};
