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
// File: alink.cpp
// 
// The command line driver for the assembly linker.
// ===========================================================================

#include "stdafx.h"

#include "msgsids.h"
#include "alink.h"
#include "common.h"
#include "merge.h"
#include "alinkexe.h"

#include <palstartup.h>

#define CLIENT_IS_ALINK
#include "consoleoutput.cpp"
#include "consoleargs.cpp"
#undef CLIENT_IS_ALINK

// Options
LPWSTR          g_szAssemblyFile = NULL;
LPWSTR          g_szAppMain = NULL;
LPWSTR          g_szCopyAssembly = NULL;
VARIANT         g_Options[optLastAssemOption];
FileList       *g_pFileListHead = NULL;
FileList      **g_ppFileListEnd = &g_pFileListHead;
ResList        *g_pResListHead = NULL;
ResList       **g_ppResListEnd = &g_pResListHead;
ULONGLONG       g_DllBase = 0;
bool            g_bNoLogo = false;
bool            g_bShowHelp = false;
bool            g_bTime = false;
bool            g_bFullPaths = false;
bool            g_bClean = false;
bool            g_bMake = false;
bool            g_bNoRefHash = false;
TargetType      g_Target = ttDll;
DWORD           g_dwComImageFlags = COMIMAGE_FLAGS_ILONLY;
DWORD           g_dwCeeCreateFlags = ICEE_CREATE_FILE_PURE_IL;
DWORD           g_dwPEKind = peILonly;
DWORD           g_dwMachine = IMAGE_FILE_MACHINE_I386;
bool            g_fullPaths = false;
bool            g_bUnicode = false;
HINSTANCE       g_hinstMessages = NULL;
HMODULE         g_hmodCorPE = NULL;
_optionused     g_bCopyOptions;


ConsoleOutput   g_output;
ConsoleArgs     g_args(&g_output);

// Runtime Version stuff?
HRESULT (__stdcall *g_pfnCreateCeeFileGen)(ICeeFileGen **ceeFileGen) = NULL; // call this to instantiate
HRESULT (__stdcall *g_pfnDestroyCeeFileGen)(ICeeFileGen **ceeFileGen) = NULL; // call this to delete

#ifndef DEBUG
// These are used by Unilib in retail builds
void * VSRetAlloc(SIZE_T sz) { return VSAlloc(sz); }
int    VSRetFree(void *pMem) { return (int)(VSFree(pMem) != NULL); }
#endif // DEBUG

/*
 * Print the help text for the compiler.
 */
HRESULT PrintHelp ()
{
    // The help text is numbered 10, 20, 30... so there is
    // ample room to put in additional strings in the middle.

    g_output.PrintString(IDS_HELP10, true);      // "Usage:"
    g_output.PrintString(IDS_HELP20, true);      // "Options:"

    g_output.PrintBlankLine();
    // Load and print the options
    for (int i=IDS_HELP20 + 1; i <= IDS_H_LASTOPTION; i++)
    {
        g_output.PrintString(i, true);
    }

    g_output.PrintBlankLine();

    // Load and print the Source statements
    g_output.PrintString(IDS_HELP30, true);
    g_output.PrintString(IDS_H_SOURCEFILE, true);
    g_output.PrintString(IDS_H_EMBED1, true);
    g_output.PrintString(IDS_H_EMBED2, true);
    g_output.PrintString(IDS_H_LINK1, true);
    g_output.PrintString(IDS_H_LINK2, true);
    g_output.PrintBlankLine();

    return S_OK;
}

/*
 * Process the "pre-switches": /nologo, /?, /help, /time, /repro, /fullpaths. If they exist,
 * they are nulled out.
 */
void ProcessPreSwitches(int argc, WCBuffer argv[])
{
    WCBuffer arg;

    if (argc < 1)
    {
        g_bShowHelp = true;
        return;
    }


    // Now do all the other options
    for (int iArg = 0; iArg < argc; ++iArg)
    {
        arg = argv[iArg];
        if (!arg.IsValid())
            continue;
        if (arg[0] == '-' || arg[0] == '/')
            arg.MoveForward(1);
        else
            continue;  // Not an option.

        if (_wcsicmp(arg, L"nologo") == 0)
            g_bNoLogo = true;
        else if (_wcsicmp(arg, L"?") == 0)
            g_bShowHelp = true;
        else if (_wcsicmp(arg, L"help") == 0)
            g_bShowHelp = true;
        else if (_wcsicmp(arg, L"time") == 0)
            g_bTime = true;
        else if (_wcsicmp(arg, L"fullpaths") == 0)
            g_bFullPaths = true;
        else
            continue;       // Not a recognized argument.

        argv[iArg].Invalidate();  // NULL out recognized argument.
    }
}

// Check a filename against all added modules
// Returns true and reports a warning if it was a duplicate
bool DupFileNameCheck(LPCWSTR pszFileName)
{
    // Check to see if a filename is a duplicate
    for (FileList *files = g_pFileListHead; files != NULL; files = files->next) {
        if ((files->arg.pszTarget && _wcsicmp(files->arg.pszTarget, pszFileName) == 0) ||
                _wcsicmp(files->arg.pszSource, pszFileName) == 0)
        {
            g_output.ShowErrorIdString(WRN_IgnoringDuplicateSource, ERROR_WARNING, pszFileName);
            return true;
        }
    }
    return false;
}

