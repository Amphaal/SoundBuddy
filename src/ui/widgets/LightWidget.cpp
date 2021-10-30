// SoundBuddy
// Small companion app that feeds SoundVitrine with ITunes / Music library metadata
// Copyright (C) 2019-2021 Guillaume Vara <guillaume.vara@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Any graphical or audio resources available within the source code may
// use a different license and copyright : please refer to their metadata
// for further details. Resources without explicit references to a
// different license and copyright still refer to this GPL.

#include "LightWidget.h"

LightWidget::LightWidget(const QColor &color, QWidget* parent) : QWidget(parent), m_color(color), m_on(true) {
    this->setLayout(new QHBoxLayout);
}

bool LightWidget::isOn() const { return m_on; }

void LightWidget::setOn(bool on) {
    if (on == m_on)
        return;
    m_on = on;
    update();
}

void LightWidget::turnOff() { setOn(false); }
void LightWidget::turnOn() { setOn(true); }

void LightWidget::paintEvent(QPaintEvent *) {
    if (!m_on) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(m_color);

    auto height = this->height();

    painter.drawEllipse(
        qFloor(height * .25),
        qFloor(height * .25),
        qFloor(height * .75),
        qFloor(height * .75)
    );
}
