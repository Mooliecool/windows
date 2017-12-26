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
#include "fusionp.h"
#include "xmlparser.h"
#include "list.h"
#include "cfgdata.h"
#include "nodefact.h"
#include "util.h"
#include "dbglog.h"
#include "policy.h"
#include "helpers.h"
#include "naming.h"
#include "parse.h"
#include "memoryreport.h"

CNodeFactory::CNodeFactory(CDebugLog *pdbglog, ParseCtl parseCtl)
: _cRef(1)
, _dwState(PSTATE_LOOKUP_CONFIGURATION)
, _dwCurDepth(0)
, _bGlobalSafeMode(FALSE)
, _pwzPrivatePath(NULL)
, _pdbglog(pdbglog)
, _pAsmInfo(NULL)
, _bCorVersionMatch(TRUE)
, _parseCtl(parseCtl)
, _pwzAssemblyStore(NULL)
, _pwzDevOverridePath(NULL)
{
    _dwSig = 0x54434146; /* 'TCAF' */
    if (_pdbglog) {
        _pdbglog->AddRef();
    }
}

CNodeFactory::~CNodeFactory()
{
    LISTNODE                           pos = NULL;
    LPCWSTR                            pwzCur = NULL;
    LPWSTR                             pwzLink = NULL;
    CAsmBindingInfo                   *pAsmInfo = NULL;
    CQualifyAssembly                  *pqa = NULL;

    SAFEDELETEARRAY(_pwzPrivatePath);
    SAFEDELETEARRAY(_pwzAssemblyStore);
    SAFEDELETEARRAY(_pwzDevOverridePath);
    SAFERELEASE(_pdbglog);

    pos = _listProcessedCfgs.GetHeadPosition();
    while (pos) {
        pwzCur = _listProcessedCfgs.GetNext(pos);
        SAFEDELETEARRAY(pwzCur);
    }

    pos = _listAsmInfo.GetHeadPosition();
    while (pos) {
        pAsmInfo = _listAsmInfo.GetNext(pos);
        SAFEDELETE(pAsmInfo);
    }

    _listAsmInfo.RemoveAll();

    pos = _listLinkedConfiguration.GetHeadPosition();
    while (pos) {
        pwzLink = _listLinkedConfiguration.GetNext(pos);
        SAFEDELETEARRAY(pwzLink);
    }

    pos = _listQualifyAssembly.GetHeadPosition();
    while (pos) {
        pqa = _listQualifyAssembly.GetNext(pos);
        SAFEDELETE(pqa);
    }

    _listQualifyAssembly.RemoveAll();

    SAFEDELETE(_pAsmInfo);
}

HRESULT CNodeFactory::QueryInterface(REFIID riid, void **ppv)
{
    HRESULT                                    hr = S_OK;

    if (!ppv) 
        return E_POINTER;
    
    *ppv = NULL;

    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IXMLNodeFactory)) {
        *ppv = static_cast<IXMLNodeFactory *>(this);
    }
    else {
        hr = E_NOINTERFACE;
    }

    if (*ppv) {
        AddRef();
    }

    return hr;
}

STDMETHODIMP_(ULONG) CNodeFactory::AddRef()
{
    return InterlockedIncrement((LONG *)&_cRef);
}

STDMETHODIMP_(ULONG) CNodeFactory::Release()
{
    ULONG                    ulRef = InterlockedDecrement((LONG *)&_cRef);

    if (!ulRef) {
        delete this;
    }

    return ulRef;
}

HRESULT CNodeFactory::NotifyEvent(IXMLNodeSource *pSource,
                                  XML_NODEFACTORY_EVENT iEvt)
{
    return S_OK;
}

HRESULT CNodeFactory::BeginChildren(IXMLNodeSource *pSource,
                                    XML_NODE_INFO *pNodeInfo)
{
    return S_OK;
}

HRESULT CNodeFactory::EndChildren(IXMLNodeSource *pSource, BOOL fEmpty,
                                  XML_NODE_INFO *pNodeInfo)
{
    HRESULT hr = S_OK;

    if (pNodeInfo->dwType == XML_ELEMENT) {
        _dwCurDepth--;
    }

    _nsmgr.OnEndChildren();

    // Unwind state

    if (_dwState == PSTATE_CONFIGURATION && _dwCurDepth < XML_CONFIGURATION_DEPTH) {
        _dwState = PSTATE_LOOKUP_CONFIGURATION;
    }
    else if (_dwState == PSTATE_RUNTIME && _dwCurDepth < XML_RUNTIME_DEPTH) {
        _dwState = PSTATE_CONFIGURATION;

        // End of <runtime> section.  We've seen all we need to see.
        // Early exit.
        if (_parseCtl == CNodeFactory::stopAfterRuntimeSection)
            pSource->Abort(NULL/*unused*/);
    }
    else if (_dwState == PSTATE_ASSEMBLYBINDING && _dwCurDepth < XML_ASSEMBLYBINDING_DEPTH) {
        _dwState = PSTATE_RUNTIME;
    }
    else if (_dwState == PSTATE_ASSEMBLYBINDINGV2 && _dwCurDepth < XML_ASSEMBLYBINDINGV2_DEPTH) {
        _dwState = PSTATE_RUNTIME;
    }
    else if (_dwState == PSTATE_ASSEMBLYBINDING_ROOT && _dwCurDepth < XML_ASSEMBLYBINDING_ROOT_DEPTH) {
        _dwState = PSTATE_CONFIGURATION;
    }
    else if (_dwState == PSTATE_DEPENDENTASSEMBLY && _dwCurDepth < XML_DEPENDENTASSEMBLY_DEPTH) {
        _dwState = PSTATE_ASSEMBLYBINDING;

        hr = AddToResult();
    }

    return hr;
}

HRESULT CNodeFactory::Error(IXMLNodeSource *pSource, HRESULT hrErrorCode,
                            USHORT cNumRecs, XML_NODE_INFO __RPC_FAR **aNodeInfo)
{
    DEBUGOUT1(_pdbglog, 1, ID_FUSLOG_XML_PARSE_ERROR_CODE, hrErrorCode);

    _nsmgr.Error(hrErrorCode);

    return S_OK;
}

HRESULT CNodeFactory::AddToResult()
{
    HRESULT         hr = S_OK;
    LISTNODE        pos;
    LISTNODE        oldpos;
    CBindingRedir   *pRedir;
    CAsmBindingInfo *pAsmInfo;
    WORD            wVers[4];
            
    if (_bCorVersionMatch) {
        _ASSERTE(_pAsmInfo);

        // Add assembly information if valid

        // check public keytoken
        if (_pAsmInfo->_pwzPublicKeyToken) {
            if (lstrlenW(_pAsmInfo->_pwzPublicKeyToken) != PUBLIC_KEY_TOKEN_DISPLAY_LEN) {
                // invalid public key token
                SAFEDELETE(_pAsmInfo);
                goto Exit;
            }
        }

        // remove invalid version redirects
        if ((_pAsmInfo->_listBindingRedir).GetCount()) {
            pos = (_pAsmInfo->_listBindingRedir).GetHeadPosition();
            while(pos) {
                oldpos = pos;
                pRedir = (_pAsmInfo->_listBindingRedir).GetNext(pos);
                if (IsMatchingVersion(pRedir->_pwzVersionOld, L"1.0.0.0") == HRESULT_FROM_WIN32(ERROR_INVALID_DATA)
                    || VersionFromString(pRedir->_pwzVersionNew, &(wVers[0]), &(wVers[1]), &(wVers[2]), &(wVers[3])) == HRESULT_FROM_WIN32(ERROR_INVALID_DATA)) {
                    (_pAsmInfo->_listBindingRedir).RemoveAt(oldpos);
                    SAFEDELETE(pRedir);
                }
            }
        }

        if (_pAsmInfo->_pwzName && (_pAsmInfo->_peKind != peInvalid)) {
            // remove binding redirects for simply named assemblies.
            if (!_pAsmInfo->_pwzPublicKeyToken && (_pAsmInfo->_listBindingRedir).GetCount()) {
                DEBUGOUT(_pdbglog, 1, ID_FUSLOG_XML_PRIVATE_ASM_REDIRECT);
            
                pos = (_pAsmInfo->_listBindingRedir).GetHeadPosition();
                while (pos) {
                    pRedir = (_pAsmInfo->_listBindingRedir).GetNext(pos);
                    SAFEDELETE(pRedir);
                }
            
                (_pAsmInfo->_listBindingRedir).RemoveAll();
            }

            // check duplicate assemblyBinding sections
            pos = _listAsmInfo.GetHeadPosition();
            while (pos) {
                pAsmInfo = _listAsmInfo.GetNext(pos);
                if (IsMatch(pAsmInfo, _pAsmInfo)) {
                    hr = MergeBindInfo(pAsmInfo, _pAsmInfo);
                    SAFEDELETE(_pAsmInfo);
                    goto Exit;
                }
            }

            if (_pAsmInfo->_peKind == peNone) {
                // if PA not specified, always put in rear.
                if (!_listAsmInfo.AddTail(_pAsmInfo))
                {
                    SAFEDELETE(_pAsmInfo);
                    hr = E_OUTOFMEMORY;
                    goto Exit;
                }
                _pAsmInfo = NULL;
            }
            else {
                if (_pAsmInfo->_peKind == peMSIL || _pAsmInfo->_peKind == g_peKindProcess) {
                    if (!_listAsmInfo.AddSorted(_pAsmInfo, (LPVOID)CAsmBindingInfo::ComparePE)) {
                        SAFEDELETE(_pAsmInfo);
                        hr = E_OUTOFMEMORY;
                        goto Exit;
                    }
                    _pAsmInfo = NULL;
                }
                else {
                    // has architecture not applicable in this process. Ignore.
                    SAFEDELETE(_pAsmInfo);
                }
            }
        }
        else {
            SAFEDELETE(_pAsmInfo);
        }
    }
    else {
        SAFEDELETE(_pAsmInfo);
    }

Exit:
    return hr;
}

