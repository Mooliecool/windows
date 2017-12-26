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
    using System.Reflection.Emit;
    using System.Globalization;
    using System.Diagnostics;
    
    internal sealed class JSWrappedMethod : JSMethod, IWrappedMember{
      internal MethodInfo method;
      private ParameterInfo[] pars;
    
      internal JSWrappedMethod(MethodInfo method, Object obj)
        : base(obj) {
        this.obj = obj;
        if (method is JSMethodInfo) method = ((JSMethodInfo)method).method;
        this.method = method.GetBaseDefinition();
        this.pars = this.method.GetParameters();
        if (obj is JSObject && !Typeob.JSObject.IsAssignableFrom(method.DeclaringType))
          if (obj is BooleanObject) this.obj = ((BooleanObject)obj).value;
          else if (obj is NumberObject) this.obj = ((NumberObject)obj).value;
          else if (obj is StringObject) this.obj = ((StringObject)obj).value;
          else if (obj is ArrayWrapper) this.obj = ((ArrayWrapper)obj).value;
      }
      
      public override MethodAttributes Attributes{
        get{
          return this.method.Attributes;
        }
      }

      private Object[] CheckArguments(Object[] args){
        Object[] newArgs = args;
        if (args != null && args.Length < this.pars.Length){
          newArgs = new Object[this.pars.Length];
          ArrayObject.Copy(args, newArgs, args.Length);
          for (int i = args.Length, size = this.pars.Length; i < size; i++)
            newArgs[i] = System.Type.Missing; // this will take care of the default value
        }
        return newArgs;
      }
        
      internal override Object Construct(Object[] args){
        if (this.method is JSMethod)
          return ((JSMethod)(this.method)).Construct(args);
        if (this.method.GetParameters().Length == 0 && this.method.ReturnType == Typeob.Object){
          Object func = this.method.Invoke(this.obj, BindingFlags.SuppressChangeType, null, null, null);
          if (func is ScriptFunction)
            return ((ScriptFunction)func).Construct(args);
        }
        throw new JScriptException(JSError.NoConstructor);
      }
      
      public override Type DeclaringType{
        get{
          return this.method.DeclaringType;
        }
      }
      
      internal override String GetClassFullName(){
        if (this.method is JSMethod)
          return ((JSMethod)this.method).GetClassFullName();
        else
          return this.method.DeclaringType.FullName;
      }
            
      internal override PackageScope GetPackage(){
        if (this.method is JSMethod){
          return ((JSMethod)this.method).GetPackage();
        }
        return null;
      }
            
      public override ParameterInfo[] GetParameters(){
        return this.pars;
      }
      
      internal override MethodInfo GetMethodInfo(CompilerGlobals compilerGlobals){
        if (this.method is JSMethod)
          return ((JSMethod)(this.method)).GetMethodInfo(compilerGlobals);
        else
          return this.method;
      }
      
      public Object GetWrappedObject(){
        return this.obj;
      }
      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      public override Object Invoke(Object obj, BindingFlags options, Binder binder, Object[] parameters, CultureInfo culture){
        parameters = CheckArguments(parameters);
        return this.Invoke(this.obj, this.obj, options, binder, parameters, culture);
      }
    
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      internal override Object Invoke(Object obj, Object thisob, BindingFlags options, Binder binder, Object[] parameters, CultureInfo culture){
        parameters = CheckArguments(parameters);
        if (this.obj != null && !(this.obj is Type)) obj = this.obj;
        if (this.method is JSMethod)
          return ((JSMethod)this.method).Invoke(obj, thisob, options, binder, parameters, culture);
        else
          return this.method.Invoke(obj, options, binder, parameters, culture);
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

      public override String ToString(){
        return this.method.ToString();
      }
    }
}
