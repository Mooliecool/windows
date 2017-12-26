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
#include "common.h"

#include "security.h"
#include "field.h"
#include "comcallablewrapper.h"
#include "typeparse.h"
#include "appdomain.inl"

HRESULT BlobToAttributeSet(BYTE* pBuffer, CORSEC_ATTRSET* pAttrSet, DWORD dwAction);

CQuickArrayStatic<PsetCacheEntry> SecurityAttributes::s_rCachedPsets;
EEPsetHashTable SecurityAttributes::s_sCachedPsetsHash;
SimpleRWLock *SecurityAttributes::s_prCachedPsetsLock = NULL;

VOID SecurityAttributes::Init()
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;

    // initialize cache of permission sets and hash table for looking them up
    s_prCachedPsetsLock = new SimpleRWLock(COOPERATIVE_OR_PREEMPTIVE, LOCK_TYPE_DEFAULT);
    s_rCachedPsets.Init();
    if (!s_sCachedPsetsHash.Init(19, &g_lockTrustMeIAmThreadSafe))
        ThrowOutOfMemory();
}

VOID SecurityAttributes::Shutdown()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;

    s_sCachedPsetsHash.Destroy();
    for (size_t i = 0; i < s_rCachedPsets.Size(); i++)
        delete [] s_rCachedPsets[i].m_pbPset;
    s_rCachedPsets.Destroy();
    if (s_prCachedPsetsLock) delete s_prCachedPsetsLock;
}

HRESULT SecurityAttributes::MapToHR(OBJECTREF refException)
{
    HRESULT hr = E_FAIL;

    CANNOTTHROWCOMPLUSEXCEPTION();

    // Temporary macro to ignore exceptions.  (However, will grab the HRESULT from
    //  any exception that happens while executing.)
    BEGIN_EXCEPTION_GLUE(&hr, NULL)
    {
        // Get the HRESULT from the exception.  Will return E_FAIL if it doesn't derive
        //  from System.Exception.
        hr = GetExceptionHResult(refException);
    }
    END_EXCEPTION_GLUE

    return hr;
}

OBJECTREF SecurityAttributes::CreatePermissionSet(BOOL fTrusted)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    OBJECTREF pPermSet = NULL;
    GCPROTECT_BEGIN(pPermSet);

    static MethodTable* pMT = NULL;
    if (pMT == NULL)
        pMT = g_Mscorlib.GetClass(CLASS__PERMISSION_SET);
    pPermSet = (OBJECTREF) AllocateObject(pMT);

    ARG_SLOT fStatus = (fTrusted) ? 1 : 0;

    MethodDescCallSite ctor(METHOD__PERMISSION_SET__CTOR);

    ARG_SLOT arg[2] = { 
        ObjToArgSlot(pPermSet),
        fStatus
    };
    ctor.Call(arg);
    
    GCPROTECT_END();

    return pPermSet;
}

void SecurityAttributes::XmlToPermissionSet(PBYTE pbXmlBlob,
                                DWORD cbXmlBlob,
                                OBJECTREF* pPermSet,
                                OBJECTREF* pEncoding,
                                PBYTE pbNonCasXmlBlob,
                                DWORD cbNonCasXmlBlob,
                                OBJECTREF* pNonCasPermSet,
                                OBJECTREF* pNonCasEncoding)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(IsProtectedByGCFrame (pPermSet));
        PRECONDITION(IsProtectedByGCFrame (pEncoding));
        PRECONDITION(IsProtectedByGCFrame (pNonCasPermSet));
        PRECONDITION(IsProtectedByGCFrame (pNonCasEncoding));
    } CONTRACTL_END;

    // Get Host Protection Flags
    EApiCategories eProtectedCategories = GetHostProtectionManager()->GetProtectedCategories();

    MethodDescCallSite decodeXML(METHOD__PERMISSION_SET__DECODE_XML, pPermSet); // can trigger GC

    // Deserialize the CAS PermissionSet
    if(pbXmlBlob && cbXmlBlob > 0)
    {
        _ASSERTE(*pbXmlBlob != LAZY_DECL_SEC_FLAG);

        // Create a new (empty) permission set.
        *pPermSet = SecurityAttributes::CreatePermissionSet(FALSE);

        // Buffer in managed space.
        SecurityAttributes::CopyEncodingToByteArray(pbXmlBlob, cbXmlBlob, pEncoding);

        ARG_SLOT args[] = { 
            ObjToArgSlot(*pPermSet),
            ObjToArgSlot(*pEncoding),
            (ARG_SLOT)eProtectedCategories,
            (ARG_SLOT)0,
        };

        // Deserialize into a managed object.
        BOOL success = FALSE;
        EX_TRY
        {
            // Elevate thread’s allowed loading level.  This can cause load failures if assemblies loaded from this point on require
            // any assemblies currently being loaded.
            OVERRIDE_LOAD_LEVEL_LIMIT(FILE_ACTIVE);
            success = decodeXML.Call_RetBool(args);
        }
        EX_SWALLOW_NONTERMINAL

        if (!success)
            COMPlusThrow(kSecurityException, IDS_ENCODEDPERMSET_DECODEFAILURE);
    }

    // Deserialize the non-CAS PermissionSet
    if(pbNonCasXmlBlob && cbNonCasXmlBlob > 0)
    {
        _ASSERTE(*pbNonCasXmlBlob != LAZY_DECL_SEC_FLAG);

        // Create a new (empty) permission set.
        *pNonCasPermSet = SecurityAttributes::CreatePermissionSet(FALSE);

        // Buffer in managed space.
        SecurityAttributes::CopyEncodingToByteArray(pbNonCasXmlBlob, cbNonCasXmlBlob, pNonCasEncoding);

        ARG_SLOT args[] = { 
            ObjToArgSlot(*pNonCasPermSet),
            ObjToArgSlot(*pNonCasEncoding),
            (ARG_SLOT)eProtectedCategories,
            (ARG_SLOT)0,
        };

        // Deserialize into a managed object.
        BOOL success = FALSE;
        EX_TRY
        {
            // Elevate thread’s allowed loading level.  This can cause load failures if assemblies loaded from this point on require
            // any assemblies currently being loaded.
            OVERRIDE_LOAD_LEVEL_LIMIT(FILE_ACTIVE);
            success = decodeXML.Call_RetBool(args);
        }
        EX_SWALLOW_NONTERMINAL

        if (!success)
            COMPlusThrow(kSecurityException, IDS_ENCODEDPERMSET_DECODEFAILURE);
    }
}


// Retrieves a previously loaded permission set by index (this will work
// even if the permission set was loaded in a different appdomain).
OBJECTREF SecurityAttributes::GetPermissionSet(DWORD dwIndex, DWORD dwAction)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    // Actual permission set objects are stored in handle tables held on each
    // unmanaged AppDomain object. These tables are lazily populated as accesses
    // are made.
    AppDomain                   *pDomain        = GetAppDomain();
    PsetHandleCache             &rCachedHandles = pDomain->m_pSecContext->m_rCachedPsets;
    size_t                      *pnTableSize    = &pDomain->m_pSecContext->m_nCachedPsetsSize;
    SimpleRWLock                *prGlobalLock   = s_prCachedPsetsLock;
    OBJECTHANDLE                nHandle;
    OBJECTREF                   orRet = NULL;
    PsetCacheEntry              *pPCE;
    PBYTE                       pbPset;
    DWORD                       cbPset;
    //
    // Check if we may need to expand the array.
    //
    if (dwIndex >= *pnTableSize)
    {
        nHandle = rCachedHandles.GrowAndGetHandle(dwIndex, pnTableSize, prGlobalLock, pDomain);
        if (!nHandle)
        {
            // Try again as someone else has grown the array list
            nHandle = (OBJECTHANDLE)rCachedHandles.Get(dwIndex);
        }
    }
    //
    // The array is large enough; get the handle at dwIndex
    //
    else
    {
        nHandle = (OBJECTHANDLE)rCachedHandles.Get(dwIndex);        
    }
    _ASSERTE(nHandle);

    if ((orRet = ObjectFromHandle( nHandle )) == NULL ) // Query the domain-wide cache
    {
        // No object allocated in this domain yet.  Decode in this appdomain and cache the result.
        // (We can't hold the lock over the call to BlobToPermissionSet since it will call into managed code.)
        {
            // Hold the read lock for s_rCachedPsets 
            // while we update the permission set
            SimpleReadLockHolder swlh(prGlobalLock);

            pPCE = &s_rCachedPsets[dwIndex];
            pbPset = pPCE->m_pbPset;
            cbPset = pPCE->m_cbPset;
        }

        SecurityAttributes::BlobToPermissionSet(pbPset,
                                              cbPset,
                                              TRUE, // fNoCache
                                              &orRet,
                                              NULL,
                                              dwAction); // pdwIndex

        StoreFirstObjectInHandle( nHandle, orRet );
    }

    return orRet;
}

// Locate the index of a permission set in the cache (returns false if the
// permission set has not yet been seen and decoded).
BOOL SecurityAttributes::LookupPermissionSet(IN PBYTE       pbPset,
                                         IN DWORD       cbPset,
                                         OUT DWORD     *pdwSetIndex)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;

    PsetCacheEntry sKey(pbPset, cbPset);
    HashDatum      dwIndex;

    // WARNING: note that we are doing a GetValue here without
    // holding the lock.  This means that we can get false failures
    // of this function.  If you call this function, you must handle
    // the false failure case appropriately (or you have to fix this
    // function to never false fail).

    if (s_sCachedPsetsHash.GetValue(&sKey, &dwIndex))
    {
        if (pdwSetIndex)
        {
            *pdwSetIndex = *((DWORD*) &dwIndex);
        }
        return TRUE;
    }
    else
        return FALSE;
}

// Insert a decoded permission set into the cache. Duplicates are discarded.
void SecurityAttributes::InsertPermissionSet(IN PBYTE       pbPset,
                                         IN DWORD       cbPset,
                                         IN OBJECTREF   orPset,
                                         OUT DWORD     *pdwSetIndex)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;

    SimpleRWLock                *prGlobalLock   = s_prCachedPsetsLock;
    PsetCacheEntry              sKey(pbPset, cbPset);
    size_t                      dwIndex;
    AppDomain                   *pDomain        = GetAppDomain();
    PsetHandleCache             &rCachedHandles = pDomain->m_pSecContext->m_rCachedPsets;
    size_t                      *pnTableSize    = &pDomain->m_pSecContext->m_nCachedPsetsSize;
    OBJECTHANDLE                nHandle = NULL;
    {
        SimpleWriteLockHolder swlh(prGlobalLock);

        // Check for duplicates.
        if (s_sCachedPsetsHash.GetValue(&sKey, (HashDatum*)&dwIndex)) {
            if (pdwSetIndex)
                *pdwSetIndex = (DWORD)dwIndex;
            return;
        }

        MEMORY_REPORT_CONTEXT_SCOPE("PermissionSet");

        // Buffer permission set blob (it might go away if the metadata scope it
        // came from is closed).
        NewArrayHolder<BYTE> pbPsetCopy(new BYTE[cbPset]);
        memcpy(pbPsetCopy, pbPset, cbPset);

        // Add another entry to the array of cache entries (this gives us an index).
        dwIndex = s_rCachedPsets.Size();
        s_rCachedPsets.ReSizeThrows(dwIndex + 1);
        PsetCacheEntry *pPCE = &s_rCachedPsets[dwIndex];

        pPCE->m_pbPset = pbPsetCopy;
        pPCE->m_cbPset = cbPset;
        pPCE->m_dwIndex = (DWORD)dwIndex;
        pPCE->m_eCanUnrestrictedOverride = CUO_DontKnow;

        // Place the new entry into the hash.
        s_sCachedPsetsHash.InsertValue(pPCE, (HashDatum)dwIndex);
        pbPsetCopy.SuppressRelease();

        if (pdwSetIndex)
            *pdwSetIndex = (DWORD)dwIndex;
        if (orPset == NULL)
            return; // There's no object to cache yet--the caller just wanted the index

        if (dwIndex >= *pnTableSize)
        {
            nHandle = rCachedHandles.GrowAndGetHandleHaveLock(dwIndex, pnTableSize, pDomain);
        } 
        if (!nHandle)
        {
            nHandle = (OBJECTHANDLE)rCachedHandles.Get((DWORD)dwIndex);
            _ASSERTE(nHandle);
        }
        _ASSERTE(ObjectFromHandle(nHandle) == NULL);

        StoreFirstObjectInHandle(nHandle, orPset);        
    }
}

DWORD SecurityAttributes::MergePermissionSets(IN DWORD dwSetIndex1, IN DWORD dwSetIndex2, IN bool fIntersect, DWORD dwAction)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    DWORD dwMergedSetIndex;
    struct _gc {
        OBJECTREF orSet1;
        OBJECTREF orSet2;
        OBJECTREF orMergedSet;
    } gc;
    memset(&gc, '\0', sizeof(gc));
    GCPROTECT_BEGIN(gc);
    {
        // Union or Intersect the two PermissionSets
        gc.orSet1 = GetPermissionSet(dwSetIndex1, dwAction);
        if(gc.orSet1 == NULL)
            dwMergedSetIndex = fIntersect ? dwSetIndex1 : dwSetIndex2;
        else
        {
            gc.orSet2 = GetPermissionSet(dwSetIndex2, dwAction);
            if(gc.orSet2 == NULL)
                dwMergedSetIndex = fIntersect ? dwSetIndex2 : dwSetIndex1;
            else
            {
                BinderMethodID methID = (fIntersect ? METHOD__PERMISSION_SET__INTERSECT : METHOD__PERMISSION_SET__UNION);
                MethodDescCallSite mergeMethod(methID, &gc.orSet1);

                ARG_SLOT args[2] = {
                    ObjToArgSlot(gc.orSet1),
                    ObjToArgSlot(gc.orSet2),
                };
                gc.orMergedSet = mergeMethod.Call_RetOBJECTREF(args);

                if(gc.orMergedSet == NULL)
                    gc.orMergedSet = CreatePermissionSet(false);

                // Convert to XML blob
                PBYTE pbData;
                DWORD cbData;
                EncodePermissionSet(&gc.orMergedSet, &pbData, &cbData);

                // Store XML blob and obtain an index to reference it
                BlobToPermissionSet(pbData, cbData, FALSE, NULL, &dwMergedSetIndex, dwAction);
            }
        }
    }
    GCPROTECT_END();

    return dwMergedSetIndex;
}

