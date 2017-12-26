/****************************** Module Header ******************************\
Module Name:  CppVDSQueryVolumes.cpp
Project:      CppVDSQueryVolumes (VDS Query Volumes)
Copyright (c) Microsoft Corporation.

Demonstrates volumes enumeration on a system using VDS, also lists volume 
properties.

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
#pragma warning(disable:4127)

#define SAFE_RELEASE(x)     if (x) { x->Release(); x = NULL; }
#define SAFE_FREE(x)        if (x) { CoTaskMemFree(x); }
#define GUID_LEN            50

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

void ListVolumeDisks(IVdsVolume *pVolume)
{
    HRESULT hr;
    IEnumVdsObject *pEnumPlex = NULL;
    wchar_t buffer[500];

    printf("\n\t... dumping disk extents ...\n");

    hr = pVolume->QueryPlexes(&pEnumPlex);

    IUnknown *pPlexUnk;
    ULONG ulFetchedPlex;
    VDS_VOLUME_PLEX_PROP PlexProp;

    do
    {
        hr = pEnumPlex->Next(1, &pPlexUnk, &ulFetchedPlex);
        if (hr == S_FALSE)
        {
            break;   // End of enumeration
        }
        if (FAILED(hr))
        {
            goto _bailout;
        }

        // Get IVdsVolumePlex
        IVdsVolumePlex *pPlex;
        hr = pPlexUnk->QueryInterface(IID_IVdsVolumePlex, (void **)&pPlex);
        if (FAILED(hr))
        {
            SAFE_RELEASE(pPlex);
            goto _bailout;
        }

        // Get plex properties
        hr = pPlex->GetProperties(&PlexProp);
        if (FAILED(hr))
        {
            SAFE_RELEASE(pPlex);
            goto _bailout;
        }

        //
        // Get extents on the plex
        VDS_DISK_EXTENT *pExtents;
        LONG lExtentsCount;

        hr = pPlex->QueryExtents(&pExtents, &lExtentsCount);

        SAFE_RELEASE(pPlex);
        if (FAILED(hr))
        {
            goto _bailout;
        }

        printf("\tNumber of extents: %ld\n", lExtentsCount);
        for (int i = 0; i < lExtentsCount; i++)
        {
            printf("\n\t\tExtent #%d:\n", i + 1);
            StringFromGUID2( pExtents[i].diskId, buffer, 500);
            wprintf(L"\t\tDisk Id: %ws\n", buffer);
            printf("\t\ttype: %ws\n", 
                pExtents[i].type==VDS_DET_UNUSABLE?ExtentTypeString[7] :
                ExtentTypeString[pExtents[i].type]
            );
            printf("\t\tOffset: %I64d\n", pExtents[i].ullOffset);
            printf("\t\tSize: %I64d\n", pExtents[i].ullSize);
            StringFromGUID2( pExtents[i].volumeId, buffer, 500);
            wprintf(L"\t\tVolume Id: %ws\n", buffer);
            StringFromGUID2( pExtents[i].plexId, buffer, 500);
            wprintf(L"\t\tPlex Id: %ws\n", buffer);
            printf("\t\tMember Id: %ld\n", pExtents[i].memberIdx);
        }
        SAFE_FREE( pExtents );

    } while (TRUE);
    SAFE_RELEASE( pEnumPlex );
_bailout:
    return;
}

int wmain(int argc, wchar_t *argv[])
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    HRESULT hr;
    IEnumVdsObject *pEnum = NULL;
    IEnumVdsObject *pEnumPack = NULL;
    IEnumVdsObject *pEnumVolume = NULL;
    wchar_t buffer[500];

    if (FAILED(CoInitialize(NULL)))
    {
        printf("CoInitialize() failed err=%d.\n", GetLastError());
        return 0;
    }


    // Create a loader instance
    IVdsServiceLoader *pLoader;
    hr = CoCreateInstance(CLSID_VdsLoader,
        NULL,
        CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER,
        IID_IVdsServiceLoader,
        (void **)&pLoader);
    if (FAILED(hr))
    {
        printf("CoCreateInstance failed: hr=%X\n", hr);
        return 0;
    }

    // Load the service on the machine.
    IVdsService *pService;
    hr = pLoader->LoadService(NULL, &pService);
    pLoader->Release();
    pLoader = NULL;
    if (FAILED(hr))
    {
        printf("LoadService failed: hr=%X\n", hr);
        return 0;
    }

    // Call QueryProviders
    hr = pService->QueryProviders(VDS_QUERY_SOFTWARE_PROVIDERS, &pEnum);
    if (FAILED(hr))
    {
        printf("QueryProviders(VDS_QUERY_SOFTWARE_PROVIDERS) failed: hr=%X\n", hr);
        goto _bailout;
    }

    IUnknown *pUnk;
    IVdsProvider *pProvider;
    ULONG ulFetched = 0;
    int iProviderNumber = 1;

    // Enumerate the VDS software providers
    do
    {
        hr = pEnum->Next(1, &pUnk, &ulFetched);
        // S_FALSE indicates there are no more providers in the list
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
            printf("\tpUnk->QueryInterface(IVdsProvider) failed: hr=%X\n", hr);
            goto _bailout;
        }

        VDS_PROVIDER_PROP provProp;
        hr = pProvider->GetProperties(&provProp);
        if (SUCCEEDED(hr))
        {
            printf("Provider #: %ld name: %ws\n", iProviderNumber++, provProp.pwszName);
        }

        // Get IVdsSwProvider
        IVdsSwProvider *pSwProvider;
        hr = pProvider->QueryInterface(IID_IVdsSwProvider, (void **)&pSwProvider);
        SAFE_RELEASE(pProvider);

        if (FAILED(hr)) 
        {
            printf("\t\tpProvider->QueryInterface(IVdsSwProvider) failed: hr=%X\n", hr);
            goto _bailout;
        }

        // Enumerate packs associated with the software provider
        hr = pSwProvider->QueryPacks(&pEnumPack);
        SAFE_RELEASE(pSwProvider);

        if (FAILED(hr))
        {
            printf("\t\tpSwProvider->QueryPacks() failed: hr=%X\n", hr);
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
                printf("\t\t\tpEnumPack->Next() failed: hr=%X\n", hr); 
                goto _bailout;
            }

            printf("Pack #: %ld\n", iPackNumber++ );

            // Get IVdsPack
            IVdsPack *pPack;
            hr = pPackUnk->QueryInterface(IID_IVdsPack, (void **)&pPack);
            SAFE_RELEASE(pPackUnk);

            if (FAILED(hr))
            {
                printf( "\n\n\npPackUnk->QueryInterface() failed: hr=%X\n", hr); 
                goto _bailout;
            }

            // Get the volume enumerator to list volumes associated with the pack
            hr = pPack->QueryVolumes( &pEnumVolume );
            SAFE_RELEASE(pPack);

            if (FAILED(hr))
            {
                printf( "\n\n\npPack->QueryVolumes() failed: hr=%X\n", hr);
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
                    break;   // End of enumeration
                }

                if (FAILED(hr))
                {
                    printf("\t\t\tpEnumVolume->Next() failed: hr=%X\n", hr);
                    goto _bailout;
                }

                printf("Volume #: %ld\n", iVolumeNumber++ );

                // Get IVdsVolume         
                IVdsVolume *pVolume;
                hr = pVolumeUnk->QueryInterface(IID_IVdsVolume, (void **)&pVolume);
                SAFE_RELEASE(pVolumeUnk);

                if (FAILED(hr))
                {
                    printf( "\t\t\tpVolumeUnk->QueryInterface() failed: hr=%X\n", hr); 
                    goto _bailout;
                }

                // Get volume properties
                VDS_VOLUME_PROP VolumeProp;
                hr = pVolume->GetProperties(&VolumeProp);
                if (SUCCEEDED(hr))
                {
                    StringFromGUID2(VolumeProp.id, buffer, GUID_LEN);
                    wprintf(L"\tVolume Id: %ws\n", buffer);
                    printf("\tType: %ws\n", TypeString[VolumeProp.type - VDS_VT_SIMPLE + 1]);
                    printf("\tStatus: %ws\n", StatusString[VolumeProp.status]);
                    printf("\tHealth: %ws\n", HealthString[VolumeProp.health]);
                    printf("\tTransitionState: %ws\n", TransitionStateString[VolumeProp.TransitionState]);
                    printf("\tSize: %I64u\n", VolumeProp.ullSize);
                    printf("\tFlags: %lx\n", VolumeProp.ulFlags);
                    printf("\tRecommended file system: %ws\n", FileSystemString[VolumeProp.RecommendedFileSystemType]);
                    wprintf(L"\tWin32 name: %s\n", VolumeProp.pwszName);
                    CoTaskMemFree(VolumeProp.pwszName);
                }
                ListVolumeDisks(pVolume);

                // Get IVdsVolumeMF
                IVdsVolumeMF *pVolumeMF;
                hr = pVolume->QueryInterface(IID_IVdsVolumeMF, (void **)&pVolumeMF);

                if (FAILED(hr))
                {
                    printf("\t\tpVolume->QueryInterface(IVdsVolumeMF) failed: hr=%X\n", hr); 
                }
                else
                {
                    // Get file system properties
                    VDS_FILE_SYSTEM_PROP FileSysProp;
                    hr = pVolumeMF->GetFileSystemProperties(&FileSysProp);
                    if (FAILED(hr))
                    {
                        printf( "              GetFileSystemProperties failed: hr=%X\n", hr); 
                    }
                    else
                        if (SUCCEEDED(hr))
                        {
                            printf("\tFile System: %ws\n", FileSystemString[FileSysProp.type]);
                            printf("\tFlags: %X\n", FileSysProp.ulFlags);
                            printf("\tAllocationUnitSize: %lu\n", FileSysProp.ulAllocationUnitSize);
                            printf("\tTotalAllocationUnits: %I64u\n", FileSysProp.ullTotalAllocationUnits);
                            printf("\tAvailableAllocationUnits: %I64u\n", FileSysProp.ullAvailableAllocationUnits);
                            wprintf(L"\tLabel: %s\n", FileSysProp.pwszLabel);
                        }

                        // Get access paths
                        LPWSTR  *pwszPathArray;
                        LONG lNumberOfAccessPaths;
                        hr = pVolumeMF->QueryAccessPaths(&pwszPathArray, &lNumberOfAccessPaths);
                        if (FAILED(hr))
                        {
                            printf("\t\tpVolumeMF->QueryAccessPaths failed: hr=%X\n", hr);
                            SAFE_RELEASE(pVolumeMF);
                            continue;
                        }

                        printf("\tNumber of access paths: %ld\n", lNumberOfAccessPaths);
                        for (int i=0; i<lNumberOfAccessPaths; i++)
                        {
                            wprintf(L"\t  %s\n", pwszPathArray[i]);
                            SAFE_FREE(pwszPathArray[i]);
                        }
                        SAFE_FREE(pwszPathArray);

                        // Get access paths properties
                        VDS_REPARSE_POINT_PROP *pReparsePointProps = NULL;
                        LONG lNumberOfReparsePointProps;
                        hr = pVolumeMF->QueryReparsePoints(&pReparsePointProps, &lNumberOfReparsePointProps);
                        if (FAILED(hr))
                        {
                            printf( "\t\tpVolumeMF->QueryReparsePoints failed: hr=%X\n", hr );
                        }
                        else if (SUCCEEDED(hr))
                        {
                            printf("\tNumber of reparse points: %d\n", lNumberOfReparsePointProps);
                            for (LONG j = 0; j < lNumberOfReparsePointProps; j++)
                            {
                                StringFromGUID2(pReparsePointProps[j].SourceVolumeId, buffer, 500);
                                wprintf(L"\t  ( %ws, %s )\n", buffer, pReparsePointProps[j].pwszPath);
                                SAFE_FREE( pReparsePointProps[j].pwszPath);
                            }
                            SAFE_FREE(pReparsePointProps);
                        }
                }

                // Get IVdsVolumeMF
                IVdsVolumeMF2 *pVolumeMF2;
                hr = pVolume->QueryInterface(IID_IVdsVolumeMF2, (void **)&pVolumeMF2);
                if (FAILED(hr)) 
                {
                    printf("\t\tpVolume->QueryInterface(IVdsVolumeMF2) failed: hr=%X\n", hr); 
                    goto _bailout;
                }

                LPWSTR pwszFileSystemTypeName;
                hr = pVolumeMF2->GetFileSystemTypeName(&pwszFileSystemTypeName);
                if (SUCCEEDED(hr))
                {
                    printf("\tFile system type: %ws\n", pwszFileSystemTypeName);
                }
                SAFE_FREE(pwszFileSystemTypeName);

                SAFE_RELEASE(pVolume);
                SAFE_RELEASE(pVolumeMF);
                SAFE_RELEASE(pVolumeMF2);
            } while (TRUE);

            SAFE_RELEASE(pEnumVolume);

        } while (TRUE);

        SAFE_RELEASE( pEnumPack );

    } while (TRUE);

    SAFE_RELEASE(pEnum);
    SAFE_RELEASE(pService);

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

