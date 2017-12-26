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
// ===========================================================================
// File: pefile.cpp
//
// Routines for creating executable files.
// ===========================================================================

#include "stdafx.h"

// Name of the DLL that implements the debug symbol writer.
#define SYMBOL_WRITER_DLL    MAKEDLLNAME_W(L"ildbsymbols")

static const DWORD rgCreateFileFlag [] = {
    ICEE_CREATE_FILE_PURE_IL,            // platformAgnostic
    ICEE_CREATE_FILE_PURE_IL,            // platformX86 - Difference between X86 and Agnostic is in corheader later
    ICEE_CREATE_FILE_PE64 | ICEE_CREATE_MACHINE_IA64,
                                         // platformIA64
    ICEE_CREATE_FILE_PE64 | ICEE_CREATE_MACHINE_AMD64,
                                         // platformAMD64
};

C_ASSERT(lengthof(rgCreateFileFlag) == platformLast);

/*
 * This initializes all the class variables and prepares it for Beginning
 */
PEFile::PEFile()
{
    compiler = NULL;
    debugemit = NULL;
    metaemit = NULL;
    assememit = NULL;
    outfile = NULL;
    ceeFile = NULL;
    ceefilegen = NULL;
    ilSection = 0;
    rdataSection = 0;
    m_dwOffset = 0;
    m_pResBuffer = NULL;
    m_dwBaseResRva = 0x00000001;
    m_dwResSize = 0;
}

/*
 * Just a little cleanup
 */
PEFile::~PEFile()
{
    Term();
}


void PEFile::Term()
{
    if (debugemit) {
        debugemit->Abort();
        debugemit.Release();
        if (outfile && compiler->ErrorCount() > 0) {
            // The symbol writer hoses the PDB so just nuke it.
            WCHAR filename[MAX_PATH];
            if (GetPDBFileName(outfile, filename, lengthof(filename)))
                W_DeleteFile(filename);
        }
    }

    metaemit.Release();
    assememit.Release();

    if (ceeFile) {
        ceefilegen->DestroyCeeFile(&ceeFile);
        ceeFile = NULL;
    }
    if (ceefilegen) {
        compiler->DestroyCeeFileGen(ceefilegen);
        ceefilegen = NULL;
    }

    outfile = NULL;
    compiler = NULL;
}


/*
 * Get the name of the PDB file associates with a give output file.
 */
bool PEFile::GetPDBFileName(OUTFILESYM * pOutFile, __out_ecount(cchFilename) PWSTR filename, size_t cchFilename)
{
    if (pOutFile->pszPDBFile)
        return SUCCEEDED(StringCchCopyW(filename, cchFilename, pOutFile->pszPDBFile));

    if (SUCCEEDED(StringCchCopyW(filename, cchFilename, pOutFile->name->text)) &&
        ReplaceFileExtension(filename, cchFilename, L".PDB"))
    {
        return true;
    }

    return false;
}

/*
 * Get the directory of a given output file (ends with a trailing slash and \0 and all
 * within cchDirName characters).
 * if the output file has no path, uses the current working directory (which might change
 * in the non-command-line case).
 */
