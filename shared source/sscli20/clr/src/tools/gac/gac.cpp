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
//
// gac.cpp : Utility for use with fusion's cache


#include <windows.h>
#include <stdio.h>
#include <corerror.h>
#include <cor.h>
#include <ndpversion.h>
#include "list.h"
#include "mscoree.h"
#include "fusion.h"
#include "shlwapi.h"
#include "gacwrap.h"
#include "safemath.h"
#include "resource.h"
#include "strsafe.h"

#include <palstartupw.h>


//
// Function pointers late bound calls to Fusion.  Fusion is delay loaded for side by side
//
typedef HRESULT (__stdcall *CreateAsmCache)(IAssemblyCache **ppAsmCache, DWORD dwReserved);
typedef HRESULT (__stdcall *CreateAsmNameObj)(LPASSEMBLYNAME *ppAssemblyNameObj, LPCWSTR szAssemblyName, DWORD dwFlags, LPVOID pvReserved);
typedef HRESULT (__stdcall *CreateAsmEnum)(IAssemblyEnum **pEnum, IUnknown *pAppCtx, IAssemblyName *pName, DWORD dwFlags, LPVOID pvReserved);

CreateAsmCache g_pfnCreateAssemblyCache = NULL;
CreateAsmNameObj g_pfnCreateAssemblyNameObject = NULL;
CreateAsmEnum g_pfnCreateAssemblyEnum = NULL;

HMODULE          g_FusionDll = NULL;

HSATELLITE      g_hResourceInst = NULL;

bool g_bdisplayLogo = true;
bool g_bSilent = false;

// commands
INT                         g_bInstall          = 0;        //  -i
INT                         g_bInstallList      = 0;        //  -il
INT                         g_bUninstall        = 0;        //  -u
INT                         g_bUninstallAllRefs = 0;        //  -uf 
INT                         g_bUninstallList    = 0;        //  -ul
INT                         g_bListAsm          = 0;        //  -l
INT                         g_bListAsmRefs      = 0;        //  -lr
INT                         g_bListDLCache      = 0;        //  -ldl
INT                         g_bClearDLCache      = 0;        //  -cdl
INT                         g_bPrintHelp        = 0;        //  -?

// arguments/options
bool                        g_bForceInstall     = false;    // -f
FUSION_INSTALL_REFERENCE   *g_pInstallReference = NULL;     // -r
LPCWSTR                     g_pAsmListFileName  = NULL;     
LPCWSTR                     g_pAsmFileName      = NULL;
LPCWSTR                     g_pAsmDisplayName   = NULL;

#define MAX_BUFFER_SIZE     1024

WCHAR       g_wzBuffer[MAX_BUFFER_SIZE];
WCHAR       g_wzFormat[MAX_BUFFER_SIZE];


void PrintString(LPCWSTR pwzString) 
{
    printf("%S", pwzString);
}

#define BSILENT_PRINTF0ARG(id)        if(!g_bSilent) { if(WszLoadString(g_hResourceInst, id, g_wzBuffer, ARRAYSIZE(g_wzBuffer))) { PrintString(g_wzBuffer); } }
#define BSILENT_PRINTF1ARG(id, arg1)  \
    do {\
        if(!g_bSilent) { \
    if(WszLoadString(g_hResourceInst, id, g_wzFormat, ARRAYSIZE(g_wzFormat))) { \
    if (SUCCEEDED(StringCchPrintf(g_wzBuffer, ARRAYSIZE(g_wzBuffer), g_wzFormat, arg1))) { \
         PrintString(g_wzBuffer); \
    }}}}while(0)
        
#define BSILENT_PRINTF2ARG(id, arg1, arg2)  \
    do {\
        if(!g_bSilent) { \
    if(WszLoadString(g_hResourceInst, id, g_wzFormat, ARRAYSIZE(g_wzFormat))) { \
    if (SUCCEEDED(StringCchPrintf(g_wzBuffer, ARRAYSIZE(g_wzBuffer), g_wzFormat, arg1, arg2))) { \
         PrintString(g_wzBuffer); \
    }}}}while(0)

#define MAX_COUNT MAX_PATH

bool FusionInit(void)
{
    bool        fSuccess = false;

    g_FusionDll = LoadLibraryA(MSCOREE_SHIM_A);
    if (!g_FusionDll) {
        goto Exit;
    }

    //
    // Save pointers to call through later
    //
    if ((g_pfnCreateAssemblyCache      = (CreateAsmCache)GetProcAddress(g_FusionDll, "CreateAssemblyCache")) == NULL) goto Exit;
    if ((g_pfnCreateAssemblyNameObject = (CreateAsmNameObj)GetProcAddress(g_FusionDll, "CreateAssemblyNameObject")) == NULL) goto Exit;
    if ((g_pfnCreateAssemblyEnum       = (CreateAsmEnum)GetProcAddress(g_FusionDll, "CreateAssemblyEnum")) == NULL) goto Exit;

    fSuccess = true;

Exit:
    return fSuccess;
}

