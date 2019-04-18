#include "Ii18n.h"

using namespace std;

class AppTranslator_FR : public IAppTranslator {
    std::string Menu_File() { return "Fichier"; }
    std::string Menu_Options() { return "Options"; }
    std::string Menu_OpenMonitor() {return "Ouvrir le tableau de bord"; }
    std::string Menu_MyWTNZ() { return "Mon WTNZ"; };
    std::string Menu_UpdateConfig() { return "Mettre à jour le fichier de configuration"; };
    std::string Menu_Quit() { return "Quitter"; };
    std::string Menu_AddToStartup() { return "Lancer au démarrage système"; };
    std::string Menu_OpenWarnings() { return "Visualiser les dernières erreurs d'export"; };
    std::string Menu_CheckForUpgrades() { return "Vérifier les mises à jour"; };
    std::string Alert_RunningWorker_Title() { return "Shout en cours !"; };
    std::string Alert_RunningWorker_Text() { return "Les shouts sont actuellement scannés : Voulez-vous vraiment quitter ?"; };
    std::string Alert_UpdateAvailable_Title() { return "Mise à jour disponible"; };
    std::string Alert_UpdateAvailable_Text() { return "Une mise à jour a été trouvé pour " + (std::string)APP_NAME + ". Voulez-vous l'installer maintenant ?"; };
    std::string Feeder_Button() { return "Générer le fichier et mettre en ligne"; };
    std::string Shout_Button() { return "Se connecter à iTunes"; };
    std::string Shout_Autolaunch() { return "Démarrage automatique"; };
    std::string FTNZNoOutputFileException(std::string outputPath) { return "\"" + outputPath  + "\" n'existe pas. Merci de générer le fichier JSON dans un premier temps."; };
    std::string FTNZOutputFileUnreadableException(std::string outputPath) {return "\"" + outputPath + "\" ne peut pas être lu. Merci de le re-générer."; };
    std::string FTNZErrorUploadingException(std::string errorMessage) { return "Problème durant la communication avec le serveur : \"" + errorMessage + (std::string)"\"."; };
    std::string FTNZXMLLibFileUnreadableException() { return "Impossible de lire le fichier XML lié à votre bibliothèque. Assurez-vous que le partage du fichier XML de la bibliothèque est bien activé dans iTunes>Préférences>Avancées"; };
    std::string FTNZMissingItunesConfigException() { return "Un problème est survenu durant la recherche de l'emplacement de la bibliothèque. Etes-vous sûr d'avoir installé iTunes ?"; };
    std::string FTNZNoMusicFoundException() { return "Aucunes musiques trouvée dans iTunes. Merci de lui en fournir quelques unes !"; };
    std::string Feeder_Warning() { return "ATTENTION ! Assurez-vous que le partage du fichier XML de la bibliothèque est bien activé dans iTunes>Préférences>Avancées"; };
    std::string Feeder_NotifyWarningsExistence(size_t warningsCount, std::string outputFileName) {
        return "ATTENTION ! " + std::to_string(warningsCount) + 
                " Fichiers dans votre bibliothèque manquent d'importantes données liées aux chansons et n'ont donc pas été incluse dans le fichier de sortie ! " + 
                "Merci de vérifier le fichier \"" + outputFileName + "\" pour plus d'informations.";
    };
    std::string Feeder_Unmolding(std::string fileName) {
        return "Démoulage du fichier \"" + fileName + "\"...";
    };
    std::string Feeder_OutputReady() { return "OK, le fichier de sorti est prêt pour le petit-dej !"; };
    std::string Feeder_StartSend() { return "On va essayer de l'envoyer..."; };
    std::string HTTP_ServerResponded(std::string response) {
        return "Le serveur a répondu: " + response;
    };
    std::string HTTP_NoResponse() { return "Aucun retour du serveur... C'est étrange, merci de vérifier que le serveur ciblé est bien le bon."; };
    std::string Feeder_TrimingFat() { return "On dégraisse..."; };
    std::string Feeder_CookingJSON() { return "On cuisine le fichier JSON..."; };
    std::string Feeder_PredigestXML() { return "On pré-traite le fichier XML..."; };
    std::string Feeder_LogTrackEmit(size_t recCount, size_t expectedCount) {
        return std::to_string(recCount) + " sur " + std::to_string(expectedCount) + "...";
    };
    std::string Feeder_GetXMLFileLoc() { return "On récupère l'emplacement du fichier XML..."; };
    std::string Shout_Nothing(std::string date) {
        return date + ": Shout -> Rien";
    };
    std::string Shout(std::string date, std::string name, std::string album, std::string artist, bool state) {
        std::string pState = state ? "joue" : "pause";
        return date + ": Shout -> " + name + " - " +
        album + " - " + artist + " (" + pState + ")";
    };
    std::string Shout_StartListening() { return "J'écoute iTunes !"; };
    std::string Shout_StopListening() { return "J'ai fini d'écouter iTunes."; };
    std::string Shout_WaitITunes() { return "J'attends qu'iTunes se lance..."; };
    std::string Shout_ITunesShutdown() { return "iTunes s'éteint !"; };
    std::string Shout_WaitITunesAgain() { return "J'attends qu'iTunes se relance..."; };
    std::string FTNZMissingConfigValuesException() { return "Des paramètres de configurations sont attendus ! Merci de vérifier le fichier de configuration."; };
    std::string FTNZErrorProcessingUploadException(long code, std::string response) { 
        return "Le serveur a rencontré une erreur durant le traitement des informations => HTTP Code " + std::to_string(code) + " : " + response;
    };
    std::string SIOWaitingConnection() { return "Connexion au serveur...";};
    std::string SIOWaitingCredentials() { return "En attente d'identifiants appropriés.";};
    std::string SIOReconnecting() { return "Reconnexion au serveur...";};
    std::string SIOAskingCredentialValidation() { return "Demande de validation des identifiants...";};
    std::string SIOLoggedAs(std::string username) { return (std::string)"Connecté en tant que \"" + username + "\"";};
    std::string SIOErrorOnValidation(std::string errorCode) { 
        std::string part;
        if(errorCode == "cdm") {
           part = "Données manquantes pour l'identification";
        } else if(errorCode == "eud") {
            part = "Base de données utilisateur vide";
        } else if(errorCode == "unfid") {
            part = "Utilisateur introuvable dans la base de données";
        } else if(errorCode == "nopass") {
            part = "Mot de passe de l'utilisateur introuvable";
        } else if(errorCode == "pmiss") {
            part = "Mot de passe incorrect";
        } else {
            return "Erreur inconnue suite à la tentative de validation";
        }
        return (std::string)"Reponse serveur : \"" + part + "\"";
    };
    std::string SearchingForUpdates() { return "Recherche des mises à jour...";};
};