#include "shout.h"
#include "../../../libs/itunescom/iTunesEventHandler.cpp"

#include <windows.h>

class ITunesEventsSink : public iTunesEventHandler {  
    public:  

        HRESULT OnPlayerPlayEvent(VARIANT iTrack) override {
            return 0;
        }  
        HRESULT OnPlayerStopEvent(VARIANT iTrack) override {
            return 0;
        }  
        HRESULT OnAboutToPromptUserToQuitEvent() override {
            return 0;
        } 
}; 


void ShoutWorker::run() {
    emit this->printLog("Waiting for iTunes to launch...");
    CoInitialize(nullptr);

    HRESULT  hRes;
    IiTunes* iITunes;
    
    //instanciate COM Object
    hRes = ::CoCreateInstance(CLSID_iTunesApp, NULL, CLSCTX_LOCAL_SERVER, IID_IiTunes, (PVOID *)&iITunes);
    
    //bind iTunes events to sink
    ITunesEventsSink *sink = new ITunesEventsSink();
    DWORD adviseCookie; 
    IConnectionPoint* iITunesEventsConnectionPoint; 
    IConnectionPointContainer* iITunesConnectionPointContainer;   
    iITunes->QueryInterface(IID_IConnectionPointContainer, (void**)&iITunesConnectionPointContainer);   
    iITunesConnectionPointContainer->FindConnectionPoint(DIID__IiTunesEvents, &iITunesEventsConnectionPoint);   
    iITunesEventsConnectionPoint->Advise(sink, &adviseCookie);   
    iITunesConnectionPointContainer->Release();   

    //wait for events to trigger
    while(this->mustListen) {
        try {
            Sleep(1000);
        } catch (const std::exception& e) {
            emit printLog(e.what());
        }
    }

    //unplug COM
    iITunesEventsConnectionPoint->Unadvise(adviseCookie);
    iITunesEventsConnectionPoint->Release();
    iITunes->Release();
    CoUninitialize();
    emit this->printLog("Stopped listening to iTunes.");
};
