#include "nativeEvents.h"
#import <AppKit/AppKit.h>

MainWindowEventFilter::MainWindowEventFilter(QMainWindow *window) : window(window) {}

bool MainWindowEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{
    if (eventType == "mac_generic_NSEvent") {
        NSEvent *event = static_cast<NSEvent *>(message);
        if ([event type] == NSEventTypeKeyDown) {

        }
    }
    return false;
}

