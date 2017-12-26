'****************************** Module Header ******************************'
' Module Name:  NativeMethod.vb
' Project:      VBSparseFile
' Copyright (c) Microsoft Corporation.
' 
' Declarations of native structs and functions for operations on sparse file.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' * 7/7/2009 11:34 PM Jialiang Ge Created
'***************************************************************************'

#Region "Imports directives"

Imports System.Runtime.InteropServices
Imports Microsoft.Win32.SafeHandles
Imports System.Threading
Imports System.Text

#End Region


<StructLayout(LayoutKind.Sequential)> _
Public Structure FILE_ZERO_DATA_INFORMATION
    Public FileOffset As Long
    Public BeyondFinalZero As Long
End Structure

<StructLayout(LayoutKind.Sequential)> _
Public Structure FILE_ALLOCATED_RANGE_BUFFER
    Public FileOffset As Long
    Public Length As Long
End Structure

<Flags()> _
Public Enum FileSystemFeature As UInt32
    CasePreservedNames = 2
    CaseSensitiveSearch = 1
    FileCompression = &H10
    NamedStreams = &H40000
    PersistentACLS = 8
    ReadOnlyVolume = &H80000
    SequentialWriteOnce = &H100000
    SupportsEncryption = &H20000
    SupportsObjectIDs = &H10000
    SupportsReparsePoints = &H80
    SupportsSparseFiles = &H40
    SupportsTransactions = &H200000
    UnicodeOnDisk = 4
    VolumeIsCompressed = &H8000
    VolumeQuotas = &H20
End Enum

