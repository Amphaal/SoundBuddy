#pragma once

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>

#include <string>
#include <fstream>
#include <exception>
#include <map>
#include <set>

#include <QtWidgets/QWidget>

#include "_exceptions.hpp"

#include "src/workers/base/ITNZThread.h"
#include "src/helpers/_const.cpp"
#include "src/helpers/platformHelper/platformHelper.h"
#include "src/helpers/stringHelper/stringHelper.cpp"
#include "src/helpers/outputHelper/outputHelper.cpp"
#include "src/helpers/iTunesLibParser/iTunesLibParser.h"

class FeederThread : public ITNZThread {

    public:
		FeederThread();
        ~FeederThread();

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
        
        static const inline set<string> _requiredAttrs = {"Track ID", "Track Number", "Year", "Name", "Album Artist", "Album", "Genre", "Date Added"};
        static const inline set<string> _ucwordsAttrs = {"Album Artist", "Album", "Genre"};

        void _processFile(const std::string &xmlFileLocation);
        void _generateJSON(const std::string &xmlFileLocation);
        void _standardizeJSON();

        ///
        /// Other Helpers
        ///

        //log...
        void _tracksEmitHelper();

        //seek in iTunes preference file the library location
        std::string _getITunesLibLocation();

};
