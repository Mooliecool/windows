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
// PEDecoder.cpp
// --------------------------------------------------------------------------------

#include "stdafx.h"

#include "ex.h"
#include "pedecoder.h"

CHECK PEDecoder::CheckFormat() const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_CHECK_END;

    CHECK(HasContents());

    if (HasNTHeaders())
    {
        CHECK(CheckNTHeaders());

        if (HasCorHeader())
        {
            CHECK(CheckCorHeader());

            if (IsILOnly())          
                CHECK(CheckILOnly());

            if (HasNativeHeader())
                CHECK(CheckNativeHeader());

            CHECK(CheckWillCreateGuardPage());
        }
    }

    CHECK_OK;
}

CHECK PEDecoder::CheckNTFormat() const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(HasContents());
    }
    CONTRACT_CHECK_END;

    CHECK(CheckFormat());
    CHECK(HasNTHeaders());

    CHECK_OK;
}

CHECK PEDecoder::CheckCORFormat() const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(HasContents());
    }
    CONTRACT_CHECK_END;

    CHECK(CheckFormat());
    CHECK(HasNTHeaders());
    CHECK(HasCorHeader());

    CHECK_OK;
}


CHECK PEDecoder::CheckILFormat() const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(HasContents());
    }
    CONTRACT_CHECK_END;

    CHECK(CheckFormat());
    CHECK(HasNTHeaders());
    CHECK(HasCorHeader());
    CHECK(!HasNativeHeader());

    CHECK_OK;
}


CHECK PEDecoder::CheckILOnlyFormat() const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(HasContents());
    }
    CONTRACT_CHECK_END;

    CHECK(CheckFormat());
    CHECK(HasNTHeaders());
    CHECK(HasCorHeader());
    CHECK(IsILOnly());
    CHECK(!HasNativeHeader());

    CHECK_OK;
}

CHECK PEDecoder::CheckNativeFormat() const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(HasContents());
    }
    CONTRACT_CHECK_END;

    CHECK(false);

    CHECK_OK;
}

BOOL PEDecoder::HasNTHeaders() const
{
    CONTRACT(BOOL)
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(HasContents());
    }
    CONTRACT_END;

    // Check for a valid DOS header

    if (m_size < sizeof(IMAGE_DOS_HEADER))
        RETURN FALSE;

    IMAGE_DOS_HEADER* pDOS = PTR_IMAGE_DOS_HEADER(m_base);

    {
        if (pDOS->e_magic != VAL16(IMAGE_DOS_SIGNATURE)
            || (DWORD) pDOS->e_lfanew == VAL32(0))
        {
            RETURN FALSE;
        }

        // Check for integer overflow
        S_SIZE_T cbNTHeaderEnd(S_SIZE_T(static_cast<SIZE_T>(VAL32(pDOS->e_lfanew))) +
                               S_SIZE_T(sizeof(IMAGE_NT_HEADERS)));
        if (cbNTHeaderEnd.IsOverflow())
        {
            RETURN FALSE;
        }

        // Now check for a valid NT header
        if (m_size < cbNTHeaderEnd.Value())
        {
            RETURN FALSE;
        }
    }

    IMAGE_NT_HEADERS *pNT = PTR_IMAGE_NT_HEADERS(m_base + VAL32(pDOS->e_lfanew));

    if (pNT->Signature != VAL32(IMAGE_NT_SIGNATURE))
        RETURN FALSE;

    if (pNT->OptionalHeader.Magic == VAL16(IMAGE_NT_OPTIONAL_HDR32_MAGIC))
    {
        if (pNT->FileHeader.SizeOfOptionalHeader != VAL16(IMAGE_SIZEOF_NT_OPTIONAL32_HEADER))
            RETURN FALSE;
    }
    else if (pNT->OptionalHeader.Magic == VAL16(IMAGE_NT_OPTIONAL_HDR64_MAGIC))
    {
        // on 64 bit we can promote this
        if (pNT->FileHeader.SizeOfOptionalHeader != VAL16(IMAGE_SIZEOF_NT_OPTIONAL64_HEADER))
            RETURN FALSE;
    }
    else
        RETURN FALSE;

    // Go ahead and cache NT header since we already found it.
    const_cast<PEDecoder *>(this)->m_pNTHeaders =
        PTR_IMAGE_NT_HEADERS(PTR_HOST_TO_TADDR(pNT));

    RETURN TRUE;
}

CHECK PEDecoder::CheckNTHeaders() const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(HasContents());
    }
    CONTRACT_CHECK_END;

    // Only check once per file
    if (m_flags & FLAG_NT_CHECKED)
        CHECK_OK;

    CHECK(HasNTHeaders());

    IMAGE_NT_HEADERS *pNT = FindNTHeaders();

    CHECK((pNT->FileHeader.Characteristics & VAL16(IMAGE_FILE_SYSTEM)) == 0);

    CHECK(CheckAlignment(VAL32(pNT->OptionalHeader.FileAlignment)));
    CHECK(CheckAlignment(VAL32(pNT->OptionalHeader.SectionAlignment)));

    CHECK(CheckAligned((UINT)VAL32(pNT->OptionalHeader.FileAlignment), 512));
    CHECK(CheckAligned((UINT)VAL32(pNT->OptionalHeader.SectionAlignment), VAL32(pNT->OptionalHeader.FileAlignment)));

    // INVESTIGATE: this doesn't seem to be necessary on Win64 - why??
    //CHECK(CheckAligned((UINT)VAL32(pNT->OptionalHeader.SectionAlignment), OS_PAGE_SIZE));
    CHECK(CheckAligned((UINT)VAL32(pNT->OptionalHeader.SectionAlignment), 0x1000)); // for base relocs logic
    CHECK(CheckAligned((UINT)VAL32(pNT->OptionalHeader.SizeOfImage), VAL32(pNT->OptionalHeader.SectionAlignment)));
    CHECK(CheckAligned((UINT)VAL32(pNT->OptionalHeader.SizeOfHeaders), VAL32(pNT->OptionalHeader.FileAlignment)));

    // Data directories will be validated later on.
    PTR_IMAGE_DATA_DIRECTORY pDataDirectories = NULL;

    if (Has32BitNTHeaders())
    {
        IMAGE_NT_HEADERS32* pNT32=GetNTHeaders32();
        CHECK(CheckAligned(VAL32(pNT32->OptionalHeader.ImageBase), 0x10000));
        CHECK((VAL32(pNT32->OptionalHeader.SizeOfStackCommit) <= VAL32(pNT32->OptionalHeader.SizeOfStackReserve)));
        CHECK((VAL32(pNT32->OptionalHeader.SizeOfHeapCommit) <= VAL32(pNT32->OptionalHeader.SizeOfHeapReserve)));
        pDataDirectories = PTR_IMAGE_DATA_DIRECTORY(
            PTR_HOST_TO_TADDR(pNT32) + offsetof(IMAGE_NT_HEADERS32, OptionalHeader.DataDirectory));
    }
    else
    {
        IMAGE_NT_HEADERS64* pNT64=GetNTHeaders64();
        CHECK(CheckAligned(VAL64(pNT64->OptionalHeader.ImageBase), 0x10000));
        CHECK((VAL64(pNT64->OptionalHeader.SizeOfStackCommit) <= VAL64(pNT64->OptionalHeader.SizeOfStackReserve)));
        CHECK((VAL64(pNT64->OptionalHeader.SizeOfHeapCommit) <= VAL64(pNT64->OptionalHeader.SizeOfHeapReserve)));
        pDataDirectories = PTR_IMAGE_DATA_DIRECTORY(
            PTR_HOST_TO_TADDR(pNT64) + offsetof(IMAGE_NT_HEADERS64, OptionalHeader.DataDirectory));
    }


    if (IsMapped())
    {
        // Ideally we would require the layout address to honor the section alignment constraints.
        // However, we do have 8K aligned IL only images which we load on 32 bit platforms. In this
        // case, we can only guarantee OS page alignment (which after all, is good enough.)
        CHECK(CheckAligned(m_base, OS_PAGE_SIZE));
    }


    UINT32 currentAddress  = 0;
    UINT32 currentOffset = 0;

    CHECK(CheckSection(currentAddress, 0, VAL32(pNT->OptionalHeader.SizeOfHeaders),
                       currentOffset, 0, VAL32(pNT->OptionalHeader.SizeOfHeaders)));

    IMAGE_SECTION_HEADER *section = FindFirstSection(pNT);
    IMAGE_SECTION_HEADER *sectionEnd = section + VAL16(pNT->FileHeader.NumberOfSections);

    while (section < sectionEnd)
    {
        // Check flags
        // Only allow a small list of characteristics
        CHECK(!(section->Characteristics &
            ~(VAL32((IMAGE_SCN_CNT_CODE           |
                  IMAGE_SCN_CNT_INITIALIZED_DATA  |
                  IMAGE_SCN_CNT_UNINITIALIZED_DATA|
                  IMAGE_SCN_MEM_DISCARDABLE       |
                  IMAGE_SCN_MEM_NOT_CACHED        |
                  IMAGE_SCN_MEM_NOT_PAGED         |
                  IMAGE_SCN_MEM_EXECUTE           |
                  IMAGE_SCN_MEM_READ              |
                  IMAGE_SCN_MEM_WRITE             |
                  // allow shared sections for all images for now.
                  // we'll constrain this in CheckILOnly
                  IMAGE_SCN_MEM_SHARED)))));

        // we should not allow writable code sections, check if both flags are set
        CHECK((section->Characteristics & VAL32((IMAGE_SCN_CNT_CODE|IMAGE_SCN_MEM_WRITE))) !=
            VAL32((IMAGE_SCN_CNT_CODE|IMAGE_SCN_MEM_WRITE)));

        CHECK(CheckSection(currentAddress, VAL32(section->VirtualAddress), VAL32(section->Misc.VirtualSize),
                           currentOffset, VAL32(section->PointerToRawData), VAL32(section->SizeOfRawData)));

        currentAddress = VAL32(section->VirtualAddress)
          + AlignUp((UINT)VAL32(section->Misc.VirtualSize), (UINT)VAL32(pNT->OptionalHeader.SectionAlignment));
        currentOffset = VAL32(section->PointerToRawData) + VAL32(section->SizeOfRawData);

        section++;
    }

    // Now check that the COR data directory is either NULL, or exists entirely in one section.
    {
        PTR_IMAGE_DATA_DIRECTORY pCORDataDir = pDataDirectories + IMAGE_DIRECTORY_ENTRY_COMHEADER;
        CHECK(CheckRva(pCORDataDir->VirtualAddress, pCORDataDir->Size, 0, NULL_OK));
    }


    const_cast<PEDecoder *>(this)->m_flags |= FLAG_NT_CHECKED;

    CHECK_OK;
}

