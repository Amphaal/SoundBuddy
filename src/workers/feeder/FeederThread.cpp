#include "FeederThread.h"

FeederThread::FeederThread() {}

void FeederThread::run() {

    this->_ohLib = new OutputHelper(OUTPUT_FILE_PATH, "uploadLib", "wtnz_file");
    this->_ohWrn = new OutputHelper(WARNINGS_FILE_PATH);

    emit printLog(I18n::tr()->Feeder_Warning());  //EMIT

    try {
        
        this->_generateLibJSONFile();
        this->_uploadLibToServer();

    } catch (const std::exception& e) {

        QString errMsg(e.what());
        emit printLog(errMsg, false, true);  //EMIT

    }

    //clear
    delete this->_ohLib;
    delete this->_ohWrn;
    delete this->_workingJSON;
    delete this->_libAsJSON;
    delete this->_libWarningsAsJSON;

}

//generate files
void FeederThread::_generateLibJSONFile() {
    auto itnzLibPath = this->_getITunesLibLocation();
    this->_processFile(itnzLibPath);

    //check warnings
    auto warningsCount = this->_libWarningsAsJSON->MemberCount();
    if (warningsCount) {
        //create warning file
        emit printLog(I18n::tr()->Feeder_NotifyWarningsExistence(warningsCount, OUTPUT_FILE_PATH));  //EMIT

        emit printLog(I18n::tr()->Feeder_Unmolding(WARNINGS_FILE_PATH));  //EMIT
        this->_ohWrn->writeAsJsonFile(*this->_libWarningsAsJSON, true);
    } else {
        //remove old warning file
        auto pToRem = this->_ohWrn->getOutputPath();
        QFile::remove(pToRem);
    }

    emit printLog(I18n::tr()->Feeder_Unmolding(OUTPUT_FILE_PATH));  //EMIT

    this->_ohLib->writeAsJsonFile(*this->_libAsJSON);

    emit printLog(I18n::tr()->Feeder_OutputReady());  //EMIT

    emit operationFinished();  //EMIT
}


//upload
void FeederThread::_uploadLibToServer() {
    emit printLog(I18n::tr()->Feeder_StartSend());  //EMIT
    
    QString response = this->_ohLib->uploadFile();
    
    if (response != "") {
        emit printLog(I18n::tr()->HTTP_ServerResponded(response));  //EMIT
    } else {
        emit printLog(I18n::tr()->HTTP_NoResponse());  //EMIT
    }

}


void FeederThread::_processFile(const QString &xmlFileLocation) {
    this->_recCount = 0;
    this->_expectedCount = 0;

    this->_libWarningsAsJSON = new rapidjson::Document;
    this->_workingJSON = new rapidjson::Document;
    this->_libAsJSON = new rapidjson::Document;

    //set default
    this->_libWarningsAsJSON->Parse("{}");
    this->_workingJSON->Parse("{}");
    this->_libAsJSON->Parse("[]");

    //format to dict
    this->_generateJSON(xmlFileLocation);
    this->_standardizeJSON();    
}

//navigate through XML and generate object
void FeederThread::_generateJSON(const QString &xmlFileLocation) {
    
    emit printLog(I18n::tr()->Feeder_PredigestXML()); //EMIT
    
    //read xml as QString
    iTunesLibParser *doc;
    try {
        doc = new iTunesLibParser(xmlFileLocation);
    } catch(...) {
        return throw FTNZXMLLibFileUnreadableException();
    }
    auto xmlAsJSONString = doc->ToJSON();
    delete doc;

    //try parse to temp JSON
    rapidjson::Document d;
    rapidjson::ParseResult s = d.Parse(xmlAsJSONString.toStdString().c_str());
    if(s.IsError()) {
        return throw FTNZXMLLibFileUnreadableException();
    }

    emit printLog(I18n::tr()->Feeder_TrimingFat());  //EMIT
    
    //retrieve tracks and pass to workingJSON
    auto v = rapidjson::Pointer("/Tracks").Get(d);
    this->_expectedCount = v->MemberCount();
    if (!this->_expectedCount) {
        return throw FTNZNoMusicFoundException();
    }
    this->_workingJSON->CopyFrom(d["Tracks"], this->_workingJSON->GetAllocator());
    
}

