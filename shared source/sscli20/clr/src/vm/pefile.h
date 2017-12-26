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
// PEFile.h
// --------------------------------------------------------------------------------

#ifndef PEFILE_H_
#define PEFILE_H_

// --------------------------------------------------------------------------------
// Required headers
// --------------------------------------------------------------------------------

#include <windef.h>

#include <fusion.h>
#include <fusionbind.h>
#include <corpolicy.h>
#include "sstring.h"
#include "peimage.h"
#include "eecontract.h"
#include "metadatatracker.h"
#include "stackwalktypes.h"
#include <specstrings.h>

// --------------------------------------------------------------------------------
// Forward declared classes
// --------------------------------------------------------------------------------

class Module;
class EditAndContinueModule;

class PEFile;
class PEModule;
class PEAssembly;
class SimpleRWLock;

// --------------------------------------------------------------------------------
// Types
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// A PEFile is an input to the CLR loader.  It is produced as a result of 
// binding, usually through fusion (although there are a few less common methods to 
// obtain one which do not go through fusion, e.g. IJW loads)
//
// Although a PEFile is usually a disk based PE file (hence the name), it is not 
// always the case. Thus it is a conscious decision to not export access to the PE
// file directly; rather the specific information required should be provided via
// individual query API.
//
// There are multiple "flavors" of PEFiles:
//
// 1. HMODULE - these PE Files are loaded in response to "spontaneous" OS callbacks.
//    These should only occur for .exe main modules and IJW dlls loaded via LoadLibrary
//    or static imports in umnanaged code.
//
// 2. Fusion loads - these are the most common case.  A path is obtained from fusion and
//    the result is loaded via PEImage.
//      a. Display name loads - these are metadata-based binds
//      b. Path loads - these are loaded from an explicit path
//
// 3. Byte arrays - loaded explicitly by user code.  These also go through PEImage.
//
// 4. Dynamic - these are not actual PE images at all, but are placeholders
//    for reflection-based modules.
//
// PEFiles are segmented into two subtypes: PEAssembly and PEModule.  The formere
// is a file to be loaded as an assembly, and the latter is to be loaded as a module.
// --------------------------------------------------------------------------------

typedef VPTR(class PEFile) PTR_PEFile;


class PEFile
{
    // ------------------------------------------------------------
    // SOS support
    // ------------------------------------------------------------

    VPTR_BASE_CONCRETE_VTABLE_CLASS(PEFile)

public:

    // ------------------------------------------------------------
    // Public API
    // ------------------------------------------------------------

    void AddRef();
    ULONG Release();

#ifdef DACCESS_COMPILE
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

#if CHECK_INVARIANTS
    CHECK Invariant();
#endif

 private:
    // ------------------------------------------------------------
    // Loader access API
    // ------------------------------------------------------------

    friend class DomainFile;
    friend class PEModule;

    // Load actually triggers loading side effects of the module.  This should ONLY
    // be done after validation has been passed
    BOOL CanLoadLibrary();
public:
    void LoadLibrary(BOOL allowNativeSkip = TRUE);
private:

    BOOL HasSkipVerification();
    void SetSkipVerification();

    // DO NOT USE !!! this is to be removed when we move to new fusion binding API
    friend class DomainAssembly;

    // Helper for creating metadata for CreateDynamic
    friend class Assembly;
    friend class COMDynamicWrite;
    friend class AssemblyNative;
    static void DefineEmitScope(GUID iid, void **ppEmit);
 protected:    
    IMDInternalImport* GetMDImport()
    {
        WRAPPER_CONTRACT;
        return GetPersistentMDImport();
    };
 public:

    // ------------------------------------------------------------
    // Generic PEFile - can be used to access metadata
    // ------------------------------------------------------------
    
    static PEFile *Open(PEImage *image);

    // ------------------------------------------------------------
    // Identity
    // ------------------------------------------------------------

#ifndef DACCESS_COMPILE
    BOOL Equals(PEFile *pFile);
    BOOL Equals(HMODULE hMod);
    BOOL Equals(PEImage *pImage);
#endif // DACCESS_COMPILE

    ULONG HashIdentity();
    BOOL  IsShareable();

    void GetMVID(GUID *pMvid);

    // ------------------------------------------------------------
    // Descriptive strings
    // ------------------------------------------------------------

