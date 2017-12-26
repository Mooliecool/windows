// ==++==
// 
//  
//   Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//  
//   The use and distribution terms for this software are contained in the file
//   named license.txt, which can be found in the root of this distribution.
//   By using this software in any fashion, you are agreeing to be bound by the
//   terms of this license.
//  
//   You must not remove this notice, or any other, from this software.
//  
// 
// ==--==
// merge.cpp : The code to handle mergin of PEs
//

#include "pch.h"

#include "alink.h"
#include "msgsids.h"
#include "common.h"
#include "merge.h"

#include <corhlpr.cpp>
#define W_CreateFile CreateFileW

OpCode OpCodes [] = {
#define InlineNone          0x0000
#define InlineVar           0x0002
#define InlineI             0x0004
#define InlineR             0x0008
#define InlineBrTarget      0x0004
#define InlineTok           0x8004
#define InlineMethod        0x8004
#define InlineField         0x8004
#define InlineType          0x8004
#define InlineString        0x8004
#define InlineSig           0x8004
#define InlineI8            0x0008
#define InlineSwitch        0x4004  // U4 + I4 ...
#define InlinePhi           0x4001  // U1 + U2 ... 
#define ShortInlineVar      0x0001
#define ShortInlineI        0x0001
#define ShortInlineR        0x0004
#define ShortInlineBrTarget 0x0001
#define OPDEF(id, name, pop, push, operand, type, len, b1, b2, cf) { b1, b2, len, (BYTE)(operand & 0x00FF) + len, ((operand & 0x8000) == 0x8000), ((operand & 0x4000) == 0x4000), ((operand & 0x2000) == 0x2000) },
#include "opcode.def"
#undef OPDEF
#undef InlineNone
#undef InlineVar
#undef InlineI
#undef InlineR
#undef InlineBrTarget
#undef InlineTok
#undef InlineMethod
#undef InlineField
#undef InlineType
#undef InlineString
#undef InlineSig
#undef InlineI8
#undef InlineSwitch
#undef InlinePhi
#undef ShortInlineVar
#undef ShortInlineI
#undef ShortInlineR
#undef ShortInlineBrTarget
};



#ifdef _DEBUG
const DWORD * CheckFlags(LPCSTR env_name, LPCSTR name)
{
    static DWORD flag = 0;
    // Try the environment strings
    char env[MAX_PATH];
    size_t retVal;
    if (getenv_s(&retVal, env, sizeof(env), env_name)) {
        char * end = NULL;
        DWORD rval = strtoul(env, &end, 0);
        if (end != NULL && *end == '\0') {
            flag = rval;
            return &flag;
        }
    }


    return NULL;
}
#endif


CDataMember::CDataMember()
{
    memset(this, 0, sizeof(CDataMember));
}

CDataMember::CDataMember(mdToken tk, DWORD RVA, DWORD Size, void * data)
{
    dwAlignment = 1;
    tkMember = tk;
    dwRVA = RVA;
    dwSize = Size;
    pvData = data;
}

CResource::CResource()
{
    memset(this, 0, sizeof(CResource));
}

CResource::~CResource()
{
    if (pszName)
        delete [] pszName;

    if (pszDescription)
        delete [] pszDescription;

    if (pszMime)
        delete [] pszMime;

    if (pszLocale)
        delete [] pszLocale;
    
    pszName = pszDescription = pszLocale = pszMime = NULL;
}

CTokenMap::CTokenMap() : m_Tokens(_TokenMap())
{
    m_Mask = mdTokenNil;
}

CTokenMap::~CTokenMap()
{
}


bool CTokenMap::ReMapToken(mdToken *pToken, bool fSwap)
{
    mdToken Token;

    Token = fSwap ? VAL32(*pToken) : *pToken;

    if ((m_Mask & Token) != Token)
        return false;

    _TokenMap find, *found;

    find.tkOldToken = Token;
    if (NULL == (found = m_Tokens.Find(find, _TokenMap::CompareOld)))
        return false;

    *pToken = fSwap ? VAL32(found->tkNewToken) : found->tkNewToken;
    return true;
}

