#include <string>
#include "QtWidgets/QPushButton"
#include "QtCore/QThread"

#include "TemplateTab.cpp"

class FeederTab : public TemplateTab {
    public:
        FeederTab(QWidget *parent) : TemplateTab(parent), 
        button(new QPushButton("Generate Digest and Upload", this))  {
            connect(this->button, &QPushButton::clicked,
                    this, &FeederTab::startThread);

            this->mainLayout->addWidget(this->messages);
            this->mainLayout->addWidget(this->button);
        }

    private:
        QPushButton *button;
        
        void startThread() {
            
        }

        void onThreadEnd() {
            this->button->setEnabled(true);
        }
    
};