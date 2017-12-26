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

#ifndef __SECURITYATTRIBUTES_H__
#define __SECURITYATTRIBUTES_H__

#include "vars.hpp"
#include "eehash.h"
#include "corperm.h"

class Security;
class SecurityDescriptor;
class AssemblySecurityDescriptor;
class SecurityStackWalk;
class SecurityPolicy;
class SecurityDeclarative;
class COMCustomAttribute;
struct TokenDeclActionInfo;

enum CanUnrestrictedOverride
{
    CUO_DontKnow = 0,
    CUO_Yes = 1,
    CUO_No = 2,
};

// Records a serialized permission set we've seen and decoded. This entry
// exists both in a global dynamic array (which gives it an index used to
// identify the pset across appdomains) and chained into a hash (which
// allows us look for existing entries quickly when decoding new psets).
struct PsetCacheEntry
{
    EEHashEntry m_sHashEntry;
    PBYTE m_pbPset;
    DWORD m_cbPset;
    DWORD m_dwIndex;
    CanUnrestrictedOverride m_eCanUnrestrictedOverride;

    PsetCacheEntry(PBYTE pbPset, DWORD cbPset) :
        m_pbPset(pbPset),
        m_cbPset(cbPset),
        m_eCanUnrestrictedOverride(CUO_DontKnow)
    {
        LEAF_CONTRACT;
    }

    BOOL IsEquiv(PsetCacheEntry *pOther)
    {
        WRAPPER_CONTRACT;
        if (m_cbPset != pOther->m_cbPset || !m_pbPset || !pOther->m_pbPset)
            return FALSE;
        return memcmp(m_pbPset, pOther->m_pbPset, m_cbPset) == 0;
    }

    DWORD Hash()
    {
        LEAF_CONTRACT;
        DWORD dwHash = 0;
        for (DWORD i = 0; i < (m_cbPset / sizeof(DWORD)); i++)
            dwHash ^= GET_UNALIGNED_VAL32(&((DWORD*)m_pbPset)[i]);
        return dwHash;
    }
};


class PsetHandleCache: public ArrayList 
{
public:
    // If the index doesn't exist, add another array of additional size to include this index
    // Need a writer lock
    OBJECTHANDLE GrowAndGetHandle(size_t dwIndex, 
                        size_t *newSize, 
                        SimpleRWLock *prGlobalLock, 
                        AppDomain *pDomain);

    OBJECTHANDLE GrowAndGetHandleHaveLock(size_t dwIndex, 
                        size_t *newSize, 
                        AppDomain *pDomain);
};




class SecurityAttributes
{
friend class Security;
friend class COMCustomAttribute;
friend class SecurityDescriptor;
friend class AssemblySecurityDescriptor;
friend class PEFileSecurityDescriptor;
friend class ApplicationSecurityDescriptor;
friend class SecurityRuntime;
friend class SecurityStackWalk;
friend class EEPsetHashTableHelper;
friend class SecurityPolicy;
friend class SecurityDeclarative;
friend class SharedSecurityDescriptor;
friend struct TokenDeclActionInfo;
friend struct MethodSecurityDescriptor;
friend HRESULT AttributeSetToBlob(CORSEC_ATTRSET* pAttrSet, BYTE* pBuffer, DWORD* pCount, IMetaDataAssemblyImport *pImport, DWORD dwAction);
friend HRESULT BlobToAttributeSet(BYTE* pBuffer, CORSEC_ATTRSET* pAttrSet, DWORD dwAction);
friend HRESULT STDMETHODCALLTYPE TranslateSecurityAttributes(CORSEC_ATTRSET    *pAttrSet,
                                                                BYTE          **ppbOutput,
                                                                DWORD          *pcbOutput,
                                                                BYTE          **ppbNonCasOutput,
                                                                DWORD          *pcbNonCasOutput,
                                                                DWORD          *pdwErrorIndex);

private:
    // Cached Permission sets
    static CQuickArrayStatic<PsetCacheEntry> s_rCachedPsets;
    static EEPsetHashTable s_sCachedPsetsHash;
    static SimpleRWLock * s_prCachedPsetsLock;

    static VOID Init();
    static VOID Shutdown();

    static HRESULT MapToHR(OBJECTREF ref);

    // Retrieves a previously loaded PermissionSet 
    // object index (this will work even if the permission set was loaded in
    // a different appdomain).
    static OBJECTREF GetPermissionSet(DWORD dwIndex, DWORD dwAction);