HRESULT CNodeFactory::CreateNode(IXMLNodeSource __RPC_FAR *pSource,
                                 PVOID pNodeParent, USHORT cNumRecs,
                                 XML_NODE_INFO __RPC_FAR **aNodeInfo)
{
    HRESULT                 hr = S_OK;
    WCHAR                   wzElementNameNS[MAX_POLICY_TAG_LENGTH];
    DWORD                   dwSize;
    CCodebaseHint           *pCodebaseInfo = NULL;
    CBindingRedir           *pRedirInfo = NULL;

    MEMORY_REPORT_CONTEXT_SCOPE("FusionXml");

    hr = _nsmgr.OnCreateNode(pSource, pNodeParent, cNumRecs, aNodeInfo);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (aNodeInfo[0]->dwType == XML_ELEMENT) {
        _dwCurDepth++;

        dwSize = ARRAYSIZE(wzElementNameNS);
        hr = ApplyNamespace(aNodeInfo[0], wzElementNameNS, &dwSize, XMLNS_FLAGS_APPLY_DEFAULT_NAMESPACE);
        if (FAILED(hr)) {
            // if more than MAX_POLICY_TAG_LENGTH,
            // not something we are interested in.
            if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
                hr = S_OK;
            }
            goto Exit;
        }

        switch (_dwState) {
            case PSTATE_LOOKUP_CONFIGURATION:
                // Still looking for <configuration> tag

                if ((_dwCurDepth == XML_CONFIGURATION_DEPTH) &&
                    (!FusionCompareString(wzElementNameNS, POLICY_TAG_CONFIGURATION))) {

                    _dwState = PSTATE_CONFIGURATION;
                }

                break;

           case PSTATE_CONFIGURATION:
                // In <configuration> tag,
                // and looking for <runtime> or <assemblyBinding V2> tag

                if ((_dwCurDepth == XML_RUNTIME_DEPTH) &&
                    (!FusionCompareString(wzElementNameNS, POLICY_TAG_RUNTIME))) {

                    _dwState = PSTATE_RUNTIME;
                } else if((_dwCurDepth == XML_ASSEMBLYBINDINGV2_DEPTH) &&
                    (!FusionCompareString(wzElementNameNS, POLICY_TAG_ASSEMBLYBINDINGV2))) {

                    _dwState = PSTATE_ASSEMBLYBINDINGV2;
                }
                else if ((_dwCurDepth == XML_ASSEMBLYBINDING_ROOT_DEPTH) &&
                    (!FusionCompareString(wzElementNameNS, POLICY_TAG_ASSEMBLYBINDING))) {
                    _dwState = PSTATE_ASSEMBLYBINDING_ROOT;

                }
                break;

           case PSTATE_ASSEMBLYBINDING_ROOT:

                if ((_dwCurDepth == XML_LINKED_CONFIGURATION_DEPTH) &&
                    (!FusionCompareString(wzElementNameNS, POLICY_TAG_LINKEDCONFIGURATION))) {
   
                   hr = ProcessLinkedConfigurationTag(aNodeInfo, cNumRecs);
                   if (FAILED(hr)) {
                       goto Exit;
                   }
                }

                break;

            case PSTATE_RUNTIME:
                // In <runtime> tag, and looking for <assemblyBinding> tag
                if ((_dwCurDepth == XML_ASSEMBLYBINDING_DEPTH) &&
                    (!FusionCompareString(wzElementNameNS, POLICY_TAG_ASSEMBLYBINDING))) {
                
                       hr = ProcessAssemblyBindingTag(aNodeInfo, cNumRecs);
                       if (FAILED(hr)) {
                           goto Exit;
                       }
                
                       _dwState = PSTATE_ASSEMBLYBINDING;
                   }
                break;

             case PSTATE_ASSEMBLYBINDING:
                 // In <assemblyBinding> tag.
                 if (_bCorVersionMatch) {
                     if ((_dwCurDepth == XML_PROBING_DEPTH) &&
                         (!FusionCompareString(wzElementNameNS, POLICY_TAG_PROBING))) {

                         hr = ProcessProbingTag(aNodeInfo, cNumRecs);
                         if (FAILED(hr)) {
                             goto Exit;
                         }
                     }
                     else if ((_dwCurDepth == XML_QUALIFYASSEMBLY_DEPTH) &&
                              (!FusionCompareString(wzElementNameNS, POLICY_TAG_QUALIFYASSEMBLY))) {
                         hr = ProcessQualifyAssemblyTag(aNodeInfo, cNumRecs);
                         if (FAILED(hr)) {
                             goto Exit;
                         }
                     }
                     else if ((_dwCurDepth == XML_GLOBAL_PUBLISHERPOLICY_DEPTH) &&
                              (!FusionCompareString(wzElementNameNS, POLICY_TAG_PUBLISHERPOLICY))) {

                         hr = ProcessPublisherPolicyTag(aNodeInfo, cNumRecs, TRUE);
                         if (FAILED(hr)) {
                             goto Exit;
                         }
                     }
                     else if ((_dwCurDepth == XML_DEPENDENTASSEMBLY_DEPTH) &&
                              (!FusionCompareString(wzElementNameNS, POLICY_TAG_DEPENDENTASSEMBLY))) {

                         // Create new assembly info struct

                         _ASSERTE(!_pAsmInfo);
    
                         _pAsmInfo = NEW(CAsmBindingInfo);
                         if (!_pAsmInfo) {
                             hr = E_OUTOFMEMORY;
                             goto Exit;
                         }

                         // Transition state
    
                         _dwState = PSTATE_DEPENDENTASSEMBLY;
                     }
                }
                break;
            
             case PSTATE_ASSEMBLYBINDINGV2:
                 if ((_dwCurDepth == XML_ASSEMBLY_STORE_DEPTH) &&
                         (!FusionCompareString(wzElementNameNS, POLICY_TAG_ASSEMBLYSTORE))) {
                    hr = ProcessAssemblyStoreTag(aNodeInfo, cNumRecs);
                    if(FAILED(hr)) {
                        goto Exit;
                    }
                 }
                 else if ((_dwCurDepth == XML_DEVOVERRIDE_PATH_DEPTH) &&
                          (!FusionCompareString(wzElementNameNS, POLICY_TAG_DEVOVERRIDE_PATH))) {
                     hr = ProcessDevOverrideTag(aNodeInfo, cNumRecs);
                     if(FAILED(hr)) {
                         goto Exit;
                     }
                 }
                break;

             case PSTATE_DEPENDENTASSEMBLY:
                 // In <dependentAssembly> tag.
                if (_bCorVersionMatch) {
                    if ((_dwCurDepth == XML_ASSEMBLYIDENTITY_DEPTH) &&
                         (!FusionCompareString(wzElementNameNS, POLICY_TAG_ASSEMBLYIDENTITY))) {

                        hr = ProcessAssemblyIdentityTag(aNodeInfo, cNumRecs);
                        if (hr == HRESULT_FROM_WIN32(ERROR_TAG_NOT_PRESENT)) {
                            hr = S_OK;
                        }
                        else if (FAILED(hr)) {
                            goto Exit;
                        }
                     }
                     else if ((_dwCurDepth == XML_BINDINGREDIRECT_DEPTH) &&
                              (!FusionCompareString(wzElementNameNS, POLICY_TAG_BINDINGREDIRECT))) {

                        pRedirInfo = NEW(CBindingRedir);
                        if (!pRedirInfo) {
                            hr = E_OUTOFMEMORY;
                            goto Exit;
                        }
                    
                        hr = ProcessBindingRedirectTag(aNodeInfo, cNumRecs, pRedirInfo);
                        if (hr == S_OK) {
                            hr = AddRedirToBindInfo(pRedirInfo, _pAsmInfo);
                            if (hr != S_OK) {
                                SAFEDELETE(pRedirInfo);
                                if (FAILED(hr)) {
                                    goto Exit;
                                }
                            }
                            
                            pRedirInfo = NULL;
                        }
                        else if (hr == HRESULT_FROM_WIN32(ERROR_TAG_NOT_PRESENT)) {
                            SAFEDELETE(pRedirInfo);
                            DEBUGOUT(_pdbglog, 1, ID_FUSLOG_XML_BINDINGREDIRECT_INSUFFICIENT_DATA);
                            hr = S_OK;
                        }
                        else if (FAILED(hr)) {
                            SAFEDELETE(pRedirInfo);
                            goto Exit;
                        }
                    }
                    else if ((_dwCurDepth == XML_CODEBASE_DEPTH) &&
                              (!FusionCompareString(wzElementNameNS, POLICY_TAG_CODEBASE))) {

                         pCodebaseInfo = NEW(CCodebaseHint);
                         if (!pCodebaseInfo) {
                             hr = E_OUTOFMEMORY;
                             goto Exit;
                         }

                         hr = ProcessCodebaseTag(aNodeInfo, cNumRecs, pCodebaseInfo);
                         if (hr == S_OK) {
                             hr = AddCodebaseHintToBindInfo(pCodebaseInfo, _pAsmInfo);
                             if ( hr != S_OK) {
                                 SAFEDELETE(pCodebaseInfo);
                                 if (FAILED(hr)) {
                                    goto Exit;
                                }
                             }
                             pCodebaseInfo = NULL;
                         }
                         else if (hr == HRESULT_FROM_WIN32(ERROR_TAG_NOT_PRESENT)) {
                             DEBUGOUT(_pdbglog, 1, ID_FUSLOG_XML_CODEBASE_HREF_MISSING);
                             SAFEDELETE(pCodebaseInfo);
                             hr = S_OK;
                         }
                         else if (FAILED(hr)) {
                             SAFEDELETE(pCodebaseInfo);
                             goto Exit;
                         }
                     }
                     else if ((_dwCurDepth == XML_PUBLISHERPOLICY_DEPTH) &&
                              (!FusionCompareString(wzElementNameNS, POLICY_TAG_PUBLISHERPOLICY))) {

                         hr = ProcessPublisherPolicyTag(aNodeInfo, cNumRecs, FALSE);
                         if (FAILED(hr)) {
                             goto Exit;
                         }
                     }
                 }
                 break;

           default:
                // Unknown state!

                _ASSERTE(!"Unknown parsing state!");
                
                hr = E_UNEXPECTED;
                break;
        }
    }

Exit:
    return hr;
}

HRESULT CNodeFactory::ProcessAssemblyBindingTag(XML_NODE_INFO **aNodeInfo, USHORT cNumRecs)
{
    HRESULT     hr = S_OK;
    USHORT      idx = 1;
    WCHAR       wzAttributeNS[MAX_POLICY_TAG_LENGTH];
    DWORD       dwSize;
    // list of runtimes this config applies to
    LPWSTR      pwzAppliesList = NULL;
    LPWSTR      pwzBuf = NULL;
    DWORD       cchBuf = 0;
    LPWSTR      pwzVer = NULL;
    DWORD       cchVer = 0;
 
    _ASSERTE(aNodeInfo && cNumRecs);
    _bCorVersionMatch = TRUE;

    while (idx < cNumRecs) 
    {
        if (aNodeInfo[idx]->dwType == XML_ATTRIBUTE) 
        {
            // Found an attribute. Find out which one, and extract the data.
            // Node: ::ExtractXMLAttribute increments idx.

            dwSize = ARRAYSIZE(wzAttributeNS);
            hr = ApplyNamespace(aNodeInfo[idx], wzAttributeNS, &dwSize, 0);
            if (FAILED(hr)) {
                // if more than MAX_POLICY_TAG_LENGTH,
                // not something we are interested in.
                if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
                    hr = S_OK;
                }
                goto Exit;
            }
            else if (hr == S_FALSE) 
            {
                idx++;
                hr = S_OK;
            }
            else if (!FusionCompareString(wzAttributeNS, XML_ATTRIBUTE_APPLIESTO)) 
            {
                _bCorVersionMatch = FALSE;
                
                hr = ::ExtractXMLAttribute(&pwzAppliesList, aNodeInfo, &idx, cNumRecs);
                if (FAILED(hr)) 
                    goto Exit;

                // nothing in appliesTo, treat it like it does not exist.
                if (!pwzAppliesList || !lstrlenW(pwzAppliesList)) {
                    goto Exit;
                }

                hr = GetCORVersion();
                if (FAILED(hr))
                    goto Exit;
               
                pwzBuf = pwzAppliesList;
                cchBuf = lstrlenW(pwzAppliesList)+1;
                
                // Now we have something in the list, 
                // Let us compare. 
                while(CParseUtils::GetDelimitedToken(&pwzBuf, &cchBuf, &pwzVer, &cchVer, L';', FALSE, NULL))
                {
                    *(pwzVer + cchVer) = L'\0';
                    if(!FusionCompareStringI(g_pwzCORVersion, pwzVer))
                    {
                        _bCorVersionMatch = TRUE;
                        goto Exit;
                    }
                }
                goto Exit;
            }
            else 
            {
                idx++;
            }
        }
        else 
        {
            idx++;
        }
    }

