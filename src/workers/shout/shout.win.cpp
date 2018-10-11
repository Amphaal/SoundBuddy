#include <windows.h>

// #include "../../../libs/itunescom/iTunesEventHandler.cpp"
#include "../../../libs/itunescom/iTunesCOMInterface.h"

#include <ActiveQt/QAxBase>
#include <ActiveQt/QAxObject>

#include "shout.h" 

#include <QMetaObject>
#include <QMetaMethod>
#include <QObject>

class ITunesEventsSink : public QObject {  
    
    private:
        QAxObject *iTunesObj;
        ShoutWorker *worker;

    public slots:
        void OnAboutToPromptUserToQuitEvent() {
            //delete iTunesObj;
            this->shoutHelper();
        } 
        void OnPlayerPlayEvent(QVariant iTrack) {
            this->shoutHelper();
        }  
        void OnPlayerStopEvent(QVariant iTrack) {
            this->shoutHelper();
        }  

    public:  
        ITunesEventsSink(QAxObject *iTunesObj, ShoutWorker *worker) : iTunesObj(iTunesObj), worker(worker) {
            this->shoutHelper();
        }

        void shoutHelper(QVariant iTrack = NULL) {
            
            QAxObject *trackObj = new QAxObject(iTrack.value<IDispatch*>());

            if(trackObj->isNull()) {
                trackObj = iTunesObj->querySubObject("CurrentTrack");
            }

            if (trackObj == NULL) return this->worker->shoutEmpty();

            auto tName = trackObj->property("Name").value<QString>().toStdString();
            auto tAlbum = trackObj->property("Album").value<QString>().toStdString();
            auto tArtist = trackObj->property("Artist").value<QString>().toStdString();
            auto tGenre = trackObj->property("Genre").value<QString>().toStdString();
            auto tDuration = trackObj->property("Duration").value<QString>().toStdString();
            auto iPlayerPos = this->iTunesObj->property("PlayerPosition").value<int>();
            auto iPlayerState = this->iTunesObj->property("PlayerState").value<bool>();

            this->worker->shoutFilled(tName, tAlbum, tArtist, tGenre, tDuration, iPlayerPos, iPlayerState);
            trackObj->clear();
        }
}; 

void ShoutWorker::run() {
    emit this->printLog("Waiting for iTunes to launch...");
    
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    wchar_t* wch = nullptr;
    HRESULT hr = ::StringFromCLSID(CLSID_iTunesApp, &wch);
    QAxObject *iITunes = new QAxObject(QString::fromWCharArray(wch));
    iITunes->blockSignals(false);
    ITunesEventsSink *sink = new ITunesEventsSink(iITunes, this);


    const QMetaObject *metaObject = sink->metaObject();//iITunes->metaObject();
    QStringList methods;
    for(int i = metaObject->methodOffset(); i < metaObject->methodCount(); ++i) {
        
        auto curMethod = metaObject->method(i);
        auto name = QString::fromLatin1(curMethod.methodSignature()).toStdString();

        if (curMethod.methodType() == QMetaMethod::MethodType::Signal && name == "OnAboutToPromptUserToQuitEvent()") {
            //bool caca = QObject::connect(iITunes, curMethod, sink, sink->metaObject().method());
            
        }
    }


    while(this->mustListen) {
        Sleep(1000);
    }

   iITunes->clear(); 
   CoUninitialize();

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
