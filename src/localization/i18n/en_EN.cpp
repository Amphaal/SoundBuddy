class AppTranslator_EN : public IAppTranslator {
    QString Alert_RunningWorker_Title() { return "Shout worker running !"; };
    QString Alert_RunningWorker_Text() { return "Shout worker is actually running : Are you sure you want to exit ?"; };
    QString Alert_UpdateAvailable_Title() { return "Update Available"; };
    QString Alert_UpdateAvailable_Text() { return QString("An update is available for %1. Would you like to install it now ?").arg(APP_NAME); };
    QString Shout_Button() { return "Connect to iTunes"; };
    QString Shout_Autolaunch() { return "Autostart at launch"; };
    QString FTNZNoOutputFileException(const QString &outputPath) { return QString("\"%1\" does not exist. Please generate JSON before.").arg(outputPath); };
    QString FTNZOutputFileUnreadableException(const QString &outputPath) {return QString("\"%1\" cannot be read. Please regenerate it.").arg(outputPath); };
    QString FTNZErrorUploadingException(const QString &errorMessage) { return QString("Error communicating with the remote server : \"%1\".").arg(errorMessage); };
    QString FTNZXMLLibFileUnreadableException() { return "Cannot read the XML file bound to your library. Are you sure you activated the XML file sharing in iTunes ?"; };
    QString FTNZMissingItunesConfigException() { return "An issue happened while fetching iTunes's XML file location. Have you installed iTunes ?"; };
    QString FTNZNoMusicFoundException() { return "No music found in your library. Please feed it some."; };
    QString Feeder_Warning() { return "WARNING ! Make sure you activated the XML file sharing in iTunes>Preferences>Advanced."; };
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
    QString FTNZErrorProcessingUploadException(long code, const QString &response) { 
        return QString("The server encountered an error while processing the data => HTTP Code %1 : %2").arg(code).arg(response);
    };
    QString SIOWaitingConnection() { return "Connecting to server...";};
    QString SIOWaitingCredentials() { return "Waiting for appropriate credentials.";};
    QString SIOReconnecting() { return "Reconnecting to server...";};
    QString SIOAskingCredentialValidation() { return "Asking for credentials validation...";};
};