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
** Class:  SafeMemoryHandle
**
**
** This class is wrapper for native (unsafe) memory pointer  
** allocated using Marshal.AllocHGlobal.
**
** 
===========================================================*/
using System;
using System.Security;
using System.Security.Permissions;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Runtime.ConstrainedExecution;
using Microsoft.Win32;
using Microsoft.Win32.SafeHandles;

namespace Microsoft.Win32.SafeHandles
{
    [SuppressUnmanagedCodeSecurityAttribute]
    internal sealed class SafeNativeMemoryHandle : SafeHandleZeroOrMinusOneIsInvalid
    {
        [SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode=true)]
        internal SafeNativeMemoryHandle() : base(true) {}

        [SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode=true)]
        internal SafeNativeMemoryHandle(IntPtr handle, bool ownsHandle) : base (ownsHandle)
        {
            SetHandle(handle);
        }

        override protected bool ReleaseHandle()
        {
            if (handle != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(handle);
                handle = IntPtr.Zero;
                return true;
            }
            return false;
        }
    }
}