<Flags()> _
Public Enum EIoControlCode As UInt32
    ChangerBase = &H30
    ChangerExchangeMedium = &H304020
    ChangerGetElementStatus = &H30C014
    ChangerGetParameters = &H304000
    ChangerGetProductData = &H304008
    ChangerGetStatus = &H304004
    ChangerInitializeElementStatus = &H304018
    ChangerMoveMedium = &H304024
    ChangerQueryVolumeTags = &H30C02C
    ChangerReinitializeTarget = &H304028
    ChangerSetAccess = &H30C010
    ChangerSetPosition = &H30401C
    DiskBase = 7
    DiskCheckVerify = &H74800
    DiskControllerNumber = &H70044
    DiskDeleteDriveLayout = &H7C100
    DiskEjectMedia = &H74808
    DiskFindNewDevices = &H74818
    DiskFormatDrive = &H7C3CC
    DiskFormatTracks = &H7C018
    DiskFormatTracksEx = &H7C02C
    DiskGetCacheInformation = &H740D4
    DiskGetDriveGeometry = &H70000
    DiskGetDriveLayout = &H7400C
    DiskGetMediaTypes = &H70C00
    DiskGetPartitionInfo = &H74004
    DiskGrowPartition = &H7C0D0
    DiskHistogramData = &H70034
    DiskHistogramReset = &H70038
    DiskHistogramStructure = &H70030
    DiskIsWritable = &H70024
    DiskLoadMedia = &H7480C
    DiskLogging = &H70028
    DiskMediaRemoval = &H74804
    DiskPerformance = &H70020
    DiskReassignBlocks = &H7C01C
    DiskRelease = &H74814
    DiskRequestData = &H70040
    DiskRequestStructure = &H7003C
    DiskReserve = &H74810
    DiskSenseDevice = &H703E0
    DiskSetCacheInformation = &H7C0D8
    DiskSetDriveLayout = &H7C010
    DiskSetPartitionInfo = &H7C008
    DiskSmartGetVersion = &H74080
    DiskSmartRcvDriveData = &H7C088
    DiskSmartSendDriveCommand = &H7C084
    DiskUpdateDriveSize = &H7C0C8
    DiskVerify = &H70014
    FsctlAllowExtendedDasdIo = &H90083
    FsctlCreateOrGetObjectId = &H900C0
    FsctlCreateUsnJournal = &H900E7
    FsctlDeleteObjectId = &H900A0
    FsctlDeleteReparsePoint = &H900AC
    FsctlDeleteUsnJournal = &H900F8
    FsctlDismountVolume = &H90020
    FsctlDumpPropertyData = &H90097
    FsctlEnableUpgrade = &H980D0
    FsctlEncryptionFsctlIo = &H900DB
    FsctlEnumUsnData = &H900B3
    FsctlExtendVolume = &H900F0
    FsctlFileSystemGetStatistics = &H90060
    FsctlFindFilesBySid = &H9008F
    FsctlGetCompression = &H9003C
    FsctlGetHfsInformation = &H9007C
    FsctlGetNtfsFileRecord = &H90068
    FsctlGetNtfsVolumeData = &H90064
    FsctlGetObjectId = &H9009C
    FsctlGetReparsePoint = &H900A8
    FsctlGetRetrievalPointers = &H90073
    FsctlGetVolumeBitmap = &H9006F
    FsctlHsmData = &H9C113
    FsctlHsmMsg = &H9C108
    FsctlInvalidateVolumes = &H90054
    FsctlIsPathnameValid = &H9002C
    FsctlIsVolumeDirty = &H90078
    FsctlIsVolumeMounted = &H90028
    FsctlLockVolume = &H90018
    FsctlMarkAsSystemHive = &H9004F
    FsctlMarkHandle = &H900FC
    FsctlMarkVolumeDirty = &H90030
    FsctlMoveFile = &H90074
    FsctlNssControl = &H9810C
    FsctlNssRcontrol = &H94118
    FsctlOpBatchAckClosePending = &H90010
    FsctlOplockBreakAckNo2 = &H90050
    FsctlOplockBreakAcknowledge = &H9000C
    FsctlOplockBreakNotify = &H90014
    FsctlQueryAllocatedRanges = &H940CF
    FsctlQueryFatBpb = &H90058
    FsctlQueryRetrievalPointers = &H9003B
    FsctlQueryUsnJournal = &H900F4
    FsctlReadFileUsnData = &H900EB
    FsctlReadPropertyData = &H90087
    FsctlReadRawEncrypted = &H900E3
    FsctlReadUsnJournal = &H900BB
    FsctlRecallFile = &H90117
    FsctlRequestBatchOplock = &H90008
    FsctlRequestFilterOplock = &H9005C
    FsctlRequestOplockLevel1 = &H90000
    FsctlRequestOplockLevel2 = &H90004
    FsctlSecurityIdCheck = &H940B7
    FsctlSetCompression = &H9C040
    FsctlSetEncryption = &H900D7
    FsctlSetObjectId = &H90098
    FsctlSetObjectIdExtended = &H900BC
    FsctlSetReparsePoint = &H900A4
    FsctlSetSparse = &H900C4
    FsctlSetZeroData = &H980C8
    FsctlSisCopyFile = &H90100
    FsctlSisLinkFiles = &H9C104
    FsctlUnlockVolume = &H9001C
    FsctlWritePropertyData = &H9008B
    FsctlWriteRawEncrypted = &H900DF
    FsctlWriteUsnCloseRecord = &H900EF
    StorageBase = &H2D
    StorageCheckVerify = &H2D4800
    StorageCheckVerify2 = &H2D0800
    StorageEjectionControl = &H2D0940
    StorageEjectMedia = &H2D4808
    StorageFindNewDevices = &H2D4818
    StorageGetDeviceNumber = &H2D1080
    StorageGetMediaTypes = &H2D0C00
    StorageGetMediaTypesEx = &H2D0C04
    StorageLoadMedia = &H2D480C
    StorageLoadMedia2 = &H2D080C
    StorageMcnControl = &H2D0944
    StorageMediaRemoval = &H2D4804
    StorageObsoleteResetBus = &H2DD000
    StorageObsoleteResetDevice = &H2DD004
    StoragePredictFailure = &H2D1100
    StorageRelease = &H2D4814
    StorageReserve = &H2D4810
    StorageResetBus = &H2D5000
    StorageResetDevice = &H2D5004
    VideoQueryDisplayBrightness = &H230498
    VideoQuerySupportedBrightness = &H230494
    VideoSetDisplayBrightness = &H23049C
End Enum

