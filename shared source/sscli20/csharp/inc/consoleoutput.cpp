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

#include "cscommon.h"
#include "consoleoutput.h"
#include "unilib.h"
#include <strsafe.h>
#include <time.h>
#include <io.h>
#include <fcntl.h>
#include <cor.h>
#include <mscoree.h>
#include <stdio.h>
#include <mbstring.h>
#include "file_can.h"

#if defined(CLIENT_IS_CSC)
const LPWSTR ConsoleOutput::m_errorPrefix = L"CS";
#elif defined(CLIENT_IS_ALINK)
const LPWSTR ConsoleOutput::m_errorPrefix = L"AL";
#endif


bool ConsoleOutput::Initialize(HINSTANCE hinstMessages, LPCWSTR messageDLL, const ERROR_INFO *pErrorInfo)
{
    ASSERT(pErrorInfo);

    m_fRedirectingToFile = true;

    GetConsoleOutputCP();

    m_pErrorInfo = pErrorInfo;
    if (! hinstMessages) {
        // we can't do anything without the messages DLL. This is the only message
        // that can't be localized!
        WCHAR *pszFormat = L"Unable to find messages file '%s'";
        WCHAR pszMessage[MAX_PATH + lengthof(pszFormat)];
        StringCchPrintfW(pszMessage,lengthof(pszMessage),pszFormat, messageDLL);
        ShowError(m_pErrorInfo[FTL_NoMessagesDLL].number, ERROR_FATAL, NULL, -1, -1, -1, -1, pszMessage);
        return false;
    }

    m_hinstMessages = hinstMessages;
    
#ifdef CLIENT_IS_CSC
    // currently only csc displays related locations
    LoadStringWide (m_hinstMessages, IDS_RELATEDWARNING, m_szRelatedWarning);
    LoadStringWide (m_hinstMessages, IDS_RELATEDERROR, m_szRelatedError);
#endif

    return true;
}

//
// Simple error display routine, takes error id, no location
//
void ConsoleOutput::ShowErrorId(int id, ERRORKIND kind)
{
    int i;
    WCHAR buffer[2000];

    i = LoadStringWide(m_hinstMessages, m_pErrorInfo[id].resid, buffer);
    ASSERT(i);  // String should be there.
    if (i) {
        // Call onto general method to show the error.
        ShowError(m_pErrorInfo[id].number, kind, NULL, -1, -1, -1, -1, buffer);
    }
}

//
// Public general display routine to report error or warning.  Converts unicode to console code-page and 
// calls the protected ShowError() method to actually show the error.
// 
void ConsoleOutput::ShowError(int id, ERRORKIND kind, PCWSTR inputfile, int line, int col, int lineEnd, int colEnd, LPCWSTR text)
{
    const WCHAR *errkind;
    switch (kind) {
    case ERROR_FATAL:
        m_fHadFatalError = true;
        m_fHadError = true;
        errkind = L"fatal error"; break;
    case ERROR_ERROR:
        m_fHadError = true;
        errkind = L"error"; break;
    case ERROR_WARNING:
        errkind = L"warning"; break;
    default:
        ASSERT(0);
        return;
    }

    // Print file and location, if available.
    // Output must be in ANSI, so convert.
    if (inputfile) {
        if (!m_fFullPaths) {
            // If the file is in a subdirectory, use a relative pathname
            // for reporting errors
            WCHAR cur[MAX_PATH], temp[MAX_PATH];

            if (W_IsUnicodeSystem()) {
                if(!GetCurrentDirectoryW(MAX_PATH, cur))
                    cur[0] = 0;
            } else {
                CHAR pszTemp[MAX_PATH];
                if (!GetCurrentDirectoryA(MAX_PATH, pszTemp) ||
                    !MultiByteToWideChar( AreFileApisANSI() ? CP_ACP : CP_OEMCP,
                        0, pszTemp, -1, cur, MAX_PATH))
                    cur[0] = 0;
            }

            int i = PathCommonPrefixW(cur, inputfile, temp);
            if (i && _wcsicmp(cur, temp) == 0 &&
                    PathRelativePathToW(temp, cur, FILE_ATTRIBUTE_DIRECTORY, inputfile, 0) &&
                    PathCanonicalizeW(cur, temp))
                print(L"%s", cur);
            else
                print(L"%s", inputfile);
        }
        else
            print(L"%s", inputfile);

        if (m_fShowErrorExtents && line > 0 && lineEnd > 0) {
            print(L"(%d,%d)-(%d,%d)", line, col, lineEnd, colEnd);
        }
        else if (line > 0) {
            print(L"(%d,%d)", line, col);
        }
        print(L": ");
    }
#ifdef CLIENT_IS_ALINK
    else 
        print(L"ALINK: ");
#endif
    // Print "error ####" -- only if not a related symbol location (id == -1)
    if (id != -1)
        print(L"%s %s%04d: ", errkind, m_errorPrefix, id);

    // Print error text. (This will indent subsequent lines by 8 characters and terminate with a newline)
    if (text && text[0])
        PrettyPrint(text, 8);
    else
        print(L"\n");
}


