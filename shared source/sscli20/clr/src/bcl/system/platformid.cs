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
** File:    PlatformID
**
**
** Purpose: Defines IDs for supported platforms
**
**
===========================================================*/
namespace System {

    [Serializable()]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum PlatformID
    {
        Win32S        = 0,
        Win32Windows  = 1,
        Win32NT       = 2,

        WinCE         = 3,
        
        Unix          = 4
    }
}
