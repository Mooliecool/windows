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
 //
//  Naming Services
//

#include <windows.h>
#include <winerror.h>
#include "strongname.h"
#include "fusionp.h"
#include "naming.h"
#include "asmcache.h"
#include "asm.h"
#include "asmimprt.h"
#include "adl.h"
#include "adlmgr.h"
#include "cblist.h"
#include "helpers.h"
#include "appctx.h"
#include "actasm.h"
#include "parse.h"
#include "adlmgr.h"
#include "policy.h"
#include "dbglog.h"
#include "util.h"    
#include "pcycache.h"
#include "cacheutils.h"
#include "lock.h"
#include "memoryreport.h"

#include "dlwrap.h"



#define VERSION_STRING_SEGMENTS                     4

const WCHAR      g_wzRTMCorVersion[]     = L"v1.0.3705";
const WCHAR      g_wzEverettCorVersion[] = L"v1.1.4322";
const WCHAR      g_wzStandardCLI2002[]   = L"Standard CLI 2002";

extern WCHAR g_wszAdminCfg[];
extern WCHAR g_wzEXEPath[MAX_PATH+1];


extern WCHAR g_wzLocalDevOverridePath[MAX_PATH + 1];
extern WCHAR g_wzGlobalDevOverridePath[MAX_PATH + 1];
extern DWORD g_dwDevOverrideFlags;

// Architecture string const
const WCHAR g_wzMSIL[]                  = L"MSIL";
const WCHAR g_wzX86[]                   = L"x86";
const WCHAR g_wzIA64[]                  = L"IA64";
const WCHAR g_wzAMD64[]                 = L"AMD64";



#define DISPLAY_NAME_DELIMITER L','
#define DISPLAY_NAME_DELIMITER_STRING L","

// ---------------------------------------------------------------------------
// CPropertyArray ctor
// ---------------------------------------------------------------------------
CPropertyArray::CPropertyArray()
{
    _dwSig = 0x504f5250; /* 'PORP' */
    memset(&_rProp, 0, ASM_NAME_MAX_PARAMS * sizeof(FusionProperty));
}

// ---------------------------------------------------------------------------
// CPropertyArray dtor
// ---------------------------------------------------------------------------
CPropertyArray::~CPropertyArray()
{
    for (DWORD i = 0; i < ASM_NAME_MAX_PARAMS; i++)
    {
        if (_rProp[i].cb > sizeof(DWORD))
        {
            if (_rProp[i].pv != NULL)
            {
                FUSION_DELETE_ARRAY((LPBYTE) _rProp[i].pv);
                _rProp[i].pv = NULL;
            }
        }
    }
}


// ---------------------------------------------------------------------------
// CPropertyArray::Set
// ---------------------------------------------------------------------------
HRESULT CPropertyArray::Set(DWORD PropertyId, 
    LPVOID pvProperty, DWORD cbProperty)
{
    HRESULT hr = S_OK;
    FusionProperty *pItem = NULL;
        
    pItem = &(_rProp[PropertyId]);

    if (!cbProperty && !pvProperty)
    {
        if (pItem->cb > sizeof(DWORD))
        {
            if (pItem->pv != NULL)
                FUSION_DELETE_ARRAY((LPBYTE) pItem->pv);
        }
        pItem->pv = NULL;
    }
    else if (cbProperty > sizeof(DWORD))
    {
        LPBYTE ptr = NEW(BYTE[cbProperty]);
        if (!ptr)
        {
            hr = E_OUTOFMEMORY;
            goto exit;
        }
        
        if (pItem->cb > sizeof(DWORD))
            FUSION_DELETE_ARRAY((LPBYTE) pItem->pv);

            memcpy(ptr, pvProperty, cbProperty);
            pItem->pv = ptr;
        }
    else
    {
        if (pItem->cb > sizeof(DWORD))
            FUSION_DELETE_ARRAY((LPBYTE) pItem->pv);

        memcpy(&(pItem->pv), pvProperty, cbProperty);

#ifdef _DEBUG
        if (PropertyId == ASM_NAME_ARCHITECTURE) {
            PEKIND pe = * ((PEKIND *)pvProperty);        
            _ASSERTE(pe != peInvalid);
        }
#endif
    }
    pItem->cb = cbProperty;

exit:
    return hr;
}     

// ---------------------------------------------------------------------------
// CPropertyArray::Get
// ---------------------------------------------------------------------------
HRESULT CPropertyArray::Get(DWORD PropertyId, 
    LPVOID pvProperty, LPDWORD pcbProperty)
{
    HRESULT hr = S_OK;
    FusionProperty *pItem;

    _ASSERTE(pcbProperty);

    if (PropertyId >= ASM_NAME_MAX_PARAMS
        || (!pvProperty && *pcbProperty))
    {
        _ASSERTE(!"Invalid Argument! Passed in NULL buffer with size non-zero!");
        hr = E_INVALIDARG;
        goto exit;
    }        

    pItem = &(_rProp[PropertyId]);

    if (pItem->cb > *pcbProperty)
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    else if (pItem->cb)
        memcpy(pvProperty, (pItem->cb > sizeof(DWORD) ? 
            pItem->pv : (LPBYTE) &(pItem->pv)), pItem->cb);

    *pcbProperty = pItem->cb;
        
exit:
    return hr;
}     

// ---------------------------------------------------------------------------
// CPropertyArray::operator []
// Wraps DWORD optimization test.
// ---------------------------------------------------------------------------
FusionProperty CPropertyArray::operator [] (DWORD PropertyId)
{
    FusionProperty prop;

    prop.pv = _rProp[PropertyId].cb > sizeof(DWORD) ?
        _rProp[PropertyId].pv : &(_rProp[PropertyId].pv);

    prop.cb = _rProp[PropertyId].cb;

    return prop;
}

// Creation funcs.

// ---------------------------------------------------------------------------
// CheckFieldsForFriendAssembly
// ---------------------------------------------------------------------------

STDAPI
CheckFieldsForFriendAssembly(
    LPASSEMBLYNAME     pAssemblyName)
{
    HRESULT hr = S_OK;
    DWORD dwSize=0;

    // Let's look at the information they gave us in the friends declaration.
    // If they put in a Processor Architecture, Culture, or Version, then we'll return an error.

    if (FAILED(hr = pAssemblyName->GetProperty(ASM_NAME_MAJOR_VERSION, NULL, &dwSize)) ||
        FAILED(hr = pAssemblyName->GetProperty(ASM_NAME_MINOR_VERSION, NULL, &dwSize)) ||
        FAILED(hr = pAssemblyName->GetProperty(ASM_NAME_BUILD_NUMBER, NULL, &dwSize)) ||
        FAILED(hr = pAssemblyName->GetProperty(ASM_NAME_REVISION_NUMBER, NULL, &dwSize)) ||
        FAILED(hr = pAssemblyName->GetProperty(ASM_NAME_CULTURE, NULL, &dwSize)) ||
        FAILED(hr = pAssemblyName->GetProperty(ASM_NAME_ARCHITECTURE, NULL, &dwSize)))
        {
            // If any of these calls failed due to an insufficient buffer, then that means
            // the assembly name contained them
            if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
                hr = META_E_CA_BAD_FRIENDS_ARGS;
    } else {
        if (FAILED(hr = pAssemblyName->GetProperty(ASM_NAME_PUBLIC_KEY_TOKEN, NULL, &dwSize))) {
                
            //
            // Public Key token should not be passed to InternalsVisibleTo 
            // attribute. This translates to the ASM_NAME_PUBLIC_KEY_TOKEN 
            // property being set, while the full public key is not.  
            //

            if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
                    
                dwSize = 0;    
                    
                if (FAILED(hr = pAssemblyName->GetProperty(ASM_NAME_PUBLIC_KEY, NULL, &dwSize))) {
                    if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
                        hr = S_OK;
                } else {
                    hr = META_E_CA_BAD_FRIENDS_ARGS;
                }
                    
            } 
        } else {
            hr = S_OK;
        }
    }
        

    return hr;
}

// ---------------------------------------------------------------------------
// CreateAssemblyNameObject
// ---------------------------------------------------------------------------
STDAPI
CreateAssemblyNameObject(
    LPASSEMBLYNAME    *ppAssemblyName,
    LPCOLESTR          szAssemblyName,
    DWORD              dwFlags,
    LPVOID             pvReserved)
{

    HRESULT hr = S_OK;
    CAssemblyName *pName = NULL;

    MEMORY_REPORT_CONTEXT_SCOPE("FusionAssemblyName");

    if (!ppAssemblyName)
    {
        hr = E_INVALIDARG;
        goto exit;
    }

    pName = NEW(CAssemblyName);
    if (!pName)
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }

    if (dwFlags & CANOF_PARSE_DISPLAY_NAME)
    {
        hr = pName->Init(NULL, NULL);
        if (FAILED(hr)) {
            goto exit;
        }

        hr = pName->Parse((LPWSTR)szAssemblyName);
    }
    else
    {
        hr = pName->Init(szAssemblyName, NULL);
    }


    if (SUCCEEDED(hr) && ((dwFlags & CANOF_VERIFY_FRIEND_ASSEMBLYNAME)))
    {
        hr = CheckFieldsForFriendAssembly(pName);
    }


    if (FAILED(hr)) 
    {
        SAFERELEASE(pName);
        goto exit;
    }

    *ppAssemblyName = pName;

exit:

    return hr;
}

// ---------------------------------------------------------------------------
// CreateAssemblyNameObjectFromMetaData
// ---------------------------------------------------------------------------

STDAPI
CreateAssemblyNameObjectFromMetaData(
    LPASSEMBLYNAME    *ppAssemblyName,
    LPCOLESTR          szAssemblyName,
    ASSEMBLYMETADATA  *pamd,
    LPVOID             pvReserved)
{

    HRESULT hr = S_OK;
    CAssemblyName *pName = NULL;

    pName = NEW(CAssemblyName);
    if (!pName)
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }

    hr = pName->Init(szAssemblyName, pamd);
        
    if (FAILED(hr)) 
    {
        SAFERELEASE(pName);
        goto exit;
    }

    *ppAssemblyName = pName;

exit:

    return hr;
}

// IUnknown methods

// ---------------------------------------------------------------------------
// CAssemblyName::AddRef
// ---------------------------------------------------------------------------
STDMETHODIMP_(ULONG)
CAssemblyName::AddRef()
{
    return InterlockedIncrement((LONG*) &_cRef);
}

// ---------------------------------------------------------------------------
// CAssemblyName::Release
// ---------------------------------------------------------------------------
STDMETHODIMP_(ULONG)
CAssemblyName::Release()
{
    ULONG ulRef = InterlockedDecrement((LONG*) &_cRef);
    if (ulRef == 0) 
    {
        delete this;
    }

    return ulRef;
}

// ---------------------------------------------------------------------------
// CAssemblyName::QueryInterface
// ---------------------------------------------------------------------------
STDMETHODIMP
CAssemblyName::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) 
        return E_POINTER;

    if (IsEqualIID(riid, IID_IAssemblyNameBinder)) {
        *ppv = static_cast<IAssemblyNameBinder*>(this);
        AddRef();
        return S_OK;
    }
    else if (   IsEqualIID(riid, IID_IUnknown)
        || IsEqualIID(riid, IID_IAssemblyName)
       )
    {
        *ppv = static_cast<IAssemblyName*> (this);
        AddRef();
        return S_OK;
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
}

// ---------------------------------------------------------------------------
// CAssemblyName::SetProperty
// ---------------------------------------------------------------------------
STDMETHODIMP
CAssemblyName::SetProperty(DWORD PropertyId, 
    LPVOID pvProperty, DWORD cbProperty)
{
    HRESULT hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    LPBYTE pbSN = NULL;
    DWORD  cbSN = 0;

    // Fail if finalized.
    if (_fIsFinalized)
    {
        _ASSERTE(!"SetProperty on a IAssemblyName while the name is finalized!");
        hr = E_UNEXPECTED;
        goto exit;
    }

    if (PropertyId >= ASM_NAME_MAX_PARAMS
        || (!pvProperty && cbProperty))
    {
        _ASSERTE(!"Invalid Argument! Passed in NULL buffer with size non-zero!");
        hr = E_INVALIDARG;
        goto exit;
    }        

    if (PropertyId == ASM_NAME_MAJOR_VERSION ||
        PropertyId == ASM_NAME_MINOR_VERSION ||
        PropertyId == ASM_NAME_BUILD_NUMBER  ||
        PropertyId == ASM_NAME_REVISION_NUMBER)
    {
        if (cbProperty > sizeof(WORD)) {
            hr = E_INVALIDARG;
            goto exit;
        }
    }

    // Check if public key is being set and if so,
    // set the public key token if not already set.
    if (PropertyId == ASM_NAME_PUBLIC_KEY)
    {
        // If setting true public key, generate hash.
        if (pvProperty && cbProperty)
        {
            // Generate the public key token from the pk.
            if (FAILED(hr = GetPublicKeyTokenFromPKBlob((LPBYTE) pvProperty, cbProperty, &pbSN, &cbSN)))
                goto exit;

            // Set the public key token property.
            if (FAILED(hr = SetProperty(ASM_NAME_PUBLIC_KEY_TOKEN, pbSN, cbSN)))
                goto exit;        
        }
        // Otherwise expect call to reset property.
        else if (!cbProperty)
        {
            if (FAILED(hr = SetProperty(ASM_NAME_PUBLIC_KEY_TOKEN, pvProperty, cbProperty)))
                goto exit;
        }
            
    }
    // Setting NULL public key clears values in public key,
    // public key token and sets public key token flag.
    else if (PropertyId == ASM_NAME_NULL_PUBLIC_KEY)
    {
        pvProperty = NULL;
        cbProperty = 0;
        hr = SetProperty(ASM_NAME_NULL_PUBLIC_KEY_TOKEN, pvProperty, cbProperty);
        goto exit;
    }
    // Setting or clearing public key token.
    else if (PropertyId == ASM_NAME_PUBLIC_KEY_TOKEN)
    {
        if (pvProperty && cbProperty)
            _fPublicKeyToken = TRUE;
        else if (!cbProperty)
            _fPublicKeyToken = FALSE;
    }
    // Setting NULL public key token clears public key token and
    // sets public key token flag.
    else if (PropertyId == ASM_NAME_NULL_PUBLIC_KEY_TOKEN)
    {
        _fPublicKeyToken = TRUE;
        pvProperty = NULL;
        cbProperty = 0;
        PropertyId = ASM_NAME_PUBLIC_KEY_TOKEN;
    }
    else if (PropertyId == ASM_NAME_CUSTOM)
    {
        if (pvProperty && cbProperty)
            _fCustom = TRUE;
        else if (!cbProperty)
            _fCustom = FALSE;
    }
    else if (PropertyId == ASM_NAME_NULL_CUSTOM)
    {
        _fCustom = TRUE;
        pvProperty = NULL;
        cbProperty = 0;
        PropertyId = ASM_NAME_CUSTOM;
    }

    // Setting "neutral" as the culture is the same as "" culture (meaning
    // culture-invariant).
    else if (PropertyId == ASM_NAME_CULTURE) {
        if (pvProperty && !FusionCompareStringI((LPWSTR)pvProperty, L"neutral")) {
            pvProperty = (void *)L"";
            cbProperty = sizeof(L"");
        }
    }

    // Set property on array.
    hr = _rProp.Set(PropertyId, pvProperty, cbProperty);

exit:
    if (SUCCEEDED(hr)) {
        LPWSTR              pwzOld;

        // Clear cache

        pwzOld = (LPWSTR)InterlockedExchangePointer((LPVOID *)&_pwzTextualIdentity, NULL);
        SAFEDELETEARRAY(pwzOld);
        pwzOld = (LPWSTR)InterlockedExchangePointer((LPVOID *)&_pwzTextualIdentityILFull, NULL);
        SAFEDELETEARRAY(pwzOld);
    }

    // Free memory allocated by crypto wrapper.
    if (pbSN) {
        StrongNameFreeBuffer(pbSN);
    }

    END_ENTRYPOINT_NOTHROW;
    return hr;
}


