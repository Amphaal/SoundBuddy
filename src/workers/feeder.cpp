#include <string>
#include <fstream>
#include <exception>
#include <map>
#include <set>
#include <pugixml.hpp>
#include "nlohmann/json.hpp"

#include "QtWidgets/QWidget"

#include "../../libs/plistcpp/Plist.hpp"

#include "base/ITNZWorker.h"
#include "../helpers/platformHelper/platformHelper.h"
#include "../helpers/stringHelper.cpp"
#include "../helpers/outputHelper.cpp"

using namespace boost;
using namespace std;
using namespace pugi;

///
/// Exceptions
///

class FTNZXMLLibFileUnreadableException : public std::exception {      
    const char * what () const throw ()
    {
        return "Cannot read the XML file bound to your library. Are you sure you activated the XML file sharing in iTunes ?";
    }
};

class FTNZMissingItunesConfigException : public std::exception {      
    const char * what () const throw ()
    {
        return "An issue happened while fetching iTunes's XML file location. Have you installed iTunes ?";
    }
};

class FTNZNoMusicFoundException : public std::exception {      
    const char * what () const throw ()
    {
        return "No music found in your library. Please feed it some.";
    }
};

///
/// End Exceptions
///

class FeederWorker : public ITNZWorker {

    const string outputFileName = "output\\output.json";
    const string warningsFileName = "output\\warnings.json";

    public:
		FeederWorker() : ohLib(this->outputFileName, "uploadLib", "wtnz_file"), ohWrn(this->warningsFileName) {}

        void run() override {
            emit printLog("WARNING ! Make sure you activated the XML file sharing in iTunes>Preferences>Advanced.");

            try {
                //this->generateLibJSONFile();
                this->uploadLibToServer();
            } catch (const std::exception& e) {
                emit printLog(e.what());
                emit error();
            }
        }

    private:

        OutputHelper ohLib;
        OutputHelper ohWrn;

        //generate files
        void generateLibJSONFile() {
            auto itnzLibPath = this->getITunesLibLocation();
            this->processFile(itnzLibPath);

            //check warnings
            auto warningsCount = this->libWarningsAsJSON.size();
            if (warningsCount) {
                emit printLog("WARNING ! " + std::to_string(warningsCount)  + 
                " files in your library are missing important metadata and consequently were removed from the output file ! " + 
                "Please check the \"" + this->outputFileName + "\" file for more informations.");

                emit printLog("Unmolding \"" + this->warningsFileName + "\"...");
                this->ohWrn.writeAsJsonFile(&this->libWarningsAsJSON);
            } else {
                std::remove(this->warningsFileName.c_str());
            }

            emit printLog("Unmolding \"" + this->outputFileName + "\"...");
            this->ohLib.writeAsJsonFile(&this->libAsJSON);

            emit printLog("OK, output file is ready for breakfast !");
        }


        //upload
        void uploadLibToServer() {
            emit printLog("Let's try to send now !");
            ohLib.uploadFile();
        }


        ///
        /// XML / JSON Helpers
        ///

        size_t recCount;
        size_t expectedCount;
        nlohmann::json libAsJSON;
        nlohmann::json libWarningsAsJSON;
        const set<string> requiredAttrs = {"Track ID", "Track Number", "Year", "Name", "Album Artist", "Album", "Genre", "Date Added"};
        const set<string> ucwordsAttrs = {"Album Artist", "Album", "Genre"};
        const string xPathExtractQuery = "/plist/dict/key[.='Tracks']/following-sibling::*[1]";

        void processFile(string xmlFileLocation) {
            emit printLog("Triming fat...");
            this->recCount = 0;
            this->expectedCount = 0;

            //open file and dig into tracks
            xml_document doc;
            try {
                xml_parse_result result = doc.load_file(xmlFileLocation.c_str());
            } catch(...) {
                return throw FTNZXMLLibFileUnreadableException();
            }

            xpath_node_set tracks = doc.select_nodes(xPathExtractQuery.c_str());

            //format to dict
            emit printLog("Cooking the JSON file...");
            this->generateJSON(&tracks);
            this->standardizeJSON();    
        }

        //navigate through XML and generate object
        void generateJSON(pugi::xpath_node_set *nodesList) {
            this->libAsJSON = {};

            //set target 
            auto target = nodesList->first().node().children();
            this->expectedCount = std::distance(target.begin(), target.end()) / 2;
            if (!this->expectedCount) {
                return throw FTNZNoMusicFoundException();
            }

            //iterate through
            for (xpath_node child : *nodesList) {
                this->resursiveDict(&child.node());
            }
        }

        //inner recursive
        string resursiveDict(xml_node *traversedNode, string pathToKey = "") {
                
                //prepare 
                string name = traversedNode->name();
                string text = traversedNode->text().as_string();
                
                if (name == "key") {
                    
                    //add key to path for subsequent insertions
                    string prefix = (StringHelper::has_only_digits(text) ? "~1" : ""); //escape keys consisting of numbers
                    return pathToKey += "/" + prefix + text;

                } else if (text != "" && name != "") {
                    
                    //if contain a type and actual value, insert at path location
                    nlohmann::json::json_pointer p0(pathToKey.c_str());
                    this->libAsJSON[p0] = text;

                    //truncate path to key because key has been consumed by value
                    return StringHelper::splitPath(pathToKey);

                }

                //traverse
                for (xpath_node child : traversedNode->children()) {
                    pathToKey = this->resursiveDict(&child.node(), pathToKey);
                }

                tracksEmitHelper();

                //return
                return StringHelper::splitPath(pathToKey);
        }

        //standardize
        void standardizeJSON() {
            
            emit printLog("Pre-digesting XML file...");
            
            this->libWarningsAsJSON = {}; 
            set<string> tracksIdToRemove = {};
            this->recCount = 0;
            this->expectedCount = this->libAsJSON.size();
            
            //through each tracks
            for(auto track : this->libAsJSON.items()) {
                
                set<string> toRemove = {};
                set<string> foundRequired = {};

                //iterate through properties
                for(auto prop : track.value().items()) {
                    string k = prop.key();
                    string v = prop.value();
                    
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
                    this->libWarningsAsJSON.push_back({track.value()["Location"], missingAttrs});
                    tracksIdToRemove.insert(track.key());
                }

                //remove useless props
                for(auto ktr : toRemove) track.value().erase(ktr);

                //set optionnal values default
                if (track.value()["Disc Number"] == nullptr) track.value()["Disc Number"] = "1";

                tracksEmitHelper();
            }

            //remove tracks with warnings
            for(auto idtr : tracksIdToRemove) this->libAsJSON.erase(idtr);
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
                string log = std::to_string(this->recCount)  + " over " + std::to_string(this->expectedCount) + "...";
                emit printLog(log, mustReplacePrevious);
            }
        }

        //seek in iTunes preference file the library location
        string getITunesLibLocation() {
            emit printLog("Getting XML file location...");

            PlatformHelper *pHelper = new PlatformHelper;

            string pathToPrefs = pHelper->getITunesPrefFileProbableLocation();
            
            map<string, any> pListAsMap; 
            try {
		        Plist::readPlist(pathToPrefs.c_str(), pListAsMap);
                return pHelper->extractItunesLibLocationFromMap(&pListAsMap);
            } catch (...) {
                throw FTNZMissingItunesConfigException();
            }
        }

};
