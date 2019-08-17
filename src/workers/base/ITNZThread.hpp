#pragma once

#include <QThread>

class ITNZThread : public QThread {
    
    Q_OBJECT

    public:
        virtual void quit() { };
        virtual void run() = 0;

    signals:
        void printLog(const QString &message, const bool replacePreviousLine = false, const bool isError = false);
        void operationFinished();
        
};