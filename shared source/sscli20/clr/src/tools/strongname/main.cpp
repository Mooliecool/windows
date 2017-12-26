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

#include <utilcode.h>
#include <winwrap.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <limits.h>
#include <strongname.h>
#include <cor.h>
#include <corimage.h>
#include <ndpversion.h>
#include <resources.h>
#include <specstrings.h>

#include <palstartupw.h>

#include "../../vm/thekey.h"
#include "../../vm/ecmakey.h"

#if PLATFORM_UNIX
#define EOL     L"\n"
#else   // PLATFORM_UNIX
#define EOL     L"\r\n"
#endif  // PLATFORM_UNIX

bool g_bQuiet = false;


#define MESSAGE_DLL L"sn.satellite"

static HSATELLITE LoadSatelliteResource()
{
    WCHAR path[MAX_PATH];
    WCHAR *pEnd, *pSlashEnd;

    if (!GetModuleFileNameW(NULL, path, MAX_PATH))
        return 0;

    pEnd = wcsrchr(path, L'\\');
    pSlashEnd = wcsrchr(path, L'/');
    if (pSlashEnd > pEnd)
        pEnd = pSlashEnd;
        
    if (!pEnd)
        return 0;
    ++pEnd;  // point just beyond.

    // Append message DLL name if it fits.
    if ((int) sizeof(MESSAGE_DLL) + (pEnd - path) * sizeof(WCHAR) > (int) sizeof(path) - 1)
        return 0;
    wcscpy(pEnd, MESSAGE_DLL);

    return PAL_LoadSatelliteResourceW(path);
}

static HSATELLITE g_hSatellite = NULL;

static UINT LoadResourceString(UINT uID, __out_ecount(nBufferMax) LPWSTR lpBuffer, UINT nBufferMax)
{
    if (g_hSatellite == NULL)
    {
        g_hSatellite = LoadSatelliteResource();
        if (!g_hSatellite) return NULL;
    }

    UINT cchCount = PAL_LoadSatelliteStringW(g_hSatellite, uID, lpBuffer, nBufferMax);
    if (cchCount)
        return cchCount;

    HINSTANCE hMscorEE = WszLoadLibrary(L"mscoree.dll");
    if (!hMscorEE)
        return 0;

    // We want to call LoadStringRC inside of mscoree.dll and not the one
    // linked in utilcode.lib to ensure we load the right satellite DLL.
    typedef HRESULT (*PUTILLOADSTRINGRC) (UINT, LPWSTR, int, int);
    PUTILLOADSTRINGRC pUtilLoadStringRC = (PUTILLOADSTRINGRC) GetProcAddress(hMscorEE, "LoadStringRC");

    UINT result;
    if (pUtilLoadStringRC)
        result = SUCCEEDED(pUtilLoadStringRC(LOWORD(uID), lpBuffer, nBufferMax, true));
    else
        result = 0;

    FreeLibrary(hMscorEE);
    return result;
}



// Various routines for formatting and writing messages to the console.
void OutputList(LPCWSTR szFormat, va_list pArgs)
{
    DWORD   dwLength;
    LPSTR   szMessage;
    DWORD   dwWritten;

        WCHAR  szBuffer[8192];
        
        if (_vsnwprintf_s(szBuffer, _countof(szBuffer), _TRUNCATE, szFormat, pArgs) == -1) {
            WCHAR   szWarning[256];
            size_t dwBuffSizeForWarningInWords;
            if (LoadResourceString(SN_TRUNCATED_OUTPUT, szWarning, sizeof(szWarning) / sizeof(WCHAR))) {
                
                //
                // wcscpy_s starts from an offset within the buffer. Hence 
                // dwBuffSizeForWarningInWords = szBufferSize - Offset.
                //

                dwBuffSizeForWarningInWords = wcslen(szWarning) + 1;
                wcscpy_s(&szBuffer[_countof(szBuffer) - dwBuffSizeForWarningInWords], dwBuffSizeForWarningInWords, szWarning);
            }
        }

        dwLength = (DWORD)(wcslen(szBuffer) + 1) * 3;
        szMessage = (LPSTR)_alloca(dwLength);
        WszWideCharToMultiByte(GetConsoleOutputCP(), 0, szBuffer, -1, szMessage, dwLength - 1, NULL, NULL);

    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), szMessage, (DWORD)strlen(szMessage), &dwWritten, NULL);
}

void OutputString(LPCWSTR szFormat, ...)
{
    va_list pArgs;

    va_start(pArgs, szFormat);
    OutputList(szFormat, pArgs);
    va_end(pArgs);
}

void Output(LPCWSTR szFormat, ...)
{
    va_list pArgs;

    va_start(pArgs, szFormat);
    OutputList(szFormat, pArgs);
    OutputList(EOL, pArgs);
    va_end(pArgs);
}

void Output(DWORD dwResId, ...)
{
    va_list pArgs;
    WCHAR   szFormat[1024];

    if (LoadResourceString(dwResId, szFormat, sizeof(szFormat)/sizeof(WCHAR))) {
        va_start(pArgs, dwResId);
        OutputList(szFormat, pArgs);
        OutputList(EOL, pArgs);
        va_end(pArgs);
    }
}


// Get the text for a given error code (inserts not supported). Note that the
// returned string is static (not need to deallocate, but calling this routine a
// second time will obliterate the result of the first call).
LPWSTR GetErrorString(ULONG ulError, LPCWSTR wszArg1 = NULL)
{
    static WCHAR szOutput[1024];

    DWORD dwFlags = FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM;
    const WCHAR *args[] = {wszArg1, NULL};


    bool bResult = false;
    szOutput[0] = L'\0';
    if (HRESULT_FACILITY(ulError) == FACILITY_URT) {
        WCHAR szMessage[1024];
        // If this is one of our errors, then grab the error from the rc file.
        if (LoadResourceString(ulError, szMessage, sizeof(szMessage) / sizeof(WCHAR))) {
            // Ok, see if we can format the error message.
            if (WszFormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY,
                                 szMessage,
                                 0,
                                 0,
                                 szOutput,
                                 sizeof(szOutput) / sizeof(WCHAR),
                                 (va_list *) args)) {
                bResult = true;
            }
        }


    } else {
        // Otherwise it isn't one of ours, so we need to see if the system can find the message for it.
        if (WszFormatMessage(
                              dwFlags,
                              NULL,
                              ulError,
                              0,
                              szOutput,
                              sizeof(szOutput) / sizeof(WCHAR),
                              NULL))
            bResult = true;
    }

    if (!bResult) {
        if (!LoadResourceString(SN_NO_ERROR_MESSAGE, szOutput, sizeof(szOutput) / sizeof(WCHAR)))
            wcsncpy_s(szOutput, _countof(szOutput), L"Unable to format error message ", _TRUNCATE);

        WCHAR szErrorNum[9];
        _snprintf_s((char*)szErrorNum, sizeof(szErrorNum), _TRUNCATE, "%08X", ulError);
        for (int i = 7; i >= 0; i--)
            szErrorNum[i] = (WCHAR)((char*)szErrorNum)[i];
        szErrorNum[8] = L'\0';

        wcscat_s(szOutput, _countof(szOutput), szErrorNum);
    }

    return szOutput;
}


