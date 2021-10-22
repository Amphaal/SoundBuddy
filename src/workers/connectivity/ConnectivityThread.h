#pragma once

#include "src/workers/connectivity/ConnectivityThread.sio.h"

#include <QFileSystemWatcher>
#include <QThread>

#include <string>
#include <iostream>

#include "src/ui/widgets/LightWidget.h"

#include "src/helpers/configHelper/authHelper.hpp"


class ConnectivityThread : public QThread {
    Q_OBJECT

 public:
    ConnectivityThread(AuthHelper *aHelper, QFileSystemWatcher* toWatchOverChanges);

    void run() override;

 signals:
    void updateSIOStatus(const QString &message, const TLW_Colors &color);
    void requestCredentialCheck();

 private:
    sio::client* _sioClient = nullptr;
    QString _loggedInUser;
    bool _requestOngoing = false;
    AuthHelper* _aHelper = nullptr;
    QFileSystemWatcher* _toWatchOverChanges = nullptr;

    // ask credentials
    void _checkCredentials(bool forceRecheck = false);
    void _checkCredentialsFromFileUpdate();
    QString _getTargetUrl();


    const QString _validationErrorTr(const QString& errorCode) const;
    void _emitLoggedUserMsg();
};
