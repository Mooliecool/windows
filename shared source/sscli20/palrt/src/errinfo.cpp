// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
// ===========================================================================
// File: errinfo.cpp
// 
// ===========================================================================
/***
*
*Purpose:
*  Implementation of the System Error Info objects and APIs.
*
*Revision History:
*
*                                                
*
*Implementation Notes:
*
*****************************************************************************/

#include <rotor_palrt.h>
#include "oaidl.h"
#include "oautil.h"
#include "convert.h"

//---------------------------------------------------------------------
//                      CErrorInfo
//---------------------------------------------------------------------

// CErrorInfo - 'errinfo'
//
// The standard system error object class.
//
class FAR CErrorInfo : public IErrorInfo, public ICreateErrorInfo
{
public:
    static HRESULT Create(CErrorInfo FAR* FAR* pperrinfo);
	
    /* IUnknown methods */
    STDMETHOD(QueryInterface)(REFIID riid, void FAR* FAR* ppvObj);
    STDMETHOD_(ULONG, AddRef)(void);
    STDMETHOD_(ULONG, Release)(void);

    /* IErrorInfo methods */
    STDMETHOD(GetGUID)(GUID FAR* pguid);
    STDMETHOD(GetSource)(BSTR FAR* pbstrSource);
    STDMETHOD(GetDescription)(BSTR FAR* pbstrDescription);
    STDMETHOD(GetHelpFile)(BSTR FAR* pbstrHelpFile);
    STDMETHOD(GetHelpContext)(ULONG FAR* pdwHelpContext);

    /* ICreateErrorInfo methods */
    STDMETHOD(SetGUID)(REFGUID rguid);
    STDMETHOD(SetSource)(LPOLESTR szSource);
    STDMETHOD(SetDescription)(LPOLESTR szDescription);
    STDMETHOD(SetHelpFile)(LPOLESTR szHelpFile);
    STDMETHOD(SetHelpContext)(ULONG dwHelpContext);

    CErrorInfo();
#ifdef __GNUC__
    virtual
#endif
	~CErrorInfo();

private:
    LONG m_cRefs;

    GUID m_guid;
    BSTR m_bstrSource;
    BSTR m_bstrDescription;
    BSTR m_bstrHelpFile;
    ULONG m_dwHelpContext;
};

CErrorInfo::CErrorInfo()
{
    m_cRefs = 0;
    m_guid = GUID_NULL;
    m_bstrSource = NULL;
    m_bstrDescription = NULL;
    m_bstrHelpFile = NULL;
    m_dwHelpContext = 0;
}

CErrorInfo::~CErrorInfo()
{
    SysFreeString(m_bstrSource);
    SysFreeString(m_bstrDescription);
    SysFreeString(m_bstrHelpFile);
}

HRESULT
CErrorInfo::Create(CErrorInfo FAR* FAR* pperrinfo)
{
    CErrorInfo FAR* perrinfo;

    if((perrinfo = new CErrorInfo()) == NULL)
      return RESULT(E_OUTOFMEMORY);
    perrinfo->m_cRefs = 1;
    *pperrinfo = perrinfo;
    return NOERROR;
}

STDMETHODIMP
CErrorInfo::QueryInterface(REFIID riid, void FAR* FAR* ppvObj)
{
    *ppvObj = NULL;
    if(riid == IID_IUnknown){
      *ppvObj = this;
    }else
    if(riid == IID_IErrorInfo){
      *ppvObj = (IErrorInfo FAR*)this;
    }else
    if(riid == IID_ICreateErrorInfo){
      *ppvObj = (ICreateErrorInfo FAR*)this;
    }

    if(*ppvObj == NULL)
      return RESULT(E_NOINTERFACE);

    (*(IUnknown FAR* FAR*)ppvObj)->AddRef();
    return NOERROR;
}

STDMETHODIMP_(ULONG)
CErrorInfo::AddRef()
{
    return (ULONG)InterlockedIncrement(&m_cRefs);
}