void Title()
{
    OutputString(EOL);
    Output(SN_TITLE, VER_FILEVERSION_STR_L);
    Output(VER_LEGALCOPYRIGHT_LOGO_STR_L);
    OutputString(EOL);
}


void Usage()
{
    Output(SN_USAGE);
    Output(SN_OPTIONS);
    Output(SN_OPT_UD_1);
    Output(SN_OPT_UD_2);
    Output(SN_OPT_E_1);
    Output(SN_OPT_E_2);
    Output(SN_OPT_K_1);
    Output(SN_OPT_K_2);
    Output(SN_OPT_O_1);
    Output(SN_OPT_O_2);
    Output(SN_OPT_O_3);
    Output(SN_OPT_P_1);
    Output(SN_OPT_P_2);
    Output(SN_OPT_Q_1);
    Output(SN_OPT_Q_2);
    Output(SN_OPT_UR_1);
    Output(SN_OPT_UR_2);
    Output(SN_OPT_UR_3);
    Output(SN_OPT_RH_1);
    Output(SN_OPT_RH_2);
    Output(SN_OPT_TP_1);
    Output(SN_OPT_TP_2);
    Output(SN_OPT_UTP_1);
    Output(SN_OPT_UTP_2);
    Output(SN_OPT_TS_1);
    Output(SN_OPT_TS_2);
    Output(SN_OPT_VF_1);
    Output(SN_OPT_VF_2);
    Output(SN_OPT_VL_1);
    Output(SN_OPT_VL_2);
    Output(SN_OPT_VR_1);
    Output(SN_OPT_VR_2);
    Output(SN_OPT_VU_1);
    Output(SN_OPT_VU_2);
    Output(SN_OPT_VX_1);
    Output(SN_OPT_VX_2);
    Output(SN_OPT_H_1);
    Output(SN_OPT_H_2);
    Output(SN_OPT_H_3);
}


// Read the entire contents of a file into a buffer. This routine allocates the
// buffer (which should be freed with delete []).
DWORD ReadFileIntoBuffer(__in_z LPWSTR szFile, BYTE **ppbBuffer, DWORD *pcbBuffer)
{
    // Open the file.
    HandleHolder hFile(WszCreateFile(szFile,
                               GENERIC_READ,
                               FILE_SHARE_READ,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                               NULL));
    if(hFile == INVALID_HANDLE_VALUE)
        return GetLastError();

    // Determine file size and allocate an appropriate buffer.
    DWORD dwHigh;
    *pcbBuffer = GetFileSize(hFile, &dwHigh);
    if (dwHigh != 0) {
        return E_FAIL;
    }
    *ppbBuffer = new BYTE[*pcbBuffer];
    if (!*ppbBuffer) {
        return ERROR_OUTOFMEMORY;
    }

    // Read the file into the buffer.
    DWORD dwBytesRead;
    if (!ReadFile(hFile, *ppbBuffer, *pcbBuffer, &dwBytesRead, NULL)) {
        return GetLastError();
    }

    return ERROR_SUCCESS;
}


// Write the contents of a buffer into a file.
DWORD WriteFileFromBuffer(LPCWSTR szFile, BYTE *pbBuffer, DWORD cbBuffer)
{
    // Create the file (overwriting if necessary).
    HandleHolder hFile(WszCreateFile(szFile,
                               GENERIC_WRITE,
                               0,
                               NULL,
                               CREATE_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL));
    if(hFile == INVALID_HANDLE_VALUE)
        return GetLastError();

    // Write the buffer contents.
    DWORD dwBytesWritten;
    if (!WriteFile(hFile, pbBuffer, cbBuffer, &dwBytesWritten, NULL)) {
        return GetLastError();
    }

    return ERROR_SUCCESS;
}


// Helper to format an 8-bit integer as a two character hex string.
void PutHex(BYTE bValue, __out_ecount(2) WCHAR *szString)
{
    static const WCHAR *szHexDigits = L"0123456789abcdef";
    szString[0] = szHexDigits[bValue >> 4];
    szString[1] = szHexDigits[bValue & 0xf];
}


// Generate a hex string for a public key token.
LPWSTR GetTokenString(BYTE *pbToken, DWORD cbToken)
{
    LPWSTR  szString;
    DWORD   i;

    szString = new WCHAR[(cbToken * 2) + 1];
    if (szString == NULL)
        return L"<out of memory>";

    for (i = 0; i < cbToken; i++)
        PutHex(pbToken[i], &szString[i * 2]);
    szString[cbToken * 2] = L'\0';

    return szString;
}


// Generate a hex string for a public key.
LPWSTR GetPublicKeyString(BYTE *pbKey, DWORD cbKey, bool fInsertLineFeed = true)
{
    LPWSTR  szString;
    DWORD   i, j, src, dst;

    szString = new WCHAR[(cbKey * 2) + (((cbKey + 38) / 39) * 2) + 1];
    if (szString == NULL)
        return L"<out of memory>";

    dst = 0;
    for (i = 0; i < (cbKey + 38) / 39; i++) {
        for (j = 0; j < 39; j++) {
            src = i * 39 + j;
            if (src == cbKey)
                break;
            PutHex(pbKey[src], &szString[dst]);
            dst += 2;
        }
        if (fInsertLineFeed) {
            szString[dst++] = L'\r';
            szString[dst++] = L'\n';
        }
    }
    szString[dst] = L'\0';

    return szString;
}

enum EnumStrongNameLevel
{
    None         = 0,
    DelaySigned  = 1,
    FullySigned  = 2,
    Undetermined = 3
};

void LocateSignature(IMAGE_NT_HEADERS* pNtHeaders, BYTE* pbBase, IMAGE_COR20_HEADER* pCorHeader, DWORD dwFileLength, BYTE** ppbSignature, DWORD* pcbSignature)
{
    PAL_TRY {
        // Set up signature pointer (if we require it).
        *ppbSignature = (BYTE*)Cor_RtlImageRvaToVa(PTR_IMAGE_NT_HEADERS(pNtHeaders),
                                                 pbBase,
                                                 VAL32(pCorHeader->StrongNameSignature.VirtualAddress),
                                                 dwFileLength);
        *pcbSignature = VAL32(pCorHeader->StrongNameSignature.Size);
    }
    PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER) {
        *ppbSignature = NULL;
        *pcbSignature = 0;
    }
    PAL_ENDTRY
}

void LocateComHeader(IMAGE_NT_HEADERS* pNtHeaders, BYTE* pbBase, DWORD dwFileLength, IMAGE_COR20_HEADER** ppCorHeader)
{
    PAL_TRY {
        // See if we can find a COM+ header extension.
        *ppCorHeader = (IMAGE_COR20_HEADER*)Cor_RtlImageDirToVa(PTR_IMAGE_NT_HEADERS(pNtHeaders),
                                                                pbBase, 
                                                                IMAGE_DIRECTORY_ENTRY_COMHEADER,
                                                                dwFileLength);
    }
    PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER) {
        *ppCorHeader = NULL;
    }
    PAL_ENDTRY
}

