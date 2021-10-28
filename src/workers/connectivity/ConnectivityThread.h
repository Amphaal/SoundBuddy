#pragma once

#include <QThread>
#include <QSettings>

#include <string>
#include <iostream>

#include "src/ui/widgets/LightWidget.h"


class ConnectivityThread : public QThread {
   Q_OBJECT

 public:
    ConnectivityThread(const QSettings* appSettings);

    void run() override;

 signals:
    void updateSIOStatus(const QString &message, const TLW_Colors &color);
    void requestCredentialCheck();

 private:
    bool _requestOngoing = false;
    sio::client* _sioClient = nullptr;
    const QSettings* _appSettings;
    QString _loggedInUser;

    // ask credentials
    void _checkCredentials(bool forceRecheck = false);
    void _checkCredentialsFromFileUpdate();
    QString _getTargetUrl();


    const QString _validationErrorTr(const QString& errorCode) const;
    void _emitLoggedUserMsg();
};
