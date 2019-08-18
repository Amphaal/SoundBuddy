#include "ConnectivityThread.h"

ConnectivityThread::ConnectivityThread(AuthHelper *aHelper, QFileSystemWatcher* toWatchOverChanges) : _aHelper(aHelper), _toWatchOverChanges(toWatchOverChanges) {}

void ConnectivityThread::run() {

    ////////////////////
    // Event Handlers //
    ////////////////////

    this->_sioClient = new sio::client();

    //tell sio is trying to reconnect
    this->_sioClient->set_reconnect_listener([&](unsigned int a, unsigned int b) {
        emit updateSIOStatus(I18n::tr()->SIOReconnecting(), TLW_Colors::YELLOW);
    });

    //on connect, check credentials
    this->_sioClient->set_open_listener([&]() {
        this->_checkCredentials();
    });


    //once server checked the credentials
    this->_sioClient->socket("/login")->on("credentialsChecked", [&](sio::event& ev) {
        
        //extract response
        auto response = ev.get_messages()[0]->get_map();
        auto isOk = response["isLoginOk"]->get_bool();
        QString accomp = QString::fromStdString(response["accomp"]->get_string());

        if(isOk) {
            this->_loggedInUser = accomp;
            emit updateSIOStatus(I18n::tr()->SIOLoggedAs(accomp), TLW_Colors::GREEN);
        } else {
            emit updateSIOStatus(I18n::tr()->SIOErrorOnValidation(accomp), TLW_Colors::RED);
        }

        //toggle flag
        this->_requestOngoing = false;
    });

    //when server tell us the database has been updated, ask for revalidation
    this->_sioClient->socket("/login")->on("databaseUpdated", [&](sio::event& ev) {
        this->_checkCredentials(true);
    });

    ////////////////////////
    // End Event Handlers //
    ////////////////////////
    
    //declare waiting for connection
    emit updateSIOStatus(I18n::tr()->SIOWaitingConnection(), TLW_Colors::YELLOW);
    
    //connect...
    this->_sioClient->connect(this->_getTargetUrl().toStdString());

    QObject::connect(
        this->_toWatchOverChanges, &QFileSystemWatcher::fileChanged,
        this, &ConnectivityThread::_checkCredentialsFromFileUpdate
    );

    this->exec();

    //clear
    delete this->_sioClient;

}

void ConnectivityThread::_checkCredentialsFromFileUpdate() {
    this->_checkCredentials(true);
}

//ask credentials
void ConnectivityThread::_checkCredentials(bool forceRecheck) {

    if(forceRecheck) {
        this->_loggedInUser = "";
        this->_requestOngoing = false;
    }

    if(this->_loggedInUser != "" && !forceRecheck) {
        emit updateSIOStatus(I18n::tr()->SIOLoggedAs(this->_loggedInUser), TLW_Colors::GREEN);
        return;
    }

    auto prerequisitesOK = this->_aHelper->ensureConfigFileIsReadyForUpload(false);

    if (!prerequisitesOK) {
        emit updateSIOStatus(I18n::tr()->SIOWaitingCredentials(), TLW_Colors::RED);
    }

    else if(prerequisitesOK && !this->_requestOngoing) {
        
        //start check
        this->_requestOngoing = true;
        emit updateSIOStatus(I18n::tr()->SIOAskingCredentialValidation(), TLW_Colors::YELLOW);
        
        sio::message::list p;
        auto username = this->_aHelper->getParamValue("username");
        auto password = this->_aHelper->getParamValue("password");

        p.push(sio::string_message::create(username.toStdString()));
        p.push(sio::string_message::create(password.toStdString()));
        this->_sioClient->socket("/login")->emit_socket("checkCredentials", p);
    }

}

QString ConnectivityThread::_getTargetUrl() {
    //extract destination url for sio connection
    auto t_qurl = this->_aHelper->getTargetUrl();
    t_qurl.setPort(SIO_PORT);
    auto turl = t_qurl.toString(QUrl::RemovePath);
    return turl;
}
