#pragma once

#include <string>
#include <QtWidgets/QCheckBox>
#include <QtCore/QString>
#include <Qt>

#include <rapidjson/document.h>

#include "src/helpers/configHelper/configHelper.hpp"
#include "./base/TemplateTab.h"
#include "src/workers/shout/ShoutThread.h"
#include "src/helpers/_const.hpp"

class ShoutTab : public TemplateTab {
 Q_OBJECT

 public:
    ShoutTab(QWidget *parent = 0) : TemplateTab(parent), cHelper(new ConfigHelper)
        {
        this->checkAutoLaunch = new QCheckBox(tr("Autostart at launch"));
        this->tButton->setText(tr("Connect to iTunes"));

        QObject::connect(this->checkAutoLaunch, &QCheckBox::stateChanged,
        this, &ShoutTab::changeAutoLaunch);

        this->layout()->addWidget(this->scrollArea);
        this->layout()->addWidget(this->tButton);
        this->layout()->addWidget(this->checkAutoLaunch);

        if (this->cHelper->getParamValue(AUTO_RUN_SHOUT_PARAM_NAME) == "true") {
            this->checkAutoLaunch->setCheckState(Qt::CheckState::Checked);
        }
    }

 private:
    ConfigHelper *cHelper = nullptr;
    QCheckBox *checkAutoLaunch = nullptr;

    void changeAutoLaunch(bool isChecked) {
        this->cHelper->updateParamValue(AUTO_RUN_SHOUT_PARAM_NAME, isChecked ? "true" : "false");
    }
};