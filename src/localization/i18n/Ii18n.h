#pragma once

#include "src/helpers/_const.cpp"
#include "src/version.h"

class IAppTranslator {
    public:
        virtual std::string Menu_File() = 0;
        virtual std::string Menu_Options() = 0;
        virtual std::string Menu_OpenMonitor() = 0;
        virtual std::string Menu_MyWTNZ() = 0;
        virtual std::string Menu_UpdateConfig() = 0;
        virtual std::string Menu_OpenWarnings() = 0;
        virtual std::string Menu_Quit() = 0;
        virtual std::string Menu_CheckForUpgrades() = 0;
        virtual std::string Menu_AddToStartup() = 0;
        virtual std::string Alert_RunningWorker_Title() = 0;
        virtual std::string Alert_RunningWorker_Text() = 0;
        virtual std::string Alert_UpdateAvailable_Title() = 0;
        virtual std::string Alert_UpdateAvailable_Text() = 0;
        virtual std::string Feeder_Button() = 0;
        virtual std::string Shout_Button() = 0;
        virtual std::string Shout_Autolaunch() = 0;
        virtual std::string FTNZNoOutputFileException(std::string outputPath) = 0;
        virtual std::string FTNZOutputFileUnreadableException(std::string outputPath) = 0;
        virtual std::string FTNZErrorUploadingException(std::string errorMessage) = 0;
        virtual std::string FTNZXMLLibFileUnreadableException() = 0;
        virtual std::string FTNZMissingItunesConfigException() = 0;
        virtual std::string FTNZNoMusicFoundException() = 0;
        virtual std::string Feeder_Warning() = 0; 
        virtual std::string Feeder_NotifyWarningsExistence(size_t warningsCount, std::string outputFileName) = 0;
        virtual std::string Feeder_Unmolding(std::string fileName) = 0;
        virtual std::string Feeder_OutputReady() = 0;
        virtual std::string Feeder_StartSend() = 0;
        virtual std::string HTTP_ServerResponded(std::string response) = 0; 
        virtual std::string HTTP_NoResponse() = 0;
        virtual std::string Feeder_TrimingFat() = 0;
        virtual std::string Feeder_CookingJSON() = 0;
        virtual std::string Feeder_PredigestXML() = 0;
        virtual std::string Feeder_LogTrackEmit(size_t recCount, size_t expectedCount) = 0;
        virtual std::string Feeder_GetXMLFileLoc() = 0;
        virtual std::string Shout_Nothing(std::string date) = 0;
        virtual std::string Shout(std::string date, std::string name, std::string album, std::string artist, bool state) = 0;
        virtual std::string Shout_StartListening() = 0;
        virtual std::string Shout_StopListening() = 0;
        virtual std::string Shout_WaitITunes() = 0;
        virtual std::string Shout_ITunesShutdown() = 0;
        virtual std::string Shout_WaitITunesAgain() = 0;
        virtual std::string FTNZMissingConfigValuesException() = 0;
        virtual std::string FTNZErrorProcessingUploadException(long code, std::string response) = 0;  
        virtual std::string SIOWaitingConnection() = 0;
        virtual std::string SIOWaitingCredentials() = 0;
        virtual std::string SIOReconnecting() = 0;
        virtual std::string SIOAskingCredentialValidation() = 0;
        virtual std::string SIOLoggedAs(std::string username) = 0;
        virtual std::string SIOErrorOnValidation(std::string errorCode) = 0;
        virtual std::string SearchingForUpdates() = 0;
};
