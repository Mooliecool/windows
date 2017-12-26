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
    using System.Diagnostics;
    
    public enum ErrorType : int {OtherError, EvalError, RangeError, ReferenceError, SyntaxError, TypeError, URIError};
      
    public sealed class ErrorConstructor : ScriptFunction{
      internal static readonly ErrorConstructor ob = new ErrorConstructor();
      internal static readonly ErrorConstructor evalOb = new ErrorConstructor("EvalError", ErrorType.EvalError);
      internal static readonly ErrorConstructor rangeOb = new ErrorConstructor("RangeError", ErrorType.RangeError);
      internal static readonly ErrorConstructor referenceOb = new ErrorConstructor("ReferenceError", ErrorType.ReferenceError);
      internal static readonly ErrorConstructor syntaxOb = new ErrorConstructor("SyntaxError", ErrorType.SyntaxError);
      internal static readonly ErrorConstructor typeOb = new ErrorConstructor("TypeError", ErrorType.TypeError);
      internal static readonly ErrorConstructor uriOb = new ErrorConstructor("URIError", ErrorType.URIError);
      
      private ErrorPrototype originalPrototype; 
      private ErrorType type;
      private GlobalObject globalObject;
      
      internal ErrorConstructor()
        : base(ErrorPrototype.ob, "Error", 2) {
        this.originalPrototype = ErrorPrototype.ob;
        ErrorPrototype.ob._constructor = this;
        this.proto = ErrorPrototype.ob;
        this.type = ErrorType.OtherError;
        this.globalObject = GlobalObject.commonInstance;
      }
    
      internal ErrorConstructor(LenientFunctionPrototype parent, LenientErrorPrototype prototypeProp, GlobalObject globalObject)
        : base(parent, "Error", 2) {
        this.originalPrototype = prototypeProp;
        prototypeProp.constructor = this;
        this.proto = prototypeProp;
        this.type = ErrorType.OtherError;
        this.globalObject = globalObject;
        this.noExpando = false;
      }
      
      internal ErrorConstructor(String subtypeName, ErrorType type)
        : base(ErrorConstructor.ob.parent, subtypeName, 2){
        this.originalPrototype = new ErrorPrototype(ob.originalPrototype, subtypeName);
        this.originalPrototype._constructor = this;
        this.proto = this.originalPrototype;
        this.type = type;
        this.globalObject = GlobalObject.commonInstance;
      }
        
      internal ErrorConstructor(String subtypeName, ErrorType type, ErrorConstructor error, GlobalObject globalObject)
        : base(error.parent, subtypeName, 2){
        this.originalPrototype = new LenientErrorPrototype((LenientFunctionPrototype)error.parent, error.originalPrototype, subtypeName);
        this.noExpando = false;
        this.originalPrototype._constructor = this;
        this.proto = this.originalPrototype;
        this.type = type;
        this.globalObject = globalObject;
        this.noExpando = false;
      }
      
      internal override Object Call(Object[] args, Object thisob){
        return this.Construct(args);
      }
      
      internal override Object Construct(Object[] args){
        return this.CreateInstance(args);
      }
    
      internal ErrorObject Construct(Object e){
        if (!(e is JScriptException) || this != this.globalObject.originalError){
          switch (this.type){
            case ErrorType.EvalError: return new EvalErrorObject(this.originalPrototype, e);
            case ErrorType.RangeError: return new RangeErrorObject(this.originalPrototype, e);
            case ErrorType.ReferenceError: return new ReferenceErrorObject(this.originalPrototype, e);
            case ErrorType.SyntaxError: return new SyntaxErrorObject(this.originalPrototype, e);
            case ErrorType.TypeError: return new TypeErrorObject(this.originalPrototype, e);
            case ErrorType.URIError: return new URIErrorObject(this.originalPrototype, e);
            default: return new ErrorObject(this.originalPrototype, e);
          }
        }
        switch (((JScriptException)e).GetErrorType()){
          case ErrorType.EvalError: return this.globalObject.originalEvalError.Construct(e);
          case ErrorType.RangeError: return this.globalObject.originalRangeError.Construct(e);
          case ErrorType.ReferenceError: return this.globalObject.originalReferenceError.Construct(e);
          case ErrorType.SyntaxError: return this.globalObject.originalSyntaxError.Construct(e);
          case ErrorType.TypeError: return this.globalObject.originalTypeError.Construct(e);
          case ErrorType.URIError: return this.globalObject.originalURIError.Construct(e);
          default: return new ErrorObject(this.originalPrototype, e);
        }
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasVarArgs)]
      public new ErrorObject CreateInstance(params Object[] args){
        switch (this.type){
          case ErrorType.EvalError: return new EvalErrorObject(this.originalPrototype, args);
          case ErrorType.RangeError: return new RangeErrorObject(this.originalPrototype, args);
          case ErrorType.ReferenceError: return new ReferenceErrorObject(this.originalPrototype, args);
          case ErrorType.SyntaxError: return new SyntaxErrorObject(this.originalPrototype, args);
          case ErrorType.TypeError: return new TypeErrorObject(this.originalPrototype, args);
          case ErrorType.URIError: return new URIErrorObject(this.originalPrototype, args);
          default: return new ErrorObject(this.originalPrototype, args);
        }
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasVarArgs)]
      public Object Invoke(params Object[] args){
        return this.CreateInstance(args);
      }
      
    }
}