HRESULT CTokenMap::Map(mdToken oldToken, mdToken newToken)
{
    _TokenMap newMap;
    newMap.tkOldToken = oldToken;
    newMap.tkNewToken = newToken;

    if (oldToken == newToken)
        return S_OK;

    m_Mask |= oldToken;
    if (m_Tokens.name.tkOldToken == m_Tokens.name.tkNewToken &&
        m_Tokens.name.tkOldToken == mdTokenNil) {
        // If the root is empty, replace it
        m_Tokens.name = newMap;
        return S_OK;
    }

    HRESULT hr = m_Tokens.Add(newMap, _TokenMap::CompareOld);
    if (hr == S_FALSE) {
        _TokenMap *found;

        found = m_Tokens.Find(newMap, _TokenMap::CompareOld);
        if (found != NULL) {
            found->tkNewToken = newToken;
            return S_FALSE;
        } else {
            ASSERT(found != NULL); // We couldn't add it because it already existed, but now we can't find it?
            return E_FAIL;
        }
    }

    return hr;
}

PIMAGE_SECTION_HEADER PEFile::ImageNextSection(PIMAGE_SECTION_HEADER section)
{
    ULONG i, dwStart = section ? section->PointerToRawData : 0, dwStop;
    PIMAGE_SECTION_HEADER NtSection;
    PIMAGE_SECTION_HEADER best = NULL;

    if (bIs64) {
        NtSection = IMAGE_FIRST_SECTION( pNT64 );
        dwStop = VAL16(pNT64->FileHeader.NumberOfSections);
    } else {
        NtSection = IMAGE_FIRST_SECTION( pNT32 );
        dwStop = VAL16(pNT32->FileHeader.NumberOfSections);
    }
    for (i=0; i < dwStop; i++) {
        if (VAL32(NtSection->PointerToRawData) > dwStart &&
            (best == NULL || VAL32(NtSection->PointerToRawData) < VAL32(best->PointerToRawData)))
            best = NtSection;
        
        ++NtSection;
    }

    return best;
}

PIMAGE_SECTION_HEADER PEFile::ImageRvaToSection(ULONG Rva)
{
    ULONG i, dwStop;
    PIMAGE_SECTION_HEADER NtSection;

    if (bIs64) {
        NtSection = IMAGE_FIRST_SECTION( pNT64 );
        dwStop = VAL16(pNT64->FileHeader.NumberOfSections);
    } else {
        NtSection = IMAGE_FIRST_SECTION( pNT32 );
        dwStop = VAL16(pNT32->FileHeader.NumberOfSections);
    }
    for (i=0; i < dwStop; i++) {
        if (Rva >= VAL32(NtSection->VirtualAddress) &&
            Rva < VAL32(NtSection->VirtualAddress) + VAL32(NtSection->SizeOfRawData))
            return NtSection;
        
        ++NtSection;
    }

    return NULL;
}

PVOID PEFile::ImageRvaToVa(ULONG Rva)
{
    PIMAGE_SECTION_HEADER NtSection = ImageRvaToSection(Rva);

    if (NtSection != NULL) {
        return (PVOID)((PCHAR)pbMapAddress +
                       (Rva - VAL32(NtSection->VirtualAddress)) +
                       VAL32(NtSection->PointerToRawData));
    }
    else
        return NULL;
}

bool PEFile::setCOMHeader()
{
    // Get the image header from the image, then get the directory location
    // of the COM+ header which may or may not be filled out.
    if (bIs64)
    {
        if (VAL32(pNT64->OptionalHeader.NumberOfRvaAndSizes) >= IMAGE_DIRECTORY_ENTRY_COMHEADER)
            pICH = (IMAGE_COR20_HEADER *) ImageRvaToVa(VAL32(pNT64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COMHEADER].VirtualAddress));
    }
    else
    {
        if (VAL32(pNT32->OptionalHeader.NumberOfRvaAndSizes) >= IMAGE_DIRECTORY_ENTRY_COMHEADER)
            pICH = (IMAGE_COR20_HEADER *) ImageRvaToVa(VAL32(pNT32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COMHEADER].VirtualAddress));
    }

    return pICH != NULL;
}

