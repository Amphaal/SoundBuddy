#pragma once

#include "QtCore/QThread"

class ITNZThread : public QThread {
    
    Q_OBJECT
        
    signals:
        void printLog(const std::string &message, bool replacePreviousLine = false);
        void error();
};