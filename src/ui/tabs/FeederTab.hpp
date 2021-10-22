#pragma once

#include <QString>

#include "base/TemplateTab.h"
#include "src/workers/feeder/FeederThread.h"

class FeederTab : public TemplateTab {
 Q_OBJECT

 public:
    FeederTab(QWidget *parent = 0, FeederThread* worker = 0) : TemplateTab(parent) {

        this->tButton->setText(tr("Generate Digest and Upload"));

        this->layout()->addWidget(this->scrollArea);
        this->layout()->addWidget(this->tButton);
    }
};