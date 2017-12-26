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
    using System.Text;
    using System.Diagnostics;

    public sealed class FunctionConstructor : ScriptFunction{
      internal static readonly FunctionConstructor ob = new FunctionConstructor();
      
      internal FunctionPrototype originalPrototype;   
      
      internal FunctionConstructor()
        : base(FunctionPrototype.ob, "Function", 1) {
        this.originalPrototype = FunctionPrototype.ob;
        FunctionPrototype._constructor = this;
        this.proto = FunctionPrototype.ob;
      }
    
      internal FunctionConstructor(LenientFunctionPrototype prototypeProp)
        : base(prototypeProp, "Function", 1) {
        this.originalPrototype = prototypeProp;
        prototypeProp.constructor = this;
        this.proto = prototypeProp;
        this.noExpando = false;
      }
      
      internal override Object Call(Object[] args, Object thisob){
        return this.Construct(args, this.engine);
      }
      
      internal override Object Construct(Object[] args){
        return this.Construct(args, this.engine);
      }
      
      internal ScriptFunction Construct(Object[] args, VsaEngine engine){
        StringBuilder func_string = new StringBuilder("function anonymous(");
        for (int i = 0, n = args.Length-2; i < n; i++){
          func_string.Append(Convert.ToString(args[i]));
          func_string.Append(", ");
        }
        if (args.Length > 1)
          func_string.Append(Convert.ToString(args[args.Length-2]));
        func_string.Append(") {\n");
        if (args.Length > 0)
          func_string.Append(Convert.ToString(args[args.Length-1]));
        func_string.Append("\n}");
        Context context = new Context(new DocumentContext("anonymous", engine), func_string.ToString());
        JSParser p = new JSParser(context);
        engine.PushScriptObject(((IActivationObject)engine.ScriptObjectStackTop()).GetGlobalScope());
        try{
          return (ScriptFunction)p.ParseFunctionExpression().PartiallyEvaluate().Evaluate();
        }finally{
          engine.PopScriptObject();
        }
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasVarArgs)]
      public new ScriptFunction CreateInstance(params Object[] args){
        return this.Construct(args, this.engine);
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasVarArgs)]
      public ScriptFunction Invoke(params Object[] args){
        return this.Construct(args, this.engine);
      }
    }
}
