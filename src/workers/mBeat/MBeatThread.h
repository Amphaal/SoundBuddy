#pragma once

#include <QThread>
#include <QWebSocket>

#include "src/helpers/AppSettings.hpp"

#include "src/ui/widgets/LightWidget.h"


class MBeatThread : public QThread {
   Q_OBJECT

 public:
    MBeatThread(const AppSettings::ConnectivityInfos &connectivityInfos);

    void run() override;

 signals:
    void updateSIOStatus(const QString &message, const TLW_Colors &color);
    void requestCredentialCheck();

 private:
    bool _requestOngoing = false;
    QWebSocket* _wsClient;
    QString _loggedInUser;

    // ask credentials
    void _checkCredentials(bool forceRecheck = false);
    void _checkCredentialsFromFileUpdate();
    QString _getPlatformHostUrl();


    const QString _validationErrorTr(const QString& errorCode) const;
    void _emitLoggedUserMsg();
};
