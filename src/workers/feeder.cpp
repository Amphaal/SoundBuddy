#include <string>
#include <fstream>
#include <exception>
#include <map>
#include <pugixml.hpp>
#include "nlohmann/json.hpp"

#include "QtWidgets/QWidget"

#include "base/ITNZWorker.h"
#include "../helpers/platformHelper/platformHelper.h"
#include "../../libs/plistcpp/Plist.hpp"
#include "../helpers/stringHelper.cpp"

using namespace boost;
using namespace std;
using namespace pugi;

class FeederWorker : public ITNZWorker {

    const string outputFileName = "output/output.json";
    const string warningsFileName = "output/warnings.json";

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
            xml_parse_result result = doc.load_file(xmlFileLocation.c_str());
            xpath_node_set tracks = doc.select_nodes(xPathExtractQuery.c_str());

            //format to dict
            emit printLog("Cooking the JSON file...");
            this->resursiveDict(&tracks);
            auto t = this->libAsJSON.dump();

        }

        //navigate through XML and generate object

        void resursiveDict(pugi::xpath_node_set *nodesList) {
            this->libAsJSON = {};

            //set target 
            auto target = nodesList->first().node().children();
            this->expectedCount = std::distance(target.begin(), target.end()) / 2;

            //iterate through
            for (xpath_node child : *nodesList) {
                this->resursiveDict(&child.node());
            }
        }

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
                if((this->recCount % 100) == 0 || this->recCount == this->expectedCount) {
                    string log = std::to_string(this->recCount)  + " over " + std::to_string(this->expectedCount) + "...";
                    emit printLog(log, mustReplacePrevious);
                }

                //return
                return StringHelper::splitPath(pathToKey);
        }

        ///
        /// Other Helpers
        ///

        //seek in iTunes preference file the library location
        string getITunesLibLocation() {
            emit printLog("Getting XML file location...");

            string pathToPrefs = this->pHelper->getITunesPrefFileProbableLocation();
            
            map<string, any> pListAsMap; 
		    Plist::readPlist(pathToPrefs.c_str(), pListAsMap);

            return this->pHelper->extractItunesLibLocationFromMap(&pListAsMap);
        }

};