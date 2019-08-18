#pragma once

#include "src/helpers/_const.hpp"
#include "src/version.h"

class IAppTranslator {
    public:
        virtual QString Menu_File() = 0;
        virtual QString Menu_Options() = 0;
        virtual QString Menu_OpenMonitor() = 0;
        virtual QString Menu_MyWTNZ() = 0;
        virtual QString Menu_UpdateConfig() = 0;
        virtual QString Menu_OpenWarnings() = 0;
        virtual QString Menu_OpenDataFolder() = 0;
        virtual QString Menu_Quit() = 0;
        virtual QString Menu_CheckForUpgrades() = 0;
        virtual QString Menu_AddToStartup() = 0;
        virtual QString Alert_RunningWorker_Title() = 0;
        virtual QString Alert_RunningWorker_Text() = 0;
        virtual QString Alert_UpdateAvailable_Title() = 0;
        virtual QString Alert_UpdateAvailable_Text() = 0;
        virtual QString Feeder_Button() = 0;
        virtual QString Shout_Button() = 0;
        virtual QString Shout_Autolaunch() = 0;
        virtual QString FTNZNoOutputFileException(const QString &outputPath) = 0;
        virtual QString FTNZOutputFileUnreadableException(const QString &outputPath) = 0;
        virtual QString FTNZErrorUploadingException(const QString &errorMessage) = 0;
        virtual QString FTNZXMLLibFileUnreadableException() = 0;
        virtual QString FTNZMissingItunesConfigException() = 0;
        virtual QString FTNZNoMusicFoundException() = 0;
        virtual QString Feeder_Warning() = 0; 
        virtual QString Feeder_NotifyWarningsExistence(size_t warningsCount, const QString &outputFileName) = 0;
        virtual QString Feeder_Unmolding(const QString &fileName) = 0;
        virtual QString Feeder_OutputReady() = 0;
        virtual QString Feeder_StartSend() = 0;
        virtual QString HTTP_ServerResponded(const QString &response) = 0; 
        virtual QString HTTP_NoResponse() = 0;
        virtual QString Feeder_TrimingFat() = 0;
        virtual QString Feeder_CookingJSON() = 0;
        virtual QString Feeder_PredigestXML() = 0;
        virtual QString Feeder_LogTrackEmit(size_t recCount, size_t expectedCount) = 0;
        virtual QString Feeder_GetXMLFileLoc() = 0;
        virtual QString Shout_Nothing(const QString &date) = 0;
        virtual QString Shout(const QString &date, const QString &name, const QString &album, const QString &artist, bool state) = 0;
        virtual QString Shout_StartListening() = 0;
        virtual QString Shout_StopListening() = 0;
        virtual QString Shout_WaitITunes() = 0;
        virtual QString Shout_ITunesShutdown() = 0;
        virtual QString Shout_WaitITunesAgain() = 0;
        virtual QString FTNZMissingConfigValuesException() = 0;
        virtual QString FTNZErrorProcessingUploadException(long code, const QString &response) = 0;  
        virtual QString SIOWaitingConnection() = 0;
        virtual QString SIOWaitingCredentials() = 0;
        virtual QString SIOReconnecting() = 0;
        virtual QString SIOAskingCredentialValidation() = 0;
        virtual QString SIOLoggedAs(const QString &username) = 0;
        virtual QString SIOErrorOnValidation(const QString &errorCode) = 0;
        virtual QString SearchingForUpdates() = 0;
};