void Title()
{
    if (g_bSilent) return;

    PrintString(L"Microsoft (R) Shared Source CLI Global Assembly Cache Utility.  Version " VER_FILEVERSION_STR_L);
    PrintString(L"\r\n");
    PrintString(VER_LEGALCOPYRIGHT_LOGO_STR_L);
    PrintString(L"\r\n\r\n");
}

void ShortUsage()
{
    if (g_bSilent) return;

    WCHAR   wzBuffer[512];
    UINT    uID = IDS_USAGE_SHORT_FIRST;

    do {
        wzBuffer[0] = L'\0';
        if(WszLoadString(g_hResourceInst, uID, wzBuffer, ARRAYSIZE(wzBuffer))) {
            PrintString(wzBuffer);
        }
        uID++;
    } while(uID <= IDS_USAGE_SHORT_LAST);
}

void LongUsage()
{
    if (g_bSilent) return;

    WCHAR   wzBuffer[512];
    UINT    uID = IDS_USAGE_LONG_FIRST;

    do {
        wzBuffer[0] = L'\0';
        if(WszLoadString(g_hResourceInst, uID, wzBuffer, ARRAYSIZE(wzBuffer))) {
            PrintString(wzBuffer);
        }
        uID++;
    } while(uID <= IDS_USAGE_LONG_LAST);
}

void ReportError(HRESULT hr)
{
    LPVOID lpMsgBuf = NULL;
    WCHAR  wzErrorStringFmt[MAX_BUFFER_SIZE];

    //
    // First, check to see if this is one of the Fusion HRESULTS
    //
    if (hr == FUSION_E_PRIVATE_ASM_DISALLOWED)
    {
        BSILENT_PRINTF0ARG(IDS_ERROR_NO_STRONG_NAME);
    }
    else if (hr == FUSION_E_SIGNATURE_CHECK_FAILED)
    {
        BSILENT_PRINTF0ARG(IDS_ERROR_INVALID_SIGNATURE);     
    }
    else if (hr == FUSION_E_ASM_MODULE_MISSING)
    {
        BSILENT_PRINTF0ARG(IDS_ERROR_MISSING_MODULE);
    }
    else if (hr == FUSION_E_UNEXPECTED_MODULE_FOUND)
    {
        BSILENT_PRINTF0ARG(IDS_ERROR_INVALID_HASH);
    }
    else if (hr == FUSION_E_DATABASE_ERROR)
    {
        BSILENT_PRINTF0ARG(IDS_ERROR_UNEXPECTED);
    }
    else if (hr == FUSION_E_INVALID_NAME)
    {
        BSILENT_PRINTF0ARG(IDS_ERROR_INVALIDNAME);
    }
    else if (hr == FUSION_E_UNINSTALL_DISALLOWED)
    {
        BSILENT_PRINTF0ARG(IDS_ERROR_UNINSTALL_DISALLOWED);
    }
    else if(hr == HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED)) {
        BSILENT_PRINTF0ARG(IDS_ERROR_ACCESS_DENIED);
    }
    else {
        //
        // Try the system messages
        //
        WszFormatMessage( 
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM | 
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            hr,
            0,
            (LPTSTR) &lpMsgBuf,
            0,
            NULL); 
        if (lpMsgBuf)
        {
            BSILENT_PRINTF1ARG(IDS_OUTPUT_FORMAT, (LPTSTR)((LPTSTR)lpMsgBuf));
            (LocalFree) ( lpMsgBuf );
            goto Exit;
        }
        //
        // Try mscorrc.dll
        //
        if (HRESULT_FACILITY(hr) == FACILITY_URT && HRESULT_CODE(hr) < MAX_URT_HRESULT_CODE) {
            if (SUCCEEDED(LoadStringRC(MSG_FOR_URT_HR(hr), wzErrorStringFmt, ARRAYSIZE(wzErrorStringFmt), TRUE))) {
                BSILENT_PRINTF1ARG(IDS_OUTPUT_FORMAT, wzErrorStringFmt);
                goto Exit;
            }
        }
        //
        // default message
        //
        BSILENT_PRINTF1ARG(IDS_UNKNOWN_ERROR, hr);        
    }
Exit:
    return;
}


