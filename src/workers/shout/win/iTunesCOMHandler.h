#include <ActiveQt/QAxObject>
#include <QVariant>

#include "../shout.h" 

class iTunesCOMHandler : public QObject {
 
    Q_OBJECT
    
    private:
        QAxObject *iTunesObj;
        ShoutWorker *worker;
        size_t lastTrackHash;

    public slots:
        void OnAboutToPromptUserToQuitEvent();
        void OnPlayerPlayEvent(QVariant iTrack);
        void OnPlayerStopEvent(QVariant iTrack);  

    public:  
        iTunesCOMHandler(QAxObject *iTunesObj, ShoutWorker *worker);
        void shoutHelper(QVariant iTrack = NULL);
        bool iTunesShutdownRequested = false;
};