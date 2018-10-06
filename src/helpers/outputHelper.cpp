#include <boost/filesystem.hpp>
#include "nlohmann/json.hpp"

#include "platformHelper/platformHelper.h"

class OutputHelper {
        public:

            //ensure a file exists
            static bool fileExists(std::string outputFileName) {
                boost::filesystem::path confP(outputFileName);
                confP = boost::filesystem::absolute(confP);
                return boost::filesystem::exists(confP);
            }

            //write outputfile
            static void writeAsJsonFile(nlohmann::json *obj, std::string filePath) {
                
                //dump as string          
                std::string result = obj->dump();

                //get all path
                boost::filesystem::path outP(filePath);
                outP = boost::filesystem::absolute(outP);
                boost::filesystem::create_directory(outP.parent_path()); //create dir if not exist

                //save on path
                std::fstream streamHandler;
                streamHandler.open(outP.c_str(), std::fstream::out);
                streamHandler << result << std::endl;
                streamHandler.close();
            }
};