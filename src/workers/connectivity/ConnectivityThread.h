#pragma once

#include <exception>
#include <string>

#include "src\ui\widgets\LightWidget.h"

#include "src/_libs/socketiocpp_custom/sio_client.h"
#include "src/helpers/configHelper/authHelper.hpp"
#include "src/localization/i18n.hpp"
#include <iostream>

#include <QThread>

class ConnectivityThread : public QThread {
    
    Q_OBJECT

    public:
        ConnectivityThread(AuthHelper *aHelper);
        ~ConnectivityThread();

        void run() override;
        void askCheckCredentials();

    signals:
        void updateSIOStatus(const QString &message, const TLW_Colors &color);

    private:
        sio::client* _sioClient = nullptr;
        QString _loggedInUser = "";
        bool _requestOngoing = false;
        AuthHelper* _aHelper = nullptr;
    
        //ask credentials
        void _checkCredentials(bool forceRecheck = false);
        QString _getTargetUrl();
};