bool InstallAssembly(LPCWSTR pszManifestFile, LPFUSION_INSTALL_REFERENCE pInstallReference, DWORD dwFlag)
{

    IAssemblyCache*     pCache      = NULL;
    HRESULT             hr          = S_OK;
    
    hr = (*g_pfnCreateAssemblyCache)(&pCache, 0);
    if (FAILED(hr))
    {
        BSILENT_PRINTF0ARG(IDS_INSTALLATION_FAILURE);
        ReportError(hr);
        if (pCache) pCache->Release();
        return false;
    }


    hr = pCache->InstallAssembly(dwFlag, pszManifestFile, pInstallReference);
    if (hr==S_FALSE)
    {
        BSILENT_PRINTF0ARG(IDS_ITEM_ALREADY_INSTALLED);
        pCache->Release();
        return true;
    }
    else
    if (SUCCEEDED(hr))
    {
        BSILENT_PRINTF0ARG(IDS_INSTALL_SUCCESSFULL);
        pCache->Release();
        return true;
    }
    else
    {
        BSILENT_PRINTF0ARG(IDS_INSTALLATION_FAILURE);
        ReportError(hr);
        pCache->Release();
        return false;
    }
}

bool InstallListOfAssemblies(LPCWSTR pwzAssembliesListFile, LPFUSION_INSTALL_REFERENCE pInstallReference, DWORD dwFlag)
{
    IAssemblyCache*     pCache      = NULL;
    HRESULT             hr          = S_OK;
    HANDLE              hFile       = INVALID_HANDLE_VALUE;
    DWORD               dwTotal     = 0;
    DWORD               dwFailed    = 0;
    DWORD               dwSize      = MAX_PATH;
    WCHAR               wzAsmFullPath[MAX_PATH];
    LPWSTR              wzFileName  = NULL;
    WCHAR               wzPath[MAX_PATH];
    WCHAR               wzAsmName[MAX_PATH];
    CHAR                szAsmName[MAX_PATH];
    CHAR               *tmp;
    DWORD               dwBytesRead = 0;
    BOOL                bRes;
    BOOL                done = FALSE;
    bool                bSucceeded = false;

    DWORD               dwPathLength = 0;
    // get the path of pwzAssembliesListFile
    dwPathLength = WszGetFullPathName(pwzAssembliesListFile, MAX_PATH, wzPath, &wzFileName);
    if (!dwPathLength)
    {
        BSILENT_PRINTF1ARG(IDS_INVALID_PATH, pwzAssembliesListFile);
        ReportError(HRESULT_FROM_WIN32(GetLastError()));
        return false;
    }
    else if (dwPathLength > MAX_PATH)
    {
        BSILENT_PRINTF1ARG(IDS_FILEPATH_TO_LONG, pwzAssembliesListFile);
        return false;
    }

    assert(wzFileName != NULL);
    // we only need the path
    *wzFileName = L'\0';

    hFile = WszCreateFile(pwzAssembliesListFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        BSILENT_PRINTF1ARG(IDS_FAILED_TO_OPEN_LIST, pwzAssembliesListFile);
        return false;
    }

    hr = g_pfnCreateAssemblyCache(&pCache, 0);
    if (FAILED(hr))
    {
        BSILENT_PRINTF0ARG(IDS_INSTALLATION_FAILURES);
        ReportError(hr);
        goto Exit;
    }
    
    // Installing
    while(!done)
    {
        // get the file name
        tmp = szAsmName;
        *tmp = '\0';
        while(1)
        {
            if (tmp >= szAsmName + MAX_PATH)
            {
                BSILENT_PRINTF0ARG(IDS_INVALID_ASSEMBLY_FILENAME);
                goto Exit;
            }
            
            bRes = ReadFile( hFile, tmp, 1, &dwBytesRead, NULL);
            if (!bRes)
            {
                BSILENT_PRINTF1ARG(IDS_ERROR_READING_FILE, pwzAssembliesListFile);
                goto Exit;
            }
           
            // end of file
            if (!dwBytesRead) 
            {
                done = TRUE;
                *tmp=0;
                break;
            }
            
            if (*tmp == '\n') 
            {
                *tmp = 0;
                break;
            }
            else if ( *tmp != '\r' && !((*tmp == ' ' || *tmp == '\t')&&( tmp == szAsmName)) ) 
            {
                tmp++;
            }
        }
        
        if (lstrlenA(szAsmName))
        {
            if (!MultiByteToWideChar(CP_ACP, 0, szAsmName, -1, wzAsmName, dwSize))
            {
                //unicode convert failed?
                BSILENT_PRINTF0ARG(IDS_INSTALLATION_FAILURES);
                ReportError(HRESULT_FROM_WIN32(GetLastError()));
                goto Exit;           
            }

            if (lstrlenW(wzPath) + lstrlenW(wzAsmName) + 1 > MAX_PATH)
            {
                // path too long
                BSILENT_PRINTF0ARG(IDS_INVALID_ASSEMBLY_FILENAME);
                goto Exit;
            }
             
            if (!PathCombine(wzAsmFullPath, wzPath, wzAsmName))
            {
                BSILENT_PRINTF1ARG(IDS_ERROR_FAILED_TO_PROCESS, wzAsmName);
                goto Exit;
            }
            
            // Now install
            dwTotal++;
            hr = pCache->InstallAssembly(dwFlag, wzAsmFullPath, pInstallReference);
            if (hr==S_FALSE)
            {
                BSILENT_PRINTF1ARG(IDS_ERROR_ALREADY_EXISTS, wzAsmFullPath);
            }
            else
            if (SUCCEEDED(hr))
            {
                BSILENT_PRINTF1ARG(IDS_INSTALLATION_SUCCESS, wzAsmFullPath);
            }
            else
            {
                BSILENT_PRINTF1ARG(IDS_ERROR_INSTALLATION, wzAsmFullPath);
                ReportError(hr);
                dwFailed++;
            }
        }
    }

    if (dwFailed == 0)
        bSucceeded = true;
    
Exit:
    if (pCache)
        pCache->Release();
    if (hFile != INVALID_HANDLE_VALUE)
        CloseHandle(hFile);

    BSILENT_PRINTF1ARG(IDS_ASSEMBLIES_PROCESSED, dwTotal);
    BSILENT_PRINTF1ARG(IDS_ASSEMBLIES_INSTALLED, dwTotal-dwFailed);
    BSILENT_PRINTF1ARG(IDS_ASSEMBLIES_FAILED, dwFailed);   

    return bSucceeded;
   
}


