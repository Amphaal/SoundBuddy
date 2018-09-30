#include <functional>
#include <iostream>
#include "./platformHelper.cpp"

using namespace std;

class ConfigHelper {

    const string configFile = "config.json";
    const vector<string> requiredConfigFields{"targetUrl", "user", "password"};
    
    typedef function<void(string)> MessageHelper;
    bool silentMode;

    public:
    ConfigHelper(function<void(string)> messageHelperFunc = NULL, bool silentMode = false) : callMessageHelper(move(messageHelperFunc)), silentMode(silentMode) {

    }

    private:
    MessageHelper callMessageHelper;
    
    //seek in iTunes preference file the library location
    string getITunesLibLocation() {
        if(this->callMessageHelper) this->callMessageHelper("Getting XML file location...");

        string pathToPrefs = PlatformHelper::getITunesPrefFileProbableLocation();
    }
}