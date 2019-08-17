#pragma once
#include <exception>
#include <QString>
#include <rapidjson/document.h>

#include "src/workers/base/ITNZThread.h"
#include "src/helpers/_const.hpp"
#include "src/helpers/outputHelper/outputHelper.cpp"

class ShoutThread : public ITNZThread {
    public:
        ShoutThread();
        ~ShoutThread();

        void run() override;
        void quit() override;
        
    private:
        void _inst();

        OutputHelper* _helper = nullptr;
        size_t _lastTrackHash;
        bool _mustListen = false;

        rapidjson::Document _createBasicShout();
        void _shoutToServer(rapidjson::Document &incoming);

        void _shoutEmpty();
        void _shoutFilled(QString name, QString album, QString artist, QString genre, int duration, int playerPosition, bool playerState, int year);
        bool _shouldUpload(bool iPlayerState, QString tName, QString tAlbum, QString tArtist, QString tDatePlayed, QString tDateSkipped);
};