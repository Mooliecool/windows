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
    using System.Collections;
    using System.Diagnostics;
    using System.Reflection;
    
    public sealed class EnumeratorConstructor : ScriptFunction{
      internal static readonly EnumeratorConstructor ob = new EnumeratorConstructor();
      
      private EnumeratorPrototype originalPrototype; 
      
      internal EnumeratorConstructor()
        : base(FunctionPrototype.ob, "Enumerator", 1) {
        this.originalPrototype = EnumeratorPrototype.ob;
        EnumeratorPrototype._constructor = this;
        this.proto = EnumeratorPrototype.ob;
      }
    
      internal EnumeratorConstructor(LenientFunctionPrototype parent, LenientEnumeratorPrototype prototypeProp)
        : base(parent, "Enumerator", 1) {
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
    
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasVarArgs)]
      public new EnumeratorObject CreateInstance(params Object[] args){
        if (args.Length != 0){
          Object obj = args[0];
          if (obj is IEnumerable)
            return new EnumeratorObject(this.originalPrototype, (IEnumerable)obj);
          throw new JScriptException(JSError.NotCollection);
        }else
          return new EnumeratorObject(this.originalPrototype, (IEnumerable)null);
      }
      
      public Object Invoke(){
        return null;
      }
      
    }
}
