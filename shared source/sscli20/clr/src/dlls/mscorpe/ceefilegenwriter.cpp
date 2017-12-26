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
// Derived class from CCeeGen which handles writing out
// the exe. All references to PEWriter pulled out of CCeeGen,
// and moved here
//
//

#include "stdafx.h"

#include <string.h>
#include <limits.h>

#include "corerror.h"
#include "stubs.h"
#include <posterror.h>


// Get the Symbol entry given the head and a 0-based index
inline IMAGE_SYMBOL* GetSymbolEntry(IMAGE_SYMBOL* pHead, SIZE_T idx)
{
    return (IMAGE_SYMBOL*) (((BYTE*) pHead) + IMAGE_SIZEOF_SYMBOL * idx);
}



//*****************************************************************************
// To get a new instance, call CreateNewInstance() or CreateNewInstanceEx() instead of new
//*****************************************************************************

HRESULT CeeFileGenWriter::CreateNewInstance(CCeeGen *pCeeFileGenFrom,
                                            CeeFileGenWriter* & pGenWriter,
                                            DWORD createFlags)
{
    return CreateNewInstanceEx(pCeeFileGenFrom, pGenWriter, createFlags);
}

//
// Seed file is used as the base file. The new file data will be "appended" to the seed file
//

HRESULT CeeFileGenWriter::CreateNewInstanceEx(CCeeGen *pCeeFileGenFrom,
                                              CeeFileGenWriter* & pGenWriter,
                                              DWORD createFlags,
                                              LPCWSTR seedFileName)
{
    HRESULT hr = S_OK;
    ULONG preallocatedOffset = 0;
    NewHolder<PEWriter> pPEWriter(NULL);
    NewHolder<CeeFileGenWriter> pPrivateGenWriter;
    CeeSection *corHeaderSection = NULL;

    pPrivateGenWriter = new (nothrow) CeeFileGenWriter;
    if (pPrivateGenWriter == NULL)
        IfFailGo(hr = E_OUTOFMEMORY);

    pPEWriter = new (nothrow) PEWriter;
    if (pPEWriter == NULL)
        IfFailGo(hr = E_OUTOFMEMORY);

    hr = pPEWriter->Init(NULL, createFlags, seedFileName);
    IfFailGo(hr);

    //Create the general PEWriter.
    pPrivateGenWriter->m_peSectionMan = pPEWriter;
    hr = pPrivateGenWriter->Init(); // base class member to finish init
    IfFailGo(hr);

    if (!seedFileName) // Use base file's preferred base (if present)
    {
        if (pPEWriter->isPE32())
        {
            pPrivateGenWriter->setImageBase((DWORD) CEE_IMAGE_BASE);   // use same default as linker
        }
        else
        {
            pPrivateGenWriter->setImageBase64((ULONGLONG) CEE_IMAGE_BASE); // use same default as linker
        }
    }

    pPrivateGenWriter->setSubsystem(IMAGE_SUBSYSTEM_WINDOWS_CUI, CEE_IMAGE_SUBSYSTEM_MAJOR_VERSION, CEE_IMAGE_SUBSYSTEM_MINOR_VERSION);

    if (pPEWriter->createCorMainStub())
    {
        hr = pPrivateGenWriter->allocateIAT(); // so the IAT goes out first
        IfFailGo(hr);
    }

    hr = pPrivateGenWriter->allocateCorHeader();   // get COR header near front
    IfFailGo(hr);


    //If we were passed a CCeeGen at the beginning, copy it's data now.
    if (pCeeFileGenFrom) {
        pCeeFileGenFrom->cloneInstance((CCeeGen*)pPrivateGenWriter);
    }
   
    hr = pPrivateGenWriter->getSectionCreate(".text0", sdExecute, &corHeaderSection);
    IfFailGo(hr);
    preallocatedOffset = corHeaderSection->dataLen();
    

    // set il RVA to be after the preallocated sections
    pPEWriter->setIlRva(preallocatedOffset);


    pPEWriter.SuppressRelease();
    pPrivateGenWriter.SuppressRelease();
    pGenWriter = pPrivateGenWriter;

ErrExit:
    return hr;
} // HRESULT CeeFileGenWriter::CreateNewInstance()

CeeFileGenWriter::CeeFileGenWriter() // ctor is protected
{
    m_outputFileName = NULL;
    m_resourceFileName = NULL;
    m_dllSwitch = false;
    m_objSwitch = false;
    m_libraryName = NULL;
    m_libraryGuid = GUID_NULL;

    m_entryPoint = 0;
    m_comImageFlags = COMIMAGE_FLAGS_ILONLY;    // ceegen PEs don't have native code
    m_iatOffset = 0;
    m_dllCount = 0;

    m_dwMacroDefinitionSize = 0;
    m_dwMacroDefinitionRVA = NULL;

    m_dwManifestSize = 0;
    m_dwManifestRVA = NULL;

    m_dwStrongNameSize = 0;
    m_dwStrongNameRVA = NULL;

    m_dwVTableSize = 0;
    m_dwVTableRVA = NULL;

    m_iDataDlls = NULL;

    m_linked = false;
    m_fixed = false;



} // CeeFileGenWriter::CeeFileGenWriter()

