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

#include "pch.h"
#include "satellite.h"

#define UILANGUAGE_REG_KEY "Software\\Microsoft\\VisualStudio\\8.0\\General"
#define UILANGUAGE_REG_VALUE "UILanguage"


// List of fallback langids we try (in order) if we can't find the messages DLL by normal means.
// These are just a bunch of common languages -- not necessarily all languages we localize to.
// This list should never be used in usual course of things -- it's just an "emergency" fallback.
const LANGID g_fallbackLangs[] = {
    MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
    MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
};


// Returns true iff the language specificed by langid is displayable in the current console code-page.
// If the user has their language settings set to something which is not displayable in the console, then
// we want to load resources for a different language.
bool static LanguageMatchesCP(LANGID langid)
{
    // Note:  This function is also implemented in alinklib.cpp for alink.


    return true;
}


struct PreInitedWFunctions{
private:
    bool triedInit;
    bool hasUnicodeFunctions;
    LPCSTR uiDllNameA;
    CStringW uiDllNameW;
public:
    PreInitedWFunctions(LPCSTR _uiDllName) {
        triedInit = hasUnicodeFunctions = false;
        uiDllNameA = _uiDllName;
        uiDllNameW = uiDllNameA;
    }
    bool HasWFunctions()
    {
        TryInit();
        return true;
    }
    void TryInit()
    {
        if (triedInit) return;
        triedInit = true;
    }
    DWORD GetModuleFileNameW(LPCWSTR lpFileName, DWORD nSize)
    {
        if (HasWFunctions()) {
            return PAL_GetPALDirectoryW((LPWSTR) lpFileName, nSize);
        }
        VSFAIL("W functions missing yet called.");
        return 0;
    }
    DWORD GetModuleFileNameA(LPCSTR lpFileName, DWORD nSize)
    {
        return PAL_GetPALDirectoryA((LPSTR) lpFileName, nSize);
    }
    HMODULE LoadLibraryW(LPCWSTR lpFilename)
    {
        if (HasWFunctions()) {
            return PAL_LoadSatelliteResourceW(lpFilename);
        }
        VSFAIL("W functions missing yet called.");
        return NULL;
    }    
    HMODULE LoadLibraryA(LPCSTR lpFilename)
    {
        return PAL_LoadSatelliteResourceA(lpFilename);
    }    
    LPCWSTR GetWName()
    {
        return uiDllNameW;
    }
    LPCSTR GetAName()
    {
        return uiDllNameA;
    }
};

// Try to load a message DLL from a location, in a subdirectory of mine given by the LANGID
// passed in (or the same directory if -1).
static HINSTANCE FindMessageDll(LANGID langid, bool fCheckLangID, PreInitedWFunctions * wFunctions)
{
    // The specified language ID is not valid for the current console code page, 
    // so don't search for the resource dll
    if (langid != (LANGID)-1 && fCheckLangID && !LanguageMatchesCP(langid))
        return 0;

    if (wFunctions->HasWFunctions()) {
        WCHAR path[MAX_PATH];
        WCHAR *pEnd;


        if (! wFunctions->GetModuleFileNameW(path, lengthof(path)))
            return 0;

        // Force null termination
        path[lengthof(path)-1] = L'\0';

        pEnd = wcsrchr(path, L'\\');
    #ifdef PLATFORM_UNIX
        WCHAR* pEndSlash = wcsrchr(path, L'/');
        if (pEndSlash > pEnd) {
            pEnd = pEndSlash;
        }
    #endif  // PLATFORM_UNIX
        if (!pEnd)
            return 0;

        ++pEnd;
        *pEnd = L'\0';  // nul terminate and point to the nul

        // Append language ID
        if (langid != (LANGID)-1) {
            if (FAILED(StringCchPrintfW(pEnd, lengthof(path) - (pEnd - path), L"%d\\", langid)))
                return 0;
        }

        // Append message DLL name.
        if (FAILED(StringCchCatW(path, lengthof(path), wFunctions->GetWName())))
            return 0;

        return wFunctions->LoadLibraryW(path);
    } else {

        CHAR path[MAX_PATH];
        CHAR *pEnd;

        if (! wFunctions->GetModuleFileNameA(path, lengthof(path)))
            return 0;

        // Force null termination
        path[lengthof(path)-1] = '\0';

        pEnd = strrchr(path, '\\');
    #ifdef PLATFORM_UNIX
        CHAR* pEndSlash = strrchr(path, '/');
        if (pEndSlash > pEnd) {
            pEnd = pEndSlash;
        }
    #endif  // PLATFORM_UNIX
        if (!pEnd)
            return 0;

        ++pEnd;
        *pEnd = '\0';  // nul terminate and point to the nul

        // Append language ID
        if (langid != (LANGID)-1) {
            if (FAILED(StringCchPrintfA(pEnd, lengthof(path) - (pEnd - path), "%d\\", langid)))
                return 0;
        }

        // Append message DLL name.
        if (FAILED(StringCchCatA(path, lengthof(path), wFunctions->GetAName())))
            return 0;

        return wFunctions->LoadLibraryA(path);
    }

}