resource * ResHelper (WCBuffer pszArg, int iOptLen, bool bEmbed)
{
    WCHAR szName[MAX_PATH];
    WCHAR * option = RemoveQuotesAndReplacePathDelim(pszArg + iOptLen);  // change ',' to '|'

    WCHAR* pContext;
    WCHAR * pTemp = wcstok_s( option, L"|", &pContext);

    // Prefix seems to think there's a chance that pszArg + iOptLen will wrap around and
    // be equal to NULL!?!?!?
    if (!option || *option == L'\0') {
        wcsncpy_s(szName, MAX_PATH, pszArg - 1, iOptLen + 1);
        szName[iOptLen + 1] = L'\0';
        g_output.ShowErrorIdString (ERR_NoFileSpec, ERROR_ERROR, szName);
        return NULL;
    }

    if (!g_args.GetFullFileName(pTemp, szName, false)) {
        return NULL;
    }

    resource *res = new resource();
    res->bEmbed = bEmbed;
    res->dwOffset = 0;
    res->pszSource = VSAllocStr(szName);

    if (res->pszSource == NULL) {
        g_output.ShowErrorId (FTL_NoMemory, ERROR_FATAL);
        delete res;
        return NULL;
    }

    if ((pTemp = wcstok_s( NULL, L"|", &pContext)) == NULL) {
        LPWSTR pName;
        // Identifier defaults to filename minus path
        if ((pName = wcsrchr(szName, L'\\')) == NULL)
            res->pszIdent = VSAllocStr(szName); // defaults to filename minus path
        else
            res->pszIdent = VSAllocStr(pName + 1);
        if (res->pszIdent == NULL) {
            delete res;
            g_output.ShowErrorId (FTL_NoMemory, ERROR_FATAL);
            return NULL;
        }
        res->bVis = true; // default
    } else {
        res->pszIdent = VSAllocStr(pTemp);
        if (res->pszIdent == NULL) {
            delete res;
            g_output.ShowErrorId (FTL_NoMemory, ERROR_FATAL);
            return NULL;
        }
        pTemp = wcstok_s( NULL, L"|", &pContext);
        if (!bEmbed && pTemp == NULL) {
            res->pszTarget = VSAllocStr(szName);    // defualt to filename
            if (res->pszTarget == NULL) {
                delete res;
                g_output.ShowErrorId (FTL_NoMemory, ERROR_FATAL);
                return NULL;
            }
            res->bVis = true;
        } else {
            if (!bEmbed) {
                if (!g_args.GetFullFileName( pTemp, szName, true)) {
                    delete res;
                    g_output.ShowErrorId (FTL_NoMemory, ERROR_FATAL);
                    return NULL;
                }
                res->pszTarget = VSAllocStr(szName);
                if (res->pszTarget == NULL) {
                    delete res;
                    g_output.ShowErrorId (FTL_NoMemory, ERROR_FATAL);
                    return NULL;
                }
                pTemp = wcstok_s( NULL, L"|", &pContext);
            }
            if (pTemp == NULL)
                res->bVis = true;
            else if (_wcsicmp(pTemp, L"private") == 0)
                res->bVis = false;
            else {
                pszArg.SetAt(iOptLen - 1, L'\0');
                g_output.ShowErrorIdString (ERR_BadOptionValue, ERROR_ERROR, pszArg.Str(), pTemp);
                delete res;
                return NULL;
            }
        }
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////
// ParseCommandLine

HRESULT ParseCommandLine (int argc, WCBuffer argv[])
{
    HRESULT                 hr = S_OK;
    bool                    usedShort;
    tree<const wchar_t*>    DupResCheck(L"Security.Evidence");

    // Okay, we're ready to process the command line
    for (int iArg = 0; iArg < argc; ++iArg)
    {
        // Skip blank args -- they've already been processed by ProcessPreSwitches or the like
        if (!argv[iArg].IsValid() || wcslen(argv[iArg]) == 0)
            continue;

        WCBuffer pszArg = argv[iArg];

        // If this is a switch, see what it is
        // NOTE: include "," here so that we give a good error message
        if (pszArg[0] == '-' || pszArg[0] == '/' || pszArg[0] == ',')
        {
            // Skip the switch leading char
            pszArg.MoveForward(1);
            usedShort = false;

            // Check for options we know about
            // NOTE: If you add an option here, be sure to add the non-colon version
            // in the else clause for better error messages
            if (_wcsnicmp (pszArg, L"out:", 4) == 0)
            {
                if (pszArg[4] == '\0') { // Check for an empty string
                    pszArg.SetAt(3, L'\0');
                    g_output.ShowErrorIdString( ERR_NoFileSpec, ERROR_ERROR, pszArg.Str());
                    hr = S_FALSE;
                    continue;
                }

                WCHAR szName[MAX_PATH];
                if (!g_args.GetFullFileName(RemoveQuotes(pszArg + 4), szName, true))
                    continue;
                if (NULL == (g_szAssemblyFile = VSAllocStr(szName))) {
                    hr = E_OUTOFMEMORY;
                    break;
                }
                g_bMake = true;
            }

            else if (_wcsnicmp (pszArg, L"template:", 9) == 0)
            {
                if (pszArg[9] == '\0') { // Check for an empty string
                    pszArg.SetAt(8, L'\0');
                    g_output.ShowErrorIdString( ERR_NoFileSpec, ERROR_ERROR, pszArg.Str());
                    hr = S_FALSE;
                    continue;
                }

                // Parent Assembly Info
                WCHAR szName[MAX_PATH];
                if (!g_args.GetFullFileName(RemoveQuotes(pszArg + 9), szName, false))
                    continue;
                g_szCopyAssembly = VSAllocStr(szName);
            }

            else if ((usedShort = (_wcsnicmp (pszArg, L"embed:", 6) == 0)) ||
                (_wcsnicmp (pszArg, L"embedresource:", 14) == 0))
            {
                resource * res = ResHelper( pszArg, usedShort ? 6 : 14, true);
                if (res == NULL) {
                    hr = S_FALSE;
                    continue;
                }

                hr = DupResCheck.Add( res->pszIdent, wcscmp);
                if (hr == E_OUTOFMEMORY)
                    g_output.ShowErrorId(FTL_NoMemory, ERROR_FATAL);
                else if (hr == S_FALSE)
                    g_output.ShowErrorIdString(ERR_DupResourceIdent, ERROR_ERROR, res->pszIdent);
                else
                {
                    *g_ppResListEnd = new ResList();
                    (*g_ppResListEnd)->arg = res;
                    g_ppResListEnd = &(*g_ppResListEnd)->next;
                    continue;
                }
                delete res;
            }

            else if ((usedShort = (_wcsnicmp (pszArg, L"link:", 5)) == 0) || (_wcsnicmp (pszArg, L"linkresource:", 13) == 0))
            {
                // Link in a resource
                resource * res = ResHelper( pszArg, usedShort ? 5 : 13, false);
                if (res == NULL) {
                    hr = S_FALSE;
                    continue;
                }

                hr = DupResCheck.Add( res->pszIdent, wcscmp);
                if (hr == E_OUTOFMEMORY)
                    g_output.ShowErrorId(FTL_NoMemory, ERROR_FATAL);
                else if (hr == S_FALSE)
                    g_output.ShowErrorIdString(ERR_DupResourceIdent, ERROR_ERROR, res->pszIdent);
                else
                {
                    *g_ppResListEnd = new ResList();
                    (*g_ppResListEnd)->arg = res;
                    g_ppResListEnd = &(*g_ppResListEnd)->next;
                    continue;
                }

                // cleanup a failed resource
                delete res;
            }

            // Evidence is nothing more than a glorified Embedded resource
            else if ((usedShort = (_wcsnicmp (pszArg, L"e:", 2)) == 0) || (_wcsnicmp (pszArg, L"evidence:", 9) == 0))
            {
                // Embedded resource
                WCHAR szName[MAX_PATH];
                int iOptLen = (usedShort ? 2 : 9);
                if (wcslen(pszArg + iOptLen) == 0) {
                    pszArg.SetAt(iOptLen - 1, L'\0');
                    g_output.ShowErrorIdString (ERR_NoFileSpec, ERROR_ERROR, pszArg.Str());
                    hr = S_FALSE;
                    continue;
                } else {
                    if (!g_args.GetFullFileName( RemoveQuotes(pszArg + iOptLen), szName, false))
                        continue;
                }

                resource *res = new resource();
                if (res == NULL) {
                    hr = E_OUTOFMEMORY;
                    break;
                }
                res->bEmbed = true;
                res->dwOffset = 0;
                if (NULL == (res->pszSource = VSAllocStr(szName)))
                    goto EVI_PARSE_OOM;

                if (NULL == (res->pszIdent = VSAllocStr(DupResCheck.name)))  // "Security.Evidence"
                    goto EVI_PARSE_OOM;

                res->bVis = false;

                if (NULL == (*g_ppResListEnd = new ResList())) {
EVI_PARSE_OOM:      if (res != NULL) {
                        delete res;
                        res = NULL;
                    }
                    hr = E_OUTOFMEMORY;
                    break;
                }
                (*g_ppResListEnd)->arg = res;
                g_ppResListEnd = &(*g_ppResListEnd)->next;
            }

            else if ((usedShort = (_wcsnicmp (pszArg, L"base:", 5) == 0)) ||
                (_wcsnicmp(pszArg, L"baseaddress:", 12) == 0))
            {
                LPCWSTR endCh;
                int iOptLen = (usedShort ? 5 : 12);
                ULONGLONG imageBase = (ConsoleArgs::wcstoul64( RemoveQuotes(pszArg + iOptLen), &endCh) + UI64(0x00008000)) & UI64(0xFFFFFFFFFFFF0000); // Round the low order word to align it
                if (*endCh == L'\0' && endCh == (pszArg + iOptLen)) // No number
                {
                    pszArg.SetAt(iOptLen - 1, L'\0');
                    g_output.ShowErrorIdString( ERR_SwitchNeedsString, ERROR_ERROR, pszArg.Str());
                    hr = S_FALSE;
                    continue;
                }
                else if (*endCh != L'\0' || (imageBase == 0 && errno == ERANGE))    // Invalid number
                {
                    pszArg.SetAt(iOptLen - 1, L'\0');
                    g_output.ShowErrorIdString( ERR_BadOptionValue, ERROR_ERROR, pszArg.Str(), (pszArg + iOptLen + 1).Str());
                    hr = S_FALSE;
                    continue;
                }

                // Set the image base.
                g_DllBase = imageBase;
            }

            else if ((usedShort = (_wcsnicmp (pszArg, L"t:", 2) == 0)) ||
                (_wcsnicmp(pszArg, L"target:", 7) == 0))
            {
                int iOptLen = (usedShort ? 2 : 7);
                LPWSTR pType = RemoveQuotes(pszArg + iOptLen);
                if (_wcsicmp(pType, L"lib") == 0 || _wcsicmp(pType, L"library") == 0)
                    g_Target = ttDll;
                else if (_wcsicmp(pType, L"exe") == 0)
                    g_Target = ttConsole;
                else if (_wcsicmp(pType, L"win") == 0 || _wcsicmp(pType, L"winexe") == 0)
                    g_Target = ttWinApp;
                else {
                    pszArg.SetAt(iOptLen - 1, 0);
                    g_output.ShowErrorIdString( ERR_BadOptionValue, ERROR_ERROR, pszArg.Str(), pType);
                    hr = S_FALSE;
                    continue;
                }
            }

            else if (_wcsnicmp (pszArg, L"main:", 5) == 0)
            {
                if (wcslen(pszArg + 5) <= 0) {
                    pszArg.SetAt(4, 0);
                    g_output.ShowErrorIdString (ERR_SwitchNeedsString, ERROR_ERROR, pszArg.Str());
                    hr = S_FALSE;
                } else {
                    if (NULL == (g_szAppMain = VSAllocStr(RemoveQuotes(pszArg + 5)))) {
                        hr = E_OUTOFMEMORY;
                        break;
                    }
                }
            }

            else if (_wcsnicmp (pszArg, L"platform:", 9) == 0)
            {
                LPWSTR pszPlatform = RemoveQuotes(pszArg + 9);
                if (*pszPlatform == L'\0'){
                    pszArg.SetAt(8, 0);
                    g_output.ShowErrorIdString (ERR_SwitchNeedsString, ERROR_ERROR, pszArg.Str());
                    hr = S_FALSE;
                } else if (_wcsicmp(pszPlatform, L"anycpu") == 0) {
                    g_dwComImageFlags = COMIMAGE_FLAGS_ILONLY;
                    g_dwCeeCreateFlags = ICEE_CREATE_FILE_PURE_IL;
                    g_dwPEKind = peILonly;
                    g_dwMachine = IMAGE_FILE_MACHINE_I386;
                } else if (_wcsicmp(pszPlatform, L"x86") == 0) {
                    g_dwComImageFlags = COMIMAGE_FLAGS_ILONLY | COMIMAGE_FLAGS_32BITREQUIRED;
                    g_dwCeeCreateFlags = ICEE_CREATE_FILE_PURE_IL;
                    g_dwPEKind = peILonly | pe32BitRequired;
                    g_dwMachine = IMAGE_FILE_MACHINE_I386;
                } else if (_wcsicmp(pszPlatform, L"itanium") == 0) {
                    g_dwComImageFlags = COMIMAGE_FLAGS_ILONLY;
                    g_dwCeeCreateFlags = ICEE_CREATE_FILE_PE64 | ICEE_CREATE_MACHINE_IA64;
                    g_dwPEKind = peILonly | pe32Plus;
                    g_dwMachine = IMAGE_FILE_MACHINE_IA64;
                } else if (_wcsicmp(pszPlatform, L"x64") == 0) {
                    g_dwComImageFlags = COMIMAGE_FLAGS_ILONLY;
                    g_dwCeeCreateFlags = ICEE_CREATE_FILE_PE64 | ICEE_CREATE_MACHINE_AMD64;
                    g_dwPEKind = peILonly | pe32Plus;
                    g_dwMachine = IMAGE_FILE_MACHINE_AMD64;
                } else {
                    pszArg.SetAt(8, 0);
                    g_output.ShowErrorIdString( ERR_BadOptionValue, ERROR_ERROR, pszArg.Str(), pszPlatform);
                    hr = S_FALSE;
                }
            }


            else
            {
                size_t  iArgLen;
                size_t  iSwitchLen = 0;
                bool    match;

                // It isn't one we recognize, so it must be an option exposed by the compiler.
                // See if we can find it.
                iArgLen = wcslen (pszArg);
                long i;
                for (i=0; i<optLastAssemOption; i++)
                {
                    if ((Options[i].flag & 0x40) != 0)
                        // This options has been removed, do not use it
                        continue;
                    // See if it matches the arg we have (up to length of switch name only)
                    match = false;

                    // FYI: below test of pszArg[iSwitchLen] < 0x40 tests for '\0', ':', '+', '-', but not a letter -- e.g., make sure
                    // this isn't a prefix of some longer switch.

                    // Try new short option
                    if (Options[i].ShortName)
                        match = (iArgLen >= (iSwitchLen = wcslen (Options[i].ShortName))
                                && (_wcsnicmp (Options[i].ShortName, pszArg, iSwitchLen) == 0
                                && pszArg[iSwitchLen] < 0x40 ));

                    // Try long option if it exists
                    if (!match && Options[i].LongName) {
                        ASSERT(Options[i].LongName);
                        match = (iArgLen >= (iSwitchLen = wcslen (Options[i].LongName))
                                && (_wcsnicmp (Options[i].LongName, pszArg, iSwitchLen) == 0
                                && pszArg[iSwitchLen] < 0x40 ));
                    }

                    if (match)
                    {
                        // We have a match, possibly.  If this is a string switch, we need to have
                        // a colon and some text.  If boolean, we need to check for optional [+|-]
                        if (Options[i].vt == VT_BOOL)
                        {
                            // A boolean option -- look for [+|-]
                            if (iArgLen == iSwitchLen) {
                                V_VT(&g_Options[i]) = VT_BOOL;
                                V_BOOL(&g_Options[i]) = VARIANT_TRUE;
                            } else if (iArgLen == iSwitchLen + 1 && (pszArg[iSwitchLen] == '-' || pszArg[iSwitchLen] == '+')) {
                                V_VT(&g_Options[i]) = VT_BOOL;
                                V_BOOL(&g_Options[i]) = (pszArg[iSwitchLen] == '+') ? VARIANT_TRUE : VARIANT_FALSE;
                            } else
                                continue;   // Keep looking
                        }
                        else
                        {
                            ASSERT(Options[i].vt == VT_BSTR || Options[i].vt == VT_UI4);
                            
                            // String option.  Following the argument should be a colon, and we'll
                            // use whatever text follows as the string value for the option.
                            if (iArgLen <= iSwitchLen + 1)
                            {
                                if (Options[i].vt == VT_BSTR)
                                    g_output.ShowErrorIdString (ERR_SwitchNeedsString, ERROR_ERROR, pszArg.Str());
                                else
                                    g_output.ShowErrorIdString (ERR_MissingOptionArg, ERROR_ERROR, pszArg.Str());
                                break;
                            }

                            if (pszArg[iSwitchLen] != ':')
                                continue;   // Keep looking

                            if (Options[i].vt == VT_BSTR) {                                
                                LPWSTR pwz = RemoveQuotes(pszArg + iSwitchLen + 1);
                                WCHAR buffer[MAX_PATH];
    
                                if ((Options[i].flag & 0x10) != 0) {
                                    // Get full paths for filename options      
                                    if (g_args.GetFullFileName( pwz, buffer, false)) {
                                        pwz = buffer;
                                    }
                                }

                                VARIANT var;
                                V_VT(&var) = VT_BSTR;
                                V_BSTR(&var) = SysAllocString(pwz);
                                if (!AddVariantOption(g_Options[i], var, !!(Options[i].flag & 0x04))) {
                                    VariantClear(&var);
                                    return E_OUTOFMEMORY;
                                }
                            } else {
                                LPWSTR pEnd = NULL;
                                ULONG val = wcstoul(RemoveQuotes(pszArg + iSwitchLen + 1), &pEnd, 16);
                                if (*pEnd != L'\0') { // Invalid Value!
                                    pszArg.SetAt((int)iSwitchLen, L'\0');
                                    g_output.ShowErrorIdString (ERR_BadOptionValue, ERROR_ERROR, pszArg.Str(), (pszArg + iSwitchLen + 1).Str());
                                } else {
                                    VARIANT var;
                                    V_VT(&var) = VT_UI4;
                                    V_UI4(&var) = val;
                                    if (!AddVariantOption(g_Options[i], var, !!(Options[i].flag & 0x04))) {
                                        VariantClear(&var);
                                        return E_OUTOFMEMORY;
                                    }
                                }
                            }

                            if ((Options[i].flag & 0x20) != 0) {
                                g_output.ShowErrorIdString (WRN_FeatureDeprecated, ERROR_WARNING, pszArg.Str(), L"nothing");
                            }
                        }

                        break;
                    }
                }

                if (i == optLastAssemOption)
                {
                    // Check for normal options minus the colon
                    // So we can give a better error message
                    if (_wcsicmp(pszArg, L"out") == 0 ||
                        _wcsicmp(pszArg, L"template") == 0 ||
                        _wcsicmp(pszArg, L"embed") == 0 || _wcsicmp(pszArg, L"embedresource") == 0 ||
                        _wcsicmp(pszArg, L"link") == 0 || _wcsicmp(pszArg, L"linkresource") == 0 ||
                        _wcsicmp(pszArg, L"e") == 0 || _wcsicmp(pszArg, L"evidence") == 0)
                        // Options that need a file-spec
                        g_output.ShowErrorIdString( ERR_NoFileSpec, ERROR_ERROR, pszArg.Str());

                    else if (_wcsicmp(pszArg, L"base") == 0 || _wcsicmp(pszArg, L"baseaddress") == 0 ||
                        _wcsicmp(pszArg, L"t") == 0 || _wcsicmp(pszArg, L"target") == 0 ||
                        _wcsicmp(pszArg, L"main") == 0)
                        // Options that need a string
                        g_output.ShowErrorIdString( ERR_SwitchNeedsString, ERROR_ERROR, pszArg.Str());

                    else if (pszArg[0] == L'\0')
                        g_output.ShowErrorIdString (ERR_BadSwitch, ERROR_ERROR, argv[iArg].Str());
                    
#if PLATFORM_UNIX
                    else if (pszArg[-1] == '/') 
                        // On BSD it could be a fully qualified file that starts with '/'
                        // Not a switch, so it might be a FileName
                        goto AssumeFileName;
#endif // PLATFORM_UNIX

                    else 
                        // Didn't find it...
                        g_output.ShowErrorIdString (ERR_BadSwitch, ERROR_ERROR, pszArg.Str());

                    hr = S_FALSE;
                }
            }
        }
        else
        {
#ifdef PLATFORM_UNIX
AssumeFileName:
#endif
            // Not a switch, so it must be a file spec
            WCHAR szSrcName[MAX_PATH];
            WCHAR szDestName[MAX_PATH];
            LPWSTR pArg = RemoveQuotesAndReplaceComma(argv[iArg]);
            LPWSTR pNewName = wcschr(pArg, L'|');
            if (pNewName) {
                *pNewName = 0;
                pNewName++;
                if (wcslen(pNewName) == 0)
                    pNewName = NULL;
            }
            if (!g_args.GetFullFileName(pArg, szSrcName, false))
                continue;
            if (pNewName != NULL) {
                if (!g_args.GetFullFileName(pNewName, szDestName, true))
                    continue;
                else
                    pNewName = szDestName;
            }

            // Check to see if a filename is a duplicate
            if (!DupFileNameCheck(pNewName ? pNewName : szSrcName)) {
                if (NULL == (*g_ppFileListEnd = new FileList())) {
                    hr = E_OUTOFMEMORY;
                    break;
                }
                (*g_ppFileListEnd)->arg.bManaged = true;
                if (NULL == ((*g_ppFileListEnd)->arg.pszSource = VSAllocStr(szSrcName))) {
                    delete *g_ppFileListEnd;
                    *g_ppFileListEnd = NULL;
                    hr = E_OUTOFMEMORY;
                    break;
                }
                if (pNewName) {
                    if (NULL == ((*g_ppFileListEnd)->arg.pszTarget = VSAllocStr(szDestName))) {
                        delete *g_ppFileListEnd;
                        *g_ppFileListEnd = NULL;
                        hr = E_OUTOFMEMORY;
                        break;
                    }
                }
                g_ppFileListEnd = &(*g_ppFileListEnd)->next;
            }
        }
    }

    if (FAILED (hr) || g_output.HadFatalError())
        return hr;

    // at least on input must be specified
    if (g_bMake && g_pResListHead == NULL && g_pFileListHead == NULL)
        g_output.ShowErrorId(ERR_NoInputs, ERROR_ERROR);

    // Make sure at least one "output" was specified
    if (g_szAssemblyFile == NULL || (!g_bMake && g_szAssemblyFile != NULL && (g_pResListHead != NULL || g_pFileListHead != NULL)))
        g_output.ShowErrorId(ERR_NoOutput, ERROR_ERROR);

    // Check for invalid main conditions
    if (g_Target == ttDll && g_szAppMain != NULL) {
        g_output.ShowErrorId(ERR_NoMainOnDlls, ERROR_ERROR);
    } else if (g_Target != ttDll && g_szAppMain == NULL) {
        g_output.ShowErrorId(ERR_AppNeedsMain, ERROR_ERROR);
    }

    if (g_szAssemblyFile && g_pFileListHead && !g_output.HadFatalError()) {

        // Check to see if a filename is the same as the assembly name
        FileList *files, *prev;
        files = g_pFileListHead;
        prev = NULL;
        while(files != NULL) {
            if ((files->arg.pszTarget && _wcsicmp(files->arg.pszTarget, g_szAssemblyFile) == 0) || 
                _wcsicmp(files->arg.pszSource, g_szAssemblyFile) == 0) {
                g_output.ShowErrorIdString(ERR_SameOutAndSource, ERROR_ERROR, g_szAssemblyFile);
                break;
            }
            prev = files;
            files = files->next;
        }

    }

    // always return S_OK if we report the error
    // because a failure HR will cause another error to be reported
    hr = S_OK;

    return hr;
}

/*
 * Main entry point
 */
extern "C" int __cdecl main (int _argc, __in_ecount(_argc) char **_argv)
{
    int argc = 0;
    WCBuffer * argv = NULL;
    HRESULT hr = E_FAIL;  // assume failure unless otherwise.
    LARGE_INTEGER timeStart, timeStop, frequency;
    IALink2  *pLinker = NULL;
    ICeeFileGen *pFileGen = NULL;
    IMetaDataDispenserEx *dispenser = NULL;
    CErrors errHandler;



    if (!PAL_RegisterLibraryW(L"rotor_palrt") ||
        !PAL_RegisterLibraryW(L"sscoree"))
    {
        g_output.ShowError(errorInfo[FTL_NoMessagesDLL].number, ERROR_FATAL, NULL, -1, -1, -1, -1, L"Unable to load critical libraries");
        return 1;
    }

    VsDebugInitialize();

    // Initialize Unilib
    g_bUnicode = (W_IsUnicodeSystem() == TRUE);

    // Try to load the message DLL.
    g_hinstMessages = GetALinkMessageDll();
    if (!g_output.Initialize(g_hinstMessages, MESSAGE_DLLW, errorInfo))
        return 1;

    // Get command line, split it into arguments. (also processes response files)
    if (!g_args.GetArgs( &argc, &argv))
        goto FAIL;

    // Do initial switch processing.
    ProcessPreSwitches(argc, argv);

    // All errors previous to this are fatal
    // This solve the error-before-logo problem
    if (g_output.HadError())
        goto FAIL;

    // Print the logo banner, unless noLogo was specified.
    if (!g_bNoLogo)
        g_output.PrintBanner();

    // If timing, start the timer.
    if (g_bTime) {
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&timeStart);
    }


    if (g_bShowHelp)
    {
        // Display help and do nothing else
        hr = PrintHelp ();
    }
    else
    {
        // Parse the command line, and if successful, compile.
        if (FAILED (hr = ParseCommandLine (argc, argv)))
        {
            g_output.ShowErrorIdString (FTL_InitError, ERROR_FATAL, g_output.ErrorHR(hr));
            goto FAIL;
        } else if (g_output.HadError()) {
            // error was already reported
            goto FAIL;
        }

        if ((FAILED (hr = CreateALink (IID_IALink2, (IUnknown**)&pLinker))) ||
            (FAILED(hr = CoCreateInstance(CLSID_CorMetaDataDispenser, NULL, CLSCTX_SERVER,
                                            IID_IMetaDataDispenserEx,
                                            (LPVOID *) & dispenser))) ||
            (FAILED(hr = SetDispenserOptions( dispenser))) ||
            ((pFileGen = CreateCeeFileGen()) == NULL) ||
            (FAILED(hr = pLinker->Init(dispenser, &errHandler)))) {
            if (hr != S_OK && FAILED(CheckHR(hr)))
                g_output.ShowErrorIdString(FTL_ComPlusInit, ERROR_FATAL, g_output.ErrorHR(hr));
        } else if (!g_output.HadError() && g_bMake) {
            MakeAssembly(pFileGen, dispenser, pLinker, &errHandler);
        }
    }

FAIL:
    if (pLinker)
        pLinker->Release();
    if (pFileGen)
        DestroyCeeFileGen(pFileGen);
    FreeLibrary(g_hmodCorPE);
    if (dispenser)
        dispenser->Release();

    // Free all the files
    FileList *files = NULL;
    while((files = g_pFileListHead) != NULL) {
        g_pFileListHead = files->next;
        delete files;
    }
    if (g_szAssemblyFile)
        delete [] g_szAssemblyFile;
    if (g_szCopyAssembly)
        delete [] g_szCopyAssembly;
    if (g_szAppMain)
        delete [] g_szAppMain;

    // Free the resources
    ResList *res = NULL;
    while((res = g_pResListHead) != NULL) {
        g_pResListHead = res->next;
        delete res->arg;
        delete res;
    }

    g_args.CleanUpArgs();

    // Cleanup our lists
    for (int i = 0; i < optLastAssemOption; i++) {
        if (V_VT(&g_Options[i]) == VT_EMPTY)
            continue;
        
        if (V_VT(&g_Options[i]) == VT_BYREF) {
            ASSERT(Options[i].flag & 0x04 && V_BYREF(&g_Options[i]) != NULL);

            list<VARIANT> * current = (list<VARIANT>*)V_BYREF(&g_Options[i]);
            while (current != NULL) {
                list<VARIANT> * temp = current->next;
                current->next = NULL;
                VariantClear(&current->arg);
                delete current;
                current = temp;
            }
            V_VT(&g_Options[i]) = VT_EMPTY;
            V_BYREF(&g_Options[i]) = NULL;
        }
        else {
            VariantClear(&g_Options[i]);
        }
    }


    if (g_bTime) {
        double elapsedTime;
        QueryPerformanceCounter(&timeStop);
        elapsedTime = (double) (timeStop.QuadPart - timeStart.QuadPart) / (double) frequency.QuadPart;
        printf("\nElapsed time: %#.4g seconds\n", elapsedTime);
    }


    // Cleanup
    VsDebugTerminate();

    // Return success code.
    return (hr != S_OK || g_output.HadError()) ? 1 : 0;
}

#define ICEEFILEGENDLL MAKEDLLNAME_W(L"mscorpe")

/*
 * Loads mscorpe.dll and gets an ICeeFileGen interface from it.
 * The ICeeFileGen interface is used for the entire compile.
 */
ICeeFileGen* CreateCeeFileGen()
{
    // Dynamically bind to ICeeFileGen functions.
    if (!g_pfnCreateCeeFileGen || !g_pfnDestroyCeeFileGen) {

        HRESULT hr = LoadLibraryShim(ICEEFILEGENDLL, NULL, NULL, &g_hmodCorPE);
        if (SUCCEEDED(hr) && g_hmodCorPE) {
            // Get the required methods.
            g_pfnCreateCeeFileGen  = (HRESULT (__stdcall *)(ICeeFileGen **ceeFileGen)) GetProcAddress(g_hmodCorPE, "CreateICeeFileGen");
            g_pfnDestroyCeeFileGen = (HRESULT (__stdcall *)(ICeeFileGen **ceeFileGen)) GetProcAddress(g_hmodCorPE, "DestroyICeeFileGen");
            if (!g_pfnCreateCeeFileGen || !g_pfnDestroyCeeFileGen) {
                g_output.ShowErrorIdString(FTL_ComPlusInit, ERROR_FATAL, g_output.ErrorLastError());
                return NULL;
            }
        }
        else {
            // MSCorPE.DLL wasn't found.
            g_output.ShowErrorIdString(FTL_RequiredFileNotFound, ERROR_FATAL, ICEEFILEGENDLL);
            return NULL;
        }
    }

    ICeeFileGen *ceefilegen = NULL;
    HRESULT hr = g_pfnCreateCeeFileGen(& ceefilegen);
    if (FAILED(hr)) {
        g_output.ShowErrorIdString(FTL_ComPlusInit, ERROR_FATAL, g_output.ErrorHR(hr));
    }

    return ceefilegen;
}

void DestroyCeeFileGen(ICeeFileGen *ceefilegen)
{
    ASSERT(g_pfnDestroyCeeFileGen != NULL ||
           ceefilegen == NULL);
    if (ceefilegen != NULL)
        g_pfnDestroyCeeFileGen(&ceefilegen);
}

/*
 * sets the options on the current metadata dispenser
 */
HRESULT SetDispenserOptions(IMetaDataDispenserEx * dispenser)
{
    VARIANT v;
    HRESULT hr = E_POINTER;

    if (dispenser) {
        // We shouldn't emit any Dups, so check for everything
        V_VT(&v) = VT_UI4;
        V_UI4(&v) = MDDupAll;
        if (FAILED(hr = dispenser->SetOption(MetaDataCheckDuplicatesFor, &v)))
            return hr;

        // Never change refs to defs
        V_VT(&v) = VT_UI4;
        V_UI4(&v) = MDRefToDefNone;
        if (FAILED(hr = dispenser->SetOption(MetaDataRefToDefCheck, &v)))
            return hr;

        // Give error if emitting out of order
        V_VT(&v) = VT_UI4;
        V_UI4(&v) = MDErrorOutOfOrderAll;
        if (FAILED(hr = dispenser->SetOption(MetaDataErrorIfEmitOutOfOrder, &v)))
            return hr;

        // Turn on regular update mode
        V_VT(&v) = VT_UI4;
        V_UI4(&v) = MDUpdateFull;
        hr = dispenser->SetOption(MetaDataSetUpdate, &v);
    }
    return hr;
}

/*
 * This actually creates the manifest file and calls into ALink to create the metadata
 */
void MakeAssembly(ICeeFileGen *pFileGen, IMetaDataDispenserEx *pDisp, IALink2 *pLinker, IMetaDataError *pErrHandler)
{
    HRESULT                     hr;
    HCEEFILE                    file;
    HCEESECTION                 ilSection;
    HCEESECTION                 resSection;
    IMetaDataEmit              *metaemit;
    IMetaDataAssemblyImport    *pImport = NULL;
    LPCWSTR                     moduleName = NULL;
    LPCWSTR                     pszLocation = NULL;
    mdAssembly                  assemID = mdTokenNil;
    mdToken                     fileID = mdTokenNil;
    DWORD                       dwTemp = 0, dwStrongName = 0;
    DWORD                       dwResOffset = 0;
    WCHAR                       buffer1[_MAX_PATH];
    WCHAR                       buffer2[_MAX_PATH];
    FileList                   *files;
    ResList                    *res;
    int                         iArgs = 0;
    bool                        bHasBogusMain = false;
    bool                        bHadFile = false;
    mdToken                     tkEntryPoint = mdTokenNil;
    mdMethodDef                 tkMain = mdTokenNil;
    LPWSTR                      szTypeName = NULL;
    LPWSTR                      szMethodName = NULL;
    AssemblyFlags               f = afNone;

    // Create a new scope to emit to
    IfFailGo(pDisp->DefineScope(CLSID_CorMetaDataRuntime,  // Format of metadata
                                       0,                         // flags
                                       IID_IMetaDataEmit,         // Emitting interface
                                       (IUnknown **) &metaemit));

    if (g_bClean)
        f = (AssemblyFlags)(f | afCleanModules);
    if (g_bNoRefHash)
        f = (AssemblyFlags)(f | afNoRefHash);
    IfFailGo(pLinker->SetAssemblyFile(g_szAssemblyFile, metaemit, f, &assemID));
    IfFailGo(pLinker->SetPEKind(assemID, assemID, g_dwPEKind, g_dwMachine));

    // Split the 'main' into TypeName and MethodName
    if (g_szAppMain) {
        szMethodName = wcsrchr(g_szAppMain, L'.');
        if (szMethodName) {
            *szMethodName = 0;
            szMethodName++;
            szTypeName = g_szAppMain;
        } else {
            szMethodName = g_szAppMain;
            szTypeName = NULL;
        }

        if ((szTypeName != NULL && wcslen(szTypeName) < 1) || wcslen(szMethodName) < 1) {
            g_output.ShowErrorIdString(ERR_BadOptionValue, ERROR_ERROR, L"main", g_szAppMain);
            delete [] g_szAppMain;
            szTypeName = szMethodName = g_szAppMain = NULL;
        }
    }

    // Get the parent assembly info, and copy into this one
    if (g_szCopyAssembly) {
        pszLocation = g_szCopyAssembly;
        IfFailGo(CopyAssembly(assemID, pLinker));
    }
    pszLocation = NULL;

    // Import all the files 
    files = g_pFileListHead;
    while(files != NULL) {
        pszLocation = files->arg.pszSource;
        g_output.OutputBinaryFileToRepro(pszLocation, g_szAssemblyFile);
        if (files->arg.bManaged) {
            IfFailGo(pLinker->ImportFile( files->arg.pszSource, files->arg.pszTarget, FALSE, &fileID, &pImport, &dwTemp));
            if (pImport != NULL || dwTemp != 1) {
                // This is an assembly
                g_output.ShowErrorIdString(WRN_IgnoringAssembly, ERROR_WARNING, files->arg.pszSource);
                pImport->Release();
                pImport = NULL;
            } else {
                CComPtr<IMetaDataImport> pI = NULL;
                if (SUCCEEDED(hr = pLinker->GetScope(assemID, fileID, 0, &pI))) {
                    if (g_szAppMain && IsNilToken(tkEntryPoint)) {
                        // Look for the 'Main' routine
                        // this also creates the __EntryPoint method, and the methodref to the original main
                        IfFailGo(FindMain( pI, metaemit, szTypeName, szMethodName, tkEntryPoint, tkMain, iArgs, bHasBogusMain));
                    }
                    IfFailGo(pLinker->AddImport(assemID, fileID, 0, &fileID));
                    bHadFile = true;
                }
            }
        } else {
            IfFailGo(pLinker->AddFile( assemID, files->arg.pszSource, ffContainsNoMetaData, NULL, &fileID));
            bHadFile = true;
        }
        files = files->next;
    }
    pszLocation = NULL;
    if (!bHadFile && !g_pResListHead) {
        g_output.ShowErrorId(ERR_NoInputs, ERROR_ERROR);
        return;
    }

    // Set the file info, etc.
    IfFailGo(pFileGen->CreateCeeFileEx( &file, g_dwCeeCreateFlags));

    IfFailGo(pFileGen->GetIlSection( file, &ilSection));

    IfFailGo(pFileGen->GetRdataSection( file, &resSection));

    _wsplitpath_s(g_szAssemblyFile, NULL, 0, NULL, 0, buffer1, _MAX_PATH, buffer2, _MAX_PATH);    // Strip the path
    wcsncat_s(buffer1, _MAX_PATH, buffer2, lengthof (buffer1));
    moduleName = buffer1;
    IfFailGo(metaemit->SetModuleProps(moduleName));

    // Report an appropriate error
    if (g_szAppMain && IsNilToken(tkEntryPoint)) {
        if (szMethodName != g_szAppMain)
            *(szMethodName - 1) = '.'; // reconstruct the full name
        if (bHasBogusMain)
            g_output.ShowErrorIdString( ERR_BadMainFound, ERROR_ERROR, g_szAppMain);
        else
            g_output.ShowErrorIdString( ERR_NoMainFound, ERROR_ERROR, g_szAppMain);
        goto ErrExit;
    }


    // Set the options, after importing files so they override the CAs
    IfFailGo(SetAssemblyOptions(assemID, pLinker));

    IfFailGo(pLinker->EmitManifest( assemID, &dwStrongName, NULL));

    if (!IsNilToken(tkEntryPoint)) {
        IfFailGo(MakeMain(pFileGen, file, ilSection, metaemit, tkMain, tkEntryPoint, iArgs));
        IfFailGo(pFileGen->SetEntryPoint( file, tkMain));
    }

    if (dwStrongName != 0) {
        void  *pvBuffer = NULL;
        IfFailGo(pFileGen->GetSectionBlock( ilSection, dwStrongName, 1, &pvBuffer));
        memset(pvBuffer, 0, dwStrongName); // Zero it out

        IfFailGo(pFileGen->GetSectionDataLen(ilSection, &dwTemp));

        IfFailGo(pFileGen->GetMethodRVA(file, dwTemp - dwStrongName, &dwTemp));

        IfFailGo(pFileGen->SetStrongNameEntry( file, dwStrongName, dwTemp));
    }

    // Add all the COM+ Resources
    for( res = g_pResListHead; res != NULL; res = res->next) {
        resource *r = res->arg;
        g_output.OutputBinaryFileToRepro(r->pszSource, g_szAssemblyFile);
        if (r->bEmbed) {
            HANDLE hFile;
            DWORD fileLen = 0, dwRead;
            void *pvBuffer;

            r->dwOffset = ((dwResOffset + 0x7) & 0xFFFFFFF8); // Pad to an 8 byte boundary

            hFile = OpenFileEx (r->pszSource, &fileLen);
            if (hFile == INVALID_HANDLE_VALUE) {
                g_output.ShowErrorIdString( ERR_CantReadResource, ERROR_ERROR, r->pszSource, g_output.ErrorLastError());
                continue;
            }
            dwResOffset = r->dwOffset + fileLen + sizeof(DWORD);

            if (FAILED(hr = pFileGen->GetSectionBlock( resSection, fileLen + sizeof(DWORD), 8, &pvBuffer))) {
                g_output.ShowErrorIdLocation( ERR_CantEmbedResource, ERROR_ERROR, g_szAssemblyFile, r->pszSource, g_output.ErrorHR(hr));
                CloseHandle( hFile);
                continue;
            }

            SET_UNALIGNED_VAL32(pvBuffer, fileLen);
            if (!ReadFile( hFile, ((DWORD*)pvBuffer) + 1, fileLen, &dwRead, NULL)) {
                g_output.ShowErrorIdString( ERR_CantReadResource, ERROR_ERROR, r->pszSource, g_output.ErrorLastError());
                CloseHandle( hFile);
                continue;
            }
            CloseHandle( hFile);

            if (FAILED(hr = pLinker->EmbedResource( assemID, assemID, r->pszIdent, r->dwOffset, r->bVis ? mrPublic : mrPrivate))) {
                if (FAILED(CheckHR(hr)))
                    g_output.ShowErrorIdString(ERR_MetaDataError, ERROR_ERROR, g_output.ErrorHR(hr));
            }

        } else {
            if (FAILED(hr = pLinker->LinkResource( assemID, r->pszSource, r->pszTarget, r->pszIdent, r->bVis ? mrPublic : mrPrivate))) {
                if (FAILED(CheckHR(hr)))
                    g_output.ShowErrorIdString(ERR_MetaDataError, ERROR_ERROR, g_output.ErrorHR(hr));
            }
        }
    }

    // This really sets the start of the COM+ resource section
    // If the offset is 0, it really means, the resources starts at size-bytes
    // from the end of this section.
    IfFailGo(pFileGen->SetManifestEntry( file, dwResOffset, 0));

    IfFailGo(pFileGen->SetOutputFileName( file, g_szAssemblyFile));


    IfFailGo(pFileGen->SetComImageFlags( file, g_dwComImageFlags));
    if (g_DllBase != 0) {
        if (g_dwPEKind & pe32Plus) {
            IfFailGo(pFileGen->SetImageBase64( file, g_DllBase));
        } else {
            if (g_DllBase > UI64(0x00000000FFFFFFFF)) {
                WCHAR szBuffer[16 + 3];
                StringCchPrintfW( szBuffer, lengthof(szBuffer), L"0x%016I64x", g_DllBase);
                g_output.ShowErrorIdString( ERR_BadOptionValue, ERROR_ERROR, L"/baseaddress", szBuffer);
            } else {
                IfFailGo(pFileGen->SetImageBase( file, (DWORD)g_DllBase));
            }
        }
    }
    if (g_Target == ttDll) {
        IfFailGo(pFileGen->SetDllSwitch( file, TRUE));
    } else {
        IfFailGo(pFileGen->SetSubsystem(file, g_Target == ttConsole ? IMAGE_SUBSYSTEM_WINDOWS_CUI : IMAGE_SUBSYSTEM_WINDOWS_GUI, 4, 0));
    }

    IfFailGo(pLinker->PreCloseAssembly(assemID));

    IfFailGo(pFileGen->EmitMetaDataEx( file, metaemit));

    // Don't create a file if we had errors!
    if (g_output.HadError()) goto ErrExit;

    IfFailGo(pFileGen->GenerateCeeFile(file));

    IfFailGo(pLinker->CloseAssembly(assemID));

    // Cleanup - Make sure we delete the temp file
ErrExit:
    if (FAILED(hr)) {
        // Check for 'known' HRESULT's
        if (FAILED(CheckHR(hr))) {
            // Then fall back ona more generic error
            if (pszLocation)
                g_output.ShowErrorIdString(ERR_ModuleImportError, ERROR_ERROR, pszLocation, g_output.ErrorHR(hr));
            else
                g_output.ShowErrorIdString(ERR_MetaDataError, ERROR_ERROR, g_output.ErrorHR(hr));
        }
    }


    return;
}

/*
 * searches the import scope for the 'Main' method
 * once found, it generates a TypeRef and MemberRef in the current emit scope
 * and a MethodDef with the same signature
 */
HRESULT FindMain( IMetaDataImport *pImport, IMetaDataEmit *pEmit, LPCWSTR pszTypeName, LPCWSTR pszMethodName, // Inputs
                 mdMemberRef &tkMain, mdMethodDef &tkNewMain, int &countArgs, bool &bHadBogusMain)  // Outputs
{
    HRESULT hr = S_OK;
    mdTypeDef tkClass = mdTokenNil;
    countArgs = 0;
    tkNewMain = tkMain = mdTokenNil;

    if (pszTypeName == NULL || SUCCEEDED(hr = pImport->FindTypeDefByName(pszTypeName, mdTokenNil, &tkClass))) {
        HCORENUM hEnum = 0;
        mdMethodDef meth[4];
        DWORD cMeth = 0;

        if (!IsNilToken(tkClass)) {
            if (S_OK != (hr = IsNonGeneric( pImport, tkClass))) {
                bHadBogusMain |= (hr == S_FALSE);
                return hr;
            }
        }

        do {
            if (FAILED(hr = pImport->EnumMethodsWithName( &hEnum, tkClass, pszMethodName, meth, lengthof(meth), &cMeth)))
                break;
            
            if (cMeth > 0) bHadBogusMain = true;
            for (DWORD iMeth = 0; iMeth < cMeth; iMeth++) {
                mdToken tkRef = mdTokenNil;
                PCCOR_SIGNATURE pSig;
                PCCOR_SIGNATURE sig;
                COR_SIGNATURE newSig[10]; // this should be big enough
                PCOR_SIGNATURE pNewSig = newSig;
                DWORD cbSig = 0, flags = 0;
                ULONG params = (ULONG)-1;

                // no generic mains
                if (S_OK != (hr = IsNonGeneric( pImport, meth[iMeth])))
                    continue;

                if (FAILED(hr = pImport->GetMethodProps(meth[iMeth], NULL, NULL, 0, NULL, &flags, &sig, &cbSig, NULL, NULL)))
                    continue;
                if (!IsMdStatic(flags) || IsMdAbstract(flags))
                    continue; // we don't allow non-static or virtual Mains
                if (pEmit != NULL && !(IsMdPublic(flags) || IsMdAssem(flags) || IsMdFamORAssem(flags)))
                    continue;
                pSig = sig;
                if (*pSig++ != IMAGE_CEE_CS_CALLCONV_DEFAULT)
                    continue; // Only allow default calling convention
                if ((params = CorSigUncompressData(pSig)) > 1)
                    continue; // Only 0 or 1 arguments
                if (*pSig == ELEMENT_TYPE_CMOD_OPT) {
                    pSig++;
                    CorSigUncompressToken( pSig); // Ignore the token
                }
                if (*pSig != ELEMENT_TYPE_I4 && *pSig != ELEMENT_TYPE_VOID)
                    continue; // must return void or int
                pNewSig = newSig;
                *pNewSig++ = IMAGE_CEE_CS_CALLCONV_DEFAULT;
                pNewSig += CorSigCompressData(params, pNewSig);
                *pNewSig++ = *pSig;
                pSig++;

                // Allow a benign cmodopt
                if (*pSig == ELEMENT_TYPE_CMOD_OPT) {
                    pSig++;
                    CorSigUncompressToken( pSig); // Ignore the token
                }

                if (params == 1) {
                    // C# style main, takes a string[]
                    countArgs = 1;
                    
                    if (*pSig++ != ELEMENT_TYPE_SZARRAY || *pSig++ != ELEMENT_TYPE_STRING)
                        continue;
                    *pNewSig++ = ELEMENT_TYPE_SZARRAY;
                    *pNewSig++ = ELEMENT_TYPE_STRING;
                } else if (params == 0) {
                    countArgs = 0;
                } else
                    // Not allowed
                    continue;

                if (tkClass != (mdTokenNil) && FAILED(hr = pEmit->DefineImportType(NULL, NULL, 0, pImport, tkClass, NULL, &tkRef)))
                    continue;
                if (SUCCEEDED(hr = pEmit->DefineImportMember( NULL, NULL, 0, pImport, meth[iMeth], NULL, tkRef, &tkMain)) && 
                    SUCCEEDED(hr = pEmit->DefineMethod(mdTokenNil, L"__EntryPoint", mdStatic, newSig, (ULONG)(pNewSig - newSig), (ULONG) -1, miManaged | miIL, &tkNewMain))) {
                    cMeth = 0;
                    hr = S_OK;
                    goto FOUND;
                }
            }
        } while (cMeth > 0 && hr == S_OK);

FOUND:
        if (hEnum != 0)
            pImport->CloseEnum( hEnum);
    
    }

    if (hr == CLDB_E_RECORD_NOTFOUND) {
        hr = S_FALSE;
    }

    return hr;
}

// Error handler/reporter
HRESULT CErrors::OnError(HRESULT hr, mdToken tkLocation)
{
    if (hr == S_OK)
        return S_OK;

    if (hr == MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_ITF, errorInfo[WRN_OptionConflicts].number) &&
        (mdToken) Options[tkLocation].opt == tkLocation) {
        if (V_VT(&g_Options[tkLocation]) == VT_EMPTY && !g_bCopyOptions.get(Options[tkLocation].opt)) {
            // If the user didn't give a command-line override
            // change this warning into a duplicate CA error!
            g_output.ShowErrorIdString(ERR_DuplicateCA, ERROR_ERROR, Options[tkLocation].CA);
        } else if (g_bCopyOptions.get(Options[tkLocation].opt)) {
            // A module setting is just overriding a template setting
            // Ignore the first one, but reset this, so we'll report the next one
            g_bCopyOptions.set(Options[tkLocation].opt, false);
        }
        // Never report a dupe error if the command-line overrides it
        return S_OK;
    }

    return CheckHR(hr);
}

