#pragma once

#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtCore/QList>
#include "src/helpers/_const.cpp"

class LogScrollView : public QWidget {
    
    private:
        int _maxLogMessages = MAX_LOG_MESSAGES;

        void limitLogSize() {
            if(this->layout()->count() > this->_maxLogMessages) {
                auto qli = this->layout()->takeAt(0);
                auto ltr = (QLabel*)qli->widget();
                ltr->setParent(nullptr);
                delete ltr;
                delete qli;
            }
        }

    public:
    
       LogScrollView(QWidget *parent) : QWidget(parent) {
            this->setLayout(new QVBoxLayout);
            this->layout()->setAlignment(Qt::AlignTop);

            //set background color
            QPalette palette = this->palette();
            palette.setColor(this->backgroundRole(), Qt::white);
            this->setPalette(palette);
        }

        void addMessage(const std::string & newMessage, const bool isError = false) {
            auto msg = QString::fromStdString(newMessage);
            auto label = new QLabel(msg);
            label->setWordWrap(true);
            
            QPalette palette = label->palette();
            auto color = isError ? Qt::red : Qt::black;
            palette.setColor(label->foregroundRole(), color);
            label->setPalette(palette);

            this->limitLogSize();

            this->layout()->addWidget(label);
        }

        void updateLatestMessage(const std::string & newMessage) {
            
            auto msg = QString::fromStdString(newMessage);
            auto i = this->layout()->count() - 1; //count items in layout
            
            //if no message, add message
            if(i < 0) {
                return this->addMessage(newMessage);
            }
            auto lbl = (QLabel*)this->layout()->itemAt(i)->widget();
            lbl->setText(msg);
        }
};