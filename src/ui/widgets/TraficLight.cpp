#pragma once

#include <QtWidgets>
#include "./LightWidget.h"

class TrafficLightWidget : public QWidget {
    public:
        TrafficLightWidget(QWidget *parent = 0)
            : QWidget(parent)
        {
            auto sbox = new QStackedLayout(this);
            m_red = new LightWidget(Qt::red);
            sbox->addWidget(m_red);
            m_yellow = new LightWidget(Qt::yellow);
            sbox->addWidget(m_yellow);
            m_green = new LightWidget(Qt::green);
            sbox->addWidget(m_green);
            QPalette pal = palette();
            pal.setColor(QPalette::Background, Qt::black);
            setPalette(pal);
            setAutoFillBackground(true);
            this->setLayout(sbox);
        }

        LightWidget *redLight() const
            { return m_red; }
        LightWidget *yellowLight() const
            { return m_yellow; }
        LightWidget *greenLight() const
            { return m_green; }

    private:
        LightWidget *m_red;
        LightWidget *m_yellow;
        LightWidget *m_green;
};