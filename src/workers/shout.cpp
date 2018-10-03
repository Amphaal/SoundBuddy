#include "./ITNZWorker.h"
#include "QtWidgets/QWidget"

class ShoutWorker : public ITNZWorker {

    public:
        ShoutWorker(QWidget *parent) :  ITNZWorker(parent) {}
        
        void run() override {

        }
};