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
// XMLReader.cpp
// 
//*****************************************************************************
//
// Lite weight xmlreader  
//

#include "stdafx.h"
#include <mscoree.h>
#include <xmlparser.hpp>
#include <objbase.h>
#include <mscorcfg.h>
#include "xmlreader.h"

#define ISWHITE(ch) ((ch) >= 0x09 && (ch) <= 0x0D || (ch) == 0x20)
#define CONST_STRING_AND_LENGTH(str) str, NumItems(str)-1

#define VERARRAYGRAN  4

class ShimFactory : public _unknown<IXMLNodeFactory, &IID_IXMLNodeFactory>
{

public:
    ShimFactory();
    ~ShimFactory();
    HRESULT STDMETHODCALLTYPE NotifyEvent( 
            /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
            /* [in] */ XML_NODEFACTORY_EVENT iEvt);

    HRESULT STDMETHODCALLTYPE BeginChildren( 
        /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
        /* [in] */ XML_NODE_INFO* __RPC_FAR pNodeInfo);

    HRESULT STDMETHODCALLTYPE EndChildren( 
        /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
        /* [in] */ BOOL fEmptyNode,
        /* [in] */ XML_NODE_INFO* __RPC_FAR pNodeInfo);
    
    HRESULT STDMETHODCALLTYPE Error( 
        /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
        /* [in] */ HRESULT hrErrorCode,
        /* [in] */ USHORT cNumRecs,
        /* [in] */ XML_NODE_INFO* __RPC_FAR * __RPC_FAR apNodeInfo)
    {
      /* 
         UNUSED(pSource);
         UNUSED(hrErrorCode);
         UNUSED(cNumRecs);
         UNUSED(apNodeInfo);
      */
        return hrErrorCode;
    }
    
    WCHAR* ReleaseVersion()
    {
        WCHAR* version = pVersion;
        pVersion = NULL;
        return version;
    }

    DWORD ReleaseSupportedVersionArray(__deref_out_opt WCHAR*** pArray)
    {
        if(pArray)
        { 
            *pArray=ppSupportedVersions;
            ppSupportedVersions=NULL;
        }
        return nSupportedVersions;
    }

    WCHAR* ReleaseImageVersion()
    {
        WCHAR* version = pImageVersion;
        pImageVersion = NULL;
        return version;
    }

    WCHAR* ReleaseBuildFlavor()
    {
        WCHAR* buildflavor = pBuildFlavor;
        pBuildFlavor = NULL;
        return buildflavor;
    }

    BOOL IsSafeMode() { return bIsSafeMode; }
    BOOL IsRequiredRuntimeSafeMode() { return bIsRequiredRuntimeSafeMode; }

    HRESULT STDMETHODCALLTYPE CreateNode( 
        /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
        /* [in] */ PVOID pNodeParent,
        /* [in] */ USHORT cNumRecs,
        /* [in] */ XML_NODE_INFO* __RPC_FAR * __RPC_FAR apNodeInfo);

private:
    HRESULT SetRequiredVersion(__in_z WCHAR* version);
    HRESULT SetImageVersion(__in_z WCHAR* version);
    HRESULT AddSupportedVersion(__in_z WCHAR* version);
    HRESULT CopyBuildFlavor(LPCWSTR val, DWORD len);
    HRESULT GetBuildFlavor(LPCWSTR version);

    WCHAR* pVersion;
    WCHAR* pImageVersion;
    WCHAR* pBuildFlavor;    
    BOOL   bIsSafeMode;
    BOOL   bIsRequiredRuntimeSafeMode;
    BOOL   fConfiguration;
    BOOL   fStartup;
    BOOL   fRuntime;    
    DWORD  nSupportedVersions;
    WCHAR** ppSupportedVersions;
    DWORD  nLevel;
    DWORD  tElement;
};

ShimFactory::ShimFactory() 
{
    pVersion = NULL;
    pImageVersion = NULL;
    pBuildFlavor = NULL;    
    bIsSafeMode = FALSE;
    bIsRequiredRuntimeSafeMode = FALSE;
    fConfiguration=FALSE;
    fStartup = FALSE;
    fRuntime = FALSE;    
    tElement = 0;
    nSupportedVersions=0;
    ppSupportedVersions=NULL;
    nLevel=0;
}

