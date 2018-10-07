#include "shout.h"
#include "../../../libs/itunescom/iTunesCOMInterface.h"


void ShoutWorker::run() {
    emit this->printLog("Waiting for iTunes to launch...");
    CoInitialize(nullptr);

    HRESULT  hRes;
    IiTunes* iITunes;
    
    // note - CLSID_iTunesApp and IID_IiTunes are defined in iTunesCOMInterface_i.c
    hRes = ::CoCreateInstance(CLSID_iTunesApp, NULL, CLSCTX_LOCAL_SERVER, IID_IiTunes, (PVOID *)&iITunes);

    // while(this->mustListen) {
    //     try {
            
    //     } catch (const std::exception& e) {
    //         emit printLog(e.what());
    //         emit error();
    //     }
    // }
    
    CoUninitialize();
    emit this->printLog("Stopped listening to iTunes.");
}