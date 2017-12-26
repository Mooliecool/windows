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
    using Microsoft.JScript.Vsa;
    using System;
    using System.Reflection;
    using System.Reflection.Emit;
    using System.Text;
    using System.Globalization;
    
    public sealed class FunctionDeclaration : AST{
      internal FunctionObject func;
      private Member declaringObject = null;
      private TypeExpression ifaceId = null;
      private String name;
      internal bool isMethod;
      private bool inFastScope = false;
      private JSVariableField field = null;
      internal JSProperty enclosingProperty = null;
      private Completion completion = new Completion();
      
      internal FunctionDeclaration(Context context, AST ifaceId, IdentifierLiteral id, ParameterDeclaration[] formal_parameters, TypeExpression return_type, 
                                   Block body, FunctionScope own_scope, FieldAttributes attributes, 
                                   bool isMethod, bool isGetter, bool isSetter, bool isAbstract, bool isFinal, CustomAttributeList customAttributes)
        : base(context) {
        MethodAttributes methodAttributes = (MethodAttributes)0;
        if ((attributes & FieldAttributes.FieldAccessMask) == FieldAttributes.Public)
          methodAttributes = MethodAttributes.Public;
        else if ((attributes & FieldAttributes.FieldAccessMask) == FieldAttributes.Private)
          methodAttributes = MethodAttributes.Private;
        else if ((attributes & FieldAttributes.FieldAccessMask) == FieldAttributes.Assembly)
          methodAttributes = MethodAttributes.Assembly;
        else if ((attributes & FieldAttributes.FieldAccessMask) == FieldAttributes.Family)
          methodAttributes = MethodAttributes.Family;
        else if ((attributes & FieldAttributes.FieldAccessMask) == FieldAttributes.FamORAssem)
          methodAttributes = MethodAttributes.FamORAssem;
        else
          methodAttributes = MethodAttributes.Public;

        if ((attributes & FieldAttributes.Static) != 0 || !isMethod)
          methodAttributes |= MethodAttributes.Static;
        else
          methodAttributes |= MethodAttributes.Virtual | MethodAttributes.NewSlot;
        
        if (isAbstract)
          methodAttributes |= MethodAttributes.Abstract;
        if (isFinal)
          methodAttributes |= MethodAttributes.Final;
        
        this.name = id.ToString();
        this.isMethod = isMethod;
        if (ifaceId != null){
          if (isMethod){
            this.ifaceId = new TypeExpression(ifaceId);
            methodAttributes &= ~MethodAttributes.MemberAccessMask;
            methodAttributes |= MethodAttributes.Private|MethodAttributes.Final;
          }else{
            this.declaringObject = new Member(ifaceId.context, ifaceId, id);
            this.name = this.declaringObject.ToString();
          }
        }
        ScriptObject enclosingScope = Globals.ScopeStack.Peek();
        if (attributes == 0 && !isAbstract && !isFinal){
          if (enclosingScope is ClassScope)
            attributes |= FieldAttributes.Public;
        }else{
          if (!(enclosingScope is ClassScope)){
            this.context.HandleError(JSError.NotInsideClass);
            attributes = (FieldAttributes)0;
            methodAttributes = MethodAttributes.Public;
          }
        }
        if (enclosingScope is ActivationObject){
          this.inFastScope = ((ActivationObject)enclosingScope).fast;
          // if later on originalName != this.name this is a property getter/setter
          String originalName = this.name; 
          // mangle the name 
          if (isGetter){
            methodAttributes |= MethodAttributes.SpecialName;
            this.name = "get_" + this.name;
            if (return_type == null)
              return_type = new TypeExpression(new ConstantWrapper(Typeob.Object, context));
          }else if (isSetter){
            methodAttributes |= MethodAttributes.SpecialName;
            this.name = "set_" + this.name;
            return_type = new TypeExpression(new ConstantWrapper(Typeob.Void, context));
          }
          attributes &= FieldAttributes.FieldAccessMask;

          MethodAttributes access = methodAttributes & MethodAttributes.MemberAccessMask;
          if ((methodAttributes & MethodAttributes.Virtual) != (MethodAttributes)0 &&
              (methodAttributes & MethodAttributes.Final) == (MethodAttributes)0 &&
              (access == MethodAttributes.Private || access == MethodAttributes.Assembly || access == MethodAttributes.FamANDAssem))
            methodAttributes |= MethodAttributes.CheckAccessOnOverride;
              
          // create the function object
          this.func = new FunctionObject(this.name, formal_parameters, return_type, body, own_scope, enclosingScope, this.context, 
                                         methodAttributes, customAttributes, this.isMethod);
          if (this.declaringObject != null) return;
          // check whether the function name (possibly mangled) is in use already
          String fieldName = this.name;
          if (this.ifaceId != null) fieldName = ifaceId.ToString()+"."+fieldName;
          JSVariableField localField = (JSVariableField)((ActivationObject)enclosingScope).name_table[fieldName];
          if (localField != null && (!(localField is JSMemberField) || !(((JSMemberField)localField).value is FunctionObject) || this.func.isExpandoMethod)){
            if (originalName != this.name)
              localField.originalContext.HandleError(JSError.ClashWithProperty);
            else{
              id.context.HandleError(JSError.DuplicateName, this.func.isExpandoMethod);
              if (localField.value is FunctionObject)
                ((FunctionObject)localField.value).suppressIL = true;
            }
          }
          // create or update the proper field
          if (this.isMethod){
            if (!(localField is JSMemberField) || !(((JSMemberField)localField).value is FunctionObject) || originalName != this.name){
              this.field = ((ActivationObject)enclosingScope).AddNewField(fieldName, this.func, attributes|FieldAttributes.Literal);
              if (originalName == this.name) // if it is a property do not assign the type
                ((JSVariableField)this.field).type = new TypeExpression(new ConstantWrapper(Typeob.FunctionWrapper, this.context));
            }else
              this.field = ((JSMemberField)localField).AddOverload(this.func, attributes|FieldAttributes.Literal);
          }else if (enclosingScope is FunctionScope){
            if (this.inFastScope) attributes |= FieldAttributes.Literal;
            this.field = ((FunctionScope)enclosingScope).AddNewField(this.name, attributes, this.func);
            if (this.field is JSLocalField){
              JSLocalField locField = (JSLocalField)this.field;
              if (this.inFastScope){
                locField.type = new TypeExpression(new ConstantWrapper(Typeob.ScriptFunction, this.context));
                locField.attributeFlags |= FieldAttributes.Literal;
              }
              locField.debugOn = this.context.document.debugOn;
              locField.isDefined = true;
            }
          }else if (this.inFastScope){
            this.field = ((ActivationObject)enclosingScope).AddNewField(this.name, this.func, attributes|FieldAttributes.Literal);
            ((JSVariableField)this.field).type = new TypeExpression(new ConstantWrapper(Typeob.ScriptFunction, this.context));
            //Do not use typeof(Closure) for the field, since that has the arguments and callee properties, which are not
            //accessible in fast mode
          }else //enclosingScope is GlobalObject
            this.field = ((ActivationObject)enclosingScope).AddNewField(this.name, this.func, attributes|FieldAttributes.Static);
          ((JSVariableField)this.field).originalContext = context;
          
          // if it is a property create/update the PropertyInfo and assign the getter/setter
          if (originalName != this.name){
            String propertyFieldName = originalName;
            if (this.ifaceId != null) propertyFieldName = ifaceId.ToString()+"."+originalName;
            FieldInfo prop = (FieldInfo)((ClassScope)enclosingScope).name_table[propertyFieldName];
            if (prop != null){
              // check whether a property was defined already
              if (prop.IsLiteral){
                Object val = ((JSVariableField)prop).value;
                if (val is JSProperty)
                  this.enclosingProperty = (JSProperty)val;
              }
              if (this.enclosingProperty == null)
                id.context.HandleError(JSError.DuplicateName, true); // the matching name was not a property
            }
            if (this.enclosingProperty == null){
              this.enclosingProperty = new JSProperty(originalName);
              prop = ((ActivationObject)enclosingScope).AddNewField(propertyFieldName, this.enclosingProperty, attributes|FieldAttributes.Literal);
              ((JSMemberField)prop).originalContext = this.context;
            }else{
              if ((isGetter && this.enclosingProperty.getter != null) || (isSetter && this.enclosingProperty.setter != null))
                id.context.HandleError(JSError.DuplicateName, true); // duplicated setter or getter
            }
            if (isGetter)
              this.enclosingProperty.getter = new JSFieldMethod(this.field, enclosingScope);
            else
              this.enclosingProperty.setter = new JSFieldMethod(this.field, enclosingScope);
          }
        }else{ //Might get here if function declaration is inside of an eval.
          this.inFastScope = false;
          this.func = new FunctionObject(this.name, formal_parameters, return_type, body, own_scope, enclosingScope, this.context, MethodAttributes.Public, null, false);
          this.field = ((StackFrame)enclosingScope).AddNewField(this.name, new Closure(this.func), attributes|FieldAttributes.Static);
        }
      }
      
      internal override Object Evaluate(){
        if (this.declaringObject != null)
          this.declaringObject.SetValue(this.func);
        return this.completion;
      }
      
      public static Closure JScriptFunctionDeclaration(RuntimeTypeHandle handle, String name, String method_name, String[] formal_parameters, 
        JSLocalField[] fields, bool must_save_stack_locals, bool hasArgumentsObject, String text, Object declaringObject, VsaEngine engine){
        Type t = Type.GetTypeFromHandle(handle);
        FunctionObject func = new FunctionObject(t, name, method_name, formal_parameters, fields, must_save_stack_locals, hasArgumentsObject, text, engine);
        return new Closure(func, declaringObject);
      }

      internal override Context GetFirstExecutableContext(){
        return null;
      }
      
      internal override AST PartiallyEvaluate(){
        if (this.ifaceId != null){
          this.ifaceId.PartiallyEvaluate();
          this.func.implementedIface = this.ifaceId.ToIReflect();
          Type t = this.func.implementedIface as Type;
          ClassScope csc = this.func.implementedIface as ClassScope;
          if (t != null && !t.IsInterface || csc != null && !csc.owner.isInterface){
            this.ifaceId.context.HandleError(JSError.NeedInterface);
            this.func.implementedIface = null;
          }
          if ((this.func.attributes & MethodAttributes.Abstract) != 0)
            this.func.funcContext.HandleError(JSError.AbstractCannotBePrivate);
        }else if (this.declaringObject != null)
          this.declaringObject.PartiallyEvaluateAsCallable();
        this.func.PartiallyEvaluate();
        if (this.inFastScope && this.func.isExpandoMethod && this.field != null && this.field.type != null)
          this.field.type.expression = new ConstantWrapper(Typeob.ScriptFunction, null);
        if ((this.func.attributes & MethodAttributes.Abstract) != 0 && 
          !((ClassScope)this.func.enclosing_scope).owner.isAbstract){
          ((ClassScope)this.func.enclosing_scope).owner.attributes |= TypeAttributes.Abstract;
          ((ClassScope)this.func.enclosing_scope).owner.context.HandleError(JSError.CannotBeAbstract, this.name);
        }
        if (this.enclosingProperty != null)
          if (!this.enclosingProperty.GetterAndSetterAreConsistent())
            this.context.HandleError(JSError.GetAndSetAreInconsistent);
        return this;
      }

      private void TranslateToILClosure(ILGenerator il){
        if (!this.func.isStatic)
          il.Emit(OpCodes.Ldarg_0);
        il.Emit(OpCodes.Ldtoken, this.func.classwriter != null ? this.func.classwriter : compilerGlobals.classwriter);
        il.Emit(OpCodes.Ldstr, this.name);
        il.Emit(OpCodes.Ldstr, this.func.GetName());
        int n = this.func.formal_parameters.Length;
        ConstantWrapper.TranslateToILInt(il, n);
        il.Emit(OpCodes.Newarr, Typeob.String);
        for (int i = 0; i < n; i++){
          il.Emit(OpCodes.Dup);
          ConstantWrapper.TranslateToILInt(il, i);
          il.Emit(OpCodes.Ldstr, this.func.formal_parameters[i]);
          il.Emit(OpCodes.Stelem_Ref);
        }
        n = this.func.fields.Length;
        ConstantWrapper.TranslateToILInt(il, n);
        il.Emit(OpCodes.Newarr, Typeob.JSLocalField);
        for (int i = 0; i < n; i++){
          JSLocalField field = this.func.fields[i];
          il.Emit(OpCodes.Dup);
          ConstantWrapper.TranslateToILInt(il, i);
          il.Emit(OpCodes.Ldstr, field.Name);
          il.Emit(OpCodes.Ldtoken, field.FieldType);
          ConstantWrapper.TranslateToILInt(il, field.slotNumber);
          il.Emit(OpCodes.Newobj, CompilerGlobals.jsLocalFieldConstructor);
          il.Emit(OpCodes.Stelem_Ref);
        }
        if (this.func.must_save_stack_locals)
          il.Emit(OpCodes.Ldc_I4_1);
        else
          il.Emit(OpCodes.Ldc_I4_0);
        if (this.func.hasArgumentsObject)
          il.Emit(OpCodes.Ldc_I4_1);
        else
          il.Emit(OpCodes.Ldc_I4_0);
        il.Emit(OpCodes.Ldstr, this.func.ToString());
        if (!this.func.isStatic)
          il.Emit(OpCodes.Ldarg_0);
        else
          il.Emit(OpCodes.Ldnull);
        this.EmitILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.jScriptFunctionDeclarationMethod);
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        Debug.PreCondition(rtype == Typeob.Void);
        return;
      }

      internal override void TranslateToILInitializer(ILGenerator il){
        if (this.func.suppressIL) return;
        this.func.TranslateToIL(compilerGlobals);
        if (this.declaringObject != null){
          this.declaringObject.TranslateToILInitializer(il);
          this.declaringObject.TranslateToILPreSet(il);
          this.TranslateToILClosure(il);
          this.declaringObject.TranslateToILSet(il);
          return;
        }
        Object tok = ((JSVariableField)this.field).metaData;
        if (this.func.isMethod){
          if (tok is FunctionDeclaration){
            ((JSVariableField)this.field).metaData = null;
            return;
          }
          this.TranslateToILSourceTextProvider();
          return;
        }
        if (tok == null) return;
        this.TranslateToILClosure(il);
        if (tok is LocalBuilder)
          il.Emit(OpCodes.Stloc, (LocalBuilder)tok);
        else if (this.func.isStatic)
          il.Emit(OpCodes.Stsfld, (FieldInfo)tok);
        else
          il.Emit(OpCodes.Stfld, (FieldInfo)tok);
      }
      
      private void TranslateToILSourceTextProvider(){
        if (this.Engine.doFast) return;
        
        // If the field name doesn't match this name, then don't emit the stub for source
        // text provider. The function is a private method impl and there is no way to get 
        // a handle to the function. i.e. name="Bar", field.name="InterfaceA.Bar"
        if (0 != String.Compare(this.name, this.field.Name, StringComparison.Ordinal))
          return;
          
        StringBuilder sb = new StringBuilder(this.func.ToString());
        JSMemberField mf = ((JSMemberField)this.field).nextOverload;
        while (mf != null){
          mf.metaData = this;
          sb.Append('\n');
          sb.Append(mf.value.ToString());
          mf = mf.nextOverload;
        }
        MethodAttributes attr = MethodAttributes.Public|MethodAttributes.Static;
        MethodBuilder mb = ((ClassScope)(this.func.enclosing_scope)).GetTypeBuilder().DefineMethod(this.name+" source", attr, Typeob.String, new Type[0]);
        ILGenerator il = mb.GetILGenerator();
        il.Emit(OpCodes.Ldstr, sb.ToString());
        il.Emit(OpCodes.Ret);
      }
    }
}
