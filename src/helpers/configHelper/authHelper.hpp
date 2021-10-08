#pragma once

#include "configHelper.hpp"

class AuthHelper : public ConfigHelper {

    public:

        AuthHelper() : ConfigHelper(AUTH_FILE_PATH, REQUIRED_CONFIG_FIELDS) { }

        QString getUsersHomeUrl() {
            
            //try to get users name
            auto tUser = this->getParamValue("username");
            if (tUser.isEmpty()) return ""; //no user, no URL

            //no target ? no url
            auto tUrl = this->getTargetUrl();
            if(tUrl.isEmpty()) return "";
            
            //add user part to url
            auto newP = tUrl.toString() + "/" + tUser;
            QUrl ret(newP);

            //check validity
            if(!ret.isValid()) return "";
            return ret.toString();
        }

        //get the targeted platform url
        QUrl getTargetUrl() {
            
            //get parameterized targetUrl
            auto tUrl = this->getParamValue("targetUrl");
            if (tUrl == "") tUrl = APP_DEFAULT_URL;
            
            //check validity
            QUrl rlObj(tUrl, QUrl::TolerantMode);
            
            return rlObj;

        }

    
        //makes sure mandatory fields for uplaods are filled
        bool ensureConfigFileIsReadyForUpload(bool throwable = true) {

            //check required field presence
            bool isReady = true;
            this->onEmptyRequiredValue([&isReady](){
                    isReady = false;
            });
            if(!isReady && throwable) throw FTNZMissingConfigValuesException();

            return isReady;
        }

    private:
       void onEmptyRequiredValue(std::function<void()> cb) {
            auto config = this->accessConfig();
            for (auto &rf : _requiredFields) {
                auto mem = config.FindMember(rf.toStdString().c_str());
                if(mem == config.MemberEnd() || !mem->value.IsString() || ((QString)mem->value.GetString() == "")) {
                    cb();
                }
            }
        }

}; 