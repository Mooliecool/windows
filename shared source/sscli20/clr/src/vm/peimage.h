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
// PEImage.h
// --------------------------------------------------------------------------------

#ifndef PEIMAGE_H_
#define PEIMAGE_H_

// --------------------------------------------------------------------------------
// Required headers
// --------------------------------------------------------------------------------

#include "clrtypes.h"
#include "peimagelayout.h"
#include "sstring.h"
#include "holder.h"

class SimpleRWLock;
// --------------------------------------------------------------------------------
// Forward declarations
// --------------------------------------------------------------------------------

class Crst;


// --------------------------------------------------------------------------------
// PEImage is a PE file loaded by our "simulated LoadLibrary" mechanism.  A PEImage 
// can be loaded either FLAT (same layout as on disk) or MAPPED (PE sections 
// mapped into virtual addresses.)
// 
// The MAPPED format is currently limited to "IL only" images - this can be checked
// for via PEDecoder::IsILOnlyImage.
//
// NOTE: PEImage will NEVER call LoadLibrary.
// --------------------------------------------------------------------------------

typedef DPTR(class PEImage) PTR_PEImage;

class PEImage 
{
    friend class PEModule;
    friend class CCLRDebugManager;
public:
    // ------------------------------------------------------------
    // Public constants
    // ------------------------------------------------------------

    enum
    {
        LAYOUT_CREATEIFNEEDED=1,
        LAYOUT_PERSISTENT=2
    };
    PTR_PEImageLayout GetLayout(DWORD imageLayoutMask,DWORD flags); //with ref
    PTR_PEImageLayout GetLoadedLayout(); //no ref
    PTR_PEImageLayout GetLoadedIntrospectionLayout(); //no ref, introspection only
    BOOL IsOpened();
    BOOL HasLoadedLayout();
    BOOL HasLoadedIntrospectionLayout();
    

public:
    // ------------------------------------------------------------
    // Public API
    // ------------------------------------------------------------

    static void Startup();

    // Normal constructed PEImages do NOT share images between calls and
    // cannot be accessed by Get methods.
    //
    // DO NOT USE these unless you want a private copy-on-write mapping of
    // the file.



public:
    ~PEImage();
    PEImage();

#ifndef DACCESS_COMPILE
    static PTR_PEImage LoadFlat(const void *flat, COUNT_T size);
    static PTR_PEImage OpenImage(const SString &path, MDInternalImportFlags flags=MDInternalImport_Default);
    static PTR_PEImage OpenImage(IStream *pIStream, UINT64 uStreamAsmId,
                                 DWORD dwModuleId, BOOL resourceFile, MDInternalImportFlags flags=MDInternalImport_Default);
    static PTR_PEImage FindById(UINT64 uStreamAsmId, DWORD dwModuleId);
    static PTR_PEImage PEImage::FindByPath(const SString& path);    
    static PTR_PEImage PEImage::FindByShortPath(const SString& path);    
    static PTR_PEImage PEImage::FindByLongPath(const SString& path);    
    void AddToHashMap();
    static BOOL CompareID(PEImage *pPE1, PEImage *pPE2);
    void   Load();
    void   SetLoadedHMODULE(HMODULE hMod);
    void   LoadNoMetaData(BOOL bIntrospection);
    void   LoadNoFile();
    void   LoadFromMapped();  
    void   LoadForIntrospection();
    
#endif
    BOOL   HasID();
    ULONG GetIDHash();
    
    const void *GetStrongNameSignature(COUNT_T *pSize = NULL);
    

    // Refcount above images.
    void AddRef();
    ULONG Release();

    // Accessors
    const SString &GetPath();
    BOOL IsFile();
    HANDLE GetFileHandle();
    HANDLE GetProtectingFileHandle(BOOL bProtectIfNotOpenedYet);

    void GetPathForErrorMessages(SString & result);

    // Equality
    BOOL Equals(PEImage *pImage);
    static ULONG HashStreamIds(UINT64 id1, DWORD id2);

    // Hashing utlities.  (These require a flat version of the file, and 
    // will open one if necessary.)

    void GetImageBits(DWORD layout, SBuffer &result);
    void ComputeHash(ALG_ID algorithm, SBuffer &result);
    CHECK CheckHash(ALG_ID algorithm, const void *pbHash, COUNT_T cbHash);

    void GetMVID(GUID *pMvid);
    const BOOL HasV1Metadata();
    IMDInternalImport* GetMDImport();
    IMDInternalImport* GetNativeMDImport();    
    

    BOOL HasSecurityDirectory();
    BOOL HasContents() ;
    BOOL HasNativeHeader() ;
    CHECK CheckFormat();
    // Check utilites
    CHECK CheckILFormat();
    static CHECK CheckCanonicalFullPath(const SString &path);
    static CHECK CheckStartup();
    const void *GetMetadata(COUNT_T *pSize = NULL);
    void GetHashedStrongNameSignature(SBuffer &result);
    void GetIdentitySignature(SBuffer &result);
    void GetEffectiveStrongNameSignature(SBuffer &result);



    static LCID GetFileSystemLocale();
    static BOOL PathEquals(const SString &p1, const SString &p2);
    void SetIsTrustedNativeImage(){LEAF_CONTRACT; m_bIsTrustedNativeImage=TRUE;};
#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
    const SString &GetModuleFileNameHintForDAC();
#endif

