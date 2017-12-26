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
// PEDecoder.inl
// --------------------------------------------------------------------------------

#ifndef _PEDECODER_INL_
#define _PEDECODER_INL_

#include "pedecoder.h"
#include "ex.h"

#ifndef DACCESS_COMPILE

inline PEDecoder::PEDecoder()
  : m_base(0),
    m_size(0),
    m_flags(0),
    m_pNTHeaders(NULL),
    m_pCorHeader(NULL),
    m_pNativeHeader(NULL)
{
    CONTRACTL
    {
        CONSTRUCTOR_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
}
#else
inline PEDecoder::PEDecoder()
{
    LEAF_CONTRACT;
}
#endif // #ifndef DACCESS_COMPILE

inline void *PEDecoder::GetBase() const
{
    LEAF_CONTRACT;

    return (void *)m_base;
}

inline BOOL PEDecoder::IsMapped() const
{
    LEAF_CONTRACT;
    
    return (m_flags & FLAG_MAPPED) != 0;
}

inline BOOL PEDecoder::IsRelocated() const
{
    LEAF_CONTRACT;
    
    return (m_flags & FLAG_RELOCATED) != 0;
}

inline BOOL PEDecoder::IsFlat() const
{
    LEAF_CONTRACT;
    
    return HasContents() && !IsMapped();
}

inline BOOL PEDecoder::HasContents() const
{
    LEAF_CONTRACT;
    
    return (m_flags & FLAG_CONTENTS) != 0;
}

inline COUNT_T PEDecoder::GetSize() const
{
    LEAF_CONTRACT;

    return m_size;
}

inline PEDecoder::PEDecoder(void *mappedBase, bool fixedUp /*= FALSE*/)
  : m_base((TADDR)mappedBase),
    m_size(0),
    m_flags(FLAG_MAPPED | FLAG_CONTENTS | FLAG_NT_CHECKED | (fixedUp ? FLAG_RELOCATED : 0)),
    m_pNTHeaders((TADDR)NULL),
    m_pCorHeader((TADDR)NULL),
    m_pNativeHeader((TADDR)NULL)
{
    CONTRACTL
    {
        CONSTRUCTOR_CHECK;
        PRECONDITION(CheckPointer(mappedBase));
        PRECONDITION(CheckAligned(mappedBase, OS_PAGE_SIZE));
        PRECONDITION(PEDecoder(mappedBase,fixedUp).CheckNTHeaders());
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // Temporarily set the size to 2 pages, so we can get the headers.
    m_size = OS_PAGE_SIZE*2;

    m_pNTHeaders = PTR_IMAGE_NT_HEADERS(FindNTHeaders());
    if (!m_pNTHeaders)
        ThrowHR(COR_E_BADIMAGEFORMAT);

    m_size = VAL32(m_pNTHeaders->OptionalHeader.SizeOfImage);
}

#ifndef DACCESS_COMPILE

inline PEDecoder::PEDecoder(void *flatBase, COUNT_T size)
  : m_base((TADDR)flatBase),
    m_size(size),
    m_flags(FLAG_CONTENTS),
    m_pNTHeaders(NULL),
    m_pCorHeader(NULL),
    m_pNativeHeader(NULL)
{
    CONTRACTL
    {
        CONSTRUCTOR_CHECK;
        PRECONDITION(CheckPointer(flatBase));
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
}

inline void PEDecoder::Init(void *flatBase, COUNT_T size)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION((size == 0) || CheckPointer(flatBase));
        PRECONDITION(!HasContents());
        NOTHROW;
        GC_NOTRIGGER;    
    }
    CONTRACTL_END;

    m_base = (TADDR)flatBase;
    m_size = size;
    m_flags = FLAG_CONTENTS;
}



inline HRESULT PEDecoder::Init(void *mappedBase, bool fixedUp /*= FALSE*/)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(mappedBase));
        PRECONDITION(CheckAligned(mappedBase, OS_PAGE_SIZE));
        PRECONDITION(!HasContents());
    }
    CONTRACTL_END;

    m_base = (TADDR)mappedBase;
    m_flags = FLAG_MAPPED | FLAG_CONTENTS;
    if (fixedUp)
        m_flags |= FLAG_RELOCATED;

    // Temporarily set the size to 2 pages, so we can get the headers.
    m_size = OS_PAGE_SIZE*2;

    m_pNTHeaders = FindNTHeaders();
    if (!m_pNTHeaders)
        return COR_E_BADIMAGEFORMAT;

    m_size = VAL32(m_pNTHeaders->OptionalHeader.SizeOfImage);
    return S_OK;
}