    // Path is the file path to the file; empty if not a file
    const SString &GetPath();

#ifdef DACCESS_COMPILE
    // This is the metadata module name. Used as a hint as file name. 
    const SString &GetModuleFileNameHint();
#endif // DACCESS_COMPILE

    // Full name is the most descriptive name available (path, codebase, or name as appropriate)
    void GetCodeBaseOrName(SString &result);
    
    // Returns security information for the assembly based on the codebase
    void GetSecurityIdentity(SString &odebase, DWORD *pdwZone, BYTE *pbUniqueID, DWORD *pcbUniqueID);
    void InitializeSecurityManager();

#ifdef LOGGING
    // This is useful for log messages
    LPCWSTR GetDebugName();
#endif

    // ------------------------------------------------------------
    // Checks
    // ------------------------------------------------------------

    CHECK CheckLoaded(BOOL allowNativeSkip = TRUE);

    // ------------------------------------------------------------
    // Classification
    // ------------------------------------------------------------

    BOOL IsAssembly();
    BOOL IsModule();
    BOOL IsSystem();
    BOOL IsDynamic();
    BOOL IsResource();
    BOOL IsIStream();
    BOOL IsIntrospectionOnly();
    // Returns self (if assembly) or containing assembly (if module)
    PEAssembly *GetAssembly();

    // ------------------------------------------------------------
    // Hash support
    // ------------------------------------------------------------

#ifndef DACCESS_COMPILE
    void GetImageBits(SBuffer &result);
    void GetHash(ALG_ID algorithm, SBuffer &result);
#endif // DACCESS_COMPILE

    void GetSHA1Hash(SBuffer &result);
    CHECK CheckHash(ALG_ID algorithm, const void *hash, COUNT_T size);

    // ------------------------------------------------------------
    // Metadata access
    // ------------------------------------------------------------

    BOOL HasMetadata();

    IMDInternalImport *GetPersistentMDImport();
    IMDInternalImport *GetMDImportWithRef();    
    void MakeMDImportPersistent() {m_bHasPersistentMDImport=TRUE;};

#ifndef DACCESS_COMPILE
    IMetaDataImport2 *GetRWImporter2();
    IMetaDataEmit *GetEmitter();
    IMetaDataAssemblyEmit *GetAssemblyEmitter();
    IMetaDataImport *GetRWImporter();
    IMetaDataAssemblyImport *GetAssemblyImporter();
#endif // DACCESS_COMPILE

    LPCUTF8 GetSimpleName();
    BOOL IsStrongNamed();
    const void *GetPublicKey(DWORD *pcbPK);
    ULONG GetHashAlgId();
    void GetVersion(USHORT *pMajor, USHORT *pMinor, USHORT *pBuild, USHORT *pRevision);
    LPCSTR GetLocale();
    DWORD GetFlags();

    COR_TRUST *GetSecuritySignature();

    // ------------------------------------------------------------
    // PE file access
    // ------------------------------------------------------------

    BOOL HasSecurityDirectory();
    WORD GetSubsystem();
    mdToken GetEntryPointToken(
#ifdef _DEBUG        
        BOOL bAssumeLoaded = FALSE
#endif //_DEBUG               
        );
    BOOL IsILOnly();
    BOOL IsDll();

    const void *GetIL(RVA il);
    CHECK CheckIL(RVA il);
    CHECK CheckIL(RVA il, COUNT_T size);
    RVA GetPhonyILRva(const void *pIL);
    BYTE *GetPhonyILBase();

    void *GetRvaField(RVA field);
    CHECK CheckRvaField(RVA field);
    CHECK CheckRvaField(RVA field, COUNT_T size);

    PCCOR_SIGNATURE GetSignature(RVA signature);
    RVA GetSignatureRva(PCCOR_SIGNATURE signature);
    CHECK CheckSignature(PCCOR_SIGNATURE signature);
    CHECK CheckSignatureRva(RVA signature);

    BOOL HasTls();
    BOOL IsRvaFieldTls(RVA field);
    UINT32 GetFieldTlsOffset(RVA field);
    UINT32 GetTlsIndex();

    const void *GetInternalPInvokeTarget(RVA target);
    CHECK CheckInternalPInvokeTarget(RVA target);

    IMAGE_COR_VTABLEFIXUP *GetVTableFixups(COUNT_T *pCount = NULL);
    void *GetVTable(RVA rva);

