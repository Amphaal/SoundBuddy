#include <QString>
#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QLockFile>

#include "ui/monitor/mainWindow.h"

int main(int argc, char** argv) {
    // prevent multiples instances
    QLockFile lockFile(QDir::tempPath() + '/' + APP_NAME + ".lock");
    if (!lockFile.tryLock(100)) {
        return 1;
    }

    // setup app
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    app.setApplicationName(APP_NAME);
    app.setOrganizationName(APP_PUBLISHER);
    app.setStyle(QStyleFactory::create("Fusion"));

    // fetch main window
    MainWindow mw;

    // wait for the app to close
    return app.exec();
}
