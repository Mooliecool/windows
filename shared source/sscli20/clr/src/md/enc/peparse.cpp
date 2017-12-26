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
#include "stdafx.h"

#include <windows.h>
#include <corhdr.h>
#include "corerror.h"
#include "pedecoder.h"


struct CORCOMPILE_HEADER;
struct CORCOMPILE_VERSION_INFO;

static const char g_szCORMETA[] = ".cormeta";


HRESULT CLiteWeightStgdbRW::FindImageMetaData(PVOID pImage, DWORD dwFileLength, BOOL bMappedImage, BOOL bGetManifestMetadata, PVOID *ppMetaData, ULONG *pcbMetaData)
{
#ifndef DACCESS_COMPILE
    HRESULT hr = S_OK;

    NewHolder<PEDecoder> pe;

    EX_TRY
    {
        // We need to use different PEDecoder constructors based on the type of data we give it.
        // We use the one with a 'bool' as the second argument when dealing with a mapped file,
        // and we use the one that takes a COUNT_T as the second argument when dealing with a
        // flat file.
        if (bMappedImage)
            pe = new (nothrow) PEDecoder(pImage, false);
        else
            pe = new (nothrow) PEDecoder(pImage, (COUNT_T)dwFileLength);
    }
    EX_CATCH
    {
        hr = COR_E_BADIMAGEFORMAT;
    }
    EX_END_CATCH(SwallowAllExceptions)

    IfFailRet(hr);
    IfNullRet(pe);

    // Minimally validate image
    if (!pe->CheckCorHeader())
        return COR_E_BADIMAGEFORMAT;


    COUNT_T size = 0;

    // Check to make sure that we're asking for manifest metadata on an ngen'ed image
    if (!pe->HasNativeHeader() && bGetManifestMetadata)
    {
        _ASSERTE(!"Calling OpenScope with ManifestMetadata flag on non-ngenned image");
        return CLDB_E_NO_DATA;
    
    }

    // See if we're being asked to open an ngen'ed image
    if (pe->HasNativeHeader() && !bGetManifestMetadata)
    {
        return CLDB_E_NO_DATA;

    }
    else // We're either looking at an IL assembly or the manifest metadata for an ngen'ed image.
    {
        *ppMetaData = (void *) pe->GetMetadata(&size);
    }

    // Couldn't find any IL metadata in this image
    if (*ppMetaData == NULL)
        return CLDB_E_NO_DATA;
        
    if (pcbMetaData != NULL)
        *pcbMetaData = size;
    
    return S_OK;
#else
    DacNotImpl();
    return E_NOTIMPL;
#endif
}

//
// Note: Remove once defined in winnt.h
//
typedef struct ANON_OBJECT_HEADER2 {
    WORD    Sig1;            // Must be IMAGE_FILE_MACHINE_UNKNOWN
    WORD    Sig2;            // Must be 0xffff
    WORD    Version;         // >= 2 (implies the CLSID field, Flags and metadata info are present) 
    WORD    Machine;
    DWORD   TimeDateStamp;
    CLSID   ClassID;         // Used to invoke CoCreateInstance
    DWORD   SizeOfData;      // Size of data that follows the header
    DWORD   Flags;
    DWORD   MetaDataSize;    // Size of CLR metadata
    DWORD   MetaDataOffset;  // Offset of CLR metadata
} ANON_OBJECT_HEADER2;

#define ANON_OBJECT_HAS_CORMETA 0x00000001
#define ANON_OBJECT_IS_PUREMSIL 0x00000002

HRESULT CLiteWeightStgdbRW::FindObjMetaData(PVOID pImage, DWORD dwFileLength, PVOID *ppMetaData, ULONG *pcbMetaData)
{
    DWORD   dwSize = 0;
    DWORD   dwOffset = 0;

    ANON_OBJECT_HEADER2 *pAnonImageHdr = (ANON_OBJECT_HEADER2 *) pImage;    // Anonymous object header

    // Check to see if this is a LTCG object
    if (dwFileLength >= sizeof(ANON_OBJECT_HEADER2) &&
         pAnonImageHdr->Sig1 == VAL16(IMAGE_FILE_MACHINE_UNKNOWN) &&
         pAnonImageHdr->Sig2 == VAL16(IMPORT_OBJECT_HDR_SIG2))
    {
        // Version 1 anonymous objects don't have metadata info
        if (VAL16(pAnonImageHdr->Version) < 2)
            goto BadFormat;

        // Anonymous objects contain the metadata info in the header
        dwOffset = VAL32(pAnonImageHdr->MetaDataOffset);
        dwSize = VAL32(pAnonImageHdr->MetaDataSize);
    }
    else
    {
        // Check to see if we have enough data
        if (dwFileLength < sizeof(IMAGE_FILE_HEADER))
            goto BadFormat;

        IMAGE_FILE_HEADER *pImageHdr = (IMAGE_FILE_HEADER *) pImage;            // Header for the .obj file.

        // Walk each section looking for .cormeta.
        DWORD nSections = VAL16(pImageHdr->NumberOfSections);

        // Check to see if we have enough data
        if (dwFileLength < sizeof(IMAGE_FILE_HEADER) + nSections * sizeof(IMAGE_SECTION_HEADER))
            goto BadFormat;

        IMAGE_SECTION_HEADER *pSectionHdr = (IMAGE_SECTION_HEADER *)(pImageHdr + 1);  // Section header.

        for (DWORD i=0; i<nSections;  i++, pSectionHdr++)
        {
            // Simple comparison to section name.
            if (memcmp((const char *) pSectionHdr->Name, g_szCORMETA, sizeof(g_szCORMETA)) == 0)
            {
                dwOffset = VAL32(pSectionHdr->PointerToRawData);
                dwSize = VAL32(pSectionHdr->SizeOfRawData);
                break;
            }
        }
    }

    if (dwOffset == 0 || dwSize == 0)
        goto BadFormat;

    // Check that raw data in the section is actually within the file.
    if (dwOffset >= dwFileLength || dwOffset + dwSize > dwFileLength)
        goto BadFormat;

    *ppMetaData = (PVOID) ((ULONG_PTR) pImage + dwOffset);
    *pcbMetaData = dwSize;
    return (S_OK);

BadFormat:
    *ppMetaData = NULL;
    *pcbMetaData = 0;
    return (COR_E_BADIMAGEFORMAT);
}
