#pragma once

#include <QLabel>
#include <QVBoxLayout>
#include <QList>
#include "src/helpers/_const.hpp"

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

        void addMessage(const QString & newMessage, const bool isError = false) {
            auto label = new QLabel(newMessage);
            label->setWordWrap(true);
            
            QPalette palette = label->palette();
            auto color = isError ? Qt::red : Qt::black;
            palette.setColor(label->foregroundRole(), color);
            label->setPalette(palette);

            this->limitLogSize();

            this->layout()->addWidget(label);
        }

        void updateLatestMessage(const QString & newMessage) {
            
            auto i = this->layout()->count() - 1; //count items in layout
            
            //if no message, add message
            if(i < 0) {
                return this->addMessage(newMessage);
            }
            auto lbl = (QLabel*)this->layout()->itemAt(i)->widget();
            lbl->setText(newMessage);
        }
};