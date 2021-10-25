#include "src/workers/connectivity/ConnectivityThread.sio.h"  // must be included foremost

#include <QString>
#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QLockFile>

#include "ui/monitor/mainWindow.h"

int main(int argc, char** argv) {
    // prevent multiples instances
    QString tmpDir = QDir::tempPath();
    QLockFile lockFile(tmpDir + "/" + APP_NAME + ".lock");
    if (!lockFile.tryLock(100)) {
        return 1;
    }

    // setup app
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    app.setApplicationName(APP_NAME);
    app.setOrganizationName(APP_PUBLISHER);
    app.setStyle(QStyleFactory::create("Fusion"));

    // configure QThreads to acknowledge specific types for data exchanges
    qRegisterMetaType<size_t>("size_t");
    qRegisterMetaType<TLW_Colors>("TLW_Colors");

    // fetch main window
    MainWindow mw;

    // wait for the app to close
    return app.exec();
}