// Check that the given file represents a strongly named assembly.
EnumStrongNameLevel IsStronglyNamedAssembly(__in_z LPWSTR szAssembly)
{
    HandleHolder               hFile(INVALID_HANDLE_VALUE);
    HANDLE                       hMap = NULL;
    BYTE                       *pbBase  = NULL;
    IMAGE_NT_HEADERS           *pNtHeaders;
    IMAGE_COR20_HEADER         *pCorHeader;
    EnumStrongNameLevel         eStrongNameLevel = Undetermined;
    DWORD                       dwFileLength;
    BYTE                       *pbSignature;
    DWORD                       cbSignature;

    // Open the file.
    hFile = WszCreateFile(szAssembly,
                        GENERIC_READ,
                        FILE_SHARE_READ,
                        NULL,
                        OPEN_EXISTING,
                        0,
                        0);
    if (hFile == INVALID_HANDLE_VALUE) 
        goto Cleanup;

    dwFileLength = SafeGetFileSize(hFile, NULL);
    if (dwFileLength == 0xffffffff)
        goto Cleanup;

    // Create a file mapping.
    hMap = WszCreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (hMap == NULL)
        goto Cleanup;

    // Map the file into memory.
    pbBase = (BYTE*)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
    if (pbBase == NULL)
        goto Cleanup;

    eStrongNameLevel = None;
    // Locate the standard file headers.
    pNtHeaders = Cor_RtlImageNtHeader(pbBase, dwFileLength);
    if (pNtHeaders == NULL)
        goto Cleanup;

    // See if we can find a COM+ header extension.
    LocateComHeader(pNtHeaders, pbBase, dwFileLength, &pCorHeader);


    if (pCorHeader == NULL)
        goto Cleanup;

    LocateSignature(pNtHeaders, pbBase, pCorHeader, dwFileLength, &pbSignature, &cbSignature);

    // Check that space has been allocated in the PE for a signature. 
    if ((pbSignature == NULL) || (cbSignature == 0))
        goto Cleanup;

    if (pCorHeader->Flags & VAL32(COMIMAGE_FLAGS_STRONGNAMESIGNED))
        eStrongNameLevel = FullySigned;
    else
        eStrongNameLevel = DelaySigned;

 Cleanup:

    // Cleanup all resources we used.
    if (pbBase)
        UnmapViewOfFile(pbBase);
    if (hMap)
        CloseHandle(hMap);

    return eStrongNameLevel;
}


// Verify a strongly named assembly for self consistency.
bool VerifyAssembly(__in_z LPWSTR szAssembly, bool bForceVerify)
{
    EnumStrongNameLevel eStrongNameLevel = IsStronglyNamedAssembly(szAssembly);
    if (eStrongNameLevel == Undetermined) {
        Output(SN_FAILED_READ, szAssembly, GetErrorString(GetLastError()));
        return false;
    }
    if (eStrongNameLevel == None) {
        Output(SN_NOT_STRONG_NAMED, szAssembly);
        return false;
    }

    if (!StrongNameSignatureVerificationEx(szAssembly, bForceVerify, NULL)) {
        if (eStrongNameLevel == DelaySigned)
            Output(SN_DELAY_SIGNED, szAssembly);
        else
            Output(SN_FAILED_VERIFY, GetErrorString(StrongNameErrorInfo(), szAssembly));
        return false;
    }

    if (!g_bQuiet) Output(SN_ASSEMBLY_VALID, szAssembly);

    return true;
}


// Generate a random public/private key pair and write it to a file.
bool GenerateKeyPair(__in_z LPWSTR szKeyFile)
{
    BYTE   *pbKey;
    DWORD   cbKey;
    DWORD   dwError;

    // Write a new public/private key pair to memory.
    if (!StrongNameKeyGen(NULL, 0, &pbKey, &cbKey)) {
        Output(SN_FAILED_KEYPAIR_GEN, GetErrorString(StrongNameErrorInfo()));
        return false;
    }

    // Persist the key pair to disk.
    if ((dwError = WriteFileFromBuffer(szKeyFile, pbKey, cbKey)) != ERROR_SUCCESS) {
        Output(SN_FAILED_CREATE, szKeyFile, GetErrorString(dwError));
        return false;
    }

    if (!g_bQuiet) Output(SN_KEYPAIR_WRITTEN, szKeyFile);

    return true;
}


// Generate a random public/private key pair with the specified size and write it to a file.
bool GenerateKeyPair(DWORD dwKeySize, __in_z LPWSTR szKeyFile)
{
    BYTE   *pbKey;
    DWORD   cbKey;
    DWORD   dwError;

    // Write a new public/private key pair to memory.
    if (!StrongNameKeyGenEx(NULL, 0, dwKeySize, &pbKey, &cbKey)) {
        Output(SN_FAILED_KEYPAIR_GEN, GetErrorString(StrongNameErrorInfo()));
        return false;
    }

    // Persist the key pair to disk.
    if ((dwError = WriteFileFromBuffer(szKeyFile, pbKey, cbKey)) != ERROR_SUCCESS) {
        Output(SN_FAILED_CREATE, szKeyFile, GetErrorString(dwError));
        return false;
    }

    if (!g_bQuiet) Output(SN_KEYPAIR_WRITTEN, szKeyFile);
        
    return true;
}


// Extract the public portion of a public/private key pair.
bool ExtractPublicKey(__in_z LPWSTR szInFile, __in_z LPWSTR szOutFile)
{
    BYTE   *pbKey;
    DWORD   cbKey;
    BYTE   *pbPublicKey;
    DWORD   cbPublicKey;
    DWORD   dwError;

    // Read the public/private key pair into memory.
    if ((dwError = ReadFileIntoBuffer(szInFile, &pbKey, &cbKey)) != ERROR_SUCCESS) {
        Output(SN_FAILED_READ, szInFile, GetErrorString(dwError));
        return false;
    }


    // Extract the public portion into a buffer.
    if (!StrongNameGetPublicKey(NULL, pbKey, cbKey, &pbPublicKey, &cbPublicKey)) {
        Output(SN_FAILED_EXTRACT, GetErrorString(StrongNameErrorInfo()));
        return false;
    }

    // Write the public portion to disk.
    if ((dwError = WriteFileFromBuffer(szOutFile, pbPublicKey, cbPublicKey)) != ERROR_SUCCESS) {
        Output(SN_FAILED_CREATE, szOutFile, GetErrorString(dwError));
        return false;
    }

    if (!g_bQuiet) Output(SN_PUBLICKEY_WRITTEN, szOutFile);
        
    return true;
}



// Display the token form of a public key read from a file.
bool DisplayTokenFromKey(__in_z LPWSTR szFile, BOOL bShowPublic)
{
    BYTE   *pbKey;
    DWORD   cbKey;
    BYTE   *pbToken;
    DWORD   cbToken;
    DWORD   dwError;

    // Read the public key from a file.
    if ((dwError = ReadFileIntoBuffer(szFile, &pbKey, &cbKey)) != ERROR_SUCCESS) {
        Output(SN_FAILED_READ, szFile, GetErrorString(dwError));
        return false;
    }

    // Convert the key into a token.
    if (!StrongNameTokenFromPublicKey(pbKey, cbKey, &pbToken, &cbToken)) {
        Output(SN_FAILED_CONVERT, GetErrorString(StrongNameErrorInfo()));
        return false;
    }

    // Display public key if requested.
    if (bShowPublic)
        Output(SN_PUBLICKEY, EOL, GetPublicKeyString(pbKey, cbKey));

    // And display it.
    Output(SN_PUBLICKEYTOKEN, GetTokenString(pbToken, cbToken));

    return true;
}


