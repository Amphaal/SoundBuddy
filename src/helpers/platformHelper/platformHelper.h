#pragma once
#include <string>
#include <fstream>

class PlatformHelper
{
    public:
        void openFileInOS(std::string cpURL);
        void openUrlInBrowser(std::string cpURL);
        std::string getITunesPrefFileProbableLocation();
        std::string getenv(char* variable);
};