CHECK PEDecoder::CheckSection(COUNT_T previousAddressEnd, COUNT_T addressStart, COUNT_T addressSize,
                              COUNT_T previousOffsetEnd, COUNT_T offsetStart, COUNT_T offsetSize) const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(HasNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_CHECK_END;

    // Fetch the NT header
    IMAGE_NT_HEADERS *pNT = FindNTHeaders();

    // OS will zero pad a mapped file up to file alignment size - some images rely on this
    COUNT_T alignedSize = AlignUp(m_size, VAL32(pNT->OptionalHeader.FileAlignment));

    // Check to make sure that our memory is big enough to cover the stated range.
    // Note that this check is only required if we have a non-flat image.
    if (IsMapped())
        CHECK(alignedSize >= VAL32(pNT->OptionalHeader.SizeOfImage));

    // Check expected alignments
    CHECK(CheckAligned(addressStart, VAL32(pNT->OptionalHeader.SectionAlignment)));
    CHECK(CheckAligned(offsetStart, VAL32(pNT->OptionalHeader.FileAlignment)));
    CHECK(CheckAligned(offsetSize, VAL32(pNT->OptionalHeader.FileAlignment)));

    // addressSize is typically not aligned, so we align it for purposes of checks.
    COUNT_T alignedAddressSize = AlignUp(addressSize, VAL32(pNT->OptionalHeader.SectionAlignment));
    CHECK(addressSize <= alignedAddressSize);

    // Check overflow
    CHECK(CheckOverflow(addressStart, alignedAddressSize));
    CHECK(CheckOverflow(offsetStart, offsetSize));

    // Make sure we don't overlap the previous section
    CHECK(addressStart >= previousAddressEnd
          && (offsetSize == 0
              || offsetStart >= previousOffsetEnd));

    // Make sure we don't overrun the end of the mapped image
    CHECK(addressStart + alignedAddressSize <= VAL32(pNT->OptionalHeader.SizeOfImage));

    // Make sure we don't overrun the end of the file (only relevant if we're not mapped, otherwise
    // we don't know the file size, as it's not declared in the headers.)
    if (!IsMapped())
        CHECK(offsetStart + offsetSize <= alignedSize);

    // Make sure the data doesn't overrun the virtual address space
    CHECK(offsetSize <= alignedAddressSize);

    CHECK_OK;
}

