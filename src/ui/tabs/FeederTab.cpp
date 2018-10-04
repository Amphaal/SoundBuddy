#include <string>
#include "QtCore/QString"

#include "TemplateTab.cpp"
#include "../../workers/feeder.cpp"

class FeederTab : public TemplateTab {

    ITNZWorker* getWorkerThread() override {
        return new FeederWorker();
    }

    public:
        FeederTab(QWidget *parent) : TemplateTab(parent) {

            this->tButton->setText(QString("Generate Digest and Upload"));

            this->mainLayout->addWidget(this->tEdit);
            this->mainLayout->addWidget(this->tButton);
        }

    private:
    
};