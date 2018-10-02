#include <string>
#include "../messageHelperR.cpp"
#include "../../helpers/platformHelper.cpp"

class Feeder : public MessageHelperR {
        
        private:
        //seek in iTunes preference file the library location
        std::string getITunesLibLocation() {
            if(this->callMessageHelper) this->callMessageHelper("Getting XML file location...");

            std::string pathToPrefs = PlatformHelper::getITunesPrefFileProbableLocation();
        }
}