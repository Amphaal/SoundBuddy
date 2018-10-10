#include <windows.h>

// #include "../../../libs/itunescom/iTunesEventHandler.cpp"
#include "../../../libs/itunescom/iTunesCOMInterface.h"
#include <ActiveQt/QAxObject>
#include <QMetaObject>
#include <QMetaMethod>

#include "shout.h" 



// class ITunesEventsSink : public iTunesEventHandler {  
//     public:  

//         HRESULT OnPlayerPlayEvent(VARIANT iTrack) override {
//             return 0;
//         }  
//         HRESULT OnPlayerStopEvent(VARIANT iTrack) override {
//             return 0;
//         }  
//         HRESULT OnAboutToPromptUserToQuitEvent() override {
//             return 0;
//         } 
// }; 


void ShoutWorker::run() {
    emit this->printLog("Waiting for iTunes to launch...");
    
    //CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    wchar_t* wch = nullptr;
    HRESULT hr = ::StringFromCLSID(CLSID_iTunesApp, &wch);
    QAxObject t(QString::fromWCharArray(wch), this);

    const QMetaObject *metaObject = t.metaObject();
    QStringList methods;
    for(int i = metaObject->methodOffset(); i < metaObject->methodCount(); ++i) {
        if (metaObject->method(i).methodType() == QMetaMethod::MethodType::Slot) {
            methods << QString::fromLatin1(metaObject->method(i).methodSignature());
        }
    }


   //CoUninitialize();

    // CoInitialize(nullptr);

    // HRESULT  hRes;
    // IiTunes* iITunes;
    
    // //instanciate COM Object
    // hRes = ::CoCreateInstance(CLSID_iTunesApp, NULL, CLSCTX_LOCAL_SERVER, IID_IiTunes, (PVOID *)&iITunes);
    
    // //bind iTunes events to sink
    // ITunesEventsSink *sink = new ITunesEventsSink();
     
    // IConnectionPoint* iITunesEventsConnectionPoint; 
    // IConnectionPointContainer* iITunesConnectionPointContainer;   
    
    // HRESULT paa = iITunes->QueryInterface(IID_IConnectionPointContainer, (void**)&iITunesConnectionPointContainer);   
    // HRESULT p = iITunesConnectionPointContainer->FindConnectionPoint(DIID__IiTunesEvents, &iITunesEventsConnectionPoint);   
    
    // DWORD adviseCookie; 
    // HRESULT popo = iITunesEventsConnectionPoint->Advise(sink, &adviseCookie);  
    // iITunesConnectionPointContainer->Release();   

    // //wait for events to trigger
    // while(this->mustListen) {
    //     try {
    //         Sleep(1000);
    //     } catch (const std::exception& e) {
    //         emit printLog(e.what());
    //     }
    // }

    // //unplug COM
    // iITunesEventsConnectionPoint->Unadvise(adviseCookie);
    // iITunesEventsConnectionPoint->Release();
    // iITunes->Release();
    // CoUninitialize();
    emit this->printLog("Stopped listening to iTunes.");
};
