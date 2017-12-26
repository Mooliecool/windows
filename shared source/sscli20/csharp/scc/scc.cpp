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
// File: scc.cpp
//
// The command line driver for the C# compiler.
// ===========================================================================

#include "stdafx.h"
#include "scc.h"
#include "msgsids.h"

#include "uniapi.h"
#include "uniprop.h"

#define CLIENT_IS_CSC
#include "cscommon.h"
#include "file_can.h"
#include "compilerhost.h"
#undef CLIENT_IS_CSC

#include "utf.h"
#include "atl.h"
#include <palstartup.h>

#define LoadStringA(hInstance,uID,lpBuffer,nBufferMax) \
    PAL_LoadSatelliteStringA((HSATELLITE)hInstance,uID,lpBuffer,nBufferMax)
#define LoadStringW(hInstance,uID,lpBuffer,nBufferMax) \
    PAL_LoadSatelliteStringW((HSATELLITE)hInstance,uID,lpBuffer,nBufferMax)
#undef LoadString
#ifdef UNICODE
#define LoadString LoadStringA
#else
#define LoadString LoadStringW
#endif

#define CLIENT_IS_CSC
#include "consoleoutput.cpp"
#include "consoleargs.cpp"
#undef CLIENT_IS_CSC


ConsoleOutput g_output;
ConsoleArgs   g_args(&g_output);

//
// Information about each error or warning.
//
#define ERRORDEF(num, name, strid)       {num,     0, strid},
#define WARNDEF(num, level, name, strid) {num, level, strid},
#define OLDWARN(num, name)               {num,    99,    -1},
#define FATALDEF(num, name, strid)       {num,    -1, strid},

static const ERROR_INFO errorInfo[ERR_COUNT] = {
    {0000, -1, 0},          // ERR_NONE - no error.
    #include "errors.h"
};

#undef ERRORDEF
#undef WARNDEF
#undef OLDWARN
#undef FATALDEF


bool g_firstInputSet = true;

#include "compres.h"

int __cdecl CompareLongs(const void * p1, const void * p2) {
     return *(long*)p1 - *(long*)p2;
}

#ifndef _DEBUG
// These are used by Unilib in retail builds
void * VSRetAlloc(SIZE_T sz) { return HeapAlloc(GetProcessHeap(), 0, sz); }
BOOL VSRetFree(void *pMem) { return HeapFree(GetProcessHeap(), 0, pMem); }
#endif // _DEBUG

////////////////////////////////////////////////////////////////////////////////
// CheckResult
//
// Helper function for reporting errors for HRESULT values coming from COM
// functions whose failure is reported generically (i.e. compiler init failed
// unexpectedly)

inline HRESULT CheckResult (HRESULT hr)
{
    if (FAILED (hr))
        g_output.ShowErrorIdString (ERR_InitError, ERROR_FATAL, g_output.ErrorHR(hr));
    return hr;
}

CSourceText::CSourceText()
{
    m_pszName = NULL;
    m_iRef = 0;
    memset(&m_checksum, 0, sizeof(m_checksum));
}

