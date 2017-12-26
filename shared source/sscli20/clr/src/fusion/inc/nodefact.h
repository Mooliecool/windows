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
#ifndef __NODEFACT_H_
#define __NODEFACT_H_

#include "list.h"
#include "xmlns.h"
#include "dbglog.h"

typedef enum tagParseState {
    PSTATE_LOOKUP_CONFIGURATION,
    PSTATE_CONFIGURATION,
    PSTATE_RUNTIME,
    PSTATE_ASSEMBLYBINDING_ROOT,
    PSTATE_ASSEMBLYBINDING,
    PSTATE_ASSEMBLYBINDINGV2,
    PSTATE_DEPENDENTASSEMBLY,
} ParseState;

#define CFG_CULTURE_NEUTRAL                         L"neutral"

#define XML_CONFIGURATION_DEPTH                    1
#define XML_RUNTIME_DEPTH                          2
#define XML_ASSEMBLYBINDINGV2_DEPTH                2
#define XML_ASSEMBLYBINDING_DEPTH                  3
#define XML_ASSEMBLY_STORE_DEPTH                   3
#define XML_DEVOVERRIDE_PATH_DEPTH                 3
#define XML_ASSEMBLYBINDING_ROOT_DEPTH             2
#define XML_LINKED_CONFIGURATION_DEPTH             3
#define XML_PROBING_DEPTH                          4
#define XML_DEPENDENTASSEMBLY_DEPTH                4
#define XML_GLOBAL_PUBLISHERPOLICY_DEPTH           4
#define XML_ASSEMBLYIDENTITY_DEPTH                 5
#define XML_BINDINGREDIRECT_DEPTH                  5
#define XML_CODEBASE_DEPTH                         5
#define XML_PUBLISHERPOLICY_DEPTH                  5



#define XML_QUALIFYASSEMBLY_DEPTH                  4
#define POLICY_TAG_QUALIFYASSEMBLY                 L"urn:schemas-microsoft-com:asm.v1^qualifyAssembly"
#define XML_ATTRIBUTE_PARTIALNAME                  L"partialName"
#define XML_ATTRIBUTE_FULLNAME                     L"fullName"


// maximum length of policy tag with namespace.
// An optimization to not allocate unnecessary memory when parsing. 
#define MAX_POLICY_TAG_LENGTH                      128

#define POLICY_TAG_CONFIGURATION                   L"configuration"
#define POLICY_TAG_RUNTIME                         L"runtime"
#define POLICY_TAG_ASSEMBLYBINDING                 L"urn:schemas-microsoft-com:asm.v1^assemblyBinding"
#define POLICY_TAG_ASSEMBLYBINDINGV2               L"urn:schemas-microsoft-com:asm.v2^assemblyBinding"
#define POLICY_TAG_PROBING                         L"urn:schemas-microsoft-com:asm.v1^probing"
#define POLICY_TAG_DEPENDENTASSEMBLY               L"urn:schemas-microsoft-com:asm.v1^dependentAssembly"
#define POLICY_TAG_ASSEMBLYIDENTITY                L"urn:schemas-microsoft-com:asm.v1^assemblyIdentity"
#define POLICY_TAG_BINDINGREDIRECT                 L"urn:schemas-microsoft-com:asm.v1^bindingRedirect"
#define POLICY_TAG_CODEBASE                        L"urn:schemas-microsoft-com:asm.v1^codeBase"
#define POLICY_TAG_PUBLISHERPOLICY                 L"urn:schemas-microsoft-com:asm.v1^publisherPolicy"
#define POLICY_TAG_LINKEDCONFIGURATION             L"urn:schemas-microsoft-com:asm.v1^linkedConfiguration"
#define POLICY_TAG_ASSEMBLYSTORE                   L"urn:schemas-microsoft-com:asm.v2^assemblyStore"
#define POLICY_TAG_DEVOVERRIDE_PATH                L"urn:schemas-microsoft-com:asm.v2^devOverridePath"

#define XML_ATTRIBUTE_NAME                         L"name"
#define XML_ATTRIBUTE_PUBLICKEYTOKEN               L"publicKeyToken"
#define XML_ATTRIBUTE_CULTURE                      L"culture"
#define XML_ATTRIBUTE_PROCESSORARCHITECTURE        L"processorArchitecture" 
#define XML_ATTRIBUTE_VERSION                      L"version"
#define XML_ATTRIBUTE_OLDVERSION                   L"oldVersion"
#define XML_ATTRIBUTE_NEWVERSION                   L"newVersion"
#define XML_ATTRIBUTE_HREF                         L"href"
#define XML_ATTRIBUTE_APPLY                        L"apply"
#define XML_ATTRIBUTE_PRIVATEPATH                  L"privatePath"
#define XML_ATTRIBUTE_NEWPUBLICKEYTOKEN            L"newPublicKeyToken" 
#define XML_ATTRIBUTE_APPLIESTO                    L"appliesTo"
#define XML_ATTRIBUTE_NEWNAME                      L"newName"
#define XML_ATTRIBUTE_LOCATION                     L"location"

