#pragma once

#include <QString>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/filewritestream.h>
#include <curl/curl.h>


#include "src/helpers/stringHelper/stringHelper.hpp"
#include "src/helpers/configHelper/authHelper.hpp"
#include "src/localization/i18n.hpp"
#include "src/helpers/_const.hpp"

#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>

#include <exception>

///
/// Exceptions
///

class FTNZNoOutputFileException : public std::exception {
    private:
        std::string exceptionMessage;

    public:
        FTNZNoOutputFileException(QString outputPath) {
            this->exceptionMessage = I18n::tr()->FTNZNoOutputFileException(outputPath).toStdString();
        }
        const char* what() const throw () {   
            return this->exceptionMessage.c_str();
        }
};

class FTNZOutputFileUnreadableException : public std::exception {
    private:
        std::string exceptionMessage;

    public:
        FTNZOutputFileUnreadableException(QString outputPath) {
            this->exceptionMessage = I18n::tr()->FTNZOutputFileUnreadableException(outputPath).toStdString();
        }
        const char* what() const throw () {   
            return this->exceptionMessage.c_str();
        }
};

class FTNZErrorUploadingException : public std::exception {
    private:
            std::string exceptionMessage;

    public:
        FTNZErrorUploadingException(QString errorMessage) {
            this->exceptionMessage = I18n::tr()->FTNZErrorUploadingException(errorMessage).toStdString();
        }
        const char* what() const throw () {
            return this->exceptionMessage.c_str();
        }
};

class FTNZErrorProcessingUploadException : public std::exception {
    private:
            std::string exceptionMessage;

    public:
        FTNZErrorProcessingUploadException(long code, QString response) {
            StringHelper::replaceFirstOccurrence(response, "\n", "");
            this->exceptionMessage = I18n::tr()->FTNZErrorProcessingUploadException(code, response).toStdString();
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
        QFileInfo _pathToFile;
        QString _pathToCert;
        QString _uploadTargetFunction;
        QString _uploadTargetUrl;
        QMap<QString, QString> _uploadPostData;
        QString _uploadFileName;
        bool _mustPrepareUpload = true;

        //upload response reader
        static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
        {
            ((std::string*)userp)->append((char*)contents, size * nmemb);
            return size * nmemb;
        }

        void prepareUpload() {

            if(!this->_mustPrepareUpload) return;

            //check config file
            AuthHelper aHelper;
            aHelper.ensureConfigFileIsReadyForUpload();

            //harvest values
            auto userUrl = aHelper.getUsersHomeUrl();
            auto password = aHelper.getParamValue("password");

            //define them
            this->_uploadTargetUrl = userUrl + "/" + this->_uploadTargetFunction;
            this->_uploadPostData.clear();
            this->_uploadPostData.insert("password", password);
            this->_uploadPostData.insert("headless", "1");

            //prepared !
            this->_mustPrepareUpload = false;      
        }

    public:
        OutputHelper(const QString &filePath, const QString &targetFunction = "", const QString &uploadFileName = "") : 
            _uploadTargetFunction(targetFunction), 
            _uploadFileName(uploadFileName) {
            
            //set definitive location and create path if not exist
            auto hostPath = PlatformHelper::getDataStorageDirectory();
            auto pathToFile = hostPath + "/" + filePath;
            this->_pathToFile.setFile(pathToFile);
            
            //certificate
            this->_pathToCert = QDir::toNativeSeparators(
                (PlatformHelper::getAppDirectory() + "/" + PEM_CERT_NAME).toStdString().c_str()
            );
        }

        QString getOutputPath() {
            return this->_pathToFile.absoluteFilePath();
        }

        //write outputfile
        void writeAsJsonFile(rapidjson::Document &obj, bool mustPrettyPrint = false) {

            //get all path
            this->_pathToFile.dir().mkpath("."); //create dir if not exist

            //save on path
            auto fp = fopen(this->getOutputPath().toStdString().c_str(), "w");

            char writeBuffer[65536];
            rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

            if(mustPrettyPrint) {
                rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
                obj.Accept(writer);
            } else {
                rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
                obj.Accept(writer);
            }    
            fclose(fp);
        }

        QString uploadFile() {
            
            this->prepareUpload();

            /* In windows, this will init the winsock stuff */ 
            curl_global_init(CURL_GLOBAL_ALL);
            
            /* get a curl handle */ 
            std::string response;
            CURL *curl = curl_easy_init();
            std::exception_ptr futureException;

            if(curl) {

                curl_mime *form = NULL;
                curl_mimepart *field = NULL;

                /* Create the form */ 
                form = curl_mime_init(curl);
            
                /* Fill in the file upload field */ 
                field = curl_mime_addpart(form);
                curl_mime_name(field, this->_uploadFileName.toStdString().c_str());
                curl_mime_filedata(field, this->getOutputPath().toStdString().c_str());
            
                /* For each field*/
                for(auto i = this->_uploadPostData.begin(); i != this->_uploadPostData.end(); i++) {
                    field = curl_mime_addpart(form);
                    curl_mime_name(field, i.key().toStdString().c_str());
                    curl_mime_data(field, i.value().toStdString().c_str(), CURL_ZERO_TERMINATED);
                }

                /* what URL that receives this POST */ 
                curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);

                //header
                struct curl_slist *list = NULL;
                auto localeHeader = QString("Accept-Language: ") + I18n::getLocaleName();
                list = curl_slist_append(list, localeHeader.toStdString().c_str());
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
                
                //response text
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

                /* try use of SSL for this */
                curl_easy_setopt(curl, CURLOPT_CAINFO, this->_pathToCert.toStdString().c_str());
                curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 1000L);


                //url and execute
                curl_easy_setopt(curl, CURLOPT_URL, this->_uploadTargetUrl.toStdString().c_str()); 
                CURLcode res = curl_easy_perform(curl); 
                
                if(res != CURLE_OK) {
                    auto descr = curl_easy_strerror(res);
                    futureException = std::make_exception_ptr<FTNZErrorUploadingException>(FTNZErrorUploadingException(descr)); 
                } else {
                    //response code
                    long code;
                    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
                    
                    //if http code is not OK
                    if(code != 200) {
                        this->_mustPrepareUpload = true; //reprepare
                        futureException = std::make_exception_ptr<FTNZErrorProcessingUploadException>(FTNZErrorProcessingUploadException(code, QString::fromStdString(response)));
                    }
                }

                curl_mime_free(form); /* then cleanup the form */ 
                curl_easy_cleanup(curl); /* always cleanup */ 
            }

            curl_global_cleanup(); /* always cleanup */ 
            
            if(futureException) {
                std::rethrow_exception(futureException);
                return "";
            } else {
                return QString::fromStdString(response);
            }
        }
};