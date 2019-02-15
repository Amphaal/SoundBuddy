#define _HAS_STD_BYTE 0 //prevent build error on mac (byte type overriding)

#include "TemplateTab.h"
#include "../../../ui/mainWindow.h"

ITNZWorker* TemplateTab::getWorkerThread() { 
    return nullptr; 
};

TemplateTab::TemplateTab(QWidget *parent) : QWidget(parent), 
    mainLayout(new QBoxLayout(QBoxLayout::TopToBottom, this)),
    tEdit(new QPlainTextEdit(this)),
    tButton(new QPushButton(this)) {
        
        this->tEdit->setReadOnly(true);

        QObject::connect(this->tButton, &QPushButton::clicked,
                this, &TemplateTab::startThread);
};

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
    this->tEdit->setPlainText("");
    this->tEdit->setPalette(this->style()->standardPalette());
    
    try {
        
        this->bThread = this->getWorkerThread();
        QObject::connect(this->bThread, &QThread::finished,
                this, &TemplateTab::onThreadEnd);
        QObject::connect(this->bThread, &ITNZWorker::printLog,
                this, &TemplateTab::printLog);
        QObject::connect(this->bThread, &ITNZWorker::operationFinished,
                this, &TemplateTab::onOperationFinished);
        QObject::connect(this->bThread, &ITNZWorker::error,
                this, &TemplateTab::colorSwap);

        this->bThread->start();
        this->workerHasRunOnce = true;
    
    } catch (const std::exception& e) {
        this->printLog(e.what());
        this->colorSwap();
        this->onThreadEnd();
    }
};

void TemplateTab::printLog(const std::string &message, bool replacePreviousLine) {
    
    //handle linefeeds in appending
    std::string messages = this->tEdit->toPlainText().toStdString();
    if (replacePreviousLine) messages = StringHelper::splitPath(messages, "\n");
    if(messages != "") {
        messages += '\r';
    }
    messages += message;

    //update the Text Edit
    this->tEdit->setPlainText(QString::fromStdString(messages));

    //check if main is hidden to perform heavy CPU consuming action
    if (!this->getMainWindow()->isHidden()) {
        QScrollBar *tabScrollBar = this->tEdit->verticalScrollBar();
        tabScrollBar->setValue(tabScrollBar->maximum());
    }
};

void TemplateTab::colorSwap() {
    QPalette p = this->tEdit->palette();
    p.setColor(QPalette::Active, QPalette::Text, Qt::red);
    p.setColor(QPalette::Inactive, QPalette::Text, Qt::red);

    this->tEdit->setPalette(p);
};

void TemplateTab::onOperationFinished(size_t warningsCount) {
    this->getMainWindow()->informWarningPresence();
};

void TemplateTab::onThreadEnd() {
    this->tButton->setEnabled(true);
    delete this->bThread;
};
