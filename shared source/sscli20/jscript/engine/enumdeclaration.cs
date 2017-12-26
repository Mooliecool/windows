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

  internal sealed class EnumDeclaration : Class{
    internal TypeExpression baseType;
    
    internal EnumDeclaration(Context context, IdentifierLiteral id, TypeExpression baseType, Block body, FieldAttributes attributes, CustomAttributeList customAttributes)
      : base(context, id, new TypeExpression(new ConstantWrapper(Typeob.Enum, null)), new TypeExpression[0], body, attributes, false, false, true, false, customAttributes){
      this.baseType = baseType != null ? baseType : new TypeExpression(new ConstantWrapper(Typeob.Int32, null));
      this.needsEngine = false;
      this.attributes &= TypeAttributes.VisibilityMask;
      TypeExpression thisType = new TypeExpression(new ConstantWrapper(this.classob, this.context));
      AST currentValue = new ConstantWrapper(-1, null);
      AST one = new ConstantWrapper(1, null);
      foreach (FieldInfo f in this.fields){
        JSVariableField field = (JSVariableField)f;
        field.attributeFlags = FieldAttributes.Public|FieldAttributes.Static|FieldAttributes.Literal;
        field.type = thisType;
        if (field.value == null)
          field.value = currentValue = new Plus(currentValue.context, currentValue, one);
        else
          currentValue = (AST)field.value;
        field.value = new DeclaredEnumValue(field.value, field.Name, this.classob);
      }
    }
      
    internal override AST PartiallyEvaluate(){
      if (!(this.classob.GetParent() is GlobalScope))
        return this; //The class has already been partially evaluated
          
      this.baseType.PartiallyEvaluate();
      IReflect ir = this.baseType.ToIReflect();
      Type bt = null;
      if (!(ir is Type) || !Convert.IsPrimitiveIntegerType(bt = (Type)ir)){
        this.baseType.context.HandleError(JSError.InvalidBaseTypeForEnum);
        this.baseType = new TypeExpression(new ConstantWrapper(Typeob.Int32, null));
        bt = Typeob.Int32;          
      }

      if (this.customAttributes != null)
        this.customAttributes.PartiallyEvaluate();

      if (this.NeedsToBeCheckedForCLSCompliance()){
        if (!TypeExpression.TypeIsCLSCompliant(ir))
          this.baseType.context.HandleError(JSError.NonCLSCompliantType);
        this.CheckMemberNamesForCLSCompliance();
      }
        
      ScriptObject scope = this.enclosingScope;
      while (!(scope is GlobalScope) && !(scope is PackageScope))
        scope = scope.GetParent();
      this.classob.SetParent(new WithObject(scope, Typeob.Enum, true));
        
      Globals.ScopeStack.Push(this.classob);
      try{
        foreach (FieldInfo f in this.fields){
          JSMemberField field = (JSMemberField)f;
          ((DeclaredEnumValue)field.value).CoerceToBaseType(bt, field.originalContext);
        }
      }finally{
        Globals.ScopeStack.Pop();
      }
      return this;
    }
      
    internal override Type GetTypeBuilderOrEnumBuilder(){
      if (this.classob.classwriter != null) return this.classob.classwriter;
      this.PartiallyEvaluate();
      ClassScope classScope = this.enclosingScope as ClassScope;
      if (classScope != null){
        TypeBuilder result = ((TypeBuilder)classScope.classwriter).DefineNestedType(this.name, this.attributes|TypeAttributes.Sealed, Typeob.Enum, null);
        this.classob.classwriter = result;
        Type underlyingType = this.baseType.ToType();
        FieldBuilder fb = result.DefineField("value__", underlyingType, FieldAttributes.Private|FieldAttributes.SpecialName);
        if (this.customAttributes != null){
          CustomAttributeBuilder[] custAtt = this.customAttributes.GetCustomAttributeBuilders(false);
          for (int j = 0; j < custAtt.Length; j++)
            result.SetCustomAttribute(custAtt[j]);
        }
        foreach (FieldInfo field in this.fields){          
          ((JSMemberField)field).metaData = fb = result.DefineField(field.Name, result, FieldAttributes.Public|FieldAttributes.Static|FieldAttributes.Literal);
          fb.SetConstant(((EnumWrapper)field.GetValue(null)).ToNumericValue());
        }
        return result;
      }else{
        EnumBuilder result = compilerGlobals.module.DefineEnum(this.name, this.attributes, this.baseType.ToType());
        this.classob.classwriter = result;
        if (this.customAttributes != null){
          CustomAttributeBuilder[] custAtt = this.customAttributes.GetCustomAttributeBuilders(false);
          for (int j = 0; j < custAtt.Length; j++)
            result.SetCustomAttribute(custAtt[j]);
        }
        foreach (FieldInfo field in this.fields)
          ((JSMemberField)field).metaData = result.DefineLiteral(field.Name, ((EnumWrapper)field.GetValue(null)).ToNumericValue());
        return result;
      }
    }
  }
}
