#pragma once

#include <QStackedWidget>
#include "./LightWidget.h"

enum TLW_Colors {
    RED = 0,
    YELLOW = 1,
    GREEN = 2
};

class TrafficLightWidget : public QStackedWidget {
    public:
        TrafficLightWidget(QWidget *parent = 0)
            : QStackedWidget(parent)
        {
            this->addWidget(new LightWidget(Qt::red));
            this->addWidget(new LightWidget(Qt::yellow));
            this->addWidget(new LightWidget(Qt::green));
        }
};

