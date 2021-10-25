#pragma once

#include <QCheckBox>
#include <QString>

#include <rapidjson/document.h>

#include <string>

#include "src/helpers/configHelper/configHelper.hpp"
#include "./base/TemplateTab.h"
#include "src/workers/shout/ShoutThread.h"

#include "src/helpers/AppSettings.hpp"

class ShoutTab : public TemplateTab {
    Q_OBJECT

 public:
    ShoutTab(QWidget *parent, QSettings* appSettings) : TemplateTab(parent), _appSettings(appSettings) {
        this->checkAutoLaunch = new QCheckBox(tr("Autostart at launch"));
        this->tButton->setText(tr("Connect to iTunes"));

        QObject::connect(
            this->checkAutoLaunch, &QCheckBox::stateChanged,
            this, &ShoutTab::changeAutoLaunch
        );

        this->layout()->addWidget(this->scrollArea);
        this->layout()->addWidget(this->tButton);
        this->layout()->addWidget(this->checkAutoLaunch);

        if (_appSettings->value(MUST_AUTORUN_SHOUT).toBool()) {
            this->checkAutoLaunch->setCheckState(Qt::CheckState::Checked);
        }
    }

 private:
    QSettings* _appSettings = nullptr;
    QCheckBox* checkAutoLaunch = nullptr;

    void changeAutoLaunch(bool isChecked) {
        _appSettings->setValue(MUST_AUTORUN_SHOUT, isChecked);
    }
};