''' <summary>
''' Native methods
''' </summary>
''' <remarks></remarks>
Public Class NativeMethod

    Public Const MAX_PATH As Integer = 260
    Public Const ERROR_MORE_DATA As Integer = &HEA

    ''' <summary>
    ''' Sends a control code directly to a specified device driver, causing 
    ''' the corresponding device to perform the corresponding operation.
    ''' </summary>
    ''' <param name="Device">
    ''' A handle to the device on which the operation is to be performed.
    ''' </param>
    ''' <param name="IoControlCode">
    ''' The control code for the operation.
    ''' </param>
    ''' <param name="InBuffer">
    ''' A pointer to the input buffer that contains the data required to 
    ''' perform the operation. This parameter can be NULL if dwIoControlCode 
    ''' specifies an operation that does not require input data.
    ''' </param>
    ''' <param name="InBufferSize">
    ''' The size of the input buffer, in bytes.
    ''' </param>
    ''' <param name="OutBuffer">
    ''' A pointer to the output buffer that is to receive the data returned 
    ''' by the operation. This parameter can be NULL if dwIoControlCode 
    ''' specifies an operation that does not return data.
    ''' </param>
    ''' <param name="OutBufferSize">
    ''' The size of the output buffer, in bytes.
    ''' </param>
    ''' <param name="BytesReturned">
    ''' A pointer to a variable that receives the size of the data stored in 
    ''' the output buffer, in bytes.
    ''' </param>
    ''' <param name="Overlapped">
    ''' A pointer to an OVERLAPPED structure.
    ''' </param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function DeviceIoControl(ByVal Device As SafeFileHandle, _
                                           ByVal IoControlCode As EIoControlCode, _
                                           ByVal InBuffer As IntPtr, _
                                           ByVal InBufferSize As Integer, _
                                           ByVal OutBuffer As IntPtr, _
                                           ByVal OutBufferSize As Integer, _
                                           ByRef BytesReturned As Integer, _
                                           <[In]()> ByRef Overlapped As NativeOverlapped) _
                                           As Boolean
    End Function


    ''' <summary>
    ''' Retrieves information about the file system and volume associated 
    ''' with the specified root directory.
    ''' </summary>
    ''' <param name="RootPathName">
    ''' A string that contains the root directory of the volume to be 
    ''' described. For example, "C:\".
    ''' </param>
    ''' <param name="VolumeNameBuffer">
    ''' A pointer to a buffer that receives the name of a specified volume. 
    ''' The maximum buffer size is MAX_PATH+1.
    ''' </param>
    ''' <param name="VolumeNameSize">
    ''' The length of a volume name buffer, in TCHARs. The maximum buffer 
    ''' size is MAX_PATH+1.
    ''' </param>
    ''' <param name="VolumeSerialNumber">
    ''' A pointer to a variable that receives the volume serial number.
    ''' </param>
    ''' <param name="MaximumComponentLength">
    ''' A pointer to a variable that receives the maximum length, in TCHARs, 
    ''' of a file name component that a specified file system supports.
    ''' </param>
    ''' <param name="FileSystemFlags">
    ''' A pointer to a variable that receives flags associated with the 
    ''' specified file system.
    ''' </param>
    ''' <param name="FileSystemNameBuffer">
    ''' A pointer to a buffer that receives the name of the file system, for 
    ''' example, the FAT file system or the NTFS file system. The maximum 
    ''' buffer size is MAX_PATH+1.
    ''' </param>
    ''' <param name="FileSystemNameSize">
    ''' The length of the file system name buffer, in TCHARs. The maximum 
    ''' buffer size is MAX_PATH+1.
    ''' </param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function GetVolumeInformation(ByVal RootPathName As String, _
                                                ByVal VolumeNameBuffer As StringBuilder, _
                                                ByVal VolumeNameSize As Integer, _
                                                <Out()> ByRef VolumeSerialNumber As UInt32, _
                                                <Out()> ByRef MaximumComponentLength As UInt32, _
                                                <Out()> ByRef FileSystemFlags As FileSystemFeature, _
                                                ByVal FileSystemNameBuffer As StringBuilder, _
                                                ByVal FileSystemNameSize As Integer) _
                                                As Boolean
    End Function


    ''' <summary>
    ''' Retrieves the actual number of bytes of disk storage used to store a 
    ''' specified file. If the file is located on a volume that supports 
    ''' compression and the file is compressed, the value obtained is the 
    ''' compressed size of the specified file. If the file is located on a 
    ''' volume that supports sparse files and the file is a sparse file, the 
    ''' value obtained is the sparse size of the specified file.
    ''' </summary>
    ''' <param name="FileName">The name of the file.</param>
    ''' <param name="FileSizeHigh">
    ''' The high-order DWORD of the compressed file size.
    ''' </param>
    ''' <returns>
    ''' If the function succeeds, the return value is the low-order DWORD of 
    ''' the actual number of bytes of disk storage used to store the 
    ''' specified file, and if lpFileSizeHigh is non-NULL, the function puts 
    ''' the high-order DWORD of that actual value into the DWORD pointed to 
    ''' by that parameter. This is the compressed file size for compressed 
    ''' files, the actual file size for noncompressed files.
    ''' </returns>
    ''' <remarks></remarks>
    <DllImport("kernel32.dll", CharSet:=CharSet.Auto, SetLastError:=True)> _
    Public Shared Function GetCompressedFileSize(ByVal FileName As String, _
                                                 <Out()> ByRef FileSizeHigh As UInt32) _
                                                 As UInt32
    End Function


End Class