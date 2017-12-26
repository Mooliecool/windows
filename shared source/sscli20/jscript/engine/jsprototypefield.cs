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
An object with a prototype acts as if the properties of its prototype are its own. Any updates to the properties of the prototype are
visible via the object. However, if the property is assigned to via the object, the object makes a local copy of the property and all
further accesses via the object refer to the local copy, not the field of the prototype. Any further updates to the corresponding property
of the prototype will no longer be visible via the object. Likewise, updates via the object do not update the corresponding property on the prototype.

This class provides a wrapper for the fields of prototype properties that implements the desired behavior. When an object is queried for
a property and finds it in its prototype chain, it wraps the prototype property in one these and adds it to its field table. All future accesses
to the property resolve to this field.

Static fields and read only fields are not wrapped since they should not behave like instance fields.
*/

namespace Microsoft.JScript {
    
    using System;
    using System.Globalization;
    using System.Reflection;
    
    internal sealed class JSPrototypeField : JSField{
      private Object prototypeObject;
      internal FieldInfo prototypeField;
      internal Object value;
    
      internal JSPrototypeField(Object prototypeObject, FieldInfo prototypeField){
        this.prototypeObject = prototypeObject;
        this.prototypeField = prototypeField;
        this.value = Missing.Value;
      }
       
      public override FieldAttributes Attributes{
        get{
          return FieldAttributes.Public|FieldAttributes.Static;
        }
      }
      
      public override Object GetValue(Object obj){
        if (this.value is Missing) //The field has not been written to, defer to the prototype.
          return this.prototypeField.GetValue(this.prototypeObject);
        else
          return this.value;
      }
      
      public override String Name{
        get{
          return this.prototypeField.Name;
        }
      }
    
      public override void SetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, CultureInfo locale){
        this.value = value;
      }
    }
}
