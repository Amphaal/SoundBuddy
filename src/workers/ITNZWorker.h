#pragma once
#include "QtCore/QObject"
#include "QtWidgets/QWidget"

class ITNZWorker : public QObject {

    Q_OBJECT
    
    public:
        explicit ITNZWorker(QWidget *parent);
        virtual void run();

    signals:
        void printLog(std::string message);
};