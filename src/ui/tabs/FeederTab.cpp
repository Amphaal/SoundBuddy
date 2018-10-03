#include <string>
#include "QtCore/QString"

#include "TemplateTab.cpp"
#include "../../workers/feeder.cpp"

class FeederTab : public TemplateTab {
    public:
        FeederTab(QWidget *parent) : TemplateTab(parent, new FeederWorker(this)) {

            this->tButton->setText(QString("Generate Digest and Upload"));

            this->mainLayout->addWidget(this->messages);
            this->mainLayout->addWidget(this->tButton);
        }

    private:
    
};