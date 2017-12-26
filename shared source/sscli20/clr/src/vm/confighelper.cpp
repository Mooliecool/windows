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
//*****************************************************************************
// ConfigHelper.cpp
// 
//*****************************************************************************
//
// XML Helper so that NodeFactory can be implemented in Managed code  
//

#include "common.h"
#include "confighelper.h"
#include "comstring.h"

const GUID IID_IConfigHandler = { /* afd0d21f-72f8-4819-99ad-3f255ee5006b */
    0xafd0d21f,
    0x72f8,
    0x4819,
    {0x99, 0xad, 0x3f, 0x25, 0x5e, 0xe5, 0x00, 0x6b}
  };

ConfigFactory::ConfigFactory(IConfigHandler *pFactory){
    LEAF_CONTRACT;
    m_pManagedFactory = pFactory;
    m_pManagedFactory->AddRef();    //AddRef the managed Factory interface
    AddRef();
}

ConfigFactory::~ConfigFactory(){
    LEAF_CONTRACT;
    m_pManagedFactory->Release();   //Release the managed Factory interface
}


HRESULT STDMETHODCALLTYPE ConfigFactory::NotifyEvent( 
            /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
            /* [in] */ XML_NODEFACTORY_EVENT iEvt)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_ANY;
        NOTHROW;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), return COR_E_STACKOVERFLOW)

    VARIANT var[1];
    V_VT(&var[0]) = VT_I4;
    V_I4(&var[0]) = iEvt;
    m_pManagedFactory->InvokeByName(L"NotifyEvent", CorFFIInvokeMethod, 
        sizeof(var)/sizeof(var[0]), var, NULL);
    END_SO_INTOLERANT_CODE

    return S_OK;
}

static void InitParameters(VARIANT* pVar, XML_NODE_INFO *pNodeInfo)
{

    V_VT(&pVar[0]) = VT_I4;
    V_I4(&pVar[0]) = pNodeInfo->dwSize;
    V_VT(&pVar[1]) = VT_I4;
    V_I4(&pVar[1]) = pNodeInfo->dwSubType;
    V_VT(&pVar[2]) = VT_I4;
    V_I4(&pVar[2]) = pNodeInfo->dwType;
    V_VT(&pVar[3]) = VT_I4;
    V_I4(&pVar[3]) = pNodeInfo->fTerminal;
    V_VT(&pVar[4]) = VT_BSTR;
    V_BSTR(&pVar[4]) = SysAllocStringLen(pNodeInfo->pwcText, pNodeInfo->ulLen);
    V_VT(&pVar[5]) = VT_I4;
    V_I4(&pVar[5]) = pNodeInfo->ulLen;
    V_VT(&pVar[6]) = VT_I4;
    V_I4(&pVar[6]) = pNodeInfo->ulNsPrefixLen;
}

static void ClearParameters(VARIANT* pVar)
{
    SysFreeString(V_BSTR(&pVar[4]));
}

//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE ConfigFactory::BeginChildren( 
    /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
    /* [in] */ XML_NODE_INFO __RPC_FAR *pNodeInfo)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_ANY;
        NOTHROW;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), return COR_E_STACKOVERFLOW)
    VARIANT var[7];
    InitParameters(var, pNodeInfo);
    m_pManagedFactory->InvokeByName(L"BeginChildren", CorFFIInvokeMethod, 
        sizeof(var)/sizeof(var[0]), var, NULL);
    ClearParameters(var);

    END_SO_INTOLERANT_CODE
    return S_OK;

}

//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE ConfigFactory::EndChildren( 
    /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
    /* [in] */ BOOL fEmptyNode,
    /* [in] */ XML_NODE_INFO __RPC_FAR *pNodeInfo)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_ANY;
        NOTHROW;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), return COR_E_STACKOVERFLOW)
        
    VARIANT var[8];
    V_VT(&var[0]) = VT_I4;
    V_I4(&var[0]) = fEmptyNode;
    InitParameters(var+1, pNodeInfo);
    m_pManagedFactory->InvokeByName(L"EndChildren", CorFFIInvokeMethod, 
        sizeof(var)/sizeof(var[0]), var, NULL);
    ClearParameters(var+1);

    END_SO_INTOLERANT_CODE
    return S_OK;
}

