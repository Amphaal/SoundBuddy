#pragma once

#include "ITNZThread.h"

class ITNZWorker : public ITNZThread {
    
    public:
        virtual void exit() = 0;

};