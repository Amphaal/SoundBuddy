#include <QString>
#include <QApplication>
#include <QStyleFactory>
#include "ui/mainWindow/mainWindow.h"

#include "helpers/_const.hpp"

#include <QDir>
#include <QLockFile>

int main(int argc, char** argv){
    
    //prevent multiples instances
    QString tmpDir = QDir::tempPath();
    QLockFile lockFile(tmpDir + "/" + APP_NAME + ".lock");
    if(!lockFile.tryLock(100)){
        return 1;
    }

    //setup app
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    app.setApplicationName(QString(APP_NAME));
    app.setOrganizationName(QString(APP_PUBLISHER));
    app.setStyle(QStyleFactory::create("Fusion")); 
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);

    //configure QThreads to acknowledge specific types for data exchanges
    qRegisterMetaType<size_t>("size_t");
    qRegisterMetaType<TLW_Colors>("TLW_Colors");

    //fetch main window
    MainWindow mw;

    //wait for the app to close
    return app.exec();
}