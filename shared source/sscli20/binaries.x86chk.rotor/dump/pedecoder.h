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
// PEDecoder.h
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// PEDecoder - Utility class for reading and verifying PE files.
// 
// Note that the Check step is optional if you are willing to trust the 
// integrity of the image.
// (Or at any rate can be factored into an initial verification step.)
//
// Functions which access the memory of the PE file take a "flat" flag - this
// indicates whether the PE images data has been loaded flat the way it resides in the file,
// or if the sections have been mapped into memory at the proper base addresses.
//
// Finally, some functions take an optional "size" argument, which can be used for 
// range verification.  This is an optional parameter, but if you omit it be sure
// you verify the size in some other way.
// --------------------------------------------------------------------------------

#ifndef PEDECODER_H_
#define PEDECODER_H_

// --------------------------------------------------------------------------------
// Required headers
// --------------------------------------------------------------------------------

#include "windows.h"
#include "clrtypes.h"
#include "check.h"
#include "contract.h"
#include "cor.h"
#include "corhdr.h"

typedef DPTR(struct COR_ILMETHOD) PTR_COR_ILMETHOD;
struct CORCOMPILE_HEADER { int dummy_field; };
typedef DPTR(struct CORCOMPILE_HEADER) PTR_CORCOMPILE_HEADER;
#define CORCOMPILE_IS_TOKEN_TAGGED(fixup) (false)

// --------------------------------------------------------------------------------
// Forward declared types
// --------------------------------------------------------------------------------

class Module;
class ApplyExternalRelocsHelper;

// --------------------------------------------------------------------------------
// RVA definition
// --------------------------------------------------------------------------------

// Needs to be DWORD to avoid conflict with <imagehlp.h>
typedef DWORD RVA;

#ifdef _MSC_VER
// Wrapper to suppress ambigous overload problems with MSVC.
inline CHECK CheckOverflow(RVA value1, COUNT_T value2)
{
    WRAPPER_CONTRACT;
    CHECK(CheckOverflow((UINT32) value1, (UINT32) value2));
    CHECK_OK;
}
#endif  // _MSC_VER

// --------------------------------------------------------------------------------
// Types
// --------------------------------------------------------------------------------

typedef DPTR(class PEDecoder) PTR_PEDecoder;

class PEDecoder
{
    friend class ApplyExternalRelocsHelper;

  public:

    // ------------------------------------------------------------
    // Public API
    // ------------------------------------------------------------
    
    // Access functions are divided into 3 categories:
    //  Has - check if the element is present
    //  Check - Do consistency checks on the element (requires Has).  
    //          This step is optional if you are willing to trust the integrity of the 
    //          image. (It is asserted in a checked build.)
    //  Get - Access the element (requires Has and Check)

    PEDecoder();
    PEDecoder(void *flatBase, COUNT_T size);              // flatBase is the raw disk layout data (using MapViewOfFile)
    PEDecoder(void *mappedBase, bool relocated = FALSE);  // mappedBase is the mapped/expanded file (using LoadLibrary)

    void Init(void *flatBase, COUNT_T size);
    HRESULT Init(void *mappedBase, bool relocated = FALSE);
    void   Reset();  //make sure you don't have a race

    void *GetBase() const;            // Currently loaded base, as opposed to GetPreferredBase()
    BOOL IsMapped() const;
    BOOL IsRelocated() const;
    BOOL IsFlat() const;
    BOOL HasContents() const;
    COUNT_T GetSize() const;          // size of file on disk, as opposed to GetVirtualSize()

    // High level image checks:

    CHECK CheckFormat() const;        // Check whatever is present
    CHECK CheckNTFormat() const;      // Check a PE file image
    CHECK CheckCORFormat() const;     // Check a COR image (IL or native)
    CHECK CheckILFormat() const;      // Check a managed image
    CHECK CheckILOnlyFormat() const;  // Check an IL only image
    CHECK CheckNativeFormat() const;  // Check a native image

    HRESULT ValidateFormat();

    // NT header access
    
    BOOL HasNTHeaders() const;
    CHECK CheckNTHeaders() const;

    // SPECIAL NOTE: the IMAGE_NT_HEADERS structure returned may actually be the opposite 32/64 bit architecture
    // unless you either know you have a correct bit image, or else have called 
    // PromoteNTHeaders.  Most of the fields in the IMAGE_NT_HEADERS can still be accessed safely, except for:
    //      OptionalHeader.ImageBase
    //      OptionalHeader.SizeOfStackReserve
    //      OptionalHeader.SizeOfStackCommit
    //      OptionalHeader.SizeOfHeapReserve
    //      OptionalHeader.SizeOfHeapCommit
    //      OptionalHeader.LoaderFlags
    //      OptionalHeader.NumberOfRvaAndSizes
    //      OptionalHeader.DataDirectory[]
    // Use the accessors below to access this information safely rather than dereferencing the headers
    // directly.
    //
    // Promote will mutate the header into the platform format in place in the image if possible using
    // padding space after the headers. (The working assumption is that due to 
    // header file padding there will always be enough space in cases we care about - otherwise we
    // will have to add a mechanism to allocate a new header on the side - which is straightforward
    // but has the unfortunate characteristic of requiring PEDecoder to have a destructor.)