BOOL PEDecoder::PromoteNTHeaders()
{
    CONTRACT(BOOL)
    {
        INSTANCE_CHECK;
        PRECONDITION(HasNTHeaders());
        PRECONDITION(CheckNTHeaders());
        POSTCONDITION(!RETVAL || Has32BitNTHeaders());
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACT_END;

    // We may need to convert the headers to the native format.

    IMAGE_NT_HEADERS *pNT = FindNTHeaders();

    if (pNT->OptionalHeader.Magic != VAL16(IMAGE_NT_OPTIONAL_HDR_MAGIC))
    {
        // We need to convert header to native version.

        // First, see if we have enough slack space to expand the header in place.
        IMAGE_SECTION_HEADER *pSection = FindFirstSection(pNT);

        // See if we can update the headers in place.  Don't even attempt this
        // if we are a flat file, since that will screw up any hash computations. </STRIP>

        if (IsMapped())
        {
            BYTE *pSectionStart = ((BYTE *) m_base) + VAL32(pSection->VirtualAddress);

            BOOL canConvert;
            IMAGE_NT_HEADERS64 *pNT64 = (IMAGE_NT_HEADERS64*) pNT;
            canConvert = (VAL64(pNT64->OptionalHeader.SizeOfHeapCommit) <= UINT32_MAX
                          && VAL64(pNT64->OptionalHeader.SizeOfHeapReserve) <= UINT32_MAX
                          && VAL64(pNT64->OptionalHeader.SizeOfStackCommit) <= UINT32_MAX
                          && VAL64(pNT64->OptionalHeader.SizeOfStackReserve) <= UINT32_MAX
                          && VAL64(pNT64->OptionalHeader.ImageBase) <= UINT32_MAX);

            if (canConvert)
            {
                DWORD oldProtection;
                if (ClrVirtualProtect((void *) pNT, pSectionStart - (BYTE*) pNT,
                                       PAGE_READWRITE, &oldProtection))
                {
                    TranslateNTHeaders(pNT, pNT);

                    ClrVirtualProtect((void *) pNT, pSectionStart - (BYTE*) pNT,
                                      oldProtection, &oldProtection);

                    RETURN TRUE;
                }
            }
        }

        // @future
        // Here, we could allocate a block of memory, convert the header into that memory,
        // and set it as m_pNTHeader. For now, we are not doing this because we don't
        // think we care about this case, and we don't want to give PEDecoder a destructor.

        RETURN FALSE;
    }

    RETURN TRUE;
}


void PEDecoder::TranslateNTHeaders(const void *from, IMAGE_NT_HEADERS *to)
{
    CONTRACT_VOID
    {
        // NOTE: from MAY equal to (the copy can occur in place)
        PRECONDITION(CheckPointer(from));
        PRECONDITION(CheckPointer(to));
        POSTCONDITION(to->OptionalHeader.Magic == VAL16(IMAGE_NT_OPTIONAL_HDR_MAGIC));
        POSTCONDITION(to->FileHeader.SizeOfOptionalHeader == VAL16(IMAGE_SIZEOF_NT_OPTIONAL_HEADER));
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_END;

    IMAGE_NT_HEADERS64 *pFrom = (IMAGE_NT_HEADERS64 *) from;;
#define TRANSLATE(x)    VAL32((UINT32)VAL64(x))

    // Move the data directory and section headers down 16 bytes.
    const BYTE *pEndFrom = (const BYTE *) (FindFirstSection((IMAGE_NT_HEADERS *)pFrom) +
                                           VAL16(pFrom->FileHeader.NumberOfSections));
    const BYTE *pStartFrom = (const BYTE *) &pFrom->OptionalHeader.DataDirectory[0];

    BYTE *pStartTo = (BYTE *) &to->OptionalHeader.DataDirectory[0];

    MoveMemory(pStartTo, pStartFrom, pEndFrom - pStartFrom);

    // Move the tail fields in reverse order.
    to->OptionalHeader.NumberOfRvaAndSizes = pFrom->OptionalHeader.NumberOfRvaAndSizes;
    to->OptionalHeader.LoaderFlags = pFrom->OptionalHeader.LoaderFlags;

    // Promote fields to 64 bits
    to->OptionalHeader.SizeOfHeapCommit = TRANSLATE(pFrom->OptionalHeader.SizeOfHeapCommit);
    to->OptionalHeader.SizeOfHeapReserve = TRANSLATE(pFrom->OptionalHeader.SizeOfHeapReserve);
    to->OptionalHeader.SizeOfStackCommit = TRANSLATE(pFrom->OptionalHeader.SizeOfStackCommit);
    to->OptionalHeader.SizeOfStackReserve = TRANSLATE(pFrom->OptionalHeader.SizeOfStackReserve);
    to->OptionalHeader.ImageBase = TRANSLATE(pFrom->OptionalHeader.ImageBase);

    // The optional header changed size.
    to->FileHeader.SizeOfOptionalHeader
      = VAL16(VAL16(pFrom->FileHeader.SizeOfOptionalHeader) + sizeof(*to) - sizeof(*pFrom));

    to->OptionalHeader.Magic = VAL16(IMAGE_NT_OPTIONAL_HDR_MAGIC);

#undef TRANSLATE

    RETURN;
}


CHECK PEDecoder::CheckDirectoryEntry(int entry, int forbiddenFlags, IsNullOK ok) const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        PRECONDITION(entry < IMAGE_NUMBEROF_DIRECTORY_ENTRIES);
        PRECONDITION(HasDirectoryEntry(entry));
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_CHECK_END;

    CHECK(CheckDirectory(GetDirectoryEntry(entry), forbiddenFlags, ok));

    CHECK_OK;
}

CHECK PEDecoder::CheckDirectory(IMAGE_DATA_DIRECTORY *pDir, int forbiddenFlags, IsNullOK ok) const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        PRECONDITION(CheckPointer(pDir));
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_CHECK_END;

    CHECK(CheckRva(VAL32(pDir->VirtualAddress), VAL32(pDir->Size), forbiddenFlags, ok));

    CHECK_OK;
}

CHECK PEDecoder::CheckRva(RVA rva, COUNT_T size, int forbiddenFlags, IsNullOK ok) const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_CHECK_END;

    if (rva == 0)
    {
        CHECK_MSG(ok == NULL_OK, "Zero RVA illegal");
        CHECK(size == 0);
    }
    else
    {
        IMAGE_SECTION_HEADER *section = RvaToSection(rva);

        CHECK(section != NULL);

        CHECK(CheckBounds(VAL32(section->VirtualAddress),
                          AlignUp((UINT)VAL32(section->Misc.VirtualSize), (UINT)VAL32(FindNTHeaders()->OptionalHeader.SectionAlignment)),
                          rva, size));
        if (forbiddenFlags!=0)
            CHECK((section->Characteristics & VAL32(forbiddenFlags))==0);
    }

    CHECK_OK;
}

CHECK PEDecoder::CheckRva(RVA rva, IsNullOK ok) const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_CHECK_END;

    if (rva == 0)
        CHECK_MSG(ok == NULL_OK, "Zero RVA illegal");
    else
        CHECK(RvaToSection(rva) != NULL);

    CHECK_OK;
}

CHECK PEDecoder::CheckOffset(COUNT_T fileOffset, COUNT_T size, IsNullOK ok) const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_CHECK_END;

    if (fileOffset == 0)
    {
        CHECK_MSG(ok == NULL_OK, "zero fileOffset illegal");
        CHECK(size == 0);
    }
    else
    {
        IMAGE_SECTION_HEADER *section = OffsetToSection(fileOffset);

        CHECK(section != NULL);

        CHECK(CheckBounds(section->PointerToRawData, section->SizeOfRawData,
                          fileOffset, size));
    }

    CHECK_OK;
}

CHECK PEDecoder::CheckOffset(COUNT_T fileOffset, IsNullOK ok) const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_CHECK_END;

    if (fileOffset == NULL)
        CHECK_MSG(ok == NULL_OK, "Null pointer illegal");
    else
    {
        CHECK(OffsetToSection(fileOffset) != NULL);
    }

    CHECK_OK;
}

CHECK PEDecoder::CheckData(const void *data, COUNT_T size, IsNullOK ok) const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_CHECK_END;

    if (data == NULL)
    {
        CHECK_MSG(ok == NULL_OK, "NULL pointer illegal");
        CHECK(size == 0);
    }
    else
    {
        CHECK(CheckUnderflow(data, m_base));
        CHECK((UINT_PTR) (((BYTE *) data) - ((BYTE *) m_base)) <= COUNT_T_MAX);

        if (IsMapped())
            CHECK(CheckRva((COUNT_T) ((BYTE *) data - (BYTE *) m_base), size));
        else
            CHECK(CheckOffset((COUNT_T) ((BYTE *) data - (BYTE *) m_base), size));
    }

    CHECK_OK;
}

CHECK PEDecoder::CheckData(const void *data, IsNullOK ok) const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_CHECK_END;

    if (data == NULL)
        CHECK_MSG(ok == NULL_OK, "Null pointer illegal");
    else
    {
        CHECK(CheckUnderflow(data, m_base));
        CHECK((UINT_PTR) (((BYTE *) data) - ((BYTE *) m_base)) <= COUNT_T_MAX);

        if (IsMapped())
            CHECK(CheckRva((COUNT_T) ((BYTE *) data - (BYTE *) m_base)));
        else
            CHECK(CheckOffset((COUNT_T) ((BYTE *) data - (BYTE *) m_base)));
    }

    CHECK_OK;
}

CHECK PEDecoder::CheckInternalAddress(SIZE_T address, IsNullOK ok) const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_CHECK_END;

    if (address == 0)
        CHECK_MSG(ok == NULL_OK, "Zero RVA illegal");
    else
        CHECK(RvaToSection(InternalAddressToRva(address)) != NULL);

    CHECK_OK;
}

CHECK PEDecoder::CheckInternalAddress(SIZE_T address, COUNT_T size, IsNullOK ok) const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_CHECK_END;

    if (address == 0)
    {
        CHECK_MSG(ok == NULL_OK, "Zero RVA illegal");
        CHECK(size == 0);
    }
    else
    {
        CHECK(CheckRva(InternalAddressToRva(address), size));
    }

    CHECK_OK;
}

RVA PEDecoder::InternalAddressToRva(SIZE_T address) const
{
    CONTRACT(RVA)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(CheckRva(RETVAL));
        SO_TOLERANT;
    }
    CONTRACT_END;

    if (m_flags & FLAG_RELOCATED)
    {
        // Address has been fixed up
        RETURN (RVA) ((BYTE *) address - (BYTE *) m_base);
    }
    else
    {
        // Address has not been fixed up
        RETURN (RVA) (address - (SIZE_T) GetPreferredBase());
    }
}

