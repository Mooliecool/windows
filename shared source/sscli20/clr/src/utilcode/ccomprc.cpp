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
#include "stdafx.h"                     // Standard header.
#include <utilcode.h>                   // Utility helpers.
#include <corerror.h>
#include <ndpversion.h>



//*****************************************************************************
// Do the mapping from an langId to an hinstance node
//*****************************************************************************
HRESOURCEDLL CCompRC::LookupNode(int langId)
{
    LEAF_CONTRACT;

    if (m_pHash == NULL) return NULL;

// Linear search
    int i;
    for(i = 0; i < m_nHashSize; i ++) {
        if (m_pHash[i].m_LangId == langId) {
            return m_pHash[i].m_hInst;
        }
    }

    return NULL;
}

//*****************************************************************************
// Add a new node to the map and return it.
//*****************************************************************************
const int MAP_STARTSIZE = 7;
const int MAP_GROWSIZE = 5;

HRESULT CCompRC::AddMapNode(int langId, HRESOURCEDLL hInst)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END;

    
    if (m_pHash == NULL) {
        m_pHash = new (nothrow)CCulturedHInstance[MAP_STARTSIZE];        
        if (m_pHash==NULL)
            return E_OUTOFMEMORY;
        m_nHashSize = MAP_STARTSIZE;
    }

// For now, place in first open slot
    int i;
    for(i = 0; i < m_nHashSize; i ++) {
        if (m_pHash[i].m_LangId == 0) {
            m_pHash[i].m_LangId = langId;
            m_pHash[i].m_hInst = hInst;
            return S_OK;
        }
    }

// Out of space, regrow
    CCulturedHInstance * pNewHash = new (nothrow)CCulturedHInstance[m_nHashSize + MAP_GROWSIZE];
    if (pNewHash)
    {
        memcpy(pNewHash, m_pHash, sizeof(CCulturedHInstance) * m_nHashSize);
        delete [] m_pHash;
        m_pHash = pNewHash;
        m_pHash[m_nHashSize].m_LangId = langId;
        m_pHash[m_nHashSize].m_hInst = hInst;
        m_nHashSize += MAP_GROWSIZE;
    }
    else
        return E_OUTOFMEMORY;
    return S_OK;
}

//*****************************************************************************
// Initialize
//*****************************************************************************
WCHAR* CCompRC::m_pDefaultResource = L"mscorrc.satellite";

HRESULT CCompRC::Init(__in_opt WCHAR* pResourceFile)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END;

    // Make sure to NEVER null out the function callbacks in the Init
    // function. They get set for the "Default CCompRC" during EEStartup
    // and we want to make sure we don't wipe them out.
    NewHolder<WCHAR> pwszResourceFile(NULL);
    
    if(pResourceFile) {
        DWORD lgth = (DWORD) wcslen(pResourceFile) + 1;
        pwszResourceFile = new(nothrow) WCHAR[lgth];
        if (pwszResourceFile == NULL)
            return E_OUTOFMEMORY;
        wcscpy_s(pwszResourceFile, lgth, pResourceFile);
    }
    else
        m_pResourceFile = m_pDefaultResource;
    

    // NOTE: there are times when the debugger's helper thread is asked to do a favor for another thread in the
    // process. Typically, this favor involves putting up a dialog for the user. Putting up a dialog usually ends
    // up involving the CCompRC code since (of course) the strings in the dialog are in a resource file. Thus, the
    // debugger's helper thread will attempt to acquire this CRST. This is okay, since the helper thread only does
    // these favors for other threads when there is no debugger attached. Thus, there are no deadlock hazards with
    // this lock, and its safe for the helper thread to take, so this CRST is marked with CRST_DEBUGGER_THREAD.
    m_csMap = ClrCreateCriticalSection("CCompRC", CrstCCompRC,
                                       (CrstFlags)(CRST_UNSAFE_ANYMODE | CRST_DEBUGGER_THREAD));

    if (m_csMap == NULL)
        return E_OUTOFMEMORY;

    if (pResourceFile)
        m_pResourceFile = pwszResourceFile.Extract();
    
    return S_OK;
}

void CCompRC::SetResourceCultureCallbacks(
        FPGETTHREADUICULTURENAMES fpGetThreadUICultureNames,
        FPGETTHREADUICULTUREID fpGetThreadUICultureId)
{
    LEAF_CONTRACT;

    m_fpGetThreadUICultureNames = fpGetThreadUICultureNames;
    m_fpGetThreadUICultureId = fpGetThreadUICultureId;
}

