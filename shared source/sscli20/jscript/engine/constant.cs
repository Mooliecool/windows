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

    internal sealed class Constant : AST{
      internal FieldAttributes attributes;
      private Completion completion;
      internal CustomAttributeList customAttributes;
      internal JSVariableField field;
      private FieldBuilder valueField;
      private Lookup identifier;
      internal String name;
      internal AST value;
    
      internal Constant(Context context, Lookup identifier, TypeExpression type, AST value, FieldAttributes attributes, CustomAttributeList customAttributes)
        : base(context){
        this.attributes = attributes | FieldAttributes.InitOnly;
        this.customAttributes = customAttributes;
        this.completion = new Completion();
        this.identifier = identifier;
        this.name = identifier.ToString();
        this.value = value;
        ScriptObject current_scope = (ScriptObject)Globals.ScopeStack.Peek();
        while (current_scope is WithObject) //Can only happen at run time and only if there is an eval
          current_scope = current_scope.GetParent();
        if (current_scope is ClassScope){
          if (this.name == ((ClassScope)current_scope).name){
            identifier.context.HandleError(JSError.CannotUseNameOfClass);
            this.name = this.name + " const";
          }
          if (attributes == 0) attributes = FieldAttributes.Public;
        }else{
          if (attributes != 0)
            this.context.HandleError(JSError.NotInsideClass);
          attributes = FieldAttributes.Public;
        }
        FieldInfo field = ((IActivationObject)current_scope).GetLocalField(this.name);
        if (field != null){
          identifier.context.HandleError(JSError.DuplicateName, true);
          this.name = this.name + " const";
        }
        if (current_scope is ActivationObject)
          this.field = ((ActivationObject)current_scope).AddNewField(this.identifier.ToString(), value, attributes);
        else
          this.field = ((StackFrame)current_scope).AddNewField(this.identifier.ToString(), value, attributes|FieldAttributes.Static);
        this.field.type = type;
        this.field.customAttributes = customAttributes;
        this.field.originalContext = context;
        if (this.field is JSLocalField)
          // emit debug info for the local only if this block of code is in a section that has debug set
          ((JSLocalField)this.field).debugOn = this.identifier.context.document.debugOn;
      }
      
      internal override Object Evaluate(){
        if (this.value == null) 
          this.completion.value = this.field.value;
        else
          this.completion.value = this.value.Evaluate();
        return this.completion;
      }
      
      internal override AST PartiallyEvaluate(){
        this.field.attributeFlags &= ~FieldAttributes.InitOnly;
        this.identifier.PartiallyEvaluateAsReference();
        if (this.field.type != null) 
          this.field.type.PartiallyEvaluate();
        ScriptObject scope = (ScriptObject)Globals.ScopeStack.Peek();
        if (this.value != null){
          this.value = this.value.PartiallyEvaluate();
          this.identifier.SetPartialValue(this.value);
          if (this.value is ConstantWrapper){
            Object val = this.field.value = this.value.Evaluate();
            if (this.field.type != null) this.field.value = Convert.Coerce(val, this.field.type, true);
            if (this.field.IsStatic && (val is Type || val is ClassScope || val is TypedArray ||
                Convert.GetTypeCode(val) != TypeCode.Object)){
              this.field.attributeFlags |= FieldAttributes.Literal;
              goto set_field_type;
            }
          }
          this.field.attributeFlags |= FieldAttributes.InitOnly;
        set_field_type:
          if (this.field.type == null)
            this.field.type = new TypeExpression(new ConstantWrapper(this.value.InferType(null), null));
        }else{
          this.value = new ConstantWrapper(null, this.context);
          this.field.attributeFlags |= FieldAttributes.InitOnly;
        }
        // deal with custom attributes
        if (this.field != null && this.field.customAttributes != null)
          this.field.customAttributes.PartiallyEvaluate();
        return this;
      }

      internal override void TranslateToIL(ILGenerator il, Type rtype){
        if ((this.field.attributeFlags & FieldAttributes.Literal) != 0){
          Object val = this.field.value;
          if (val is Type || val is ClassScope || val is TypedArray){
            this.field.attributeFlags &= ~FieldAttributes.Literal;
            this.identifier.TranslateToILPreSet(il);
            this.identifier.TranslateToILSet(il, new ConstantWrapper(val, null));
            this.field.attributeFlags |= FieldAttributes.Literal;
          }
        }else{
          if (!this.field.IsStatic){
            FieldBuilder fb = this.valueField = this.field.metaData as FieldBuilder;
            if (fb != null)
              this.field.metaData = ((TypeBuilder)fb.DeclaringType).DefineField(this.name+" value", this.field.type.ToType(), FieldAttributes.Private);
          }
          this.field.attributeFlags &= ~FieldAttributes.InitOnly;
          this.identifier.TranslateToILPreSet(il);
          this.identifier.TranslateToILSet(il, this.value);
          this.field.attributeFlags |= FieldAttributes.InitOnly;
        }
      }

      internal void TranslateToILInitOnlyInitializers(ILGenerator il){
        FieldBuilder fb = this.valueField;
        if (fb != null){
          il.Emit(OpCodes.Ldarg_0);
          il.Emit(OpCodes.Dup);
          il.Emit(OpCodes.Ldfld, (FieldBuilder)this.field.metaData);
          il.Emit(OpCodes.Stfld, fb);
          this.valueField = (FieldBuilder)this.field.metaData;
          this.field.metaData = fb;
        }
      }

      internal override void TranslateToILInitializer(ILGenerator il){
        if (this.value != null)
          this.value.TranslateToILInitializer(il);
      }
      
    }
}
