#include "shout.h"
#include "src/localization/i18n.cpp"

ShoutWorker::ShoutWorker() : helper(OutputHelper(SHOUT_FILE_PATH, "uploadShout", "shout_file")) {};

rapidjson::Document ShoutWorker::createBasicShout() {
    
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

void ShoutWorker::shoutEmpty(){
    auto obj = this->createBasicShout();
    emit this->printLog(I18n::tr()->Shout_Nothing(obj["date"].GetString()));
    this->shoutToServer(obj);
};

void ShoutWorker::shoutFilled(string name, string album, string artist, string genre, int duration, int playerPosition, bool playerState, int year) {
    
    //fill obj
    auto obj = this->createBasicShout();
    rapidjson::Document::AllocatorType &alloc = obj.GetAllocator();

    //factory for value generation
    std::function<rapidjson::Value(std::string)> valGen = [&alloc](std::string defVal) {
        rapidjson::Value p(defVal.c_str(), alloc);
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
    string logMessage = I18n::tr()->Shout(
        obj["date"].GetString(),
        obj["name"].GetString(),
        obj["album"].GetString(),
        obj["artist"].GetString(),
        obj["playerState"].GetBool()
    );
    emit this->printLog(logMessage);

    this->shoutToServer(obj);
};

void ShoutWorker::shoutToServer(rapidjson::Document &incoming) {
    try {
        this->helper.writeAsJsonFile(incoming);
        this->helper.uploadFile();
    } catch(const std::exception& e) {
        emit this->printLog(e.what(), false, true);
    }
};

void ShoutWorker::exit() {
    this->mustListen = false;
}

//compare with old shout, if equivalent, don't reshout
bool ShoutWorker::shouldUpload(bool iPlayerState, string tName, string tAlbum, string tArtist, string tDatePlayed, string tDateSkipped) {
    
    size_t currHash = std::hash<std::string>{}(StringHelper::boolToString(iPlayerState) + tName + tAlbum + tArtist + (tDatePlayed >= tDateSkipped ? tDatePlayed : tDateSkipped));
    bool isHashIdentical = this->lastTrackHash == currHash;
    
    this->lastTrackHash = currHash;

    return !isHashIdentical;
}

#ifdef __APPLE__
    #include "mac/mac.cpp"
#endif
#ifdef _WIN32
    #include "win/win.cpp"
#endif