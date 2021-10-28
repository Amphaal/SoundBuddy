#pragma once

#include <QString>

#include <functional>

#include "src/workers/base/ITNZThread.hpp"

class ShoutThread : public ITNZThread {
    Q_OBJECT

 public:
    ShoutThread(const Uploader* uploder, const AppSettings::ConnectivityInfos connectivityInfos);

    void run() override;
    void quit() override;

    void shoutEmpty();

    void shoutFilled(
        const QString &name,
        const QString &album,
        const QString &artist,
        const QString &genre,
        int duration,
        int playerPosition,
        bool playerState,
        int year
    );

    bool shouldUpload(
        bool iPlayerState,
        const QString &tName,
        const QString &tAlbum,
        const QString &tArtist,
        const QString &tDatePlayed,
        const QString &tDateSkipped
    );

 private:
    QString _lastTrackHash;
    bool _mustListen = true;

    QJsonObject _createBasicShout() const;
    void _shoutToServer(const QJsonObject &incoming);
};