LPWSTR
ConsoleOutput::FormatErrorIdW (int id, ERRORKIND kind, va_list args)
{
    WCHAR buffer1[1000];
    if (!LoadStringW(m_hinstMessages, m_pErrorInfo[id].resid, buffer1, lengthof (buffer1)))
        return NULL;

    // Insert fill-ins.
    WCHAR buffer2[2000];
    if (!FormatMessageW(FORMAT_MESSAGE_FROM_STRING, buffer1, 0, 0,buffer2, lengthof (buffer2), &args))
        return NULL;

    size_t iLenPlusNull = (wcslen(buffer2)+1);
    LPWSTR wbuffer = (LPWSTR) VSAlloc(iLenPlusNull * sizeof(WCHAR));
    if (!wbuffer)
        return NULL;

    StringCchCopyW(wbuffer, iLenPlusNull, buffer2);
    return wbuffer;
}


LPWSTR
ConsoleOutput::FormatErrorId (int id, ERRORKIND kind, va_list args)
{
    LPWSTR error_string = NULL;

    error_string = FormatErrorIdW (id, kind, args);
    ASSERT (error_string);
    return error_string;
}

//
// Simple error display routine, takes resource id (textid), fill-in arguments, no location.
//
void _cdecl ConsoleOutput::ShowErrorIdString(int id, ERRORKIND kind, ...)
{
    LPWSTR error_string;
    va_list args;
    va_start (args, kind);

    error_string = FormatErrorId (id, kind, args);
    va_end (args);
    ASSERT (error_string);

    if (error_string) {
        ShowError(m_pErrorInfo[id].number, kind, NULL, -1, -1, -1, -1, error_string);
        VSFree(error_string);
    }
}

//
// Simple error display routine, takes resource id (textid), location, fill-in arguments.
//
void _cdecl ConsoleOutput::ShowErrorIdLocation(int id, ERRORKIND kind, LPCWSTR szLocation, ...)
{
    LPWSTR error_string;
    va_list args;
    va_start (args, szLocation);
    
    error_string = FormatErrorId (id, kind, args);
    va_end (args);
    ASSERT (error_string);
    
    if (error_string) {
        ShowError(m_pErrorInfo[id].number, kind, szLocation, -1, -1, -1, -1, error_string);
        VSFree (error_string);
    }
}

/*
 * Get a string for system error
 */
LPCWSTR ConsoleOutput::ErrorSystemError(DWORD error)
{
    int r;

    DWORD dwFormatMessageFlags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

    // Use FormatMessage to get the error string from the system.

    r = FormatMessageW(dwFormatMessageFlags,
                       NULL, error, 0,
                       m_errorBuffer, lengthof(m_errorBuffer),
                       NULL);


    // Check for errors
    if (r == 0) {
        HRESULT hr; hr = StringCchPrintfW( m_errorBuffer, lengthof(m_errorBuffer), L"0x%08x", error);
        ASSERT(SUCCEEDED(hr));
    }

    return m_errorBuffer;
}