// Display the token form of a public key used to sign an assembly.
bool DisplayTokenFromAssembly(__in_z LPWSTR szAssembly, BOOL bShowPublic)
{
    BYTE   *pbToken;
    DWORD   cbToken;
    BYTE   *pbKey;
    DWORD   cbKey;

    EnumStrongNameLevel eStrongNameLevel = IsStronglyNamedAssembly(szAssembly);
    if (eStrongNameLevel == Undetermined) {
        Output(SN_FAILED_READ, szAssembly, GetErrorString(GetLastError()));
        return false;
    }
    if (eStrongNameLevel == None) {
        Output(SN_NOT_STRONG_NAMED, szAssembly);
        return false;
    }

    // Read the token direct from the assembly.
    if (!StrongNameTokenFromAssemblyEx(szAssembly, &pbToken, &cbToken, &pbKey, &cbKey)) {
        Output(SN_FAILED_READ_TOKEN, GetErrorString(StrongNameErrorInfo(), szAssembly));
        return false;
    }

    // Display public key if requested.
    if (bShowPublic)
        Output(SN_PUBLICKEY, EOL, GetPublicKeyString(pbKey, cbKey));

    // And display it.
    Output(SN_PUBLICKEYTOKEN, GetTokenString(pbToken, cbToken));

    return true;
}


// Write a public key to a file as a comma separated value list.
bool WriteCSV(__in_z LPWSTR szInFile, __in_z LPWSTR szOutFile)
{
    BYTE   *pbKey;
    DWORD   cbKey;
    DWORD   dwError;
    BYTE   *pbBuffer;
    DWORD   cbBuffer;
    DWORD   i;

    // Read the public key from a file.
    if ((dwError = ReadFileIntoBuffer(szInFile, &pbKey, &cbKey)) != ERROR_SUCCESS) {
        Output(SN_FAILED_READ, szInFile, GetErrorString(dwError));
        return false;
    }

    // Check for non-empty file.
    if (cbKey == 0) {
        Output(SN_EMPTY, szInFile);
        return false;
    }

    // Calculate the size of the text output buffer:
    //    Each byte -> 3 chars (space prefixed decimal) + 2 (", ")
    //  + 2 chars ("\r\n")
    //  - 2 chars (final ", " not needed)
    cbBuffer = (cbKey * (3 + 2)) + 2 - 2;

    // Allocate buffer (plus an extra couple of characters for the temporary
    // slop-over from writing a trailing ", " we don't need).
    pbBuffer = new BYTE[cbBuffer + 2];
    if (pbBuffer == NULL) {
        Output(SN_FAILED_ALLOCATE);
        return false;
    }

    // Convert the byte stream into a CSV (Comma Seperated Value) list.
    for (i = 0; i < cbKey; i++)
        sprintf_s((char*)&pbBuffer[i * 5], (cbBuffer+2-i*5), "% 3u, ", pbKey[i]);
    pbBuffer[cbBuffer - 2] = '\r';
    pbBuffer[cbBuffer - 1] = '\n';

    // If an output filename was provided write the CSV list to it.
    if (szOutFile) {
        if ((dwError = WriteFileFromBuffer(szOutFile, pbBuffer, cbBuffer)) != ERROR_SUCCESS) {
            Output(SN_FAILED_CREATE, szOutFile, GetErrorString(dwError));
            return false;
        }
        if (!g_bQuiet) Output(SN_PUBLICKEY_WRITTEN_CSV, szOutFile);
    } else {

        DWORD dwBytesWritten;
        if (!WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), pbBuffer, cbBuffer, &dwBytesWritten, NULL) != ERROR_SUCCESS) {
            return false;
        }

        if (!g_bQuiet) Output(SN_PUBLICKEY_WRITTEN_CSV, L"console");

    }

    return true;
}


// Extract the public key from an assembly and place it in a file.
bool ExtractPublicKeyFromAssembly(__in_z LPWSTR szAssembly, __in_z LPWSTR szFile)
{
    BYTE   *pbToken;
    DWORD   cbToken;
    BYTE   *pbKey;
    DWORD   cbKey;
    DWORD   dwError;

    EnumStrongNameLevel eStrongNameLevel = IsStronglyNamedAssembly(szAssembly);
    if (eStrongNameLevel == Undetermined) {
        Output(SN_FAILED_READ, szAssembly, GetErrorString(GetLastError()));
        return false;
    }
    if (eStrongNameLevel == None) {
        Output(SN_NOT_STRONG_NAMED, szAssembly);
        return false;
    }

    // Read the public key from the assembly.
    if (!StrongNameTokenFromAssemblyEx(szAssembly, &pbToken, &cbToken, &pbKey, &cbKey)) {
        Output(SN_FAILED_READ_TOKEN, GetErrorString(StrongNameErrorInfo(), szAssembly));
        return false;
    }

    // And write it to disk.
    if ((dwError = WriteFileFromBuffer(szFile, pbKey, cbKey)) != ERROR_SUCCESS) {
        Output(SN_FAILED_CREATE, szFile, GetErrorString(dwError));
        return false;
    }

    if (!g_bQuiet) Output(SN_PUBLICKEY_EXTRACTED, szFile);

    return true;
}


// Check that two assemblies differ only by their strong name signature.
bool DiffAssemblies(__in_z LPWSTR szAssembly1, __in_z LPWSTR szAssembly2)
{
    DWORD   dwResult;

    EnumStrongNameLevel eStrongNameLevel1 = IsStronglyNamedAssembly(szAssembly1);
    if (eStrongNameLevel1 == Undetermined) {
        Output(SN_FAILED_READ, szAssembly1, GetErrorString(GetLastError()));
        return false;
    }
    if (eStrongNameLevel1 == None) {
        Output(SN_NOT_STRONG_NAMED, szAssembly1);
        return false;
    }

    EnumStrongNameLevel eStrongNameLevel2 = IsStronglyNamedAssembly(szAssembly2);
    if (eStrongNameLevel2 == Undetermined) {
        Output(SN_FAILED_READ, szAssembly2, GetErrorString(GetLastError()));
        return false;
    }
    if (eStrongNameLevel2 == None) {
        Output(SN_NOT_STRONG_NAMED, szAssembly2);
        return false;
    }

    // Compare the assembly images.
    if (!StrongNameCompareAssemblies(szAssembly1, szAssembly2, &dwResult)) {
        Output(SN_FAILED_COMPARE, GetErrorString(StrongNameErrorInfo()));
        return false;
    }

    // Print a message describing how similar they are.
    if (!g_bQuiet)
        switch (dwResult) {
        case SN_CMP_DIFFERENT:
            Output(SN_DIFFER_MORE);
            break;
        case SN_CMP_IDENTICAL:
            Output(SN_DIFFER_NOT);
            break;
        case SN_CMP_SIGONLY:
            Output(SN_DIFFER_ONLY);
            break;
        default:
            Output(SN_INTERNAL_1, dwResult);
            return false;
        }

    // Return a failure code on differences.
    return dwResult != SN_CMP_DIFFERENT;
}

