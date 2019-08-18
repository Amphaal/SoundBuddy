#pragma once

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>

#include <fstream>
#include <algorithm>
#include <exception>
#include <set>

#include <QString>

#include "_exceptions.hpp"

#include "src/workers/base/ITNZThread.hpp"
#include "src/helpers/_const.hpp"
#include "src/helpers/platformHelper/platformHelper.h"
#include "src/helpers/stringHelper/stringHelper.hpp"
#include "src/helpers/outputHelper/outputHelper.hpp"
#include "src/helpers/iTunesLibParser/iTunesLibParser.h"

class FeederThread : public ITNZThread {

    public:
		FeederThread();

        void run() override;

    private:
        OutputHelper* _ohLib = nullptr;
        OutputHelper* _ohWrn = nullptr;

        //generate files
        void _generateLibJSONFile();

        //upload
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

        //log...
        void _tracksEmitHelper();

        //seek in iTunes preference file the library location
        QString _getITunesLibLocation();

};