int __cdecl MatchNumber(const void *p1, const void *p2)
{
    const ERROR_INFO * e1 = (const ERROR_INFO*)p1;
    const ERROR_INFO * e2 = (const ERROR_INFO*)p2;

    return e1->number - e2->number;
}

HRESULT CheckHR(HRESULT hr)
{
    if (hr == S_OK)
        return S_OK;

    HRESULT hr2;
    CComPtr<IErrorInfo> err;
    CComBSTR str;
    GUID g = GUID_NULL;

    if ((hr2 = GetErrorInfo( 0, &err)) == S_OK) {
        if (FAILED(hr2 = err->GetGUID( &g)))
            g = GUID_NULL;
    }
    if (SUCCEEDED(hr2) && err != NULL && SUCCEEDED(err->GetDescription(&str)) &&
        HRESULT_FACILITY(hr) == FACILITY_ITF
        && (g == IID_IALink || g == IID_IALink2)) { // One of our errors

        ERROR_INFO match, *found;
        match.number = (short)(HRESULT_CODE(hr));
        found = (ERROR_INFO*)bsearch( &match, errorInfo, ERR_COUNT, sizeof(ERROR_INFO), MatchNumber);
        ASSERT(found);
        if (found) {
            int id = (int)(found - errorInfo);
            ERRORKIND kind = (errorInfo[id].level > 0) ? ERROR_WARNING : ERROR_ERROR;
            // Call onto general method to show the error.
            g_output.ShowError(errorInfo[id].number, kind, NULL, -1, -1, -1,-1, str);

        } else {
            SetErrorInfo(0L, err); // Put it back
            return hr;
        }
    } else {
        SetErrorInfo(0L, err); // Put it back
        return hr;
    }

    return S_OK;
}

