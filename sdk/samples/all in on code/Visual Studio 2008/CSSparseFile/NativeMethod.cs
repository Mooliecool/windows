/****************************** Module Header ******************************\
* Module Name:  NativeMethod.cs
* Project:      CSSparseFile
* Copyright (c) Microsoft Corporation.
* 
* Declarations of native structs and functions for operations on sparse file.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 7/5/2009 11:42 AM Jialiang Ge Created
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Threading;
using Microsoft.Win32.SafeHandles;
using System.IO;
#endregion


[StructLayout(LayoutKind.Sequential)]
public struct FILE_ZERO_DATA_INFORMATION
{
    public Int64 FileOffset;
    public Int64 BeyondFinalZero;
}

[StructLayout(LayoutKind.Sequential)]
public struct FILE_ALLOCATED_RANGE_BUFFER
{
    public Int64 FileOffset;
    public Int64 Length;
}

[Flags]
public enum FileSystemFeature : uint
{
    CaseSensitiveSearch = 1,
    CasePreservedNames = 2,
    UnicodeOnDisk = 4,
    PersistentACLS = 8,
    FileCompression = 0x10,
    VolumeQuotas = 0x20,
    SupportsSparseFiles = 0x40,
    SupportsReparsePoints = 0x80,
    VolumeIsCompressed = 0x8000,
    SupportsObjectIDs = 0x10000,
    SupportsEncryption = 0x20000,
    NamedStreams = 0x40000,
    ReadOnlyVolume = 0x80000,
    SequentialWriteOnce = 0x100000,
    SupportsTransactions = 0x200000,
}

[Flags]
public enum EIoControlCode : uint
{
    ChangerBase = 0x30,
    ChangerExchangeMedium = 0x304020,
    ChangerGetElementStatus = 0x30c014,
    ChangerGetParameters = 0x304000,
    ChangerGetProductData = 0x304008,
    ChangerGetStatus = 0x304004,
    ChangerInitializeElementStatus = 0x304018,
    ChangerMoveMedium = 0x304024,
    ChangerQueryVolumeTags = 0x30c02c,
    ChangerReinitializeTarget = 0x304028,
    ChangerSetAccess = 0x30c010,
    ChangerSetPosition = 0x30401c,
    DiskBase = 7,
    DiskCheckVerify = 0x74800,
    DiskControllerNumber = 0x70044,
    DiskDeleteDriveLayout = 0x7c100,
    DiskEjectMedia = 0x74808,
    DiskFindNewDevices = 0x74818,
    DiskFormatDrive = 0x7c3cc,
    DiskFormatTracks = 0x7c018,
    DiskFormatTracksEx = 0x7c02c,
    DiskGetCacheInformation = 0x740d4,
    DiskGetDriveGeometry = 0x70000,
    DiskGetDriveLayout = 0x7400c,
    DiskGetMediaTypes = 0x70c00,
    DiskGetPartitionInfo = 0x74004,
    DiskGrowPartition = 0x7c0d0,
    DiskHistogramData = 0x70034,
    DiskHistogramReset = 0x70038,
    DiskHistogramStructure = 0x70030,
    DiskIsWritable = 0x70024,
    DiskLoadMedia = 0x7480c,
    DiskLogging = 0x70028,
    DiskMediaRemoval = 0x74804,
    DiskPerformance = 0x70020,
    DiskReassignBlocks = 0x7c01c,
    DiskRelease = 0x74814,
    DiskRequestData = 0x70040,
    DiskRequestStructure = 0x7003c,
    DiskReserve = 0x74810,
    DiskSenseDevice = 0x703e0,
    DiskSetCacheInformation = 0x7c0d8,
    DiskSetDriveLayout = 0x7c010,
    DiskSetPartitionInfo = 0x7c008,
    DiskSmartGetVersion = 0x74080,
    DiskSmartRcvDriveData = 0x7c088,
    DiskSmartSendDriveCommand = 0x7c084,
    DiskUpdateDriveSize = 0x7c0c8,
    DiskVerify = 0x70014,
    FsctlAllowExtendedDasdIo = 0x90083,
    FsctlCreateOrGetObjectId = 0x900c0,
    FsctlCreateUsnJournal = 0x900e7,
    FsctlDeleteObjectId = 0x900a0,
    FsctlDeleteReparsePoint = 0x900ac,
    FsctlDeleteUsnJournal = 0x900f8,
    FsctlDismountVolume = 0x90020,
    FsctlDumpPropertyData = 0x90097,
    FsctlEnableUpgrade = 0x980d0,
    FsctlEncryptionFsctlIo = 0x900db,
    FsctlEnumUsnData = 0x900b3,
    FsctlExtendVolume = 0x900f0,
    FsctlFileSystemGetStatistics = 0x90060,
    FsctlFindFilesBySid = 0x9008f,
    FsctlGetCompression = 0x9003c,
    FsctlGetHfsInformation = 0x9007c,
    FsctlGetNtfsFileRecord = 0x90068,
    FsctlGetNtfsVolumeData = 0x90064,
    FsctlGetObjectId = 0x9009c,
    FsctlGetReparsePoint = 0x900a8,
    FsctlGetRetrievalPointers = 0x90073,
    FsctlGetVolumeBitmap = 0x9006f,
    FsctlHsmData = 0x9c113,
    FsctlHsmMsg = 0x9c108,
    FsctlInvalidateVolumes = 0x90054,
    FsctlIsPathnameValid = 0x9002c,
    FsctlIsVolumeDirty = 0x90078,
    FsctlIsVolumeMounted = 0x90028,
    FsctlLockVolume = 0x90018,
    FsctlMarkAsSystemHive = 0x9004f,
    FsctlMarkHandle = 0x900fc,
    FsctlMarkVolumeDirty = 0x90030,
    FsctlMoveFile = 0x90074,
    FsctlNssControl = 0x9810c,
    FsctlNssRcontrol = 0x94118,
    FsctlOpBatchAckClosePending = 0x90010,
    FsctlOplockBreakAckNo2 = 0x90050,
    FsctlOplockBreakAcknowledge = 0x9000c,
    FsctlOplockBreakNotify = 0x90014,
    FsctlQueryAllocatedRanges = 0x940cf,
    FsctlQueryFatBpb = 0x90058,
    FsctlQueryRetrievalPointers = 0x9003b,
    FsctlQueryUsnJournal = 0x900f4,
    FsctlReadFileUsnData = 0x900eb,
    FsctlReadPropertyData = 0x90087,
    FsctlReadRawEncrypted = 0x900e3,
    FsctlReadUsnJournal = 0x900bb,
    FsctlRecallFile = 0x90117,
    FsctlRequestBatchOplock = 0x90008,
    FsctlRequestFilterOplock = 0x9005c,
    FsctlRequestOplockLevel1 = 0x90000,
    FsctlRequestOplockLevel2 = 0x90004,
    FsctlSecurityIdCheck = 0x940b7,
    FsctlSetCompression = 0x9c040,
    FsctlSetEncryption = 0x900d7,
    FsctlSetObjectId = 0x90098,
    FsctlSetObjectIdExtended = 0x900bc,
    FsctlSetReparsePoint = 0x900a4,
    FsctlSetSparse = 0x900c4,
    FsctlSetZeroData = 0x980c8,
    FsctlSisCopyFile = 0x90100,
    FsctlSisLinkFiles = 0x9c104,
    FsctlUnlockVolume = 0x9001c,
    FsctlWritePropertyData = 0x9008b,
    FsctlWriteRawEncrypted = 0x900df,
    FsctlWriteUsnCloseRecord = 0x900ef,
    StorageBase = 0x2d,
    StorageCheckVerify = 0x2d4800,
    StorageCheckVerify2 = 0x2d0800,
    StorageEjectionControl = 0x2d0940,
    StorageEjectMedia = 0x2d4808,
    StorageFindNewDevices = 0x2d4818,
    StorageGetDeviceNumber = 0x2d1080,
    StorageGetMediaTypes = 0x2d0c00,
    StorageGetMediaTypesEx = 0x2d0c04,
    StorageLoadMedia = 0x2d480c,
    StorageLoadMedia2 = 0x2d080c,
    StorageMcnControl = 0x2d0944,
    StorageMediaRemoval = 0x2d4804,
    StorageObsoleteResetBus = 0x2dd000,
    StorageObsoleteResetDevice = 0x2dd004,
    StoragePredictFailure = 0x2d1100,
    StorageRelease = 0x2d4814,
    StorageReserve = 0x2d4810,
    StorageResetBus = 0x2d5000,
    StorageResetDevice = 0x2d5004,
    VideoQueryDisplayBrightness = 0x230498,
    VideoQuerySupportedBrightness = 0x230494,
    VideoSetDisplayBrightness = 0x23049c
}

/// <summary>
/// Native methods
/// </summary>
internal class NativeMethod
{
    public const int MAX_PATH = 260;
    public const int ERROR_MORE_DATA = 234;

    /// <summary>
    /// Sends a control code directly to a specified device driver, causing 
    /// the corresponding device to perform the corresponding operation.
    /// </summary>
    /// <param name="Device">
    /// A handle to the device on which the operation is to be performed.
    /// </param>
    /// <param name="IoControlCode">
    /// The control code for the operation.
    /// </param>
    /// <param name="InBuffer">
    /// A pointer to the input buffer that contains the data required to 
    /// perform the operation. This parameter can be NULL if dwIoControlCode 
    /// specifies an operation that does not require input data.
    /// </param>
    /// <param name="InBufferSize">
    /// The size of the input buffer, in bytes.
    /// </param>
    /// <param name="OutBuffer">
    /// A pointer to the output buffer that is to receive the data returned 
    /// by the operation. This parameter can be NULL if dwIoControlCode 
    /// specifies an operation that does not return data.
    /// </param>
    /// <param name="OutBufferSize">
    /// The size of the output buffer, in bytes.
    /// </param>
    /// <param name="BytesReturned">
    /// A pointer to a variable that receives the size of the data stored in 
    /// the output buffer, in bytes.
    /// </param>
    /// <param name="Overlapped">
    /// A pointer to an OVERLAPPED structure.
    /// </param>
    /// <returns></returns>
    [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Auto)]
    public static extern bool DeviceIoControl(
        SafeFileHandle Device,
        EIoControlCode IoControlCode,
        IntPtr InBuffer,
        int InBufferSize,
        IntPtr OutBuffer,
        int OutBufferSize,
        ref int BytesReturned,
        [In] ref NativeOverlapped Overlapped
        );

    /// <summary>
    /// Retrieves information about the file system and volume associated 
    /// with the specified root directory.
    /// </summary>
    /// <param name="RootPathName">
    /// A string that contains the root directory of the volume to be 
    /// described. For example, "C:\".
    /// </param>
    /// <param name="VolumeNameBuffer">
    /// A pointer to a buffer that receives the name of a specified volume. 
    /// The maximum buffer size is MAX_PATH+1.
    /// </param>
    /// <param name="VolumeNameSize">
    /// The length of a volume name buffer, in TCHARs. The maximum buffer 
    /// size is MAX_PATH+1.
    /// </param>
    /// <param name="VolumeSerialNumber">
    /// A pointer to a variable that receives the volume serial number.
    /// </param>
    /// <param name="MaximumComponentLength">
    /// A pointer to a variable that receives the maximum length, in TCHARs, 
    /// of a file name component that a specified file system supports.
    /// </param>
    /// <param name="FileSystemFlags">
    /// A pointer to a variable that receives flags associated with the 
    /// specified file system.
    /// </param>
    /// <param name="FileSystemNameBuffer">
    /// A pointer to a buffer that receives the name of the file system, for 
    /// example, the FAT file system or the NTFS file system. The maximum 
    /// buffer size is MAX_PATH+1.
    /// </param>
    /// <param name="FileSystemNameSize">
    /// The length of the file system name buffer, in TCHARs. The maximum 
    /// buffer size is MAX_PATH+1.
    /// </param>
    /// <returns></returns>
    [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Auto)]
    public static extern bool GetVolumeInformation(
        string RootPathName,
        StringBuilder VolumeNameBuffer,
        int VolumeNameSize,
        out uint VolumeSerialNumber,
        out uint MaximumComponentLength,
        out FileSystemFeature FileSystemFlags,
        StringBuilder FileSystemNameBuffer,
        int FileSystemNameSize
        );

    /// <summary>
    /// Retrieves the actual number of bytes of disk storage used to store a 
    /// specified file. If the file is located on a volume that supports 
    /// compression and the file is compressed, the value obtained is the 
    /// compressed size of the specified file. If the file is located on a 
    /// volume that supports sparse files and the file is a sparse file, the 
    /// value obtained is the sparse size of the specified file.
    /// </summary>
    /// <param name="FileName">The name of the file.</param>
    /// <param name="FileSizeHigh">
    /// The high-order DWORD of the compressed file size.
    /// </param>
    /// <returns>
    /// If the function succeeds, the return value is the low-order DWORD of 
    /// the actual number of bytes of disk storage used to store the 
    /// specified file, and if lpFileSizeHigh is non-NULL, the function puts 
    /// the high-order DWORD of that actual value into the DWORD pointed to 
    /// by that parameter. This is the compressed file size for compressed 
    /// files, the actual file size for noncompressed files.
    /// </returns>
    [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Auto)]
    public static extern uint GetCompressedFileSize(string FileName, 
        out uint FileSizeHigh);
}