void SecurityAttributes::CopyEncodingToByteArray(IN PBYTE   pbData,
                                                IN DWORD   cbData,
                                                OUT OBJECTREF* pArray)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    U1ARRAYREF pObj;
    _ASSERTE(pArray);

    pObj = (U1ARRAYREF)AllocatePrimitiveArray(ELEMENT_TYPE_U1,cbData);          
    memcpyNoGCRefs(pObj->m_Array, pbData, cbData);
    *pArray = (OBJECTREF) pObj;        
}

void SecurityAttributes::CopyByteArrayToEncoding(IN U1ARRAYREF* pArray,
                                             OUT PBYTE*   ppbData,
                                             OUT DWORD*   pcbData)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pArray));
        PRECONDITION(CheckPointer(ppbData));
        PRECONDITION(CheckPointer(pcbData));
        PRECONDITION(*pArray != NULL);
    } CONTRACTL_END;

    DWORD size = (DWORD) (*pArray)->GetNumComponents();
    *ppbData = new BYTE[size];
    *pcbData = size;
        
    CopyMemory(*ppbData, (*pArray)->GetDirectPointerToNonObjectElements(), size);
}


void SecurityAttributes::EncodePermissionSet(IN OBJECTREF* pRef,
                                         OUT PBYTE* ppbData,
                                         OUT DWORD* pcbData)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(IsProtectedByGCFrame (pRef));
    } CONTRACTL_END;

    MethodDescCallSite encodeXML(METHOD__PERMISSION_SET__ENCODE_XML);

    // Encode up the result
    ARG_SLOT args1[1];
    args1[0] = ObjToArgSlot(*pRef);
    OBJECTREF pByteArray = NULL;
    pByteArray = encodeXML.Call_RetOBJECTREF(args1);
        
    SecurityAttributes::CopyByteArrayToEncoding((U1ARRAYREF*) &pByteArray,
                                            ppbData,
                                            pcbData);
}

// Append a string to a buffer, enlarging buffer as needed.
BOOL AppendToStringBuffer(__inout_z LPSTR *pszBuffer, DWORD *pdwBuffer, LPCSTR szString)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;

    DWORD   cbString = (DWORD)strlen(szString);
    DWORD   cbBuffer = *pszBuffer ? (DWORD)strlen(*pszBuffer) : 0;

    // Expand buffer as needed.
    if ((*pdwBuffer - cbBuffer) < (cbString + 1)) {
        DWORD   cbNewBuffer;
        LPSTR   szNewBuffer;

        cbNewBuffer = cbBuffer + cbString + 1 + 100;
        szNewBuffer = new (nothrow) CHAR[cbNewBuffer];
        if (szNewBuffer == NULL)
            return FALSE;
        memcpy(szNewBuffer, *pszBuffer, cbBuffer);
        *pszBuffer = szNewBuffer;
        *pdwBuffer = cbNewBuffer;
    }

    // Append new string.
    memcpy(*pszBuffer + cbBuffer, szString, cbString);
    (*pszBuffer)[cbBuffer + cbString] = '\0';

    return TRUE;
}

static void SetupRestrictSecAttributes()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    EX_TRY
    {
        MethodDescCallSite setupSecurity(METHOD__PERMISSION_SET__SETUP_SECURITY);
           
        setupSecurity.Call(NULL);
    }
    EX_CATCH
    {
        // There is a possibility that we've already set the appdomain policy
        // level for this process.  In that case we'll get a policy exception
        // that we are free to ignore.
        OBJECTREF pThrowable = GETTHROWABLE();
        DefineFullyQualifiedNameForClassOnStack();
        LPCUTF8 szClass = GetFullyQualifiedNameForClass(pThrowable->GetClass());
        if (strcmp(g_PolicyExceptionClassName, szClass) != 0)
            COMPlusThrow(pThrowable);
    }
    EX_END_CATCH(RethrowTerminalExceptions)
}


Assembly* SecurityAttributes::LoadAssemblyFromToken(IMetaDataAssemblyImport *pImport, mdAssemblyRef tkAssemblyRef)
{
    _ASSERTE(TypeFromToken(tkAssemblyRef) == mdtAssemblyRef);

    // Find all the details needed to name an assembly for loading.
    ASSEMBLYMETADATA            sContext;
    BYTE                       *pbPublicKeyOrToken;
    DWORD                       cbPublicKeyOrToken;
    DWORD                       dwFlags;
    LPWSTR                      wszName;
    DWORD                       cchName;

    // Initialize ASSEMBLYMETADATA structure.
    ZeroMemory(&sContext, sizeof(ASSEMBLYMETADATA));

    // Retrieve size of assembly name.
    HRESULT hr = pImport->GetAssemblyRefProps(tkAssemblyRef,          // [IN] The AssemblyRef for which to get the properties.
                                        NULL,                 // [OUT] Pointer to the public key or token.
                                        NULL,                 // [OUT] Count of bytes in the public key or token.
                                        NULL,                 // [OUT] Buffer to fill with name.
                                        NULL,                 // [IN] Size of buffer in wide chars.
                                        &cchName,             // [OUT] Actual # of wide chars in name.
                                        &sContext,            // [OUT] Assembly MetaData.
                                        NULL,                 // [OUT] Hash blob.
                                        NULL,                 // [OUT] Count of bytes in the hash blob.
                                        NULL);                // [OUT] Flags.
    _ASSERTE(SUCCEEDED(hr));

    // Allocate the necessary buffers.
    wszName = (LPWSTR)_alloca(cchName * sizeof(WCHAR));
    sContext.szLocale = (LPWSTR)_alloca(sContext.cbLocale * sizeof(WCHAR));
    sContext.rProcessor = (DWORD *)_alloca(sContext.ulProcessor * sizeof(DWORD));
    sContext.rOS = (OSINFO *)_alloca(sContext.ulOS * sizeof(OSINFO));

    // Get the assembly name and rest of naming properties.
    hr = pImport->GetAssemblyRefProps(tkAssemblyRef,
                                        (const void **)&pbPublicKeyOrToken,
                                        &cbPublicKeyOrToken,
                                        wszName,
                                        cchName,
                                        &cchName,
                                        &sContext,
                                        NULL,
                                        NULL,
                                        &dwFlags);
    _ASSERTE(SUCCEEDED(hr));

    // We've got the details of the assembly, just need to load it.

    // Convert assembly name to UTF8.
    MAKE_UTF8PTR_FROMWIDE(uszAssemblyName, wszName);

    AssemblyMetaDataInternal internalContext;

    // Initialize the structure.
    ZeroMemory(&internalContext, sizeof(AssemblyMetaDataInternal));

    internalContext.usMajorVersion = sContext.usMajorVersion;
    internalContext.usMinorVersion = sContext.usMinorVersion;
    internalContext.usBuildNumber = sContext.usBuildNumber;
    internalContext.usRevisionNumber = sContext.usRevisionNumber;
    internalContext.rProcessor = sContext.rProcessor;
    internalContext.ulProcessor = sContext.ulProcessor;
    internalContext.rOS = sContext.rOS;
    internalContext.ulOS = sContext.ulOS;
    if(sContext.cbLocale)
    {
        MAKE_UTF8PTR_FROMWIDE(pLocale, sContext.szLocale);
        internalContext.szLocale = pLocale;
    }
    else
    {
        internalContext.szLocale = "";
    }

    Assembly* pAssembly = NULL;
    {
        // Elevate thread’s allowed loading level.  This can cause load failures if assemblies loaded from this point on require
        // any assemblies currently being loaded.
        OVERRIDE_LOAD_LEVEL_LIMIT(FILE_ACTIVE);
        pAssembly = AssemblySpec::LoadAssembly(uszAssemblyName, 
                                               &internalContext,
                                               pbPublicKeyOrToken,
                                               cbPublicKeyOrToken,
                                               dwFlags);
    }


    return pAssembly;
}

TypeHandle FindSecurityAttributeHandle(LPCWSTR wszTypeName)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    TypeHandle hType;
    MethodDescCallSite findSecurityAttributeTypeHandle(METHOD__SECURITY_ATTRIBUTE__FIND_SECURITY_ATTRIBUTE_TYPE_HANDLE);

    struct _gc {
        STRINGREF str;
    } gc;
    
    ZeroMemory(&gc, sizeof(gc));
    
    GCPROTECT_BEGIN(gc);
    gc.str = COMString::NewString(wszTypeName);
    ARG_SLOT arg[1] = {
        ObjToArgSlot(gc.str)
    };

    TypeHandle th = TypeHandle::FromPtr(findSecurityAttributeTypeHandle.Call_RetLPVOID(arg));
    hType = th;
    GCPROTECT_END();

    return hType;
}

HRESULT SecurityAttributes::AttributeSetToManaged(OBJECTREF* /*OUT*/obj, CORSEC_ATTRSET* pAttrSet, OBJECTREF* pThrowable, DWORD* pdwErrorIndex, bool bLazy)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        // Assumption: if the first obj is protected, the whole array is protected
        if (pAttrSet->dwAttrCount > 0) {PRECONDITION(IsProtectedByGCFrame (obj));}
    } CONTRACTL_END;

    HRESULT hr = S_OK;
    DWORD i;
    TypeHandle hType;
    MethodTable *pMT = NULL;
    MethodDesc *pMD = NULL;

    // Elevate the allowed loading level
    // Elevate thread’s allowed loading level.  This can cause load failures if assemblies loaded from this point on require any assemblies currently being loaded.  
    OVERRIDE_LOAD_LEVEL_LIMIT(FILE_ACTIVE); 

    for (i = 0; i < pAttrSet->dwAttrCount; i++)
    {
        CORSEC_ATTRIBUTE *pAttr = &pAttrSet->pAttrs[i];

        if (pdwErrorIndex)
            *pdwErrorIndex = pAttr->dwIndex;

        // Find the assembly that contains the security attribute class.
        _ASSERTE(pAttr->pName);
        Assembly *pAssembly;
        if(bLazy)
        {
            // Convert type name to Unicode
            MAKE_WIDEPTR_FROMUTF8(wszTypeName, pAttr->pName);


            {
                // Load the type
                {
                    DWORD error = (DWORD)-1;
                    TypeName typeName(wszTypeName, &error);
                    if (error == (DWORD)(-1) && !(typeName.GetAssembly()->IsEmpty()))
                    {
                        hType = TypeName::GetTypeFromAsmQualifiedName(wszTypeName, FALSE);
                    }
                    else
                    {
                        hType = TypeName::GetTypeFromAssembly(wszTypeName, SystemDomain::SystemAssembly());
                    }
                }

                if (hType.IsNull() || hType.GetMethodTable() == NULL)
                hType = FindSecurityAttributeHandle(wszTypeName);
            }
        }
        else
        {
            if (!IsNilToken(pAttr->tkAssemblyRef) && TypeFromToken(pAttr->tkAssemblyRef) == mdtAssemblyRef)
            {
                // Load from AssemblyRef token stored in the CORSEC_ATTRSET
                pAssembly = LoadAssemblyFromToken(pAttrSet->pImport, pAttr->tkAssemblyRef);
            }
            else
            {
                // Load from MSCORLIB.
                pAssembly = SystemDomain::SystemAssembly();
            }
            _ASSERTE(pAssembly && "Failed to find assembly with declarative attribute");
            hType = ClassLoader::LoadTypeByNameNoThrow(pAssembly, NULL, pAttr->pName, pThrowable);
        }

        // Load the security attribute class.
        if (hType.IsNull() || (pMT = hType.GetMethodTable()) == NULL)
        {
            MAKE_WIDEPTR_FROMUTF8(wszTemp, pAttr->pName);
            SString sMessage;
            GetExceptionMessage(*pThrowable, sMessage);
            if (!sMessage.IsEmpty())
                hr = VMPostError(CORSECATTR_E_TYPE_LOAD_FAILED_EX, wszTemp, sMessage.GetUnicode());
            else
                hr = VMPostError(CORSECATTR_E_TYPE_LOAD_FAILED, wszTemp);
            return hr;
        }

        // Make sure it's not abstract.
        if (pMT->IsAbstract())
            return VMPostError(CORSECATTR_E_ABSTRACT);

#ifdef _DEBUG
        // Make sure it's really a security attribute class
        /*{
            MethodTable *pParentMT = pMT->GetParentMethodTable();
            CHAR       *szClass;
            DefineFullyQualifiedNameForClassOnStack();
            while (pParentMT) {
                szClass = GetFullyQualifiedNameForClass(pParentMT->GetClass());
                if (stricmpUTF8(szClass, COR_BASE_SECURITY_ATTRIBUTE_CLASS_ANSI) == 0)
                    break;
                pParentMT = pParentMT->GetParentMethodTable();
            }
            _ASSERTE(pParentMT && "Security attribute not derived from COR_BASE_SECURITY_ATTRIBUTE_CLASS");
        }*/
#endif

        // Instantiate an instance.
        obj[i] = pMT->Allocate();

        PCCOR_SIGNATURE pBinarySig;
        ULONG       cbBinarySigLength;

        gsig_IM_SecurityAction_RetVoid.GetBinarySig(&pBinarySig, &cbBinarySigLength);

        // Find and call the constructor.
        pMD = pMT->GetClass()->FindConstructor(pBinarySig, cbBinarySigLength,
                                        gsig_IM_SecurityAction_RetVoid.GetModule());
        if (pMD == NULL)
            return VMPostError(CORSECATTR_E_MISSING_CONSTRUCTOR);
        MethodDescCallSite ctor(pMD);
        ARG_SLOT args[] = {
            ObjToArgSlot(obj[i]),
            (ARG_SLOT)pAttrSet->dwAction
        };
        ctor.Call(args);

        // Set the attributes and properties
        hr = SetAttrFieldsAndProperties(pAttr, pThrowable, pMT, &obj[i]);
        if (FAILED(hr))
            return hr;
    }

    return hr;
}


