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
// --------------------------------------------------------------------------------
// PEFile.cpp
// --------------------------------------------------------------------------------

#include "common.h"
#include "pefile.h"
#include "strongname.h"
#include "corperm.h"
#include "eecontract.h"
#include "apithreadstress.h"
#include "eeconfig.h"
#include "fusionpriv.h"
#include "product_version.h"
#include "memoryreport.h"
#include "eventtrace.h"
#include "timeline.h"
#include "security.h"
#include "corperm.h"
#include "eventtrace.h"
#include "dbginterface.h"
#include "peimagelayout.inl"
#include "dlwrap.h"
#include "invokeutil.h"




#ifndef DACCESS_COMPILE

// ================================================================================
// PEFile class - this is an abstract base class for PEModule and PEAssembly
// ================================================================================

PEFile::PEFile(PEImage *identity, BOOL fCheckSecurity/*=TRUE*/) :
#if _DEBUG
    m_pDebugName(NULL),
#endif
    m_identity(NULL),
    m_ILimage(NULL),
    m_nativeImage(NULL),
    m_fCanUseNativeImage(TRUE),
    m_bHasPersistentMDImport(FALSE),
    m_pMDImport(NULL),
    m_pImporter(NULL),
    m_pImporter2(NULL),
    m_pAssemblyImporter(NULL),
    m_pEmitter(NULL),
    m_pAssemblyEmitter(NULL),
    m_pMetadataLock(::new SimpleRWLock(PREEMPTIVE, LOCK_TYPE_DEFAULT)),
    m_refCount(1),
    m_hash(NULL),
    m_flags(0),
    m_certificate(NULL)
{
    CONTRACTL
    {
        CONSTRUCTOR_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    if (identity)
    {
        identity->AddRef();
        m_identity=identity;
        identity->AddRef();
                m_ILimage=identity;
    }

    if (fCheckSecurity)
        CheckSecurity();
}



PEFile::~PEFile()
{
    CONTRACTL
    {
        DESTRUCTOR_CHECK;
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;



    ReleaseMetadataInterfaces(TRUE);

    if (m_hash != NULL)
        delete m_hash;



    if (m_ILimage != NULL)
        m_ILimage->Release();
    if (m_identity != NULL)
        m_identity->Release();
    if (m_pMetadataLock)
        delete m_pMetadataLock;
    if (m_certificate && !g_pCertificateCache->Contains(m_certificate))
        CoTaskMemFree(m_certificate);
}

#ifndef  DACCESS_COMPILE
void PEFile::ReleaseIL()
{
    WRAPPER_CONTRACT;
    if (m_ILimage!=NULL )
    {
        ReleaseMetadataInterfaces(TRUE, TRUE);
        if (m_identity != NULL)
        {
            m_identity->Release();
            m_identity=NULL;
        }
        m_ILimage->Release();
        m_ILimage=NULL;
    }
}
#endif

/* static */
PEFile *PEFile::Open(PEImage *image)
{
    CONTRACT(PEFile *)
    {
        PRECONDITION(image != NULL);
        PRECONDITION(image->CheckFormat());
        POSTCONDITION(RETVAL != NULL);
        POSTCONDITION(!RETVAL->IsModule());
        POSTCONDITION(!RETVAL->IsAssembly());
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    MEMORY_REPORT_CONTEXT_SCOPE("PEFile");

    PEFile *pFile = new PEFile(image);

    if (image->HasNTHeaders() && image->HasCorHeader())
        pFile->OpenMDImport_Unsafe(); //no one else can see the object yet

#if _DEBUG
    pFile->m_debugName = image->GetPath();
    pFile->m_debugName.Normalize();
    pFile->m_pDebugName = pFile->m_debugName;
#endif

    RETURN pFile;
}

// ------------------------------------------------------------
// Loader support routines
// ------------------------------------------------------------

template<class T> void CoTaskFree(T *p)
{
    if (p != NULL)
    {
        p->T::~T();

        CoTaskMemFree(p);
    }
}


NEW_HOLDER_TEMPLATE1(CoTaskNewHolder, typename TYPE, Wrapper,
                     TYPE*, DoNothing<TYPE*>, CoTaskFree<TYPE>, NULL);

BOOL PEFile::CanLoadLibrary()
{
    WRAPPER_CONTRACT;

    // Dynamic and resource modules don't need LoadLibrary.
    if (IsDynamic() || IsResource()||IsLoaded())
        return TRUE;

    // If we're been granted skip verification, OK
    if (HasSkipVerification())
        return TRUE;

    // Otherwise, we can only load if IL only.
    return IsILOnly();
}
void PEFile::LoadLibrary(BOOL allowNativeSkip/*=TRUE*/) // if allowNativeSkip==FALSE force IL image load
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        POSTCONDITION(CheckLoaded());
        THROWS;
        WRAPPER(GC_TRIGGERS);
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;


    // See if we've already loaded it.
    if (CheckLoaded(allowNativeSkip))
        RETURN;

    // Note that we may be racing other threads here, in the case of domain neutral files

    // Resource images are always flat.
    if (IsResource())
    {
        m_ILimage->LoadNoMetaData(IsIntrospectionOnly());
        RETURN;
    }



    // Don't do this if we are unverifiable
    if (!CanLoadLibrary())
        ThrowHR(SECURITY_E_UNVERIFIABLE);


    // We need contents now
    if (!HasNativeImage())
    {
        EnsureImageOpened();
    }

    if (IsIntrospectionOnly())
    {
        m_ILimage->LoadForIntrospection();
        RETURN;
    }


    //---- Below this point, only do the things necessary for execution ----
    _ASSERTE(!IsIntrospectionOnly());


    // Since we couldn't call LoadLibrary, we must be an IL only image
    // or the image may still contain unfixed up stuff
    // Note that we make an exception for CompilationDomains, since PEImage
    // will map non-ILOnly images in a compilation domain.
    if (!m_ILimage->IsILOnly() && !GetAppDomain()->IsCompilationDomain())
    {
        if (!m_ILimage->HasV1Metadata())
            ThrowHR(COR_E_FIXUPSINEXE);
    }



    // If we are already mapped, we can just use the current image.
    {
        if (m_ILimage->IsFile())
            m_ILimage->LoadFromMapped();
        else
            m_ILimage->LoadNoFile();
    }
    RETURN;
}


/* static */
void PEFile::DefineEmitScope(GUID iid, void **ppEmit)
{
    CONTRACT_VOID
    {
        PRECONDITION(CheckPointer(ppEmit));
        POSTCONDITION(CheckPointer(*ppEmit));
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    SafeComHolder<IMetaDataDispenserEx>    pDispenser;

    // Get the Dispenser interface.
    MetaDataGetDispenser(CLSID_CorMetaDataDispenser,
                         IID_IMetaDataDispenserEx, (void **)&pDispenser);
    if (!pDispenser)
        ThrowOutOfMemory();

    // Set the option on the dispenser turn on duplicate check for TypeDef and moduleRef
    VARIANT varOption;
    V_VT(&varOption) = VT_UI4;
    V_I4(&varOption) = MDDupDefault | MDDupTypeDef | MDDupModuleRef | MDDupExportedType | MDDupAssemblyRef | MDDupPermission | MDDupFile;
    IfFailThrow(pDispenser->SetOption(MetaDataCheckDuplicatesFor, &varOption));

    // turn on the thread safety!
    V_I4(&varOption) = MDThreadSafetyOn;
    IfFailThrow(pDispenser->SetOption(MetaDataThreadSafetyOptions, &varOption));

    IfFailThrow(pDispenser->DefineScope(CLSID_CorMetaDataRuntime, 0, iid, (IUnknown **)ppEmit));

    RETURN;
}

// ------------------------------------------------------------
// Identity
// ------------------------------------------------------------

BOOL PEFile::Equals(PEFile *pFile)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pFile));
        GC_NOTRIGGER;
        NOTHROW;
        MODE_ANY;
    }
    CONTRACTL_END;

    // Same object is equal
    if (pFile == this)
        return TRUE;


    // Execution and introspection files are NOT equal
    if ( (!IsIntrospectionOnly()) != !(pFile->IsIntrospectionOnly()) )
    {
        return FALSE;
    }

    // Same identity is equal
    if (m_identity != NULL && pFile->m_identity != NULL
        && m_identity->Equals(pFile->m_identity))
        return TRUE;

    // Same image is equal
    if (m_ILimage != NULL && pFile->m_ILimage != NULL
        && m_ILimage->Equals(pFile->m_ILimage))
        return TRUE;


    return FALSE;
}

BOOL PEFile::Equals(PEImage *pImage)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pImage));
        GC_NOTRIGGER;
        NOTHROW;
        MODE_ANY;
    }
    CONTRACTL_END;

    // Same object is equal
    if (pImage == m_identity || pImage == m_ILimage || pImage == m_nativeImage)
        return TRUE;

    // Same identity is equal
    if (m_identity != NULL
        && m_identity->Equals(pImage))
        return TRUE;

    // Same image is equal
    if (m_ILimage != NULL
        && m_ILimage->Equals(pImage))
        return TRUE;


    return FALSE;
}