bool PEFile::GetOutputPath(OUTFILESYM * pOutFile, DWORD cchDirName, __out_ecount(cchDirName) PWSTR wszDirName)
{
    PCWSTR pStart = NULL;
    PCWSTR pBSlash = NULL;

    pStart = pOutFile->name->text;
    pBSlash = wcsrchr( pStart, L'\\');

    if (pBSlash == NULL) {
        pBSlash = wcsrchr( pStart, L'/');
    } else {
        PCWSTR pFSlash = wcsrchr( pBSlash, L'/');
        if (pFSlash != NULL && pFSlash > pBSlash)
            pBSlash = pFSlash;
    }

    // Can't have a fully-qualified filename without a slash in it somewhere
    if (pBSlash == NULL) {
        // Since we don't have an output directory so give the current directory
        DWORD cchCurDir = W_GetCurrentDirectory( cchDirName, wszDirName);
        if (cchCurDir == 0)
            return false;
        if (cchCurDir >= cchDirName) {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return false;
        }
        if (wszDirName[IndexAdd(cchCurDir, -1)] == L'\\') {
            // Done
            return true;
        }
        if (SizeAdd(cchCurDir, 1) >= cchDirName) {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return false;
        }

        // There's enough room so stick on the trailing "\\\0"
        wszDirName[cchCurDir++] = L'\\';
        wszDirName[cchCurDir] = L'\0';
        return true;
    }

    pBSlash++; // Move past the slash
    ASSERT(pBSlash - pStart <= 0x7FFFFFFF);
    if ((DWORD)(pBSlash - pStart) >= cchDirName) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return false;
    } else {
        wcsncpy_s(wszDirName, cchDirName, pStart, pBSlash - pStart);
        StoreAtIndex(wszDirName, pBSlash - pStart, cchDirName, L'\0');
        return true;
    }
}


/*
 * Create the debug symbol writer, if possible.
 */
typedef  HRESULT ( WINAPI * PFN_GETCLASSOBJECT)(REFCLSID, REFIID, LPVOID *);

HRESULT PEFile::CreateSymbolWriter(ISymUnmanagedWriter ** pSymwriter)
{
    HRESULT hr = E_FAIL;
    HMODULE hModule;
    PFN_GETCLASSOBJECT pfnGetClassObject;

    // First, try to load the symbol writer DLL manually and get the object. This insulates us from
    // registry goofiness.
    hModule = FindAndLoadHelperLibrary(SYMBOL_WRITER_DLL);
    if (hModule != NULL) {
        pfnGetClassObject = (PFN_GETCLASSOBJECT) GetProcAddress(hModule, "DllGetClassObject");
        if (pfnGetClassObject) {
            IClassFactory * pClassFactory;
            if (SUCCEEDED(pfnGetClassObject(CLSID_CorSymWriter_SxS, IID_IClassFactory, (LPVOID *) &pClassFactory))) {
                hr = pClassFactory->CreateInstance(NULL, IID_ISymUnmanagedWriter, (LPVOID *) pSymwriter);
                    
                pClassFactory->Release();
                if (SUCCEEDED(hr))
                    return hr;
            }
        }
    }

    hr = PAL_CoCreateInstance (
        CLSID_CorSymWriter_SxS,
        IID_ISymUnmanagedWriter,
        (void**)pSymwriter);
    return hr;
}

HRESULT PEFile::CreateSymbolReader(ISymUnmanagedReader ** pSymreader)
{
    HRESULT hr = E_FAIL;
    HMODULE hModule;
    PFN_GETCLASSOBJECT pfnGetClassObject;
    
    // First, try to load the symbol writer DLL manually and get the object. This insulates us from
    // registry goofiness.
    hModule = FindAndLoadHelperLibrary(SYMBOL_WRITER_DLL);
    if (hModule != NULL) {
        pfnGetClassObject = (PFN_GETCLASSOBJECT) GetProcAddress(hModule, "DllGetClassObject");
        if (pfnGetClassObject) {
            IClassFactory * pClassFactory;
            if (SUCCEEDED(pfnGetClassObject(CLSID_CorSymReader_SxS, IID_IClassFactory, (LPVOID *) &pClassFactory))) {
                hr = pClassFactory->CreateInstance(NULL, IID_ISymUnmanagedReader, (LPVOID *) pSymreader);
                    
                pClassFactory->Release();
                if (SUCCEEDED(hr))
                    return hr;
            }
        }
    }
    hr = PAL_CoCreateInstance(CLSID_CorSymReader_SxS,
                           IID_ISymUnmanagedReader,
                           (void**)pSymreader);
    return hr;
}


/*
 * Begin emitting an output file. If an error occurs, it is reported to the user
 * and then false is returned.
 */