HRESULT SecurityAttributes::SetAttrFieldsAndProperties(CORSEC_ATTRIBUTE *pAttr, OBJECTREF* pThrowable, MethodTable* pMT, OBJECTREF* pObj)
{
    // Setup fields and properties on the object, as specified by the
    // serialized data passed to us.
    BYTE   *pbBuffer = pAttr->pbValues;
    DWORD   cbBuffer = pAttr->cbValues;
    DWORD j;
    HRESULT hr = S_OK;

    BEGIN_EXCEPTION_GLUE(&hr, pThrowable)
    {
        for (j = 0; j < pAttr->wValues; j++)
        {
            DWORD       dwType = 0;
            BOOL        bIsField = FALSE;
            BYTE       *pbName;
            DWORD       cbName;
            DWORD       dwLength;
            NewArrayHolder<CHAR> szName(NULL);
            TypeHandle      hEnum;
            CorElementType  eEnumType = ELEMENT_TYPE_END;

            // Check we've got at least the field/property specifier and the
            // type code.
            if(cbBuffer < (sizeof(BYTE) + sizeof(BYTE)))
            {
                hr = VMPostError(CORSECATTR_E_TRUNCATED);
                goto Error;
            }

            // Grab the field/property specifier.
            bIsField = *(BYTE*)pbBuffer == SERIALIZATION_TYPE_FIELD;
            if(!bIsField && *(BYTE*)pbBuffer != SERIALIZATION_TYPE_PROPERTY)
            {
                hr = VMPostError(CORSECATTR_E_TRUNCATED);
                goto Error;
            }
            pbBuffer += sizeof(BYTE);
            cbBuffer -= sizeof(BYTE);

            // Grab the value type.
            dwType = *(BYTE*)pbBuffer;
            pbBuffer += sizeof(BYTE);
            cbBuffer -= sizeof(BYTE);

            // If it's a type that needs further specification, get that information
            switch (dwType)
            {
            case SERIALIZATION_TYPE_ENUM:
                // Immediately after the enum type token is the fully
                // qualified name of the value type used to represent
                // the enum.
                pbName = (BYTE*)CPackedLen::GetData((const void *)pbBuffer, &cbName);
                dwLength = CPackedLen::Size(cbName) + cbName;
                    if(cbBuffer < dwLength)
                    {
                        hr = VMPostError(CORSECATTR_E_TRUNCATED);
                        goto Error;
                    }
                pbBuffer += dwLength;
                cbBuffer -= dwLength;

                // Buffer the name and nul terminate it.
                szName = new (nothrow) CHAR[cbName + 1];
                    if (szName == NULL)
                    {
                    hr = E_OUTOFMEMORY;
                    goto Error;
                }
                memcpy(szName, pbName, cbName);
                szName[cbName] = '\0';

                // Lookup the type (possibly loading an assembly containing
                // the type).
                hEnum = TypeName::GetTypeUsingCASearchRules(szName, NULL);
 
                //If we couldn't find the type, post an error
                if (hEnum.IsNull())
                {
                    MAKE_WIDEPTR_FROMUTF8(wszTemp, szName);
                    SString sMessage;
                    GetExceptionMessage(*pThrowable, sMessage);
                    if (!sMessage.IsEmpty())
                        hr = VMPostError(CORSECATTR_E_TYPE_LOAD_FAILED_EX, wszTemp, sMessage.GetUnicode());
                    else
                        hr = VMPostError(CORSECATTR_E_TYPE_LOAD_FAILED, wszTemp);
                    goto Error;
                }

                // Calculate the underlying primitive type of the
                // enumeration.
                eEnumType = hEnum.GetInternalCorElementType();
                break;
            case SERIALIZATION_TYPE_SZARRAY:
            case SERIALIZATION_TYPE_TYPE:
                // Can't deal with these yet.
                hr = VMPostError(CORSECATTR_E_UNSUPPORTED_TYPE);
                goto Error;
            }

            // Grab the field/property name and length.
            pbName = (BYTE*)CPackedLen::GetData((const void *)pbBuffer, &cbName);
            dwLength = CPackedLen::Size(cbName) + cbName;
            if(cbBuffer < dwLength)
            {
                hr = VMPostError(CORSECATTR_E_TRUNCATED);
                goto Error;
            }
            pbBuffer += dwLength;
            cbBuffer -= dwLength;

            // Buffer the name and null terminate it.
            szName = new (nothrow) CHAR[cbName + 1];
            if (szName == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto Error;
            }
            memcpy(szName, pbName, cbName);
            szName[cbName] = '\0';

            // Set the field or property
            if (bIsField)
                hr = SetAttrField(&pbBuffer, &cbBuffer, dwType, hEnum, pMT, szName, pObj, dwLength, pbName, cbName, eEnumType);
            else
                hr = SetAttrProperty(&pbBuffer, &cbBuffer, pMT, dwType, szName, pObj, dwLength, pbName, cbName, eEnumType);
        }
    }
Error:;
    END_EXCEPTION_GLUE
    return hr;
}

HRESULT SecurityAttributes::SetAttrField(BYTE** ppbBuffer, DWORD* pcbBuffer, DWORD dwType, TypeHandle hEnum, MethodTable* pMT, __in_z LPSTR szName, OBJECTREF* pObj, DWORD dwLength, BYTE* pbName, DWORD cbName, CorElementType eEnumType)
{
    DWORD cbSig = 0;
    NewArrayHolder<BYTE> pbSig(new (nothrow) BYTE[128]);
    if (pbSig == NULL)
        return E_OUTOFMEMORY;

    // Build the field signature.
    cbSig += CorSigCompressData((ULONG)IMAGE_CEE_CS_CALLCONV_FIELD, &pbSig[cbSig]);
    switch (dwType)
    {
    case SERIALIZATION_TYPE_BOOLEAN:
    case SERIALIZATION_TYPE_I1:
    case SERIALIZATION_TYPE_I2:
    case SERIALIZATION_TYPE_I4:
    case SERIALIZATION_TYPE_I8:
    case SERIALIZATION_TYPE_U1:
    case SERIALIZATION_TYPE_U2:
    case SERIALIZATION_TYPE_U4:
    case SERIALIZATION_TYPE_U8:
    case SERIALIZATION_TYPE_R4:
    case SERIALIZATION_TYPE_R8:
    case SERIALIZATION_TYPE_CHAR:
        C_ASSERT(SERIALIZATION_TYPE_BOOLEAN == (CorSerializationType)ELEMENT_TYPE_BOOLEAN);
        C_ASSERT(SERIALIZATION_TYPE_I1 == (CorSerializationType)ELEMENT_TYPE_I1);
        C_ASSERT(SERIALIZATION_TYPE_I2 == (CorSerializationType)ELEMENT_TYPE_I2);
        C_ASSERT(SERIALIZATION_TYPE_I4 == (CorSerializationType)ELEMENT_TYPE_I4);
        C_ASSERT(SERIALIZATION_TYPE_I8 == (CorSerializationType)ELEMENT_TYPE_I8);
        C_ASSERT(SERIALIZATION_TYPE_U1 == (CorSerializationType)ELEMENT_TYPE_U1);
        C_ASSERT(SERIALIZATION_TYPE_U2 == (CorSerializationType)ELEMENT_TYPE_U2);
        C_ASSERT(SERIALIZATION_TYPE_U4 == (CorSerializationType)ELEMENT_TYPE_U4);
        C_ASSERT(SERIALIZATION_TYPE_U8 == (CorSerializationType)ELEMENT_TYPE_U8);
        C_ASSERT(SERIALIZATION_TYPE_R4 == (CorSerializationType)ELEMENT_TYPE_R4);
        C_ASSERT(SERIALIZATION_TYPE_R8 == (CorSerializationType)ELEMENT_TYPE_R8);
        C_ASSERT(SERIALIZATION_TYPE_CHAR == (CorSerializationType)ELEMENT_TYPE_CHAR);
            cbSig += CorSigCompressData(dwType, &pbSig[cbSig]);
        break;
    case SERIALIZATION_TYPE_STRING:
            cbSig += CorSigCompressData((ULONG)ELEMENT_TYPE_STRING, &pbSig[cbSig]);
        break;
    case SERIALIZATION_TYPE_ENUM:
        // To avoid problems when the field and enum are defined
        // in different scopes (we'd have to go hunting for
        // typerefs), we build a signature with a special type
        // (ELEMENT_TYPE_INTERNAL, which contains a TypeHandle).
        // This compares loaded types for indentity.
            cbSig += CorSigCompressData((ULONG)ELEMENT_TYPE_INTERNAL, &pbSig[cbSig]);
            cbSig += CorSigCompressPointer(hEnum.AsPtr(), &pbSig[cbSig]);
        break;
    default:
        return VMPostError(CORSECATTR_E_UNSUPPORTED_TYPE);
    }


    // Locate a field desc.
    FieldDesc* pFD = pMT->GetClass()->FindField(szName, (PCCOR_SIGNATURE)pbSig,
                                     cbSig, pMT->GetModule());
    if (pFD == NULL)
    {
        MAKE_WIDEPTR_FROMUTF8(wszTemp, szName);
        return VMPostError(CORSECATTR_E_NO_FIELD, wszTemp);
    }

    // Set the field value.
    LPSTR szString;
    switch (dwType)
    {
    case SERIALIZATION_TYPE_BOOLEAN:
    case SERIALIZATION_TYPE_I1:
    case SERIALIZATION_TYPE_U1:
            if(*pcbBuffer < sizeof(BYTE))
                return VMPostError(CORSECATTR_E_TRUNCATED);
        pFD->SetValue8(*pObj, *(BYTE*)(*ppbBuffer));
        (*ppbBuffer) += sizeof(BYTE);
        (*pcbBuffer) -= sizeof(BYTE);
        break;
    case SERIALIZATION_TYPE_CHAR:
    case SERIALIZATION_TYPE_I2:
    case SERIALIZATION_TYPE_U2:
            if(*pcbBuffer < sizeof(WORD))
                return VMPostError(CORSECATTR_E_TRUNCATED);
        pFD->SetValue16(*pObj, GET_UNALIGNED_VAL16(*ppbBuffer));
        (*ppbBuffer) += sizeof(WORD);
        (*pcbBuffer) -= sizeof(WORD);
        break;
    case SERIALIZATION_TYPE_I4:
    case SERIALIZATION_TYPE_U4:
    case SERIALIZATION_TYPE_R4:
            if(*pcbBuffer < sizeof(DWORD))
                return VMPostError(CORSECATTR_E_TRUNCATED);
        pFD->SetValue32(*pObj, GET_UNALIGNED_VAL32(*ppbBuffer));
        (*ppbBuffer) += sizeof(DWORD);
        (*pcbBuffer) -= sizeof(DWORD);
        break;
    case SERIALIZATION_TYPE_I8:
    case SERIALIZATION_TYPE_U8:
    case SERIALIZATION_TYPE_R8:
            if(*pcbBuffer < sizeof(INT64))
                return VMPostError(CORSECATTR_E_TRUNCATED);
        pFD->SetValue32(*pObj, GET_UNALIGNED_VAL64(*ppbBuffer));
        (*ppbBuffer) += sizeof(INT64);
        (*pcbBuffer) -= sizeof(INT64);
        break;
    case SERIALIZATION_TYPE_STRING:
        // Locate string data.
        // Special case 'null' (represented as a length byte of '0xFF').
        if (*(*ppbBuffer) == 0xFF) {
            szString = NULL;
            dwLength = sizeof(BYTE);
                if(*pcbBuffer < sizeof(BYTE))
                    return VMPostError(CORSECATTR_E_TRUNCATED);
        } else {
            pbName = (BYTE*)CPackedLen::GetData((const void *)(*ppbBuffer), &cbName);
            dwLength = CPackedLen::Size(cbName) + cbName;
            if(dwLength < cbName || dwLength < (DWORD)CPackedLen::Size(cbName) || *pcbBuffer < dwLength)
                return VMPostError(CORSECATTR_E_TRUNCATED);

            DWORD allocLen = cbName + 1;
            if (allocLen < cbName)
                COMPlusThrowSO();
            // Buffer and nul terminate it.
            szString = (LPSTR)_alloca(allocLen);
            memcpy(szString, pbName, cbName);
            szString[cbName] = '\0';

        }

        // Allocate and initialize a managed version of the string.
        {
            STRINGREF orString;
            if (szString)
            {
                orString = COMString::NewString(szString, cbName);
                if (orString == NULL)
                    COMPlusThrowOM();
            }
            else
                orString = NULL;

            pFD->SetRefValue(*pObj, (OBJECTREF)orString);
        }

        (*ppbBuffer) += dwLength;
        (*pcbBuffer) -= dwLength;
        break;
    case SERIALIZATION_TYPE_ENUM:
        // Get the underlying primitive type.
        switch (eEnumType)
        {
        case ELEMENT_TYPE_I1:
        case ELEMENT_TYPE_U1:
                    if(*pcbBuffer < sizeof(BYTE))
                        return VMPostError(CORSECATTR_E_TRUNCATED);
            pFD->SetValue8(*pObj, *(BYTE*)(*ppbBuffer));
            (*ppbBuffer) += sizeof(BYTE);
            (*pcbBuffer) -= sizeof(BYTE);
            break;
        case ELEMENT_TYPE_I2:
        case ELEMENT_TYPE_U2:
                    if(*pcbBuffer < sizeof(WORD))
                        return VMPostError(CORSECATTR_E_TRUNCATED);
            pFD->SetValue16(*pObj, GET_UNALIGNED_VAL16(*ppbBuffer));
            (*ppbBuffer) += sizeof(WORD);
            (*pcbBuffer) -= sizeof(WORD);
            break;
        case ELEMENT_TYPE_I4:
        case ELEMENT_TYPE_U4:
                    if(*pcbBuffer < sizeof(DWORD))
                        return VMPostError(CORSECATTR_E_TRUNCATED);
            pFD->SetValue32(*pObj, GET_UNALIGNED_VAL32(*ppbBuffer));
            (*ppbBuffer) += sizeof(DWORD);
            (*pcbBuffer) -= sizeof(DWORD);
            break;
        default:
            return VMPostError(CORSECATTR_E_UNSUPPORTED_ENUM_TYPE);
        }
        break;
    default:
        return VMPostError(CORSECATTR_E_UNSUPPORTED_TYPE);
    }
    return S_OK;
}