// ------------------------------------------------------------
// Descriptive strings
// ------------------------------------------------------------

void PEFile::GetCodeBaseOrName(SString &result)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (m_identity != NULL && !m_identity->GetPath().IsEmpty())
    {
        result.Set(m_identity->GetPath());
    }
    else if (IsAssembly())
    {
        ((PEAssembly*)this)->GetCodeBase(result);
    }
    else
        result.SetUTF8(GetSimpleName());
}

// Returns security information for the assembly based on the codebase
void PEFile::GetSecurityIdentity(SString &codebase, DWORD *pdwZone, BYTE *pbUniqueID, DWORD *pcbUniqueID)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pdwZone));
        PRECONDITION(CheckPointer(pbUniqueID));
        PRECONDITION(CheckPointer(pcbUniqueID));
    }
    CONTRACTL_END;

    if (IsAssembly())
    {
        ((PEAssembly*)this)->GetCodeBase(codebase);
    }
    else if (m_identity != NULL && !m_identity->GetPath().IsEmpty())
    {
        codebase.Set(L"file:///");
        codebase.Append(m_identity->GetPath());
    }
    else
    {
        _ASSERTE( !"Unable to determine security identity" );
    }

    GCX_PREEMP();

    if(!codebase.IsEmpty()) {
        *pdwZone = Security::QuickGetZone( codebase );

    }
}


// ------------------------------------------------------------
// Checks
// ------------------------------------------------------------



CHECK PEFile::CheckLoaded(BOOL bAllowNativeSkip/*=TRUE*/)
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_CHECK_END;

    CHECK(IsLoaded(bAllowNativeSkip)
          // We are allowed to skip LoadLibrary in most cases for ngen'ed IL only images
          || (bAllowNativeSkip && HasNativeImage() && IsILOnly()));

    CHECK_OK;
}

// ------------------------------------------------------------
// Hash support
// ------------------------------------------------------------

#ifndef SHA1_HASH_SIZE
#define SHA1_HASH_SIZE 20
#endif

void PEFile::GetSHA1Hash(SBuffer &result)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckValue(result));
        THROWS;
        MODE_ANY;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // Cache the SHA1 hash in a buffer
    if (m_hash == NULL)
    {
        // We shouldn't have to compute a SHA1 hash in any scenarios
        // where the image opening should be suppressed.
        EnsureImageOpened();

        m_hash = new InlineSBuffer<SHA1_HASH_SIZE>();
        m_ILimage->ComputeHash(CALG_SHA1, *m_hash);
    }

    result.Set(*m_hash);
}

// ------------------------------------------------------------
// Metadata access
// ------------------------------------------------------------

#endif // #ifndef DACCESS_COMPILE
 const void *PEFile::GetMetadata(COUNT_T *pSize)
{
    CONTRACT(const void *)
    {
        INSTANCE_CHECK;
        POSTCONDITION(CheckPointer(pSize, NULL_OK));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACT_END;


    if (m_ILimage == NULL
         || !m_ILimage->HasNTHeaders()
         || !m_ILimage->HasCorHeader())
    {
        if (pSize != NULL)
            *pSize = 0;
        RETURN NULL;
    }
    else
    {
        RETURN m_ILimage->GetMetadata(pSize);
    }
}

const void *PEFile::GetLoadedMetadata(COUNT_T *pSize)
{
    CONTRACT(const void *)
    {
        INSTANCE_CHECK;
        POSTCONDITION(CheckPointer(pSize, NULL_OK));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_END;


    if (m_ILimage == NULL
         || !m_ILimage->GetLoadedLayout()->HasNTHeaders()
         || !m_ILimage->GetLoadedLayout()->HasCorHeader())
    {
        if (pSize != NULL)
            *pSize = 0;
        RETURN NULL;
    }
    else
    {
        RETURN GetLoadedIL()->GetMetadata(pSize);
    }
}

#ifndef DACCESS_COMPILE

void PEFile::OpenImporter()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    } 
    CONTRACTL_END;

    // Make sure internal MD is in RW format.
    ConvertMDInternalToReadWrite();
 
    IMetaDataImport *pIMDImport = NULL;
    IfFailThrow(GetMetaDataPublicInterfaceFromInternal((void*)GetPersistentMDImport(), 
                                                       IID_IMetaDataImport, 
                                                       (void **)&pIMDImport));

    IMetaDataImport *pIMDImport2 = NULL;
    IfFailThrow(GetMetaDataPublicInterfaceFromInternal((void*)GetPersistentMDImport(), 
                                                       IID_IMetaDataImport2, 
                                                       (void **)&pIMDImport2));

    // Atomically swap it into the field (release it if we lose the race)
    if (FastInterlockCompareExchangePointer((void **)&m_pImporter, pIMDImport, NULL) != NULL)
        pIMDImport->Release();

    if (FastInterlockCompareExchangePointer((void **)&m_pImporter2, pIMDImport2, NULL) != NULL)
        pIMDImport2->Release();
}

void PEFile::ConvertMDInternalToReadWrite()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(EX_THROW(EEMessageException, (E_OUTOFMEMORY)););
    }
    CONTRACTL_END;

    IMDInternalImport *pOld;            // Old (current RO) value of internal import.
    IMDInternalImport *pNew = NULL;     // New (RW) value of internal import.

    // Take a local copy of *ppImport.  This may be a pointer to an RO
    //  or to an RW MDInternalXX.
    pOld = m_pMDImport;
    IMetaDataImport *pIMDImport2 = m_pImporter2;
    if (pIMDImport2 != NULL)
    {
        HRESULT hr = GetMetaDataInternalInterfaceFromPublic(pIMDImport2, IID_IMDInternalImport, (void **)&pNew);
        if (FAILED(hr))
        {
            EX_THROW(EEMessageException, (hr));
        }
        if (pNew == pOld)
        {
            pNew->Release();
            return;
        }
    }
    else
    {
    // If an RO, convert to an RW, return S_OK.  If already RW, no conversion
    //  needed, return S_FALSE.
    HRESULT hr = ConvertMDInternalImport(pOld, &pNew);

    if (FAILED(hr))
    {
        EX_THROW(EEMessageException, (hr));
    }

    // If no conversion took place, don't change pointers.
    if (hr == S_FALSE)
        return;
    }

    // Swap the pointers in a thread safe manner.  If the contents of *ppImport
    //  equals pOld then no other thread got here first, and the old contents are
    //  replaced with pNew.  The old contents are returned.
    _ASSERTE(m_bHasPersistentMDImport);
    if (FastInterlockCompareExchangePointer((void**)&m_pMDImport, pNew, pOld) == pOld)
    {   // Swapped -- get the metadata to hang onto the old Internal import.
        HRESULT hr=m_pMDImport->SetUserContextData(pOld);
        _ASSERTE(SUCCEEDED(hr)||!"Leaking old MDImport");
        IfFailThrow(hr);
    }
    else
    {   // Some other thread finished first.  Just free the results of this conversion.
        pNew->Release();
    }
}

void PEFile::ConvertMetadataToRWForEnC()
{
    WRAPPER_CONTRACT;

    // This should only ever be called on EnC capable files.
    _ASSERTE(Module::IsEditAndContinueCapable(this));

    // This should only be called if we're debugging, stopped, and on the helper thread.
    _ASSERTE(CORDebuggerAttached());
    _ASSERTE((g_pDebugInterface != NULL) && g_pDebugInterface->ThisIsHelperThread());
    _ASSERTE((g_pDebugInterface != NULL) && g_pDebugInterface->IsStopped());

    // Convert the metadata to RW for Edit and Continue, properly replacing the metadata import interface pointer and 
    // properly preserving the old importer. This will be called before the EnC system tries to apply a delta to the module's 
    // metadata. ConvertMDInternalToReadWrite() does that quite nicely for us.
    ConvertMDInternalToReadWrite();
}

void PEFile::OpenMDImport_Unsafe()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (m_pMDImport != NULL)
        return;
    if (m_nativeImage != NULL)
    {
        // Use native image for metadata
        m_flags |= PEFILE_HAS_NATIVE_IMAGE_METADATA;
        m_pMDImport=m_nativeImage->GetMDImport();
    }
    else
    {
        m_flags &= ~PEFILE_HAS_NATIVE_IMAGE_METADATA;
        if (m_ILimage != NULL
           && m_ILimage->HasNTHeaders()
             && m_ILimage->HasCorHeader())
        {
            m_pMDImport=m_ILimage->GetMDImport();

        }
        else
            ThrowHR(COR_E_BADIMAGEFORMAT);
    }
    _ASSERTE(m_pMDImport);
    m_pMDImport->AddRef();
}

void PEFile::OpenAssemblyImporter()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // Make sure internal MD is in RW format.
    ConvertMDInternalToReadWrite();

    // Get the interface
    IMetaDataAssemblyImport *pIMDAImport = NULL;
    IfFailThrow(GetMetaDataPublicInterfaceFromInternal((void*)GetPersistentMDImport(), 
                                                       IID_IMetaDataAssemblyImport, 
                                                       (void **)&pIMDAImport));

    // Atomically swap it into the field (release it if we lose the race)
    if (FastInterlockCompareExchangePointer((void **)&m_pAssemblyImporter, pIMDAImport, NULL) != NULL)
        pIMDAImport->Release();
}

