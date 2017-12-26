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
    
    internal class JSProperty : PropertyInfo{
      private String name;
      private ParameterInfo[] formal_parameters;
      internal PropertyBuilder metaData;
      internal JSMethod getter;
      internal JSMethod setter;
    
      internal JSProperty(String name){
        this.name = name;
        this.formal_parameters = null;
        this.getter = null;
        this.setter = null;
      }
      
      public override PropertyAttributes Attributes{
        get{
          return PropertyAttributes.None;
        }
      }
      
      public override bool CanRead{
        get{
          return JSProperty.GetGetMethod(this, true) != null;
        }
      }
      
      public override bool CanWrite{
        get{
          return JSProperty.GetSetMethod(this, true) != null;
        }
      }
    
      internal virtual String GetClassFullName(){
        if (this.getter != null)
          return this.getter.GetClassFullName();
        else
          return this.setter.GetClassFullName();
      }

      internal bool GetterAndSetterAreConsistent(){
        if (this.getter == null || this.setter == null)
          return true;
        ((JSFieldMethod)this.getter).func.PartiallyEvaluate();
        ((JSFieldMethod)this.setter).func.PartiallyEvaluate();
        ParameterInfo[] getterPars = this.getter.GetParameters();
        ParameterInfo[] setterPars = this.setter.GetParameters();
        int n = getterPars.Length;
        int m = setterPars.Length;
        if (n != m-1)
          return false;
        if (!((JSFieldMethod)this.getter).func.ReturnType(null).Equals(((ParameterDeclaration)setterPars[n]).type.ToIReflect()))
          return false;
        for (int i = 0; i < n; i++)
          if (((ParameterDeclaration)getterPars[i]).type.ToIReflect() != ((ParameterDeclaration)setterPars[i]).type.ToIReflect())
            return false;
        return (this.getter.Attributes&~MethodAttributes.Abstract) == (this.setter.Attributes&~MethodAttributes.Abstract);
      }
      
      public override Type DeclaringType{
        get{
          if (this.getter != null)
            return this.getter.DeclaringType;
          else
            return this.setter.DeclaringType;
        }
      }                                         
      
      public sealed override Object[] GetCustomAttributes(Type t, bool inherit){
        return new Object[0];
      }

      public sealed override Object[] GetCustomAttributes(bool inherit){
        if (this.getter != null)
          return this.getter.GetCustomAttributes(true);
        if (this.setter != null)
          return this.setter.GetCustomAttributes(true);
        return new Object[0];
      }
    
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      internal static Object GetValue(PropertyInfo prop, Object obj, Object[] index){
        JSProperty jsprop = prop as JSProperty;
        if (jsprop != null) return jsprop.GetValue(obj, BindingFlags.ExactBinding, null, index, null);
        JSWrappedProperty jswrappedprop = prop as JSWrappedProperty;
        if (jswrappedprop != null) return jswrappedprop.GetValue(obj, BindingFlags.ExactBinding, null, index, null);
        MethodInfo meth = JSProperty.GetGetMethod(prop, false);
        if (meth != null){ 
          try { return meth.Invoke(obj, BindingFlags.ExactBinding, null, index, null); }
          catch (TargetInvocationException e) { throw e.InnerException; }
        }
        throw new MissingMethodException();
      }
    
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      public override Object GetValue(Object obj, BindingFlags invokeAttr, Binder binder, Object[] index, CultureInfo culture){
        MethodInfo getter = this.getter;
        JSObject jsOb = obj as JSObject;
        if (getter == null && jsOb != null){
          getter = jsOb.GetMethod("get_"+this.name, BindingFlags.Instance|BindingFlags.Public|BindingFlags.NonPublic);
          JSWrappedMethod wrm = getter as JSWrappedMethod;
          if (wrm != null) getter = wrm.method;
        }
        if (getter == null)
          getter = this.GetGetMethod(false);
        if (getter != null) {
          try { return getter.Invoke(obj, invokeAttr, binder, index == null ? new Object[0] : index, culture); }
          catch (TargetInvocationException e) { throw e.InnerException; }
        }
        else
          return Missing.Value;
  }
      
      public override MethodInfo[] GetAccessors(bool nonPublic){
        if (this.getter != null && (nonPublic || this.getter.IsPublic))
          if (this.setter != null && (nonPublic || this.setter.IsPublic))
            return new MethodInfo[]{this.getter, this.setter};
          else
            return new MethodInfo[]{this.getter};
        else
          if (this.setter != null && (nonPublic || this.setter.IsPublic))
            return new MethodInfo[]{this.setter};
          else
            return new MethodInfo[]{};
      }
      
      internal static MethodInfo GetGetMethod(PropertyInfo prop, bool nonPublic){
        if (prop == null) return null;
        JSProperty jsprop = prop as JSProperty;
        if (jsprop != null) return jsprop.GetGetMethod(nonPublic);
        MethodInfo meth = prop.GetGetMethod(nonPublic);
        if (meth != null) return meth;
        Type pdt = prop.DeclaringType;
        if (pdt == null) return null;
        Type bt = pdt.BaseType;
        if (bt == null) return null;
        meth = prop.GetGetMethod(nonPublic);
        if (meth == null) return null;
        BindingFlags flags = BindingFlags.Public;
        if (meth.IsStatic) 
          flags |= BindingFlags.Static|BindingFlags.FlattenHierarchy;
        else 
          flags |= BindingFlags.Instance;
        if (nonPublic) flags |= BindingFlags.NonPublic;
        String pname = prop.Name;
        prop = null;
        try{
          prop = bt.GetProperty(pname, flags, null, null, new Type[]{}, null);
        }catch(AmbiguousMatchException){}
        if (prop != null) return JSProperty.GetGetMethod(prop, nonPublic);
        return null;
      }

      public override MethodInfo GetGetMethod(bool nonPublic){
        if (this.getter == null){
          try{
            IReflect ir = ((ClassScope)this.setter.obj).GetSuperType();
            BindingFlags flags = BindingFlags.Public;
            if (this.setter.IsStatic) 
              flags |= BindingFlags.Static|BindingFlags.FlattenHierarchy;
            else 
              flags |= BindingFlags.Instance;
            if (nonPublic) flags |= BindingFlags.NonPublic;
            PropertyInfo prop = ir.GetProperty(this.name, flags, null, null, new Type[]{}, null);
            if (prop is JSProperty)
              return prop.GetGetMethod(nonPublic);
            else
              return JSProperty.GetGetMethod(prop, nonPublic);
          }catch(AmbiguousMatchException){}
        }
        if (nonPublic || this.getter.IsPublic)
          return this.getter;
        else
          return null;
      }
      
      public override ParameterInfo[] GetIndexParameters(){
        if (this.formal_parameters == null){
          if (this.getter != null)
            this.formal_parameters = this.getter.GetParameters();
          else{
            ParameterInfo[] setterPars = this.setter.GetParameters();
            int n = setterPars.Length;
            if (n <= 1) n = 1;
            this.formal_parameters = new ParameterInfo[n-1];
            for (int i = 0; i < n-1; i++)
              this.formal_parameters[i] = setterPars[i];
          }
        }
        return this.formal_parameters;
      }
    
      internal static MethodInfo GetSetMethod(PropertyInfo prop, bool nonPublic){
        if (prop == null) return null;
        JSProperty jsProp = prop as JSProperty;
        if (jsProp != null) return jsProp.GetSetMethod(nonPublic);
        MethodInfo meth = prop.GetSetMethod(nonPublic);
        if (meth != null) return meth;
        Type pdt = prop.DeclaringType;
        if (pdt == null) return null;
        Type bt = pdt.BaseType;
        if (bt == null) return null;
        meth = prop.GetGetMethod(nonPublic);
        if (meth == null) return null;
        BindingFlags flags = BindingFlags.Public;
        if (meth.IsStatic) 
          flags |= BindingFlags.Static|BindingFlags.FlattenHierarchy;
        else 
          flags |= BindingFlags.Instance;
        if (nonPublic) flags |= BindingFlags.NonPublic;
        String pname = prop.Name;
        prop = null;
        try{
          prop = bt.GetProperty(pname, flags, null, null, new Type[]{}, null);
        }catch(AmbiguousMatchException){}
        if (prop != null) return JSProperty.GetSetMethod(prop, nonPublic);
        return null;
      }

      public override MethodInfo GetSetMethod(bool nonPublic){
        if (this.setter == null){
          try{
            IReflect ir = ((ClassScope)this.getter.obj).GetSuperType();
            BindingFlags flags = BindingFlags.Public;
            if (this.getter.IsStatic) 
              flags |= BindingFlags.Static|BindingFlags.FlattenHierarchy;
            else 
              flags |= BindingFlags.Instance;
            if (nonPublic) flags |= BindingFlags.NonPublic;
            PropertyInfo prop = ir.GetProperty(this.name, flags, null, null, new Type[]{}, null);
            if (prop is JSProperty)
              return prop.GetSetMethod(nonPublic);
            else
              return JSProperty.GetSetMethod(prop, nonPublic);
          }catch(AmbiguousMatchException){}
        }
        if (nonPublic || this.setter.IsPublic)
          return this.setter;
        else
          return null;
      }
      
      public sealed override bool IsDefined(Type type, bool inherit){ 
        return false;
      }
      
      public override MemberTypes MemberType{
        get{
          return MemberTypes.Property;
        }
      }
      
      public override String Name{
        get{
          return this.name;
        }
      }
      
      internal IReflect PropertyIR(){
        if (this.getter is JSFieldMethod)
          return ((JSFieldMethod)this.getter).ReturnIR();
        if (this.setter != null){
          ParameterInfo[] pars = this.setter.GetParameters();
          if (pars.Length > 0){
            ParameterInfo par = pars[pars.Length-1];
            if (par is ParameterDeclaration)
              return ((ParameterDeclaration)par).ParameterIReflect;
            else
              return par.ParameterType;
          }
        }
        return Typeob.Void;
      }
            
      public override Type PropertyType{
        get{
          if (this.getter != null)
            return this.getter.ReturnType;
          if (this.setter != null){
            ParameterInfo[] pars = this.setter.GetParameters();
            if (pars.Length > 0)
              return pars[pars.Length-1].ParameterType;
          }
          return Typeob.Void;
        }
      }
            
      public override Type ReflectedType{
        get{
          if (this.getter != null)
            return this.getter.ReflectedType;
          else
            return this.setter.ReflectedType;
        }
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      internal static void SetValue(PropertyInfo prop, Object obj, Object value, Object[] index){
        JSProperty jsprop = prop as JSProperty;
        if (jsprop != null) {jsprop.SetValue(obj, value, BindingFlags.ExactBinding, null, index, null); return;}
        MethodInfo meth = JSProperty.GetSetMethod(prop, false);
        if (meth != null) {
          int n = (index == null ? 0 : index.Length);
          Object[] args = new Object[n+1];
          if (n > 0)
            ArrayObject.Copy(index, 0, args, 0, n); 
          args[n] = value;
          meth.Invoke(obj, BindingFlags.ExactBinding, null, args, null); 
          return;
        }
        throw new MissingMethodException();
      }
    
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      public override void SetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, Object[] index, CultureInfo culture) {
        MethodInfo setter = this.setter;
        JSObject jsOb = obj as JSObject;
        if (setter == null && jsOb != null){
          setter = jsOb.GetMethod("set_"+this.name, BindingFlags.Instance|BindingFlags.Public|BindingFlags.NonPublic);
          JSWrappedMethod wrm = setter as JSWrappedMethod;
          if (wrm != null) setter = wrm.method;
        }
        if (setter == null)
          setter = this.GetSetMethod(false);
        if (setter != null){
          if (index == null || index.Length == 0)
            setter.Invoke(obj, invokeAttr, binder, new Object[]{value}, culture);
          else{
            int n = index.Length;
            Object[] args = new Object[n+1];
            ArrayObject.Copy(index, 0, args, 0, n); 
            args[n] = value;
            setter.Invoke(obj, invokeAttr, binder, args, culture);
          }
        }
      }
    
    }
}
