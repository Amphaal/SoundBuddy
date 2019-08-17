#pragma once

#include <string>
#include <QtCore/QString>

#include "base/TemplateTab.h"
#include "src/workers/feeder/FeederThread.h"
#include "src/localization/i18n.cpp"

class FeederTab : public TemplateTab {
    public:
        FeederTab(QWidget *parent = 0, FeederThread* worker = 0) : TemplateTab(parent) {

            this->tButton->setText(QString(I18n::tr()->Feeder_Button().toUtf8()));

            this->layout()->addWidget(this->scrollArea);
            this->layout()->addWidget(this->tButton);
        }
};