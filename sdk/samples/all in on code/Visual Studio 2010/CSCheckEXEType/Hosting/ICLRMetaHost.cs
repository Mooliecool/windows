/****************************** Module Header ******************************\
* Module Name:  IClrMetaHost.cs
* Project:	    CSCheckEXEType
* Copyright (c) Microsoft Corporation.
* 
* 
* Provides methods that return a specific version of the common language 
* runtime (CLR) based on its version number, list all installed CLRs, list all 
* runtimes that are loaded in a specified process, discover the CLR version used 
* to compile an assembly, exit a process with a clean runtime shutdown, and query 
* legacy API binding.
*
* 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;

namespace CSCheckEXEType.Hosting
{
    [ComImport]
    [SecurityCritical]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Guid("D332DB9E-B9B3-4125-8207-A14884F53216")]
    public interface IClrMetaHost
    {
        [return: MarshalAs(UnmanagedType.Interface)]
        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        object GetRuntime(
            [In, MarshalAs(UnmanagedType.LPWStr)] string version, 
            [In, MarshalAs(UnmanagedType.LPStruct)] Guid interfaceId);

        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        void GetVersionFromFile(
            [In, MarshalAs(UnmanagedType.LPWStr)] string filePath, 
            [Out, MarshalAs(UnmanagedType.LPWStr)] StringBuilder buffer, 
            [In, Out, MarshalAs(UnmanagedType.U4)] ref uint bufferLength);

        [return: MarshalAs(UnmanagedType.Interface)]
        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        IEnumUnknown EnumerateInstalledRuntimes();

        [return: MarshalAs(UnmanagedType.Interface)]
        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        IEnumUnknown EnumerateLoadedRuntimes([In] IntPtr processHandle);

        [PreserveSig, MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        int Reserved01([In] IntPtr reserved1);
    }
}