    // Locate the index of a permission set in the cache (returns false if the
    // permission set has not yet been seen and decoded).
    static BOOL LookupPermissionSet(IN PBYTE       pbPset,
                                    IN DWORD       cbPset,
                                    OUT DWORD     *pdwSetIndex);

    // Creates a new permission set
    static OBJECTREF CreatePermissionSet(BOOL fTrusted);


    // Takes two PermissionSets (referenced by index) and merges them (unions or intersects
    // depending on fIntersect) and returns the index of the merged PermissionSet
    static DWORD MergePermissionSets(IN DWORD dwSetIndex1, IN DWORD dwSetIndex2, IN bool fIntersect, IN DWORD dwAction);

    // Uses new to create the byte array that is returned.
    static void CopyByteArrayToEncoding(IN U1ARRAYREF* pArray,
                                        OUT PBYTE* pbData,
                                        OUT DWORD* cbData);

    static void EncodePermissionSet(IN OBJECTREF* pRef,
                                    OUT PBYTE* ppbData,
                                    OUT DWORD* pcbData);

    // Generic routine, use with encoding calls that 
    // use the EncodePermission client data
    // Uses new to create the byte array that is returned.
    static void CopyEncodingToByteArray(IN PBYTE   pbData,
                                        IN DWORD   cbData,
                                        IN OBJECTREF* pArray);

    static BOOL RestrictiveRequestsInAssembly(IMDInternalImport* pImport);
    static BOOL PermissionsRequestedInAssembly(IN Assembly* pAssembly);

    // Returns the declared PermissionSet or PermissionSetCollection for the
    // specified action type.
    static HRESULT GetDeclaredPermissions(IN IMDInternalImport *pInternalImport,
                                          IN mdToken token, // token for method, class, or assembly
                                          IN CorDeclSecurity action, // SecurityAction
                                          OUT OBJECTREF *pDeclaredPermissions, // The returned PermissionSet for that SecurityAction
                                          OUT DWORD* pdwSetIndex = NULL); // The index of the PermissionSet


    static HRESULT TranslateSecurityAttributesHelper(
                                CORSEC_ATTRSET    *pAttrSet,
                                BYTE          **ppbOutput,
                                DWORD          *pcbOutput,
                                BYTE          **ppbNonCasOutput,
                                DWORD          *pcbNonCasOutput,
                                DWORD          *pdwErrorIndex);

    static HRESULT FixUpPermissionSetAttribute(CORSEC_ATTRIBUTE* pPerm);
    static HRESULT SerializeAttribute(CORSEC_ATTRIBUTE* pAttr, BYTE* pBuffer, DWORD* pCount, IMetaDataAssemblyImport *pImport);
    static HRESULT DeserializeAttribute(CORSEC_ATTRIBUTE *pAttr, BYTE* pBuffer, DWORD* pPos);
    static bool ContainsBuiltinCASPermsOnly(DWORD dwSetIndex, DWORD dwAction)
    {
        // Check the cache
        PsetCacheEntry *pPCE = &s_rCachedPsets[dwSetIndex];
        if(pPCE->m_eCanUnrestrictedOverride == CUO_Yes)
            return true;
        if(pPCE->m_eCanUnrestrictedOverride == CUO_No)
            return false;
        
        bool bRet = ContainsBuiltinCASPermsOnlyInternal(pPCE, dwAction);
        // Cache the results
        if(bRet)
            pPCE->m_eCanUnrestrictedOverride = CUO_Yes;
        else
            pPCE->m_eCanUnrestrictedOverride = CUO_No;
        
        return bRet;

    }

    static bool ContainsBuiltinCASPermsOnly(CORSEC_ATTRSET* pAttrSet)
    {
        DWORD n;
        for(n = 0; n < pAttrSet->dwAttrCount; n++)
        {
            CORSEC_ATTRIBUTE* pAttr = &pAttrSet->pAttrs[n];
            if(!IsBuiltInCASPermissionAttribute(pAttr))
                return false;
        }
        return true;
        
    }
private:
    static bool IsUnrestrictedPermissionSetAttribute(CORSEC_ATTRIBUTE* pAttr);
    static bool IsBuiltInCASPermissionAttribute(CORSEC_ATTRIBUTE* pAttr);