    BOOL GetResource(LPCSTR szName, DWORD *cbResource,
                     PBYTE *pbInMemoryResource, DomainAssembly** pAssemblyRef,
                     LPCSTR *szFileName, DWORD *dwLocation, 
                     StackCrawlMark *pStackMark, BOOL fSkipSecurityCheck,
                     BOOL fSkipRaiseResolveEvent, DomainAssembly* pDomainAssembly,
                     AppDomain* pAppDomain);

    const void *GetMetadata(COUNT_T *pSize);
    const void *GetLoadedMetadata(COUNT_T *pSize);

    ULONG GetILImageTimeDateStamp();

    // ------------------------------------------------------------
    // Image memory access
    //
    // WARNING: do not abuse these.  There are scenarios where the image
    // is not in memory as an optimization.  
    //
    // In general, you should add an entry point to get the specific info 
    // you are interested in, rather than using these general purpose 
    // entry points.  The info can then be extracted from the native image
    // in the no-IL image case.
    // ------------------------------------------------------------

    // For IJW purposes only - this asserts that we have an IJW image.
    HMODULE GetIJWBase();

    // The debugger can tolerate a null value here for native only loading cases
    void *GetDebuggerContents(COUNT_T *pSize = NULL);

#ifndef DACCESS_COMPILE
    // Returns the IL image range; may force a LoadLibrary
    const void *GetManagedFileContents(COUNT_T *pSize = NULL);
#endif // DACCESS_COMPILE

    const void *GetLoadedImageContents(COUNT_T *pSize = NULL);
    
    // ------------------------------------------------------------
    // Native image access
    // ------------------------------------------------------------

    // Does the loader support using a native image for this file?
    // Some implementation restrictions prevent native images from being used
    // in some cases.
    BOOL CanUseNativeImage() { LEAF_CONTRACT; return m_fCanUseNativeImage; }
    void SetCannotUseNativeImage() { LEAF_CONTRACT; m_fCanUseNativeImage = FALSE; }
    void SetForceRetainNativeImage() { LEAF_CONTRACT; m_flags |= PEFILE_FORCE_RETAIN_NATIVEIMAGE; }
    void SetNativeImageUsedExclusively() { LEAF_CONTRACT; m_flags|=PEFILE_NATIVE_IMAGE_USED_EXCLUSIVELY;};
    BOOL IsNativeImageUsedExclusively() { LEAF_CONTRACT; return m_flags&PEFILE_NATIVE_IMAGE_USED_EXCLUSIVELY;};    
    BOOL HasNativeImage();
    BOOL IsNativeLoaded();
    PEImage *GetNativeImageWithRef();
    PEImage *GetPersistentNativeImage();
    PTR_PEImageLayout GetLoadedNative();
    PTR_PEImageLayout GetLoadedIL();    
    PTR_PEImageLayout GetAnyILWithRef();        //AddRefs!
    IStream * GetPdbStream();       
    void ClearPdbStream();
    BOOL IsLoaded(BOOL bAllowNativeSkip=TRUE) ;
    BOOL PassiveDomainOnly();
#ifdef DACCESS_COMPILE    
    PEImage *GetNativeImage()
    {
        LEAF_CONTRACT;
        return m_nativeImage;
    }
#endif


    // ------------------------------------------------------------
    // Resource access
    // ------------------------------------------------------------

    void GetEmbeddedResource(DWORD dwOffset, DWORD *cbResource, PBYTE *pbInMemoryResource);

    // ------------------------------------------------------------
    // File loading
    // ------------------------------------------------------------

    PEAssembly *LoadAssembly(mdAssemblyRef kAssemblyRef, IMDInternalImport *pImport = NULL);

    // ------------------------------------------------------------
    // Logging
    // ------------------------------------------------------------

    // The format string is intentionally unicode to avoid globalization bugs
    void ExternalLog(DWORD level, const WCHAR *fmt, ...) DAC_EMPTY();
    void ExternalLog(DWORD level, const char *msg) DAC_EMPTY();

    virtual void ExternalVLog(DWORD level, const WCHAR *fmt, va_list args) DAC_EMPTY();
    virtual void FlushExternalLog() DAC_EMPTY();

    // ------------------------------------------------------------
    // Zap monitor
    // ------------------------------------------------------------

