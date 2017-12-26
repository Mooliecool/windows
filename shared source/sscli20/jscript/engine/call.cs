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
    using System.Collections;
    using System.Reflection;
    using System.Reflection.Emit;
    
    internal class Call : AST{
      internal AST func;
      private ASTList args;
      private Object[] argValues;
      private int outParameterCount;
      internal bool isConstructor;
      internal bool inBrackets;
      private FunctionScope enclosingFunctionScope;
      private bool alreadyPartiallyEvaluated;
      private bool isAssignmentToDefaultIndexedProperty;
            
      internal Call(Context context, AST func, ASTList args, bool inBrackets)
        : base(context) {
        this.func = func;
        this.args = args == null ? new ASTList(context) : args;
        this.argValues = null;
        this.outParameterCount = 0;
        for (int i = 0, n = this.args.count; i < n; i++)
          if (this.args[i] is AddressOf)
             this.outParameterCount++;
        this.isConstructor = false;
        this.inBrackets = inBrackets;
        this.enclosingFunctionScope = null;
        this.alreadyPartiallyEvaluated = false;
        this.isAssignmentToDefaultIndexedProperty = false;
        ScriptObject scope = Globals.ScopeStack.Peek();
        while (!(scope is FunctionScope)){
          scope = scope.GetParent();
          if (scope == null)
            return;
        }
        this.enclosingFunctionScope = (FunctionScope)scope;
      }
      
      private bool AllParamsAreMissing(){
        for (int i = 0, n = this.args.count; i < n; i++){
          AST p = this.args[i];
          if (!(p is ConstantWrapper) || ((ConstantWrapper)p).value != System.Reflection.Missing.Value) return false;
        }
        return true;
      }
    
      private IReflect[] ArgIRs(){
        int n = this.args.count;
        IReflect[] argIRs = new IReflect[n];
        for (int i = 0; i < n; i++){
          AST arg = this.args[i];
          IReflect ir = argIRs[i] = arg.InferType(null);
          if (arg is AddressOf){
            if (ir is ClassScope) ir = ((ClassScope)ir).GetBakedSuperType(); //this should change if ever JS can declare out params
            argIRs[i] = Convert.ToType("&", Convert.ToType(ir));
          }
        }
        return argIRs;
      }
    
      // the parser may invoke this function in error condition to change this call to a function declaration
      internal bool CanBeFunctionDeclaration(){
        bool canBeFunc = this.func is Lookup && this.outParameterCount == 0;
        if (canBeFunc){
          for (int i = 0, n = this.args.count; i < n; i++){
            AST item = this.args[i];
            canBeFunc = item is Lookup;
            if (!canBeFunc) break;
          }
        }
        return canBeFunc;
      }

      internal override void CheckIfOKToUseInSuperConstructorCall(){
        this.func.CheckIfOKToUseInSuperConstructorCall();
      }
      
      internal override bool Delete(){
        Object[] actual_arguments = this.args == null ? null : this.args.EvaluateAsArray();
        int n = actual_arguments.Length;
        Object ob = this.func.Evaluate();
        if (ob == null) return true; //As per the standard, not backwards compatible with JS5
        if (n == 0) return true;
        Type obType = ob.GetType();
        MethodInfo deleteOp = obType.GetMethod("op_Delete", BindingFlags.ExactBinding|BindingFlags.Public|BindingFlags.Static, 
                                                null, new Type[]{obType, Typeob.ArrayOfObject}, null);
        if (deleteOp == null || (deleteOp.Attributes & MethodAttributes.SpecialName) == 0 || deleteOp.ReturnType != Typeob.Boolean)
          return LateBinding.DeleteMember(ob, Convert.ToString(actual_arguments[n-1]));
        else{
          deleteOp = new JSMethodInfo(deleteOp);
          return (bool)deleteOp.Invoke(null, new Object[]{ob, actual_arguments});
        }
      }
      
      internal override Object Evaluate(){
        if (this.outParameterCount > 0 && VsaEngine.executeForJSEE)
          throw new JScriptException(JSError.RefParamsNonSupportedInDebugger);
        LateBinding funcref = this.func.EvaluateAsLateBinding();
        Object[] actual_arguments = this.args == null ? null : this.args.EvaluateAsArray();
        Globals.CallContextStack.Push(new CallContext(this.context, funcref, actual_arguments));
        try{
          try{
            Object result = null;
            CallableExpression cexpr = this.func as CallableExpression;
            if (cexpr == null || !(cexpr.expression is Call))
              result = funcref.Call(actual_arguments, this.isConstructor, this.inBrackets, this.Engine);
            else
              result = LateBinding.CallValue(funcref.obj, actual_arguments, this.isConstructor, this.inBrackets, this.Engine,
                cexpr.GetObject2(), JSBinder.ob, null, null);
            if (this.outParameterCount > 0)
              for (int i = 0, n = this.args.count; i < n; i++)
                if (this.args[i] is AddressOf)
                  this.args[i].SetValue(actual_arguments[i]);
            return result;
          }catch(TargetInvocationException e){
            JScriptException se;
            if (e.InnerException is JScriptException){
              se = (JScriptException)e.InnerException;
              if (se.context == null)
                if (((uint)se.Number) == (0x800A0000|(uint)JSError.ObjectExpected))
                  se.context = this.func.context;
                else
                  se.context = this.context;
            }else
              se = new JScriptException(e.InnerException, this.context);
            throw se;
          }catch(JScriptException e){
            if (e.context == null){
              if (((uint)e.Number) == (0x800A0000|(uint)JSError.ObjectExpected))
                e.context = this.func.context;
              else
                e.context = this.context;
            }
            throw e;
          }catch(Exception e){
            throw new JScriptException(e, this.context);
          }catch{
            throw new JScriptException(JSError.NonClsException, this.context);
          }
        }finally{    
          Globals.CallContextStack.Pop();
        }
      }

      internal void EvaluateIndices(){
        this.argValues = this.args.EvaluateAsArray();
      }
      
      // the parser may invoke this function in error condition to change this call to a function declaration
      internal IdentifierLiteral GetName(){
        Debug.Assert(this.func is Lookup && this.outParameterCount == 0);
        return new IdentifierLiteral(this.func.ToString(), this.func.context);
      }

      // the parser may invoke this function in error condition to change this call to a function declaration
      internal void GetParameters(ArrayList parameters){
        Debug.Assert(this.func is Lookup && this.outParameterCount == 0);
        for (int i = 0, n = this.args.count; i < n; i++){
          AST item = this.args[i];
          parameters.Add(new ParameterDeclaration(item.context, item.ToString(), null, null));
        }
      }

      internal override IReflect InferType(JSField inference_target){
        if (this.func is Binding)
          return ((Binding)this.func).InferTypeOfCall(inference_target, this.isConstructor);
        else if (this.func is ConstantWrapper){
          Object value = ((ConstantWrapper)this.func).value;
          if (value is Type || value is ClassScope || value is TypedArray)
            return (IReflect)value;
        }
        return Typeob.Object;
      }

      private JSLocalField[] LocalsThatWereOutParameters(){
        int n = this.outParameterCount;
        if (n == 0) return null;
        JSLocalField[] result = new JSLocalField[n];
        int m = 0;
        for (int i = 0; i < n; i++){
          AST arg = this.args[i];
          if (arg is AddressOf){
            FieldInfo field = ((AddressOf)arg).GetField();
            if (field is JSLocalField)
              result[m++] = (JSLocalField)field;
          }
        }
        return result;
      }

      internal void MakeDeletable(){
        if (this.func is Binding){
          Binding b = (Binding)this.func;
          b.InvalidateBinding();
          b.PartiallyEvaluateAsCallable();
          b.ResolveLHValue();
        }
      }
      
      internal override AST PartiallyEvaluate(){
        if (this.alreadyPartiallyEvaluated)
          return this;
        this.alreadyPartiallyEvaluated = true;
        if (this.inBrackets && this.AllParamsAreMissing()){
          if (this.isConstructor) this.args.context.HandleError(JSError.TypeMismatch);
          IReflect ir = ((TypeExpression)(new TypeExpression(this.func)).PartiallyEvaluate()).ToIReflect();
          return new ConstantWrapper(new TypedArray(ir, this.args.count+1), this.context);
        }
        this.func = this.func.PartiallyEvaluateAsCallable();
        this.args = (ASTList)this.args.PartiallyEvaluate();
        IReflect[] argIRs = this.ArgIRs();
        this.func.ResolveCall(this.args, argIRs, this.isConstructor, this.inBrackets);
        if (!this.isConstructor && !this.inBrackets && this.func is Binding && this.args.count == 1){
          Binding b = (Binding)this.func;
          if (b.member is Type){
            Type t = (Type)b.member;
            ConstantWrapper arg0 = this.args[0] as ConstantWrapper;
            if (arg0 != null){
              try{
                if (arg0.value == null || arg0.value is DBNull)
                  return this;
                else if (arg0.isNumericLiteral && (t == Typeob.Decimal || t == Typeob.Int64 || t == Typeob.UInt64 || t == Typeob.Single))
                  return new ConstantWrapper(Convert.CoerceT(arg0.context.GetCode(), t, true), this.context);
                else
                  return new ConstantWrapper(Convert.CoerceT(arg0.Evaluate(), t, true), this.context);
              }catch{
                arg0.context.HandleError(JSError.TypeMismatch);
              }
            }else{
              if (!Binding.AssignmentCompatible(t, this.args[0], argIRs[0], false))
                this.args[0].context.HandleError(JSError.ImpossibleConversion);
            }
          }else if (b.member is JSVariableField){
            JSVariableField field = (JSVariableField)b.member;
            if (field.IsLiteral){
              if (field.value is ClassScope){
                ClassScope csc = (ClassScope)field.value;
                IReflect ut = csc.GetUnderlyingTypeIfEnum();
                if (ut != null){
                  if (!Convert.IsPromotableTo(argIRs[0], ut) && !Convert.IsPromotableTo(ut, argIRs[0]) && (argIRs[0] != Typeob.String || ut == csc))
                    this.args[0].context.HandleError(JSError.ImpossibleConversion);
                }else{
                  if (!Convert.IsPromotableTo(argIRs[0], csc) && !Convert.IsPromotableTo(csc, argIRs[0]))
                    this.args[0].context.HandleError(JSError.ImpossibleConversion);
                }
              }else if (field.value is TypedArray){
                TypedArray ta = (TypedArray)field.value;
                if (!Convert.IsPromotableTo(argIRs[0], ta) && !Convert.IsPromotableTo(ta, argIRs[0]))
                  this.args[0].context.HandleError(JSError.ImpossibleConversion);
              }
            }
          }
        }
        return this;
      }
      
      internal override AST PartiallyEvaluateAsReference(){
        this.func = this.func.PartiallyEvaluateAsCallable();
        this.args = (ASTList)this.args.PartiallyEvaluate();
        return this;
      }

      internal override void SetPartialValue(AST partial_value){
        if (this.isConstructor)
          this.context.HandleError(JSError.IllegalAssignment);
        else if (this.func is Binding)
          ((Binding)this.func).SetPartialValue(this.args, this.ArgIRs(), partial_value, this.inBrackets);
        else if (this.func is ThisLiteral)
          ((ThisLiteral)this.func).ResolveAssignmentToDefaultIndexedProperty(this.args, this.ArgIRs(), partial_value);
      }
      
      internal override void SetValue(Object value){
        LateBinding prop = this.func.EvaluateAsLateBinding();
        try{
          prop.SetIndexedPropertyValue(this.argValues != null ? this.argValues : this.args.EvaluateAsArray(), value);
        }catch(JScriptException e){
          if (e.context == null)
            e.context = this.func.context;
          throw e;
        }catch(Exception e){
          throw new JScriptException(e, this.func.context);
        }catch{
          throw new JScriptException(JSError.NonClsException, this.context);
        }
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        if (this.context.document.debugOn)
          il.Emit(OpCodes.Nop);
        bool restoreLocals = true;
        if (this.enclosingFunctionScope != null && this.enclosingFunctionScope.owner != null){
          Binding b = this.func as Binding;
          if (b != null && !this.enclosingFunctionScope.closuresMightEscape){
            if (b.member is JSLocalField)
              this.enclosingFunctionScope.owner.TranslateToILToSaveLocals(il);
            else
              restoreLocals = false;
          }else
            this.enclosingFunctionScope.owner.TranslateToILToSaveLocals(il);
        }
        this.func.TranslateToILCall(il, rtype, this.args, this.isConstructor, this.inBrackets);
        if (restoreLocals && this.enclosingFunctionScope != null && this.enclosingFunctionScope.owner != null)
          if (this.outParameterCount == 0)
            this.enclosingFunctionScope.owner.TranslateToILToRestoreLocals(il);
          else
            this.enclosingFunctionScope.owner.TranslateToILToRestoreLocals(il, this.LocalsThatWereOutParameters());
      }

      internal CustomAttribute ToCustomAttribute(){
        return new CustomAttribute(this.context, this.func, this.args);
      }

      internal override void TranslateToILDelete(ILGenerator il, Type rtype){
        IReflect ir = this.func.InferType(null);
        Type obType = Convert.ToType(ir);
        this.func.TranslateToIL(il, obType);
        this.args.TranslateToIL(il, Typeob.ArrayOfObject);
        if (this.func is Binding){
          MethodInfo deleteOp;
          if (ir is ClassScope)
            deleteOp = ((ClassScope)ir).owner.deleteOpMethod;
          else
            deleteOp = ir.GetMethod("op_Delete", BindingFlags.ExactBinding|BindingFlags.Public|BindingFlags.Static, 
                                                  null, new Type[]{obType, Typeob.ArrayOfObject}, null);
          if (deleteOp != null && (deleteOp.Attributes & MethodAttributes.SpecialName) != 0 && deleteOp.ReturnType == Typeob.Boolean){
            il.Emit(OpCodes.Call, deleteOp);
            Convert.Emit(this, il, Typeob.Boolean, rtype);
            return;
          }
        }
        ConstantWrapper.TranslateToILInt(il, this.args.count-1);
        il.Emit(OpCodes.Ldelem_Ref);
        Convert.Emit(this, il, Typeob.Object, Typeob.String);
        il.Emit(OpCodes.Call, CompilerGlobals.deleteMemberMethod);
        Convert.Emit(this, il, Typeob.Boolean, rtype);
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        this.func.TranslateToILInitializer(il);
        this.args.TranslateToILInitializer(il);
      }

      internal override void TranslateToILPreSet(ILGenerator il){
        this.func.TranslateToILPreSet(il, this.args);
      }
      
      internal override void TranslateToILPreSet(ILGenerator il, ASTList args){
        this.isAssignmentToDefaultIndexedProperty = true;
        base.TranslateToILPreSet(il, args);
      }
      
      internal override void TranslateToILPreSetPlusGet(ILGenerator il){
        this.func.TranslateToILPreSetPlusGet(il, this.args, this.inBrackets);
      }
      
      internal override void TranslateToILSet(ILGenerator il, AST rhvalue){
        if (this.isAssignmentToDefaultIndexedProperty)
          base.TranslateToILSet(il, rhvalue);
        else
          this.func.TranslateToILSet(il, rhvalue);
      }
    }
}
