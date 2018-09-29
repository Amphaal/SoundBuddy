#include <QApplication>
#include <QIcon>
#include <QPushButton>

int main(int argc, char** argv){
    QApplication app(argc, argv);
    
    //Add app icon for windows
    QIcon mainIcon(":/src/resources/icon.ico");
    app.setWindowIcon(mainIcon);

    //inst
    QPushButton bouton("Salut les Zéros, la forme ?");
    bouton.show();
    
    return app.exec();
}