void CCompRC::GetResourceCultureCallbacks(
        FPGETTHREADUICULTURENAMES* fpGetThreadUICultureNames,
        FPGETTHREADUICULTUREID* fpGetThreadUICultureId)
{
    LEAF_CONTRACT;

    if(fpGetThreadUICultureNames)
        *fpGetThreadUICultureNames=m_fpGetThreadUICultureNames;

    if(fpGetThreadUICultureId)
        *fpGetThreadUICultureId=m_fpGetThreadUICultureId;
}

void CCompRC::Destroy()
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
#ifdef      MODE_PREEMPTIVE
        MODE_PREEMPTIVE;
#endif
    }
    CONTRACTL_END;

    // Free all resource libraries

    //*****************************************************************************
    // Free the loaded library if we ever loaded it and only if we are not on
    // Win 95 which has a known bug with DLL unloading (it randomly unloads a
    // dll on shut down, not necessarily the one you asked for).  This is done
    // only in debug mode to make coverage runs accurate.
    //*****************************************************************************

#if defined(_DEBUG)
    if (m_Primary.m_hInst) {
        PAL_FreeSatelliteResource(m_Primary.m_hInst);
        m_Primary.m_hInst = NULL;
    }

    if (m_pHash != NULL) {
        int i;
        for(i = 0; i < m_nHashSize; i ++) {
            if (m_pHash[i].m_hInst != NULL) {
                PAL_FreeSatelliteResource(m_pHash[i].m_hInst);
                break;
            }
        }
    }
#endif

    // destroy map structure
    if(m_pResourceFile != m_pDefaultResource)
        delete [] m_pResourceFile;
    m_pResourceFile = NULL;

    if(m_csMap) {
        ClrDeleteCriticalSection(m_csMap);
        ZeroMemory(&(m_csMap), sizeof(CRITSEC_COOKIE));
    }

    if(m_pHash != NULL) {
        delete [] m_pHash;
        m_pHash = NULL;
    }
}


LONG    CCompRC::m_dwDefaultInitialized = 0;
CCompRC CCompRC::m_DefaultResourceDll;

CCompRC* CCompRC::GetDefaultResourceDll()
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
#ifdef      MODE_PREEMPTIVE
        MODE_PREEMPTIVE;
#endif
    }
    CONTRACTL_END;
    
    if (m_dwDefaultInitialized == -1)
        return &m_DefaultResourceDll;

    while(InterlockedCompareExchange(&m_dwDefaultInitialized, 1, 0) != 0)
    {
        // Someone has begun initializing... let's wait for them.
        ClrSleepEx(1, FALSE);

        if (m_dwDefaultInitialized == -1)
            return &m_DefaultResourceDll;

        // It's also possible that they failed the initialization. We'll try and initialize again.
    }

    if(FAILED(m_DefaultResourceDll.Init(NULL)))
    {
        // We're not going to initialize this after all....
        m_dwDefaultInitialized = 0;
        return NULL;
    }
    m_dwDefaultInitialized = -1;
    
    return &m_DefaultResourceDll;
}

//*****************************************************************************
//*****************************************************************************

HRESULT CCompRC::GetLibrary(int langId, HRESOURCEDLL* phInst)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
#ifdef      MODE_PREEMPTIVE
        MODE_PREEMPTIVE;