    //
    // This function will clear our cached pointer for the pdb stream and release our reference to 
    // to the host PDB stream. SQL needs this because many queries live in one AppDomain. While 
    // many modules come and go, many pdb streams are loaded they would like to see PDB stream is 
    // released before AppDomain unload. This will be triggered when DebuggerUpdatemModuleSym is done. 
    // 
    void ClearPdbStream()
    {
        LEAF_CONTRACT;

        CrstHolder lockHolder(&m_PdbStreamLock);

        if (m_pPdbStream != NULL)
        {
            m_pPdbStream->Release();
            m_pPdbStream = NULL;
        }
    }

    //
    // This function will return the cached stream pointer, giving the caller a AddRef() to the 
    // pointer.
    //
    IStream *GetPdbStream() 
    { 
        LEAF_CONTRACT; 

        CrstHolder lockHolder(&m_PdbStreamLock);

        if (m_pPdbStream != NULL)
        {
            m_pPdbStream->AddRef();
        }

        return m_pPdbStream; 
    }

    const BOOL HasNTHeaders();
    const BOOL HasCorHeader(); 
    void SetPassiveDomainOnly();
    BOOL PassiveDomainOnly();
    const BOOL HasDirectoryEntry(int entry);
    const mdToken GetEntryPointToken(BOOL bIgoreNativeEntryPoint = FALSE);
    const BOOL IsILOnly();
    const BOOL IsDll();
    BOOL  IsFileLocked();
    const BOOL HasStrongNameSignature();
    const HRESULT VerifyStrongName(DWORD* verifyOutputFlags);    
    BOOL IsStrongNameSigned();
    void* GetMetadataProfileData();
    BOOL Has32BitNTHeaders();
    

private:
    PTR_PEImageLayout GetLayoutInternal(DWORD imageLayoutMask,DWORD flags); //with ref
    void OpenMDImport();
    void OpenNativeMDImport();    
    // ------------------------------------------------------------
    // Private routines
    // ------------------------------------------------------------

    void  InitForSearch(UINT64 uStreamAsmId, DWORD dwModuleId);
    void  InitForSearch(const SString& path);
    void  Init(const SString& path);
    void  Init(IStream* pStream, UINT64 uStreamAsmId,
               DWORD dwModuleId, BOOL resourceFile);

    static BOOL CompareImage(UPTR image1, UPTR image2);

    void DECLSPEC_NORETURN ThrowFormat();

    static CHECK CheckLayoutFormat(PEDecoder *pe);

    // ------------------------------------------------------------
    // Instance members
    // ------------------------------------------------------------

    Crst        m_PdbStreamLock;
    IStream     *m_pPdbStream;
    SString     m_path;
    LONG        m_refCount;

    SString     m_sModuleFileNameHintUsedByDac; // This is only used by DAC
private:
    BOOL        m_bInitedForSearch;
    BOOL        m_bIsTrustedNativeImage;
    BOOL        m_bPassiveDomainOnly;

protected:

    enum 
    {
        IMAGE_FLAT=0,
        IMAGE_MAPPED=1,
        IMAGE_LOADED=2,
        IMAGE_LOADED_FOR_INTROSPECTION=3,
        IMAGE_COUNT=4
    };
    
    SimpleRWLock *m_pLayoutLock;
    PTR_PEImageLayout m_pLayouts[IMAGE_COUNT] ;
    BOOL      m_bInHashMap;
#ifndef DACCESS_COMPILE    
    void   SetLayout(DWORD dwLayout, PTR_PEImageLayout pLayout);
#endif // DACCESS_COMPILE


#ifdef METADATATRACKER_DATA
    class MetaDataTracker   *m_pMDTracker;
#endif // METADATATRACKER_DATA

    IMDInternalImport* m_pMDImport;
    IMDInternalImport* m_pNativeMDImport;

    UINT64      m_StreamAsmId;
    DWORD       m_dwStreamModuleId;
    BOOL        m_fIsIStream;

private:


    // ------------------------------------------------------------
    // Static members
    // ------------------------------------------------------------

    static CrstStatic   s_hashLock;

    static PtrHashMap   *s_Images;

    HANDLE m_hFile;

    BOOL        m_bSignatureInfoCached;
    HRESULT   m_hrSignatureInfoStatus;
    BOOL        m_dwSignatureInfo;    
private:
    DWORD m_dwPEKind;
    DWORD m_dwMachine;
    BOOL  m_fUnknownKindAndMachine;
public:
    void  GetPEKindAndMachine();
    DWORD GetPEKind();
    DWORD GetMachine();
    DWORD GetFusionProcessorArchitecture();
};

FORCEINLINE void PEImageRelease(PEImage *i)
{
    WRAPPER_CONTRACT;
    i->Release();
}

#if defined (_MSC_VER) && _MSC_VER <= 1300
template void DoNothing(PEImage*);
template BOOL CompareDefault(PEImage*,PEImage*);
#endif
typedef Wrapper<PEImage *, DoNothing, PEImageRelease> PEImageHolder;

// ================================================================================
// Inline definitions
// ================================================================================

#include "peimage.inl"

#endif  // PEIMAGE_H_
