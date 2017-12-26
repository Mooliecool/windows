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
** Class:  SafePEFileHandle 
**
**
** A wrapper for pefile pointers
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

namespace Microsoft.Win32.SafeHandles
{
    internal sealed class SafePEFileHandle: SafeHandleZeroOrMinusOneIsInvalid
    {
        // 0 is an Invalid Handle
        private SafePEFileHandle(IntPtr handle) : base (true)
        {
            SetHandle(handle);
        }

        internal static SafePEFileHandle InvalidHandle
        {
            get { return new SafePEFileHandle(IntPtr.Zero); }
        }

        override protected bool ReleaseHandle()
        {
            return true;
        }
    }
}

