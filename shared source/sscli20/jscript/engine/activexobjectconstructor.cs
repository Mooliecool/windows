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

    /// <summary>
    /// Internal implementation of JScript's ActiveXObject constructor.
    /// </summary>
    /// <internalonly/>
    public sealed class ActiveXObjectConstructor : ScriptFunction{
      internal static readonly ActiveXObjectConstructor ob = new ActiveXObjectConstructor();
      
      internal ActiveXObjectConstructor() 
        : base(FunctionPrototype.ob, "ActiveXObject", 1) {
      }
      
      internal ActiveXObjectConstructor(LenientFunctionPrototype parent)
        : base(parent, "ActiveXObject", 1) {
         this.noExpando = false;
     }
      
      internal override Object Call(Object[] args, Object thisob){
        return null;
      }
      
      internal override Object Construct(Object[] args){
        return this.CreateInstance(args);
      }
    
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasVarArgs)]
      public new Object CreateInstance(params Object[] args){
      // FEATURE_PAL doesn't support COM, so we just generate a runtime error here
        throw new JScriptException(JSError.CantCreateObject);
      }
      
      public Object Invoke(){
        return null;
      }
      
      internal override bool HasInstance(Object ob){
        if (ob is JSObject)
          return false;
        return true;  
      }
      
    }
}
