#include <rapidjson/document.h>
#include <rapidjson/pointer.h>

#include <string>
#include <fstream>
#include <exception>
#include <map>
#include <set>
#include <pugixml.hpp>

#include <QtWidgets/QWidget>
#include <boost/algorithm/string/join.hpp>
#include <boost/any.hpp>
#include "../../libs/plistcpp/Plist.hpp"

#include "base/ITNZWorker.h"
#include "../helpers/const.cpp"
#include "../helpers/platformHelper/platformHelper.h"
#include "../helpers/stringHelper.cpp"
#include "../helpers/outputHelper.cpp"
#include "../localization/i18n.cpp"

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
            emit printLog(I18n::tr()->Feeder_Warning());

            try {
                this->generateLibJSONFile();
                this->uploadLibToServer();
            } catch (const std::exception& e) {
                emit printLog(e.what());
                emit error();
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
                emit printLog(I18n::tr()->Feeder_NotifyWarningsExistence(warningsCount, OUTPUT_FILE_PATH));

                emit printLog(I18n::tr()->Feeder_Unmolding(WARNINGS_FILE_PATH));
                this->ohWrn.writeAsJsonFile(this->libWarningsAsJSON, true);
            } else {
                std::remove(WARNINGS_FILE_PATH.c_str());
            }

            emit printLog(I18n::tr()->Feeder_Unmolding(OUTPUT_FILE_PATH));
            this->ohLib.writeAsJsonFile(this->libAsJSON);

            emit printLog(I18n::tr()->Feeder_OutputReady());

            emit operationFinished(warningsCount);
        }


        //upload
        void uploadLibToServer() {
            emit printLog(I18n::tr()->Feeder_StartSend());
            string response = ohLib.uploadFile();
            if (response != "") {
                emit printLog(I18n::tr()->HTTP_ServerResponded(response));
            } else {
                emit printLog(I18n::tr()->HTTP_NoResponse());
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
        const string xPathExtractQuery = "/plist/dict/key[.='Tracks']/following-sibling::*[1]";

        void processFile(string xmlFileLocation) {
            emit printLog(I18n::tr()->Feeder_TrimingFat());
            this->recCount = 0;
            this->expectedCount = 0;

            //open file and dig into tracks
            pugi::xml_document doc;
            try {
                doc.load_file(xmlFileLocation.c_str());
            } catch(...) {
                return throw FTNZXMLLibFileUnreadableException();
            }

            pugi::xpath_node_set tracks = doc.select_nodes(xPathExtractQuery.c_str());

            //format to dict
            emit printLog(I18n::tr()->Feeder_CookingJSON());
            this->generateJSON(&tracks);
            this->standardizeJSON();    
        }

        //navigate through XML and generate object
        void generateJSON(pugi::xpath_node_set *nodesList) {

            //set target 
            auto target = nodesList->first().node().children();
            this->expectedCount = std::distance(target.begin(), target.end()) / 2;
            if (!this->expectedCount) {
                return throw FTNZNoMusicFoundException();
            }

            //iterate through
            for (pugi::xpath_node child : *nodesList) {
                this->resursiveDict(child.node());
            }
        }

        //inner recursive
        string resursiveDict(pugi::xml_node traversedNode, string pathToKey = "") {
                
                //prepare 
                string name = traversedNode.name();
                string text = traversedNode.text().as_string();
                
                if (name == "key") {
                    
                    //add key to path for subsequent insertions
                    string prefix = (StringHelper::has_only_digits(text) ? "~1" : ""); //escape keys consisting of numbers
                    return pathToKey += "/" + prefix + text;

                } else if (text != "" && name != "") {
                    
                    //if contain a type and actual value, insert at path location
                    rapidjson::Pointer(pathToKey.c_str()).Set(this->workingJSON, text.c_str());

                    //truncate path to key because key has been consumed by value
                    return StringHelper::splitPath(pathToKey);

                }

                //traverse
                for (pugi::xpath_node child : traversedNode.children()) {
                    pathToKey = this->resursiveDict(child.node(), pathToKey);
                }

                tracksEmitHelper();

                //return
                return StringHelper::splitPath(pathToKey);
        }

        //standardize
        void standardizeJSON() {
            
            emit printLog(I18n::tr()->Feeder_PredigestXML());
            
            set<string> tracksIdToRemove = {};
            this->recCount = 0;
            this->expectedCount = this->workingJSON.MemberCount();
            
            //through each tracks
            for (auto& track = this->workingJSON.MemberBegin(); track != this->workingJSON.MemberEnd(); ++track) {
                
                set<string> toRemove = {};
                set<string> foundRequired = {};

                //iterate through properties
                for (auto& prop = track->value.MemberBegin(); prop != track->value.MemberEnd(); ++prop) {
                    string k = prop->name.GetString();
                    string v = prop->value.GetString();
                    
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
                    rapidjson::Value key(track->value["Location"].GetString(), this->libWarningsAsJSON.GetAllocator());
                    rapidjson::Value val(boost::algorithm::join(missingAttrs, ", ").c_str(), this->libWarningsAsJSON.GetAllocator());
                    this->libWarningsAsJSON.AddMember(key, val, this->libWarningsAsJSON.GetAllocator());
                    tracksIdToRemove.insert(track->name.GetString());
                }

                //remove useless props
                for(auto ktr : toRemove) track->value.RemoveMember(ktr.c_str());

                //set optionnal values default
                if (!track->value.HasMember("Disc Number")) track->value.AddMember("Disc Number","1", this->workingJSON.GetAllocator());

                tracksEmitHelper();
            }

            //remove tracks with warnings
            for(auto idtr : tracksIdToRemove) this->workingJSON.RemoveMember(idtr.c_str());

            //remove ids
            for (auto& track = this->workingJSON.MemberBegin(); track != this->workingJSON.MemberEnd(); ++track) {
                this->libAsJSON.PushBack(track->value);
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
                emit printLog(I18n::tr()->Feeder_LogTrackEmit(this->recCount, this->expectedCount), mustReplacePrevious);
            }
        }

        //seek in iTunes preference file the library location
        string getITunesLibLocation() {
            emit printLog(I18n::tr()->Feeder_GetXMLFileLoc());

            PlatformHelper pHelper;

            string pathToPrefs = pHelper.getITunesPrefFileProbableLocation();
            
            map<string, boost::any> pListAsMap; 
            try {
		        Plist::readPlist(pathToPrefs.c_str(), pListAsMap);
                return pHelper.extractItunesLibLocationFromMap(&pListAsMap);
            } catch (...) {
                throw FTNZMissingItunesConfigException();
            }
        }

};
