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
            QDir hostDir(hostPath.c_str());
            if (!hostDir.exists()) hostDir.mkpath(".");
            this->configFile = hostPath + "/" + CONFIG_FILE_PATH;

            //check if exists, if not create valid json file
            if(!this->fileExists(this->configFile)) {
                std::fstream streamHandler;
                streamHandler.open(this->configFile, std::fstream::out);
                streamHandler << "{}";
                streamHandler.close();
            }

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
            this->onEmptyValues(config, [&mustThrow](){
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
            this->onMissingMember(config, [&mustWrite, &config](std::string rf){
                    config[rf.c_str()] = "";
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
        void updateConfigFile(std::string paramToUpdate, std::string value) {
            auto config = this->accessConfigRaw();
            config[paramToUpdate.c_str()] = value;
            this->writeFormatedFileFromObj(config);
        }

        //ensure a file exists
        bool fileExists(std::string outputFileName) {
            boost::filesystem::path confP(outputFileName);
            confP = boost::filesystem::absolute(confP);
            return boost::filesystem::exists(confP);
        }

        std::string getConfigFileFullPath() {
            boost::filesystem::path confP(this->configFile);
            return boost::filesystem::absolute(confP).string();
        }

    private:
        std::string configFile;
        PlatformHelper pHelper;

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
            auto fp = fopen(this->configFile.c_str(), "r");
            char readBuffer[65536];
            rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
            rapidjson::Document d;
            d.ParseStream(is);
            fclose(fp);
            return d;
        }

        void onMissingMember(rapidjson::Document &config, std::function<void(std::string)> cb) {
            for (auto &rf : REQUIRED_CONFIG_FIELDS) {
                auto mem = config.FindMember(rf.c_str());
                if(mem == config.MemberEnd() || !mem->value.IsString()) {
                    cb(rf);
                }
            }
        }

       void onEmptyValues(rapidjson::Document &config, std::function<void()> cb) {
            for (auto &rf : REQUIRED_CONFIG_FIELDS) {
                auto mem = config.FindMember(rf.c_str());
                if(mem == config.MemberEnd() || mem->value == "") {
                    cb();
                }
            }
        }
};