bool UninstallListOfAssemblies(IAssemblyCache* pCache,
                                         __in_ecount(dwDispNameCount) LPWSTR arrpszDispName[],
                                         DWORD dwDispNameCount,
                                         LPCFUSION_INSTALL_REFERENCE pInstallReference,
                                         bool bRemoveAllRefs,
                                         DWORD *pdwFailures,
                                         DWORD *pdwOmitFromCount)
{
    HRESULT         hr              = S_OK;
    DWORD           dwCount         = 0;
    DWORD           dwFailures      = 0;
    DWORD           dwOmitFromCount = 0;
    bool            bOmit           = false;
    ULONG           ulDisp          = 0;

    while( dwDispNameCount > dwCount)
    {
        LPWSTR szDisplayName = arrpszDispName[dwCount];
        
        BSILENT_PRINTF1ARG(IDS_ASSEMBLY_NAME, szDisplayName);
        

            //Call uninstall with full display name (will always uninstall only 1 assembly)
            hr = pCache->UninstallAssembly(0, szDisplayName, pInstallReference, &ulDisp);

            if (hr == S_OK)
            {
                BSILENT_PRINTF1ARG(IDS_UNINSTALLED, szDisplayName);
            }
            else if (hr != S_FALSE)
            {
                dwFailures++;
                BSILENT_PRINTF1ARG(IDS_ERROR_UNINSTALL_FAILED_FOR, szDisplayName);
                ReportError(hr);
                hr = S_OK;
            }
            else // hr == S_FALSE
            {
                if ((ulDisp == IASSEMBLYCACHE_UNINSTALL_DISPOSITION_REFERENCE_NOT_FOUND) ||
                    (ulDisp == IASSEMBLYCACHE_UNINSTALL_DISPOSITION_HAS_INSTALL_REFERENCES))
                {
                }

                bOmit = true;
            }

        dwCount++;
        if (bOmit)
        {
            dwOmitFromCount++;
            bOmit = false;
        }
    }

    *pdwFailures = dwFailures;
    *pdwOmitFromCount = dwOmitFromCount;

    return true;
}