//*****************************************************************************
// Cleanup
//*****************************************************************************
HRESULT CeeFileGenWriter::Cleanup() // virtual
{
    ((PEWriter *)m_peSectionMan)->Cleanup();  // call derived cleanup
    delete m_peSectionMan;
    m_peSectionMan = NULL; // so base class won't delete

    delete[] m_outputFileName;
    delete[] m_resourceFileName;

    if (m_iDataDlls) {
        for (int i=0; i < m_dllCount; i++) {
            if (m_iDataDlls[i].m_methodName)
                delete[] m_iDataDlls[i].m_methodName;
        }
        delete[] m_iDataDlls;
    }

    return CCeeGen::Cleanup();
} // HRESULT CeeFileGenWriter::Cleanup()

HRESULT CeeFileGenWriter::EmitMacroDefinitions(void *pData, DWORD cData)
{
    // OBSOLETE
    m_dwMacroDefinitionSize = 0;

    return S_OK;
} // HRESULT CeeFileGenWriter::EmitMacroDefinitions()

HRESULT CeeFileGenWriter::link()
{
    HRESULT hr = checkForErrors();
    if (! SUCCEEDED(hr))
        return hr;


    // Don't set this if SetManifestEntry was not called - zapper sets the
    // resource directory explicitly
    if (m_dwManifestSize != 0)
    {
        m_corHeader->Resources.VirtualAddress = VAL32(m_dwManifestRVA);
        m_corHeader->Resources.Size = VAL32(m_dwManifestSize);
    }

    if (m_dwStrongNameSize != 0)
    {
        m_corHeader->StrongNameSignature.VirtualAddress = VAL32(m_dwStrongNameRVA);
        m_corHeader->StrongNameSignature.Size = VAL32(m_dwStrongNameSize);
    }

    if (m_dwVTableSize != 0)
    {
        m_corHeader->VTableFixups.VirtualAddress = VAL32(m_dwVTableRVA);
        m_corHeader->VTableFixups.Size = VAL32(m_dwVTableSize);
    }

    unsigned characteristicsMask = IMAGE_FILE_EXECUTABLE_IMAGE    |
                                   IMAGE_FILE_LINE_NUMS_STRIPPED  |
                                   IMAGE_FILE_LOCAL_SYMS_STRIPPED |
                                   IMAGE_FILE_32BIT_MACHINE;
    if (!getPEWriter().isPE32())
        characteristicsMask |= IMAGE_FILE_LARGE_ADDRESS_AWARE;

    getPEWriter().setCharacteristics(characteristicsMask);

    m_corHeader->cb = VAL32(sizeof(IMAGE_COR20_HEADER));
    m_corHeader->MajorRuntimeVersion = VAL16(COR_VERSION_MAJOR);
    m_corHeader->MinorRuntimeVersion = VAL16(COR_VERSION_MINOR);
    if (m_dllSwitch)
        getPEWriter().setCharacteristics(IMAGE_FILE_DLL);
    if (m_objSwitch)
        getPEWriter().clearCharacteristics(IMAGE_FILE_DLL | IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_LOCAL_SYMS_STRIPPED);
    m_corHeader->Flags = VAL32(m_comImageFlags);
    IMAGE_COR20_HEADER_FIELD(*m_corHeader, EntryPointToken) = VAL32(m_entryPoint);
    _ASSERTE(TypeFromToken(m_entryPoint) == mdtMethodDef || m_entryPoint == mdTokenNil ||
             TypeFromToken(m_entryPoint) == mdtFile);
    setDirectoryEntry(getCorHeaderSection(), IMAGE_DIRECTORY_ENTRY_COMHEADER, sizeof(IMAGE_COR20_HEADER), m_corHeaderOffset);

    if ((m_comImageFlags & COMIMAGE_FLAGS_IL_LIBRARY) == 0
        && !m_linked && !m_objSwitch)
    {
        hr = emitExeMain();
        if (FAILED(hr))
            return hr;

    }

    m_linked = true;

    IfFailRet(getPEWriter().link());

    return S_OK;
} // HRESULT CeeFileGenWriter::link()


