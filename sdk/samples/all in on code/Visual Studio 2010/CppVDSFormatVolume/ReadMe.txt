=============================================================================
       CONSOLE APPLICATION : CppVDSFormatVolume Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

This sample demonstrates how to use VDS to format volumes.

Warning: formatting a volume may result in unexpected data loss.  Use this 
sample with care.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

The minimum supported client system of VDS API is Windows Vista.  The minimum 
supported server system is Windows Server 2003. 


/////////////////////////////////////////////////////////////////////////////
Demo:

You must run this sample project as administrator.  The sample takes the 
volume number and the target file system type as user input:

    CppVDSFormatVolume.exe [-options]...
      -v <volume number (decimal)> REQUIRED INPUT
      -t <target file system type: NTFS, FAT32, etc.> UPPER CASE ONLY, REQUIRED INPUT
      e.g.:  CppVDSFormatVolume.exe -v 3 -t NTFS 

The sample project tries to find the volume based on the volume number, and 
format the volume with the specified file system type.

To get the volume number of a certain volume (e.g. "E:\"), you can download 
and run the sample project "CppVDSQueryVolumes" from All-In-One Code 
Framework. CppVDSQueryVolumes lists all volumes in your system and displays 
their volume number appended to the Win32 name output:

    Win32 name: \\?\GLOBALROOT\Device\HarddiskVolume<volume number>

For example, 

    Win32 name: \\?\GLOBALROOT\Device\HarddiskVolume5

Formatting a volume may result in unexpected data loss, so please use this 
sample with care.  To play with the sample project safely, you may consider 
plugging in a useless USB flash drive. Find its volume number using the 
CppVDSQueryVolumes sample project. Then pass the volume number to 
CppVDSFormatVolume to format the volume. 

    CppVDSFormatVolume.exe -v <volume number (decimal)> -t <target file system type e.g. FAT32>


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

    IVdsProvider *pProvider;
    hr = pUnk->QueryInterface(IID_IVdsProvider, (void **)&pProvider);
	...
	VDS_PROVIDER_PROP provProp;
    hr = pProvider->GetProperties(&provProp);
    ...
	IVdsSwProvider *pSwProvider;
    hr = pProvider->QueryInterface(IID_IVdsSwProvider, (void **)&pSwProvider);
    ...

4. for a given provider, list the packs and the volumes contained in those packs.

    hr = pPack->QueryVolumes( &pEnumVolume );
	// Get IVdsVolume         
    IVdsVolume *pVolume;
    hr = pVolumeUnk->QueryInterface(IID_IVdsVolume, (void **)&pVolume);

5. for each volume, check for the volume number requested in the user input.

6. if the volume is found, format it with the file system requested in the user input.

    hr = pVolumeMF3->FormatEx2(wszFsType,
                               Revision,
                               UnitAllocationSize,
                               Label,
                               (DWORD) Options,
                               &pAsync);


/////////////////////////////////////////////////////////////////////////////
References:

The location for complete information on VDS is:
http://msdn.microsoft.com/en-us/library/dd405618(v=VS.85).aspx


/////////////////////////////////////////////////////////////////////////////
