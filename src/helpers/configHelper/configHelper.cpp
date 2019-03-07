#pragma once

#include <exception>
#include "libs/filesystem/path.hpp"

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>

#include "src/helpers/platformHelper/platformHelper.h"
#include "src/localization/i18n.cpp"
#include "src/helpers/_const.cpp"
#include "src/version.h"

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
        ConfigHelper(
            const std::string rPathToConfigFile = APP_CONFIG_FILE_PATH, 
            const std::vector<std::string> requiredFields = {AUTO_RUN_SHOUT_PARAM_NAME}
        ) : _requiredFields(requiredFields) {

            //set definitive location and create path if not exist
            std::string hostPath = PlatformHelper::getDataStorageDirectory();
            this->_hostDir = new QDir(hostPath.c_str());
            this->_configFilePath = hostPath + "/" + rPathToConfigFile;

        }

        ~ConfigHelper() {
            delete this->_hostDir;
        }
                
        //open the configuration file
        void openConfigFile() {
            PlatformHelper::openFileInOS(this->_configFilePath);
        }

        //update the current config file
        void updateParamValue(const std::string paramToUpdate, const std::string value) {
            auto config = this->accessConfig();
            this->defineParamValue(config, paramToUpdate, value);

            this->writeFormatedFileFromObj(config);
        }

        //get the param value
        std::string getParamValue(const std::string param) {
            auto config = this->accessConfig();
            auto mem = config.FindMember(param.c_str());
            if(mem == config.MemberEnd()) return "";
            return !config[param.c_str()].IsString() ? "" : config[param.c_str()].GetString();
        }

        //get full path of the config file
        std::string getConfigFileFullPath() {
            filesystem::path confP(this->_configFilePath);
            return confP.make_absolute().str();
        }

    protected:
        std::string _configFilePath;
        QDir *_hostDir;
        std::vector<std::string> _requiredFields;


        //prepare data presentation for user to see
        void includeRequiredMembers(rapidjson::Document &config, bool mustWrite = false) {

            //check required field presence and adds them if missing
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
        }

        //write pretty printed document into file
        void writeFormatedFileFromObj(rapidjson::Document &d) {
            auto fp = fopen(this->_configFilePath.c_str(), "w");
            char writeBuffer[65536];
            rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
            rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);    
            d.Accept(writer);
            fclose(fp);
        }
        
        //get the config file and parse file content to variable
        rapidjson::Document accessConfig() {

            //create path if not exist
            if (!this->_hostDir->exists()) this->_hostDir->mkpath(".");

            //check if exists, if not create valid json file
            if(!PlatformHelper::fileExists(this->_configFilePath)) {
                this->writeNewConfig();
            }

            //open file
            auto fp = fopen(this->_configFilePath.c_str(), "r");
            char readBuffer[65536];
            rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
            rapidjson::Document d;
            rapidjson::ParseResult s = d.ParseStream(is);
            fclose(fp);

            if(s.IsError()) {
                this->writeNewConfig();
                return this->accessConfig();
            }

            this->includeRequiredMembers(d);

            return d;
        }


        void onMissingRequiredMember(rapidjson::Document &config, std::function<void(std::string)> cb) {
            for (auto &rf : this->_requiredFields) {
                auto mem = config.FindMember(rf.c_str());
                if(mem == config.MemberEnd()) {
                    cb(rf);
                }
            }
        }

        //rewrite config file
        void writeNewConfig() {
            rapidjson::Document d;
            d.Parse("{}");
            includeRequiredMembers(d, true); //force writing
        }

        //create a parameter into the config file if it doesnt exist, else define its value
        void defineParamValue(rapidjson::Document &config, std::string paramToFind, std::string defVal = "") {
            
            auto mem = config.FindMember(paramToFind.c_str());
            rapidjson::Document::AllocatorType &alloc = config.GetAllocator();

                if(mem == config.MemberEnd()) {
                    rapidjson::Value param(paramToFind.c_str(), alloc);
                    rapidjson::Value val(defVal.c_str(), alloc);
                    config.AddMember(param, val, alloc);
                } else {
                    mem->value.SetString(defVal.c_str(), alloc);
                }
        }
};