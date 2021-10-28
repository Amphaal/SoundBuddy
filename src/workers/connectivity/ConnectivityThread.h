#pragma once

#include <QThread>

#include <string>
#include <iostream>

#include "src/helpers/AppSettings.hpp"

#include "src/ui/widgets/LightWidget.h"


class ConnectivityThread : public QThread {
   Q_OBJECT

 public:
    ConnectivityThread(const AppSettings::ConnectivityInfos &connectivityInfos);

    void run() override;

 signals:
    void updateSIOStatus(const QString &message, const TLW_Colors &color);
    void requestCredentialCheck();

 private:
    bool _requestOngoing = false;
    sio::client* _sioClient = nullptr;
    const AppSettings::ConnectivityInfos _connectivityInfos;
    QString _loggedInUser;

    // ask credentials
    void _checkCredentials(bool forceRecheck = false);
    void _checkCredentialsFromFileUpdate();
    QString _getPlatformHostUrl();


    const QString _validationErrorTr(const QString& errorCode) const;
    void _emitLoggedUserMsg();
};