// Re-hash the linked modules in a multi-module assembly.
bool RehashModules(__in_z LPWSTR szAssembly)
{
    // Recompute the hashes of linked modules in the assembly file.
    if (!StrongNameSignatureGenerationEx(szAssembly, NULL,
                                         NULL, NULL, NULL, NULL,
                                         SN_SIGN_ALL_FILES)) {
        Output(SN_FAILED_REHASH, GetErrorString(StrongNameErrorInfo(), szAssembly));
        return false;
    }

    if (!g_bQuiet) Output(SN_ASSEMBLY_REHASHED, szAssembly);

    return true;
}

// Re-sign/Test-sign a previously signed assembly with a key pair from a file or a key container.
bool ResignAssembly(__in_z LPWSTR szAssembly, __in_z LPWSTR szFileOrContainer, bool bContainer, bool bAllFiles, bool bTestSign)
{
    LPWSTR  szContainer = NULL;
    BYTE   *pbKey = NULL;
    DWORD   cbKey = NULL;
    BYTE   *pbPublicKey = NULL;
    DWORD   cbPublicKey;
    BYTE   *pbPublicKeyAsm = NULL;
    DWORD   cbPublicKeyAsm;
    BYTE   *pbToken = NULL;
    DWORD   cbToken;
    DWORD   dwError;

    EnumStrongNameLevel eStrongNameLevel = IsStronglyNamedAssembly(szAssembly);
    if (eStrongNameLevel == Undetermined) {
        Output(SN_FAILED_READ, szAssembly, GetErrorString(GetLastError()));
        return false;
    }
    if (eStrongNameLevel == None) {
        Output(SN_NOT_STRONG_NAMED, szAssembly);
        return false;
    }
    if (bTestSign) {
        if (eStrongNameLevel == FullySigned) {
            Output(SN_CANNOT_TESTSIGN_STRONG_NAMED);
            return false;
        }
    }

    if (bContainer) {
        // Key is provided in container.
        szContainer = szFileOrContainer;

        // Extract the public portion into a buffer.
        if (!StrongNameGetPublicKey(szContainer, NULL, 0, &pbPublicKey, &cbPublicKey)) {
            Output(SN_FAILED_EXTRACT, GetErrorString(StrongNameErrorInfo()));
            return false;
        }
    } else {
        // Key is provided in file.

        // Read the public/private key pair into memory.
        if ((dwError = ReadFileIntoBuffer(szFileOrContainer, &pbKey, &cbKey)) != ERROR_SUCCESS) {
            Output(SN_FAILED_READ, szFileOrContainer, GetErrorString(dwError));
            return false;
        }


        // Extract the public portion into a buffer.
        if (!StrongNameGetPublicKey(NULL, pbKey, cbKey, &pbPublicKey, &cbPublicKey)) {
            Output(SN_FAILED_EXTRACT, GetErrorString(StrongNameErrorInfo()));
            return false;
        }
    }

    // substitute the neutral key if needed here.
    if (cbPublicKey == sizeof(g_rbTheKey) && memcmp(pbPublicKey, g_rbTheKey, sizeof(g_rbTheKey)) == 0) {
        pbPublicKey = (BYTE*) g_rbNeutralPublicKey;
        cbPublicKey = sizeof(g_rbNeutralPublicKey);
    }

    // Read the public key from the assembly.
    if (!StrongNameTokenFromAssemblyEx(szAssembly, &pbToken, &cbToken, &pbPublicKeyAsm, &cbPublicKeyAsm)) {
        Output(SN_FAILED_READ_TOKEN, GetErrorString(StrongNameErrorInfo(), szAssembly));
        return false;
    }

    // substitute the neutral key if needed here.
    if (cbPublicKeyAsm == sizeof(g_rbTheKey) && memcmp(pbPublicKeyAsm, g_rbTheKey, sizeof(g_rbTheKey)) == 0) {
        pbPublicKeyAsm = (BYTE*) g_rbNeutralPublicKey;
        cbPublicKeyAsm = sizeof(g_rbNeutralPublicKey);
    }

    // Compare the public key read from the assembly to the one used to re-sign the assembly
    if (bTestSign) {
        // special case the ECMA special strong name key.
        if (memcmp(pbPublicKeyAsm, g_rbNeutralPublicKey, sizeof(g_rbNeutralPublicKey)) == 0) {
            if (cbPublicKey != sizeof(g_rbNeutralPublicKey) && cbPublicKey != sizeof(g_rbTheKey)) {
                Output(SN_INVALID_PUBLICKEY, szAssembly);
                return false;
            }
        }
        else {
            if (cbPublicKeyAsm != cbPublicKey) {
                Output(SN_INVALID_PUBLICKEY, szAssembly);
                return false;
            }
        }
    }
    else if ((cbPublicKeyAsm != cbPublicKey) || ((memcmp(pbPublicKey, pbPublicKeyAsm, cbPublicKey) != 0))) {
        Output(SN_INVALID_PUBLICKEY, szAssembly);
        return false;
    }

    // Recompute the signature in the assembly file.
    DWORD dwFlags = bAllFiles ? SN_SIGN_ALL_FILES : 0;
    if (bTestSign)
        dwFlags |= SN_TEST_SIGN;
    if (!StrongNameSignatureGenerationEx(szAssembly, szContainer, pbKey, cbKey, NULL, NULL, dwFlags)) {
        Output(bTestSign ? SN_FAILED_TESTSIGN : SN_FAILED_RESIGN, GetErrorString(StrongNameErrorInfo(), szAssembly));
        return false;
    }

    if (!g_bQuiet) Output(SN_ASSEMBLY_RESIGNED, szAssembly);

    return true;
}

