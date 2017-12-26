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
** File: ThreadAttributes.cs
**
** Author: 
**
** Purpose: For Threads-related custom attributes.
**
** Date: July, 2000
**
=============================================================================*/


namespace System {
    [AttributeUsage (AttributeTargets.Method)]  
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class STAThreadAttribute : Attribute
    {
        public STAThreadAttribute()
        {
        }
    }

    [AttributeUsage (AttributeTargets.Method)]  
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class MTAThreadAttribute : Attribute
    {
        public MTAThreadAttribute()
        {
        }
    }
}
