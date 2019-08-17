#pragma once

#include <exception>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>

#include "src/helpers/platformHelper/platformHelper.h"
#include "src/localization/i18n.hpp"
#include "src/helpers/_const.hpp"
#include "src/version.h"

#include <QStandardPaths>
#include <QUrl>
#include <QDir>
#include <QFileInfo>

///
/// Exceptions
///

class FTNZMissingConfigValuesException : public std::exception {    
    private:
        QString exceptionMessage;

    public:
        FTNZMissingConfigValuesException() {
            this->exceptionMessage = I18n::tr()->FTNZMissingConfigValuesException();
        }  
        const char * what () const throw () {
            return this->exceptionMessage.toUtf8();
        }
};

///
/// End Exceptions
///

class ConfigHelper {
    
    public:
        ConfigHelper(
            const QString &rPathToConfigFile = APP_CONFIG_FILE_PATH, 
            const QVector<QString> &requiredFields = { AUTO_RUN_SHOUT_PARAM_NAME }
        ) : _requiredFields(requiredFields) {

            //set definitive location and create path if not exist
            auto hostPath = PlatformHelper::getDataStorageDirectory();
            this->_hostDir = new QDir(hostPath);
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
        void updateParamValue(const QString &paramToUpdate, const QString &value) {
            auto config = this->accessConfig();
            this->defineParamValue(config, paramToUpdate, value);

            this->writeFormatedFileFromObj(config);
        }

        //get the param value
        QString getParamValue(const QString &param) {
            auto config = this->accessConfig();
            auto mem = config.FindMember(param.toUtf8());
            if(mem == config.MemberEnd()) return "";
            auto cParam = param.toStdString().c_str();
            return !config[cParam].IsString() ? "" : config[cParam].GetString();
        }

        //get full path of the config file
        QString getConfigFileFullPath() {
            QFileInfo confP(this->_configFilePath);
            return confP.absoluteFilePath();
        }

    protected:
        QString _configFilePath;
        QDir* _hostDir = nullptr;
        QVector<QString> _requiredFields;


        //prepare data presentation for user to see
        void includeRequiredMembers(rapidjson::Document &config, bool mustWrite = false) {

            //check required field presence and adds them if missing
            rapidjson::Document::AllocatorType &alloc = config.GetAllocator();
            this->onMissingRequiredMember(config, [&mustWrite, &config, &alloc](QString rf){
                    rapidjson::Value n(rf.toUtf8(), alloc);
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
            auto fp = fopen(this->_configFilePath.toUtf8(), "w");
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
            auto fp = fopen(this->_configFilePath.toUtf8(), "r");
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


        void onMissingRequiredMember(rapidjson::Document &config, std::function<void(QString)> cb) {
            for (auto &rf : this->_requiredFields) {
                auto mem = config.FindMember(rf.toUtf8());
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
        void defineParamValue(rapidjson::Document &config, const QString &paramToFind, const QString &defVal = "") {
            
            auto mem = config.FindMember(paramToFind.toUtf8());
            rapidjson::Document::AllocatorType &alloc = config.GetAllocator();

                if(mem == config.MemberEnd()) {
                    rapidjson::Value param(paramToFind.toUtf8(), alloc);
                    rapidjson::Value val(defVal.toUtf8(), alloc);
                    config.AddMember(param, val, alloc);
                } else {
                    mem->value.SetString(defVal.toUtf8(), alloc);
                }
        }
};