#pragma once

#include <string>
#include <QtWidgets/QCheckBox>
#include <QtCore/QString>
#include <Qt>

#include <rapidjson/document.h>

#include "../../helpers/configHelper.cpp"
#include "base/TemplateTab.h"
#include "../../workers/shout/shout.h"

class ShoutTab : public TemplateTab {
    
    const std::string autoLaunchConfigParam = "autoLaunchShout";
    
    ITNZWorker* getWorkerThread() override {
        return new ShoutWorker();
    }
    
    public:
        ShoutTab(QWidget *parent, ConfigHelper *helper) : TemplateTab(parent), 
        helper(helper),
        checkAutoLaunch(new QCheckBox(I18n::tr()->Shout_Autolaunch().c_str(), this))
          {
            
            this->tButton->setText(QString(I18n::tr()->Shout_Button().c_str()));

            QObject::connect(this->checkAutoLaunch, &QCheckBox::stateChanged,
            this, &ShoutTab::changeAutoLaunch);

            this->mainLayout->addWidget(this->tEdit);
            this->mainLayout->addWidget(this->tButton);
            this->mainLayout->addWidget(this->checkAutoLaunch);

            if (this->helper->accessConfig()[this->autoLaunchConfigParam.c_str()] == "true") {
                this->checkAutoLaunch->setCheckState(Qt::CheckState::Checked);
                this->startThread();
            }
        }

    private:
        ConfigHelper *helper;
        QCheckBox *checkAutoLaunch;

        void changeAutoLaunch(bool isChecked) {
            this->helper->updateConfigFile(this->autoLaunchConfigParam, isChecked ? "true" : "false");
        }
};