void PEFile::OpenEmitter()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // Make sure internal MD is in RW format.
    ConvertMDInternalToReadWrite();

    IMetaDataEmit *pIMDEmit = NULL;
    IfFailThrow(GetMetaDataPublicInterfaceFromInternal((void*)GetPersistentMDImport(),
                                                       IID_IMetaDataEmit,
                                                       (void **)&pIMDEmit));

    // Atomically swap it into the field (release it if we lose the race)
    if (FastInterlockCompareExchangePointer((void **)&m_pEmitter, pIMDEmit, NULL) != NULL)
        pIMDEmit->Release();
}

void PEFile::OpenAssemblyEmitter()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // Make sure internal MD is in RW format.
    ConvertMDInternalToReadWrite();

    IMetaDataAssemblyEmit *pIMDAssemblyEmit = NULL;
    IfFailThrow(GetMetaDataPublicInterfaceFromInternal((void*)GetPersistentMDImport(),
                                                       IID_IMetaDataAssemblyEmit,
                                                       (void **)&pIMDAssemblyEmit));

    // Atomically swap it into the field (release it if we lose the race)
    if (FastInterlockCompareExchangePointer((void **)&m_pAssemblyEmitter, pIMDAssemblyEmit, NULL) != NULL)
        pIMDAssemblyEmit->Release();
}

void PEFile::ReleaseMetadataInterfaces(BOOL bDestructor, BOOL bKeepNativeData/*=FALSE*/)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(bDestructor||m_pMetadataLock->IsWriterLock());
    }
    CONTRACTL_END;
    _ASSERTE(bDestructor || !m_bHasPersistentMDImport);
    if (m_pAssemblyImporter != NULL)
    {
        m_pAssemblyImporter->Release();
        m_pAssemblyImporter = NULL;
    }
    if (m_pImporter != NULL)
    {
        m_pImporter->Release();
        m_pImporter = NULL;
    }
    if (m_pImporter2 != NULL)
    {
        m_pImporter2->Release();
        m_pImporter2 = NULL;
    }
    if (m_pEmitter != NULL)
    {
        m_pEmitter->Release();
        m_pEmitter = NULL;
    }
    if(m_pAssemblyEmitter)
    {
        m_pAssemblyEmitter->Release();
        m_pAssemblyEmitter=NULL;
    }

    if (m_pMDImport != NULL && (!bKeepNativeData || !HasNativeImage()))
    {
        m_pMDImport->Release();
        m_pMDImport=NULL;
     }
}

void PEFile::CheckSecurity()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // Check any security signature in the header.

    // This publisher data can potentially be cached and passed back in via
    // PEAssembly::CreateDelayed.
    //
    // HOWEVER - even if we cache it, the certificate still may need to be verified at
    // load time.  The only real caching can be done when the COR_TRUST certificate is
    // ABSENT.
    //
    // (In the case where it is present, we could still theoretically
    // cache the certificate and re-verify it and at least avoid touching the image
    // again, however this path is not implemented yet, so this is TBD if we decide
    // it is an important case to optimize for.)

    if (!HasSecurityDirectory())
        LOG((LF_SECURITY, LL_INFO1000, "No certificates found in module\n"));
    else
    {
    }
}

// ------------------------------------------------------------
// PE file access
// ------------------------------------------------------------

// Note that most of these APIs are currently passed through
// to the main image.  However, in the near future they will
// be rerouted to the native image in the prejitted case so
// we can avoid using the original IL image.


// ------------------------------------------------------------
// Resource access
// ------------------------------------------------------------

void PEFile::GetEmbeddedResource(DWORD dwOffset, DWORD *cbResource, PBYTE *pbInMemoryResource)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(ThrowOutOfMemory(););
    }
    CONTRACTL_END;

    // NOTE: it's not clear whether to load this from m_image or m_loadedImage.
    // m_loadedImage is probably preferable, but this may be called by security
    // before the image is loaded.

    PEImage *image;

    if (m_nativeImage != NULL)
        image = m_nativeImage;
    else if (m_ILimage != NULL)
        image = m_ILimage;
    else
    {
        EnsureImageOpened();
        image = m_ILimage;
    }

    PEImageLayoutHolder theImage(image->GetLayout(PEImageLayout::LAYOUT_ANY,PEImage::LAYOUT_CREATEIFNEEDED));
    if (!theImage->CheckResource(dwOffset))
        ThrowHR(COR_E_BADIMAGEFORMAT);

    COUNT_T size;
    theImage->MakePersistent();
    const void *resource = theImage->GetResource(dwOffset, &size);

    *cbResource = size;
    *pbInMemoryResource = (PBYTE) resource;
}

// ------------------------------------------------------------
// File loading
// ------------------------------------------------------------

PEAssembly *PEFile::LoadAssembly(mdAssemblyRef kAssemblyRef,
                                 IMDInternalImport *pImport/*=NULL*/)
{
    CONTRACT(PEAssembly *)
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    if (pImport == NULL)
        pImport = GetPersistentMDImport();

    if( ((TypeFromToken(kAssemblyRef) != mdtAssembly) &&
         (TypeFromToken(kAssemblyRef) != mdtAssemblyRef)) ||
        (!pImport->IsValidToken(kAssemblyRef)) )
            ThrowHR(COR_E_BADIMAGEFORMAT);

    AssemblySpec spec;

    spec.InitializeSpec(kAssemblyRef, pImport, GetAssembly(), IsIntrospectionOnly());

    RETURN GetAppDomain()->BindAssemblySpec(&spec, TRUE, IsIntrospectionOnly());
}

// ------------------------------------------------------------
// Logging
// ------------------------------------------------------------

void PEFile::ExternalLog(DWORD level, const WCHAR *fmt, ...)
{
    WRAPPER_CONTRACT;

    va_list args;
    va_start(args, fmt);

    ExternalVLog(level, fmt, args);

    va_end(args);
}

void PEFile::ExternalLog(DWORD level, const char *msg)
{
    WRAPPER_CONTRACT;

    // It is OK to use %S here. We know that msg is ASCII-only.
    ExternalLog(level, L"%S", msg);
}

void PEFile::ExternalVLog(DWORD level, const WCHAR *fmt, va_list args)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACT_END;

    BOOL fOutputToDebugger = (level == LL_ERROR && IsDebuggerPresent());
    BOOL fOutputToLogging = LoggingOn(LF_ZAP, level);

    if (!fOutputToDebugger && !fOutputToLogging)
        return;

    StackSString message;
    message.VPrintf(fmt, args);

    if (fOutputToLogging)
    {
        if (GetMDImport() != NULL)
            LOG((LF_ZAP, level, "ZAP: \"%s\"", GetSimpleName()));
        else
            LOG((LF_ZAP, level, "ZAP: \"%S\"", ((const WCHAR *)GetPath())));

        LOG((LF_ZAP, level, "%S", message.GetUnicode()));
        LOG((LF_ZAP, level, "\n"));
    }

    if (fOutputToDebugger)
    {
        WszOutputDebugString(L"CLR:(");

        StackSString codebase;
        GetCodeBaseOrName(codebase);
        WszOutputDebugString(codebase);

        WszOutputDebugString(L") ");

        WszOutputDebugString(message);
        WszOutputDebugString(L"\n");
    }

    RETURN;
}

void PEFile::FlushExternalLog()
{
    LEAF_CONTRACT;
}

