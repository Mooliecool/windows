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

    public sealed class LenientNumberPrototype : NumberPrototype{
      public new Object constructor;
      public new Object toExponential;
      public new Object toFixed;
      public new Object toLocaleString;
      public new Object toPrecision;
      public new Object toString;
      public new Object valueOf;
     
      internal LenientNumberPrototype(LenientFunctionPrototype funcprot, LenientObjectPrototype parent)
        : base(parent) {
        this.noExpando = false;
        //this.constructor is given a value by the proper constructor class
        Type super = typeof(NumberPrototype);
        this.toExponential = new BuiltinFunction("toExponential", this, super.GetMethod("toExponential"), funcprot);
        this.toFixed = new BuiltinFunction("toFixed", this, super.GetMethod("toFixed"), funcprot);
        this.toLocaleString = new BuiltinFunction("toLocaleString", this, super.GetMethod("toLocaleString"), funcprot);
        this.toPrecision = new BuiltinFunction("toPrecision", this, super.GetMethod("toPrecision"), funcprot);
        this.toString = new BuiltinFunction("toString", this, super.GetMethod("toString"), funcprot);
        this.valueOf = new BuiltinFunction("valueOf", this, super.GetMethod("valueOf"), funcprot);
      }
    }
}
