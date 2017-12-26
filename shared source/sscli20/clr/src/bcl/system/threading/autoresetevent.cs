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
/*=============================================================================
**
** Class: AutoResetEvent	
**
**
** Purpose: An example of a WaitHandle class
**
**
=============================================================================*/
namespace System.Threading {
    
    using System;
    using System.Security.Permissions;
    using System.Runtime.InteropServices;

    [HostProtection(Synchronization=true, ExternalThreading=true)]
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class AutoResetEvent : EventWaitHandle
    {
        public AutoResetEvent(bool initialState) : base(initialState,EventResetMode.AutoReset){ }
    }
}
    