// ---------------------------------------------------------------------------
// CAssemblyName::GetProperty
// ---------------------------------------------------------------------------
STDMETHODIMP
CAssemblyName::GetProperty(DWORD PropertyId, 
    LPVOID pvProperty, LPDWORD pcbProperty)
{
    HRESULT hr;

    BEGIN_ENTRYPOINT_NOTHROW;

    // Retrieve the property.
    switch(PropertyId)
    {
        case ASM_NAME_NULL_PUBLIC_KEY_TOKEN:
        case ASM_NAME_NULL_PUBLIC_KEY:
        {
            hr = (_fPublicKeyToken && !_rProp[PropertyId].cb) ? S_OK : S_FALSE;
            break;
        }
        case ASM_NAME_NULL_CUSTOM:
        {
            hr = (_fCustom && !_rProp[PropertyId].cb) ? S_OK : S_FALSE;
            break;
        }
        default:        
        {
            hr = _rProp.Get(PropertyId, pvProperty, pcbProperty);
            break;
        }
    }
    END_ENTRYPOINT_NOTHROW;

    return hr;
}

// ---------------------------------------------------------------------------
// CAssemblyName::GetName
// ---------------------------------------------------------------------------
STDMETHODIMP
CAssemblyName::GetName(
        __inout LPDWORD lpcwBuffer,
        __out_ecount_opt(*lpcwBuffer) LPOLESTR pwzBuffer)
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    DWORD cbBuffer = *lpcwBuffer * sizeof(TCHAR);
    hr = GetProperty(ASM_NAME_NAME, pwzBuffer, &cbBuffer);
    *lpcwBuffer = cbBuffer / sizeof(TCHAR);
    END_ENTRYPOINT_NOTHROW;

    return hr;
}


// ---------------------------------------------------------------------------
// CAssemblyName::GetVersion
// ---------------------------------------------------------------------------
STDMETHODIMP
CAssemblyName::GetVersion(
        /* [out] */ LPDWORD pdwVersionHi,
        /* [out] */ LPDWORD pdwVersionLow)
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    // Get Assembly Version
    hr = GetVersion( ASM_NAME_MAJOR_VERSION, pdwVersionHi, pdwVersionLow);
    END_ENTRYPOINT_NOTHROW;
    return hr;
}


// ---------------------------------------------------------------------------
// CAssemblyName::GetVersion
// ---------------------------------------------------------------------------
HRESULT
CAssemblyName::GetVersion(
        /* [in]  */ DWORD   dwMajorVersionEnumValue,
        /* [out] */ LPDWORD pdwVersionHi,
        /* [out] */ LPDWORD pdwVersionLow)
{
    HRESULT     hr = S_OK;
    DWORD       cb = sizeof(WORD);
    WORD        wVerMajor = 0, wVerMinor = 0, wRevNo = 0, wBldNo = 0;

    if(!pdwVersionHi || !pdwVersionLow) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *pdwVersionHi = *pdwVersionLow = 0;

    if(FAILED( (hr = GetProperty(dwMajorVersionEnumValue, &wVerMajor, &(cb = sizeof(WORD))))))
        goto Exit;
    if (cb == 0) {
        hr = FUSION_E_INVALID_NAME;
        goto Exit;
    }

    if(FAILED( (hr = GetProperty(dwMajorVersionEnumValue+1, &wVerMinor, &(cb = sizeof(WORD))))))
        goto Exit;

    if (cb == 0) {
        hr = FUSION_E_INVALID_NAME;
        goto Exit;
    }

    if(FAILED( (hr = GetProperty(dwMajorVersionEnumValue+2, &wBldNo, &(cb = sizeof(WORD))))))
        goto Exit;
    if (cb == 0) {
        hr = FUSION_E_INVALID_NAME;
        goto Exit;
    }

    if(FAILED( (hr = GetProperty(dwMajorVersionEnumValue+3, &wRevNo, &(cb = sizeof(WORD))))))
        goto Exit;

    if (cb == 0) {
        hr = FUSION_E_INVALID_NAME;
        goto Exit;
    }

    *pdwVersionHi  = MAKELONG(wVerMinor, wVerMajor);
    *pdwVersionLow = MAKELONG(wRevNo, wBldNo);

Exit:
    return hr;
}

// ---------------------------------------------------------------------------
// CAssemblyName::GetFileVersion
// ---------------------------------------------------------------------------
HRESULT
CAssemblyName::GetFileVersion(
        /* [out] */ LPDWORD pdwVersionHi,
        /* [out] */ LPDWORD pdwVersionLow)
{
    return GetVersion( ASM_NAME_FILE_MAJOR_VERSION, pdwVersionHi, pdwVersionLow);
}

// ---------------------------------------------------------------------------
// CAssemblyName::IsEqual
// ---------------------------------------------------------------------------
STDMETHODIMP
CAssemblyName::IsEqual(LPASSEMBLYNAME pName, DWORD dwCmpFlags)
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    hr = IsEqualLogging(pName, dwCmpFlags, NULL);
    END_ENTRYPOINT_NOTHROW;
    return hr;
}

STDMETHODIMP
CAssemblyName::IsEqualLogging(LPASSEMBLYNAME pName, DWORD dwCmpFlags, CDebugLog *pdbglog)
{
    HRESULT hr = S_OK;
    DWORD dwPartialCmpMask = 0;
    BOOL  fIsPartial = FALSE;
    CAssemblyName *pCName = static_cast<CAssemblyName *>(pName);

    if(!pName) {
        return S_FALSE;
    }

    const DWORD SIMPLE_VERSION_MASK = ASM_CMPF_VERSION;

    FusionProperty propThis;
    FusionProperty propPara;

    // Get the ref partial comparison mask, if any.    
    fIsPartial = CAssemblyName::IsPartial(this, &dwPartialCmpMask);

    if (dwCmpFlags == ASM_CMPF_DEFAULT) {
         // Set all comparison flags.
        dwCmpFlags = ASM_CMPF_IL_ALL | ASM_CMPF_ARCHITECTURE;

        // don't compare architecture if ref does not have architecture.
        if (!(dwPartialCmpMask & ASM_CMPF_ARCHITECTURE)) {
            dwCmpFlags &= ~ASM_CMPF_ARCHITECTURE;
        }

        // Otherwise, if ref is simple (possibly partial)
        // we mask off all version bits.
        if (!CAssemblyName::IsStronglyNamed(this)) 
        {
            // we don't have a public key token, but we don't know
            // it is because we are simply named assembly or we are
            // just partial on public key token.
            if (dwPartialCmpMask & ASM_CMPF_PUBLIC_KEY_TOKEN)
            {
                // now we know we are simply named assembly since we
                // have a public key token, but it is NULL.
                dwCmpFlags &= ~SIMPLE_VERSION_MASK;
            }
            // If neither of these two cases then public key token
            // is not set in ref , but def may be simple or strong.
            // The comparison mask is chosen based on def.
            else
            {
                if (!CAssemblyName::IsStronglyNamed(pName))
                    dwCmpFlags &= ~SIMPLE_VERSION_MASK;            
            }
        }
    }   

    // Mask off flags (either passed in or generated
    // by default flag with the comparison mask generated 
    // from the ref.
    dwCmpFlags &= dwPartialCmpMask;

    
    // The individual name fields can now be compared..

    // Compare name

    if (dwCmpFlags & ASM_CMPF_NAME) 
    {
        propThis = _rProp[ASM_NAME_NAME];
        propPara = pCName->_rProp[ASM_NAME_NAME];

        if (propThis.cb != propPara.cb) 
        {
            DEBUGOUT(pdbglog, 0, ID_FUSLOG_ISEQUAL_DIFF_NAME);
            hr = S_FALSE;
            goto Exit;
        }
    
        if (propThis.cb && FusionCompareStringI((LPWSTR)propThis.pv, (LPWSTR)propPara.pv)) 
        {
            DEBUGOUT(pdbglog, 0, ID_FUSLOG_ISEQUAL_DIFF_NAME);
            hr = S_FALSE;
            goto Exit;
        }
    }

    // Compare version

    if (dwCmpFlags & ASM_CMPF_MAJOR_VERSION) 
    {
        propThis = _rProp[ASM_NAME_MAJOR_VERSION];
        propPara = pCName->_rProp[ASM_NAME_MAJOR_VERSION];

        if (*((LPWORD) propThis.pv) != *((LPWORD)propPara.pv))
        {
            DEBUGOUT(pdbglog, 0, ID_FUSLOG_ISEQUAL_DIFF_VERSION_MAJOR);
            hr = S_FALSE;
            goto Exit;
        }
    }

    if (dwCmpFlags & ASM_CMPF_MINOR_VERSION) 
    {
        propThis = _rProp[ASM_NAME_MINOR_VERSION];
        propPara = pCName->_rProp[ASM_NAME_MINOR_VERSION];

        if (*((LPWORD) propThis.pv) != *((LPWORD)propPara.pv))
        {
            DEBUGOUT(pdbglog, 0, ID_FUSLOG_ISEQUAL_DIFF_VERSION_MINOR);
            hr = S_FALSE;
            goto Exit;
        }
    }

    if (dwCmpFlags & ASM_CMPF_REVISION_NUMBER) 
    {
        propThis = _rProp[ASM_NAME_REVISION_NUMBER];
        propPara = pCName->_rProp[ASM_NAME_REVISION_NUMBER];

        if (*((LPWORD) propThis.pv) != *((LPWORD)propPara.pv))
        {
            DEBUGOUT(pdbglog, 0, ID_FUSLOG_ISEQUAL_DIFF_VERSION_REVISION);
            hr = S_FALSE;
            goto Exit;
        }
    }

    if (dwCmpFlags & ASM_CMPF_BUILD_NUMBER)
    {
        propThis = _rProp[ASM_NAME_BUILD_NUMBER];
        propPara = pCName->_rProp[ASM_NAME_BUILD_NUMBER];

        if (*((LPWORD) propThis.pv) != *((LPWORD)propPara.pv))
        {
            DEBUGOUT(pdbglog, 0, ID_FUSLOG_ISEQUAL_DIFF_VERSION_BUILD);
            hr = S_FALSE;
            goto Exit;
        }
    }

    // Compare public key token

    if (dwCmpFlags & ASM_CMPF_PUBLIC_KEY_TOKEN) 
    {
        // compare public key if both of them have public key set. 
        propThis = _rProp[ASM_NAME_PUBLIC_KEY];
        propPara = pCName->_rProp[ASM_NAME_PUBLIC_KEY];
        if (!propThis.cb || !propPara.cb) {
            // otherwise, compare public key token
            propThis = _rProp[ASM_NAME_PUBLIC_KEY_TOKEN];
            propPara = pCName->_rProp[ASM_NAME_PUBLIC_KEY_TOKEN];
        }
    
        if (propThis.cb != propPara.cb) {
            DEBUGOUT(pdbglog, 0, ID_FUSLOG_ISEQUAL_DIFF_PUBLIC_KEY_TOKEN);
            hr = S_FALSE;
            goto Exit; 
        }

        if (propThis.cb && memcmp(propThis.pv, propPara.pv, propThis.cb)) {
            DEBUGOUT(pdbglog, 0, ID_FUSLOG_ISEQUAL_DIFF_PUBLIC_KEY_TOKEN);
            hr = S_FALSE;
            goto Exit;
        }
    }

    // Compare Culture
    
    if (dwCmpFlags & ASM_CMPF_CULTURE)
    {
        propThis = _rProp[ASM_NAME_CULTURE];
        propPara = pCName->_rProp[ASM_NAME_CULTURE];

        if (propThis.cb != propPara.cb) 
        {
            DEBUGOUT(pdbglog, 0, ID_FUSLOG_ISEQUAL_DIFF_CULTURE);
            hr = S_FALSE;
            goto Exit;
        }
    
        if (propThis.cb && FusionCompareStringI((LPWSTR)propThis.pv, (LPWSTR)propPara.pv)) 
        {
            DEBUGOUT(pdbglog, 0, ID_FUSLOG_ISEQUAL_DIFF_CULTURE);
            hr = S_FALSE;
            goto Exit;
        }
    }

    // Compare Custom attribute.

    if (dwCmpFlags & ASM_CMPF_CUSTOM) 
    {
        propThis = _rProp[ASM_NAME_PUBLIC_KEY_TOKEN];
        propPara = pCName->_rProp[ASM_NAME_PUBLIC_KEY_TOKEN];
    
        if (propThis.cb != propPara.cb) {
            DEBUGOUT(pdbglog, 0, ID_FUSLOG_ISEQUAL_DIFF_CUSTOM);
            hr = S_FALSE;
            goto Exit; 
        }

        if (propThis.cb && memcmp(propThis.pv, propPara.pv, propThis.cb)) {
            DEBUGOUT(pdbglog, 0, ID_FUSLOG_ISEQUAL_DIFF_CUSTOM);
            hr = S_FALSE;
            goto Exit;
        }
    }

    // Compare Retarget flag
    if (dwCmpFlags & ASM_CMPF_RETARGET)
    {
        propThis = _rProp[ASM_NAME_RETARGET];
        propPara = pCName->_rProp[ASM_NAME_RETARGET];

        if (*((LPDWORD) propThis.pv) != *((LPDWORD)propPara.pv))
        {
            DEBUGOUT(pdbglog, 0, ID_FUSLOG_ISEQUAL_DIFF_RETARGET);
            hr = S_FALSE;
            goto Exit;
        }
    }

    // compare config mask
    if (dwCmpFlags & ASM_CMPF_CONFIG_MASK) 
    {
        propThis = _rProp[ASM_NAME_CONFIG_MASK];
        propPara = pCName->_rProp[ASM_NAME_CONFIG_MASK];

        if (*((LPDWORD) propThis.pv) != *((LPDWORD)propPara.pv))
        {
            hr = S_FALSE;
            goto Exit;
        }

    }

    // compare architecture
    if (dwCmpFlags & ASM_CMPF_ARCHITECTURE) 
    {
        propThis = _rProp[ASM_NAME_ARCHITECTURE];
        propPara = pCName->_rProp[ASM_NAME_ARCHITECTURE];
    
        if (propThis.cb != propPara.cb) {
            DEBUGOUT2(pdbglog, 0, ID_FUSLOG_ISEQUAL_DIFF_PROCESSOR_ARCHITECTURE, ProcessorArchitectureFromPEKIND(*((PEKIND *) propThis.pv)), ProcessorArchitectureFromPEKIND(*((PEKIND *) propPara.pv)));
            hr = S_FALSE;
            goto Exit; 
        }

        if (propThis.cb) {
            if (*((LPDWORD) propThis.pv) != *((LPDWORD)propPara.pv)) {
                DEBUGOUT2(pdbglog, 0, ID_FUSLOG_ISEQUAL_DIFF_PROCESSOR_ARCHITECTURE, ProcessorArchitectureFromPEKIND(*((PEKIND *) propThis.pv)), ProcessorArchitectureFromPEKIND(*((PEKIND *) propPara.pv)));
                hr = S_FALSE;
                goto Exit;
            }
        }
    }

    // compare MVID
    if (dwCmpFlags & ASM_CMPF_MVID) 
    {
        propThis = _rProp[ASM_NAME_MVID];
        propPara = pCName->_rProp[ASM_NAME_MVID];
    
        if (propThis.cb != propPara.cb) {
            hr = S_FALSE;
            goto Exit; 
        }

        if (propThis.cb && memcmp(propThis.pv, propPara.pv, propThis.cb)) {
            hr = S_FALSE;
            goto Exit;
        }
    }

    // compare Signature
    if (dwCmpFlags & ASM_CMPF_SIGNATURE) 
    {
        propThis = _rProp[ASM_NAME_SIGNATURE_BLOB];
        propPara = pCName->_rProp[ASM_NAME_SIGNATURE_BLOB];
    
        if (propThis.cb != propPara.cb) {
            hr = S_FALSE;
            goto Exit; 
        }

        if (propThis.cb && memcmp(propThis.pv, propPara.pv, propThis.cb)) {
            hr = S_FALSE;
            goto Exit;
        }
    }

    hr = S_OK;
Exit:
    return hr;
}

