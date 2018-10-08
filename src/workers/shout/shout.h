#pragma once
#include <exception>
#include <string>
#include "nlohmann/json.hpp"

#include "../base/ITNZWorker.h"
#include "../../helpers/outputHelper.cpp"

class ShoutWorker : public ITNZWorker {
    public:
        void exit() override;
        void run() override;
        ShoutWorker();
    
    private:
        bool mustListen = true;
        const std::string shoutFileName = "output\\shout.json";
        OutputHelper helper;
        nlohmann::json createBasicShout();
        void shoutEmpty();
        void shoutFilled(string name, string album, string artist, string genre, string duration, string playerPosition, string playerState);
        void shoutToServer(nlohmann::json *incoming);
};