bool PEFile::BeginOutputFile(COMPILER * cmp, POUTFILESYM pOutFile)
{

    WCHAR filename[MAX_PATH];
    HRESULT hr = S_OK;

    // Make sure an output file isn't already open.
    if (metaemit) {
        ASSERT(0);  // shouldn't happen.
        EndOutputFile(false);
    }

    ASSERT (!pOutFile->isUnnamed());

    compiler = cmp;

    ceefilegen = compiler->CreateCeeFileGen();

    // Get the right platform flags
    ASSERT(compiler->options.m_platform < (PlatformType)lengthof(rgCreateFileFlag));

    // Get the HCEEFILE and the important sections in the file.
    CheckHR(ceefilegen->CreateCeeFileEx(&ceeFile, FetchAtIndex(rgCreateFileFlag, compiler->options.m_platform)));

    CheckHR(ceefilegen->GetIlSection(ceeFile, &ilSection));

    CheckHR(ceefilegen->GetRdataSection(ceeFile, &rdataSection));

    //
    // Get a new metadata scope to emit to, and connect with the ceefilegen.
    //
    IMetaDataDispenser * dispenser;

    dispenser = compiler->GetMetadataDispenser();

    CheckHR(dispenser->DefineScope(CLSID_CorMetaDataRuntime,  // Format of metadata
                                    0,                         // flags
                                    IID_IMetaDataEmit2,         // Emitting interface
                                    (IUnknown **) &metaemit));

    // For remapping tokens (need to forward on to debug interface)
    CheckHR(ceefilegen->AddNotificationHandler(ceeFile, this));

    if (compiler->BuildAssembly() && FAILED(hr = metaemit->QueryInterface(IID_IMetaDataAssemblyEmit, (void**)&assememit)))
    {
        // Better error message.
        compiler->Error(NULL, FTL_ComPlusInit, compiler->ErrHR(hr));
    }

    if (compiler->options.m_fEMITDEBUGINFO) {
        if (FAILED(hr = CreateSymbolWriter(&debugemit)))
        {
            // Better error message.
            compiler->Error(NULL, FTL_DebugInit, compiler->ErrHR(hr));
        }


        // Create the PDB filename
        if (!GetPDBFileName(pOutFile, filename, lengthof(filename)))
        {
            compiler->Error(NULL, FTL_DebugInitFile, filename, compiler->ErrHR(HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW)));
        }

        // Initialize debugger
        if (FAILED(hr = debugemit->Initialize(metaemit, filename, NULL, true))) {
            if (hr == HRESULT_FROM_WIN32(ERROR_BAD_FORMAT)) 
                compiler->Error(NULL, FTL_BadPDBFormat, filename);
            else
                compiler->Error(NULL, FTL_DebugInitFile, filename, compiler->ErrHR(hr));
        }
    }

    // Set the module properties
    CheckHR(metaemit->SetModuleProps(GetModuleName(pOutFile, compiler)->text));

    // Remember output file
    outfile = pOutFile;
    return true;
}

/*static*/
NAME * PEFile::GetModuleName(POUTFILESYM pOutFile, COMPILER * compiler)
{
    if (!pOutFile->nameModule) {
        {
            // Compute and cache the name.
            ASSERT(!pOutFile->isUnnamed());

            PCWSTR pBSlash = wcsrchr(pOutFile->name->text, '\\');
            if (pBSlash)
                pBSlash++;
            else
                pBSlash = pOutFile->name->text;
            PCWSTR pFSlash = wcsrchr(pBSlash, '/');
            if (pFSlash && pFSlash > pBSlash)
                pBSlash = pFSlash + 1;

            pOutFile->nameModule = compiler->getNamemgr()->AddString(pBSlash);
        }
    }

    return pOutFile->nameModule;
}

