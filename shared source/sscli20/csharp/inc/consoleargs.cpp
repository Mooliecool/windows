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

#include "consoleargs.h"
#include "unilib.h"
#include <strsafe.h>
#include "file_can.h"

#if !defined(CLIENT_IS_CSC) && !defined(CLIENT_IS_ALINK)
#error "Must define either CLIENT_IS_CSC or CLIENT_IS_ALINK when using ConsoleOutput"
#endif

void ConsoleArgs::CleanUpArgs()
{
    while (m_listArgs) {
        WStrList * next = m_listArgs->next;
        if (m_listArgs->arg)
            VSFree(m_listArgs->arg);
        delete m_listArgs;
        m_listArgs = next;
    }

    if (m_rgArgs)
        VSFree((void*)m_rgArgs);
    m_rgArgs = NULL;
    m_output = NULL;
}

bool ConsoleArgs::GetFullFileName(const WCBuffer filenameBuffer, bool fOutputFilename)
{
    if (0 == GetCanonFilePath(filenameBuffer, filenameBuffer, fOutputFilename)) {
        if (GetLastError() == ERROR_FILE_EXISTS) {
            ASSERT(fOutputFilename);
            m_output->ShowErrorIdString(FTL_OutputFileExists, ERROR_FATAL, filenameBuffer.Str());
        }
        else {
            m_output->ShowErrorIdString(FTL_InputFileNameTooLong, ERROR_FATAL, filenameBuffer.Str());
        }
        return false;
    }
    return true;
}

bool ConsoleArgs::GetFullFileName(LPCWSTR szSource, const WCBuffer filenameBuffer, bool fOutputFilename)
{
    if (0 == GetCanonFilePath( szSource, filenameBuffer, fOutputFilename)) {
        if (filenameBuffer[0] == L'\0') {
            // This could easily fail because of an overflow, but that's OK
            // we only want what will fit in the output buffer so we can print
            // a good error message
            StringCchCopyW(filenameBuffer.GetData(), filenameBuffer.Count() - 4, szSource);
            // Don't cat on the ..., only stick it in the last 4 characters
            // to indicate truncation (if the string is short than this it just won't print)
            StringCchCopyW(filenameBuffer.GetData() + filenameBuffer.Count() - 4, 4, L"...");
        }
        if (GetLastError() == ERROR_FILE_EXISTS) {
            ASSERT(fOutputFilename);
            m_output->ShowErrorIdString(FTL_OutputFileExists, ERROR_FATAL, filenameBuffer.Str());
        }
        else {
            m_output->ShowErrorIdString(FTL_InputFileNameTooLong, ERROR_FATAL, filenameBuffer.Str());
        }
        return false;
    }
    return true;
}


//
// Create a simple leaf tree node with the given text
//
b_tree * ConsoleArgs::MakeLeaf(LPCWSTR text)
{
    b_tree * t = NULL;
    size_t name_len = wcslen(text)+1;
    LPWSTR szCopy = (LPWSTR)VSAlloc(name_len * sizeof(WCHAR));

    if (!szCopy) {
        m_output->ShowErrorId (FTL_NoMemory, ERROR_FATAL);
        return NULL;
    }

    HRESULT hr; hr = StringCchCopyW (szCopy, name_len, text);
    ASSERT (SUCCEEDED (hr));

    t = new b_tree(szCopy);
    if (!t) {
        VSFree(szCopy);
        m_output->ShowErrorId (FTL_NoMemory, ERROR_FATAL);
        return NULL;
    }
    return t;
}

bool FreeString(LPCWSTR szText)
{
    if (szText)
        VSFree(const_cast<LPWSTR>(szText));
    return true;
}

//
// Free the memory allocated by the tree (recursive)
//
void ConsoleArgs::CleanupTree(b_tree *root)
{
    if (root == NULL)
        return;
    root->InOrderWalk(FreeString);
    delete root;
}

//
// Search the binary tree and add the given string
// return true if it was added or false if it already
// exists
//
HRESULT ConsoleArgs::TreeAdd(b_tree **root, LPCWSTR add)
{
    // Special case - init the tree if it
    // doesn't already exist
    if (*root == NULL) {
        *root = MakeLeaf(add);
        return *root == NULL ? E_OUTOFMEMORY : S_OK;
    }

    size_t name_len = wcslen(add)+1;
    LPWSTR szCopy = (LPWSTR)VSAlloc(name_len * sizeof(WCHAR));

    if (!szCopy) {
        m_output->ShowErrorId (FTL_NoMemory, ERROR_FATAL);
        return NULL;
    }

    HRESULT hr = StringCchCopyW (szCopy, name_len, add);
    ASSERT (SUCCEEDED (hr));

    // otherwise, just let the template do the work
    hr = (*root)->Add(szCopy, _wcsicmp);

    if (hr != S_OK) // S_FALSE means it already existed
        VSFree(szCopy);

    return hr;
}