bool UninstallAssembly(LPCWSTR pszAssemblyName, LPCFUSION_INSTALL_REFERENCE pInstallReference, bool bRemoveAllRefs)
{
    IAssemblyCache* pCache          = NULL;
    IAssemblyName*  pEnumName       = NULL;
    IAssemblyName*  pAsmName        = NULL;
    IAssemblyEnum*  pEnum           = NULL;
    HRESULT         hr              = S_OK;
    DWORD           dwCount         = 0;
    DWORD           dwFailures      = 0;
    DWORD           dwTotalFailures  = 0;
    DWORD           dwOmitFromCount = 0;
    DWORD           dwTotalOmits     = 0;
    LPWSTR          szDisplayName = NULL;
    DWORD           dwLen       = 0;
    DWORD           dwDisplayFlags = ASM_DISPLAYF_FULL;

    LPWSTR arrpszDispName[1];
    DWORD dwDispNameCount=0;

    memset( (LPBYTE) arrpszDispName, 0, sizeof(arrpszDispName));

    hr = (*g_pfnCreateAssemblyCache)(&pCache, 0);
    if (FAILED(hr))
    {
        BSILENT_PRINTF0ARG(IDS_ERROR_UNINSTALL);
        ReportError(hr);
        return false;
    }
    
    //Name passed in may be partial, therefore enumerate matching assemblies
    //and uninstall each one. Uninstall API should be called with full name ref.

    //Create AssemblyName for enum
    if (FAILED(hr = (*g_pfnCreateAssemblyNameObject)(&pEnumName, pszAssemblyName, CANOF_PARSE_DISPLAY_NAME, NULL)))
    {
        BSILENT_PRINTF0ARG(IDS_ERROR_UNINSTALL);
        ReportError(hr);
        if (pCache) pCache->Release();
        return false;
    }

    hr = (*g_pfnCreateAssemblyEnum)(&pEnum, 
                            NULL, 
                            pEnumName,
                            ASM_CACHE_GAC, 
                            NULL);
    if (hr != S_OK)
    {
        BSILENT_PRINTF1ARG(IDS_NO_MATCHES_FOUND, pszAssemblyName);
    }

    if (pEnumName) {
        pEnumName->Release();
        pEnumName = NULL;
    }

    //Loop through assemblies and uninstall each one
    while (hr == S_OK)
    {
        hr = pEnum->GetNextAssembly(NULL, &pAsmName, 0);
        if (hr == S_OK)
        {
            dwLen = 0;
            hr = pAsmName->GetDisplayName(NULL, &dwLen, dwDisplayFlags);
            if (dwLen)
            {
                szDisplayName = new WCHAR[dwLen+1];
                if (szDisplayName) {
                    hr = pAsmName->GetDisplayName(szDisplayName, &dwLen, dwDisplayFlags);
                    if (SUCCEEDED(hr))
                    {
                        dwDispNameCount++;
                        dwFailures = 0;
                        dwOmitFromCount = 0;
                        arrpszDispName[0] = szDisplayName;
                        UninstallListOfAssemblies(pCache, 
                                         arrpszDispName,
                                         1,
                                         pInstallReference,
                                         bRemoveAllRefs,
                                         &dwFailures,
                                         &dwOmitFromCount);
                        dwTotalFailures += dwFailures;
                        dwTotalOmits += dwOmitFromCount;
                    }
                    else
                    {
                        BSILENT_PRINTF1ARG(IDS_HRERROR_DISPLAY, hr);
                    }

                    delete[] szDisplayName;
                }
                else {
                    BSILENT_PRINTF0ARG(IDS_ERROR_OUT_OF_MEMORY);
                    return false;
                }
            }

            if (pAsmName)
            {
                pAsmName->Release();
                pAsmName = NULL;
            }
        }
    }

    dwCount = dwDispNameCount - dwTotalFailures - dwTotalOmits;

    if (pEnum) pEnum->Release();

    BSILENT_PRINTF1ARG(IDS_ASSEMBLIES_UNINSTALLED,dwCount);
    BSILENT_PRINTF1ARG(IDS_ASSEMBLIES_FAILED,dwTotalFailures);
    
    if ((dwTotalFailures != 0) && (dwCount == 0))
        return false;
    else
        return true;
}

int EnumerateAssemblies(DWORD dwWhichCache, LPCWSTR pszAssemblyName, bool bPrintInstallRefs)
{
    HRESULT                 hr              = S_OK;
    IAssemblyEnum*          pEnum           = NULL;
    IAssemblyName*          pAsmName        = NULL;
    IAssemblyName*           pEnumName       = NULL;
    DWORD                   dwCount         = 0;
    WCHAR*                  szDisplayName   = NULL;
    DWORD                   dwLen           = 0;
    DWORD                   dwDisplayFlags  = ASM_DISPLAYF_FULL;

    if (pszAssemblyName)
    {
            if (FAILED(hr = (*g_pfnCreateAssemblyNameObject)(&pEnumName, pszAssemblyName, CANOF_PARSE_DISPLAY_NAME, NULL)))
            {
                BSILENT_PRINTF0ARG(IDS_ERROR_ENUMERATION);
                ReportError(hr);            
                return false;
            }
    }
    
    hr = (*g_pfnCreateAssemblyEnum)(&pEnum,
                                    NULL, 
                                    pEnumName,
                                    dwWhichCache,
                                    NULL);
    while (hr == S_OK)
    {
        hr = pEnum->GetNextAssembly(NULL, &pAsmName, 0);
        if (hr == S_OK)
        {
            dwCount++;
            dwLen = 0;
            hr = pAsmName->GetDisplayName(NULL, &dwLen, dwDisplayFlags);
            if (dwLen)
            {
                szDisplayName = new WCHAR[dwLen+1];
                if (!szDisplayName) {
                    pEnum->Release();
                    pAsmName->Release();
                    BSILENT_PRINTF0ARG(IDS_ERROR_ENUMERATION);
                    BSILENT_PRINTF0ARG(IDS_ERROR_OUT_OF_MEMORY);
                    return 0;
                }

                hr = pAsmName->GetDisplayName(szDisplayName, &dwLen, dwDisplayFlags);
                if (SUCCEEDED(hr))
                {
                    BSILENT_PRINTF1ARG(IDS_OUTPUT_FORMAT, szDisplayName);
                }
                else
                {
                    BSILENT_PRINTF1ARG(IDS_HRERROR_DISPLAY, hr);
                }
                delete [] szDisplayName;
                szDisplayName = NULL;
            }

            if (pAsmName)
            {

                pAsmName->Release();
                pAsmName = NULL;
            }
        }
    }
    
    if (pEnum)
    {
        pEnum->Release();
        pEnum = NULL;
    }

    if (pEnumName)
    {
        pEnumName->Release();
        pEnumName = NULL;
    }

    return dwCount;
}

