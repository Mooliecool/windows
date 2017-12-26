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

    public sealed class LenientRegExpPrototype : RegExpPrototype{
      public new Object constructor;
      public new Object compile;
      public new Object exec;
      public new Object test;
      public new Object toString;
      
      internal LenientRegExpPrototype(LenientFunctionPrototype funcprot, LenientObjectPrototype parent)
        : base(parent) {
        this.noExpando = false;
        //this.constructor is given a value by the proper constructor class
        Type super = typeof(RegExpPrototype);
        this.compile = new BuiltinFunction("compile", this, super.GetMethod("compile"), funcprot);
        this.exec = new BuiltinFunction("exec", this, super.GetMethod("exec"), funcprot);
        this.test = new BuiltinFunction("test", this, super.GetMethod("test"), funcprot);
        this.toString = new BuiltinFunction("toString", this, super.GetMethod("toString"), funcprot);
      }
    }
}
