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

    public sealed class LenientMathObject : MathObject{
      public new const double E = 2.7182818284590452354;
      public new const double LN10 = 2.302585092994046;
      public new const double LN2 = 0.6931471805599453;
      public new const double LOG2E = 1.4426950408889634;
      public new const double LOG10E = 0.4342944819032518;
      public new const double PI = 3.14159265358979323846;
      public new const double SQRT1_2 = 0.7071067811865476;
      public new const double SQRT2 = 1.4142135623730951;
      
      public new Object abs;
      public new Object acos;
      public new Object asin;
      public new Object atan;
      public new Object atan2;
      public new Object ceil;
      public new Object cos;
      public new Object exp;
      public new Object floor;
      public new Object log;
      public new Object max;
      public new Object min;
      public new Object pow;
      public new Object random;
      public new Object round;
      public new Object sin;
      public new Object sqrt;
      public new Object tan;
    
      internal LenientMathObject(ScriptObject parent, FunctionPrototype funcprot)
        : base(parent) {
        this.noExpando = false;
        Type super = typeof(MathObject);
    	  this.abs = new BuiltinFunction("abs", this, super.GetMethod("abs"), funcprot);
    	  this.acos = new BuiltinFunction("acos", this, super.GetMethod("acos"), funcprot);
    	  this.asin = new BuiltinFunction("asin", this, super.GetMethod("asin"), funcprot);
    	  this.atan = new BuiltinFunction("atan", this, super.GetMethod("atan"), funcprot);
    	  this.atan2 = new BuiltinFunction("atan2", this, super.GetMethod("atan2"), funcprot);
    	  this.ceil = new BuiltinFunction("ceil", this, super.GetMethod("ceil"), funcprot);
    	  this.cos = new BuiltinFunction("cos", this, super.GetMethod("cos"), funcprot);
    	  this.exp = new BuiltinFunction("exp", this, super.GetMethod("exp"), funcprot);
    	  this.floor = new BuiltinFunction("floor", this, super.GetMethod("floor"), funcprot);
    	  this.log = new BuiltinFunction("log", this, super.GetMethod("log"), funcprot);
    	  this.max = new BuiltinFunction("max", this, super.GetMethod("max"), funcprot);
    	  this.min = new BuiltinFunction("min", this, super.GetMethod("min"), funcprot);
    	  this.pow = new BuiltinFunction("pow", this, super.GetMethod("pow"), funcprot);
    	  this.random = new BuiltinFunction("random", this, super.GetMethod("random"), funcprot);
    	  this.round = new BuiltinFunction("round", this, super.GetMethod("round"), funcprot);
    	  this.sin = new BuiltinFunction("sin", this, super.GetMethod("sin"), funcprot);
    	  this.sqrt = new BuiltinFunction("sqrt", this, super.GetMethod("sqrt"), funcprot);
    	  this.tan = new BuiltinFunction("tan", this, super.GetMethod("tan"), funcprot);
      }
    }
}
