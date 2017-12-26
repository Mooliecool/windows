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
** Class: ParameterizedThreadStart
**
**
** Purpose: This class is a Delegate which defines the start method
**  for starting a thread.  That method must match this delegate.
**
**
=============================================================================*/


namespace System.Threading {
    using System.Security.Permissions;
    using System.Threading;
    using System.Runtime.InteropServices;

    [ComVisibleAttribute(false)]
    public delegate void ParameterizedThreadStart(object obj);
}
