#pragma once

#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtCore/QList>

class LogScrollView : public QWidget {
    
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
            auto c = this->layout()->count();
            auto msg = QString::fromStdString(newMessage);
            auto label = new QLabel(msg);
            label->setWordWrap(true);
            
            if(isError) {
                QPalette palette = label->palette();
                palette.setColor(label->foregroundRole(), Qt::red);
                label->setPalette(palette);
            }

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