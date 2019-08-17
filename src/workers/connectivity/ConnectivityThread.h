#pragma once

#include <exception>
#include <string>

#include "src\ui\widgets\LightWidget.h"

#include "src/_libs/socketiocpp_custom/sio_client.h"
#include "src/helpers/configHelper/authHelper.cpp"
#include "src/localization/i18n.cpp"
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
        void updateSIOStatus(const std::string &message, const TLW_Colors &color);

    private:
        sio::client* _sioClient = nullptr;
        std::string _loggedInUser = "";
        bool _requestOngoing = false;
        AuthHelper *_aHelper = nullptr;
    
        //ask credentials
        void _checkCredentials(bool forceRecheck = false);
        std::string _getTargetUrl();
};