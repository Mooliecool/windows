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

    public sealed class LenientArrayPrototype : ArrayPrototype{  
      public new Object constructor;
      public new Object concat;
      public new Object join;
      public new Object pop;
      public new Object push;
      public new Object reverse;
      public new Object shift;
      public new Object slice;
      public new Object sort;
      public new Object splice;
      public new Object unshift;
      public new Object toLocaleString;
      public new Object toString;
      
      internal LenientArrayPrototype(FunctionPrototype funcprot, ObjectPrototype parent)
        : base(parent) {
        this.noExpando = false;
        Type super = typeof(ArrayPrototype);
        //this.constructor is given a value by the proper constructor class
        this.concat = new BuiltinFunction("concat", this, super.GetMethod("concat"), funcprot);
        this.join = new BuiltinFunction("join", this, super.GetMethod("join"), funcprot);
        this.pop = new BuiltinFunction("pop", this, super.GetMethod("pop"), funcprot);
        this.push = new BuiltinFunction("push", this, super.GetMethod("push"), funcprot);
        this.reverse = new BuiltinFunction("reverse", this, super.GetMethod("reverse"), funcprot);
        this.shift = new BuiltinFunction("shift", this, super.GetMethod("shift"), funcprot);
        this.slice = new BuiltinFunction("slice", this, super.GetMethod("slice"), funcprot);
        this.sort = new BuiltinFunction("sort", this, super.GetMethod("sort"), funcprot);
        this.splice = new BuiltinFunction("splice", this, super.GetMethod("splice"), funcprot);
        this.unshift = new BuiltinFunction("unshift", this, super.GetMethod("unshift"), funcprot);
        this.toLocaleString = new BuiltinFunction("toLocaleString", this, super.GetMethod("toLocaleString"), funcprot);
        this.toString = new BuiltinFunction("toString", this, super.GetMethod("toString"), funcprot);
      }
    }
}
