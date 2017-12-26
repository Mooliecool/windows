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

    public sealed class LenientEnumeratorPrototype : EnumeratorPrototype{
      public new Object constructor;
      public new Object atEnd;
      public new Object item;
      public new Object moveFirst;
      public new Object moveNext;
      
      internal LenientEnumeratorPrototype(LenientFunctionPrototype funcprot, LenientObjectPrototype parent)
        : base(parent) {
        this.noExpando = false;
        //this.constructor is given a value by the constructor class
        Type super = typeof(EnumeratorPrototype);
        this.atEnd = new BuiltinFunction("atEnd", this, super.GetMethod("atEnd"), funcprot);
        this.item = new BuiltinFunction("item", this, super.GetMethod("item"), funcprot);
        this.moveFirst = new BuiltinFunction("moveFirst", this, super.GetMethod("moveFirst"), funcprot);
        this.moveNext = new BuiltinFunction("moveNext", this, super.GetMethod("moveNext"), funcprot);
      }
    }
}