HRESULT CeeFileGenWriter::fixup()
{
    HRESULT hr;

    m_fixed = true;

    if (!m_linked)
        IfFailRet(link());

    CeeGenTokenMapper *pMapper = getTokenMapper();

    // Apply token remaps if there are any.
    if (! m_fTokenMapSupported && pMapper != NULL) {
        IMetaDataImport *pImport;
        hr = pMapper->GetMetaData(&pImport);
        _ASSERTE(SUCCEEDED(hr));
        hr = MapTokens(pMapper, pImport);
        pImport->Release();

    }

    // remap the entry point if entry point token has been moved
    if (pMapper != NULL && !m_objSwitch)
    {
        mdToken tk = m_entryPoint;
        pMapper->HasTokenMoved(tk, tk);
        IMAGE_COR20_HEADER_FIELD(*m_corHeader, EntryPointToken) = VAL32(tk);
    }

    IfFailRet(getPEWriter().fixup(pMapper));

    return S_OK;
} // HRESULT CeeFileGenWriter::fixup()

HRESULT CeeFileGenWriter::generateImage(void **ppImage)
{
    HRESULT hr = S_OK;
    LPWSTR outputFileName = NULL;


        if (!m_fixed)
            IfFailGo(fixup());

    outputFileName = m_outputFileName;

    if (! outputFileName && ppImage == NULL) {
        if (m_comImageFlags & COMIMAGE_FLAGS_IL_LIBRARY)
            outputFileName = L"output.ill";
        else if (m_dllSwitch)
            outputFileName = L"output.dll";
        else if (m_objSwitch)
            outputFileName = L"output.obj";
        else
            outputFileName = L"output.exe";
    }

    // output file name and ppImage are mutually exclusive
    _ASSERTE((NULL == outputFileName && ppImage != NULL) || (outputFileName != NULL && NULL == ppImage));

    if (outputFileName != NULL)
        IfFailGo(getPEWriter().write(outputFileName));
    else
        IfFailGo(getPEWriter().write(ppImage));

ErrExit:
    return hr;
} // HRESULT CeeFileGenWriter::generateImage()

HRESULT CeeFileGenWriter::setOutputFileName(__in LPWSTR fileName)
{
    if (m_outputFileName)
        delete[] m_outputFileName;
    int len = lstrlenW(fileName) + 1;
    m_outputFileName = (LPWSTR)new (nothrow) WCHAR[len];
    TESTANDRETURN(m_outputFileName!=NULL, E_OUTOFMEMORY);
    wcscpy_s(m_outputFileName, len, fileName);
    return S_OK;
} // HRESULT CeeFileGenWriter::setOutputFileName()

HRESULT CeeFileGenWriter::setResourceFileName(__in LPWSTR fileName)
{
    if (m_resourceFileName)
        delete[] m_resourceFileName;
    int len = lstrlenW(fileName) + 1;
    m_resourceFileName = (LPWSTR)new (nothrow) WCHAR[len];
    TESTANDRETURN(m_resourceFileName!=NULL, E_OUTOFMEMORY);
    wcscpy_s(m_resourceFileName, len, fileName);
    return S_OK;
} // HRESULT CeeFileGenWriter::setResourceFileName()

HRESULT CeeFileGenWriter::setLibraryName(__in LPWSTR libraryName)
{
    if (m_libraryName)
        delete[] m_libraryName;
    int len = lstrlenW(libraryName) + 1;
    m_libraryName = (LPWSTR)new (nothrow) WCHAR[len];
    TESTANDRETURN(m_libraryName != NULL, E_OUTOFMEMORY);
    wcscpy_s(m_libraryName, len, libraryName);
    return S_OK;
} // HRESULT CeeFileGenWriter::setLibraryName()

HRESULT CeeFileGenWriter::setLibraryGuid(__in LPWSTR libraryGuid)
{
    return IIDFromString(libraryGuid, &m_libraryGuid);
} // HRESULT CeeFileGenWriter::setLibraryGuid()


HRESULT CeeFileGenWriter::emitLibraryName(IMetaDataEmit *emitter)
{
    HRESULT hr;
    IfFailRet(emitter->SetModuleProps(m_libraryName));

    // Set the GUID as a custom attribute, if it is not NULL_GUID.
    if (m_libraryGuid != GUID_NULL)
    {
        static COR_SIGNATURE _SIG[] = INTEROP_GUID_SIG;
        mdTypeRef tr;
        mdMemberRef mr;
        WCHAR wzGuid[40];
        BYTE  rgCA[50];
        IfFailRet(emitter->DefineTypeRefByName(mdTypeRefNil, INTEROP_GUID_TYPE_W, &tr));
        IfFailRet(emitter->DefineMemberRef(tr, L".ctor", _SIG, sizeof(_SIG), &mr));
        StringFromGUID2(m_libraryGuid, wzGuid, lengthof(wzGuid));
        memset(rgCA, 0, sizeof(rgCA));
        // Tag is 0x0001
        rgCA[0] = 1;
        // Length of GUID string is 36 characters.
        rgCA[2] = 0x24;
        // Convert 36 characters, skipping opening {, into 3rd byte of buffer.
        WszWideCharToMultiByte(CP_ACP,0, wzGuid+1,36, reinterpret_cast<char*>(&rgCA[3]),36, 0,0);
        hr = emitter->DefineCustomAttribute(1,mr,rgCA,41,0);
    }
    return (hr);
} // HRESULT CeeFileGenWriter::emitLibraryName()

