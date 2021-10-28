#pragma once

#include <QString>
#include <QObject>

#include "src/version.h"

const QString musicAppName() {
    #ifdef _WIN32
        return "iTunes";
    #endif
    #ifdef APPLE
        return QObject::tr("Music");
    #endif
}