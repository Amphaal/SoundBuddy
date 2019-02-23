#pragma once

#include <QtCore/QThread>
#include "src/ui/widgets/LightWidget.h"

class IConnectivityWorker : public QThread {
    
    Q_OBJECT
        
    signals:
        void updateSIOStatus(const std::string &message, const TLW_Colors &color);
};