class CAsmBindingInfo;
class CQualifyAssembly;
class CCodebaseHint;
class CBindingRedir;
class CDebugLog;
class CNamespaceManager;

class CNodeFactory : public IXMLNodeFactory
{
    friend class CPublisherPolicy;

    public:
        enum ParseCtl {
            parseAll,               // parse entire config file
            stopAfterRuntimeSection // stop after <runtime>...</runtime> section
        };

        CNodeFactory(CDebugLog *pdbglog, ParseCtl parseCtl = parseAll);
        virtual ~CNodeFactory();

        // IUnknown methods

        STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
        STDMETHODIMP_(ULONG) AddRef();
        STDMETHODIMP_(ULONG) Release();

        // IXMLNodeFactory methods

        STDMETHODIMP NotifyEvent(IXMLNodeSource *pSource, XML_NODEFACTORY_EVENT iEvt);
        STDMETHODIMP BeginChildren(IXMLNodeSource *pSource, XML_NODE_INFO *pNodeInfo);
        STDMETHODIMP EndChildren(IXMLNodeSource *pSource, BOOL fEmpty, XML_NODE_INFO *pNodeInfo);
        STDMETHODIMP Error(IXMLNodeSource *pSource, HRESULT hrErrorCode, USHORT cNumRecs, XML_NODE_INFO __RPC_FAR **aNodeInfo);
        STDMETHODIMP CreateNode(IXMLNodeSource __RPC_FAR *pSource, PVOID pNodeParent, USHORT cNumRecs, XML_NODE_INFO __RPC_FAR **aNodeInfo);

        // methods to retrieve various properties

        HRESULT GetPolicyVersion(LPCWSTR wzAssemblyName, 
                                 LPCWSTR wzPublicKeyToken,
                                 LPCWSTR wzCulture, 
                                 LPCWSTR wzVersionIn, 
                                 PEKIND peIn,
                                 __out_ecount(*pdwSizeVer) LPWSTR  pwzVersionOut,
                                 __inout LPDWORD pdwSizeVer,
                                 PEKIND *peOut);

        HRESULT GetSafeMode(LPCWSTR wzAssemblyName, LPCWSTR wzPublicKeyToken,
                            LPCWSTR wzCulture, LPCWSTR wzVersion, PEKIND pe, 
                            BOOL *pbSafeMode);

        HRESULT GetCodebaseHint(LPCWSTR pwzAsmName, LPCWSTR pwzVersion,
                                LPCWSTR pwzPublicKeyToken, LPCWSTR pwzCulture, PEKIND pe, 
                                LPCWSTR pwzAppBase, __deref_out LPWSTR *ppwzCodebase);

        HRESULT GetPrivatePath(__deref_out LPWSTR *ppwzPrivatePath);
        
        HRESULT QualifyAssembly(LPCWSTR pwzDisplayName, IAssemblyName **ppNameQualified, CDebugLog *pdbglog);

        HRESULT ProcessLinkedConfiguration(LPCWSTR pwzRootConfigPath);

        HRESULT GetAssemblyStorePath(__deref_out LPWSTR *ppwzAssemblyStore);
        HRESULT GetDevOverridePath(__deref_out LPWSTR *ppwzDevOverridePath);

        HRESULT HasFrameworkRedirect(BOOL *pbHasFxRedirect);

        HRESULT OutputToUTF8(DWORD *pcbBufferSize,
                             BYTE  *pbBuffer);

        HRESULT AddPolicy(   LPCWSTR wzAsmName,
                             LPCWSTR wzOldVersion,
                             LPCWSTR wzCulture,
                             LPCWSTR wzPublicKeyToken,
                             PEKIND  pe,
                             LPCWSTR wzNewVersion,
                             DWORD   dwModifyPolicyFlags);

        HRESULT RemovePolicy(LPCWSTR wzAsmName,
                             LPCWSTR wzVersion,     // This is the target version
                             LPCWSTR wzCulture,
                             LPCWSTR wzPublicKeyToken,
                             PEKIND  pe);
        
        void ReleaseLogObject() {SAFERELEASE(_pdbglog);}
        
    private:
        HRESULT ProcessAssemblyStoreTag(XML_NODE_INFO **aNodeInfo, USHORT cNumRecs); 
        HRESULT ProcessDevOverrideTag(XML_NODE_INFO **aNodeInfo, USHORT cNumRecs); 
        HRESULT ProcessProbingTag(XML_NODE_INFO **aNodeInfo, USHORT cNumRecs);

