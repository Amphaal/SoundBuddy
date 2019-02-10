#include "Ii18n.h"

using namespace std;

class FeedTNZTranslator_EN : public IFeedTNZTranslator {
    string Menu_File() { return "File"; }
    string Menu_Options() { return "Options"; }
    string Menu_OpenMonitor() {return "Open monitor..."; }
    string Menu_MyWTNZ() { return "My WTNZ"; };
    string Menu_UpdateConfig() { return "Update configuration file"; };
    string Menu_Quit() { return "Quit"; };
    string Menu_AddToStartup() { return "Lauch at system boot"; };
    string Menu_OpenWarnings() { return "Access recent upload warnings"; };
    string Menu_CheckForUpgrades() { return "Check for upgrades"; };
    string Alert_RunningWorker_Title() { return "Shout worker running !"; };
    string Alert_RunningWorker_Text() { return "Shout worker is actually running : Are you sure you want to exit ?"; };
    string Alert_UpdateAvailable_Title() { return "Update Available"; };
    string Alert_UpdateAvailable_Text() { return "An update is available for " + APP_NAME + ". Would you like to install it now ?"; };
    string Feeder_Button() { return "Generate Digest and Upload"; };
    string Shout_Button() { return "Connect to iTunes"; };
    string Shout_Autolaunch() { return "Autostart at launch"; };
    string FTNZNoOutputFileException(string outputPath) { return "\"" + outputPath  + "\" does not exist. Please generate JSON before."; };
    string FTNZOutputFileUnreadableException(string outputPath) {return "\"" + outputPath + "\" cannot be read. Please regenerate it."; };
    string FTNZErrorUploadingException(string errorMessage) { return "Error communicating with the remote server : \"" + errorMessage + (string)"\"."; };
    string FTNZXMLLibFileUnreadableException() { return "Cannot read the XML file bound to your library. Are you sure you activated the XML file sharing in iTunes ?"; };
    string FTNZMissingItunesConfigException() { return "An issue happened while fetching iTunes's XML file location. Have you installed iTunes ?"; };
    string FTNZNoMusicFoundException() { return "No music found in your library. Please feed it some."; };
    string Feeder_Warning() { return "WARNING ! Make sure you activated the XML file sharing in iTunes>Preferences>Advanced."; };
    string Feeder_NotifyWarningsExistence(size_t warningsCount, string outputFileName) {
        return "WARNING ! " + std::to_string(warningsCount) + 
                " files in your library are missing important metadata and consequently were removed from the output file ! " + 
                "Please check the \"" + outputFileName + "\" file for more informations.";
    };
    string Feeder_Unmolding(string fileName) {
        return "Unmolding \"" + fileName + "\"...";
    };
    string Feeder_OutputReady() { return "OK, output file is ready for breakfast !"; };
    string Feeder_StartSend() { return "Let's try to send now !"; };
    string HTTP_ServerResponded(string response) {
        return "Server responded: " + response;
    };
    string HTTP_NoResponse() { return "No feedback from the server ? Strange... Please check the targeted host."; };
    string Feeder_TrimingFat() { return "Triming fat..."; };
    string Feeder_CookingJSON() { return "Cooking the JSON file..."; };
    string Feeder_PredigestXML() { return "Pre-digesting XML file..."; };
    string Feeder_LogTrackEmit(size_t recCount, size_t expectedCount) {
        return std::to_string(recCount) + " over " + std::to_string(expectedCount) + "...";
    };
    string Feeder_GetXMLFileLoc() { return "Getting XML file location..."; };
    string Shout_Nothing(string date) {
        return date + ": Shouting -> Nothing";
    };
    string Shout(string date, string name, string album, string artist, bool state) {
        string pState = state ? "playing" : "paused";
        return date + ": Shouting -> " + name + " - " +
        album + " - " + artist + " (" + pState + ")";
    };
    string Shout_StartListening() { return "Listening to iTunes !"; };
    string Shout_StopListening() { return "Stopped listening to iTunes."; };
    string Shout_WaitITunes() { return "Waiting for iTunes to launch..."; };
    string Shout_ITunesShutdown() { return "iTunes shutting down !"; };
    string Shout_WaitITunesAgain() { return "Waiting for iTunes to launch again..."; };
    string FTNZMissingConfigValuesException() { return "Expected configuration values are missing. Please check the configuration file !"; };
    string FTNZErrorProcessingUploadException(long code, string response) { 
        return "The server encountered an error while processing the data => HTTP Code " + std::to_string(code) + " : " + response;
    };
};