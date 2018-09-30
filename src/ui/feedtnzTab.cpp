#include "QtCore/QString"
#include "QtWidgets/QWidget"
#include "QtWidgets/QPlainTextEdit"

using namespace std;

class FeedTNZTab : public QWidget {
    
    QPlainTextEdit *messages;
    
    public:
    FeedTNZTab(QWidget *parent) : QWidget(parent) {
        QPlainTextEdit messages(this);
        this->messages = &messages;
        this->messages->setReadOnly(true);
    }

    void messageHandler(string &message) {
        
        //handle linefeeds in appending
        QString before = this->messages->toPlainText();
        if(before != "") {
            before.append('\r');
        }
        before.append(QString::fromStdString(message));

        //update the Text Edit
        this->messages->setPlainText(before);

    }
};