inline void PEDecoder::Reset()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    m_base=NULL;
    m_flags=NULL;
    m_size=NULL;
    m_pNTHeaders=NULL;
    m_pCorHeader=NULL;
    m_pNativeHeader=NULL;
}
#endif // #ifndef DACCESS_COMPILE


inline IMAGE_NT_HEADERS32 *PEDecoder::GetNTHeaders32() const
{
    WRAPPER_CONTRACT;
    return PTR_IMAGE_NT_HEADERS32(PTR_HOST_TO_TADDR(FindNTHeaders()));
}

inline IMAGE_NT_HEADERS64 *PEDecoder::GetNTHeaders64() const
{
    WRAPPER_CONTRACT;
    return PTR_IMAGE_NT_HEADERS64(PTR_HOST_TO_TADDR(FindNTHeaders()));
}

inline BOOL PEDecoder::Has32BitNTHeaders() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(HasNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    return (FindNTHeaders()->OptionalHeader.Magic == VAL16(IMAGE_NT_OPTIONAL_HDR32_MAGIC));
}

inline const void *PEDecoder::GetHeaders(COUNT_T *pSize) const
{
    CONTRACT(const void *)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    //even though some data in OptionalHeader is different for 32 and 64, this field is the same
    if (pSize != NULL)
        *pSize = VAL32(FindNTHeaders()->OptionalHeader.SizeOfHeaders); 

    RETURN (const void *) m_base;
}

inline BOOL PEDecoder::IsDll() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return ((FindNTHeaders()->FileHeader.Characteristics & VAL16(IMAGE_FILE_DLL)) != 0);
}

inline BOOL PEDecoder::HasBaseRelocations() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return ((FindNTHeaders()->FileHeader.Characteristics & VAL16(IMAGE_FILE_RELOCS_STRIPPED)) == 0);
}

inline const void *PEDecoder::GetPreferredBase() const
{
    CONTRACT(const void *)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    if (Has32BitNTHeaders())
        RETURN (const void *) (SIZE_T) VAL32(GetNTHeaders32()->OptionalHeader.ImageBase);
    else
        RETURN (const void *) (SIZE_T) VAL64(GetNTHeaders64()->OptionalHeader.ImageBase);
}

inline COUNT_T PEDecoder::GetVirtualSize() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    //even though some data in OptionalHeader is different for 32 and 64,  this field is the same
    return VAL32(FindNTHeaders()->OptionalHeader.SizeOfImage);
}

inline WORD PEDecoder::GetSubsystem() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    //even though some data in OptionalHeader is different for 32 and 64,  this field is the same
    return VAL16(FindNTHeaders()->OptionalHeader.Subsystem);
}

inline DWORD PEDecoder::GetTimeDateStamp() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return VAL32(FindNTHeaders()->FileHeader.TimeDateStamp);
}

inline DWORD PEDecoder::GetCheckSum() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    //even though some data in OptionalHeader is different for 32 and 64,  this field is the same
    return VAL32(FindNTHeaders()->OptionalHeader.CheckSum);
}

inline DWORD PEDecoder::GetFileAlignment() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    //even though some data in OptionalHeader is different for 32 and 64,  this field is the same
    return VAL32(FindNTHeaders()->OptionalHeader.FileAlignment);
}

inline DWORD PEDecoder::GetSectionAlignment() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    //even though some data in OptionalHeader is different for 32 and 64,  this field is the same
    return VAL32(FindNTHeaders()->OptionalHeader.SectionAlignment);
}

inline WORD PEDecoder::GetMachine() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return VAL16(FindNTHeaders()->FileHeader.Machine);
}

inline SIZE_T PEDecoder::GetSizeOfStackReserve() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4328)
#endif

    if (Has32BitNTHeaders())
        return (SIZE_T) VAL32(GetNTHeaders32()->OptionalHeader.SizeOfStackReserve);
    else
        return (SIZE_T) VAL64(GetNTHeaders64()->OptionalHeader.SizeOfStackReserve);

#ifdef _MSC_VER
#pragma warning(pop)
#endif
}


inline SIZE_T PEDecoder::GetSizeOfStackCommit() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4328)
#endif

    if (Has32BitNTHeaders())
        return (SIZE_T) VAL32(GetNTHeaders32()->OptionalHeader.SizeOfStackCommit);
    else
        return (SIZE_T) VAL64(GetNTHeaders64()->OptionalHeader.SizeOfStackCommit);