// ---------------------------------------------------------------------------
// CAssemblyName constructor
// ---------------------------------------------------------------------------
CAssemblyName::CAssemblyName()
{
    _dwSig              = 0x454d414e; /* 'EMAN' */
    _fIsFinalized       = FALSE;
    _fPublicKeyToken    = FALSE;
    _fCustom            = TRUE;
    _cRef               = 1;
    _pwzPathModifier    = NULL;
    _pwzTextualIdentity = NULL;
    _pwzTextualIdentityILFull = NULL;
}

// ---------------------------------------------------------------------------
// CAssemblyName destructor
// ---------------------------------------------------------------------------
CAssemblyName::~CAssemblyName()
{
    SAFEDELETEARRAY(_pwzPathModifier);
    SAFEDELETEARRAY(_pwzTextualIdentity);
    SAFEDELETEARRAY(_pwzTextualIdentityILFull);
}

// ---------------------------------------------------------------------------
// CAssemblyName::Init
// ---------------------------------------------------------------------------

HRESULT
CAssemblyName::Init(LPCTSTR pszAssemblyName, ASSEMBLYMETADATA *pamd)
{
    HRESULT hr = S_OK;

    // Name
    if (pszAssemblyName) 
    {
        hr = SetProperty(ASM_NAME_NAME, (LPTSTR) pszAssemblyName, 
            (lstrlenW(pszAssemblyName)+1) * sizeof(TCHAR));
        if (FAILED(hr))
            goto exit;
    }

    if (pamd) {
            // Major version
        if (FAILED(hr = SetProperty(ASM_NAME_MAJOR_VERSION,
                &pamd->usMajorVersion, sizeof(WORD)))
    
            // Minor version
            || FAILED(hr = SetProperty(ASM_NAME_MINOR_VERSION, 
                &pamd->usMinorVersion, sizeof(WORD)))
    
            // Revision number
            || FAILED(hr = SetProperty(ASM_NAME_REVISION_NUMBER, 
                &pamd->usRevisionNumber, sizeof(WORD)))
    
            // Build number
            || FAILED(hr = SetProperty(ASM_NAME_BUILD_NUMBER, 
                &pamd->usBuildNumber, sizeof(WORD)))
    
            // Culture
            || FAILED(hr = SetProperty(ASM_NAME_CULTURE,
                pamd->szLocale, pamd->cbLocale * sizeof(WCHAR)))
                )
            {
                goto exit;
            }
    }

exit:
    return hr;
}

HRESULT CAssemblyName::Clone(IAssemblyName **ppName)
{
    HRESULT         hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    CAssemblyName  *pClone = NULL;
    
    MEMORY_REPORT_CONTEXT_SCOPE("FusionAssemblyName");

    if (!ppName) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *ppName = NULL;

    pClone = NEW(CAssemblyName);
    if( !pClone ) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = CopyProperties(this, pClone, NULL, 0);
    if (FAILED(hr)) {
        goto Exit;
    }
    
    *ppName = pClone;
    (*ppName)->AddRef();

Exit:
    SAFERELEASE(pClone);
    END_ENTRYPOINT_NOTHROW;

    return hr;
}

/* static */
HRESULT CAssemblyName::CloneForBind(IAssemblyName *pNameSource, IAssemblyName **ppNameClone)
{
    HRESULT         hr = S_OK;
    CAssemblyName  *pSource = static_cast<CAssemblyName *>(pNameSource);
    CAssemblyName  *pClone = NULL;
    static const DWORD bindingProperties[] = {  ASM_NAME_NAME, 
                                                ASM_NAME_PUBLIC_KEY,
                                                ASM_NAME_PUBLIC_KEY_TOKEN,
                                                ASM_NAME_MAJOR_VERSION,
                                                ASM_NAME_MINOR_VERSION,
                                                ASM_NAME_BUILD_NUMBER,
                                                ASM_NAME_REVISION_NUMBER,
                                                ASM_NAME_CULTURE,
                                                ASM_NAME_RETARGET,
                                                ASM_NAME_ARCHITECTURE};
    DWORD dwSize = ARRAYSIZE(bindingProperties);
    
    MEMORY_REPORT_CONTEXT_SCOPE("FusionAssemblyName");

    if (!ppNameClone) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *ppNameClone = NULL;

    pClone = NEW(CAssemblyName);
    if( !pClone ) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = CopyProperties(pSource, pClone, bindingProperties, dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }
    
    *ppNameClone = pClone;
    (*ppNameClone)->AddRef();

Exit:
    SAFERELEASE(pClone);
    return hr;
}

HRESULT CAssemblyName::CopyProperties(CAssemblyName *pSource, CAssemblyName *pTarget, const DWORD properties[], DWORD dwSize)
{
    HRESULT         hr = S_OK;
    DWORD           i = 0;
    FusionProperty  prop;
    
    _ASSERTE(pSource && pTarget);

    if (!dwSize) {
        for( i = 0; i < ASM_NAME_MAX_PARAMS; i ++) {
            prop = pSource->_rProp[i];

            if (prop.cb) {
                if (FAILED(hr = pTarget->SetProperty(i, prop.pv, prop.cb))) {
                    goto Exit;
                }
            }
        }
    }
    else {
        for (i = 0; i<dwSize; i++) {
            _ASSERTE(properties[i] < ASM_NAME_MAX_PARAMS);
            prop = pSource->_rProp[properties[i]];
            if (prop.cb) {
                if (FAILED(hr = pTarget->SetProperty(properties[i], prop.pv, prop.cb))) {
                    goto Exit;
                }
            }   
        }
    }

    pTarget->_fPublicKeyToken = pSource->_fPublicKeyToken;
    pTarget->_fCustom = pSource->_fCustom;

    if (pSource->_pwzPathModifier) {
        pTarget->_pwzPathModifier = WSTRDupDynamic(pSource->_pwzPathModifier);
        if(!pTarget->_pwzPathModifier) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
    }

Exit:
    return hr;
}

/* static */
HRESULT CAssemblyName::GetHash(
        IAssemblyName *pName, 
        DWORD dwFlags,
        DWORD dwHashSize,
        LPDWORD pdwHash)
{
    HRESULT hr = S_OK;
    DWORD dwHash = 0;
    FusionProperty prop;
    DWORD i;
    CAssemblyName *pCName = static_cast<CAssemblyName *> (pName);

    _ASSERTE(pName && pdwHash);

    _ASSERTE(pCName);

    dwHash = *pdwHash;

    prop = pCName->_rProp[ASM_NAME_NAME];
    if (prop.cb) {
        dwHash = HashString((LPWSTR)prop.pv, dwHash, 0, FALSE);
    }

    if (dwFlags & ASM_DISPLAYF_VERSION) {
        for (i = 0; i < 4; i++) {
            prop = pCName->_rProp[ASM_NAME_MAJOR_VERSION + i];
            dwHash = HashBlob((LPBYTE)prop.pv, prop.cb, dwHash, 0);
        }
    }

    if (dwFlags & ASM_DISPLAYF_CULTURE) {
        prop = pCName->_rProp[ASM_NAME_CULTURE];
        if (prop.cb) {
            LPWSTR pwzCulture =(LPWSTR) prop.pv;
            if (FusionCompareStringI(pwzCulture, L"") && FusionCompareStringI(pwzCulture, L"neutral")) {
                dwHash = HashString((LPWSTR)prop.pv, dwHash, 0, FALSE);
            }
        }
    }

    if (dwFlags & ASM_DISPLAYF_PUBLIC_KEY_TOKEN) {
        prop = pCName->_rProp[ASM_NAME_PUBLIC_KEY_TOKEN];
        if (prop.cb) {
            dwHash = HashBlob((LPBYTE)prop.pv, prop.cb, dwHash, 0);
        }
    }

    if (dwFlags & ASM_DISPLAYF_CUSTOM) {
        prop = pCName->_rProp[ASM_NAME_CUSTOM];
        if (prop.cb) {
            dwHash = HashString((LPWSTR)prop.pv, dwHash, 0, FALSE);
        }
    }

    if (dwFlags & ASM_DISPLAYF_RETARGET) {
        prop = pCName->_rProp[ASM_NAME_RETARGET];
        if (prop.cb) {
            dwHash = HashBlob((LPBYTE)prop.pv, prop.cb, dwHash, 0);
        }
    }

    if (dwFlags & ASM_DISPLAYF_CONFIG_MASK) {
        prop = pCName->_rProp[ASM_NAME_CONFIG_MASK];
        if (prop.cb) {
            dwHash = HashBlob((LPBYTE)prop.pv, prop.cb, dwHash, 0);
        }
    }

    if (dwFlags & ASM_DISPLAYF_PROCESSORARCHITECTURE) {
        prop = pCName->_rProp[ASM_NAME_ARCHITECTURE];
        if (prop.cb) {
            PEKIND peKind = *((PEKIND *)prop.pv);
            if (peKind != peNone) {
                dwHash = HashBlob((LPBYTE)prop.pv, prop.cb, dwHash, 0);
            }
        }
    }

    *pdwHash = dwHash % dwHashSize;

    return hr;
}

STDMETHODIMP
CAssemblyName::Reserved(
        /* in      */  REFIID               refIID,
        /* in      */  IUnknown            *pUnkBindSink,
        /* in      */  IUnknown            *pUnkAppCtx,
        /* in      */  LPCOLESTR            szCodebaseIn,
        /* in      */  LONGLONG             llFlags,
        /* in      */  LPVOID               pvReserved,
        /* in      */  DWORD                cbReserved,
        /*     out */  VOID               **ppv)
{
    BEGIN_ENTRYPOINT_NOTHROW;
    END_ENTRYPOINT_NOTHROW;

    return E_NOTIMPL;
}

// ---------------------------------------------------------------------------
// CAssemblyName::BindToObject
// ---------------------------------------------------------------------------
STDMETHODIMP
CAssemblyName::BindToObject(
        /* in      */  REFIID               refIID,
        /* in      */  IUnknown            *pUnkBindSink,
        /* in      */  IUnknown            *pUnkAppCtx,
        /* in      */  LPCOLESTR            szCodebaseIn,
        /* in      */  LONGLONG             llFlags,
        /* in      */  LPVOID               pvReserved,
        /* in      */  DWORD                cbReserved,
        /*     out */  VOID               **ppv,
        /*     out */  VOID               **ppvNI)

