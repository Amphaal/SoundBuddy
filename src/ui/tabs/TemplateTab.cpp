#pragma once

#include "QtCore/QString"
#include "QtWidgets/QWidget"
#include "QtWidgets/QPlainTextEdit"
#include "QtWidgets/QBoxLayout"
#include "nlohmann/json.hpp"

using namespace std;

class TemplateTab : public QWidget {

    virtual void onThreadEnd() {};
    virtual void startThread() {};

    public:
        TemplateTab(QWidget *parent) : QWidget(parent), 
        mainLayout(new QBoxLayout(QBoxLayout::TopToBottom, this)),
        messages(new QPlainTextEdit(this)) {
            this->messages->setReadOnly(true);
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
};