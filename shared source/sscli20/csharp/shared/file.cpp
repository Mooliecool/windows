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
////////////////////////////////////////////////////////////////////////////////
// FILE.CPP
//
// Contains common code for file manipulations (i.e. concatinating filenames, opening text files, manipulating paths)
// this file is 'shared' with ALink and C# so make sure to integrate
// any changes into both files

#include "pch.h"
#include "file_can.h"

#ifdef PLATFORM_UNIX
#define ENVIRONMENT_SEPARATOR L':'
#else   // PLATFORM_UNIX
#define ENVIRONMENT_SEPARATOR L';'
#endif  // PLATFORM_UNIX


size_t SafeStrLower( LPCWSTR wszSrc, size_t cchSrc, WCBuffer & destBuffer)
{
    if (cchSrc == (size_t)-1)
        cchSrc = wcslen(wszSrc);

    if (cchSrc >= destBuffer.Count()) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return 0;
    }

    LPWSTR wszDestEnd = ToLowerCase( wszSrc, destBuffer.GetData(), cchSrc);
    wszDestEnd[0] = L'\0';
    return (size_t)(wszDestEnd - destBuffer);
}

size_t SafeStrCopy( LPCWSTR wszSrc, size_t cchSrc, WCBuffer & destBuffer)
{
    if (cchSrc == (size_t)-1)
        cchSrc = wcslen(wszSrc);

    if (cchSrc >= destBuffer.Count()) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return 0;
    }

    if (FAILED(StringCchCopyNW( destBuffer.GetData(), destBuffer.Count(), wszSrc, cchSrc))) {
        VSFAIL("How did this fail?  We already checked that the buffer was big enough!");
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return 0;
    }
    ASSERT(destBuffer[(int)cchSrc] == L'\0');
    return cchSrc;
}

bool ProcessSlashes(WCBuffer & outBuffer, LPCWSTR * pszCur)
{
                // All this weird slash stuff follows the standard argument processing routines
    size_t iSlash = 0;
    LPCWSTR pCur = *pszCur;
    bool fIsQuoted = false;

    while (*pCur == L'\\')
        iSlash++, pCur++;

    if (*pCur == L'\"') {
        // Slashes followed by a quote character
        // put one slash in the output for every 2 slashes in the input
        for (; iSlash >= 2; iSlash -= 2)
            outBuffer.AddAndIncrement(L'\\');

        // If there's 1 remaining slash, it's escaping the quote
        // so ignore the slash and keep the quote (as a normal character)
        if (iSlash & 1) { // Is it odd?
            outBuffer.AddAndIncrement(*pCur++);
        } else {
            // A regular quote, so eat it and change the bQuoted
            pCur++;
            fIsQuoted = true;
        }
    } else {
        // Slashs not followed by a quote are just slashes
        for (; iSlash > 0; iSlash--)
            outBuffer.AddAndIncrement(L'\\');
    }

    *pszCur = pCur;
    return fIsQuoted;
}


// Remove quote marks from a string
// The translation is done in-place, and the argument is returned.
LPWSTR RemoveQuotes(WCBuffer textBuffer)
{

    if (!textBuffer.IsValid()) return NULL;

    LPCWSTR pIn;
    LPWSTR input = textBuffer.GetData();
    WCHAR ch;

    pIn = input;
    for (;;) {
        switch (ch = *pIn) {
        case L'\0':
            // End of string. We're done.
            textBuffer.SetAt(0, L'\0');
            return input;

        case L'\\':
            ProcessSlashes(textBuffer, &pIn);
            // Not break because ProcessSlashes has already advanced pIn
            continue;

        case L'\"': 
            break;
        default:
            textBuffer.AddAndIncrement(ch);
            break;
        }

        ++pIn;
    } 
}

