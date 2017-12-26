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
** File:    AssemblyNameFlags
**
**
** Purpose: Flags controlling how an AssemblyName is used
**          during binding
**
**
===========================================================*/
namespace System.Reflection {
    
    using System;
    [Serializable, FlagsAttribute()]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum AssemblyNameFlags
    {
        None                      = 0x0000,
        // Flag used to indicate that an assembly ref contains the full public key, not the compressed token.
        // Must match afPublicKey in CorHdr.h.
        PublicKey                 = 0x0001,        
        //ProcArchMask              = 0x00F0,     // Bits describing the processor architecture
        					// Accessible via AssemblyName.ProcessorArchitecture
        EnableJITcompileOptimizer = 0x4000, 
        EnableJITcompileTracking  = 0x8000, 
        Retargetable              = 0x0100, 
    }

    [Serializable]
    [System.Runtime.InteropServices.ComVisible(true)]
    public enum ProcessorArchitecture
    {
        None                    = 0x0000,
        MSIL                    = 0x0001,
        X86                     = 0x0002,
        IA64                    = 0x0003,
        Amd64                   = 0x0004
    }
}