__forceinline void putOpcode( BYTE **temp, ILCODE code) {
    if ((code & 0xFF00) == 0xFF00) {
        **temp = (BYTE)(code & 0xFF);
        *temp += 1;
    } else {
        **temp = (BYTE)((code >> 8) & 0xFF);
        *temp += 1;
        **temp = (BYTE)(code & 0xFF);
        *temp += 1;
    }
}

/*
 * Generates the IL for the __EntryPoint method, that does a tail call into the real main
 */
HRESULT MakeMain(ICeeFileGen *pFileGen, HCEEFILE file, HCEESECTION ilSection, IMetaDataEmit *pEmit,
                 mdMethodDef tkMain, mdMemberRef tkEntryPoint, int iArgs)
{
    HRESULT hr;
    BYTE *buffer = NULL;
    BYTE *temp = NULL;
    BYTE code[64];
    BYTE len = 0;
    DWORD rva = 0;

    ASSERT(iArgs < 2); // How'd we get so many args!

    // Generate the IL and Calculate the size
    temp = code;
    if (iArgs > 0) {
        putOpcode( &temp, CEE_LDARG_0);
    }
    putOpcode( &temp, CEE_TAILCALL);
    putOpcode( &temp, CEE_CALL);
    SET_UNALIGNED_VAL32(temp, tkEntryPoint);
    temp += sizeof(tkEntryPoint);
    putOpcode( &temp, CEE_RET);

    len = (BYTE)(temp - code);

    if (FAILED(hr = pFileGen->GetSectionBlock(ilSection, len + 1, 1, (void**)&buffer))) // +1 for header
        return hr;

    temp = buffer;
    *temp = ((len << 2) | CorILMethod_TinyFormat);
    temp++;
    memcpy(temp, code, len);

    if (FAILED(hr = pFileGen->ComputeSectionOffset(ilSection, (char*)buffer, (UINT*)&rva)))
        return hr;
    if (FAILED(hr = pFileGen->GetMethodRVA(file, rva, &rva)))
        return hr;
    return pEmit->SetMethodProps(tkMain, mdStatic | mdPrivateScope, rva, miManaged | miIL);
}

