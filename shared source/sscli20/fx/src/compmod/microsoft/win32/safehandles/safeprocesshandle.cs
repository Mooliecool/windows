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
** Class:  SafeProcessHandle 
**
** A wrapper for a process handle
**
** 
===========================================================*/

using System;
using System.Security;
using System.Diagnostics;
using System.Security.Permissions;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using Microsoft.Win32;
using Microsoft.Win32.SafeHandles;
using System.Runtime.ConstrainedExecution;

namespace Microsoft.Win32.SafeHandles {
    [HostProtectionAttribute(MayLeakOnAbort = true)]
    [SuppressUnmanagedCodeSecurityAttribute]
    internal sealed class SafeProcessHandle : SafeHandleZeroOrMinusOneIsInvalid
    { 
        internal static SafeProcessHandle InvalidHandle = new SafeProcessHandle(IntPtr.Zero); 
    
        // Note that OpenProcess returns 0 on failure

        internal SafeProcessHandle() : base(true) {} 

        [SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode=true)]        
        internal SafeProcessHandle(IntPtr handle) : base (true) {
            SetHandle(handle);
        }
        
        // Not currently called
        //[SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode=true)]
        //internal SafeProcessHandle(IntPtr existingHandle, bool ownsHandle) : base(ownsHandle) {
        //    SetHandle(existingHandle);
        //}

        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        internal static extern SafeProcessHandle OpenProcess(int access, bool inherit, int processId);

        
        internal void InitialSetHandle(IntPtr h){
            Debug.Assert(base.IsInvalid, "Safe handle should only be set once");
            base.handle = h;
        }
        
        override protected bool ReleaseHandle()
        {
            return SafeNativeMethods.CloseHandle(handle);
        }

    }
}




