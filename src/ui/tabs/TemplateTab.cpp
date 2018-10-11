#pragma once

#include "QtCore/QString"
#include "QtCore/QThread"
#include "QtWidgets/QWidget"
#include "QtWidgets/QPlainTextEdit"
#include "QtWidgets/QBoxLayout"
#include "QtWidgets/QPushButton"
#include "QtWidgets/QScrollBar"
#include <QStyle>

#include "../../workers/base/ITNZWorker.h"
#include "../../helpers/stringHelper.cpp"

using namespace std;

class TemplateTab : public QWidget {

    virtual ITNZWorker* getWorkerThread() { return nullptr; }

    public:
        TemplateTab(QWidget *parent) : QWidget(parent), 
        mainLayout(new QBoxLayout(QBoxLayout::TopToBottom, this)),
        tEdit(new QPlainTextEdit(this)),
        tButton(new QPushButton(this)) {
            this->tEdit->setReadOnly(true);

            QObject::connect(this->tButton, &QPushButton::clicked,
                    this, &TemplateTab::startThread);
        }

        bool isWorkerRunning() {
            return this->bThread->isRunning();
        }

        void endThread() {
            this->bThread->exit();
            this->bThread->wait();
        }

    protected:
        ITNZWorker *bThread;
        QBoxLayout *mainLayout;
        QPlainTextEdit *tEdit;
        QPushButton *tButton;
        
        void startThread() {

            this->tButton->setEnabled(false);
            this->tEdit->setPlainText("");
            this->tEdit->setPalette(this->style()->standardPalette());
            
            try {
                
                this->bThread = this->getWorkerThread();
                QObject::connect(this->bThread, &QThread::finished,
                        this, &TemplateTab::onThreadEnd);
                QObject::connect(this->bThread, &ITNZWorker::printLog,
                        this, &TemplateTab::printLog);
                QObject::connect(this->bThread, &ITNZWorker::error,
                        this, &TemplateTab::colorSwap);
                
                this->bThread->start();
            
            } catch (const std::exception& e) {
                this->printLog(e.what());
                this->colorSwap();
                this->onThreadEnd();
            }
        }

        void printLog(const std::string &message, bool replacePreviousLine = false) {
            
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
            QWidget *mainWindow = (QWidget*)this->parent()->parent()->parent();
            if (!mainWindow->isHidden()) {
                QScrollBar *tabScrollBar = this->tEdit->verticalScrollBar();
                tabScrollBar->setValue(tabScrollBar->maximum());
            }
        }
        
        void colorSwap() {
            QPalette p = this->tEdit->palette();
            p.setColor(QPalette::Active, QPalette::Base, Qt::red);
            p.setColor(QPalette::Inactive, QPalette::Base, Qt::red);

            this->tEdit->setPalette(p);
        }

        void onThreadEnd() {
           this->tButton->setEnabled(true);
        }
};