#include <string>
#include <fstream>
#include <exception>
#include "QtWidgets/QWidget"

#include "base/ITNZWorker.h"
#include "../helpers/platformHelper/platformHelper.h"
#include "../../libs/plistcpp/Plist.hpp"

class FeederWorker : public ITNZWorker {

    const std::string outputFileName = "output/output.json";
    const std::string warningsFileName = "output/warnings.json";

    public:
		FeederWorker() : pHelper(new PlatformHelper()) {}

        void run() override {
            emit printLog("WARNING ! Make sure you activated the XML file sharing in iTunes>Preferences>Advanced.");

            try {
                this->generateLibJSONFile();
                this->uploadLibToServer();
            } catch (const std::exception& e) {
                emit printLog(e.what());
            }
        }

    private:
        PlatformHelper *pHelper;

        void generateLibJSONFile() {}
        void uploadLibToServer() {}

        ///
        /// HELPERS
        ///

        //seek in iTunes preference file the library location
        std::string getITunesLibLocation() {
            emit printLog("Getting XML file location...");

            std::string pathToPrefs = this->pHelper->getITunesPrefFileProbableLocation();

            return "";
        }

};