#endif
        PRECONDITION(phInst != NULL);
    }
    CONTRACTL_END;

    HRESULT     hr = E_FAIL;
    HRESOURCEDLL    hInst = 0;
    HRESOURCEDLL    hLibInst = 0; //Holds early library instance
    BOOL        fLibAlreadyOpen = FALSE; //Determine if we can close the opened library.

    // Try to match the primary entry, or else use the primary if we don't care.
    if (m_Primary.m_LangId == langId || 
        (m_Primary.m_LangId != 0 && langId == (int)UICULTUREID_DONTCARE)) {
        hInst = m_Primary.m_hInst;
        hr = S_OK;
    }
    // If this is the first visit, we must set the primary entry
    else if (m_Primary.m_LangId == 0) {
        IfFailRet(LoadLibrary(&hLibInst));
        
        CRITSEC_Holder csh (m_csMap);
        // As we expected
        if (m_Primary.m_LangId == 0) {
            hInst = m_Primary.m_hInst = hLibInst;
            m_Primary.m_LangId = langId;
        }
        
        // Someone got into this critical section before us and set the primary already
        else if (m_Primary.m_LangId == langId) {
            hInst = m_Primary.m_hInst;
            fLibAlreadyOpen = TRUE;
        }
        
        // If neither case is true, someone got into this critical section before us and
        //  set the primary to other than the language we want...
        else
            fLibAlreadyOpen = TRUE;
        
        if (fLibAlreadyOpen)
        {
            PAL_FreeSatelliteResource(hLibInst);
            fLibAlreadyOpen = FALSE;
        }
    }


    // If we enter here, we know that the primary is set to something other than the
    // language we want - multiple languages use the hash table
    if (hInst == NULL) {

        // See if the resource exists in the hash table
        {
            CRITSEC_Holder csh (m_csMap);
            hInst = LookupNode(langId);
        }

        // If we didn't find it, we have to load the library and insert it into the hash
        if (hInst == NULL) 
        {
            IfFailRet(LoadLibrary(&hLibInst));
            {
                CRITSEC_Holder csh (m_csMap);
                
                // Double check - someone may have entered this section before us
                hInst = LookupNode(langId);
                if (hInst == NULL) {
                    hInst = hLibInst;
                    hr=AddMapNode(langId, hInst);  
                }
                else
                    fLibAlreadyOpen = TRUE;
            }

            if (fLibAlreadyOpen || FAILED(hr)) {
                PAL_FreeSatelliteResource(hLibInst);
            }
        }
        else 
        {
            hr = S_OK;
        }
    }

    *phInst = hInst;
    return hr;
}



//*****************************************************************************
// Load the string 
// We load the localized libraries and cache the handle for future use.
// Mutliple threads may call this, so the cache structure is thread safe.
//*****************************************************************************
HRESULT CCompRC::LoadString(UINT iResourceID, __out_ecount(iMax) LPWSTR szBuffer, int iMax, int bQuiet, int *pcwchUsed)
{
    WRAPPER_CONTRACT;
    LCID langId;
    // Must resolve current thread's langId to a dll.   
    if(m_fpGetThreadUICultureId) {
        langId = (*m_fpGetThreadUICultureId)();
        
        // Callback can't return 0, since that indicates empty.
        // To indicate empty, callback should return UICULTUREID_DONTCARE
        _ASSERTE(langId != 0);
        
    }
    else {
        langId = UICULTUREID_DONTCARE;
    }
    

    return LoadString(langId, iResourceID, szBuffer, iMax, bQuiet, pcwchUsed);
}

HRESULT CCompRC::LoadString(LCID langId, UINT iResourceID, __out_ecount(iMax) LPWSTR szBuffer, int iMax, int bQuiet, int *pcwchUsed)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
#ifdef      MODE_PREEMPTIVE
        MODE_PREEMPTIVE;
#endif
    }
    CONTRACTL_END;

    HRESULT         hr;
    HRESOURCEDLL    hInst = 0; //instance of cultured resource dll

    IfFailGo(GetLibrary(langId, &hInst));

    // Now that we have the proper dll handle, load the string
    _ASSERTE(hInst != NULL);

    int length;
    length = PAL_LoadSatelliteStringW(hInst, iResourceID, szBuffer, iMax);

    if(length > 0) {
        if(pcwchUsed) {
            *pcwchUsed = length;
        }
        return (S_OK);
    }
    if (GetLastError()==ERROR_OUTOFMEMORY)
    {
        hr=E_OUTOFMEMORY;
    }
    else
    {
        // Allows caller to check for string not found without extra debug checking.
        if (bQuiet)
            hr = E_FAIL;
        else 
        {
            _ASSERTE(0);
            hr = HRESULT_FROM_GetLastError();
        }
    }
ErrExit:
    // Return an empty string to save the people with a bad error handling
    if (szBuffer && iMax)
        *szBuffer = L'\0';

    return hr;
}

HRESULT CCompRC::LoadResourceFile(HRESOURCEDLL * pHInst, LPCWSTR lpFileName)
{
    if ((*pHInst = PAL_LoadSatelliteResourceW(lpFileName)) == NULL) {
        return HRESULT_FROM_GetLastError();
    }

    return S_OK;
}

//*****************************************************************************
// Load the library for this thread's current language
// Called once per language. 
// Search order is: 
//  1. Dll in localized path (<dir of this module><lang name (en-US format)>\mscorrc.dll)
//  2. Dll in localized (parent) path (<dir of this module><lang name> (en format)\mscorrc.dll)
//  3. Dll in root path (<dir of this module>\mscorrc.dll)
//  4. Dll in current path   (<current dir>\mscorrc.dll)
//*****************************************************************************
HRESULT CCompRC::LoadLibraryHelper(HRESOURCEDLL *pHInst,
                                   __out_ecount(rcPathSize) __out_z WCHAR *rcPath, const DWORD rcPathSize)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
