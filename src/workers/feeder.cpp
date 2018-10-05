#include <string>
#include <fstream>
#include <exception>
#include <map>
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

class FeederWorker : public ITNZWorker {

    const string outputFileName = "output\\output.json";
    const string warningsFileName = "output\\warnings.json";

    public:
		FeederWorker() : pHelper(new PlatformHelper()) {}

        void run() override {
            emit printLog("WARNING ! Make sure you activated the XML file sharing in iTunes>Preferences>Advanced.");

            try {
                this->generateLibJSONFile();
                this->uploadLibToServer();
            } catch (const std::exception& e) {
                emit printLog(e.what());
                emit error();
            }
        }

    private:
        PlatformHelper *pHelper;
 
        void generateLibJSONFile() {
            auto itnzLibPath = this->getITunesLibLocation();
            this->processFile(itnzLibPath);

            nlohmann::json *outputJSON = &this->libAsJSON[0];
            nlohmann::json *warningsJSON = &this->libAsJSON[1];

            //check warinings
            size_t warningsCount = warningsJSON->size();
            if (warningsCount) {
                emit printLog("WARNING ! " + std::to_string(warningsCount)  + 
                " files in your library are missing important metadata and consequently were removed from the output file ! " + 
                "Please check the \"" + this->outputFileName + "\" file in the output folder for more informations.");

                emit printLog("Unmolding \"" + this->warningsFileName + "\"...");
                OutputHelper::writeAsJsonFile(warningsJSON, this->warningsFileName);
            } else {
                std::remove(this->warningsFileName.c_str());
            }

            emit printLog("Unmolding \"" + this->outputFileName + "\"...");
            OutputHelper::writeAsJsonFile(outputJSON, this->outputFileName);

            emit printLog("OK, output file is ready for breakfast !");
        }
        void uploadLibToServer() {

        }


        ///
        /// XML Helpers
        ///

        int recCount;
        int expectedCount;
        nlohmann::json libAsJSON;
        const vector<string> requiredAttrs = {"Track ID", "Track Number", "Year", "Name", "Album Artist", "Album", "Genre", "Date Added"};
        const vector<string> ucwordsAttrs = {"Album Artist", "Album", "Genre"};
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
                throw "Cannot read the XML file bound to your library. Are you sure you activated the XML file sharing in iTunes ?";
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
                throw "No music found in your library. Please feed it some.";
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

                //log...
                bool mustReplacePrevious = this->recCount;
                this->recCount++;
                bool canLog = ((this->recCount % 100) == 0 && this->recCount <= this->expectedCount) || this->recCount == this->expectedCount || !mustReplacePrevious;
                if(canLog) {
                    string log = std::to_string(this->recCount)  + " over " + std::to_string(this->expectedCount) + "...";
                    emit printLog(log, mustReplacePrevious);
                }

                //return
                return StringHelper::splitPath(pathToKey);
        }

        //standardize
        nlohmann::json* standardizeJSON() {
            
            emit printLog("Pre-digesting XML file...");

            nlohmann::json output = {};
            nlohmann::json warnings = {};

            // for(nlohmann::json track : this->libAsJSON) {
            //     for(nlohmann::json prop : track) {

            //     }
            // }

            //return {output, warnings};
            //return this->libAsJSON;
        }

        ///
        /// Other Helpers
        ///

        //seek in iTunes preference file the library location
        string getITunesLibLocation() {
            emit printLog("Getting XML file location...");

            string pathToPrefs = this->pHelper->getITunesPrefFileProbableLocation();
            
            map<string, any> pListAsMap; 
            try {
		        Plist::readPlist(pathToPrefs.c_str(), pListAsMap);
                return this->pHelper->extractItunesLibLocationFromMap(&pListAsMap);
            } catch (...) {
                throw "An issue happened while fetching iTunes's XML file location. Have you installed iTunes ?";
            }
        }

};