    void *GetMetadataProfileData()
    {
        WRAPPER_CONTRACT;


        return NULL;
    }

protected:
    PTR_PEImageLayout GetLoaded();

    // ------------------------------------------------------------
    // Internal constants
    // ------------------------------------------------------------

    enum
    { 
        PEFILE_SYSTEM                 = 0x01,
        PEFILE_ASSEMBLY               = 0x02,
        PEFILE_MODULE                 = 0x04,
        PEFILE_SKIP_VERIFICATION      = 0x08,
        PEFILE_SKIP_MODULE_HASH_CHECKS= 0x10,
        PEFILE_ISTREAM                = 0x100,
        PEFILE_HAS_NATIVE_IMAGE_METADATA = 0x200,
        PEFILE_INTROSPECTIONONLY      = 0x400,
        PEFILE_FORCE_RETAIN_NATIVEIMAGE = 0x800,  // if frozen objects in image must not unload
        PEFILE_NATIVE_IMAGE_USED_EXCLUSIVELY =0x1000
    };

    // ------------------------------------------------------------
    // Internal routines
    // ------------------------------------------------------------

#ifndef DACCESS_COMPILE
    PEFile(PEImage *image, BOOL fCheckSecurity = TRUE);
    virtual ~PEFile();

    virtual void ReleaseIL();
#endif

    void OpenMDImport();
    void RestoreMDImport(IMDInternalImport* pImport);
    void OpenMDImport_Unsafe();
    void OpenImporter();
    void OpenAssemblyImporter();
    void OpenEmitter();
    void OpenAssemblyEmitter();

    void ConvertMDInternalToReadWrite();
    void ReleaseMetadataInterfaces(BOOL bDestructor, BOOL bKeepNativeData=FALSE);

    void CheckSecurity();
    friend class Module;

#ifndef DACCESS_COMPILE
    void EnsureImageOpened();
#endif // DACCESS_COMPILE

    friend class ClrDataAccess;
    BOOL HasNativeImageMetadata();

    // ------------------------------------------------------------
    // Instance fields
    // ------------------------------------------------------------
    
#ifdef _DEBUG
    LPCWSTR                 m_pDebugName;
    SString                 m_debugName;
#endif
    
    // Identity image
    PTR_PEImage              m_identity;
    // IL image
    PTR_PEImage              m_ILimage;
    // Native image
    PTR_PEImage              m_nativeImage;

    BOOL                     m_fCanUseNativeImage;
    BOOL                     m_bHasPersistentMDImport;
    
#ifndef DACCESS_COMPILE
    IMDInternalImport       *m_pMDImport;
#else
    IMDInternalImport       *m_pMDImport_UseAccessor;
#endif
    IMetaDataImport         *m_pImporter;
    IMetaDataImport2        *m_pImporter2;
    IMetaDataAssemblyImport *m_pAssemblyImporter;
    IMetaDataEmit           *m_pEmitter;
    IMetaDataAssemblyEmit   *m_pAssemblyEmitter;
    SimpleRWLock            *m_pMetadataLock;
    LONG                    m_refCount;
    SBuffer                 *m_hash;                   // cached SHA1 hash value
    int                     m_flags;

    COR_TRUST               *m_certificate;    


#ifdef DEBUGGING_SUPPORTED
#endif
public:
    PEImage *GetILimage()
    {
        LEAF_CONTRACT;
        return m_ILimage;
    }

    void GetPathForErrorMessages(SString & result);

    void ConvertMetadataToRWForEnC();
};

typedef VPTR(class PEAssembly) PTR_PEAssembly;

