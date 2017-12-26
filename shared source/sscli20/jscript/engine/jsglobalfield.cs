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
    using System.Globalization;
    using System.Reflection;
    
    internal sealed class JSGlobalField : JSVariableField{
      internal FieldInfo ILField; //Used for named items

      internal JSGlobalField(ScriptObject obj, String name, Object value, FieldAttributes attributeFlags)
        : base(name, obj, attributeFlags) {
        this.value = value;
        this.ILField = null;
      }
      
      public override Object GetValue(Object obj){
        if (this.ILField == null) return this.value;
        return this.ILField.GetValue(null);
      }

      public override void SetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, CultureInfo culture){
        if (this.ILField != null){
          this.ILField.SetValue(null, value, invokeAttr, binder, culture);
          return;
        }
        if ((this.IsLiteral || this.IsInitOnly) && !(this.value is Missing)){
          if (this.value is FunctionObject && value is FunctionObject && this.Name.Equals(((FunctionObject)value).name)){
            this.value = value;
            return;
          }
          throw new JScriptException(JSError.AssignmentToReadOnly);
        }
        if (this.type != null)
          this.value = Convert.Coerce(value, this.type);
        else
          this.value = value;
      }
    }
}
