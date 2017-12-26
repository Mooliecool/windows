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

namespace Microsoft.JScript{
    
    using System;
    using System.Reflection;
    using System.Globalization;
    
    internal class JSPropertyInfo : PropertyInfo{
      private PropertyInfo property;
      private Type declaringType = null;
      internal MethodInfo getter = null;
      internal MethodInfo setter = null;
    
      internal JSPropertyInfo(PropertyInfo property){
        this.property = property;
      }
      
      public override PropertyAttributes Attributes{
        get{
          return this.property.Attributes;
        }
      }
      
      public override bool CanRead{
        get{
          return this.property.CanRead;
        }
      }
      
      public override bool CanWrite{
        get{
          return this.property.CanWrite;
        }
      }
      
      public override Type DeclaringType{
        get{
          Type result = this.declaringType;
          if (result == null) this.declaringType = result = this.property.DeclaringType;
          return result;
        }
      }                                         
      
      public override MethodInfo GetGetMethod(bool nonPublic){
        MethodInfo getter = this.getter;
        if (getter == null){
          getter = this.property.GetGetMethod(nonPublic);
          if (getter != null)
            getter = new JSMethodInfo(getter);
          this.getter = getter;
        }
        return getter;
      }
      
      public override ParameterInfo[] GetIndexParameters(){
        MethodInfo getter = this.GetGetMethod(false);
        if (getter != null) return getter.GetParameters();
        return this.property.GetIndexParameters();
      }
      
      public override MethodInfo GetSetMethod(bool nonPublic){
        MethodInfo setter = this.setter;
        if (setter == null){
          setter = this.property.GetSetMethod(nonPublic);
          if (setter != null)
            setter = new JSMethodInfo(setter);
          this.setter = setter;
        }
        return setter;
      }
      
      public override MethodInfo[] GetAccessors(bool nonPublic){
        throw new JScriptException(JSError.InternalError);
      }
      
      public override Object[] GetCustomAttributes(Type t, bool inherit){
        return CustomAttribute.GetCustomAttributes(this.property, t, inherit);
      }
      
      public override Object[] GetCustomAttributes(bool inherit){
        return this.property.GetCustomAttributes(inherit);
      }

      public override Object GetValue(Object obj, BindingFlags invokeAttr, Binder binder, Object[] index, CultureInfo culture){
        return this.GetGetMethod(false).Invoke(obj, invokeAttr, binder, index == null ? new Object[0] : index, culture);
      }
      
      public override bool IsDefined(Type type, bool inherit){ 
        return CustomAttribute.IsDefined(this.property, type, inherit);
      }
      
      public override String Name{
        get{
          return this.property.Name;
        }
      }
      
      public override Type PropertyType{
        get{
          return this.property.PropertyType;
        }
      }
      
      public override Type ReflectedType{
        get{
          return this.property.ReflectedType;
        }
      }
    
      public override void SetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, Object[] index, CultureInfo culture) {
        if (index == null || index.Length == 0)
          this.GetSetMethod(false).Invoke(obj, invokeAttr, binder, new Object[]{value}, culture);
        else{
          int n = index.Length;
          Object[] args = new Object[n+1];
          ArrayObject.Copy(index, 0, args, 0, n); 
          args[n] = value;
          this.GetSetMethod(false).Invoke(obj, invokeAttr, binder, args, culture);
        }
      }
    
    }
}