#pragma once


#include <QMetaObject>
#include <QMetaMethod>
#include <QCoreApplication>
#include <QDateTime>
#include <QVariant>

#include "MusicAppCOMHandler.h"

#include "src/workers/shout/ShoutThread.h"

class MusicAppCOMHandler : public QObject {
   Q_OBJECT

 private:
    QAxObject *MusicAppObj;
    ShoutThread *worker;

 public slots:
    void OnAboutToPromptUserToQuitEvent();
    void OnPlayerPlayEvent(QVariant iTrack);
    void OnPlayerStopEvent(QVariant iTrack);

 public:
    MusicAppCOMHandler(QAxObject *MusicAppObj, ShoutThread *worker);
    void shoutHelper(QVariant iTrack = QVariant());
    bool musicAppShutdownRequested = false;
};
