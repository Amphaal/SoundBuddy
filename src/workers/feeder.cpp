#include <rapidjson/document.h>
#include <rapidjson/pointer.h>

#include <string>
#include <fstream>
#include <exception>
#include <map>
#include <set>
#include <boost/filesystem.hpp>

#include <QtWidgets/QWidget>
#include <boost/algorithm/string/join.hpp>

#include "base/ITNZWorker.h"
#include "../helpers/_const.cpp"
#include "../helpers/platformHelper/platformHelper.h"
#include "../helpers/stringHelper.cpp"
#include "../helpers/outputHelper.cpp"
#include "../localization/i18n.cpp"
#include "../helpers/iTunesLibParser/iTunesLibParser.h"

///
/// Exceptions
///

class FTNZXMLLibFileUnreadableException : public std::exception {      
    private:
        std::string exceptionMessage;

    public:
        FTNZXMLLibFileUnreadableException() {
            this->exceptionMessage = I18n::tr()->FTNZXMLLibFileUnreadableException();
        }
        const char * what () const throw () {
            return this->exceptionMessage.c_str();
        }
};

class FTNZMissingItunesConfigException : public std::exception {
    private:
        std::string exceptionMessage;
    
    public:
        FTNZMissingItunesConfigException() {
            this->exceptionMessage = I18n::tr()->FTNZMissingItunesConfigException();
        }
        const char * what () const throw () {
            return this->exceptionMessage.c_str();
        }
};

class FTNZNoMusicFoundException : public std::exception {
    private:
        std::string exceptionMessage;
    
    public:
        FTNZNoMusicFoundException() {
            this->exceptionMessage = I18n::tr()->FTNZNoMusicFoundException();
        }
        const char * what () const throw () {
            return this->exceptionMessage.c_str();
        }
};

///
/// End Exceptions
///

class FeederWorker : public ITNZWorker {

    public:
		FeederWorker() : ohLib(OUTPUT_FILE_PATH, "uploadLib", "wtnz_file"), ohWrn(WARNINGS_FILE_PATH) {}


        void run() override {
            emit printLog(I18n::tr()->Feeder_Warning());  //EMIT

            try {
                this->generateLibJSONFile();
                this->uploadLibToServer();
            } catch (const std::exception& e) {
                emit printLog(e.what());  //EMIT
                emit error();  //EMIT
            }
        }

        void exit() override {

        }

    private:

        OutputHelper ohLib;
        OutputHelper ohWrn;

        //generate files
        void generateLibJSONFile() {
            auto itnzLibPath = this->getITunesLibLocation();
            this->processFile(itnzLibPath);

            //check warnings
            auto warningsCount = this->libWarningsAsJSON.MemberCount();
            if (warningsCount) {
                //create warning file
                emit printLog(I18n::tr()->Feeder_NotifyWarningsExistence(warningsCount, OUTPUT_FILE_PATH));  //EMIT

                emit printLog(I18n::tr()->Feeder_Unmolding(WARNINGS_FILE_PATH));  //EMIT
                this->ohWrn.writeAsJsonFile(this->libWarningsAsJSON, true);
            } else {
                //remove old warning file
                auto pToRem = this->ohWrn.getOutputPath();
                auto result = boost::filesystem::remove(pToRem.c_str());
            }

            emit printLog(I18n::tr()->Feeder_Unmolding(OUTPUT_FILE_PATH));  //EMIT

            this->ohLib.writeAsJsonFile(this->libAsJSON);

            emit printLog(I18n::tr()->Feeder_OutputReady());  //EMIT

            emit operationFinished(warningsCount);  //EMIT
        }


        //upload
        void uploadLibToServer() {
            emit printLog(I18n::tr()->Feeder_StartSend());  //EMIT
            string response = ohLib.uploadFile();
            if (response != "") {
                emit printLog(I18n::tr()->HTTP_ServerResponded(response));  //EMIT
            } else {
                emit printLog(I18n::tr()->HTTP_NoResponse());  //EMIT
            }
        }


        ///
        /// XML / JSON Helpers
        ///

        size_t recCount;
        size_t expectedCount;
        rapidjson::Document workingJSON;
        rapidjson::Document libAsJSON;
        rapidjson::Document libWarningsAsJSON;
        const set<string> requiredAttrs = {"Track ID", "Track Number", "Year", "Name", "Album Artist", "Album", "Genre", "Date Added"};
        const set<string> ucwordsAttrs = {"Album Artist", "Album", "Genre"};

        void processFile(string xmlFileLocation) {
            this->recCount = 0;
            this->expectedCount = 0;

            //set default
            this->libWarningsAsJSON.Parse("{}");
            this->workingJSON.Parse("{}");
            this->libAsJSON.Parse("[]");

            //format to dict
            this->generateJSON(xmlFileLocation);
            this->standardizeJSON();    
        }

