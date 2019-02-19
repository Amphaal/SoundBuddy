#pragma once

#include <exception>
#include <boost/filesystem.hpp>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>

#include "platformHelper/platformHelper.h"
#include "../localization/i18n.cpp"
#include "_const.cpp"
#include "../version.h"

#include <QStandardPaths>
#include <QUrl>
#include <QDir>

///
/// Exceptions
///

class FTNZMissingConfigValuesException : public std::exception {    
    private:
        std::string exceptionMessage;

    public:
        FTNZMissingConfigValuesException() {
            this->exceptionMessage = I18n::tr()->FTNZMissingConfigValuesException();
        }  
        const char * what () const throw () {
            return this->exceptionMessage.c_str();
        }
};

///
/// End Exceptions
///

class ConfigHelper {
    
    public:
        ConfigHelper() : pHelper(PlatformHelper()) {

            //set definitive location and create path if not exist
            std::string hostPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation).toStdString();
            this->hostDir = new QDir(hostPath.c_str());
            this->configFile = hostPath + "/" + CONFIG_FILE_PATH;

        }

        ~ConfigHelper() {
            delete this->hostDir;
        }
                
        //open the configuration file
        void openConfigFile() {
            this->pHelper.openFileInOS(this->configFile);
        }

        std::string getUsersHomeUrl(rapidjson::Document &config) {
            
            //try to get users name
            auto tUser = this->getParamValue(config, "username");
            if (tUser == "") return ""; //no user, no URL

            //no target ? no url
            auto tUrl = this->getTargetUrl(config);
            if (tUrl == NULL) return "";
            
            //add user part to url
            auto newP = tUrl->toString().toStdString() + "/" + tUser;
            QUrl ret(newP.c_str());
            delete tUrl;

            //check validity
            if(!ret.isValid()) return "";
            return ret.toString().toStdString();
        }

        //get the targeted platform url
        QUrl* getTargetUrl(rapidjson::Document &config) {
            
            //get parameterized targetUrl
            auto tUrl = this->getParamValue(config, "targetUrl");
            if (tUrl == "") tUrl = APP_DEFAULT_URL;
            
            //check validity
            auto rlObj = new QUrl(tUrl.c_str(), QUrl::TolerantMode);
            if(rlObj->isValid()) {
                return rlObj;
            }
            else {
                delete rlObj;
                return NULL;
            }
        }

        //makes sure mandatory fields for uplaods are filled
        bool ensureConfigFileIsReadyForUpload(rapidjson::Document &config, bool throwable = true) {

            //check required field presence
            bool isReady = true;
            this->onEmptyRequiredValue(config, [&isReady](){
                    isReady = false;
            });
            if(!isReady && throwable) throw FTNZMissingConfigValuesException();

            return isReady;
        }

        //get configuration data from file
        rapidjson::Document accessConfig() {
            
            auto config = this->accessConfigRaw();

            //check required field presence and adds them if missing
            bool mustWrite = false;
            rapidjson::Document::AllocatorType &alloc = config.GetAllocator();
            this->onMissingRequiredMember(config, [&mustWrite, &config, &alloc](std::string rf){
                    rapidjson::Value n(rf.c_str(), alloc);
                    config.AddMember(n, "", alloc);
                    mustWrite = true;
            });

            //re-write as formated string
            if(mustWrite) {
                this->writeFormatedFileFromObj(config);
            }
            
            //return values
            return config;
        }

        //update the current config file
        void updateParamValue(std::string paramToUpdate, std::string value) {
            auto config = this->accessConfigRaw();
            this->createParamIfNotExist(config, paramToUpdate, value);
            this->writeFormatedFileFromObj(config);
        }

        //get the param value
        std::string getParamValue(rapidjson::Document &config, std::string param) {
            auto mem = config.FindMember(param.c_str());
            if(mem == config.MemberEnd()) return "";
            return !config[param.c_str()].IsString() ? "" : config[param.c_str()].GetString();
        }

        //get full path of the config file
        std::string getFullPath() {
            boost::filesystem::path confP(this->configFile);
            return boost::filesystem::absolute(confP).string();
        }

    private:
        std::string configFile;
        PlatformHelper pHelper;
        QDir *hostDir;

        //write pretty printed document into file
        void writeFormatedFileFromObj(rapidjson::Document &d) {
            auto fp = fopen(this->configFile.c_str(), "w");
            char writeBuffer[65536];
            rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
            rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);    
            d.Accept(writer);
            fclose(fp);
        }
        
        //get the config file and parse file content to variable
        rapidjson::Document accessConfigRaw() {

            //create path if not exist
            if (!hostDir->exists()) hostDir->mkpath(".");

            //check if exists, if not create valid json file
            if(!this->pHelper.fileExists(this->configFile)) {
                this->writeNewConfig();
            }

            //open file
            auto fp = fopen(this->configFile.c_str(), "r");
            char readBuffer[65536];
            rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
            rapidjson::Document d;
            rapidjson::ParseResult s = d.ParseStream(is);
            fclose(fp);

            if(s.IsError()) {
                this->writeNewConfig();
                return this->accessConfigRaw();
            }

            return d;
        }

        void onMissingRequiredMember(rapidjson::Document &config, std::function<void(std::string)> cb) {
            for (auto &rf : REQUIRED_CONFIG_FIELDS) {
                auto mem = config.FindMember(rf.c_str());
                if(mem == config.MemberEnd()) {
                    cb(rf);
                }
            }
        }

       //helper
       void onEmptyRequiredValue(rapidjson::Document &config, std::function<void()> cb) {
            for (auto &rf : REQUIRED_CONFIG_FIELDS) {
                auto mem = config.FindMember(rf.c_str());
                if(mem == config.MemberEnd() || !mem->value.IsString() || ((std::string)mem->value.GetString() == "")) {
                    cb();
                }
            }
        }

        //rewrite config file
        void writeNewConfig() {
            std::fstream streamHandler;
            streamHandler.open(this->configFile, std::fstream::out);
            streamHandler << "{}";
            streamHandler.close();
        }

        //create a parameter into the config file if it doesnt exist
        void createParamIfNotExist(rapidjson::Document &config, std::string paramToFind, std::string defVal = "") {
            
            auto mem = config.FindMember(paramToFind.c_str());
            rapidjson::Document::AllocatorType &alloc = config.GetAllocator();

                if(mem == config.MemberEnd()) {
                    rapidjson::Value param(paramToFind.c_str(), alloc);
                    rapidjson::Value val(defVal.c_str(), alloc);
                    config.AddMember(param, val, alloc);
                }
        }
};