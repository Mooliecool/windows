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

/*
This class allows JSObjects to return fields that are not their own, as if they were. Think of it as a field "delegate".
*/


namespace Microsoft.JScript {
    
    using System;
    using System.Globalization;
    using System.Reflection;
    
    internal sealed class JSWrappedField : JSField, IWrappedMember{
      internal FieldInfo wrappedField;
      internal Object wrappedObject;
    
      internal JSWrappedField(FieldInfo field, Object obj){
        if (field is JSFieldInfo) field = ((JSFieldInfo)field).field;
        this.wrappedField = field;
        this.wrappedObject = obj;
        if (obj is JSObject && !Typeob.JSObject.IsAssignableFrom(field.DeclaringType))
          if (obj is BooleanObject) this.wrappedObject = ((BooleanObject)obj).value;
          else if (obj is NumberObject) this.wrappedObject = ((NumberObject)obj).value;
          else if (obj is StringObject) this.wrappedObject = ((StringObject)obj).value;
      }
             
      public override String Name{
        get{
          return this.wrappedField.Name;
        }
      }
    
      public override FieldAttributes Attributes{
        get{
          return this.wrappedField.Attributes;
        }
      }
       
      public override Type DeclaringType{
        get{
          return this.wrappedField.DeclaringType;
        }
      }
    
      public override Type FieldType{
        get{
          return this.wrappedField.FieldType;
        }
      }
      
      internal override String GetClassFullName(){
        if (this.wrappedField is JSField)
          return ((JSField)this.wrappedField).GetClassFullName();
        else
          return this.wrappedField.DeclaringType.FullName;
      }
            
      internal override Object GetMetaData(){
        if (this.wrappedField is JSField)
          return ((JSField)this.wrappedField).GetMetaData();
        else
          return this.wrappedField;
      }
      
      internal override PackageScope GetPackage(){
        if (this.wrappedField is JSField){
          return ((JSField)this.wrappedField).GetPackage();
        }
        return null;
      }
            
      public override Object GetValue(Object obj){
        return this.wrappedField.GetValue(this.wrappedObject);
      }
      
      public Object GetWrappedObject(){
        return this.wrappedObject;
      }
      
      public override void SetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, CultureInfo locale){
        this.wrappedField.SetValue(this.wrappedObject, value, invokeAttr, binder, locale);
      }
    }
}
