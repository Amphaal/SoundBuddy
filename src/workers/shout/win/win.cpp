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
#include <QDebug>

void ShoutWorker::run() {
    
    //start with log
    emit this->printLog(I18n::tr()->Shout_WaitITunes());
    
    //prepare CLID
    HWND currentITunesWindowsHandler;
    DWORD currentProcessID;

    //iTunes IID extracted from Apple API
    wchar_t* wch = nullptr;
    HRESULT hr = ::StringFromCLSID({0xDC0C2640,0x1415,0x4644,{0x87,0x5C,0x6F,0x4D,0x76,0x98,0x39,0xBA}}, &wch);
    auto iTunesCLID = QString::fromWCharArray(wch);

    do {
        
        //search for iTunes...
        currentITunesWindowsHandler = FindWindowA(0, "iTunes");
        
        //if not found, wait and retry
        if(!currentITunesWindowsHandler) {
            this->sleep(1);
            continue;
        } 

        //iTunes found, store the associated PID
        GetWindowThreadProcessId(currentITunesWindowsHandler, &currentProcessID);
        
        //log..
        emit printLog(I18n::tr()->Shout_StartListening());

        //initiate COM object
        CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
        wchar_t* wch = nullptr;

        //iTunes IID extracted from Apple API
        QAxObject *iITunes = new QAxObject(iTunesCLID);
        iTunesCOMHandler *handler = new iTunesCOMHandler(iITunes, this);      

        //bind events to sink handler
        auto oatputqe = QObject::connect(iITunes, iITunes->metaObject()->method(5), handler, handler->metaObject()->method(5));
        auto oppe = QObject::connect(iITunes, iITunes->metaObject()->method(9), handler, handler->metaObject()->method(6));
        auto opse = QObject::connect(iITunes, iITunes->metaObject()->method(11), handler, handler->metaObject()->method(7));

        //iITunes->dumpObjectInfo();

        //process events
        while(this->mustListen && !handler->iTunesShutdownRequested) {
            QCoreApplication::processEvents();
            this->msleep(20);
        }

        //disconnect events
        QObject::disconnect(oatputqe);
        QObject::disconnect(oppe);
        QObject::disconnect(opse);

        //clear COM related Obj
        this->shoutEmpty();
        delete handler;
        iITunes->clear(); 
        delete iITunes;

        //uninitialize COM
        CoUninitialize();

        //if iTunes is shutting down...
        if(this->mustListen && handler->iTunesShutdownRequested) {
            
            //say we acknoledge iTunes shutting down...
            emit this->printLog(I18n::tr()->Shout_ITunesShutdown());

            //wait for old iTunes window to finally shutdown
            do {
                
                //check if window still exists 
                HWND checkHandler = FindWindowA(0, "iTunes");
                if(checkHandler) {

                    //if it exists, check the PID (shutting down window...)
                    DWORD checkProcessID;
                    DWORD checkProcessID_worked = GetWindowThreadProcessId(checkHandler, &checkProcessID);
                    if(checkProcessID_worked) {
                        
                        //if old ID <> checked ID, means a new window has been opened, so we can close
                        if(checkProcessID != currentProcessID) {
                            break;
                        }
                    }

                //if no window found, break...
                } else {
                    break;
                }
                
                
                //finally, sleep
                this->sleep(1);

            } while (this->mustListen);
            
            //say we relooped
            emit this->printLog(I18n::tr()->Shout_WaitITunesAgain());
        }

    } while (this->mustListen);

    //end with log
    emit printLog(I18n::tr()->Shout_StopListening());

};

#endif