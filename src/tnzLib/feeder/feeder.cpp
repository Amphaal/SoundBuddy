#include <string>
#include <fstream>
#include "../IMessageHelper.cpp"
#include "../../helpers/platformHelper/platformHelper.h"
#include "../../../libs/plistcpp/Plist.hpp"

class Feeder : public IMessageHelper {
        
        const std::string outputFileName = "output/output.json";
        const std::string warningsFileName = "output/warnings.json";

        private:
            PlatformHelper *pHelper;

            Feeder() : pHelper(new PlatformHelper()) {
                if(this->callMessageHelper) this->callMessageHelper("WARNING ! Make sure you activated the XML file sharing in iTunes>Preferences>Advanced.");
            }

            //seek in iTunes preference file the library location
            std::string getITunesLibLocation() {
                if(this->callMessageHelper) this->callMessageHelper("Getting XML file location...");

                std::string pathToPrefs = this->pHelper->getITunesPrefFileProbableLocation();



            }


        public:
            void generateLibJSONFile() {}
            void uploadLibToServer() {}

}