bool PEFile::FindNTHeader()
{
    IMAGE_DOS_HEADER   *pDosHeader;

    pDosHeader = (IMAGE_DOS_HEADER *) pbMapAddress;
    pNT32 = NULL;
    bIs64 = false;

    if ((pDosHeader->e_magic == VAL16(IMAGE_DOS_SIGNATURE)) &&
        (pDosHeader->e_lfanew != 0))
    {
        pNT32 = (IMAGE_NT_HEADERS32*) (VAL32(pDosHeader->e_lfanew) + (BYTE*) pDosHeader);

        // NOTE: at this point, it might be either a 32-bit or 64-bit image
        // but the headers are the same upto the Magic
        if ((pNT32->Signature != VAL32(IMAGE_NT_SIGNATURE))) {
            pNT32 = NULL;
            return false;
        }
        if (pNT32->OptionalHeader.Magic == VAL16(IMAGE_NT_OPTIONAL_HDR32_MAGIC)) {
            bIs64 = false;
            return true;
        } else if (pNT64->OptionalHeader.Magic == VAL16(IMAGE_NT_OPTIONAL_HDR64_MAGIC)) {
            bIs64 = true;
            return true;
        } else {
            pNT32 = NULL;
            return false;
        }

    }
    else
        return false;

}

HRESULT PEFile::OpenFile( LPCWSTR pszFilename, bool bWriteable)
{
    HRESULT             hr = S_OK;
    DWORD               dwRead = 0;
    HANDLE              hFile, hMap = NULL;

    hFile = OpenFileEx(pszFilename, &dwLength, NULL, bWriteable);
    if (hFile == INVALID_HANDLE_VALUE) {
        dwRead = GetLastError();
        hr = HRESULT_FROM_WIN32(dwRead);
        goto FAIL;
    }

    hMap = CreateFileMappingA( hFile, NULL, bWriteable ? PAGE_READWRITE : PAGE_READONLY, 0, dwLength, NULL);
    if (hMap == NULL) {
        dwRead = GetLastError();
        hr = HRESULT_FROM_WIN32(dwRead);
        goto FAIL;
    }
    
    pbMapAddress = (BYTE*)MapViewOfFile(hMap, bWriteable ? FILE_MAP_WRITE : FILE_MAP_READ, 0, 0, dwLength);
    if (!pbMapAddress) {
        dwRead = GetLastError();
        hr = HRESULT_FROM_WIN32(dwRead);
        goto FAIL;
    }

    // Don't need these anymore
    CloseHandle(hFile);
    hFile = INVALID_HANDLE_VALUE;
    CloseHandle(hMap);
    hMap = NULL;

    return hr;

FAIL:
    
    if (hFile != INVALID_HANDLE_VALUE)
        CloseHandle(hFile);
    
    if (hMap != NULL)
        CloseHandle(hMap);

    if (pbMapAddress)
        UnmapViewOfFile(pbMapAddress);
    pbMapAddress = NULL;

    return hr;
}

HRESULT PEFile::OpenFileAs(IMetaDataDispenserEx *pDisp, LPCWSTR pszFileName, bool bAllowNonIl, bool bWriteable)
{
    HRESULT             hr = E_FAIL;
    VARIANT             vOld;

    V_VT(&vOld) = VT_EMPTY;
    if (FAILED(hr = OpenFile(pszFileName, bWriteable)))
        goto FAIL;

    // Extract header info from file
    if (!FindNTHeader() ||
        !setCOMHeader())
        goto FAIL;

    if ((pICH->Flags & VAL32(COMIMAGE_FLAGS_ILONLY)) == 0 && !bAllowNonIl) {
        // This image is not marked as IL-only!
        // we can't merge it
        hr = E_INVALIDARG;
        goto FAIL;
    }

    if (bWriteable) {
        VARIANT v;

        if (FAILED(hr = pDisp->GetOption(MetaDataSetUpdate, &vOld)))
            goto FAIL;

        // Turn on incremental build for the schema (so we can delete tokens etc.)
        V_VT(&v) = VT_UI4;
        V_UI4(&v) = MDUpdateIncremental;
        if (FAILED(hr = pDisp->SetOption(MetaDataSetUpdate, &v)))
            goto FAIL;
    }

    hr = pDisp->OpenScopeOnMemory(
            ImageRvaToVa(VAL32(pICH->MetaData.VirtualAddress)),
            VAL32(pICH->MetaData.Size),
            bWriteable ? ofRead | ofWrite : ofRead, 
            IID_IMetaDataImport, 
            (IUnknown **)&pImport);

    if (FAILED(hr))
        goto FAIL;
    else if ((pICH->Flags & VAL32(COMIMAGE_FLAGS_ILONLY)) == 0 && bAllowNonIl && hr == S_OK)
        hr = S_FALSE;

    if (V_VT(&vOld) != VT_EMPTY) {
        HRESULT hr2 = pDisp->SetOption(MetaDataSetUpdate, &vOld);
        if (FAILED(hr2)) {
            hr = hr2;
            goto FAIL;
        }
    }

    return hr;

FAIL:
    
    if (pbMapAddress)
        UnmapViewOfFile(pbMapAddress);
    pbMapAddress = NULL;

    if (pDisp && V_VT(&vOld) != VT_EMPTY)
        pDisp->SetOption(MetaDataSetUpdate, &vOld);

    return hr;
}

