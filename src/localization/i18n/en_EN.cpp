#include "Ii18n.h"

using namespace std;

class AppTranslator_EN : public IAppTranslator {
    std::string Menu_File() { return "File"; }
    std::string Menu_Options() { return "Options"; }
    std::string Menu_OpenMonitor() {return "Open monitor..."; }
    std::string Menu_MyWTNZ() { return "My WTNZ"; };
    std::string Menu_UpdateConfig() { return "Update configuration file"; };
    std::string Menu_Quit() { return "Quit"; };
    std::string Menu_AddToStartup() { return "Lauch at system boot"; };
    std::string Menu_OpenWarnings() { return "Access recent upload warnings"; };
    std::string Menu_CheckForUpgrades() { return "Check for upgrades"; };
    std::string Alert_RunningWorker_Title() { return "Shout worker running !"; };
    std::string Alert_RunningWorker_Text() { return "Shout worker is actually running : Are you sure you want to exit ?"; };
    std::string Alert_UpdateAvailable_Title() { return "Update Available"; };
    std::string Alert_UpdateAvailable_Text() { return "An update is available for " + (std::string)APP_NAME + ". Would you like to install it now ?"; };
    std::string Feeder_Button() { return "Generate Digest and Upload"; };
    std::string Shout_Button() { return "Connect to iTunes"; };
    std::string Shout_Autolaunch() { return "Autostart at launch"; };
    std::string FTNZNoOutputFileException(std::string outputPath) { return "\"" + outputPath  + "\" does not exist. Please generate JSON before."; };
    std::string FTNZOutputFileUnreadableException(std::string outputPath) {return "\"" + outputPath + "\" cannot be read. Please regenerate it."; };
    std::string FTNZErrorUploadingException(std::string errorMessage) { return "Error communicating with the remote server : \"" + errorMessage + (std::string)"\"."; };
    std::string FTNZXMLLibFileUnreadableException() { return "Cannot read the XML file bound to your library. Are you sure you activated the XML file sharing in iTunes ?"; };
    std::string FTNZMissingItunesConfigException() { return "An issue happened while fetching iTunes's XML file location. Have you installed iTunes ?"; };
    std::string FTNZNoMusicFoundException() { return "No music found in your library. Please feed it some."; };
    std::string Feeder_Warning() { return "WARNING ! Make sure you activated the XML file sharing in iTunes>Preferences>Advanced."; };
    std::string Feeder_NotifyWarningsExistence(size_t warningsCount, std::string outputFileName) {
        return "WARNING ! " + std::to_string(warningsCount) + 
                " files in your library are missing important metadata and consequently were removed from the output file ! " + 
                "Please check the \"" + outputFileName + "\" file for more informations.";
    };
    std::string Feeder_Unmolding(std::string fileName) {
        return "Unmolding \"" + fileName + "\"...";
    };
    std::string Feeder_OutputReady() { return "OK, output file is ready for breakfast !"; };
    std::string Feeder_StartSend() { return "Let's try to send now !"; };
    std::string HTTP_ServerResponded(std::string response) {
        return "Server responded: " + response;
    };
    std::string HTTP_NoResponse() { return "No feedback from the server ? Strange... Please check the targeted host."; };
    std::string Feeder_TrimingFat() { return "Triming fat..."; };
    std::string Feeder_CookingJSON() { return "Cooking the JSON file..."; };
    std::string Feeder_PredigestXML() { return "Pre-digesting XML file..."; };
    std::string Feeder_LogTrackEmit(size_t recCount, size_t expectedCount) {
        return std::to_string(recCount) + " over " + std::to_string(expectedCount) + "...";
    };
    std::string Feeder_GetXMLFileLoc() { return "Getting XML file location..."; };
    std::string Shout_Nothing(std::string date) {
        return date + ": Shouting -> Nothing";
    };
    std::string Shout(std::string date, std::string name, std::string album, std::string artist, bool state) {
        std::string pState = state ? "playing" : "paused";
        return date + ": Shouting -> " + name + " - " +
        album + " - " + artist + " (" + pState + ")";
    };
    std::string Shout_StartListening() { return "Listening to iTunes !"; };
    std::string Shout_StopListening() { return "Stopped listening to iTunes."; };
    std::string Shout_WaitITunes() { return "Waiting for iTunes to launch..."; };
    std::string Shout_ITunesShutdown() { return "iTunes shutting down !"; };
    std::string Shout_WaitITunesAgain() { return "Waiting for iTunes to launch again..."; };
    std::string FTNZMissingConfigValuesException() { return "Expected configuration values are missing. Please check the configuration file !"; };
    std::string FTNZErrorProcessingUploadException(long code, std::string response) { 
        return "The server encountered an error while processing the data => HTTP Code " + std::to_string(code) + " : " + response;
    };
    std::string SIOWaitingConnection() { return "Connecting to server...";};
    std::string SIOWaitingCredentials() { return "Waiting for appropriate credentials.";};
    std::string SIOReconnecting() { return "Reconnecting to server...";};
    std::string SIOAskingCredentialValidation() { return "Asking for credentials validation...";};
    std::string SIOLoggedAs(std::string username) { return (std::string)"Logged as \"" + username + "\"";};
    std::string SIOErrorOnValidation(std::string errorCode) { 
        std::string part;
        if(errorCode == "cdm") {
           part = "Credential data missing";
        } else if(errorCode == "eud") {
            part = "Empty users database";
        } else if(errorCode == "unfid") {
            part = "Username not found in database";
        } else if(errorCode == "nopass") {
            part = "Password for the user not found in database";
        } else if(errorCode == "pmiss") {
            part = "Password missmatch";
        } else {
            return "Unknown error from the validation request";
        }
        return (std::string)"Server responded with : \"" + part + "\"";
    };
    std::string SearchingForUpdates() { return "Searching for updates...";};
};