// Build a name for an assembly that includes the internal name and a hex
// representation of the strong name (public key).
LPWSTR GetAssemblyName(__in_z LPWSTR szAssembly)
{
    HRESULT                     hr;
    IMetaDataDispenser         *pDisp;
    IMetaDataAssemblyImport    *pAsmImport;
    mdAssembly                  tkAssembly;
    BYTE                       *pbKey;
    DWORD                       cbKey;
    static WCHAR                szAssemblyName[1024];
    WCHAR                       szStrongName[1024];
    BYTE                       *pbToken;
    DWORD                       cbToken;
    DWORD                       i;


    if (FAILED(hr = CoCreateInstance(CLSID_CorMetaDataDispenser,
                                     NULL,
                                     CLSCTX_INPROC_SERVER, 
                                     IID_IMetaDataDispenser,
                                     (void**)&pDisp))) {
        Output(SN_FAILED_MD_ACCESS, GetErrorString(hr));
        return NULL;
    }

    // Open a scope on the file.
    if (FAILED(hr = pDisp->OpenScope(szAssembly,
                                     0,
                                     IID_IMetaDataAssemblyImport,
                                     (IUnknown**)&pAsmImport))) {
        Output(SN_FAILED_MD_OPEN, szAssembly, GetErrorString(hr));
        return NULL;
    }

    // Determine the assemblydef token.
    if (FAILED(hr = pAsmImport->GetAssemblyFromScope(&tkAssembly))) {
        Output(SN_FAILED_MD_ASSEMBLY, szAssembly, GetErrorString(hr));
        return NULL;
    }

    // Read the assemblydef properties to get the public key and name.
    if (FAILED(hr = pAsmImport->GetAssemblyProps(tkAssembly,
                                                 (const void **)&pbKey,
                                                 &cbKey,
                                                 NULL,
                                                 szAssemblyName,
                                                 sizeof(szAssemblyName) / sizeof(WCHAR),
                                                 NULL,
                                                 NULL,
                                                 NULL))) {
        Output(SN_FAILED_STRONGNAME, szAssembly, GetErrorString(hr));
        return NULL;
    }

    // Check for strong name.
    if ((pbKey == NULL) || (cbKey == 0)) {
        Output(SN_NOT_STRONG_NAMED, szAssembly);
        return NULL;
    }

    // Compress the strong name down to a token.
    if (!StrongNameTokenFromPublicKey(pbKey, cbKey, &pbToken, &cbToken)) {
        Output(SN_FAILED_CONVERT, GetErrorString(StrongNameErrorInfo(), szAssembly));
        return NULL;
    }

    _ASSERTE(cbToken == 8);
    // Turn the token into hex.
    for (i = 0; i < cbToken; i++)
        _snwprintf_s(&szStrongName[i * 2], _countof(szStrongName)-(i*2), _TRUNCATE, L"%02X", pbToken[i]);

    // Build the name (in a static buffer).
    wcscat_s(szAssemblyName, _countof(szAssemblyName), L",");
    wcscat_s(szAssemblyName, _countof(szAssemblyName), szStrongName);

    StrongNameFreeBuffer(pbToken);
    pAsmImport->Release();
    pDisp->Release();


    return szAssemblyName;
}


BOOL CheckStrongName(__in_ecount(length) WCHAR* pwz, DWORD length)
{
    while (length--)
    {
        WCHAR ch = *pwz++;
        if (((ch < '0') || (ch > '9')) &&
            ((ch < 'A') || (ch > 'F')) &&
            ((ch < 'a') || (ch > 'f')))
            return FALSE;
    }

    return TRUE;
}

// Parse an assembly name handed to a register/unregister
// verification skipping function. The input name can be "*" for all assemblies,
// "*,<hex digits>" for all assemblies with a given strong name or the filename
// of a specific assembly. The output is a string of the form:
// "<simple name>,<hex strong name>" where the first or both fields can be
// wildcarded with "*", and the hex strong name is a hexidecimal representation
// of the public key token (as we expect in the "*,<hex digits>" input form).
LPWSTR ParseAssemblyName(__in_z LPWSTR szAssembly)
{
    if ((wcscmp(L"*", szAssembly) == 0) ||
        (wcscmp(L"*,*", szAssembly) == 0))
        return L"*,*";
    LPWSTR pszComma = wcschr(szAssembly, L',');
    if (pszComma && (pszComma != szAssembly)) {
        DWORD i = (DWORD)wcslen(pszComma) - 1;
        if ((i == 0) || (i & 1) || !CheckStrongName(&pszComma[1], i)) {
            Output(SN_INVALID_SVR_FORMAT);
            return NULL;
        }
        return szAssembly;
    } else
        return GetAssemblyName(szAssembly);
}


#define CONFIG_FILE_NAME   L"strongname.ini"
#define DIRECTORYSEPERATOR L"\\"
#define DIRECTORYSEPERATORLENGTH 1

static BOOL GetStrongNameConfigPath(__inout_z WCHAR* lpFileName, DWORD cbFileName)
{
    if (!PAL_GetMachineConfigurationDirectoryW(lpFileName, cbFileName))
        return FALSE;

        if (wcslen(lpFileName) + sizeof(CONFIG_FILE_NAME)/sizeof(WCHAR) + DIRECTORYSEPERATORLENGTH >= 
        cbFileName) {
        return FALSE;
    }

    wcscat(lpFileName, DIRECTORYSEPERATOR);
    wcscat(lpFileName, CONFIG_FILE_NAME);

    return TRUE;
}

static DWORD PAL_IniReadStringEx(HINI hIni, 
             LPCWSTR pszSection, 
             LPCWSTR pszKey, 
             __out LPWSTR* ppszValue)
{
    DWORD       dwRet;
    LPWSTR      pwzBuf = NULL;
    DWORD       dwSize = 128;

    for (;;) {
        pwzBuf = new WCHAR[dwSize];
        if (!pwzBuf) {
            *ppszValue = NULL;
            return 0;
        }
    
        dwRet = PAL_IniReadString(hIni, pszSection, pszKey, 
                                        pwzBuf, dwSize);
        if ( (pszSection && pszKey && dwRet == dwSize - 1) ||
            ((!pszSection || !pszKey) && dwRet == dwSize - 2) )
        {
            delete [] pwzBuf;
            dwSize *= 2;
            continue;
        }

        if (dwRet == 0)
        {
            delete [] pwzBuf;
            *ppszValue = NULL;
            return 0;
        }

        break;
    }

    *ppszValue = pwzBuf;
    return dwRet;
}

// Register an assembly for verification skipping.
bool VerifyRegister(__in_z LPWSTR szAssembly, __in_opt __in_z LPWSTR szUserlist, BYTE* pbPublicKey, DWORD cbPublicKey)
{
    LPWSTR      szAssemblyName;
    WCHAR       szSubKey[MAX_PATH + 1];
    WCHAR       wzConfigPath[MAX_PATH+1];
    HINI        hIni = NULL;
    WCHAR      *pEntries = NULL;
    WCHAR      *p;
    int         c = 1;
    WCHAR       szKey[20];

    // Get the internal name for the assembly (possibly containing wildcards).
    szAssemblyName = ParseAssemblyName(szAssembly);
    if (szAssemblyName == NULL)
        return false;

    if (!GetStrongNameConfigPath(wzConfigPath, MAX_PATH+1))
        goto LError;

    hIni = PAL_IniCreate();
    if (!hIni)
        goto LError;

    PAL_IniLoad(hIni, wzConfigPath);

    // Blow away any old user list

    if (PAL_IniReadStringEx(hIni, SN_CONFIG_VERIFICATION_W, NULL, &pEntries))
    {
        p = pEntries;

        while (*p != '\0')
        {
            if (PAL_IniReadString(hIni, SN_CONFIG_VERIFICATION_W, p, szSubKey, MAX_PATH + 1))
            {
                if (wcscmp(szSubKey, szAssemblyName) == 0) {
                    PAL_IniWriteString(hIni, SN_CONFIG_VERIFICATION_W, p, NULL);
                    PAL_IniWriteString(hIni, SN_CONFIG_USERLIST_W, p, NULL);
                }
            }

            p += wcslen(p) + 1;
        }
    }

    // find a unused key

    for (;;)
    {
        _snwprintf(szKey, NumItems(szKey), L"_%d", c);
        if (PAL_IniReadString(hIni, SN_CONFIG_VERIFICATION_W, szKey, szSubKey, MAX_PATH + 1) == 0)
            break;
        c++;
    }

    PAL_IniWriteString(hIni, SN_CONFIG_VERIFICATION_W, szKey, szAssemblyName);

    // If a test public key is provided, persist it.
    if (pbPublicKey) {
        LPWSTR szPublicKey = GetPublicKeyString(pbPublicKey, cbPublicKey, false);
        PAL_IniWriteString(hIni, SN_CONFIG_TESTPUBLICKEY_W, szKey, szPublicKey);
    }
    else {
        PAL_IniWriteString(hIni, SN_CONFIG_TESTPUBLICKEY_W, szKey, NULL);
    }

    if (szUserlist && (wcscmp(L"*", szUserlist) != 0)) {
        PAL_IniWriteString(hIni, SN_CONFIG_USERLIST_W, szKey, szUserlist);
    }
    else {
        PAL_IniWriteString(hIni, SN_CONFIG_USERLIST_W, szKey, NULL);
    }

    if (!PAL_IniSave(hIni, wzConfigPath, TRUE)) {
        Output(SN_FAILED_REG_WRITE, GetErrorString(GetLastError()));
        goto LError;
    }

    PAL_IniClose(hIni);
    delete [] pEntries;

    if (!g_bQuiet) Output(SN_SVR_ADDED, szAssemblyName);

    return true;

LError:
    PAL_IniClose(hIni);
    delete [] pEntries;
    return false;
}