#ifdef _MSC_VER
#pragma warning(pop)
#endif
}


inline SIZE_T PEDecoder::GetSizeOfHeapReserve() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4328)
#endif

    if (Has32BitNTHeaders())
        return (SIZE_T) VAL32(GetNTHeaders32()->OptionalHeader.SizeOfHeapReserve);
    else
        return (SIZE_T) VAL64(GetNTHeaders64()->OptionalHeader.SizeOfHeapReserve);

#ifdef _MSC_VER
#pragma warning(pop)
#endif
}


inline SIZE_T PEDecoder::GetSizeOfHeapCommit() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4328)
#endif

    if (Has32BitNTHeaders())
        return (SIZE_T) VAL32(GetNTHeaders32()->OptionalHeader.SizeOfHeapCommit);
    else
        return (SIZE_T) VAL64(GetNTHeaders64()->OptionalHeader.SizeOfHeapCommit);

#ifdef _MSC_VER
#pragma warning(pop)
#endif
}

inline UINT32 PEDecoder::GetLoaderFlags() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (Has32BitNTHeaders())
        return VAL32(GetNTHeaders32()->OptionalHeader.LoaderFlags);
    else
        return VAL32(GetNTHeaders64()->OptionalHeader.LoaderFlags);
}

inline COUNT_T PEDecoder::GetNumberOfRvaAndSizes() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (Has32BitNTHeaders())
        return VAL32(GetNTHeaders32()->OptionalHeader.NumberOfRvaAndSizes);
    else
        return VAL32(GetNTHeaders64()->OptionalHeader.NumberOfRvaAndSizes);
}

inline BOOL PEDecoder::HasDirectoryEntry(int entry) const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (Has32BitNTHeaders())
        return (GetNTHeaders32()->OptionalHeader.DataDirectory[entry].VirtualAddress != 0);
    else
        return (GetNTHeaders64()->OptionalHeader.DataDirectory[entry].VirtualAddress != 0);
}

inline IMAGE_DATA_DIRECTORY *PEDecoder::GetDirectoryEntry(int entry) const
{
    CONTRACT(IMAGE_DATA_DIRECTORY *)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(CheckPointer(RETVAL));
        SO_TOLERANT;
    }
    CONTRACT_END;

    if (Has32BitNTHeaders())
        RETURN PTR_IMAGE_DATA_DIRECTORY(
            PTR_HOST_TO_TADDR(GetNTHeaders32()) +
                              offsetof(IMAGE_NT_HEADERS32,
                                       OptionalHeader.DataDirectory) +
            entry * sizeof(IMAGE_DATA_DIRECTORY));
    else
        RETURN PTR_IMAGE_DATA_DIRECTORY(
            PTR_HOST_TO_TADDR(GetNTHeaders64()) +
                              offsetof(IMAGE_NT_HEADERS64,
                                       OptionalHeader.DataDirectory) +
            entry * sizeof(IMAGE_DATA_DIRECTORY));
}

inline TADDR PEDecoder::GetDirectoryEntryData(int entry, COUNT_T *pSize) const
{
    CONTRACT(TADDR)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        PRECONDITION(CheckDirectoryEntry(entry, 0, NULL_OK));
        PRECONDITION(CheckPointer(pSize, NULL_OK));
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(CheckPointer((void *)RETVAL, NULL_OK));
        SO_TOLERANT;
    }
    CONTRACT_END;

    IMAGE_DATA_DIRECTORY *pDir = GetDirectoryEntry(entry);

    if (pSize != NULL)
        *pSize = VAL32(pDir->Size);

    RETURN GetDirectoryData(pDir);
}

inline TADDR PEDecoder::GetDirectoryData(IMAGE_DATA_DIRECTORY *pDir) const
{
    CONTRACT(TADDR)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        PRECONDITION(CheckDirectory(pDir, 0, NULL_OK));
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
        POSTCONDITION(CheckPointer((void *)RETVAL, NULL_OK));
    }
    CONTRACT_END;

    // Prefix complained. GetRvaData returns NULL in case of failure anyway.
    if (pDir == NULL)
        RETURN NULL;
    else
        RETURN GetRvaData(VAL32(pDir->VirtualAddress));
}