//
// Get a string (LPCWSTR) for an HRESULT
//
LPCWSTR ConsoleOutput::ErrorHR(HRESULT hr)
{
    LPCWSTR result = NULL;
    HRESULT hrOriginal = hr;

    for (;;) {
        CComPtr<IErrorInfo> err;
        CComBSTR str;

        if ((HRESULT_FACILITY(hr) == FACILITY_COMPLUS || HRESULT_FACILITY(hr) == FACILITY_URT || HRESULT_FACILITY(hr) == FACILITY_ITF) &&
            GetErrorInfo( 0, &err) == S_OK && SUCCEEDED(err->GetDescription(&str))) {
            StringCchCopyW(m_errorBuffer, lengthof (m_errorBuffer), str);
            result = m_errorBuffer;
        } else {
            result = ErrorSystemError(hr);
        }

        // Check for errors, and possibly repeat.
        if (result != NULL) {
            break;
        }

        if (hr != E_FAIL) {
            hr = E_FAIL;
            continue;
        }
        else {
            // Something really extreme. I don't understand why this would happen.
            ASSERT(0);      // investigate please.
            return L"Unknown fatal error";
        }
    }

    if (hr == E_FAIL && hr != hrOriginal && result == m_errorBuffer) {
        WCHAR errorCodeBuffer[16];
        HRESULT hr2; hr2 = StringCchPrintfW( errorCodeBuffer, lengthof(errorCodeBuffer), L" (0x%08x)", hr);
        ASSERT(SUCCEEDED(hr2));
        StringCchCatW( m_errorBuffer, lengthof(m_errorBuffer), errorCodeBuffer);
        // we don't really care if it fails because there's nothing we can do about it.
    }

    return result;
}

//
// Get a string for system error
//
LPCWSTR ConsoleOutput::ErrorLastError()
{
    LPCWSTR result = ErrorSystemError(GetLastError());

    // Check for errors
    if (result == NULL) {
        // Something really extreme. I don't understand why this would happen.
        ASSERT(0);      // investigate please.
        return L"Unknown fatal error";
    }

    return result;
}



//
// Load a string from a resource and print it, followed by an optional newline.
//
void ConsoleOutput::PrintString(unsigned id, bool newline, bool prettyprint)
{
    WCHAR buffer[512];
    int i;

    i = LoadStringWide(m_hinstMessages, id, buffer);

#ifdef CLIENT_IS_CSC
    // ALink's PrintHelp() function will just iterate through a range of ids, and only print the ones which exist.
    // Thus, the string may not exist for ALink, but it should for CSC.
    ASSERT(i);
#endif
    
    if (i) {
        if (prettyprint) {
            ASSERT(newline);
            PrettyPrint(buffer, 0);
        } else {
            print(L"%s", buffer);
            if (newline)
                print(L"\n");
        }
    }
}

//
// Print the introductory banner to the compiler.
//
void ConsoleOutput::PrintBanner()
{
    WCHAR version[100];
    WCHAR versionCLR[100];

    DisableReproOutput();   // Don't put banner in the repro file

    // Get the version string for this file.
    GetFileVersion(NULL, version);

    // Get the version string for CLR
    GetCLRVersion(versionCLR);
    
    // Print it out.
    PrintString(IDS_BANNER1, false);
    print(L"%s\n", version);
    PrintString(IDS_BANNER1PART2, false);
    print(L"%s\n", versionCLR);
    PrintString(IDS_BANNER2, true);
    print(L"\n");
    
    EnableReproOutput(); // Reenable the repro output.
}

void ConsoleOutput::Indent(size_t cchIndent)
{
    if (cchIndent >= m_width)
        cchIndent = 0;
        
    if (m_currentColumn > cchIndent)
        print (L"\n");
        
    ASSERT(cchIndent >= m_currentColumn);
    print (L"%*s", cchIndent - m_currentColumn, L"");
    ASSERT(m_currentColumn == cchIndent);
}