bool UninstallListOfAssemblies(LPCWSTR pwzAssembliesListFile, LPFUSION_INSTALL_REFERENCE pInstallReference)
{
    IAssemblyCache     *pCache      = NULL; 
    HRESULT             hr          = S_OK;
    HANDLE              hFile       = INVALID_HANDLE_VALUE;
    DWORD               dwTotal     = 0;
    DWORD               dwFailed    = 0;
    DWORD               dwSize      = MAX_PATH;
    DWORD               dwDisp      = 0;
    WCHAR               wzAsmName[MAX_PATH];
    CHAR                szAsmName[MAX_PATH];
    CHAR               *tmp;
    DWORD               dwBytesRead = 0;
    BOOL                bRes;
    BOOL                done = FALSE;
    bool                bSucceeded = false;

    hFile = WszCreateFile(pwzAssembliesListFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
         BSILENT_PRINTF1ARG(IDS_FAILED_TO_OPEN_LIST, pwzAssembliesListFile);
        return false;
    }

    hr = (*g_pfnCreateAssemblyCache)(&pCache, 0);
    if (FAILED(hr))
    {
        BSILENT_PRINTF0ARG(IDS_ERROR_UNINSTALL);
        ReportError(hr);
        goto Exit;
    }

    // uninstalling
    while(!done)
    {
        // get the assembly display name
        tmp = szAsmName;
        *tmp = '\0';
        while(1)
        {
            if (tmp == szAsmName + MAX_PATH)
            {
                BSILENT_PRINTF0ARG(IDS_ERROR_INVALID_INPUT_FILE);
                goto Exit;
            }
            
            bRes = ReadFile( hFile, tmp, 1, &dwBytesRead, NULL);
            if (!bRes)
            {
                BSILENT_PRINTF1ARG(IDS_ERROR_READING_FILE, pwzAssembliesListFile);
                goto Exit;
            }
           
            // end of file
            if (!dwBytesRead) {
                done = TRUE;
                *tmp=0;
                break;
            }

            if (*tmp == '\n') 
            {
                *tmp = 0;
                break;
            }
            else if ( *tmp != '\r' && !((*tmp == ' ' || *tmp == '\t')&&( tmp == szAsmName)) ) 
            {
                tmp++;
            }
        }
        
        if (lstrlenA(szAsmName))
        {
            if (!MultiByteToWideChar(CP_ACP, 0, szAsmName, -1, wzAsmName, dwSize))
            {
                //unicode convert failed?
                BSILENT_PRINTF0ARG(IDS_INSTALLATION_FAILURES);
                ReportError(HRESULT_FROM_WIN32(GetLastError()));
                goto Exit;           
            }

            BSILENT_PRINTF1ARG(IDS_ASSEMBLY_NAME, wzAsmName);

            dwTotal++;
            hr = pCache->UninstallAssembly(0, wzAsmName, pInstallReference, &dwDisp);
            if (FAILED(hr))
            {
                dwFailed++;
                BSILENT_PRINTF1ARG(IDS_ERROR_UNINSTALL_FAILED_FOR, wzAsmName);
                ReportError(hr);
            }
            else if (hr == S_FALSE)
            {
                switch (dwDisp)
                {
                    case IASSEMBLYCACHE_UNINSTALL_DISPOSITION_ALREADY_UNINSTALLED:
                        BSILENT_PRINTF1ARG(IDS_NO_MATCHES_FOUND, wzAsmName);
                        dwFailed++;
                        break;
                    default:
                        dwFailed++;
                        break;
                }
            }
            else 
            {
                BSILENT_PRINTF1ARG(IDS_UNINSTALLED, wzAsmName);
            }
        }              
    }

    if (dwFailed == 0)
        bSucceeded = true;

Exit:
    if (pCache != NULL)
        pCache->Release();
    if (hFile != INVALID_HANDLE_VALUE)
        CloseHandle(hFile);
    
    BSILENT_PRINTF1ARG(IDS_ASSEMBLIES_PROCESSED, dwTotal);
    BSILENT_PRINTF1ARG(IDS_ASSEMBLIES_UNINSTALLED, dwTotal - dwFailed);
    BSILENT_PRINTF1ARG(IDS_ASSEMBLIES_FAILED, dwFailed);

    return bSucceeded;
}


