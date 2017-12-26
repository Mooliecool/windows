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
** File:    AssemblyHashAlgorithm
**
**
** Purpose: 
**
**
===========================================================*/
namespace System.Configuration.Assemblies {
    
    using System;
    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum AssemblyHashAlgorithm
    {
        None        = 0,
        MD5         = 0x8003,
        SHA1        = 0x8004
    }
}
