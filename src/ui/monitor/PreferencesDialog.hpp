#pragma once

#include <QDialog>
#include <QPushButton>
#include <QGroupBox>
#include <QLineEdit>
#include <QGridLayout>
#include <QCheckBox>

#include "src/helpers/platformHelper/platformHelper.h"
#include "src/helpers/AppSettings.hpp"

class PreferencesDialog : public QDialog {
    Q_OBJECT

 public:
    explicit PreferencesDialog(QSettings* appSettings, QWidget* parent = nullptr) : QDialog(parent), _appSettings(appSettings) {
        this->setWindowTitle(tr("Preferences"));
        this->setModal(true);
        auto layout = new QGridLayout;
        this->setLayout(layout);

        // Platform connectivity
        auto connectivityGroup = new QGroupBox(tr("Connectivity"), this);

            this->_PlatformURLEdit = new QLineEdit(tr("%1 Host URL").arg(DEST_PLATFORM_PRODUCT_NAME), this);
            this->_PlatformUsernameEdit = new QLineEdit(tr("%1 username").arg(DEST_PLATFORM_PRODUCT_NAME), this);
            this->_PlatformPasswordEdit = new QLineEdit(tr("%1 user password").arg(DEST_PLATFORM_PRODUCT_NAME), this);
            this->_PlatformPasswordEdit->setEchoMode(QLineEdit::Password);

            this->_PlatformURLEdit->setText(this->_appSettings->value(AppSettings::PLATFORM_HOST_URL).toString());
            this->_PlatformUsernameEdit->setText(this->_appSettings->value(AppSettings::PLATFORM_USERNAME).toString());
            this->_PlatformPasswordEdit->setText(this->_appSettings->value(AppSettings::PLATFORM_PASSWORD).toString());

            connectivityGroup->layout()->addWidget(this->_PlatformURLEdit);
            connectivityGroup->layout()->addWidget(this->_PlatformPasswordEdit);
            connectivityGroup->layout()->addWidget(this->_PlatformPasswordEdit);

        layout->addWidget(connectivityGroup);

        // Automation
        auto automationGroup = new QGroupBox(tr("Automation"), this);

            this->_launchAppAtStartupChk = new QCheckBox(tr("Launch %1 at system boot").arg(APP_NAME), this);

            this->_launchAppAtStartupChk->setChecked(this->_appSettings->value(AppSettings::MUST_RUN_AT_STARTUP).toBool());

            automationGroup->layout()->addWidget(this->_launchAppAtStartupChk);

            if (PlatformHelper::isLaunchingAtStartup()) {
                this->_launchAppAtStartupChk->setChecked(true);
            }

            QObject::connect(
                this->_launchAppAtStartupChk, &QCheckBox::stateChanged,
                this, &_mightAddAppToStartup
            );

        layout->addWidget(automationGroup);
    }

 private:
    QSettings* _appSettings;

    QLineEdit* _PlatformURLEdit;
    QLineEdit* _PlatformUsernameEdit;
    QLineEdit* _PlatformPasswordEdit;

    QCheckBox* _launchAppAtStartupChk;

    void _mightAddAppToStartup() {
        PlatformHelper::switchStartupLaunch();
        this->_appSettings->setValue(AppSettings::MUST_RUN_AT_STARTUP, this->_launchAppAtStartupChk->isChecked());
    }

    void closeEvent(QCloseEvent *event) override {
        this->_appSettings->setValue(AppSettings::PLATFORM_HOST_URL, this->_PlatformURLEdit->text());
        this->_appSettings->setValue(AppSettings::PLATFORM_USERNAME, this->_PlatformUsernameEdit->text());
        this->_appSettings->setValue(AppSettings::PLATFORM_PASSWORD, this->_PlatformPasswordEdit->text());
        this->_appSettings->sync();
    }
};
