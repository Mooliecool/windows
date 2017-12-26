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
using System.Reflection;
using System.Runtime.InteropServices;

namespace System.Runtime.CompilerServices 
{
    [AttributeUsage(AttributeTargets.Class | 
                    AttributeTargets.Method |
                    AttributeTargets.Property |
                    AttributeTargets.Field |
                    AttributeTargets.Event |
                    AttributeTargets.Struct)]

   
    public sealed class SpecialNameAttribute : Attribute
    {
        public SpecialNameAttribute() { }
    }
}



