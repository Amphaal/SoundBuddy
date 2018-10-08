#pragma once

#include "iTunesCOMInterface.h"

class iTunesEventHandler : public _IiTunesEvents {

    private:
    long m_dwRefCount;
    ITypeInfo* m_pITypeInfo ;
    ULONG m_cRef;

    public:

        ///
        /// 
        ///

        virtual HRESULT OnPlayerPlayEvent(VARIANT iTrack) = 0;
        virtual HRESULT OnPlayerStopEvent(VARIANT iTrack) = 0;  
        virtual HRESULT OnAboutToPromptUserToQuitEvent() = 0;  

        ///
        /// 
        ///

        iTunesEventHandler() : m_dwRefCount(1) {
    
            ITypeLib* pITypeLib = NULL;

            HRESULT	 hr = ::LoadRegTypeLib
                (LIBID_iTunesLib, 
                1, 5, 
                0x00, &pITypeLib);

            hr = pITypeLib->GetTypeInfoOfGuid(DIID__IiTunesEvents,
                &m_pITypeInfo);

            pITypeLib->Release();
        }

        STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject) {
            *ppvObject = NULL;
            if (riid == IID_IUnknown || riid == DIID__IiTunesEvents) 
            {
                *ppvObject = this;
                return S_OK;
            } else {
                return E_NOINTERFACE;
            }
        }

        STDMETHODIMP_(ULONG) AddRef(void) {    
            return InterlockedIncrement(&m_dwRefCount);
        }

        STDMETHODIMP_(ULONG) Release(void) {
            if (InterlockedDecrement(&m_dwRefCount) == 0)
            {
                delete this;
                return 0;
            }
            return m_dwRefCount;
        }

        STDMETHODIMP GetTypeInfoCount(UINT *pctinfo) {
            //this method is not called by iTunes
            if (pctinfo)
                *pctinfo = 0;

            return S_OK;
        }

        STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo) {
            //this method is not called by iTunes
            return TYPE_E_ELEMENTNOTFOUND;
        }

        STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, 
                                                        LCID lcid, DISPID *rgDispId)
        {
            //this method is not called by iTunes
            return E_INVALIDARG;
        }

        STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, 
                                                DISPPARAMS *pDispParams, VARIANT *pVarResult,
                                                EXCEPINFO *pExcepInfo, UINT *puArgErr)
        {
            switch (dispIdMember) {
                case ITEventPlayerPlay:
                    this->OnPlayerPlayEvent(*pVarResult);
                    break;

                case ITEventPlayerStop:
                    this->OnPlayerStopEvent(*pVarResult);
                    break;

                case 9://ITEventAboutToPromptUserToQuit:
                    this->OnAboutToPromptUserToQuitEvent();
                    break;
                default:
                    break;
            }

            return S_OK;
        }
};