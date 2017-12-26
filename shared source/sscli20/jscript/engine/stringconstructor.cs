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
    using System.Text;

    public class StringConstructor : ScriptFunction{
      internal static readonly StringConstructor ob = new StringConstructor();
    
      private StringPrototype originalPrototype; 
      
      internal StringConstructor()
        : base(FunctionPrototype.ob, "String", 1) {
        this.originalPrototype = StringPrototype.ob;
        StringPrototype._constructor = this;
        this.proto = StringPrototype.ob;
      }
      
      internal StringConstructor(FunctionPrototype parent, LenientStringPrototype prototypeProp)
        : base(parent, "String", 1) {
        this.originalPrototype = prototypeProp;
        prototypeProp.constructor = this;
        this.proto = prototypeProp;
        this.noExpando = false;
      }
      
      internal override Object Call(Object[] args, Object thisob){
        if (args.Length == 0)
          return "";
        else
          return Convert.ToString(args[0]);
      }
      
      internal StringObject Construct(){
        return new StringObject(this.originalPrototype, "", false);
      }
    
      internal override Object Construct(Object[] args){
        return this.CreateInstance(args);
      }
      
      internal StringObject ConstructImplicitWrapper(String arg){
        return new StringObject(this.originalPrototype, arg, true);
      }
    
      internal StringObject ConstructWrapper(String arg){
        return new StringObject(this.originalPrototype, arg, false);
      }
    
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasVarArgs)]
      public new StringObject CreateInstance(params Object[] args){
        return new StringObject(this.originalPrototype, args.Length == 0 ? "" : Convert.ToString(args[0]), false);
      }
      
      public String Invoke(Object arg){
        return Convert.ToString(arg);
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasVarArgs, JSBuiltin.String_fromCharCode)]
      public static String fromCharCode(params Object[] args){
        StringBuilder str = new StringBuilder(args.Length);
        for (int i = 0; i < args.Length; i++)
          str.Append(Convert.ToChar(args[i]));
        return str.ToString();
      }
    }
}
