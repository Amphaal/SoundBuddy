#include "ShoutPayload.h"
#include "src/helpers/HashHelper.hpp"
#include <QDateTime>

QString ShoutPayload::hasChangedHash() const {
    return QString::number(iPlayerState) + 
        tName + 
        tAlbum + 
        tArtist + 
        QString::number(iPlayerPosMS) +
        (tDatePlayed >= tDateSkipped ? tDatePlayed : tDateSkipped);
}

QString ShoutPayload::getFileHash() const {
    //
    if (tFileLocation.isEmpty()) {
        return QString();
    }

    //
    return QString(calculateMD5(tFileLocation));
}

QJsonObject ShoutPayload::toTimestampedJSON() {
    // get ISO date, JS compatible (2011-10-08T07:07:09Z)
    auto currentTime = QDateTime::currentDateTimeUtc(); // Important ! we should not use local time, only UTC+0 !

    // return json obj
    QJsonObject obj;
    obj["date"] = currentTime.toString(Qt::DateFormat::ISODate);
    return obj;
}

ShoutJSONParsingResult ShoutPayload::toJSON() const
{
    auto obj = ShoutPayload::toTimestampedJSON();
    auto audioFileHash = getFileHash();
    const auto isEmptyShout = audioFileHash.isEmpty();

    if (!isEmptyShout) {
        obj["name"] = tName;
        obj["album"] = tAlbum;
        obj["artist"] = tArtist;
        obj["genre"] = tGenre;
        obj["duration"] = iDuration;
        obj["playerPositionMS"] = iPlayerPosMS;
        obj["playerState"] = iPlayerState;
        obj["year"] = tYear;
        obj["md5"] = audioFileHash;
    }

    return {
        std::move(obj),
        std::move(audioFileHash),
        isEmptyShout
    };
}
