#include <QAbstractNativeEventFilter>
#include "QtWidgets/QMainWindow"

class MainWindowEventFilter : public QAbstractNativeEventFilter
{
    public:
        MainWindowEventFilter(QMainWindow *window);
        bool nativeEventFilter(const QByteArray &eventType, void *message, long *) override;
    private:
    QMainWindow *window;
};