{
    HRESULT                                    hr = S_OK;
    LPWSTR                                     szCodebaseDupe = NULL;
    LPWSTR                                     szCodebase = NULL;
    ICodebaseList                             *pCodebaseList = NULL;
    DWORD                                      dwSize = 0;
    CDebugLog                                 *pdbglog = NULL;
    IAssemblyBindSink                         *pAsmBindSink = NULL;
    IApplicationContext                       *pAppCtx = NULL;
    CAsmDownloadMgr                           *pDLMgr = NULL;
    CAssemblyDownload                         *padl = NULL;
    CApplicationContext                       *pCAppCtx = NULL;
    LPWSTR                                     pwzAsmName;


    MEMORY_REPORT_CONTEXT_SCOPE("FusionBindToObject");

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(hr = COR_E_STACKOVERFLOW);

    if (!ppv || !pUnkAppCtx || !pUnkBindSink) {
        hr = E_INVALIDARG;
        goto Exit;
    }
    
    *ppv = NULL;

    // Cannot have both name and codebase set
    if(szCodebaseIn) {
        if (*szCodebaseIn) {            
            dwSize = 0;
            if (GetProperty(ASM_NAME_NAME, NULL, &dwSize) == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
                hr = E_INVALIDARG;
                goto Exit;
            }
            dwSize = 0;
        }
    }

    if(szCodebaseIn && *szCodebaseIn) {
        if (lstrlenW(szCodebaseIn)+1 > MAX_URL_LENGTH) {
            hr = E_INVALIDARG;
            goto Exit;
        }
        szCodebaseDupe = WSTRDupDynamic(szCodebaseIn);
        if (!szCodebaseDupe) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
    }
    else if (IsSystem(this)) {
        *ppv = g_pSystemAssembly;
        if (g_pSystemAssembly) {
            g_pSystemAssembly->AddRef();
        }
        hr = S_OK;
        goto Exit;
    }

    hr = pUnkAppCtx->QueryInterface(IID_IApplicationContext, (void**)&pAppCtx);
    if (FAILED(hr)) {
        goto Exit;
    }

    pCAppCtx = static_cast<CApplicationContext *>(pAppCtx); // dynamic_cast
    if (!pCAppCtx) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    hr = pUnkBindSink->QueryInterface(IID_IAssemblyBindSink, (void **)&pAsmBindSink);
    if (FAILED(hr)) {
        goto Exit;
    }


    pwzAsmName = (LPWSTR)(_rProp[ASM_NAME_NAME].pv);
    if (pwzAsmName) {
        LPWSTR                    pwzPart = NULL;

        if (lstrlenW(pwzAsmName) >= MAX_PATH) {
            // Name is too long.
            hr = FUSION_E_INVALID_NAME;
            goto Exit;
        }

        pwzPart = NEW(WCHAR[MAX_URL_LENGTH]);
        if (!pwzPart) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        dwSize = MAX_URL_LENGTH;
        hr = UrlGetPartW(pwzAsmName, pwzPart, &dwSize, URL_PART_SCHEME, 0);
        if (SUCCEEDED(hr) && lstrlenW(pwzPart)) {
            // The assembly name looks like a protocol (ie. it starts with
            // the form protocol:// ). Abort binds in this case.

            hr = FUSION_E_INVALID_NAME;
            SAFEDELETEARRAY(pwzPart);
            goto Exit;
        }

        SAFEDELETEARRAY(pwzPart);
    }

    if (szCodebaseDupe) {
        szCodebase = StripFilePrefix((LPWSTR)szCodebaseDupe);
    }

    if (IsLoggingNeeded()) {
        IAssemblyName      *pNameCalling = NULL;
        IAssembly          *pAsmCalling = NULL;
        LPWSTR              pwzCallingAsm = NULL;
        DWORD               dw;

        pwzCallingAsm = NEW(WCHAR[MAX_URL_LENGTH]);
        if (!pwzCallingAsm) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        hr = StringCchCopy(pwzCallingAsm, MAX_URL_LENGTH, L"(Unknown)");
        if (FAILED(hr)) {
            goto Exit;
        }
    
        if (pvReserved && (llFlags & ASM_BINDF_PARENT_ASM_HINT)) {
            pAsmCalling = (IAssembly *)pvReserved;

            if (pAsmCalling->GetAssemblyNameDef(&pNameCalling) == S_OK) {
                dw = MAX_URL_LENGTH;
                LoggingGetDisplayName(pNameCalling, pwzCallingAsm, &dw, FALSE);
            }

            SAFERELEASE(pNameCalling);
        }

        CDebugLog::Create(pAppCtx, this, szCodebase, &pdbglog);
        DescribeBindInfo(pdbglog, pAppCtx, szCodebase, pwzCallingAsm);

        SAFEDELETEARRAY(pwzCallingAsm);
    }
    if (g_bFusionHosted && g_pHostAsmList && !(llFlags & ASM_BINDF_INSPECTION_ONLY)) {
        dwSize = 0;
        if (GetProperty(ASM_NAME_NAME, NULL, &dwSize) != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
            // where-ref bind is prohibited in a hosted environment except for inspection only.
            hr = FUSION_E_LOADFROM_BLOCKED;
            goto Exit;
        }
        dwSize = 0;
    }

    if (!(llFlags & ASM_BINDF_INSPECTION_ONLY)) {
        hr = IsValidAssemblyOnThisPlatform(this, FALSE);
        if (FAILED(hr)) {
            if (hr == HRESULT_FROM_WIN32(ERROR_BAD_FORMAT)) {
                hr = FUSION_E_INVALID_NAME;
                DEBUGOUT(pdbglog, 1, ID_FUSLOG_INCOMPATIBLE_PROCESSOR_ARCHITECTURE);
            }
            goto Exit;
        }
    }

    // ensure custom/config mask is not set
    if (_rProp[ASM_NAME_CUSTOM].cb || _rProp[ASM_NAME_CONFIG_MASK].cb) {
        hr = FUSION_E_INVALID_NAME;
        goto Exit;
    }

    // Setup policy cache in appctx
    hr = PreparePolicyCache(pAppCtx, NULL);
    if (FAILED(hr)) {
        DEBUGOUT(pdbglog, 1, ID_FUSLOG_POLICY_CACHE_FAILURE);
    }

    // Create download objects for the real assembly download
    hr = CAsmDownloadMgr::Create(&pDLMgr, this, pAppCtx, pCodebaseList,
                                 (szCodebase) ? (szCodebase) : (NULL),
                                 pdbglog, pvReserved, llFlags
                                 );
    if (FAILED(hr)) {
        goto Exit;
    }
    
    hr = CAssemblyDownload::Create(&padl, pDLMgr, pDLMgr, pdbglog, llFlags);
    if (FAILED(hr)) {
        goto Exit;
    }

    // Download app.cfg if we don't already have it

    hr = pCAppCtx->Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    dwSize = 0;
    hr = pAppCtx->Get(ACTAG_APP_CFG_DOWNLOAD_ATTEMPTED, NULL, &dwSize, 0);
    if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
            hr = DownloadAppCfg(pAppCtx, padl, pAsmBindSink, pdbglog, TRUE);
    }
    else {
        hr = S_OK;
    }

    pCAppCtx->Unlock();

    // If hr==S_OK, then we either had an app.cfg already, or
    // it was on the local hard disk.
    //
    // If hr==S_FALSE, then no app.cfg exists, continue regular download
    //
    // If hr==E_PENDING, then went async.

    if (SUCCEEDED(hr)) {
        hr = padl->PreDownload(FALSE, ppv, ppvNI);

        if (hr == S_OK) {
            hr = padl->AddClient(pAsmBindSink, TRUE);
            if (FAILED(hr)) {
                SAFERELEASE(pDLMgr);
                SAFERELEASE(padl);
                goto Exit;
            }

            hr = padl->KickOffDownload(TRUE);
        }
        else {
            // failure or completed synchronously
            FusionBindInfo               bindInfo;

            memset(&bindInfo, 0, sizeof(bindInfo));

            if (SUCCEEDED(pDLMgr->GetBindInfo(&bindInfo))) {
                if (FAILED(hr) && !IsLoggingNeeded()) {
                    SAFERELEASE(bindInfo.pdbglog);
                    CreateInformationalLogObject();
                    bindInfo.pdbglog = g_pInformationalLog;
                    if (bindInfo.pdbglog) {
                        bindInfo.pdbglog->AddRef();
                    }
                }
                if (pAsmBindSink) {
                    pAsmBindSink->OnProgress(ASM_NOTIFICATION_BIND_INFO,
                                             S_OK, NULL, 0, 0, &bindInfo, NULL);
                }

                SAFERELEASE(bindInfo.pdbglog);
                SAFERELEASE(bindInfo.pNamePolicy);
            }
            // Completed synchronously
            if (hr == S_FALSE) {
                hr = S_OK;
            }
        }

    }
Exit:
    ;

    if (hr != E_PENDING && (g_dwForceLog || (FAILED(hr) && (!pDLMgr || pDLMgr->LogResult() == S_OK)))) {
        if (pdbglog) {
            pdbglog->SetResultCode(FUSION_BIND_LOG_CATEGORY_DEFAULT, hr);
            DUMPDEBUGLOG(pdbglog, g_dwLogLevel);
        }
    }

    DUMPDEBUGLOGNGEN(pdbglog, g_dwLogLevel);


    SAFERELEASE(pDLMgr);
    SAFERELEASE(padl);
    SAFERELEASE(pdbglog);

    SAFERELEASE(pAsmBindSink);
    SAFERELEASE(pAppCtx);

    SAFEDELETEARRAY(szCodebaseDupe);

        END_SO_INTOLERANT_CODE;

    return hr;
}

HRESULT CAssemblyName::DescribeBindInfo(CDebugLog *pdbglog,
                                        IApplicationContext *pAppCtx,
                                        LPCWSTR wzCodebase,
                                        LPCWSTR pwzCallingAsm)
{
    HRESULT                                     hr = S_OK;
    DWORD                                       dwSize;
    WCHAR                                       wzProperty[MAX_PATH];
    FusionProperty                              propName = _rProp[ASM_NAME_NAME];

    if (!pdbglog || !pAppCtx) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    DEBUGOUTHEADER(pdbglog, 0, ID_FUSLOG_PREBIND_INFO_START);

    
    if (propName.cb) {
        // This is not a where-ref bind.
        dwSize = MAX_PATH;
        wzProperty[0] = L'\0';
        GetDisplayName(wzProperty, &dwSize, ASM_DISPLAYF_FULL);

        DEBUGOUTHEADER2(pdbglog, 0, ID_FUSLOG_PREBIND_INFO_DISPLAY_NAME, wzProperty, (IsPartial(this, 0)) ? (L"Partial") : (L"Fully-specified"));
    }
    else {
        _ASSERTE(wzCodebase);

        DEBUGOUTHEADER1(pdbglog, 0, ID_FUSLOG_PREBIND_INFO_WHERE_REF, wzCodebase);
    }

    // appbase
    dwSize = MAX_PATH * sizeof(WCHAR);
    wzProperty[0] = L'\0';
    pAppCtx->Get(ACTAG_APP_BASE_URL, wzProperty, &dwSize, 0);
    DEBUGOUTHEADER1(pdbglog, 0, ID_FUSLOG_PREBIND_INFO_APPBASE, wzProperty);

    // Dev Override

    if (g_dwDevOverrideFlags & DEVOVERRIDE_GLOBAL) {
        DEBUGOUTHEADER1(pdbglog, 0, ID_FUSLOG_PREBIND_INFO_DEVOVERRIDE_GLOBAL, g_wzGlobalDevOverridePath);
    }

    if (g_dwDevOverrideFlags & DEVOVERRIDE_LOCAL) {
        DEBUGOUTHEADER1(pdbglog, 0, ID_FUSLOG_PREBIND_INFO_DEVOVERRIDE_LOCAL, g_wzLocalDevOverridePath);
    }
    
    // devpath
    dwSize = MAX_PATH * sizeof(WCHAR);
    wzProperty[0] = L'\0';
    pAppCtx->Get(ACTAG_DEV_PATH, wzProperty, &dwSize, 0);

    if (wzProperty[0]) {
        DEBUGOUTHEADER1(pdbglog, 0, ID_FUSLOG_PREBIND_INFO_DEVPATH, wzProperty);
    }

    // private path
    dwSize = MAX_PATH * sizeof(WCHAR);
    wzProperty[0] = L'\0';
    pAppCtx->Get(ACTAG_APP_PRIVATE_BINPATH, wzProperty, &dwSize, 0);
    DEBUGOUTHEADER1(pdbglog, 0, ID_FUSLOG_PREBIND_INFO_PRIVATE_PATH, (wzProperty[0]) ? (wzProperty) : (L"NULL"));

    // dynamic base
    dwSize = MAX_PATH * sizeof(WCHAR);
    wzProperty[0] = L'\0';
    pAppCtx->Get(ACTAG_APP_DYNAMIC_BASE, wzProperty, &dwSize, 0);
    DEBUGOUTHEADER1(pdbglog, 1, ID_FUSLOG_PREBIND_INFO_DYNAMIC_BASE, (wzProperty[0]) ? (wzProperty) : (L"NULL"));

    // cache base
    dwSize = MAX_PATH * sizeof(WCHAR);
    wzProperty[0] = L'\0';
    pAppCtx->Get(ACTAG_APP_CACHE_BASE, wzProperty, &dwSize, 0);
    DEBUGOUTHEADER1(pdbglog, 1, ID_FUSLOG_PREBIND_INFO_CACHE_BASE, (wzProperty[0]) ? (wzProperty) : (L"NULL"));

    // App name
    dwSize = MAX_PATH * sizeof(WCHAR);
    wzProperty[0] = L'\0';
    pAppCtx->Get(ACTAG_APP_NAME, wzProperty, &dwSize, 0);
    DEBUGOUTHEADER1(pdbglog, 1, ID_FUSLOG_PREBIND_INFO_APP_NAME, (wzProperty[0]) ? (wzProperty) : (L"NULL"));

    // Calling assembly

    DEBUGOUTHEADER1(pdbglog, 0, ID_FUSLOG_CALLING_ASSEMBLY, ((pwzCallingAsm) ? (pwzCallingAsm) : L"(Unknown)"));

    // Output debug info trailer

    DEBUGOUTHEADER(pdbglog, 0, ID_FUSLOG_PREBIND_INFO_END);

Exit:
    return hr;
}

// ---------------------------------------------------------------------------
// CAssemblyName::Finalize
// ---------------------------------------------------------------------------
STDMETHODIMP
CAssemblyName::Finalize()
{
    BEGIN_ENTRYPOINT_NOTHROW;

    _fIsFinalized = TRUE;
    END_ENTRYPOINT_NOTHROW;

    return S_OK;
}

// ---------------------------------------------------------------------------
// CAssemblyName::GetDisplayName
// ---------------------------------------------------------------------------
STDMETHODIMP
CAssemblyName::GetDisplayName(
        __out_ecount_opt(*pccDisplayName) LPOLESTR  szDisplayName,
        __inout LPDWORD pccDisplayName, 
        DWORD dwDisplayFlags)
{
    HRESULT                      hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    if (!dwDisplayFlags) {
        dwDisplayFlags = ASM_DISPLAYF_DEFAULT;
    }

    hr =  GetDisplayNameLegacy(szDisplayName, pccDisplayName, dwDisplayFlags);
    END_ENTRYPOINT_NOTHROW;
    return hr;

}

#define REMAINING_BUFFER_SIZE ((*pccDisplayName) - (pszBuf - szDisplayName))

