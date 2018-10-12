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
        this->openFileInOS(cpURL);
    };

    std::string PlatformHelper::getEnvironmentVariable(const char* variable) {
        return std::getenv(variable);
    }

    std::string PlatformHelper::getITunesPrefFileProbableLocation() {
        return this->getEnvironmentVariable("HOME") + std::string("/Library/Preferences/com.apple.iTunes.plist");
    };

    std::string PlatformHelper::extractItunesLibLocationFromMap(std::map<std::string, boost::any> *pListAsMap) {
        auto rAsAny = pListAsMap->at("NSNavLastRootDirectory");
        //auto rAsVector = boost::any_cast<std::vector<char>>(rAsAny);
        //auto rAsString = string(rAsVector.start(), rAsVector.end());
        auto rAsString = boost::any_cast<std::string>(rAsAny);
        boost::filesystem::path mapP(rAsString);
        mapP = mapP.parent_path();
        return boost::filesystem::absolute(mapP).string() + "iTunes Music Library.xml";
    }

#endif