LANGID GetUsersPreferredUILanguage()
{
    // first try GetUserDefaultUILanguage if we're on WinXP
    LANGID langid = 0;

    // Next try user locale.
    langid = GetUserDefaultLangID();
    if (langid)
        return langid;

    
    langid = LANGIDFROMLCID(GetThreadLocale());
    if (langid)
        return langid;

    // and finally System
    langid = GetSystemDefaultLangID();
    return langid;
}

HINSTANCE FindMessageDllTryDefaultLang(LANGID *langid, bool fCheckLangID, PreInitedWFunctions * wFunctions)
{
    HINSTANCE hModuleMessagesLocal = FindMessageDll(*langid, fCheckLangID, wFunctions);
    if (!hModuleMessagesLocal) {
        *langid = MAKELANGID(PRIMARYLANGID(*langid), SUBLANG_DEFAULT);
        hModuleMessagesLocal = FindMessageDll(*langid, fCheckLangID, wFunctions);
    }
        
    return hModuleMessagesLocal;
}

HINSTANCE GetMessageDllWorker(HMODULE & hModuleMessages, LANGID providedId, LPCSTR uiDllname, LANGID * langIdFound)
{
    LANGID langid;
    PreInitedWFunctions wFunctions(uiDllname);

    if (! hModuleMessages) {

        HINSTANCE hModuleMessagesLocal = 0;

        // Next try user's preferred language
        if (!hModuleMessagesLocal) {
            langid = GetUsersPreferredUILanguage();
            hModuleMessagesLocal = FindMessageDllTryDefaultLang(&langid, true, &wFunctions);
        }

        // Try a fall-back list of locales.
        if (!hModuleMessagesLocal) {
            for (unsigned int i = 0; i < lengthof(g_fallbackLangs); ++i) {
                langid = g_fallbackLangs[i];
                hModuleMessagesLocal = FindMessageDll(langid, false, &wFunctions);
                if (hModuleMessagesLocal)
                    break;
            }
        }

        // Try current directory.
        if (!hModuleMessagesLocal) {
            langid = (LANGID)-1;
            hModuleMessagesLocal = FindMessageDll((LANGID)-1, false, &wFunctions);
        }

        if (hModuleMessagesLocal)
        {
            if (InterlockedCompareExchangePointer( (void**)&hModuleMessages, hModuleMessagesLocal, NULL)) {
                ASSERT(hModuleMessages != hModuleMessagesLocal);
                PAL_FreeSatelliteResource((HSATELLITE)hModuleMessagesLocal);
            } else {
                ASSERT(hModuleMessagesLocal == hModuleMessages);
            }
            if (langIdFound) {
                *langIdFound = langid;
            }
            GetATLModule().m_hInstResource = hModuleMessages;
        }

    }
    return hModuleMessages;
}
