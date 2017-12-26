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
    
    public sealed class JSMethodInfo : MethodInfo{
      internal MethodInfo method;
      private MethodAttributes methAttributes;
      private String name = null;
      private Type declaringType = null;
      private ParameterInfo[] parameters = null;
      private Object[] attributes = null;
      private MethodInvoker methodInvoker = null;
      
      internal JSMethodInfo(MethodInfo method){
        this.method = method;
        this.methAttributes = method.Attributes;
      }
      
      public override MethodAttributes Attributes{
        get{
          return this.methAttributes;
        }
      }
        
      public override Type DeclaringType{
        get{
          Type result = this.declaringType;
          if (result == null) this.declaringType = result = this.method.DeclaringType;
          return result;
        }
      }
      
      public override MethodInfo GetBaseDefinition(){
        return this.method.GetBaseDefinition();
      }

      public sealed override Object[] GetCustomAttributes(bool inherit){
        Object[] attrs = this.attributes;
        if (attrs != null) return attrs;
        return this.attributes = this.method.GetCustomAttributes(true);
      }
    
      public sealed override Object[] GetCustomAttributes(Type type, bool inherit){
        if (type != typeof(JSFunctionAttribute))
          return null;
        Object[] attrs = this.attributes;
        if (attrs != null) return attrs;
        return this.attributes = CustomAttribute.GetCustomAttributes(this.method, type, true);
      }

      public override MethodImplAttributes GetMethodImplementationFlags(){
        return this.method.GetMethodImplementationFlags();
      }
      
      public override ParameterInfo[] GetParameters(){
        ParameterInfo[] parameters = this.parameters;
        if (parameters != null) return parameters;
        parameters = this.method.GetParameters();
        for (int i = 0, n = parameters.Length; i < n; i++)
          parameters[i] = new JSParameterInfo(parameters[i]);
        return this.parameters = parameters;
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      public override Object Invoke(Object obj, BindingFlags options, Binder binder, Object[] parameters, CultureInfo culture){
        MethodInfo method = TypeReferences.ToExecutionContext(this.method);
        if (binder != null){
          try{
            return method.Invoke(obj, options, binder, parameters, culture);
          }catch(TargetInvocationException e){
            throw e.InnerException;
          }
        }
        MethodInvoker invoker = this.methodInvoker;
        if (invoker == null){
          this.methodInvoker = invoker = MethodInvoker.GetInvokerFor(method);
          if (invoker == null)  //because the method is not safe to call via a thunk
            try{
              return method.Invoke(obj, options, binder, parameters, culture);
            }catch(TargetInvocationException e){
              throw e.InnerException;
            }
        }
        return invoker.Invoke(obj, parameters);
      }
    
      public sealed override bool IsDefined(Type type, bool inherit){ 
        Debug.PreCondition(type == typeof(JSFunctionAttribute));
        Object[] attrs = this.attributes;
        if (attrs == null)
          this.attributes = attrs = CustomAttribute.GetCustomAttributes(this.method, type, true);
        return attrs.Length > 0;
      }

      public override MemberTypes MemberType{
        get{
          return MemberTypes.Method;
        }
      }
      
      public override RuntimeMethodHandle MethodHandle{
        get{
          return this.method.MethodHandle;
        }
      }
      
      public override String Name{
        get{
          String result = this.name;
          if (result == null) this.name = result = this.method.Name;
          return result;
        }
      }
    
      public override Type ReflectedType{
        get{
          return this.method.ReflectedType;
        }
      }
      
      public override Type ReturnType{
        get{
          return this.method.ReturnType;
        }
      }
      
      public override ICustomAttributeProvider ReturnTypeCustomAttributes {
        get {
          return this.method.ReturnTypeCustomAttributes;
        }
      }
      
      public override String ToString(){
        return this.method.ToString();
      }

    }
}
