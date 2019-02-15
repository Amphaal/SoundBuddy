#pragma once
#include <exception>
#include <string>
#include <rapidjson/document.h>

#include "../base/ITNZWorker.h"
#include "../../helpers/_const.cpp"
#include "../../helpers/outputHelper.cpp"

class ShoutWorker : public ITNZWorker {
    public:
        ShoutWorker();
        void exit() override;
        void run() override;  
        void shoutEmpty();
        void shoutFilled(string name, string album, string artist, string genre, int duration, int playerPosition, bool playerState);
        bool shouldUpload(bool iPlayerState, string tName, string tAlbum, string tArtist, string tDatePlayed, string tDateSkipped);
        
    private:
        bool mustListen = true;
        OutputHelper helper;
        size_t lastTrackHash;

        rapidjson::Document createBasicShout();
        void shoutToServer(rapidjson::Document &incoming);
};