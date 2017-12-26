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

// Makes a PropertyInfo look like a FieldInfo for use in compiled with statements

namespace Microsoft.JScript {
    
    using System;
    using System.Globalization;
    using System.Reflection;
    
    internal sealed class JSPropertyField : JSField{
      internal PropertyInfo wrappedProperty;
      internal Object wrappedObject;
    
      internal JSPropertyField(PropertyInfo field, Object obj){
        this.wrappedProperty = field;
        this.wrappedObject = obj;
      }
             
      public override String Name{
        get{
          return this.wrappedProperty.Name;
        }
      }
    
      public override FieldAttributes Attributes{
        get{
          return FieldAttributes.Public;
        }
      }
       
      public override Type DeclaringType{
        get{
          return this.wrappedProperty.DeclaringType;
        }
      }
    
      public override Type FieldType{
        get{
          return this.wrappedProperty.PropertyType;
        }
      }
      
      public override Object GetValue(Object obj){
        return this.wrappedProperty.GetValue(this.wrappedObject, new Object[0]);
      }
      
      public override void SetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, CultureInfo locale){
        this.wrappedProperty.SetValue(this.wrappedObject, value, invokeAttr, binder, new Object[0], locale);
      }
    }
}