Exit:  
    SAFEDELETEARRAY(pwzAppliesList);
    return hr;
}

HRESULT CNodeFactory::ProcessAssemblyStoreTag(XML_NODE_INFO **aNodeInfo, USHORT cNumRecs)
{
    HRESULT     hr = S_OK;
    USHORT      idx = 1;
    WCHAR       pwzAttributeNS[MAX_POLICY_TAG_LENGTH];
    DWORD       dwSize;

    _ASSERTE(aNodeInfo && cNumRecs);

    while (idx < cNumRecs) {
        if (aNodeInfo[idx]->dwType == XML_ATTRIBUTE) {
            // Found an attribute. Find out which one, and extract the data.
            // Node: ::ExtractXMLAttribute increments idx.

            dwSize = ARRAYSIZE(pwzAttributeNS);
            hr = ApplyNamespace(aNodeInfo[idx], pwzAttributeNS, &dwSize, 0);
            if (FAILED(hr)) {
                // if more than MAX_POLICY_TAG_LENGTH,
                // not something we are interested in.
                if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
                    hr = S_OK;
                }
                goto Exit;
            }
            else if (hr == S_FALSE) {
                idx++;
                hr = S_OK;
            }
            else if (!FusionCompareString(pwzAttributeNS, XML_ATTRIBUTE_LOCATION)) {
                if (_pwzAssemblyStore) {
                    DEBUGOUT(_pdbglog, 1, ID_FUSLOG_ASSEMBLYSTORE_DUPLICATE);
                    idx++;
                }
                else {
                    hr = ::ExtractXMLAttribute(&_pwzAssemblyStore, aNodeInfo, &idx, cNumRecs);
                    if(FAILED(hr)) {
                        goto Exit;
                    }

                    if(lstrlenW(_pwzAssemblyStore) > MAX_PATH) {
                        hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
                        goto Exit;
                    }
                }
            }
            else {
                idx++;
            }
        }
        else {
            idx++;
        }
    }

Exit:
    return hr;
}

HRESULT CNodeFactory::ProcessDevOverrideTag(XML_NODE_INFO **aNodeInfo, USHORT cNumRecs)
{
    HRESULT     hr = S_OK;
    USHORT      idx = 1;
    WCHAR       pwzAttributeNS[MAX_POLICY_TAG_LENGTH];
    DWORD       dwSize;

    _ASSERTE(aNodeInfo && cNumRecs);

    while (idx < cNumRecs) {
        if (aNodeInfo[idx]->dwType == XML_ATTRIBUTE) {
            // Found an attribute. Find out which one, and extract the data.
            // Node: ::ExtractXMLAttribute increments idx.

            dwSize = ARRAYSIZE(pwzAttributeNS);
            hr = ApplyNamespace(aNodeInfo[idx], pwzAttributeNS, &dwSize, 0);
            if (FAILED(hr)) {
                // if more than MAX_POLICY_TAG_LENGTH,
                // not something we are interested in.
                if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
                    hr = S_OK;
                }
                goto Exit;
            }
            else if (hr == S_FALSE) {
                idx++;
                hr = S_OK;
            }
            else if (!FusionCompareString(pwzAttributeNS, XML_ATTRIBUTE_LOCATION)) {
                if (_pwzDevOverridePath) {
                    DEBUGOUT(_pdbglog, 1, ID_FUSLOG_DEVOVERRIDE_DUPLICATE);
                    idx++;
                }
                else {
                    hr = ::ExtractXMLAttribute(&_pwzDevOverridePath, aNodeInfo, &idx, cNumRecs);
                    if (FAILED(hr)) {
                        goto Exit;
                    }

                    if(lstrlenW(_pwzDevOverridePath) > MAX_PATH) {
                        hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
                        goto Exit;
                    }
                }
            }
            else {
                idx++;
            }
        }
        else {
            idx++;
        }
    }

Exit:
    return hr;
}

HRESULT CNodeFactory::ProcessProbingTag(XML_NODE_INFO **aNodeInfo, USHORT cNumRecs)
{
    HRESULT     hr = S_OK;
    USHORT      idx = 1;
    WCHAR       pwzAttributeNS[MAX_POLICY_TAG_LENGTH];
    DWORD       dwSize;                                        

    _ASSERTE(aNodeInfo && cNumRecs);

    while (idx < cNumRecs) {
        if (aNodeInfo[idx]->dwType == XML_ATTRIBUTE) {
            // Found an attribute. Find out which one, and extract the data.
            // Node: ::ExtractXMLAttribute increments idx.
            dwSize = ARRAYSIZE(pwzAttributeNS);
            hr = ApplyNamespace(aNodeInfo[idx], pwzAttributeNS, &dwSize, 0);
            if (FAILED(hr)) {
                // if more than MAX_POLICY_TAG_LENGTH,
                // not something we are interested in.
                if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
                    hr = S_OK;
                }
                goto Exit;
            }
            else if (hr == S_FALSE) {
                idx++;
                hr = S_OK;
            }
            else if (!FusionCompareString(pwzAttributeNS, XML_ATTRIBUTE_PRIVATEPATH)) {
                if (_pwzPrivatePath) {
                    DEBUGOUT(_pdbglog, 1, ID_FUSLOG_PRIVATE_PATH_DUPLICATE);
                    idx++;
                }
                else {
                    hr = ::ExtractXMLAttribute(&_pwzPrivatePath, aNodeInfo, &idx, cNumRecs);
                    if (FAILED(hr)) {
                        goto Exit;
                    }
                }
            }
            else {
                idx++;
            }
        }
        else {
            idx++;
        }
    }

Exit:
    return hr;
}

HRESULT CNodeFactory::ProcessLinkedConfigurationTag(XML_NODE_INFO **aNodeInfo, USHORT cNumRecs)
{
    HRESULT     hr = S_OK;
    USHORT      idx = 1;
    WCHAR       pwzAttributeNS[MAX_POLICY_TAG_LENGTH];
    DWORD       dwSize;
    
    _ASSERTE(aNodeInfo && cNumRecs);

    while (idx < cNumRecs) {
        if (aNodeInfo[idx]->dwType == XML_ATTRIBUTE) {
            // Found an attribute. Find out which one, and extract the data.
            // Node: ::ExtractXMLAttribute increments idx.

            dwSize = ARRAYSIZE(pwzAttributeNS);
            hr = ApplyNamespace(aNodeInfo[idx], pwzAttributeNS, &dwSize, 0);
            if (FAILED(hr)) {
                // if more than MAX_POLICY_TAG_LENGTH,
                // not something we are interested in.
                if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
                    hr = S_OK;
                }
                goto Exit;
            }
            else if (hr == S_FALSE) {
                idx++;
                hr = S_OK;
            }
            else if (!FusionCompareString(pwzAttributeNS, XML_ATTRIBUTE_HREF)) {
                LPWSTR                         pwzLink = NULL;

                hr = ::ExtractXMLAttribute(&pwzLink, aNodeInfo, &idx, cNumRecs);
                if (FAILED(hr)) {
                    goto Exit;
                }

                if (!_listLinkedConfiguration.AddTail(pwzLink)) {
                    SAFEDELETEARRAY(pwzLink);
                    hr = E_OUTOFMEMORY;
                    goto Exit;
                }
            }
            else {
                idx++;
            }
        }
        else {
            idx++;
        }
    }

Exit:
    return hr;
}

