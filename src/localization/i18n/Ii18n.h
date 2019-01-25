#pragma once
#include <string>

#include "../../helpers/const.cpp"

using namespace std;

class IFeedTNZTranslator {
    public:
        virtual string Menu_File() = 0;
        virtual string Menu_Options() = 0;
        virtual string Menu_OpenMonitor() = 0;
        virtual string Menu_MyWTNZ() = 0;
        virtual string Menu_UpdateConfig() = 0;
        virtual string Menu_OpenWarnings() = 0;
        virtual string Menu_Quit() = 0;
        virtual string Menu_CheckForUpgrades() = 0;
        virtual string Menu_AddToStartup() = 0;
        virtual string Alert_RunningWorker_Title() = 0;
        virtual string Alert_RunningWorker_Text() = 0;
        virtual string Alert_UpdateAvailable_Title() = 0;
        virtual string Alert_UpdateAvailable_Text() = 0;
        virtual string Feeder_Button() = 0;
        virtual string Shout_Button() = 0;
        virtual string Shout_Autolaunch() = 0;
        virtual string FTNZNoOutputFileException(string outputPath) = 0;
        virtual string FTNZOutputFileUnreadableException(string outputPath) = 0;
        virtual string FTNZErrorUploadingException() = 0;
        virtual string FTNZXMLLibFileUnreadableException() = 0;
        virtual string FTNZMissingItunesConfigException() = 0;
        virtual string FTNZNoMusicFoundException() = 0;
        virtual string Feeder_Warning() = 0; 
        virtual string Feeder_NotifyWarningsExistence(size_t warningsCount, string outputFileName) = 0;
        virtual string Feeder_Unmolding(string fileName) = 0;
        virtual string Feeder_OutputReady() = 0;
        virtual string Feeder_StartSend() = 0;
        virtual string HTTP_ServerResponded(string response) = 0; 
        virtual string HTTP_NoResponse() = 0;
        virtual string Feeder_TrimingFat() = 0;
        virtual string Feeder_CookingJSON() = 0;
        virtual string Feeder_PredigestXML() = 0;
        virtual string Feeder_LogTrackEmit(size_t recCount, size_t expectedCount) = 0;
        virtual string Feeder_GetXMLFileLoc() = 0;
        virtual string Shout_Nothing(string date) = 0;
        virtual string Shout(string date, string name, string album, string artist, bool state) = 0;
        virtual string Shout_StartListening() = 0;
        virtual string Shout_StopListening() = 0;
        virtual string Shout_WaitITunes() = 0;
        virtual string Shout_ITunesShutdown() = 0;
        virtual string Shout_WaitITunesAgain() = 0;
        virtual string FTNZMissingConfigValuesException() = 0;
        virtual string FTNZErrorProcessingUploadException(long code, string response) = 0;   
};
