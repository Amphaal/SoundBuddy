#pragma once

#include <QString>

#include <fstream>
#include <algorithm>
#include <set>

#include "src/workers/base/ITNZThread.hpp"

#include "src/helpers/platformHelper/platformHelper.h"
#include "src/helpers/outputHelper/outputHelper.hpp"

#include <ITunesLibraryParser.hpp>

class FeederThread : public ITNZThread {
   Q_OBJECT

 public:
    FeederThread();

    void run() override;

 private:
    OutputHelper* _ohLib = nullptr;
    OutputHelper* _ohWrn = nullptr;

    // generate files
    void _generateLibJSONFile();

    // upload
    void _uploadLibToServer();

    ///
    /// XML / JSON Helpers
    ///

    size_t _recCount;
    size_t _expectedCount;
    rapidjson::Document* _workingJSON = nullptr;
    rapidjson::Document* _libAsJSON = nullptr;
    rapidjson::Document* _libWarningsAsJSON = nullptr;

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

    // seek in iTunes preference file the library location
    QString _getITunesLibLocation();
};