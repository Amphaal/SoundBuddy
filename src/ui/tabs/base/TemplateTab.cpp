#define _HAS_STD_BYTE 0 //prevent build error on mac (byte type overriding)

#include "TemplateTab.h"
#include "../../../ui/mainWindow.h"

ITNZWorker* TemplateTab::getWorkerThread() { 
    return nullptr; 
};

TemplateTab::TemplateTab(QWidget *parent) : QWidget(parent),
    scrollArea(new QScrollArea(this)),
    tButton(new QPushButton(this)) {
        
        this->setLayout(new QBoxLayout(QBoxLayout::TopToBottom, this));
        this->scrollArea->setWidgetResizable(true);

        QObject::connect(this->tButton, &QPushButton::clicked,
                this, &TemplateTab::startThread);
};

void TemplateTab::createNewLog() {
    if(this->lsv != nullptr) delete this->lsv;
    this->lsv = new LogScrollView(nullptr);
    this->scrollArea->setWidget(this->lsv);
}

bool TemplateTab::isWorkerRunning() {
    if (this->bThread == NULL || !this->workerHasRunOnce) return false; //handle bug with QThread::isRunning when has not been run even once 
    return this->bThread->isRunning();
};

void TemplateTab::endThread() {
    this->bThread->exit();
    this->bThread->wait();
};

MainWindow* TemplateTab::getMainWindow() {
    return (MainWindow*)this->parent()->parent()->parent();
};

void TemplateTab::startThread() {

    this->tButton->setEnabled(false);
    this->createNewLog();
    
    try {
        
        this->bThread = this->getWorkerThread();
        QObject::connect(this->bThread, &QThread::finished,
                this, &TemplateTab::onThreadEnd);
        QObject::connect(this->bThread, &ITNZWorker::printLog,
                this, &TemplateTab::printLog);
        QObject::connect(this->bThread, &ITNZWorker::operationFinished,
                this, &TemplateTab::onOperationFinished);

        this->bThread->start();
        this->workerHasRunOnce = true;
    
    } catch (const std::exception& e) {
        this->printLog(e.what(), false, true);
        this->onThreadEnd();
    }
};

void TemplateTab::printLog(const std::string &message, const bool replacePreviousLine, const bool isError) {
    
    if(replacePreviousLine) {
        this->lsv->updateLatestMessage(message);
    } else {
        this->lsv->addMessage(message, isError);
    }

    //check if main is hidden to perform heavy CPU consuming action
    if (!this->getMainWindow()->isHidden()) {
        auto tabScrollBar = this->scrollArea->verticalScrollBar();
        tabScrollBar->setValue(tabScrollBar->maximum());
    }
};

void TemplateTab::onOperationFinished(size_t warningsCount) {
    this->getMainWindow()->informWarningPresence();
};

void TemplateTab::onThreadEnd() {
    this->tButton->setEnabled(true);
    if(this->workerHasRunOnce) delete this->bThread;
};