HRESULT CeeFileGenWriter::setImageBase(size_t imageBase)
{
    _ASSERTE(getPEWriter().isPE32());
        getPEWriter().setImageBase32((DWORD)imageBase);
    return S_OK;
} // HRESULT CeeFileGenWriter::setImageBase()

HRESULT CeeFileGenWriter::setImageBase64(ULONGLONG imageBase)
{
    _ASSERTE(!getPEWriter().isPE32());
    getPEWriter().setImageBase64(imageBase);
    return S_OK;
} // HRESULT CeeFileGenWriter::setImageBase64()

HRESULT CeeFileGenWriter::setFileAlignment(ULONG fileAlignment)
{
    getPEWriter().setFileAlignment(fileAlignment);
    return S_OK;
} // HRESULT CeeFileGenWriter::setFileAlignment()

HRESULT CeeFileGenWriter::setSubsystem(DWORD subsystem, DWORD major, DWORD minor)
{
    getPEWriter().setSubsystem(subsystem, major, minor);
    return S_OK;
} // HRESULT CeeFileGenWriter::setSubsystem()

HRESULT CeeFileGenWriter::checkForErrors()
{
    if (TypeFromToken(m_entryPoint) == mdtMethodDef) {
        if (m_dllSwitch) {
//          }
        }
        return S_OK;
    }
    return S_OK;
} // HRESULT CeeFileGenWriter::checkForErrors()

HRESULT CeeFileGenWriter::getMethodRVA(ULONG codeOffset, ULONG *codeRVA)
{
    _ASSERTE(codeRVA);
        *codeRVA = getPEWriter().getIlRva() + codeOffset;
    return S_OK;
} // HRESULT CeeFileGenWriter::getMethodRVA()

HRESULT CeeFileGenWriter::setDirectoryEntry(CeeSection &section, ULONG entry, ULONG size, ULONG offset)
{
    return getPEWriter().setDirectoryEntry((PEWriterSection*)(&section.getImpl()), entry, size, offset);
} // HRESULT CeeFileGenWriter::setDirectoryEntry()

HRESULT CeeFileGenWriter::getFileTimeStamp(DWORD *pTimeStamp)
{
    return getPEWriter().getFileTimeStamp(pTimeStamp);
} // HRESULT CeeFileGenWriter::getFileTimeStamp()

HRESULT CeeFileGenWriter::setAddrReloc(UCHAR *instrAddr, DWORD value)
{
    *(DWORD *)instrAddr = VAL32(value);
    return S_OK;
} // HRESULT CeeFileGenWriter::setAddrReloc()

HRESULT CeeFileGenWriter::addAddrReloc(CeeSection &thisSection, UCHAR *instrAddr, DWORD offset, CeeSection *targetSection)
{
    if (!targetSection) {
        thisSection.addBaseReloc(offset, srRelocHighLow);
    } else {
        thisSection.addSectReloc(offset, *targetSection, srRelocHighLow);
    }
    return S_OK;
}

//
//
//

HRESULT CeeFileGenWriter::allocateIAT()
{
    m_dllCount = 1;
    m_iDataDlls = new (nothrow) IDataDllInfo[m_dllCount];
    if (m_iDataDlls == NULL) {
        return E_OUTOFMEMORY;
    }
    memset(m_iDataDlls, '\0', m_dllCount * sizeof(IDataDllInfo));
    m_iDataDlls[0].m_name = "mscoree.dll";
    m_iDataDlls[0].m_numMethods = 1;
    m_iDataDlls[0].m_methodName = new (nothrow) char*[m_iDataDlls[0].m_numMethods];
    if (! m_iDataDlls[0].m_methodName) {
        return E_OUTOFMEMORY;
    }
    m_iDataDlls[0].m_methodName[0] = NULL;

    int iDataSizeIAT = 0;

    for (int i=0; i < m_dllCount; i++) {
        m_iDataDlls[i].m_iatOffset = iDataSizeIAT;
        iDataSizeIAT += (m_iDataDlls[i].m_numMethods + 1)
                      * (getPEWriter().isPE32() ? sizeof(IMAGE_THUNK_DATA32)
                                                : sizeof(IMAGE_THUNK_DATA64));
    }

    HRESULT hr = getSectionCreate(".text0", sdExecute, &m_iDataSectionIAT);
    TESTANDRETURNHR(hr);
    m_iDataOffsetIAT = m_iDataSectionIAT->dataLen();
    _ASSERTE(m_iDataOffsetIAT == 0);
    m_iDataIAT = m_iDataSectionIAT->getBlock(iDataSizeIAT);
    if (! m_iDataIAT) {
        return E_OUTOFMEMORY;
    }
    memset(m_iDataIAT, '\0', iDataSizeIAT);

    // Don't set the IAT directory entry yet, since we may not actually end up doing
    // an emitExeMain.

    return S_OK;
} // HRESULT CeeFileGenWriter::allocateIAT()