        HRESULT ProcessLinkedConfigurationTag(XML_NODE_INFO **aNodeInfo, USHORT cNumRecs);
        
        HRESULT ProcessQualifyAssemblyTag(XML_NODE_INFO **aNodeInfo, USHORT cNumRecs);

        HRESULT ProcessBindingRedirectTag(XML_NODE_INFO **aNodeInfo,
                                          USHORT cNumRecs, CBindingRedir *pRedir);
        HRESULT ProcessAssemblyBindingTag(XML_NODE_INFO **aNodeInfo, USHORT cNumRecs);
        HRESULT ProcessCodebaseTag(XML_NODE_INFO **aNodeInfo, USHORT cNumRecs,
                                   CCodebaseHint *pCB);
        HRESULT ProcessPublisherPolicyTag(XML_NODE_INFO **aNodeInfo,
                                          USHORT cNumRecs,
                                          BOOL bGlobal);
        HRESULT ProcessAssemblyIdentityTag(XML_NODE_INFO **aNodeInfo, USHORT cNumRecs);
        
        HRESULT ApplyNamespace(XML_NODE_INFO *pNodeInfo, 
                               __out_ecount_opt(*pdwSize) LPWSTR pwzTokenNS,
                               __inout LPDWORD pdwSize, 
                               DWORD dwFlags);

        HRESULT CheckProcessedConfigurations(LPCWSTR pwzCanonicalUrl,
                                             BOOL *pbProcessed);

        HRESULT AddToResult();

    private:
        DWORD                             _dwSig;
        LONG                              _cRef;
        DWORD                             _dwState;
        DWORD                             _dwCurDepth;
        BOOL                              _bGlobalSafeMode;
        LPWSTR                            _pwzPrivatePath;
        List<LPWSTR>                      _listLinkedConfiguration;
        List<CAsmBindingInfo *>           _listAsmInfo;
        List<CQualifyAssembly *>          _listQualifyAssembly;
        List<LPCWSTR>                     _listProcessedCfgs;
        CDebugLog                        *_pdbglog;
        CAsmBindingInfo                  *_pAsmInfo;
        CNamespaceManager                 _nsmgr;
        // Is current runtime version matching the ones specified in "appliesTo"?
        BOOL                              _bCorVersionMatch;
        ParseCtl                          _parseCtl;
        LPWSTR                            _pwzAssemblyStore;
        LPWSTR                            _pwzDevOverridePath;
};

// all size in wchar_t
HRESULT GetSerializeRequiredSize(CAsmBindingInfo *pAsmBindInfo, DWORD *pdwSize);
HRESULT GetSerializeRequiredSize(CQualifyAssembly *pQualifyAssembly, DWORD *pdwSize);
HRESULT GetSerializeRequiredSize(CBindingRedir *pRedir, DWORD *pdwSize);
HRESULT GetSerializeRequiredSize(CCodebaseHint *pCBHint, DWORD *pdwSize);

HRESULT Serialize(CAsmBindingInfo *pAsmBindInfo, 
                __out_ecount(*pdwSize) LPWSTR pwzBuffer,
                __inout DWORD *pdwSize);
HRESULT Serialize(CQualifyAssembly *pQualifyAssembly, 
                __out_ecount(*pdwSize) LPWSTR pwzBuffer,
                __inout DWORD *pdwSize);
HRESULT Serialize(CBindingRedir *pRedir, 
                __out_ecount(*pdwSize) LPWSTR pwzBuffer,
                __inout DWORD *pdwSize);
HRESULT Serialize(CCodebaseHint *pCBHint, 
                __out_ecount(*pdwSize) LPWSTR pwzBuffer,
                __inout DWORD *pdwSize);

BOOL    IsMatch(CAsmBindingInfo *pAsmInfo, 
                LPCWSTR pwzName,
                LPCWSTR pwzCulture,
                LPCWSTR pwzPublicKeyToken,
                PEKIND  pe);

BOOL    IsMatch(CAsmBindingInfo *pAsmInfo, 
                CAsmBindingInfo *pAsmInfo2);

BOOL    IsDuplicate(CAsmBindingInfo *pAsmInfoExist,
                    CAsmBindingInfo *pAsmInfo);

HRESULT AddRedirToBindInfo(CBindingRedir *pRedir,
                           CAsmBindingInfo *pAsmInfo);

HRESULT AddCodebaseHintToBindInfo(CCodebaseHint *pCodebase,
                           CAsmBindingInfo *pAsmInfo);

HRESULT MergeBindInfo(CAsmBindingInfo *pAsmInfo,
                      CAsmBindingInfo *pAsmInfoNew);

#endif  // __NODEFACT_H_
