#define _HAS_STD_BYTE 0 //prevent build error on mac (byte type overriding)

#include "TemplateTab.h"

TemplateTab::TemplateTab(QWidget *parent) : QWidget(parent),
    tButton(new QPushButton(this)),
    scrollArea(new QScrollArea(this)) {
        
        QObject::connect(
            this->scrollArea->verticalScrollBar(), &QScrollBar::rangeChanged,
            this, &TemplateTab::scrollUpdate
        );

        this->setLayout(new QBoxLayout(QBoxLayout::TopToBottom, this));
        this->scrollArea->setWidgetResizable(true);
};

void TemplateTab::createNewLog() {
    if(!this->lsv) delete this->lsv;
    this->lsv = new LogScrollView(nullptr);
    this->scrollArea->setWidget(this->lsv);
};

void TemplateTab::printLog(const std::string &message, const bool replacePreviousLine, const bool isError) {
    
    if(replacePreviousLine) {
        this->lsv->updateLatestMessage(message);
    } else {
        this->lsv->addMessage(message, isError);
    }
};

void TemplateTab::scrollUpdate(int min, int max) {
    //to perform heavy CPU consuming action
    auto tabScrollBar = this->scrollArea->verticalScrollBar();
    tabScrollBar->setValue(tabScrollBar->maximum());
}

void TemplateTab::onThreadEnd() {
    this->tButton->setEnabled(true);
};

void TemplateTab::onThreadStart() {
    this->tButton->setEnabled(false);
    this->createNewLog();
};

void TemplateTab::bindWithWorker(ITNZWorker *bThread) {
    QObject::connect(bThread, &ITNZWorker::printLog,
            this, &TemplateTab::printLog);
    QObject::connect(bThread, &QThread::started,
            this, &TemplateTab::onThreadStart);
    QObject::connect(bThread, &QThread::finished,
            this, &TemplateTab::onThreadEnd);
}