BOOL PEFile::GetResource(LPCSTR szName, DWORD *cbResource,
                                 PBYTE *pbInMemoryResource, DomainAssembly** pAssemblyRef,
                                 LPCSTR *szFileName, DWORD *dwLocation,
                                 StackCrawlMark *pStackMark, BOOL fSkipSecurityCheck,
                                 BOOL fSkipRaiseResolveEvent, DomainAssembly* pDomainAssembly, AppDomain* pAppDomain)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
        WRAPPER(GC_TRIGGERS);
    }
    CONTRACTL_END;


    mdToken            mdLinkRef;
    DWORD              dwResourceFlags;
    DWORD              dwOffset;
    mdManifestResource mdResource;
    Assembly*          pAssembly = NULL;
    PEFile*            pPEFile = NULL;
    ReleaseHolder<IMDInternalImport> pImport (GetMDImportWithRef());
    if (SUCCEEDED(pImport->FindManifestResourceByName(szName,
                                                                       &mdResource)))
    {
        pPEFile=this;
        pImport->GetManifestResourceProps(mdResource,
                                                                            NULL, //&szName,
                                                                            &mdLinkRef,
                                                                            &dwOffset,
                                                                            &dwResourceFlags);
    }
    else
    {
        if(fSkipRaiseResolveEvent || pAppDomain == NULL)
            return FALSE;

        pAssembly = pAppDomain->RaiseResourceResolveEvent(szName);
        if (pAssembly == NULL)
            return FALSE;

        pDomainAssembly = pAssembly->GetDomainAssembly(pAppDomain);
        pPEFile = pDomainAssembly->GetFile();

        if (FAILED(pAssembly->GetManifestImport()->FindManifestResourceByName(szName,
                                                                              &mdResource)))
            return FALSE;

        if (dwLocation) {
            if (pAssemblyRef)
                *pAssemblyRef = pDomainAssembly;

            *dwLocation = *dwLocation | 2; // ResourceLocation.containedInAnotherAssembly
        }
        pPEFile->GetPersistentMDImport()->GetManifestResourceProps(mdResource,
                                                                            NULL, //&szName,
                                                                            &mdLinkRef,
                                                                            &dwOffset,
                                                                            &dwResourceFlags);
    }


    switch(TypeFromToken(mdLinkRef)) {
    case mdtAssemblyRef:
        {
            if (pDomainAssembly == NULL)
                return FALSE;

            AssemblySpec spec;
            spec.InitializeSpec(mdLinkRef, GetPersistentMDImport(), pDomainAssembly->GetFile(), pDomainAssembly->GetFile()->IsIntrospectionOnly());
            pDomainAssembly = spec.LoadDomainAssembly(FILE_LOADED);

            if (dwLocation) {
                if (pAssemblyRef)
                    *pAssemblyRef = pDomainAssembly;

                *dwLocation = *dwLocation | 2; // ResourceLocation.containedInAnotherAssembly
            }

            return pDomainAssembly->GetResource(szName,
                                                cbResource,
                                                pbInMemoryResource,
                                                pAssemblyRef,
                                                szFileName,
                                                dwLocation,
                                                pStackMark,
                                                fSkipSecurityCheck,
                                                fSkipRaiseResolveEvent);
        }

    case mdtFile:
        if (mdLinkRef == mdFileNil) {
            // The resource is embedded in the manifest file

            if (!IsMrPublic(dwResourceFlags) && pStackMark && !fSkipSecurityCheck) {
                Assembly *pCallersAssembly = SystemDomain::GetCallersAssembly(pStackMark);
                if (pCallersAssembly &&  // full trust for interop
                    (!pCallersAssembly->GetManifestFile()->Equals(this))) {

                    RefSecContext sCtx;
                    if (!sCtx.DemandMemberAccess(REFSEC_CHECK_MEMBERACCESS))
                        return FALSE;
                }
            }

            if (dwLocation) {
                *dwLocation = *dwLocation | 5; // ResourceLocation.embedded |

                                               // ResourceLocation.containedInManifestFile
                return TRUE;
            }

            pPEFile->GetEmbeddedResource(dwOffset, cbResource, pbInMemoryResource);

            return TRUE;
        }

        // The resource is either linked or embedded in a non-manifest-containing file
        if (pDomainAssembly == NULL)
            return FALSE;

        return pDomainAssembly->GetModuleResource(mdLinkRef, szName, cbResource,
                                                  pbInMemoryResource, szFileName,
                                                  dwLocation, IsMrPublic(dwResourceFlags),
                                                  pStackMark, fSkipSecurityCheck);

    default:
        ThrowHR(COR_E_BADIMAGEFORMAT, BFA_INVALID_TOKEN_IN_MANIFESTRES);
    }
}

ULONG PEFile::GetILImageTimeDateStamp()
{
    WRAPPER_CONTRACT;


    return GetLoadedIL()->GetTimeDateStamp();
}



// ================================================================================
// PEAssembly class - a PEFile which represents an assembly
// ================================================================================

PEAssembly::PEAssembly(PEImage *image, IMetaDataEmit *pEmit,
                       IAssembly *pIAssembly, IAssembly* pNativeFusionAssembly,
                       IFusionBindLog *pFusionLog, IHostAssembly *pIHostAssembly,
                       PEFile *creator, BOOL system, BOOL introspectionOnly/*=FALSE*/)
  : PEFile(image, FALSE),
    m_creator(NULL),
    m_pAssemblyImporter(NULL),
    m_pFusionAssemblyName(NULL),
    m_pFusionAssembly(NULL),
    m_pFusionLog(NULL),
    m_pIHostAssembly(NULL),
    m_pNativeFusionAssembly(NULL),
    m_pNativeImageClosure(NULL)
{
    CONTRACTL
    {
        CONSTRUCTOR_CHECK;
        PRECONDITION(CheckPointer(image, NULL_OK));
        PRECONDITION(CheckPointer(pEmit, NULL_OK));
        PRECONDITION(image != NULL || pEmit != NULL);
        PRECONDITION(CheckPointer(pIAssembly, NULL_OK));
        PRECONDITION(CheckPointer(pFusionLog, NULL_OK));
        PRECONDITION(CheckPointer(pIHostAssembly, NULL_OK));
        PRECONDITION(CheckPointer(creator, NULL_OK));
        THROWS;
        GC_TRIGGERS; // Fusion uses crsts on AddRef/Release
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (introspectionOnly)
    {
        if (!system)  // Implementation restriction: mscorlib.dll cannot be loaded as introspection. The architecture depends on there being exactly one mscorlib.
        {
            m_flags |= PEFILE_INTROSPECTIONONLY;
            SetCannotUseNativeImage();
        }
    }

    if (pIAssembly)
    {
        m_pFusionAssembly = pIAssembly;
        pIAssembly->AddRef();

        IfFailThrow(pIAssembly->GetAssemblyNameDef(&m_pFusionAssemblyName));
    }
    else if (pIHostAssembly)
    {
        m_flags |= PEFILE_ISTREAM;
        m_fCanUseNativeImage = FALSE;

        m_pIHostAssembly = pIHostAssembly;
        pIHostAssembly->AddRef();

        IfFailThrow(pIHostAssembly->GetAssemblyNameDef(&m_pFusionAssemblyName));
    }

    if (pFusionLog)
    {
        m_pFusionLog = pFusionLog;
        pFusionLog->AddRef();
    }

    if (creator)
    {
        m_creator = creator;
        creator->AddRef();
    }

    m_flags |= PEFILE_ASSEMBLY;
    if (system)
        m_flags |= PEFILE_SYSTEM;


    // If we have no native image, we require a mapping for the file.
    if (!HasNativeImage() || !IsILOnly())
        EnsureImageOpened();

    // Check security related stuff
    CheckSecurity();

    // Open metadata eagerly to minimize failure windows
    if (pEmit == NULL)
        OpenMDImport_Unsafe(); //constructor, cannot race with anything
    else
    {
        _ASSERTE(!m_bHasPersistentMDImport);
        IfFailThrow(GetMetaDataInternalInterfaceFromPublic(pEmit, IID_IMDInternalImport,
                                                           (void **)&m_pMDImport));
        m_pEmitter = pEmit;
        pEmit->AddRef();
        m_bHasPersistentMDImport=TRUE;
    }

    // Make sure this is an assembly
    if (!m_pMDImport->IsValidToken(TokenFromRid(1, mdtAssembly)))
        ThrowHR(COR_E_ASSEMBLYEXPECTED);

    // Verify name eagerly
    LPCUTF8 szName = GetSimpleName();
    if (!*szName)
    {
        ThrowHR(COR_E_BADIMAGEFORMAT, BFA_EMPTY_ASSEMDEF_NAME);
    }

    if (IsResource() || IsDynamic())
        m_fCanUseNativeImage = FALSE;

     if (m_pFusionAssembly) {
         m_loadContext = m_pFusionAssembly->GetFusionLoadContext();
         m_pFusionAssembly->GetAssemblyLocation(&m_dwLocationFlags);
     }

#if _DEBUG
    GetCodeBaseOrName(m_debugName);
    m_debugName.Normalize();
    m_pDebugName = m_debugName;
#endif
}

PEAssembly::~PEAssembly()
{
    CONTRACTL
    {
        DESTRUCTOR_CHECK;
        NOTHROW;
        GC_TRIGGERS; // Fusion uses crsts on AddRef/Release
        MODE_ANY;
    }
    CONTRACTL_END;

    GCX_PREEMP();
    if (m_pFusionAssemblyName != NULL)
        m_pFusionAssemblyName->Release();
    if (m_pFusionAssembly != NULL)
        m_pFusionAssembly->Release();
    if (m_pIHostAssembly != NULL)
        m_pIHostAssembly->Release();
    if (m_pNativeFusionAssembly != NULL)
        m_pNativeFusionAssembly->Release();
    if (m_pNativeImageClosure!=NULL)
        m_pNativeImageClosure->Release();
    if (m_pFusionLog != NULL)
        m_pFusionLog->Release();

    if (m_creator != NULL)
        m_creator->Release();
}

#ifndef  DACCESS_COMPILE
void PEAssembly::ReleaseIL()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_TRIGGERS; 
        MODE_ANY;
    }
    CONTRACTL_END;

    GCX_PREEMP();
    if (m_pFusionAssemblyName != NULL)
    {
        m_pFusionAssemblyName->Release();
        m_pFusionAssemblyName=NULL;
    }
    if (m_pFusionAssembly != NULL)
    {
        m_pFusionAssembly->Release();
        m_pFusionAssembly=NULL;
    }
    if (m_pIHostAssembly != NULL)
    {
        m_pIHostAssembly->Release();
        m_pIHostAssembly=NULL;
    }
    if (m_pNativeFusionAssembly != NULL)
    {
        m_pNativeFusionAssembly->Release();
        m_pNativeFusionAssembly=NULL;
    }
    _ASSERTE(m_pNativeImageClosure==NULL);
    
    if (m_pFusionLog != NULL)
    {
        m_pFusionLog->Release();
        m_pFusionLog=NULL;
    }

    if (m_creator != NULL)
    {
        m_creator->Release();
        m_creator=NULL;
    }

    PEFile::ReleaseIL();
}
#endif 