HRESULT CNodeFactory::ProcessQualifyAssemblyTag(XML_NODE_INFO **aNodeInfo, USHORT cNumRecs)
{
    HRESULT                                            hr = S_OK;
    USHORT                                             idx = 1;
    LPWSTR                                             pwzPartialName = NULL;
    LPWSTR                                             pwzFullName = NULL;
    CQualifyAssembly                                  *pqa = NULL;
    IAssemblyName                                     *pNameFull = NULL;
    IAssemblyName                                     *pNamePartial = NULL;
    IAssemblyName                                     *pNameQualified = NULL;
    LPWSTR                                             wzCanonicalDisplayName=NULL;
    WCHAR       pwzAttributeNS[MAX_POLICY_TAG_LENGTH];
    DWORD       dwSize;   

    _ASSERTE(aNodeInfo && cNumRecs);

    while (idx < cNumRecs) {
        if (aNodeInfo[idx]->dwType == XML_ATTRIBUTE) {
            // Found an attribute. Find out which one, and extract the data.
            // Node: ::ExtractXMLAttribute increments idx.

            dwSize = ARRAYSIZE(pwzAttributeNS);
            hr = ApplyNamespace(aNodeInfo[idx], pwzAttributeNS, &dwSize, 0);
            if (FAILED(hr)) {
                // if more than MAX_POLICY_TAG_LENGTH,
                // not something we are interested in.
                if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
                    hr = S_OK;
                }
                goto Exit;
            }
            else if (hr == S_FALSE) {
                idx++;
                hr = S_OK;
            }
            else if (!FusionCompareString(pwzAttributeNS, XML_ATTRIBUTE_PARTIALNAME)) {
                if (pwzPartialName) {
                    // Ignore duplicate attribute
                    idx++;
                }
                else {
                    hr = ::ExtractXMLAttribute(&pwzPartialName, aNodeInfo, &idx, cNumRecs);
                    if (FAILED(hr)) {
                        goto Exit;
                    }
                }
            }
            else if (!FusionCompareString(pwzAttributeNS, XML_ATTRIBUTE_FULLNAME)) {
                if (pwzFullName) {
                    // Ignore duplicate attribute
                    idx++;
                }
                else {
                    hr = ::ExtractXMLAttribute(&pwzFullName, aNodeInfo, &idx, cNumRecs);
                    if (FAILED(hr)) {
                        goto Exit;
                    }
                }

            }
            else {
                idx++;
            }
        }
        else {
            idx++;
        }
    }

    if (pwzPartialName && pwzFullName) {
        DWORD                               adwProperties[] = { ASM_NAME_NAME,
                                                                ASM_NAME_MAJOR_VERSION,
                                                                ASM_NAME_MINOR_VERSION,
                                                                ASM_NAME_BUILD_NUMBER,
                                                                ASM_NAME_REVISION_NUMBER,
                                                                ASM_NAME_CULTURE,
                                                                ASM_NAME_PUBLIC_KEY_TOKEN ,
                                                                ASM_NAME_RETARGET,
                                                                ASM_NAME_ARCHITECTURE
                                                                };
        DWORD                               adwCmpFlags[] = {   ASM_CMPF_NAME,
                                                                ASM_CMPF_MAJOR_VERSION,
                                                                ASM_CMPF_MINOR_VERSION,
                                                                ASM_CMPF_BUILD_NUMBER,
                                                                ASM_CMPF_REVISION_NUMBER,
                                                                ASM_CMPF_CULTURE,
                                                                ASM_CMPF_PUBLIC_KEY_TOKEN ,
                                                                ASM_CMPF_RETARGET,
                                                                ASM_CMPF_ARCHITECTURE
                                                                };
        DWORD                               dwNumProps = sizeof(adwProperties) / sizeof(adwProperties[0]);

        if (FAILED(hr = CreateAssemblyNameObject(&pNameFull, pwzFullName,
                                            CANOF_PARSE_DISPLAY_NAME, 0))) {
            goto Exit;
        }

        if (FAILED(hr = CreateAssemblyNameObject(&pNamePartial, pwzPartialName,
                                            CANOF_PARSE_DISPLAY_NAME, 0))) {
            goto Exit;
        }

        // Check validity of qualification

        if (CAssemblyName::IsPartial(pNameFull) || !CAssemblyName::IsPartial(pNamePartial)) {
            goto Exit;
        }

        if (FAILED(hr = pNamePartial->Clone(&pNameQualified))) {
            goto Exit;
        }

        for (DWORD i = 0; i < dwNumProps; i++) {
            dwSize = 0;
            if (pNamePartial->GetProperty(adwProperties[i], NULL, &dwSize) != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
                // Partial on this field. Set pNameQualified's corresponding
                // property to whatever is in pNameFull.

                dwSize = 0;
                pNameFull->GetProperty(adwProperties[i], NULL, &dwSize);
                if (!dwSize) {
                    if (adwProperties[i] == ASM_NAME_RETARGET || adwProperties[i] == ASM_NAME_ARCHITECTURE) {
                            continue;
                    }
                    else {
                        _ASSERTE(!"Expect property!");
                        hr = E_UNEXPECTED;
                        goto Exit;
                    }
                }
                else {
                    BYTE                       *pBuf;

                    pBuf = NEW(BYTE[dwSize]);
                    if (!pBuf) {
                        hr = E_OUTOFMEMORY;
                        goto Exit;
                    }

                    if (FAILED(hr = pNameFull->GetProperty(adwProperties[i], pBuf, &dwSize))) {
                        SAFEDELETEARRAY(pBuf);
                        goto Exit;
                    }

                    if (FAILED(hr = pNameQualified->SetProperty(adwProperties[i], pBuf, dwSize))) {
                        SAFEDELETEARRAY(pBuf);
                        goto Exit;
                    }

                    SAFEDELETEARRAY(pBuf);
                }
            }
            else {
                // Full-specified on this field. Make sure it matches the full ref specified.

                if ((hr = pNamePartial->IsEqual(pNameFull, adwCmpFlags[i])) != S_OK) {
                    goto Exit;
                }
            }
        }

        if (CAssemblyName::IsPartial(pNameQualified)) {
            goto Exit;
        }

        // Get canonical display name format

        wzCanonicalDisplayName = NEW(WCHAR[MAX_URL_LENGTH+1]);
        if (!wzCanonicalDisplayName)
        {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        dwSize = MAX_URL_LENGTH;
        if (FAILED(hr = pNamePartial->GetDisplayName(wzCanonicalDisplayName, &dwSize, 0))) {
            goto Exit;
        }

        // Add qualified assembly entry to list

        pqa = NEW(CQualifyAssembly);
        if (!pqa) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        pqa->_pwzPartialName = WSTRDupDynamic(wzCanonicalDisplayName);
        if (!pqa->_pwzPartialName) {
            SAFEDELETE(pqa);
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        pqa->_pNameFull = pNameQualified;
        pNameQualified->AddRef();

        if (!_listQualifyAssembly.AddTail(pqa))
        {
            hr = E_OUTOFMEMORY;
            SAFEDELETE(pqa);
            goto Exit;
        }
    }

Exit:
    SAFEDELETEARRAY(pwzPartialName);
    SAFEDELETEARRAY(pwzFullName);

    SAFERELEASE(pNameFull);
    SAFERELEASE(pNamePartial);
    SAFERELEASE(pNameQualified);

    SAFEDELETEARRAY(wzCanonicalDisplayName);
    return hr;
}


HRESULT CNodeFactory::ProcessBindingRedirectTag(XML_NODE_INFO **aNodeInfo,
                                                USHORT cNumRecs,
                                                CBindingRedir *pRedir)
{
    HRESULT                                            hr = S_OK;
    USHORT                                             idx = 1;
    WCHAR       pwzAttributeNS[MAX_POLICY_TAG_LENGTH];
    DWORD       dwSize;

    _ASSERTE(aNodeInfo && cNumRecs && pRedir);
    _ASSERTE(!pRedir->_pwzVersionOld && !pRedir->_pwzVersionNew);

    while (idx < cNumRecs) {
        if (aNodeInfo[idx]->dwType == XML_ATTRIBUTE) {
            // Found an attribute. Find out which one, and extract the data.
            // Note: ::ExtractXMLAttribute increments idx.

            dwSize = ARRAYSIZE(pwzAttributeNS);
            hr = ApplyNamespace(aNodeInfo[idx], pwzAttributeNS, &dwSize, 0);
            if (FAILED(hr)) {
                // if more than MAX_POLICY_TAG_LENGTH,
                // not something we are interested in.
                if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
                    hr = S_OK;
                }
                goto Exit;
            }
            else if (hr == S_FALSE) {
                idx++;
                hr = S_OK;
            }
            else if (!FusionCompareString(pwzAttributeNS, XML_ATTRIBUTE_OLDVERSION)) {

                hr = ::ExtractXMLAttribute(&(pRedir->_pwzVersionOld), aNodeInfo, &idx, cNumRecs);
                if (FAILED(hr)) {
                    goto Exit;
                }
                
                if (pRedir->_pwzVersionOld)
                    TrimString(pRedir->_pwzVersionOld);
            }
            else if (!FusionCompareString(pwzAttributeNS, XML_ATTRIBUTE_NEWVERSION)) {

                hr = ::ExtractXMLAttribute(&(pRedir->_pwzVersionNew), aNodeInfo, &idx, cNumRecs);
                if (FAILED(hr)) {
                    goto Exit;
                }

                if (pRedir->_pwzVersionNew)
                    TrimString(pRedir->_pwzVersionNew);
            }
            else {
                idx++;
            }
            
        }
        else {
            idx++;
        }
    }

    if (!pRedir->_pwzVersionOld || !pRedir->_pwzVersionNew) {
        // Data was incomplete. These are required fields.

        hr = HRESULT_FROM_WIN32(ERROR_TAG_NOT_PRESENT);
        goto Exit;
    }

Exit:
    return hr;    
}

HRESULT CNodeFactory::ProcessCodebaseTag(XML_NODE_INFO **aNodeInfo,
                                         USHORT cNumRecs,
                                         CCodebaseHint *pCB)
{
    HRESULT                                            hr = S_OK;
    USHORT                                             idx = 1;
    WCHAR       pwzAttributeNS[MAX_POLICY_TAG_LENGTH];
    DWORD       dwSize;

    _ASSERTE(aNodeInfo && cNumRecs && pCB);
    _ASSERTE(!pCB->_pwzVersion && !pCB->_pwzCodebase);

    while (idx < cNumRecs) {
        if (aNodeInfo[idx]->dwType == XML_ATTRIBUTE) {
            // Found an attribute. Find out which one, and extract the data.
            // Note: ::ExtractXMLAttribute increments idx.

            dwSize = ARRAYSIZE(pwzAttributeNS);
            hr = ApplyNamespace(aNodeInfo[idx], pwzAttributeNS, &dwSize, 0);
            if (FAILED(hr)) {
                // if more than MAX_POLICY_TAG_LENGTH,
                // not something we are interested in.
                if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
                    hr = S_OK;
                }
                goto Exit;
            }
            else if (hr == S_FALSE) {
                idx++;
                hr = S_OK;
            }
            else if (!FusionCompareString(pwzAttributeNS, XML_ATTRIBUTE_VERSION)) {

                hr = ::ExtractXMLAttribute(&(pCB->_pwzVersion), aNodeInfo, &idx, cNumRecs);
                if (FAILED(hr)) {
                    goto Exit;
                }
            }
            else if (!FusionCompareString(pwzAttributeNS, XML_ATTRIBUTE_HREF)) {

                hr = ::ExtractXMLAttribute(&(pCB->_pwzCodebase), aNodeInfo, &idx, cNumRecs);
                if (FAILED(hr)) {
                    goto Exit;
                }
            }
            else {
                idx++;
            }
        }
        else {
            idx++;
        }
    }

    if (!pCB->_pwzCodebase) {
        // Data was incomplete. 

        hr = HRESULT_FROM_WIN32(ERROR_TAG_NOT_PRESENT);
        goto Exit;
    }

Exit:
    return hr;    
}

HRESULT CNodeFactory::ProcessPublisherPolicyTag(XML_NODE_INFO **aNodeInfo,
                                                USHORT cNumRecs,
                                                BOOL bGlobal)
{
    HRESULT                                            hr = S_OK;
    LPWSTR                                             pwzTmp = NULL;
    USHORT                                             idx = 1;
    WCHAR       pwzAttributeNS[MAX_POLICY_TAG_LENGTH];
    DWORD       dwSize;

    _ASSERTE(aNodeInfo && cNumRecs);

    while (idx < cNumRecs) {
        if (aNodeInfo[idx]->dwType == XML_ATTRIBUTE) {
            // Found an attribute. Find out which one, and extract the data.
            // Node: ::ExtractXMLAttribute increments idx.

            dwSize = ARRAYSIZE(pwzAttributeNS);
            hr = ApplyNamespace(aNodeInfo[idx], pwzAttributeNS, &dwSize, 0);
            if (FAILED(hr)) {
                // if more than MAX_POLICY_TAG_LENGTH,
                // not something we are interested in.
                if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
                    hr = S_OK;
                }
                goto Exit;
            }
            else if (hr == S_FALSE) {
                idx++;
                hr = S_OK;
            }
            else if (!FusionCompareString(pwzAttributeNS, XML_ATTRIBUTE_APPLY)) {
                hr = ::ExtractXMLAttribute(&pwzTmp, aNodeInfo, &idx, cNumRecs);
                if (FAILED(hr)) {
                    goto Exit;
                }

                if (pwzTmp && !FusionCompareString(pwzTmp, L"no")) {
                    if (bGlobal) {
                        _bGlobalSafeMode = TRUE;
                    }
                    else {
                        _pAsmInfo->_bApplyPublisherPolicy = FALSE;
                    }
                }

                SAFEDELETEARRAY(pwzTmp);
            }
            else {
                idx++;
            }
        }
        else {
            idx++;
        }
    }

Exit:
    return hr;
}

