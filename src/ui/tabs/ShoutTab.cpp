#include <string>
#include "QtWidgets/QPushButton"
#include "QtWidgets/QCheckBox"
#include "QtCore/QThread"

#include "../../helpers/configHelper.cpp"

#include "TemplateTab.cpp"

class ShoutTab : public TemplateTab {
    
    const std::string autoLaunchConfigParam = "autoLaunchShout";
    
    public:
        ShoutTab(QWidget *parent, ConfigHelper *helper) : 
        TemplateTab(parent), helper(helper), 
        button(new QPushButton("Connect to iTunes", this)),
        checkAutoLaunch(new QCheckBox("Autostart at launch", this))
          {
            
            connect(this->button, &QPushButton::clicked,
                    this, &ShoutTab::startThread);

            this->mainLayout->addWidget(this->messages);
            this->mainLayout->addWidget(this->button);
            this->mainLayout->addWidget(this->checkAutoLaunch);
        }

    private:
        ConfigHelper *helper;
        QPushButton *button;
        QCheckBox *checkAutoLaunch;

        void startThread() {
            
        }
    
        void onThreadEnd() {
            this->button->setEnabled(true);
        }
};