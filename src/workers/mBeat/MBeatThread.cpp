#include "MBeatThread.h"

MBeatThread::MBeatThread(const AppSettings::ConnectivityInfos &connectivityInfos) : _connectivityInfos(connectivityInfos) {}

void MBeatThread::run() {
    ////////////////////
    // Event Handlers //
    ////////////////////

    this->_sioClient = new sio::client();

    // tell sio is trying to reconnect
    this->_sioClient->set_reconnect_listener([&](unsigned int a, unsigned int b) {
        emit updateSIOStatus(tr("Reconnecting to server..."), TLW_Colors::YELLOW);
    });

    // on connect, check credentials
    this->_sioClient->set_open_listener([&]() {
        this->_checkCredentials();
    });


    // once server checked the credentials
    this->_sioClient->socket("/login")->on("credentialsChecked", [&](sio::event& ev) {
        // extract response
        auto response = ev.get_messages()[0]->get_map();
        auto isOk = response["isLoginOk"]->get_bool();
        auto extraInfo = QString::fromStdString(response["accomp"]->get_string());

        if(isOk) {
            this->_loggedInUser = extraInfo;
            _emitLoggedUserMsg();
        } else {
            emit updateSIOStatus(_validationErrorTr(extraInfo), TLW_Colors::RED);
        }

        // toggle flag
        this->_requestOngoing = false;
    });

    // when server tell us the database has been updated, ask for revalidation
    this->_sioClient->socket("/login")->on("databaseUpdated", [&](sio::event& ev) {
        this->_checkCredentials(true);
    });

    ////////////////////////
    // End Event Handlers //
    ////////////////////////

    // declare waiting for connection
    emit updateSIOStatus(tr("Connecting to server..."), TLW_Colors::YELLOW);

    // connect...
    this->_sioClient->connect(this->_getPlatformHostUrl().toStdString());

    QObject::connect(
        this->_toWatchOverChanges, &QFileSystemWatcher::fileChanged,
        this, &MBeatThread::_checkCredentialsFromFileUpdate
    );

    this->exec();

    // clear
    delete this->_sioClient;
}

void MBeatThread::_emitLoggedUserMsg() {
    emit updateSIOStatus(tr("Logged as \"%1\"").arg(this->_loggedInUser), TLW_Colors::GREEN);
}

const QString MBeatThread::_validationErrorTr(const QString& errorCode) const {
    QString errorMsg;

    if(errorCode == "cdm") {
        errorMsg = tr("Credential data missing");
    } else if(errorCode == "eud") {
        errorMsg = tr("Empty users database");
    } else if(errorCode == "unfid") {
        errorMsg = tr("Username not found in database");
    } else if(errorCode == "nopass") {
        errorMsg = tr("Password for the user not found in database");
    } else if(errorCode == "pmiss") {
        errorMsg = tr("Password missmatch");
    } else {
        return tr("Unknown error from the validation request");
    }

    return tr("Server responded with : \"%1\"").arg(errorMsg);
}

void MBeatThread::_checkCredentialsFromFileUpdate() {
    this->_checkCredentials(true);
}

// ask credentials
void MBeatThread::_checkCredentials(bool forceRecheck) {
    //
    if(forceRecheck) {
        this->_loggedInUser = "";
        this->_requestOngoing = false;
    }

    if(this->_loggedInUser != "" && !forceRecheck) {
        _emitLoggedUserMsg();
        return;
    }

    auto prerequisitesOK = this->_aHelper->ensureConfigFileIsReadyForUpload(false);

    if (!prerequisitesOK) {
        emit updateSIOStatus(tr("Waiting for appropriate credentials."), TLW_Colors::RED);
    } else if (prerequisitesOK && !this->_requestOngoing) {
        // start check
        this->_requestOngoing = true;
        emit updateSIOStatus(tr("Asking for credentials validation..."), TLW_Colors::YELLOW);

        sio::message::list p;
        auto username = this->_aHelper->getParamValue("username");
        auto password = this->_aHelper->getParamValue("password");

        p.push(sio::string_message::create(username.toStdString()));
        p.push(sio::string_message::create(password.toStdString()));
        _sio_checkCredentials(this->_sioClient, p);
    }
}

QString MBeatThread::_getPlatformHostUrl() {
    // extract destination url for sio connection
    auto t_qurl = this->_aHelper->getPlatformHostUrl();
    t_qurl.setPort(3000);
    auto turl = t_qurl.toString(QUrl::RemovePath);
    return turl;
}

void _sio_checkCredentials(sio::client* sioClient, sio::message::list& p) {
   sioClient->socket("/login")->emit("checkCredentials", p);
}