inline TADDR PEDecoder::GetInternalAddressData(SIZE_T address) const
{
    CONTRACT(TADDR)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        PRECONDITION(CheckInternalAddress(address, NULL_OK));
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(CheckPointer((void *)RETVAL));
        SO_TOLERANT;
    }
    CONTRACT_END;

    RETURN GetRvaData(InternalAddressToRva(address));
}

inline BOOL PEDecoder::HasCorHeader() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return HasDirectoryEntry(IMAGE_DIRECTORY_ENTRY_COMHEADER);
}

inline BOOL PEDecoder::IsILOnly() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(HasCorHeader());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return((GetCorHeader()->Flags & VAL32(COMIMAGE_FLAGS_ILONLY)) != 0);
}

inline COUNT_T PEDecoder::RvaToOffset(RVA rva) const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        PRECONDITION(CheckRva(rva,NULL_OK));
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    if(rva > 0)
    {
        IMAGE_SECTION_HEADER *section = RvaToSection(rva);
        PREFIX_ASSUME (section!=-NULL); //TODO: actually it is possible that it si null we need to rethink how we handle this cases and do better there
    
        return rva - VAL32(section->VirtualAddress) + VAL32(section->PointerToRawData);
    }
    else return 0;
}

inline RVA PEDecoder::OffsetToRva(COUNT_T fileOffset) const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        PRECONDITION(CheckOffset(fileOffset,NULL_OK));
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    if(fileOffset > 0)
    {
        IMAGE_SECTION_HEADER *section = OffsetToSection(fileOffset);
        PREFIX_ASSUME (section!=-NULL); //TODO: actually it is possible that it si null we need to rethink how we handle this cases and do better there
    
        return fileOffset - VAL32(section->PointerToRawData) + VAL32(section->VirtualAddress);
    }
    else return 0;
}


inline BOOL PEDecoder::IsStrongNameSigned() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(HasCorHeader());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return ((GetCorHeader()->Flags & VAL32(COMIMAGE_FLAGS_STRONGNAMESIGNED)) != 0);
}

inline BOOL PEDecoder::HasStrongNameSignature() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(HasCorHeader());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return (GetCorHeader()->StrongNameSignature.VirtualAddress != 0);
}

inline CHECK PEDecoder::CheckStrongNameSignature() const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        PRECONDITION(HasCorHeader());
        PRECONDITION(HasStrongNameSignature());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_CHECK_END;

    return CheckDirectory(&GetCorHeader()->StrongNameSignature, IMAGE_SCN_MEM_WRITE, NULL_OK);
}

inline const void *PEDecoder::GetStrongNameSignature(COUNT_T *pSize) const
{
    CONTRACT(const void *)
    {
        INSTANCE_CHECK;
        PRECONDITION(HasCorHeader());
        PRECONDITION(HasStrongNameSignature());
        PRECONDITION(CheckStrongNameSignature());
        PRECONDITION(CheckPointer(pSize, NULL_OK));
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    IMAGE_DATA_DIRECTORY *pDir = &GetCorHeader()->StrongNameSignature;

    if (pSize != NULL)
        *pSize = VAL32(pDir->Size);

    RETURN (const void *) GetDirectoryData(pDir);
}

inline BOOL PEDecoder::HasTls() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    return HasDirectoryEntry(IMAGE_DIRECTORY_ENTRY_TLS);
}

inline CHECK PEDecoder::CheckTls() const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_CHECK_END;

    CHECK(CheckDirectoryEntry(IMAGE_DIRECTORY_ENTRY_TLS, 0, NULL_OK));

    IMAGE_TLS_DIRECTORY *pTlsHeader = (IMAGE_TLS_DIRECTORY *) GetDirectoryEntryData(IMAGE_DIRECTORY_ENTRY_TLS);

    CHECK(CheckUnderflow(VALPTR(pTlsHeader->EndAddressOfRawData), VALPTR(pTlsHeader->StartAddressOfRawData)));
    CHECK(VALPTR(pTlsHeader->EndAddressOfRawData) - VALPTR(pTlsHeader->StartAddressOfRawData) <= COUNT_T_MAX);

    CHECK(CheckInternalAddress(VALPTR(pTlsHeader->StartAddressOfRawData), 
        (COUNT_T) (VALPTR(pTlsHeader->EndAddressOfRawData) - VALPTR(pTlsHeader->StartAddressOfRawData))));

    CHECK_OK;
}

