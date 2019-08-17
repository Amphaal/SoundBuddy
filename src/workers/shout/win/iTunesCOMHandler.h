#pragma once

#include <QAxBase>
#include <QAxObject>
#include <QVariant>

#include <windows.h>

#include "src/workers/shout/ShoutThread.h" 
#include "iTunesCOMHandler.h"
#include "src/helpers/stringHelper/stringHelper.hpp"

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
        void shoutHelper(QVariant iTrack = QVariant());
        bool iTunesShutdownRequested = false;
};