#ifdef CLIENT_IS_CSC
// The following functions are currently only used for CSC, some strings are not defined in alinkui.dll

//
// Help strings for each of the COF_ARG flags
// NOTE: keep this in the same order as the flags
//
static DWORD helpArgs[] = {
    (DWORD) -1,
    IDS_OD_ARG_FILELIST,
    IDS_OD_ARG_FILE,
    IDS_OD_ARG_SYMLIST,
    IDS_OD_ARG_WILDCARD,
    IDS_OD_ARG_TYPE,
    IDS_OD_ARG_RESINFO,
    IDS_OD_ARG_WARNLIST,
    IDS_OD_ARG_ADDR,
    IDS_OD_ARG_NUMBER,
    IDS_OD_ARG_DEBUGTYPE,
    IDS_OD_ARG_STRING,
    IDS_OD_ARG_ALIAS,
};

// This is the amount to shift the flags to go from COF_GRP_* to a number
#define ARG_SHIFT   20

//
// Print single option name and description
//
HRESULT ConsoleOutput::PrintOption( DWORD CurrentGrp, LPCWSTR pszLongSwitch, LPCWSTR pszShortSwitch, LPCWSTR pszDesc, DWORD dwFlags)
{
    // Never show Hidden options
    if (dwFlags & COF_HIDDEN)
        return S_FALSE;

    // Only do the current group's options or ungrouped options (for the misc. group)
    if (((dwFlags & COF_GRP_MASK) != CurrentGrp) &&
        (((dwFlags & COF_GRP_MASK) != 0) || (CurrentGrp != COF_GRP_MISC)))
        return S_FALSE;

    if (dwFlags & COF_BOOLEAN) {
        print(L"/%s[+|-]", pszLongSwitch);
    } else {
        if (dwFlags & COF_ARG_MASK) {
            WCHAR pwszArgStr[64];
            if (0 == LoadStringWide( m_hinstMessages, helpArgs[(dwFlags & COF_ARG_MASK) >> ARG_SHIFT], pwszArgStr)) {
                if (dwFlags & COF_ARG_NOCOLON)
                    pwszArgStr[0] = L'\0';
                else
                    wcsncpy_s(pwszArgStr, lengthof(pwszArgStr), L"<str>", 5);
            }
            if (dwFlags & COF_ARG_NOCOLON) {
                print (L"%s%s", pszLongSwitch, pwszArgStr);
            } else if (!!(dwFlags & COF_ARG_BOOLSTRING)) {
                print (L"/%s[+|-]:%s", pszLongSwitch, pwszArgStr);
            } else {
                print (L"/%s:%s", pszLongSwitch, pwszArgStr);
            }
        } else {
            if (dwFlags & COF_ARG_NOCOLON) {
                print (L"%s", pszLongSwitch);
            } else {
                print (L"/%s", pszLongSwitch);
            }
        }
    }

    Indent(30);

    // optionally add the short form to the description
    WCHAR wszShortForm[64];
    if (pszShortSwitch && LoadStringWide( m_hinstMessages, IDS_SHORTFORM, wszShortForm))
    {
        size_t cchNewDescription = wcslen(pszDesc) + wcslen(wszShortForm) + wcslen(pszShortSwitch) + 7;
        WCHAR *pwszNewDescription = (WCHAR*)VSAlloc(cchNewDescription * sizeof(WCHAR));
        if (!pwszNewDescription)
            return E_OUTOFMEMORY;

        StringCchPrintfW( pwszNewDescription, cchNewDescription, L"%s (%s: /%s)", pszDesc, wszShortForm, pszShortSwitch);
        PrettyPrint(pwszNewDescription, 30);
        VSFree(pwszNewDescription);
        return S_OK;
    }

    PrettyPrint(pszDesc, 30);
    return S_OK;
}