// *pwszFileName is a possibly relative path to be interpreted relative to wszOldBase. If it
// is a relative path, then converted it to a path, relative if possible, that is relative to 
// newBase. Otherwise, leave it unchanged.
void RerelativizeFileName(WCAllocBuffer &bufFileName, PCWSTR wszOldBase, PCWSTR wszNewBase)
{
    WCHAR wszNewFileName[MAX_PATH];
    WCHAR wszCanonicalFileName[MAX_PATH];

    // Only need to do work if the path is relative.
    if (! PathIsRelativeW(bufFileName))
        return;

    // Get directory part of old  base.
    if (FAILED(StringCchCopyW(wszCanonicalFileName, lengthof(wszCanonicalFileName), wszOldBase)))
        return;

    if (! PathRemoveFileSpecW( wszCanonicalFileName))
        return;

    // Make absolute path based on the relative path and the old directory.
    if (! PathCombineW(wszNewFileName, wszCanonicalFileName, bufFileName))
        return;

    // Canonicalize it.
    if (! PathCanonicalizeW(wszCanonicalFileName, wszNewFileName))
        return;
    
    // Relativize it based on the new base
    if (! PathRelativePathToW(wszNewFileName, wszNewBase, FILE_ATTRIBUTE_NORMAL, wszCanonicalFileName, FILE_ATTRIBUTE_NORMAL))
        return; // can't make relative, don't want to use the absolute path, so give up.

    //re-allocate buffer and copy result into it.
    size_t cch = wcslen(wszNewFileName);
    bufFileName.Clear();
    if (FAILED(bufFileName.AllocCount(cch+1)))
        return;

    bufFileName.CopyFrom((WCBuffer)wszNewFileName, cch);
    bufFileName.SetAt((int)cch, 0);
}


