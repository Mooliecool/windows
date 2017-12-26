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
** Class:  CriticalFinalizerObject
**
**
** Deriving from this class will cause any finalizer you define to be critical
** (i.e. the finalizer is guaranteed to run, won't be aborted by the host and is
** run after the finalizers of other objects collected at the same time).
**
** You must possess UnmanagedCode permission in order to derive from this class.
**
** 
===========================================================*/

using System;
using System.Security.Permissions;
using System.Runtime.InteropServices;

namespace System.Runtime.ConstrainedExecution
{

    [SecurityPermission(SecurityAction.InheritanceDemand, UnmanagedCode=true)]
    [System.Runtime.InteropServices.ComVisible(true)]
    public abstract class CriticalFinalizerObject
    {
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
        protected CriticalFinalizerObject()
        {
        }

        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        ~CriticalFinalizerObject()
        {
        }
    }
}
