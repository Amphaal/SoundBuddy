#pragma once
#include <string>
#include <fstream>
#include <map>
#include <boost/any.hpp>

class PlatformHelper
{
    public:
        void openFileInOS(std::string cpURL);
        void openUrlInBrowser(std::string cpURL);
        std::string getITunesPrefFileProbableLocation();
        std::string getenv(char* variable);
        std::string extractItunesLibLocationFromMap(std::map<std::string, boost::any> *pListAsMap);
};