HRESULT SecurityAttributes::SetAttrProperty(BYTE** ppbBuffer, DWORD* pcbBuffer, MethodTable* pMT, DWORD dwType, __in_z LPSTR szName, OBJECTREF* pObj, DWORD dwLength, BYTE* pbName, DWORD cbName, CorElementType eEnumType)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(IsProtectedByGCFrame (pObj));
    } CONTRACTL_END;

    // Locate the property setter.
    MethodDesc* pMD = pMT->GetClass()->FindPropertyMethod(szName, PropertySet);
    if (pMD == NULL)
    {
        MAKE_WIDEPTR_FROMUTF8(wszTemp, szName);
        return VMPostError(CORSECATTR_E_NO_PROPERTY, wszTemp);
    }

    MethodDescCallSite propSet(pMD);

    // Build the argument list.
    ARG_SLOT args[2] = { NULL, NULL };
    LPSTR szString;
    NewHolder<BYTE> tmpLargeStringHolder (NULL);

    switch (dwType)
    {
    case SERIALIZATION_TYPE_BOOLEAN:
    case SERIALIZATION_TYPE_I1:
    case SERIALIZATION_TYPE_U1:
        if(*pcbBuffer < sizeof(BYTE))
            return VMPostError(CORSECATTR_E_TRUNCATED);
        args[1] = (ARG_SLOT)*(BYTE*)(*ppbBuffer);
        (*ppbBuffer) += sizeof(BYTE);
        (*pcbBuffer) -= sizeof(BYTE);
        break;
    case SERIALIZATION_TYPE_CHAR:
    case SERIALIZATION_TYPE_I2:
    case SERIALIZATION_TYPE_U2:
        if(*pcbBuffer < sizeof(WORD))
            return VMPostError(CORSECATTR_E_TRUNCATED);
        args[1] = (ARG_SLOT)GET_UNALIGNED_VAL16(*ppbBuffer);
        (*ppbBuffer) += sizeof(WORD);
        (*pcbBuffer) -= sizeof(WORD);
        break;
    case SERIALIZATION_TYPE_I4:
    case SERIALIZATION_TYPE_U4:
    case SERIALIZATION_TYPE_R4:
        if(*pcbBuffer < sizeof(DWORD))
            return VMPostError(CORSECATTR_E_TRUNCATED);
        args[1] = (ARG_SLOT)GET_UNALIGNED_VAL32(*ppbBuffer);
        (*ppbBuffer) += sizeof(DWORD);
        (*pcbBuffer) -= sizeof(DWORD);
        break;
    case SERIALIZATION_TYPE_I8:
    case SERIALIZATION_TYPE_U8:
    case SERIALIZATION_TYPE_R8:
        if(*pcbBuffer < sizeof(INT64))
            return VMPostError(CORSECATTR_E_TRUNCATED);
        args[1] = (ARG_SLOT)GET_UNALIGNED_VAL64(*ppbBuffer);
        (*ppbBuffer) += sizeof(INT64);
        (*pcbBuffer) -= sizeof(INT64);
        break;
    case SERIALIZATION_TYPE_STRING:
        
        // Locate string data.
        // Special case 'null' (represented as a length byte of '0xFF').
        if (*(*ppbBuffer) == 0xFF) {
            szString = NULL;
            dwLength = sizeof(BYTE);
            if(*pcbBuffer < sizeof(BYTE))
                return VMPostError(CORSECATTR_E_TRUNCATED);
        } else {

            DWORD relSize;

            pbName = (BYTE*)CPackedLen::GetData((const void *)(*ppbBuffer), &cbName);
            relSize = (DWORD)CPackedLen::Size(cbName);
            dwLength = relSize + cbName;
            if(dwLength < cbName || dwLength < relSize || *pcbBuffer < dwLength)                    
                return VMPostError(CORSECATTR_E_TRUNCATED);
            
            DWORD allocLen = cbName + 1;
            if (allocLen < cbName)
                COMPlusThrowSO();

            // 
            // For smaller size strings allocate from stack, use heap otherwise
            //

            if (relSize < 4) {
                // Buffer and nul terminate it.
                szString = (LPSTR)_alloca(allocLen);
            } else {
                tmpLargeStringHolder = new BYTE[allocLen];                
                szString = (LPSTR) ((BYTE*)tmpLargeStringHolder);
            }

            memcpy(szString, pbName, cbName);
            szString[cbName] = '\0';

        }

        // Allocate and initialize a managed version of the string.
        {
            STRINGREF orString;

            if (szString) {
                orString = COMString::NewString(szString, cbName);
                if (orString == NULL)
                    COMPlusThrowOM();
            } else
                orString = NULL;

            args[1] = ObjToArgSlot(orString);
        }

        (*ppbBuffer) += dwLength;
        (*pcbBuffer) -= dwLength;
        break;
    case SERIALIZATION_TYPE_ENUM:
        // Get the underlying primitive type.
        switch (eEnumType)
        {
        case ELEMENT_TYPE_I1:
        case ELEMENT_TYPE_U1:
                if(*pcbBuffer < sizeof(BYTE))
                    return VMPostError(CORSECATTR_E_TRUNCATED);
            args[1] = (ARG_SLOT)*(BYTE*)(*ppbBuffer);
            (*ppbBuffer) += sizeof(BYTE);
            (*pcbBuffer) -= sizeof(BYTE);
            break;
        case ELEMENT_TYPE_I2:
        case ELEMENT_TYPE_U2:
                if(*pcbBuffer < sizeof(WORD))
                    return VMPostError(CORSECATTR_E_TRUNCATED);
            args[1] = (ARG_SLOT)GET_UNALIGNED_VAL16(*ppbBuffer);
            (*ppbBuffer) += sizeof(WORD);
            (*pcbBuffer) -= sizeof(WORD);
            break;
        case ELEMENT_TYPE_I4:
        case ELEMENT_TYPE_U4:
                if(*pcbBuffer < sizeof(DWORD))
                    return VMPostError(CORSECATTR_E_TRUNCATED);
            args[1] = (ARG_SLOT)GET_UNALIGNED_VAL32(*ppbBuffer);
            (*ppbBuffer) += sizeof(DWORD);
            (*pcbBuffer) -= sizeof(DWORD);
            break;
        default:
            return VMPostError(CORSECATTR_E_UNSUPPORTED_ENUM_TYPE);
        }
        break;
    default:
        return VMPostError(CORSECATTR_E_UNSUPPORTED_TYPE);
    }


    // ! don't move this up, COMString::NewString
    // ! inside the switch causes a GC
    args[0] = ObjToArgSlot(*pObj);

    // Call the setter.
    propSet.Call(args);

    return S_OK;
}


void SecurityAttributes::AttrSetBlobToPermissionSets(
                                            IN BYTE* pbRawPermissions, 
                                            IN DWORD cbRawPermissions,
                                            OUT OBJECTREF* pObj,
                                            DWORD dwAction)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    _ASSERTE(pbRawPermissions);
    _ASSERTE(cbRawPermissions > 0);
    _ASSERTE(pbRawPermissions[0] == LAZY_DECL_SEC_FLAG);

    HRESULT hr = S_OK;
    CORSEC_ATTRSET pset;

    // Deserialize the CORSEC_ATTRSET
    hr = BlobToAttributeSet(pbRawPermissions, &pset, dwAction);
    if(FAILED(hr))
        COMPlusThrowHR(hr);

    OBJECTREF throwable = NULL;
    GCPROTECT_BEGIN(throwable);
    {
        // allocate and GC-protect an array of objectrefs to reference the permissions
        OBJECTREF* attrArray = (OBJECTREF*)_alloca(pset.dwAttrCount * sizeof(OBJECTREF));
        memset(attrArray, 0, pset.dwAttrCount * sizeof(OBJECTREF));
        GCPROTECT_ARRAY_BEGIN(*attrArray, pset.dwAttrCount);
        {
            // Convert to a managed array of attribute objects
            DWORD dwErrorIndex;
            hr = AttributeSetToManaged(/*OUT*/attrArray, &pset, &throwable, &dwErrorIndex, true);

            // Convert the array of attribute objects to a serialized PermissionSet
            if (SUCCEEDED(hr))
            {
                BYTE* pbXmlBlob = NULL;
                DWORD cbXmlBlob = 0;
                BYTE* pbNonCasXmlBlob = NULL;
                DWORD cbNonCasXmlBlob = 0;
                    AttrArrayToPermissionSet(attrArray, false, pset.dwAttrCount, &pbXmlBlob, &cbXmlBlob, &pbNonCasXmlBlob, &cbNonCasXmlBlob, pObj);
                _ASSERTE(pbXmlBlob == NULL && cbXmlBlob == 0 && pbNonCasXmlBlob == NULL && cbNonCasXmlBlob == 0);
            }
        }
        GCPROTECT_END();
    }
    GCPROTECT_END();

    if(FAILED(hr))
        COMPlusThrowHR(hr);
}

HRESULT SecurityAttributes::TranslateSecurityAttributesHelper(
                            CORSEC_ATTRSET    *pAttrSet,
                            BYTE          **ppbOutput,
                            DWORD          *pcbOutput,
                            BYTE          **ppbNonCasOutput,
                            DWORD          *pcbNonCasOutput,
                            DWORD          *pdwErrorIndex)
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    HRESULT                     hr = S_OK;
    OBJECTREF                  *attrArray;
    DWORD                       dwGlobalError = 0;

    BEGIN_EXCEPTION_GLUE(&hr, NULL)
    {
        if (pdwErrorIndex)
            dwGlobalError = *pdwErrorIndex;

        // Get into the context of the special compilation appdomain (which has an
        // AppBase set to the current directory).
        ComCallWrapper *pWrap = ComCallWrapper::GetWrapperFromIP(pAttrSet->pAppDomain);

        ENTER_DOMAIN_ID(pWrap->GetDomainID())
        {
            struct _gc {
                OBJECTREF throwable;
                OBJECTREF orPermSet;
            } gc;
            ZeroMemory(&gc, sizeof(gc));
            GCPROTECT_BEGIN(gc);
            {
                // we need to setup special security settings that we use during compilation
                SetupRestrictSecAttributes();

                // allocate and protect an array of objectrefs to reference the permissions
                attrArray = (OBJECTREF*)_alloca(pAttrSet->dwAttrCount * sizeof(OBJECTREF));
                memset(attrArray, 0, pAttrSet->dwAttrCount * sizeof(OBJECTREF));
                GCPROTECT_ARRAY_BEGIN(*attrArray, pAttrSet->dwAttrCount);
                {
                    // Convert to an array of attributes, and then serialize to XML
                    hr = AttributeSetToManaged(/*OUT*/attrArray, pAttrSet, &gc.throwable, pdwErrorIndex, false);
                    if (SUCCEEDED(hr))
                    {
                        if (pdwErrorIndex)
                            *pdwErrorIndex = dwGlobalError;

                        // Convert the array of attribute objects to a serialized PermissionSet or PermissionSetCollection
                            AttrArrayToPermissionSet(attrArray, true, pAttrSet->dwAttrCount, ppbOutput, pcbOutput, ppbNonCasOutput, pcbNonCasOutput, &gc.orPermSet);
                    }
                }
                GCPROTECT_END();
            }
            GCPROTECT_END(); // for throwable
        }
        END_DOMAIN_TRANSITION;   
        CONTRACT_VIOLATION(ThrowsViolation);    // SCAN seems to have trouble with the nested EH goin on in this method
    }
    END_EXCEPTION_GLUE

    return hr;
}

bool SecurityAttributes::IsBuiltInCASPermissionAttribute(CORSEC_ATTRIBUTE* pAttr)
{
    static const char s_permissionsNamespace[] = "System.Security.Permissions.";
    if(strncmp(pAttr->pName, s_permissionsNamespace, sizeof(s_permissionsNamespace) - 1) != 0)
        return false; // not built-in permission
    static const char s_principalPermissionName[] = "System.Security.Permissions.PrincipalPermissionAttribute, mscorlib";

    // ASSERT: at this point we know we are in builtin namespace...so compare with PrincipalPermissionAttribute
    if (strncmp(pAttr->pName, s_principalPermissionName, sizeof(s_principalPermissionName)-1) == 0)
        return false; // found a principal permission => Not a built-in CAS permission

    // special-case the unrestricted permission set attribute.
    static const char s_PermissionSetName[] = "System.Security.Permissions.PermissionSetAttribute, mscorlib";
    if (strncmp(pAttr->pName, s_PermissionSetName, sizeof(s_PermissionSetName)-1) == 0)
        return IsUnrestrictedPermissionSetAttribute(pAttr);

    return true; //built-in perm, but not principal perm => IsBuiltInCASPermissionAttribute
}

void SecurityAttributes::CreatePermissionSetFromXml(
                                                       IN PBYTE pbRawPermissions,
                                                       IN DWORD cbRawPermissions,
                                                       OUT OBJECTREF* pPset,
                                                       OUT OBJECTREF* pEncoding,
                                                       IN BOOL fNoCache,
                                                       OUT DWORD* pdwSetIndex)
{
    // Deserialize XML to a managed object
    if(pbRawPermissions && cbRawPermissions > 0)
    {
            struct _gc
            {
                OBJECTREF orNonCasPset;
                OBJECTREF orNonCasEncoding;
            } gc;
            ZeroMemory(&gc, sizeof(gc));
            GCPROTECT_BEGIN (gc);
            XmlToPermissionSet(pbRawPermissions, cbRawPermissions, pPset, pEncoding, NULL, 0, &gc.orNonCasPset, &gc.orNonCasEncoding);
            GCPROTECT_END ();
        }

    // Cache the decoded set or collection
    if (!fNoCache)
    {
        if(pbRawPermissions)
        {
            _ASSERTE(*pPset != NULL);
            InsertPermissionSet(pbRawPermissions, cbRawPermissions, *pPset, pdwSetIndex);
        }
    }
}

void SecurityAttributes::CreatePermissionSetFromAttrArray(
                                                       IN PBYTE pbRawPermissions,
                                                       IN DWORD cbRawPermissions,
                                                       OUT OBJECTREF* pObj,
                                                       IN BOOL fNoCache,
                                                       OUT DWORD* pdwIndex,
                                                       DWORD dwAction)
{
    // Make the managed object
    if (pObj)
        AttrSetBlobToPermissionSets(pbRawPermissions, cbRawPermissions, pObj, dwAction);

    // Cache the set or collection
    if(!fNoCache)
    {
        InsertPermissionSet(pbRawPermissions, cbRawPermissions, (pObj == NULL ? NULL : *pObj), pdwIndex);
    }
}