//
//  Print CommandLine options where the description is a resourceID.
//
HRESULT ConsoleOutput::PrintOptionId( DWORD CurrentGrp, LPCWSTR pszLongSwitch, LPCWSTR pszShortSwitch, DWORD dwDescId, DWORD dwFlags)
{
    WCHAR pszBuffer[1024];

    if (!LoadStringWide( m_hinstMessages, dwDescId, pszBuffer))
        return E_OUTOFMEMORY;
    return PrintOption(CurrentGrp, pszLongSwitch, pszShortSwitch, pszBuffer, dwFlags);
}

//
//  Display a related location of an error.
//
void ConsoleOutput::ShowRelatedLocation(ERRORKIND kind, PCWSTR inputfile, int line, int col, int lineEnd, int colEnd)
{
    // currently this is only used by CSC.  If this gets added to ALINK, these strings need to be defined.
    PWSTR pszMessage = (kind == ERROR_WARNING) ? m_szRelatedWarning : m_szRelatedError;
    ShowError(-1, kind, inputfile, line, col, lineEnd, colEnd, pszMessage);
}

#endif // CLIENT_IS_CSC

//
// Wrapper for printf that handles the repro file also.
//
void __cdecl ConsoleOutput::print(const WCHAR * format, ...)
{
    va_list argptr;
    va_start(argptr, format);

    int cchSize = _vscwprintf(format, argptr);
    WCAllocBuffer buffer;
    if (FAILED(buffer.AllocCount(cchSize + 1)))
        return;
    vswprintf_s(buffer.GetData(), cchSize + 1, format, argptr);
    buffer.SetAt(cchSize, L'\0');

    va_end(argptr);

    print_internal(buffer.GetData());
}

void ConsoleOutput::EnableUTF8Output()
{
    // can't output utf 8 to console, only to file
    m_fUTF8Output = m_fRedirectingToFile;

}


UINT ConsoleOutput::GetConsoleOutputCP()
{
    if (m_ConsoleCodepage == 0)
        m_ConsoleCodepage = ::GetConsoleOutputCP();
        
    return m_ConsoleCodepage;
}

/*
 * Like WideCharToMultiByte, but translates to the console code page. Returns length, 
 * INCLUDING null terminator.
 */
int ConsoleOutput::WideCharToConsole(LPCWSTR wideStr, LPSTR lpBuffer, int nBufferMax)
{
    if (m_fUTF8Output) {
        if (nBufferMax == 0) {
            return UTF8LengthOfUnicode(wideStr, (int)wcslen(wideStr)) + 1; // +1 for nul terminator
        }
        else {
            int cchConverted = NULL_TERMINATED_MODE;
            return UnicodeToUTF8 (wideStr, &cchConverted, lpBuffer, nBufferMax);
        }

    }
    else {
        return WideCharToMultiByte(GetConsoleOutputCP(), 0, wideStr, -1, lpBuffer, nBufferMax, 0, 0);
    }
}

/*
 * Convert Unicode string to Console ANSI string allocated with VSAlloc
 */
HRESULT ConsoleOutput::WideToConsole(LPCWSTR wideStr, CAllocBuffer &buffer)
{
    int cch = WideCharToConsole(wideStr, NULL, 0);
    buffer.AllocCount(cch);
    if (0 == WideCharToConsole(wideStr, buffer.GetData(), cch)) {
        VSFAIL("How'd the string size change?");
        // We have to NULL terminate the output because WideCharToMultiByte didn't
        buffer.SetAt(0, '\0');
        return E_FAIL;
    }
    return S_OK;
}


void ConsoleOutput::print_internal(LPCWSTR text)
{
    CAllocBuffer consoleText;
    if (SUCCEEDED(WideToConsole(text, consoleText)))
    {
        DWORD dwBytesWritten;
        WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), consoleText.GetData(), consoleText.Count() - 1, &dwBytesWritten, NULL);
    }

}