HRESULT ImportCA(const BYTE * pvData, DWORD cbSize, AssemblyOptions opt, mdAssembly assemID, IALink * pLinker)
{
    // Make sure that data is in the correct format. Check the prolog, then
    // move beyond it.
    if (cbSize < sizeof(WORD) || GET_UNALIGNED_VAL16(pvData) != 1)
        return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
    pvData += sizeof(WORD);
    cbSize -= sizeof(WORD);
    HRESULT hr = E_INVALIDARG;
    VARIANT var;
    VariantInit(&var);

    switch(Options[opt].vt) {
    case VT_BSTR:
    {
        ASSERT(opt != optAssemOS);
        WCAllocBuffer buffer;

        // String is stored as compressed length, UTF8.
        if (*pvData == 0xFF) {
            pvData++;
            cbSize--;
        } else {
            DWORD  len, cchLen;

            PCCOR_SIGNATURE sig = (PCCOR_SIGNATURE)pvData;
            len = CorSigUncompressData(sig);

            //EDMAURER If len 0, don't setup buffer with an empty string. RerelativizeFileName ()
            //below doesn't like an empty string.
            if (len) {
                pvData = (const BYTE *)sig;
                cchLen = UnicodeLengthOfUTF8((PCSTR)pvData, len) + 1;
                IfFailRet(buffer.AllocCount(cchLen));
                UTF8ToUnicode((const char*)pvData, len, buffer.GetData(), buffer.Count());
                buffer.SetAt(buffer.Count() - 1, L'\0');
                pvData += len;
                cbSize -= len;
            }
        }
        if (buffer.Count() > 0 && Options[opt].flag & 0x10) {
            // this option is a filename, so convert from being relative to the source,
            // to being relative to the destination
            RerelativizeFileName(buffer, g_szCopyAssembly, g_szAssemblyFile);
            g_output.OutputBinaryFileToRepro(buffer, g_szAssemblyFile);
        }

        V_VT(&var) = VT_BSTR;
        V_BSTR(&var) = SysAllocString(buffer.Str());
        hr = pLinker->SetAssemblyProps( assemID, assemID, opt, var);
        break;
    }
    case VT_BOOL:
        if (cbSize < sizeof(BYTE))
            return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);

        V_VT(&var) = VT_BOOL;
        V_BOOL(&var) = (*(BYTE*)pvData) ? VARIANT_TRUE : VARIANT_FALSE;
        hr = pLinker->SetAssemblyProps( assemID, assemID, opt, var);
        cbSize -= sizeof(BYTE);
        pvData += sizeof(BYTE);
        break;

    case VT_UI4:
        if (cbSize < sizeof(ULONG))
            return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);

        V_VT(&var) = VT_UI4;
        V_UI4(&var) = GET_UNALIGNED_VAL32(pvData);
        hr = pLinker->SetAssemblyProps( assemID, assemID, opt, var);
        cbSize -= sizeof(ULONG);
        pvData += sizeof(ULONG);
    }

    VariantClear(&var);

    return hr;
}

