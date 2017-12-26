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
** Class:  SafeViewOfFileHandle 
**
**
** A wrapper for file handles
**
** 
===========================================================*/
using System;
using System.Security;
using System.Security.Permissions;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Runtime.ConstrainedExecution;
using System.Runtime.Versioning;
using Microsoft.Win32;
using Microsoft.Win32.SafeHandles;

namespace Microsoft.Win32.SafeHandles
{
    internal sealed class SafeViewOfFileHandle : SafeHandleZeroOrMinusOneIsInvalid
    {
        [SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode=true)]
        internal SafeViewOfFileHandle() : base(true) {}

        // 0 is an Invalid Handle
        [SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode=true)]
        internal SafeViewOfFileHandle(IntPtr handle, bool ownsHandle) : base (ownsHandle) {
            SetHandle(handle);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        override protected bool ReleaseHandle()
        {
            if (Win32Native.UnmapViewOfFile(handle))
            {
                handle = IntPtr.Zero;
                return true;
            }
            
            return false;
        }
    }
}

