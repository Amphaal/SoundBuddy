#include "QtCore/QObject"

class ITNZWorker : public QObject {
    public:
        virtual void run();
        ITNZWorker() {}

    signals:
        void printLog(const std::string &message);
};