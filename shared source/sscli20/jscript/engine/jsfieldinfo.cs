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
    using System.Collections;
    using System.Globalization;
    using System.Reflection;
    
    public sealed class JSFieldInfo : FieldInfo{
      internal FieldInfo field;
      private FieldAttributes attributes;
      private Type declaringType = null;
      private Type fieldType = null;
      private FieldAccessor fieldAccessor = null;
      
      internal JSFieldInfo(FieldInfo field){
        this.field = field;
        this.attributes = field.Attributes;
      }
      
      public override FieldAttributes Attributes{
        get{
          return this.attributes;
        }
      }
       
      public override Type DeclaringType{
        get{
          Type result = this.declaringType;
          if (result == null)
            this.declaringType = result = this.field.DeclaringType;
          return result;
        }
      }
    
      public override RuntimeFieldHandle FieldHandle{
        get{
          return this.field.FieldHandle;
        }
      }
      
      public override Type FieldType{
        get{
          Type result = this.fieldType;
          if (result == null)
            this.fieldType = result = this.field.FieldType;
          return result;
        }
      }
      
      public override Object[] GetCustomAttributes(Type t, bool inherit){
        return new FieldInfo[0];
      }

      public override Object[] GetCustomAttributes(bool inherit){
        return new FieldInfo[0];
      }
      
      public override Object GetValue(Object obj){
        FieldAccessor accessor = this.fieldAccessor;
        if (accessor == null)
          this.fieldAccessor = accessor = FieldAccessor.GetAccessorFor(TypeReferences.ToExecutionContext(this.field));
        return accessor.GetValue(obj);
      }
    
      public override bool IsDefined(Type type, bool inherit){ 
        return false;
      }

      public override MemberTypes MemberType{
        get{
          return MemberTypes.Field;
        }
      }
      
      public override String Name{
        get{
          return this.field.Name;
        }
      }
    
      public override Type ReflectedType{
        get{
          return this.field.ReflectedType;
        }
      }

      public new void SetValue(Object obj, Object value){
        if ((this.attributes & FieldAttributes.InitOnly) != 0)
          throw new JScriptException(JSError.AssignmentToReadOnly);
        this.SetValue(obj, value, BindingFlags.SetField, null, null);
      }
      
      public override void SetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, CultureInfo culture){
        Debug.PreCondition(binder == null && culture == null);
        FieldAccessor accessor = this.fieldAccessor;
        if (accessor == null)
          this.fieldAccessor = accessor = FieldAccessor.GetAccessorFor(this.field);
        accessor.SetValue(obj, value);
      }
     
    }
}
