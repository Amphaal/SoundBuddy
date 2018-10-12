#ifdef __APPLE__

    #include <string>
    #include <stdlib.h>
    #include "platformHelper.h"
    #include "../stringHelper.cpp"
    #include <boost/filesystem.hpp>
    #include <map>
    #include <pwd.h>
    #include <boost/any.hpp>

    void PlatformHelper::openFileInOS(std::string cpURL) {
        std::string command = "open " + cpURL;
        system(command.c_str());
    };

    void PlatformHelper::openUrlInBrowser(std::string cpURL) {
        this->openFileInOS(cpURL);
    };

    std::string PlatformHelper::getEnvironmentVariable(const char* variable) {
    
        const char *homeDir = getenv(variable);

        if (!homeDir) {
            struct passwd* pwd = getpwuid(getuid());
            if (pwd) homeDir = pwd->pw_dir;
        }

        return homeDir;
    };

    std::string PlatformHelper::getITunesPrefFileProbableLocation() {
        return this->getEnvironmentVariable("HOME") + std::string("/Library/Preferences/com.apple.iTunes.plist");
    };

    std::string PlatformHelper::extractItunesLibLocationFromMap(std::map<std::string, boost::any> *pListAsMap) {
        //get path to iTunes as string
        auto rAsAny = pListAsMap->at("NSNavLastRootDirectory");
        auto rAsString = boost::any_cast<std::string>(rAsAny);

        //get parent directory
        boost::filesystem::path mapP(rAsString);
        mapP = mapP.parent_path();
        auto pre = mapP.string();

        //replace tilda with full path
        StringHelper::replaceFirstOccurrence(pre, "~", this->getEnvironmentVariable("HOME"));

        return pre + "/iTunes Music Library.xml";
    };

#endif