HRESULT PEFile::EmitMembers(ICeeFileGen *pOutFile, HCEEFILE hCeeFile, HCEESECTION hIlSection, HCEESECTION hResSection, DWORD * pdwOffset, CTokenMap *pMap, IMetaDataEmit *pEmit)
{
#define COUNT   32

    HRESULT                     hr = E_FAIL;
    HCORENUM                    hTypes = NULL, hMembers;
    IMetaDataAssemblyImport *   pAImport = NULL;
    IMetaDataAssemblyEmit *     pAEmit = NULL;
    mdManifestResource          tkRes[COUNT];
    mdTypeDef                   tkTypes[COUNT];
    mdMethodDef                 tkMethods[COUNT];
    mdFieldDef                  tkFields[COUNT];
    mdToken                     tkLocation = 0;
    ULONG                       cTypes = 0, cMembers = 0, cRes = 0, iRes;
    ULONG                       RVA, newRVA, size, dwFlags;
    UINT                        offset;
    BYTE *                      pWrite = NULL;
    bool                        bMove = (pOutFile != NULL);

    if (bMove) {
        if (hCeeFile == NULL || hIlSection == NULL || hResSection == NULL || pdwOffset == NULL)
            return E_POINTER;
    } else {
        if (hCeeFile != NULL || hIlSection != NULL || hResSection != NULL || pdwOffset != NULL)
            return E_INVALIDARG;
    }

    // Do this to get any 'global' methods or fields
    cTypes = 1;
    tkTypes[0] = NULL;
    goto ENUM_MEMBERS;

    do {
        IfFailGo(pImport->EnumTypeDefs( &hTypes, tkTypes, COUNT, &cTypes));

ENUM_MEMBERS:
        for (ULONG iType = 0; iType < cTypes; iType++) {
            // Methods
            hMembers = NULL;
            do {
                IfFailGo(pImport->EnumMethods( &hMembers, tkTypes[iType], tkMethods, COUNT, &cMembers));

                for (ULONG iMember = 0; iMember < cMembers; iMember++) {
                    IfFailGo(pImport->GetRVA( tkMethods[iMember], &RVA, &dwFlags));

                    if ((dwFlags & miNative) && RVA != 0) { // NOTE: this also catche miRuntime!
                        // We can't merge this because we don't know the size!
                        hr = S_FALSE;
                        continue;
                    }

                    pMap->ReMapToken(&tkMethods[iMember], false);
                    if (RVA) {
                        COR_ILMETHOD *pMethod = (COR_ILMETHOD*)ImageRvaToVa(RVA);

                        if (bMove) {
                            COR_ILMETHOD_DECODER meth((const COR_ILMETHOD *)pMethod);
                            pWrite = NULL;

                            size = meth.GetOnDiskSize(pMethod);
                            offset = newRVA = 0;

                            // Move the method
                            IfFailGo(pOutFile->GetSectionBlock( hIlSection, size, 
                                ((meth.IsFat() || meth.EHCount() > 0) ? 4 : 1), (void**)&pWrite));
                            IfFailGo(pOutFile->ComputeSectionOffset( hIlSection, (char*)pWrite, &offset));
                            IfFailGo(pOutFile->GetMethodRVA( hCeeFile, offset, &newRVA));
                            IfFailGo(pEmit->SetRVA( tkMethods[iMember], newRVA));
                            memcpy(pWrite, pMethod, size);
                        } else {
                            pWrite = (BYTE*)pMethod;
                            IfFailGo(pEmit->SetRVA( tkMethods[iMember], RVA));
                        }

                        // Remap the tokens in the new file
                        IfFailGo(FindTokens(pWrite, pMap));
                    }
                }
            } while (cMembers > 0);
            pImport->CloseEnum(hMembers);

            // Fields
            hMembers = NULL;
            do {
                IfFailGo(pImport->EnumFields( &hMembers, tkTypes[iType], tkFields, COUNT, &cMembers));

                for (ULONG iMember = 0; iMember < cMembers; iMember++) {
                    RVA = 0;
                    hr = pImport->GetRVA( tkFields[iMember], &RVA, NULL);
                    if (hr == CLDB_E_RECORD_NOTFOUND)
                        continue;
                    IfFailGo(hr);

                    if (RVA) {
                        PCCOR_SIGNATURE pSig;
                        ULONG   cbSig;
                        ULONG   ulSize = 0;
                        IfFailGo(pImport->GetFieldProps(tkFields[iMember], NULL, NULL, 0, NULL, NULL, &pSig, &cbSig, NULL, NULL, NULL));
                        if(*pSig++ == IMAGE_CEE_CS_CALLCONV_FIELD) {
                            if (*pSig++ == ELEMENT_TYPE_VALUETYPE) {
                                mdToken tkType = CorSigUncompressToken( pSig);
                                IfFailGo(pImport->GetClassLayout( tkType, NULL, NULL, 0, NULL, &ulSize));
                            }
                        }
                        pMap->ReMapToken(&tkFields[iMember], false);
                        if (!bMove) {
                            IfFailGo(pEmit->SetRVA( tkFields[iMember], RVA));
                        } else if (ulSize > 0) {
                            // Move the field
                            void * pData = ImageRvaToVa(RVA);
                            offset = newRVA = 0;
                            IfFailGo(pOutFile->GetSectionBlock( hIlSection, ulSize, 8, (void**)&pWrite));
                            IfFailGo(pOutFile->ComputeSectionOffset( hIlSection, (char*)pWrite, &offset));
                            IfFailGo(pOutFile->GetMethodRVA( hCeeFile, offset, &newRVA));
                            IfFailGo(pEmit->SetRVA( tkFields[iMember], newRVA));
                            memcpy(pWrite, pData, ulSize);
                            hr = S_OK;
                        } else {
                            // We can't merge this because we don't know the size!
                            hr = S_FALSE;
                        }
                    }

                }
            } while (cMembers > 0);
            pImport->CloseEnum(hMembers);
        }
    } while (cTypes > 0);

    hMembers = NULL;
    if (!bMove)
        // If we're cleaning up the module, we don't need to copy the resource info
        goto ErrExit;

    // Now get embedded resources
    IfFailGo(pImport->QueryInterface( IID_IMetaDataAssemblyImport, (void**)&pAImport));
    IfFailGo(pEmit->QueryInterface( IID_IMetaDataAssemblyEmit, (void**)&pAEmit));

    newRVA = *pdwOffset;
    do {
        IfFailGo(pAImport->EnumManifestResources( &hMembers,  tkRes, COUNT, &cRes));

        for (iRes = 0; iRes < cRes; iRes++) {
            DWORD cchLen;
            IfFailGo(pAImport->GetManifestResourceProps( tkRes[iRes], NULL, 0, &cchLen, &tkLocation, &RVA, &dwFlags));

            if (IsNilToken(tkLocation) && RVA < VAL32(pICH->Resources.Size)) {
                // Remember that RVA is really an offset!
                DWORD *pSize = (DWORD*)ImageRvaToVa(VAL32(pICH->Resources.VirtualAddress) + RVA);
                LPWSTR pszName = NULL;
                if (!pSize || RVA + GET_UNALIGNED_VAL32(pSize) + sizeof(DWORD) > VAL32(pICH->Resources.Size)) {
                    // We can't merge this because the resource is too big!
                    IfFailGo(E_FAIL);
                }

                size = GET_UNALIGNED_VAL32(pSize) + sizeof(DWORD);
                if (cchLen)
                    pszName = new WCHAR[cchLen];
                IfFailGo(pAImport->GetManifestResourceProps( tkRes[iRes], pszName, cchLen, NULL, NULL, NULL, NULL));
                // Don't need to worry about this getting ReMapped, because the merger ignores them

                if (bMove) {
                    IfFailGo(pOutFile->GetSectionBlock( hResSection, size, 1, (void**)&pWrite));
                    ASSERT(SUCCEEDED(pOutFile->ComputeSectionOffset( hResSection, (char*)pWrite, &offset)) && offset == newRVA);
                    memcpy(pWrite, pSize, size);
                    *pdwOffset = offset;
                } else {
                    newRVA = RVA;
                }
                IfFailGo(pAEmit->DefineManifestResource( pszName, mdTokenNil, newRVA, dwFlags, &tkRes[iRes]));
                newRVA += size;
            } else {
                // Presumably everything else is a linked resource?
                // Which shouldn't be allowed in modules.
            }
        }
    } while (cRes > 0);
ErrExit:
    if (hTypes)
        pImport->CloseEnum(hTypes);
    if (hMembers)
        pImport->CloseEnum(hMembers);
    if (pAImport)
        pAImport->Release();
    if (pAEmit)
        pAEmit->Release();

    return hr;

#undef COUNT
}

