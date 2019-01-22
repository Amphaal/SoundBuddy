#pragma once

#include <string>
#include "QtWidgets/QCheckBox"
#include "QtCore/QString"
#include <Qt>

#include "nlohmann/json.hpp"

#include "../../helpers/configHelper.cpp"
#include "base/TemplateTab.h"
#include "../../workers/shout/shout.h"

class ShoutTab : public TemplateTab {
    
    const std::string autoLaunchConfigParam = "autoLaunchShout";
    
    ITNZWorker* getWorkerThread() override {
        return new ShoutWorker();
    }
    
    public:
        ShoutTab(QWidget *parent, ConfigHelper *helper, nlohmann::json config) : TemplateTab(parent), 
        helper(helper), config(config),
        checkAutoLaunch(new QCheckBox(I18n::tr()->Shout_Autolaunch().c_str(), this))
          {
            
            this->tButton->setText(QString(I18n::tr()->Shout_Button().c_str()));

            QObject::connect(this->checkAutoLaunch, &QCheckBox::stateChanged,
            this, &ShoutTab::changeAutoLaunch);

            this->mainLayout->addWidget(this->tEdit);
            this->mainLayout->addWidget(this->tButton);
            this->mainLayout->addWidget(this->checkAutoLaunch);

            if (this->config[this->autoLaunchConfigParam] == "true") {
                this->checkAutoLaunch->setCheckState(Qt::CheckState::Checked);
                this->startThread();
            }
        }

    private:
        ConfigHelper *helper;
        nlohmann::json config;
        QCheckBox *checkAutoLaunch;

        void changeAutoLaunch(bool isChecked) {
            this->helper->updateConfigFile(this->autoLaunchConfigParam, isChecked ? "true" : "false");
        }
};