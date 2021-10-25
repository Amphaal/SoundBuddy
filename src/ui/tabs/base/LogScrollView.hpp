#pragma once

#include <QLabel>
#include <QVBoxLayout>
#include <QList>

class LogScrollView : public QWidget {
 private:
    static const int _maxLogMessages = 100;

    void limitLogSize() {
        if(this->layout()->count() > this->_maxLogMessages) {
            auto qli = this->layout()->takeAt(0);
            auto ltr = reinterpret_cast<QLabel*>(qli->widget());
            ltr->setParent(nullptr);
            delete ltr;
            delete qli;
        }
    }

 public:
    explicit LogScrollView(QWidget* parent) : QWidget(parent) {
        this->setLayout(new QVBoxLayout);
        this->layout()->setAlignment(Qt::AlignTop);

        // set background color
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
        // count items in layout
        auto i = this->layout()->count() - 1;

        // if no message, add message
        if(i < 0) {
            return this->addMessage(newMessage);
        }
        auto lbl = reinterpret_cast<QLabel*>(this->layout()->itemAt(i)->widget());
        lbl->setText(newMessage);
    }
};
