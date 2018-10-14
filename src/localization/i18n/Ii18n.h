#pragma once
#include <string>

class IFeedTNZTranslator {
    public:
        virtual char* Menu_File() = 0;
        virtual char* Menu_OpenMonitor() = 0;
        virtual char* Menu_MyWTNZ() = 0;
        virtual char* Menu_UpdateConfig() = 0;
        virtual char* Menu_Quit() = 0;
        virtual char* Alert_RunningWorker_Title() = 0;
        virtual char* Alert_RunningWorker_Text() = 0;
        virtual char* Feeder_Button() = 0;
        virtual char* Shout_Button() = 0;
        virtual char* Shout_Autolaunch() = 0;
        virtual char* FTNZNoOutputFileException() = 0;
        virtual char* FTNZOutputFileUnreadableException() = 0;
        virtual char* FTNZErrorUploadingException() = 0;
        virtual char* FTNZXMLLibFileUnreadableException() = 0;
        virtual char* FTNZMissingItunesConfigException() = 0;
        virtual char* FTNZNoMusicFoundException() = 0;
        virtual char* Feeder_Warning() = 0; 
        virtual char* Feeder_NotifyWarningsExistence(size_t warningsCount, string outputFileName) = 0;
        virtual char* Feeder_Unmolding() = 0;
        virtual char* Feeder_OutputReady() = 0;
        virtual char* Feeder_StartSend() = 0;
        virtual char* HTTP_ServerResponded(string response) = 0; 
        virtual char* HTTP_NoResponse() = 0;
        virtual char* Feeder_TrimingFat() = 0;
        virtual char* Feeder_CookingJSON() = 0;
        virtual char* Feeder_PredigestXML() = 0;
        virtual char* Feeder_LogTrackEmit(size_t recCount, size_t expectedCount) = 0;
        virtual char* Feeder_GetXMLFileLoc() = 0;
};
