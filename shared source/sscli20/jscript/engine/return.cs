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
    
    internal sealed class Return : AST{
      private Completion completion;
      private AST operand;
      private FunctionScope enclosingFunctionScope;
      private bool leavesFinally;
      
      internal Return(Context context, AST operand, bool leavesFinally)
        : base(context) {
        this.completion = new Completion();
        this.completion.Return = true;
        this.operand = operand;
        ScriptObject scope = Globals.ScopeStack.Peek();
        while (!(scope is FunctionScope)){
          scope = scope.GetParent();
          if (scope == null){
            this.context.HandleError(JSError.BadReturn);
            scope = new FunctionScope(null);
          }
        }
        this.enclosingFunctionScope = ((FunctionScope)scope);
        if (this.operand != null && this.enclosingFunctionScope.returnVar == null)
          this.enclosingFunctionScope.AddReturnValueField();
        this.leavesFinally = leavesFinally;
      }
    
      internal override Object Evaluate(){
        if (this.operand != null)
          this.completion.value = this.operand.Evaluate();
        return this.completion;
      }
      
      internal override bool HasReturn(){
        return true;
      }
      
      internal override AST PartiallyEvaluate(){
        if (this.leavesFinally)
          this.context.HandleError(JSError.BadWayToLeaveFinally);
        if (this.operand != null){
          this.operand = this.operand.PartiallyEvaluate();
          if (this.enclosingFunctionScope.returnVar != null)
            if (this.enclosingFunctionScope.returnVar.type == null){
              this.enclosingFunctionScope.returnVar.SetInferredType(this.operand.InferType(this.enclosingFunctionScope.returnVar), this.operand);
            }else
              Binding.AssignmentCompatible(this.enclosingFunctionScope.returnVar.type.ToIReflect(), this.operand, this.operand.InferType(null), true);
          else{ //returning something from a function explicitly typed as Void
            this.context.HandleError(JSError.CannotReturnValueFromVoidFunction);
            this.operand = null;
          }
        }else if (this.enclosingFunctionScope.returnVar != null)
          this.enclosingFunctionScope.returnVar.SetInferredType(Typeob.Object, null);
        return this;
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        //This assumes that rtype == Void.class.
        this.context.EmitLineInfo(il);
        if (this.operand != null)
          this.operand.TranslateToIL(il, this.enclosingFunctionScope.returnVar.FieldType);
        else if (this.enclosingFunctionScope.returnVar != null){
          il.Emit(OpCodes.Ldsfld, CompilerGlobals.undefinedField);
          Convert.Emit(this, il, Typeob.Object, this.enclosingFunctionScope.returnVar.FieldType);
        }
        if (this.enclosingFunctionScope.returnVar != null)
          il.Emit(OpCodes.Stloc, (LocalBuilder)this.enclosingFunctionScope.returnVar.GetMetaData());
        if (this.leavesFinally){
          il.Emit(OpCodes.Newobj, CompilerGlobals.returnOutOfFinallyConstructor);
          il.Emit(OpCodes.Throw);
        }else if (compilerGlobals.InsideProtectedRegion)
          il.Emit(OpCodes.Leave, this.enclosingFunctionScope.owner.returnLabel);
        else
          il.Emit(OpCodes.Br, this.enclosingFunctionScope.owner.returnLabel);
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        if (this.operand != null)
          this.operand.TranslateToILInitializer(il);
      }
    }
 }
