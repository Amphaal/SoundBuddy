#include "ShoutThread.h"

ShoutThread::ShoutThread() {};

ShoutThread::~ShoutThread() {
    if(this->_helper) delete this->_helper;
}

void ShoutThread::_inst() {
    this->_helper = new OutputHelper(SHOUT_FILE_PATH, "uploadShout", "shout_file");
}

void ShoutThread::quit() {
    this->_mustListen = false;
}

rapidjson::Document ShoutThread::_createBasicShout() {
    
    //get iso date
    time_t now;
    time(&now);
    char buf[sizeof "2011-10-08T07:07:09Z"];
    strftime(buf, sizeof buf, "%FT%TZ", gmtime(&now));
    
    //return json obj
    rapidjson::Document obj;
    obj.Parse("{}");
    rapidjson::Document::AllocatorType &alloc = obj.GetAllocator();
    auto dateAsJSONVal = rapidjson::Value(buf, alloc);
    obj.AddMember("date", dateAsJSONVal, alloc);
    return obj;
};

void ShoutThread::_shoutEmpty(){
    auto obj = this->_createBasicShout();
    emit this->printLog(I18n::tr()->Shout_Nothing(obj["date"].GetString()));
    this->_shoutToServer(obj);
};

void ShoutThread::_shoutFilled(QString name, QString album, QString artist, QString genre, int duration, int playerPosition, bool playerState, int year) {
    
    //fill obj
    auto obj = this->_createBasicShout();
    rapidjson::Document::AllocatorType &alloc = obj.GetAllocator();

    //factory for value generation
    auto valGen = [&alloc](QString defVal) {
        rapidjson::Value p(defVal.toUtf8(), alloc);
        return p;
    };

    obj.AddMember("name", valGen(name), alloc);
    obj.AddMember("album", valGen(album), alloc);
    obj.AddMember("artist", valGen(artist), alloc);
    obj.AddMember("genre", valGen(genre), alloc);
    obj.AddMember("duration", duration, alloc);
    obj.AddMember("playerPosition", playerPosition, alloc);
    obj.AddMember("playerState", playerState, alloc);
    obj.AddMember("year", year, alloc);

    //log...
    QString logMessage = I18n::tr()->Shout(
        obj["date"].GetString(),
        obj["name"].GetString(),
        obj["album"].GetString(),
        obj["artist"].GetString(),
        obj["playerState"].GetBool()
    );
    emit this->printLog(logMessage);

    this->_shoutToServer(obj);
};

void ShoutThread::_shoutToServer(rapidjson::Document &incoming) {
    try {
        this->_helper.writeAsJsonFile(incoming);
        this->_helper.uploadFile();
    } catch(const std::exception& e) {
        emit this->printLog(e.what(), false, true);
    }
};

//compare with old shout, if equivalent, don't reshout
bool ShoutThread::_shouldUpload(bool iPlayerState, QString tName, QString tAlbum, QString tArtist, QString tDatePlayed, QString tDateSkipped) {
    
    size_t currHash = std::hash<QString>{}(StringHelper::boolToString(iPlayerState) + tName + tAlbum + tArtist + (tDatePlayed >= tDateSkipped ? tDatePlayed : tDateSkipped));
    bool isHashIdentical = this->_lastTrackHash == currHash;
    
    this->_lastTrackHash = currHash;

    return !isHashIdentical;
}

#ifdef __APPLE__
    #include "mac/mac.cpp"
#endif
#ifdef _WIN32
    #include "win/win.cpp"
#endif