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
    
    internal sealed class DoWhile : AST{
      private AST body;
      private AST condition;
      private Completion completion;
    
      internal DoWhile(Context context, AST body, AST condition)
        : base(context) {
        this.body = body;
        this.condition = condition;
        this.completion = new Completion();
      }
    
      internal override Object Evaluate(){
        this.completion.Continue = 0;
        this.completion.Exit = 0;
        this.completion.value = null;
        do{
          Completion c = (Completion)this.body.Evaluate();
          if (c.value != null)
            this.completion.value = c.value;
          if (c.Continue > 1){
            this.completion.Continue = c.Continue - 1;
            break;
          }
          if (c.Exit > 0){
            this.completion.Exit = c.Exit - 1;
            break;
          }
          if (c.Return)
            return c;
        }while (Convert.ToBoolean(this.condition.Evaluate()) == true);
        return this.completion;
      }
      
      internal override AST PartiallyEvaluate(){
        ScriptObject current_scope = Globals.ScopeStack.Peek();
        while (current_scope is WithObject) current_scope = current_scope.GetParent();
        if (current_scope is FunctionScope){
          FunctionScope scope = (FunctionScope)current_scope;
          BitArray before = scope.DefinedFlags;
          this.body = this.body.PartiallyEvaluate();
          scope.DefinedFlags = before; //The body may have a continue in it
          this.condition = this.condition.PartiallyEvaluate();
          scope.DefinedFlags = before;
          //This is very pessimistic, but the potential presence of break/continue/return/throw
          //inside the loop presents complications that are hard to resolve naively.
        }else{
          this.body = this.body.PartiallyEvaluate();
          this.condition = this.condition.PartiallyEvaluate();
        }
        IReflect conditiontype = this.condition.InferType(null);
        if (conditiontype is FunctionPrototype || conditiontype == Typeob.ScriptFunction)
          this.context.HandleError(JSError.SuspectLoopCondition);
        return this;
      }  
    
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        //This assumes that rtype == Void.class
        Label loop_start = il.DefineLabel();
        Label condition_start = il.DefineLabel();
        Label loop_end = il.DefineLabel();
        compilerGlobals.BreakLabelStack.Push(loop_end);
        compilerGlobals.ContinueLabelStack.Push(condition_start);
        il.MarkLabel(loop_start);
        this.body.TranslateToIL(il, Typeob.Void);
        il.MarkLabel(condition_start);
        this.context.EmitLineInfo(il);
        this.condition.TranslateToConditionalBranch(il, true, loop_start, false);
        il.MarkLabel(loop_end);
        compilerGlobals.BreakLabelStack.Pop();
        compilerGlobals.ContinueLabelStack.Pop();
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        this.body.TranslateToILInitializer(il);
        this.condition.TranslateToILInitializer(il);
      }

      internal override Context GetFirstExecutableContext(){
        return this.body.GetFirstExecutableContext();
      }

    }

}
