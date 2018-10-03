#pragma once

#include "QtCore/QString"
#include "QtCore/QThread"
#include "QtWidgets/QWidget"
#include "QtWidgets/QPlainTextEdit"
#include "QtWidgets/QBoxLayout"
#include "QtWidgets/QPushButton"
#include "QtWidgets/QScrollBar"

#include "../../workers/ITNZWorker.cpp"

using namespace std;

class TemplateTab : public QWidget {

    public:
        TemplateTab(QWidget *parent, ITNZWorker *bWorker) : QWidget(parent), 
        bWorker(bWorker),
        mainLayout(new QBoxLayout(QBoxLayout::TopToBottom, this)),
        messages(new QPlainTextEdit(this)),
        tButton(new QPushButton(this)) {
            this->messages->setReadOnly(true);

            connect(this->tButton, &QPushButton::clicked,
                    this, &TemplateTab::startThread);
        }

    protected:
        QThread *bThread;
        ITNZWorker *bWorker;
        QBoxLayout *mainLayout;
        QPlainTextEdit *messages;
        QPushButton *tButton;
        
        void startThread() {
            this->messages->setPlainText("");
            this->tButton->setEnabled(false);
            
            this->bThread = NULL;
            this->bThread = new QThread(this);
            
            connect(this->bThread, &QThread::finished,
                    this, &TemplateTab::onThreadEnd);
            connect(this->bWorker, &ITNZWorker::printLog,
                    this, &TemplateTab::printLog);
            this->bWorker->moveToThread(this->bThread);                
            this->bThread->start();
        }

        void printLog(string &message) {
            
            //handle linefeeds in appending
            QString before = this->messages->toPlainText();
            if(before != "") {
                before.append('\r');
            }
            before.append(QString::fromStdString(message));

            //update the Text Edit
            this->messages->setPlainText(before);

            //check if main is hidden to perform heavy CPU consuming action
            QWidget *mainWindow = (QWidget*)this->parent()->parent()->parent();
            if (!mainWindow->isHidden()) {
                QScrollBar *tabScrollBar = this->messages->verticalScrollBar();
                tabScrollBar->setValue(tabScrollBar->maximum());
            }
        }
        
        void onThreadEnd() {
            this->tButton->setEnabled(true);
        }
};