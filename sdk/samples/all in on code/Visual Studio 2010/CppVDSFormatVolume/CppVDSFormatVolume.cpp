/****************************** Module Header ******************************\
Module Name:  CppVDSFormatVolume.cpp
Project:      CppVDSFormatVolume (VDS Format Volume)
Copyright (c) Microsoft Corporation.

Demonstrates how to use VDS to format volumes.

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
#define DEFIND_GUID
#include <initguid.h>
#include <vds.h>

// Turn off do..while(TRUE) warnings.
#pragma warning(disable: 4127)

#define SAFE_RELEASE(x)     if (x) { x->Release(); x = NULL; }
#define SAFE_FREE(x)        if (x) { CoTaskMemFree(x); }
#define GUID_LEN 50

wchar_t *HealthString[] = 
{
    L"UNKNOWN",
    L"HEALTHY",
    L"REBUILDING",
    L"STALE",
    L"FAILING",
    L"FAILING_REDUNDANCY",
    L"FAILED_REDUNDANCY",
    L"FAILED_REDUNDANCY_FAILING",
    L"FAILED"
};

wchar_t *TypeString[] =
{
    L"UNKNOWN",
    L"SIMPLE",
    L"SPAN",
    L"STRIPE",
    L"MIRROR",
    L"PARITY"
};

wchar_t *StatusString[] =
{
    L"UNKNOWN",
    L"ONLINE",
    L"NOT_READY",
    L"NO_MEDIA", 
    L"FAILED"
};

wchar_t *TransitionStateString[] =
{
    L"UNKNOWN",
    L"STABLE", 
    L"EXTENDING",
    L"SHRINKING",
    L"RECONFIGING"
};

wchar_t *FileSystemString[] =
{
    L"UNKNOWN",
    L"RAW",
    L"FAT",
    L"FAT32",
    L"NTFS",
    L"CDFS",
    L"UDF"
};

wchar_t *ExtentTypeString[] = 
{
    L"UNKNOWN",
    L"FREE",
    L"DATA",
    L"OEM",
    L"ESP",
    L"MSR",
    L"LMD",
    L"UNUSABLE"
};

void usage()
{
    printf("CppVDSFormatVolume.exe [-options]...\n");
    printf("  -v <volume number (decimal)> REQUIRED INPUT\n");
    printf("  -t <target file system type: NTFS, FAT32, etc.> UPPER CASE ONLY, REQUIRED INPUT\n");
    printf("  e.g.:  format -v 3 -t NTFS\n");
}

int wmain(int argc, wchar_t *argv[])
{
    HRESULT hr, AsyncHr;
    IEnumVdsObject *pEnum = NULL;
    IEnumVdsObject *pEnumPack = NULL;
    IEnumVdsObject *pEnumVolume = NULL;
    IVdsAsync      *pAsync = NULL;
    VDS_ASYNC_OUTPUT AsyncOut;
    VDS_FILE_SYSTEM_TYPE FileSystemType;
    wchar_t buffer[500];
    int VolumeNumber = -1;
    int cargc = 1;
    BOOL opt_v = FALSE, opt_t = FALSE;
    wchar_t cmd;
    wchar_t szFsType[8];
    wchar_t szVolumeWin32Name[40];

    if (argc < 2) 
    {
        usage();
        return 0;
    }

    cargc = 1;
    while (--argc > 0 && argv[cargc][0] == L'-') 
    {
        cmd = argv[cargc++][1];
        //wprintf(L"cmd = %c\n", cmd);
        switch (cmd) 
        {
            case L'v':
            case L'V':
                opt_v = TRUE;
                //wprintf(L"d = %s\n", argv[cargc]);
				swscanf_s(argv[cargc], L"%d", &VolumeNumber);
                cargc++, --argc; // Inc/decrement if there is a parameter
                swprintf_s(szVolumeWin32Name, _countof(szVolumeWin32Name), 
                    L"\\\\?\\GLOBALROOT\\Device\\HarddiskVolume%d", VolumeNumber);
                break;
            case L't':
            case L'T':
                //wprintf(L"t = %s\n", argv[cargc]);
                swscanf_s(argv[cargc], L"%s", szFsType, _countof(szFsType));
                // toupper
                cargc++, --argc; // inc/decrement if there is a parameter
                if (wcsncmp(szFsType, L"FAT32", 5) == 0) 
                {
                    opt_t = TRUE;
                    FileSystemType = VDS_FST_FAT32;
                    //printf("got FAT32 ..\n");
                }
                else if (wcsncmp(szFsType, L"NTFS", 4) == 0)
                {
                    opt_t = TRUE;
                    FileSystemType = VDS_FST_NTFS;
                    //printf("got NTFS ..\n");
                }
                break;
            default:
                usage();
                break;
        }
    }
    if (!opt_v || !opt_t) 
    {
        printf("insufficient input...\n");
        usage();
        return 0;
    }

    wprintf(L"ok to format volume %s, fs = %s?  [y/n]\n", 
           szVolumeWin32Name,
           szFsType);

    char c = (char) getchar();
    switch (c) 
    {
        case 'y':
        case 'Y': break;
        default:
            printf("cancelling...\n");
            return 0;
            break;
    }
    printf("and off we go...\n");

    if (FAILED(CoInitialize(NULL)))
    {
        printf("CoInitialize() failed err=%d.\n", GetLastError());
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
    hr = CoCreateInstance(CLSID_VdsLoader,
                          NULL,
                          CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER,
                          IID_IVdsServiceLoader,
                          (void **)&pLoader
                         );
    if (FAILED(hr))
    {
        printf("\nCoCreateInstance failed: hr=%X\n", hr);
        return 0;
    }

    // Load the service on the machine.
    IVdsService *pService;
    hr = pLoader->LoadService(L"", &pService);
    SAFE_RELEASE(pLoader);
    if (FAILED(hr))
    {
        printf("\nLoadService failed: hr=%X\n", hr);
        return 0;
    }

    // call QueryProviders
    hr = pService->QueryProviders(VDS_QUERY_SOFTWARE_PROVIDERS, &pEnum);
    if (FAILED(hr))
    {
        printf("\nQueryProviders(VDS_QUERY_SOFTWARE_PROVIDERS) failed: hr=%X\n", hr);
        goto _bailout;
    }

    IUnknown *pUnk;
    IVdsProvider *pProvider;
    ULONG ulFetched = 0;
    int iProviderNumber = 1;
    BOOL fVolumeFound = FALSE;

    do 
    {
        hr = pEnum->Next(1, &pUnk, &ulFetched);
        if (hr == S_FALSE)
        {
            break;
        }

        if (FAILED(hr))
        {
            printf("\npEnum->Next() failed: hr=%X\n", hr);
            goto _bailout;
        }
        
        // Get IVdsProvider
        hr = pUnk->QueryInterface(IID_IVdsProvider, (void **)&pProvider);
        SAFE_RELEASE(pUnk);

        if (FAILED(hr))
        {
            printf("  pUnk->QueryInterface(IVdsProvider) failed: hr=%X\n", hr);
            goto _bailout;
        }
        
        VDS_PROVIDER_PROP provProp;
        hr = pProvider->GetProperties(&provProp);
        if (SUCCEEDED(hr))
        {
            wprintf(L"Provider #: %ld name: %s\n", iProviderNumber++, provProp.pwszName);
        }

        // Get IVdsSwProvider
        IVdsSwProvider *pSwProvider;
        hr = pProvider->QueryInterface(IID_IVdsSwProvider, (void **)&pSwProvider);
        SAFE_RELEASE(pProvider);

        if (FAILED(hr))
        {
            printf("    pProvider->QueryInterface(IVdsSwProvider) failed: hr=%X\n", hr); 
            goto _bailout;
        }

        // Enumerate packs associated with the software provider
        hr = pSwProvider->QueryPacks(&pEnumPack);
        SAFE_RELEASE(pSwProvider);

        if (FAILED(hr))
        {
            printf("    pSwProvider->QueryPacks() failed: hr=%X\n", hr); 
            goto _bailout;
        }

        IUnknown *pPackUnk;
        ULONG ulFetchedPack;
        int iPackNumber = 1;

        // Enumerate the packs
        do 
        {
            hr = pEnumPack->Next(1, &pPackUnk, &ulFetchedPack);
            if (hr == S_FALSE)
            {
                break;
            }

            if (FAILED(hr))
            {
                printf("        pEnumPack->Next() failed: hr=%X\n", hr); 
                goto _bailout;
            }

            printf("Pack #: %ld\n", iPackNumber++);

            // Get IVdsPack
            IVdsPack *pPack;
            hr = pPackUnk->QueryInterface(IID_IVdsPack, (void **)&pPack);
            SAFE_RELEASE(pPackUnk);

            if (FAILED(hr))
            {
                printf("        pPackUnk->QueryInterface() failed: hr=%X\n", hr);
                goto _bailout;
            }

            // Get the volume enumerator to list volumes associated with the pack
            hr = pPack->QueryVolumes(&pEnumVolume);
            SAFE_RELEASE(pPack);
            if (FAILED(hr))
            {
                printf("        pPack->QueryVolumes() failed: hr=%X\n", hr);
                goto _bailout;
            }

            IUnknown *pVolumeUnk;
            ULONG ulFetchedVolume;
            int iVolumeNumber = 1;

            // Enumerate the volumes in the pack
            do
            {
                hr = pEnumVolume->Next(1, &pVolumeUnk, &ulFetchedVolume);
                if (hr == S_FALSE) 
                {
                    break;   // end of enumeration
                }

                if (FAILED(hr))
                {
                    printf("\n            pEnumVolume->Next() failed: hr=%X\n", hr);
                    goto _bailout;
                }

                printf("Volume #: %ld\n", iVolumeNumber++);

                // Get IVdsVolume
                IVdsVolume *pVolume;
                hr = pVolumeUnk->QueryInterface(IID_IVdsVolume, (void **)&pVolume);
                SAFE_RELEASE(pVolumeUnk);

                if (FAILED(hr))
                {
                    printf("            pVolumeUnk->QueryInterface() failed: hr=%X\n", hr); 
                    goto _bailout;
                }

                // Get volume properties
                VDS_VOLUME_PROP VolumeProp;
                hr = pVolume->GetProperties(&VolumeProp);
                if (SUCCEEDED(hr))
                {
                    StringFromGUID2(VolumeProp.id, buffer, GUID_LEN);
                    wprintf(L"                Volume Id: %s\n", buffer);
                    wprintf(L"                Type: %s\n", TypeString[VolumeProp.type - VDS_VT_SIMPLE + 1]);
                    wprintf(L"                Status: %s\n", StatusString[VolumeProp.status]);
                    wprintf(L"                Health: %s\n", HealthString[VolumeProp.health]);
                    wprintf(L"                TransitionState: %s\n", TransitionStateString[VolumeProp.TransitionState]);
                    wprintf(L"                Size: %I64u\n", VolumeProp.ullSize);
                    wprintf(L"                Flags: %lx\n", VolumeProp.ulFlags);
                    wprintf(L"                Recommended file system: %s\n", FileSystemString[VolumeProp.RecommendedFileSystemType]);
                    wprintf(L"                Win32 name: %s\n", VolumeProp.pwszName);

                    int ret = wcscmp(szVolumeWin32Name,VolumeProp.pwszName);
                    if (ret != 0)
                    {
                        SAFE_RELEASE(pVolume);
                        continue;
                    }
                    wprintf(L"found the volume - name %s\n", VolumeProp.pwszName);
                    fVolumeFound = TRUE;
                    CoTaskMemFree(VolumeProp.pwszName);
                }

                // Get IVdsVolumeMF
                IVdsVolumeMF3 *pVolumeMF3;
                hr = pVolume->QueryInterface(IID_IVdsVolumeMF3, (void **)&pVolumeMF3);
                if (FAILED(hr))
                {
                    printf("              pVolume->QueryInterface(IVdsVolumeMF3) failed: hr=%X\n", hr); 
                }
                else
                {
                    //printf("success IVdsVolumeMF3 ...\n");
                    USHORT Revision = 1;
                    ULONG UnitAllocationSize = 0;
                    wchar_t Label[] = L"VDSTest";
                    ULONG Options = VDS_FSOF_QUICK;

#if 0                    
                    typedef enum  {
                      VDS_FSOF_NONE                 = 0x00000000,
                      VDS_FSOF_FORCE                = 0x00000001,
                      VDS_FSOF_QUICK                = 0x00000002,
                      VDS_FSOF_COMPRESSION          = 0x00000004,
                      VDS_FSOF_DUPLICATE_METADATA   = 0x00000008 
                    } VDS_FORMAT_OPTION_FLAGS;
                    
#endif
                    ::memset(&AsyncOut, 0, sizeof(VDS_ASYNC_OUTPUT));
                    hr = pVolumeMF3->FormatEx2(szFsType,
                                               Revision,
                                               UnitAllocationSize,
                                               Label,
                                               (DWORD) Options,
                                               &pAsync);

                    hr = pAsync->Wait(&AsyncHr, &AsyncOut);
                    
                    if (FAILED(hr))
                    {
                        printf("pAsync->Wait hr failed: %X\n", hr);
                    }
                    else if (FAILED(AsyncHr))
                    {
                        switch(AsyncHr)
                        {
                            case VDS_E_VOLUME_NOT_ONLINE:
                                printf("IDS_FORMAT_OFFLINE\n");
                                break;
                            default:
                                printf("pAsync->Wait AsyncHr failed: %X\n", AsyncHr);
                                break;
                        }
                    }
                    else
                    {
                        // The volume is formatted successfully.
                        printf("format complete.\n");
                    }
                }
                SAFE_RELEASE(pVolume);
                SAFE_RELEASE(pVolumeMF3);
            } while (TRUE);
            SAFE_RELEASE(pEnumVolume);
        } while (TRUE);
        SAFE_RELEASE(pEnumPack);
    } while (TRUE);
    SAFE_RELEASE(pEnum);
    SAFE_RELEASE(pService);

    if (!fVolumeFound)
    {
        printf("cannot find the volume.\n");
    }

    CoUninitialize();
    return 0;

_bailout:
    SAFE_RELEASE(pEnum);
    SAFE_RELEASE(pEnumPack);
    SAFE_RELEASE(pEnumVolume);
    SAFE_RELEASE(pService);
    CoUninitialize();
    return 0;
}