// Unregister an assembly for verification skipping.
bool VerifyUnregister(__in_z LPWSTR szAssembly)
{
    LPWSTR      szAssemblyName;
    WCHAR       szSubKey[MAX_PATH + 1];
    WCHAR       wzConfigPath[MAX_PATH+1];
    HINI        hIni = NULL;
    WCHAR      *pEntries = NULL;
    WCHAR      *p;

    // Get the internal name for the assembly (possibly containing wildcards).
    szAssemblyName = ParseAssemblyName(szAssembly);
    if (szAssemblyName == NULL)
        return false;

    if (!GetStrongNameConfigPath(wzConfigPath, MAX_PATH+1))
        goto LError;

    hIni = PAL_IniCreate();
    if (!hIni)
        goto LError;

    PAL_IniLoad(hIni, wzConfigPath);

    if (PAL_IniReadStringEx(hIni, SN_CONFIG_VERIFICATION_W, NULL, &pEntries))
    {
        p = pEntries;

        while (*p != '\0')
        {
            if (PAL_IniReadString(hIni, SN_CONFIG_VERIFICATION_W, p, szSubKey, MAX_PATH + 1))
            {
                if (wcscmp(szSubKey, szAssemblyName) == 0) {
                    PAL_IniWriteString(hIni, SN_CONFIG_VERIFICATION_W, p, NULL);
                    PAL_IniWriteString(hIni, SN_CONFIG_USERLIST_W, p, NULL);
                }
            }

            p += wcslen(p) + 1;
        }
    }

    if (!PAL_IniSave(hIni, wzConfigPath, TRUE)) {
        Output(SN_FAILED_REG_WRITE, GetErrorString(GetLastError()));
        goto LError;
    }

    PAL_IniClose(hIni);
    delete [] pEntries;

    if (!g_bQuiet) Output(SN_SVR_REMOVED, szAssemblyName);

    return true;

LError:
    if (hIni != NULL)
        PAL_IniClose(hIni);
    delete [] pEntries;
    return false;
}

// Unregister all verification skipping entries.
bool VerifyUnregisterAll()
{
    WCHAR   wzConfigPath[MAX_PATH+1];
    HINI    hIni = NULL;

    if (!GetStrongNameConfigPath(wzConfigPath, MAX_PATH+1))
        goto LError;

    hIni = PAL_IniCreate();
    if (!hIni)
        goto LError;

    PAL_IniLoad(hIni, wzConfigPath);

    if (!PAL_IniWriteString(hIni, SN_CONFIG_VERIFICATION_W, NULL, NULL))
        goto LError;

    if (!PAL_IniWriteString(hIni, SN_CONFIG_USERLIST_W, NULL, NULL))
        goto LError;

    if (!PAL_IniSave(hIni, wzConfigPath, TRUE)) {
        Output(SN_FAILED_REG_WRITE, GetErrorString(GetLastError()));
        goto LError;
    }

    PAL_IniClose(hIni);

    if (!g_bQuiet) Output(SN_SVR_ALL_REMOVED);

    return true;

LError:
    if (hIni != NULL)
        PAL_IniClose(hIni);
    return false;
}

// List state of verification on this machine.
bool VerifyList()
{
    DWORD           j;
    DWORD           dwEntries;
    WCHAR           szSubKey[MAX_PATH + 1];
    WCHAR          *mszUserList;
    WCHAR          *szUser;
    WCHAR           wzConfigPath[MAX_PATH+1];
    HINI            hIni = NULL;
    WCHAR          *pEntries = NULL;
    WCHAR          *p;
    DWORD           cchPad;
    LPWSTR          szPad;
    WCHAR          *szTestPublicKey;

    // Count entries we find.
    dwEntries = 0;

    if (!GetStrongNameConfigPath(wzConfigPath, MAX_PATH+1))
        goto LError;

    hIni = PAL_IniCreate();
    if (!hIni)
        goto LError;

    PAL_IniLoad(hIni, wzConfigPath);

    if (!PAL_IniReadStringEx(hIni, SN_CONFIG_VERIFICATION_W, NULL, &pEntries))
        goto Finished;

    p = pEntries;

    while (*p != '\0')
    {
        if (PAL_IniReadString(hIni, SN_CONFIG_VERIFICATION_W, p, szSubKey, MAX_PATH + 1))
        {
            dwEntries++;

            if (dwEntries == 1) {
                Output(SN_SVR_TITLE_1);
                Output(SN_SVR_TITLE_2);
            }

            if (wcslen(szSubKey) < 38) {
                cchPad = 38 - wcslen(szSubKey);
                szPad = (LPWSTR)_alloca((cchPad + 1) * sizeof(WCHAR));
                memset(szPad, 0, (cchPad + 1) * sizeof(WCHAR));
                for (j = 0; j < cchPad; j++)
                    szPad[j] = L' ';
                OutputString(L"%s%s", szSubKey, szPad);
            } else
                OutputString(L"%s ", szSubKey);

            // Read a list of valid users, if supplied.
            mszUserList = NULL;
            if (PAL_IniReadStringEx(hIni, SN_CONFIG_USERLIST_W, p, &mszUserList)) {

                szUser = mszUserList;
                while (szUser) {
                    WCHAR* szEnd = wcschr(szUser, ',');
                    if (szEnd) 
                        *szEnd = '\0';
                    OutputString(L"%s ", szUser);
                    szUser = szEnd ? (szEnd + 1) : NULL;
                }
                OutputString(EOL);

                delete [] mszUserList;
            } else
                Output(SN_ALL_USERS);

            // Read the test public key, if supplied
            szTestPublicKey = NULL;
            if (PAL_IniReadStringEx(hIni, SN_CONFIG_TESTPUBLICKEY_W, p, &szTestPublicKey)) {
                // Display the test public key
                Output(SN_SVR_TITLE_3);
                OutputString(L"%s", szTestPublicKey);
                OutputString(EOL);
            }
        }

        p += wcslen(p) + 1;
    }

Finished:
    PAL_IniClose(hIni);
    delete [] pEntries;

    if (!g_bQuiet && (dwEntries == 0))
        Output(SN_NO_SVR);

    return true;

LError:
    if (hIni != NULL)
        PAL_IniClose(hIni);
    delete [] pEntries;
    return false;
}