#ifdef      MODE_PREEMPTIVE
        MODE_PREEMPTIVE;
#endif
    }
    CONTRACTL_END;
    
    HRESULT     hr = E_FAIL;
    const int   MAX_LANGPATH = 20;

    WCHAR       rcDrive[_MAX_DRIVE];    // Volume name.
    WCHAR       rcDir[_MAX_PATH];       // Directory.
    WCHAR       rcCurrentLanguage[MAX_LANGPATH + 2];   // extension to path for language
    WCHAR       rcParentLanguage[MAX_LANGPATH + 2];   // extension to path for language

    DWORD       rcDriveLen;
    DWORD       rcDirLen;
    DWORD       rcLangLen;
    DWORD       rcPartialPathLen;


    _ASSERTE(m_pResourceFile != NULL);

    DWORD  rcMscorrcLen = Wszlstrlen(m_pResourceFile);

    // Try and get both the culture and the parent culture's name         

    if (m_fpGetThreadUICultureNames) {
        hr = (*m_fpGetThreadUICultureNames)(rcCurrentLanguage, MAX_LANGPATH, rcParentLanguage, MAX_LANGPATH);
        
        if (SUCCEEDED(hr)) {

            if (*rcCurrentLanguage)
                wcscat_s(rcCurrentLanguage, NumItems(rcCurrentLanguage), L"\\");
            if (*rcParentLanguage)
                wcscat_s(rcParentLanguage, NumItems(rcParentLanguage), L"\\");
        }
    } else {
        rcCurrentLanguage[0] = 0;
        rcParentLanguage[0] = 0;
    }

    // Import directory from path
    wcscpy_s(rcDir, _MAX_PATH, rcPath);
    rcDrive[0] = L'\0';

    rcDriveLen = Wszlstrlen(rcDrive);
    rcDirLen   = Wszlstrlen(rcDir);
    
    // Length that does not include culture name length
    rcPartialPathLen = rcDriveLen + rcDirLen + rcMscorrcLen + 1;

    // Use either the current thread culture language, the parent culture language, or the
    // default (English) language

    WCHAR* wszLangs[] = {rcCurrentLanguage, rcParentLanguage, L""};
    DWORD i=0;

    do
    {
        rcLangLen = Wszlstrlen(wszLangs[i]);
        if (rcPartialPathLen + rcLangLen <= rcPathSize)
        {
            wcscpy_s(rcPath, rcDriveLen+1, rcDrive);
            WCHAR *rcPathPtr = rcPath + rcDriveLen;

            wcscpy_s(rcPathPtr, rcDirLen+1, rcDir);
            rcPathPtr += rcDirLen;

            wcscpy_s(rcPathPtr, rcLangLen+1, wszLangs[i]);
            wcscpy_s(rcPathPtr + rcLangLen, rcMscorrcLen+1, m_pResourceFile);

            // Feedback for debugging to eliminate unecessary loads.
            DEBUG_STMT(DbgWriteEx(L"Loading %s to load strings.\n", rcPath));

            // Load the resource library as a data file, so that the OS doesn't have
            // to allocate it as code.  This only works so long as the file contains
            // only strings.
            hr = LoadResourceFile(pHInst, rcPath);
        }
        else
        {
            _ASSERTE(!"Buffer not big enough");
            hr = E_FAIL;
        
        }
        i++;
    } while(FAILED(hr) && i < NumItems(wszLangs));
    
    // Last ditch search effort in current directory
    if (FAILED(hr)) {
        hr = LoadResourceFile(pHInst, m_pResourceFile);
    }

    return hr;
}

// Two-stage approach:
// First try module directory, then try CORSystemDirectory for default resource
HRESULT CCompRC::LoadLibrary(HRESOURCEDLL * pHInst)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
#ifdef      MODE_PREEMPTIVE
        MODE_PREEMPTIVE;
#endif
    }
    CONTRACTL_END;
    
    WCHAR       rcPath[_MAX_PATH];      // Path to resource DLL.

    HRESULT hr = S_OK;
    
    _ASSERTE(pHInst != NULL);

    // Try first in the same directory as this dll.
    if (!PAL_GetPALDirectory(rcPath, NumItems(rcPath)))
        return HRESULT_FROM_GetLastError();

    hr = LoadLibraryHelper(pHInst, rcPath, NumItems(rcPath));


    return hr;
}