void SecurityAttributes::BlobToPermissionSet(IN PBYTE pbAttrBlob,
                                           IN DWORD cbAttrBlob,
                                           IN BOOL fNoCache,
                                           OUT OBJECTREF *pObj,
                                           OUT DWORD *pdwIndex,
                                           DWORD dwAction)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    // Initialize results
    if (pdwIndex)
        *pdwIndex = ~0;
    if (pObj)
        *pObj = NULL;

    // Handle the empty case
    if ((pbAttrBlob == NULL) || (cbAttrBlob == 0))
    {
        if (pObj)
            *pObj = CreatePermissionSet(FALSE);
        return;
    }

    struct _gc {
        OBJECTREF pset;
        OBJECTREF encoding;
        OBJECTREF nonCasPset;
    } gc;
    memset(&gc, 0, sizeof(gc));

    GCPROTECT_BEGIN(gc);

    // Check the cache to see if we've already decoded an identical blob
    bool bGotIt = false;
    if (!fNoCache)
    {
        DWORD dwDummyIndex;
        DWORD* pIndex = (pdwIndex ? pdwIndex : &dwDummyIndex);
        bGotIt = CheckCache(pbAttrBlob, cbAttrBlob, pIndex);
        if(bGotIt && pObj)
        {
            // Found an identical blob in the cache, so get the managed object
            gc.pset = GetPermissionSet(*pIndex, dwAction);
        }
    }

    // If we haven't found anything yet, create managed object(s) from the blob
    if(!bGotIt)
    {
        if(cbAttrBlob > 0 &&
            pbAttrBlob[0] == LAZY_DECL_SEC_FLAG)
        {
            OBJECTREF* pObjRef = (pObj ? &gc.pset : NULL);
            CreatePermissionSetFromAttrArray(pbAttrBlob, cbAttrBlob, pObjRef, fNoCache, pdwIndex, dwAction);
        }
        else
        {
            DWORD dwDummyIndex;
            DWORD* pIndex = (pdwIndex ? pdwIndex : &dwDummyIndex);
            CreatePermissionSetFromXml(pbAttrBlob, cbAttrBlob, &gc.pset, &gc.encoding, fNoCache, pIndex);
        }
    }

    // return the results 
    if (pObj)
        *pObj = gc.pset;
    GCPROTECT_END();
}


// Check to see if there's already an equivalent blob in the cache.  If there is, get the index.
bool SecurityAttributes::CheckCache(IN PBYTE pbAttrBlob, IN DWORD cbAttrBlob, OUT DWORD *pdwIndex)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

            if(LookupPermissionSet(pbAttrBlob, cbAttrBlob, pdwIndex))
        return true;
    return false;
}


// Call into managed code to group permissions into a PermissionSet and serialize it to XML
void SecurityAttributes::AttrArrayToPermissionSet(OBJECTREF* attrArray, bool fSerialize, DWORD attrCount, BYTE **ppbOutput, DWORD *pcbOutput, BYTE **ppbNonCasOutput, DWORD *pcbNonCasOutput, OBJECTREF* pPermSet)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    MEMORY_REPORT_CONTEXT_SCOPE("PermissionSet");

    EApiCategories eProtectedCategories = (EApiCategories)(GetHostProtectionManager()->GetProtectedCategories());

    MethodDescCallSite createSerialized(METHOD__PERMISSION_SET__CREATE_SERIALIZED);

    // Allocate a managed array of security attribute objects for input to the function.
    PTRARRAYREF orInput = (PTRARRAYREF) AllocateObjectArray(attrCount, g_pObjectClass);

    // Copy over the permission objects references.
    DWORD i;
    for (i = 0; i < attrCount; i++)
    {
        orInput->SetAt(i, attrArray[i]);
    }

    // Call the routine.
    struct _gc {
        U1ARRAYREF orNonCasOutput;
    } gc;
    ZeroMemory(&gc, sizeof(gc));
    GCPROTECT_BEGIN(gc);

    ARG_SLOT fSer = (fSerialize) ? 1 : 0;
    ARG_SLOT args[] = { 
        ObjToArgSlot(orInput),
        fSer,
        PtrToArgSlot(&gc.orNonCasOutput),
        PtrToArgSlot(pPermSet),
        (ARG_SLOT)eProtectedCategories
    };
    U1ARRAYREF orOutput = NULL;

    {
        // Elevate the allowed loading level
        // Elevate thread’s allowed loading level.  This can cause load failures if assemblies loaded from this point on require any assemblies currently being loaded.
        OVERRIDE_LOAD_LEVEL_LIMIT(FILE_ACTIVE); 

        orOutput = (U1ARRAYREF) createSerialized.Call_RetOBJECTREF(args);
    }

    // Buffer the managed output in a native binary blob.
    // Special case the empty blob. We might get a second blob output if
    // there were any non-CAS permissions present.
    NewArrayHolder<BYTE> TempOutput(NULL);
    NewArrayHolder<BYTE> TempNonCasOutput(NULL);

    if (orOutput == NULL)
    {
        *pcbOutput = 0;
    }
    else
    {
        BYTE   *pbArray = orOutput->GetDataPtr();
        DWORD   cbArray = orOutput->GetNumComponents();
        TempOutput = new BYTE[cbArray];
        memcpy(TempOutput, pbArray, cbArray);
        *pcbOutput = cbArray;
    }

    if (gc.orNonCasOutput == NULL)
    {
        *pcbNonCasOutput = 0;
    }
    else
    {
        BYTE   *pbArray = gc.orNonCasOutput->GetDataPtr();
        DWORD   cbArray = gc.orNonCasOutput->GetNumComponents();
        TempNonCasOutput = new BYTE[cbArray];
        memcpy(TempNonCasOutput, pbArray, cbArray);
        *pcbNonCasOutput = cbArray;
    }

    *ppbOutput = TempOutput;
    *ppbNonCasOutput = TempNonCasOutput;

    TempOutput.SuppressRelease();
    TempNonCasOutput.SuppressRelease();

    GCPROTECT_END();
}


//
// This is a public exported method
//

// Translate a set of security custom attributes into a serialized permission set blob.
HRESULT STDMETHODCALLTYPE TranslateSecurityAttributes(CORSEC_ATTRSET    *pAttrSet,
                            BYTE          **ppbOutput,
                            DWORD          *pcbOutput,
                            BYTE          **ppbNonCasOutput,
                            DWORD          *pcbNonCasOutput,
                            DWORD          *pdwErrorIndex)
{

    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
        ENTRY_POINT;    
        MODE_ANY;
    } CONTRACTL_END;
    HRESULT hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    GCX_COOP(); // because it calls into managed code to instantiate the PermissionSet objects
    hr = SecurityAttributes::TranslateSecurityAttributesHelper(pAttrSet, ppbOutput, pcbOutput, 
                ppbNonCasOutput, pcbNonCasOutput, pdwErrorIndex);        

    END_ENTRYPOINT_NOTHROW;

    return hr;
}


//
// This is a public exported method
//

// Reads permission requests (if any) from the manifest of an assembly.
HRESULT STDMETHODCALLTYPE GetPermissionRequests(LPCWSTR   pwszFileName,
                      BYTE    **ppbMinimal,
                      DWORD    *pcbMinimal,
                      BYTE    **ppbOptional,
                      DWORD    *pcbOptional,
                      BYTE    **ppbRefused,
                      DWORD    *pcbRefused)
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
        ENTRY_POINT;
    } CONTRACTL_END;

    HRESULT hr = S_OK;

    BEGIN_EXTERNAL_ENTRYPOINT(&hr)
    {
        MEMORY_REPORT_CONTEXT_SCOPE("PermissionSet");

        IMetaDataDispenser         *pMD = NULL;
        IMetaDataAssemblyImport    *pMDAsmImport = NULL;
        IMetaDataImport            *pMDImport = NULL;
        mdAssembly                  mdAssembly;
        BYTE                       *pbMinimal = NULL;
        DWORD                       cbMinimal = 0;
        BYTE                       *pbOptional = NULL;
        DWORD                       cbOptional = 0;
        BYTE                       *pbRefused = NULL;
        DWORD                       cbRefused = 0;
        HCORENUM                    hEnumDcl = NULL;
        mdPermission                rPSets[dclMaximumValue + 1];
        DWORD                       dwSets;
        DWORD                       i;

        *ppbMinimal = NULL;
        *pcbMinimal = 0;
        *ppbOptional = NULL;
        *pcbOptional = 0;
        *ppbRefused = NULL;
        *pcbRefused = 0;

        // Get the meta data interface dispenser.
        hr = MetaDataGetDispenser(CLSID_CorMetaDataDispenser,
                                  IID_IMetaDataDispenserEx,
                                  (void **)&pMD);
        if (FAILED(hr))
            goto Error;

        // Open a scope on the assembly file.
        hr = pMD->OpenScope(pwszFileName,
                            0,
                            IID_IMetaDataAssemblyImport,
                            (IUnknown**)&pMDAsmImport);
        if (FAILED(hr))
            goto Error;

        // Determine the assembly token.
        hr = pMDAsmImport->GetAssemblyFromScope(&mdAssembly);
        if (FAILED(hr))
            goto Error;

        // QI for a normal import interface.
        hr = pMDAsmImport->QueryInterface(IID_IMetaDataImport, (void**)&pMDImport);
        if (FAILED(hr))
            goto Error;

        // Look for permission request sets hung off the assembly token.
        hr = pMDImport->EnumPermissionSets(&hEnumDcl,
                                           mdAssembly,
                                           dclActionNil,
                                           rPSets,
                                           dclMaximumValue + 1,
                                           &dwSets);
        if (FAILED(hr))
            goto Error;

        for (i = 0; i < dwSets; i++) {
            BYTE   *pbData;
            DWORD   cbData;
            DWORD   dwAction;

            pMDImport->GetPermissionSetProps(rPSets[i],
                                             &dwAction,
                                             (void const **)&pbData,
                                             &cbData);

            switch (dwAction) {
            case dclRequestMinimum:
                _ASSERTE(pbMinimal == NULL);
                pbMinimal = pbData;
                cbMinimal = cbData;
                break;
            case dclRequestOptional:
                _ASSERTE(pbOptional == NULL);
                pbOptional = pbData;
                cbOptional = cbData;
                break;
            case dclRequestRefuse:
                _ASSERTE(pbRefused == NULL);
                pbRefused = pbData;
                cbRefused = cbData;
                break;
            default:
                _ASSERTE(FALSE);
            }
        }

        pMDImport->CloseEnum(hEnumDcl);

        // Buffer the results (since we're about to close the metadata scope and
        // lose the original data).
        if (pbMinimal) {
            *ppbMinimal = new (nothrow) BYTE[cbMinimal];
            if (*ppbMinimal == NULL) {
                hr = E_OUTOFMEMORY;
                goto Error;
            }
            memcpy(*ppbMinimal, pbMinimal, cbMinimal);
            *pcbMinimal = cbMinimal;
        }

        if (pbOptional) {
            *ppbOptional = new (nothrow) BYTE[cbOptional];
            if (*ppbOptional == NULL) {
                hr = E_OUTOFMEMORY;
                goto Error;
            }
            memcpy(*ppbOptional, pbOptional, cbOptional);
            *pcbOptional = cbOptional;
        }

        if (pbRefused) {
            *ppbRefused = new (nothrow) BYTE[cbRefused];
            if (*ppbRefused == NULL) {
                hr = E_OUTOFMEMORY;
                goto Error;
            }
            memcpy(*ppbRefused, pbRefused, cbRefused);
            *pcbRefused = cbRefused;
        }

    Error:
        if (pMDImport)
            pMDImport->Release();
        if (pMDAsmImport)
            pMDAsmImport->Release();
        if (pMD)
            pMD->Release();
    }
    END_EXTERNAL_ENTRYPOINT;

    return hr;
}

// Load permission requests in their serialized form from assembly metadata.
// This consists of a required permissions set and optionally an optional and
// deny permission set.
void SecurityAttributes::LoadPermissionRequestsFromAssembly(IN IMDInternalImport*     pImport,
                                                        OUT OBJECTREF*   pReqdPermissions,
                                                        OUT OBJECTREF*   pOptPermissions,
                                                        OUT OBJECTREF*   pDenyPermissions)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    mdAssembly          mdAssembly;
    HRESULT             hr;

    *pReqdPermissions = NULL;
    *pOptPermissions = NULL;
    *pDenyPermissions = NULL;

    // It's OK to be called with a NULL assembly. This can happen in the code
    // path where we're just checking for a signature, nothing else. So just
    // return without doing anything.
    if (pImport == NULL)
        return;

    // Locate assembly metadata token since the various permission sets are
    // written as custom values against this token.
    if (pImport->GetAssemblyFromScope(&mdAssembly) != S_OK) {
        _ASSERT(FALSE);
        return;
    }

    struct _gc {
        OBJECTREF reqdPset;
        OBJECTREF optPset;
        OBJECTREF denyPset;
    } gc;
    ZeroMemory(&gc, sizeof(gc));
                
    {
        GCX_COOP(); // because GetDeclaredPermissions may call into managed code
        GCPROTECT_BEGIN(gc);

        // Read and translate required permission set.
        hr = Security::GetDeclaredPermissions(pImport, mdAssembly, dclRequestMinimum, &gc.reqdPset, NULL);
        _ASSERT(SUCCEEDED(hr) || (hr == CLDB_E_RECORD_NOTFOUND));

        // Now the optional permission set.
        hr = Security::GetDeclaredPermissions(pImport, mdAssembly, dclRequestOptional, &gc.optPset, NULL);
        _ASSERT(SUCCEEDED(hr) || (hr == CLDB_E_RECORD_NOTFOUND));

        // And finally the refused permission set.
        hr = Security::GetDeclaredPermissions(pImport, mdAssembly, dclRequestRefuse, &gc.denyPset, NULL);
        _ASSERT(SUCCEEDED(hr) || (hr == CLDB_E_RECORD_NOTFOUND));

        *pReqdPermissions = gc.reqdPset;
        *pOptPermissions = gc.optPset;
        *pDenyPermissions = gc.denyPset;

        GCPROTECT_END();
    }
}