/* static */
PEAssembly *PEAssembly::OpenSystem()
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;

    PEAssembly *result = NULL;

    EX_TRY
    {
        result = DoOpenSystem();
    }
    EX_HOOK
    {
        Exception *ex = GET_EXCEPTION();

        // Rethrow non-transient exceptions as file load exceptions with proper
        // context

        if (!ex->IsTransient())
            EEFileLoadException::Throw(SystemDomain::System()->BaseLibrary(), ex->GetHR(), ex);
    }
    EX_END_HOOK;
    return result;
}

/* static */
PEAssembly *PEAssembly::DoOpenSystem()
{
    CONTRACT(PEAssembly *)
    {
        POSTCONDITION(CheckPointer(RETVAL));
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    MEMORY_REPORT_CONTEXT_SCOPE("PEAssembly");

    GCX_PREEMP();

    SafeComHolder<IAssemblyName> pName;
    IfFailThrow(CreateAssemblyNameObject(&pName, L"mscorlib", 0, NULL));

    UINT64 publicKeyValue = I64(CONCAT_MACRO(0x, VER_ECMA_PUBLICKEY));
    BYTE publicKeyToken[8] =
        {
            (BYTE) (publicKeyValue>>56),
            (BYTE) (publicKeyValue>>48),
            (BYTE) (publicKeyValue>>40),
            (BYTE) (publicKeyValue>>32),
            (BYTE) (publicKeyValue>>24),
            (BYTE) (publicKeyValue>>16),
            (BYTE) (publicKeyValue>>8),
            (BYTE) (publicKeyValue),
        };

    IfFailThrow(pName->SetProperty(ASM_NAME_PUBLIC_KEY_TOKEN, publicKeyToken, sizeof(publicKeyToken)));

    USHORT version = VER_ASSEMBLYMAJORVERSION;
    IfFailThrow(pName->SetProperty(ASM_NAME_MAJOR_VERSION, &version, sizeof(version)));
    version = VER_ASSEMBLYMINORVERSION;
    IfFailThrow(pName->SetProperty(ASM_NAME_MINOR_VERSION, &version, sizeof(version)));
    version = VER_ASSEMBLYBUILD;
    IfFailThrow(pName->SetProperty(ASM_NAME_BUILD_NUMBER, &version, sizeof(version)));
    version = VER_ASSEMBLYBUILD_QFE;
    IfFailThrow(pName->SetProperty(ASM_NAME_REVISION_NUMBER, &version, sizeof(version)));

    IfFailThrow(pName->SetProperty(ASM_NAME_CULTURE, L"", sizeof(WCHAR)));


    SafeComHolder<IAssembly> pIAssembly;
    SafeComHolder<IAssembly> pNativeFusionAssembly;

    {
        ETWTraceStartup trace(ETW_TYPE_STARTUP_FUSION_BINDING);
        IfFailThrow(BindToSystem(pName, SystemDomain::System()->SystemDirectory(), NULL, &pIAssembly, &pNativeFusionAssembly,NULL));
    }

    StackSString path;
    FusionBind::GetAssemblyManifestModulePath(pIAssembly, path);

    // Open the image with no required mapping.  This will be
    // promoted to a real open if we don't have a native image.
    PEImageHolder image (PEImage::OpenImage(path));

    RETURN new PEAssembly(image, NULL, pIAssembly,pNativeFusionAssembly,NULL, NULL, NULL, TRUE, FALSE);
}

/* static */
PEAssembly *PEAssembly::Open(IAssembly *pIAssembly,
                             IAssembly* pNativeFusionAssembly,
                             IFusionBindLog *pFusionLog/*=NULL*/,
                             BOOL isSystemAssembly/*=FALSE*/,
                             BOOL isIntrospectionOnly/*=FALSE*/)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;

    PEAssembly *result = NULL;
    EX_TRY
    {
        result = DoOpen(pIAssembly, pNativeFusionAssembly, pFusionLog, isSystemAssembly, isIntrospectionOnly);
    }
    EX_HOOK
    {
        Exception *ex = GET_EXCEPTION();

        // Rethrow non-transient exceptions as file load exceptions with proper
        // context
        if (!ex->IsTransient())
            EEFileLoadException::Throw(pIAssembly, NULL, ex->GetHR(), ex);
    }
    EX_END_HOOK;

    return result;
}

// Thread stress
class DoOpenIAssemblyStress : APIThreadStress
{
public:
    IAssembly *pIAssembly;
    IAssembly* pNativeFusionAssembly;
    IFusionBindLog *pFusionLog;
    DoOpenIAssemblyStress(IAssembly *pIAssembly, IAssembly* pNativeFusionAssembly, IFusionBindLog *pFusionLog)
          : pIAssembly(pIAssembly), pNativeFusionAssembly(pNativeFusionAssembly),pFusionLog(pFusionLog) {LEAF_CONTRACT;}
    void Invoke()
    {
        WRAPPER_CONTRACT;
        PEAssemblyHolder result (PEAssembly::Open(pIAssembly, pNativeFusionAssembly, pFusionLog, FALSE, FALSE));
    }
};

/* static */
PEAssembly *PEAssembly::DoOpen(IAssembly *pIAssembly,
                               IAssembly* pNativeFusionAssembly,
                               IFusionBindLog *pFusionLog,
                               BOOL isSystemAssembly,
                               BOOL isIntrospectionOnly/*=FALSE*/)
{
    CONTRACT(PEAssembly *)
    {
        PRECONDITION(CheckPointer(pIAssembly));
        POSTCONDITION(CheckPointer(RETVAL));
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    MEMORY_REPORT_CONTEXT_SCOPE("PEAssembly");

    DoOpenIAssemblyStress ts(pIAssembly,pNativeFusionAssembly,pFusionLog);

    PEImageHolder image;

    StackSString path;
    FusionBind::GetAssemblyManifestModulePath(pIAssembly, path);

    // Open the image with no required mapping.  This will be
    // promoted to a real open if we don't have a native image.
    image = PEImage::OpenImage(path);

    PEAssemblyHolder assembly (new PEAssembly(image, NULL, pIAssembly, pNativeFusionAssembly, pFusionLog,
                                               NULL, NULL, isSystemAssembly, isIntrospectionOnly));

    RETURN assembly.Extract();
}

/* static */
PEAssembly *PEAssembly::Open(IHostAssembly *pIHostAssembly, BOOL isSystemAssembly, BOOL isIntrospectionOnly)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;

    PEAssembly *result = NULL;

    EX_TRY
    {
        result = DoOpen(pIHostAssembly, isSystemAssembly, isIntrospectionOnly);
    }
    EX_HOOK
    {
        Exception *ex = GET_EXCEPTION();

        // Rethrow non-transient exceptions as file load exceptions with proper
        // context

        if (!ex->IsTransient())
            EEFileLoadException::Throw(NULL, pIHostAssembly, ex->GetHR(), ex);
    }
    EX_END_HOOK;
    return result;
}

// Thread stress
class DoOpenIHostAssemblyStress : APIThreadStress
{
public:
    IHostAssembly *pIHostAssembly;
    DoOpenIHostAssemblyStress(IHostAssembly *pIHostAssembly) :
        pIHostAssembly(pIHostAssembly) {LEAF_CONTRACT;}
    void Invoke()
    {
        WRAPPER_CONTRACT;
        PEAssemblyHolder result (PEAssembly::Open(pIHostAssembly, FALSE, FALSE));
    }
};

/* static */
PEAssembly *PEAssembly::DoOpen(IHostAssembly *pIHostAssembly, BOOL isSystemAssembly,
                               BOOL isIntrospectionOnly)
{
    CONTRACT(PEAssembly *)
    {
        PRECONDITION(CheckPointer(pIHostAssembly));
        POSTCONDITION(CheckPointer(RETVAL));
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    MEMORY_REPORT_CONTEXT_SCOPE("PEAssembly");

    DoOpenIHostAssemblyStress ts(pIHostAssembly);

    UINT64 AssemblyId;
    IfFailThrow(pIHostAssembly->GetAssemblyId(&AssemblyId));

    PEImageHolder image(PEImage::FindById(AssemblyId, 0));

    PEAssemblyHolder assembly (new PEAssembly(image, NULL, NULL, NULL, NULL,
                                              pIHostAssembly, NULL, isSystemAssembly, isIntrospectionOnly));

    RETURN assembly.Extract();
}

/* static */
PEAssembly *PEAssembly::OpenMemory(PEAssembly *pParentAssembly,
                                   const void *flat, COUNT_T size,
                                   BOOL isIntrospectionOnly/*=FALSE*/)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;

    PEAssembly *result = NULL;

    EX_TRY
    {
        result = DoOpenMemory(pParentAssembly, flat, size, isIntrospectionOnly);
    }
    EX_HOOK
    {
        Exception *ex = GET_EXCEPTION();

        // Rethrow non-transient exceptions as file load exceptions with proper
        // context

        if (!ex->IsTransient())
            EEFileLoadException::Throw(pParentAssembly, flat, size, ex->GetHR(), ex);
    }
    EX_END_HOOK;

    return result;
}

