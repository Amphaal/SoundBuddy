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
 public:
    explicit PreferencesDialog(QSettings* appSettings, QWidget* parent = nullptr) : QDialog(parent), _appSettings(appSettings) {
        this->setWindowTitle(tr("Preferences"));
        this->setModal(true);
        auto layout = new QGridLayout;
        this->setLayout(layout);

        // WTNZ connectivity
        auto connectivityGroup = new QGroupBox(tr("Connectivity"), this);

            this->_WTNZURLEdit = new QLineEdit(tr("WTNZ Profile URL"), this);
            this->_WTNZUsernameEdit = new QLineEdit(tr("WTNZ username"), this);
            this->_WTNZPasswordEdit = new QLineEdit(tr("WTNZ user password"), this);
            this->_WTNZPasswordEdit->setEchoMode(QLineEdit::Password);

            this->_WTNZURLEdit->setText(this->_appSettings->value(AppSettings::WTNZ_PROFILE_URL).toString());
            this->_WTNZUsernameEdit->setText(this->_appSettings->value(AppSettings::WTNZ_USERNAME).toString());
            this->_WTNZPasswordEdit->setText(this->_appSettings->value(AppSettings::WTNZ_PASSWORD).toString());

            connectivityGroup->layout()->addWidget(this->_WTNZURLEdit);
            connectivityGroup->layout()->addWidget(this->_WTNZPasswordEdit);
            connectivityGroup->layout()->addWidget(this->_WTNZPasswordEdit);

        layout->addWidget(connectivityGroup);

        // Automation
        auto automationGroup = new QGroupBox(tr("Automation"), this);

            this->_launchAppAtStartupChk = new QCheckBox(tr("Launch FeedTNZ at system boot"), this);

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

    QLineEdit* _WTNZURLEdit;
    QLineEdit* _WTNZUsernameEdit;
    QLineEdit* _WTNZPasswordEdit;

    QCheckBox* _launchAppAtStartupChk;

    void _mightAddAppToStartup() {
        PlatformHelper::switchStartupLaunch();
        this->_appSettings->setValue(AppSettings::MUST_RUN_AT_STARTUP, this->_launchAppAtStartupChk->isChecked());
    }

    void closeEvent(QCloseEvent *event) override {
        this->_appSettings->setValue(AppSettings::WTNZ_PROFILE_URL, this->_WTNZURLEdit->text());
        this->_appSettings->setValue(AppSettings::WTNZ_USERNAME, this->_WTNZUsernameEdit->text());
        this->_appSettings->setValue(AppSettings::WTNZ_PASSWORD, this->_WTNZPasswordEdit->text());
        this->_appSettings->sync();
    }
};
