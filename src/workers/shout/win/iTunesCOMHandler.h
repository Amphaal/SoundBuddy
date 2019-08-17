#pragma once

#include <ActiveQt/QAxBase>
#include <ActiveQt/QAxObject>
#include <QVariant>

#include <windows.h>

#include "src/workers/shout/ShoutThread.h" 
#include "iTunesCOMHandler.h"
#include "src/helpers/stringHelper/stringHelper.cpp"

#include <QMetaObject>
#include <QMetaMethod>
#include <QObject>
#include <QCoreApplication>
#include <QDateTime>

class iTunesCOMHandler : public QObject {
 
    Q_OBJECT
    
    private:
        QAxObject *iTunesObj;
        ShoutThread *worker;
        
    public slots:
        void OnAboutToPromptUserToQuitEvent();
        void OnPlayerPlayEvent(QVariant iTrack);
        void OnPlayerStopEvent(QVariant iTrack);

    public:  
        iTunesCOMHandler(QAxObject *iTunesObj, ShoutThread *worker);
        void shoutHelper(QVariant iTrack = NULL);
        bool iTunesShutdownRequested = false;
};
