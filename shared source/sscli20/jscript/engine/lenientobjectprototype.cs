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
    
    using Microsoft.JScript.Vsa;
    using System;

    public class LenientObjectPrototype : ObjectPrototype{
      public new Object constructor;
      public new Object hasOwnProperty;
      public new Object isPrototypeOf;
      public new Object propertyIsEnumerable;
      public new Object toLocaleString;
      public new Object toString;
      public new Object valueOf;
      
      internal LenientObjectPrototype(VsaEngine engine)
        : base() {
        this.engine = engine;
        this.noExpando = false;
        //this.constructor is given a value by the proper constructor class
      }
      
      internal void Initialize(LenientFunctionPrototype funcprot){
        Type super = typeof(ObjectPrototype);
        this.hasOwnProperty = new BuiltinFunction("hasOwnProperty", this, super.GetMethod("hasOwnProperty"), funcprot);
        this.isPrototypeOf = new BuiltinFunction("isPrototypeOf", this, super.GetMethod("isPrototypeOf"), funcprot);
        this.propertyIsEnumerable = new BuiltinFunction("propertyIsEnumerable", this, super.GetMethod("propertyIsEnumerable"), funcprot);
        this.toLocaleString = new BuiltinFunction("toLocaleString", this, super.GetMethod("toLocaleString"), funcprot);
        this.toString = new BuiltinFunction("toString", this, super.GetMethod("toString"), funcprot);
        this.valueOf = new BuiltinFunction("valueOf", this, super.GetMethod("valueOf"), funcprot);
      }  
    }
}
