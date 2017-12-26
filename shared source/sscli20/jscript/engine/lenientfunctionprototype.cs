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

    public sealed class LenientFunctionPrototype : FunctionPrototype{
      public new Object constructor;
      public new Object apply;
      public new Object call;
      public new Object toString;
      
      internal LenientFunctionPrototype(ScriptObject parent)
        : base(parent) {
        this.noExpando = false;
        //this.constructor is given a value by ObjectConstructor
        Type super = typeof(FunctionPrototype);
        this.apply = new BuiltinFunction("apply", this, super.GetMethod("apply"), this);
        this.call = new BuiltinFunction("call", this, super.GetMethod("call"), this);
        this.toString = new BuiltinFunction("toString", this, super.GetMethod("toString"), this);
      }
    }
}
