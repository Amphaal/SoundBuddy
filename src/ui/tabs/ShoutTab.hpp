// SoundBuddy
// Small companion app that feeds SoundVitrine with ITunes / Music library metadata
// Copyright (C) 2019-2023 Guillaume Vara <guillaume.vara@gmail.com>

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

#pragma once

#include <QCheckBox>
#include <QString>

#include "base/TemplateTab.h"
#include "src/workers/shout/ShoutWatcher.h"

#include "src/helpers/AppSettings.hpp"
#include "src/i18n/trad.hpp"

class ShoutTab : public TemplateTab {
    Q_OBJECT

 public:
    ShoutTab(QWidget* parent, AppSettings* appSettings) : TemplateTab(parent), _appSettings(appSettings) {
        this->checkAutoLaunch = new QCheckBox(tr("Start shouting at app launch"));
        this->tButton->setText(tr("Connect to %1").arg(musicAppName()));

        QObject::connect(
            this->checkAutoLaunch, &QCheckBox::checkStateChanged,
            this, &ShoutTab::changeAutoLaunch
        );

        this->layout()->addWidget(this->scrollArea);
        this->layout()->addWidget(this->tButton);
        this->layout()->addWidget(this->checkAutoLaunch);

        if (_appSettings->value(AppSettings::MUST_AUTORUN_SHOUT).toBool()) {
            this->checkAutoLaunch->setCheckState(Qt::CheckState::Checked);
        }
    }

 private:
    AppSettings* _appSettings;
    QCheckBox* checkAutoLaunch = nullptr;

    void changeAutoLaunch(Qt::CheckState checkState) {
        _appSettings->setValue(AppSettings::MUST_AUTORUN_SHOUT, checkState == Qt::CheckState::Checked);
    }
};
