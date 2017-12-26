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

    public sealed class LenientVBArrayPrototype : VBArrayPrototype{
      public new Object constructor;
      public new Object dimensions;
      public new Object getItem;
      public new Object lbound;
      public new Object toArray;
      public new Object ubound;
      
      internal LenientVBArrayPrototype(LenientFunctionPrototype funcprot, LenientObjectPrototype parent)
        : base(funcprot, parent) {
        this.noExpando = false;
        //this.constructor is given a value by the constructor class
        Type super = typeof(VBArrayPrototype);
        this.dimensions = new BuiltinFunction("dimensions", this, super.GetMethod("dimensions"), funcprot);
        this.getItem = new BuiltinFunction("getItem", this, super.GetMethod("getItem"), funcprot);
        this.lbound = new BuiltinFunction("lbound", this, super.GetMethod("lbound"), funcprot);
        this.toArray = new BuiltinFunction("toArray", this, super.GetMethod("toArray"), funcprot);
        this.ubound = new BuiltinFunction("ubound", this, super.GetMethod("ubound"), funcprot);
      }
    }
}
