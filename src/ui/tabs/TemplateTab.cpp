#pragma once

#include "QtCore/QString"
#include "QtCore/QThread"
#include "QtWidgets/QWidget"
#include "QtWidgets/QPlainTextEdit"
#include "QtWidgets/QBoxLayout"
#include "QtWidgets/QPushButton"
#include "QtWidgets/QScrollBar"

#include "../../workers/base/ITNZWorker.h"

using namespace std;

class TemplateTab : public QWidget {

    virtual ITNZWorker* getWorkerThread() { return nullptr; }

    public:
        TemplateTab(QWidget *parent) : QWidget(parent), 
        mainLayout(new QBoxLayout(QBoxLayout::TopToBottom, this)),
        tEdit(new QPlainTextEdit(this)),
        tButton(new QPushButton(this)) {
            this->tEdit->setReadOnly(true);

            connect(this->tButton, &QPushButton::clicked,
                    this, &TemplateTab::startThread);
        }

    protected:
        ITNZWorker *bThread;
        QBoxLayout *mainLayout;
        QPlainTextEdit *tEdit;
        QPushButton *tButton;
        
        void startThread() {

            this->tButton->setEnabled(false);
            this->tEdit->setPlainText("");

            this->bThread = this->getWorkerThread();
            connect(this->bThread, &QThread::finished,
                    this, &TemplateTab::onThreadEnd);
            connect(this->bThread, &ITNZWorker::printLog,
                    this, &TemplateTab::printLog);
            this->bThread->start();
        }

        void printLog(const std::string &message) {
            
            //handle linefeeds in appending
            std::string messages = this->tEdit->toPlainText().toStdString();
            if(messages != "") {
                messages += '\r';
            }
            messages += message;

            //update the Text Edit
            this->tEdit->setPlainText(QString::fromStdString(messages));

            //check if main is hidden to perform heavy CPU consuming action
            QWidget *mainWindow = (QWidget*)this->parent()->parent()->parent();
            if (!mainWindow->isHidden()) {
                QScrollBar *tabScrollBar = this->tEdit->verticalScrollBar();
                tabScrollBar->setValue(tabScrollBar->maximum());
            }
        }
        
        void onThreadEnd() {
           this->tButton->setEnabled(true);
        }
};