#pragma once

#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtCore/QList>

class LogScrollView : public QWidget {
    
    public:
       LogScrollView(QWidget *parent) : QWidget(parent) {
            this->setLayout(new QVBoxLayout);
            this->layout()->setAlignment(Qt::AlignTop);
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
            auto i = this->layout()->count() - 1;
            if(i < 0) return this->addMessage(newMessage);
            ((QLabel*)this->layout()->itemAt(i))->setText(msg);
        }
};