// Determine whether a RequestOptional or RequestRefused are made in the assembly manifest.
BOOL SecurityAttributes::RestrictiveRequestsInAssembly(IMDInternalImport* pImport)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;

    mdAssembly          mdAssembly;
    HRESULT             hr;
    HENUMInternal       hEnumDcl;

    // Locate assembly metadata token since the various permission sets are
    // written as custom values against this token.
    hr = pImport->GetAssemblyFromScope(&mdAssembly);
    if (FAILED(hr))
        return TRUE;

    hr = pImport->EnumPermissionSetsInit(mdAssembly,
                                         dclRequestRefuse,
                                         &hEnumDcl);

    BOOL bFoundRequestRefuse = (hr != CLDB_E_RECORD_NOTFOUND);
    pImport->EnumClose(&hEnumDcl);

    if (bFoundRequestRefuse)
        return TRUE;

    hr = pImport->EnumPermissionSetsInit(mdAssembly,
                                         dclRequestOptional,
                                         &hEnumDcl);
    BOOL bFoundRequestOptional = (hr != CLDB_E_RECORD_NOTFOUND);
    pImport->EnumClose(&hEnumDcl);

    return bFoundRequestOptional;
}

// Determine whether permission requests were made in the assembly manifest.
BOOL SecurityAttributes::PermissionsRequestedInAssembly(IN Assembly* pAssembly)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    mdAssembly          mdAssembly;
    IMDInternalImport*  pImport;
    HRESULT             hr;
    HENUMInternal       hEnumDcl;
    BOOL                bFoundRequest;

    // Check for existence of manifest within assembly.
    if ((pImport = pAssembly->GetManifestImport()) == NULL)
        return false;

    // Locate assembly metadata token since the various permission sets are
    // written as custom values against this token.
    hr = pImport->GetAssemblyFromScope(&mdAssembly);
    if (FAILED(hr))
        COMPlusThrowHR(hr);

    // Scan for any requests on the assembly (we assume these must be permission
    // requests, since declarative security can't be applied to assemblies).
    hr = pImport->EnumPermissionSetsInit(mdAssembly,
                                         dclActionNil,
                                         &hEnumDcl);
    if (FAILED(hr))
        COMPlusThrowHR(hr);

    bFoundRequest = pImport->EnumGetCount(&hEnumDcl) > 0;

    pImport->EnumClose(&hEnumDcl);

    return bFoundRequest;
}


// Returns the declared PermissionSet for the specified action type.
HRESULT SecurityAttributes::GetDeclaredPermissions(IN IMDInternalImport *pInternalImport,
                                               IN mdToken token,
                                               IN CorDeclSecurity action,
                                               OUT OBJECTREF *pDeclaredPermissions,
                                               OUT DWORD* pdwSetIndex)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    HRESULT         hr = S_FALSE;
    PBYTE           pbPerm = NULL;
    ULONG           cbPerm = 0;
    void const **   ppData = const_cast<void const**> (reinterpret_cast<void**> (&pbPerm));
    mdPermission    tkPerm;
    HENUMInternal   hEnumDcl;

    _ASSERTE(action > dclActionNil && action <= dclMaximumValue);
    
    // Initialize the output parameters.
    if (pDeclaredPermissions)
        *pDeclaredPermissions = NULL;
    if(pdwSetIndex)
        *pdwSetIndex = ~0;
    
    DWORD dwActionDummy;
    
    // Lookup the permissions for the given declarative action type.
    hr = pInternalImport->EnumPermissionSetsInit(
        token,
        action,
        &hEnumDcl);

    bool bCloseImport = false;
    if (SUCCEEDED(hr))
    {
        bCloseImport = true;
        if (hr != S_FALSE)
        {
            _ASSERTE(pInternalImport->EnumGetCount(&hEnumDcl) == 1 && "Multiple permissions sets for the same declaration aren't currently supported.");        
            if (pInternalImport->EnumNext(&hEnumDcl, &tkPerm))
            {
                pInternalImport->GetPermissionSetProps(
                    tkPerm,
                    &dwActionDummy,
                    ppData,
                    &cbPerm);
            }
            else
            {
                _ASSERTE(!"At least one enumeration expected");
            }
        }
    }
    bool bCas = !(action == dclNonCasDemand || action == dclNonCasLinkDemand || action == dclNonCasInheritance);
    if(pbPerm)
    {
        SecurityAttributes::BlobToPermissionSet(pbPerm,
                                            cbPerm,
                                            FALSE, // fNoCache
                                            pDeclaredPermissions, // pObj
                                            pdwSetIndex, // pdwIndex
                                            action);
    }
    else if(!bCas)
    {
        // We're looking for a non-CAS action which may be encoded with the corresponding CAS action

        // Determine the corresponding CAS action
        CorDeclSecurity casAction = dclDemand;
        if(action == dclNonCasLinkDemand)
            casAction = dclLinktimeCheck;
        else if(action == dclNonCasInheritance)
            casAction = dclInheritanceCheck;

        // Get the blob for the CAS action from the security action table in metadata
        HENUMInternal   hEnumDcl2;
        hr = pInternalImport->EnumPermissionSetsInit(
            token,
            casAction,
            &hEnumDcl2);

        if (SUCCEEDED(hr))
        {
            if (hr != S_FALSE)
            {
                _ASSERTE(pInternalImport->EnumGetCount(&hEnumDcl2) == 1 && "Multiple permissions sets for the same declaration aren't currently supported.");
                if (pInternalImport->EnumNext(&hEnumDcl2, &tkPerm))
                {
                    pInternalImport->GetPermissionSetProps(
                        tkPerm,
                        &dwActionDummy,
                        ppData,
                        &cbPerm);

                    if(pbPerm && cbPerm > 0 && pbPerm[0] == LAZY_DECL_SEC_FLAG) // if it's a serialized CORSEC_ATTRSET
                    {
                        SecurityAttributes::BlobToPermissionSet(pbPerm,
                                                            cbPerm,
                                                            FALSE, // fNoCache
                                                            pDeclaredPermissions, // pObj
                                                            pdwSetIndex, // pdwIndex
                                                            action);
                    }
                }
            }
            pInternalImport->EnumClose(&hEnumDcl2);
        }
    }

    if(bCloseImport)
        pInternalImport->EnumClose(&hEnumDcl);

    return hr;
}

bool SecurityAttributes::IsUnrestrictedPermissionSetAttribute(CORSEC_ATTRIBUTE* pPerm)
{
    BYTE* pbBuffer = pPerm->pbValues;
    DWORD cbBuffer = pPerm->cbValues;

    if (cbBuffer < 2 * sizeof(BYTE))
        return false;

    // Get the field/property specifier
    if (*(BYTE*)pbBuffer == SERIALIZATION_TYPE_FIELD)
        return false;

    _ASSERTE(*(BYTE*)pbBuffer == SERIALIZATION_TYPE_PROPERTY);
    pbBuffer += sizeof(BYTE);
    cbBuffer -= sizeof(BYTE);

    // Get the value type
    DWORD dwType = *(BYTE*)pbBuffer;
    pbBuffer += sizeof(BYTE);
    cbBuffer -= sizeof(BYTE);
    if (dwType != SERIALIZATION_TYPE_BOOLEAN)
        return false;

    // Grab the field/property name and length.
    DWORD cbName;
    BYTE* pbName = (BYTE*)CPackedLen::GetData((const void *)pbBuffer, &cbName);
    PREFIX_ASSUME(pbName != NULL);
    DWORD dwLength = CPackedLen::Size(cbName) + cbName;
    if (cbBuffer < dwLength)
        return false;
    pbBuffer += dwLength;
    cbBuffer -= dwLength;

    // Buffer the name of the property and null terminate it.
    DWORD allocLen = cbName + 1;
    if (allocLen < cbName)
        return false;

    LPSTR szName = (LPSTR)_alloca(allocLen);
    memcpy(szName, pbName, cbName);
    szName[cbName] = '\0';

    if (strcmp(szName, "Unrestricted") != 0)
        return false;

    // Make sure the value isn't "false"
    return (*pbBuffer != 0);
}

// This takes a PermissionSetAttribute blob and looks to see if it uses the "FILE" property.  If it
// does, then it loads the file now and modifies the attribute to use the XML property instead
// (because the file may not be available at runtime.)
HRESULT SecurityAttributes::FixUpPermissionSetAttribute(CORSEC_ATTRIBUTE* pPerm)
{
    _ASSERTE(pPerm->wValues == 1 && strcmp(pPerm->pName, "System.Security.Permissions.PermissionSetAttribute") == 0);
    BYTE* pbBuffer = pPerm->pbValues;
    DWORD cbBuffer = pPerm->cbValues;

    // Check we've got at least the field/property specifier and the
    // type code.
    _ASSERTE(cbBuffer >= (sizeof(BYTE) + sizeof(BYTE)));

    // Grab the field/property specifier.
    bool bIsField = *(BYTE*)pbBuffer == SERIALIZATION_TYPE_FIELD;
    _ASSERTE(bIsField || (*(BYTE*)pbBuffer == SERIALIZATION_TYPE_PROPERTY));
    pbBuffer += sizeof(BYTE);
    cbBuffer -= sizeof(BYTE);

    // Grab the value type.
    DWORD dwType = *(BYTE*)pbBuffer;
    pbBuffer += sizeof(BYTE);
    cbBuffer -= sizeof(BYTE);

    if(bIsField)
        return S_OK;
    if(dwType != SERIALIZATION_TYPE_STRING)
        return S_OK;

    // Grab the field/property name and length.
    DWORD cbName;
    BYTE* pbName = (BYTE*)CPackedLen::GetData((const void *)pbBuffer, &cbName);
    DWORD dwLength = CPackedLen::Size(cbName) + cbName;
    _ASSERTE(cbBuffer >= dwLength);
    pbBuffer += dwLength;
    cbBuffer -= dwLength;

    PREFIX_ASSUME(pbName != NULL);
    // Buffer the name of the property and null terminate it.
    // Check for overflow:
    DWORD allocLen = cbName + 1;
    if (allocLen < cbName)
        COMPlusThrowSO();
    LPSTR szName = (LPSTR)_alloca(allocLen);
    memcpy(szName, pbName, cbName);
    szName[cbName] = '\0';

    if(strcmp(szName, "File") != 0)
        return S_OK;
    if(*pbBuffer == 0xFF) // special case that represents NULL string
        return S_OK;

    pbName = (BYTE*)CPackedLen::GetData((const void *)pbBuffer, &cbName);
    PREFIX_ASSUME(pbName != NULL);    
    dwLength = CPackedLen::Size(cbName) + cbName;
    _ASSERTE(cbBuffer >= dwLength);

    // Open the file
    MAKE_WIDEPTR_FROMUTF8N(wszFileName, (LPCSTR)pbName, cbName);
    HandleHolder hFile(WszCreateFile (wszFileName,
                                        GENERIC_READ,
                                        FILE_SHARE_READ,
                                        NULL,
                                        OPEN_EXISTING,
                                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                                        NULL));
    if (hFile == INVALID_HANDLE_VALUE)
        return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    DWORD dwFileLen = SafeGetFileSize(hFile, 0);
    if (dwFileLen == 0xFFFFFFFF)
        return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

    // Read the file
    BYTE* pFileBuffer = new (nothrow) BYTE[(dwFileLen + 4) * sizeof(BYTE)];
    if(!pFileBuffer)
        return E_OUTOFMEMORY;
    DWORD dwBytesRead;
    if ((SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == 0xFFFFFFFF) ||
        (!ReadFile(hFile, pFileBuffer, dwFileLen, &dwBytesRead, NULL)))
    {
        delete [] pFileBuffer;
        return E_FAIL;
    }
    if(dwBytesRead < dwFileLen)
    {
        delete [] pFileBuffer;
        return E_FAIL;
    }

    // Make the new attribute blob
    BYTE* pNewAttrBuffer = new (nothrow) BYTE[(dwFileLen + 10) * 2 * sizeof(BYTE)];
    if(!pNewAttrBuffer)
        return E_OUTOFMEMORY;
    BYTE* pCurBuf = pNewAttrBuffer;
    *pCurBuf = (BYTE)SERIALIZATION_TYPE_PROPERTY;
    pCurBuf++;
    *pCurBuf = (BYTE)SERIALIZATION_TYPE_STRING;
    pCurBuf++;
    pCurBuf = (BYTE*)CPackedLen::PutLength(pCurBuf, 3);
    memcpy(pCurBuf, "Hex", 3);
    pCurBuf += 3;
    pCurBuf = (BYTE*)CPackedLen::PutLength(pCurBuf, dwFileLen * 2);
    DWORD n;
    BYTE b;
    for(n = 0; n < dwFileLen; n++)
    {
        b = (pFileBuffer[n] >> 4) & 0xf;
        *pCurBuf = (b < 10 ? '0' + b : 'a' + b - 10);
        pCurBuf++;
        b = pFileBuffer[n] & 0xf;
        *pCurBuf = (b < 10 ? '0' + b : 'a' + b - 10);
        pCurBuf++;
    }
    delete [] pFileBuffer;
    DWORD dwNewAttrSize = (pCurBuf - pNewAttrBuffer);

    // Set the new values
    delete(pPerm->pbValues);
    pPerm->pbValues = pNewAttrBuffer;
    pPerm->cbValues = dwNewAttrSize;
    return S_OK;
}

