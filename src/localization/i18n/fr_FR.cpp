#include "Ii18n.h"

class AppTranslator_FR : public IAppTranslator {
    QString Menu_File() { return "Fichier"; }
    QString Menu_Options() { return "Options"; }
    QString Menu_OpenMonitor() {return "Ouvrir le tableau de bord"; }
    QString Menu_MyWTNZ() { return "Mon WTNZ"; };
    QString Menu_UpdateConfig() { return "Mettre à jour le fichier de configuration"; };
    QString Menu_Quit() { return "Quitter"; };
    QString Menu_AddToStartup() { return "Lancer au démarrage système"; };
    QString Menu_OpenWarnings() { return "Visualiser les dernières erreurs d'export"; };
    QString Menu_OpenDataFolder() { return "Accéder aux données d'export"; };
    QString Menu_CheckForUpgrades() { return "Vérifier les mises à jour"; };
    QString Alert_RunningWorker_Title() { return "Shout en cours !"; };
    QString Alert_RunningWorker_Text() { return "Les shouts sont actuellement scannés : Voulez-vous vraiment quitter ?"; };
    QString Alert_UpdateAvailable_Title() { return "Mise à jour disponible"; };
    QString Alert_UpdateAvailable_Text() { return QString("Une mise à jour a été trouvé pour %1. Voulez-vous l'installer maintenant ?").arg(APP_NAME); };
    QString Feeder_Button() { return "Générer le fichier et mettre en ligne"; };
    QString Shout_Button() { return "Se connecter à iTunes"; };
    QString Shout_Autolaunch() { return "Démarrage automatique"; };
    QString FTNZNoOutputFileException(const QString &outputPath) { return QString("\"%1\" n'existe pas. Merci de générer le fichier JSON dans un premier temps.").arg(outputPath); };
    QString FTNZOutputFileUnreadableException(const QString &outputPath) {return QString("\"%1\" ne peut pas être lu. Merci de le re-générer.").arg(outputPath); };
    QString FTNZErrorUploadingException(const QString &errorMessage) { return QString("Problème durant la communication avec le serveur : \"%1\".").arg(errorMessage); };
    QString FTNZXMLLibFileUnreadableException() { return "Impossible de lire le fichier XML lié à votre bibliothèque. Assurez-vous que le partage du fichier XML de la bibliothèque est bien activé dans iTunes>Préférences>Avancées"; };
    QString FTNZMissingItunesConfigException() { return "Un problème est survenu durant la recherche de l'emplacement de la bibliothèque. Etes-vous sûr d'avoir installé iTunes ?"; };
    QString FTNZNoMusicFoundException() { return "Aucunes musiques trouvée dans iTunes. Merci de lui en fournir quelques unes !"; };
    QString Feeder_Warning() { return "ATTENTION ! Assurez-vous que le partage du fichier XML de la bibliothèque est bien activé dans iTunes>Préférences>Avancées"; };
    QString Feeder_NotifyWarningsExistence(size_t warningsCount, const QString &outputFileName) {
        return QString("ATTENTION ! %1 fichiers dans votre bibliothèque manquent d'importantes données liées aux chansons et n'ont donc pas été incluse dans le fichier de sortie ! Merci de vérifier le fichier \"%2\" pour plus d'informations.").arg(warningsCount).arg(outputFileName);
    };
    QString Feeder_Unmolding(const QString &fileName) {
        return QString("Démoulage du fichier \"%1\"...").arg(fileName);
    };
    QString Feeder_OutputReady() { return "OK, le fichier de sorti est prêt pour le petit-dej !"; };
    QString Feeder_StartSend() { return "On va essayer de l'envoyer..."; };
    QString HTTP_ServerResponded(const QString &response) {
        return QString("Le serveur a répondu: %1").arg(response);
    };
    QString HTTP_NoResponse() { return "Aucun retour du serveur... C'est étrange, merci de vérifier que le serveur ciblé est bien le bon."; };
    QString Feeder_TrimingFat() { return "On dégraisse..."; };
    QString Feeder_CookingJSON() { return "On cuisine le fichier JSON..."; };
    QString Feeder_PredigestXML() { return "On pré-traite le fichier XML..."; };
    QString Feeder_LogTrackEmit(size_t recCount, size_t expectedCount) {
        return QString("%1 sur %2 ...").arg(recCount).arg(expectedCount);
    };
    QString Feeder_GetXMLFileLoc() { return "On récupère l'emplacement du fichier XML..."; };
    QString Shout_Nothing(const QString &date) {
        return QString("%1: Shout -> Rien").arg(date);
    };
    QString Shout(const QString &date, const QString &name, const QString &album, const QString &artist, bool state) {
        QString pState = state ? "joue" : "pause";
        return QString("%1: Shouting -> %2 - %3 - %4 (%5)").arg(date).arg(name).arg(album).arg(artist).arg(pState);
    };
    QString Shout_StartListening() { return "J'écoute iTunes !"; };
    QString Shout_StopListening() { return "J'ai fini d'écouter iTunes."; };
    QString Shout_WaitITunes() { return "J'attends qu'iTunes se lance..."; };
    QString Shout_ITunesShutdown() { return "iTunes s'éteint !"; };
    QString Shout_WaitITunesAgain() { return "J'attends qu'iTunes se relance..."; };
    QString FTNZMissingConfigValuesException() { return "Des paramètres de configurations sont attendus ! Merci de vérifier le fichier de configuration."; };
    QString FTNZErrorProcessingUploadException(long code, const QString &response) { 
        return QString("Le serveur a rencontré une erreur durant le traitement des informations => HTTP Code %1 : %2").arg(code).arg(response);
    };
    QString SIOWaitingConnection() { return "Connexion au serveur...";};
    QString SIOWaitingCredentials() { return "En attente d'identifiants appropriés.";};
    QString SIOReconnecting() { return "Reconnexion au serveur...";};
    QString SIOAskingCredentialValidation() { return "Demande de validation des identifiants...";};
    QString SIOLoggedAs(const QString &username) { return QString("Connecté en tant que \"%1\"").arg(username);};
    QString SIOErrorOnValidation(const QString &errorCode) { 
        
        QString part;
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

        return QString("Reponse serveur : \"%1\"").arg(part);
    };
    QString SearchingForUpdates() { return "Recherche des mises à jour...";};
};