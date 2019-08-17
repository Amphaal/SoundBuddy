#include "FeederThread.h"

FeederThread::FeederThread() {}
FeederThread::~FeederThread() {
    if(this->_ohLib) delete this->_ohLib;
    if(this->_ohWrn) delete this->_ohWrn;
    if(this->_workingJSON) delete this->_workingJSON;
    if(this->_libAsJSON) delete this->_libAsJSON;
    if(this->_libWarningsAsJSON) delete this->_libAsJSON;
}

void FeederThread::run() {

    this->_ohLib = new OutputHelper(OUTPUT_FILE_PATH, "uploadLib", "wtnz_file");
    this->_ohWrn = new OutputHelper(WARNINGS_FILE_PATH);

    emit printLog(I18n::tr()->Feeder_Warning());  //EMIT

    try {
        
        this->_generateLibJSONFile();
        this->_uploadLibToServer();

    } catch (const std::exception& e) {

        emit printLog(e.what(), false, true);  //EMIT

    }
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
        std::remove(pToRem.c_str());
    }

    emit printLog(I18n::tr()->Feeder_Unmolding(OUTPUT_FILE_PATH));  //EMIT

    this->_ohLib->writeAsJsonFile(*this->_libAsJSON);

    emit printLog(I18n::tr()->Feeder_OutputReady());  //EMIT

    emit operationFinished();  //EMIT
}


//upload
void FeederThread::_uploadLibToServer() {
    emit printLog(I18n::tr()->Feeder_StartSend());  //EMIT
    string response = this->_ohLib->uploadFile();
    if (response != "") {
        emit printLog(I18n::tr()->HTTP_ServerResponded(response));  //EMIT
    } else {
        emit printLog(I18n::tr()->HTTP_NoResponse());  //EMIT
    }
}


void FeederThread::_processFile(const std::string &xmlFileLocation) {
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
void FeederThread::_generateJSON(const std::string &xmlFileLocation) {
    
    emit printLog(I18n::tr()->Feeder_PredigestXML()); //EMIT
    
    //read xml as string
    iTunesLibParser *doc;
    try {
        doc = new iTunesLibParser(xmlFileLocation.c_str());
    } catch(...) {
        return throw FTNZXMLLibFileUnreadableException();
    }
    auto xmlAsJSONString = doc->ToJSON();
    delete doc;

    //try parse to temp JSON
    rapidjson::Document d;
    rapidjson::ParseResult s = d.Parse(xmlAsJSONString.c_str());
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
    rapidjson::Document::AllocatorType &lajAlloc = this->_libAsJSON->GetAllocator();
    rapidjson::Document::AllocatorType &lwajAlloc = this->_libWarningsAsJSON->GetAllocator();
    rapidjson::Document::AllocatorType &wjAlloc = this->_workingJSON->GetAllocator();

    //prepare
    set<string> tracksIdToRemove = {};
    this->_recCount = 0;
    this->_expectedCount = this->_workingJSON->MemberCount();
    
    //through each tracks
    for (auto track = this->_workingJSON->MemberBegin(); track != this->_workingJSON->MemberEnd(); ++track) {
        
        set<string> toRemove = {};
        set<string> foundRequired = {};

        //iterate through properties
        for (auto prop = track->value.MemberBegin(); prop != track->value.MemberEnd(); ++prop) {
            string k = prop->name.GetString();
            
            //check presence of required attrs
            if (_requiredAttrs.find(k) == _requiredAttrs.end()) {
                if (k != "Disc Number") toRemove.insert(k); //dont remove optional values !
            } else {
                foundRequired.insert(k);
            }
        }

        //check required attrs, else go warnings
        set<string> missingAttrs;
        set_difference(
            _requiredAttrs.begin(), _requiredAttrs.end(), 
            foundRequired.begin(), foundRequired.end(),
            inserter(missingAttrs, missingAttrs.end())
        );
        if (missingAttrs.size()) {
            rapidjson::Value key(track->value["Location"].GetString(), lwajAlloc);
            auto joined = StringHelper::join(missingAttrs, ", ");
            rapidjson::Value val(joined.c_str(), lwajAlloc);
            this->_libWarningsAsJSON->AddMember(key, val, lwajAlloc);
            tracksIdToRemove.insert(track->name.GetString());
        }

        //remove useless props
        for(auto ktr : toRemove) {
            track->value.RemoveMember(ktr.c_str());
        }

        //set optionnal values default
        if (!track->value.HasMember("Disc Number")) {
            track->value.AddMember("Disc Number","1", wjAlloc);
        }

        this->_tracksEmitHelper();
    }

    //remove tracks with warnings
    for(auto idtr : tracksIdToRemove) this->_workingJSON->RemoveMember(idtr.c_str());

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
string FeederThread::_getITunesLibLocation() {
    emit printLog(I18n::tr()->Feeder_GetXMLFileLoc());  //EMIT

    std::string pathToPrefs = PlatformHelper::getITunesPrefFileProbableLocation();
    
    try {
        return PlatformHelper::extractItunesLibLocation(pathToPrefs);
    } catch(...) {
        throw FTNZMissingItunesConfigException();
    }
    
}