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

namespace System.Reflection 
{
   
    using System;
    // This Enum matchs the CorMethodImpl defined in CorHdr.h
    [Serializable()] 
[System.Runtime.InteropServices.ComVisible(true)]
    public enum MethodImplAttributes
    {
        // code impl mask
        CodeTypeMask        =   0x0003,   // Flags about code type.   
        IL                  =   0x0000,   // Method impl is IL.
        Native              =   0x0001,   // Method impl is native.     
        /// <internalonly/>
        OPTIL               =   0x0002,   // Method impl is OPTIL 
        Runtime             =   0x0003,   // Method impl is provided by the runtime.
        // end code impl mask
    
        // managed mask
        ManagedMask       =   0x0004,   // Flags specifying whether the code is managed or unmanaged.
        Unmanaged         =   0x0004,   // Method impl is unmanaged, otherwise managed.
        Managed           =   0x0000,   // Method impl is managed.
        // end managed mask
    
        // implementation info and interop
        ForwardRef          =   0x0010,   // Indicates method is not defined; used primarily in merge scenarios.
        PreserveSig         =   0x0080,   // Indicates method sig is exported exactly as declared.

        InternalCall        =   0x1000,   // Internal Call...
    
        Synchronized      =   0x0020,   // Method is single threaded through the body.
        NoInlining        =   0x0008,   // Method may not be inlined.

        MaxMethodImplVal  = 0xFFFF,     // Range check value
    }
}