    // helper to avoid EH overhead in the public API if we take the fastpath
    static bool ContainsBuiltinCASPermsOnlyInternal(PsetCacheEntry *pPCE, DWORD dwAction)
    {
        // Deserialize the CORSEC_ATTRSET
        CORSEC_ATTRSET attrSet;
        HRESULT hr = BlobToAttributeSet(pPCE->m_pbPset, &attrSet, dwAction);
        if(FAILED(hr))
            COMPlusThrowHR(hr);

        if (hr == S_FALSE)
            return FALSE;

        return ContainsBuiltinCASPermsOnly(&attrSet);
    }
    static void SecurityAttributes::BlobToPermissionSet(IN PBYTE pbAttrBlob,
                                                       IN DWORD cbAttrBlob,
                                                       IN BOOL fNoCache,
                                                       OUT OBJECTREF *pObj,
                                                       OUT DWORD *pdwIndex,
                                                       IN DWORD dwAction);

    static void LoadPermissionRequestsFromAssembly(IN IMDInternalImport *pImport,
                                                   OUT OBJECTREF*   pReqdPermissions,
                                                   OUT OBJECTREF*   pOptPermissions,
                                                   OUT OBJECTREF*   pDenyPermissions);

    // Insert a decoded permission set into the cache. Duplicates are discarded.
    static void InsertPermissionSet(IN PBYTE pbPset,
                                    IN DWORD cbPset,
                                    IN OBJECTREF orPset,
                                    OUT DWORD *pdwSetIndex);

    static BOOL TrustMeIAmSafe(void *pLock) 
    {
        LEAF_CONTRACT;
        return TRUE;
    }

    static Assembly* LoadAssemblyFromToken(IMetaDataAssemblyImport *pImport, mdAssemblyRef tkAssemblyRef);
    static Assembly* LoadAssemblyFromNameString(__in_z WCHAR* pAssemblyName);
    static HRESULT AttributeSetToManaged(OBJECTREF* /*OUT*/obj, CORSEC_ATTRSET* pAttrSet, OBJECTREF* pThrowable, DWORD* pdwErrorIndex, bool bLazy);
    static HRESULT SetAttrFieldsAndProperties(CORSEC_ATTRIBUTE *pAttr, OBJECTREF* pThrowable, MethodTable* pMT, OBJECTREF* pObj);
    static HRESULT SetAttrField(BYTE** ppbBuffer, DWORD* pcbBuffer, DWORD dwType, TypeHandle hEnum, MethodTable* pMT, __in_z LPSTR szName, OBJECTREF* pObj, DWORD dwLength, BYTE* pbName, DWORD cbName, CorElementType eEnumType);
    static HRESULT SetAttrProperty(BYTE** ppbBuffer, DWORD* pcbBuffer, MethodTable* pMT, DWORD dwType, __in_z LPSTR szName, OBJECTREF* pObj, DWORD dwLength, BYTE* pbName, DWORD cbName, CorElementType eEnumType);
    static void AttrArrayToPermissionSet(OBJECTREF* attrArray, bool fSerialize, DWORD attrCount, BYTE **ppbOutput, DWORD *pcbOutput, BYTE **ppbNonCasOutput, DWORD *pcbNonCasOutput, OBJECTREF* pPermSet);
    static void AttrSetBlobToPermissionSets(IN BYTE* pbRawPermissions, IN DWORD cbRawPermissions, OUT OBJECTREF* pObj, IN DWORD dwAction);

    static void CreatePermissionSetFromXml(
                                           IN PBYTE pbRawPermissions,
                                           IN DWORD cbRawPermissions,
                                           OUT OBJECTREF* pPset,
                                           OUT OBJECTREF* pEncoding,
                                           IN BOOL fNoCache,
                                           OUT DWORD* pdwSetIndex);
    static void CreatePermissionSetFromAttrArray(
                                           IN PBYTE pbRawPermissions,
                                           IN DWORD cbRawPermissions,
                                           OUT OBJECTREF* pObj,
                                           IN BOOL fNoCache,
                                           OUT DWORD* pdwIndex,
                                           IN DWORD dwAction);

    static void XmlToPermissionSet(PBYTE pbXmlBlob,
                                    DWORD cbXmlBlob,
                                    OBJECTREF* pPermSet,
                                    OBJECTREF* pEncoding,
                                    PBYTE pbNonCasXmlBlob,
                                    DWORD cbNonCasXmlBlob,
                                    OBJECTREF* pNonCasPermSet,
                                    OBJECTREF* pNonCasEncoding);


    // Check to see if there's already an equivalent blob in the cache.  If there is, get the index and special flags
    static bool CheckCache(IN PBYTE pbAttrBlob, IN DWORD cbAttrBlob, OUT DWORD *pdwIndex);
};

#define LAZY_DECL_SEC_FLAG '.'

#endif // __SECURITYATTRIBUTES_H__

