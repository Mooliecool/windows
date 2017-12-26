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
using System;
using System.Runtime.InteropServices;

namespace System.Runtime.CompilerServices {
    [Serializable, AttributeUsage(AttributeTargets.Struct, Inherited = true),
     System.Runtime.InteropServices.ComVisible(true)]
    public sealed class NativeCppClassAttribute : Attribute
    {
        public NativeCppClassAttribute () {}
    }
}
