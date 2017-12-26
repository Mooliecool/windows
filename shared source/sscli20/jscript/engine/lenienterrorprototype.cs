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

    public sealed class LenientErrorPrototype : ErrorPrototype{
      public new Object constructor;
      public new Object name;
      public new Object toString;
      
      internal LenientErrorPrototype(LenientFunctionPrototype funcprot, ScriptObject parent, String name)
        : base(parent, name) {
        this.noExpando = false;
        //this.constructor is given a value by the constructor class
        this.name = name;
        Type super = typeof(ErrorPrototype);
        this.toString = new BuiltinFunction("toString", this, super.GetMethod("toString"), funcprot);
      }
    
    }
}