OpCode *PEFile::FindOpCode(BYTE *code)
{
    size_t i, max = lengthof(OpCodes);
    for (i = 0; i < max; i++) {
        if (OpCodes[i].OpLen == 1) {
            if (code[0] == OpCodes[i].b2)
                return OpCodes + i;
        } else {
            if (code[0] == OpCodes[i].b1 && code[1] == OpCodes[i].b2)
                return OpCodes + i;
        }
    }

    // Not found
    return NULL;
}

HRESULT PEFile::FindTokens(void *pMethod, CTokenMap *pMap)
{
    ASSERT(pMethod);
    COR_ILMETHOD *pData = (COR_ILMETHOD*)pMethod;

    COR_ILMETHOD_DECODER meth((const COR_ILMETHOD *)pData);
    HRESULT hr = S_OK;

    if (meth.Code) {
        ASSERT(meth.CodeSize > 0);
        // Search the code for tokens
        if (meth.IsFat()) {
            pMap->ReMapToken(&pData->Fat.LocalVarSigTok, true);
        }

        BYTE *pCurrent = (BYTE*)meth.Code;
        BYTE *pLast = pCurrent + meth.GetCodeSize();
        do {
            size_t offset = 1;     // Default increment 1 byte (assume it's a CEE_ILLEGAL or CEE_NOP)
            OpCode *pOp = FindOpCode(pCurrent);

            if (pOp) {
                if (pOp->HasToken) {
                    ASSERT(pOp->TotLen - pOp->OpLen == 4);
                    pMap->ReMapToken((mdToken*)(pCurrent + pOp->OpLen), true);
                }

                if (pOp->Unhandled) {
                    // Warn that we might be missing some tokens
                    hr = S_FALSE;
                }

                if (pOp->Special) {
                    offset = pOp->TotLen;
                    if (pOp->TotLen - pOp->OpLen == 4) {
                        DWORD count = GET_UNALIGNED_32(pCurrent + pOp->OpLen);
                        offset += count * 4; // Number of I4's
                    } else {
                        ASSERT(pOp->TotLen - pOp->OpLen == 1);
                        BYTE count = *(pCurrent + pOp->OpLen);
                        offset += count * 2; // Number of U2's
                    }
                } else {
                    offset = pOp->TotLen;
                }
            }

            // Move to the next instruction
            if (pCurrent >= pLast - offset)
                break;
            else
                pCurrent += offset;
        } while (pCurrent && pCurrent <= pLast);
    }

    if (SUCCEEDED(hr) && meth.EHCount() > 0) {
        // Get the Exception Handler tokens
        IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT *fat;
        IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_SMALL *tiny;
        COR_ILMETHOD_SECT_EH *current = (COR_ILMETHOD_SECT_EH*)meth.EH;
        unsigned count, index;

        do {
            count = current->EHCount();
            if (current->IsFat()) {
                fat = current->Fat.Clauses;
                for (index = 0; index < count; index++) {
                    if (fat[index].Flags == COR_ILEXCEPTION_CLAUSE_NONE)
                        pMap->ReMapToken((mdToken*)&fat[index].ClassToken, true);
                }
            } else {
                tiny = current->Small.Clauses;
                for (index = 0; index < count; index++) {
                    if (tiny[index].Flags == COR_ILEXCEPTION_CLAUSE_NONE)
                        pMap->ReMapToken((mdToken*)&tiny[index].ClassToken, true);
                }
            }
            if (FAILED(hr))
                break;

            // Find the EH clause section
            do {
                current = (COR_ILMETHOD_SECT_EH*)current->Next();
            } while (current && (current->Kind() & CorILMethod_Sect_KindMask) != CorILMethod_Sect_EHTable);

        } while (current);
    }

    if (meth.Sect) {
        // Warn that we might be missing some tokens
    }
    return hr;
}

