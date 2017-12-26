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
// EEConfigFactory.cpp -
//
// Factory used to with the XML parser to read configuration files
//
//
#include "common.h"
#include "eeconfigfactory.h"

#define ISWHITE(ch) ((ch) >= 0x09 && (ch) <= 0x0D || (ch) == 0x20)

#define CONST_STRING_AND_LEN(str) str, NumItems(str)-1


int EEXMLStringCompare(const WCHAR *pStr1, 
                    DWORD cchStr1, 
                    const WCHAR *pStr2, 
                    DWORD cchStr2)
{
    if (cchStr1 != cchStr2)
        return -1;

    return wcsncmp(pStr1, pStr2, cchStr1);
}// EEXMLStringCompare


int EEXMLStringComparei(const WCHAR *pStr1, 
                    DWORD cchStr1, 
                    const WCHAR *pStr2, 
                    DWORD cchStr2)
{
    if (cchStr1 != cchStr2)
        return -1;

    return SString::_wcsnicmp(pStr1, pStr2, cchStr1);
}// EEXMLStringCompare



EEConfigFactory::EEConfigFactory(
    EEUnicodeStringHashTable* pTable,
    LPCWSTR pString,
    ParseCtl parseCtl) 
{
    LEAF_CONTRACT;
    m_pTable = pTable;
    m_pVersion = pString;
    m_dwDepth = 0;
    m_fUnderRuntimeElement = FALSE;
    m_fDeveloperSettings = FALSE;
    m_fVersionedRuntime= FALSE;
    m_fOnEnabledAttribute = FALSE;
    m_pCurrentRuntimeElement = m_pBuffer;
    m_dwCurrentRuntimeElement = 0;
    m_dwSize = CONFIG_KEY_SIZE;
    m_parseCtl = parseCtl;
}

EEConfigFactory::~EEConfigFactory() 
{
    LEAF_CONTRACT;
    DeleteKey();
}

