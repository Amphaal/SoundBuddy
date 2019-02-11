#pragma once

#include <exception>
#include <boost/filesystem.hpp>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>

#include "platformHelper/platformHelper.h"
#include "../localization/i18n.cpp"
#include "const.cpp"

#include <QStandardPaths>
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

        //makes sure mandatory fields for uplaods are filled
        bool ensureConfigFileIsReadyForUpload() {
            auto config = this->accessConfigRaw();

            //check required field presence
            bool mustThrow = false;
            this->onEmptyRequiredValue(config, [&mustThrow](){
                    mustThrow = true;
            });
            if(mustThrow) throw FTNZMissingConfigValuesException();

            return true;
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

        std::string getParamValue(rapidjson::Document &config, std::string param) {
            createParamIfNotExist(config, param);
            return !config[param.c_str()].IsString() ? "" : config[param.c_str()].GetString();
        }

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

       void onEmptyRequiredValue(rapidjson::Document &config, std::function<void()> cb) {
            for (auto &rf : REQUIRED_CONFIG_FIELDS) {
                auto mem = config.FindMember(rf.c_str());
                if(mem == config.MemberEnd() || !mem->value.IsString() || ((std::string)mem->value.GetString() == "")) {
                    cb();
                }
            }
        }

        void writeNewConfig() {
            std::fstream streamHandler;
            streamHandler.open(this->configFile, std::fstream::out);
            streamHandler << "{}";
            streamHandler.close();
        }

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