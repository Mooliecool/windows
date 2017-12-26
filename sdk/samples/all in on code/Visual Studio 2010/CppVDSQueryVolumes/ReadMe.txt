=============================================================================
       CONSOLE APPLICATION : CppVDSQueryVolumes Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

This sample demonstrates how to use VDS to enumerate and interface with 
volumes on a system.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

The minimum supported client system of VDS API is Windows Vista.  The minimum 
supported server system is Windows Server 2003. 


/////////////////////////////////////////////////////////////////////////////
Demo:

The code sample must be run as administrator. Otherwise, the sample project 
will output the following error, which means COR_E_UNAUTHORIZEDACCESS.
    LoadService failed: hr=80070005

This sample takes no user input, it is executed as just the name of the app, 
and the result is a list of volumes, their properties, and the disk extents 
that back the volume.  Here is an example output of the sample.

    Provider #: 1 name: Microsoft Virtual Disk Service Dynamic Provider
    Pack #: 1
    Provider #: 2 name: Microsoft Virtual Disk Service Basic Provider
    Pack #: 1
    Volume #: 1
            Volume Id: {619631A8-8F87-4AB0-B769-78B84A9D10DC}
            Type: SIMPLE
            Status: NO_MEDIA
            Health: HEALTHY
            TransitionState: STABLE
            Size: 0
            Flags: 2000
            Recommended file system: UNKNOWN
            Win32 name: \\?\GLOBALROOT\Device\CdRom0

            ... dumping disk extents ...
            Number of extents: 1

                    Extent #1:
                    Disk Id: {F10B8064-B7B2-47D8-BE1C-447854AFE575}
                    type: UNKNOWN
                    Offset: 0
                    Size: 0
                    Volume Id: {619631A8-8F87-4AB0-B769-78B84A9D10DC}
                    Plex Id: {29DFC345-559F-47B5-98F0-1BB796561A8F}
                    Member Id: 0
                  GetFileSystemProperties failed: hr=80042412
            Number of access paths: 1
              E:\
            Number of reparse points: 0
    Pack #: 2
    Volume #: 1
            Volume Id: {EAEC0907-CEC1-49B8-AC5E-AAE7B31BDD25}
            Type: SIMPLE
            Status: ONLINE
            Health: HEALTHY
            TransitionState: STABLE
            Size: 104857600
            Flags: a0465
            Recommended file system: NTFS
            Win32 name: \\?\GLOBALROOT\Device\HarddiskVolume1

            ... dumping disk extents ...
            Number of extents: 1

                    Extent #1:
                    Disk Id: {51662905-42AF-49D2-9B28-CC151F6F4F61}
                    type: DATA
                    Offset: 1048576
                    Size: 104857600
                    Volume Id: {EAEC0907-CEC1-49B8-AC5E-AAE7B31BDD25}
                    Plex Id: {ABA368A2-3695-4E8B-B9FC-9253BFF8326B}
                    Member Id: 0
            File System: NTFS
            Flags: 0
            AllocationUnitSize: 4096
            TotalAllocationUnits: 25599
            AvailableAllocationUnits: 18399
            Label: System Reserved
            Number of access paths: 0
            Number of reparse points: 0
            File system type: NTFS
    ......


/////////////////////////////////////////////////////////////////////////////
Implementation:

The main logic of this code is:

1. connect to the VDS service
2. loop over all software providers: basic and dynamic
3. for a given provider, list the packs and the volumes contained in those packs.
4. for each volume, provide the properties of the volume
5. for each volume display the file system information.
6. for each volume list the disk extents backing the volume


/////////////////////////////////////////////////////////////////////////////
References:

The location for complete information on VDS is:
http://msdn.microsoft.com/en-us/library/bb986750.aspx


/////////////////////////////////////////////////////////////////////////////