    IMAGE_NT_HEADERS32 *GetNTHeaders32() const;
    IMAGE_NT_HEADERS64 *GetNTHeaders64() const;
    BOOL Has32BitNTHeaders() const;
    BOOL PromoteNTHeaders();

    const void *GetHeaders(COUNT_T *pSize = NULL) const;

    BOOL IsDll() const;
    BOOL HasBaseRelocations() const;
    const void *GetPreferredBase() const; // OptionalHeaders.ImageBase
    COUNT_T GetVirtualSize() const; // OptionalHeaders.SizeOfImage - size of mapped/expanded image in memory
    WORD GetSubsystem() const;
    DWORD GetTimeDateStamp() const;
    DWORD GetCheckSum() const;
    WORD GetMachine() const;  
    DWORD GetFileAlignment() const;
    DWORD GetSectionAlignment() const;
    SIZE_T GetSizeOfStackReserve() const;
    SIZE_T GetSizeOfStackCommit() const;
    SIZE_T GetSizeOfHeapReserve() const;
    SIZE_T GetSizeOfHeapCommit() const;
    UINT32 GetLoaderFlags() const;
    COUNT_T GetNumberOfRvaAndSizes() const;
    COUNT_T GetNumberOfSections() const;
    COUNT_T GetNumberOfWritableSections() const;
    IMAGE_SECTION_HEADER *FindFirstSection() const;

    DWORD GetImageIdentity() const;

    // Directory entry access

    BOOL HasDirectoryEntry(int entry) const;
    CHECK CheckDirectoryEntry(int entry, int forbiddenFlags = 0, IsNullOK ok = NULL_NOT_OK) const;
    IMAGE_DATA_DIRECTORY *GetDirectoryEntry(int entry) const;
    TADDR GetDirectoryEntryData(int entry, COUNT_T *pSize = NULL) const;

    // IMAGE_DATA_DIRECTORY access

    CHECK CheckDirectory(IMAGE_DATA_DIRECTORY *pDir, int forbiddenFlags = 0, IsNullOK ok = NULL_NOT_OK) const;
    TADDR GetDirectoryData(IMAGE_DATA_DIRECTORY *pDir) const;

    // IMAGE_EXPORT_DIRECTORY

    WORD GetEntryPointOrdinal( char const * name ) const;

    // Basic RVA access

    CHECK CheckRva(RVA rva, IsNullOK ok = NULL_NOT_OK) const;
    CHECK CheckRva(RVA rva, COUNT_T size, int forbiddenFlags=0, IsNullOK ok = NULL_NOT_OK) const;
    TADDR GetRvaData(RVA rva, IsNullOK ok = NULL_NOT_OK) const;
    // Called with ok=NULL_OK only for mapped fields (RVA statics)

    CHECK CheckData(const void *data, IsNullOK ok = NULL_NOT_OK) const;
    CHECK CheckData(const void *data, COUNT_T size, IsNullOK ok = NULL_NOT_OK) const;
    RVA GetDataRva(const TADDR data) const;

    // Flat mapping utilities - using PointerToRawData instead of (Relative)VirtualAddress
    CHECK CheckOffset(COUNT_T fileOffset, IsNullOK ok = NULL_NOT_OK) const;
    CHECK CheckOffset(COUNT_T fileOffset, COUNT_T size, IsNullOK ok = NULL_NOT_OK) const;
    TADDR GetOffsetData(COUNT_T fileOffset, IsNullOK ok = NULL_NOT_OK) const;
    // Called with ok=NULL_OK only for mapped fields (RVA statics)

    // Mapping between RVA and file offsets
    COUNT_T RvaToOffset(RVA rva) const;
    RVA OffsetToRva(COUNT_T fileOffset) const;
    
    // Base intra-image pointer access
    // (These are for pointers retrieved out of the PE image)

    CHECK CheckInternalAddress(SIZE_T address, IsNullOK ok = NULL_NOT_OK) const;
    CHECK CheckInternalAddress(SIZE_T address, COUNT_T size, IsNullOK ok = NULL_NOT_OK) const;
    TADDR GetInternalAddressData(SIZE_T address) const;

    // CLR loader IL Image verification - these checks apply to IL_ONLY images.

    BOOL IsILOnly() const;
    CHECK CheckILOnly() const;

    void LayoutILOnly(void *base, BOOL allowFullPE = FALSE) const;
    void ApplyILOnlyBaseRelocations(BOOL allowFullPE = FALSE);

    // Strong name & hashing support

    BOOL HasStrongNameSignature() const;
    CHECK CheckStrongNameSignature() const;
    const void *GetStrongNameSignature(COUNT_T *pSize = NULL) const;

    // IsStrongNameSigned indicates whether the signature has been filled in.
    // (otherwise if it has a signature it is delay signed.)
    BOOL IsStrongNameSigned() const;

    // TLS

    BOOL HasTls() const;
    CHECK CheckTls() const;
    void *GetTlsRange(COUNT_T *pSize = NULL) const;
    UINT32 GetTlsIndex() const;