// Remove quote marks from a string. Also, commas (,) that are not quoted
// are converted to the pipe (|) character.
// The translation is done in-place, and the argument is returned.
LPWSTR RemoveQuotesAndReplaceComma(WCBuffer textBuffer)
{
    if (!textBuffer.IsValid()) return NULL;

    LPCWSTR pIn;
    LPWSTR input = textBuffer.GetData();

    WCHAR ch;
    bool inQuote;

    pIn = input;
    inQuote = false;
    for (;;) {
        switch (ch = *pIn) {
        case L'\0':
            // End of string. We're done.
            textBuffer.SetAt(0, L'\0');
            return input;

        case L'\"': 
            inQuote = !inQuote;
            break;

        case L'|':
            VSFAIL("How did we get this here!");
            __assume(0);
            goto DEFAULT;

        case L'\\':
            if (ProcessSlashes( textBuffer, &pIn))
                inQuote = !inQuote;
            // Not break because ProcessSlashes has already advanced pIn
            continue;

        case L',':
            if (inQuote)
                goto DEFAULT;
            textBuffer.AddAndIncrement(L'|');
            break;

        default:
DEFAULT:
            textBuffer.AddAndIncrement(ch);
            break;
        }

        ++pIn;
    } 
}

// Remove quote marks from a string. Also, commas (,) and semicolons (;)
// that are not quoted are converted to the pipe (|) character.
// The translation is done in-place, and the argument is returned.
LPWSTR RemoveQuotesAndReplacePathDelim(WCBuffer textBuffer)
{
    if (!textBuffer.IsValid()) return NULL;

    LPCWSTR pIn;
    LPWSTR input = textBuffer.GetData();

    WCHAR ch;
    bool inQuote;

    pIn = input;
    inQuote = false;
    for (;;) {
        switch (ch = *pIn) {
        case L'\0':
            // End of string. We're done.
            textBuffer.SetAt(0, L'\0');
            return input;


        case L'\\':
            if (ProcessSlashes( textBuffer, &pIn))
                inQuote = !inQuote;
            // Not break because ProcessSlashes has already advanced pIn
            continue;

        case L'\"': 
            inQuote = !inQuote;
            break;

        case L'|':
            VSFAIL("How did we get this here!");
            goto DEFAULT;

        case L',':
        case ENVIRONMENT_SEPARATOR:
            if (inQuote)
                goto DEFAULT;
            textBuffer.AddAndIncrement(L'|');
            break;

        default:
DEFAULT:
            textBuffer.AddAndIncrement(ch);
            break;
        }

        ++pIn;
    } 
}

// Remove quote marks from a string. Also, commas (,), semicolons (;), and equals (=)
// that are not quoted are converted to the pipe (|) or angle ('\x01') character.
// The translation is done in-place, and the argument is returned.
LPWSTR RemoveQuotesAndReplaceAlias(WCBuffer textBuffer)
{
    if (!textBuffer.IsValid()) return NULL;

    LPCWSTR pIn;
    LPWSTR input = textBuffer.GetData();

    WCHAR ch;
    bool inQuote;
    bool fSeenEquals;

    pIn = input;
    inQuote = false;
    fSeenEquals = false;
    for (;;) {
        switch (ch = *pIn) {
        case L'\0':
            // End of string. We're done.
            textBuffer.SetAt(0, L'\0');
            return input;

        case L'\"': 
            inQuote = !inQuote;
            break;

        case L'\x01':
        case L'|':
            VSFAIL("How did we get this here!");
            goto DEFAULT;


        case L',':
        case L';':
            if (inQuote)
                goto DEFAULT;
            textBuffer.AddAndIncrement(L'|');
            fSeenEquals = false;
            break;

        case L'\\':
            if (ProcessSlashes(textBuffer, &pIn))
                inQuote = !inQuote;
            // Not break because ProcessSlashes has already advanced pIn
            continue;

        case L'=':
            if (inQuote || fSeenEquals)
                goto DEFAULT;
            textBuffer.AddAndIncrement(L'\x01');
            fSeenEquals = true;
            break;

        default:
DEFAULT:
            textBuffer.AddAndIncrement(ch);
            break;
        }

        ++pIn;
    } 
}



