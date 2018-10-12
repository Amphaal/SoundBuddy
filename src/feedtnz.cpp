#define _HAS_STD_BYTE 0 //prevent build error on mac (byte type overriding)
#include "QtCore/QString"
#include "QtWidgets/QApplication"
#include "QtWidgets/QStyleFactory"
#include "ui/mainWindow.cpp"

int main(int argc, char** argv){
    
    //setup app
    QString title("FeedTNZ");
    QApplication app(argc, argv);
    app.setApplicationName(title);
    app.setStyle(QStyleFactory::create("Fusion")); 
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);

    //configure threads
    qRegisterMetaType<std::string>("std::string");

    //fetch main window
    MainWindow mw(&title);

    //wait for the app to close
    return app.exec();
}