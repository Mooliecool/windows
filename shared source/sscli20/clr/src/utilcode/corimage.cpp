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

/*============================================================
**
** CorImage.cpp
**
** IMAGEHLP routines so we can avoid early binding to that DLL.
**
===========================================================*/

#include "stdafx.h"
#include "contract.h"
#include <daccess.h>
#include "corimage.h"
#include "safemath.h"

#define RTL_MEG                   (1024UL * 1024UL)
#define RTLP_IMAGE_MAX_DOS_HEADER ( 256UL * RTL_MEG)

// IMAGE_FIRST_SECTION doesn't need 32/64 versions since the file header is
// the same either way.

#define PTR_IMAGE_FIRST_SECTION( ntheader )                            \
   PTR_IMAGE_SECTION_HEADER                                            \
    (PTR_TO_TADDR(ntheader) +                                          \
     FIELD_OFFSET( IMAGE_NT_HEADERS, OptionalHeader ) +                \
     VAL16((ntheader)->FileHeader.SizeOfOptionalHeader)                \
    )

#ifndef DACCESS_COMPILE

IMAGE_NT_HEADERS *Cor_RtlImageNtHeader(VOID *pvBase, ULONG FileLength)
{
    LEAF_CONTRACT;
    IMAGE_NT_HEADERS *pNtHeaders = NULL;
    if (pvBase && (pvBase != (VOID*)-1)) {
        IMAGE_DOS_HEADER *pDos = (IMAGE_DOS_HEADER*)pvBase;
        PAL_TRY {
            if (   (pDos->e_magic == VAL16(IMAGE_DOS_SIGNATURE)) 
                && ((DWORD)VAL32(pDos->e_lfanew) < RTLP_IMAGE_MAX_DOS_HEADER) 
                && ovadd_lt((DWORD)VAL32(pDos->e_lfanew), sizeof(IMAGE_FILE_HEADER) + sizeof(DWORD), FileLength)) {
                pNtHeaders = (IMAGE_NT_HEADERS*)((BYTE*)pvBase + VAL32(pDos->e_lfanew));
                if (pNtHeaders->Signature != VAL32(IMAGE_NT_SIGNATURE))
                    pNtHeaders = NULL;
            }
        }
        PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER) {
            pNtHeaders = NULL;
        }
        PAL_ENDTRY
    }
    return pNtHeaders;
}

#endif // #ifndef DACCESS_COMPILE

EXTERN_C PIMAGE_SECTION_HEADER
Cor_RtlImageRvaToSection32(PTR_IMAGE_NT_HEADERS32 NtHeaders,
                           ULONG Rva,
                           ULONG FileLength)
{
    LEAF_CONTRACT;
    ULONG i;
    PTR_IMAGE_SECTION_HEADER NtSection;

    NtSection = PTR_IMAGE_FIRST_SECTION( NtHeaders );
    for (i=0; i<NtHeaders->FileHeader.NumberOfSections; i++) {
        if (FileLength &&
            ((VAL32(NtSection->PointerToRawData) > FileLength)) ||
            (VAL32(NtSection->SizeOfRawData) > FileLength - VAL32(NtSection->PointerToRawData)))
            return NULL;
        if (Rva >= VAL32(NtSection->VirtualAddress) &&
            Rva < VAL32(NtSection->VirtualAddress) + VAL32(NtSection->SizeOfRawData))
            return NtSection;
        
        ++NtSection;
    }

    return NULL;
}

EXTERN_C PIMAGE_SECTION_HEADER
Cor_RtlImageRvaToSection64(PTR_IMAGE_NT_HEADERS64 NtHeaders,
                           ULONG Rva,
                           ULONG FileLength)
{
    LEAF_CONTRACT;
    ULONG i;
    PTR_IMAGE_SECTION_HEADER NtSection;

    NtSection = PTR_IMAGE_FIRST_SECTION( NtHeaders );
    for (i=0; i<VAL16(NtHeaders->FileHeader.NumberOfSections); i++) {
        if (FileLength &&
            ((VAL32(NtSection->PointerToRawData) > FileLength)) ||
            (VAL32(NtSection->SizeOfRawData) > FileLength - VAL32(NtSection->PointerToRawData)))
            return NULL;
        if (Rva >= VAL32(NtSection->VirtualAddress) &&
            Rva < VAL32(NtSection->VirtualAddress) + VAL32(NtSection->SizeOfRawData))
            return NtSection;
        
        ++NtSection;
    }

    return NULL;
}

EXTERN_C PIMAGE_SECTION_HEADER
Cor_RtlImageRvaToSection(PTR_IMAGE_NT_HEADERS NtHeaders,
                         ULONG Rva,
                         ULONG FileLength)
{
    LEAF_CONTRACT;
    if (NtHeaders->OptionalHeader.Magic == VAL16(IMAGE_NT_OPTIONAL_HDR32_MAGIC))
        return Cor_RtlImageRvaToSection32((PTR_IMAGE_NT_HEADERS32)NtHeaders,
                                          Rva, FileLength);
    else if(NtHeaders->OptionalHeader.Magic == VAL16(IMAGE_NT_OPTIONAL_HDR64_MAGIC))
        return Cor_RtlImageRvaToSection64((PTR_IMAGE_NT_HEADERS64)NtHeaders,
                                          Rva, FileLength);
    else {
        _ASSERTE(!"Invalid File Type");
        return NULL;
    }
}

