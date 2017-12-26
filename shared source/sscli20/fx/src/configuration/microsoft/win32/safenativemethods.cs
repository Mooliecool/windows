//------------------------------------------------------------------------------
// <copyright file="SafeNativeMethods.cs" company="Microsoft">
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
    using System;
    using System.Security.Permissions;
    using System.Collections;
    using System.IO;
    using System.Text;
    using System.Threading;
    using Microsoft.Win32.SafeHandles;    

    [ 
    System.Security.SuppressUnmanagedCodeSecurityAttribute()
    ]
    internal static class SafeNativeMethods {
#if NOPERF
        [DllImport(ExternDll.Kernel32, SetLastError=true)]
        internal static extern bool QueryPerformanceCounter(out long value);
        
        [DllImport(ExternDll.Kernel32, SetLastError=true)]
        internal static extern bool QueryPerformanceFrequency(out long value);
#endif
    }
}