ShimFactory::~ShimFactory() 
{
    if(pVersion != NULL) {
        delete [] pVersion;
    }
    if(pImageVersion != NULL) {
        delete [] pImageVersion;
    }
    if(pBuildFlavor != NULL) {
        delete [] pBuildFlavor;
    }    
    if(ppSupportedVersions)
    {
        for (DWORD i=0;i<nSupportedVersions;i++)
            delete[] ppSupportedVersions[i];

        delete[] ppSupportedVersions;
    }
}

HRESULT ShimFactory::SetRequiredVersion(__in __in_z WCHAR* version)
{
    if (pVersion)
        delete[] pVersion;

    pVersion = version;

    return S_OK;
}

HRESULT ShimFactory::SetImageVersion(__in __in_z WCHAR* imageVersion)
{
    pImageVersion = imageVersion;
    return S_OK;
}

HRESULT ShimFactory::CopyBuildFlavor(LPCWSTR val, DWORD len)
{    
    if ((_wcsnicmp(val, L"true", len) == 0) || (_wcsnicmp(val, L"false", len) == 0))
    {
        if (pBuildFlavor)
        {
            delete[] pBuildFlavor;
        }
        pBuildFlavor = new WCHAR[4];
        if(pBuildFlavor == NULL) 
        {
            return E_OUTOFMEMORY;
        }

        if (_wcsnicmp(val, L"true", len) == 0)
        {
            wcscpy_s(pBuildFlavor, 4, L"svr");
        }
        else 
        {
            wcscpy_s(pBuildFlavor, 4, L"wks");
        }
    }
    return S_OK;
}    

HRESULT ShimFactory::AddSupportedVersion(__in __in_z WCHAR* version)
{
    if ((nSupportedVersions % VERARRAYGRAN)==0)
    {
        //reallocate
        WCHAR** pNewPtr=new (nothrow) WCHAR*[nSupportedVersions+VERARRAYGRAN];
        if(pNewPtr == NULL)
            return E_OUTOFMEMORY;
        if (ppSupportedVersions)
        {
            memcpy(pNewPtr,ppSupportedVersions,nSupportedVersions*sizeof(WCHAR*));
            memset(pNewPtr+nSupportedVersions,0,VERARRAYGRAN*sizeof(WCHAR*));
            delete[] ppSupportedVersions;
        }
        ppSupportedVersions=pNewPtr;
    }
   
    ppSupportedVersions[nSupportedVersions]=version;
    nSupportedVersions++;
    return S_OK;
};

HRESULT ExtractXMLVersionAttribute(XML_NODE_INFO* __RPC_FAR * __RPC_FAR apNodeInfo, 
                                        DWORD cNumRecs, 
                                        __inout DWORD *cCurrentRec, 
                                        __deref_out_z __deref_out_opt LPWSTR* lpwszVersion)
{
    HRESULT hr = S_OK;
    DWORD nCurrentLen = 0;
    *lpwszVersion = NULL;
    WCHAR* version = new (nothrow) WCHAR[MAX_VERSION_LENGTH];

    IfNullRet(version);

    // Join all of the consecutive XML_PCDATA records together into one big string
    while(((*cCurrentRec) < cNumRecs) && (apNodeInfo[*cCurrentRec]->dwType == XML_PCDATA))
    {   
        XML_NODE_INFO* node = apNodeInfo[*cCurrentRec];
    
        DWORD lgth = node->ulLen;
        WCHAR *ptr = (WCHAR*) node->pwcText;
        
        // Strip out the whitespace in the front and the back
        for(;*ptr && ISWHITE(*ptr) && lgth>0; ptr++, lgth--);
        for(;lgth > 0 && ISWHITE(ptr[lgth-1]);lgth--);

        if (nCurrentLen < MAX_VERSION_LENGTH)
        {
            wcsncpy_s(version+nCurrentLen, MAX_VERSION_LENGTH-nCurrentLen, ptr, min(MAX_VERSION_LENGTH-nCurrentLen-1,lgth));
            nCurrentLen+=lgth;
        }
        (*cCurrentRec)++;
    }

    // Move the current record back one....
    (*cCurrentRec)--;
    *lpwszVersion = version;
    return hr;
}// ExtractXMLVersionAttribute



