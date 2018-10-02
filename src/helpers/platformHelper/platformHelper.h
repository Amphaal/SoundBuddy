#include <string>
#include <fstream>

class PlatformHelper
{
    public:
        PlatformHelper() {};
        void openFileInOS(std::string cpURL);
        void openUrlInBrowser(std::string cpURL);
        std::string getITunesPrefFileProbableLocation();
        std::string getProbableITunesLibraryLocation(std::fstream *fileStream);
        std::string getenv(char* variable);
};