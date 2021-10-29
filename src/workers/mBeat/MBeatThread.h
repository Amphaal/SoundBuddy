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
    void updateConnectivityStatus(const QString &message, const TLW_Colors &color);

 private:
    const AppSettings::ConnectivityInfos _connectivityInfos;
    
    const QString _validationErrorTr(const QString& returnCode) const;
    void _emitLoggedUserMsg();
};