CSourceText::~CSourceText()
{
    FreeText ();
    if (m_pszName)
        VSFree (m_pszName);

    if (m_checksum.pvData)
        VSFree (m_checksum.pvData);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceText::Initialize

HRESULT CSourceText::Initialize (PCWSTR pszFileName, BOOL fComputeChecksum, UINT uiCodePage)
{
    HRESULT hr = S_OK;

    //Remember the file name as given to us
    size_t cch = wcslen (pszFileName) + 1;
    m_pszName = (PWSTR)VSAlloc (cch * sizeof(WCHAR));
    if (m_pszName == NULL)
        return E_OUTOFMEMORY;

    hr = StringCchCopyW(m_pszName, cch, pszFileName);
    ASSERT(SUCCEEDED(hr));

    FileType fileType;
    hr = ReadTextFile (pszFileName, uiCodePage, m_textBuffer, &fileType);

    if (fileType == ftBinary) {
        ASSERT(FAILED(hr));
        g_output.ShowErrorIdString(ERR_BinaryFile, ERROR_ERROR, pszFileName);
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceText::GetChecksum

HRESULT CSourceText::GetChecksum (Checksum * checksum)
{
    *checksum = m_checksum;
    return S_OK;
}

//
// Help strings for each of the COF_GRP flags
// NOTE: keep this in the same order as the flags
//
static DWORD helpGroups[] = {
    (DWORD) -1,
    IDS_OD_GRP_OUTPUT,
    IDS_OD_GRP_INPUT,
    IDS_OD_GRP_RES,
    IDS_OD_GRP_CODE,
    IDS_OD_GRP_ERRORS,
    IDS_OD_GRP_LANGUAGE,
    IDS_OD_GRP_MISC,
    IDS_OD_GRP_ADVANCED
};
// This is the amount to shift the flags to go from COF_GRP_* to a number
#define GRP_SHIFT   12

//
// Print the help text for the compiler.
//
HRESULT PrintHelp (ICSCompilerConfig *pConfig)
{
    // The help text is numbered 10, 20, 30... so there is
    // ample room to put in additional strings in the middle.

    g_output.print(L"                      "); // 24 blanks to line up the title
    g_output.PrintString(IDS_HELP10, true);      // "Available compiler options:"

    long    iCount;
    HRESULT hr;
    DWORD   iGrp;
static struct _CmdOptions {
    DWORD   idDesc;
    PCWSTR  pszShort;
    PCWSTR  pszLong;
    DWORD   dwFlags;
} CmdOptions[] = {
#define CMDOPTDEF( id, s, l, f) {id, s, l, f},
#include "cmdoptdef.h"
    };

    // Iterate through the compiler options and display the appropriate ones
    if (FAILED (hr = CheckResult(pConfig->GetOptionCount (&iCount))))
        return hr;

    for (iGrp = 1; iGrp < lengthof(helpGroups); iGrp++) {
        g_output.print(L"\n                        "); // 24 blanks to line up the title
        g_output.PrintString( helpGroups[iGrp]);

        for (long i=0; i < (long) lengthof(CmdOptions); i++)
            IfFailRet(g_output.PrintOptionId(iGrp << GRP_SHIFT, CmdOptions[i].pszLong, CmdOptions[i].pszShort, CmdOptions[i].idDesc, CmdOptions[i].dwFlags));

        for (long i=0; i<iCount; i++)
        {
            PCWSTR  pszLongSwitch, pszShortSwitch, pszDesc;
            long    iID;
            DWORD   dwFlags;

            IfFailRet(CheckResult(pConfig->GetOptionInfoAtEx (i, &iID, &pszShortSwitch, &pszLongSwitch, NULL, &pszDesc, &dwFlags)));

            IfFailRet(g_output.PrintOption(iGrp << GRP_SHIFT, pszLongSwitch, pszShortSwitch, pszDesc, dwFlags));
        }

        if (iGrp << GRP_SHIFT == COF_GRP_RES) {
            g_output.print(L"                              "); // 30 blanks to line up with help text
            g_output.PrintString( IDS_RESINFO_DESCRIPTION, true, true);
        }
    }

    g_output.PrintBlankLine();

    return S_OK;
}

//
// Process the "pre-switches": /nologo, /?, /help, /time, /repro, /fullpaths. If they exist,
// they are nulled out.
//
void ProcessPreSwitches(int argc, WCBuffer argv[], bool * noLogo, bool * showHelp, bool * timeCompile, UINT * uiCodePage)
{
    WCBuffer arg;

    *noLogo = *showHelp = *timeCompile = false;

    for (int iArg = 0; iArg < argc; ++iArg)
    {
        arg = argv[iArg];
        if (!arg.IsValid())
            continue;
        if (arg[0] == '-' || arg[0] == '/')
            arg.MoveForward(1);
        else
            continue;  // Not an option.

        if (_wcsnicmp(arg, L"test:", 5) == 0) {
            arg.MoveForward(5);
            bool fT = true;
            if (arg[0] == L'+')
                arg.MoveForward(1);
            else if (arg[0] == L'-') {
                fT = false;
                arg.MoveForward(1);
            }

            if (_wcsicmp(arg, L"timing") == 0) {
                *timeCompile = fT;
                continue; // pass through to compiler.
            }
            if (_wcsicmp(arg, L"extents") == 0)
                g_output.ShowErrorExtents(fT);
            else
                continue; // pass through to compiler.
        }
        else if (_wcsicmp(arg, L"nologo") == 0)
            *noLogo = true;
        else if (_wcsicmp(arg, L"?") == 0)
            *showHelp = true;
        else if (_wcsicmp(arg, L"help") == 0)
            *showHelp = true;
        else if (_wcsnicmp(arg, L"codepage:", 9) == 0) {
            LPCWSTR cp = RemoveQuotes(arg + 9);
            if (*cp == '\0') {
                g_output.ShowErrorIdString( ERR_SwitchNeedsString, ERROR_FATAL, arg.Str());
            }
            else {
                int len1, len2 = (int)wcslen(cp);
                if (swscanf_s( cp, L"%u%n", uiCodePage, &len1) != 1 ||
                    len1 != len2 ||     // Did we read the WHOLE CodePageID?
                    ! (IsValidCodePage( *uiCodePage) || *uiCodePage == CP_UTF8 || *uiCodePage == CP_WINUNICODE)) {
                    g_output.ShowErrorIdString( FTL_BadCodepage, ERROR_FATAL, cp);
                }
            }
        }
        else if (_wcsicmp(arg, L"codepage") == 0) {
            g_output.ShowErrorIdString( ERR_SwitchNeedsString, ERROR_FATAL, arg.Str());
        }
        else if (_wcsicmp(arg, L"bugreport") == 0) {
            g_output.ShowErrorIdString( ERR_NoFileSpec, ERROR_FATAL, arg.Str());
        }
        else if (_wcsicmp(arg, L"utf8output") == 0) {
            g_output.EnableUTF8Output();
        }
        else if (_wcsicmp(arg, L"fullpaths") == 0)
            g_output.EnableFullPaths();
        else
            continue;       // Not a recognized argument.

        argv[iArg].Invalidate();  // NULL out recognized argument.
    }
}

//
// Process a single file name from the command line. Expand wild cards if there
// are any.
//
HRESULT ProcessFileName (LPCWSTR filename, ICSInputSet *pInput, UINT uiCodePage, BOOL *pfFilesAdded, BOOL bRecurse)
{
    HANDLE hSearch;
    WIN32_FIND_DATAW findData;
    WCHAR fullPath[MAX_PATH * 2];
    WCHAR * pFilePart;
    WCHAR searchPart[MAX_PATH];
    bool bFound = false;
    HRESULT hr = S_OK;

    if (wcslen(filename) >= MAX_PATH) {
        g_output.ShowErrorIdString(FTL_InputFileNameTooLong, ERROR_FATAL, filename);
        return S_OK;
    }

    // Copy file name, and find location of file part.
    IfFailRet(StringCchCopyW(fullPath, lengthof(fullPath), filename));

    pFilePart = wcsrchr(fullPath, L'\\');
#ifdef PLATFORM_UNIX
    WCHAR *pSlashFilePart = wcsrchr(fullPath, L'/');
    if (pSlashFilePart > pFilePart) {
        pFilePart = pSlashFilePart;
    }
#endif  // PLATFORM_UNIX
    if (!pFilePart)
        pFilePart = wcsrchr(fullPath, L':');
    if (!pFilePart)
        pFilePart = fullPath;
    else
        ++pFilePart;

    if (bRecurse) {
        // Search for subdirectories
        IfFailRet(StringCchCopyW(searchPart, lengthof(searchPart), pFilePart));
        // Search for ALL subdirectories
        IfFailRet(StringCchCopyW(pFilePart, lengthof(fullPath) - (pFilePart - fullPath), L"*.*"));

        hSearch = W_FindFirstFile(fullPath, &findData);
        if (hSearch == 0 || hSearch == INVALID_HANDLE_VALUE) {
            // No Subdirectories
            goto FileSearch;
        }

        for (;;) {
            if ((~findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) &&
                (findData.cFileName[0] != '.' ||
                (findData.cFileName[1] != '\0' && (findData.cFileName[1] != '.' || findData.cFileName[2] != '\0'))) &&
                (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                SUCCEEDED(hr = StringCchPrintfW(pFilePart, lengthof(fullPath) - (pFilePart - fullPath),
                    L"%s\\%s", findData.cFileName, searchPart))) {
                hr = ProcessFileName (fullPath, pInput, uiCodePage, pfFilesAdded, TRUE);
            }

            if (FAILED (hr) || !W_FindNextFile(hSearch, &findData))
                break;
        }

        FindClose(hSearch);
        if (FAILED (hr))
            return hr;
        IfFailRet(StringCchCopyW(pFilePart, lengthof(fullPath) - (pFilePart - fullPath), searchPart));
    }

FileSearch:
    // Search for first matching file.
    hSearch = W_FindFirstFile(fullPath, &findData);
    if ((hSearch == 0 || hSearch == INVALID_HANDLE_VALUE)) {
        if (!bRecurse)
            g_output.ShowErrorIdString(ERR_FileNotFound, ERROR_ERROR, filename);
        return S_OK;
    }

    // Add this file name and all subsequent ones.
    for (;;) {
        if (!(findData.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN)) &&
            SUCCEEDED(hr = StringCchCopyW(pFilePart, lengthof(fullPath) - (pFilePart - fullPath), findData.cFileName))) {

            WCHAR temp[_MAX_PATH];
            WCBuffer tempBuffer(temp);

            bFound = true;
            if (0 == GetCanonFilePath( fullPath, tempBuffer, false)) {
                hr = HRESULT_FROM_WIN32(GetLastError());
            } else if (SUCCEEDED (hr = pInput->AddSourceFile (temp, &findData.ftLastWriteTime))) {
                *pfFilesAdded = TRUE;


            }

            if (hr == S_FALSE)
                g_output.ShowErrorIdString(WRN_FileAlreadyIncluded, ERROR_WARNING, fullPath);

            //compiler->AddInputFile(fullPath, findData.ftLastWriteTime);
        }

        if (FAILED (hr) || !W_FindNextFile(hSearch, &findData))
            break;
    }

    if (!bFound && !bRecurse)
        g_output.ShowErrorIdString(ERR_FileNotFound, ERROR_ERROR, filename);

    FindClose(hSearch);
    return hr;
}


////////////////////////////////////////////////////////////////////////////////
// BeginNewInputSet
//
// This function is called to make sure the given input set is "fresh", meaning
// it hasn't had any files added to it yet.

HRESULT BeginNewInputSet (ICSCompiler *pCompiler, CComPtr<ICSInputSet> &spInput, BOOL *pfFilesAdded)
{
    if (*pfFilesAdded)
    {
        // Need to create a new one
        HRESULT hr;
        spInput.Release();
        *pfFilesAdded = FALSE;
        hr = pCompiler->AddInputSet (&spInput);
        g_firstInputSet = false;
        if (SUCCEEDED(hr)) {
            // default for subsequent executables is DLL.
            hr = spInput->SetOutputFileType(OUTPUT_MODULE);
        }
        return hr;
    }

    // This one is still fresh...
    return S_OK;
}

HRESULT ResHelper (ICSInputSet * spInput, WCBuffer pszArg, int iOptLen, bool bEmbed)
{
    if (!pszArg)
        return E_INVALIDARG;

    HRESULT hr = E_FAIL;
    WCHAR szName[MAX_PATH];
    LPWSTR option = RemoveQuotesAndReplaceComma( pszArg + iOptLen);  // change ',' to '|'
    WCBuffer identBuffer = WCBuffer::CreateFrom(wcschr(option, L'|'));
    
    //default is public
    BOOL fPublic = true;
    if (identBuffer.Count() != 0) {
        identBuffer.AddAndIncrement(L'\0');
        WCBuffer visBuffer = identBuffer.wcschr(L'|');
        if (visBuffer.Count() != 0) {
            visBuffer.AddAndIncrement(L'\0');
            if (_wcsicmp(visBuffer, L"private") == 0)
                fPublic = false;
            else if (_wcsicmp(visBuffer, L"public") != 0) {
                // resources must be public or private
                g_output.ShowErrorIdString (ERR_BadResourceVis, ERROR_ERROR, visBuffer.Str());
                return E_FAIL;
            }
        }
    }

    
    if (*option == L'\0') {
        wcsncpy_s(szName, MAX_PATH, pszArg - 1, iOptLen + 1);
        szName[iOptLen + 1] = L'\0';
        g_output.ShowErrorIdString (ERR_NoFileSpec, ERROR_FATAL, szName);
        return E_FAIL;
    }

    if (!g_args.GetFullFileName(option, szName, false)) {
        return E_FAIL;
    }
    if (identBuffer.Count() == 0 || identBuffer[0] == L'\0') {
        // Identifier defaults to filename minus path
        LPWSTR pTemp = wcsrchr(option, L'\\');
#ifdef PLATFORM_UNIX
        LPWSTR pTemp2 = wcsrchr(option, L'/');
        if (pTemp2 > pTemp) {
            pTemp = pTemp2;
        }
#endif // PLATFORM_UNIX
        if (pTemp == NULL)
            identBuffer = WCBuffer::CreateFrom(option);
        else
            identBuffer = WCBuffer::CreateFrom(pTemp + 1);
    }

    IfFailRet(CheckResult (spInput->AddResourceFile (szName, identBuffer, bEmbed, fPublic)));
    if (hr == S_FALSE) {
        // pszIdent could be infinitely long, so truncate it so we don't overflow our
        // error buffer
        size_t  len = 0;
        HRESULT hr2 = StringCchLengthW (identBuffer, MAX_PATH, &len);
        if (FAILED (hr2) || len > MAX_PATH) {
            hr2 = StringCchCopyW (identBuffer.GetData() + MAX_PATH - 4, 4, L"...");
            ASSERT (SUCCEEDED (hr2));
        }
        g_output.ShowErrorIdString( ERR_ResourceNotUnique, ERROR_ERROR, identBuffer.Str());
        return E_FAIL;
    }

    return hr;
}


////////////////////////////////////////////////////////////////////////////////
// ParseCommandLine

HRESULT ParseCommandLine (ICSCompiler *pCompiler, ICSCompilerConfig *pConfig, int argc, WCBuffer argv[], UINT uiCodePage)
{
    HRESULT                 hr = E_FAIL;
    CComPtr<ICSInputSet>    spInput;
    long                    iOptionCount;
    long                    iWarnCount;
    const long              *pWarnIds;
    bool                    usedShort;
    bool                    madeAssembly = true;

    // First, get the compiler option count, and create the first input set.
    if (FAILED (hr = pConfig->GetOptionCount (&iOptionCount)) ||
        FAILED (hr = pCompiler->AddInputSet (&spInput)) ||
        FAILED (hr = pConfig->GetWarnNumbers (&iWarnCount, &pWarnIds)))
    {
        return CheckResult(hr);
    }

    CComBSTR    sbstrDefines, sbstrImports, sbstrModules, sbstrLibPaths;
    BOOL        fFilesAdded = FALSE, fResourcesAdded = FALSE, fModulesAdded = FALSE;

    // Okay, we're ready to process the command line
    for (int iArg = 0; iArg < argc; ++iArg)
    {
        // Skip blank args -- they've already been processed by ProcessPreSwitches or the like
        if (!argv[iArg].IsValid())
            continue;

        WCBuffer pszArg = argv[iArg].CreateTraversal();

        // If this is a switch, see what it is
        if (pszArg[0] == '-' || pszArg[0] == '/')
        {
            // Skip the switch leading char
            WCHAR chLeading;
            chLeading = pszArg[0];
            pszArg.MoveForward(1);
            usedShort = false;

            // Check for options we know about
            if (_wcsnicmp (pszArg, L"out:", 4) == 0)
            {
                if (pszArg[4] == '\0') { // Check for an empty string
                    g_output.ShowErrorIdString( ERR_NoFileSpec, ERROR_FATAL, (pszArg - 1));
                    break;
                }

                // Output file specification.  Make sure we have a fresh input set, and
                // set its output files name to the next arg
                WCHAR szFilename[MAX_PATH];
                if (!g_args.GetFullFileName(RemoveQuotes(pszArg + 4), szFilename, true)) {
                    hr = E_FAIL;
                    break;
                }
                if (FAILED (hr = CheckResult (BeginNewInputSet (pCompiler, spInput, &fFilesAdded))) ||
                    FAILED (hr = CheckResult (spInput->SetOutputFileName (szFilename))))
                    break;
            }

            else if ((usedShort = ((*pszArg == 'm' || *pszArg == 'M') && pszArg[1] == ':')) ||
                _wcsnicmp (pszArg, L"main:", 5) == 0)
            {
                int iOptLen = usedShort ? 2 : 5;
                if (pszArg[iOptLen] == '\0') { // Check for an empty string
                    g_output.ShowErrorIdString( ERR_SwitchNeedsString, ERROR_FATAL, (pszArg - 1));
                    break;
                }

                // Output file specification.  Make sure we have a fresh input set, and
                // set its output files name to the next arg
                if (FAILED (hr = spInput->SetMainClass( RemoveQuotes(pszArg + iOptLen))))
                {
                    if (hr == E_INVALIDARG)
                        g_output.ShowErrorIdString( ERR_NoMainOnDLL, ERROR_FATAL);
                    else
                        CheckResult(hr);
                    break;
                }
            }


            else if ((usedShort = (_wcsnicmp (pszArg, L"res:", 4) == 0)) ||
                (_wcsnicmp (pszArg, L"resource:", 9) == 0))
            {
                // Embedded resource
                if (SUCCEEDED(hr = ResHelper (spInput, pszArg, usedShort ? 4 : 9, true)))
                    fResourcesAdded = TRUE;
                else
                    break;
            }

            else if ((usedShort = (_wcsnicmp (pszArg, L"linkres:", 8)) == 0) || (_wcsnicmp (pszArg, L"linkresource:", 13) == 0))
            {
                // Link in a resource
                if (SUCCEEDED(hr = ResHelper (spInput, pszArg, usedShort ? 8 : 13, false)))
                    fResourcesAdded = TRUE;
                else
                    break;
            }

            else if ((usedShort = ((*pszArg == 'd' || *pszArg == 'D') && pszArg[1] == ':')) ||
                (_wcsnicmp(pszArg, L"define:", 7) == 0))
            {
                int iOptLen = (usedShort ? 2 : 7);
                LPCWSTR szOpt = RemoveQuotes(pszArg+iOptLen);
                if (*szOpt == '\0') // Check for an empty string
                    g_output.ShowErrorIdString( ERR_SwitchNeedsString, ERROR_ERROR, (pszArg - 1));

                // Conditional compilation symbol(s) -- we separate this one out ourselves to allow multiple instances to be accumulative
                if (sbstrDefines != NULL)
                    sbstrDefines += L";";
                sbstrDefines += szOpt;
            }

            else if ((usedShort = ((*pszArg == 'r' || *pszArg == 'R') && pszArg[1] == ':')) ||
                (_wcsnicmp(pszArg, L"reference:", 10) == 0))
            {
                int iOptLen = (usedShort ? 2 : 10);
                if (pszArg[iOptLen] == '\0') // Check for an empty string
                    g_output.ShowErrorIdString( ERR_NoFileSpec, ERROR_ERROR, (pszArg - 1));

                // imports switch has multiple file names seperated by |
                if (sbstrImports != NULL)
                    sbstrImports += L"|";

                LPCWSTR szRefs = RemoveQuotesAndReplaceAlias( pszArg+iOptLen); // change ';' or ',' to '|', and '=' to 'x01'
                LPCWSTR szFileSep = NULL;
                // Allow one initial and one terminating semicolon
                if (wcschr(szRefs, L'\x01') != NULL && (szFileSep = wcschr(szRefs + 1, L'|')) != NULL && szFileSep[1] != L'\0')
                    g_output.ShowErrorId( ERR_OneAliasPerRefernce, ERROR_ERROR);
                sbstrImports += szRefs;
            }

            else if (_wcsnicmp(pszArg, L"addmodule:", 10) == 0)
            {
                if (pszArg[10] == '\0') // Check for an empty string
                    g_output.ShowErrorIdString( ERR_NoFileSpec, ERROR_ERROR, (pszArg - 1));
                else
                    fModulesAdded = TRUE;

                // imports switch has multiple file names seperated by |
                if (sbstrModules != NULL)
                    sbstrModules += L"|";
                sbstrModules += RemoveQuotesAndReplacePathDelim( pszArg+10); // change ';' or ',' to '|');
            }

            else if (_wcsnicmp(pszArg, L"lib:", 4) == 0)
            {
                if (pszArg[4] == '\0') // Check for an empty string
                    g_output.ShowErrorIdString( ERR_NoFileSpec, ERROR_ERROR, (pszArg - 1));
                else
                {
                    // Similar to /D switch -- we allow multiple of these separated by '|'
                    if (sbstrLibPaths != NULL)
                        sbstrLibPaths += L"|";
                    sbstrLibPaths += RemoveQuotesAndReplacePathDelim( pszArg+4); // change ';' or ',' to '|'
                }
            }

            else if (_wcsnicmp (pszArg, L"baseaddress:", 12) == 0)
            {
                LPCWSTR unquoted = RemoveQuotes(pszArg + 12);
                LPCWSTR endCh;
                ULONGLONG imageBase = (ConsoleArgs::wcstoul64(unquoted, &endCh) + I64(0x00008000)) & UI64(0xFFFFFFFFFFFF0000);
                if (*unquoted == L'\0' || *endCh != L'\0' || (imageBase == 0 && errno == ERANGE))    // Invalid number
                {
                    g_output.ShowErrorIdString( ERR_BadBaseNumber, ERROR_ERROR, (pszArg + 12).Str());
                    continue;
                }

                // Set the image base.
                if (FAILED (hr = CheckResult (spInput->SetImageBase2( imageBase))))
                {
                    break;
                }
            }
            else if (wcscmp (pszArg, L"baseaddress") == 0)
            {
                g_output.ShowErrorIdString(ERR_SwitchNeedsNumber, ERROR_FATAL, argv[iArg].Str());
                continue;
            }


            else if (_wcsnicmp (pszArg, L"filealign:", 10) == 0)
            {
                LPCWSTR unquoted = RemoveQuotes(pszArg + 10);
                LPWSTR endCh;
                DWORD align = wcstoul(unquoted, &endCh, 0);
                if (*unquoted == L'\0' || *endCh != L'\0' || 
		                (align != 512 && align != 1024 && align != 2048 && align != 4096 && align != 8192))    // Invalid number
                {
                    g_output.ShowErrorIdString( ERR_BadFileAlignment, ERROR_ERROR, (pszArg + 10).Str());
                    continue;
                }

                // Set the alignment - this also does some checking.
                hr = spInput->SetFileAlignment( align);
                if (hr == E_INVALIDARG)
                    g_output.ShowErrorIdString( ERR_BadFileAlignment, ERROR_ERROR, (pszArg + 10).Str());
                else if (FAILED(hr = CheckResult(hr)))
                    break;
            }
            else if (wcscmp (pszArg, L"filealign") == 0)
            {
                g_output.ShowErrorIdString(ERR_SwitchNeedsNumber, ERROR_FATAL, argv[iArg].Str());
                continue;
            }
    
            else if ((usedShort = (_wcsnicmp(pszArg, L"t:", 2) == 0)) || _wcsnicmp(pszArg, L"target:", 7) == 0) 
            {
                // Begin a new input set.
                if (FAILED (hr = CheckResult (BeginNewInputSet (pCompiler, spInput, &fFilesAdded))))
                    break;

                WCBuffer subOption = pszArg.CreateTraversal();
                subOption.MoveForward(usedShort ? 2 : 7);
                if (wcscmp(subOption, L"module") == 0) {
                    if (g_firstInputSet)
                        madeAssembly = false;
                    if (FAILED (hr = CheckResult (spInput->SetOutputFileType(OUTPUT_MODULE))))
                    {
                        break;
                    }
                }
                else if (wcscmp(subOption, L"library") == 0) {
                    if (!g_firstInputSet)
                        g_output.ShowErrorIdString(ERR_BadTargetForSecondInputSet, ERROR_ERROR);
                    else {
                        if (FAILED (hr = CheckResult (spInput->SetOutputFileType(OUTPUT_LIBRARY))))
                            break;
                    }
                }
                else if (wcscmp(subOption, L"exe") == 0) {
                    if (!g_firstInputSet)
                        g_output.ShowErrorIdString(ERR_BadTargetForSecondInputSet, ERROR_ERROR);
                    else {
                        if (FAILED (hr = CheckResult (spInput->SetOutputFileType(OUTPUT_CONSOLE))))
                            break;
                    }
                }
                else if (wcscmp(subOption, L"winexe") == 0) {
                    if (!g_firstInputSet)
                        g_output.ShowErrorIdString(ERR_BadTargetForSecondInputSet, ERROR_ERROR);
                    else {
                        if (FAILED (hr = CheckResult (spInput->SetOutputFileType(OUTPUT_WINDOWS))))
                            break;
                    }
                }
                else {
                    g_output.ShowErrorIdString(FTL_InvalidTarget, ERROR_FATAL);
                }
                if (hr == S_FALSE) {
                    g_output.ShowErrorIdString( ERR_NoMainOnDLL, ERROR_FATAL);
                    hr = E_FAIL;
                    break;
                }
            }
            else if (_wcsnicmp (pszArg, L"recurse:", 8) == 0)
            {
                unsigned int iOptLen = usedShort ? 2 : 8;

                // Recursive file specification.
                if (wcslen(pszArg) <= iOptLen)
                {
                    g_output.ShowErrorIdString (ERR_NoFileSpec, ERROR_FATAL, pszArg - 1);
                    hr = E_FAIL;
                    break;
                }

                if (FAILED (hr = CheckResult (ProcessFileName (RemoveQuotes(pszArg + iOptLen), spInput, uiCodePage, &fFilesAdded, TRUE))))
                    break;
            }
            else if (_wcsicmp (pszArg, L"noconfig") == 0)
            {
                // '/noconfig' would've been handled earlier if it was on the original command line
                // so just give a warning that we're ignoring it.
                g_output.ShowErrorId (WRN_NoConfigNotOnCommandLine, ERROR_WARNING);
            }
            else if (_wcsnicmp (pszArg, L"pdb:", 4) == 0)
            {
                if (pszArg[4] == '\0') { // Check for an empty string
                    g_output.ShowErrorIdString( ERR_NoFileSpec, ERROR_FATAL, (pszArg - 1));
                    break;
                }

                // PDB Resource file specification.
                WCHAR szFilename[MAX_PATH];
                if (!g_args.GetFullFileName(RemoveQuotes(pszArg + 4), szFilename, false)) {
                    hr = E_FAIL;
                    break;
                }
                if (FAILED (hr = spInput->SetPDBFileName(szFilename))) {
                    hr = CheckResult(hr);
                    break;
                }
            }
            else if (_wcsicmp (pszArg, L"pdb") == 0)
            {
                g_output.ShowErrorIdString(ERR_NoFileSpec, ERROR_FATAL, (pszArg - 1));
                continue;
            }
            else
            {
                PCWSTR  pszShortSwitch, pszLongSwitch, pszDescSwitch;
                long    iOptionId;
                size_t  iArgLen;
                size_t  iSwitchLen = 0;
                DWORD   dwFlags;
                bool    match;

                // It isn't one we recognize, so it must be an option exposed by the compiler.
                // See if we can find it.
                long i;
                for (i=0; i<iOptionCount; i++)
                {
                    // Get the info for this switch
                    IfFailRet(CheckResult (pConfig->GetOptionInfoAtEx (i, &iOptionId, &pszShortSwitch, &pszLongSwitch, &pszDescSwitch, NULL, &dwFlags)));

                    // See if it matches the arg we have (up to length of switch name only)
                    iArgLen = wcslen (pszArg);
                    match = false;

                    // FYI: below test of pszArg[iSwitchLen] < 0x40 tests for '\0', ':', '+', '-', but not a letter -- e.g., make sure
                    // this isn't a prefix of some longer switch.

                    // Try new short option
                    if (pszShortSwitch)
                        match = (iArgLen >= (iSwitchLen = wcslen (pszShortSwitch))
                                && (_wcsnicmp (pszShortSwitch, pszArg, iSwitchLen) == 0
                                && pszArg[iSwitchLen] < 0x40 ));
                    // Try new long option
                    if (!match) {
                        ASSERT(pszLongSwitch);
                        match = (iArgLen >= (iSwitchLen = wcslen (pszLongSwitch))
                                && (_wcsnicmp (pszLongSwitch, pszArg, iSwitchLen) == 0
                                && pszArg[iSwitchLen] < 0x40 ));
                    }

                    if (match)
                    {
                        VARIANT vt;
                        CComBSTR sbstrValue; // holds the string for the VARIANT vt;

                        // We have a match, possibly.  If this is a string switch, we need to have
                        // a colon and some text.  If boolean, we need to check for optional [+|-]
                        if (dwFlags & COF_BOOLEAN)
                        {
                            VARIANT_BOOL    fValue;

                            // A boolean option -- look for [+|-]
                            if (iArgLen == iSwitchLen)
                                fValue = TRUE;
                            else if (iArgLen == iSwitchLen + 1 && (pszArg[iSwitchLen] == '-' || pszArg[iSwitchLen] == '+'))
                                fValue = (pszArg[iSwitchLen] == '+');
                            else
                                continue;   // Keep looking
                            V_VT(&vt) = VT_BOOL;
                            V_BOOL(&vt) = fValue;
                        }
                        else if (iOptionId == OPTID_WARNASERRORLIST)
                        {
                            BOOL fError = TRUE;
                            LPWSTR stringOpt;
                            // Boolean + String option.  Following the argument should be a plus, a minus or nothing
                            // then a colon, and we'll use whatever text follows as the string value for the option.

                            if (pszArg[iSwitchLen] == '-')
                            {
                                fError = FALSE;
                                iSwitchLen++;
                            }
                            else if (pszArg[iSwitchLen] == '+')
                                iSwitchLen++;

                            if (pszArg[iSwitchLen] == '\0')
                                stringOpt = NULL;
                            else if (pszArg[iSwitchLen] == ':')
                                stringOpt = RemoveQuotes((pszArg + iSwitchLen) + 1);
                            else
                                continue; // Must be something else

                            if (wcslen(stringOpt) == 0) {
                                g_output.ShowErrorIdString (ERR_SwitchNeedsString, ERROR_FATAL, argv[iArg].Str());
                                return E_FAIL;
                            }

                            if (stringOpt == NULL) {
                                iOptionId = OPTID_WARNINGSAREERRORS;

                                V_VT(&vt) = VT_BOOL;
                                V_BOOL(&vt) = fError;
                            } else {
                                WCHAR* pContext;
                                LPCWSTR pszTok = wcstok_s(stringOpt, L" ,;", &pContext);
                                LPWSTR  endPtr;
                                long    warnNumber;
                                long    *pIndex = NULL;
                                bool    fFail = false;

                                IfFailRet(CheckResult(pConfig->GetOption(OPTID_WARNASERRORLIST, &vt)));

                                ASSERT(V_VT(&vt) == VT_BSTR);
                                if (SysStringLen(V_BSTR(&vt)) < (unsigned)iWarnCount) {
                                    ASSERT(SysStringLen(V_BSTR(&vt)) == 0);
                                    if (NULL == (V_BSTR(&vt) = SysAllocStringLen( NULL, iWarnCount)))
                                        return CheckResult(E_OUTOFMEMORY);
                                    memset(V_BSTR(&vt), 0, iWarnCount * sizeof(WCHAR));
                                }

                                sbstrValue.Attach(V_BSTR(&vt));

                                while (pszTok) {
                                    warnNumber = wcstoul(pszTok, &endPtr, 10);

                                    if (*endPtr || (endPtr != pszTok + wcslen(pszTok)) ||
                                        NULL == (pIndex = (long*)bsearch( &warnNumber, pWarnIds, iWarnCount, sizeof(*pWarnIds), CompareLongs))) {
                                        g_output.ShowErrorIdString( WRN_BadWarningNumber, ERROR_WARNING, pszTok);
                                    } else {
                                        sbstrValue[pIndex - pWarnIds] = fError;
                                    }
                                    pszTok = wcstok_s( NULL, L" ,;", &pContext);
                                }

                                if (fFail)
                                    return E_INVALIDARG;
                            }
                        }
                        else
                        {
                            // String option.  Following the argument should be a colon, and we'll
                            // use whatever text follows as the string value for the option.
                            if (iArgLen == iSwitchLen)
                            {
                                g_output.ShowErrorIdString (ERR_SwitchNeedsString, ERROR_FATAL, argv[iArg].Str());
                                return E_FAIL;
                            }

                            if (pszArg[iSwitchLen] != ':')
                                continue;   // Keep looking

                            LPCWSTR stringOpt = RemoveQuotes(pszArg + iSwitchLen + 1);
                            if (stringOpt == NULL || wcslen(stringOpt) == 0) {
                                g_output.ShowErrorIdString (ERR_SwitchNeedsString, ERROR_FATAL, argv[iArg].Str());
                                return E_FAIL;
                            }
                            sbstrValue = stringOpt;

                            V_VT(&vt) = VT_BSTR;
                            V_BSTR(&vt) = sbstrValue;
                        }

                        // assemble is deprecated.
                        if (dwFlags & COF_WARNONOLDUSE) {
                            PCWSTR pszAltText = NULL;
                            //No deprecated options for now
                            //switch(iOptionId) {
                            //default:
                                VSFAIL("It's a deprecated switch but we don't know the replacement!");
                                pszAltText = L"unknown";
                            //    break;
                            //}
                            g_output.ShowErrorIdString(WRN_UseNewSwitch, ERROR_WARNING, pszDescSwitch, pszAltText);
                        }
                        
                        // nowarn should accumulate with existing value option, with "," separator.
                        if (iOptionId == OPTID_NOWARNLIST) {
                            VARIANT old;
                            VariantInit(&old);
                            if (SUCCEEDED(pConfig->GetOption(OPTID_NOWARNLIST, &old)) && V_VT (&old) == VT_BSTR) {
                                CComBSTR sbstrTemp;

                                sbstrTemp.Attach(V_BSTR (&old));
                                V_BSTR (&old) = NULL;

                                if (sbstrTemp.Length() > 0) {
                                    sbstrTemp += L",";
                                    sbstrTemp += V_BSTR (&vt);

                                    sbstrValue.Attach(sbstrTemp.Detach());

                                    V_VT(&vt) = VT_BSTR;
                                    V_BSTR(&vt) = sbstrValue;
                                }
                            }
                            VariantClear(&old);
                        }

                        // Got the option -- call in to set it
                        IfFailRet(CheckResult (pConfig->SetOption (iOptionId, vt)));

                        break;
                    }
                }

                if (i == iOptionCount)
                {
#ifdef PLATFORM_UNIX
                    if (chLeading == '/')
                    {
                        // On BSD it could be a file that starts with '/'
                        // Not a switch, so it might be a source file spec
                        if (FAILED (hr = CheckResult (ProcessFileName (RemoveQuotes(argv[iArg]), spInput, uiCodePage, &fFilesAdded, FALSE))))
                            break;
                    }
                    else
#endif
                    {
                        // Didn't find it...
                        g_output.ShowErrorIdString (ERR_BadSwitch, ERROR_FATAL, argv[iArg].Str());
                        return E_FAIL;
                    }
                }
            }
        }
        else
        {
            // Not a switch, so it must be a source file spec
            if (FAILED (hr = CheckResult (ProcessFileName (RemoveQuotes(argv[iArg]), spInput, uiCodePage, &fFilesAdded, FALSE))))
                break;
        }
    }

    if (FAILED (hr))
        return hr;

    // Make sure at least one "output" was created successfully
    // Non assemblies must have sources, but assemblies may be resource-only or only have addmodules
    if ((!madeAssembly && !fFilesAdded) || (madeAssembly && !fFilesAdded && (!g_firstInputSet || (!fResourcesAdded && !fModulesAdded))))
    {
        if (g_firstInputSet)
            g_output.ShowErrorId (ERR_NoSources, ERROR_FATAL);
        else
            g_output.ShowErrorId (ERR_NoSourcesInLastInputSet, ERROR_FATAL);
        return E_FAIL;
    }

    // Don't forget out our /D switch accumulation...
    if (sbstrDefines != NULL)
    {
        VARIANT vt;
        V_VT(&vt) = VT_BSTR;
        // No need to allocate a new BSTR and Free it in this case
        V_BSTR(&vt) = sbstrDefines;

        IfFailRet(CheckResult (pConfig->SetOption (OPTID_CCSYMBOLS, vt)));
    }

    // ...or our import specifications
    if (sbstrImports != NULL)
    {
        VARIANT vt;
        V_VT(&vt) = VT_BSTR;
        // No need to allocate a new BSTR and Free it in this case
        V_BSTR(&vt) = sbstrImports;

        IfFailRet(CheckResult (pConfig->SetOption (OPTID_IMPORTS, vt)));
    }

    // ...or our added modules
    if (sbstrModules != NULL)
    {
        VARIANT vt;
        V_VT(&vt) = VT_BSTR;
        // No need to allocate a new BSTR and Free it in this case
        V_BSTR(&vt) = sbstrModules;

        IfFailRet(CheckResult (pConfig->SetOption (OPTID_MODULES, vt)));
    }

    // ...or the LIB path
    if (sbstrLibPaths != NULL)
    {
        VARIANT vt;
        V_VT(&vt) = VT_BSTR;
        // No need to allocate a new BSTR and Free it in this case
        V_BSTR(&vt) = sbstrLibPaths;

        IfFailRet(CheckResult (pConfig->SetOption (OPTID_LIBPATH, vt)));
    }

    CComPtr<ICSError>   spError;

    // Okay, we've successfully finished parsing options.  Now we must validate the settings.
    IfFailRet(CheckResult (pConfig->CommitChanges (&spError)));

    if (hr == S_FALSE)
    {
        ASSERT (spError != NULL);

        // Something with the compiler settings is no good.

        hr = E_FAIL;

        ERRORKIND errorKind;
        if (SUCCEEDED (spError->GetErrorInfo(NULL, &errorKind, NULL)))
        {
            if (ERROR_WARNING == errorKind)
                hr = S_FALSE;
        }

        CompilerHost::ReportError (spError, &g_output);
    }

    if (SUCCEEDED(hr) && g_output.HadError())
        hr = E_FAIL;

    return hr;
}

//
// Main entry point
//
int __cdecl main(int argcIgnored, __in_ecount(argcIgnored) char **argvIgnored)
{
    HRESULT hr = E_FAIL;  // assume failure unless otherwise.
    LARGE_INTEGER timeStart, timeStop, frequency;
    bool timeCompiler = false;
    if (!PAL_RegisterLibraryW(L"rotor_palrt") ||
        !PAL_RegisterLibraryW(L"sscoree"))
    {
        g_output.ShowError(errorInfo[FTL_NoMessagesDLL].number, ERROR_FATAL, NULL, -1, -1, -1, -1, L"Unable to load critical libraries");
        return 1;
    }

    VsDebugInitialize();


    // Initialize Unilib
    W_IsUnicodeSystem();

    if (!g_output.Initialize(GetMessageDll(), MESSAGE_DLLW, errorInfo))
        goto FAIL;

    // The first part of command line processing is done as a linked list of
    // arguments, which is created, added to, and manipulated by several functions.
    // "vals" is the beginning of the list. "valsLast" points to where to add to the end.

    // Get command line, process the initial repsonse files, split it into arguments.
    int argc;
    WCBuffer * argv; argv = NULL;
    if (!g_args.GetArgs( &argc, &argv))
        goto FAIL;

    // Do initial switch processing.
    bool noLogo, showHelp;
    UINT uiCodePage; uiCodePage = 0;
    ProcessPreSwitches(argc, argv, &noLogo, &showHelp, &timeCompiler, &uiCodePage);

    // All errors previous to this are fatal
    // This solve the error-before-logo problem
    if (g_output.HadError())
        goto FAIL;

    // Print the logo banner, unless noLogo was specified.
    if (!noLogo)
        g_output.PrintBanner();

    // If timing, start the timer.
    if (timeCompiler) {
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&timeStart);
    }


    {
        CompilerHost        host(&g_output);
        ICSCompiler         *compiler = NULL;
        ICSCompilerFactory  *pFactory = NULL;
        ICSCompilerConfig   *pConfig = NULL;
        const DWORD COMPILER_FLAGS = CCF_TRACKCOMMENTS;

        // Instanciate the compiler factory and create a compiler.  Let it
        // create its own name table.
        if (FAILED (hr = CreateCompilerFactory (&pFactory)) ||
            FAILED (hr = pFactory->CreateCompiler (COMPILER_FLAGS, &host, NULL, &compiler)) ||
            FAILED (hr = compiler->GetConfiguration (&pConfig)))
        {
            if (pFactory != NULL)
                pFactory->Release();
            if (compiler != NULL)
                compiler->Release();
            g_output.ShowErrorIdString(ERR_InitError, ERROR_FATAL, g_output.ErrorHR(hr));
            goto FAIL;
        }

        host.SetCompiler (compiler);
        host.SetCodePage (uiCodePage);
        if (showHelp)
        {
            // Display help and do nothing else
            hr = PrintHelp (pConfig);
        }
        else
        {
            // Parse the command line, and if successful, compile.
            if (SUCCEEDED (hr = ParseCommandLine (compiler, pConfig, argc, argv, uiCodePage)))
            {
                hr = compiler->Compile (NULL);

                CheckResult(hr);
            }
        }

        if (pConfig != NULL)
            pConfig->Release();

        if (compiler)
        {
            compiler->Shutdown();
            compiler->Release();
        }

        if (pFactory != NULL)
            pFactory->Release();
    }

FAIL:


    g_args.CleanUpArgs();

    if (timeCompiler) {
        double elapsedTime;
        QueryPerformanceCounter(&timeStop);
        elapsedTime = (double) (timeStop.QuadPart - timeStart.QuadPart) / (double) frequency.QuadPart;
        g_output.print(L"\nElapsed time: %#.4g seconds\n", elapsedTime);
    }

    g_output.CloseBugReport();


    // Return success code.
    return (hr != S_OK || g_output.HadError()) ? 1 : 0;
}


