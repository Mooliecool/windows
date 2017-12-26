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
    
    internal sealed class JSFieldMethod : JSMethod{
      internal FieldInfo field;
      internal FunctionObject func;
    
      private static readonly ParameterInfo[] EmptyParams = new ParameterInfo[0];
      
      internal JSFieldMethod(FieldInfo field, Object obj)
        : base(obj){ //The object is never used, but it is convenient to have the field on JSMethod.
        this.field = field;
        this.func = null;
        if (!field.IsLiteral)
          return;
        //If we get here, we know at compile time what function we are calling via the field. I.e. we're in fast mode.
        Object val = field is JSVariableField ? ((JSVariableField)field).value : field.GetValue(null);
        if (val is FunctionObject)
          this.func = (FunctionObject)val;
      }
      
      internal override Object Construct(Object[] args){
        return LateBinding.CallValue(this.field.GetValue(this.obj), args, true, false, ((ScriptObject)this.obj).engine, null, JSBinder.ob, null, null);
      }
      
      public override MethodAttributes Attributes{
        get{
          if (this.func != null)
            return this.func.attributes;
          else
            if (field.IsPublic)
              return MethodAttributes.Public;
            else if (field.IsFamily)
              return MethodAttributes.Family;
            else if (field.IsAssembly)
              return MethodAttributes.Assembly;
            else
              return MethodAttributes.Private;
        }
      }
        
      public override Type DeclaringType{
        get{
          if (this.func != null)
            return Convert.ToType(this.func.enclosing_scope);
          else
            return Typeob.Object;
        }
      }
      
      internal ScriptObject EnclosingScope(){
        if (this.func != null)
          return this.func.enclosing_scope;
        else
          return null;
      }
      
      public override Object[] GetCustomAttributes(bool inherit){
        if (this.func != null){
          CustomAttributeList caList = this.func.customAttributes;
          if (caList != null) return (Object[])caList.Evaluate(inherit);
        }
        return new Object[0];
      }
    
      public override ParameterInfo[] GetParameters(){
        if (this.func != null)
          return this.func.parameter_declarations;
        else
          return JSFieldMethod.EmptyParams;
      }
      
      internal override MethodInfo GetMethodInfo(CompilerGlobals compilerGlobals){
        return this.func.GetMethodInfo(compilerGlobals);
      }
      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      internal override Object Invoke(Object obj, Object thisob, BindingFlags options, Binder binder, Object[] parameters, CultureInfo culture){
        bool construct = (options & BindingFlags.CreateInstance) != 0;
        bool brackets = (options & BindingFlags.GetProperty) != 0 && (options & BindingFlags.InvokeMethod) == 0;
        Object f = this.func;
        if (f == null) f = this.field.GetValue(this.obj);
        FunctionObject func = f as FunctionObject;
        JSObject jsOb = obj as JSObject;
        if (jsOb != null && func != null && func.isMethod && (func.attributes & MethodAttributes.Virtual) != 0 && 
           jsOb.GetParent() != func.enclosing_scope && ((ClassScope)func.enclosing_scope).HasInstance(jsOb)){
          LateBinding lb = new LateBinding(func.name);
          lb.obj = jsOb;
          return lb.Call(parameters, construct, brackets, ((ScriptObject)this.obj).engine);
        }
        return LateBinding.CallValue(f, parameters, construct, brackets, ((ScriptObject)this.obj).engine, thisob, binder, culture, null);
      }     
      
      internal bool IsAccessibleFrom(ScriptObject scope){
        return ((JSMemberField)this.field).IsAccessibleFrom(scope);
      }
      
      public override String Name{
        get{
          return this.field.Name;
        }
      }
    
      public override Type ReturnType{
        get{
          if (this.func != null)
            return Convert.ToType(this.func.ReturnType(null));
          else
            return Typeob.Object;
        }
      }
      
      internal IReflect ReturnIR(){
        if (this.func != null)
          return this.func.ReturnType(null);
        else
          return Typeob.Object;
      }
    }
}
