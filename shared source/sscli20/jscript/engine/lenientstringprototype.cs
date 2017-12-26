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

    public sealed class LenientStringPrototype : StringPrototype{
      public new Object constructor;
      public new Object anchor;
      public new Object big;
      public new Object blink;
      public new Object bold;
      public new Object charAt;
      public new Object charCodeAt;
      public new Object concat;
      public new Object @fixed;
      public new Object fontcolor;
      public new Object fontsize;
      public new Object indexOf;
      public new Object italics;
      public new Object lastIndexOf;
      public new Object link;
      public new Object localeCompare;
      public new Object match;
      public new Object replace;
      public new Object search;
      public new Object slice;
      public new Object small;
      public new Object split;
      public new Object strike;
      public new Object sub;
      [NotRecommended ("substr")]
      public new Object substr;
      public new Object substring;
      public new Object sup;
      public new Object toLocaleLowerCase;
      public new Object toLocaleUpperCase;
      public new Object toLowerCase;
      public new Object toString;
      public new Object toUpperCase;
      public new Object valueOf;
      
      internal LenientStringPrototype(LenientFunctionPrototype funcprot, LenientObjectPrototype parent)
        : base(funcprot, parent) {
        this.noExpando = false;
        
        //this.constructor is given a value by the proper constructor class
        Type super = typeof(StringPrototype);
        this.anchor = new BuiltinFunction("anchor", this, super.GetMethod("anchor"), funcprot);
        this.big = new BuiltinFunction("big", this, super.GetMethod("big"), funcprot);
        this.blink = new BuiltinFunction("blink", this, super.GetMethod("blink"), funcprot);
        this.bold = new BuiltinFunction("bold", this, super.GetMethod("bold"), funcprot);
        this.charAt = new BuiltinFunction("charAt", this, super.GetMethod("charAt"), funcprot);
        this.charCodeAt = new BuiltinFunction("charCodeAt", this, super.GetMethod("charCodeAt"), funcprot);
        this.concat = new BuiltinFunction("concat", this, super.GetMethod("concat"), funcprot);
        this.@fixed = new BuiltinFunction("fixed", this, super.GetMethod("fixed"), funcprot);
        this.fontcolor = new BuiltinFunction("fontcolor", this, super.GetMethod("fontcolor"), funcprot);
        this.fontsize = new BuiltinFunction("fontsize", this, super.GetMethod("fontsize"), funcprot);
        this.indexOf = new BuiltinFunction("indexOf", this, super.GetMethod("indexOf"), funcprot);
        this.italics = new BuiltinFunction("italics", this, super.GetMethod("italics"), funcprot);
        this.lastIndexOf = new BuiltinFunction("lastIndexOf", this, super.GetMethod("lastIndexOf"), funcprot);
        this.link = new BuiltinFunction("link", this, super.GetMethod("link"), funcprot);
        this.localeCompare = new BuiltinFunction("localeCompare", this, super.GetMethod("localeCompare"), funcprot);
        this.match = new BuiltinFunction("match", this, super.GetMethod("match"), funcprot);
        this.replace = new BuiltinFunction("replace", this, super.GetMethod("replace"), funcprot);
        this.search = new BuiltinFunction("search", this, super.GetMethod("search"), funcprot);
        this.slice = new BuiltinFunction("slice", this, super.GetMethod("slice"), funcprot);
        this.small = new BuiltinFunction("small", this, super.GetMethod("small"), funcprot);
        this.split = new BuiltinFunction("split", this, super.GetMethod("split"), funcprot);
        this.strike = new BuiltinFunction("strike", this, super.GetMethod("strike"), funcprot);
        this.sub = new BuiltinFunction("sub", this, super.GetMethod("sub"), funcprot);
        this.substr = new BuiltinFunction("substr", this, super.GetMethod("substr"), funcprot);
        this.substring = new BuiltinFunction("substring", this, super.GetMethod("substring"), funcprot);
        this.sup = new BuiltinFunction("sup", this, super.GetMethod("sup"), funcprot);
        this.toLocaleLowerCase = new BuiltinFunction("toLocaleLowerCase", this, super.GetMethod("toLocaleLowerCase"), funcprot);
        this.toLocaleUpperCase = new BuiltinFunction("toLocaleUpperCase", this, super.GetMethod("toLocaleUpperCase"), funcprot);
        this.toLowerCase = new BuiltinFunction("toLowerCase", this, super.GetMethod("toLowerCase"), funcprot);
        this.toString = new BuiltinFunction("toString", this, super.GetMethod("toString"), funcprot);
        this.toUpperCase = new BuiltinFunction("toUpperCase", this, super.GetMethod("toUpperCase"), funcprot);
        this.valueOf = new BuiltinFunction("valueOf", this, super.GetMethod("valueOf"), funcprot);
      }
    }
}