HRESULT PEFile::WriteNewMetaData(IMetaDataEmit *pEmit, DWORD newSize)
{
    DWORD oldSize;
    PVOID md = NULL;
    HRESULT hr;

    if (!pEmit)
        return E_POINTER;

    oldSize = VAL32(pICH->MetaData.Size);
    if (newSize > oldSize)
        return E_INVALIDARG;

    md = ImageRvaToVa(VAL32(pICH->MetaData.VirtualAddress));
    if (!md)
        return E_INVALIDARG;

    if (FAILED(hr = pEmit->SaveToMemory( md, newSize)))
        return hr;

    // Erase any old stuff, just to be sure!
    if (newSize < oldSize)
        memset(((PBYTE)md) + newSize, 0, oldSize - newSize);

    return S_OK;
}

HRESULT PEFile::Close()
{
    DWORD * CheckSum = NULL;
    HRESULT hr = S_OK;

    if (pImport)
        pImport->Release();

    if (bIsWriteable && pbMapAddress) {
        if (bIs64) {
            if (pNT64->OptionalHeader.CheckSum)
                CheckSum = &pNT64->OptionalHeader.CheckSum;
        } else {
            if (pNT32->OptionalHeader.CheckSum)
                CheckSum = &pNT32->OptionalHeader.CheckSum;
        }

        if (CheckSum) {
            // Just set it to 0
            *CheckSum = 0;
        }
    }

    if (pbMapAddress)
        UnmapViewOfFile(pbMapAddress);

    pImport = NULL;
    pICH = NULL;
    pNT32 = NULL;
    pbMapAddress = NULL;
    bIs64 = false;
    
    return hr;
}

