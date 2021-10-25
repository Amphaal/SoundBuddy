#include "ShoutThread.h"

#include <string>

ShoutThread::ShoutThread() {}

ShoutThread::~ShoutThread() {
    if (this->_helper)
        delete this->_helper;
}

void ShoutThread::_inst() {
    this->_helper = new OutputHelper(SHOUT_FILE_PATH, "uploadShout", "shout_file");
}

void ShoutThread::quit() {
    this->_mustListen = false;
}

rapidjson::Document ShoutThread::_createBasicShout() {
    // get iso date
    time_t now;
    time(&now);
    char buf[sizeof "2011-10-08T07:07:09Z"];
    strftime(buf, sizeof buf, "%FT%TZ", gmtime(&now));

    // return json obj
    rapidjson::Document obj;
    obj.Parse("{}");
    rapidjson::Document::AllocatorType &alloc = obj.GetAllocator();
    auto dateAsJSONVal = rapidjson::Value(buf, alloc);
    obj.AddMember("date", dateAsJSONVal, alloc);
    return obj;
}

void ShoutThread::shoutEmpty() {
    auto obj = this->_createBasicShout();
    const auto date = obj["date"].GetString();

    emit printLog(
        tr("%1: Shouting -> Nothing").arg(date)
    );

    this->_shoutToServer(obj);
}

void ShoutThread::shoutFilled(
        const QString &name,
        const QString &album,
        const QString &artist,
        const QString &genre,
        int duration,
        int playerPosition,
        bool playerState,
        int year
    ) {
    // fill obj
    auto obj = this->_createBasicShout();
    rapidjson::Document::AllocatorType &alloc = obj.GetAllocator();

    // factory for value generation
    auto valGen = [&alloc](QString defVal) {
        rapidjson::Value p(defVal.toStdString().c_str(), alloc);
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

    auto pState = obj["playerState"].GetBool() ? tr("playing") : tr("paused");

    // log...
    auto logMessage =
        tr("%1: Shouting -> %2 - %3 - %4 (%5)")
            .arg(obj["date"].GetString())
            .arg(obj["name"].GetString())
            .arg(obj["album"].GetString())
            .arg(obj["artist"].GetString())
            .arg(pState);

    emit printLog(logMessage);

    this->_shoutToServer(obj);
}

void ShoutThread::_shoutToServer(rapidjson::Document &incoming) {
    try {
        this->_helper->writeAsJsonFile(incoming);
        this->_helper->uploadFile();
    } catch(const std::exception& e) {
        emit printLog(e.what(), false, true);
    }
}

// compare with old shout, if equivalent, don't reshout
bool ShoutThread::shouldUpload(
        bool iPlayerState,
        const QString &tName,
        const QString &tAlbum,
        const QString &tArtist,
        const QString &tDatePlayed,
        const QString &tDateSkipped
    ) {
    // hash blueprint
    const auto currHash = QString::number(iPlayerState) + tName + tAlbum + tArtist + (tDatePlayed >= tDateSkipped ? tDatePlayed : tDateSkipped);

    // check if strings are identical
    bool isHashIdentical = (this->_lastTrackHash == currHash);

    // replace old hash with new
    this->_lastTrackHash = currHash;

    // if not identical, shout !
    return !isHashIdentical;
}
