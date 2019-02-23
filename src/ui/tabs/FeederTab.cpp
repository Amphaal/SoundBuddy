#pragma once

#include <string>
#include <QtCore/QString>

#include "base/TemplateTab.h"
#include "../../workers/feeder.cpp"
#include "../../localization/i18n.cpp"

class FeederTab : public TemplateTab {
    public:
        FeederTab(QWidget *parent = 0, FeederWorker* worker = 0) : TemplateTab(parent) {

            this->tButton->setText(QString(I18n::tr()->Feeder_Button().c_str()));

            this->layout()->addWidget(this->scrollArea);
            this->layout()->addWidget(this->tButton);
        }
};