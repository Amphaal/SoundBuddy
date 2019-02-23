
#pragma once

#include "configHelper.cpp"

class AuthHelper : public ConfigHelper {

    public:

        AuthHelper() : ConfigHelper(AUTH_FILE_PATH, REQUIRED_CONFIG_FIELDS) { }

        std::string getUsersHomeUrl() {
            
            //try to get users name
            auto tUser = this->getParamValue("username");
            if (tUser == "") return ""; //no user, no URL

            //no target ? no url
            auto tUrl = this->getTargetUrl();
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
        QUrl* getTargetUrl() {
            
            //get parameterized targetUrl
            auto tUrl = this->getParamValue("targetUrl");
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
                auto mem = config.FindMember(rf.c_str());
                if(mem == config.MemberEnd() || !mem->value.IsString() || ((std::string)mem->value.GetString() == "")) {
                    cb();
                }
            }
        }

}; 