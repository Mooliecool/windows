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
** File:    AssemblyVersionCompatibility
**
**                                     
**
** Purpose: defining the different flavor's assembly version compatibility
**
** Date:    June 4, 1999
**
===========================================================*/
namespace System.Configuration.Assemblies {
    
    using System;
     [Serializable()]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum AssemblyVersionCompatibility
    {
        SameMachine         = 1,
        SameProcess         = 2,
        SameDomain          = 3,
    }
}
