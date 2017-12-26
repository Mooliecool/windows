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
    
    internal sealed class For : AST{
      private AST initializer;
      private AST condition;
      private AST incrementer;
      private AST body;
      private Completion completion;
    
      internal For(Context context, AST initializer, AST condition, AST incrementer, AST body)
        : base(context) {
        this.initializer = initializer;
        this.condition = condition;
        this.incrementer = incrementer;
        this.body = body;
        this.completion = new Completion();
      }
    
      // no debug info for the for statement iteself
      internal override Object Evaluate(){
        this.completion.Continue = 0;
        this.completion.Exit = 0;
        this.completion.value = null;
        this.initializer.Evaluate();
        while (Convert.ToBoolean(this.condition.Evaluate()) == true){
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
          this.incrementer.Evaluate();
        }
        return this.completion;
      }
      
      internal override AST PartiallyEvaluate(){
        this.initializer = this.initializer.PartiallyEvaluate();
        ScriptObject current_scope = Globals.ScopeStack.Peek();
        while (current_scope is WithObject) current_scope = current_scope.GetParent();
        if (current_scope is FunctionScope){
          FunctionScope scope = (FunctionScope)current_scope;
          BitArray before = scope.DefinedFlags;
          this.condition = this.condition.PartiallyEvaluate();
          //Do not need to clear the defined flags because the condition is always completely executed if the body is reached.
          this.body = this.body.PartiallyEvaluate();
          scope.DefinedFlags = before;
          this.incrementer = this.incrementer.PartiallyEvaluate();
          scope.DefinedFlags = before;
        }else{      
          this.condition = this.condition.PartiallyEvaluate();
          this.body = this.body.PartiallyEvaluate();
          this.incrementer = this.incrementer.PartiallyEvaluate();
        }
        IReflect conditiontype = this.condition.InferType(null);
        if (conditiontype is FunctionPrototype || conditiontype == Typeob.ScriptFunction)
          this.context.HandleError(JSError.SuspectLoopCondition);
        return this;
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        //This assumes that rtype == Void.class
        Label body_start = il.DefineLabel();
        Label continue_lab = il.DefineLabel();
        Label first_time = il.DefineLabel();
        Label loop_end = il.DefineLabel();
        bool noCondition = false;
        compilerGlobals.BreakLabelStack.Push(loop_end);
        compilerGlobals.ContinueLabelStack.Push(continue_lab);
        if (!(this.initializer is EmptyLiteral)){
          this.initializer.context.EmitLineInfo(il);
          this.initializer.TranslateToIL(il, Typeob.Void);
        }
        il.MarkLabel(body_start);
        if (!(this.condition is ConstantWrapper) || !(this.condition.Evaluate() is bool) || !((bool)this.condition.Evaluate())){
          this.condition.context.EmitLineInfo(il);
          this.condition.TranslateToConditionalBranch(il, false, loop_end, false);
        }else
          if (this.condition.context.StartPosition + 1 == this.condition.context.EndPosition)
            // the for does not have a condition specified. If there is no increment as well emit debug info for the whole for
            noCondition = true;
        this.body.TranslateToIL(il, Typeob.Void);
        il.MarkLabel(continue_lab);
        if (!(this.incrementer is EmptyLiteral)){
          this.incrementer.context.EmitLineInfo(il);
          this.incrementer.TranslateToIL(il, Typeob.Void);
        }else if (noCondition){
          this.context.EmitLineInfo(il);
        }
        il.Emit(OpCodes.Br, body_start);
        il.MarkLabel(loop_end);
        compilerGlobals.BreakLabelStack.Pop();
        compilerGlobals.ContinueLabelStack.Pop();
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        this.initializer.TranslateToILInitializer(il);
        this.condition.TranslateToILInitializer(il);
        this.incrementer.TranslateToILInitializer(il);
        this.body.TranslateToILInitializer(il);
      }
    }

}
