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

namespace Microsoft.JScript {
    
    using System;

    public class LenientStringConstructor : StringConstructor{
      public new Object fromCharCode;
      
      internal LenientStringConstructor(LenientFunctionPrototype parent, LenientStringPrototype prototypeProp)
        : base(parent, prototypeProp) {
        this.noExpando = false;
        Type super = typeof(StringConstructor);
        this.fromCharCode = new BuiltinFunction("fromCharCode", this, super.GetMethod("fromCharCode"), parent);
      }
    }
}