EXTERN_C PBYTE Cor_RtlImageRvaToVa32(PTR_IMAGE_NT_HEADERS32 NtHeaders,
                                     PBYTE Base,
                                     ULONG Rva,
                                     ULONG FileLength)
{
    LEAF_CONTRACT;
    PIMAGE_SECTION_HEADER NtSection =
        Cor_RtlImageRvaToSection32(NtHeaders,
                                   Rva,
                                   FileLength);

    if (NtSection != NULL)
        return (Base +
                (Rva - VAL32(NtSection->VirtualAddress)) +
                VAL32(NtSection->PointerToRawData));
    else
        return NULL;
}

EXTERN_C PBYTE Cor_RtlImageRvaToVa64(PTR_IMAGE_NT_HEADERS64 NtHeaders,
                                     PBYTE Base,
                                     ULONG Rva,
                                     ULONG FileLength)
{
    LEAF_CONTRACT;
    PIMAGE_SECTION_HEADER NtSection =
        Cor_RtlImageRvaToSection64(NtHeaders,
                                   Rva,
                                   FileLength);

    if (NtSection != NULL)
        return (Base +
                (Rva - VAL32(NtSection->VirtualAddress)) +
                VAL32(NtSection->PointerToRawData));
    else
        return NULL;
}

EXTERN_C PBYTE Cor_RtlImageRvaToVa(PTR_IMAGE_NT_HEADERS NtHeaders,
                                   PBYTE Base,
                                   ULONG Rva,
                                   ULONG FileLength)
{
    LEAF_CONTRACT;
    if (NtHeaders->OptionalHeader.Magic == VAL16(IMAGE_NT_OPTIONAL_HDR32_MAGIC))
        return Cor_RtlImageRvaToVa32((PTR_IMAGE_NT_HEADERS32)NtHeaders,
                                     Base, Rva, FileLength);
    else if(NtHeaders->OptionalHeader.Magic == VAL16(IMAGE_NT_OPTIONAL_HDR64_MAGIC))
        return Cor_RtlImageRvaToVa64((PTR_IMAGE_NT_HEADERS64)NtHeaders,
                                     Base, Rva, FileLength);
    else {
        _ASSERTE(!"Invalid File Type");
        return NULL;
    }
}

EXTERN_C PBYTE Cor_RtlImageDirToVa(PTR_IMAGE_NT_HEADERS NtHeaders,
                                   PBYTE Base,
                                   UINT  DirIndex,
                                   ULONG FileLength)
{
    LEAF_CONTRACT;
    if (NtHeaders->OptionalHeader.Magic == VAL16(IMAGE_NT_OPTIONAL_HDR32_MAGIC))
        return Cor_RtlImageRvaToVa32((PTR_IMAGE_NT_HEADERS32)NtHeaders, Base, 
                                     VAL32(((PTR_IMAGE_NT_HEADERS32)NtHeaders)->OptionalHeader.DataDirectory[DirIndex].VirtualAddress), 
                                     FileLength);
    else if(NtHeaders->OptionalHeader.Magic == VAL16(IMAGE_NT_OPTIONAL_HDR64_MAGIC))
        return Cor_RtlImageRvaToVa64((PTR_IMAGE_NT_HEADERS64)NtHeaders, Base, 
                                     VAL32(((PTR_IMAGE_NT_HEADERS64)NtHeaders)->OptionalHeader.DataDirectory[DirIndex].VirtualAddress), 
                                     FileLength);
    else {
        _ASSERTE(!"Invalid File Type");
        return NULL;
    }
}

EXTERN_C PIMAGE_SECTION_HEADER
Cor_RtlImageRvaRangeToSection(PTR_IMAGE_NT_HEADERS NtHeaders,
                              ULONG Rva,
                              ULONG Range,
                              ULONG FileLength)
{
    LEAF_CONTRACT;
    ULONG i;
    PTR_IMAGE_SECTION_HEADER NtSection;

    if (!Range)
        return Cor_RtlImageRvaToSection(NtHeaders, Rva, FileLength);

    NtSection = PTR_IMAGE_FIRST_SECTION( NtHeaders );
    for (i=0; i<VAL16(NtHeaders->FileHeader.NumberOfSections); i++) {
        if (FileLength &&
            ((VAL32(NtSection->PointerToRawData) > FileLength) ||
             (VAL32(NtSection->SizeOfRawData) > FileLength - VAL32(NtSection->PointerToRawData))))
            return NULL;
        if (Rva >= VAL32(NtSection->VirtualAddress) &&
            Rva + Range <= VAL32(NtSection->VirtualAddress) + VAL32(NtSection->SizeOfRawData))
            return NtSection;
        
        ++NtSection;
    }

    return NULL;
}

EXTERN_C DWORD Cor_RtlImageRvaToOffset(PTR_IMAGE_NT_HEADERS NtHeaders,
                                       ULONG Rva,
                                       ULONG FileLength)
{
    LEAF_CONTRACT;
    PIMAGE_SECTION_HEADER NtSection =
        Cor_RtlImageRvaToSection(NtHeaders,
                                 Rva,
                                 FileLength);

    if (NtSection)
        return ((Rva - VAL32(NtSection->VirtualAddress)) +
                VAL32(NtSection->PointerToRawData));
    else
        return NULL;
}