IMAGE_SECTION_HEADER *PEDecoder::RvaToSection(RVA rva) const
{
    CONTRACT(IMAGE_SECTION_HEADER *)
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    PTR_IMAGE_SECTION_HEADER section =
        PTR_IMAGE_SECTION_HEADER(PTR_HOST_TO_TADDR(FindFirstSection(FindNTHeaders())));
    PTR_IMAGE_SECTION_HEADER sectionEnd = section + VAL16(FindNTHeaders()->FileHeader.NumberOfSections);

    while (section < sectionEnd)
    {
        if (rva < (VAL32(section->VirtualAddress)
                   + AlignUp((UINT)VAL32(section->Misc.VirtualSize), (UINT)VAL32(FindNTHeaders()->OptionalHeader.SectionAlignment))))
        {
            if (rva < VAL32(section->VirtualAddress))
                RETURN NULL;
            else
            {
                RETURN section;
            }
        }

        section++;
    }

    RETURN NULL;
}

IMAGE_SECTION_HEADER *PEDecoder::OffsetToSection(COUNT_T fileOffset) const
{
    CONTRACT(IMAGE_SECTION_HEADER *)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    PTR_IMAGE_SECTION_HEADER section =
        PTR_IMAGE_SECTION_HEADER(PTR_HOST_TO_TADDR(FindFirstSection(FindNTHeaders())));
    PTR_IMAGE_SECTION_HEADER sectionEnd = section + VAL16(FindNTHeaders()->FileHeader.NumberOfSections);

    while (section < sectionEnd)
    {
        if (fileOffset < section->PointerToRawData + section->SizeOfRawData)
        {
            if (fileOffset < section->PointerToRawData)
                RETURN NULL;
            else
                RETURN section;
        }

        section++;
    }

    RETURN NULL;
}

TADDR PEDecoder::GetRvaData(RVA rva, IsNullOK ok /*= NULL_NOT_OK*/) const
{
    CONTRACT(TADDR)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        PRECONDITION(CheckRva(rva, NULL_OK));
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;    
    }
    CONTRACT_END;

    if ((rva == 0)&&(ok == NULL_NOT_OK))
        RETURN NULL;

    RVA offset;
    if (IsMapped())
        offset = rva;
    else
    {
        // !!! check for case where rva is in padded portion of segment
        offset = RvaToOffset(rva);
    }

    RETURN( m_base + offset );
}

RVA PEDecoder::GetDataRva(const TADDR data) const
{
    CONTRACT(RVA)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        PRECONDITION(CheckData((void *)data, NULL_OK));
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_END;

    if (data == NULL)
        RETURN 0;

    COUNT_T offset = (COUNT_T) (data - m_base);
    if (IsMapped())
        RETURN offset;
    else
        RETURN OffsetToRva(offset);
}

TADDR PEDecoder::GetOffsetData(COUNT_T fileOffset, IsNullOK ok /*= NULL_NOT_OK*/) const
{
    CONTRACT(TADDR)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        PRECONDITION(CheckOffset(fileOffset, NULL_OK));
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_END;

    if ((fileOffset == 0)&&(ok == NULL_NOT_OK))
        RETURN NULL;

    RETURN GetRvaData(OffsetToRva(fileOffset));
}

//-------------------------------------------------------------------------------
// Lifted from "..\md\inc\mdfileformat.h"
// (cannot #include it here because it references lot of other stuff)
#define STORAGE_MAGIC_SIG   0x424A5342  // BSJB
struct STORAGESIGNATURE
{
    ULONG       lSignature;             // "Magic" signature.
    USHORT      iMajorVer;              // Major file version.
    USHORT      iMinorVer;              // Minor file version.
    ULONG       iExtraData;             // Offset to next structure of information 
    ULONG       iVersionString;         // Length of version string
};
typedef STORAGESIGNATURE UNALIGNED * PSTORAGESIGNATURE;

struct STORAGEHEADER
{
    BYTE        fFlags;                 // STGHDR_xxx flags.
    BYTE        pad;
    USHORT      iStreams;               // How many streams are there.
};
typedef STORAGEHEADER UNALIGNED * PSTORAGEHEADER;

struct STORAGESTREAM
{
    ULONG       iOffset;                // Offset in file for this stream.
    ULONG       iSize;                  // Size of the file.
    char        rcName[32];  // Start of name, null terminated.
};
typedef STORAGESTREAM UNALIGNED * PSTORAGESTREAM;

// if the stream's name is shorter than 32 bytes (incl.zero terminator),
// the size of storage stream header is less than sizeof(STORAGESTREAM)
// and is padded to 4-byte alignment
inline PSTORAGESTREAM NextStorageStream(PSTORAGESTREAM pSS)
{
    BYTE* pc = (BYTE*)pSS;
    pc += (sizeof(STORAGESTREAM) - 32 /*sizeof(STORAGESTREAM::rcName)*/ + strlen(pSS->rcName)+1+3)&~3;
    return (PSTORAGESTREAM)pc;
}
//-------------------------------------------------------------------------------


