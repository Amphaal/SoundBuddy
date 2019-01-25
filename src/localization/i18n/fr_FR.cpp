#include "Ii18n.h"

using namespace std;

class FeedTNZTranslator_FR : public IFeedTNZTranslator {
    string Menu_File() { return "Fichier"; }
    string Menu_Options() { return "Options"; }
    string Menu_OpenMonitor() {return "Ouvrir le tableau de bord"; }
    string Menu_MyWTNZ() { return "Mon WTNZ"; };
    string Menu_UpdateConfig() { return "Mettre à jour le fichier de configuration"; };
    string Menu_Quit() { return "Quitter"; };
    string Menu_AddToStartup() { return "Lancer au démarrage système"; };
    string Menu_OpenWarnings() { return "Visualiser les dernières erreurs d'export"; };
    string Menu_CheckForUpgrades() { return "Vérifier les mises à jour"; };
    string Alert_RunningWorker_Title() { return "Shout en cours !"; };
    string Alert_RunningWorker_Text() { return "Les shouts sont actuellement scannés : Voulez-vous vraiment quitter ?"; };
    string Alert_UpdateAvailable_Title() { return "Mise à jour disponible"; };
    string Alert_UpdateAvailable_Text() { return "Une mise à jour a été trouvé pour " + APP_NAME + ". Voulez-vous l'installer maintenant ?"; };
    string Feeder_Button() { return "Générer le fichier et mettre en ligne"; };
    string Shout_Button() { return "Se connecter à iTunes"; };
    string Shout_Autolaunch() { return "Démarrage automatique"; };
    string FTNZNoOutputFileException(string outputPath) { return "\"" + outputPath  + "\" n'existe pas. Merci de générer le fichier JSON dans un premier temps."; };
    string FTNZOutputFileUnreadableException(string outputPath) {return "\"" + outputPath + "\" ne peut pas être lu. Merci de le re-générer."; };
    string FTNZErrorUploadingException() { return "Problème durant la communication avec le serveur"; };
    string FTNZXMLLibFileUnreadableException() { return "Impossible de lire le fichier XML lié à votre bibliothèque. Assurez-vous que le partage du fichier XML de la bibliothèque est bien activé dans iTunes>Préférences>Avancées"; };
    string FTNZMissingItunesConfigException() { return "Un problème est survenu durant la recherche de l'emplacement de la bibliothèque. Etes-vous sûr d'avoir installé iTunes ?"; };
    string FTNZNoMusicFoundException() { return "Aucunes musiques trouvée dans iTunes. Merci de lui en fournir quelques unes !"; };
    string Feeder_Warning() { return "ATTENTION ! Assurez-vous que le partage du fichier XML de la bibliothèque est bien activé dans iTunes>Préférences>Avancées"; };
    string Feeder_NotifyWarningsExistence(size_t warningsCount, string outputFileName) {
        return "ATTENTION ! " + std::to_string(warningsCount) + 
                " Fichiers dans votre bibliothèque manquent d'importantes données liées aux chansons et n'ont donc pas été incluse dans le fichier de sortie ! " + 
                "Merci de vérifier le fichier \"" + outputFileName + "\" pour plus d'informations.";
    };
    string Feeder_Unmolding(string fileName) {
        return "Démoulage du fichier \"" + fileName + "\"...";
    };
    string Feeder_OutputReady() { return "OK, le fichier de sorti est prêt pour le petit-dej !"; };
    string Feeder_StartSend() { return "On va essayer de l'envoyer..."; };
    string HTTP_ServerResponded(string response) {
        return "Le serveur a répondu: " + response;
    };
    string HTTP_NoResponse() { return "Aucun retour du serveur... C'est étrange, merci de vérifier que le serveur ciblé est bien le bon."; };
    string Feeder_TrimingFat() { return "On dégraisse..."; };
    string Feeder_CookingJSON() { return "On cuisine le fichier JSON..."; };
    string Feeder_PredigestXML() { return "On pré-traite le fichier XML..."; };
    string Feeder_LogTrackEmit(size_t recCount, size_t expectedCount) {
        return std::to_string(recCount) + " sur " + std::to_string(expectedCount) + "...";
    };
    string Feeder_GetXMLFileLoc() { return "On récupère l'emplacement du fichier XML..."; };
    string Shout_Nothing(string date) {
        return date + ": Shout -> Rien";
    };
    string Shout(string date, string name, string album, string artist, bool state) {
        string pState = state ? "joue" : "pause";
        return date + ": Shout -> " + name + " - " +
        album + " - " + artist + " (" + pState + ")";
    };
    string Shout_StartListening() { return "J'écoute iTunes !"; };
    string Shout_StopListening() { return "J'ai fini d'écouter iTunes."; };
    string Shout_WaitITunes() { return "J'attends qu'iTunes se lance..."; };
    string Shout_ITunesShutdown() { return "iTunes s'éteint !"; };
    string Shout_WaitITunesAgain() { return "J'attends qu'iTunes se relance..."; };
    string FTNZMissingConfigValuesException() { return "Des paramètres de configurations sont attendus ! Merci de vérifier le fichier de configuration."; };
    string FTNZErrorProcessingUploadException(long code, string response) { 
        return "Le serveur a rencontré une erreur durant le traitement des informations => HTTP Code " + std::to_string(code) + " : " + response;
    };
};