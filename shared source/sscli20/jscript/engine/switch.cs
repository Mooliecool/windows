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
    
    internal sealed class Switch : AST{
      private AST expression;
      private ASTList cases;
      private int default_case;
      private Completion completion;
      
      internal Switch(Context context, AST expression, ASTList cases)
        : base(context) {
        this.expression = expression;
        this.cases = cases;
        this.default_case = -1;
        for (int i = 0, n = this.cases.count; i < n; i++){
          if (((SwitchCase)(this.cases[i])).IsDefault()){
            this.default_case = i;
            break;
          }
        }
        this.completion = new Completion();
      }
    
      internal override Object Evaluate(){
        this.completion.Continue = 0;
        this.completion.Exit = 0;
        this.completion.value = null;
        Object val = this.expression.Evaluate();
        Completion c = null;
        int i, n = this.cases.count;
        for (i = 0; i < n; i++){
          if (i == this.default_case)
            continue;
          c = ((SwitchCase)(this.cases[i])).Evaluate(val);
          if (c != null)
            break;
        }
        if (c == null)
          if (this.default_case >= 0){
            i = this.default_case;
            c = (Completion)((SwitchCase)(this.cases[i])).Evaluate();
          }else
            return this.completion;
        while(true){
          if (c.value != null)
            this.completion.value = c.value;
          if (c.Continue > 0){
            this.completion.Continue = c.Continue - 1;
            break;
          }
          if (c.Exit > 0){
            this.completion.Exit = c.Exit - 1;
            break;
          }
          if (c.Return)
            return c;
          if (i >= n-1)
            return this.completion;
          c = (Completion)((SwitchCase)(this.cases[++i])).Evaluate();
        }
        return this.completion;
      }
      
      internal override AST PartiallyEvaluate(){
        this.expression = this.expression.PartiallyEvaluate();
        ScriptObject current_scope = Globals.ScopeStack.Peek();
        while (current_scope is WithObject) current_scope = current_scope.GetParent();
        if (current_scope is FunctionScope){
          FunctionScope scope = (FunctionScope)current_scope;
          BitArray before = scope.DefinedFlags;
          for (int i = 0, n = this.cases.count; i < n; i++){
            this.cases[i] = this.cases[i].PartiallyEvaluate();
            scope.DefinedFlags = before;
          }
        }else
          for (int i = 0, n = this.cases.count; i < n; i++)
            this.cases[i] = this.cases[i].PartiallyEvaluate();
        return this;
      }  
    
      internal override void TranslateToIL(ILGenerator il, Type rtype){ 
        //This assumes that rtype == Void.class
        //compile the expression and store it in a local
        Type etype = Convert.ToType(this.expression.InferType(null));
        this.expression.context.EmitLineInfo(il);
        this.expression.TranslateToIL(il, etype);
        LocalBuilder exprval = il.DeclareLocal(etype);
        il.Emit(OpCodes.Stloc, exprval);
        
        //compile a series of conditional branches to the bodies
        int n = this.cases.count;
        Label[] labels = new Label[this.cases.count];
        for (int i = 0; i < n; i++){
          labels[i] = il.DefineLabel();
          if (i == this.default_case)
            continue;
          il.Emit(OpCodes.Ldloc, exprval);
          ((SwitchCase)this.cases[i]).TranslateToConditionalBranch(il, etype, true, labels[i], false);
        }
        Label endOfSwitch = il.DefineLabel();
        if (this.default_case >= 0)
          il.Emit(OpCodes.Br, labels[this.default_case]);
        else
          il.Emit(OpCodes.Br, endOfSwitch);
        
        //push a break label on the stack
        compilerGlobals.BreakLabelStack.Push(endOfSwitch);
        compilerGlobals.ContinueLabelStack.Push(endOfSwitch);
        
        //compile the bodies
        for (int i = 0; i < n; i++){
          il.MarkLabel(labels[i]);
          this.cases[i].TranslateToIL(il, Typeob.Void);
        }
        il.MarkLabel(endOfSwitch);
        
        //pop the break label
        compilerGlobals.BreakLabelStack.Pop();
        compilerGlobals.ContinueLabelStack.Pop();
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        this.expression.TranslateToILInitializer(il);
        for (int i = 0, n = this.cases.count; i < n; i++)
          this.cases[i].TranslateToILInitializer(il);
      }

      internal override Context GetFirstExecutableContext(){
        return this.expression.context;
      }
    }
}
