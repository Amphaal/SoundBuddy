#include <ActiveQt/QAxObject>
#include <QVariant>

#include "src/workers/shout/shout.h" 

class iTunesCOMHandler : public QObject {
 
    Q_OBJECT
    
    private:
        QAxObject *iTunesObj;
        ShoutWorker *worker;
        
    public slots:
        void OnAboutToPromptUserToQuitEvent();
        void OnPlayerPlayEvent(QVariant iTrack);
        void OnPlayerStopEvent(QVariant iTrack);

    public:  
        iTunesCOMHandler(QAxObject *iTunesObj, ShoutWorker *worker);
        void shoutHelper(QVariant iTrack = NULL);
        bool iTunesShutdownRequested = false;
};
