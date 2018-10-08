#include "shout.h"
#include "../../../libs/itunescom/iTunesCOMInterface.h"

#include <atlbase.h>  
#include <atlcom.h>  
#include <stdio.h>  

void ShoutWorker::run() {
    emit this->printLog("Waiting for iTunes to launch...");
    CoInitialize(nullptr);

    HRESULT  hRes;
    IiTunes* iITunes;
    
    hRes = ::CoCreateInstance(CLSID_iTunesApp, NULL, CLSCTX_LOCAL_SERVER, IID_IiTunes, (PVOID *)&iITunes);
    ITunesEventsSink sink;

    // while(this->mustListen) {
    //     try {
            
    //     } catch (const std::exception& e) {
    //         emit printLog(e.what());
    //         emit error();
    //     }
    // }
    
    CoUninitialize();
    emit this->printLog("Stopped listening to iTunes.");
};

[module(name="EventReceiver")];  
[event_receiver(com)]  
class ITunesEventsSink {  
    public:  
        HRESULT OnPlayerPlayEvent(VARIANT iTrack) {  // name and signature matches MyEvent1  
            ...  
        }  
        HRESULT OnPlayerStopEvent(VARIANT iTrack) {  // signature doesn't match MyEvent2  
            ...  
        }  
        HRESULT OnAboutToPromptUserToQuitEvent() {  // name doesn't match MyEvent1 (or 2)  
            ...  
        }  
};  