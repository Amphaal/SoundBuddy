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
                std::fstream streamHandler;
                boost::filesystem::path outP(filePath);
                outP = boost::filesystem::absolute(outP);
                streamHandler.open(outP.c_str(), std::fstream::out);
                std::string result = obj->dump();
                streamHandler << result << std::endl;
                streamHandler.close();
            }
};