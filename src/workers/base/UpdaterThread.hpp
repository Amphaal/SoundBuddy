#pragma once

#include <QThread>
#include <IFWUpdateChecker.hpp>
Q_DECLARE_METATYPE(UpdateChecker::CheckResults)

#include "src/version.h"

class UpdaterThread : public QThread {
    Q_OBJECT

 public:
    void run() override {
        UpdateChecker checker(APP_REMOTE_MANIFEST_URL);
        auto result = checker.isNewerVersionAvailable();
        emit isNewerVersionAvailable(result);
    }

 signals:
    void isNewerVersionAvailable(const UpdateChecker::CheckResults results);
};