DWORD GetCanonFilePath(LPCWSTR wszSrcFileName, const WCBuffer outBuffer, bool fPreserveSrcCasing)
{
    // Although this would be a nice ASSERT, we cannot make it because the source is a potentially untrusted
    // string, and thus may be unbounded (that's the purpose of this API, is to bound it and make sure it's safe)
    // So make no assumptions about wszSrcFileName!
    // ASSERT(wcslen(wszSrcFileName) <= cchDestFileName );
    DWORD full_len;
    WCBuffer fullPathBuffer(STACK_ALLOC(WCHAR, outBuffer.Count()), outBuffer.Count());
    WCBuffer tempPathBuffer(STACK_ALLOC(WCHAR, outBuffer.Count()), outBuffer.Count());

    WCBuffer outCurrent = outBuffer.CreateTraversal();

    fullPathBuffer.InitWith(0);

    if (outCurrent.GetData() != wszSrcFileName) 
        outCurrent.SetAt(0, 0);

    // Replace '\\' with single backslashes in paths, because W_GetFullPathName fails to do this on win9x.
    size_t i = 0;
    size_t j = 0;
    size_t length = wcslen(wszSrcFileName);
    while (j<length)
    {
        // UNC paths start with '\\' so skip the first character if it is a backslash.
        if (j!= 0 && wszSrcFileName[j] == '\\' && wszSrcFileName[j+1] == '\\')
            j++;
        else
            tempPathBuffer.SetAt((int)(i++), wszSrcFileName[j++]);
        if (i >= tempPathBuffer.Count()) {
            SetLastError(ERROR_FILENAME_EXCED_RANGE);
            goto FAIL;
        }
    }
    tempPathBuffer.SetAt((int)i, 0);

    full_len = W_GetFullPathName(tempPathBuffer, fullPathBuffer.Count(), fullPathBuffer.GetData(),  NULL);
    if (wszSrcFileName == outBuffer.GetData())
        outBuffer.SetAt(outBuffer.Count() - 1, L'\0');
    if (full_len == 0) {
        goto FAIL;
    } else if (full_len >= outBuffer.Count()) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        goto FAIL;
    }

    HRESULT hr;
    hr = StringCchCopyW (outBuffer.GetData(), outBuffer.Count(), fullPathBuffer);
    if (FAILED (hr)) {
        ASSERT (!"StringCchCopyW failed");
        goto FAIL;
    }

    return full_len;

FAIL:
    return 0;
}

