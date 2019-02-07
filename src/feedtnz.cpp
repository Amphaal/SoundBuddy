#define _HAS_STD_BYTE 0 //prevent build error on mac (byte type overriding)
#include <QtCore/QString>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleFactory>
#include "ui/mainWindow.h"

#include "helpers/const.cpp"

#include <QDir>
#include <QLockFile>

int main(int argc, char** argv){
    
    //prevent multiples instances
    QString tmpDir = QDir::tempPath();
    QLockFile lockFile(tmpDir + "/feedtnz.lock");
    if(!lockFile.tryLock(100)){
        return 1;
    }

    //setup app
    QString title(APP_NAME.c_str());
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    app.setApplicationName(title);
    app.setStyle(QStyleFactory::create("Fusion")); 
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);

    //configure QThreads to acknowledge specific types for data exchanges
    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<size_t>("size_t");

    //fetch main window
    MainWindow mw(&title);

    //wait for the app to close
    return app.exec();
}