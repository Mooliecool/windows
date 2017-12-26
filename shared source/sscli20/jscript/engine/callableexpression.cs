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
    
    internal sealed class CallableExpression : Binding{
      internal AST expression;
      private IReflect expressionInferredType;
        
      internal CallableExpression(AST expression)
        : base(expression.context, ""){
        this.expression = expression;
        JSLocalField field = new JSLocalField("", null, 0, Missing.Value);
        this.expressionInferredType = expression.InferType(field);
        field.inferred_type = this.expressionInferredType;
        this.member = field;
        this.members = new MemberInfo[]{field};
      }
      
      internal override LateBinding EvaluateAsLateBinding(){
        return new LateBinding(null, this.expression.Evaluate(), VsaEngine.executeForJSEE);
      }
    
      protected override Object GetObject(){
        return this.GetObject2();
      }
      
      internal Object GetObject2(){
        Call c = this.expression as Call;
        if (c == null || !c.inBrackets)
          return Convert.ToObject(this.expression.Evaluate(), this.Engine);
        else
          return Convert.ToObject(c.func.Evaluate(), this.Engine);
      }
      
      protected override void HandleNoSuchMemberError(){
        throw new JScriptException(JSError.InternalError, this.context);
      }
      
      internal override AST PartiallyEvaluate(){
        return this;
      }
      
      //Called when there is an early binding to default members on rtype
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        this.expression.TranslateToIL(il, rtype);
      }

      internal override void TranslateToILCall(ILGenerator il, Type rtype, ASTList argList, bool construct, bool brackets){
        if (this.defaultMember != null && construct && brackets){
          base.TranslateToILCall(il, rtype, argList, construct, brackets);
          return;
        }
        JSGlobalField gf = this.member as JSGlobalField;
        if (gf != null && gf.IsLiteral && argList.count == 1){
          Type t = Convert.ToType((IReflect)gf.value);
          argList[0].TranslateToIL(il, t);
          Convert.Emit(this, il, t, rtype);
          return;
        }
        this.TranslateToILWithDupOfThisOb(il);
        argList.TranslateToIL(il, Typeob.ArrayOfObject);
        if (construct)
          il.Emit(OpCodes.Ldc_I4_1);
        else
          il.Emit(OpCodes.Ldc_I4_0);
        if (brackets)
          il.Emit(OpCodes.Ldc_I4_1);
        else
          il.Emit(OpCodes.Ldc_I4_0);
        this.EmitILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.callValueMethod);
        Convert.Emit(this, il, Typeob.Object, rtype);
      }

      
      protected override void TranslateToILObject(ILGenerator il, Type obType, bool noValue){
        this.EmitILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.scriptObjectStackTopMethod);
        il.Emit(OpCodes.Castclass, Typeob.IActivationObject);
        il.Emit(OpCodes.Callvirt, CompilerGlobals.getGlobalScopeMethod);
      }
      
      //Called when a late bound call is being made
      protected override void TranslateToILWithDupOfThisOb(ILGenerator il){
        Call c = this.expression as Call;
        if (c == null || !c.inBrackets)
          this.TranslateToILObject(il, null, false);
        else if (c.isConstructor && c.inBrackets){
          //Dealing with (new T[x])[..] or (new T[x])(...), being handled late bound because it is an edge case
          c.TranslateToIL(il, Typeob.Object);
          il.Emit(OpCodes.Dup);
          return;
        }else
          c.func.TranslateToIL(il, Typeob.Object);
        this.expression.TranslateToIL(il, Typeob.Object);
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        this.expression.TranslateToILInitializer(il);
        if (!this.expressionInferredType.Equals(this.expression.InferType(null))){
          MemberInfo[] savedMembers = this.members;
          this.InvalidateBinding();
          this.members = savedMembers;
        }
      }
    }
 }
