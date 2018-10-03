#pragma once
#include "QtCore/QObject"

class ITNZWorker : public QObject {

    Q_OBJECT

    public:
        virtual void run();
        ITNZWorker() {}

    signals:
        void printLog(const std::string &message);
};