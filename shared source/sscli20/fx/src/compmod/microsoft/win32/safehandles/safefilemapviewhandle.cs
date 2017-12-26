// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
/*============================================================
**
** Class:  SafeFileMapViewHandle
**
**                                                     
**
** A wrapper for handles returned from MapViewOfFile, used
** for shared memory.
**
** Date:  August 7, 2002
** 
===========================================================*/

using System;
using System.Security;
using System.Security.Permissions;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using Microsoft.Win32;
using Microsoft.Win32.SafeHandles;
using System.Runtime.ConstrainedExecution;

namespace Microsoft.Win32.SafeHandles {
    [HostProtectionAttribute(MayLeakOnAbort = true)]
    [SuppressUnmanagedCodeSecurityAttribute]
    internal sealed class SafeFileMapViewHandle : SafeHandleZeroOrMinusOneIsInvalid
    { 
        // Note that MapViewOfFile returns 0 on failure

        internal SafeFileMapViewHandle() : base(true) {}
        
        [DllImport(ExternDll.Kernel32, ExactSpelling=true, CharSet=CharSet.Auto)]
        internal static extern SafeFileMapViewHandle MapViewOfFile(SafeFileMappingHandle hFileMappingObject, int dwDesiredAccess, int dwFileOffsetHigh, int dwFileOffsetLow, UIntPtr dwNumberOfBytesToMap);

        [DllImport(ExternDll.Kernel32, ExactSpelling=true, SetLastError=true)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        private static extern bool UnmapViewOfFile(IntPtr handle);

        override protected bool ReleaseHandle()
        {
            return UnmapViewOfFile(handle);
        }

    }
}