// Thread stress
class DoOpenFlatStress : APIThreadStress
{
public:
    PEAssembly *pParentAssembly;
    const void *flat;
    COUNT_T size;
    DoOpenFlatStress(PEAssembly *pParentAssembly, const void *flat, COUNT_T size)
        : pParentAssembly(pParentAssembly), flat(flat), size(size) {LEAF_CONTRACT;}
    void Invoke()
    {
        WRAPPER_CONTRACT;
        PEAssemblyHolder result(PEAssembly::OpenMemory(pParentAssembly, flat, size, FALSE));
    }
};

/* static */
PEAssembly *PEAssembly::DoOpenMemory(PEAssembly *pParentAssembly,
                                   const void *flat, COUNT_T size, BOOL isIntrospectionOnly)
{
    CONTRACT(PEAssembly *)
    {
        PRECONDITION(CheckPointer(flat));
        PRECONDITION(CheckOverflow(flat, size));
        PRECONDITION(CheckPointer(pParentAssembly));
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    MEMORY_REPORT_CONTEXT_SCOPE("PEAssembly");

    // Thread stress
    DoOpenFlatStress ts(pParentAssembly, flat, size);

    // Note that we must have a flat image stashed away for two reasons.
    // First, we need a private copy of the data which we can verify
    // before doing the mapping.  And secondly, we can only compute
    // the strong name hash on a flat image.

    PEImageHolder image(PEImage::LoadFlat(flat, size));

    // Need to verify that this is a CLR assembly
    if (!image->CheckILFormat())
        ThrowHR(COR_E_BADIMAGEFORMAT, BFA_BAD_IL);

    RETURN new PEAssembly(image, NULL, NULL, NULL, NULL,NULL, pParentAssembly, FALSE, isIntrospectionOnly);
}


/* static */
PEAssembly *PEAssembly::Create(PEAssembly *pParentAssembly,
                               IMetaDataAssemblyEmit *pAssemblyEmit,
                               BOOL bIsIntrospectionOnly)
{
    CONTRACT(PEAssembly *)
    {
        PRECONDITION(CheckPointer(pParentAssembly));
        PRECONDITION(CheckPointer(pAssemblyEmit));
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    // Set up the metadata pointers in the PEAssembly. (This is the only identity
    // we have.)
    SafeComHolder<IMetaDataEmit> pEmit;
    pAssemblyEmit->QueryInterface(IID_IMetaDataEmit, (void **)&pEmit);

    PEAssemblyHolder pFile(new PEAssembly(NULL, pEmit, NULL, NULL, NULL, NULL, pParentAssembly, FALSE, bIsIntrospectionOnly));

    RETURN pFile.Extract();
}


BOOL PEAssembly::IsBindingCodeBase()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (m_pIHostAssembly)
        return FALSE;

    if (!m_pFusionAssembly)
        return (!GetPath().IsEmpty());

    if (m_dwLocationFlags == ASMLOC_UNKNOWN)
        return FALSE;

    return ((m_dwLocationFlags & ASMLOC_CODEBASE_HINT) != 0);
}

BOOL PEAssembly::IsSourceGAC()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (m_pIHostAssembly || (!m_pFusionAssembly))
        return FALSE;

    return ((m_dwLocationFlags & ASMLOC_LOCATION_MASK) == ASMLOC_GAC);
}

BOOL PEAssembly::IsSourceDownloadCache()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (m_pIHostAssembly || (!m_pFusionAssembly))
        return FALSE;

    return ((m_dwLocationFlags & ASMLOC_LOCATION_MASK) == ASMLOC_DOWNLOAD_CACHE);
}

BOOL PEAssembly::IsContextLoadFrom()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (m_pIHostAssembly || (!m_pFusionAssembly))
        return FALSE;

    return (m_loadContext == LOADCTX_TYPE_LOADFROM);
}

#endif // #ifndef DACCESS_COMPILE

LOADCTX_TYPE PEAssembly::GetLoadContext()
{
    LEAF_CONTRACT;

    return m_loadContext;
}

DWORD PEAssembly::GetLocationFlags()
{
    LEAF_CONTRACT;

    return m_dwLocationFlags;
}

#ifndef DACCESS_COMPILE

IAssemblyName *PEAssembly::GetFusionAssemblyName()
{
    CONTRACT(IAssemblyName *)
    {
        INSTANCE_CHECK;
        POSTCONDITION(CheckPointer(RETVAL));
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    if (m_pFusionAssemblyName == NULL)
    {
        AssemblySpec spec;
        spec.InitializeSpec(this);
        PEImage* pImage = GetILimage();
        if(pImage != NULL)
        {
            spec.SetPAFlags(pImage->GetFusionProcessorArchitecture());
        }

        GCX_PREEMP();

        IfFailThrow(spec.CreateFusionName(&m_pFusionAssemblyName, FALSE));
    }

    RETURN m_pFusionAssemblyName;
}

IAssembly *PEAssembly::GetFusionAssembly()
{
    CONTRACT(IAssembly *)
    {
        INSTANCE_CHECK;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_END;

    RETURN m_pFusionAssembly;
}

IAssembly *PEAssembly::GetNativeAssembly()
{
    CONTRACT(IAssembly *)
    {
        INSTANCE_CHECK;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_END;

    RETURN m_pNativeFusionAssembly;
}


IHostAssembly *PEAssembly::GetIHostAssembly()
{
    CONTRACT(IHostAssembly *)
    {
        INSTANCE_CHECK;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_END;

    RETURN m_pIHostAssembly;
}

IAssembly *PEAssembly::GetNativeFusionAssembly()
{
    CONTRACT(IAssembly *)
    {
        INSTANCE_CHECK;
        PRECONDITION(HasNativeImage());
        POSTCONDITION(CheckPointer(RETVAL));
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_END;

    RETURN m_pNativeFusionAssembly;
}

// ------------------------------------------------------------
// Hash support
// ------------------------------------------------------------

void PEAssembly::CheckSecurity()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    ETWTraceStartup trace(ETW_TYPE_STARTUP_SECURITYCATCHALL);

    // System and dynamic assemblies don't need hash checks
    if (IsSystem() || IsDynamic())
    {
        m_flags |= PEFILE_SKIP_MODULE_HASH_CHECKS;
        return;
    }

    PEFile::CheckSecurity();

    // Next, verify the strong name, if necessary

    // See if the assembly comes from a secure location
    IAssembly *pFusionAssembly = GetAssembly()->GetFusionAssembly();
    if (pFusionAssembly)
    {
        DWORD dwLocation;
        IfFailThrow(pFusionAssembly->GetAssemblyLocation(&dwLocation));

        switch (dwLocation & ASMLOC_LOCATION_MASK)
        {
        case ASMLOC_GAC:
        case ASMLOC_DOWNLOAD_CACHE:
        case ASMLOC_DEV_OVERRIDE:
            // Assemblies from the GAC or download cache have
            // already been verified by Fusion.
            m_flags |= PEFILE_SKIP_MODULE_HASH_CHECKS;
            return;

        case ASMLOC_RUN_FROM_SOURCE:
        case ASMLOC_UNKNOWN:
            // For now, just verify these every time, we need to
            // cache the fact that at least one verification has
            // been performed (if strong name policy permits
            // caching of verification results)
            break;

        default:
            UNREACHABLE();
        }
    }

    // Check format of image. Note we must delay this until after the GAC status has been
    // checked, to handle the case where we are not loading m_image.
    EnsureImageOpened();
    if (!m_ILimage->CheckILFormat())
        ThrowHR(COR_E_BADIMAGEFORMAT);

    // Check the strong name if present.
    if (IsIntrospectionOnly())
    {
        // For introspection assemblies, we don't need to check strong names and we don't
        // need to do module hash checks.
        m_flags |= PEFILE_SKIP_MODULE_HASH_CHECKS;
    }
    else
    {
        DWORD verifyOutputFlags;
        HRESULT hr=m_ILimage->VerifyStrongName(&verifyOutputFlags);
        if (SUCCEEDED(hr))
        {
            // Strong name verified or delay sign OK'ed.
            // We will skip verification of modules in the delay signed case.

            if ((verifyOutputFlags & SN_OUTFLAG_WAS_VERIFIED) == 0)
                m_flags |= PEFILE_SKIP_MODULE_HASH_CHECKS;
        }
        else
        {
            // Strong name missing or error.  Throw in the latter case.
            if (hr != CORSEC_E_MISSING_STRONGNAME)
                ThrowHR(hr);

            // Since we are not strong named, don't check module hashes.
            // (Unless we have a security certificate, in which case check anyway.)

            if (m_certificate == NULL)
                m_flags |= PEFILE_SKIP_MODULE_HASH_CHECKS;
        }
    }
}

void PEAssembly::GetIdentitySignature(SBuffer &result)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckValue(result));
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;


    if (HasStrongNameSignature())
        GetEffectiveStrongNameSignature(result);
    else
        GetSHA1Hash(result);
}

// ------------------------------------------------------------
// Descriptive strings
// ------------------------------------------------------------

// Effective path is the path of nearest parent (creator) assembly which has a nonempty path.

const SString &PEAssembly::GetEffectivePath()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    PEAssembly *pAssembly = this;

    while (pAssembly->m_identity == NULL
           || pAssembly->m_identity->GetPath().IsEmpty())
    {
        if (pAssembly->m_creator)
            pAssembly = pAssembly->m_creator->GetAssembly();
        else // Unmanaged exe which loads byte[]/IStream assemblies
            return SString::Empty();
    }

    return pAssembly->m_identity->GetPath();
}


// Codebase is the fusion codebase or path for the assembly.  It is in URL format.
// Note this may be obtained from the parent PEFile if we don't have a path or fusion
// assembly.
//
// fCopiedName means to get the "shadow copied" path rather than the original path, if applicable
void PEAssembly::GetCodeBase(SString &result, BOOL fCopiedName/*=FALSE*/)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // For a copied name, we always use the actual file path rather than the fusion info
    if (!fCopiedName && m_pFusionAssembly)
    {
        if ( ((m_dwLocationFlags & ASMLOC_LOCATION_MASK) == ASMLOC_RUN_FROM_SOURCE) ||
             ((m_dwLocationFlags & ASMLOC_LOCATION_MASK) == ASMLOC_DOWNLOAD_CACHE) )
        {
            // Assemblies in the download cache or run from source should have
            // a proper codebase set in them.
            FusionBind::GetAssemblyNameStringProperty(GetFusionAssemblyName(),
                                                      ASM_NAME_CODEBASE_URL,
                                                      result);
            return;
        }
    }
    else if (m_pIHostAssembly)
    {
        FusionBind::GetAssemblyNameStringProperty(GetFusionAssemblyName(),
                                                  ASM_NAME_CODEBASE_URL,
                                                  result);
        return;
    }

    // All other cases use the file path.
    result.Set(GetEffectivePath());
    if (!result.IsEmpty())
        PathToUrl(result);
}