inline void *PEDecoder::GetTlsRange(COUNT_T *pSize) const
{
    CONTRACT(void *)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        PRECONDITION(HasTls());
        PRECONDITION(CheckTls());
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(CheckPointer(RETVAL));
        SO_TOLERANT;
    }
    CONTRACT_END;

    IMAGE_TLS_DIRECTORY *pTlsHeader =
        PTR_IMAGE_TLS_DIRECTORY(GetDirectoryEntryData(IMAGE_DIRECTORY_ENTRY_TLS));

    if (pSize != 0)
        *pSize = (COUNT_T) (VALPTR(pTlsHeader->EndAddressOfRawData) - VALPTR(pTlsHeader->StartAddressOfRawData));
    PREFIX_ASSUME (pTlsHeader!=NULL);
    RETURN (void *)GetInternalAddressData(pTlsHeader->StartAddressOfRawData);
}

inline UINT32 PEDecoder::GetTlsIndex() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        PRECONDITION(HasTls());
        PRECONDITION(CheckTls());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    IMAGE_TLS_DIRECTORY *pTlsHeader = (IMAGE_TLS_DIRECTORY *) GetDirectoryEntryData(IMAGE_DIRECTORY_ENTRY_TLS);
    
    return (UINT32)*PTR_UINT32(GetInternalAddressData(VALPTR(pTlsHeader->AddressOfIndex)));
}

inline IMAGE_COR20_HEADER *PEDecoder::GetCorHeader() const
{
    CONTRACT(IMAGE_COR20_HEADER *)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        PRECONDITION(HasCorHeader());
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    if (m_pCorHeader == NULL)
        const_cast<PEDecoder *>(this)->m_pCorHeader =
            PTR_IMAGE_COR20_HEADER(PTR_HOST_TO_TADDR(FindCorHeader()));

    RETURN m_pCorHeader;
}


inline BOOL PEDecoder::IsI386() const
{
    if (!HasContents() || !HasNTHeaders() )
        return FALSE;
    _ASSERTE(m_pNTHeaders);
    //do not call GetNTHeaders as we do not want to bother with PE32->PE32+ conversion
    return m_pNTHeaders->FileHeader.Machine==IMAGE_FILE_MACHINE_I386;
}

inline CORCOMPILE_HEADER *PEDecoder::GetNativeHeader() const
{
    CONTRACT(CORCOMPILE_HEADER *)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        PRECONDITION(HasCorHeader());
        PRECONDITION(HasNativeHeader());
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(CheckPointer(RETVAL));
        SO_TOLERANT;        
    }
    CONTRACT_END;

    if (m_pNativeHeader == NULL)
        const_cast<PEDecoder *>(this)->m_pNativeHeader =
            PTR_CORCOMPILE_HEADER(PTR_HOST_TO_TADDR(FindNativeHeader()));

    RETURN m_pNativeHeader;
}


// static
inline IMAGE_SECTION_HEADER *PEDecoder::FindFirstSection(IMAGE_NT_HEADERS* pNTHeaders)
{
    LEAF_CONTRACT;

    return PTR_IMAGE_SECTION_HEADER(
        PTR_HOST_TO_TADDR(pNTHeaders) +
        FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader) +
        VAL16(pNTHeaders->FileHeader.SizeOfOptionalHeader));
}

inline COUNT_T PEDecoder::GetNumberOfSections() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return VAL16(FindNTHeaders()->FileHeader.NumberOfSections);
}

inline COUNT_T PEDecoder::GetNumberOfWritableSections() const
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        PRECONDITION(HasNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    COUNT_T numOfWriteSecs = 0;
    IMAGE_SECTION_HEADER *sections = FindFirstSection();
    COUNT_T numOfSecs = GetNumberOfSections();
    for (COUNT_T idx=0; idx<numOfSecs; idx++)
    {
        if (sections[idx].Characteristics & VAL32(IMAGE_SCN_MEM_WRITE))
            numOfWriteSecs++;
    }             
    
    return numOfWriteSecs;
}


inline DWORD PEDecoder::GetImageIdentity() const
{
    WRAPPER_CONTRACT;
    return GetTimeDateStamp() ^ GetCheckSum() ^ DWORD( GetVirtualSize() );
}


inline IMAGE_SECTION_HEADER *PEDecoder::FindFirstSection() const
{
    CONTRACT(IMAGE_SECTION_HEADER *)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    RETURN FindFirstSection(FindNTHeaders());
}

