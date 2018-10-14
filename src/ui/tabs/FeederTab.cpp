#include <string>
#include "QtCore/QString"

#include "base/TemplateTab.cpp"
#include "../../workers/feeder.cpp"
#include "../../localization/i18n.cpp"

class FeederTab : public TemplateTab {

    ITNZWorker* getWorkerThread() override {
        return new FeederWorker();
    }

    public:
        FeederTab(QWidget *parent) : TemplateTab(parent) {

            this->tButton->setText(QString(I18n::tr()->Feeder_Button()));

            this->mainLayout->addWidget(this->tEdit);
            this->mainLayout->addWidget(this->tButton);
        }
    
};