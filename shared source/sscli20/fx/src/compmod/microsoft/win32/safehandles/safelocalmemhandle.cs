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
** Class:  SafeLocalMemHandle
**
**                                                   
**
** A wrapper for handle to local memory
**
** Date:  July 8, 2002
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
    internal sealed class SafeLocalMemHandle : SafeHandleZeroOrMinusOneIsInvalid
    { 
        internal SafeLocalMemHandle() : base(true) {}
        
        [SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode=true)]
        internal SafeLocalMemHandle(IntPtr existingHandle, bool ownsHandle) : base(ownsHandle) {
            SetHandle(existingHandle);
        }
                

        [DllImport(ExternDll.Kernel32)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        private static extern IntPtr LocalFree(IntPtr hMem);

        override protected bool ReleaseHandle()
        {
            return LocalFree(handle) == IntPtr.Zero;
        }

    }
}





