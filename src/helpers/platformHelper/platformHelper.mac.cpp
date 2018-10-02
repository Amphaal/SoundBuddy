#ifdef __APPLE__

    #include <string>
    #include "platformHelper.h"
    
    void PlatformHelper::openFileInOS(std::string cpURL) {
        std::string command = "open " + cpURL;
        system(command.c_str());
    };

    void PlatformHelper::openUrlInBrowser(std::string cpURL) {
        PlatformHelper::openFileInOS(cpURL);
    };

#endif