//
// Command line parsing code...
//

#define CURRENT_ARG_STRING &(argv[currentArg][1])

bool ValidSwitchSyntax(int currentArg, __in WCHAR **argv)
{
    if ((argv[currentArg][0] == L'-') || (argv[currentArg][0] == L'/')) 
    {
        return true;
    }
    else
    {
        return false;
    }
}

void SetDisplayOptions(int argc, __in_ecount(argc) WCHAR *argv[])
{
    for(int currentArg = 1; currentArg < argc; currentArg++)
    {
        // only check switches
        if ((argv[currentArg][0] == L'-') || (argv[currentArg][0] == L'/'))  
        {
            if (_wcsicmp(CURRENT_ARG_STRING, L"nologo") == 0) g_bdisplayLogo = false;
            if (_wcsicmp(CURRENT_ARG_STRING, L"silent") == 0) g_bSilent = true;
        }
    }
}

bool CheckArgs(UINT currentArg, UINT argsRequired, UINT argCount, __in_ecount(argCount) WCHAR *argv[], bool bShowError)
{
    assert(argCount >= 2); // if we got this far, we've got at least 2 args.

    if (ovadd_ge(currentArg, argsRequired, argCount))
    {
        if (bShowError)
        {
            if (argsRequired == 1)
            {
                BSILENT_PRINTF1ARG(IDS_COMMAND_LINE_ERROR_1, argv[currentArg]);
            }
            else
            {
                BSILENT_PRINTF2ARG(IDS_COMMAND_LINE_ERROR_2, argv[currentArg], argsRequired);
            }
        }
        return false;
    }


    return true;
}

bool CheckArgs(int currentArg, int argsRequired, int argCount, __in WCHAR **argv)
{
    return CheckArgs(currentArg, argsRequired, argCount, argv, true);
}

bool ParseArguments(int argc, __in_ecount(argc) WCHAR *argv[])
{
    int currentArg = 1;         // skip the name of the program

    while ((currentArg < argc))
    {
        if (!ValidSwitchSyntax(currentArg, argv))
        {
            BSILENT_PRINTF1ARG(IDS_ERROR_UKNOWN_COMMAND, argv[currentArg]);
            return false;
        }

        // fix the turkish 'i' problem
        if (_wcsicmp(CURRENT_ARG_STRING, L"i") == 0)
        {
            if (!CheckArgs(currentArg, 1, argc, argv))
                return false;
          
            if (g_bInstall)
            {
                BSILENT_PRINTF1ARG(IDS_ERROR_DUPLICATE_COMMAND, argv[currentArg]);
                return false;
            }
            g_bInstall = 1;
            g_pAsmFileName = argv[currentArg+1];
            currentArg += 2;
        }
        else if (_wcsicmp(CURRENT_ARG_STRING, L"il") == 0)
        {
            if (!CheckArgs(currentArg, 1, argc, argv))
                return false;
            
            if (g_bInstallList)
            {
                BSILENT_PRINTF1ARG(IDS_ERROR_DUPLICATE_COMMAND, "il");
                return false;
            }          
            g_bInstallList = 1;
            g_pAsmListFileName = argv[currentArg+1];
            
            currentArg += 2;
        }
        else if (_wcsicmp(CURRENT_ARG_STRING, L"u") == 0)
        {
            if (!CheckArgs(currentArg, 1, argc, argv))
                return false;
            
            if (g_bUninstall)
            {
                BSILENT_PRINTF1ARG(IDS_ERROR_DUPLICATE_COMMAND, "u");
                return false;
            }   

            g_bUninstall = 1;
            g_pAsmDisplayName = argv[currentArg + 1];
            currentArg += 2;
        }
        else if (_wcsicmp(CURRENT_ARG_STRING, L"ul") == 0)
        {
            if (!CheckArgs(currentArg, 1, argc, argv))
                return false;
            
            if (g_bUninstallList)
            {
                BSILENT_PRINTF1ARG(IDS_ERROR_DUPLICATE_COMMAND, "ul");
                return false;
            }               
            g_bUninstallList = 1;
            g_pAsmListFileName = argv[currentArg + 1];
            currentArg += 2;
        }
        else if (_wcsicmp(CURRENT_ARG_STRING, L"l") == 0) 
        {
            bool bNameSpecified = false;

            if (CheckArgs(currentArg, 1, argc, argv, false))
                bNameSpecified = true;

            if (g_bListAsm)
            {
                BSILENT_PRINTF1ARG(IDS_ERROR_DUPLICATE_COMMAND, "l");
                return false;
            }
        
            g_bListAsm = 1;
            if (bNameSpecified)
            {
                g_pAsmDisplayName = argv[currentArg + 1];
                currentArg += 2;
            }
            else
                currentArg++;
        }
        else if ((_wcsicmp(CURRENT_ARG_STRING, L"nologo") == 0) || (_wcsicmp(CURRENT_ARG_STRING, L"silent") == 0))
        {
            // just skip it.
            currentArg++;
        }
        else if ((_wcsicmp(CURRENT_ARG_STRING, L"?") == 0) || (_wcsicmp(CURRENT_ARG_STRING, L"h") ==0))
        {
            g_bPrintHelp = true;
            return true;
        }
        else
        {
            BSILENT_PRINTF1ARG(IDS_ERROR_UKNOWN_COMMAND, argv[currentArg]);
            return false;
        }
    }

    return true;
}

