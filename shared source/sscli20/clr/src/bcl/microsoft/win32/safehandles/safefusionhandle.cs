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
** Class:  SafeFusionHandle 
**
** A wrapper for fusion handles
**
** Date:  July 8, 2002
** 
===========================================================*/

using System;
using System.Security;
using System.Security.Permissions;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using System.Runtime.CompilerServices;
using System.Runtime.ConstrainedExecution;

namespace Microsoft.Win32.SafeHandles {

    internal sealed class SafeFusionHandle : SafeHandleZeroOrMinusOneIsInvalid {

        [SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode=true)]
        internal SafeFusionHandle() : base(true) {}
        

        [SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode=true)]
        internal SafeFusionHandle (IntPtr preexistingHandle, bool ownshandle) : base(ownshandle) {
            SetHandle(preexistingHandle);
        }

        internal static SafeFusionHandle InvalidHandle {
            get { return new SafeFusionHandle(IntPtr.Zero, true); }
        }
        
        [MethodImplAttribute(MethodImplOptions.InternalCall),
         ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        private static extern void ReleaseFusionHandle(IntPtr pp);

        override protected bool ReleaseHandle()
        {
            ReleaseFusionHandle(handle);
            return true;
        }
    }
}


