#pragma once

#include <QString>

#include <fstream>
#include <algorithm>
#include <set>

#include "src/workers/base/ITNZThread.hpp"

#include "src/helpers/PlatformHelper.h"

#include <ITunesLibraryParser.hpp>

class FeederThread : public ITNZThread {
   Q_OBJECT

 public:
    FeederThread(const UploadHelper* uploder, const AppSettings::ConnectivityInfos connectivityInfos);

    void run() override;

 private:
    // log...
    void _tracksEmitHelper();
    void _tracksUnmolding(const char* filename);

    // seek in Music App preference file the library location
    QString _getMusicAppLibLocation();

   signals:
      void operationFinished();
};