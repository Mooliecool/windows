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

    public sealed class VBArrayConstructor : ScriptFunction{
      internal static readonly VBArrayConstructor ob = new VBArrayConstructor();
      
      private VBArrayPrototype originalPrototype;
      
      internal VBArrayConstructor()
        : base(FunctionPrototype.ob, "VBArray", 1) {
        this.originalPrototype = VBArrayPrototype.ob;
        VBArrayPrototype._constructor = this;
        this.proto = VBArrayPrototype.ob;
      }
    
      internal VBArrayConstructor(LenientFunctionPrototype parent, LenientVBArrayPrototype prototypeProp)
        : base(parent, "VBArray", 1) {
        this.originalPrototype = prototypeProp;
        prototypeProp.constructor = this;
        this.proto = prototypeProp;
        this.noExpando = false;
      }
      
      internal override Object Call(Object[] args, Object thisob){
        return null;
      }
      
      internal override Object Construct(Object[] args){
        return this.CreateInstance(args);
      }
      
      //Only to be used for type inference
      internal VBArrayObject Construct(){
        return new VBArrayObject(this.originalPrototype, null);
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasVarArgs)]
      public new Object CreateInstance(params Object[] args){
        if (args.Length < 1 || !typeof(Array).IsAssignableFrom(args[0].GetType()))
          throw new JScriptException(JSError.VBArrayExpected);
        return new VBArrayObject(this.originalPrototype, (Array)args[0]);
      }
    }
}