void PEFile::SetAttributes(bool fDll)
{
    // Set output file attributes.
    CheckHR(ceefilegen->SetDllSwitch(ceeFile, fDll));

    CheckHR(ceefilegen->SetEntryPoint(ceeFile,
        outfile->entrySym ? outfile->entrySym->tokenEmit : mdTokenNil));

    if (outfile->entrySym && compiler->options.m_fEMITDEBUGINFO) {
        CheckHR(FTL_DebugEmitFailure, debugemit->SetUserEntryPoint(outfile->entrySym->tokenEmit));
    }
}

ULONG PEFile::GetRVAOfOffset(ULONG offset)
{
    ULONG codeRVA;
    CheckHR(ceefilegen->GetMethodRVA(ceeFile, offset, &codeRVA));

    return codeRVA;
}

void *PEFile::AllocateRVABlockCore(ULONG cb, ULONG alignment, ULONG *codeRVA, ULONG *offset)
{
    void * codeLocation;

    // Only call this if you are really emitting code
    ASSERT(cb > 0);

    // Get a block of size cb
    ULONG offsetCur;

    CheckHR(ceefilegen->GetSectionBlock(ilSection, cb, alignment, & codeLocation));

    CheckHR(ceefilegen->GetSectionDataLen(ilSection, &offsetCur));

    offsetCur -= cb;
    *codeRVA = GetRVAOfOffset(offsetCur);
    if (offset)
        *offset = (int) offsetCur;

    // Return the block of code allocated for this method.
    return codeLocation;
}


void PEFile::WriteCryptoKey()
{
    DWORD dwKeySize = 0;
    HRESULT hr;

    hr = compiler->linker->EmitManifest(compiler->assemID, &dwKeySize, NULL);

    if (FAILED(hr)) {
        compiler->Error(NULL, ERR_CryptoFailed, 
            compiler->ErrHR(hr), outfile->name->text);
    } else if (hr == S_OK && dwKeySize > 0) {
        DWORD dwKeyRVA = 0;
        PBYTE pbBuffer = NULL;

        pbBuffer = (PBYTE)AllocateRVABlock(dwKeySize, 1, &dwKeyRVA);
        memset(pbBuffer, 0, dwKeySize); // Zero it out

        CheckHR(ceefilegen->SetStrongNameEntry( ceeFile, dwKeySize, dwKeyRVA));
    }

}

/*
 * Get the size of embedded resources so we can emit them together
 */
bool PEFile::CalcResource(PRESFILESYM pRes)
{
    if (!pRes->isEmbedded)
        return true;

    ASSERT(m_pResBuffer == NULL && m_dwOffset == 0); // Can't add a resource after we've emitted some

    DWORD fileLen = 0;
    HANDLE hFile = OpenFileEx( pRes->filename, &fileLen);
    if (hFile == INVALID_HANDLE_VALUE) {
        compiler->Error( NULL, ERR_CantReadResource, pRes->filename, compiler->ErrGetLastError());
        return false;
    }
    CloseHandle(hFile);
    // Add up the size (with padding to an 8-byte boundary)
    m_dwResSize = RoundUp8(m_dwResSize) + fileLen + sizeof(DWORD);

    return true;
}

bool PEFile::AllocResourceBlob()
{
    ASSERT(m_pResBuffer == NULL && m_dwOffset == 0);
    if (m_dwResSize == 0)
        return true;

    ASSERT(m_dwBaseResRva == 0x00000001);

    m_pResBuffer = (PBYTE)AllocateRVABlock(m_dwResSize, 8, &m_dwBaseResRva);

    // Within the file it must be 8-byte aligned, but we write out
    // DWORDs so the in-memory buffer must be at least DWORD aligned!
    ASSERT(((DWORD_PTR)m_pResBuffer & 0x03) == 0);

    return (m_pResBuffer != NULL);
}

/*
 * Adds a resource to the Assembly Manifest
 * If pRes->isEmbedded is true, then it will embed the resource within this assembly's PE file
 *
 */