bool AddVariantOption( VARIANT &var, VARIANT &val, bool fAllowMulti)
{
    // Only allow lists if this is an 'allow-multi' option

    if (!fAllowMulti || V_VT(&var) == VT_EMPTY) {
        if (V_VT(&var) != VT_EMPTY)
            VariantClear(&var);
        var = val;
    }
    else {        
        if (V_VT(&var) == VT_BYREF) {
            list<VARIANT> * temp = new list<VARIANT>(val, (list<VARIANT>*)V_BYREF(&var));
            if (temp == NULL)
                return false;
            V_BYREF(&var) = temp;
        } else {
            list<VARIANT> * temp = new list<VARIANT>(var, NULL);
            if (temp == NULL)
                return false;
            list<VARIANT> * temp2 = new list<VARIANT>(val, temp);
            if (temp2 == NULL) {
                delete temp;
                temp = NULL;
                return false;
            }
            V_VT(&var) = VT_BYREF;
            V_BYREF(&var) = temp2;
        }
    }
    // ownership transfered
    V_VT(&val) = VT_EMPTY;
    return true;
}

HRESULT CopyAssembly(mdAssembly assemID, IALink * pLinker)
{
    // Get the parent assembly info, and copy into this one
    HRESULT hr = S_OK;
    mdToken tkParent = mdTokenNil;
    CComPtr<IMetaDataImport> pImp;
    CComPtr<IMetaDataAssemblyImport> pAsm;
    mdAssembly tkAssembly = mdAssemblyNil;
    DWORD dwCount = 0;

    ASSERT(g_szCopyAssembly != NULL);
    g_output.OutputBinaryFileToRepro(g_szCopyAssembly, g_szAssemblyFile);

    if (FAILED(hr = pLinker->ImportFile( g_szCopyAssembly, NULL, FALSE, &tkParent, &pAsm, &dwCount)))
        return hr;
    if (!pAsm || dwCount == 0) {
        g_output.ShowErrorIdString(ERR_ParentNotAnAssembly, ERROR_ERROR, g_szCopyAssembly);
        return S_FALSE;
    }
    if (FAILED(hr = pLinker->GetScope( assemID, tkParent, 0, &pImp)))
        return hr;
    if (FAILED(hr = pAsm->GetAssemblyFromScope(&tkAssembly)))
        return hr;

    if (SUCCEEDED(hr) && pAsm && pImp) {
        ASSEMBLYMETADATA md;
        WCHAR buffer[1024];
        DWORD dwAlgId = 0, dwFlags = 0;
        VARIANT var;
        VariantInit(&var);
        memset( &md, 0, sizeof(md));

        // Get everything
        if (SUCCEEDED(hr = pAsm->GetAssemblyProps( tkAssembly, NULL, NULL, &dwAlgId, NULL, 0, NULL, &md, &dwFlags))) {
            if (md.ulOS) {
                if (NULL == (md.rOS = new OSINFO[md.ulOS]))
                    return E_OUTOFMEMORY;
            }
            if (md.ulProcessor) {
                if (NULL == (md.rProcessor = new ULONG[md.ulProcessor]))
                    return E_OUTOFMEMORY;
            }
            if (md.cbLocale) {
                if (NULL == (md.szLocale = new WCHAR[md.cbLocale + 1]))
                    return E_OUTOFMEMORY;
            }

            if (SUCCEEDED(hr = pAsm->GetAssemblyProps( tkAssembly, NULL, NULL, NULL, NULL, 0, NULL, &md, NULL))) {

                if (V_VT(&g_Options[optAssemOS]) == VT_EMPTY) {
                    ASSERT(Options[optAssemOS].flag & 0x04);
                    for (ULONG o = 0; o < md.ulOS && SUCCEEDED(hr); o++) {
                        hr = StringCchPrintfW(buffer, NUMBER_OF(buffer), L"%d.%d.%d", md.rOS[o].dwOSPlatformId, md.rOS[o].dwOSMajorVersion, md.rOS[o].dwOSMinorVersion);
                        ASSERT (SUCCEEDED (hr));
                        V_VT(&var) = VT_BSTR;
                        V_BSTR(&var) = SysAllocString(buffer);
                        if (SUCCEEDED(hr = pLinker->SetAssemblyProps( assemID, assemID, optAssemOS, var)))
                            g_bCopyOptions.set(optAssemOS);
                        VariantClear(&var);
                    }
                }

                if (V_VT(&g_Options[optAssemProcessor]) == VT_EMPTY && SUCCEEDED(hr)) {
                    ASSERT(Options[optAssemProcessor].flag & 0x04);
                    V_VT(&var) = VT_UI4;
                    for (ULONG p = 0; p < md.ulProcessor && SUCCEEDED(hr); p++) {
                        V_UI4(&var) = md.rProcessor[p];
                        if (SUCCEEDED(hr = pLinker->SetAssemblyProps( assemID, assemID, optAssemProcessor, var)))
                            g_bCopyOptions.set(optAssemProcessor);
                    }
                }

                if (V_VT(&g_Options[optAssemLocale]) == VT_EMPTY &&
                    SUCCEEDED(hr) && md.cbLocale > 0 &&
                    md.szLocale != NULL && md.szLocale[0] != L'\0') {
                    V_VT(&var) = VT_BSTR;
                    V_BSTR(&var) = SysAllocString(md.szLocale);
                    if (SUCCEEDED(hr = pLinker->SetAssemblyProps( assemID, assemID, optAssemLocale, var)))
                        g_bCopyOptions.set(optAssemLocale);
                    VariantClear(&var);
                }

                if (V_VT(&g_Options[optAssemVersion]) == VT_EMPTY && SUCCEEDED(hr)) {
                    hr = StringCchPrintfW(buffer, NUMBER_OF(buffer), L"%hu.%hu.%hu.%hu", md.usMajorVersion, md.usMinorVersion, md.usBuildNumber, md.usRevisionNumber);
                    ASSERT (SUCCEEDED (hr));
                    V_VT(&var) = VT_BSTR;
                    V_BSTR(&var) = SysAllocString(buffer);
                    if (SUCCEEDED(hr = pLinker->SetAssemblyProps( assemID, assemID, optAssemVersion, var)))
                        g_bCopyOptions.set(optAssemVersion);
                    VariantClear(&var);
                }

            }

            if (V_VT(&g_Options[optAssemAlgID]) == VT_EMPTY) {
                V_VT(&var) = VT_UI4;
                V_UI4(&var) = dwAlgId;
                if (SUCCEEDED(hr = pLinker->SetAssemblyProps( assemID, assemID, optAssemAlgID, var)))
                    g_bCopyOptions.set(optAssemAlgID);
            }

            if (V_VT(&g_Options[optAssemFlags]) == VT_EMPTY) {
                V_VT(&var) = VT_UI4;
                V_UI4(&var) = dwFlags;
                if (SUCCEEDED(hr = pLinker->SetAssemblyProps( assemID, assemID, optAssemFlags, var)))
                    g_bCopyOptions.set(optAssemFlags);
            }

            if (md.rOS)
                delete [] md.rOS;
            if (md.rProcessor)
                delete [] md.rProcessor;
            if (md.szLocale)
                delete [] md.szLocale;
        }
    }

    for (int i = 0; i < optLastAssemOption && SUCCEEDED(hr); i++) {
        if (V_VT(&g_Options[i]) != VT_EMPTY ||  // Don't copy if it's overriden by the command-line

            // We don't ever copy these
            i == optAssemConfig ||
            //


            // These come from bits, and are already done
            i == optAssemOS || i == optAssemProcessor ||
            i == optAssemLocale || i == optAssemVersion ||
            i == optAssemAlgID || i == optAssemFlags) {
            //

            continue;

        } else {
            const void *pvData = NULL;
            DWORD   cbData = 0;

            // This only works for non-AllowMulti CAs
            ASSERT((Options[i].flag & 0x04) == 0);
            if (S_OK == (hr = pImp->GetCustomAttributeByName( tkAssembly, Options[i].CA, &pvData, &cbData))) {
                AssemblyOptions o = (i == optAssemSatelliteVer ? optAssemVersion : (AssemblyOptions)i);
                if (SUCCEEDED(hr = ImportCA( (const BYTE *)pvData, cbData, o, assemID, pLinker)))
                    g_bCopyOptions.set(o);
            }
            // hr == S_FLASE means the CA doesn't exist
        }
    }

    return hr;
}