CHECK PEDecoder::CheckCorHeader() const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_CHECK_END;

    if (m_flags & FLAG_COR_CHECKED)
        CHECK_OK;

    CHECK(CheckNTHeaders());

    CHECK(HasCorHeader());

    IMAGE_DATA_DIRECTORY *pDir = GetDirectoryEntry(IMAGE_DIRECTORY_ENTRY_COMHEADER);

    CHECK(CheckDirectory(pDir, IMAGE_SCN_MEM_WRITE, NULL_NOT_OK));

    CHECK(VAL32(pDir->Size) >= sizeof(IMAGE_COR20_HEADER));

    IMAGE_SECTION_HEADER *section = RvaToSection(VAL32(pDir->VirtualAddress));
    CHECK(section != NULL);
    CHECK((section->Characteristics & VAL32(IMAGE_SCN_MEM_READ))!=0);

    IMAGE_COR20_HEADER *pCor = GetCorHeader();

    //CHECK(((ULONGLONG)pCor & 0x3)==0);

    // If the file is COM+ 1.0, which by definition has nothing the runtime can
    // use, or if the file requires a newer version of this engine than us,
    // it cannot be run by this engine.
    CHECK(VAL16(pCor->MajorRuntimeVersion) > 1 && VAL16(pCor->MajorRuntimeVersion) <= COR_VERSION_MAJOR);

    CHECK(CheckDirectory(&pCor->MetaData, IMAGE_SCN_MEM_WRITE, HasNativeHeader() ? NULL_OK : NULL_NOT_OK));
    CHECK(CheckDirectory(&pCor->Resources, IMAGE_SCN_MEM_WRITE, NULL_OK));
    CHECK(CheckDirectory(&pCor->StrongNameSignature, IMAGE_SCN_MEM_WRITE, NULL_OK));
    CHECK(CheckDirectory(&pCor->CodeManagerTable, IMAGE_SCN_MEM_WRITE, NULL_OK));
    CHECK(CheckDirectory(&pCor->VTableFixups, 0, NULL_OK));
    CHECK(CheckDirectory(&pCor->ExportAddressTableJumps, 0, NULL_OK));
    CHECK(CheckDirectory(&pCor->ManagedNativeHeader, 0, NULL_OK));

    CHECK(VAL32(pCor->cb) >= offsetof(IMAGE_COR20_HEADER, ManagedNativeHeader) + sizeof(IMAGE_DATA_DIRECTORY));

    DWORD validBits = COMIMAGE_FLAGS_ILONLY
      | COMIMAGE_FLAGS_32BITREQUIRED
      | COMIMAGE_FLAGS_TRACKDEBUGDATA
      | COMIMAGE_FLAGS_STRONGNAMESIGNED
      | COMIMAGE_FLAGS_NATIVE_ENTRYPOINT
      | COMIMAGE_FLAGS_IL_LIBRARY;

    CHECK((pCor->Flags&VAL32(~validBits)) == 0);

    // Pure IL images should not have VTable fixups or EAT jumps
    if (IsILOnly())
    {
        CHECK(pCor->VTableFixups.Size == VAL32(0));
        CHECK(pCor->ExportAddressTableJumps.Size == VAL32(0));
        CHECK(!(pCor->Flags & VAL32(COMIMAGE_FLAGS_NATIVE_ENTRYPOINT)));
    }
    else
    {
        if (pCor->Flags & VAL32(COMIMAGE_FLAGS_NATIVE_ENTRYPOINT))
        {
            CHECK(CheckRva(VAL32(IMAGE_COR20_HEADER_FIELD(*pCor,EntryPointRVA))));
        }
    }

    // Strong name signed images should have a signature
    if (IsStrongNameSigned())
        CHECK(HasStrongNameSignature());

    // IL library files (really a misnomer - these are native images) only
    // may have a native image header
    if ((pCor->Flags&VAL32(COMIMAGE_FLAGS_IL_LIBRARY)) == 0)
    {
        CHECK(VAL32(pCor->ManagedNativeHeader.Size) == 0);
    }

    // Metadata header checks
    IMAGE_DATA_DIRECTORY *pDirMD = &pCor->MetaData;
    COUNT_T ctMD = (COUNT_T)VAL32(pDirMD->Size);
    BYTE*   pcMD = (BYTE*)GetDirectoryData(pDirMD);

    if(pcMD != NULL)
    {
        // Storage signature checks
        const STORAGESIGNATURE *pSSig = (STORAGESIGNATURE*)pcMD;
        CHECK(VAL32(pSSig->lSignature) == STORAGE_MAGIC_SIG);
        COUNT_T ctSSig;
        CHECK(ClrSafeInt<COUNT_T>::addition(sizeof(STORAGESIGNATURE), (COUNT_T)VAL32(pSSig->iVersionString), ctSSig));
        CHECK(ctMD > ctSSig);

        // Storage header checks
        pcMD += ctSSig;
        ctMD -= ctSSig;
        CHECK(ctMD >= sizeof(STORAGEHEADER));
        PSTORAGEHEADER pSHdr = (PSTORAGEHEADER)pcMD;
        pcMD = (BYTE*)(pSHdr+1);
        ctMD -= sizeof(STORAGEHEADER);
        WORD nStreams = VAL16(pSHdr->iStreams);
        
        // Storage streams checks
        PSTORAGESTREAM pStr = (PSTORAGESTREAM)pcMD;
        PSTORAGESTREAM pSSOutOfRange = (PSTORAGESTREAM)(pcMD + ctMD);
        size_t namelen;
        WORD iStr;
        PSTORAGESTREAM pSS;
        for(iStr = 1, pSS = pStr; iStr <= nStreams; iStr++)
        {
            CHECK(pSS < pSSOutOfRange);
            
            for(namelen=0; (namelen<32)&&(pSS->rcName[namelen]!=0); namelen++);
            CHECK((0 < namelen)&&(namelen < 32));

            pcMD = (BYTE*)NextStorageStream(pSS);
            ctMD -= (COUNT_T)(pcMD - (BYTE*)pSS);
            
            pSS = (PSTORAGESTREAM)pcMD;
        }

        // At this moment, pcMD is pointing past the last stream header
        // and ctMD contains total size left for streams per se
        // Now, check the offsets and sizes of streams
        COUNT_T ctStreamsBegin = (COUNT_T)(pcMD - (BYTE*)pSSig);  // min.possible offset
        COUNT_T  ctSS, ctSSbegin, ctSSend;
        for(iStr = 1, pSS = pStr; iStr <= nStreams; iStr++,pSS = NextStorageStream(pSS))
        {
            ctSSbegin = (COUNT_T)VAL32(pSS->iOffset);
            CHECK(ctStreamsBegin <= ctSSbegin);


            ctSS = (COUNT_T)VAL32(pSS->iSize);
            CHECK(ctMD >= ctSS);
            CHECK(ClrSafeInt<COUNT_T>::addition(ctSSbegin, ctSS, ctSSend));
            ctMD -= ctSS;

            // Check stream overlap
            for(PSTORAGESTREAM pSSprior=pStr; pSSprior < pSS; pSSprior = NextStorageStream(pSSprior))
            {
                COUNT_T ctSSprior_end;
                CHECK(ClrSafeInt<COUNT_T>::addition((COUNT_T)VAL32(pSSprior->iOffset), (COUNT_T)VAL32(pSSprior->iSize), ctSSprior_end));
                CHECK((ctSSbegin >= ctSSprior_end)||(ctSSend <= (COUNT_T)VAL32(pSSprior->iOffset)));
            }
        }
    }  //end if(pcMD != NULL)
    
    const_cast<PEDecoder *>(this)->m_flags |= FLAG_COR_CHECKED;

    CHECK_OK;
}

const void *PEDecoder::GetMetadata(COUNT_T *pSize) const
{
    CONTRACT(const void *)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckCorHeader());
        PRECONDITION(CheckPointer(pSize, NULL_OK));
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_END;

    IMAGE_DATA_DIRECTORY *pDir = &GetCorHeader()->MetaData;

    if (pSize != NULL)
        *pSize = VAL32(pDir->Size);

    RETURN (void *)GetDirectoryData(pDir);
}

const void *PEDecoder::GetResources(COUNT_T *pSize) const
{
    CONTRACT(const void *)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckCorHeader());
        PRECONDITION(CheckPointer(pSize, NULL_OK));
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_END;

    IMAGE_DATA_DIRECTORY *pDir = &GetCorHeader()->Resources;

    if (pSize != NULL)
        *pSize = VAL32(pDir->Size);

    RETURN (void *)GetDirectoryData(pDir);
}

CHECK PEDecoder::CheckResource(COUNT_T offset) const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckCorHeader());
    }
    CONTRACT_CHECK_END;

    IMAGE_DATA_DIRECTORY *pDir = &GetCorHeader()->Resources;

    CHECK(CheckOverflow(VAL32(pDir->VirtualAddress), offset));

    RVA rva = VAL32(pDir->VirtualAddress) + offset;

    // Make sure we have at least enough data for a length
    CHECK(CheckRva(rva, sizeof(DWORD)));

    // Make sure resource is within resource section
    CHECK(CheckBounds(VAL32(pDir->VirtualAddress), VAL32(pDir->Size),
                      rva + sizeof(DWORD), GET_UNALIGNED_VAL32((LPVOID)GetRvaData(rva))));

    CHECK_OK;
}

const void *PEDecoder::GetResource(COUNT_T offset, COUNT_T *pSize) const
{
    CONTRACT(const void *)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckCorHeader());
        PRECONDITION(CheckPointer(pSize, NULL_OK));
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_END;

    IMAGE_DATA_DIRECTORY *pDir = &GetCorHeader()->Resources;

    // 403571: Prefix complained correctly about need to always perform rva check
    if (CheckResource(offset) == FALSE)
        return NULL;

    void * resourceBlob = (void *)GetRvaData(VAL32(pDir->VirtualAddress) + offset);
    // Holds if CheckResource(offset) == TRUE
    PREFIX_ASSUME(resourceBlob != NULL);

     if (pSize != NULL)
        *pSize = GET_UNALIGNED_VAL32(resourceBlob);

    RETURN (const void *) ((BYTE*)resourceBlob+sizeof(DWORD));
}

BOOL PEDecoder::HasManagedEntryPoint() const
{
    CONTRACTL {
        INSTANCE_CHECK;
        PRECONDITION(CheckCorHeader());
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    ULONG flags = GetCorHeader()->Flags;
    return (!(flags & VAL32(COMIMAGE_FLAGS_NATIVE_ENTRYPOINT)) &&
            (!IsNilToken(GetEntryPointToken())));
}

ULONG PEDecoder::GetEntryPointToken() const
{
    CONTRACT(ULONG)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckCorHeader());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_END;

    RETURN VAL32(IMAGE_COR20_HEADER_FIELD(*GetCorHeader(), EntryPointToken));
}

