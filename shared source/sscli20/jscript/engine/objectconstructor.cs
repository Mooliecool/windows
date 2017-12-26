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
    using System.Reflection;

    public sealed class ObjectConstructor : ScriptFunction{
      internal static readonly ObjectConstructor ob = new ObjectConstructor();
      
      internal ObjectPrototype originalPrototype;
      
      internal ObjectConstructor()
        : base(FunctionPrototype.ob, "Object", 1) {
        this.originalPrototype = ObjectPrototype.ob;
        ObjectPrototype._constructor = this;
        this.proto = ObjectPrototype.ob;
      }
      
      internal ObjectConstructor(LenientFunctionPrototype parent, LenientObjectPrototype prototypeProp)
        : base(parent, "Object", 1) {
        this.originalPrototype = prototypeProp;
        prototypeProp.constructor = this;
        this.proto = prototypeProp;
        this.noExpando = false;
      }
      
      internal override Object Call(Object[] args, Object thisob){
        if (args.Length == 0)
          return this.ConstructObject();
        Object val = args[0];
        if (val == null || val == DBNull.Value)
          return this.Construct(args);
        else
          return Convert.ToObject3(val, this.engine);
      }
      
      internal override Object Construct(Object[] args){
        if (args.Length == 0)
          return this.ConstructObject();
        Object val = args[0];
        switch (Convert.GetTypeCode(val)){
          case TypeCode.Empty:
          case TypeCode.DBNull: return this.ConstructObject();
          case TypeCode.Object: 
            if (val is ScriptObject) return val;
            IReflect ir = null;
            if (val is IReflect)
              ir = (IReflect)val;
            else
              ir = val.GetType();
            const BindingFlags flags = 
              BindingFlags.Public|BindingFlags.OptionalParamBinding|
              BindingFlags.CreateInstance;
            return ir.InvokeMember(String.Empty, flags, JSBinder.ob, val, new Object[0], null, null, null);
        }
        return Convert.ToObject3(val, this.engine);
      }
      
      public JSObject ConstructObject(){
        return new JSObject(this.originalPrototype, false);
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasVarArgs)]
      public new Object CreateInstance(params Object[] args){
        return this.Construct(args);
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasVarArgs)]
      public Object Invoke(params Object[] args){
        return this.Call(args, null);
      }
      
    }
}
