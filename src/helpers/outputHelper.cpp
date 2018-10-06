#include <boost/filesystem.hpp>
#include "nlohmann/json.hpp"
#include <curl/curl.h>

#include "platformHelper/platformHelper.h"
#include "./configHelper.cpp"

class OutputHelper {
        private:
            boost::filesystem::path *pathToFile;
            string uploadTargetUrl;
            string uploadPostData;

        public:
            
            OutputHelper(std::string filePath, std::string targetFunction) {
                
                this->pathToFile = new boost::filesystem::path(filePath);
                this->pathToFile = &boost::filesystem::absolute(*this->pathToFile);
                
                ConfigHelper helper;
                helper.ensureConfigFileIsReadyForUpload();
                nlohmann::json config = helper.accessConfig();

                this->uploadTargetUrl = config["targetUrl"].get<string>() + "/" + config["user"].get<string>() + "/" + targetFunction.c_str();
                this->uploadPostData = "password=" + config["password"].get<string>() + "&headless=true";
            }

            //ensure a file exists
            bool fileExists() {
                return boost::filesystem::exists(*this->pathToFile);
            }

            //write outputfile
            void writeAsJsonFile(nlohmann::json *obj) {

                //get all path
                boost::filesystem::create_directory(this->pathToFile->parent_path()); //create dir if not exist

                //save on path
                std::fstream streamHandler;
                streamHandler.open(this->pathToFile->c_str(), std::fstream::out);
                streamHandler << obj->dump() << std::endl;
                streamHandler.close();
            }

            void uploadFile() {

                //prepare upload
                CURL *curl;
                CURLcode res;
                
                /* In windows, this will init the winsock stuff */ 
                curl_global_init(CURL_GLOBAL_ALL);
                
                /* get a curl handle */ 
                CURL *curl = curl_easy_init();
                if(curl) {

                    curl_easy_setopt(curl, CURLOPT_URL, this->uploadTargetUrl.c_str()); //url
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, this->uploadPostData.c_str()); //POST
                
                    /* tell it to "upload" to the URL */ 
                    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

                    /*read file*/
                    FILE *fd = fopen(this->pathToFile->c_str(), "rb"); /* open file to upload */ 
                    if(!fd) return;
                    
                    /* to get the file size */ 
                    struct stat file_info;
                    if(fstat(fileno(fd), &file_info) != 0) return;

                    curl_easy_setopt(curl, CURLOPT_READDATA, fd);

                    CURLcode res = curl_easy_perform(curl); /* Perform the request, res will get the return code */  
                    if(res != CURLE_OK) throw "Error communicating with the remote server."; /* Check for errors */ 
                    
                    curl_easy_cleanup(curl); /* always cleanup */ 
                }

                curl_global_cleanup(); /* always cleanup */ 
            }
};