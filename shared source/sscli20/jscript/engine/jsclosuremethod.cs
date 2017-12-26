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
    using System.Reflection;
    using System.Globalization;
    
    internal sealed class JSClosureMethod : JSMethod{
      internal MethodInfo method;
        
      internal JSClosureMethod(MethodInfo method)
        : base(null){
        this.method = method;
      }
      
      internal override Object Construct(Object[] args){
        throw new JScriptException(JSError.InternalError); //should never happen at compile time.
      }
      
      public override MethodAttributes Attributes{
        get{
          return this.method.Attributes&(~MethodAttributes.Virtual)|MethodAttributes.Static; //Fool the analysis code to OK calls to this method from static code
        }
      }
        
      public override Type DeclaringType{
        get{
          return this.method.DeclaringType;
        }
      }
      
      public override ParameterInfo[] GetParameters(){
        return this.method.GetParameters();
      }
      
      internal override MethodInfo GetMethodInfo(CompilerGlobals compilerGlobals){
        if (this.method is JSMethod)
          return ((JSMethod)this.method).GetMethodInfo(compilerGlobals);
        else
          return this.method;
      }
      
      internal override Object Invoke(Object obj, Object thisob, BindingFlags options, Binder binder, Object[] parameters, CultureInfo culture){
        if (obj is StackFrame)
          return this.method.Invoke(((StackFrame)((StackFrame)obj).engine.ScriptObjectStackTop()).closureInstance, options, binder, parameters, culture);
        throw new JScriptException(JSError.InternalError); //should never happen at compile time.
      }
      
      public override String Name{
        get{
          return this.method.Name;
        }
      }
    
      public override Type ReturnType{
        get{
          return this.method.ReturnType;
        }
      }
    }

  
}
