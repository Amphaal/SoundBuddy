#ifdef _WIN32

#include <windows.h>

#include <ActiveQt/QAxBase>
#include <ActiveQt/QAxObject>

#include "src/workers/shout/shout.h" 
#include "iTunesCOMHandler.h"
#include "src/helpers/stringHelper/stringHelper.cpp"
#include "src/localization/i18n.cpp"

#include <QMetaObject>
#include <QMetaMethod>
#include <QObject>
#include <QCoreApplication>

void ShoutWorker::run() {
    
    //start with log
    emit this->printLog(I18n::tr()->Shout_WaitITunes());
    HWND iTunesWindowsHandler;

    do {
        
        //prepare for process search...
        iTunesWindowsHandler = FindWindowA(0, "iTunes");

        //search for iTunes...
        if(iTunesWindowsHandler != NULL) {
            
            //log..
            emit printLog(I18n::tr()->Shout_StartListening());

            //initiate COM object
            CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
            wchar_t* wch = nullptr;

            //iTunes IID extracted from Apple API
            HRESULT hr = ::StringFromCLSID({0xDC0C2640,0x1415,0x4644,{0x87,0x5C,0x6F,0x4D,0x76,0x98,0x39,0xBA}}, &wch);
            QAxObject *iITunes = new QAxObject(QString::fromWCharArray(wch));
            iTunesCOMHandler *handler = new iTunesCOMHandler(iITunes, this);

            //bind events to sink handler
            auto oatputqe = QObject::connect(iITunes, iITunes->metaObject()->method(5), handler, handler->metaObject()->method(5));
            auto oppe = QObject::connect(iITunes, iITunes->metaObject()->method(9), handler, handler->metaObject()->method(6));
            auto opse = QObject::connect(iITunes, iITunes->metaObject()->method(11), handler, handler->metaObject()->method(7));

            //process events
            while(this->mustListen && !handler->iTunesShutdownRequested) {
                QCoreApplication::processEvents();
                this->msleep(20);
            }

            //clear COM usage
            this->shoutEmpty();
            iITunes->clear(); 
            CoUninitialize();

            //if iTunes is shutting down...
            if(this->mustListen && handler->iTunesShutdownRequested) {
                
                emit this->printLog(I18n::tr()->Shout_ITunesShutdown());

                //wait for itunes to finally shutdown
                do {
                    iTunesWindowsHandler = FindWindowA(0, "iTunes");
                    if(iTunesWindowsHandler != NULL) this->sleep(1);
                } while(this->mustListen && iTunesWindowsHandler != NULL);
                

                emit this->printLog(I18n::tr()->Shout_WaitITunesAgain());
            }

            //free
            QObject::disconnect(oatputqe);
            QObject::disconnect(oppe);
            QObject::disconnect(opse);
            delete handler;
            delete iITunes;

        } else {
            //if not found, wait and retry
            this->sleep(1);
        }
    } while (this->mustListen);

    //end with log
    emit printLog(I18n::tr()->Shout_StopListening());

};

#endif