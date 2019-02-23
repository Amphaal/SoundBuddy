#pragma once

#include <QtCore/QThread>

class ITNZThread : public QThread {
    
    Q_OBJECT
        
    signals:
        void printLog(const std::string &message, const bool replacePreviousLine = false, const bool isError = false);
        void operationFinished();
};