        //navigate through XML and generate object
        void generateJSON(string xmlFileLocation) {
            
            emit printLog(I18n::tr()->Feeder_PredigestXML()); //EMIT
            
            //read xml as string
            iTunesLibParser *doc;
            try {
                doc = new iTunesLibParser(xmlFileLocation.c_str());
            } catch(...) {
                return throw FTNZXMLLibFileUnreadableException();
            }
            auto xmlAsJSONString = doc->ToJSON();
            delete doc;

            //try parse to temp JSON
            rapidjson::Document d;
            rapidjson::ParseResult s = d.Parse(xmlAsJSONString.c_str());
            if(s.IsError()) {
                return throw FTNZXMLLibFileUnreadableException();
            }

            emit printLog(I18n::tr()->Feeder_TrimingFat());  //EMIT
            
            //retrieve tracks and pass to workingJSON
            auto v = rapidjson::Pointer("/Tracks").Get(d);
            this->expectedCount = v->MemberCount();
            if (!this->expectedCount) {
                return throw FTNZNoMusicFoundException();
            }
            this->workingJSON.CopyFrom(d["Tracks"], this->workingJSON.GetAllocator());
            
        }

        //standardize
        void standardizeJSON() {
            
            emit printLog(I18n::tr()->Feeder_CookingJSON());  //EMIT

            //declare allocators
            rapidjson::Document::AllocatorType &lajAlloc = this->libAsJSON.GetAllocator();
            rapidjson::Document::AllocatorType &lwajAlloc = this->libWarningsAsJSON.GetAllocator();
            rapidjson::Document::AllocatorType &wjAlloc = this->workingJSON.GetAllocator();

            //prepare
            set<string> tracksIdToRemove = {};
            this->recCount = 0;
            this->expectedCount = this->workingJSON.MemberCount();
            
            //through each tracks
            for (auto track = this->workingJSON.MemberBegin(); track != this->workingJSON.MemberEnd(); ++track) {
                
                set<string> toRemove = {};
                set<string> foundRequired = {};

                //iterate through properties
                for (auto prop = track->value.MemberBegin(); prop != track->value.MemberEnd(); ++prop) {
                    string k = prop->name.GetString();
                    
                    //check presence of required attrs
                    if (this->requiredAttrs.find(k) == this->requiredAttrs.end()) {
                        if (k != "Disc Number") toRemove.insert(k); //dont remove optional values !
                    } else {
                        foundRequired.insert(k);
                    }
                }

                //check required attrs, else go warnings
                set<string> missingAttrs;
                set_difference(
                    this->requiredAttrs.begin(), this->requiredAttrs.end(), 
                    foundRequired.begin(), foundRequired.end(),
                    inserter(missingAttrs, missingAttrs.end())
                );
                if (missingAttrs.size()) {
                    rapidjson::Value key(track->value["Location"].GetString(), lwajAlloc);
                    rapidjson::Value val(boost::algorithm::join(missingAttrs, ", ").c_str(), lwajAlloc);
                    this->libWarningsAsJSON.AddMember(key, val, lwajAlloc);
                    tracksIdToRemove.insert(track->name.GetString());
                }

                //remove useless props
                for(auto ktr : toRemove) {
                    track->value.RemoveMember(ktr.c_str());
                }

                //set optionnal values default
                if (!track->value.HasMember("Disc Number")) {
                    track->value.AddMember("Disc Number","1", wjAlloc);
                }

                tracksEmitHelper();
            }

            //remove tracks with warnings
            for(auto idtr : tracksIdToRemove) this->workingJSON.RemoveMember(idtr.c_str());

            //turn obect based container into an array one
            for (auto track = this->workingJSON.MemberBegin(); track != this->workingJSON.MemberEnd(); ++track) {
                this->libAsJSON.PushBack(track->value, lajAlloc);
            }
        }

        ///
        /// Other Helpers
        ///

        //log...
        void tracksEmitHelper() {
            bool mustReplacePrevious = this->recCount;
            this->recCount++;
            bool canLog = ((this->recCount % 100) == 0 && this->recCount <= this->expectedCount) || this->recCount == this->expectedCount || !mustReplacePrevious;
            if(canLog) {
                emit printLog(I18n::tr()->Feeder_LogTrackEmit(this->recCount, this->expectedCount), mustReplacePrevious);  //EMIT
            }
        }

        //seek in iTunes preference file the library location
        string getITunesLibLocation() {
            emit printLog(I18n::tr()->Feeder_GetXMLFileLoc());  //EMIT

            PlatformHelper pHelper;
            string pathToPrefs = pHelper.getITunesPrefFileProbableLocation();
            
            try {
                return pHelper.extractItunesLibLocation(pathToPrefs);
            } catch(...) {
                throw FTNZMissingItunesConfigException();
            }
            
        }

};