// if tkAssemblyRef is NULL, this assumes the type is in this assembly
// uszClassName should be a UTF8 string including both namespace and class
HRESULT GetFullyQualifiedTypeName(SString* pString, mdAssemblyRef tkAssemblyRef, __in_z CHAR* uszClassName, IMetaDataAssemblyImport *pImport, mdToken tkCtor)
{
    // Add class name
    MAKE_WIDEPTR_FROMUTF8(wszClassName, uszClassName);
    (*pString) += wszClassName;
    if(IsNilToken(tkAssemblyRef))
        tkAssemblyRef = TokenFromRid(1, mdtAssembly);

    // Add a comma separator
    (*pString) += L", ";

    DWORD dwDisplayFlags = ASM_DISPLAYF_VERSION | ASM_DISPLAYF_PUBLIC_KEY_TOKEN | ASM_DISPLAYF_CULTURE;

    // Retrieve size of assembly name
    ASSEMBLYMETADATA sContext;
    ZeroMemory(&sContext, sizeof(ASSEMBLYMETADATA));
    HRESULT hr = S_OK;
    LPWSTR wszAssemblyName = NULL;
    BYTE *pbPublicKeyOrToken = NULL;
    DWORD cbPublicKeyOrToken = 0;
    DWORD dwFlags = 0;
    if(TypeFromToken(tkAssemblyRef) == mdtAssembly)
    {
        DWORD cchName;
        hr = pImport->GetAssemblyProps(tkAssemblyRef,    // [IN] The Assembly for which to get the properties.
                                            NULL,        // [OUT] Pointer to the public key or token.
                                            NULL,        // [OUT] Count of bytes in the public key or token.
                                            NULL,        // [OUT] Hash Algorithm
                                            NULL,        // [OUT] Buffer to fill with name.
                                            NULL,        // [IN] Size of buffer in wide chars.
                                            &cchName,    // [OUT] Actual # of wide chars in name.
                                            &sContext,   // [OUT] Assembly MetaData.
                                            NULL);       // [OUT] Flags.
        if(FAILED(hr))
            return hr;

        // Get the assembly name other naming properties
        wszAssemblyName = (LPWSTR)_alloca(cchName * sizeof(WCHAR));
        hr = pImport->GetAssemblyProps(tkAssemblyRef,
                                            (const void **)&pbPublicKeyOrToken,
                                            &cbPublicKeyOrToken,
                                            NULL,
                                            wszAssemblyName,
                                            cchName,
                                            &cchName,
                                            &sContext,
                                            &dwFlags);
        if(FAILED(hr))
            return hr;
    }
    else if(TypeFromToken(tkAssemblyRef) == mdtAssemblyRef)
    {
        DWORD cchName;
        hr = pImport->GetAssemblyRefProps(tkAssemblyRef, // [IN] The AssemblyRef for which to get the properties.
                                            NULL,        // [OUT] Pointer to the public key or token.
                                            NULL,        // [OUT] Count of bytes in the public key or token.
                                            NULL,        // [OUT] Buffer to fill with name.
                                            NULL,        // [IN] Size of buffer in wide chars.
                                            &cchName,    // [OUT] Actual # of wide chars in name.
                                            &sContext,   // [OUT] Assembly MetaData.
                                            NULL,        // [OUT] Hash blob.
                                            NULL,        // [OUT] Count of bytes in the hash blob.
                                            NULL);       // [OUT] Flags.
        if(FAILED(hr))
            return hr;

        // Get the assembly name other naming properties
        wszAssemblyName = (LPWSTR)_alloca(cchName * sizeof(WCHAR));
        hr = pImport->GetAssemblyRefProps(tkAssemblyRef,
                                            (const void **)&pbPublicKeyOrToken,
                                            &cbPublicKeyOrToken,
                                            wszAssemblyName,
                                            cchName,
                                            &cchName,
                                            &sContext,
                                            NULL,
                                            NULL,
                                            &dwFlags);
        if(FAILED(hr))
            return hr;
    }
    else
    {
        _ASSERTE(false && "unexpected token");
    }

    // Convert to an AssemblyNameObject
    LPASSEMBLYNAME pAssemblyNameObj;
    hr = CreateAssemblyNameObject(&pAssemblyNameObj, wszAssemblyName, CANOF_PARSE_DISPLAY_NAME, NULL);
    if(FAILED(hr))
        return hr;
    _ASSERTE(pAssemblyNameObj && "assembly name object shouldn't be NULL");
    pAssemblyNameObj->SetProperty(ASM_NAME_MAJOR_VERSION, &sContext.usMajorVersion, sizeof(WORD));
    pAssemblyNameObj->SetProperty(ASM_NAME_MINOR_VERSION, &sContext.usMinorVersion, sizeof(WORD));
    pAssemblyNameObj->SetProperty(ASM_NAME_BUILD_NUMBER, &sContext.usBuildNumber, sizeof(WORD));
    pAssemblyNameObj->SetProperty(ASM_NAME_REVISION_NUMBER, &sContext.usRevisionNumber, sizeof(WORD));
    pAssemblyNameObj->SetProperty(ASM_NAME_CULTURE, L"", sizeof(WCHAR));
    if(pbPublicKeyOrToken && cbPublicKeyOrToken > 0)
    {
        if(dwFlags & afPublicKey)
            pAssemblyNameObj->SetProperty(ASM_NAME_PUBLIC_KEY, pbPublicKeyOrToken, cbPublicKeyOrToken);
        else
            pAssemblyNameObj->SetProperty(ASM_NAME_PUBLIC_KEY_TOKEN, pbPublicKeyOrToken, cbPublicKeyOrToken);
    }

    // Convert assembly name to an ole string
    StackSString name;
    FusionBind::GetAssemblyNameDisplayName(pAssemblyNameObj, name, dwDisplayFlags);

    _ASSERTE(!name.IsEmpty() && "the assembly name should not be empty here");

    (*pString) += name;
    return S_OK;
}

HRESULT SecurityAttributes::SerializeAttribute(CORSEC_ATTRIBUTE* pAttr, BYTE* pBuffer, DWORD* pCount, IMetaDataAssemblyImport *pImport)
{
    // pBuffer can be NULL if the caller is only trying to determine the size of the serialized blob.  In that case, let's make a little temp buffer to facilitate CPackedLen::PutLength
    DWORD cbPos = *pCount;
    BYTE* pTempBuf = pBuffer;
    DWORD* pTempPos = &cbPos;
    BYTE tempBuf[8];
    DWORD zero = 0;
    if(!pTempBuf)
    {
        pTempBuf = tempBuf;
        pTempPos = &zero;
    }
    BYTE* pOldPos;

    // Get the fully qualified type name
    SString sType;
    HRESULT hr = GetFullyQualifiedTypeName(&sType, pAttr->tkAssemblyRef, pAttr->pName, pImport, pAttr->tkCtor);
    if(FAILED(hr))
        return hr;

    // Convert assembly name to UTF8.
    const WCHAR* wszTypeName = sType.GetUnicode();
    MAKE_UTF8PTR_FROMWIDE(uszTypeName, wszTypeName);
    DWORD dwUTF8TypeNameLen = (DWORD)strlen(uszTypeName);

    // Serialize the type name length
    pOldPos = &pTempBuf[*pTempPos];
    cbPos += (BYTE*)CPackedLen::PutLength(&pTempBuf[*pTempPos], dwUTF8TypeNameLen) - pOldPos;

    // Serialize the type name
    if(pBuffer)
        memcpy(&pBuffer[cbPos], uszTypeName, dwUTF8TypeNameLen);
    cbPos += dwUTF8TypeNameLen;

    // Serialize the size of the properties blob
    BYTE temp[32];
    int nSizeOfCompressedPropertiesCount = (BYTE*)CPackedLen::PutLength(temp, pAttr->wValues) - temp;
    pOldPos = &pTempBuf[*pTempPos];
    cbPos += (BYTE*)CPackedLen::PutLength(&pTempBuf[*pTempPos], pAttr->cbValues + nSizeOfCompressedPropertiesCount) - pOldPos;

    // Serialize the count of properties
    pOldPos = &pTempBuf[*pTempPos];
    cbPos += (BYTE*)CPackedLen::PutLength(&pTempBuf[*pTempPos], pAttr->wValues) - pOldPos;

    // Serialize the properties blob
    if(pBuffer)
        memcpy(&pBuffer[cbPos], pAttr->pbValues, pAttr->cbValues);
    cbPos += pAttr->cbValues;

    *pCount = cbPos;
    return hr;
}

HRESULT SecurityAttributes::DeserializeAttribute(CORSEC_ATTRIBUTE *pAttr, BYTE* pBuffer, DWORD* pPos)
{
    // Deserialize the size of the type name
    int nLengthSize = 0;
    DWORD dwClassNameSize = CPackedLen::GetLength(&pBuffer[*pPos], &nLengthSize);
    (*pPos) += nLengthSize;

    // Deserialize the type name
    BYTE* pClassName = &pBuffer[*pPos];
    (*pPos) += dwClassNameSize;
    pAttr->pName = new (nothrow) CHAR[dwClassNameSize + 1];
    if(!pAttr->pName)
        return E_OUTOFMEMORY;
    memcpy(pAttr->pName, pClassName, dwClassNameSize);
    pAttr->pName[dwClassNameSize] = '\0';

    // Deserialize the CA blob size
    nLengthSize = 0;
    DWORD dwCABlobSize = CPackedLen::GetLength(&pBuffer[*pPos], &nLengthSize);
    (*pPos) += nLengthSize;

    // Deserialize the CA blob value count
    nLengthSize = 0;
    DWORD dwCABlobValues = CPackedLen::GetLength(&pBuffer[*pPos], &nLengthSize);
    (*pPos) += nLengthSize;
    pAttr->wValues = dwCABlobValues;

    dwCABlobSize -= nLengthSize;
    pAttr->cbValues = dwCABlobSize;

    // Deserialize the CA blob
    BYTE* pCABlob = &pBuffer[*pPos];
    (*pPos) += dwCABlobSize;
    pAttr->pbValues = new (nothrow) BYTE[dwCABlobSize];
    if(!pAttr->pbValues)
        return E_OUTOFMEMORY;
    memcpy(pAttr->pbValues, pCABlob, dwCABlobSize);

    return S_OK;
}

HRESULT AttributeSetToBlob(CORSEC_ATTRSET* pAttrSet, BYTE* pBuffer, DWORD* pCount, IMetaDataAssemblyImport *pImport, DWORD dwAction)
{
    // pBuffer can be NULL if the caller is only trying to determine the size of the serialized blob.  In that case, let's make a little temp buffer to facilitate CPackedLen::PutLength
    DWORD cbPos = 0;
    BYTE* pTempBuf = pBuffer;
    DWORD* pTempPos = &cbPos;
    BYTE tempBuf[8];
    DWORD zero = 0;
    if(!pTempBuf)
    {
        pTempBuf = tempBuf;
        pTempPos = &zero;
    }
    BYTE* pOldPos;
    HRESULT hr = S_OK;

    // Serialize a LAZY_DECL_SEC_FLAG to identify the blob format (as opposed to '<' which would indicate the older XML format)
    if(pBuffer)
        pBuffer[cbPos] = LAZY_DECL_SEC_FLAG;
    cbPos++;

    // Serialize the attribute count
    pOldPos = &pTempBuf[*pTempPos];
    cbPos += (BYTE*)CPackedLen::PutLength(&pTempBuf[*pTempPos], pAttrSet->dwAttrCount) - pOldPos;

    // Serialize the attributes
    DWORD i;
    for(i = 0; i < pAttrSet->dwAttrCount; i++)
    {
        // Get the attribute
        CORSEC_ATTRIBUTE *pAttr = &pAttrSet->pAttrs[i];

        // Perform any necessary fix-up hacks on it
        if(pAttr->wValues == 1 && strcmp(pAttr->pName, "System.Security.Permissions.PermissionSetAttribute") == 0)
            IfFailGo(SecurityAttributes::FixUpPermissionSetAttribute(pAttr));
        else if((dwAction == dclLinktimeCheck || 
                 dwAction == dclInheritanceCheck) &&
            strcmp(pAttr->pName, "System.Security.Permissions.PrincipalPermissionAttribute") == 0)
        {
            VMPostError(CORSECATTR_E_BAD_NONCAS);
            return CORSECATTR_E_BAD_NONCAS;
        }

        // Serialize it
        DWORD dwAttrSize = 0;
        IfFailGo(SecurityAttributes::SerializeAttribute(pAttr, pBuffer ? pBuffer + cbPos : NULL, &dwAttrSize, pImport));
        cbPos += dwAttrSize;
    }
    if(pCount != NULL)
        *pCount = cbPos;

ErrExit:
    if (FAILED(hr))
        VMPostError(CORSECATTR_E_FAILED_TO_CREATE_PERM); // Allows for the correct message to be printed by the compiler

    return hr;
}

HRESULT BlobToAttributeSet(BYTE* pBuffer, CORSEC_ATTRSET* pAttrSet, DWORD dwAction)
{
    HRESULT hr = S_OK;
    DWORD cbPos = 0;
    memset(pAttrSet, '\0', sizeof(CORSEC_ATTRSET));
    if (dwAction >= dclDemand && dwAction <= dclRequestRefuse)
        pAttrSet->dwAction = dwAction; // Already lies in the publicly visible range ( values that managed enum SecurityAction can take)
    else
    {
        // Map the action to a publicly visible value
        if (dwAction == dclNonCasDemand)
            pAttrSet->dwAction = dclDemand;    
        else if (dwAction == dclNonCasInheritance)
            pAttrSet->dwAction = dclInheritanceCheck;
        else if (dwAction == dclNonCasLinkDemand)
            pAttrSet->dwAction = dclLinktimeCheck;
        else
        {
            _ASSERTE(false && "Unexpected dwAction");
            pAttrSet->dwAction = dclActionNil;
        }

    }

    // Deserialize the LAZY_DECL_SEC_FLAG to identify serialization of CORSEC_ATTRSET (as opposed to '<' which would indicate a serialized permission as Xml)
    BYTE firstChar = pBuffer[cbPos];
    cbPos++;
    if(firstChar != LAZY_DECL_SEC_FLAG)
        return S_FALSE;

    // Deserialize the attribute count
    int nLengthSize = 0;
    pAttrSet->dwAttrCount = CPackedLen::GetLength(&pBuffer[cbPos], &nLengthSize);
    cbPos += nLengthSize;
    if(pAttrSet->dwAttrCount > 0)
    {
        pAttrSet->pAttrs = new (nothrow) CORSEC_ATTRIBUTE[pAttrSet->dwAttrCount];
        if(!pAttrSet->pAttrs)
            return E_OUTOFMEMORY;
        pAttrSet->dwAllocated = pAttrSet->dwAttrCount;
    }

    // Deserialize the attributes
    DWORD i;
    for(i = 0; i < pAttrSet->dwAttrCount; i++)
    {
        CORSEC_ATTRIBUTE *pAttr = &pAttrSet->pAttrs[i];
        hr = SecurityAttributes::DeserializeAttribute(pAttr, pBuffer, &cbPos);
        if(FAILED(hr))
            return hr;
    }

    return S_OK;
}

