#pragma once

#include <QString>

#include <functional>

#include "src/workers/base/ITNZThread.hpp"

class ShoutThread : public ITNZThread {
    Q_OBJECT

 public:
    ShoutThread(const AppSettings::ConnectivityInfos &connectivityInfos);

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
    const AppSettings::ConnectivityInfos _connectivityInfos;
    QString _lastTrackHash;
    bool _mustListen = true;

    QJsonDocument _createBasicShout();
    void _shoutToServer(QJsonDocument &incoming);
};
