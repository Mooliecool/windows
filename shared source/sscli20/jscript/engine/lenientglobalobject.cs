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
    using System.Reflection;
    using System.Collections;
    
    public sealed class LenientGlobalObject : GlobalObject{
      // properties
      public new Object Infinity;
      private Object MathField;
      public new Object NaN;
      public new Object undefined;
      // backing fields for constructor properties
      private Object ActiveXObjectField;
      private Object ArrayField;
      private Object BooleanField;
      private Object DateField;
      private Object EnumeratorField;
      private Object ErrorField;
      private Object EvalErrorField;
      private Object FunctionField;
      private Object NumberField;
      private Object ObjectField;
      private Object RangeErrorField;
      private Object ReferenceErrorField;
      private Object RegExpField;
      private Object StringField;
      private Object SyntaxErrorField;
      private Object TypeErrorField;
      private Object VBArrayField;
      private Object URIErrorField;

      // function properties
      public new Object decodeURI;
      public new Object decodeURIComponent;
      public new Object encodeURI;
      public new Object encodeURIComponent;
      [NotRecommended ("escape")]
      public new Object escape;
      public new Object eval;
      public new Object isNaN;
      public new Object isFinite;
      public new Object parseInt;
      public new Object parseFloat;
      public new Object GetObject;
      public new Object ScriptEngine;
      public new Object ScriptEngineBuildVersion;
      public new Object ScriptEngineMajorVersion;
      public new Object ScriptEngineMinorVersion;
      [NotRecommended ("unescape")]
      public new Object unescape;
      // built-in types
      public new Object boolean;
      public new Object @byte;
      public new Object @char;
      public new Object @decimal;
      public new Object @double;
      public new Object @float;
      public new Object @int;
      public new Object @long;
      public new Object @sbyte;
      public new Object @short;
      public new Object @void;
      public new Object @uint;
      public new Object @ulong;
      public new Object @ushort;

      //backing fields for private properties used to initialize orignalXXX properties
      private LenientArrayPrototype arrayPrototypeField;
      private LenientFunctionPrototype functionPrototypeField;
      private LenientObjectPrototype objectPrototypeField;
      private VsaEngine engine;
      
      internal LenientGlobalObject(VsaEngine engine){
        this.engine = engine;
        
        this.Infinity = Double.PositiveInfinity;
        this.NaN = Double.NaN;
        this.undefined = null;
    
        this.ActiveXObjectField = Missing.Value;
        this.ArrayField = Missing.Value;
        this.BooleanField = Missing.Value;
        this.DateField = Missing.Value;
        this.EnumeratorField = Missing.Value;
        this.ErrorField = Missing.Value;
        this.EvalErrorField = Missing.Value;
        this.FunctionField = Missing.Value;
        this.MathField = Missing.Value;
        this.NumberField = Missing.Value;
        this.ObjectField = Missing.Value;
        this.RangeErrorField = Missing.Value;
        this.ReferenceErrorField = Missing.Value;
        this.RegExpField = Missing.Value; 
        this.StringField = Missing.Value;
        this.SyntaxErrorField = Missing.Value;
        this.TypeErrorField = Missing.Value;
        this.VBArrayField = Missing.Value;
        this.URIErrorField = Missing.Value;

        Type super = typeof(GlobalObject);
        LenientFunctionPrototype fprot = this.functionPrototype;
        this.decodeURI = new BuiltinFunction("decodeURI", this, super.GetMethod("decodeURI"), fprot);
        this.decodeURIComponent = new BuiltinFunction("decodeURIComponent", this, super.GetMethod("decodeURIComponent"), fprot);
        this.encodeURI = new BuiltinFunction("encodeURI", this, super.GetMethod("encodeURI"), fprot);
        this.encodeURIComponent = new BuiltinFunction("encodeURIComponent", this, super.GetMethod("encodeURIComponent"), fprot);
        this.escape = new BuiltinFunction("escape", this, super.GetMethod("escape"), fprot);
        this.eval = new BuiltinFunction("eval", this, super.GetMethod("eval"), fprot);
        this.isNaN = new BuiltinFunction("isNaN", this, super.GetMethod("isNaN"), fprot);
        this.isFinite = new BuiltinFunction("isFinite", this, super.GetMethod("isFinite"), fprot);
        this.parseInt = new BuiltinFunction("parseInt", this, super.GetMethod("parseInt"), fprot);
        this.GetObject = new BuiltinFunction("GetObject", this, super.GetMethod("GetObject"), fprot);
        this.parseFloat = new BuiltinFunction("parseFloat", this, super.GetMethod("parseFloat"), fprot);
        this.ScriptEngine = new BuiltinFunction("ScriptEngine", this, super.GetMethod("ScriptEngine"), fprot); 
        this.ScriptEngineBuildVersion = new BuiltinFunction("ScriptEngineBuildVersion", this, super.GetMethod("ScriptEngineBuildVersion"), fprot);
        this.ScriptEngineMajorVersion = new BuiltinFunction("ScriptEngineMajorVersion", this, super.GetMethod("ScriptEngineMajorVersion"), fprot);
        this.ScriptEngineMinorVersion = new BuiltinFunction("ScriptEngineMinorVersion", this, super.GetMethod("ScriptEngineMinorVersion"), fprot);
        this.unescape = new BuiltinFunction("unescape", this, super.GetMethod("unescape"), fprot);
        
        this.boolean = Typeob.Boolean;
        this.@byte = Typeob.Byte;
        this.@char = Typeob.Char;
        this.@decimal = Typeob.Decimal;
        this.@double = Typeob.Double;
        this.@float = Typeob.Single;
        this.@int = Typeob.Int32;
        this.@long = Typeob.Int64;
        this.@sbyte = Typeob.SByte;
        this.@short = Typeob.Int16;
        this.@void = Typeob.Void;
        this.@uint = Typeob.UInt32;
        this.@ulong = Typeob.UInt64;
        this.@ushort = Typeob.UInt16;
      }
      
      private LenientArrayPrototype arrayPrototype{
        get{
          if (this.arrayPrototypeField == null)
            this.arrayPrototypeField = new LenientArrayPrototype(this.functionPrototype, this.objectPrototype);
          return this.arrayPrototypeField;
        }
      }
      
      private LenientFunctionPrototype functionPrototype{
        get{
          if (this.functionPrototypeField == null){
            Object junk = this.objectPrototype; //initialize functionPrototypeField indiretly because of circularity
          }
          return this.functionPrototypeField;
        }
      }
      
      private LenientObjectPrototype objectPrototype{
        get{
          if (this.objectPrototypeField == null){
            LenientObjectPrototype prototype = this.objectPrototypeField = new LenientObjectPrototype(this.engine);
            LenientFunctionPrototype fprot = this.functionPrototypeField = new LenientFunctionPrototype(prototype);
            prototype.Initialize(fprot);
            JSObject prot = new JSObject(prototype, false);
            prot.AddField("constructor").SetValue(prot, fprot);
            fprot.proto = prot;
          }
          return this.objectPrototypeField;
        }
      }
      
      internal override ActiveXObjectConstructor originalActiveXObject{
        get{
          if (this.originalActiveXObjectField == null)
            this.originalActiveXObjectField = new ActiveXObjectConstructor(this.functionPrototype);
          return this.originalActiveXObjectField;
        }
      }

      internal override ArrayConstructor originalArray{
        get{
          if (this.originalArrayField == null)
            this.originalArrayField = new ArrayConstructor(this.functionPrototype, this.arrayPrototype);
          return this.originalArrayField;
        }
      }

      internal override BooleanConstructor originalBoolean{
        get{
          if (this.originalBooleanField == null)
            this.originalBooleanField = new BooleanConstructor(this.functionPrototype, new LenientBooleanPrototype(this.functionPrototype, this.objectPrototype));
          return this.originalBooleanField;
        }
      }

      internal override DateConstructor originalDate{
        get{
          if (this.originalDateField == null)
            this.originalDateField = new LenientDateConstructor(this.functionPrototype, new LenientDatePrototype(this.functionPrototype, this.objectPrototype));
          return this.originalDateField;
        }
      }

      internal override ErrorConstructor originalError{
        get{
          if (this.originalErrorField == null)
            this.originalErrorField = new ErrorConstructor(this.functionPrototype, new LenientErrorPrototype(this.functionPrototype, this.objectPrototype, "Error"), this);
          return this.originalErrorField;
        }
      }
      
      internal override EnumeratorConstructor originalEnumerator{
        get{
          if (this.originalEnumeratorField == null)
              this.originalEnumeratorField = new EnumeratorConstructor(this.functionPrototype, new LenientEnumeratorPrototype(this.functionPrototype, this.objectPrototype));
          return this.originalEnumeratorField;
        }
      }

      internal override ErrorConstructor originalEvalError{
        get{
          if (this.originalEvalErrorField == null)
            this.originalEvalErrorField = new ErrorConstructor("EvalError", ErrorType.EvalError, this.originalError, this);
          return this.originalEvalErrorField;
        }
      }

      internal override FunctionConstructor originalFunction{
        get{
          if (this.originalFunctionField == null)
            this.originalFunctionField = new FunctionConstructor(this.functionPrototype);
          return this.originalFunctionField;
        }
      }

      internal override NumberConstructor originalNumber{
        get{
          if (this.originalNumberField == null)
            this.originalNumberField = new NumberConstructor(this.functionPrototype, new LenientNumberPrototype(this.functionPrototype, this.objectPrototype));
          return this.originalNumberField;
        }
      }

      internal override ObjectConstructor originalObject{
        get{
          if (this.originalObjectField == null)
            this.originalObjectField = new ObjectConstructor(this.functionPrototype, this.objectPrototype);
          return this.originalObjectField;
        }
      }

      internal override ObjectPrototype originalObjectPrototype{
        get{
          if (this.originalObjectPrototypeField == null)
            this.originalObjectPrototypeField = ObjectPrototype.ob;
          return this.originalObjectPrototypeField;
        }
      }

      internal override ErrorConstructor originalRangeError{
        get{
          if (this.originalRangeErrorField == null)
            this.originalRangeErrorField = new ErrorConstructor("RangeError", ErrorType.RangeError, this.originalError, this);
          return this.originalRangeErrorField;
        }
      }

      internal override ErrorConstructor originalReferenceError{
        get{
          if (this.originalReferenceErrorField == null)
            this.originalReferenceErrorField = new ErrorConstructor("ReferenceError", ErrorType.ReferenceError, this.originalError, this);
          return this.originalReferenceErrorField;
        }
      }

      internal override RegExpConstructor originalRegExp{
        get{
          if (this.originalRegExpField == null)
            this.originalRegExpField = new RegExpConstructor(this.functionPrototype, new LenientRegExpPrototype(this.functionPrototype, 
              this.objectPrototype), this.arrayPrototype);
          return this.originalRegExpField;
        }
      }

      internal override StringConstructor originalString{
        get{
          if (this.originalStringField == null)
            this.originalStringField = new LenientStringConstructor(this.functionPrototype, 
              new LenientStringPrototype(this.functionPrototype, this.objectPrototype));
          return this.originalStringField;
        }
      }

      internal override ErrorConstructor originalSyntaxError{
        get{
          if (this.originalSyntaxErrorField == null)
            this.originalSyntaxErrorField = new ErrorConstructor("SyntaxError", ErrorType.SyntaxError, this.originalError, this);
          return this.originalSyntaxErrorField;
        }
      }

      internal override ErrorConstructor originalTypeError{
        get{
          if (this.originalTypeErrorField == null)
            this.originalTypeErrorField = new ErrorConstructor("TypeError", ErrorType.TypeError, this.originalError, this);
          return this.originalTypeErrorField;
        }
      }

      internal override ErrorConstructor originalURIError{
        get{
          if (this.originalURIErrorField == null)
            this.originalURIErrorField = new ErrorConstructor("URIError", ErrorType.URIError, this.originalError, this);
          return this.originalURIErrorField;
        }
      }

      internal override VBArrayConstructor originalVBArray{
        get{
          if (this.originalVBArrayField == null)
            this.originalVBArrayField = new VBArrayConstructor(this.functionPrototype, 
              new LenientVBArrayPrototype(this.functionPrototype, this.objectPrototype));
          return this.originalVBArrayField;
        }
      }
      
      new public Object ActiveXObject{
        get{
          if (this.ActiveXObjectField is Missing)
            this.ActiveXObjectField = this.originalActiveXObject;
          return this.ActiveXObjectField;
        }
        set{
          this.ActiveXObjectField = value;
        }
      }
      
      new public Object Array{
        get{
          if (this.ArrayField is Missing)
            this.ArrayField = this.originalArray;
          return this.ArrayField;
        }
        set{
          this.ArrayField = value;
        }
      }
      
      new public Object Boolean{
        get{
          if (this.BooleanField is Missing)
            this.BooleanField = this.originalBoolean;
          return this.BooleanField;
        }
        set{
          this.BooleanField = value;
        }
      }
      
      new public Object Date{
        get{
          if (this.DateField is Missing)
            this.DateField = this.originalDate;
          return this.DateField;
        }
        set{
          this.DateField = value;
        }
      }
      
      new public Object Enumerator{
        get{
          if (this.EnumeratorField is Missing)
            this.EnumeratorField = this.originalEnumerator;
          return this.EnumeratorField;
        }
        set{
          this.EnumeratorField = value;
        }
      }
      
      new public Object Error{
        get{
          if (this.ErrorField is Missing)
            this.ErrorField = this.originalError;
          return this.ErrorField;
        }
        set{
          this.ErrorField = value;
        }
      }
      
      new public Object EvalError{
        get{
          if (this.EvalErrorField is Missing)
            this.EvalErrorField = this.originalEvalError;
          return this.EvalErrorField;
        }
        set{
          this.EvalErrorField = value;
        }
      }
      
      new public Object Function{
        get{
          if (this.FunctionField is Missing)
            this.FunctionField = this.originalFunction;
          return this.FunctionField;
        }
        set{
          this.FunctionField = value;
        }
      }
      
      new public Object Math{
        get{
          if (this.MathField is Missing)
            this.MathField = new LenientMathObject(this.objectPrototype, this.functionPrototype);
          return this.MathField;
        }
        set{
          this.MathField = value;
        }
      }
      
      new public Object Number{
        get{
          if (this.NumberField is Missing)
            this.NumberField = this.originalNumber;
          return this.NumberField;
        }
        set{
          this.NumberField = value;
        }
      }
      
      new public Object Object{
        get{
          if (this.ObjectField is Missing)
            this.ObjectField = this.originalObject;
          return this.ObjectField;
        }
        set{
          this.ObjectField = value;
        }
      }
      
      new public Object RangeError{
        get{
          if (this.RangeErrorField is Missing)
            this.RangeErrorField = this.originalRangeError;
          return this.RangeErrorField;
        }
        set{
          this.RangeErrorField = value;
        }
      }
      
      new public Object ReferenceError{
        get{
          if (this.ReferenceErrorField is Missing)
            this.ReferenceErrorField = this.originalReferenceError;
          return this.ReferenceErrorField;
        }
        set{
          this.ReferenceErrorField = value;
        }
      }
      
      new public Object RegExp{
        get{
          if (this.RegExpField is Missing)
            this.RegExpField = this.originalRegExp;
          return this.RegExpField;
        }
        set{
          this.RegExpField = value;
        }
      }
      
      new public Object String{
        get{
          if (this.StringField is Missing)
            this.StringField = this.originalString;
          return this.StringField;
        }
        set{
          this.StringField = value;
        }
      }
      
      new public Object SyntaxError{
        get{
          if (this.SyntaxErrorField is Missing)
            this.SyntaxErrorField = this.originalSyntaxError;
          return this.SyntaxErrorField;
        }
        set{
          this.SyntaxErrorField = value;
        }
      }
      
      new public Object TypeError{
        get{
          if (this.TypeErrorField is Missing)
            this.TypeErrorField = this.originalTypeError;
          return this.TypeErrorField;
        }
        set{
          this.TypeErrorField = value;
        }
      }
      
      new public Object URIError{
        get{
          if (this.URIErrorField is Missing)
            this.URIErrorField = this.originalURIError;
          return this.URIErrorField;
        }
        set{
          this.URIErrorField = value;
        }
      }
      
      new public Object VBArray{
        get{
          if (this.VBArrayField is Missing)
            this.VBArrayField = this.originalVBArray;
          return this.VBArrayField;
        }
        set{
          this.VBArrayField = value;
        }
      }
    }
}