//
// Parse the text into a list of argument
// return the total count 
// and set 'args' to point to the last list element's 'next'
// This function assumes the text is NULL terminated
//
void ConsoleArgs::TextToArgs(LPCWSTR szText, WStrList ** listReplace)
{
    WStrList **argLast;
    const WCHAR *pCur;
    size_t iSlash;
    int iCount;

    argLast = listReplace;
    pCur = szText;
    iCount = 0;

    // Guaranteed that all tokens are no bigger than the entire file.
    LPWSTR szTemp = (LPWSTR)VSAlloc(sizeof(WCHAR) * (wcslen(szText) + 1));
    if (!szTemp) {
        m_output->ShowErrorId (FTL_NoMemory, ERROR_FATAL);
        return;
    }
    while (*pCur != '\0')
    {
        WCHAR *pPut, *pFirst, *pLast;
        WCHAR chIllegal;

LEADINGWHITE:
        while (IsWhitespace( *pCur) && *pCur != '\0')
            pCur++;

        if (*pCur == '\0')
            break;
        else if (*pCur == L'#')
        {
            while ( *pCur != '\0' && *pCur != '\n')
                pCur++; // Skip to end of line
            goto LEADINGWHITE;
        }

        int cQuotes = 0;
        pPut = pFirst = szTemp;
        chIllegal = 0;
        while ((!IsWhitespace( *pCur) || !!(cQuotes & 1)) && *pCur != '\0')
        {
            switch (*pCur)
            {
                // All this weird slash stuff follows the standard argument processing routines
            case L'\\':
                iSlash = 0;
                // Copy and advance while counting slashes
                while (*pCur == L'\\') {
                    *pPut++ = *pCur++;
                    iSlash++;
                }

                // Slashes not followed by a quote character don't matter now
                if (*pCur != L'\"')
                    break;

                // If there's an odd count of slashes, it's escaping the quote
                // Otherwise the quote is a quote
                if ((iSlash & 1) == 0) {
                    ++cQuotes;
                }
                *pPut++ = *pCur++;
                break;

            case L'\"':
                ++cQuotes;
                *pPut++ = *pCur++;
                break;

            case L'\x01': case L'\x02': case L'\x03': case L'\x04': case L'\x05': case L'\x06': case L'\x07': case L'\x08':
            case L'\x09': case L'\x0A': case L'\x0B': case L'\x0C': case L'\x0D': case L'\x0E': case L'\x0F': case L'\x10':
            case L'\x11': case L'\x12': case L'\x13': case L'\x14': case L'\x15': case L'\x16': case L'\x17': case L'\x18':
            case L'\x19': case L'\x1A': case L'\x1B': case L'\x1C': case L'\x1D': case L'\x1E': case L'\x1F':
            case L'|':
                // Save the first legal character and skip over them
                if (chIllegal == 0)
                    chIllegal = *pCur;
                pCur++;
                break;

            default:
                *pPut++ = *pCur++;  // Copy the char and advance
                break;
            }
        }

        pLast = pPut;
        *pPut++ = '\0';

        // If the string is surrounded by quotes, with no interior quotes, remove them.
        if (cQuotes == 2 && *pFirst == L'\"' && *(pLast - 1) == L'\"') {
            ++pFirst;
            --pLast;
            *pLast = L'\0';
        }

        if (chIllegal != 0) {
            m_output->ShowErrorIdString( ERR_IllegalOptionChar, ERROR_ERROR, chIllegal, pFirst);
        }

        size_t cchLen = pLast - pFirst + 1;
        WCHAR * szArgCopy = (WCHAR*)VSAlloc( sizeof(WCHAR) * cchLen);
        if (!szArgCopy || FAILED(StringCchCopyW(szArgCopy, cchLen, pFirst))) {
            VSFAIL("The string changed size, or our pointers got messed up");
            m_output->ShowErrorId(FTL_NoMemory, ERROR_FATAL);
            break;
        }

        WStrList * listArgNew = new WStrList( szArgCopy, (*argLast));
        if (!listArgNew) {
            m_output->ShowErrorId(FTL_NoMemory, ERROR_FATAL);
            break;
        }

        *argLast = listArgNew;
        argLast = &listArgNew->next;
    }

    VSFree(szTemp);

}

inline int  HexValue (WCHAR c) { ASSERT (c < 0xFF && isxdigit((char)c)); return (c >= '0' && c <= '9') ? c - '0' : (c & 0xdf) - 'A' + 10; }