// Check that a given command line option has been given the right number of arguments.
#define OPT_CHECK(_opt, _min, _max) do {                                                                \
    if (wcscmp(L##_opt, &argv[1][1])) {                                                                 \
        Output(SN_INVALID_OPTION, argv[1]);                                                             \
        Usage();                                                                                        \
        return 1;                                                                                       \
    } else if ((argc > ((_max) + 2)) && (argv[2 + (_max)][0] == '-' || argv[2 + (_max)][0] == '/')) {   \
        Output(SN_OPT_ONLY_ONE);                                                                        \
        return 1;                                                                                       \
    } else if ((argc < ((_min) + 2)) || (argc > ((_max) + 2))) {                                        \
        if ((_min) == (_max)) {                                                                         \
            if ((_min) == 0)                                                                            \
                Output(SN_OPT_NO_ARGS, (L##_opt));                                                      \
            else if ((_min) == 1)                                                                       \
                Output(SN_OPT_ONE_ARG, (L##_opt));                                                      \
            else                                                                                        \
                Output(SN_OPT_N_ARGS, (L##_opt), (_min));                                               \
        } else                                                                                          \
            Output(SN_OPT_ARG_RANGE, (L##_opt), (_min), (_max));                                        \
        Usage();                                                                                        \
        return 1;                                                                                       \
    }                                                                                                   \
} while (0)


extern "C" int _cdecl wmain(int argc, WCHAR **argv)
{
    bool bResult;
    BYTE* pbPublicKey = NULL;
    DWORD cbPublicKey = 0;

    // Initialize Wsz wrappers.
    OnUnicodeSystem();

    if (!PAL_RegisterLibrary(L"rotor_palrt") ||
        !PAL_RegisterLibrary(L"sscoree"))
    {
        fprintf(stderr, "Unable to register libraries\n");
        return 1;
    }
    
    // Check for quiet mode.
    if ((argc > 1) &&
        ((argv[1][0] == L'-') || (argv[1][0] == L'/'))) {
        if (wcscmp(L"q", &argv[1][1]) == 0) {
            g_bQuiet = true;
            argc--;
            argv = &argv[1];
        }
    }

    if (!g_bQuiet)
        Title();

    if ((argc < 2) || ((argv[1][0] != L'-') && (argv[1][0] != L'/'))) {
        Usage();
        return 0;
    }

    switch (argv[1][1]) {
    case 'v':
        if (argv[1][2] == L'f') {
            OPT_CHECK("vf", 1, 1);
            bResult = VerifyAssembly(argv[2], true);
        } else {
            OPT_CHECK("v", 1, 1);
            bResult = VerifyAssembly(argv[2], false);
        }
        break;
    case 'k':
        OPT_CHECK("k", 1, 2);
        if (argc > 3 && !_wtoi(argv[2])) {
            Usage();
            bResult = false;
            break;
        }
        bResult = argc > 3 ? GenerateKeyPair(_wtoi(argv[2]), argv[3]) : GenerateKeyPair(argv[2]);
        break;

    case 'p':
        {
            OPT_CHECK("p", 2, 2);
            bResult = ExtractPublicKey(argv[2], argv[3]);
        }
        break;
    case 'V':
        switch (argv[1][2]) {
        case 'l':
            OPT_CHECK("Vl", 0, 0);
            bResult = VerifyList();
            break;
        case 'r':
            OPT_CHECK("Vr", 1, 3);
            if (argc == 3) {
                bResult = VerifyRegister(argv[2], NULL, NULL, NULL);
                break;
            }

            ReadFileIntoBuffer(argv[argc - 1], &pbPublicKey, &cbPublicKey);
            if (argc == 5)
                bResult = VerifyRegister(argv[2], argv[3], pbPublicKey, cbPublicKey);
            else
                bResult = VerifyRegister(argv[2], pbPublicKey ? NULL : argv[3], pbPublicKey, cbPublicKey);

            break;
        case 'u':
            OPT_CHECK("Vu", 1, 1);
            bResult = VerifyUnregister(argv[2]);
            break;
        case 'x':
            OPT_CHECK("Vx", 0, 0);
            bResult = VerifyUnregisterAll();
            break;
        default:
            Output(SN_INVALID_V_OPT, argv[1]);
            Usage();
            return 1;
        }
        break;
    case 't':
        if (argv[1][2] == L'p') {
            OPT_CHECK("tp", 1, 1);
            bResult = DisplayTokenFromKey(argv[2], true);
        } else {
            OPT_CHECK("t", 1, 1);
            bResult = DisplayTokenFromKey(argv[2], false);
        }
        break;
    case 'T':
        switch (argv[1][2]) {
        case 'S':
            {
                OPT_CHECK("TS", 2, 2);
                bResult = ResignAssembly(argv[2], argv[3], false, false, true);
            }
            break;
        case 'p':
            OPT_CHECK("Tp", 1, 1);
            bResult = DisplayTokenFromAssembly(argv[2], true);
            break;
        default:
            OPT_CHECK("T", 1, 1);
            bResult = DisplayTokenFromAssembly(argv[2], false);
            break;
        }
        break;
    case 'e':
        OPT_CHECK("e", 2, 2);
        bResult = ExtractPublicKeyFromAssembly(argv[2], argv[3]);
        break;
    case 'o':
        OPT_CHECK("o", 1, 2);
        bResult = WriteCSV(argv[2], argc > 3 ? argv[3] : NULL);
        break;
    case 'D':
        OPT_CHECK("D", 2, 2);
        bResult = DiffAssemblies(argv[2], argv[3]);
        break;
    case 'R':
        if (argv[1][2] == L'h') {
            OPT_CHECK("Rh", 1, 1);
            bResult = RehashModules(argv[2]);
        }
        else {
            if (argv[1][2] == L'a') {
                OPT_CHECK("Ra", 2, 2);
                bResult = ResignAssembly(argv[2], argv[3], false, true, false);
            } else {
                OPT_CHECK("R", 2, 2);
                bResult = ResignAssembly(argv[2], argv[3], false, false, false);
            }
        }
        break;
    case '?':
    case 'h':
    case 'H':
        Usage();
        bResult = true;
        break;
    default:
        Output(SN_INVALID_OPTION, &argv[1][1]);
        bResult = false;
    }

    return bResult ? 0 : 1;
}

HINSTANCE GetModuleInst()
{
    return (NULL);
}