HRESULT CeeFileGenWriter::emitExeMain()
{
    if (m_dllCount == 0)
        return S_OK;

    // Note: code later on in this method assumes that mscoree.dll is at
    // index m_iDataDlls[0], with CorDllMain or CorExeMain at method[0]

    _ASSERTE(getPEWriter().createCorMainStub());

    if (m_dllSwitch) {
        m_iDataDlls[0].m_methodName[0] = "_CorDllMain";
    } else {
        m_iDataDlls[0].m_methodName[0] = "_CorExeMain";
    }

    // IMAGE_IMPORT_DESCRIPTOR on PE/PE+ must be 4-byte or 8-byte aligned
    int align     = (getPEWriter().isPE32()) ? 4 : 8;
    int curOffset = getTextSection().dataLen();

    int diff = ((curOffset + align -1) & ~(align-1)) - curOffset;
    if (diff)
    {
        char* pDiff = getTextSection().getBlock(diff);
        if (NULL==pDiff) return E_OUTOFMEMORY;
        memset(pDiff,0,diff);
    }

    int iDataSizeRO = (m_dllCount + 1) * sizeof(IMAGE_IMPORT_DESCRIPTOR);
    CeeSection &iDataSectionRO = getTextSection();
    int iDataOffsetRO = iDataSectionRO.dataLen();
    int iDataSizeIAT = 0;
    int i;
    for (i=0; i < m_dllCount; i++) {
        m_iDataDlls[i].m_iltOffset = iDataSizeRO + iDataSizeIAT;
        iDataSizeIAT += (m_iDataDlls[i].m_numMethods + 1)
                      * (getPEWriter().isPE32() ? sizeof(IMAGE_THUNK_DATA32)
                                                : sizeof(IMAGE_THUNK_DATA64));
    }

    iDataSizeRO += iDataSizeIAT;

    for (i=0; i < m_dllCount; i++) {
        int delta = (iDataSizeRO + iDataOffsetRO) % 16;
        // make sure is on a 16-byte offset
        if (delta != 0)
            iDataSizeRO += (16 - delta);
        _ASSERTE((iDataSizeRO + iDataOffsetRO) % 16 == 0);
        m_iDataDlls[i].m_ibnOffset = iDataSizeRO;
        for (int j=0; j < m_iDataDlls[i].m_numMethods; j++) {
            int nameLen = (int)(strlen(m_iDataDlls[i].m_methodName[j]) + 1);
            iDataSizeRO += sizeof(WORD) + nameLen + nameLen%2;
        }
    }
    for (i=0; i < m_dllCount; i++) {
        m_iDataDlls[i].m_nameOffset = iDataSizeRO;
        iDataSizeRO += (int)(strlen(m_iDataDlls[i].m_name) + 2);
    }

    char *iDataRO = iDataSectionRO.getBlock(iDataSizeRO);

    if (!iDataRO) return E_OUTOFMEMORY;

    memset(iDataRO, '\0', iDataSizeRO);

    setDirectoryEntry(iDataSectionRO, IMAGE_DIRECTORY_ENTRY_IMPORT, iDataSizeRO, iDataOffsetRO);

    IMAGE_IMPORT_DESCRIPTOR *iid = (IMAGE_IMPORT_DESCRIPTOR *)iDataRO;
    for (i=0; i < m_dllCount; i++) {

        // fill in the import descriptors for each DLL
        IMAGE_IMPORT_DESC_FIELD(iid[i], OriginalFirstThunk) = VAL32((ULONG)(m_iDataDlls[i].m_iltOffset + iDataOffsetRO));
        iid[i].Name = VAL32(m_iDataDlls[i].m_nameOffset + iDataOffsetRO);
        iid[i].FirstThunk = VAL32((ULONG)(m_iDataDlls[i].m_iatOffset + m_iDataOffsetIAT));

        iDataSectionRO.addSectReloc(
            (unsigned)(iDataOffsetRO + (char *)(&IMAGE_IMPORT_DESC_FIELD(iid[i], OriginalFirstThunk)) - iDataRO), iDataSectionRO, srRelocAbsolute);
        iDataSectionRO.addSectReloc(
            (unsigned)(iDataOffsetRO + (char *)(&iid[i].Name) - iDataRO), iDataSectionRO, srRelocAbsolute);
        iDataSectionRO.addSectReloc(
            (unsigned)(iDataOffsetRO + (char *)(&iid[i].FirstThunk) - iDataRO), *m_iDataSectionIAT, srRelocAbsolute);

        if (getPEWriter().isPE32())
        {
        // now fill in the import lookup table for each DLL
            IMAGE_THUNK_DATA32 *ilt = (IMAGE_THUNK_DATA32*) (iDataRO + m_iDataDlls[i].m_iltOffset);
            IMAGE_THUNK_DATA32 *iat = (IMAGE_THUNK_DATA32*) (m_iDataIAT + m_iDataDlls[i].m_iatOffset);

            int ibnOffset = m_iDataDlls[i].m_ibnOffset;
            for (int j=0; j < m_iDataDlls[i].m_numMethods; j++)
            {
                ilt[j].u1.AddressOfData = VAL32((ULONG)(ibnOffset + iDataOffsetRO));
                iat[j].u1.AddressOfData = VAL32((ULONG)(ibnOffset + iDataOffsetRO));
    
                iDataSectionRO.addSectReloc( (unsigned)(iDataOffsetRO + (char *)(&ilt[j].u1.AddressOfData) - iDataRO),
                                                iDataSectionRO, srRelocAbsolute);
                m_iDataSectionIAT->addSectReloc( (unsigned)(m_iDataOffsetIAT + (char *)(&iat[j].u1.AddressOfData) - m_iDataIAT),
                                                    iDataSectionRO, srRelocAbsolute);
                int nameLen = (int)(strlen(m_iDataDlls[i].m_methodName[j]) + 1);
                memcpy(iDataRO + ibnOffset + offsetof(IMAGE_IMPORT_BY_NAME, Name),
                                        m_iDataDlls[i].m_methodName[j], nameLen);
                ibnOffset += sizeof(WORD) + nameLen + nameLen%2;
            }
        }
        else
        {
            // now fill in the import lookup table for each DLL
            IMAGE_THUNK_DATA64 *ilt = (IMAGE_THUNK_DATA64*) (iDataRO + m_iDataDlls[i].m_iltOffset);
            IMAGE_THUNK_DATA64 *iat = (IMAGE_THUNK_DATA64*) (m_iDataIAT + m_iDataDlls[i].m_iatOffset);

            int ibnOffset = m_iDataDlls[i].m_ibnOffset;
            for (int j=0; j < m_iDataDlls[i].m_numMethods; j++)
            {
                ilt[j].u1.AddressOfData = VAL64((ULONG)(ibnOffset + iDataOffsetRO));
                iat[j].u1.AddressOfData = VAL64((ULONG)(ibnOffset + iDataOffsetRO));

                iDataSectionRO.addSectReloc( (unsigned)(iDataOffsetRO + (char *)(&ilt[j].u1.AddressOfData) - iDataRO),
                                             iDataSectionRO, srRelocAbsolute);
                m_iDataSectionIAT->addSectReloc( (unsigned)(m_iDataOffsetIAT + (char *)(&iat[j].u1.AddressOfData) - m_iDataIAT),
                                                 iDataSectionRO, srRelocAbsolute);
                int nameLen = (int)(strlen(m_iDataDlls[i].m_methodName[j]) + 1);
                memcpy(iDataRO + ibnOffset + offsetof(IMAGE_IMPORT_BY_NAME, Name),
                       m_iDataDlls[i].m_methodName[j], nameLen);
                ibnOffset += sizeof(WORD) + nameLen + nameLen%2;
            }
        }

        // now fill in the import lookup table for each DLL
        strcpy_s(iDataRO + m_iDataDlls[i].m_nameOffset, 
                 iDataSizeRO - m_iDataDlls[i].m_nameOffset,
                 m_iDataDlls[i].m_name);

    } // end of for loop i < m_dllCount


    if (getPEWriter().isI386())
    {
        // Put the entry point code into the PE file
        unsigned entryPointOffset = getTextSection().dataLen();
        int iatOffset = (int) (entryPointOffset + (m_dllSwitch ? CorDllMainX86IATOffset : CorExeMainX86IATOffset));
        align = 4;    // x86 fixups must be 4-byte aligned

        // The IAT offset must be aligned because fixup is applied to it.
        diff = ((iatOffset + align -1) & ~(align-1)) - iatOffset;
        if (diff)
        {
            char* pDiff = getTextSection().getBlock(diff);
            if(NULL==pDiff) return E_OUTOFMEMORY;
            memset(pDiff,0,diff);
            entryPointOffset += diff;
        }
        _ASSERTE((getTextSection().dataLen() + (m_dllSwitch ? CorDllMainX86IATOffset : CorExeMainX86IATOffset)) % align == 0);

        getPEWriter().setEntryPointTextOffset(entryPointOffset);
        if (m_dllSwitch)
        {
            UCHAR *dllMainBuf = (UCHAR*)getTextSection().getBlock(sizeof(DllMainX86Template));
            if(dllMainBuf==NULL) return E_OUTOFMEMORY;
            memcpy(dllMainBuf, DllMainX86Template, sizeof(DllMainX86Template));
            //mscoree.dll
            setAddrReloc(dllMainBuf+CorDllMainX86IATOffset, m_iDataDlls[0].m_iatOffset + m_iDataOffsetIAT);
            addAddrReloc(getTextSection(), dllMainBuf, entryPointOffset+CorDllMainX86IATOffset, m_iDataSectionIAT);
        }
        else
        {
            UCHAR *exeMainBuf = (UCHAR*)getTextSection().getBlock(sizeof(ExeMainX86Template));
            if(exeMainBuf==NULL) return E_OUTOFMEMORY;
            memcpy(exeMainBuf, ExeMainX86Template, sizeof(ExeMainX86Template));
            //mscoree.dll
            setAddrReloc(exeMainBuf+CorExeMainX86IATOffset, m_iDataDlls[0].m_iatOffset + m_iDataOffsetIAT);
            addAddrReloc(getTextSection(), exeMainBuf, entryPointOffset+CorExeMainX86IATOffset, m_iDataSectionIAT);
        }
    }
    else if (getPEWriter().isAMD64())
    {
        // Put the entry point code into the PE file
        unsigned entryPointOffset = getTextSection().dataLen();
        int iatOffset = (int) (entryPointOffset + (m_dllSwitch ? CorDllMainAMD64IATOffset : CorExeMainAMD64IATOffset));
        align = 16;    // AMD64 fixups must be 8-byte aligned

        // The IAT offset must be aligned because fixup is applied to it.
        diff = ((iatOffset + align -1) & ~(align-1)) - iatOffset;
        if (diff)
        {
            char* pDiff = getTextSection().getBlock(diff);
            if(NULL==pDiff) return E_OUTOFMEMORY;
            memset(pDiff,0,diff);
            entryPointOffset += diff;
        }
        _ASSERTE((getTextSection().dataLen() + (m_dllSwitch ? CorDllMainAMD64IATOffset : CorExeMainAMD64IATOffset)) % align == 0);
        
        getPEWriter().setEntryPointTextOffset(entryPointOffset);
        if (m_dllSwitch)
        {
            UCHAR *dllMainBuf = (UCHAR*)getTextSection().getBlock(sizeof(DllMainAMD64Template));
            if(dllMainBuf==NULL) return E_OUTOFMEMORY;
            memcpy(dllMainBuf, DllMainAMD64Template, sizeof(DllMainAMD64Template));
            //mscoree.dll
            setAddrReloc(dllMainBuf+CorDllMainAMD64IATOffset, m_iDataDlls[0].m_iatOffset + m_iDataOffsetIAT);
            addAddrReloc(getTextSection(), dllMainBuf, entryPointOffset+CorDllMainAMD64IATOffset, m_iDataSectionIAT);
        }
        else
        {
            UCHAR *exeMainBuf = (UCHAR*)getTextSection().getBlock(sizeof(ExeMainAMD64Template));
            if(exeMainBuf==NULL) return E_OUTOFMEMORY;
            memcpy(exeMainBuf, ExeMainAMD64Template, sizeof(ExeMainAMD64Template));
            //mscoree.dll
            setAddrReloc(exeMainBuf+CorExeMainAMD64IATOffset, m_iDataDlls[0].m_iatOffset + m_iDataOffsetIAT);
            addAddrReloc(getTextSection(), exeMainBuf, entryPointOffset+CorExeMainAMD64IATOffset, m_iDataSectionIAT);
        }
    }
    else if (getPEWriter().isIA64())
    {
        // Must have a PE+ PE64 file
        //_ASSERTE(!getPEWriter().isPE32());

        // Put the entry point code into the PE+ file
        curOffset = getTextSection().dataLen();
        align = 16;       // instructions on ia64 must be 16-byte aligned

        // The entry point address be aligned
        diff = ((curOffset + align -1) & ~(align-1)) - curOffset;
        if (diff)
        {
            char* pDiff = getTextSection().getBlock(diff);
            if(NULL==pDiff) return E_OUTOFMEMORY;
            memset(pDiff,0,diff);
        }

        unsigned entryPointOffset = getTextSection().dataLen();

        if (m_dllSwitch)
        {
            UCHAR *dllMainBuf = (UCHAR*)getTextSection().getBlock(sizeof(DllMainIA64Template));
            if (dllMainBuf==NULL) return E_OUTOFMEMORY;
            memcpy(dllMainBuf, DllMainIA64Template, sizeof(DllMainIA64Template));
        }
        else
        {
            UCHAR *exeMainBuf = (UCHAR*)getTextSection().getBlock(sizeof(ExeMainIA64Template));
            if (exeMainBuf==NULL) return E_OUTOFMEMORY;
            memcpy(exeMainBuf, ExeMainIA64Template, sizeof(ExeMainIA64Template));
        }

        // Put the entry point function pointer into the PE file
        unsigned entryPlabelOffset = getTextSection().dataLen();
        getPEWriter().setEntryPointTextOffset(entryPlabelOffset);

        UCHAR * entryPtr = (UCHAR*)getTextSection().getBlock(sizeof(ULONGLONG));
        UCHAR * gpPtr    = (UCHAR*)getTextSection().getBlock(sizeof(ULONGLONG));

        memset(entryPtr,0,sizeof(ULONGLONG));
        memset(gpPtr,0,sizeof(ULONGLONG));

        setAddrReloc(entryPtr, entryPointOffset);
        addAddrReloc(getTextSection(), entryPtr, entryPlabelOffset, &getTextSection());

        setAddrReloc(gpPtr, m_iDataDlls[0].m_iatOffset + m_iDataOffsetIAT);
        addAddrReloc(getTextSection(), gpPtr, entryPlabelOffset+8, m_iDataSectionIAT);
    }
    else
    {
        _ASSERTE(!"Unknown target machine");
    }

    // Now set our IAT entry since we're using the IAT
    setDirectoryEntry(*m_iDataSectionIAT, IMAGE_DIRECTORY_ENTRY_IAT, iDataSizeIAT, m_iDataOffsetIAT);

    return S_OK;
} // HRESULT CeeFileGenWriter::emitExeMain()