//
// ANSI/UNICODE indifferent file opener
// also gets the file size iff fileLen != NULL
//
HANDLE OpenFileEx( LPCWSTR filename, DWORD *fileLen, LPCWSTR relPath, bool bWriteAlso)
{
    HANDLE local;
    DWORD len;

    ASSERT(filename != NULL && wcslen(filename) < MAX_PATH);
    ASSERT(relPath == NULL || wcslen(relPath) < MAX_PATH);

    if (bWriteAlso)
        local = W_CreateFile( filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    else
        local = W_CreateFile( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    // Try a relative path
    WCHAR newPath[MAX_PATH];
    WCHAR fullPath[MAX_PATH+1];
    WCBuffer fullPathBuffer(fullPath);
    if( local == INVALID_HANDLE_VALUE && relPath && 
        SUCCEEDED(StringCchCopyW(newPath, lengthof(newPath), relPath)) &&
        PathRemoveFileSpecW(newPath) && 
        MakePath(newPath, filename, fullPathBuffer))
    {
        if (bWriteAlso)
            local = W_CreateFile( fullPath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        else
            local = W_CreateFile( fullPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    }

    if (local == INVALID_HANDLE_VALUE || !fileLen)
        return local;

    len = GetFileSize( local, NULL);
    if (len == 0xFFFFFFFF) {
        CloseHandle( local);
        *fileLen = (DWORD) -1;
        local = INVALID_HANDLE_VALUE;
    } else {
        *fileLen = len;
    }

    return local;
}

HRESULT OpenAndReadFile (PCWSTR pszFileName, CAllocBuffer & outBuffer)
{
    DWORD       cbFile, cbFileHigh;
    HANDLE      hFile = OpenFileEx(pszFileName, NULL);
    HRESULT     hr = S_OK;
    CAllocBuffer localBuffer;

    if (hFile == INVALID_HANDLE_VALUE || hFile == 0)
        return HRESULT_FROM_WIN32(GetLastError());


    // Find out how big it is
    cbFile = GetFileSize (hFile, &cbFileHigh);

    if (cbFile == INVALID_FILE_SIZE)
    {
        EnsureFailedWinErrAndExit(GetLastError());
    }

    if (cbFileHigh != 0)
    {
        // Whoops!  Too big.
        EnsureFailedWinErrAndExit(ERROR_ARITHMETIC_OVERFLOW);
    }

    if (FAILED(hr = localBuffer.AllocFromFile(hFile, cbFile))) {
        goto LExit;
    }

LExit:
    CloseHandle(hFile);
    if (SUCCEEDED(hr)) {
        outBuffer.TransferOwnershipFrom(localBuffer);
    }

    return hr;
}

HRESULT ConvertToUnicode (CBuffer & inBuffer, UINT uiCodePage, WCAllocBuffer & outBuffer, FileType *fileType)
{
    WCAllocBuffer textBuffer;
    FileType    ft = ftUnknown;
    DWORD       ichStart = 0, cchUnicode = 0;
    HRESULT     hr = S_OK;

    // Determine Character encoding of file

    if (inBuffer.Count() == 0) goto EmptyFile;

    switch( inBuffer[0] ) {
    case '\xFE':
        if (inBuffer.Count() < 2 || inBuffer.Count() & 1 || inBuffer[1] != '\xFF')
            ft = ftASCII;     // Not a Unicode File
        else {
            ft = ftSwappedUnicode;
            ichStart = 2;
        }
        break;
    case '\xFF':
        if (inBuffer.Count() < 2 || inBuffer.Count() & 1 || inBuffer[1] != '\xFE')
            ft = ftASCII;     // Not a Unicode File
        else {
            ft = ftUnicode;
            ichStart = 2;
        }
        break;
    case '\xEF':
        if ( inBuffer.Count() < 3 || inBuffer[1] != '\xBB' || inBuffer[2] != '\xBF')
            ft = ftASCII;     // Not a UTF-8 File
        else {
            ft = ftUTF8;
            ichStart = 3;
        }
        break;
    case 'M':
        if (inBuffer.Count() > 1 && inBuffer[1] == 'Z') {
            ft = ftBinary;
            hr = E_FAIL;
            goto LExit;
        }
        else
            ft = ftASCII;

        break;

    default:
EmptyFile:
        ft = ftASCII;
        break;
    }

    // handle UTF8 and UNICODE codepage specifiers
    if (ft == ftASCII) {
        if (uiCodePage == CP_UTF8)
            ft = ftUTF8;
        else if (!(inBuffer.Count() & 1) && (uiCodePage == CP_WINUNICODE))
            ft = ftUnicode;
        else if (!(inBuffer.Count() & 1) && uiCodePage == CP_UNICODESWAP)
            ft = ftSwappedUnicode;
        else if (uiCodePage == 0) {
            uiCodePage = GetACP();
        }
    }


    inBuffer.MoveForward(ichStart);

    switch( ft ) {
    case ftUnicode:
    case ftSwappedUnicode:
        if (FAILED(hr = textBuffer.AllocCount(inBuffer.Count() / sizeof(WCHAR) + 1))) goto LExit;

        if (ft == ftUnicode) {
            // No conversion needed
            textBuffer.CopyFrom(inBuffer);
        } else {
            // Swap bytes.
            _swab( inBuffer.GetData(), (LPSTR) textBuffer.GetData(), inBuffer.Count());   // Swap all possible bytes
        }

        textBuffer.SetAt(textBuffer.Count() - 1, 0);
        break;
    case ftUTF8:
        SetLastError(0);
        // Calculate the size needed
        cchUnicode = UnicodeLengthOfUTF8 (inBuffer, inBuffer.Count());
        if (cchUnicode == 0 && GetLastError() != ERROR_SUCCESS)
        {
            EnsureFailedWinErrAndExit(GetLastError());
        }

        if (FAILED (hr = textBuffer.AllocCount(SizeAdd(cchUnicode, 1)))) goto LExit;

        // Convert, remembering number of characters
        cchUnicode = UTF8ToUnicode (inBuffer, inBuffer.Count(), textBuffer.GetData(), cchUnicode + 1);
        if (cchUnicode == 0 && GetLastError() != ERROR_SUCCESS)
        {
            EnsureFailedWinErrAndExit(GetLastError());
        }
        textBuffer.SetAt(textBuffer.Count() - 1, 0);
        break;
    case ftASCII:
        ASSERT(ichStart == 0);
        if (inBuffer.Count() == 0) { // Special case for completely empty files
            if (FAILED(hr = textBuffer.AllocCount(1))) goto LExit;
            textBuffer.SetAt(0, 0);
            break;
        }

        // Calculate the size needed
        cchUnicode = MultiByteToWideChar (uiCodePage, 0, inBuffer, inBuffer.Count(), NULL, 0);
        if (cchUnicode == 0 && GetLastError() != ERROR_SUCCESS)
        {
            EnsureFailedWinErrAndExit(GetLastError());
        }

        if (FAILED(hr = textBuffer.AllocCount(cchUnicode + 1))) goto LExit;

        // Convert, remembering number of characters
        cchUnicode = MultiByteToWideChar (uiCodePage, 0, inBuffer, inBuffer.Count(), textBuffer.GetData(), textBuffer.Count());
        if (cchUnicode == 0 && GetLastError() != ERROR_SUCCESS)
        {
            EnsureFailedWinErrAndExit(GetLastError());
        }

        textBuffer.SetAt(textBuffer.Count() - 1, 0);
        break;

    default:
        hr = E_INVALIDARG;
        break;
    }

LExit:
    if (SUCCEEDED(hr)) {
        outBuffer.TransferOwnershipFrom(textBuffer);
    }
    *fileType = ft;

    return hr;
}

HRESULT ReadTextFile (PCWSTR pszFileName, UINT uiCodePage, WCAllocBuffer & outBuffer, FileType *fileType)
{
    *fileType = ftUnknown;

    CAllocBuffer cTextBuffer;
    HRESULT     hr = S_OK;
    
    // Open the file
    if (SUCCEEDED(hr = OpenAndReadFile( pszFileName, cTextBuffer)))
    {

        hr = ConvertToUnicode( cTextBuffer, uiCodePage, outBuffer, fileType);
    }

    return hr;
}


////////////////////////////////////////////////////////////////////////////////
// MakePath
//
// Joins a relative or absolute filename to the given path and stores the new
// filename in lpBuffer

bool MakePath(LPCWSTR lpPath, LPCWSTR lpFileName, const WCBuffer pathBuffer)
{
    size_t nBufferLength = pathBuffer.Count();
    LPWSTR lpNewFile = STACK_ALLOC(WCHAR, nBufferLength);

    if (PathIsURLW(lpFileName))
    {
        StringCchCopyW(pathBuffer.GetData(), pathBuffer.Count(), lpFileName);

        // Do not pass URLs through GetCanonFilePath()
        return true;
    }

#ifdef PLATFORM_UNIX
    if (lpFileName[0] == L'\\' || lpFileName[0] == L'/')
#else
    if (PathIsUNCW(lpFileName) || lpFileName[1] == L':')
#endif
    {
        // This is already a fully qualified name beginning with
        // a drive letter or UNC path, just return it
        ASSERT(!PathIsRelativeW(lpFileName));
        StringCchCopyW(lpNewFile, nBufferLength, lpFileName);
    }
#ifndef PLATFORM_UNIX
    else if (lpFileName[0] == L'\\' && lpPath[0] != L'\0') 
    {
        // This a root-relative path name, just cat the drive
        if (lpPath[0] == L'\\') 
        { 
            LPWSTR lpSlash = lpNewFile;

            // a UNC path, use "\\server\share" as the drive
            StringCchCopyW(lpNewFile, nBufferLength, lpPath);

            // Set the 4th slash to NULL to terminate the new file-name
            for (int x = 0; x < 3 && lpSlash; x++) 
            {
                lpSlash = wcschr(lpSlash + 1, L'\\');
                ASSERT(lpSlash);
            }

            if (lpSlash) 
                *lpSlash = L'\0';
        } 
        else 
        { 
            // a drive letter
            StringCchCopyNW(lpNewFile, nBufferLength, lpPath, 2);
        }
        StringCchCatW(lpNewFile, nBufferLength, lpFileName);

    }
#endif
    else
    {
        ASSERT(PathIsRelativeW(lpFileName));
        // This is a relative path name, just cat everything
        StringCchCopyW(lpNewFile, nBufferLength, lpPath);

        if (lpNewFile[wcslen(lpNewFile)-1] != '\\')
            StringCchCatW(lpNewFile, nBufferLength, L"\\");
        StringCchCatW(lpNewFile, nBufferLength, lpFileName);
    }

    // Now fix everything up to use cannonical form
    return (0 != GetCanonFilePath(lpNewFile, pathBuffer, false));
}
