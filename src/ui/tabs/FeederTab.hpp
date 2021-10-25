#pragma once

#include <QString>

#include "base/TemplateTab.h"
#include "src/workers/feeder/FeederThread.h"

class FeederTab : public TemplateTab {
    Q_OBJECT

 public:
    FeederTab(QWidget *parent = nullptr, FeederThread* worker = nullptr) : TemplateTab(parent) {
        this->tButton->setText(tr("Generate Digest and Upload"));

        this->layout()->addWidget(this->scrollArea);
        this->layout()->addWidget(this->tButton);
    }
};