// Similar to wcstoul, but returns a 64-bit number, and always assumes base is 0 
// static
ULONGLONG ConsoleArgs::wcstoul64( LPCWSTR nptr, LPCWSTR * endptr)
{
    unsigned __int64 val = 0; // accumulator
    unsigned __int64 maxval, maxdigval; // some limits
    const WCHAR *p = nptr;  // scanning/peek pointer
    unsigned ibase;
    unsigned digval;
    WCHAR c;                // current char
    bool fNegated = false;
    bool fHadDigits = false;
    bool fOverflow = false;

    if (endptr != NULL)
        *endptr = (WCHAR*)nptr;
	errno = 0;

    c = *p++;
    while(iswspace(c))
        c = *p++;

    if (c == L'+')
        c = *p++;
    else if (*nptr == L'-') {
        fNegated = true;
        c = *p++;
    }

    if (c == L'0') {
        if (*p == L'x' || *p == L'X') {
            // Hex
            ++p;
            c = *p++;
            ibase = 16;
            maxval = UI64(0xFFFFFFFFFFFFFFFF) / 16;
        }
        else {
            // Octal
            ibase = 8;
            maxval = UI64(0xFFFFFFFFFFFFFFFF) / 8;
        }
    }
    else {
        // Decimal
        ibase = 10;
    }

    maxval = UI64(0xFFFFFFFFFFFFFFFF) / ibase;
    maxdigval = UI64(0xFFFFFFFFFFFFFFFF) % ibase;

    for (;;) {
        if (c > 0xFF || !isxdigit((char)c) || (digval = HexValue(c)) >= ibase)
            break;

        fHadDigits = true;

        if (!fOverflow && (val < maxval || (val == maxval && digval <= maxdigval)))
            val = val * ibase + digval;
        else {
            fOverflow = true;
            if (endptr == NULL) {
                /* no need to keep on parsing if we
                   don't have to return the endptr. */
                break;
            }
        }

        c = *p++;
    }

    --p;                /* point to place that stopped scan */

    if (!fHadDigits) {
        /* no number there; return 0 and point to beginning of
           string */
        if (endptr)
            /* store beginning of string in endptr later on */
            p = nptr;
        ASSERT(val == 0);
    }
    else if ( fOverflow )
    {
        /* overflow occurred */
        errno = ERANGE;
        val = UI64(0xFFFFFFFFFFFFFFFF);
    }

    if (endptr != NULL)
        /* store pointer to char that stopped the scan */
        *endptr = (WCHAR*)p;

    if (fNegated)
        /* negate result if there was a neg sign */
        val = (unsigned __int64)(-(__int64)val);

    return val;
}


// Copy an argument list into an argv array. The argv array is allocated, the
// arguments themselves are not allocated -- just pointer copied.
bool ConsoleArgs::GetArgs( int * pargc, WCBuffer ** pargv)
{
    LPWSTR cmdLine = GetCommandLineW();
    *pargc = 0;
    *pargv = NULL;
    
    TextToArgs(cmdLine, &m_listArgs);
    if (m_output->HadFatalError())
        return false;

    if (m_listArgs) { // make prefix happy...
        // Strip the process name
        VSFree(m_listArgs->arg);
        WStrList * listProc = m_listArgs;
        m_listArgs = m_listArgs->next;
        delete listProc;
    }

#ifdef CLIENT_IS_CSC
    // Process '/noconfig', and CSC.CFG, modifying the argument list
    ProcessAutoConfig();
#endif

    // Process Response Files
    ProcessResponseArgs();
    if (m_output->HadError())
        return false;

    // Now convert to an argc/argv form for remaining processing.
    int argc = 0;
    for (WStrList * listCurArg = m_listArgs; listCurArg != NULL; listCurArg = listCurArg->next) {
        if (listCurArg->arg)
            ++argc;
    }

    m_rgArgs = (WCBuffer *)VSAlloc(sizeof(WCBuffer) * (argc));
    if (!m_rgArgs) {
        m_output->ShowErrorId (FTL_NoMemory, ERROR_FATAL);
        return false;       
    }
    int i = 0;
    for (WStrList * listCurArg = m_listArgs; listCurArg != NULL; listCurArg = listCurArg->next) {
        if (listCurArg->arg)
            m_rgArgs[i++] = WCBuffer::CreateFrom(listCurArg->arg);
    }

    *pargc = argc;
    *pargv = m_rgArgs;
    return !m_output->HadError();
}

/*
 * Process Response files on the command line
 * Returns true if it allocated a new argv array that must be freed later
 */