HRESULT CeeFileGenWriter::setManifestEntry(ULONG size, ULONG offset)
{
    if (offset)
        m_dwManifestRVA = offset;
    else {
        CeeSection TextSection = getTextSection();
        getMethodRVA(TextSection.dataLen() - size, &m_dwManifestRVA);
    }

    m_dwManifestSize = size;
    return S_OK;
} // HRESULT CeeFileGenWriter::setManifestEntry()

HRESULT CeeFileGenWriter::setStrongNameEntry(ULONG size, ULONG offset)
{
    m_dwStrongNameRVA = offset;
    m_dwStrongNameSize = size;
    return S_OK;
} // HRESULT CeeFileGenWriter::setStrongNameEntry()

HRESULT CeeFileGenWriter::setVTableEntry64(ULONG size, void* ptr)
{
    if (ptr && size)
    {
        void * pv;
        CeeSection TextSection = getTextSection();
        // make it DWORD-aligned
        ULONG L = TextSection.dataLen();
        if((L &= ((ULONG)sizeof(DWORD)-1)))
        {
            L = (ULONG)sizeof(DWORD) - L;
            if((pv = TextSection.getBlock(L)))
                memset(pv,0,L);
            else
                return E_OUTOFMEMORY;
        }
        getMethodRVA(TextSection.dataLen(), &m_dwVTableRVA);
        if((pv = TextSection.getBlock(size)))
        {
            memcpy(pv,ptr,size);
        }
        else
            return E_OUTOFMEMORY;
        m_dwVTableSize = size;
    }

    return S_OK;
} // HRESULT CeeFileGenWriter::setVTableEntry()

