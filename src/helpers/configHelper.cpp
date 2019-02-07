#pragma once

#include <exception>
#include <boost/filesystem.hpp>
#include <nlohmann/json.hpp>

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
    
        //constructor
        ConfigHelper() : streamHandler(std::fstream()), pHelper(PlatformHelper()) {

            //set definitive location and create path if not exist
            std::string hostPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation).toStdString();
            QDir hostDir(hostPath.c_str());
            if (!hostDir.exists()) hostDir.mkpath(".");
            this->configFile = hostPath + "/" + CONFIG_FILE_PATH;

        }
                
        //open the configuration file
        void openConfigFile() {
            this->pHelper.openFileInOS(this->configFile);
        }

        //makes sure mandatory fields for uplaods are filled
        bool ensureConfigFileIsReadyForUpload() {
            nlohmann::json config = this->accessConfigRaw();

            //check required field presence and adds them if missing
            for (auto &rf : REQUIRED_CONFIG_FIELDS) {  
                if (config[rf] == nullptr || config[rf] == "") {
                    throw FTNZMissingConfigValuesException();
                    return false;
                }
            }

            return true;
        }

        //get configuration data from file
        nlohmann::json accessConfig() {
            
            //check if exists, if not create file
            if(!this->fileExists(this->configFile)) {
                this->streamHandler.open(this->configFile, std::fstream::out);
                this->streamHandler.close();
            }
            
            nlohmann::json config = this->accessConfigRaw();

            //check required field presence and adds them if missing
            bool mustWrite = false;
            for (auto &rf : REQUIRED_CONFIG_FIELDS) {
                if (config.find(rf) == config.end()) {
                    config[rf] = nullptr;
                    mustWrite = true;
                }
            }

            //re-write as formated string
            if(mustWrite) {
                this->writeFormatedFileFromObj(&config);
            }
            
            //return values
            return config;
        }

        //update the current config file
        void updateConfigFile(std::string paramToUpdate, std::string value) {
            this->streamHandler.open(this->configFile, std::fstream::in);
                nlohmann::json config;
                this->streamHandler >> config;
                config[paramToUpdate] = value;
            this->streamHandler.close();
            this->writeFormatedFileFromObj(&config);
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
        std::fstream streamHandler;
        PlatformHelper pHelper;

        //write
        std::string writeFormatedFileFromObj(nlohmann::json *obj) {
            this->streamHandler.open(this->configFile, std::fstream::out);
            std::string result = obj->dump(4);
            this->streamHandler << result << std::endl;
            this->streamHandler.close();
            return result;
        }
        
        //get the config file and parse file content to variable
        nlohmann::json accessConfigRaw() {
            nlohmann::json config;
            this->streamHandler.open(this->configFile, std::fstream::in);
                try {
                    this->streamHandler >> config;
                } catch(const std::exception&) {
                    config = {};
                }
            this->streamHandler.close();
            return config;
        }
};