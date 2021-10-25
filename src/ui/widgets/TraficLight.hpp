#pragma once

#include <QStackedWidget>
#include "./LightWidget.h"

class TrafficLightWidget : public QStackedWidget {
 public:
    explicit TrafficLightWidget(QWidget *parent = nullptr) : QStackedWidget(parent) {
        this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        this->addWidget(new LightWidget(Qt::red));
        this->addWidget(new LightWidget(Qt::yellow));
        this->addWidget(new LightWidget(Qt::green));
    }
};