IMAGE_COR_VTABLEFIXUP *PEDecoder::GetVTableFixups(COUNT_T *pCount) const
{
    CONTRACT(IMAGE_COR_VTABLEFIXUP *)
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckCorHeader());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_END;

    IMAGE_DATA_DIRECTORY *pDir = &GetCorHeader()->VTableFixups;

    if (pCount != NULL)
        *pCount = VAL32(pDir->Size)/sizeof(IMAGE_COR_VTABLEFIXUP);

    RETURN PTR_IMAGE_COR_VTABLEFIXUP(GetDirectoryData(pDir));
}

CHECK PEDecoder::CheckILOnly() const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_CHECK_END;

    if (m_flags & FLAG_IL_ONLY_CHECKED)
        CHECK_OK;

    CHECK(CheckCorHeader());

    // Allow only verifiable directories.

    static int s_allowedBitmap =
        ((1 << (IMAGE_DIRECTORY_ENTRY_IMPORT   )) |
         (1 << (IMAGE_DIRECTORY_ENTRY_RESOURCE )) |
         (1 << (IMAGE_DIRECTORY_ENTRY_SECURITY )) |
         (1 << (IMAGE_DIRECTORY_ENTRY_BASERELOC)) |
         (1 << (IMAGE_DIRECTORY_ENTRY_DEBUG    )) |
         (1 << (IMAGE_DIRECTORY_ENTRY_IAT      )) |
         (1 << (IMAGE_DIRECTORY_ENTRY_COMHEADER)));

    for (UINT32 entry=0; entry<GetNumberOfRvaAndSizes(); ++entry)
    {
        if (HasDirectoryEntry(entry))
        {
            CHECK((s_allowedBitmap & (1 << entry)) != 0);
            if (entry!=IMAGE_DIRECTORY_ENTRY_SECURITY)  //ignored by OS loader
                CHECK(CheckDirectoryEntry(entry,IMAGE_SCN_MEM_SHARED,NULL_NOT_OK));
        }
    }
    if (HasDirectoryEntry(IMAGE_DIRECTORY_ENTRY_IMPORT) ||
        HasDirectoryEntry(IMAGE_DIRECTORY_ENTRY_BASERELOC) ||
        FindNTHeaders()->OptionalHeader.AddressOfEntryPoint != 0)
    {
        CHECK(CheckILOnlyImportDlls());
        CHECK(CheckILOnlyBaseRelocations());
        CHECK(CheckILOnlyEntryPoint());
    }

    // Don't allow shared sections for IL-only images
    IMAGE_NT_HEADERS *pNT = FindNTHeaders();
    IMAGE_SECTION_HEADER *section = FindFirstSection(pNT);
    IMAGE_SECTION_HEADER *sectionEnd = section + VAL16(pNT->FileHeader.NumberOfSections);
    while (section < sectionEnd)
    {
        CHECK(!(section->Characteristics & IMAGE_SCN_MEM_SHARED));
        section++;
    }

    const_cast<PEDecoder *>(this)->m_flags |= FLAG_IL_ONLY_CHECKED;

    CHECK_OK;
}

CHECK PEDecoder::CheckILOnlyImportDlls() const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_CHECK_END;

    // The only allowed DLL Imports are MscorEE.dll:_CorExeMain,_CorDllMain

    // !!! NEED TO FIGURE OUT HOW TO DEAL WITH THIS
    // On win64, when the image is LoadLibrary'd, we whack the import and IAT directories
    // A similar thing happens on Win9x from the OS loader.
        
    // We need a real way to know whether this is a post-LoadLibrary image
    if (!IsMapped() || 
        !RunningOnWin95()
        )
    {
        CHECK(HasDirectoryEntry(IMAGE_DIRECTORY_ENTRY_IMPORT));
        CHECK(CheckDirectoryEntry(IMAGE_DIRECTORY_ENTRY_IMPORT, IMAGE_SCN_MEM_WRITE));

        // Get the import directory entry
        PIMAGE_DATA_DIRECTORY pDirEntryImport = GetDirectoryEntry(IMAGE_DIRECTORY_ENTRY_IMPORT);
        CHECK(pDirEntryImport != NULL);
        PREFIX_ASSUME(pDirEntryImport != NULL);

        // There should be space for 2 entries. (mscoree and NULL)
        CHECK(VAL32(pDirEntryImport->Size) >= (2 * sizeof(IMAGE_IMPORT_DESCRIPTOR)));

        // Get the import data
        PIMAGE_IMPORT_DESCRIPTOR pID = (PIMAGE_IMPORT_DESCRIPTOR) GetDirectoryData(pDirEntryImport);
        CHECK(pID != NULL);
        PREFIX_ASSUME(pID != NULL);
        
        // Entry 0: ILT, Name, IAT must be be non-null.  Forwarder, DateTime should be NULL.
        CHECK( IMAGE_IMPORT_DESC_FIELD(pID[0], Characteristics) != 0
            && pID[0].TimeDateStamp == 0
            && (pID[0].ForwarderChain == 0 || pID[0].ForwarderChain == static_cast<ULONG>(-1))
            && pID[0].Name != 0
            && pID[0].FirstThunk != 0);

        // Entry 1: must be all nulls.
        CHECK( IMAGE_IMPORT_DESC_FIELD(pID[1], Characteristics) == 0
            && pID[1].TimeDateStamp == 0
            && pID[1].ForwarderChain == 0
            && pID[1].Name == 0
            && pID[1].FirstThunk == 0);

        // Ensure the RVA of the name plus its length is valid for this image
        UINT nameRVA = VAL32(pID[0].Name);
        CHECK(CheckRva(nameRVA, (COUNT_T) sizeof("mscoree.dll")));

        // Make sure the name is equal to mscoree
        CHECK(SString::_stricmp( (char *)GetRvaData(nameRVA), "mscoree.dll") == 0);

        // Check the Hint/Name table.
        CHECK(CheckILOnlyImportByNameTable(VAL32(IMAGE_IMPORT_DESC_FIELD(pID[0], OriginalFirstThunk))));

        // The IAT needs to be checked only for size.
        CHECK(CheckRva(VAL32(pID[0].FirstThunk), 2*sizeof(UINT32)));
    }

    CHECK_OK;
}

CHECK PEDecoder::CheckILOnlyImportByNameTable(RVA rva) const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_CHECK_END;

    // Check if we have enough space to hold 2 DWORDS
    CHECK(CheckRva(rva, 2*sizeof(UINT32)));

    UINT32 UNALIGNED *pImportArray = (UINT32 UNALIGNED *) GetRvaData(rva);

    CHECK(GET_UNALIGNED_VAL32(&pImportArray[0]) != 0);
    CHECK(GET_UNALIGNED_VAL32(&pImportArray[1]) == 0);

    UINT32 importRVA = GET_UNALIGNED_VAL32(&pImportArray[0]);

    // First bit Set implies Ordinal lookup
    CHECK((importRVA & 0x80000000) == 0);

#define DLL_NAME "_CorDllMain"
#define EXE_NAME "_CorExeMain"

    COMPILE_TIME_ASSERT(sizeof(DLL_NAME) == sizeof(EXE_NAME));

    // Check if we have enough space to hold 4 bytes +
    // _CorExeMain or _CorDllMain and a NULL char
    CHECK(CheckRva(importRVA, 4 + sizeof(DLL_NAME)));

    IMAGE_IMPORT_BY_NAME *import = (IMAGE_IMPORT_BY_NAME*) GetRvaData(importRVA);

    CHECK(SString::_stricmp((char *) import->Name, DLL_NAME) == 0 || _stricmp((char *) import->Name, EXE_NAME) == 0);

    CHECK_OK;
}

