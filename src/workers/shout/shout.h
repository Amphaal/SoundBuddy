#pragma once
#include <exception>
#include <string>
#include "nlohmann/json.hpp"

#include "../base/ITNZWorker.h"
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
        const std::string shoutFileName = "output\\shout.json";
        OutputHelper helper;
        nlohmann::json createBasicShout();
        void shoutToServer(nlohmann::json *incoming);
        size_t lastTrackHash;
};