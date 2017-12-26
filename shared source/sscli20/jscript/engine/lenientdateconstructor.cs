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

    public sealed class LenientDateConstructor : DateConstructor{
      public new Object parse;
      public new Object UTC;
      
      internal LenientDateConstructor(LenientFunctionPrototype parent, LenientDatePrototype prototypeProp)
        : base(parent, prototypeProp) {
        this.noExpando = false;
        Type super = typeof(DateConstructor);
        this.parse = new BuiltinFunction("parse", this, super.GetMethod("parse"), parent);
        this.UTC = new BuiltinFunction("UTC", this, super.GetMethod("UTC"), parent);
      }
    }
}
