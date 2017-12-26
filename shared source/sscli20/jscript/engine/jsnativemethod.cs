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
    using System.Reflection.Emit;
    using System.Globalization;
    using System.Diagnostics;
    
    internal sealed class JSNativeMethod : JSMethod{
      private MethodInfo method;
      private ParameterInfo[] formalParams;
      private bool hasThis;
      private bool hasVarargs;
      private bool hasEngine;
      private VsaEngine engine;
        
      internal JSNativeMethod(MethodInfo method, Object obj, VsaEngine engine)
        : base(obj){
        this.method = method;
        this.formalParams = method.GetParameters();
        Object[] attrs = CustomAttribute.GetCustomAttributes(method, typeof(JSFunctionAttribute), false);
        JSFunctionAttribute attr = attrs.Length > 0 ? (JSFunctionAttribute)attrs[0] : new JSFunctionAttribute((JSFunctionAttributeEnum)0);
        JSFunctionAttributeEnum attrVal = attr.attributeValue;
        if ((attrVal & JSFunctionAttributeEnum.HasThisObject) != 0)
          this.hasThis = true;
        if ((attrVal & JSFunctionAttributeEnum.HasEngine) != 0)
          this.hasEngine = true;
        if ((attrVal & JSFunctionAttributeEnum.HasVarArgs) != 0)
          this.hasVarargs = true;
        this.engine = engine;
      }
      
      internal override Object Construct(Object[] args){
        throw new JScriptException(JSError.NoConstructor);
      }
      
      public override MethodAttributes Attributes{
        get{
          return this.method.Attributes;
        }
      }
        
      public override Type DeclaringType{
        get{
          return this.method.DeclaringType;
        }
      }
      
      public override ParameterInfo[] GetParameters(){
        return this.formalParams;
      }
      
      internal override MethodInfo GetMethodInfo(CompilerGlobals compilerGlobals){
        return this.method;
      }
      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      internal override Object Invoke(Object obj, Object thisob, BindingFlags options, Binder binder, Object[] parameters, CultureInfo culture){
        int n = this.formalParams.Length;
        int pn = (parameters != null ? parameters.Length : 0);
        if (!this.hasThis && !this.hasVarargs && n == pn)
          if (binder != null)
            return TypeReferences.ToExecutionContext(this.method).Invoke(this.obj, BindingFlags.SuppressChangeType, null, this.ConvertParams(0, parameters, binder, culture), null);
          else
            return TypeReferences.ToExecutionContext(this.method).Invoke(this.obj, options, binder, parameters, culture);
        int offset = (this.hasThis ? 1 : 0) + (this.hasEngine ? 1 : 0);
        Object[] arguments = new Object[n];
        if (this.hasThis){
          arguments[0] = thisob;
          if (this.hasEngine)
            arguments[1] = this.engine;
        }else if (this.hasEngine)
          arguments[0] = this.engine;
        if (this.hasVarargs){
          if (n == offset+1) //No params other than the vararg array
            arguments[offset] = parameters;
          else{
            //Some of the values in parameters must be passed separately from the vararg array
            int argsToCopy = n - 1 - offset; //The number of separate arguments
            if (pn > argsToCopy){
              ArrayObject.Copy(parameters, 0, arguments, offset, argsToCopy);
              int vn = pn-argsToCopy;
              Object[] varargs = new Object[vn];
              ArrayObject.Copy(parameters, argsToCopy, varargs, 0, vn); 
              arguments[n - 1] = varargs;
            }else{
              ArrayObject.Copy(parameters, 0, arguments, offset, pn);
              for (int i = pn; i < argsToCopy; i++)
                arguments[i+offset] = Missing.Value;
              arguments[n - 1] = new Object[0];
            }
          }
        }else{
          if(parameters != null){
            if (n-offset < pn)
              ArrayObject.Copy(parameters, 0, arguments, offset, n-offset);
            else
              ArrayObject.Copy(parameters, 0, arguments, offset, pn);
          }
          if(n-offset > pn){
            for (int i = pn+offset; i < n; i++)
              if (i == n-1 && this.formalParams[i].ParameterType.IsArray && CustomAttribute.IsDefined(this.formalParams[i], typeof(ParamArrayAttribute), true))
                arguments[i] = System.Array.CreateInstance(this.formalParams[i].ParameterType.GetElementType(), 0);
              else
                arguments[i] = Missing.Value;
          }
        }
        if (binder != null)
          return TypeReferences.ToExecutionContext(this.method).Invoke(this.obj, BindingFlags.SuppressChangeType, null, this.ConvertParams(offset, arguments, binder, culture), null);
        else
          return TypeReferences.ToExecutionContext(this.method).Invoke(this.obj, options, binder, arguments, culture);
      }
      
      private Object[] ConvertParams(int offset, Object[] parameters, Binder binder, CultureInfo culture){
        int n = this.formalParams.Length;
        if (this.hasVarargs) n--;
        for (int i = offset; i < n; i++){
          Type fpt = this.formalParams[i].ParameterType;
          if (fpt != Typeob.Object)
            parameters[i] = binder.ChangeType(parameters[i], fpt, culture);
        }
        return parameters;  
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
