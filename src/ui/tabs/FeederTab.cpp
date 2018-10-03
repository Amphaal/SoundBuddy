#include <string>
#include "QtCore/QString"

#include "TemplateTab.cpp"

class FeederTab : public TemplateTab {
    public:
        FeederTab(QWidget *parent) : TemplateTab(parent) {

            this->tButton->setText(QString("Generate Digest and Upload"));

            this->mainLayout->addWidget(this->messages);
            this->mainLayout->addWidget(this->tButton);
        }

    private:
    
};