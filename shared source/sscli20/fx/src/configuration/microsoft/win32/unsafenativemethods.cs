//------------------------------------------------------------------------------
// <copyright file="UnsafeNativeMethods.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------

namespace Microsoft.Win32 {
    using System.Runtime.InteropServices;
    using System.Threading;
    using System;
    using System.Security.Permissions;
    using System.Collections;
    using System.IO;
    using System.Text;
    using Microsoft.Win32.SafeHandles;
    using System.Configuration;

    [
    System.Security.SuppressUnmanagedCodeSecurityAttribute()
    ]
    internal static class UnsafeNativeMethods {
        [DllImport(ExternDll.Kernel32, SetLastError=true, CharSet=CharSet.Auto, BestFitMapping=false)]
        internal static extern bool GetFileAttributesEx(string name, int fileInfoLevel, out WIN32_FILE_ATTRIBUTE_DATA data);

        internal const int GetFileExInfoStandard = 0;

        [StructLayout(LayoutKind.Sequential)]
        internal struct WIN32_FILE_ATTRIBUTE_DATA {
            internal int fileAttributes;
            internal uint ftCreationTimeLow;
            internal uint ftCreationTimeHigh;
            internal uint ftLastAccessTimeLow;
            internal uint ftLastAccessTimeHigh;
            internal uint ftLastWriteTimeLow;
            internal uint ftLastWriteTimeHigh;
            internal uint fileSizeHigh;
            internal uint fileSizeLow;
        }

        [DllImport(ExternDll.Kernel32, CharSet=CharSet.Auto, BestFitMapping=false)]
        internal static extern int GetModuleFileName(HandleRef hModule, StringBuilder buffer, int length);


        // MoveFile Parameter
        internal const int MOVEFILE_REPLACE_EXISTING = 0x00000001;

        [DllImport(ExternDll.Kernel32, CharSet=CharSet.Auto, BestFitMapping=false)]
        internal static extern bool   MoveFileEx(string lpExistingFileName, string lpNewFileName, int dwFlags);
    }
}
