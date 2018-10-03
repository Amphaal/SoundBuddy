#pragma once

#include "QtCore/QString"
#include "QtCore/QThread"
#include "QtWidgets/QWidget"
#include "QtWidgets/QPlainTextEdit"
#include "QtWidgets/QBoxLayout"
#include "QtWidgets/QPushButton"

#include "../../tnzLib/IThread.cpp"

using namespace std;

class TemplateTab : public QWidget {

    public:
        TemplateTab(QWidget *parent) : QWidget(parent), 
        mainLayout(new QBoxLayout(QBoxLayout::TopToBottom, this)),
        tButton(new QPushButton(this)),
        messages(new QPlainTextEdit(this)) {
            this->messages->setReadOnly(true);

            connect(this->tButton, &QPushButton::clicked,
                    this, &TemplateTab::startThread);
        }

        void messageHandler(string &message) {
            
            //handle linefeeds in appending
            QString before = this->messages->toPlainText();
            if(before != "") {
                before.append('\r');
            }
            before.append(QString::fromStdString(message));

            //update the Text Edit
            this->messages->setPlainText(before);
        }

    protected:
        QPlainTextEdit *messages;
        QBoxLayout *mainLayout;
        QThread *bThread;
        QPushButton *tButton;
        
        void startThread() {
            this->messages->setPlainText("");
            this->tButton->setEnabled(false);
            
            //this->bThread = NULL;
        }

        void onThreadEnd() {
            this->tButton->setEnabled(true);
        }
};