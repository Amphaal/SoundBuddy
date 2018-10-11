#include <ActiveQt/QAxObject>
#include <QVariant>

#include "../shout.h" 

class ITunesEventsSink : public QObject {
 
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
        ITunesEventsSink(QAxObject *iTunesObj, ShoutWorker *worker);
        void shoutHelper(QVariant iTrack = NULL);
};