class PEAssembly : public PEFile
    { 
    VPTR_VTABLE_CLASS(PEAssembly, PEFile)
    
  public:
    // ------------------------------------------------------------
    // Public API
    // ------------------------------------------------------------

    // This opens the canonical mscorlib.dll
    static PEAssembly *OpenSystem();

    static PEAssembly *Open(IAssembly *pIAssembly, IAssembly* pNativeFusionAssembly, IFusionBindLog *pFusionLog = NULL,
                            BOOL isSystemAssembly = FALSE, BOOL isIntrospectionOnly = FALSE);
    static PEAssembly *Open(IHostAssembly *pIHostAssembly, BOOL isSystemAssembly = FALSE,
                            BOOL isIntrospectionOnly = FALSE);

    static PEAssembly *OpenMemory(PEAssembly *pParentAssembly, const void *flat,
                                  COUNT_T size, BOOL isIntrospectionOnly = FALSE);


    static PEAssembly *Create(PEAssembly *pParentAssembly, IMetaDataAssemblyEmit *pEmit, BOOL isIntrospectionOnly);
    
    // A Delayed assembly will not have its file accessed until absolutely necessary. 
    // Note that this can only be used in cases where you guarantee that the file exists
    // and will not be deleted between now and the delayed open, as we don't check the file
    // or open a handle on it.  (Typically the only case this is true is for native image
    // depedencies.)

#ifdef DACCESS_COMPILE
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

    static PEAssembly *OpenDelayed(IAssembly *pFusionAssembly,
                                   COR_TRUST *pPublisher,
                                   DWORD pPublisherSize);

  private:
    // Private helpers for crufty exception handling reasons
    static PEAssembly *DoOpenSystem();
    static PEAssembly *DoOpen(IAssembly *pIAssembly, IAssembly* pNativeFusionAssembly, IFusionBindLog *pFusionLog,
                              BOOL isSystemAssembly, BOOL isIntrospectionOnly = FALSE);
    static PEAssembly *DoOpen(IHostAssembly *pIHostAssembly, BOOL isSystemAssembly,
                              BOOL isIntrospectionOnly = FALSE);
    static PEAssembly *DoOpenMemory(PEAssembly *pParentAssembly, const void *flat,
                                    COUNT_T size, BOOL isIntrospectionOnly = FALSE);

  public:

    // ------------------------------------------------------------
    // binding & source
    // ------------------------------------------------------------

    BOOL IsBindingCodeBase();

    BOOL IsSourceGAC();
    BOOL IsSourceDownloadCache();

    BOOL IsContextLoadFrom();

    LOADCTX_TYPE GetLoadContext();

    DWORD GetLocationFlags();

#ifndef  DACCESS_COMPILE
    virtual void ReleaseIL();
#endif
    
    // Can we avoid exposing these?
    IAssembly *GetFusionAssembly(); 
    IAssembly *GetNativeAssembly(); 
    IHostAssembly *GetIHostAssembly(); 
    IAssemblyName *GetFusionAssemblyName();

    IAssembly *GetNativeFusionAssembly(); 

    // ------------------------------------------------------------
    // Hash support
    // ------------------------------------------------------------

    BOOL NeedsModuleHashChecks();

    void GetIdentitySignature(SBuffer &result);

    BOOL HasStrongNameSignature();
#ifndef  DACCESS_COMPILE
    void GetEffectiveStrongNameSignature(SBuffer &result);
#endif // DACCESS_COMPILE

    // ------------------------------------------------------------
    // Descriptive strings
    // ------------------------------------------------------------

    // This returns a non-empty path representing the source of the assembly; it may 
    // be the parent assembly for dynamic or memory assemblies
    const SString &GetEffectivePath();

    // Codebase is the fusion codebase or path for the assembly.  It is in URL format.
    // Note this may be obtained from the parent PEFile if we don't have a path or fusion
    // assembly.
    //
    // fCopiedName means to get the "shadow copied" path rather than the original path, if applicable
    void GetCodeBase(SString &result, BOOL fCopiedName = FALSE);
    
    // Display name is the fusion binding name for an assembly
    void GetDisplayName(SString &result, DWORD flags = 0);

    // ------------------------------------------------------------
    // Metadata access
    // ------------------------------------------------------------

    LPCUTF8 GetSimpleName();

    // ------------------------------------------------------------
    // Utility functions
    // ------------------------------------------------------------

    static void PathToUrl(SString &string);
    static void UrlToPath(SString &string);
    static BOOL FindLastPathSeparator(const SString &path, SString::Iterator &i);

    // ------------------------------------------------------------
    // Logging
    // ------------------------------------------------------------

    void ExternalVLog(DWORD level, const WCHAR *fmt, va_list args) DAC_EMPTY();
    void FlushExternalLog() DAC_EMPTY();

  protected:

#ifndef DACCESS_COMPILE
    PEAssembly(PEImage *image, IMetaDataEmit *pEmit, IAssembly *pIAssembly, IAssembly* pNativeFusionAssembly, IFusionBindLog *pFusionLog,
               IHostAssembly *pIHostAssembly, PEFile *creator, BOOL system, BOOL introspectionOnly = FALSE);
    ~PEAssembly();
#endif

    // ------------------------------------------------------------
    // Loader access API
    // ------------------------------------------------------------

    friend class DomainAssembly;

  private:

    void CheckSecurity();

    // ------------------------------------------------------------
    // Instance fields
    // ------------------------------------------------------------

    PTR_PEFile               m_creator;

    IMetaDataAssemblyImport *m_pAssemblyImporter;
    
    IAssemblyName           *m_pFusionAssemblyName;
    IAssembly               *m_pFusionAssembly;
    IFusionBindLog          *m_pFusionLog;
    IHostAssembly           *m_pIHostAssembly;
    IAssembly               *m_pNativeFusionAssembly;
    IAssemblyBindingClosure *m_pNativeImageClosure; //present only for shared 
    LOADCTX_TYPE             m_loadContext;
    DWORD                    m_dwLocationFlags;

  public:
    PTR_PEFile GetCreator() {
        LEAF_CONTRACT;
        return m_creator;
    }
};

