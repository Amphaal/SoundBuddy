#include <boost/filesystem.hpp>
#include "nlohmann/json.hpp"

#include "platformHelper/platformHelper.h"

class OutputHelper {
        public:

        //ensure a file exists
        bool fileExists(std::string outputFileName) {
            boost::filesystem::path confP(outputFileName);
            confP = boost::filesystem::absolute(confP);
            return boost::filesystem::exists(confP);
        }

        //write outputfile
        void writeAsJsonFile(nlohmann::json *obj, std::string filePath) {
            std::fstream streamHandler;
            streamHandler.open(filePath, std::fstream::out);
            std::string result = obj->dump();
            streamHandler << result << std::endl;
            streamHandler.close();
        }

        private:
            typedef std::function<void(std::string)> MessageHelper;
            MessageHelper callMessageHelper;
            bool silentMode;
};