#include <string>
#include "../IMessageHelper.cpp"
#include "../../helpers/platformHelper/platformHelper.h"

class Feeder : public IMessageHelper {
        
        private:
        PlatformHelper pHelper;

        Feeder() : pHelper(PlatformHelper()) {}

        //seek in iTunes preference file the library location
        std::string getITunesLibLocation() {
            if(this->callMessageHelper) this->callMessageHelper("Getting XML file location...");

            std::string pathToPrefs = this->pHelper.getITunesPrefFileProbableLocation();
        }
}