#ifdef _X86_
// jmp dword ptr ds:[XXXX]
#define JMP_DWORD_PTR_DS_OPCODE { 0xFF, 0x25 }
#define JMP_DWORD_PTR_DS_OPCODE_SIZE   2        // Size of opcode
#define JMP_SIZE   6                            // Size of opcode + operand
#endif

CHECK PEDecoder::CheckILOnlyBaseRelocations() const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_CHECK_END;

    if (!HasDirectoryEntry(IMAGE_DIRECTORY_ENTRY_BASERELOC))
    {
        // We require base relocs for dlls.
        CHECK(!IsDll());

        CHECK((FindNTHeaders()->FileHeader.Characteristics & VAL16(IMAGE_FILE_RELOCS_STRIPPED)) != 0);
    }
    else
    {
        CHECK((FindNTHeaders()->FileHeader.Characteristics & VAL16(IMAGE_FILE_RELOCS_STRIPPED)) == 0);
        
        CHECK(CheckDirectoryEntry(IMAGE_DIRECTORY_ENTRY_BASERELOC, IMAGE_SCN_MEM_WRITE));

        IMAGE_DATA_DIRECTORY *pRelocDir = GetDirectoryEntry(IMAGE_DIRECTORY_ENTRY_BASERELOC);

        IMAGE_SECTION_HEADER *section = RvaToSection(VAL32(pRelocDir->VirtualAddress));
        CHECK(section != NULL);
        CHECK((section->Characteristics & VAL32(IMAGE_SCN_MEM_READ))!=0);

        IMAGE_BASE_RELOCATION *pReloc = (IMAGE_BASE_RELOCATION *)
          GetRvaData(VAL32(pRelocDir->VirtualAddress));

        // 403569: PREfix correctly complained about pReloc being possibly NULL
        CHECK(pReloc != NULL);
        CHECK(VAL32(pReloc->SizeOfBlock) == VAL32(pRelocDir->Size));

        UINT16 *pRelocEntry = (UINT16 *) (pReloc + 1);
        UINT16 *pRelocEntryEnd = (UINT16 *) ((BYTE *) pReloc + VAL32(pReloc->SizeOfBlock));
        if(FindNTHeaders()->FileHeader.Machine == VAL16(IMAGE_FILE_MACHINE_IA64))
        {
            // Exactly 2 Reloc records, both IMAGE_REL_BASED_DIR64
            CHECK(VAL32(pReloc->SizeOfBlock) >= (sizeof(IMAGE_BASE_RELOCATION)+2*sizeof(UINT16)));
            CHECK((VAL16(pRelocEntry[0]) & 0xF000) == (IMAGE_REL_BASED_DIR64 << 12));
            pRelocEntry++;
            CHECK((VAL16(pRelocEntry[0]) & 0xF000) == (IMAGE_REL_BASED_DIR64 << 12));
        }
        else
        {
            // Only one Reloc record is expected
            CHECK(VAL32(pReloc->SizeOfBlock) >= (sizeof(IMAGE_BASE_RELOCATION)+sizeof(UINT16)));
            if(FindNTHeaders()->FileHeader.Machine == VAL16(IMAGE_FILE_MACHINE_AMD64))
                CHECK((VAL16(pRelocEntry[0]) & 0xF000) == (IMAGE_REL_BASED_DIR64 << 12));
            else
                CHECK((VAL16(pRelocEntry[0]) & 0xF000) == (IMAGE_REL_BASED_HIGHLOW << 12));
        }

        while (++pRelocEntry < pRelocEntryEnd)
        {
            // NULL padding entries are allowed
            CHECK((VAL16(pRelocEntry[0]) & 0xF000) == IMAGE_REL_BASED_ABSOLUTE);
        }
    }

    CHECK_OK;
}

CHECK PEDecoder::CheckILOnlyEntryPoint() const
{
    CONTRACT_CHECK
    {
        INSTANCE_CHECK;
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_CHECK_END;

    CHECK(FindNTHeaders()->OptionalHeader.AddressOfEntryPoint != 0);

#ifdef _X86_    
    if(FindNTHeaders()->FileHeader.Machine == VAL16(IMAGE_FILE_MACHINE_I386))
    {
        // !!! NEED TO FIGURE OUT HOW TO DEAL WITH THIS
        // When the image is LoadLibrary'd, we whack the entry point to point to _CorDllMain directly
        // I'm not sure how to know whether it's been whacked yet or not though.
    
        if (!IsMapped())
        {
            // EntryPoint should be a jmp dword ptr ds:[XXXX] instruction.
            // XXXX should be RVA of the first and only entry in the IAT.
    
            CHECK(CheckRva(VAL32(FindNTHeaders()->OptionalHeader.AddressOfEntryPoint), JMP_SIZE));
    
            BYTE *stub = (BYTE *) GetRvaData(VAL32(FindNTHeaders()->OptionalHeader.AddressOfEntryPoint));
    
            static const BYTE s_DllOrExeMain[] = JMP_DWORD_PTR_DS_OPCODE;
    
            // 403570: prefix complained about stub being possibly NULL.
            // Unsure here. PREFIX_ASSUME might be also correct as indices are
            // verified in the above CHECK statement.
            CHECK(stub != NULL);
            CHECK(memcmp(stub, s_DllOrExeMain, JMP_DWORD_PTR_DS_OPCODE_SIZE) == 0);
    
            // Verify target of jump - it should be first entry in the IAT.
    
            PIMAGE_IMPORT_DESCRIPTOR pID =
              (PIMAGE_IMPORT_DESCRIPTOR) GetDirectoryEntryData(IMAGE_DIRECTORY_ENTRY_IMPORT);
    
            UINT32 va = * (UINT32 *) (stub + JMP_DWORD_PTR_DS_OPCODE_SIZE);
    
            CHECK(VAL32(pID[0].FirstThunk) == (va - (SIZE_T) GetPreferredBase()));
        }
    }
#endif

    CHECK_OK;
}

#ifndef DACCESS_COMPILE

void PEDecoder::LayoutILOnly(void *base, BOOL allowFullPE) const
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(allowFullPE || CheckILOnlyFormat());
        PRECONDITION(HasBaseRelocations() || !IsDll());
        PRECONDITION(CheckZeroedMemory(base, VAL32(FindNTHeaders()->OptionalHeader.SizeOfImage)));
        // Ideally we would require the layout address to honor the section alignment constraints.
        // However, we do have 8K aligned IL only images which we load on 32 bit platforms. In this
        // case, we can only guarantee OS page alignment (which after all, is good enough.)
        PRECONDITION(CheckAligned((SIZE_T)base, OS_PAGE_SIZE));
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACT_END;

    // We're going to copy everything first, and write protect what we need to later.

    // First, copy headers
    CopyMemory(base, (void *)m_base, VAL32(FindNTHeaders()->OptionalHeader.SizeOfHeaders));

    // Now, copy all sections to appropriate virtual address

    IMAGE_SECTION_HEADER *sectionStart = IMAGE_FIRST_SECTION(FindNTHeaders());
    IMAGE_SECTION_HEADER *sectionEnd = sectionStart + VAL16(FindNTHeaders()->FileHeader.NumberOfSections);

    IMAGE_SECTION_HEADER *section = sectionStart;
    while (section < sectionEnd)
    {
        // Raw data may be less than section size if tail is zero, but may be more since VirtualSize is
        // not padded.
        DWORD size = min(VAL32(section->SizeOfRawData), VAL32(section->Misc.VirtualSize));

        CopyMemory((BYTE *) base + VAL32(section->VirtualAddress), (BYTE *) m_base + VAL32(section->PointerToRawData), size);

        // Note that our memory is zeroed already, so no need to initialize any tail.

        section++;
    }

    // Apply write protection to copied headers
    DWORD oldProtection;
    if (!ClrVirtualProtect((void *) base, VAL32(FindNTHeaders()->OptionalHeader.SizeOfHeaders),
                           PAGE_READONLY, &oldProtection))
        ThrowLastError();

    // Finally, apply proper protection to copied sections
    section = sectionStart;
    while (section < sectionEnd)
    {
        // Add appropriate page protection.
        if ((section->Characteristics & VAL32(IMAGE_SCN_MEM_WRITE)) == 0)
        {
            if (!ClrVirtualProtect((void *) ((BYTE *)base + VAL32(section->VirtualAddress)),
                                   VAL32(section->Misc.VirtualSize),
                                   PAGE_READONLY, &oldProtection))
                ThrowLastError();
        }

        section++;
    }

    RETURN;
}

