#include <functional>
#include <fstream>
#include <filesystem>

#ifdef __APPLE__
    #include "./mac/platformHelper.cpp"
#endif
#ifdef _WIN32
    #include "./windows/platformHelper.cpp"
#endif

class ConfigHelper {

    const char* configFile = "config.json";
    const vector<string> requiredConfigFields{"targetUrl", "user", "password"};
    
    public:
        ConfigHelper(function<void(string)> messageHelperFunc = NULL, bool silentMode = false) : callMessageHelper(move(messageHelperFunc)), silentMode(silentMode), streamHandler(fstream()) {}

        //get configuration data from file
        void accessConfig() {
            
            //check if exists, if not create file
             filesystem::path confP(this->configFile);
            // if(filesystem::exists(confP)) {
            //     this->streamHandler.open(this->configFile, fstream::out);
            //     this->streamHandler.close();
            // }

            //PlatformHelper::openFileInOS(this->configFile);
        }

    private:
        typedef function<void(string)> MessageHelper;
        MessageHelper callMessageHelper;
        bool silentMode;
        fstream streamHandler;
        
        //seek in iTunes preference file the library location
        string getITunesLibLocation() {
            if(this->callMessageHelper) this->callMessageHelper("Getting XML file location...");

            //string pathToPrefs = PlatformHelper::getITunesPrefFileProbableLocation();
        }
};