/* static */
void PEAssembly::PathToUrl(SString &string)
{
    CONTRACTL
    {
        PRECONDITION(PEImage::CheckCanonicalFullPath(string));
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    SString::Iterator i = string.Begin();

#if !defined(PLATFORM_UNIX)
    if (i[0] == L'\\')
    {
        // Network path
        string.Insert(i, SL("file://"));
        string.Skip(i, SL("file://"));
    }
    else
    {
        // Disk path
        string.Insert(i, SL("file:///"));
        string.Skip(i, SL("file:///"));
    }
#else
    // Unix doesn't have a distinction between a network or a local path
    _ASSERTE( i[0] == L'\\' || i[0] == L'/');
    string.Insert(i, SL("file://"));
    string.Skip(i, SL("file://"));
#endif

    while (string.Find(i, L'\\'))
    {
        string.Replace(i, L'/');
    }
}

void PEAssembly::UrlToPath(SString &string)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACT_END;

    SString::Iterator i = string.Begin();

#if !defined(PLATFORM_UNIX)
    if (string.MatchCaseInsensitive(i, SL("file:///")))
        string.Delete(i, 8);
    else
#endif
    if (string.MatchCaseInsensitive(i, SL("file://")))
        string.Delete(i, 7);

    while (string.Find(i, L'/'))
    {
        string.Replace(i, L'\\');
    }

    RETURN;
}

BOOL PEAssembly::FindLastPathSeparator(const SString &path, SString::Iterator &i)
{
#ifdef PLATFORM_UNIX
    SString::Iterator slash = i;
    SString::Iterator backSlash = i;
    BOOL foundSlash = path.FindBack(slash, '/');
    BOOL foundBackSlash = path.FindBack(backSlash, '\\');
    if (!foundSlash && !foundBackSlash)
        return FALSE;
    else if (foundSlash && !foundBackSlash)
        i = slash;
    else if (!foundSlash && foundBackSlash)
        i = backSlash;
    else
        i = (backSlash > slash) ? backSlash : slash;
    return TRUE;
#else
    return path.FindBack(i, '\\');
#endif //PLATFORM_UNIX
}


// ------------------------------------------------------------
// Logging
// ------------------------------------------------------------

void PEAssembly::ExternalVLog(DWORD level, const WCHAR *fmt, va_list args)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACT_END;

    PEFile::ExternalVLog(level, fmt, args);

    if (m_pFusionLog != NULL)
    {
        StackSString message;
        message.VPrintf(fmt, args);
        m_pFusionLog->LogMessage(0, FUSION_BIND_LOG_CATEGORY_NGEN, message);

        if (level == LL_ERROR) {
            m_pFusionLog->SetResultCode(FUSION_BIND_LOG_CATEGORY_NGEN, E_FAIL);
            FlushExternalLog();
        }
    }

    RETURN;
}

extern DWORD g_dwLogLevel;

void PEAssembly::FlushExternalLog()
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACT_END;

    if (m_pFusionLog != NULL) {
        m_pFusionLog->Flush(g_dwLogLevel, FUSION_BIND_LOG_CATEGORY_NGEN);
    }

    RETURN;
}

// ------------------------------------------------------------
// Metadata access
// ------------------------------------------------------------

void PEFile::GetVersion(USHORT *pMajor, USHORT *pMinor, USHORT *pBuild, USHORT *pRevision)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pMajor, NULL_OK));
        PRECONDITION(CheckPointer(pMinor, NULL_OK));
        PRECONDITION(CheckPointer(pBuild, NULL_OK));
        PRECONDITION(CheckPointer(pRevision, NULL_OK));
        NOTHROW;
        WRAPPER(GC_TRIGGERS);
        MODE_ANY;
    }
    CONTRACTL_END;

    AssemblyMetaDataInternal md;
    if (m_bHasPersistentMDImport)
    {
        _ASSERTE(GetPersistentMDImport()->IsValidToken(TokenFromRid(1, mdtAssembly)));
        GetPersistentMDImport()->GetAssemblyProps(TokenFromRid(1, mdtAssembly), NULL, NULL, NULL, NULL, &md, NULL);
    }
    else
    {
        ReleaseHolder<IMDInternalImport> pImport(GetMDImportWithRef());
        _ASSERTE(pImport->IsValidToken(TokenFromRid(1, mdtAssembly)));
        pImport->GetAssemblyProps(TokenFromRid(1, mdtAssembly), NULL, NULL, NULL, NULL, &md, NULL);
    }

    if (pMajor != NULL)
        *pMajor = md.usMajorVersion;
    if (pMinor != NULL)
        *pMinor = md.usMinorVersion;
    if (pBuild != NULL)
        *pBuild = md.usBuildNumber;
    if (pRevision != NULL)
        *pRevision = md.usRevisionNumber;
}

// ================================================================================
// PEModule class - a PEFile which represents a satellite module
// ================================================================================