//
// Wrapper for LoadString that works on Win9x.
//
int ConsoleOutput::LoadStringWide(HINSTANCE hInstance, UINT uID, WCBuffer lpBuffer)
{
    int cch = LoadStringW(hInstance, uID, lpBuffer.GetData(), lpBuffer.Count());
    if (cch == 0 && GetLastError() == ERROR_CALL_NOT_IMPLEMENTED) {
        // Use ANSI version:
        CHAR * lpBufferAnsi = (CHAR *) _alloca(lpBuffer.Count() * 2);
        int cchAnsi = LoadStringA(hInstance, uID, lpBufferAnsi, lpBuffer.Count() * 2);
        if (cchAnsi) {
            cch = MultiByteToWideChar(CP_ACP, 0, lpBufferAnsi, cchAnsi, lpBuffer.GetData(), lpBuffer.Count() - 1);
            lpBuffer.SetAt(cch, 0);
        } else {
            ASSERT(!"String Not Found");
            return 0;
        }
    }
    return cch;
}


//
// Print out the given text, with word-wraping based on screen size
// and indenting the wrapped lines. Word wraps on whitespace chars
// if possible.
// Indents all lines after the first.
// There may be newlines in pszText.
// Always finishes with the console at the start of a new line.
//
void ConsoleOutput::PrettyPrint(LPCWSTR pwszText, size_t cchIndent)
{
    
    // 100 characters should be more than large enough for any indent
    ASSERT(cchIndent >= 0 && cchIndent <= 100);
    static const WCHAR pwszIndent[] = L"                                                                                                    ";
    print(L"%.*c%s\n", (int)cchIndent, pwszIndent[0], pwszText);
    
}

//
// Get the file version as a string.
//
void ConsoleOutput::GetFileVersion(HINSTANCE hinst, WCBuffer szVersion)
{
    HRESULT hr;
    hr = StringCchCopyW (szVersion.GetData(), szVersion.Count(), SSCLI_VERSION_STRW);
    ASSERT (SUCCEEDED (hr));
}

// Get version of the command language runtime.
void ConsoleOutput::GetCLRVersion(WCBuffer szVersion)
{
    WCHAR ver[MAX_PATH];
    DWORD dwLen;

    HRESULT hr;
    hr = StringCchCopyW (szVersion.GetData(), szVersion.Count(), L"---");  // default in case we can't get it.
    ASSERT (SUCCEEDED (hr));

    GetCORVersion(ver, MAX_PATH, &dwLen);
    if (ver[0] == L'v') {
        StringCchCopyW(szVersion.GetData(), szVersion.Count(), ver + 1);
        return;
    }
}



HRESULT ConsoleOutput::InitBugReport(LPCWSTR pszFilename, int argc, WCBuffer argv[], bool fPromptUser)
{
    return S_OK;
}

void ConsoleOutput::CloseBugReport()
{
}

void ConsoleOutput::OutputFileToReproFile(PCWSTR pszName, PCWSTR pszText)
{
}

void ConsoleOutput::OutputBinaryFileToRepro(LPCWSTR pszFilename, LPCWSTR pszAssemblyFile)
{
}

//
//   Loops through command line and prints it out to STDOUT
//
void ConsoleOutput::OutputCommandLine(bool fBugReport, int argc, WCBuffer argv[])
{
    for (int i = 0; i < argc; i++) {
        if (argv[i] == NULL)
            continue;

        print(L"%s\r\n", argv[i].Str());
    }   
    print(L"\r\n");
}

void ConsoleOutput::OutputUserReproDescription()
{
}


HRESULT ConsoleOutput::GetBugReportFileName(WCBuffer pszFileName)
{
    ASSERT(pszFileName.Count() >= MAX_PATH);
    {
        pszFileName.SetAt(0, '\0');
        return S_FALSE;
    }

}


//
// Disable or enable routing compiler output to the repro file.
//
void ConsoleOutput::DisableReproOutput()
{
}

void ConsoleOutput::EnableReproOutput()
{
}
