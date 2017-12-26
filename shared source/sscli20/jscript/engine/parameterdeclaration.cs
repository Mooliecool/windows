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
    
    internal sealed class ParameterDeclaration : ParameterInfo{
      internal String identifier;
      internal TypeExpression type;
      internal Context context;
      internal CustomAttributeList customAttributes;
      
      internal ParameterDeclaration(Context context, String identifier, TypeExpression type, CustomAttributeList customAttributes)
        : base() {
        this.identifier = identifier;
        this.type = type == null ? new TypeExpression(new ConstantWrapper(Typeob.Object, context)) : type;
        this.context = context;
        ActivationObject current_scope = (ActivationObject)context.document.engine.Globals.ScopeStack.Peek();
        if (current_scope.name_table[this.identifier] != null)
          //Only happens if there is another parameter declarations with the same name
          context.HandleError(JSError.DuplicateName, this.identifier, current_scope is ClassScope || current_scope.fast || type != null);
        else{
          JSVariableField field = current_scope.AddNewField(this.identifier, null, 0);
          field.originalContext = context;
        }
        this.customAttributes = customAttributes;
      }

      internal ParameterDeclaration(Type type, String identifier)
        : base(){
        this.identifier = identifier;
        this.type = new TypeExpression(new ConstantWrapper(type, null));
        this.customAttributes = null;
      }
    
      public override Object DefaultValue{
        get{
          return System.Convert.DBNull;
        }
      }
    
      public override Object[] GetCustomAttributes(bool inherit){
        return new FieldInfo[0];
      }
      
      public override Object[] GetCustomAttributes(Type attributeType, bool inherit){
        return new FieldInfo[0];
      }
      
      public override bool IsDefined(Type attributeType, bool inherit){
        if (this.customAttributes != null)
          return this.customAttributes.GetAttribute(attributeType) != null;
        else
          return false;
      }
      
      public override String Name{
        get{
          return this.identifier;
        }
      }
      
      internal IReflect ParameterIReflect{
        get{
          return this.type.ToIReflect();
        }
      }
    
      public override Type ParameterType{
        get{
          Type result = this.type.ToType();
          if (result == Typeob.Void) result = Typeob.Object;
          return result;
        }
      }
      
      internal void PartiallyEvaluate(){
        if (this.type != null)
          this.type.PartiallyEvaluate();
        if (this.customAttributes != null){
          this.customAttributes.PartiallyEvaluate();
          if (CustomAttribute.IsDefined(this, typeof(ParamArrayAttribute), false)){
            if (this.type != null){
              IReflect ir = this.type.ToIReflect();
              if ((ir is Type && ((Type)ir).IsArray) || (ir is TypedArray)) return;
            }
            this.customAttributes.context.HandleError(JSError.IllegalParamArrayAttribute);
          }
        }
      }
      
    }
}