// static
HRESULT PEFile::GetPEKind(LPCWSTR pszFilename, DWORD *pdwPEKind, DWORD *pdwMachine)
{
    PEFile file;
    HRESULT hr;
    
    if (FAILED(hr = file.OpenFile( pszFilename, false)))
        return hr;

    if (!file.FindNTHeader()) {
        *pdwPEKind = peNot;
        *pdwMachine = 0;
        return S_OK;
    }

    DWORD dwPEKind = peNot, dwMachine = 0;
    if (file.bIs64) {
        dwPEKind |= pe32Plus;
        dwMachine = (DWORD)VAL16(file.pNT64->FileHeader.Machine);
    }
    else {
        dwMachine = (DWORD)VAL16(file.pNT32->FileHeader.Machine);
    }

    if (!file.setCOMHeader()) {
        dwPEKind |= pe32Unmanaged;
    }
    else {
        DWORD dwCorFlags = file.pICH->Flags;

        if (dwCorFlags & VAL32(COMIMAGE_FLAGS_ILONLY))
            dwPEKind |= (DWORD)peILonly;

        if (dwCorFlags & VAL32(COMIMAGE_FLAGS_32BITREQUIRED))
            dwPEKind |= (DWORD)pe32BitRequired;

        // compensate for MC++ peculiarity
        if (dwPEKind == peNot)
            dwPEKind = (DWORD)pe32BitRequired;
    }

    *pdwPEKind = dwPEKind;
    *pdwMachine = dwMachine;

    return S_OK;
}


PEFile::PEFile() : pICH(NULL), pbMapAddress(NULL), dwLength(0), pImport(NULL), bIs64(false), pNT32(NULL)
{
}

PEFile::~PEFile()
{
    if (pImport)
        pImport->Release();

    if (pbMapAddress)
        UnmapViewOfFile(pbMapAddress);

    pICH = NULL;
    pNT32 = NULL;
    pbMapAddress = NULL;
    pImport = NULL;
}