STDMETHODIMP
CAssemblyName::GetDisplayNameLegacy(
        __out_ecount_opt(*pccDisplayName) LPOLESTR  szDisplayName,
        __inout LPDWORD pccDisplayName, 
        DWORD dwDisplayFlags)
{
    HRESULT     hr = S_OK;
    WCHAR       szVersion[MAX_VERSION_DISPLAY_SIZE + 1];
    WCHAR       *pszBuf = NULL; // point to the next start of writing
    WCHAR       *szCulture = NULL;  
    DWORD       ccRequired = 0; // required cc, not including '\0'
    DWORD       ccBuf = 0;

    szVersion[0] = L'\0';

    FusionProperty prop;

    BOOL fRetarget = FALSE;

    // Validate input buffer.
    if(!pccDisplayName || (!szDisplayName && *pccDisplayName))
    {
        hr = E_INVALIDARG;
        goto exit;
    }

    pszBuf = szDisplayName;

    prop = _rProp[ASM_NAME_NAME];

    // Name required
    if(!prop.cb) {
        hr = FUSION_E_INVALID_NAME;
        goto exit;
    }
   
    ccRequired += prop.cb / sizeof(WCHAR) - 1 /*'\0'*/;

    if (*pccDisplayName > ccRequired) {
        memcpy(pszBuf, prop.pv, prop.cb);
        pszBuf = szDisplayName + ccRequired;
    }

    if(dwDisplayFlags & ASM_DISPLAYF_VERSION) {
        ccBuf = 0;
        for(DWORD i = 0; i < 4; i++)
        {
            prop = _rProp[ASM_NAME_MAJOR_VERSION + i];
            // No version -> we're done.
            if (!prop.cb)
                break;

            // We always copy a word
            _ASSERTE(prop.cb == sizeof(WORD));

            // Print to buf.        
            hr = StringCchPrintf(szVersion + ccBuf, MAX_VERSION_DISPLAY_SIZE - ccBuf + 1, L"%hu.", *((LPWORD)prop.pv));
            if (FAILED(hr)) {
                goto exit;
            }

            ccBuf += lstrlenW(szVersion+ccBuf);
        }
        
        if(ccBuf) {
            // Remove last '.' printed in above loop.
            ccBuf --;
            szVersion[ccBuf] = L'\0';

            ccBuf += CTSTRLEN(L", Version=");
            ccRequired += ccBuf; 

            if (*pccDisplayName > ccRequired) {
                ccBuf = (DWORD) (pszBuf - szDisplayName);
                hr = StringCchPrintf(pszBuf, *pccDisplayName - ccBuf, L", Version=%ws", szVersion);
                if (FAILED(hr)) {
                    goto exit;
                }
                pszBuf = szDisplayName + ccRequired;
            }
        }
    }

    // Display culture
    if (dwDisplayFlags & ASM_DISPLAYF_CULTURE) {
        prop = _rProp[ASM_NAME_CULTURE];
        if (prop.cb) {
            szCulture = (LPWSTR)prop.pv;
            if (!*szCulture) {
                szCulture = L"neutral";
                ccBuf = CTSTRLEN(L"neutral");
            }
            else {
                ccBuf = prop.cb / sizeof(WCHAR) - 1;
            }

            ccBuf += CTSTRLEN(L", Culture=");
            ccRequired += ccBuf;

            if (*pccDisplayName > ccRequired) {
                ccBuf = (DWORD)(pszBuf - szDisplayName);
                hr = StringCchPrintf(pszBuf, *pccDisplayName - ccBuf, L", Culture=%ws", szCulture);
                if (FAILED(hr)) {
                    goto exit;
                }
                pszBuf = szDisplayName + ccRequired;
            }
        }
    }
   
    if (dwDisplayFlags & ASM_DISPLAYF_PUBLIC_KEY_TOKEN) {
        prop = _rProp[ASM_NAME_PUBLIC_KEY_TOKEN];
        if (prop.cb) {
            ccBuf = 2 * prop.cb;
            ccBuf += CTSTRLEN(L", PublicKeyToken=");
            ccRequired += ccBuf;
           
            if (*pccDisplayName > ccRequired) {
                ccBuf = (DWORD)(pszBuf - szDisplayName);
                hr = StringCchPrintf(pszBuf, *pccDisplayName - ccBuf, L", PublicKeyToken=");
                if (FAILED(hr)) {
                    goto exit;
                }
                CParseUtils::BinToUnicodeHex((LPBYTE)prop.pv, prop.cb, pszBuf + CTSTRLEN(L", PublicKeyToken="));
                pszBuf = szDisplayName + ccRequired;
                *pszBuf = L'\0';
            }
        }
        else if (_fPublicKeyToken) {
            ccBuf = CTSTRLEN(L", PublicKeyToken=null");
            ccRequired += ccBuf;
           
            if (*pccDisplayName > ccRequired) {
                hr = StringCchCopy(pszBuf, REMAINING_BUFFER_SIZE, L", PublicKeyToken=null");
                if (FAILED(hr)) {
                    goto exit;
                }

                pszBuf = szDisplayName + ccRequired;
            }
        }
    }

    if (dwDisplayFlags & ASM_DISPLAYF_CUSTOM) {
        prop = _rProp[ASM_NAME_CUSTOM];
        if (prop.cb) {
            ccBuf = prop.cb / sizeof(WCHAR) - 1;
            ccBuf += CTSTRLEN(L", Custom=");
            ccRequired += ccBuf;
           
            if (*pccDisplayName > ccRequired) {
                ccBuf = (DWORD)(pszBuf - szDisplayName);
                hr = StringCchPrintf(pszBuf, *pccDisplayName - ccBuf, L", Custom=%ws", (LPWSTR)prop.pv);
                if (FAILED(hr)) {
                    goto exit;
                }
                pszBuf = szDisplayName + ccRequired;
            }
        }
        else if (_fCustom) {
            ccBuf = CTSTRLEN(L", Custom=null");
            ccRequired += ccBuf;
           
            if (*pccDisplayName > ccRequired) {
                hr = StringCchCopy(pszBuf, REMAINING_BUFFER_SIZE, L", Custom=null");
                if (FAILED(hr)) {
                    goto exit;
                }
                pszBuf = szDisplayName + ccRequired;
            }
        }
    }

    if (dwDisplayFlags & ASM_DISPLAYF_CONFIG_MASK) {
        prop = _rProp[ASM_NAME_CONFIG_MASK];
        if (prop.cb) {
            ccBuf = 2 * prop.cb;
            ccBuf += CTSTRLEN(L", ConfigMask=");
            ccRequired += ccBuf;
           
            if (*pccDisplayName > ccRequired) {
                hr = StringCchCopy(pszBuf, REMAINING_BUFFER_SIZE, L", ConfigMask=");
                if (FAILED(hr)) {
                    goto exit;
                }

                CParseUtils::BinToUnicodeHex((LPBYTE)prop.pv, prop.cb, pszBuf + CTSTRLEN(L", ConfigMask="));
                pszBuf = szDisplayName + ccRequired;
                *pszBuf = L'\0';
            }
        }
    }

    if (dwDisplayFlags & ASM_DISPLAYF_MVID) {
        prop = _rProp[ASM_NAME_MVID];
        if (prop.cb) {
            ccBuf = 2 * prop.cb;
            ccBuf += CTSTRLEN(L", MVID=");
            ccRequired += ccBuf;

            if (*pccDisplayName > ccRequired) {
                hr = StringCchCopy(pszBuf, REMAINING_BUFFER_SIZE, L", MVID=");
                if (FAILED(hr)) {
                    goto exit;
                }

                CParseUtils::BinToUnicodeHex((LPBYTE)prop.pv, prop.cb, pszBuf + CTSTRLEN(L", MVID="));
                pszBuf = szDisplayName + ccRequired;
                *pszBuf = L'\0';
            }
        }
    }

    // output retarget flag
    if(dwDisplayFlags & ASM_DISPLAYF_RETARGET) {
        prop = _rProp[ASM_NAME_RETARGET];
        if (prop.cb)
        {
            fRetarget = *((LPBOOL)prop.pv);
            if (fRetarget)
            {
                ccBuf = CTSTRLEN(L", Retargetable=Yes");
                ccRequired += ccBuf;
                if (*pccDisplayName > ccRequired) {
                    hr = StringCchCopy(pszBuf, REMAINING_BUFFER_SIZE, L", Retargetable=Yes");
                    if (FAILED(hr)) {
                        goto exit;
                    }

                    pszBuf = szDisplayName + ccRequired;
                }
            }
        }
    }

    // output processor architecture
    if(dwDisplayFlags & ASM_DISPLAYF_PROCESSORARCHITECTURE) {

        if(_rProp[ASM_NAME_ARCHITECTURE].cb) {
            DWORD PeKind = *((LPDWORD)_rProp[ASM_NAME_ARCHITECTURE].pv);
            LPCWSTR pwzArch = NULL;

            if(PeKind != peNone) {
                if(PeKind == peMSIL) {
                    pwzArch =  g_wzMSIL;
                    ccBuf = ARRAYSIZE(g_wzMSIL) - 1;
                }
                else if(PeKind == peI386) {
                    pwzArch = g_wzX86;
                    ccBuf = ARRAYSIZE(g_wzX86) - 1;
                }
                else if(PeKind == peIA64) {
                    pwzArch = g_wzIA64;
                    ccBuf = ARRAYSIZE(g_wzIA64) - 1;
                }
                else if(PeKind == peAMD64) {
                    pwzArch = g_wzAMD64;
                    ccBuf = ARRAYSIZE(g_wzAMD64) - 1;
                }

                if (pwzArch) {
                    ccBuf += CTSTRLEN(L", ProcessorArchitecture=");
                    ccRequired += ccBuf;

                    if ( *pccDisplayName > ccRequired) {
                        ccBuf = (DWORD)(pszBuf - szDisplayName);
                        hr = StringCchPrintf(pszBuf, *pccDisplayName - ccBuf, L", processorArchitecture=%ws", pwzArch);
                        if (FAILED(hr)) {
                            goto exit;
                        }
                        pszBuf = szDisplayName + ccRequired;
                    }
                }
            }
        }
    }

    // If we came in under buffer size null terminate.
    // otherwise ccBuf contains required buffer size.
    if(ccRequired < *pccDisplayName)
    {
        szDisplayName[ccRequired] = L'\0';
        hr = S_OK;
    }
    // Indicate to caller.
    else {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        if (szDisplayName) {
            szDisplayName[0] = L'\0';
        }
    }
        
    // In either case indicate required size.
    *pccDisplayName = ccRequired + 1;

exit:
    return hr;
}

// ---------------------- Private funcs --------------------------------------

//--------------------------------------------------------------------
// Parse
//   Parses ASSEMBLYMETADATA and text name from simple string rep.
//--------------------------------------------------------------------
HRESULT CAssemblyName::Parse(LPCWSTR szDisplayName)
{
    return ParseLegacy(szDisplayName);
}