    // COR header fields

    BOOL HasCorHeader() const;
    CHECK CheckCorHeader() const;
    IMAGE_COR20_HEADER *GetCorHeader() const;

    const void *GetMetadata(COUNT_T *pSize = NULL) const;

    const void *GetResources(COUNT_T *pSize = NULL) const;
    CHECK CheckResource(COUNT_T offset) const;
    const void *GetResource(COUNT_T offset, COUNT_T *pSize = NULL) const;

    BOOL HasManagedEntryPoint() const;
    ULONG GetEntryPointToken() const;
    IMAGE_COR_VTABLEFIXUP *GetVTableFixups(COUNT_T *pCount = NULL) const;

    // Native header access
    BOOL HasNativeHeader() const;
    CHECK CheckNativeHeader() const;
    CORCOMPILE_HEADER *GetNativeHeader() const;
    BOOL IsI386() const;

    void GetPEKindAndMachine(DWORD* pdwKind, DWORD* pdwMachine);



    CHECK CheckWillCreateGuardPage() const;

    // Native DLLMain Entrypoint
    BOOL HasNativeEntryPoint() const;
    void *GetNativeEntryPoint() const;

#ifdef _DEBUG
    // Stress mode for relocations
    static BOOL GetForceRelocs();
    static BOOL ForceRelocForDLL(LPCWSTR lpFileName);
#endif

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags, bool enumThis);
#endif

  protected:

    // ------------------------------------------------------------
    // Protected API for subclass use
    // ------------------------------------------------------------
    
    // Checking utilites
    static CHECK CheckBounds(RVA rangeBase, COUNT_T rangeSize, RVA rva);
    static CHECK CheckBounds(RVA rangeBase, COUNT_T rangeSize, RVA rva, COUNT_T size);

    static CHECK CheckBounds(const void *rangeBase, COUNT_T rangeSize, const void *pointer);
    static CHECK CheckBounds(const void *rangeBase, COUNT_T rangeSize, const void *pointer, COUNT_T size);
  
  private:

    // ------------------------------------------------------------
    // Internal functions
    // ------------------------------------------------------------
    
    static IMAGE_SECTION_HEADER *FindFirstSection(IMAGE_NT_HEADERS* pNTHeaders);

    IMAGE_NT_HEADERS *FindNTHeaders() const;
    IMAGE_COR20_HEADER *FindCorHeader() const;
    CORCOMPILE_HEADER *FindNativeHeader() const;

    // Native header conversion
    static void TranslateNTHeaders(const void *from, IMAGE_NT_HEADERS *to);

    // Flat mapping utilities
    RVA InternalAddressToRva(SIZE_T address) const;

    // Flat mapping utilities - using PointerToRawData instead of (Relative)VirtualAddress
    IMAGE_SECTION_HEADER *RvaToSection(RVA rva) const;
    IMAGE_SECTION_HEADER *OffsetToSection(COUNT_T fileOffset) const;

    // NT header subchecks
    CHECK CheckSection(COUNT_T previousAddressEnd, COUNT_T addressStart, COUNT_T addressSize,
                       COUNT_T previousOffsetEnd, COUNT_T offsetStart, COUNT_T offsetSize) const;

    // Pure managed subchecks
    CHECK CheckILOnlyImportDlls() const;
    CHECK CheckILOnlyImportByNameTable(RVA rva) const;
    CHECK CheckILOnlyBaseRelocations() const;
    CHECK CheckILOnlyEntryPoint() const;

    // ------------------------------------------------------------
    // Instance members
    // ------------------------------------------------------------

    enum
    {
        FLAG_MAPPED             = 0x01, // the file is mapped/hydrated (vs. the raw disk layout)
        FLAG_CONTENTS           = 0x02, // the file has contents
        FLAG_RELOCATED          = 0x04, // relocs have been applied
        FLAG_NT_CHECKED         = 0x10,
        FLAG_COR_CHECKED        = 0x20,
        FLAG_IL_ONLY_CHECKED    = 0x40,
        FLAG_NATIVE_CHECKED     = 0x80
    };
    
    TADDR               m_base;
    COUNT_T             m_size;     // size of file on disk, as opposed to OptionalHeaders.SizeOfImage
    ULONG               m_flags;

    PTR_IMAGE_NT_HEADERS   m_pNTHeaders;
    PTR_IMAGE_COR20_HEADER m_pCorHeader;
    PTR_CORCOMPILE_HEADER  m_pNativeHeader;
};

#include "fusion.h"

#include "pedecoder.inl"

//*****************************************************************************
//  Intreprets CLRPeKind and dwImageType to get PeKind as per the CLRBitness
//  API, CLRPeKind and dwImageType can be recoved from GetPEKind() if you
//  have the metadata, or retrieved directly from the headers as per the
//  implementation in shim.cpp:_CorValidateImage.
//*****************************************************************************
HRESULT TranslatePEToArchitectureTypeUtil(CorPEKind CLRPeKind, DWORD dwImageType, PEKIND *PeKind);

#endif  // PEDECODER_H_
