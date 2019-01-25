#pragma once

#include <string>
#include <boost/filesystem.hpp>
#include "nlohmann/json.hpp"
#include <curl/curl.h>

#include "platformHelper/platformHelper.h"
#include "./stringHelper.cpp"
#include "./configHelper.cpp"
#include "../localization/i18n.cpp"

#include <QStandardPaths>
#include <QDir>

using namespace std;

///
/// Exceptions
///

class FTNZNoOutputFileException : public std::exception {
    private:
        std::string exceptionMessage;

    public:
        FTNZNoOutputFileException(std::string outputPath) {
            this->exceptionMessage = I18n::tr()->FTNZNoOutputFileException(outputPath);
        }
        const char* what() const throw () {   
            return this->exceptionMessage.c_str();
        }
};

class FTNZOutputFileUnreadableException : public std::exception {
    private:
        std::string exceptionMessage;

    public:
        FTNZOutputFileUnreadableException(std::string outputPath) {
            this->exceptionMessage = I18n::tr()->FTNZOutputFileUnreadableException(outputPath);
        }
        const char* what() const throw () {   
            return this->exceptionMessage.c_str();
        }
};

class FTNZErrorUploadingException : public std::exception {
    private:
            std::string exceptionMessage;

    public:
        FTNZErrorUploadingException() {
            this->exceptionMessage = I18n::tr()->FTNZErrorUploadingException();
        }
        const char* what() const throw () {   
            return this->exceptionMessage.c_str();
        }
};

class FTNZErrorProcessingUploadException : public std::exception {
    private:
            std::string exceptionMessage;

    public:
        FTNZErrorProcessingUploadException(long code, string response) {
            StringHelper::replaceFirstOccurrence(response, "\n", "");
            this->exceptionMessage = I18n::tr()->FTNZErrorProcessingUploadException(code, response);
        }
        const char* what() const throw () {   
            return this->exceptionMessage.c_str();
        }
};


///
/// End Exceptions
///

class OutputHelper {
        private:
            boost::filesystem::path pathToFile;
            string uploadTargetUrl;
            map<string, string> uploadPostData;
            string uploadFileName;

            //upload response reader
            static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
            {
                ((std::string*)userp)->append((char*)contents, size * nmemb);
                return size * nmemb;
            }

        public:
            OutputHelper(std::string filePath, std::string targetFunction = "", std::string uploadFileName = "") : pathToFile(filePath), uploadFileName(uploadFileName) {
                
                //set definitive location and create path if not exist
                std::string hostPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation).toStdString();
                QDir hostDir(hostPath.c_str());
                if (!hostDir.exists()) hostDir.mkpath(".");
                this->pathToFile = hostPath + "/" + this->pathToFile.string();
                
                if(targetFunction == "" || uploadFileName == "") return;

                ConfigHelper helper;
                helper.ensureConfigFileIsReadyForUpload();
                nlohmann::json config = helper.accessConfig();

                this->uploadTargetUrl = config["targetUrl"].get<string>() + "/" + config["user"].get<string>() + "/" + targetFunction.c_str();
                
                this->uploadPostData.insert(pair<string, string>("password", config["password"].get<string>()));
                this->uploadPostData.insert(pair<string, string>("headless", "1"));
            }

            //ensure a file exists
            bool fileExists() {
                return boost::filesystem::exists(this->pathToFile);
            }

            //
            std::string getOutputPath() {
                return this->pathToFile.generic_string();
            }

            //write outputfile
            void writeAsJsonFile(nlohmann::json *obj) {

                //get all path
                boost::filesystem::create_directory(this->pathToFile.parent_path()); //create dir if not exist

                //save on path
                std::fstream streamHandler;
                streamHandler.open(this->pathToFile.c_str(), std::fstream::out);
                streamHandler << obj->dump() << std::endl;
                streamHandler.close();
            }

            std::string uploadFile() {
                
                /* In windows, this will init the winsock stuff */ 
                curl_global_init(CURL_GLOBAL_ALL);
                
                /* get a curl handle */ 
                std::string response;
                CURL *curl = curl_easy_init();
                if(curl) {

                    curl_mime *form = NULL;
                    curl_mimepart *field = NULL;

                    /* Create the form */ 
                    form = curl_mime_init(curl);
                
                    /* Fill in the file upload field */ 
                    field = curl_mime_addpart(form);
                    curl_mime_name(field, this->uploadFileName.c_str());
                    curl_mime_filedata(field, this->pathToFile.string().c_str());
                
                    /* For each field*/
                    for(auto kvp : this->uploadPostData) {
                        field = curl_mime_addpart(form);
                        curl_mime_name(field, kvp.first.c_str());
                        curl_mime_data(field, kvp.second.c_str(), CURL_ZERO_TERMINATED);
                    }

                    /* what URL that receives this POST */ 
                    curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);

                    //header
                    struct curl_slist *list = NULL;
                    auto localeHeader = string("Accept-Language: ") + I18n::getLocaleName();
                    list = curl_slist_append(list, localeHeader.c_str());
                    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
                    
                    //response text
                    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

                    /* try use of SSL for this */
                    //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0); // issue with libcurl's Conan package forcing unsecure SSL
                    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);

                    //url and execute
                    curl_easy_setopt(curl, CURLOPT_URL, this->uploadTargetUrl.c_str()); 
                    CURLcode res = curl_easy_perform(curl); 
                    
                    if(res != CURLE_OK) {
                        throw FTNZErrorUploadingException(); 
                        return "";
                    } else {
                        //response code
                        long code;
                        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
                        
                        //if http code is not OK
                        if(code != 200) {
                            throw FTNZErrorProcessingUploadException(code, response);
                            return "";
                        }
                    }

                    curl_easy_cleanup(curl); /* always cleanup */ 
                    curl_mime_free(form); /* then cleanup the form */ 
                }

                curl_global_cleanup(); /* always cleanup */ 
                return response;
            }
};