bool Run()
{
    if (g_bPrintHelp)
    {
        LongUsage();
        return true;
    }

    INT totalCommands = g_bInstall + g_bInstallList + g_bUninstall 
                + g_bUninstallList + g_bListAsm + g_bListAsmRefs 
                + g_bListDLCache + g_bClearDLCache + g_bUninstallAllRefs;

    if (totalCommands == 0)
    {
        // nothing to do
        return true;
    }

    if (totalCommands > 1)
    {
        BSILENT_PRINTF0ARG(IDS_ERROR_MULTIPLE_COMMANDS);
        return false;
    }

    DWORD dwFlag = IASSEMBLYCACHE_INSTALL_FLAG_REFRESH;

    if (g_bForceInstall)
        dwFlag = IASSEMBLYCACHE_INSTALL_FLAG_FORCE_REFRESH;   

    if (g_bInstall)
    {
        return InstallAssembly(g_pAsmFileName, g_pInstallReference, dwFlag);
    }

    if (g_bInstallList)
    {
        return InstallListOfAssemblies(g_pAsmListFileName, g_pInstallReference, dwFlag);
    }

    if (g_bUninstall)
    {
        return UninstallAssembly(g_pAsmDisplayName, g_pInstallReference, false);
    }

    if (g_bUninstallAllRefs)
    {
        return UninstallAssembly(g_pAsmDisplayName, NULL, true);
    }

    if (g_bUninstallList)
    {
        return UninstallListOfAssemblies(g_pAsmListFileName, g_pInstallReference);
    }

    if (g_bListAsm || g_bListAsmRefs)
    {
        bool bPrintRefs = g_bListAsmRefs?true:false;
        BSILENT_PRINTF0ARG(IDS_CACHE_CONTAINS);
        int gacCount = 0;
        gacCount = EnumerateAssemblies(ASM_CACHE_GAC, g_pAsmDisplayName, bPrintRefs);

        BSILENT_PRINTF1ARG(IDS_NUMBER_OF_ITEMS, gacCount);
        return true;
    }

    
    return true;
}


#define MESSAGE_DLL L"gacutil.satellite"

static HSATELLITE LoadSatelliteResource()
{
    WCHAR path[MAX_PATH];
    WCHAR * pEnd;

    if (!GetModuleFileNameW(NULL, path, MAX_PATH))
        return 0;

#ifndef PLATFORM_UNIX
    pEnd = wcsrchr(path, L'\\');
#else
    pEnd = wcsrchr(path, L'/');
#endif
    if (!pEnd)
        return 0;
    ++pEnd;  // point just beyond.

    // Append message DLL name.
    if ((int) sizeof(MESSAGE_DLL) + pEnd - path > (int) sizeof(path) - 1)
        return 0;
    wcscpy_s(pEnd, MAX_PATH + path - pEnd, MESSAGE_DLL);

    return PAL_LoadSatelliteResourceW(path);
}


extern "C" int __cdecl wmain(int argc, __in_ecount(argc) WCHAR *argv[])
{
    bool bResult = true;

    if (!PAL_RegisterLibraryW(L"rotor_palrt"))
    {
        PrintString(L"Failed to register libraries\r\n");
        goto Exit;
    }

    if (!(g_hResourceInst = LoadSatelliteResource()))
    {
        PrintString(L"Failed to load resource strings\r\n");
        goto Exit;
    }

    // Initialize Wsz wrappers.
    OnUnicodeSystem();

    // Initialize Fusion
    if(!FusionInit())
    {
        BSILENT_PRINTF0ARG(IDS_FAILURE_TO_INITIALIZE);     
        goto Exit;
    }

    if ((argc < 2) || (!ValidSwitchSyntax(1, argv))) 
    {
        Title();
        ShortUsage();
        goto Exit;
    }

    SetDisplayOptions(argc, argv); // sets g_bdisplayLogo and g_bSilent
    if (g_bdisplayLogo)
    {
        Title();
    }

    bResult = ParseArguments(argc, argv);

    if (bResult)
    {
        bResult = Run();
    }


Exit:

    if(g_FusionDll) {
        FreeLibrary(g_FusionDll);
        g_FusionDll = NULL;
    }

    return bResult ? 0 : 1;
}