//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE ConfigFactory::CreateNode(
    /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
    /* [in] */ PVOID pNode,
    /* [in] */ USHORT cNumRecs,
    /* [in] */ XML_NODE_INFO* __RPC_FAR * __RPC_FAR apNodeInfo)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        MODE_ANY;
        NOTHROW;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    BEGIN_SO_INTOLERANT_CODE_NOTHROW(GetThread(), return COR_E_STACKOVERFLOW)

    DWORD i;
    
    for( i = 0; i < cNumRecs; i++) { 
        VARIANT var[7];
        InitParameters(var, apNodeInfo[i]);
        if(i == 0) {
            m_pManagedFactory->InvokeByName(L"CreateNode", CorFFIInvokeMethod, 
                sizeof(var)/sizeof(var[0]), var, NULL);
        }
        else {
            m_pManagedFactory->InvokeByName(L"CreateAttribute", CorFFIInvokeMethod, 
                sizeof(var)/sizeof(var[0]), var, NULL);
        }
        ClearParameters(var);
    }


    END_SO_INTOLERANT_CODE
    return S_OK;
}



STDAPI GetXMLObjectEx(IXMLParser **ppv, CrstLevel crstLevel);

//
//Helper routines to call into managed Node Factory
//

HRESULT ConfigNative::RunInternal(IConfigHandler *pFactory, LPCWSTR filename)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    HRESULT        hr = S_OK;  
    SafeComHolder<IXMLParser> pIXMLParser;
    SafeComHolder<ConfigFactory>  helperfactory; 
    SafeComHolder<IStream> pFile; 
    if (!pFactory){
        return E_POINTER;
    }

    hr = CreateConfigStream(filename,&pFile);
    if(FAILED(hr))
        return hr;
    
    hr = GetXMLObjectEx(&pIXMLParser, CrstXMLParserManaged);
    if(FAILED(hr))
        return hr;

    helperfactory = new (nothrow)ConfigFactory(pFactory);  // RefCount = 1 
    if ( ! helperfactory) { 
            return E_OUTOFMEMORY; 
        }

    

    hr = pIXMLParser->SetInput(pFile); // filestream's RefCount=2
    if(FAILED(hr))
        return hr;

    hr = pIXMLParser->SetFactory(helperfactory); // factory's RefCount=2
    if(FAILED(hr))
        return hr;

    hr = pIXMLParser->Run(-1);

    if (hr== (HRESULT) XML_E_MISSINGROOT)  //empty file
        hr=S_OK;
    return hr;
}

//
// Entrypoint to return an Helper interface which Managed code can call to build managed Node factory
//

FCIMPL2(void, ConfigNative::RunParser, Object* refHandlerUNSAFE, StringObject* strFileNameUNSAFE)
{
    CONTRACTL
    {
        MODE_COOPERATIVE;
        DISABLED(GC_TRIGGERS);  // can't use this in an FCALL because we're in forbid gc mode until we setup a H_M_F.
        THROWS;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    OBJECTREF  refHandler = (OBJECTREF) refHandlerUNSAFE;
    STRINGREF  strFileName = (STRINGREF) strFileNameUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_2(refHandler, strFileName);
    //-[autocvtpro]-------------------------------------------------------

    HRESULT     hr;
    WCHAR*      pString;
    int         iString;
    LPWSTR      pFileName;
    CQuickBytes qb;
    SafeComHolder<IConfigHandler> pConfigHandler;

    if (refHandler == NULL) {
        COMPlusThrowArgumentNull(L"handler");
    }

    if (strFileName == NULL) {
        COMPlusThrowArgumentNull(L"fileName");
    }

    //Get string data.
    RefInterpretGetStringValuesDangerousForGC(strFileName, &pString, &iString);
    
    pFileName = (LPWSTR) qb.AllocThrows((iString + 1) * sizeof(WCHAR));
    memcpy(pFileName, pString, (iString + 1) * sizeof(WCHAR));

    // We are about to use COM functions, ensure COM is started.
    EnsureComStarted();

    {
        SafeComHolder<IUnknown> punk(GetComIPFromObjectRef(&refHandler));
        hr = punk->QueryInterface(IID_IManagedInstanceWrapper, (void**)&pConfigHandler);
    }
    if (FAILED(hr))
        COMPlusThrowHR(hr);

    {
        GCX_PREEMP();
        hr = RunInternal(pConfigHandler, pFileName);
    }

    if (FAILED(hr))
        COMPlusThrowHR(hr);

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

