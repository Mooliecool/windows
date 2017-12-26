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
    
    internal sealed class VariableDeclaration : AST{
      internal Lookup identifier;
      private TypeExpression type;
      internal AST initializer;
      internal JSVariableField field;
      private Completion completion;
      
      internal VariableDeclaration(Context context, Lookup identifier, TypeExpression type, AST initializer, FieldAttributes attributes, CustomAttributeList customAttributes)
        : base(context) {
        if (initializer != null)
          this.context.UpdateWith(initializer.context);
        else if (type != null)
          this.context.UpdateWith(type.context);
        this.identifier = identifier;
        this.type = type;
        this.initializer = initializer;
        ScriptObject current_scope = (ScriptObject)Globals.ScopeStack.Peek();
        while (current_scope is WithObject) //Can only happen at run time and only if there is an eval
          current_scope = current_scope.GetParent();
        String name = this.identifier.ToString();
        if (current_scope is ClassScope){
          if (name == ((ClassScope)current_scope).name){
            identifier.context.HandleError(JSError.CannotUseNameOfClass);
            name = name + " var";
          }
        }else{
          if (attributes != (FieldAttributes)0){
            this.context.HandleError(JSError.NotInsideClass);
            attributes = FieldAttributes.Public;
          }else
            attributes |= FieldAttributes.Public;
        }
        FieldInfo field = ((IActivationObject)current_scope).GetLocalField(name);
        if (field != null){
          if (field.IsLiteral || current_scope is ClassScope || type != null)
            identifier.context.HandleError(JSError.DuplicateName, true);
          this.type = type = null;
        }
        if (current_scope is ActivationObject)
          if (field == null || field is JSVariableField)
            this.field = ((ActivationObject)current_scope).AddFieldOrUseExistingField(this.identifier.ToString(), Missing.Value, attributes);
          else
            this.field = ((ActivationObject)current_scope).AddNewField(this.identifier.ToString(), null, attributes);
        else
          this.field = ((StackFrame)current_scope).AddNewField(this.identifier.ToString(), null, attributes|FieldAttributes.Static);
        this.field.type = type;
        this.field.customAttributes = customAttributes;
        this.field.originalContext = context;
        if (this.field is JSLocalField)
          // emit debug info for the local only if this block of code is in a section that has debug set
          ((JSLocalField)this.field).debugOn = this.identifier.context.document.debugOn;
        this.completion = new Completion();
      }
    
      internal override Object Evaluate(){
        ScriptObject scope = Globals.ScopeStack.Peek();
        Object value = null;
        if (this.initializer != null)
          value = this.initializer.Evaluate();
        if (this.type != null)
          value = Convert.Coerce(value, this.type);
        else{
          while (scope is BlockScope) scope = scope.GetParent();
          if (scope is WithObject)
            this.identifier.SetWithValue((WithObject)scope, value);
          while (scope is WithObject || scope is BlockScope)
            scope = scope.GetParent();
          if (this.initializer == null && !(this.field.value is Missing)){
            this.completion.value = this.field.value;
            return this.completion;
          }
        }
        this.field.SetValue(scope, this.completion.value = value);
        return this.completion;
      }
       
      internal override AST PartiallyEvaluate(){
        AST id = this.identifier = (Lookup)this.identifier.PartiallyEvaluateAsReference();
        if (this.type != null)
          this.field.type = this.type = (TypeExpression)this.type.PartiallyEvaluate();
        else if (this.initializer == null && !(this.field is JSLocalField) &&
        this.field.value is Missing){
          id.context.HandleError(JSError.VariableLeftUninitialized);
          this.field.type = this.type = new TypeExpression(new ConstantWrapper(Typeob.Object, id.context));
        }
        if (this.initializer != null){
          if (this.field.IsStatic){
            ScriptObject scope = this.Engine.ScriptObjectStackTop();
            ClassScope cscope = null;
            while (scope != null && (cscope = scope as ClassScope) == null) scope = scope.GetParent();
            if (cscope != null) cscope.inStaticInitializerCode = true;
            this.initializer = this.initializer.PartiallyEvaluate();
            if (cscope != null) cscope.inStaticInitializerCode = false;
          }else
            this.initializer = this.initializer.PartiallyEvaluate();
          id.SetPartialValue(this.initializer);
        }
        // deal with custom attributes
        if (this.field != null && this.field.customAttributes != null)
          this.field.customAttributes.PartiallyEvaluate();
        return this;
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        //This assumes that rtype == Void.class
          
        //If the initializer is missing. Do nothing, we're done.
        if (this.initializer == null)
          return;
        
        // this.initializer.context can be null when a default initializer is forced in the variable (i.e. for_in case)
        if (this.context.document.debugOn && this.initializer.context != null)
          this.context.EmitLineInfo(il);

        Lookup id = (Lookup)this.identifier;
        id.TranslateToILPreSet(il, true);
        id.TranslateToILSet(il, true, this.initializer);
      }
        
      internal override void TranslateToILInitializer(ILGenerator il){
        if (this.type != null)
          this.type.TranslateToILInitializer(il);
        if (this.initializer != null)
          this.initializer.TranslateToILInitializer(il);
      }
      
      internal override Context GetFirstExecutableContext(){
        if (this.initializer == null)
          return null;
        else
          return this.context;
      }

    
    }
}
