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
    
    internal sealed class Conditional : AST{
      private AST condition;
      private AST operand1;
      private AST operand2;
      
      internal Conditional(Context context, AST condition, AST operand1, AST operand2)
        : base(context) {
        this.condition = condition;
        this.operand1 = operand1;
        this.operand2 = operand2;
      }
    
      internal override Object Evaluate(){
        if (Convert.ToBoolean(this.condition.Evaluate()))
          return this.operand1.Evaluate();
        else
          return this.operand2.Evaluate();
      }
      
      internal override AST PartiallyEvaluate(){
        this.condition = this.condition.PartiallyEvaluate();
        ScriptObject current_scope = Globals.ScopeStack.Peek();
        while (current_scope is WithObject) current_scope = current_scope.GetParent();
        if (current_scope is FunctionScope){
          FunctionScope scope = (FunctionScope)current_scope;
          BitArray before = scope.DefinedFlags;
          this.operand1 = this.operand1.PartiallyEvaluate();
          BitArray after1 = scope.DefinedFlags;
          scope.DefinedFlags = before;
          this.operand2 = this.operand2.PartiallyEvaluate();
          BitArray after2 = scope.DefinedFlags;
          int n = after1.Length;
          int m = after2.Length;
          if (n < m) after1.Length = m;
          if (m < n) after2.Length = n;
          scope.DefinedFlags = after1.And(after2);
        }else{
          this.operand1 = this.operand1.PartiallyEvaluate();
          this.operand2 = this.operand2.PartiallyEvaluate();
        }
        return this;
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        Label else_label = il.DefineLabel();
        Label endif_label = il.DefineLabel();
        this.condition.TranslateToConditionalBranch(il, false, else_label, false);
        this.operand1.TranslateToIL(il, rtype);
        il.Emit(OpCodes.Br, endif_label);
        il.MarkLabel(else_label);
        this.operand2.TranslateToIL(il, rtype);
        il.MarkLabel(endif_label);
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        this.condition.TranslateToILInitializer(il);
        this.operand1.TranslateToILInitializer(il);
        this.operand2.TranslateToILInitializer(il);
      }
    }

}