HRESULT CNodeFactory::ProcessAssemblyIdentityTag(XML_NODE_INFO **aNodeInfo,
                                                 USHORT cNumRecs)
{
    HRESULT     hr = S_OK;
    USHORT      idx = 1;
    WCHAR       pwzAttributeNS[MAX_POLICY_TAG_LENGTH];
    DWORD       dwSize;

    _ASSERTE(aNodeInfo && cNumRecs && _pAsmInfo);

    while (idx < cNumRecs) {
        if (aNodeInfo[idx]->dwType == XML_ATTRIBUTE) {
            // Found an attribute. Find out which one, and extract the data.
            // Note: ::ExtractXMLAttribute increments idx.

            dwSize = ARRAYSIZE(pwzAttributeNS);
            hr = ApplyNamespace(aNodeInfo[idx], pwzAttributeNS, &dwSize, 0);
            if (FAILED(hr)) {
                // if more than MAX_POLICY_TAG_LENGTH,
                // not something we are interested in.
                if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
                    hr = S_OK;
                }
                goto Exit;
            }
            else if (hr == S_FALSE) {
                idx++;
                hr = S_OK;
            }
            else if (!FusionCompareString(pwzAttributeNS, XML_ATTRIBUTE_NAME)) {

                if (!_pAsmInfo->_pwzName) {
                    hr = ::ExtractXMLAttribute(&(_pAsmInfo->_pwzName), aNodeInfo, &idx, cNumRecs);
                    if (FAILED(hr)) {
                        goto Exit;
                    }
                }
                else {
                    DEBUGOUT(_pdbglog, 1, ID_FUSLOG_XML_MULTIPLE_IDENTITIES);
                    hr = E_FAIL;
                    goto Exit;
                }
            }
            else if (!FusionCompareString(pwzAttributeNS, XML_ATTRIBUTE_PUBLICKEYTOKEN)) {
                if (!_pAsmInfo->_pwzPublicKeyToken) {
                    hr = ::ExtractXMLAttribute(&(_pAsmInfo->_pwzPublicKeyToken), aNodeInfo, &idx, cNumRecs);
                    if (FAILED(hr)) {
                        goto Exit;
                    }

                    if (_pAsmInfo->_pwzPublicKeyToken && !FusionCompareStringI(_pAsmInfo->_pwzPublicKeyToken, L"null")) {
                        SAFEDELETEARRAY(_pAsmInfo->_pwzPublicKeyToken);
                    }
                }
                else {
                    DEBUGOUT(_pdbglog, 1, ID_FUSLOG_XML_MULTIPLE_IDENTITIES);
                    hr = E_FAIL;
                    goto Exit;
                }

            }
            else if (!FusionCompareString(pwzAttributeNS, XML_ATTRIBUTE_CULTURE)) {
                if (!_pAsmInfo->_pwzCulture) {
                    hr = ::ExtractXMLAttribute(&(_pAsmInfo->_pwzCulture), aNodeInfo, &idx, cNumRecs);
                    if (FAILED(hr)) {
                        goto Exit;
                    }

                    // Change all synonyms for culture=neutral/"" to unset

                    if (_pAsmInfo->_pwzCulture && (!lstrlenW(_pAsmInfo->_pwzCulture) || !FusionCompareStringI(_pAsmInfo->_pwzCulture, CFG_CULTURE_NEUTRAL))) {
                        SAFEDELETEARRAY(_pAsmInfo->_pwzCulture);
                    }
                }
                else {
                    DEBUGOUT(_pdbglog, 1, ID_FUSLOG_XML_MULTIPLE_IDENTITIES);
                    hr = E_FAIL;
                    goto Exit;
                }
            }
            else if (!FusionCompareString(pwzAttributeNS, XML_ATTRIBUTE_PROCESSORARCHITECTURE)) {
                if (_pAsmInfo->_peKind == peNone) {
                    LPWSTR pwzProc = NULL;
                    hr = ::ExtractXMLAttribute(&pwzProc, aNodeInfo, &idx, cNumRecs);
                    if (FAILED(hr)) {
                        goto Exit;
                    }
                    hr = MapProcessorArchitectureToPEKIND(pwzProc, &(_pAsmInfo->_peKind));
                    SAFEDELETEARRAY(pwzProc);
                    if (FAILED(hr)) {
                        DEBUGOUT(_pdbglog, 1, ID_FUSLOG_XML_INVALID_PROCESSORARCHITECTURE);
                        // set peKind to peInvalid to indicate a parse error.
                        _pAsmInfo->_peKind = peInvalid;
                        hr = S_OK;
                    }
                }
                else {
                    DEBUGOUT(_pdbglog, 1, ID_FUSLOG_XML_MULTIPLE_IDENTITIES);
                    hr = E_FAIL;
                    goto Exit;
                }
            }
            else {
                idx++;
            }
        }
        else {
            idx++;
        }
    }

    if (!_pAsmInfo->_pwzName) {
        // Data was incomplete. 
        hr = HRESULT_FROM_WIN32(ERROR_TAG_NOT_PRESENT);
        DEBUGOUT(_pdbglog, 1, ID_FUSLOG_XML_ASSEMBLYIDENTITY_MISSING_NAME);
        goto Exit;
    }

Exit:
    return hr;    
}

HRESULT CNodeFactory::GetPrivatePath(__deref_out LPWSTR *ppwzPrivatePath)
{
    HRESULT                                     hr = S_OK;

    if (!ppwzPrivatePath) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *ppwzPrivatePath = NULL;

    if (!_pwzPrivatePath) {
        hr = S_FALSE;
        goto Exit;
    }
    
    *ppwzPrivatePath = WSTRDupDynamic(_pwzPrivatePath);
    if (!*ppwzPrivatePath) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

Exit:
    return hr;
}

HRESULT CNodeFactory::GetAssemblyStorePath(__deref_out LPWSTR *ppwzAssemblyStore)
{
    HRESULT     hr = S_OK;

    if (!ppwzAssemblyStore) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *ppwzAssemblyStore = NULL;

    if (!_pwzAssemblyStore) {
        hr = S_FALSE;
        goto Exit;
    }
    
    *ppwzAssemblyStore = WSTRDupDynamic(_pwzAssemblyStore);
    if (!*ppwzAssemblyStore) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

Exit:
    return hr;
}

HRESULT CNodeFactory::GetDevOverridePath(__deref_out LPWSTR *ppwzDevOverridePath)
{
    HRESULT     hr = S_OK;

    if (!ppwzDevOverridePath) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *ppwzDevOverridePath = NULL;

    if (!_pwzDevOverridePath) {
        hr = S_FALSE;
        goto Exit;
    }
    
    *ppwzDevOverridePath = WSTRDupDynamic(_pwzDevOverridePath);
    if (!*ppwzDevOverridePath) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

Exit:
    return hr;
}

HRESULT CNodeFactory::GetPolicyVersion(LPCWSTR wzAssemblyName,
                                       LPCWSTR wzPublicKeyToken,
                                       LPCWSTR wzCulture,
                                       LPCWSTR wzVersionIn,
                                       PEKIND  peIn,
                                       __out_ecount(*pdwSizeVer) LPWSTR  pwzVersionOut,
                                       __inout LPDWORD pdwSizeVer,
                                       PEKIND *peOut)
{
    HRESULT                 hr = S_OK;
    LISTNODE                pos = NULL;
    LISTNODE                posVer = NULL;
    LPCWSTR                 pwzCultureFormatted = NULL;
    CBindingRedir           *pRedir = NULL;
    CAsmBindingInfo         *pAsmInfo = NULL;
    LPCWSTR                 pVerMatched = NULL;
    DWORD                   dwSize = 0;

    _ASSERTE(wzAssemblyName);
    _ASSERTE(wzPublicKeyToken);
    _ASSERTE(wzVersionIn);
    _ASSERTE(pwzVersionOut);
    _ASSERTE(pdwSizeVer);

    if (wzCulture && (!FusionCompareStringI(wzCulture, CFG_CULTURE_NEUTRAL) || !wzCulture[0])) {
        pwzCultureFormatted = NULL;
    }
    else {
        pwzCultureFormatted = wzCulture;
    }

    pos = _listAsmInfo.GetHeadPosition();
    while (pos && !pVerMatched) {
        pAsmInfo = _listAsmInfo.GetNext(pos);
        _ASSERTE(pAsmInfo);

        if (IsMatch(pAsmInfo, wzAssemblyName, pwzCultureFormatted, wzPublicKeyToken, peIn)) {
            // Look for matching version.

            posVer = (pAsmInfo->_listBindingRedir).GetHeadPosition();
            while (posVer) {
                pRedir = (pAsmInfo->_listBindingRedir).GetNext(posVer);

                hr = IsMatchingVersion(pRedir->_pwzVersionOld, wzVersionIn);
                if (FAILED(hr)) {
                    goto Exit;
                }
                if (hr == S_OK) {
                    // Match found
                    pVerMatched = pRedir->_pwzVersionNew;
                    if (peIn != peInvalid) {
                        *peOut = peIn;
                    }
                    else {
                        if (pAsmInfo->_peKind == peNone) {
                            *peOut = peInvalid;
                        }
                        else {
                            *peOut = pAsmInfo->_peKind;
                        }
                    }
                    break;
                }
            }

            // We could break out of the loop here, but this prevents
            // multiple matches (ie. XML had many identical <dependentAssembly>
            // tags...
        }
    }

    // If we got here, we didn't find a match. Input Version == Output Version
    if (!pVerMatched) {
        pVerMatched = wzVersionIn;
        *peOut = peIn;
    }
    else {
        hr = S_OK;
    }
  
    dwSize = lstrlenW(pVerMatched) + 1;
    
    if (*pdwSizeVer < dwSize) {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }
    else {
        hr = StringCchCopy(pwzVersionOut, *pdwSizeVer, pVerMatched);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (pVerMatched == wzVersionIn) {
            hr = S_FALSE;
        }
        else {
            hr = S_OK;
        }
    }

    *pdwSizeVer = dwSize;

Exit:
    return hr;
}

HRESULT CNodeFactory::GetSafeMode(LPCWSTR wzAssemblyName, LPCWSTR wzPublicKeyToken,
                                  LPCWSTR wzCulture, LPCWSTR wzVersionIn, PEKIND pe,
                                  BOOL *pbSafeMode)
{
    HRESULT                                hr = S_OK;
    LISTNODE                               pos = NULL;
    LPCWSTR                                pwzCultureFormatted = NULL;
    CAsmBindingInfo                       *pAsmInfo = NULL;

    if (!wzAssemblyName || !wzPublicKeyToken || !wzVersionIn || !pbSafeMode) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if (wzCulture && (!FusionCompareStringI(wzCulture, CFG_CULTURE_NEUTRAL) || !lstrlenW(wzCulture))) {
        pwzCultureFormatted = NULL;
    }
    else {
        pwzCultureFormatted = wzCulture;
    }
    
    if (_bGlobalSafeMode) {
        // Global safe mode is set

        *pbSafeMode = TRUE;
        goto Exit;
    }

    *pbSafeMode = FALSE;

    // Look for per-assembly safemode. If safe mode is set in any matching
    // section, then safe mode is enabled.

    pos = _listAsmInfo.GetHeadPosition();
    while (pos) {
        pAsmInfo = _listAsmInfo.GetNext(pos);
        _ASSERTE(pAsmInfo);

        if (IsMatch(pAsmInfo, wzAssemblyName, pwzCultureFormatted, wzPublicKeyToken, pe)) {
            if (!*pbSafeMode) {
                *pbSafeMode = (pAsmInfo->_bApplyPublisherPolicy == FALSE);
            }
        }
    }

Exit:
    return hr;
}