HRESULT CeeFileGenWriter::setVTableEntry(ULONG size, ULONG offset)
{
    return setVTableEntry64(size,(void*)(ULONG_PTR)offset);
} // HRESULT CeeFileGenWriter::setVTableEntry()

HRESULT CeeFileGenWriter::setEnCRvaBase(ULONG dataBase, ULONG rdataBase)
{
    setEnCMode();
    getPEWriter().setEnCRvaBase(dataBase, rdataBase);
    return S_OK;
} // HRESULT CeeFileGenWriter::setEnCRvaBase()

HRESULT CeeFileGenWriter::computeSectionOffset(CeeSection &section, __in char *ptr,
                                               unsigned *offset)
{
    *offset = section.computeOffset(ptr);

    return S_OK;
} // HRESULT CeeFileGenWriter::computeSectionOffset()

HRESULT CeeFileGenWriter::computeOffset(__in char *ptr,
                                        CeeSection **pSection, unsigned *offset)
{
    TESTANDRETURNPOINTER(pSection);

    CeeSection **s = m_sections;
    CeeSection **sEnd = s + m_numSections;
    while (s < sEnd)
    {
        if ((*s)->containsPointer(ptr))
        {
            *pSection = *s;
            *offset = (*s)->computeOffset(ptr);

            return S_OK;
        }
        s++;
    }

    return E_FAIL;
} // HRESULT CeeFileGenWriter::computeOffset()

HRESULT CeeFileGenWriter::getCorHeader(IMAGE_COR20_HEADER **ppHeader)
{
    *ppHeader = m_corHeader;
    return S_OK;
} // HRESULT CeeFileGenWriter::getCorHeader()



// Globals.
HINSTANCE       g_hThisInst;            // This library.

//*****************************************************************************
// Handle lifetime of loaded library.
//*****************************************************************************
extern "C"
BOOL WINAPI DllMain(HANDLE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        {   // Save the module handle.
            g_hThisInst = (HINSTANCE)hInstance;
            DisableThreadLibraryCalls((HMODULE)hInstance);
        }
        break;
    case DLL_PROCESS_DETACH:
#ifdef _DEBUG
        DbgAllocReport(NULL,TRUE,FALSE);
#endif
        break;
    }

    return (true);
} // BOOL WINAPI DllMain()


HINSTANCE GetModuleInst()
{
    return (g_hThisInst);
} // HINSTANCE GetModuleInst()

