#pragma once

#include <string>
#include "QtCore/QString"

#include "base/TemplateTab.h"
#include "../../workers/feeder.cpp"
#include "../../localization/i18n.cpp"

class FeederTab : public TemplateTab {

    ITNZWorker* getWorkerThread() override {
        return new FeederWorker();
    }

    public:
        FeederTab(QWidget *parent) : TemplateTab(parent) {

            this->tButton->setText(QString(I18n::tr()->Feeder_Button().c_str()));

            this->mainLayout->addWidget(this->tEdit);
            this->mainLayout->addWidget(this->tButton);
        }
    
};