typedef VPTR(class PEModule) PTR_PEModule;

class PEModule : public PEFile
{
    VPTR_VTABLE_CLASS(PEModule, PEFile)
    
  public:

    // ------------------------------------------------------------
    // Public API
    // ------------------------------------------------------------

    static PEModule *Open(PEAssembly *assembly, mdFile token,
                          const SString &fileName);

    static PEModule *OpenMemory(PEAssembly *assembly, mdFile kToken,
                                const void *flat, COUNT_T size); 

    static PEModule *Create(PEAssembly *assembly, mdFile kToken, IMetaDataEmit *pEmit);

#ifdef DACCESS_COMPILE
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

  private:
    // Private helpers for crufty exception handling reasons
    static PEModule *DoOpen(PEAssembly *assembly, mdFile token,
                            const SString &fileName);

    static PEModule *DoOpenMemory(PEAssembly *assembly, mdFile kToken,
                                  const void *flat, COUNT_T size); 
  public:

    // ------------------------------------------------------------
    // Metadata access
    // ------------------------------------------------------------

    PEAssembly *GetAssembly();
    mdFile GetToken();
    BOOL IsResource();
    BOOL IsIStream();
    LPCUTF8 GetSimpleName();

    // ------------------------------------------------------------
    // Logging
    // ------------------------------------------------------------

    void ExternalVLog(DWORD level, const WCHAR *fmt, va_list args) DAC_EMPTY();
    void FlushExternalLog() DAC_EMPTY();

private:
    // ------------------------------------------------------------
    // Loader access API
    // ------------------------------------------------------------

    friend class DomainModule;

private:

#ifndef DACCESS_COMPILE
    PEModule(PEImage *image, PEAssembly *assembly, mdFile token, IMetaDataEmit *pEmit);
    ~PEModule();
#endif

    // ------------------------------------------------------------
    // Instance fields
    // ------------------------------------------------------------

    PTR_PEAssembly          m_assembly;
    mdFile                  m_token;
    BOOL                    m_bIsResource;
};

#if defined (_MSC_VER) && _MSC_VER <= 1300
template void DoNothing(PEFile*);
template BOOL CompareDefault(PEFile*,PEFile*);
template void DoTheRelease(PEFile*);
#endif
typedef Wrapper<PEFile*,DoNothing<PEFile*>,DoTheRelease<PEFile>,NULL> PEFileHolder;

#if defined (_MSC_VER) && _MSC_VER <= 1300
template void DoNothing(PEAssembly*);
template BOOL CompareDefault(PEAssembly*,PEAssembly*);
template void DoTheRelease(PEAssembly*);
#endif
typedef Wrapper<PEAssembly*,DoNothing<PEAssembly*>,DoTheRelease<PEAssembly>,NULL> PEAssemblyHolder;

#if defined (_MSC_VER) && _MSC_VER <= 1300
template void DoNothing(PEModule*);
template BOOL CompareDefault(PEModule*,PEModule*);
template void DoTheRelease(PEModule *);
#endif
typedef Wrapper<PEModule*,DoNothing<PEModule*>,DoTheRelease<PEModule>,NULL> PEModuleHolder;

// ================================================================================
// Inline definitions
// ================================================================================


#endif  // PEFILE_H_