#endif // #ifndef DACCESS_COMPILE

void PEDecoder::ApplyILOnlyBaseRelocations(BOOL allowFullPE)
{
    CONTRACT_VOID
    {
        INSTANCE_CHECK;
        PRECONDITION(allowFullPE || CheckILOnlyFormat());
        PRECONDITION(IsDll());
        PRECONDITION(HasBaseRelocations());
        PRECONDITION(IsMapped());
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACT_END;

    // Note we are constrained to having a single relocation, since we are pure managed only

    SSIZE_T delta = (SIZE_T) m_base - (SIZE_T) GetPreferredBase();

    IMAGE_DATA_DIRECTORY *dir = GetDirectoryEntry(IMAGE_DIRECTORY_ENTRY_BASERELOC);

    IMAGE_BASE_RELOCATION *r = (IMAGE_BASE_RELOCATION *) GetDirectoryData(dir);

    // 403513: Prefix complained correctly.
    if (r == NULL)
        ThrowHR(COR_E_BADIMAGEFORMAT);
    PREFIX_ASSUME(r != NULL);

    USHORT *fixups = (USHORT *) (r+1);

    void * pageAddress = (void *)GetRvaData(VAL32(r->VirtualAddress));

    SIZE_T *address = (SIZE_T*)((SIZE_T) pageAddress + (VAL16(*fixups)&0xfff));

    DWORD oldProtection;
    if (!ClrVirtualProtect((void *) address, sizeof(void *), PAGE_READWRITE,
                           &oldProtection))
        ThrowLastError();

    *address += delta;

    if (!ClrVirtualProtect((void *) address, sizeof(void*), oldProtection,
                           &oldProtection))
        ThrowLastError();

    RETURN;
}


BOOL PEDecoder::HasNativeHeader() const
{
    CONTRACT(BOOL)
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_END;

    RETURN FALSE;
}


CHECK PEDecoder::CheckNativeHeader() const
{
    CONTRACT_CHECK
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_CHECK_END;

    CHECK(false);

    CHECK_OK;
}



CHECK PEDecoder::CheckWillCreateGuardPage() const
{
    CONTRACT_CHECK
    {
        PRECONDITION(CheckNTHeaders());
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_CHECK_END;


    CHECK_OK;
}

BOOL PEDecoder::HasNativeEntryPoint() const
{
    CONTRACTL {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckCorHeader());
    } CONTRACTL_END;

    ULONG flags = GetCorHeader()->Flags;
    return ((flags & VAL32(COMIMAGE_FLAGS_NATIVE_ENTRYPOINT)) &&
            (IMAGE_COR20_HEADER_FIELD(*GetCorHeader(), EntryPointRVA) != VAL32(0)));
}

void *PEDecoder::GetNativeEntryPoint() const
{
    CONTRACT (void *) {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckCorHeader());
        PRECONDITION(HasNativeEntryPoint());
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    } CONTRACT_END;

    RETURN ((void *) GetRvaData((RVA)VAL32(IMAGE_COR20_HEADER_FIELD(*GetCorHeader(), EntryPointRVA))));
}

#ifdef DACCESS_COMPILE

void
PEDecoder::EnumMemoryRegions(CLRDataEnumMemoryFlags flags,
                             bool enumThis)
{
    if (enumThis)
    {
        DAC_ENUM_DTHIS();
    }

    DacEnumMemoryRegion((TADDR)m_base, sizeof(IMAGE_DOS_HEADER));
    m_pNTHeaders.EnumMem();
    m_pCorHeader.EnumMem();
    m_pNativeHeader.EnumMem();

    if (HasNTHeaders())
    {
        // resource file does not have NT Header.
        //
        // we also need to write out section header.
        DacEnumMemoryRegion(PTR_HOST_TO_TADDR(FindFirstSection()), sizeof(IMAGE_SECTION_HEADER) * GetNumberOfSections());
    }
}

#endif // #ifdef DACCESS_COMPILE


HRESULT PEDecoder::ValidateFormat()
{
    if(!HasContents()) return PEFMT_E_NO_CONTENTS;
    if(!HasNTHeaders()) return PEFMT_E_NO_NTHEADERS;
    if(!Has32BitNTHeaders()) return PEFMT_E_64BIT;
    if(GetNTHeaders32()->OptionalHeader.SizeOfCode == 0) return PEFMT_E_ZERO_SIZEOFCODE;
    if(!HasCorHeader()) return PEFMT_E_NO_CORHEADER;
    if(!CheckCorHeader()) return PEFMT_E_BAD_CORHEADER;
    if(!IsILOnly()) return PEFMT_E_NOT_ILONLY;
    if(!CheckILOnlyImportDlls()) return PEFMT_E_IMPORT_DLLS;
    if(!CheckILOnlyBaseRelocations()) return PEFMT_E_BASE_RELOCS;
    if(HasManagedEntryPoint())
    {
        if(!CheckILOnlyEntryPoint()) return PEFMT_E_ENTRYPOINT;
    }
    else
    {
        if(!IsDll()) return PEFMT_E_EXE_NOENTRYPOINT;
    }

    return S_OK;
}


HRESULT TranslatePEToArchitectureTypeUtil(CorPEKind CLRPeKind, DWORD dwImageType, PEKIND *PeKind)
{
    HRESULT     hr = S_OK;

    _ASSERTE(PeKind);

    *PeKind = peNone;

    if(CLRPeKind == peNot) 
    {        
        // Not a PE. Shouldn't ever get here.
        hr = HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
        goto Exit;
    }
    else 
    {
        if ((CLRPeKind & peILonly) && !(CLRPeKind & pe32Plus) &&
            !(CLRPeKind & pe32BitRequired) && dwImageType == IMAGE_FILE_MACHINE_I386) 
        {
            // Processor-agnostic (MSIL)
            *PeKind = peMSIL;
        }
        else if (CLRPeKind & pe32Plus) 
        {
            // 64-bit
            
            if (CLRPeKind & pe32BitRequired) 
            {
                // Invalid
                hr = HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                goto Exit;
            }

            // Regardless of whether ILONLY is set or not, the architecture
            // is the machine type.

            if(dwImageType == IMAGE_FILE_MACHINE_IA64) 
                *PeKind = peIA64;
            else if(dwImageType == IMAGE_FILE_MACHINE_AMD64) 
                *PeKind = peAMD64;
            else 
            {
                // We don't support other architectures
                hr = HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                goto Exit;
            }
        }
        else 
        {
            // 32-bit, non-agnostic

            if (dwImageType != IMAGE_FILE_MACHINE_I386) 
            {
                // Not supported
                hr = HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
                goto Exit;
            }

            *PeKind = peI386;
        }
    }

Exit:
    return hr;
}

// --------------------------------------------------------------------------------

#ifdef _DEBUG

// This is a stress mode to force DLLs to be relocated.
// This is particularly useful for hardbinding of ngen images as we
// embed pointers into other hardbound ngen dependencies.

BOOL PEDecoder::GetForceRelocs()
{
    WRAPPER_CONTRACT;

    static ConfigDWORD forceRelocs;
    return (forceRelocs.val(L"ForceRelocs", 0) != 0);
}

BOOL PEDecoder::ForceRelocForDLL(LPCWSTR lpFileName)
{
    // Use static contracts to avoid recursion, as the dynamic contracts
    // do WszLoadLibrary(MSCOREE_SHIM_W).
    STATIC_CONTRACT_DEBUG_ONLY;

    return TRUE;
}

#endif // _DEBUG

