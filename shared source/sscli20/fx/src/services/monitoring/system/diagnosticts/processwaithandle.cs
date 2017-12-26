//------------------------------------------------------------------------------
// <copyright file="ProcessWaitHandle.cs" company="Microsoft">
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

using System;
using System.Threading;
using Microsoft.Win32;
using Microsoft.Win32.SafeHandles;
using System.Runtime.InteropServices;

namespace System.Diagnostics {
    internal class ProcessWaitHandle : WaitHandle {

        internal ProcessWaitHandle( SafeProcessHandle processHandle): base() {
            SafeWaitHandle waitHandle = null;
            bool succeeded = NativeMethods.DuplicateHandle(
                new HandleRef(this, NativeMethods.GetCurrentProcess()),
                processHandle,
                new HandleRef(this, NativeMethods.GetCurrentProcess()),
                out waitHandle,
                0,
                false,
                NativeMethods.DUPLICATE_SAME_ACCESS);
                    
            if (!succeeded) {                    
                Marshal.ThrowExceptionForHR(Marshal.GetHRForLastWin32Error());
            }

            this.SafeWaitHandle = waitHandle;         
        }
    }
}