HRESULT SetAssemblyOptions(mdAssembly assemID, IALink *pLinker)
{
    HRESULT hr = S_OK;
    // Set the options, after importing files so they override the CAs
    for (int i = 0; i < optLastAssemOption && SUCCEEDED(hr); i++) {
        if (V_VT(&g_Options[i]) != VT_EMPTY) {
            if (V_VT(&g_Options[i]) == VT_BYREF) {
                ASSERT(Options[i].flag & 0x04 && V_BYREF(&g_Options[i]) != NULL);

                for (list<VARIANT> * current = (list<VARIANT>*)V_BYREF(&g_Options[i]);  // Init
                    current != NULL && SUCCEEDED(hr);                                       // Condition
                    current = current->next) {                                              // Increment
                    hr = pLinker->SetAssemblyProps( assemID, assemID, Options[i].opt, current->arg);
                }
            } else {
                hr = pLinker->SetAssemblyProps( assemID, assemID, Options[i].opt, g_Options[i]);
                if (hr == S_FALSE) {
                    ASSERT(!g_bCopyOptions.get(Options[i].opt)); // We shouldn't have copied this from the template
                    g_output.ShowErrorIdString(WRN_OptionConflicts, ERROR_WARNING, Options[i].LongName);
                }
            }
        }
    }

    return hr;
}

HRESULT IsNonGeneric( IMetaDataImport * pImport, mdToken tkClassOrMethod)
{
    mdGenericParam gen;
    ULONG cnt = 0;
    HRESULT hr;
    HCORENUM hEnum = 0;
    CComPtr<IMetaDataImport2> pImport2;

    hr = pImport->QueryInterface(IID_IMetaDataImport2, (void**)&pImport2);
    if (hr == E_NOINTERFACE)
        return S_FALSE;
    else
    if (FAILED(hr))
        return hr;

    hr = pImport2->EnumGenericParams( &hEnum, tkClassOrMethod, &gen, 1, &cnt);
    pImport2->CloseEnum(hEnum);

    if (FAILED(hr))
        return hr;

    return (cnt == 0) ? S_OK : S_FALSE;
}
