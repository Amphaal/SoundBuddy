#ifdef __APPLE__

    #include <string>
    #include <stdlib.h>
    #include "platformHelper.h"
    
    void PlatformHelper::openFileInOS(std::string cpURL) {
        std::string command = "open " + cpURL;
        system(command.c_str());
    };

    void PlatformHelper::openUrlInBrowser(std::string cpURL) {
        PlatformHelper::openFileInOS(cpURL);
    };

    std::string PlatformHelper::getenv(char* variable) {
        return getenv(variable);
    }

    std::string PlatformHelper::getITunesPrefFileProbableLocation() {
        return PlatformHelper::getenv("HOME") + std::string("/Library/Preferences/com.apple.iTunes.plist");
    };

#endif