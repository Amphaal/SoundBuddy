#pragma once
#include <exception>
#include <string>
#include <rapidjson/document.h>

#include "src/workers/base/ITNZThread.h"
#include "src/helpers/_const.cpp"
#include "src/helpers/outputHelper/outputHelper.cpp"

class ShoutThread : public ITNZThread {
    public:
        ShoutThread();

        void run() override;
        
    private:
        OutputHelper _helper;
        size_t _lastTrackHash;
        bool _mustListen = false;

        rapidjson::Document _createBasicShout();
        void _shoutToServer(rapidjson::Document &incoming);

        void _shoutEmpty();
        void _shoutFilled(std::string name, std::string album, std::string artist, std::string genre, int duration, int playerPosition, bool playerState, int year);
        bool _shouldUpload(bool iPlayerState, std::string tName, std::string tAlbum, std::string tArtist, std::string tDatePlayed, std::string tDateSkipped);
};