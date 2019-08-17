#include "Ii18n.h"

class AppTranslator_EN : public IAppTranslator {
    QString Menu_File() { return "File"; }
    QString Menu_Options() { return "Options"; }
    QString Menu_OpenMonitor() {return "Open monitor..."; }
    QString Menu_MyWTNZ() { return "My WTNZ"; };
    QString Menu_UpdateConfig() { return "Update configuration file"; };
    QString Menu_Quit() { return "Quit"; };
    QString Menu_AddToStartup() { return "Lauch at system boot"; };
    QString Menu_OpenWarnings() { return "Access recent upload warnings"; };
    QString Menu_CheckForUpgrades() { return "Check for upgrades"; };
    QString Alert_RunningWorker_Title() { return "Shout worker running !"; };
    QString Alert_RunningWorker_Text() { return "Shout worker is actually running : Are you sure you want to exit ?"; };
    QString Alert_UpdateAvailable_Title() { return "Update Available"; };
    QString Alert_UpdateAvailable_Text() { return QString("An update is available for %1. Would you like to install it now ?").arg(APP_NAME); };
    QString Feeder_Button() { return "Generate Digest and Upload"; };
    QString Shout_Button() { return "Connect to iTunes"; };
    QString Shout_Autolaunch() { return "Autostart at launch"; };
    QString FTNZNoOutputFileException(const QString &outputPath) { return QString("\"%1\" does not exist. Please generate JSON before.").arg(outputPath); };
    QString FTNZOutputFileUnreadableException(const QString &outputPath) {return QString("\"%1\" cannot be read. Please regenerate it.").arg(outputPath); };
    QString FTNZErrorUploadingException(const QString &errorMessage) { return QString("Error communicating with the remote server : \"%1\".").arg(errorMessage); };
    QString FTNZXMLLibFileUnreadableException() { return "Cannot read the XML file bound to your library. Are you sure you activated the XML file sharing in iTunes ?"; };
    QString FTNZMissingItunesConfigException() { return "An issue happened while fetching iTunes's XML file location. Have you installed iTunes ?"; };
    QString FTNZNoMusicFoundException() { return "No music found in your library. Please feed it some."; };
    QString Feeder_Warning() { return "WARNING ! Make sure you activated the XML file sharing in iTunes>Preferences>Advanced."; };
    QString Feeder_NotifyWarningsExistence(size_t warningsCount, const QString &outputFileName) {
        return QString("WARNING ! %1 files in your library are missing important metadata and consequently were removed from the output file ! Please check the \"%2\" file for more informations.").arg(warningsCount).arg(outputFileName);
    };
    QString Feeder_Unmolding(const QString &fileName) {
        return QString("Unmolding \"%1\"...").arg(fileName);
    };
    QString Feeder_OutputReady() { return "OK, output file is ready for breakfast !"; };
    QString Feeder_StartSend() { return "Let's try to send now !"; };
    QString HTTP_ServerResponded(const QString &response) {
        return QString("Server responded: %1").arg(response);
    };
    QString HTTP_NoResponse() { return "No feedback from the server ? Strange... Please check the targeted host."; };
    QString Feeder_TrimingFat() { return "Triming fat..."; };
    QString Feeder_CookingJSON() { return "Cooking the JSON file..."; };
    QString Feeder_PredigestXML() { return "Pre-digesting XML file..."; };
    QString Feeder_LogTrackEmit(size_t recCount, size_t expectedCount) {
        return QString("%1 over %2 ...").arg(recCount).arg(expectedCount);
    };
    QString Feeder_GetXMLFileLoc() { return "Getting XML file location..."; };
    QString Shout_Nothing(const QString &date) {
        return QString("%1: Shouting -> Nothing").arg(date);
    };
    QString Shout(const QString &date, const QString &name, const QString &album, const QString &artist, bool state) {
        QString pState = state ? "playing" : "paused";
        return QString("%1: Shouting -> %2 - %3 - %4 (%5)").arg(date).arg(name).arg(album).arg(artist).arg(pState);
    };
    QString Shout_StartListening() { return "Listening to iTunes !"; };
    QString Shout_StopListening() { return "Stopped listening to iTunes."; };
    QString Shout_WaitITunes() { return "Waiting for iTunes to launch..."; };
    QString Shout_ITunesShutdown() { return "iTunes shutting down !"; };
    QString Shout_WaitITunesAgain() { return "Waiting for iTunes to launch again..."; };
    QString FTNZMissingConfigValuesException() { return "Expected configuration values are missing. Please check the configuration file !"; };
    QString FTNZErrorProcessingUploadException(long code, const QString &response) { 
        return QString("The server encountered an error while processing the data => HTTP Code %1 : %2").arg(code).arg(response);
    };
    QString SIOWaitingConnection() { return "Connecting to server...";};
    QString SIOWaitingCredentials() { return "Waiting for appropriate credentials.";};
    QString SIOReconnecting() { return "Reconnecting to server...";};
    QString SIOAskingCredentialValidation() { return "Asking for credentials validation...";};
    QString SIOLoggedAs(const QString &username) { return QString("Logged as \"%1\"").arg(username);};
    QString SIOErrorOnValidation(const QString &errorCode) { 
        QString part;
        
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

        return QString("Server responded with : \"%1\"").arg(part);
    };
    QString SearchingForUpdates() { return "Searching for updates...";};
};