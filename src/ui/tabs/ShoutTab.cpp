#include <string>
#include "QtWidgets/QCheckBox"
#include "QtCore/QString"
#include <Qt>

#include "nlohmann/json.hpp"

#include "../../helpers/configHelper.cpp"
#include "TemplateTab.cpp"

class ShoutTab : public TemplateTab {
    
    const std::string autoLaunchConfigParam = "autoLaunchShout";
    
    public:
        ShoutTab(QWidget *parent, ConfigHelper *helper, nlohmann::json config) : 
        TemplateTab(parent), helper(helper), config(config),
        checkAutoLaunch(new QCheckBox("Autostart at launch", this))
          {
            
            this->tButton->setText(QString("Connect to iTunes"));

            connect(this->checkAutoLaunch, &QCheckBox::stateChanged,
            this, &ShoutTab::changeAutoLaunch);

            this->mainLayout->addWidget(this->messages);
            this->mainLayout->addWidget(this->tButton);
            this->mainLayout->addWidget(this->checkAutoLaunch);

            if (this->config[this->autoLaunchConfigParam] == "true") {
                this->checkAutoLaunch->setCheckState(Qt::CheckState::Checked);
                this->startThread();
            }
        }

    private:
        ConfigHelper *helper;
        nlohmann::json config;
        QPushButton *button;
        QCheckBox *checkAutoLaunch;

        void startThread() {
            this->messages->setPlainText("");
            this->tButton->setEnabled(false);
        }
    
        void onThreadEnd() {
            this->tButton->setEnabled(true);
        }

        void changeAutoLaunch(bool isChecked) {
            this->helper->updateConfigFile(this->autoLaunchConfigParam, isChecked ? "true" : "false");
        }
};