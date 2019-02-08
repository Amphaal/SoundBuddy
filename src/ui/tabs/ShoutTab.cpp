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
        ShoutTab(QWidget *parent, ConfigHelper *cHelper) : TemplateTab(parent), 
        cHelper(cHelper),
        checkAutoLaunch(new QCheckBox(I18n::tr()->Shout_Autolaunch().c_str(), this))
          {
            
            this->tButton->setText(QString(I18n::tr()->Shout_Button().c_str()));

            QObject::connect(this->checkAutoLaunch, &QCheckBox::stateChanged,
            this, &ShoutTab::changeAutoLaunch);

            this->mainLayout->addWidget(this->tEdit);
            this->mainLayout->addWidget(this->tButton);
            this->mainLayout->addWidget(this->checkAutoLaunch);

            auto config = this->cHelper->accessConfig();
            if (this->cHelper->getParamValue(config, this->autoLaunchConfigParam) == "true") {
                this->checkAutoLaunch->setCheckState(Qt::CheckState::Checked);
                this->startThread();
            }
        }

    private:
        ConfigHelper *cHelper;
        QCheckBox *checkAutoLaunch;

        void changeAutoLaunch(bool isChecked) {
            this->cHelper->updateParamValue(this->autoLaunchConfigParam, isChecked ? "true" : "false");
        }
};