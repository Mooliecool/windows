=============================================================================
       CONSOLE APPLICATION : CppVDSUninstallDisks Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

This sample demonstrates how to use VDS to uninstall disks and volumes.

Warning: uninstalling a disk may lead to potential data loss.  Use this 
sample with care.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

The minimum supported client system of VDS API is Windows Vista.  The minimum 
supported server system is Windows Server 2003. 


/////////////////////////////////////////////////////////////////////////////
Demo:

This sample takes the disk number performs a complete uninstall, including the 
dismount of the volume and uninstall underlying disks.

To use this sample you will need to provide the disk number for input. 
This can be found by right clicking on my computer choosing manage, once the 
Manage UI is open choose Disk Management and look for the TEST disk that you 
wish to uninstall.  The number associated with this TEST disk will be the input 
parameter.  After you run the sample this TEST disk should disappear from Disk
Management.  If you are not using VHD's for this test you will need to reboot
your system for the disk to be recognized by Disk Management.


/////////////////////////////////////////////////////////////////////////////
Implementation:

The main logic of this code is:

1. connect to the VDS service

    IVdsServiceLoader *pLoader;
    hr = CoCreateInstance(CLSID_VdsLoader,
                          NULL,
                          CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER,
                          IID_IVdsServiceLoader,
                          (void **)&pLoader
                          );
    ...
    IVdsService *pService;
	hr = pLoader->LoadService( L"", &pService );
    ...

2. validate user input

3. loop over all software providers: basic and dynamic

    hr = pService->QueryProviders(VDS_QUERY_SOFTWARE_PROVIDERS, &pEnum );
    hr = pUnk->QueryInterface(IID_IVdsProvider, (void **)&pProvider);
    IVdsSwProvider *pSwProvider;
    hr = pProvider->QueryInterface(IID_IVdsSwProvider, (void **)&pSwProvider);

4. for a given provider, list the packs and the disks contained in those packs.

    hr = pSwProvider->QueryPacks(&pEnumPack);

5. for each disk, check for the disk number requested in the user input.

    hr = pPack->QueryDisks(&pEnumDisk);
    hr = GetDiskIds(pEnumDisk, bVerbose, &pDiskArray, ulDisk);

6. if the disk is found, uninstall it.

    hr = pService->QueryInterface(
        IID_IVdsServiceUninstallDisk, 
        (void **)&pUninstallIntf
        );


/////////////////////////////////////////////////////////////////////////////
References:

The location for complete information on VDS is:
http://msdn.microsoft.com/en-us/library/aa382815(v=VS.85).aspx


/////////////////////////////////////////////////////////////////////////////