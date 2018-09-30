#include "QtCore/QString"
#include "QtCore/QSize"
#include "QtCore/QObject"
#include "QtWidgets/QTabWidget"
#include "QtWidgets/QMainWindow"
#include "QtWidgets/QWidget"
#include "QtWidgets/QMenuBar"
#include "QtWidgets/QMenu"
#include "QtWidgets/QAction"
#include "QtWidgets/QSystemTrayIcon"
#include "QtGui/QCloseEvent"
#include "QtGui/QHideEvent"
#include "QtGui/QIcon"

#include "feedtnzTab.cpp"
#include "../helpers/uiHelper.cpp"

class MainWindow : public QMainWindow {

    QString *title;
    QSystemTrayIcon *trayIcon;
    
    public:
    MainWindow(QString *title) {
        this->title = title;
        this->_initUI();
    }
    
    private:

    void _initUI() {
        this->setMinimumSize(QSize(480, 400));
        this->_initUITabs();
        this->_initUIMenu();
        this->_initUITray();
        this->show();
    }

    void _initUITabs() {
        QTabWidget *tabs = new QTabWidget;
        FeedTNZTab *shoutTab = new FeedTNZTab(tabs);
        FeedTNZTab *feedTab = new FeedTNZTab(tabs);

        tabs->addTab(shoutTab, "Shout!");
        tabs->addTab(feedTab, "Feeder");

        this->setCentralWidget(tabs);
    }

    void _initUIMenu() {
        //Menu
        QMenuBar *menu = new QMenuBar;
        QMenu *fileMenuItem = menu->addMenu("File");

        /*Actions*/
        //myWTNZAction
        QAction *myWTNZAction = new QAction("My WTNZ", fileMenuItem);
        myWTNZAction->setEnabled(false);
        QObject::connect(
            myWTNZAction, &QAction::triggered,
            this, &MainWindow::accessWTNZ
        );

        //updateConfigAction
        QAction *updateConfigAction = new QAction("Update configuration file", fileMenuItem);
        QObject::connect(
            updateConfigAction, &QAction::triggered,
            this, &MainWindow::openConfigFile
        );

        //bind
        fileMenuItem->addAction(myWTNZAction);
        fileMenuItem->addAction(updateConfigAction);
        this->setMenuWidget(menu);
    }

    void _initUITray() {
        QSystemTrayIcon *trayIcon = new QSystemTrayIcon;
        this->trayIcon = trayIcon;
        trayIcon->setIcon(QIcon(":/icons/feedtnz.png"));
        trayIcon->setToolTip(*this->title);
        QObject::connect(
           trayIcon, &QSystemTrayIcon::activated,
           this, &MainWindow::trueShow
        );
        trayIcon->show();
    }

    void accessWTNZ() {
        
    }

    void openConfigFile() {

    }


    void MainWindow::closeEvent (QCloseEvent *event) {
        this->trayIcon->hide();
    }

    void MainWindow::hideEvent (QHideEvent *event) {
        event->ignore();
        this->hide();
    }

    void MainWindow::trueShow(QSystemTrayIcon::ActivationReason reason) {
        this->raise();
        this->activateWindow();
        this->showNormal();
    }
};
