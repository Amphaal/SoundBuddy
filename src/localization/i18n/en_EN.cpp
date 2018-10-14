#include "Ii18n.h"

class FeedTNZTranslator_EN : public IFeedTNZTranslator {
    char* Menu_File() { return "File"; }
    char* Menu_OpenMonitor() {return "Open monitor..."; }
    char* Menu_MyWTNZ() { return "My WTNZ"; };
    char* Menu_UpdateConfig() { return "Update configuration file"; };
    char* Menu_Quit() { return "Quit"; };
    char* Alert_RunningWorker_Title() { return "Shout worker running !"; };
    char* Alert_RunningWorker_Text() { return "Shout worker is actually running : Are you sure you want to exit ?"; };
    char* Feeder_Button() { return "Generate Digest and Upload"; };
    char* Shout_Button() { return "Connect to iTunes"; };
    char* Shout_Autolaunch() { return "Autostart at launch"; };
    char* FTNZNoOutputFileException() { return "does not exist. Please generate JSON before."; };
    char* FTNZOutputFileUnreadableException() {return "cannot be read. Please regenerate it."; };
    char* FTNZErrorUploadingException() { return "Error communicating with the remote server."; };
    char* FTNZXMLLibFileUnreadableException() { return "Cannot read the XML file bound to your library. Are you sure you activated the XML file sharing in iTunes ?" };
    char* FTNZMissingItunesConfigException() { return "An issue happened while fetching iTunes's XML file location. Have you installed iTunes ?"; };
    char* FTNZNoMusicFoundException() { return "No music found in your library. Please feed it some."; };
    char* Feeder_Warning() { return "WARNING ! Make sure you activated the XML file sharing in iTunes>Preferences>Advanced."; };
    char* Feeder_NotifyWarningsExistence(size_t warningsCount, string outputFileName) {
        return "WARNING ! " + std::to_string(warningsCount).c_str()  + 
                " files in your library are missing important metadata and consequently were removed from the output file ! " + 
                "Please check the \"" + outputFileName.c_str() + "\" file for more informations.";
    };
    char* Feeder_Unmolding(string fileName) {
        return "Unmolding \"" + fileName.c_str() + "\"...";
    };
    char* Feeder_OutputReady() { return "OK, output file is ready for breakfast !"; };
    char* Feeder_StartSend() { return "Let's try to send now !"; };
    char* HTTP_ServerResponded(string response) {
        return "Server responded: " + response.c_str();
    };
    char* HTTP_NoResponse() { return "No feedback from the server ? Strange... Please check the targeted host."; };
    char* Feeder_TrimingFat() { return "Triming fat..."; };
    char* Feeder_CookingJSON() { return "Cooking the JSON file..."; };
    char* Feeder_PredigestXML() { return "Pre-digesting XML file..."; };
    char* Feeder_LogTrackEmit(size_t recCount, size_t expectedCount) {
        return std::to_string(this->recCount).c_str()  + " over " + std::to_string(this->expectedCount).c_str() + "...";
    };
    char* Feeder_GetXMLFileLoc() { return "Getting XML file location..."; };

};