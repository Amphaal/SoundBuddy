#include "shout.h"
#include "../../localization/i18n.cpp"

ShoutWorker::ShoutWorker() : helper(OutputHelper(this->shoutFileName, "uploadShout", "shout_file")) {};

nlohmann::json ShoutWorker::createBasicShout() {
    
    //get iso date
    time_t now;
    time(&now);
    char buf[sizeof "2011-10-08T07:07:09Z"];
    strftime(buf, sizeof buf, "%FT%TZ", gmtime(&now));
    
    //return json obj
    nlohmann::json obj = {};
    obj["date"] = buf;
    return obj;
};

void ShoutWorker::shoutEmpty(){
    nlohmann::json obj = this->createBasicShout();
    emit this->printLog(I18n::tr()->Shout_Nothing(obj["date"].get<string>()));
    this->shoutToServer(&obj);
};

void ShoutWorker::shoutFilled(string name, string album, string artist, string genre, int duration, int playerPosition, bool playerState) {
    
    //fill obj
    nlohmann::json obj = this->createBasicShout();
    obj["name"] = name;
    obj["album"] = album;
    obj["artist"] = artist;
    obj["genre"] = genre;
    obj["duration"] = duration;
    obj["playerPosition"] = playerPosition;
    obj["playerState"] = playerState;

    //log...
    string logMessage = I18n::tr()->Shout(
        obj["date"].get<string>(),
        obj["name"].get<string>(),
        obj["album"].get<string>(),
        obj["artist"].get<string>(),
        obj["playerState"].get<bool>()
    );
    emit this->printLog(logMessage);

    this->shoutToServer(&obj);
};

void ShoutWorker::shoutToServer(nlohmann::json *incoming) {
    try {
        this->helper.writeAsJsonFile(incoming);
        this->helper.uploadFile();
    } catch(const std::exception& e) {
        emit this->printLog(e.what());
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