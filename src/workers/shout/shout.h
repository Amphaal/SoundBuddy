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
        void shoutFilled(string name, string album, string artist, string genre, string duration, int playerPosition, bool playerState);
        
    private:
        bool mustListen = true;
        const std::string shoutFileName = "output\\shout.json";
        OutputHelper helper;
        nlohmann::json createBasicShout();
        void shoutToServer(nlohmann::json *incoming);
};