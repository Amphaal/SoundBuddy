#pragma once
#include "base/ITNZWorker.h"

class FeederWorker : public ITNZWorker {
    bool mustListen = true;
    void exit();
};