HRESULT CAssemblyName::ParseLegacy(__in_z LPCWSTR szDisplayName)
{
    HRESULT hr;
    
    WCHAR* szBuffer = NULL;
    WCHAR* pszBuf = NULL;
    WCHAR* pszToken = NULL;
    WCHAR* pszKey = NULL;
    WCHAR* pszValue = NULL;

    DWORD ccBuffer;
    DWORD ccKey;
    DWORD ccValue;
    DWORD ccBuf;
    DWORD ccToken;
        
    WORD wVer[4] = {0,0,0,0};
    WORD wFileVer[4] = {0,0,0,0};
    
    BOOL fRetarget = FALSE;
    DWORD i=0;
    DWORD dwCountOfAssemblyVers=0;
    DWORD dwCountOfFileVers = 0;
    
    DWORD cbProp;
    BYTE *pbProp = NULL;

    DWORD dwParsed = 0;
    
    // Verify display name passed in.
    if (!(szDisplayName && *szDisplayName))
    {
        hr = E_INVALIDARG;
        goto exit;
    }

    // Make local copy for parsing.
    ccBuffer = lstrlenW(szDisplayName) + 1;
    szBuffer = NEW(WCHAR[ccBuffer]);
    if (!szBuffer)
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }
    memcpy(szBuffer, szDisplayName, ccBuffer * sizeof(WCHAR));

    // Begin parsing buffer.
    pszBuf = szBuffer;
    ccBuf = ccBuffer;
    
    // Get first comma or NULL delimited token.
    if (!CParseUtils::GetDelimitedToken(&pszBuf, &ccBuf, &pszToken, &ccToken, DISPLAY_NAME_DELIMITER, FALSE, NULL))
    {
        hr = FUSION_E_INVALID_NAME;
        goto exit;
    }
    
    *(pszToken + ccToken) = L'\0';
    // Set name.
    hr = SetProperty(ASM_NAME_NAME, pszToken, (ccToken + 1) * sizeof(WCHAR));
    if (FAILED(hr)) {
        goto exit;
    }

    // Get additional params. 
    while (CParseUtils::GetDelimitedToken(&pszBuf, &ccBuf, &pszToken, &ccToken, DISPLAY_NAME_DELIMITER, FALSE, NULL)) {
        // Parse key=vaue form
        if (CParseUtils::GetKeyValuePair(pszToken, ccToken, &pszKey, &ccKey, &pszValue, &ccValue)) {
            if (pszValue) {
                *(pszValue + ccValue) = L'\0';

                // Order of comparison corresponds to the most seen order, 
                // like, "name, version=..., culture=..., publickeytoken=..."
                
                // version
                if ((!(dwParsed & FUSION_NAME_PARSED_VERSION)) && (ccKey == CTSTRLEN(L"Version")) && !FusionCompareStringNI(pszKey, L"Version", ccKey)) {
                    dwParsed |= FUSION_NAME_PARSED_VERSION;

                    ccValue++;
                    INT iVersion;
                    dwCountOfAssemblyVers = 0;
                    while (CParseUtils::GetDelimitedToken(&pszValue, &ccValue, &pszToken, &ccToken, L'.', FALSE, NULL)) {
                        if (dwCountOfAssemblyVers < VERSION_STRING_SEGMENTS) {
                            iVersion = StrToInt(pszToken);
                            if (iVersion > 0xffff)
                            {
                                hr = FUSION_E_INVALID_NAME;
                                goto exit;
                            }
                            wVer[dwCountOfAssemblyVers++] = (WORD)iVersion;
                        }
                    }

                    for (i = 0; i < dwCountOfAssemblyVers; i++) {
                        if (FAILED(hr = SetProperty(i + ASM_NAME_MAJOR_VERSION,
                            &wVer[i], sizeof(WORD))))
                            goto exit;
                    }
                }
                // Culture
                else if (!(dwParsed & FUSION_NAME_PARSED_CULTURE) && (ccKey == CTSTRLEN(L"Culture")) && !FusionCompareStringNI(pszKey, L"Culture", ccKey)) {
                    dwParsed |= FUSION_NAME_PARSED_CULTURE;

                    if (!ccValue || !FusionCompareStringI(pszValue, L"neutral")) {
                        LPWSTR nullStr = L"\0";
                        hr = SetProperty(ASM_NAME_CULTURE, nullStr, sizeof(WCHAR));
                        if (FAILED(hr)) {
                            goto exit;
                        }
                    }
                    else {
                        hr = SetProperty(ASM_NAME_CULTURE, pszValue, (ccValue + 1) * sizeof(WCHAR));
                        if (FAILED(hr)) {
                            goto exit;
                        }
                    }
                }
                // Public Key Token
                else if (!(dwParsed & FUSION_NAME_PARSED_PUBLIC_KEY_TOKEN) && (ccKey == CTSTRLEN(L"PublicKeyToken")) && !FusionCompareStringNI(pszKey, L"PublicKeyToken", ccKey)) {
                    dwParsed |= FUSION_NAME_PARSED_PUBLIC_KEY_TOKEN;

                    // SN=NULL/PK=NULL sets null property.
                    if (!ccValue || ((ccValue == (sizeof("NULL") -1 )) && !(FusionCompareStringNI(pszValue, L"NULL", sizeof("NULL") - 1)))) {
                        hr = SetProperty(ASM_NAME_NULL_PUBLIC_KEY_TOKEN, NULL, 0);
                        if (FAILED(hr)) {
                            goto exit;
                        }
                    }
                    // Otherwise setting public key or public key token.
                    else {
                        cbProp = ccValue / 2;
                        pbProp = NEW(BYTE[cbProp]);
                        if (!pbProp)
                        {
                            hr = E_OUTOFMEMORY;
                            goto exit;
                        }                
                        CParseUtils::UnicodeHexToBin(pszValue, ccValue, pbProp);
                        hr = SetProperty(ASM_NAME_PUBLIC_KEY_TOKEN, pbProp, cbProp);
                        if (FAILED(hr)) {
                            goto exit;
                        }
                        SAFEDELETEARRAY(pbProp);
                    }
                }               
                // Custom
                else if (!(dwParsed & FUSION_NAME_PARSED_CUSTOM) && (ccKey == CTSTRLEN(L"Custom")) && !FusionCompareStringNI(pszKey, L"Custom", ccKey)) {
                    dwParsed |= FUSION_NAME_PARSED_CUSTOM;

                    // Custom=null sets null property
                    if (!ccValue || ((ccValue == (sizeof("NULL") -1 )) && !(FusionCompareStringNI(pszValue, L"NULL", sizeof("NULL") - 1)))) {
                        hr = SetProperty(ASM_NAME_NULL_CUSTOM, NULL, 0);
                        if (FAILED(hr)) {
                            goto exit;
                        }
                    }   
                    // Otherwise setting custom blob.
                    else {
                        hr = SetProperty(ASM_NAME_CUSTOM, pszValue, (ccValue+1)*sizeof(WCHAR));
                        if (FAILED(hr)) {
                            goto exit;
                        }
                    }
                }
                // MVID
                else if (!(dwParsed & FUSION_NAME_PARSED_MVID) && (ccKey == CTSTRLEN(L"MVID")) && !FusionCompareStringNI(pszKey, L"MVID", ccKey)) {
                    dwParsed |= FUSION_NAME_PARSED_MVID;
                    if (ccValue) {
                        if (ccValue != MVID_LENGTH * 2) {
                            hr = FUSION_E_INVALID_NAME;
                            goto exit;
                        }

                        cbProp = ccValue/ 2;
                        pbProp = NEW(BYTE[cbProp]);
                        if (!pbProp) {
                            hr = E_OUTOFMEMORY;
                            goto exit;
                        }                
                        CParseUtils::UnicodeHexToBin(pszValue, ccValue, pbProp);
                        hr = SetProperty(ASM_NAME_MVID, pbProp, cbProp);
                        if (FAILED(hr)) {
                            goto exit;
                        }
                        SAFEDELETEARRAY(pbProp);
                    }
                }               
                // Config Mask
                else if (!(dwParsed & FUSION_NAME_PARSED_CONFIG_MASK) && (ccKey == CTSTRLEN(L"ConfigMask")) && !FusionCompareStringNI(pszKey, L"ConfigMask", ccKey)) {
                    dwParsed |= FUSION_NAME_PARSED_CONFIG_MASK;
                    if (ccValue) {
                        if (ccValue != sizeof(DWORD) * 2) {
                            hr = FUSION_E_INVALID_NAME;
                            goto exit;
                        }

                        cbProp = ccValue/ 2;
                        pbProp = NEW(BYTE[cbProp]);
                        if (!pbProp) {
                            hr = E_OUTOFMEMORY;
                            goto exit;
                        }                
                        CParseUtils::UnicodeHexToBin(pszValue, ccValue, pbProp);
                        hr = SetProperty(ASM_NAME_CONFIG_MASK, pbProp, cbProp);
                        if (FAILED(hr)) {
                            goto exit;
                        }
                        SAFEDELETEARRAY(pbProp);
                    }
                }
                // Retarget
                else if (!(dwParsed & FUSION_NAME_PARSED_RETARGET) && (ccKey == CTSTRLEN(L"Retargetable")) && !FusionCompareStringNI(pszKey, L"Retargetable", ccKey)) {
                    dwParsed |= FUSION_NAME_PARSED_RETARGET;

                    // retarget only accepts true or false
                    if ((ccValue == (sizeof("Yes") -1 )) && !(FusionCompareStringNI(pszValue, L"Yes", sizeof("Yes") - 1))) {
                        fRetarget = TRUE;
                    }
                    else if ((ccValue == (sizeof("No") -1 )) && !(FusionCompareStringNI(pszValue, L"No", sizeof("No") - 1))) {
                        fRetarget = FALSE;
                    }
                    else {
                        hr = FUSION_E_INVALID_NAME;
                        goto exit;
                    }
                   
                    if (fRetarget) {
                        if (FAILED(hr = SetProperty(ASM_NAME_RETARGET, &fRetarget, sizeof(BOOL))))
                            goto exit;    
                    }
                }
                // ProcessorArchitecture
                else if (!(dwParsed & FUSION_NAME_PARSED_PROCESSOR_ARCHITECTURE) && (ccKey == CTSTRLEN(L"ProcessorArchitecture")) && !FusionCompareStringNI(pszKey, L"ProcessorArchitecture", ccKey)) {
                    dwParsed |= FUSION_NAME_PARSED_PROCESSOR_ARCHITECTURE;
                    
                    PEKIND PeKind = peNone;
                    if (ccValue) {
                        hr = MapProcessorArchitectureToPEKIND(pszValue, &PeKind);
                        if(FAILED(hr)) {
                            goto exit;
                        }
                    }
                    else {
                        hr = FUSION_E_INVALID_NAME;
                        goto exit;
                    }

                    // Set this property
                    hr = SetProperty(ASM_NAME_ARCHITECTURE, (LPBYTE) &PeKind, sizeof(PeKind));
                    if(FAILED(hr)) {
                        goto exit;
                    }
                }
                // FileVersion
                else if (!(dwParsed & FUSION_NAME_PARSED_FILE_VERSION) && (ccKey == CTSTRLEN(L"FileVersion")) && !FusionCompareStringNI(pszKey, L"FileVersion", ccKey)) {
                    dwParsed |= FUSION_NAME_PARSED_FILE_VERSION;

                    ccValue++;
                    INT iVersion;
                    dwCountOfFileVers = 0;
                    while (CParseUtils::GetDelimitedToken(&pszValue, &ccValue, &pszToken, &ccToken, L'.', FALSE, NULL)) {
                        if (dwCountOfFileVers == VERSION_STRING_SEGMENTS) {
                            // too many dots
                            hr = FUSION_E_INVALID_NAME;
                            goto exit;
                        }

                        for(LPWSTR pwChar = pszToken; *pwChar && *pwChar != L'.'; pwChar++){
                            // has to be all digit
                            if (*pwChar < L'0' || *pwChar > L'9'){
                                hr = FUSION_E_INVALID_NAME;
                                goto exit;
                            }
                        }
                        if (dwCountOfFileVers < VERSION_STRING_SEGMENTS) {
                            iVersion = StrToInt(pszToken);
                            if (iVersion > 0xffff)
                            {
                                hr = FUSION_E_INVALID_NAME;
                                goto exit;
                            }
                            wFileVer[dwCountOfFileVers ++] = (WORD)iVersion;
                        }
                    }

                    for (i = 0; i < dwCountOfFileVers ; i++) {
                        if (FAILED(hr = SetProperty(i + ASM_NAME_FILE_MAJOR_VERSION,
                            &wFileVer[i], sizeof(WORD))))
                            goto exit;
                    }
                }
                // Public Key
                else if (!(dwParsed & FUSION_NAME_PARSED_PUBLIC_KEY) && (ccKey == CTSTRLEN(L"PublicKey")) && !FusionCompareStringNI(pszKey, L"PublicKey", ccKey)) {
                    dwParsed |= FUSION_NAME_PARSED_PUBLIC_KEY;

                    // SN=NULL/PK=NULL sets null property.
                    if (!ccValue || (ccValue == (sizeof("NULL") -1 )) && !(FusionCompareStringNI(pszValue, L"NULL", sizeof("NULL") - 1))) {
                        hr = SetProperty(ASM_NAME_NULL_PUBLIC_KEY, NULL, 0);
                        if (FAILED(hr)) {
                            goto exit;
                        }
                    }
                    // Otherwise setting public key or public key token.
                    else {

                        if (ccValue % 2) {
                            hr = FUSION_E_INVALID_NAME;
                            goto exit;
                        }

                        cbProp = ccValue / 2;
                        pbProp = NEW(BYTE[cbProp]);
                        if (!pbProp)
                        {
                            hr = E_OUTOFMEMORY;
                            goto exit;
                        }                
                        CParseUtils::UnicodeHexToBin(pszValue, ccValue, pbProp);
                        hr = SetProperty(ASM_NAME_PUBLIC_KEY, pbProp, cbProp);
                        if (FAILED(hr)) {
                            goto exit;
                        }
                        SAFEDELETEARRAY(pbProp);
                    }
                }
            }
        }
    }

    // If pszRetarget is set, make sure the name is fully specified.
    if (dwParsed & FUSION_NAME_PARSED_RETARGET) {
        if (IsPartial(this, NULL)) {
            hr = FUSION_E_INVALID_NAME;
            goto exit;
        }
    }

exit:
    SAFEDELETEARRAY(szBuffer);
    SAFEDELETEARRAY(pbProp);

    return hr;
}

// ---------------------------------------------------------------------------
// CAssemblyName::GetPublicKeyTokenFromPKBlob
// ---------------------------------------------------------------------------
HRESULT CAssemblyName::GetPublicKeyTokenFromPKBlob(LPBYTE pbPublicKeyToken, DWORD cbPublicKeyToken,
    LPBYTE *ppbSN, LPDWORD pcbSN)
{    
    HRESULT hr = S_OK;

    // Generate the hash of the public key.
    if (!StrongNameTokenFromPublicKey(pbPublicKeyToken, cbPublicKeyToken, ppbSN, pcbSN))
    {
        hr = StrongNameErrorInfo();
    }

    return hr;
}

// ---------------------------------------------------------------------------
// CAssemblyName::GetPublicKeyToken
// ---------------------------------------------------------------------------
HRESULT CAssemblyName::GetPublicKeyToken(LPDWORD pcbBuf, LPBYTE pbBuf,
    BOOL fDisplay)
{    
    HRESULT hr;
    LPBYTE pbPublicKeyToken = NULL;
    DWORD cbPublicKeyToken = 0, cbRequired = 0;

    if (!pcbBuf) {
        hr = E_INVALIDARG;
        goto exit;
    }

    // If display format is not required then call 
    // GetProperty directly on the provided buffer.
    if (!fDisplay)
    {
        hr = GetProperty(ASM_NAME_PUBLIC_KEY_TOKEN, pbBuf, pcbBuf);
        goto exit;
    }

    // Otherwise, display format required.

    // Get the required public key token output buf size.
    hr = GetProperty(ASM_NAME_PUBLIC_KEY_TOKEN, NULL, &cbPublicKeyToken);

    // No public key token or unexpected error
    if (!cbPublicKeyToken || (hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)))
    {
        *pcbBuf = cbPublicKeyToken;
        goto exit;
    }

    // We will convert binary format to hex encoded unicode - 
    // Calculated actual output buffer size in bytes - 
    // one byte maps to two unicode chars = 4 bytes/byte + null term.
    cbRequired = (2 * cbPublicKeyToken + 1) * sizeof(WCHAR);

    // Inform client if insufficient buffer
    if (*pcbBuf < cbRequired)
    {
        *pcbBuf = cbRequired;
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        goto exit;
    }

    if (!pbBuf) {
        if (pcbBuf) {
            *pcbBuf = cbRequired;
        }

        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        goto exit;
    }

    // Allocate temp space for public key token
    pbPublicKeyToken = NEW(BYTE[cbPublicKeyToken]);
    if (!pbPublicKeyToken)
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }

    // Get the public key token
    if (FAILED(hr = GetProperty(ASM_NAME_PUBLIC_KEY_TOKEN, pbPublicKeyToken, &cbPublicKeyToken)))
        goto exit;

    // Convert to unicode.
    CParseUtils::BinToUnicodeHex(pbPublicKeyToken, cbPublicKeyToken, (LPWSTR) pbBuf);

    *pcbBuf = cbRequired;

exit:
    SAFEDELETEARRAY(pbPublicKeyToken);
    return hr;
}

// ---------------------------------------------------------------------------
// CAssemblyName::IsPartial
// ---------------------------------------------------------------------------
BOOL CAssemblyName::IsPartial(IAssemblyName *pIName, LPDWORD pdwCmpMask)
{
    DWORD dwCmpMask = 0;
    BOOL fPartial    = FALSE;

    static const ASM_NAME rNameFlags[] ={ASM_NAME_NAME, 
                                         ASM_NAME_CULTURE, 
                                         ASM_NAME_PUBLIC_KEY_TOKEN, 
                                         ASM_NAME_MAJOR_VERSION, 
                                         ASM_NAME_MINOR_VERSION, 
                                         ASM_NAME_BUILD_NUMBER, 
                                         ASM_NAME_REVISION_NUMBER, 
                                         ASM_NAME_CUSTOM
                                        };

    static const ASM_CMP_FLAGS rCmpFlags[] = {ASM_CMPF_NAME, 
                                              ASM_CMPF_CULTURE, 
                                              ASM_CMPF_PUBLIC_KEY_TOKEN, 
                                              ASM_CMPF_MAJOR_VERSION, 
                                              ASM_CMPF_MINOR_VERSION, 
                                              ASM_CMPF_BUILD_NUMBER, 
                                              ASM_CMPF_REVISION_NUMBER, 
                                              ASM_CMPF_CUSTOM
                                             };

    CAssemblyName *pName = static_cast<CAssemblyName*> (pIName); // dynamic_cast
    _ASSERTE(pName);
    
    DWORD iNumOfComparison = sizeof(rNameFlags) / sizeof(rNameFlags[0]);
    
    for (DWORD i = 0; i < iNumOfComparison; i++)
    {
        if (pName->_rProp[rNameFlags[i]].cb 
            || (rNameFlags[i] == ASM_NAME_PUBLIC_KEY_TOKEN
                && pName->_fPublicKeyToken)
            || (rNameFlags[i] == ASM_NAME_CUSTOM 
                && pName->_fCustom))
        {
            dwCmpMask |= rCmpFlags[i];            
        }
        else {
            fPartial = TRUE;
        }
    }

    if(pName->_rProp[ASM_NAME_ARCHITECTURE].cb) {
        dwCmpMask |= ASM_CMPF_ARCHITECTURE;
    }

    if (pName->_rProp[ASM_NAME_RETARGET].cb) {
        dwCmpMask |= ASM_CMPF_RETARGET;
    }

    if (pName->_rProp[ASM_NAME_CONFIG_MASK].cb) {
        dwCmpMask |= ASM_CMPF_CONFIG_MASK;
    }

    if (pName->_rProp[ASM_NAME_MVID].cb) {
        dwCmpMask |= ASM_CMPF_MVID;
    }

    if (pName->_rProp[ASM_NAME_SIGNATURE_BLOB].cb) {
        dwCmpMask |= ASM_CMPF_SIGNATURE;
    }

    if (pdwCmpMask)
        *pdwCmpMask = dwCmpMask;

    return fPartial;
}


