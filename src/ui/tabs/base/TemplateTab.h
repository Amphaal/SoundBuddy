#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QStyle>

#include "../../../workers/base/ITNZWorker.h"
#include "../../../helpers/stringHelper.cpp"
#include "LogScrollView.cpp"
#include <QtWidgets/QScrollArea>

class MainWindow; //Forward Declaration 

class TemplateTab : public QWidget {

    virtual ITNZWorker* getWorkerThread();

    public:
        TemplateTab(QWidget *parent);
        bool isWorkerRunning();
        void endThread();

    protected:
        ITNZWorker *bThread;
        LogScrollView *lsv;
        QScrollArea *scrollArea;
        QPushButton *tButton;
        bool workerHasRunOnce = false;
        
        MainWindow* getMainWindow();

        void createNewLog();
        void startThread();
        void printLog(const std::string &message, const bool replacePreviousLine = false, const bool isError = false);

        void onOperationFinished(size_t warningsCount);
        void onThreadEnd();
};