HRESULT CNodeFactory::GetCodebaseHint(LPCWSTR pwzAsmName, 
                                      LPCWSTR pwzVersion,
                                      LPCWSTR pwzPublicKeyToken, 
                                      LPCWSTR pwzCulture, 
                                      PEKIND pe,
                                      LPCWSTR pwzAppBase, 
                                      __deref_out LPWSTR *ppwzCodebase)
{
    HRESULT                                   hr = S_OK;
    LISTNODE                                  pos = NULL;
    LISTNODE                                  posCB = NULL;
    LPCWSTR                                   pwzCultureFormatted;
    LPWSTR                                    wzCombined=NULL;
    LPWSTR                                    pwzAppBaseCombine=NULL;
    DWORD                                     dwSize;
    DWORD                                     dwLen;
    CAsmBindingInfo                          *pAsmInfo = NULL;
    CCodebaseHint                            *pCodebase = NULL;

    if (!pwzAsmName || (pwzPublicKeyToken && !pwzVersion) || !ppwzCodebase) {
        hr = E_INVALIDARG;
        goto Exit;
    }
    
    if (pwzCulture && (!FusionCompareStringI(pwzCulture, CFG_CULTURE_NEUTRAL) || !lstrlenW(pwzCulture))) {
        pwzCultureFormatted = NULL;
    }
    else {
        pwzCultureFormatted = pwzCulture;
    }

    *ppwzCodebase = NULL;

    wzCombined = NEW(WCHAR[MAX_URL_LENGTH+1]);
    if (!wzCombined)
    {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    if (pwzAppBase) {
        _ASSERTE(pwzAppBase[0]);

        dwLen = lstrlenW(pwzAppBase); 

        pwzAppBaseCombine = NEW(WCHAR[dwLen + 2]); // Allocate room for potential trailing '/'
        if (!pwzAppBaseCombine) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        hr = StringCchPrintf(pwzAppBaseCombine, dwLen + 2, L"%ws%ws", pwzAppBase,
                   (pwzAppBase[dwLen - 1] == L'\\' || pwzAppBase[dwLen - 1] == L'/') ? (L"") : (L"/"));
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    pos = _listAsmInfo.GetHeadPosition();
    while (pos) {
        pAsmInfo = _listAsmInfo.GetNext(pos);
        _ASSERTE(pAsmInfo);


        if (IsMatch(pAsmInfo, pwzAsmName, pwzCultureFormatted, pwzPublicKeyToken, pe)) {
            posCB = (pAsmInfo->_listCodebase).GetHeadPosition();
            while (posCB) {
                pCodebase = (pAsmInfo->_listCodebase).GetNext(posCB);
                _ASSERTE(pCodebase);

                if (!pwzPublicKeyToken) {
                    // No PublicKeyToken, take the first codebase (version ignored).

                    dwSize = MAX_URL_LENGTH;
                    hr = UrlCombineUnescape(pwzAppBaseCombine, pCodebase->_pwzCodebase,
                                            wzCombined, &dwSize, 0);
                    if (FAILED(hr)) {
                        goto Exit;
                    }

                    *ppwzCodebase = WSTRDupDynamic(wzCombined);
                    if (!*ppwzCodebase) {
                        hr = E_OUTOFMEMORY;
                        goto Exit;
                    }

                    goto Exit;
                }
                else {
                    // Match version.

                    if (pCodebase->_pwzVersion && !FusionCompareStringI(pwzVersion, pCodebase->_pwzVersion)) {
                        // Match found.

                        dwSize = MAX_URL_LENGTH;
                        if (pwzAppBaseCombine) {
                            hr = UrlCombineUnescape(pwzAppBaseCombine, pCodebase->_pwzCodebase,
                                                    wzCombined, &dwSize, 0);
                        }
                        else {
                            hr = UrlCanonicalizeUnescape(pCodebase->_pwzCodebase, wzCombined, &dwSize, 0);
                        }
                        if (FAILED(hr)) {
                            goto Exit;
                        }                                                

                        *ppwzCodebase = WSTRDupDynamic(wzCombined);
                        if (!*ppwzCodebase) {
                            hr = E_OUTOFMEMORY;
                            goto Exit;
                        }

                        goto Exit;
                    }
                }
            }
        }
    }

    // Did not find codebase hint.

    hr = S_FALSE;

Exit:
    SAFEDELETEARRAY(wzCombined);
    SAFEDELETEARRAY(pwzAppBaseCombine);

    return hr;
}

HRESULT CNodeFactory::ApplyNamespace(XML_NODE_INFO *pNodeInfo, 
                                     __out_ecount_opt(*pdwSize) LPWSTR pwzTokenNS,
                                     __inout LPDWORD pdwSize, 
                                     DWORD dwFlags) 
{
    HRESULT                                  hr = S_OK;

    _ASSERTE(pNodeInfo && pwzTokenNS && pdwSize);

    MEMORY_REPORT_CONTEXT_SCOPE("FusionXml");

    if (!FusionCompareStringN(pNodeInfo->pwcText, XML_NAMESPACE_TAG, XML_NAMESPACE_TAG_LEN)) {
        hr = S_FALSE;
        goto Exit;
    }

    hr = _nsmgr.Map(pNodeInfo->pwcText, pNodeInfo->ulLen, pwzTokenNS, pdwSize, dwFlags);
    if (FAILED(hr)) {
        goto Exit;
    }

Exit:
    return hr;
}

HRESULT CNodeFactory::ProcessLinkedConfiguration(LPCWSTR pwzRootConfigPath)
{
    HRESULT                                  hr = S_OK;
    LPWSTR                                   pwzCanonicalUrl = NULL;
    DWORD                                    dwLen = MAX_URL_LENGTH;
    WCHAR                                    wzPath[MAX_PATH + 1];
    BOOL                                     bProcessed;
    LISTNODE                                 pos;
    LPWSTR                                   pwzLinkedConfiguration;

    pos = _listLinkedConfiguration.GetHeadPosition();
    if (!pos) {
        goto Exit;
    }

    pwzCanonicalUrl = NEW(WCHAR[dwLen]);
    if (!pwzCanonicalUrl) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }
    
    hr = UrlCanonicalizeUnescape(pwzRootConfigPath, pwzCanonicalUrl, &dwLen, 0);
    if (FAILED(hr)) {
        goto Exit;
    }
    
    if (!_listProcessedCfgs.AddTail(pwzCanonicalUrl)) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    pwzCanonicalUrl = NULL;
    
    while (pos) {
        pwzLinkedConfiguration = _listLinkedConfiguration.GetAt(pos);
        _ASSERTE(pwzLinkedConfiguration);

        dwLen = MAX_URL_LENGTH;

        pwzCanonicalUrl = NEW(WCHAR[dwLen]);
        if (!pwzCanonicalUrl) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
    
        hr = UrlCanonicalizeUnescape(pwzLinkedConfiguration, pwzCanonicalUrl, &dwLen, 0);
        if (FAILED(hr)) {
            goto Exit;
        }
        
        if (!UrlIsW(pwzCanonicalUrl, URLIS_FILEURL)) {
            // Only support file:/// linked configuration (no UNC or http)
    
            DEBUGOUT1(_pdbglog, 0, ID_FUSLOG_UNSUPPORTED_LINKED_CONFIGURATION, pwzCanonicalUrl);
            hr = HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
            goto Exit;
        }

        DEBUGOUT1(_pdbglog, 0, ID_FUSLOG_PROCESSING_LINKED_CONFIGURATION, pwzCanonicalUrl);

        hr = CheckProcessedConfigurations(pwzCanonicalUrl, &bProcessed);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (bProcessed) {
            // We've seen this link before. Skip it.

            DEBUGOUT1(_pdbglog, 0, ID_FUSLOG_LINKED_CONFIGURATION_DUPLICATE, pwzCanonicalUrl);

            SAFEDELETEARRAY(pwzCanonicalUrl);
            _listLinkedConfiguration.GetNext(pos);
            continue;
        }
    
        dwLen = MAX_PATH;
        hr = PathCreateFromUrlWrap(pwzCanonicalUrl, wzPath, &dwLen, 0);
        if (FAILED(hr)) {
            goto Exit;
        }
        
        if (!_listProcessedCfgs.AddTail(pwzCanonicalUrl)) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
            
        pwzCanonicalUrl = NULL;

        hr = ParseXML(this, wzPath, _pdbglog);
        if (FAILED(hr)) {
            goto Exit;
        }

        _listLinkedConfiguration.GetNext(pos);
    }
    
Exit:
    if (FAILED(hr)) {
        DEBUGOUT1(_pdbglog, 0, ID_FUSLOG_LINKED_CONFIGURATION_PARSE_ERROR, hr);
        SAFEDELETEARRAY(pwzCanonicalUrl);
    }

    return hr;
}

HRESULT CNodeFactory::CheckProcessedConfigurations(LPCWSTR pwzCanonicalUrl,
                                                   BOOL *pbProcessed)
{
    HRESULT                                  hr = S_OK;
    LISTNODE                                 pos = NULL;
    LPCWSTR                                  pwzCur = NULL;

    _ASSERTE(pwzCanonicalUrl && pbProcessed);

    *pbProcessed = FALSE;

    pos = _listProcessedCfgs.GetHeadPosition();
    while (pos) {
        pwzCur = _listProcessedCfgs.GetNext(pos);
        if (!FusionCompareStringI(pwzCur, pwzCanonicalUrl)) {
            *pbProcessed = TRUE;
            break;
        }
    }

    return hr;
}

HRESULT CNodeFactory::HasFrameworkRedirect(BOOL *pbHasFXRedirect)
{
    HRESULT hr = S_OK;
    LISTNODE pos;
    CAsmBindingInfo *pBind = NULL;
    
    *pbHasFXRedirect = FALSE;
    
    pos = _listAsmInfo.GetHeadPosition();
    while(pos && !*pbHasFXRedirect) {
        pBind = _listAsmInfo.GetNext(pos);

        hr = IsFrameworkAssembly(pBind->_pwzName, L"0.0.0.0", pBind->_pwzCulture, pBind->_pwzPublicKeyToken, pbHasFXRedirect);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

Exit:
    return hr;
}

HRESULT CNodeFactory::QualifyAssembly(LPCWSTR pwzDisplayName, IAssemblyName **ppNameQualified, CDebugLog *pdbglog)
{
    HRESULT                                  hr = S_OK;
    DWORD                                    dwSize;
    CQualifyAssembly                        *pqa;
    LISTNODE                                 pos;
    LPWSTR                                   wzDispName=NULL;

    pos = _listQualifyAssembly.GetHeadPosition();
    while (pos) {
        pqa = _listQualifyAssembly.GetNext(pos);
        if (!FusionCompareString(pwzDisplayName, pqa->_pwzPartialName)) {
            // Found match

            *ppNameQualified = pqa->_pNameFull;
            (*ppNameQualified)->AddRef();

            if (pdbglog && IsLoggingNeeded()) {
                wzDispName = NEW(WCHAR[MAX_URL_LENGTH+1]);
                if (!wzDispName)
                {
                    hr = E_OUTOFMEMORY;
                    goto Exit;
                }

                dwSize = MAX_URL_LENGTH;
                if ((*ppNameQualified)->GetDisplayName(wzDispName, &dwSize, 0) == S_OK) {
                    DEBUGOUT1(pdbglog, 0, ID_FUSLOG_QUALIFIED_ASSEMBLY, wzDispName);
                }
            }

            goto Exit;
        }
    }

    // No match found

    hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);

Exit:
    SAFEDELETEARRAY(wzDispName);
    return hr;
}

HRESULT CNodeFactory::RemovePolicy(LPCWSTR wzAsmName,
                             LPCWSTR wzVersion,
                             LPCWSTR wzCulture,
                             LPCWSTR wzPublicKeyToken,
                             PEKIND  pe)
{
    LISTNODE oldpos;
    LISTNODE pos;
    CAsmBindingInfo *pAsmInfo= NULL;
    LISTNODE oldposRedir;
    LISTNODE posRedir;
    CBindingRedir   *pRedir = NULL;

    pos = _listAsmInfo.GetHeadPosition();
    while(pos) {
        oldpos = pos;
        pAsmInfo = _listAsmInfo.GetNext(pos);
        if (IsMatch(pAsmInfo, wzAsmName, wzCulture, wzPublicKeyToken, pe)) {
            posRedir = pAsmInfo->_listBindingRedir.GetHeadPosition();
            while(posRedir) {
                oldposRedir = posRedir;
                pRedir = pAsmInfo->_listBindingRedir.GetNext(posRedir);
                if (!FusionCompareStringI(wzVersion, pRedir->_pwzVersionNew)) {
                    pAsmInfo->_listBindingRedir.RemoveAt(oldposRedir);
                    SAFEDELETE(pRedir);
                }
            }
            if (pAsmInfo->_listBindingRedir.GetCount() == 0) {
                _listAsmInfo.RemoveAt(oldpos);
                SAFEDELETE(pAsmInfo);
            }
        }
    }

    return S_OK;
}

HRESULT CNodeFactory::AddPolicy(LPCWSTR wzAsmName,
                             LPCWSTR wzOldVersion,
                             LPCWSTR wzCulture,
                             LPCWSTR wzPublicKeyToken,
                             PEKIND  pe,
                             LPCWSTR wzNewVersion,
                             DWORD   dwModifyPolicyFlags)
{

    HRESULT hr = S_OK;
    CBindingRedir *pRedirInfo = NULL;
    LISTNODE pos;
    CAsmBindingInfo *pAsmInfo = NULL;
    CAsmBindingInfo *pAsmInfoExist = NULL;
    
    // construct CAsmBindingInfo. 
    pAsmInfo = NEW(CAsmBindingInfo);
    if (!pAsmInfo) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    pAsmInfo->_pwzName = WSTRDupDynamic(wzAsmName);
    if (!pAsmInfo->_pwzName) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    if (wzPublicKeyToken && wzPublicKeyToken[0]) {
        pAsmInfo->_pwzPublicKeyToken = WSTRDupDynamic(wzPublicKeyToken);
        if (!pAsmInfo->_pwzPublicKeyToken) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
    }

    if (wzCulture && wzCulture[0] && FusionCompareStringI(wzCulture, CFG_CULTURE_NEUTRAL)) {
        pAsmInfo->_pwzCulture = WSTRDupDynamic(wzCulture);
        if (!pAsmInfo->_pwzCulture) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
    }

    pAsmInfo->_peKind = pe;

    // construct CBinginRedir
    pRedirInfo = NEW(CBindingRedir);
    if (!pRedirInfo) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    pRedirInfo->_pwzVersionOld = WSTRDupDynamic(wzOldVersion);
    if (!pRedirInfo->_pwzVersionOld) {
        SAFEDELETE(pRedirInfo);
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    pRedirInfo->_pwzVersionNew = WSTRDupDynamic(wzNewVersion);
    if (!pRedirInfo->_pwzVersionNew) {
        SAFEDELETE(pRedirInfo);
        hr = E_OUTOFMEMORY;
        goto Exit;
    }   
    
    if (!(pAsmInfo->_listBindingRedir).AddTail(pRedirInfo)) {
        SAFEDELETE(pRedirInfo);
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    pRedirInfo = NULL;

    // check duplicate 
    pos = _listAsmInfo.GetHeadPosition();
    while(pos) {
        pAsmInfoExist = _listAsmInfo.GetNext(pos);
        if (IsDuplicate(pAsmInfoExist, pAsmInfo)) {
            // no-op
            goto Exit;
        }
    }

    // add the CAsmBindInfo to node factory. 
    if (!_listAsmInfo.AddHead(pAsmInfo)) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    pAsmInfo = NULL;

    if (dwModifyPolicyFlags == HOST_BINDING_POLICY_MODIFY_CHAIN) {
        pos = _listAsmInfo.GetHeadPosition();

        while(pos) {
            pAsmInfoExist = _listAsmInfo.GetNext(pos);
            if (IsMatch(pAsmInfoExist, wzAsmName, (wzCulture && wzCulture[0])?wzCulture:NULL, wzPublicKeyToken, pe)) {
                LISTNODE pos2 = (pAsmInfoExist->_listBindingRedir).GetHeadPosition();
                while (pos2) {
                    pRedirInfo = (pAsmInfoExist->_listBindingRedir).GetNext(pos2);
                    if (!FusionCompareStringI(pRedirInfo->_pwzVersionNew, wzOldVersion)) {
                        LPWSTR pwzTmp = WSTRDupDynamic(wzNewVersion);
                        if (!pwzTmp) {
                            hr = E_OUTOFMEMORY;
                            goto Exit;
                        }
                        SAFEDELETEARRAY(pRedirInfo->_pwzVersionNew);
                        pRedirInfo->_pwzVersionNew = pwzTmp;
                    }
                }
            }
        }
    }


Exit:
    SAFEDELETE(pAsmInfo);
    return hr;
}

#define POLICY_TAG_CONFIGURATION_START  L"<configuration>"
#define POLICY_TAG_CONFIGURATION_END    L"</configuration>"
#define POLICY_TAG_RUNTIME_START        L"<runtime>"
#define POLICY_TAG_RUNTIME_END          L"</runtime>"
#define POLICY_TAG_ASSEMBLYBINDING_START L"<assemblyBinding xmlns=\"urn:schemas-microsoft-com:asm.v1\">"
#define POLICY_TAG_ASSEMBLYBINDING_END  L"</assemblyBinding>"
#define POLICY_TAG_DEPENDENTASSEMBLY_START L"<dependentAssembly>"
#define POLICY_TAG_DEPENDENTASSEMBLY_END   L"</dependentAssembly>"


HRESULT CNodeFactory::OutputToUTF8(DWORD *pcbBufferSize,
                                   BYTE *pbBuffer)
{
    HRESULT hr = S_OK;
    LPWSTR  pwzSerialized = NULL;
    DWORD   dwTotalSize = 0;
    DWORD   dwSize = 0;
    LPWSTR  pwzTmp = NULL;

    LISTNODE pos;
    CAsmBindingInfo *pAsmBindInfo = NULL;

    pos = _listAsmInfo.GetHeadPosition();
    while(pos) {
        pAsmBindInfo = _listAsmInfo.GetNext(pos);
        dwSize = 0;
        hr = GetSerializeRequiredSize(pAsmBindInfo, &dwSize);
        if (FAILED(hr)) {
            goto Exit;
        }
        if (dwSize) {
            dwTotalSize += dwSize - 1; // remove the null terminator. 
        }
    }
    
    dwTotalSize++; // add the null terminator
    
    dwTotalSize += lstrlenW(POLICY_TAG_CONFIGURATION_START) 
                +  lstrlenW(POLICY_TAG_CONFIGURATION_END) // <configuration> and end tag.
                +  lstrlenW(POLICY_TAG_RUNTIME_START)
                +  lstrlenW(POLICY_TAG_RUNTIME_END) // <runtime> and end tag.
                +  lstrlenW(POLICY_TAG_ASSEMBLYBINDING_START)
                +  lstrlenW(POLICY_TAG_ASSEMBLYBINDING_END); // <assemblyBidning> and end tag

    pwzSerialized = NEW(WCHAR[dwTotalSize]);
    if (!pwzSerialized) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    pwzSerialized[0] = L'\0';
    
    hr = StringCchCopy(pwzSerialized, dwTotalSize, POLICY_TAG_CONFIGURATION_START);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = StringCchCat(pwzSerialized, dwTotalSize, POLICY_TAG_RUNTIME_START);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = StringCchCat(pwzSerialized, dwTotalSize, POLICY_TAG_ASSEMBLYBINDING_START);
    if (FAILED(hr)) {
        goto Exit;
    }

    pos = _listAsmInfo.GetHeadPosition();
    while(pos) {
        pAsmBindInfo = _listAsmInfo.GetNext(pos);

        dwSize = lstrlenW(pwzSerialized);
        pwzTmp = pwzSerialized + dwSize;
        dwSize = dwTotalSize - dwSize;

        hr = Serialize(pAsmBindInfo, pwzTmp, &dwSize);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    hr = StringCchCat(pwzSerialized, dwTotalSize, POLICY_TAG_ASSEMBLYBINDING_END);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = StringCchCat(pwzSerialized, dwTotalSize, POLICY_TAG_RUNTIME_END);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = StringCchCat(pwzSerialized, dwTotalSize, POLICY_TAG_CONFIGURATION_END);
    if (FAILED(hr)) {
        goto Exit;
    }
            
    // output it to UTF8. 
    
    dwTotalSize = lstrlenW(pwzSerialized);
        
    dwSize = WszWideCharToMultiByte(CP_UTF8, 0, pwzSerialized, dwTotalSize, NULL, 0, NULL, NULL);
    if (!dwSize) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Exit;
    }
    
    if (*pcbBufferSize >= dwSize) {
        if (!WszWideCharToMultiByte(CP_UTF8, 0, pwzSerialized, dwTotalSize, (LPSTR)pbBuffer, *pcbBufferSize, NULL, NULL)) {
            hr = HRESULT_FROM_WIN32(GetLastError());
            goto Exit;
        }
    }
    else {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }
    *pcbBufferSize = dwSize;

Exit:
    SAFEDELETEARRAY(pwzSerialized);
    return hr;
}

HRESULT GetSerializeRequiredSize(CAsmBindingInfo *pAsmBindInfo, DWORD *pdwSize)
{
    HRESULT hr = S_OK;
    LISTNODE pos;
    DWORD   dwSize = 0;
    DWORD   dwRedirSize = 0;
    CBindingRedir *pRedir = NULL;

    if (!pAsmBindInfo->_listBindingRedir.GetCount()) {
        // if no redir, no need to output anything.
        *pdwSize = 0;
        goto Exit;
    }

    dwSize += lstrlenW(POLICY_TAG_DEPENDENTASSEMBLY_START)
           +  lstrlenW(POLICY_TAG_DEPENDENTASSEMBLY_END)    // <dependentAssembly> and end tag
           +  lstrlenW(L"<assemblyIdentity name=\"\"/>")
           +  lstrlenW(pAsmBindInfo->_pwzName);

    if (pAsmBindInfo->_pwzCulture && pAsmBindInfo->_pwzCulture[0]) {
        dwSize += lstrlenW(L" culture=\"\"")
               +  lstrlenW(pAsmBindInfo->_pwzCulture);
    }

    if (pAsmBindInfo->_pwzPublicKeyToken && pAsmBindInfo->_pwzPublicKeyToken[0]) {
        dwSize += lstrlenW(L" publicKeyToken=\"\"")
              +  lstrlenW(pAsmBindInfo->_pwzPublicKeyToken);
    }

    if (pAsmBindInfo->_peKind != peNone) {
        dwSize += lstrlenW(L" processorArchitecture=\"\"")
               +  lstrlenW(ProcessorArchitectureFromPEKIND(pAsmBindInfo->_peKind));
    }

    dwSize++; // add null terminator
    
    pos = pAsmBindInfo->_listBindingRedir.GetHeadPosition();
    while(pos) {
        pRedir = pAsmBindInfo->_listBindingRedir.GetNext(pos);
        dwRedirSize = 0;
        hr = GetSerializeRequiredSize(pRedir, &dwRedirSize);
        if (FAILED(hr)) {
            goto Exit;
        }
        dwSize += dwRedirSize - 1; //remove the null terminator.
    }
    
    *pdwSize = dwSize;

Exit:
    return hr;
}

HRESULT GetSerializeRequiredSize(CQualifyAssembly *pQualifyAssembly, DWORD *pdwSize)
{
    *pdwSize = 0;
    return S_OK;
}

HRESULT GetSerializeRequiredSize(CBindingRedir *pRedir, DWORD *pdwSize)
{
    //<bindingRedirect oldVersion="1.0.0.0" newVersion="2.0.0.0"/>
    *pdwSize = lstrlenW(L"<bindingRedirect oldVersion=\"\" newVersion=\"\"/>") + lstrlenW(pRedir->_pwzVersionOld) + lstrlenW(pRedir->_pwzVersionNew) + 1;
    return S_OK;
}

HRESULT GetSerializeRequiredSize(CCodebaseHint *pCBHint, DWORD *pdwSize)
{
    *pdwSize = 0;
    return S_OK;
}

HRESULT Serialize(CAsmBindingInfo *pAsmBindInfo, 
                __out_ecount_opt(*pdwSize) LPWSTR pwzBuffer, __inout DWORD *pdwSize)
{
    HRESULT hr = S_OK;
    DWORD dwSize = 0;
    LISTNODE pos;
    CBindingRedir *pRedir = NULL;
    LPWSTR pwzTmp = NULL;

    hr = GetSerializeRequiredSize(pAsmBindInfo, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (dwSize == 0) {
        *pdwSize = 0;
        goto Exit;
    }

    if (*pdwSize < dwSize) {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        *pdwSize = dwSize;
        goto Exit;
    }

    // <dependentAssembly>
    hr = StringCchCopy(pwzBuffer, *pdwSize, POLICY_TAG_DEPENDENTASSEMBLY_START);
    if (FAILED(hr)) {
        goto Exit;
    }

    // <assemblyIdentity name="
    hr = StringCchCat(pwzBuffer, *pdwSize, L"<assemblyIdentity name=\"");
    if (FAILED(hr)) {
        goto Exit;
    }

    // name
    hr = StringCchCat(pwzBuffer, *pdwSize, pAsmBindInfo->_pwzName);
    if (FAILED(hr)) {
        goto Exit;
    }
    
    // "
    hr = StringCchCat(pwzBuffer, *pdwSize, L"\"");
    if (FAILED(hr)) {
        goto Exit;
    }
    
    if (pAsmBindInfo->_pwzCulture && pAsmBindInfo->_pwzCulture[0]) {
        // culture="
        hr = StringCchCat(pwzBuffer, *pdwSize, L" culture=\"");
        if (FAILED(hr)) {
            goto Exit;
        }
    
        // culture
        
        hr = StringCchCat(pwzBuffer, *pdwSize, pAsmBindInfo->_pwzCulture);
        if (FAILED(hr)) {
            goto Exit;
        }

        // "
        hr = StringCchCat(pwzBuffer, *pdwSize, L"\"");
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    if (pAsmBindInfo->_pwzPublicKeyToken && pAsmBindInfo->_pwzPublicKeyToken[0]) {
        //  publicKeyToken="
        hr = StringCchCat(pwzBuffer, *pdwSize, L" publicKeyToken=\"");
        if (FAILED(hr)) {
            goto Exit;
        }

        // public key token
        hr = StringCchCat(pwzBuffer, *pdwSize, pAsmBindInfo->_pwzPublicKeyToken);
        if (FAILED(hr)) {
            goto Exit;
        }

        // "
        hr = StringCchCat(pwzBuffer, *pdwSize, L"\"");
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    if (pAsmBindInfo->_peKind != peNone) {
        // processorArchitecture="
        hr = StringCchCat(pwzBuffer, *pdwSize, L" processorArchitecture=\"");
        if (FAILED(hr)) {
            goto Exit;
        }

        // processor architecture
        hr = StringCchCat(pwzBuffer, *pdwSize, ProcessorArchitectureFromPEKIND(pAsmBindInfo->_peKind));
        if (FAILED(hr)) {
            goto Exit;
        }

        // "
        hr = StringCchCat(pwzBuffer, *pdwSize, L"\"");
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    // />
    hr = StringCchCat(pwzBuffer, *pdwSize, L"/>");
    if (FAILED(hr)) {
        goto Exit;
    }

    pos = pAsmBindInfo->_listBindingRedir.GetHeadPosition();
    while(pos) {
        pRedir = pAsmBindInfo->_listBindingRedir.GetNext(pos);

        dwSize = lstrlenW(pwzBuffer);
        pwzTmp = pwzBuffer + dwSize;
        dwSize = *pdwSize - dwSize;

        hr = Serialize(pRedir, pwzTmp, &dwSize);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    hr = StringCchCat(pwzBuffer, *pdwSize, POLICY_TAG_DEPENDENTASSEMBLY_END);
    if (FAILED(hr)) {
        goto Exit;
    }
    
    
    *pdwSize = lstrlenW(pwzBuffer) + 1;
Exit:
    return hr;
}

HRESULT Serialize(CQualifyAssembly *pQualifyAssembly, 
                __out_ecount_opt(*pdwSize) LPWSTR pwzBuffer, __inout DWORD *pdwSize)
{
    *pdwSize = 0;
    return S_OK;
}

HRESULT Serialize(CBindingRedir *pRedir, 
                __out_ecount_opt(*pdwSize) LPWSTR pwzBuffer, __inout DWORD *pdwSize)
{
    HRESULT hr = S_OK;
    DWORD dwSize = 0;

    hr = GetSerializeRequiredSize(pRedir, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (*pdwSize >= dwSize) {
        hr = StringCchPrintf(pwzBuffer, *pdwSize, L"<bindingRedirect oldVersion=\"%ws\" newVersion=\"%ws\"/>", pRedir->_pwzVersionOld, pRedir->_pwzVersionNew);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    else {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }

    *pdwSize = dwSize;

Exit:
    return hr;
}

HRESULT Serialize(CCodebaseHint *pCBHint, 
                __out_ecount_opt(*pdwSize) LPWSTR pwzBuffer, __inout DWORD *pdwSize)
{
    *pdwSize = 0;
    return S_OK;
}

BOOL  IsMatch(CAsmBindingInfo *pAsmInfo, 
                LPCWSTR pwzName,
                LPCWSTR pwzCulture,
                LPCWSTR pwzPublicKeyToken,
                PEKIND  pe)
{
    if (FusionCompareStringI(pwzName, pAsmInfo->_pwzName)) {
        return FALSE;
    }

    if (pAsmInfo->_pwzPublicKeyToken && !pwzPublicKeyToken) {
        return FALSE;
    }

    if (!pAsmInfo->_pwzPublicKeyToken && pwzPublicKeyToken) {
        return FALSE;
    }

    if (pAsmInfo->_pwzPublicKeyToken) {
        if (FusionCompareStringI(pwzPublicKeyToken, pAsmInfo->_pwzPublicKeyToken)) {
            return FALSE;
        }
    }

    if (pAsmInfo->_pwzCulture && !pwzCulture) {
        return FALSE;
    }

    if (!pAsmInfo->_pwzCulture && pwzCulture) {
        return FALSE;
    }

    if (pAsmInfo->_pwzCulture) {
        if (FusionCompareStringI(pwzCulture, pAsmInfo->_pwzCulture)) {
            return FALSE;
        }
    }

    if (pe != peInvalid) { // input PE specified 
        if (pAsmInfo->_peKind != peNone && pAsmInfo->_peKind != pe) {
            // PE does not match.
            return FALSE;
        }
    }

    return TRUE;
}

BOOL    IsMatch(CAsmBindingInfo *pAsmInfo, 
                CAsmBindingInfo *pAsmInfo2)
{
    return IsMatch(pAsmInfo, pAsmInfo2->_pwzName, pAsmInfo2->_pwzCulture, pAsmInfo2->_pwzPublicKeyToken, pAsmInfo2->_peKind);
}

BOOL IsDuplicate(CAsmBindingInfo *pAsmInfoExist, CAsmBindingInfo *pAsmInfo) 
{
    _ASSERTE(pAsmInfo->_listBindingRedir.GetCount() == 1);

    if (!IsMatch(pAsmInfoExist, pAsmInfo->_pwzName, pAsmInfo->_pwzCulture, pAsmInfo->_pwzPublicKeyToken, pAsmInfo->_peKind)) {
        return FALSE;
    }

    LISTNODE pos;
    CBindingRedir *pRedirExist = NULL;
    CBindingRedir *pRedir = NULL;

    pos = pAsmInfo->_listBindingRedir.GetHeadPosition();
    while(pos) {
        // only check the first one
        pRedir = pAsmInfo->_listBindingRedir.GetNext(pos);
        break;
    }

    _ASSERTE(pRedir);

    pos = pAsmInfoExist->_listBindingRedir.GetHeadPosition();
    while(pos) {
        pRedirExist = pAsmInfoExist->_listBindingRedir.GetNext(pos);
        if (!FusionCompareStringI(pRedir->_pwzVersionOld, pRedirExist->_pwzVersionOld)
            && !FusionCompareStringI(pRedir->_pwzVersionNew, pRedirExist->_pwzVersionNew)) {
            return TRUE;
        }
    }

    return FALSE;
}

HRESULT AddRedirToBindInfo(CBindingRedir *pRedir,
                           CAsmBindingInfo *pAsmInfo)
{
    HRESULT hr = S_OK;
    CBindingRedir *pRedirExist = NULL;
    LISTNODE pos;

    pos = pAsmInfo->_listBindingRedir.GetHeadPosition();
    while(pos) {
        pRedirExist = pAsmInfo->_listBindingRedir.GetNext(pos);
        // dup exists, do nothing
        if (!FusionCompareStringI(pRedir->_pwzVersionOld, pRedirExist->_pwzVersionOld)) {
            hr = S_FALSE;
            goto Exit;
        }
    }

    // no dup
    if (!(pAsmInfo->_listBindingRedir).AddTail(pRedir)) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = S_OK;

Exit:
    return hr;
}

HRESULT AddCodebaseHintToBindInfo(CCodebaseHint *pCodebase,
                           CAsmBindingInfo *pAsmInfo)
{
    HRESULT hr = S_OK;
    CCodebaseHint *pCodebaseExist = NULL;
    LISTNODE pos;

    pos = pAsmInfo->_listCodebase.GetHeadPosition();
    while(pos) {
        pCodebaseExist = pAsmInfo->_listCodebase.GetNext(pos);
        // dup exists, do nothing
        if (!FusionCompareStringI(pCodebase->_pwzVersion, pCodebaseExist->_pwzVersion)) {
            hr = S_FALSE;
            goto Exit;
        }
    }

    // no dup
    if (!(pAsmInfo->_listCodebase).AddTail(pCodebase)) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = S_OK;

Exit:
    return hr;
}

HRESULT MergeBindInfo(CAsmBindingInfo *pAsmInfo,
                      CAsmBindingInfo *pAsmInfoNew)
{
    HRESULT hr = S_OK;
    CBindingRedir *pRedir = NULL;
    CCodebaseHint *pCodebase = NULL;
    LISTNODE pos;
    LISTNODE oldpos;

    pos = pAsmInfoNew->_listBindingRedir.GetHeadPosition();
    while(pos) {
        oldpos = pos;
        pRedir = pAsmInfoNew->_listBindingRedir.GetNext(pos);
        pAsmInfoNew->_listBindingRedir.RemoveAt(oldpos);

        hr = AddRedirToBindInfo(pRedir, pAsmInfo);
        if (hr != S_OK) {
            SAFEDELETE(pRedir);
            if (FAILED(hr)) {
                goto Exit;
            }
        }
    }

    pos = pAsmInfoNew->_listCodebase.GetHeadPosition();
    while(pos) {
        oldpos = pos;
        pCodebase = pAsmInfoNew->_listCodebase.GetNext(pos);
        pAsmInfoNew->_listCodebase.RemoveAt(oldpos);

        hr = AddCodebaseHintToBindInfo(pCodebase, pAsmInfo);
        if (hr != S_OK) {
            SAFEDELETE(pCodebase);
            if (FAILED(hr)) {
                goto Exit;
            }
        }
    }

    hr = S_OK;

Exit:
    return hr;
}
