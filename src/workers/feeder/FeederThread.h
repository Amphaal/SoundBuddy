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
    // generate files
    void _generateLibJSONFile();

    // upload
    void _uploadLibToServer();

    ///
    /// XML / JSON Helpers
    ///

    size_t _recCount;
    size_t _expectedCount;
    QJsonDocument* _workingJSON = nullptr;
    QJsonDocument* _libAsJSON = nullptr;
    QJsonDocument* _libWarningsAsJSON = nullptr;

    static const inline std::set<QString> _requiredAttrs {"Track ID", "Track Number", "Year", "Name", "Album Artist", "Album", "Genre", "Date Added"};
    static const inline std::set<QString> _ucwordsAttrs {"Album Artist", "Album", "Genre"};

    void _processFile(const QString &xmlFileLocation);
    void _generateJSON(const QString &xmlFileLocation);
    void _standardizeJSON();

    ///
    /// Other Helpers
    ///

    // log...
    void _tracksEmitHelper();
    void _tracksUnmolding(const char* filename);

    // seek in Music App preference file the library location
    QString _getMusicAppLibLocation();

   signals:
      void operationFinished();
};