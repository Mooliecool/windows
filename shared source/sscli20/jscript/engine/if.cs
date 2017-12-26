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
    using System.Collections;
    using System.Reflection;
    using System.Reflection.Emit;
    
    internal sealed class If : AST{
      private AST condition;
      private AST operand1;
      private AST operand2;
      private Completion completion;
      
      internal If(Context context, AST condition, AST true_branch, AST false_branch)
        : base(context) {
        this.condition = condition;
        this.operand1 = true_branch;
        this.operand2 = false_branch;
        this.completion = new Completion();
      }
    
      internal override Object Evaluate(){
        if (this.operand1 == null && this.operand2 == null)
          return this.completion;
        Completion c = null;
        if (this.condition != null){ 
          if (Convert.ToBoolean(this.condition.Evaluate()) == true)
            c = (Completion)this.operand1.Evaluate();
          else if (this.operand2 != null)
            c = (Completion)this.operand2.Evaluate();
          else
            c = new Completion();
        }else
          if (this.operand1 != null)
            c = (Completion)this.operand1.Evaluate();
          else
            c = (Completion)this.operand2.Evaluate();
        this.completion.value = c.value;
        if (c.Continue > 1)
          this.completion.Continue = c.Continue - 1;
        else
          this.completion.Continue = 0;  
        if (c.Exit > 0)
          this.completion.Exit = c.Exit - 1;
        else
          this.completion.Exit = 0;
        if (c.Return)
          return c;
        return this.completion;
      }
      
      internal override bool HasReturn(){
        if (this.operand1 != null){
          if (!this.operand1.HasReturn())
            return false;
          if (this.operand2 != null)
            return this.operand2.HasReturn();
          return false;
        }else{
          if (this.operand2 != null)
            return this.operand2.HasReturn();
          return false;
        }        
      }
      
      internal override AST PartiallyEvaluate(){
        this.condition = this.condition.PartiallyEvaluate();
        if (this.condition is ConstantWrapper){
          if (Convert.ToBoolean(this.condition.Evaluate()) == true)
            this.operand2 = null;
          else
            this.operand1 = null;
          this.condition = null;
        }
        ScriptObject current_scope = Globals.ScopeStack.Peek();
        while (current_scope is WithObject) current_scope = current_scope.GetParent();
        if (current_scope is FunctionScope){
          FunctionScope scope = (FunctionScope)current_scope;
          BitArray before = scope.DefinedFlags;
          BitArray after1 = before;
          if (this.operand1 != null){
            this.operand1 = this.operand1.PartiallyEvaluate();
            after1 = scope.DefinedFlags;
            scope.DefinedFlags = before;
          }
          if (this.operand2 != null){
            this.operand2 = this.operand2.PartiallyEvaluate();
            BitArray after2 = scope.DefinedFlags;
            int n = after1.Length;
            int m = after2.Length;
            if (n < m) after1.Length = m;
            if (m < n) after2.Length = n;
            before = after1.And(after2);
          }
          scope.DefinedFlags = before;
        }else{
          if (this.operand1 != null)
            this.operand1 = this.operand1.PartiallyEvaluate();
          if (this.operand2 != null)
            this.operand2 = this.operand2.PartiallyEvaluate();
        }
        return this;
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        //This assumes that rtype == Void.class
        if (this.operand1 == null && this.operand2 == null)
          return;
        Label else_label = il.DefineLabel();
        Label endif_label = il.DefineLabel();
        compilerGlobals.BreakLabelStack.Push(endif_label);
        compilerGlobals.ContinueLabelStack.Push(endif_label);
        if (this.condition != null){
          this.context.EmitLineInfo(il);
          if (this.operand2 != null)
            this.condition.TranslateToConditionalBranch(il, false, else_label, false);
          else
            this.condition.TranslateToConditionalBranch(il, false, endif_label, false);
          if (this.operand1 != null)
            this.operand1.TranslateToIL(il, Typeob.Void);
          if (this.operand2 != null) {
            if (this.operand1 != null && !this.operand1.HasReturn())
              il.Emit(OpCodes.Br, endif_label);
            il.MarkLabel(else_label);
            this.operand2.TranslateToIL(il, Typeob.Void);
          }
        }else{
          if (this.operand1 != null)
            this.operand1.TranslateToIL(il, Typeob.Void);
          else
            this.operand2.TranslateToIL(il, Typeob.Void);
        }
        il.MarkLabel(endif_label);
        compilerGlobals.BreakLabelStack.Pop();
        compilerGlobals.ContinueLabelStack.Pop();
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        if (this.condition != null)
          this.condition.TranslateToILInitializer(il);
        if (this.operand1 != null)
          this.operand1.TranslateToILInitializer(il);
        if (this.operand2 != null)
          this.operand2.TranslateToILInitializer(il);
      }
    }
}