inline IMAGE_NT_HEADERS *PEDecoder::FindNTHeaders() const
{
    CONTRACT(IMAGE_NT_HEADERS *)
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(CheckPointer(RETVAL));
        SO_TOLERANT;
    }
    CONTRACT_END;

    RETURN PTR_IMAGE_NT_HEADERS(m_base + VAL32(PTR_IMAGE_DOS_HEADER(m_base)->e_lfanew));
}

inline IMAGE_COR20_HEADER *PEDecoder::FindCorHeader() const
{
    CONTRACT(IMAGE_COR20_HEADER *)
    {
        INSTANCE_CHECK;
        PRECONDITION(HasCorHeader());
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;
    const IMAGE_COR20_HEADER * pCor=PTR_IMAGE_COR20_HEADER(GetDirectoryEntryData(IMAGE_DIRECTORY_ENTRY_COMHEADER));
    RETURN ((IMAGE_COR20_HEADER*)pCor);
}

inline CORCOMPILE_HEADER *PEDecoder::FindNativeHeader() const
{
    CONTRACT(CORCOMPILE_HEADER *)
    {
        INSTANCE_CHECK;
        PRECONDITION(HasNativeHeader());
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(CheckPointer(RETVAL));
        SO_TOLERANT;        
    }
    CONTRACT_END;

    RETURN PTR_CORCOMPILE_HEADER(GetDirectoryData(&GetCorHeader()->ManagedNativeHeader));
}

inline CHECK PEDecoder::CheckBounds(RVA rangeBase, COUNT_T rangeSize, RVA rva)
{
    WRAPPER_CONTRACT;
    CHECK(CheckOverflow(rangeBase, rangeSize));
    CHECK(rva >= rangeBase);
    CHECK(rva <= rangeBase + rangeSize);
    CHECK_OK;
}

inline CHECK PEDecoder::CheckBounds(RVA rangeBase, COUNT_T rangeSize, RVA rva, COUNT_T size)
{
    WRAPPER_CONTRACT;
    CHECK(CheckOverflow(rangeBase, rangeSize));
    CHECK(CheckOverflow(rva, size));
    CHECK(rva >= rangeBase);
    CHECK(rva + size <= rangeBase + rangeSize);
    CHECK_OK;
}

inline CHECK PEDecoder::CheckBounds(const void *rangeBase, COUNT_T rangeSize, const void *pointer)
{
    WRAPPER_CONTRACT;
    CHECK(CheckOverflow(rangeBase, rangeSize));
    CHECK(pointer >= rangeBase);
    CHECK((BYTE *) pointer <= (BYTE *) rangeBase + rangeSize);
    CHECK_OK;
}

inline CHECK PEDecoder::CheckBounds(const void *rangeBase, COUNT_T rangeSize, const void *pointer, COUNT_T size)
{
    WRAPPER_CONTRACT;
    CHECK(CheckOverflow(rangeBase, rangeSize));
    CHECK(CheckOverflow(pointer, size));
    CHECK(pointer >= rangeBase);
    CHECK((BYTE *) pointer + size <= (BYTE *) rangeBase + rangeSize);
    CHECK_OK;
}

inline void PEDecoder::GetPEKindAndMachine(DWORD* pdwKind, DWORD* pdwMachine)
{
    if(HasContents() && HasNTHeaders())
    {
        DWORD dwKind=0,dwMachine=0;
        BOOL fIsPE32Plus = !Has32BitNTHeaders(); 
        
        dwMachine=GetMachine();

        if (fIsPE32Plus)
            dwKind |= (DWORD)pe32Plus;

        if (HasCorHeader())
        {
            IMAGE_COR20_HEADER * pCorHdr = GetCorHeader();
            if(pCorHdr != NULL)
            {
                DWORD dwCorFlags = pCorHdr->Flags;

                if (dwCorFlags & VAL32(COMIMAGE_FLAGS_ILONLY))
                {
                    dwKind |= (DWORD)peILonly;
                }

                if (dwCorFlags & VAL32(COMIMAGE_FLAGS_32BITREQUIRED))
                    dwKind |= (DWORD)pe32BitRequired;

                // compensate for MC++ peculiarity
                if(dwKind == 0)
                    dwKind = (DWORD)pe32BitRequired;
            }
            else
            {
                dwKind |= (DWORD)pe32Unmanaged;
            }
        }
        else
        {
            dwKind |= (DWORD)pe32Unmanaged;
        }
        if(pdwKind) *pdwKind = dwKind;
        if(pdwMachine) *pdwMachine = dwMachine;
    }
}

#endif // _PEDECODER_INL_
