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
    
    internal sealed class JSClosureField : JSVariableField{
      internal FieldInfo field; //Used to indicate that the field is accessed from a nested (static) function.
    
      internal JSClosureField(FieldInfo field)
        : base(field.Name, null, field.Attributes|FieldAttributes.Static){ //Fool the analysis code to OK calls to this method from static code
        if (field is JSFieldInfo) field = ((JSFieldInfo)field).field;
        this.field = field;
      }
      
      public override Type DeclaringType{
        get{
          return this.field.DeclaringType;
        }
      }
    
      public override Type FieldType{
        get{
          return this.field.FieldType;
        }
      }
            
      internal override IReflect GetInferredType(JSField inference_target){
        if (this.field is JSMemberField)
          return ((JSMemberField)this.field).GetInferredType(inference_target);
        else
          return this.field.FieldType;
      }
    
      internal override Object GetMetaData(){
        if (this.field is JSField)
          return ((JSField)this.field).GetMetaData();
        else
          return this.field;
      }

      public override Object GetValue(Object obj){
        if (obj is StackFrame)
          return this.field.GetValue(((StackFrame)((StackFrame)obj).engine.ScriptObjectStackTop()).closureInstance);
        throw new JScriptException(JSError.InternalError); //should never happen
      }
      
      public override void SetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, CultureInfo locale){
        if (obj is StackFrame){
          this.field.SetValue(((StackFrame)((StackFrame)obj).engine.ScriptObjectStackTop()).closureInstance, value, invokeAttr, binder, locale);
          return;
        }
        throw new JScriptException(JSError.InternalError); //should never happen
      }
    }
}
