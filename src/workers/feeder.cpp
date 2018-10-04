#include <string>
#include <fstream>
#include <exception>
#include <map>
#include "QtWidgets/QWidget"

#include "base/ITNZWorker.h"
#include "../helpers/platformHelper/platformHelper.h"
#include "../../libs/plistcpp/Plist.hpp"

using namespace boost;
using namespace std;

class FeederWorker : public ITNZWorker {

    const string outputFileName = "output/output.json";
    const string warningsFileName = "output/warnings.json";

    public:
		FeederWorker() : pHelper(new PlatformHelper()) {}

        void run() override {
            emit printLog("WARNING ! Make sure you activated the XML file sharing in iTunes>Preferences>Advanced.");

            try {
                this->generateLibJSONFile();
                this->uploadLibToServer();
            } catch (const std::exception& e) {
                emit printLog(e.what());
                emit error();
            }
        }

    private:
        PlatformHelper *pHelper;

        void generateLibJSONFile() {
            auto itnzLibPath = this->getITunesLibLocation();
        }
        void uploadLibToServer() {

        }

        ///
        /// HELPERS
        ///

        //seek in iTunes preference file the library location
        string getITunesLibLocation() {
            emit printLog("Getting XML file location...");

            string pathToPrefs = this->pHelper->getITunesPrefFileProbableLocation();
            
            map<string, any> pListAsMap; 
		    Plist::readPlist(pathToPrefs.c_str(), pListAsMap);

            return this->pHelper->extractItunesLibLocationFromMap(&pListAsMap);
        }

};