STDMETHODIMP_(ULONG)
CErrorInfo::Release()
{
    LONG cRefs = InterlockedDecrement(&m_cRefs);
    if (cRefs == 0) {
        delete this;
    }
    return (ULONG)cRefs;
}

//---------------------------------------------------------------------
//                      IErrorInfo methods 
//---------------------------------------------------------------------

STDMETHODIMP
CErrorInfo::GetGUID(GUID FAR* pguid)
{
    *pguid = m_guid;
    return NOERROR;
}

STDMETHODIMP
CErrorInfo::GetSource(BSTR FAR* pbstrSource)
{
    return ErrStringCopy(m_bstrSource, pbstrSource);
}

STDMETHODIMP
CErrorInfo::GetDescription(BSTR FAR* pbstrDescription)
{
    return ErrStringCopy(m_bstrDescription, pbstrDescription);
}

STDMETHODIMP
CErrorInfo::GetHelpFile(BSTR FAR* pbstrHelpFile)
{
    return ErrStringCopy(m_bstrHelpFile, pbstrHelpFile);
}

STDMETHODIMP
CErrorInfo::GetHelpContext(ULONG FAR* pdwHelpContext)
{
    *pdwHelpContext = m_dwHelpContext;
    return NOERROR;
}


//---------------------------------------------------------------------
//                   ICreateErrorInfo methods
//---------------------------------------------------------------------

STDMETHODIMP
CErrorInfo::SetGUID(REFGUID rguid)
{
    m_guid = rguid;
    return NOERROR;
}

STDMETHODIMP
CErrorInfo::SetSource(LPOLESTR szSource)
{
    SysFreeString(m_bstrSource);
    m_bstrSource = NULL;
    return ErrSysAllocString(szSource, &m_bstrSource);
}

STDMETHODIMP
CErrorInfo::SetDescription(LPOLESTR szDescription)
{
    SysFreeString(m_bstrDescription);
    m_bstrDescription = NULL;
    return ErrSysAllocString(szDescription, &m_bstrDescription);
}

STDMETHODIMP
CErrorInfo::SetHelpFile(LPOLESTR szHelpFile)
{
    SysFreeString(m_bstrHelpFile);
    m_bstrHelpFile = NULL;
    return ErrSysAllocString(szHelpFile, &m_bstrHelpFile);
}

STDMETHODIMP
CErrorInfo::SetHelpContext(ULONG dwHelpContext)
{
    m_dwHelpContext = dwHelpContext;
    return NOERROR;
}


//---------------------------------------------------------------------
//                      Error Info APIs
//---------------------------------------------------------------------


STDAPI
SetErrorInfo(ULONG dwReserved, IErrorInfo FAR* perrinfo)
{
    APP_DATA FAR *pappdata;
    HRESULT hresult;
    IErrorInfo FAR* perrinfoOld;

    if (FAILED(hresult = GetAppData(&pappdata))) {
      return hresult;
    }

    // Do this first, so that if the Release() reenters, we're not pointing
    // at them
    perrinfoOld = pappdata->m_perrinfo;
    pappdata->m_perrinfo = NULL;

    if (perrinfoOld != NULL) {
      perrinfoOld->Release();
    }

    pappdata->m_perrinfo = perrinfo;
    
    if (perrinfo) {
      perrinfo->AddRef();
    }

    return NOERROR;
}

STDAPI
GetErrorInfo(ULONG dwReserved, IErrorInfo FAR* FAR* pperrinfo)
{
    APP_DATA FAR *pappdata;
    HRESULT hresult;

    if (FAILED(hresult = GetAppData(&pappdata))) {
      return hresult;
    }

    *pperrinfo = pappdata->m_perrinfo;

    if (*pperrinfo == NULL) {
      return S_FALSE;
    }
    
    pappdata->m_perrinfo = NULL;

    return NOERROR;
}

STDAPI
CreateErrorInfo(ICreateErrorInfo FAR* FAR* pperrinfo)
{
    CErrorInfo FAR* perrinfo;

    IfFailRet(CErrorInfo::Create(&perrinfo));
    *pperrinfo = (ICreateErrorInfo FAR*)perrinfo;
    return NOERROR;
}













