#ifdef __APPLE__

    #include <string>
    #include <stdlib.h>
    #include "platformHelper.h"
    #include <boost/filesystem.hpp>
    #include <map>
    #include <boost/any.hpp>

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

    std::string PlatformHelper::extractItunesLibLocationFromMap(std::map<std::string, boost::any> *pListAsMap) {
        auto rAsAny = convertedPlist->at("NSNavLastRootDirectory");
        auto rAsVector = any_cast<vector<char>>(rAsAny);
        auto rAsString = string(rAsVector.start(), rAsVector.end());
        boost::filesystem::path mapP(rAsString);
        mapP = mapP.parent_path();
        return boost::filesystem::absolute(mapP).string() + "iTunes Music Library.xml";
    }

#endif