// This function takes an array of COR_SECATTR (which wrap custom security attribute blobs) and
// converts it to an array of CORSEC_ATTRSET (which contains partially-parsed custom security attribute
// blobs grouped by SecurityAction).  Note that you must delete all the pPermissions that this allocates
// for each COR_SECATTR
HRESULT STDMETHODCALLTYPE GroupSecurityAttributesByAction(
                                CORSEC_ATTRSET /*OUT*/rPermSets[],
                                COR_SECATTR rSecAttrs[],
                                ULONG cSecAttrs,
                                mdToken tkObj,
                                ULONG *pulErrorAttr,
                                CMiniMdRW* pMiniMd,
                                IMDInternalImport* pInternalImport)
{
    HRESULT hr = S_OK;
    DWORD i, j, k;
    DWORD dwAction;
    BYTE* pData = NULL;
    CORSEC_ATTRIBUTE* pPerm;
    mdTypeDef tkParent;
    TypeDefRec* pTypeDefRec;
    LPCSTR szNamespace;
    LPCSTR szClass;
    MemberRefRec* pMemberRefRec;
    TypeRefRec* pTypeRefRec;

    // If you are calling this at compile-time, you should pass in pMiniMd, and pInternalImport should be NULL
    // If you are calling this at run-time, you should pass in pInternalImport, and pMiniMd should be NULL
    _ASSERTE((pMiniMd && !pInternalImport) || (!pMiniMd && pInternalImport));

    // Calculate number and sizes of permission sets to produce. This depends on
    // the security action code encoded as the single parameter to the
    // constructor for each security custom attribute.
    for (i = 0; i < cSecAttrs; i++) 
    {
        if (pulErrorAttr)
            *pulErrorAttr = i;

        // Perform basic validation of the header of each security custom
        // attribute constructor call.
        pData = (BYTE*)rSecAttrs[i].pCustomAttribute;

        // Check minimum length.
        if (rSecAttrs[i].cbCustomAttribute < (sizeof(WORD) + sizeof(DWORD) + sizeof(WORD))) 
        {
            VMPostError(CORSECATTR_E_TRUNCATED);
            IfFailGo(CORSECATTR_E_TRUNCATED);
        }

        // Check version.
        if (GET_UNALIGNED_VAL16(pData) != 1) 
        {
            VMPostError(CORSECATTR_E_BAD_VERSION);
            IfFailGo(CORSECATTR_E_BAD_VERSION);
        }
        pData += sizeof(WORD);

        // Extract and check security action.
        if(pData[2] == SERIALIZATION_TYPE_PROPERTY) // check to see if it's a HostProtection attribute w/o an action
            dwAction = dclLinktimeCheck;
        else
            dwAction = GET_UNALIGNED_VAL32(pData);
        if (dwAction == dclActionNil || dwAction > dclMaximumValue) 
        {
            VMPostError(CORSECATTR_E_BAD_ACTION);
            IfFailGo(CORSECATTR_E_BAD_ACTION);
        }

        // All other declarative security only valid on types and methods.
        if (TypeFromToken(tkObj) == mdtAssembly) 
        {
            // Assemblies can only take permission requests.
            if (dwAction != dclRequestMinimum &&
                dwAction != dclRequestOptional &&
                dwAction != dclRequestRefuse) 
            {
                VMPostError(CORSECATTR_E_BAD_ACTION_ASM);
                IfFailGo(CORSECATTR_E_BAD_ACTION_ASM);
            }
        }
        else if (TypeFromToken(tkObj) == mdtTypeDef || TypeFromToken(tkObj) == mdtMethodDef) 
        {
            // Types and methods can only take declarative security.
            if (dwAction != dclRequest &&
                dwAction != dclDemand &&
                dwAction != dclAssert &&
                dwAction != dclDeny &&
                dwAction != dclPermitOnly &&
                dwAction != dclLinktimeCheck &&
                dwAction != dclInheritanceCheck) 
            {
                VMPostError(CORSECATTR_E_BAD_ACTION_OTHER);
                IfFailGo(CORSECATTR_E_BAD_ACTION_OTHER);
            }
        } 
        else 
        {
            // Permission sets can't be attached to anything else.
            VMPostError(CORSECATTR_E_BAD_PARENT);
            IfFailGo(CORSECATTR_E_BAD_PARENT);
        }

        rPermSets[dwAction].dwAttrCount++;
    }

    // Initialize the descriptor for each type of permission set we are going to
    // produce.
    for (i = 0; i <= dclMaximumValue; i++) 
    {
        if (rPermSets[i].dwAttrCount == 0)
            continue;

        rPermSets[i].tkObj = tkObj;
        rPermSets[i].dwAction = i;
        rPermSets[i].pImport = NULL;
        rPermSets[i].pAppDomain = NULL;
        rPermSets[i].pAttrs = new (nothrow) CORSEC_ATTRIBUTE[rPermSets[i].dwAttrCount];
        IfNullGo(rPermSets[i].pAttrs);

        // Initialize a descriptor for each permission within the permission set.
        for (j = 0, k = 0; j < rPermSets[i].dwAttrCount; j++, k++) 
        {
            // Locate the next security attribute that contributes to this
            // permission set.
            for (; k < cSecAttrs; k++) 
            {
                pData = (BYTE*)rSecAttrs[k].pCustomAttribute;
                if(pData[4] == SERIALIZATION_TYPE_PROPERTY) // check to see if it's a HostProtection attribute w/o an action
                    dwAction = dclLinktimeCheck;
                else
                    dwAction = GET_UNALIGNED_VAL32(pData + sizeof(WORD));
                if (dwAction == i)
                    break;
            }
            _ASSERTE(k < cSecAttrs);

            if (pulErrorAttr)
                *pulErrorAttr = k;

            // Initialize the permission.
            pPerm = &rPermSets[i].pAttrs[j];
            pPerm->tkCtor = rSecAttrs[k].tkCtor;
            pPerm->dwIndex = k;
            if(pData[4] == SERIALIZATION_TYPE_PROPERTY) // check to see if it's a HostProtection attribute w/o an action
            {
                _ASSERTE(!pPerm->pbValues);
                //pPerm->pbValues = pData + (sizeof (WORD) + sizeof(WORD));
                pPerm->cbValues = rSecAttrs[k].cbCustomAttribute - (sizeof (WORD) + sizeof(WORD));
                pPerm->wValues = GET_UNALIGNED_VAL16(pData + sizeof (WORD));
                pPerm->pbValues = new (nothrow) BYTE[pPerm->cbValues + sizeof(WORD)];
                if(!pPerm->pbValues)
                    return E_OUTOFMEMORY;
                memcpy(pPerm->pbValues, pData + (sizeof (WORD) + sizeof(WORD)), pPerm->cbValues);
            }
            else
            {
                _ASSERTE(!pPerm->pbValues);
                //pPerm->pbValues = pData + (sizeof (WORD) + sizeof(DWORD) + sizeof(WORD));
                pPerm->cbValues = rSecAttrs[k].cbCustomAttribute - (sizeof (WORD) + sizeof(DWORD) + sizeof(WORD));
                pPerm->wValues = GET_UNALIGNED_VAL16(pData + sizeof (WORD) + sizeof(DWORD));
                pPerm->pbValues = new (nothrow) BYTE[pPerm->cbValues + sizeof(WORD)];
                if(!pPerm->pbValues)
                    return E_OUTOFMEMORY;
                memcpy(pPerm->pbValues, pData + (sizeof (WORD) + sizeof(DWORD) + sizeof(WORD)), pPerm->cbValues);
            }

            if(pMiniMd)
            {
                _ASSERTE(!pInternalImport);

                // Follow the security custom attribute constructor back up to its
                // defining assembly (so we know how to load its definition). If the
                // token resolution scope is not defined, it's assumed to be
                // mscorlib. If a methoddef rather than a memberref is supplied for
                // the parent of the constructor, we potentially have an error
                // condition (since we don't allow security custom attributes to be
                // used in the same assembly as which they're defined). However,
                // this is legal in one specific case, building mscorlib.
                if (TypeFromToken(rSecAttrs[k].tkCtor) == mdtMethodDef) 
                {
                    pPerm->tkTypeRef = mdTokenNil;
                    pPerm->tkAssemblyRef = mdTokenNil;

                    IfFailGo(pMiniMd->FindParentOfMethodHelper(rSecAttrs[k].tkCtor, &tkParent));
                    pTypeDefRec = pMiniMd->getTypeDef(RidFromToken(tkParent));
                    szNamespace = pMiniMd->getNamespaceOfTypeDef(pTypeDefRec);
                    szClass = pMiniMd->getNameOfTypeDef(pTypeDefRec);
                    CQuickBytes qb;
                    CHAR* szTmp = (CHAR*) qb.AllocNoThrow((MAX_CLASSNAME_LENGTH+1) * sizeof(CHAR));
                    if(szTmp == NULL)
                        return E_OUTOFMEMORY;
                    ns::MakePath(szTmp, MAX_CLASSNAME_LENGTH, szNamespace, szClass);
                    size_t len = strlen(szTmp) + 1;
                    pPerm->pName = new (nothrow) CHAR[len];
                    if(!pPerm->pName)
                        return E_OUTOFMEMORY;
                    strcpy_s(pPerm->pName, len, szTmp);
                } 
                else 
                {
                    // Get the type ref
                    _ASSERTE(TypeFromToken(rSecAttrs[k].tkCtor) == mdtMemberRef);
                    pMemberRefRec = pMiniMd->getMemberRef(RidFromToken(rSecAttrs[k].tkCtor));
                    pPerm->tkTypeRef = pMiniMd->getClassOfMemberRef(pMemberRefRec);

                    // Get an assembly ref
                    _ASSERTE(TypeFromToken(pPerm->tkTypeRef) == mdtTypeRef);
                    pTypeRefRec = pMiniMd->getTypeRef(RidFromToken(pPerm->tkTypeRef));
                    pPerm->tkAssemblyRef = pMiniMd->getResolutionScopeOfTypeRef(pTypeRefRec);

                    // Get the fully-qualified type name
                    CQuickBytes qb;
                    CHAR* szTmp = (CHAR*) qb.AllocNoThrow((MAX_CLASSNAME_LENGTH+1) * sizeof(CHAR));
                    if(szTmp == NULL)
                        return E_OUTOFMEMORY;
                    ns::MakePath(szTmp, MAX_CLASSNAME_LENGTH, 
                                 pMiniMd->getNamespaceOfTypeRef(pTypeRefRec),
                                 pMiniMd->getNameOfTypeRef(pTypeRefRec));
                    size_t len = strlen(szTmp) + 1;
                    pPerm->pName = new (nothrow) CHAR[len];
                    if(!pPerm->pName)
                        return E_OUTOFMEMORY;
                    strcpy_s(pPerm->pName, len, szTmp);

                    // We only support the use of security custom attributes defined
                    // in a separate, distinct assembly. So the type resolution
                    // scope must be an assembly ref or the special case of nil
                    // (which implies the attribute is defined in mscorlib). Nested
                    // types (resolution scope of another typeref/def) are also not
                    // supported.
/*                    if ((TypeFromToken(pPerm->tkAssemblyRef) != mdtAssemblyRef) &&
                        !IsNilToken(pPerm->tkAssemblyRef)) 
                    {
                        pPerm->tkAssemblyRef = mdTokenNil;
                        //VMPostError(CORSECATTR_E_NO_SELF_REF);
                        //IfFailGo(CORSECATTR_E_NO_SELF_REF);
                    }*/
                }
            }
            else
            {
                _ASSERTE(pInternalImport);
                LPCSTR pszname;
                LPCSTR psznamespace;
                if (TypeFromToken(rSecAttrs[k].tkCtor) == mdtMethodDef) 
                {
                    IfFailGo(pInternalImport->GetParentToken(rSecAttrs[k].tkCtor, &tkParent));
                    pInternalImport->GetNameOfTypeDef(tkParent, &pszname, &psznamespace);
                    pPerm->tkTypeRef = mdTokenNil;
                    pPerm->tkAssemblyRef = mdTokenNil;
                }
                else
                {
                    _ASSERTE(TypeFromToken(rSecAttrs[k].tkCtor) == mdtMemberRef);

                    // Get the type ref
                    pPerm->tkTypeRef = pInternalImport->GetParentOfMemberRef(rSecAttrs[k].tkCtor);
                    
                    // Get an assembly ref
                    pPerm->tkAssemblyRef = pPerm->tkTypeRef;
                    do{
                        pPerm->tkAssemblyRef = pInternalImport->GetResolutionScopeOfTypeRef(pPerm->tkAssemblyRef);
                    } while(TypeFromToken(pPerm->tkAssemblyRef) == mdtTypeRef); // loop because nested types have a resolution scope of the parent type rather than an assembly

                    // Figure out the fully qualified type name
                    pInternalImport->GetNameOfTypeRef(pPerm->tkTypeRef, &psznamespace, &pszname);                
                }
                CQuickBytes qb;
                CHAR* szTmp = (CHAR*) qb.AllocNoThrow((MAX_CLASSNAME_LENGTH+1) * sizeof(CHAR));
                if(szTmp == NULL)
                    return E_OUTOFMEMORY;
                ns::MakePath(szTmp, MAX_CLASSNAME_LENGTH, psznamespace, pszname);
                size_t len = strlen(szTmp) + 1;
                pPerm->pName = new (nothrow) CHAR[len];
                if(!pPerm->pName)
                    return E_OUTOFMEMORY;
                strcpy_s(pPerm->pName, len, szTmp);
            }
        }
    }

ErrExit:
    return (hr);
}

OBJECTHANDLE PsetHandleCache::GrowAndGetHandle(size_t dwIndex, size_t *newSize, SimpleRWLock *prGlobalLock, AppDomain *pDomain)
{        
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    SimpleWriteLockHolder swlh(prGlobalLock);
    return GrowAndGetHandleHaveLock(dwIndex, newSize, pDomain);        
}

// Call this only if you already have a writer lock for the PermSet cache
OBJECTHANDLE PsetHandleCache::GrowAndGetHandleHaveLock(size_t dwIndex, size_t *newSize, AppDomain *pDomain)
{              
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // Check if another thread has already grown the array list
    // If nobody has, then we will append to the array list        
    if (dwIndex >= GetCount())
    {
        size_t i = GetCount(); 
        OBJECTHANDLE ret = NULL;
        EX_TRY
        {
            // Fill-in handles
            for (; i < dwIndex ; i++) 
            {
                IfFailThrow(Append(pDomain->CreateHandle(NULL)));
            }
            ret = pDomain->CreateHandle(NULL);
            IfFailThrow(Append(ret));
        }
        EX_CATCH
        {
            *newSize = GetCount();
            EX_RETHROW;
        }
        EX_END_CATCH_UNREACHABLE;

        *newSize = GetCount();        
        return ret;
    }
    // Two cases: 
    // 1) Caller didn't really have to grow the arr, should have used Get() instead
    // 2) While we were waiting for the lock, another thread has grown the arr
    //    Must retry using Get()
    return NULL;
}





