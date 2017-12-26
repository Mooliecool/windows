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
    using System.Diagnostics;
    
    internal class JSWrappedProperty : PropertyInfo, IWrappedMember{
      internal Object obj;
      internal PropertyInfo property;
    
      internal JSWrappedProperty(PropertyInfo property, Object obj){
        this.obj = obj;
        this.property = property;
        if (obj is JSObject){
          Type pt = property.DeclaringType;
          if (pt == Typeob.Object || pt == Typeob.String || pt.IsPrimitive || pt == Typeob.Array){
            if (obj is BooleanObject) this.obj = ((BooleanObject)obj).value;
            else if (obj is NumberObject) this.obj = ((NumberObject)obj).value;
            else if (obj is StringObject) this.obj = ((StringObject)obj).value;
            else if (obj is ArrayWrapper) this.obj = ((ArrayWrapper)obj).value;
          }
        }
      }
      
      internal virtual String GetClassFullName(){
        if (this.property is JSProperty)
          return ((JSProperty)this.property).GetClassFullName();
        else
          return this.property.DeclaringType.FullName;
      }
            
      public override MemberTypes MemberType{
        get{
          return MemberTypes.Property;
        }
      }
      
      public override String Name{
        get{
          if (this.obj is LenientGlobalObject && this.property.Name.StartsWith("Slow", StringComparison.Ordinal))
            return this.property.Name.Substring(4);
          else 
            return this.property.Name;
        }
      }
      
      public override Type DeclaringType{
        get{
          return this.property.DeclaringType;
        }
      }                                         
      
      public override Type ReflectedType{
        get{
          return this.property.ReflectedType;
        }
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
    
      public override Object[] GetCustomAttributes(Type t, bool inherit){
        return CustomAttribute.GetCustomAttributes(this.property, t, inherit);
      }
      
      public override Object[] GetCustomAttributes(bool inherit){
        return this.property.GetCustomAttributes(inherit);
      }

#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      public override Object GetValue(Object obj, BindingFlags invokeAttr, Binder binder, Object[] index, CultureInfo culture) {
        return this.property.GetValue(this.obj, invokeAttr, binder, index, culture);
      }
      
#if !DEBUG
      [DebuggerStepThroughAttribute]
      [DebuggerHiddenAttribute]
#endif      
      public override void SetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, Object[] index, CultureInfo culture) {
        this.property.SetValue(this.obj, value, invokeAttr, binder, index, culture);
      }
    
      public override MethodInfo[] GetAccessors(bool nonPublic){
        return this.property.GetAccessors(nonPublic);
      }
      
      public override MethodInfo GetGetMethod(bool nonPublic){
        MethodInfo meth = JSProperty.GetGetMethod(this.property, nonPublic);
        if (meth == null) return null;
        return new JSWrappedMethod(meth, this.obj);
      }
      
      public override ParameterInfo[] GetIndexParameters(){
        return this.property.GetIndexParameters();
      }
    
      public override MethodInfo GetSetMethod(bool nonPublic){
        MethodInfo meth = JSProperty.GetSetMethod(this.property, nonPublic);
        if (meth == null) return null;
        return new JSWrappedMethod(meth, this.obj);
      }
      
      public Object GetWrappedObject(){
        return this.obj;
      }
      
      public override Type PropertyType{
        get{
          return this.property.PropertyType;
        }
      }
            
      public override bool IsDefined(Type type, bool inherit){ 
        return CustomAttribute.IsDefined(this.property, type, inherit);
      }
    }
}
