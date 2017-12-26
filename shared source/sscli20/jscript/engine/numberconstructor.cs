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

    public class NumberConstructor : ScriptFunction{
      public const double MAX_VALUE = Double.MaxValue;
      public const double MIN_VALUE = Double.Epsilon; //smallest postive value
      public const double NaN = Double.NaN;
      public const double NEGATIVE_INFINITY = Double.NegativeInfinity;
      public const double POSITIVE_INFINITY = Double.PositiveInfinity;
      
      internal static readonly NumberConstructor ob = new NumberConstructor();
      
      private NumberPrototype originalPrototype; 
      
      internal NumberConstructor()
        : base(FunctionPrototype.ob, "Number", 1) {
        this.originalPrototype = NumberPrototype.ob;
        NumberPrototype._constructor = this;
        this.proto = NumberPrototype.ob;
      }
      
      internal NumberConstructor(LenientFunctionPrototype parent, LenientNumberPrototype prototypeProp)
        : base(parent, "Number", 1) {
        this.originalPrototype = prototypeProp;
        prototypeProp.constructor = this;
        this.proto = prototypeProp;
        this.noExpando = false;
      }
      
      internal override Object Call(Object[] args, Object thisob){
        if (args.Length == 0)
          return 0;
        else
          return Convert.ToNumber(args[0]);
      }
      
      internal NumberObject Construct(){
        return new NumberObject(this.originalPrototype, 0.0, false);
      }

      
      internal override Object Construct(Object[] args){
        return this.CreateInstance(args);
      }
      
      internal NumberObject ConstructImplicitWrapper(Object arg){
        return new NumberObject(this.originalPrototype, arg, true);
      }
      
      internal NumberObject ConstructWrapper(Object arg){
        return new NumberObject(this.originalPrototype, arg, false);
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasVarArgs)]
      public new NumberObject CreateInstance(params Object[] args){
        if (args.Length == 0) 
          return new NumberObject(this.originalPrototype, 0.0, false);
        else 
          return new NumberObject(this.originalPrototype, Convert.ToNumber(args[0]), false);
      }
      
      public double Invoke(Object arg){
        return Convert.ToNumber(arg);
      }
      
    }
}
