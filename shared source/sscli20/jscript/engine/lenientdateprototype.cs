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

    public sealed class LenientDatePrototype : DatePrototype{  
      public new Object constructor;
      public new Object getTime;
      [NotRecommended ("getYear")]
      public new Object getYear;
      public new Object getFullYear;
      public new Object getUTCFullYear;
      public new Object getMonth;
      public new Object getUTCMonth;
      public new Object getDate;
      public new Object getUTCDate;
      public new Object getDay;
      public new Object getUTCDay;
      public new Object getHours;
      public new Object getUTCHours;
      public new Object getMinutes;
      public new Object getUTCMinutes;
      public new Object getSeconds;
      public new Object getUTCSeconds;
      public new Object getMilliseconds;
      public new Object getUTCMilliseconds;
      public new Object getVarDate;
      public new Object getTimezoneOffset;
      public new Object setTime;
      public new Object setMilliseconds;
      public new Object setUTCMilliseconds;
      public new Object setSeconds;
      public new Object setUTCSeconds;
      public new Object setMinutes;
      public new Object setUTCMinutes;
      public new Object setHours;
      public new Object setUTCHours;
      public new Object setDate;
      public new Object setUTCDate;
      public new Object setMonth;
      public new Object setUTCMonth;
      public new Object setFullYear;
      public new Object setUTCFullYear;
      [NotRecommended ("setYear")]
      public new Object setYear;
      [NotRecommended ("toGMTString")]
      public new Object toGMTString;
      public new Object toDateString;
      public new Object toLocaleDateString;
      public new Object toLocaleString;
      public new Object toLocaleTimeString;
      public new Object toString;
      public new Object toTimeString;
      public new Object toUTCString;
      public new Object valueOf;
      
      internal LenientDatePrototype(LenientFunctionPrototype funcprot, LenientObjectPrototype parent)
        : base(parent) {
        this.noExpando = false;
    
        //this.constructor is given a value by the proper constructor class
        Type super = typeof(DatePrototype);
        this.getTime = new BuiltinFunction("getTime", this, super.GetMethod("getTime"), funcprot);
        this.getYear = new BuiltinFunction("getYear", this, super.GetMethod("getYear"), funcprot);
        this.getFullYear = new BuiltinFunction("getFullYear", this, super.GetMethod("getFullYear"), funcprot);
        this.getUTCFullYear = new BuiltinFunction("getUTCFullYear", this, super.GetMethod("getUTCFullYear"), funcprot);
        this.getMonth = new BuiltinFunction("getMonth", this, super.GetMethod("getMonth"), funcprot);
        this.getUTCMonth = new BuiltinFunction("getUTCMonth", this, super.GetMethod("getUTCMonth"), funcprot);
        this.getDate = new BuiltinFunction("getDate", this, super.GetMethod("getDate"), funcprot);
        this.getUTCDate = new BuiltinFunction("getUTCDate", this, super.GetMethod("getUTCDate"), funcprot);
        this.getDay = new BuiltinFunction("getDay", this, super.GetMethod("getDay"), funcprot);
        this.getUTCDay = new BuiltinFunction("getUTCDay", this, super.GetMethod("getUTCDay"), funcprot);
        this.getHours = new BuiltinFunction("getHours", this, super.GetMethod("getHours"), funcprot);
        this.getUTCHours = new BuiltinFunction("getUTCHours", this, super.GetMethod("getUTCHours"), funcprot);
        this.getMinutes = new BuiltinFunction("getMinutes", this, super.GetMethod("getMinutes"), funcprot);
        this.getUTCMinutes = new BuiltinFunction("getUTCMinutes", this, super.GetMethod("getUTCMinutes"), funcprot);
        this.getSeconds = new BuiltinFunction("getSeconds", this, super.GetMethod("getSeconds"), funcprot);
        this.getUTCSeconds = new BuiltinFunction("getUTCSeconds", this, super.GetMethod("getUTCSeconds"), funcprot);
        this.getMilliseconds = new BuiltinFunction("getMilliseconds", this, super.GetMethod("getMilliseconds"), funcprot);
        this.getUTCMilliseconds = new BuiltinFunction("getUTCMilliseconds", this, super.GetMethod("getUTCMilliseconds"), funcprot);
        this.getVarDate = new BuiltinFunction("getVarDate", this, super.GetMethod("getVarDate"), funcprot);
        this.getTimezoneOffset = new BuiltinFunction("getTimezoneOffset", this, super.GetMethod("getTimezoneOffset"), funcprot);
        this.setTime = new BuiltinFunction("setTime", this, super.GetMethod("setTime"), funcprot);
        this.setMilliseconds = new BuiltinFunction("setMilliseconds", this, super.GetMethod("setMilliseconds"), funcprot);
        this.setUTCMilliseconds = new BuiltinFunction("setUTCMilliseconds", this, super.GetMethod("setUTCMilliseconds"), funcprot);
        this.setSeconds = new BuiltinFunction("setSeconds", this, super.GetMethod("setSeconds"), funcprot);
        this.setUTCSeconds = new BuiltinFunction("setUTCSeconds", this, super.GetMethod("setUTCSeconds"), funcprot);
        this.setMinutes = new BuiltinFunction("setMinutes", this, super.GetMethod("setMinutes"), funcprot);
        this.setUTCMinutes = new BuiltinFunction("setUTCMinutes", this, super.GetMethod("setUTCMinutes"), funcprot);
        this.setHours = new BuiltinFunction("setHours", this, super.GetMethod("setHours"), funcprot);
        this.setUTCHours = new BuiltinFunction("setUTCHours", this, super.GetMethod("setUTCHours"), funcprot);
        this.setDate = new BuiltinFunction("setDate", this, super.GetMethod("setDate"), funcprot);
        this.setUTCDate = new BuiltinFunction("setUTCDate", this, super.GetMethod("setUTCDate"), funcprot);
        this.setMonth = new BuiltinFunction("setMonth", this, super.GetMethod("setMonth"), funcprot);
        this.setUTCMonth = new BuiltinFunction("setUTCMonth", this, super.GetMethod("setUTCMonth"), funcprot);
        this.setFullYear = new BuiltinFunction("setFullYear", this, super.GetMethod("setFullYear"), funcprot);
        this.setUTCFullYear = new BuiltinFunction("setUTCFullYear", this, super.GetMethod("setUTCFullYear"), funcprot);
        this.setYear = new BuiltinFunction("setYear", this, super.GetMethod("setYear"), funcprot);
        this.toDateString = new BuiltinFunction("toDateString", this, super.GetMethod("toDateString"), funcprot);
        this.toLocaleDateString = new BuiltinFunction("toLocaleDateString", this, super.GetMethod("toLocaleDateString"), funcprot);
        this.toLocaleString = new BuiltinFunction("toLocaleString", this, super.GetMethod("toLocaleString"), funcprot);
        this.toLocaleTimeString = new BuiltinFunction("toLocaleTimeString", this, super.GetMethod("toLocaleTimeString"), funcprot);
        this.toGMTString = new BuiltinFunction("toUTCString", this, super.GetMethod("toUTCString"), funcprot);
        this.toString = new BuiltinFunction("toString", this, super.GetMethod("toString"), funcprot);
        this.toTimeString = new BuiltinFunction("toTimeString", this, super.GetMethod("toTimeString"), funcprot);
        this.toUTCString = new BuiltinFunction("toUTCString", this, super.GetMethod("toUTCString"), funcprot);
        this.valueOf = new BuiltinFunction("valueOf", this, super.GetMethod("valueOf"), funcprot);
      }
    }
}
