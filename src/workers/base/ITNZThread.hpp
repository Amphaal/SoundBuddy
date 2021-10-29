#pragma once

#include <QThread>

#include "src/helpers/AppSettings.hpp"
#include "src/helpers/UploadHelper.hpp"

class ITNZThread : public QThread {
    Q_OBJECT

 public:
    ITNZThread(const UploadHelper* uploder, const AppSettings::ConnectivityInfos connectivityInfos) :
        _uploder(uploder), _connectivityInfos(connectivityInfos) {}

 protected:
    const AppSettings::ConnectivityInfos _connectivityInfos;
    const UploadHelper* _uploder;

 signals:
    void printLog(const QString &message, const bool replacePreviousLine = false, const bool isError = false);
};
