#pragma once

#include <QCheckBox>
#include <QString>

#include <string>

#include "base/TemplateTab.h"
#include "src/workers/shout/ShoutThread.h"

#include "src/helpers/AppSettings.hpp"
#include "src/_i18n/trad.hpp"

class ShoutTab : public TemplateTab {
    Q_OBJECT

 public:
    ShoutTab(QWidget* parent, AppSettings* appSettings) : TemplateTab(parent), _appSettings(appSettings) {
        this->checkAutoLaunch = new QCheckBox(tr("Autostart at launch"));
        this->tButton->setText(tr("Connect to %1").arg(musicAppName()));

        QObject::connect(
            this->checkAutoLaunch, &QCheckBox::stateChanged,
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

    void changeAutoLaunch(bool isChecked) {
        _appSettings->setValue(AppSettings::MUST_AUTORUN_SHOUT, isChecked);
    }
};