void ConsoleArgs::ProcessResponseArgs()
{
    HRESULT hr;
    b_tree *response_files = NULL;

    WCHAR   szFilename[MAX_PATH];
    WCAllocBuffer textBuffer;

    for (WStrList * listCurArg = m_listArgs;
        listCurArg != NULL && !m_output->HadFatalError();
        listCurArg = listCurArg->next)
    {
        WCHAR  * szArg = listCurArg->arg;

        // Skip everything except Response files
        if (szArg == NULL || szArg[0] != '@')
            continue;

        if (wcslen(szArg) == 1) {
            m_output->ShowErrorIdString( ERR_NoFileSpec, ERROR_ERROR, szArg);
            goto CONTINUE;
        }

        // Check for duplicates
        if (!GetFullFileName( RemoveQuotes(WCBuffer::CreateFrom(&szArg[1])), szFilename, false))
            continue;

        hr = TreeAdd(&response_files, szFilename);
        if (hr == E_OUTOFMEMORY) {
            m_output->ShowErrorId(FTL_NoMemory, ERROR_FATAL);
            goto CONTINUE;
        } else if (hr == S_FALSE) {
            m_output->ShowErrorIdString(ERR_DuplicateResponseFile, ERROR_ERROR, szFilename);
            goto CONTINUE;
        }

        FileType fileType;
        textBuffer.Clear();
        if (FAILED(hr = ReadTextFile(szFilename, NULL, textBuffer, &fileType)) || hr == S_FALSE)
        {
            if (hr == E_OUTOFMEMORY) {
                m_output->ShowErrorId(FTL_NoMemory, ERROR_FATAL);
            } else if (FAILED(hr)) {
                if (fileType == ftBinary)
                    m_output->ShowErrorIdString(ERR_BinaryFile, ERROR_ERROR, szFilename);
                else
                    m_output->ShowErrorIdString(ERR_OpenResponseFile, ERROR_ERROR, szFilename, m_output->ErrorHR(hr));
            }
            goto CONTINUE;
        }


        TextToArgs( textBuffer, &listCurArg->next);

CONTINUE: // remove the response file argument, and continue to the next.
        listCurArg->arg = NULL;
        VSFree(szArg);
    }

    CleanupTree(response_files);
}



#ifdef CLIENT_IS_CSC
//
// Process Auto Config options:
// #1 search for '/noconfig'
// if not present and csc.cfg exists in EXE dir, inject after env var stuff
//
void ConsoleArgs::ProcessAutoConfig()
{
    bool fFoundNoConfig = false;

    // Scan the argument list for the "/noconfig" options. If present, just kill it and bail.
    for (WStrList * listArgCur = m_listArgs; listArgCur; listArgCur = listArgCur->next)
    {
        // Skip everything except options
        WCHAR * szArg = listArgCur->arg;
        if (szArg == NULL || (szArg[0] != '/' && szArg[0] != '-'))
            continue;

        if (_wcsicmp(szArg + 1, L"noconfig") == 0) {
            listArgCur->arg = NULL;
            VSFree(szArg);
            // We found it, empty it but keep checking in case they specified it twice
            fFoundNoConfig = true;
        }
    }

    if (fFoundNoConfig)
        return;

    // If we got here it means there was no '/noconfig'
    WCHAR szPath[MAX_PATH];
    if (W_IsUnicodeSystem()) {
        if(!GetModuleFileNameW(NULL, szPath, lengthof(szPath)))
            szPath[0] = 0;
    } else {
        CHAR szTemp[MAX_PATH];
        if (!GetModuleFileNameA(NULL, szTemp, lengthof(szTemp)) ||
            !MultiByteToWideChar( AreFileApisANSI() ? CP_ACP : CP_OEMCP,
                0, szTemp, -1, szPath, lengthof(szPath)))
            szPath[0] = 0;
    }
    if (*szPath && PathRemoveFileSpecW(szPath) &&
        PathAppendW(szPath, L"csc.rsp")) {

        if (W_Access( szPath, 4) == 0) { 
            // We know the file exists and that we have read access
            // so add into the list
            size_t cchLen = wcslen(szPath) + 2; // +2 for @ and terminator
            WCHAR * szPathCopy = (WCHAR*)VSAlloc( sizeof(WCHAR) * cchLen);
            if (!szPathCopy || FAILED(StringCchCopyW(szPathCopy + 1, cchLen, szPath))) {
                VSFAIL("The string changed size, or our pointers got messed up");
                m_output->ShowErrorId(FTL_NoMemory, ERROR_FATAL);
                return;
            }
            szPathCopy[0] = L'@';

            WStrList * listArgNew = new WStrList( szPathCopy, m_listArgs);
            if (!listArgNew) {
                VSFree(szPathCopy);
                m_output->ShowErrorId(FTL_NoMemory, ERROR_FATAL);
                return;
            }

            m_listArgs = listArgNew;
        }
    }
}

#endif // CLIENT_IS_CSC