// ---------------------------------------------------------------------------
// CAssemblyName::GetVersion
//---------------------------------------------------------------------------
HRESULT CAssemblyName::GetVersion(IAssemblyName *pName, 
                                  BOOL fFailIfIncomplete, 
                                  ULONGLONG *pullVer)
{
    ULONGLONG ullVer = 0;
    WORD  wVers[4] = {0, 0, 0, 0};
    DWORD dwSize = 0;
    HRESULT hr = S_OK;
    DWORD dwVerHigh = 0, dwVerLow = 0;

    if (fFailIfIncomplete) {
        hr = pName->GetVersion(&dwVerHigh, &dwVerLow);
        if (SUCCEEDED(hr)) {
            ullVer = ((ULONGLONG) dwVerHigh) << sizeof(DWORD) * 8;
            ullVer |= (ULONGLONG) dwVerLow;
        }
    }
    else {
        for (int i = 0; i < 4; i++ ) {
            dwSize = sizeof(WORD);
            hr = pName->GetProperty(ASM_NAME_MAJOR_VERSION + i, &wVers[i], &dwSize);
            if (FAILED(hr)) {
                goto Exit;
            }
            ullVer = ((ullVer << (sizeof(WORD) * 8))& UI64(0xFFFFFFFFFFFF0000)) | ((ULONGLONG)wVers[i]);
        }
    }

    *pullVer = ullVer;

Exit:
    return hr;
}

