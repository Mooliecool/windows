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

    public sealed class BooleanConstructor : ScriptFunction{
      internal static readonly BooleanConstructor ob = new BooleanConstructor();
      
      private BooleanPrototype originalPrototype; 
      
      internal BooleanConstructor()
        : base(FunctionPrototype.ob, "Boolean", 1) {
        this.originalPrototype = BooleanPrototype.ob;
        BooleanPrototype._constructor = this;
        this.proto = BooleanPrototype.ob;
      }
    
      internal BooleanConstructor(LenientFunctionPrototype parent, LenientBooleanPrototype prototypeProp)
        : base(parent, "Boolean", 1) {
        this.originalPrototype = prototypeProp;
        prototypeProp.constructor = this;
        this.proto = prototypeProp;
        this.noExpando = false;
      }
      
      internal override Object Call(Object[] args, Object thisob){
        if (args.Length == 0)
          return false;
        else
          return Convert.ToBoolean(args[0]);
      }
      
      internal BooleanObject Construct(){
        return new BooleanObject(originalPrototype, false, false);
      }
      
      internal override Object Construct(Object[] args){
        return this.CreateInstance(args);
      }
      
      internal BooleanObject ConstructImplicitWrapper(bool arg){
        return new BooleanObject(originalPrototype, arg, true);
      }
      
      internal BooleanObject ConstructWrapper(bool arg){
        return new BooleanObject(originalPrototype, arg, false);
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasVarArgs)]
      public new BooleanObject CreateInstance(params Object[] args){
        return new BooleanObject(originalPrototype, args.Length == 0 ? false : Convert.ToBoolean(args[0]), false);
      }
      
      public bool Invoke(Object arg){
        return Convert.ToBoolean(arg);
      }
    }
}