bool PEFile::AddResource(PRESFILESYM pRes)
{
    ASSERT(metaemit);

    if (pRes->isEmbedded) {
        HANDLE hFile;
        DWORD fileLen = 0, dwRead;

        ASSERT(m_pResBuffer != NULL && m_dwResSize > 0);
        DWORD dwOffset = RoundUp8(m_dwOffset); // Pad
        ASSERT(dwOffset < m_dwResSize);
        BYTE * pbBuffer = ((BYTE*)m_pResBuffer) + m_dwOffset;

        // Zero out the padding (if any)
        if (dwOffset != m_dwOffset) {
            memset( pbBuffer, 0, dwOffset - m_dwOffset);
            pbBuffer = ((BYTE*)m_pResBuffer) + dwOffset;
        }

        // If we've done our math right, this should always be on an 8-byte boundary
        ASSERT((((DWORD_PTR)pbBuffer - (DWORD_PTR)m_pResBuffer) & 0x07) == 0);

        hFile = OpenFileEx( pRes->filename, &fileLen);
        if (hFile == INVALID_HANDLE_VALUE) {
            compiler->Error( NULL, ERR_CantReadResource, pRes->filename, compiler->ErrGetLastError());
            return false;
        }

        m_dwOffset = dwOffset + fileLen + sizeof(DWORD);
        ASSERT(m_dwOffset <= m_dwResSize);
        *(DWORD*)pbBuffer = VAL32(fileLen);
        pbBuffer += sizeof(DWORD);
        if (!ReadFile( hFile, pbBuffer, fileLen, &dwRead, NULL)) {
            compiler->Error( NULL, ERR_CantReadResource, pRes->filename, compiler->ErrGetLastError());
            CloseHandle( hFile);
            return false;
        }
        CloseHandle( hFile);

        CheckHR(compiler->linker->EmbedResource(compiler->assemID, compiler->assemFile.outfile->idFile, pRes->name->text, 
            dwOffset, pRes->isVis ? mrPublic : mrPrivate));
    } else {
        if (compiler->BuildAssembly()) {
            CheckHR(compiler->linker->LinkResource(compiler->assemID, pRes->filename, NULL, pRes->name->text, 
                pRes->isVis ? mrPublic : mrPrivate));
        } else {
            compiler->Error(NULL, ERR_CantRefResource, pRes->name->text);
            return false;
        }
    }

    return true;
}


#define WToMB(pwstr, cchW, pstr, cchMB) WideCharToMultiByte(CP_ACP, 0, pwstr, cchW, pstr, cchMB, 0, 0)

//--------------------------------------------------------------
BOOL WINAPI W_MoveFile (PCWSTR pSourceFileName, PCWSTR pDestFileName)
{
    if (!MoveFileW(pSourceFileName, pDestFileName)) {

        return false;
    }

    return true;
}

/*
 * End writing an output file. If true is passed, the output file is actually written.
 * If false is passed (e.g., because an error occurred), the output file is not
 * written.
 *
 * true is returned if the output file was successfully written.
 */