// ---------------------------------------------------------------------------
// CAssemblyName::SetPathModifier
//---------------------------------------------------------------------------
HRESULT CAssemblyName::SetPathModifier(LPCWSTR pwzPathModifyer)
{
    HRESULT     hr = S_OK;

    _ASSERTE(!_pwzPathModifier);

    if(pwzPathModifyer) {
        if(lstrlenW(pwzPathModifyer) > (int) MAX_PATH_MODIFIER_LENGTH) {
            hr = E_INVALIDARG;
            goto Exit;
        }

        _pwzPathModifier = NEW(WCHAR[MAX_PATH_MODIFIER_LENGTH]);
        if(!_pwzPathModifier) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        *_pwzPathModifier = L'\0';
        hr = StringCchCopy(_pwzPathModifier, MAX_PATH_MODIFIER_LENGTH, pwzPathModifyer);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

Exit:
    return hr;
}

// ---------------------------------------------------------------------------
// CAssemblyName::GetPathModifier
//---------------------------------------------------------------------------
HRESULT CAssemblyName::GetPathModifier(
            __out_ecount_opt(*pcchPathModifierSize) LPWSTR pwzPathModifier, 
            __inout LPDWORD pcchPathModifierSize)
{
    HRESULT     hr = S_OK;
    DWORD       dwLen = 0;

    if(!pcchPathModifierSize) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if(!_pwzPathModifier) {
        goto Exit;
    }

    dwLen = lstrlenW(_pwzPathModifier) + 1;   // Null term

    if(*pcchPathModifierSize < dwLen) {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        goto Exit;
    }

    if(!pwzPathModifier) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    hr = StringCchCopy(pwzPathModifier, *pcchPathModifierSize, _pwzPathModifier);
    if (FAILED(hr)) {
        goto Exit;
    }

Exit:

    if(pcchPathModifierSize) {
        *pcchPathModifierSize = dwLen;
    }

    return hr;
}

// ---------------------------------------------------------------------------
// CAssemblyName::IsLegacyAssembly
//---------------------------------------------------------------------------
BOOL CAssemblyName::IsLegacyAssembly()
{
    FusionProperty prop = _rProp[ASM_NAME_ARCHITECTURE];
    if (!prop.cb) {
        return TRUE;
    }
    else if (*((LPDWORD)prop.pv) == peNone) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

// 
//
STDAPI PreBindAssemblyEx(
                IApplicationContext *pAppCtx, IAssemblyName *pName, 
                IAssembly *pAsmParent, LPCWSTR pwzRuntimeVersion,  
                IAssemblyName **ppNamePostPolicy, LPVOID pvReserved)
{
    HRESULT             hr = S_OK;

    SO_NOT_MAINLINE_FUNCTION;

    if (!pAppCtx || !pName || !ppNamePostPolicy) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    // Name ref must be fully-specified
    if (CAssemblyName::IsPartial(pName, NULL)) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if (!CAssemblyName::IsStronglyNamed(pName)) {
        hr = CAssemblyName::CloneForBind(pName, ppNamePostPolicy);
        goto Exit;
    }

    hr = PreBindAssemblyExInternal(pAppCtx, pName, NULL, pwzRuntimeVersion, ppNamePostPolicy, NULL, pvReserved);

Exit:

    return hr;
}

STDAPI PreBindAssemblyExInternal(
                IApplicationContext *pAppCtx, IAssemblyName *pName, 
                CLoadContext *pLoadContext, LPCWSTR pwzRuntimeVersion,
                IAssemblyName **ppNamePostPolicy, LPDWORD pdwPoliciesApplied,
                LPVOID pvReserved)
{
    HRESULT                               hr = S_OK;
    DWORD                                 dwSize = 0;
    CPolicyCache                         *pPolicyCache = NULL;
    IAssembly                            *pAsm = NULL;
    IUnknown                             *pUnk = NULL;
    DWORD                                 dwPolicyApplied;

    SO_NOT_MAINLINE_FUNCTION;

    dwSize = 0;
    hr = pAppCtx->Get(ACTAG_APP_CFG_DOWNLOAD_ATTEMPTED, NULL, &dwSize, 0);
    if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
        hr = DownloadAppCfg(pAppCtx, NULL, NULL, NULL, FALSE);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    *ppNamePostPolicy = NULL;

    // Check policy cache
    
    dwSize = sizeof(pPolicyCache);
    hr = pAppCtx->Get(ACTAG_APP_POLICY_CACHE, &pPolicyCache, &dwSize, APP_CTX_FLAGS_INTERFACE);
    if (hr == S_OK) {
        // Policy cache exists.

        hr = pPolicyCache->LookupPolicy(pName, ppNamePostPolicy, NULL, &dwPolicyApplied);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (hr == S_OK) {
            if (pName->IsEqual(*ppNamePostPolicy, ASM_CMPF_DEFAULT) != S_OK) {
                // Policy resulted in a different ref. Fail.
    
                hr = FUSION_E_REF_DEF_MISMATCH;
                goto Exit;
            }

            goto Exit;
        }

        // Missed in policy cache. Fall through to apply policy.
    }
    else {
        // Policy cache didn't exist in this context.

        hr = PreparePolicyCache(pAppCtx, &pPolicyCache);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    // Apply Policy
    hr = ApplyPolicy(pName, pAppCtx, NULL, 
                     ppNamePostPolicy, NULL, NULL, &dwPolicyApplied);
    if (FAILED(hr)) {
        goto Exit;
    }

    // Record resolution in policy cache

    hr = pPolicyCache->InsertPolicy(pName, *ppNamePostPolicy, NULL, dwPolicyApplied);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (pName->IsEqual(*ppNamePostPolicy, ASM_CMPF_DEFAULT) != S_OK) {
        // Policy resulted in a different ref. Fail.

        hr = FUSION_E_REF_DEF_MISMATCH;
        goto Exit;
    }

    if (pdwPoliciesApplied) {
        *pdwPoliciesApplied = dwPolicyApplied;
    }

Exit:
    SAFERELEASE(pPolicyCache);
    SAFERELEASE(pAsm);

    SAFERELEASE(pUnk);

    return hr;
}

//
// PreBindAssembly
//
// Returns:
//    FUSION_E_REF_DEF_MISMATCH: Policy will be applied to the pName, and
//                               will result in a different def.
//
//    S_OK: Policy will not affect the pName. 
//
//    S_FALSE: Policy will not affect the pName, but we do not know the signature blob.
//

STDAPI PreBindAssembly(IApplicationContext *pAppCtx, IAssemblyName *pName,
                       IAssembly *pAsmParent, IAssemblyName **ppNamePostPolicy,
                       LPVOID pvReserved)
{
    return PreBindAssemblyEx(pAppCtx, pName, pAsmParent, NULL, ppNamePostPolicy, pvReserved);
}

HRESULT DownloadAppCfg(IApplicationContext *pAppCtx,
                       CAssemblyDownload *padl,
                       IAssemblyBindSink *pbindsink,
                       CDebugLog *pdbglog,
                       BOOL bAsyncAllowed)
{
    HRESULT                                hr = S_OK;
    DWORD                                  dwLen;
    DWORD                                  dwFileAttr;
    LPWSTR                                 wszURL=NULL;
    WCHAR                                  wszAppCfg[MAX_PATH];
    LPWSTR                                 wszAppBase=NULL;
    LPWSTR                                 wszAppBaseStr = NULL;
    LPWSTR                                 wszAppCfgFile = NULL;
    BOOL                                   bIsFileUrl = FALSE;
    static const DWORD                     dwAppBaseSize = MAX_URL_LENGTH*2+2;

    SO_NOT_MAINLINE_FUNCTION;

    if (!pAppCtx) {
        hr = E_INVALIDARG;
        goto Exit;
    }
    
    hr = ::AppCtxGetWrapper(pAppCtx, ACTAG_APP_BASE_URL, &wszAppBaseStr);
    if (FAILED(hr) || hr == S_FALSE) {
        goto Exit;
    }

    _ASSERTE(wszAppBaseStr && wszAppBaseStr[0]);

    if (!wszAppBaseStr || !lstrlenW(wszAppBaseStr)) {
        hr = E_UNEXPECTED;
        goto Exit;
    }

    wszAppBase = NEW(WCHAR[dwAppBaseSize]);
    if (!wszAppBase)
    {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    wszURL = wszAppBase + MAX_URL_LENGTH + 1;

    hr = StringCchCopy(wszAppBase, dwAppBaseSize, wszAppBaseStr);
    if (FAILED(hr)) {
        goto Exit;
    }

    dwLen = lstrlenW(wszAppBase) - 1;
    if (wszAppBase[dwLen] != L'/' && wszAppBase[dwLen] != L'\\') {
        if (dwLen + 1 >= MAX_URL_LENGTH) {
            hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            goto Exit;
        }

        hr = StringCchCat(wszAppBase, dwAppBaseSize, L"/");
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    hr = ::AppCtxGetWrapper(pAppCtx, ACTAG_APP_CONFIG_FILE, &wszAppCfgFile);
    if (FAILED(hr) || hr == S_FALSE) {
        goto Exit;
    }

    dwLen = MAX_URL_LENGTH;
    hr = UrlCombineUnescape(wszAppBase, wszAppCfgFile, wszURL, &dwLen, 0);
    if (hr == S_OK) {
        DEBUGOUT1(pdbglog, 1, ID_FUSLOG_APP_CFG_DOWNLOAD_LOCATION, wszURL);

        bIsFileUrl = UrlIsW(wszURL, URLIS_FILEURL);
        if (bIsFileUrl) {
            BOOL        fExists = FALSE;
            
            dwLen = MAX_PATH;
            
            hr = PathCreateFromUrlWrap(wszURL, wszAppCfg, &dwLen, 0);
            if (FAILED(hr)) {
                goto Exit;
            }

            // Indicate that we've already searched for app.cfg
            dwLen = sizeof(L"");
            hr = pAppCtx->Set(ACTAG_APP_CFG_DOWNLOAD_ATTEMPTED, (void *)L"", dwLen, 0);

            hr = CheckFileExistence(wszAppCfg, &fExists, &dwFileAttr);
            if(FAILED(hr)) {
                goto Exit;
            }

            if(!fExists || (dwFileAttr & FILE_ATTRIBUTE_DIRECTORY)) {
                // App.cfg does not exist (or user incorrectly specified a
                // directory!).
                DEBUGOUT1(pdbglog, 0, ID_FUSLOG_CFG_NOT_EXIST, wszAppCfg);
                hr = S_FALSE;
                goto Exit;
            }
            else {
                // Add app.cfg filepath to appctx
                DEBUGOUT1(pdbglog, 0, ID_FUSLOG_APP_CFG_FOUND, wszAppCfg);
                hr = SetAppCfgFilePath(pAppCtx, wszAppCfg);
                if (FAILED(hr)) {
                    goto Exit;
                }
            }
        }
        else {
            hr = HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
            goto Exit;
        }
    }

Exit:
    SAFEDELETEARRAY(wszAppBaseStr);
    SAFEDELETEARRAY(wszAppCfgFile);
    SAFEDELETEARRAY(wszAppBase);

    return hr;
}


// ---------------------------------------------------------------------------
// CAssemblyName::IsCustom
// Tests for custom data set (ref or def case) OR custom
// data specifically wildcarded (ref case only).
//---------------------------------------------------------------------------
/* static */
BOOL CAssemblyName::IsCustom(IAssemblyName *pName)
{
    DWORD dwCmpMask = 0;
    BOOL  bIsCustom;
    CAssemblyName *pCName = static_cast<CAssemblyName *> (pName);
    _ASSERTE(pCName);

    bIsCustom = (pCName->_rProp[ASM_NAME_CONFIG_MASK].cb != 0);

    if (bIsCustom) {
        bIsCustom = (pCName->_rProp[ASM_NAME_CUSTOM].cb != 0);

        if (!bIsCustom) {
            bIsCustom = IsPartial(pName, &dwCmpMask) && !(dwCmpMask & ASM_CMPF_CUSTOM);
        }
    }

    return bIsCustom;
}

/* static */
BOOL CAssemblyName::IsStronglyNamed(IAssemblyName *pName)
{
    CAssemblyName *pCName = static_cast<CAssemblyName *> (pName);
    _ASSERTE(pCName);
    
    return (pCName->_rProp[ASM_NAME_PUBLIC_KEY_TOKEN].cb != 0);
}
/* static */
BOOL CAssemblyName::IsSystem(IAssemblyName *pName)
{
    _ASSERTE(pName);

    if (pName == g_pSystemAssemblyName) {
        return TRUE;
    }
    else {
        return (pName->IsEqual(g_pSystemAssemblyName, ASM_CMPF_NAME) == S_OK);
    }
}


/* static */
HRESULT CAssemblyName::ParseCustomDisplayName(
            __in_z LPCWSTR szDisplayName,
            IAssemblyName **ppName)
{
    HRESULT hr;
    
    WCHAR* szBuffer = NULL;
    WCHAR* pszBuf = NULL;
    WCHAR* pszToken = NULL;
    WCHAR* pszValue = NULL;
    BOOL   bEscaped = FALSE; 

    DWORD ccBuffer;
    DWORD ccValue;
    DWORD ccBuf;
    DWORD ccToken;
        
    WORD wVer[4] = {0,0,0,0};
    
    DWORD i=0;
    DWORD dwCountOfAssemblyVers=0;
    
    DWORD cbProp;
    BYTE  bProp[PUBLIC_KEY_TOKEN_LEN];

    IAssemblyName *pName = NULL;

    // Verify display name passed in.
    if (!(szDisplayName && *szDisplayName))
    {
        hr = E_INVALIDARG;
        goto exit;
    }

    hr = CreateAssemblyNameObject(&pName, NULL, 0, NULL);
    if (FAILED(hr)) {
        goto exit;
    }

    // Make local copy for parsing.
    ccBuffer = lstrlenW(szDisplayName) + 1;
    szBuffer = NEW(WCHAR[ccBuffer]);
    if (!szBuffer)
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }
    memcpy(szBuffer, szDisplayName, ccBuffer * sizeof(WCHAR));

    // Begin parsing buffer.
    pszBuf = szBuffer;
    ccBuf = ccBuffer;
    
    // Get first comma or NULL delimited token.
    if (!CParseUtils::GetDelimitedToken(&pszBuf, &ccBuf, &pszToken, &ccToken, DISPLAY_NAME_DELIMITER, TRUE, &bEscaped))
    {
        hr = FUSION_E_INVALID_NAME;
        goto exit;
    }
    
    *(pszToken + ccToken) = L'\0';

    if (bEscaped) {
        CParseUtils::UnEscapeDelim(pszToken);
    }

    // Set name.
    hr = pName->SetProperty(ASM_NAME_NAME, pszToken, (lstrlenW(pszToken)+1) * sizeof(WCHAR));
    if (FAILED(hr)) {
        goto exit;
    }

    // version
    if (CParseUtils::GetDelimitedToken(&pszBuf, &ccBuf, &pszToken, &ccToken, DISPLAY_NAME_DELIMITER, FALSE, NULL)) {

        *(pszToken + ccToken) = L'\0';
        pszValue = pszToken;
        ccValue = ccToken;
        ccValue++;
        INT iVersion;
        dwCountOfAssemblyVers = 0;
        while (CParseUtils::GetDelimitedToken(&pszValue, &ccValue, &pszToken, &ccToken, L'.', FALSE, NULL)) {
            if (dwCountOfAssemblyVers == VERSION_STRING_SEGMENTS) {
                // too many dots
                hr = FUSION_E_INVALID_NAME;
                goto exit;
            }

            for(LPWSTR pwChar = pszToken; *pwChar && *pwChar != L'.'; pwChar++){
                // has to be all digit
                if (*pwChar < L'0' || *pwChar > L'9'){
                    hr = FUSION_E_INVALID_NAME;
                    goto exit;
                }
            }

            if (dwCountOfAssemblyVers < VERSION_STRING_SEGMENTS) {
                iVersion = StrToInt(pszToken);
                if (iVersion > 0xffff)
                {
                    hr = FUSION_E_INVALID_NAME;
                    goto exit;
                }
                wVer[dwCountOfAssemblyVers++] = (WORD)iVersion;
            }
        }

        for (i = 0; i < dwCountOfAssemblyVers; i++) {
            if (FAILED(hr = pName->SetProperty(i + ASM_NAME_MAJOR_VERSION,
                &wVer[i], sizeof(WORD))))
                goto exit;
        }
    }

    bEscaped = FALSE;
    // culture
    if (CParseUtils::GetDelimitedToken(&pszBuf, &ccBuf, &pszToken, &ccToken, DISPLAY_NAME_DELIMITER, TRUE, &bEscaped)) {
        *(pszToken + ccToken) = L'\0';
        if (!ccToken) {
            LPWSTR nullStr = L"\0";
            hr = pName->SetProperty(ASM_NAME_CULTURE, nullStr, sizeof(WCHAR));
            if (FAILED(hr)) {
                goto exit;
            }
        }
        else {
            if (bEscaped) {
                CParseUtils::UnEscapeDelim(pszToken);
            }
            hr = pName->SetProperty(ASM_NAME_CULTURE, pszToken, (lstrlenW(pszToken)+1) * sizeof(WCHAR));
            if (FAILED(hr)) {
                goto exit;
            }
        }
    }

    // public key token
    if (CParseUtils::GetDelimitedToken(&pszBuf, &ccBuf, &pszToken, &ccToken, DISPLAY_NAME_DELIMITER, FALSE, NULL)) {
        *(pszToken + ccToken) = L'\0';
        if (!ccToken) {
            hr = pName->SetProperty(ASM_NAME_NULL_PUBLIC_KEY_TOKEN, NULL, 0);
            if (FAILED(hr)) {
                goto exit;
            }
        }
        // Otherwise setting public key or public key token.
        else {
            // public key token is 16 characters long.
            if (ccToken!= PUBLIC_KEY_TOKEN_DISPLAY_LEN) {
                hr = FUSION_E_INVALID_NAME;
                goto exit;
            }

            cbProp = ccToken / 2;
            CParseUtils::UnicodeHexToBin(pszToken, ccToken, bProp);
            hr = pName->SetProperty(ASM_NAME_PUBLIC_KEY_TOKEN, bProp, cbProp);
            if (FAILED(hr)) {
                goto exit;
            }
        }
    }

    // PA
    if (CParseUtils::GetDelimitedToken(&pszBuf, &ccBuf, &pszToken, &ccToken, DISPLAY_NAME_DELIMITER, FALSE, NULL)) {
        *(pszToken + ccToken) = L'\0';
        PEKIND PeKind = peNone;
        if (ccToken) {
            hr = MapProcessorArchitectureToPEKIND(pszToken, &PeKind);
            if(FAILED(hr)) {
                goto exit;
            }

            // Set this property
            hr = pName->SetProperty(ASM_NAME_ARCHITECTURE, (LPBYTE) &PeKind, sizeof(PeKind));
            if(FAILED(hr)) {
                goto exit;
            }
        }
    }

    *ppName = pName;
    pName->AddRef();
    
exit:
    SAFEDELETEARRAY(szBuffer);
    SAFERELEASE(pName);

    return hr;
}


// custom format: "name,version,culture,pkt,pa"
/* static */
HRESULT CAssemblyName::GetCustomDisplayName(
            IAssemblyName *pName, 
            __out_ecount_opt(*pccDisplayName) LPOLESTR  szDisplayName,
            __inout LPDWORD pccDisplayName,
            DWORD dwDisplayFlags)
{
    HRESULT     hr = S_OK;
    WCHAR       szVersion[MAX_VERSION_DISPLAY_SIZE + 1];
    WCHAR       *szCulture = NULL;  
    DWORD       ccRequired = 0; // required cc, not including '\0'
    DWORD       ccBuf = 0;
    CAssemblyName *pCName = static_cast<CAssemblyName*> (pName);
    WCHAR       szPublicKeyToken[PUBLIC_KEY_TOKEN_DISPLAY_LEN+1];
    FusionProperty prop;
    WCHAR       *psz = NULL;
    DWORD        dwNumOfComma = 0;

    if (!pName) {
        return E_INVALIDARG;
    }
    
    // Validate input buffer.
    if(!pccDisplayName || (!szDisplayName && *pccDisplayName))
    {
        return E_INVALIDARG;
    }

    _ASSERTE(pCName);

    szVersion[0] = L'\0';

    if (!dwDisplayFlags) {
        dwDisplayFlags = ASM_DISPLAYF_DEFAULT;
    }

    prop = pCName->_rProp[ASM_NAME_NAME];

    // Name required
    if(!prop.cb) {
        hr = FUSION_E_INVALID_NAME;
        goto exit;
    }
   
    psz = (LPWSTR)prop.pv;
    while (*psz) {
        if (*psz == DISPLAY_NAME_DELIMITER || *psz == DISPLAY_NAME_ESCAPE_CHAR) {
            dwNumOfComma++;
        }
        psz++;
    }
    
    ccRequired += prop.cb / sizeof(WCHAR) - 1 /*'\0'*/;
    if (dwNumOfComma) {
        ccRequired += dwNumOfComma;
    }

    if (*pccDisplayName > ccRequired) {
        if (!dwNumOfComma) {
            memcpy(szDisplayName, prop.pv, prop.cb);
        }
        else {
            CParseUtils::EscapeDelim((LPWSTR)prop.pv, szDisplayName, DISPLAY_NAME_DELIMITER);
        }
    }

    if(dwDisplayFlags & ASM_DISPLAYF_VERSION) {
        ccBuf = 0;
        for(DWORD i = 0; i < 4; i++)
        {
            prop = pCName->_rProp[ASM_NAME_MAJOR_VERSION + i];
            // No version -> we're done.
            if (!prop.cb)
                break;

            // We always copy a word
            _ASSERTE(prop.cb == sizeof(WORD));

            // Print to buf.        
            hr = StringCchPrintf(szVersion + ccBuf, MAX_VERSION_DISPLAY_SIZE - ccBuf + 1, L"%hu.", *((LPWORD)prop.pv));
            if (FAILED(hr)) {
                goto exit;
            }
            ccBuf += lstrlenW(szVersion + ccBuf);
        }
        
        if(ccBuf) {
            // Remove last '.' printed in above loop.
            szVersion[ccBuf-1] = L'\0';
            ccRequired += ccBuf; 

            if (*pccDisplayName > ccRequired) {
                hr = StringCchCat(szDisplayName, *pccDisplayName, DISPLAY_NAME_DELIMITER_STRING);
                if (FAILED(hr)) {
                    goto exit;
                }

                hr = StringCchCat(szDisplayName, *pccDisplayName, szVersion);
                if (FAILED(hr)) {
                    goto exit;
                }
            }
        }
    }

    dwNumOfComma = 0;
    // Display culture
    if (dwDisplayFlags & ASM_DISPLAYF_CULTURE) {
        prop = pCName->_rProp[ASM_NAME_CULTURE];
        if (prop.cb) {
            szCulture = (LPWSTR)prop.pv;
            if (!*szCulture) {
                ccBuf = 1;
            }
            else {
                ccBuf = prop.cb / sizeof(WCHAR);
                psz = (LPWSTR)prop.pv;
                while (*psz) {
                    if (*psz == DISPLAY_NAME_DELIMITER || *psz == DISPLAY_NAME_ESCAPE_CHAR) {
                        dwNumOfComma++;
                    }
                    psz++;
                }
            }

            ccRequired += ccBuf + dwNumOfComma;

            if (*pccDisplayName > ccRequired) {
                hr = StringCchCat(szDisplayName, *pccDisplayName, DISPLAY_NAME_DELIMITER_STRING);
                if (FAILED(hr)) {
                    goto exit;
                }

                if (*szCulture) {
                    if (!dwNumOfComma) {
                        hr = StringCchCat(szDisplayName, *pccDisplayName, szCulture);
                        if (FAILED(hr)) {
                            goto exit;
                        }
                    }
                    else {
                        CParseUtils::EscapeDelim((LPWSTR)prop.pv, szDisplayName + lstrlenW(szDisplayName), DISPLAY_NAME_DELIMITER);
                    }
                }
            }
        }
    }
  
    // display pkt
    if (dwDisplayFlags & ASM_DISPLAYF_PUBLIC_KEY_TOKEN) {
        prop = pCName->_rProp[ASM_NAME_PUBLIC_KEY_TOKEN];
        if (prop.cb) {
            ccBuf = 2 * prop.cb + 1;
            ccRequired += ccBuf;
           
            if (*pccDisplayName > ccRequired) {
                CParseUtils::BinToUnicodeHex((LPBYTE)prop.pv, prop.cb, szPublicKeyToken);
                hr = StringCchCat(szDisplayName, *pccDisplayName, DISPLAY_NAME_DELIMITER_STRING);
                if (FAILED(hr)) {
                    goto exit;
                }

                hr = StringCchCat(szDisplayName, *pccDisplayName, szPublicKeyToken);
                if (FAILED(hr)) {
                    goto exit;
                }
            }
        }
        else {
            ccRequired++;
           
            if (*pccDisplayName > ccRequired) {
                hr = StringCchCat(szDisplayName, *pccDisplayName, DISPLAY_NAME_DELIMITER_STRING);
                if (FAILED(hr)) {
                    goto exit;
                }
            }
        }
    }

    // output processor architecture
    if(dwDisplayFlags & ASM_DISPLAYF_PROCESSORARCHITECTURE) {
        prop = pCName->_rProp[ASM_NAME_ARCHITECTURE];
        if(prop.cb) {
            DWORD PeKind = *((LPDWORD)prop.pv);
            LPCWSTR pwzArch = NULL;

            if(PeKind != peNone) {
                if(PeKind == peMSIL) {
                    pwzArch =  g_wzMSIL;
                    ccBuf = ARRAYSIZE(g_wzMSIL);
                }
                else if(PeKind == peI386) {
                    pwzArch = g_wzX86;
                    ccBuf = ARRAYSIZE(g_wzX86);
                }
                else if(PeKind == peIA64) {
                    pwzArch = g_wzIA64;
                    ccBuf = ARRAYSIZE(g_wzIA64);
                }
                else if(PeKind == peAMD64) {
                    pwzArch = g_wzAMD64;
                    ccBuf = ARRAYSIZE(g_wzAMD64);
                }

                if (pwzArch) {
                    ccRequired += ccBuf;

                    if ( *pccDisplayName > ccRequired) {
                        hr = StringCchCat(szDisplayName, *pccDisplayName, DISPLAY_NAME_DELIMITER_STRING);
                        if (FAILED(hr)) {
                            goto exit;
                        }

                        hr = StringCchCat(szDisplayName, *pccDisplayName, pwzArch);
                        if (FAILED(hr)) {
                            goto exit;
                        }
                    }
                }
            }
        }
    }

    // If we came in under buffer size null terminate.
    // otherwise ccBuf contains required buffer size.
    if(ccRequired < *pccDisplayName)
    {
        hr = S_OK;
    }
    // Indicate to caller.
    else {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        if (szDisplayName) {
            szDisplayName[0] = L'\0';
        }
    }
        
    // In either case indicate required size.
    *pccDisplayName = ccRequired + 1;

exit:
    return hr;
}

