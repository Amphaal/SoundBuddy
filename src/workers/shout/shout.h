#pragma once
#include "../base/ITNZWorker.h"
#include <exception>

class ShoutWorker : public ITNZWorker {
    public:
        bool mustListen = true;
        void exit();
        void run() override;
};