void PEFile::EndOutputFile(bool writeFile)
{
    bool useTempResFile = false;
    WCHAR outFileName[MAX_PATH+1];
    WCHAR resFileName[MAX_PATH+1];
    HRESULT hr;

    PAL_TRY {

        if (writeFile && outfile->name->text[0] != L'*') {
            // Set output file attributes.
            ASSERT (outfile->name->text[0] != L'?' && outfile->name->text[0] != L'*');
            if (FAILED(hr = StringCchCopyW(outFileName, lengthof(outFileName), outfile->name->text))) {
                CheckHR(ERR_OutputWriteFailed, hr);
                goto FAILED;
            }

            CheckHR(ceefilegen->SetOutputFileName(ceeFile, outFileName));

            DWORD dwFlags = COMIMAGE_FLAGS_ILONLY;
            if (compiler->options.m_platform == platformX86)
                dwFlags |= COMIMAGE_FLAGS_32BITREQUIRED;
            CheckHR(ceefilegen->SetComImageFlags(ceeFile, dwFlags));

            if (outfile->imageBase != 0) {
                if (compiler->options.Is64Bit()) {
                    CheckHR(ceefilegen->SetImageBase64(ceeFile, outfile->imageBase));
                }
                else {
                    if (outfile->imageBase > UI64(0x00000000FFFFFFFF)) {
                        WCHAR szBuffer[16 + 3];
                        StringCchPrintfW( szBuffer, lengthof(szBuffer), L"0x%016I64x", outfile->imageBase);
                        compiler->Error( NULL, ERR_BadBaseNumber, szBuffer);
                    }
                    else {
                        CheckHR(ceefilegen->SetImageBase(ceeFile, (DWORD)outfile->imageBase));
                    }
                }
            }

            if (outfile->fileAlign != 0) {
                CheckHR(ceefilegen->SetFileAlignment(ceeFile, outfile->fileAlign));
            }

            // this is actually for managed resources
            if (m_dwResSize > 0) {
                CheckHR(ceefilegen->SetManifestEntry(ceeFile, m_dwResSize, m_dwBaseResRva));
            }

            CheckHR(ceefilegen->SetSubsystem(ceeFile, outfile->isConsoleApp ? IMAGE_SUBSYSTEM_WINDOWS_CUI : IMAGE_SUBSYSTEM_WINDOWS_GUI, 4, 0));


            if (compiler->FAbortEarly(0 /*, &es*/))
                goto FAILED;

            // Write the metadata (and force token remapping).
            CheckHR(ceefilegen->EmitMetaDataEx(ceeFile, metaemit));

            if (compiler->FAbortEarly(0 /*, &es*/))
                goto FAILED;

            if (compiler->options.m_fEMITDEBUGINFO) {
                DWORD cbCVInfo;
                BYTE * pbCVInfo;
                ULONG offset;

                // Get size of path information to PDB file.
                CheckHR(FTL_DebugEmitFailure, debugemit->GetDebugInfo(NULL, 0, &cbCVInfo, NULL));

                if (cbCVInfo > 0) {
                    // Allocate space to store it.
                    DWORD rva;
                    pbCVInfo = (BYTE*)AllocateRVABlockCore(sizeof(IMAGE_DEBUG_DIRECTORY) + cbCVInfo, 4, &rva, &offset);

                    // Write it into the PE
                    CheckHR(FTL_DebugEmitFailure, debugemit->GetDebugInfo((IMAGE_DEBUG_DIRECTORY *)pbCVInfo, cbCVInfo, &cbCVInfo, pbCVInfo + sizeof(IMAGE_DEBUG_DIRECTORY)));

                    ((IMAGE_DEBUG_DIRECTORY *) pbCVInfo)->AddressOfRawData = VAL32(offset + sizeof(IMAGE_DEBUG_DIRECTORY));
                    ((IMAGE_DEBUG_DIRECTORY *) pbCVInfo)->PointerToRawData = VAL32(offset + sizeof(IMAGE_DEBUG_DIRECTORY));
                    DWORD timeStamp;
                    CheckHR(ceefilegen->GetFileTimeStamp(ceeFile, &timeStamp));
                    ((IMAGE_DEBUG_DIRECTORY *) pbCVInfo)->TimeDateStamp = VAL32(timeStamp);

                    CheckHR(ceefilegen->AddSectionReloc(ilSection, offset + offsetof(IMAGE_DEBUG_DIRECTORY, AddressOfRawData),
                                                        ilSection, srRelocAbsolute));
                    CheckHR(ceefilegen->AddSectionReloc(ilSection, offset + offsetof(IMAGE_DEBUG_DIRECTORY, PointerToRawData),
                                                        ilSection, srRelocFilePos));

                    // Set the directory entry.
                    CheckHR(ceefilegen->SetDirectoryEntry(ceeFile, ilSection, IMAGE_DIRECTORY_ENTRY_DEBUG, sizeof(IMAGE_DEBUG_DIRECTORY), offset));
                }

                CheckHR(FTL_DebugEmitFailure, debugemit->Close());
                debugemit.Release();

                if (compiler->FAbortEarly(0 /*, &es*/))
                    goto FAILED;
            }

            if (compiler->FAbortEarly(0 /*, &es*/))
                goto FAILED;

            // Write the output file.
            hr = ceefilegen->GenerateCeeFile(ceeFile);
                CheckHR(ERR_OutputWriteFailed, hr);
        }
FAILED:     // Goto here if we had errors
        ;
    }
    PAL_FINALLY {
        if (useTempResFile) {
            // Delete the auto-generated .RES file
            if (!W_DeleteFile(resFileName))
                compiler->Error(NULL, WRN_DeleteAutoResFailed, resFileName, compiler->ErrGetLastError());
        }

        // This needs to always happen, even if writing failed.
        Term();
    }
    PAL_ENDTRY
}