PEModule::PEModule(PEImage *image, PEAssembly *assembly, mdFile token, IMetaDataEmit *pEmit)
  : PEFile(image),
    m_assembly(NULL),
    m_token(token),
    m_bIsResource(-1)
{
    CONTRACTL
    {
        PRECONDITION(CheckPointer(image, NULL_OK));
        PRECONDITION(CheckPointer(assembly));
        PRECONDITION(!IsNilToken(token));
        PRECONDITION(CheckPointer(pEmit, NULL_OK));
        PRECONDITION(image != NULL || pEmit != NULL);
        CONSTRUCTOR_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    DWORD flags;

    // get only the data which is required, here - flags
    // this helps avoid unnecessary memory touches
    assembly->GetPersistentMDImport()->GetFileProps(token, NULL, NULL, NULL, &flags);

    if (image)
    {
        if (IsFfContainsMetaData(flags) && !image->CheckILFormat())
        ThrowHR(COR_E_BADIMAGEFORMAT);

        if (assembly->IsIStream())
        {
            m_flags |= PEFILE_ISTREAM;
            m_fCanUseNativeImage = FALSE;
        }
    }

    assembly->AddRef();

    m_assembly = assembly;

    m_flags |= PEFILE_MODULE;
    if (assembly->IsSystem())
        m_flags |= PEFILE_SYSTEM;
    else
        if (assembly->IsIntrospectionOnly())
        {
            m_flags |= PEFILE_INTROSPECTIONONLY;
            SetCannotUseNativeImage();        
        }
        

    // Verify module format.  Note that some things have already happened:
    // - Fusion has verified the name matches the metadata
    // - PEimage has performed PE file format validation

    if (assembly->NeedsModuleHashChecks())
    {
        ULONG size;
        const void *hash;
        assembly->GetPersistentMDImport()->GetFileProps(token, NULL, &hash, &size, NULL);
        
        if (!CheckHash(assembly->GetHashAlgId(), hash, size))
            ThrowHR(COR_E_MODULE_HASH_CHECK_FAILED);
    }


#if _DEBUG
    GetCodeBaseOrName(m_debugName);
    m_pDebugName = m_debugName;
#endif

    if (IsFfContainsMetaData(flags))
    {
        if (image)
            OpenMDImport_Unsafe(); //constructor. cannot race with anything
        else
        {
            _ASSERTE(!m_bHasPersistentMDImport);
            IfFailThrow(GetMetaDataInternalInterfaceFromPublic(pEmit, IID_IMDInternalImport,
                                                               (void **)&m_pMDImport));
            m_pEmitter = pEmit;
            pEmit->AddRef();
            m_bHasPersistentMDImport=TRUE;
        }

        // Fusion probably checks this, but we need to check this ourselves if
        // this file didn't come from Fusion
        if (!m_pMDImport->IsValidToken(m_pMDImport->GetModuleFromScope()))
            COMPlusThrowHR(COR_E_BADIMAGEFORMAT);
    }
    else
    {
        // Go ahead and "load" image since it is essentially a noop, but will enable
        // more operations on the module earlier in the loading process.
        LoadLibrary();
    }

    if (IsResource() || IsDynamic())
        m_fCanUseNativeImage = FALSE;
}

PEModule::~PEModule()
{
    CONTRACTL
    {
        DESTRUCTOR_CHECK;
        NOTHROW;
        MODE_ANY;
    }
    CONTRACTL_END;

    m_assembly->Release();
}

/* static */
PEModule *PEModule::Open(PEAssembly *assembly, mdFile token,
                         const SString &fileName)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;

    PEModule *result = NULL;

    EX_TRY
    {
        result = DoOpen(assembly, token, fileName);
    }
    EX_HOOK
    {
        Exception *ex = GET_EXCEPTION();

        // Rethrow non-transient exceptions as file load exceptions with proper
        // context

        if (!ex->IsTransient())
            EEFileLoadException::Throw(fileName, ex->GetHR(), ex);
    }
    EX_END_HOOK;

    return result;
}

// Thread stress
class DoOpenPathStress : APIThreadStress
{
public:
    PEAssembly *assembly;
    mdFile token;
    const SString &fileName;
    DoOpenPathStress(PEAssembly *assembly, mdFile token,
           const SString &fileName)
        : assembly(assembly), token(token), fileName(fileName)
    {
        WRAPPER_CONTRACT;
        fileName.Normalize();
    }
    void Invoke()
    {
        WRAPPER_CONTRACT;
        PEModuleHolder result(PEModule::Open(assembly, token, fileName));
    }
};

/* static */
PEModule *PEModule::DoOpen(PEAssembly *assembly, mdFile token,
                           const SString &fileName)
{
    CONTRACT(PEModule *)
    {
        PRECONDITION(CheckPointer(assembly));
        PRECONDITION(CheckValue(fileName));
        PRECONDITION(!IsNilToken(token));
        PRECONDITION(!fileName.IsEmpty());
        POSTCONDITION(CheckPointer(RETVAL));
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    MEMORY_REPORT_CONTEXT_SCOPE("PEModule");

    DoOpenPathStress ts(assembly, token, fileName);

    // If this is a resource module, we must explicitly request a flat mapping
    DWORD flags;
    assembly->GetPersistentMDImport()->GetFileProps(token, NULL, NULL, NULL, &flags);

    PEImageHolder image;

    if (assembly->IsIStream())
    {
        SafeComHolder<IHostAssemblyModuleImport> pModuleImport;
        IfFailThrow(assembly->GetIHostAssembly()->GetModuleByName(fileName, &pModuleImport));

        SafeComHolder<IStream> pIStream;
        IfFailThrow(pModuleImport->GetModuleStream(&pIStream));

        DWORD dwModuleId;
        IfFailThrow(pModuleImport->GetModuleId(&dwModuleId));
        image = PEImage::OpenImage(pIStream, assembly->m_identity->m_StreamAsmId,
                                   dwModuleId, (flags & ffContainsNoMetaData));
    }
    else
        image = PEImage::OpenImage(fileName);

    if (flags & ffContainsNoMetaData)
        image->LoadNoMetaData(assembly->IsIntrospectionOnly());

    RETURN new PEModule(image, assembly, token, NULL);
}

/* static */
PEModule *PEModule::OpenMemory(PEAssembly *assembly, mdFile token,
                               const void *flat, COUNT_T size)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;

    PEModule *result = NULL;

    EX_TRY
    {
        result = DoOpenMemory(assembly, token, flat, size);
    }
    EX_HOOK
    {
        Exception *ex = GET_EXCEPTION();

        // Rethrow non-transient exceptions as file load exceptions with proper
        // context
        if (!ex->IsTransient())
            EEFileLoadException::Throw(assembly, flat, size, ex->GetHR(), ex);
    }
    EX_END_HOOK;
    return result;
}

// Thread stress
class DoOpenTokenStress : APIThreadStress
{
public:
    PEAssembly *assembly;
    mdFile token;
    const void *flat;
    COUNT_T size;
    DoOpenTokenStress(PEAssembly *assembly, mdFile token,
           const void *flat, COUNT_T size)
        : assembly(assembly), token(token), flat(flat), size(size) {LEAF_CONTRACT;}
    void Invoke()
    {
        WRAPPER_CONTRACT;
        PEModuleHolder result(PEModule::OpenMemory(assembly, token, flat, size));
    }
};

/* static */
PEModule *PEModule::DoOpenMemory(PEAssembly *assembly, mdFile token,
                                 const void *flat, COUNT_T size)
{
    CONTRACT(PEModule *)
    {
        PRECONDITION(CheckPointer(assembly));
        PRECONDITION(!IsNilToken(token));
        PRECONDITION(CheckPointer(flat));
        POSTCONDITION(CheckPointer(RETVAL));
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    MEMORY_REPORT_CONTEXT_SCOPE("PEModule");

    DoOpenTokenStress ts(assembly, token, flat, size);

    PEImageHolder image(PEImage::LoadFlat(flat, size));

    RETURN new PEModule(image, assembly, token, NULL);
}

/* static */
PEModule *PEModule::Create(PEAssembly *assembly, mdFile token, IMetaDataEmit *pEmit)
{
    CONTRACT(PEModule *)
    {
        PRECONDITION(CheckPointer(assembly));
        PRECONDITION(!IsNilToken(token));
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    RETURN new PEModule(NULL, assembly, token, pEmit);
}

// ------------------------------------------------------------
// Logging
// ------------------------------------------------------------

void PEModule::ExternalVLog(DWORD level, const WCHAR *fmt, va_list args)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACT_END;

    m_assembly->ExternalVLog(level, fmt, args);

    RETURN;
}

void PEModule::FlushExternalLog()
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACT_END;

    m_assembly->FlushExternalLog();

    RETURN;
}


void PEFile::EnsureImageOpened()
{
    WRAPPER_CONTRACT;
    if (IsDynamic())
        return;
    if(HasNativeImage())
        m_nativeImage->GetLayout(PEImageLayout::LAYOUT_ANY,PEImage::LAYOUT_CREATEIFNEEDED)->Release();
    else
        m_ILimage->GetLayout(PEImageLayout::LAYOUT_ANY,PEImage::LAYOUT_CREATEIFNEEDED)->Release();
}

#endif // #ifndef DACCESS_COMPILE

#ifdef DACCESS_COMPILE

void
PEFile::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;
    DAC_ENUM_VTHIS();
    EMEM_OUT(("MEM: %p PEFile\n", PTR_HOST_TO_TADDR(this)));

    if (m_identity.IsValid())
    {
        m_identity->EnumMemoryRegions(flags);
    }
    if (m_ILimage.IsValid())
    {
        m_ILimage->EnumMemoryRegions(flags);
    }
    if (m_nativeImage.IsValid())
    {
        m_nativeImage->EnumMemoryRegions(flags);
    }
}

void
PEAssembly::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;

    PEFile::EnumMemoryRegions(flags);

    if (m_creator.IsValid())
    {
        m_creator->EnumMemoryRegions(flags);
    }
}


void
PEModule::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;

    PEFile::EnumMemoryRegions(flags);

    if (m_assembly.IsValid())
    {
        m_assembly->EnumMemoryRegions(flags);
    }
}

#endif // #ifdef DACCESS_COMPILE


//-------------------------------------------------------------------------------
// Make best-case effort to obtain an image name for use in an error message.
//
// This routine must expect to be called before the this object is fully loaded.
// It can return an empty if the name isn't available or the object isn't initialized
// enough to get a name, but it mustn't crash.
//-------------------------------------------------------------------------------
void PEFile::GetPathForErrorMessages(SString & result)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    PEImage *pImage = GetILimage();
    if (pImage)
    {
        pImage->GetPathForErrorMessages(result);
    }
    else
    {
        result = L"";
    }
}
