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

    //fetch main window
    MainWindow mw(&title);

    //wait for the app to close
    return app.exec();
}