HRESULT STDMETHODCALLTYPE EEConfigFactory::NotifyEvent( 
            /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
            /* [in] */ XML_NODEFACTORY_EVENT iEvt)
{
    LEAF_CONTRACT;
    if(iEvt == XMLNF_ENDDOCUMENT) {
    }
    
    return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE EEConfigFactory::BeginChildren( 
    /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
    /* [in] */ XML_NODE_INFO __RPC_FAR *pNodeInfo)
{
    LEAF_CONTRACT;

    m_dwDepth++;
    
    return S_OK;

}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE EEConfigFactory::EndChildren( 
    /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
    /* [in] */ BOOL fEmptyNode,
    /* [in] */ XML_NODE_INFO __RPC_FAR *pNodeInfo)
{
    LEAF_CONTRACT;
    if ( fEmptyNode ) { 
        m_fDeveloperSettings = FALSE;        
    }
    else {
        m_dwDepth--;
    }
    if (m_fUnderRuntimeElement && wcscmp(pNodeInfo->pwcText, L"runtime") == 0) {
        m_fUnderRuntimeElement = FALSE;
        m_fVersionedRuntime = FALSE;
        ClearKey();
        // CLR_STARTUP_OPT:
        // Early out if we only need to read <runtime> section.
        //
        if (m_parseCtl == stopAfterRuntimeSection)
            pSource->Abort(NULL/*unused*/);
    }
    
    return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE EEConfigFactory::CreateNode( 
    /* [in] */ IXMLNodeSource __RPC_FAR *pSource,
    /* [in] */ PVOID pNode,
    /* [in] */ USHORT cNumRecs,
    /* [in] */ XML_NODE_INFO* __RPC_FAR * __RPC_FAR apNodeInfo)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END;
    
    if(m_dwDepth > 3)
    {
        
        return S_OK;
    }

    HRESULT hr = S_OK;
    DWORD  dwStringSize = 0;
    WCHAR* pszString = NULL;
    DWORD  i; 
    BOOL   fRuntimeKey = FALSE;
    BOOL   fVersion = FALSE;

    for( i = 0; i < cNumRecs; i++) { 
        CONTRACT_VIOLATION(ThrowsViolation); // Lots of stuff in here throws!
        
        if(apNodeInfo[i]->dwType == XML_ELEMENT ||
           apNodeInfo[i]->dwType == XML_ATTRIBUTE ||
           apNodeInfo[i]->dwType == XML_PCDATA) {

            dwStringSize = apNodeInfo[i]->ulLen;
            pszString = (WCHAR*) apNodeInfo[i]->pwcText;
            // Trim the value

            // we should never decrement lgth if it's 0, because it's unsigned

            for(;*pszString && ISWHITE(*pszString) && dwStringSize>0; pszString++, dwStringSize--);
            while( dwStringSize > 0 && ISWHITE(pszString[dwStringSize-1]))
                   dwStringSize--;

            // NOTE: pszString is not guaranteed to be null terminated. Use EEXMLStringCompare to do
            // string comparisions on it

            switch(apNodeInfo[i]->dwType) {
            case XML_ELEMENT : 
                fRuntimeKey = FALSE;
                ClearKey();
                
                if (m_dwDepth == 1 && EEXMLStringCompare(pszString, dwStringSize, CONST_STRING_AND_LEN(L"runtime")) == 0) {
                    m_fUnderRuntimeElement = TRUE;
                    fRuntimeKey = TRUE;
                }
    
                if(m_dwDepth == 2 && m_fUnderRuntimeElement) {
                    
                    // Developer settings can look like
                    // <runtime>
                    //     <developerSettings installationVersion="v2.0.40223.0" />
                    //
                    // or
                    //
                    //     <developmentMode developerInstallation="true" />
                    //
                    // Neither one is your standard config setting.
                    if (!EEXMLStringCompare(pszString, dwStringSize, CONST_STRING_AND_LEN(L"developerSettings")) ||
                        !EEXMLStringCompare(pszString, dwStringSize, CONST_STRING_AND_LEN(L"developmentMode")))
                        m_fDeveloperSettings = TRUE;
                    else
                    {
                        // This is a standard element under the runtime node.... it could look like this
                        // <runtime>
                        //     <pszString enabled="1" />

                        hr = CopyToKey(pszString, dwStringSize);
                        if(FAILED(hr)) return hr;
                    }
                }
                // If our depth isn't 2, and we're not under the runtime element....
                else
                    ClearKey();

                break ;     
                
            case XML_ATTRIBUTE : 
                if(fRuntimeKey && EEXMLStringCompare(pszString, dwStringSize, CONST_STRING_AND_LEN(L"version")) == 0) {
                    fVersion = TRUE;
                }
                else 
                {
                    if (m_dwDepth == 2 && m_fUnderRuntimeElement)
                    {
                        if (!m_fDeveloperSettings)
                        {
                            _ASSERTE(m_dwCurrentRuntimeElement > 0);

                            // The standard model for runtime config settings is as follows
                            //
                            // <runtime>
                            //    <m_pCurrentRuntimeElement enabled="true|false" />
                            //
                            // or
                            //    <m_pCurrentRuntimeElement enabled="1|0" />
                            
                            // If the attribute that we found isn't "enabled", then we know we don't have
                            // a setting that follows the standard model. With the exception of developerSettings,
                            // there currently aren't any other schemas

                            if (EEXMLStringComparei(pszString, dwStringSize, CONST_STRING_AND_LEN(L"enabled")) != 0) 
                            {
                                m_fOnEnabledAttribute = FALSE;
                            }
                            else
                                m_fOnEnabledAttribute = TRUE;
                        }
                        else // We're looking at developer settings 
                        {       
                            // Developer settings look like
                            //     <developerSettings installationVersion="v2.0.40223.0" />
                            //
                            // or
                            //
                            //     <developmentMode developerInstallation="true" />
                            //

                            // The key name will actually be the attribute name                            
                            
                            hr = CopyToKey(pszString, dwStringSize);
                            if(FAILED(hr)) return hr;
                            m_fOnEnabledAttribute = FALSE;
                        }
                    }     
                }
                break;
            case XML_PCDATA:
                if(fVersion) {
                    // if this is not the right version
                    // then we are not interested 
                    if(EEXMLStringCompare(pszString, dwStringSize, m_pVersion, (DWORD)wcslen(m_pVersion))) {
                        m_fUnderRuntimeElement = FALSE;
                    }
                    else {
                        // if it is the right version then overwrite
                        // all entries that exist in the hash table
                        m_fVersionedRuntime = TRUE;
                    }

                    fVersion = FALSE;
                }
                else if(fRuntimeKey) {
                    break; // Ignore all other attributes on <runtime>
                }
                
                // m_dwCurrentRuntimeElement is set when we called CopyToKey in the XML_ELEMENT case
                // section above.
                //
                // We'll only pay attention to this data if its for the Developer Settings or if the
                // attribute text is "enabled"
                else if(m_dwCurrentRuntimeElement > 0 && (m_fDeveloperSettings || m_fOnEnabledAttribute)) {

                    // This means that, either we are working on attribute values for the developer settings,
                    // or we've got what "enabled" is equal to....
                    //
                    // <runtime>
                    //   <m_pwzCurrentElementUnderRuntimeElement m_pLastKey=pString />
                        
                    if (!m_fDeveloperSettings) {
                        // For the non-developer settings, let's convert all trues to 1s and the falses to 0s
                        if (EEXMLStringComparei(pszString, dwStringSize, CONST_STRING_AND_LEN(L"false")) == 0) {
                            pszString = L"0";
                            dwStringSize = 1;
                        }    
                        else if (EEXMLStringComparei(pszString, dwStringSize, CONST_STRING_AND_LEN(L"true")) == 0) {
                            pszString = L"1";
                            dwStringSize = 1;
                        }                                

                    }    

                    EEStringData sKey(m_dwCurrentRuntimeElement, m_pCurrentRuntimeElement);
                    HashDatum data;

                    // Allocate memory that can store this setting
                    NewHolder<WCHAR> pStringToKeep(new (nothrow)WCHAR[dwStringSize+1]);
                    if (pStringToKeep == NULL)
                        return E_OUTOFMEMORY;
                    wcsncpy_s(pStringToKeep, dwStringSize + 1, pszString, dwStringSize);
                    pStringToKeep[dwStringSize] = '\0';

                    // See if we've already picked up a value for this setting
                    if(m_pTable->GetValue(&sKey, &data)) {
                            
                        // If this is a config section for this runtime version, then it's allowed to overwrite
                        // previous settings that we've picked up
                        if(m_fVersionedRuntime) {
                            pStringToKeep.SuppressRelease();
                            m_pTable->ReplaceValue(&sKey, pStringToKeep);
                        }
                    }
                    else {
                        // We're adding a new config item
                        pStringToKeep.SuppressRelease();
                        CONTRACT_VIOLATION(ThrowsViolation); // Mismatch between CreateNode and InsertValue
                        m_pTable->InsertValue(&sKey, pStringToKeep, TRUE);
                    }
                }
                    
                break ;     
            default: 
                ;
            } // end of switch
        }
    }
    return hr;
}

