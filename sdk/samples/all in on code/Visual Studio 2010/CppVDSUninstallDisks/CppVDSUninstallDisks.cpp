/****************************** Module Header ******************************\
Module Name:  CppVDSUninstallDisks.cpp
Project:      CppVDSUninstallDisks (VDS Uninstall Disks)
Copyright (c) Microsoft Corporation.

Demonstrates disk and volumes uninstall using VDS.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <initguid.h>
#include <vds.h>

// Turn off do..while(TRUE) warnings.
#pragma warning(disable: 4127)

#define SAFE_RELEASE(x)     if (x) { x->Release(); x = NULL; }
#define SAFE_FREE(x)        if (x) { CoTaskMemFree(x); }


void usage()
{
    printf("Warning: uninstalling a disk may lead to potential data loss. \n");
    printf("Usage: CppVDSUninstallDisks.exe <disknumber <disknumber> <etc>>\n");
    printf("  e.g.:  CppVDSUninstallDisks 1 4 2\n");
}

HRESULT GetDiskIds(IEnumVdsObject *pEnumDisk, BOOL bVerbose, VDS_OBJECT_ID **ppDiskArray, DWORD dwDiskNumber)
{
    UNREFERENCED_PARAMETER(bVerbose);

    IUnknown *pDiskUnk = NULL;
    ULONG ulFetchedDisk = 0L;
    int iDiskNumber = 1;
    HRESULT hr = S_OK;
    DWORD dwNumber = 0L;
    IVdsDisk *pDisk = NULL;
    VDS_DISK_PROP diskprop;

    do
    {
        hr = pEnumDisk->Next(1, &pDiskUnk, &ulFetchedDisk);
        if (hr == S_FALSE) 
        {
            break;   // end of enumeration
        }
        if (FAILED(hr))
        {
            printf("\npEnumDisk->Next(): hr=%X\n", hr);
            return hr;
        }
        printf(" Disk #: %ld\n", iDiskNumber++);

        // Get IVdsDisk
        hr = pDiskUnk->QueryInterface(IID_IVdsDisk, (void **)&pDisk);
        SAFE_RELEASE(pDiskUnk);
        if (FAILED(hr)) 
        {
            printf("pDiskUnk->QueryInterface(): hr=%X\n", hr); 
            return hr;
        }

        // Get properties
        hr = pDisk->GetProperties(&diskprop);
        if (FAILED(hr))
        {
            printf("\t pDisk->GetProperties(): hr=%X\n", hr); 
            SAFE_RELEASE(pDisk);
            return hr;
        }

        if (NULL == diskprop.pwszName || ::wcslen(diskprop.pwszName) < 18) 
        {
            continue;
            //printf("\t diskprop.pwszName: %S, %s\n", 
            //        diskprop.pwszName ? diskprop.pwszName : L"UNKNOWN");             
        }

        printf("\t diskprop.pwszName: %ws\n", 
            diskprop.pwszName ? diskprop.pwszName : L"UNKNOWN"); 

        dwNumber = _wtol(&(diskprop.pwszName[17]));

        SAFE_FREE(diskprop.pwszName);
        SAFE_FREE(diskprop.pwszDiskAddress);
        SAFE_FREE(diskprop.pwszFriendlyName);
        SAFE_FREE(diskprop.pwszAdaptorName);
        SAFE_FREE(diskprop.pwszDevicePath);

        //
        // If this disk matches the input disk number, copy
        // the disk id to the buffer.
        if (dwNumber == dwDiskNumber) 
        {
            ::memcpy((*ppDiskArray), &(diskprop.id), sizeof(VDS_OBJECT_ID));
            (*ppDiskArray)++;
        }

        SAFE_RELEASE(pDisk);
    } while (TRUE);

    return S_OK;
}


int wmain(int argc, wchar_t *argv[])
{
    HRESULT hr;
    IVdsService		*pService	= NULL;
    IEnumVdsObject	*pEnum	    = NULL;
    IEnumVdsObject	*pEnumPack	= NULL;
    IEnumVdsObject	*pEnumDisk	= NULL;
    IVdsServiceUninstallDisk    *pUninstallIntf = NULL;
    VDS_OBJECT_ID   *pDiskArray	= NULL;
    VDS_OBJECT_ID   *pDiskArrayHead = NULL;

    BOOL			bVerbose = FALSE;
    ULONG           ulDisk = 999999L;
    HRESULT         *pResults = NULL;
    BOOLEAN         bReboot = FALSE;
    LONG			i = 0L;
    LONG            numDisks;

    if (argc < 2)
    {
        usage();
        exit(1);
    }

    numDisks = argc - 1;
    pDiskArray = (VDS_OBJECT_ID *) ::CoTaskMemAlloc(sizeof(VDS_OBJECT_ID) * numDisks);
    if (NULL == pDiskArray) 
    {
        printf("\tmain, 1: %lX\n", E_OUTOFMEMORY);
        goto _bailout;
    }
    ::memset(pDiskArray, 0, (sizeof(VDS_OBJECT_ID) * numDisks));
    pDiskArrayHead = pDiskArray;

    if (FAILED(CoInitialize(NULL)))
    {
        printf("CoInitialize() failed.\n");
        return 0;
    }

    hr = CoInitializeSecurity(
        NULL, 
        -1, 
        NULL, 
        NULL,
        RPC_C_AUTHN_LEVEL_CONNECT,
        RPC_C_IMP_LEVEL_IMPERSONATE, 
        NULL, 
        0,
        NULL
        );
    if (FAILED(hr))
    {
        printf("CoInitializeSecurity failed: hr=%X.\n", hr);
        return 0;
    }

    // Create a loader instance
    IVdsServiceLoader *pLoader;
    COSERVERINFO      ServerInfo;
    MULTI_QI          MultiQi;

    ServerInfo.dwReserved1  = 0;
    ServerInfo.pwszName     = NULL;
    ServerInfo.pAuthInfo    = NULL;
    ServerInfo.dwReserved2  = 0;

    MultiQi.pIID            = &IID_IVdsServiceLoader;
    MultiQi.pItf            = NULL;
    MultiQi.hr              = 0;

    hr = CoCreateInstanceEx(CLSID_VdsLoader,
        NULL,
        CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER,
        &ServerInfo,
        1,
        &MultiQi
        );
    if (FAILED(hr)) 
    {
        printf("\nCoCreateInstanceEx failed: hr=%X  MultiQi.hr=%X\n", hr, MultiQi.hr);
        return 0;
    }
    if (FAILED(MultiQi.hr)) 
    {
        printf("\nCoCreateInstanceEx failed: hr=%X  MultiQi.hr=%X\n", hr, MultiQi.hr);
        return 0;
    }

    // Load the service on the machine.
    pLoader = (IVdsServiceLoader *)MultiQi.pItf;
    hr = pLoader->LoadService(NULL, &pService);
    pLoader->Release();
    pLoader = NULL;
    if (FAILED(hr))
    {
        printf("\nLoadService failed: hr=%X\n", hr);
        return 0;
    }

    // Call QueryUnallocatedDisks.
    for (i = 1; i <= numDisks; i++)
    {
        hr = pService->QueryUnallocatedDisks(&pEnumDisk);
        if (FAILED(hr))
        {
            printf("QueryUnallocatedDisks: hr=%X\n", hr);
            return 0;
        }
        ulDisk = _wtol(argv[i]);
        GetDiskIds(pEnumDisk, bVerbose, &pDiskArray, ulDisk);
        SAFE_RELEASE(pEnumDisk);
    }

    // Call QueryProviders
    hr = pService->QueryProviders(VDS_QUERY_SOFTWARE_PROVIDERS, &pEnum);
    if (FAILED(hr)) 
    {
        printf("\nQueryProviders(VDS_QUERY_SOFTWARE_PROVIDERS): hr=%X\n",hr);
        goto _bailout;
    }

    IUnknown *pUnk;
    IVdsProvider *pProvider;
    ULONG ulFetched = 0;
    int iProviderNumber = 1;

    do 
    {
        hr = pEnum->Next(1, &pUnk, &ulFetched);
        if (hr == S_FALSE)
        {
            break;
        }
        if (FAILED(hr)) 
        {
            goto _bailout;
        }

        printf("  Provider #: %ld\n", iProviderNumber++);

        // Get IVdsProvider
        hr = pUnk->QueryInterface(IID_IVdsProvider, (void **)&pProvider);
        SAFE_RELEASE(pUnk);

        if (FAILED(hr)) 
        {
            printf("  pUnk->QueryInterface(IVdsProvider): hr=%X\n", hr);
            goto _bailout;
        }

        // Get IVdsSwProvider
        IVdsSwProvider *pSwProvider;
        hr = pProvider->QueryInterface(IID_IVdsSwProvider, (void **)&pSwProvider);
        SAFE_RELEASE(pProvider);

        if (FAILED(hr))
        {
            printf("    pProvider->QueryInterface(IVdsSwProvider): hr=%X\n", hr); 
            goto _bailout;
        }

        // Get packs
        hr = pSwProvider->QueryPacks(&pEnumPack);
        SAFE_RELEASE(pSwProvider);

        if (FAILED(hr)) 
        {
            printf("    pSwProvider->QueryPacks(): hr=%X\n", hr); 
            goto _bailout;
        }

        IUnknown *pPackUnk;
        ULONG ulFetchedPack;
        int iPackNumber = 1;
        do 
        {
            hr = pEnumPack->Next(1, &pPackUnk, &ulFetchedPack);
            if (hr == S_FALSE) 
            {
                break;
            }
            if (FAILED(hr))
            {
                goto _bailout;
            }
            printf("Pack #: %ld\n", iPackNumber++);

            // Get IVdsPack
            IVdsPack *pPack;
            hr = pPackUnk->QueryInterface(IID_IVdsPack, (void **)&pPack);
            SAFE_RELEASE(pPackUnk);

            if (FAILED(hr))
            {
                printf("        pPackUnk->QueryInterface(): hr=%X\n", hr); 
                goto _bailout;
            }

            // Get provider
            IVdsProvider *pProvider;
            hr = pPack->GetProvider(&pProvider);
            SAFE_RELEASE(pProvider);

            // QueryDisks()
            if (FAILED(hr))
            {
                printf("        pPack->GetProvider(): hr=%X\n", hr);
                goto _bailout;
            }

            for (i = 1; i <= numDisks; i++) 
            {
                ulDisk = _wtol(argv[i]);
                hr = pPack->QueryDisks(&pEnumDisk);
                hr = GetDiskIds(pEnumDisk, bVerbose, &pDiskArray, ulDisk);
                printf("\t\tpPack->GetDiskIds(): hr=%X\n", hr);
                SAFE_RELEASE(pEnumDisk);
            }
            SAFE_RELEASE(pPack);

            if (FAILED(hr)) 
            {
                goto _bailout;
            }
        } while (TRUE);
        SAFE_RELEASE(pEnumPack);
    } while (TRUE);

    //
    // Uninstall the disks
    hr = pService->QueryInterface(
        IID_IVdsServiceUninstallDisk, 
        (void **)&pUninstallIntf 
        );
    if (FAILED(hr)) 
    {
        printf("\tIID_IVdsServiceUninstallDisk: %lX\n", hr);
        goto _bailout;
    }

    SIZE_T size = sizeof(HRESULT) * numDisks;
    pResults = (HRESULT *) ::CoTaskMemAlloc(size);
    if (NULL == pResults) 
    {
        printf("\tmain, 2: %lX\n", E_OUTOFMEMORY);
        goto _bailout;
    }

    pDiskArray = pDiskArrayHead;
    hr = pUninstallIntf->UninstallDisks(pDiskArray, numDisks, FALSE, &bReboot, pResults);
    if (FAILED(hr)) 
    {
        printf("\tUninstallDisks failed: %lX\n", hr);
        goto _bailout;
    }
    else 
    {
        printf("\tUninstallDisks succeeded\n");
        for (i = 1; i <= numDisks; i++) 
        {
            printf("uninstall status for disk %d => %lX\n", _wtol(argv[i]), pResults[i-1]);
        }
    }

_bailout:

    SAFE_RELEASE(pUninstallIntf);
    SAFE_RELEASE(pEnum);
    SAFE_RELEASE(pEnumPack);
    SAFE_RELEASE(pEnumDisk);
    SAFE_RELEASE(pService);

    if (pDiskArray) 
    {
        CoTaskMemFree(pDiskArray);
    }
    if (pResults) 
    {
        CoTaskMemFree(pResults);
    }

    CoUninitialize();
    return 0;
}
