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
//
// Abstract derivations of SafeHandle designed to provide the common
// functionality supporting Win32 handles. More specifically, they describe how
// an invalid handle looks (for instance, some handles use -1 as an invalid
// handle value, others use 0).
//
// Further derivations of these classes can specialise this even further (e.g.
// file or registry handles).
// 
//

using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Security.Permissions;
using System.Runtime.ConstrainedExecution;

namespace Microsoft.Win32.SafeHandles
{
    // Class of safe handle which uses 0 or -1 as an invalid handle.
    [SecurityPermission(SecurityAction.InheritanceDemand, UnmanagedCode=true)]
    [SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode=true)]
    public abstract class SafeHandleZeroOrMinusOneIsInvalid : SafeHandle
    {
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        protected SafeHandleZeroOrMinusOneIsInvalid(bool ownsHandle) : base(IntPtr.Zero, ownsHandle) 
        {
        }

        public override bool IsInvalid {
            get { return handle.IsNull() || handle == new IntPtr(-1); }
        }
    }

    // Class of safe handle which uses only -1 as an invalid handle.
    [SecurityPermission(SecurityAction.InheritanceDemand, UnmanagedCode=true)]
    [SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode=true)]
    public abstract class SafeHandleMinusOneIsInvalid : SafeHandle
    {
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        protected SafeHandleMinusOneIsInvalid(bool ownsHandle) : base(new IntPtr(-1), ownsHandle) 
        {
        }

        public override bool IsInvalid {
            get { return handle == new IntPtr(-1); }
        }
    }

    // Class of critical handle which uses 0 or -1 as an invalid handle.
    [SecurityPermission(SecurityAction.InheritanceDemand, UnmanagedCode=true)]
    [SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode=true)]
    public abstract class CriticalHandleZeroOrMinusOneIsInvalid : CriticalHandle
    {
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        protected CriticalHandleZeroOrMinusOneIsInvalid() : base(IntPtr.Zero) 
        {
        }

        public override bool IsInvalid {
            get { return handle.IsNull() || handle == new IntPtr(-1); }
        }
    }

    // Class of critical handle which uses only -1 as an invalid handle.
    [SecurityPermission(SecurityAction.InheritanceDemand, UnmanagedCode=true)]
    [SecurityPermission(SecurityAction.LinkDemand, UnmanagedCode=true)]
    public abstract class CriticalHandleMinusOneIsInvalid : CriticalHandle
    {
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        protected CriticalHandleMinusOneIsInvalid() : base(new IntPtr(-1)) 
        {
        }

        public override bool IsInvalid {
            get { return handle == new IntPtr(-1); }
        }
    }

}
