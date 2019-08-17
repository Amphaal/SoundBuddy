#pragma once

#include <functional>
#include <exception>
#include <QString>
#include <rapidjson/document.h>

#include "src/workers/base/ITNZThread.hpp"
#include "src/helpers/_const.hpp"
#include "src/helpers/outputHelper/outputHelper.hpp"

class ShoutThread : public ITNZThread {
    public:
        ShoutThread();
        ~ShoutThread();

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
        void _inst();

        OutputHelper* _helper = nullptr;
        size_t _lastTrackHash;
        bool _mustListen = true;

        rapidjson::Document _createBasicShout();
        void _shoutToServer(rapidjson::Document &incoming);
};