/*
 * Modifies lpszFilename by removing it's extension and replacing it
 * with the one specified
 */
bool PEFile::ReplaceFileExtension(__inout_ecount(cchFilename) PWSTR lpszFilename, size_t cchFilename, PCWSTR lpszExt)
{
    const WCHAR *ext;

    // Extension is last '.' not followed by '\' or '/'
    ext = wcsrchr(lpszFilename, L'.');
    if (ext && wcschr(ext, L'/') == NULL && wcschr(ext, L'\\') == NULL) {
        lpszFilename[(DWORD)(ext-lpszFilename)] = L'\0';
    }

    // append extension
    if (SUCCEEDED(StringCchCatW(lpszFilename, cchFilename, lpszExt)))
        return true;
    else
        return false;
}

/*
 * Semi-generic method to check for failure of a meta-data method.
 */
__forceinline void PEFile::CheckHR(HRESULT hr)
{
    if (FAILED(hr))
        MetadataFailure(hr);
    SetErrorInfo(0, NULL); // Clear out any stale ErrorInfos
}


/*
 * Semi-generic method to check for failure of a meta-data method. Passed
 * an error ID in cases where the generic one is no good.
 */
__forceinline void PEFile::CheckHR(int errid, HRESULT hr)
{
    if (FAILED(hr))
        MetadataFailure(errid, hr);
    SetErrorInfo(0, NULL); // Clear out any stale ErrorInfos
}

/*
 * Handle a generic meta-data API failure.
 */
void PEFile::MetadataFailure(HRESULT hr)
{
    MetadataFailure(FTL_MetadataEmitFailure, hr);
}

/*
 * Handle an API failure. The passed error code is expected to take one insertion string
 * that is filled in with the HRESULT.
 */
void PEFile::MetadataFailure(int errid, HRESULT hr)
{
    compiler->Error(NULL, errid, compiler->ErrHR(hr), this->outfile ? this->outfile->name->text : L"");
}


STDMETHODIMP PEFile::Map(mdToken tkOld, mdToken tkNew)
{
    HRESULT hr = S_OK;

    // NOTE: for incremental compilation, we used to record the
    // TypeDefs and FieldDefs we created for <PrivateImplementationDetails>
    // compiler generated class.  We also used to create multiple
    // <PID> classes so we could preserve metadata ordering
    // and not cause any of the tokens to get remapped
    // while emitting anonymous methods and iterators.
    // Since we've canned incremental compilation, it makes no sense
    // to not remap the tokens (we'll have to remap anyways for the
    // iterators/anonymous methods).
    //
    // So beware fields will move!
    // ASSERT(TypeFromToken(tkOld) != mdtFieldDef);

    ASSERT(TypeFromToken(tkOld) != mdtTypeDef);

    if (debugemit) {
       hr = debugemit->RemapToken( tkOld, tkNew);
    }

    return hr;
}


