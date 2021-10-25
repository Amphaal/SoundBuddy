#pragma once


#include <QMetaObject>
#include <QMetaMethod>
#include <QCoreApplication>
#include <QDateTime>
#include <QVariant>

#include "iTunesCOMHandler.h"

#include "src/workers/shout/ShoutThread.h"

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