HRESULT STDMETHODCALLTYPE ShimFactory::NotifyEvent( 
            /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
            /* [in] */ XML_NODEFACTORY_EVENT iEvt)
{

    UNUSED(pSource);
    UNUSED(iEvt);
    return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE ShimFactory::BeginChildren( 
    /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
    /* [in] */ XML_NODE_INFO __RPC_FAR *pNodeInfo)
{
    UNUSED(pSource);
    UNUSED(pNodeInfo); 

    if (nLevel==1 && wcscmp(pNodeInfo->pwcText, L"configuration") == 0) 
        fConfiguration = TRUE;
    if (nLevel==2 && fConfiguration && wcscmp(pNodeInfo->pwcText, L"startup") == 0) 
    {
        fStartup = TRUE;
        fRuntime = FALSE;
    }
    if (nLevel==2 && fConfiguration && wcscmp(pNodeInfo->pwcText, L"runtime") == 0) 
    {
        fRuntime = TRUE;
        fStartup = FALSE;
    }

    return S_OK;

}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE ShimFactory::EndChildren( 
    /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
    /* [in] */ BOOL fEmptyNode,
    /* [in] */ XML_NODE_INFO __RPC_FAR *pNodeInfo)
{
    UNUSED(pSource);
    UNUSED(fEmptyNode);
    UNUSED(pNodeInfo);
    if (pNodeInfo->dwType == XML_ELEMENT)
        nLevel--;
    if ( fEmptyNode ) { 

    }
    else {
        if (fStartup && wcscmp(pNodeInfo->pwcText, L"startup") == 0) {
            fStartup = FALSE;
            tElement=0;
        }
        if (fRuntime && wcscmp(pNodeInfo->pwcText, L"runtime") == 0) {
            fRuntime = FALSE;
            tElement=0;
        }            
    }
    return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE ShimFactory::CreateNode( 
    /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
    /* [in] */ PVOID pNode,
    /* [in] */ USHORT cNumRecs,
    /* [in] */ XML_NODE_INFO* __RPC_FAR * __RPC_FAR apNodeInfo)
{
    if (apNodeInfo[0]->dwType == XML_ELEMENT)  
        nLevel++;

    HRESULT hr = S_OK;
    DWORD  i; 
    DWORD  tAttribute=0;

    // Unique tags
    enum {
        tVersion = 1,
        tImageVersion,
        tRequiredRuntimeSafeMode,
        tSafeMode,
        tRequiredVersion,
        tSupportedVersion,
        tStartup,
        tRuntime,
        tGcServer,
        tGcServerEnabled
    };

    UNUSED(pSource);
    UNUSED(pNode);
    UNUSED(apNodeInfo);
    UNUSED(cNumRecs);

    for( i = 0; i < cNumRecs; i++) { 
        if(apNodeInfo[i]->dwType == XML_ELEMENT ||
           apNodeInfo[i]->dwType == XML_ATTRIBUTE ||
           apNodeInfo[i]->dwType == XML_PCDATA) {

            DWORD lgth = apNodeInfo[i]->ulLen;
            WCHAR *ptr = (WCHAR*) apNodeInfo[i]->pwcText;
            // Trim the value
            for(;*ptr && ISWHITE(*ptr) && lgth>0; ptr++, lgth--);
            while( lgth > 0 && ISWHITE(ptr[lgth-1]))
                   lgth--;
            
            switch(apNodeInfo[i]->dwType) {
            case XML_ELEMENT : 
                tElement=0;
                if(fStartup == TRUE) {

                    // NumItems will return the length of the string including the null. We should only pass in the length
                    // not including the null (hence the -1)
                    if(XMLStringCompare(ptr, lgth, CONST_STRING_AND_LENGTH(L"requiredRuntime")) == 0)
                    {
                        tElement = tRequiredVersion;
                        break;
                    }
                    
                    if(XMLStringCompare(ptr, lgth, CONST_STRING_AND_LENGTH(L"supportedRuntime")) == 0) {
                        tElement=tSupportedVersion;
                        break;
                    }
                }

                if(fRuntime == TRUE) {
                    if(XMLStringCompare(ptr, lgth, CONST_STRING_AND_LENGTH(L"gcServer")) == 0) {
                        tElement=tGcServer;
                        break;
                    }
                }

                if(XMLStringCompare(ptr, lgth, CONST_STRING_AND_LENGTH(L"startup")) == 0) {
                    fStartup = TRUE;
                    tElement=tStartup;
                    break ; 
                }
                if(XMLStringCompare(ptr, lgth, CONST_STRING_AND_LENGTH(L"runtime")) == 0) {
                    fRuntime = TRUE;
                    tElement=tRuntime;
                    break ; 
                }               
                break;

            case XML_ATTRIBUTE : 
                if(fStartup == TRUE) 
                {
                    tAttribute=0;
                    if(((tElement==tRequiredVersion) || (tElement==tSupportedVersion)) 
                        && XMLStringCompare(ptr, lgth, CONST_STRING_AND_LENGTH(L"version")) == 0)
                    {
                        tAttribute = tVersion;
                        break;
                    }

                    if((tElement==tRequiredVersion) 
                        && XMLStringCompare(ptr, lgth, CONST_STRING_AND_LENGTH(L"imageVersion")) == 0)
                    {
                        tAttribute = tImageVersion;
                        break;
                    }

                }
                if(fRuntime == TRUE)
                {
                    tAttribute=0;
                    if((tElement==tGcServer) && 
                        XMLStringCompare(ptr, lgth, CONST_STRING_AND_LENGTH(L"enabled")) == 0)
                    {
                        tAttribute = tGcServerEnabled;
                        break;
                    }
                }                
                break;
            case XML_PCDATA:
                if(fStartup == TRUE) 
                {
                    WCHAR* version = NULL;
                
                    if (tAttribute == tVersion || tAttribute == tImageVersion)
                    {
                        // ExtractXMLVersionAttribute will allocate memory for the version
                        IfFailRet(ExtractXMLVersionAttribute(apNodeInfo, cNumRecs, &i, &version));
                    }

                
                    if ((tElement==tSupportedVersion) && (tAttribute == tVersion)) {
                        hr=AddSupportedVersion(version);
                        break;
                    }

                    if((tElement == tRequiredVersion) && (tAttribute == tVersion)) {
                        hr = SetRequiredVersion(version);
                        break;
                    }

                    // We shouldn't know about any other version strings
                    _ASSERTE(tAttribute != tVersion);

                    switch(tAttribute) {
                    case tImageVersion:
                        hr = SetImageVersion(version);
                        break;
                    default:
                        break;
                    }
                }
                if(fRuntime == TRUE)
                {
                    if((tElement==tGcServer) && (tAttribute==tGcServerEnabled))
                    {
                        hr=CopyBuildFlavor(ptr, lgth);
                    }
                }                
                break ;     
            default: 
                break;
            } // end of switch
        }
    }

    return hr;  
}


LPWSTR GetShimInfoFromConfigFile(LPCWSTR name, LPCWSTR wszFileName){
    _ASSERTE(name);
    return NULL; 
}

HRESULT XMLGetVersionFromStream(IStream* pStream, 
                                DWORD reserved, 
                                __out LPWSTR* pVersion, 
                                __out_opt LPWSTR* pImageVersion, 
                                __out_opt LPWSTR* pBuildFlavor, 
                                __out_opt BOOL* bSafeMode,
                                __out_opt BOOL* bRequiredRuntimeSafeMode)
{
    return XMLGetVersionWithSupportedFromStream(pStream,
                                                reserved,
                                                pVersion,
                                                pImageVersion,
                                                pBuildFlavor,
                                                bSafeMode,
                                                bRequiredRuntimeSafeMode,
                                                NULL,NULL);
}

HRESULT XMLGetVersionWithSupportedFromStream(IStream* pStream, 
                                             DWORD reserved, 
                                             __out LPWSTR* pVersion, 
                                             __out_opt LPWSTR* pImageVersion, 
                                             __out_opt LPWSTR* pBuildFlavor, 
                                             __out_opt BOOL* bSafeMode,
                                             __out_opt BOOL* bRequiredRuntimeSafeMode,
                                             __out_opt LPWSTR** ppwszSupportedVersions, DWORD* pnSupportedVersions)
{  
    if(pVersion == NULL) return E_POINTER;

    HRESULT        hr = S_OK;  
    IXMLParser     *pIXMLParser = NULL;
    ShimFactory    *factory = NULL; 

    hr = GetXMLObject(&pIXMLParser);
    if(FAILED(hr)) goto Exit;

    factory = new (nothrow) ShimFactory();
    if ( ! factory) { 

        hr = E_OUTOFMEMORY; 
        goto Exit; 
    }
    factory->AddRef(); // RefCount = 1 

    hr = pIXMLParser->SetInput(pStream); // filestream's +1
    if ( ! SUCCEEDED(hr)) 
        goto Exit;

    hr = pIXMLParser->SetFactory(factory); // factory's RefCount=2
    if ( ! SUCCEEDED(hr)) 
        goto Exit;

    hr = pIXMLParser->Run(-1);
    if(SUCCEEDED(hr) || hr == STARTUP_FOUND) {
        if (pVersion)
            *pVersion  = factory->ReleaseVersion();
        if (bSafeMode)
            *bSafeMode = factory->IsSafeMode();
        if (bRequiredRuntimeSafeMode)
            *bRequiredRuntimeSafeMode = factory->IsRequiredRuntimeSafeMode();
        if (pBuildFlavor)
            *pBuildFlavor = factory->ReleaseBuildFlavor();
        if(pImageVersion)
            *pImageVersion = factory->ReleaseImageVersion();
        if(pnSupportedVersions)
        {
            *pnSupportedVersions=factory->ReleaseSupportedVersionArray(ppwszSupportedVersions);
        }
        hr = S_OK; 
    }
Exit:  

    if (hr == (HRESULT) XML_E_MISSINGROOT || hr == E_PENDING)
        hr=S_OK;

    if (pIXMLParser) { 
        pIXMLParser->Release();
        pIXMLParser= NULL ; 
    }
    if ( factory) {
        factory->Release();
        factory=NULL;
    }
    return hr;  
}


HRESULT XMLGetVersion(PCWSTR filename, 
                      __out LPWSTR* pVersion, 
                      __out_opt LPWSTR* pImageVersion, 
                      __out_opt LPWSTR* pBuildFlavor, 
                      __out_opt BOOL* bSafeMode,
                      __out_opt BOOL* bRequiredRuntimeSafeMode)
    
{
    return XMLGetVersionWithSupported(filename,
                                      pVersion,
                                      pImageVersion,
                                      pBuildFlavor,
                                      bSafeMode,
                                      bRequiredRuntimeSafeMode,
                                      NULL,NULL);
}

HRESULT XMLGetVersionWithSupported(PCWSTR filename, 
                                   __out LPWSTR* pVersion, 
                                   __out_opt LPWSTR* pImageVersion, 
                                   __out_opt LPWSTR* pBuildFlavor, 
                                   __out_opt BOOL* bSafeMode,
                                   __out_opt BOOL* bRequiredRuntimeSafeMode,
                                   __out_opt LPWSTR** ppwszSupportedVersions, DWORD* pnSupportedVersions)
{
    if(pVersion == NULL) return E_POINTER;

    HRESULT        hr = S_OK;  
    IStream        *pFile = NULL;

    hr = CreateConfigStream(filename, &pFile);
    if(FAILED(hr)) goto Exit;

    hr = XMLGetVersionWithSupportedFromStream(pFile, 
                                              0, pVersion, 
                                              pImageVersion, 
                                              pBuildFlavor, 
                                              bSafeMode,
                                              bRequiredRuntimeSafeMode,
                                              ppwszSupportedVersions, pnSupportedVersions);
Exit:  
    if ( pFile) {
        pFile->Release();
        pFile=NULL;
    }

    return hr;  
}

int XMLStringCompare(const WCHAR *pStr1, 
                    DWORD cchStr1, 
                    const WCHAR *pStr2, 
                    DWORD cchStr2)
{
    if (cchStr1 != cchStr2)
        return -1;

    return wcsncmp(pStr1, pStr2, cchStr1);
}// XMLStringCompare