//standardize
void FeederThread::_standardizeJSON() {
    
    emit printLog(I18n::tr()->Feeder_CookingJSON());  //EMIT

    //declare allocators
    auto &lajAlloc = this->_libAsJSON->GetAllocator();
    auto &lwajAlloc = this->_libWarningsAsJSON->GetAllocator();
    auto &wjAlloc = this->_workingJSON->GetAllocator();

    //prepare
    QSet<QString> tracksIdToRemove = {};
    this->_recCount = 0;
    this->_expectedCount = this->_workingJSON->MemberCount();
    
    //through each tracks
    for (auto track = this->_workingJSON->MemberBegin(); track != this->_workingJSON->MemberEnd(); ++track) {
        
        QSet<QString> toRemove = {};
        std::set<QString> foundRequired;

        //iterate through properties
        for (auto prop = track->value.MemberBegin(); prop != track->value.MemberEnd(); ++prop) {
            QString k = prop->name.GetString();
            
            //check presence of required attrs
            if (_requiredAttrs.find(k) == _requiredAttrs.end()) {
                if (k != "Disc Number") toRemove.insert(k); //dont remove optional values !
            } else {
                foundRequired.insert(k);
            }
        }

        //apply diff on required attr, dump into missingAttrs
        QStringList missingAttrs;
        std::set_difference(
            _requiredAttrs.begin(), _requiredAttrs.end(), 
            foundRequired.begin(), foundRequired.end(),
            std::inserter(missingAttrs, missingAttrs.end())
        );

        //if there are missing attrs
        if (missingAttrs.length()) {
            rapidjson::Value key(track->value["Location"].GetString(), lwajAlloc);
            auto joined = missingAttrs.join(", ");
            rapidjson::Value val(joined.toStdString().c_str(), lwajAlloc);
            this->_libWarningsAsJSON->AddMember(key, val, lwajAlloc);
            tracksIdToRemove.insert(track->name.GetString());
        }

        //remove useless props
        for(auto ktr : toRemove) {
            track->value.RemoveMember(ktr.toStdString().c_str());
        }

        //set optionnal values default
        if (!track->value.HasMember("Disc Number")) {
            track->value.AddMember("Disc Number","1", wjAlloc);
        }

        this->_tracksEmitHelper();
    }

    qDebug() << this->_workingJSON->MemberCount();

    //remove tracks with warnings
    for(auto &idtr : tracksIdToRemove) {
        this->_workingJSON->RemoveMember(idtr.toStdString().c_str());
    }

    qDebug() << this->_workingJSON->MemberCount();

    //turn obect based container into an array one
    for (auto track = this->_workingJSON->MemberBegin(); track != this->_workingJSON->MemberEnd(); ++track) {
        this->_libAsJSON->PushBack(track->value, lajAlloc);
    }
}

///
/// Other Helpers
///

//log...
void FeederThread::_tracksEmitHelper() {
    bool mustReplacePrevious = this->_recCount;
    this->_recCount++;
    bool canLog = ((this->_recCount % 100) == 0 && this->_recCount <= this->_expectedCount) || this->_recCount == this->_expectedCount || !mustReplacePrevious;
    if(canLog) {
        emit printLog(I18n::tr()->Feeder_LogTrackEmit(this->_recCount, this->_expectedCount), mustReplacePrevious);  //EMIT
    }
}

//seek in iTunes preference file the library location
QString FeederThread::_getITunesLibLocation() {
    emit printLog(I18n::tr()->Feeder_GetXMLFileLoc());  //EMIT

    QString pathToPrefs = PlatformHelper::getITunesPrefFileProbableLocation();
    
    try {
        return PlatformHelper::extractItunesLibLocation(pathToPrefs);
    } catch(...) {
        throw FTNZMissingItunesConfigException();
    }
    
}