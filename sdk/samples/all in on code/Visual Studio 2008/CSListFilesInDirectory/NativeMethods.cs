/************************************* Module Header **************************************\
* Module Name:  NativeMethods.cs
* Project:      CSListFilesInDirectory
* Copyright (c) Microsoft Corporation.
* 
* The CSListFilesInDirectory project demonstrates how to implement an IEnumerable<string>
* that utilizes the Win32 File Management functions to enable application to get files and
* sub-directories in a specified directory one item a time.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 7/7/2009 8:00 PM Jie Wang Created
\******************************************************************************************/

#region Using directives
using System;
using System.Runtime.InteropServices;
using System.Runtime.ConstrainedExecution;
using System.Security.Permissions;
using Microsoft.Win32.SafeHandles;
#endregion


[Serializable, StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto), 
BestFitMapping(false)]
internal class WIN32_FIND_DATA
{
    internal int dwFileAttributes;
    internal int ftCreationTime_dwLowDateTime;
    internal int ftCreationTime_dwHighDateTime;
    internal int ftLastAccessTime_dwLowDateTime;
    internal int ftLastAccessTime_dwHighDateTime;
    internal int ftLastWriteTime_dwLowDateTime;
    internal int ftLastWriteTime_dwHighDateTime;
    internal int nFileSizeHigh;
    internal int nFileSizeLow;
    internal int dwReserved0;
    internal int dwReserved1;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)]
    internal string cFileName;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 14)]
    internal string cAlternateFileName;
}

/// <summary>
/// Win32 Native P/Invoke
/// </summary>
internal static class NativeMethods
{
    [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    internal static extern SafeFindHandle FindFirstFile(
        string fileName, [In, Out] WIN32_FIND_DATA data);

    [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    internal static extern bool FindNextFile(
        SafeFindHandle hndFindFile, 
        [In, Out, MarshalAs(UnmanagedType.LPStruct)] 
        WIN32_FIND_DATA lpFindFileData);

    [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success), DllImport("kernel32.dll")]
    internal static extern bool FindClose(IntPtr handle);

    internal const int ERROR_SUCCESS = 0;
    internal const int ERROR_NO_MORE_FILES = 18;
    internal const int ERROR_FILE_NOT_FOUND = 2;
    internal const int FILE_ATTRIBUTE_DIRECTORY = 0x00000010;
}

/// <summary>
/// Safe handle for using with the Find File APIs.
/// </summary>
internal sealed class SafeFindHandle : SafeHandleZeroOrMinusOneIsInvalid
{
    [SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode = true)]
    internal SafeFindHandle()
        : base(true)
    {
    }

    protected override bool ReleaseHandle()
    {
        // Close the search handle.
        return NativeMethods.FindClose(base.handle);
    }
}