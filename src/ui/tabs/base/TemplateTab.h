#pragma once

#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtWidgets/QWidget>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QStyle>

#include "../../../workers/base/ITNZWorker.h"
#include "../../../helpers/stringHelper.cpp"

using namespace std;

class MainWindow; //Forward Declaration 

class TemplateTab : public QWidget {

    virtual ITNZWorker* getWorkerThread();

    public:
        TemplateTab(QWidget *parent);
        bool isWorkerRunning();
        void endThread();

    protected:
        ITNZWorker *bThread;
        QBoxLayout *mainLayout;
        QPlainTextEdit *tEdit;
        QPushButton *tButton;
        bool workerHasRunOnce = false;
        
        MainWindow* getMainWindow();

        void startThread();
        void printLog(const std::string &message, bool replacePreviousLine = false);
        void colorSwap();

        void onOperationFinished(size_t warningsCount);
        void onThreadEnd();
};