#include <string>
#include <fstream>
#include "../ITNZWorker.cpp"
#include "../../helpers/platformHelper/platformHelper.h"
#include "../../../libs/plistcpp/Plist.hpp"

class FeedWorker : public ITNZWorker {
        
    const std::string outputFileName = "output/output.json";
    const std::string warningsFileName = "output/warnings.json";

    public:
        FeedWorker() : pHelper(new PlatformHelper()) {}

        void run() {
            emit printLog("WARNING ! Make sure you activated the XML file sharing in iTunes>Preferences>Advanced.");
        }

    private:
        PlatformHelper *pHelper;

        //seek in iTunes preference file the library location
        std::string getITunesLibLocation() {
            emit printLog("Getting XML file location...");

            std::string pathToPrefs = this->pHelper->getITunesPrefFileProbableLocation();
        }

    void generateLibJSONFile() {}
    void uploadLibToServer() {}

}