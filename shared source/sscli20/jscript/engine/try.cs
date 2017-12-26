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
    
    public sealed class Try : AST{
      private AST body;
      private TypeExpression type;
      private AST handler;
      private AST finally_block;
      private BlockScope handler_scope;
      private FieldInfo field;
      private String fieldName;
      private bool finallyHasControlFlowOutOfIt;
      private Context tryEndContext;
      
      internal Try(Context context, AST body, AST identifier, TypeExpression type, AST handler, AST finally_block, bool finallyHasControlFlowOutOfIt, Context tryEndContext)
        : base(context) {
        this.body = body;
        this.type = type;
        this.handler = handler;
        this.finally_block = finally_block;
        ScriptObject current_scope = (ScriptObject)Globals.ScopeStack.Peek();
        while (current_scope is WithObject) //Can only happen at run time and only if there is an eval
          current_scope = current_scope.GetParent();
        this.handler_scope = null;
        this.field = null;
        if (identifier != null){
          this.fieldName = identifier.ToString();
          this.field = current_scope.GetField(this.fieldName, BindingFlags.Public|BindingFlags.Instance|BindingFlags.Static);
          if (this.field != null){
            if (type == null && (field is JSVariableField && field.IsStatic && ((JSVariableField)field).type == null) && !field.IsLiteral && !field.IsInitOnly)
              return; //preserve legacy semantics by using the existing variable
            if (((IActivationObject)current_scope).GetLocalField(this.fieldName) != null)
              identifier.context.HandleError(JSError.DuplicateName, false);
          }
          this.handler_scope = new BlockScope(current_scope);
          this.handler_scope.catchHanderScope = true;
          JSVariableField f = this.handler_scope.AddNewField(identifier.ToString(), Missing.Value, FieldAttributes.Public); // must be a local 
          this.field = f; f.originalContext = identifier.context;
          if (identifier.context.document.debugOn && this.field is JSLocalField){
            this.handler_scope.AddFieldForLocalScopeDebugInfo((JSLocalField)this.field);
          }
        }
        this.finallyHasControlFlowOutOfIt = finallyHasControlFlowOutOfIt;
        this.tryEndContext = tryEndContext;
      }
    
      internal override Object Evaluate(){
        int i = Globals.ScopeStack.Size();
        int j = Globals.CallContextStack.Size();
        Completion bc = null;
        Completion fc = null;
        try{
          Object eValue = null;
          try{
            bc = (Completion)this.body.Evaluate();
          }catch(Exception e){
            if (this.handler == null) throw;
            eValue = e;
            if (this.type != null){
              Type ht = this.type.ToType();
              if (Typeob.Exception.IsAssignableFrom(ht)){
                if (!ht.IsInstanceOfType(e)) throw;
              }else if (!ht.IsInstanceOfType(eValue = JScriptExceptionValue(e, this.Engine)))
                throw;
            }else
              eValue = JScriptExceptionValue(e, this.Engine);
          }catch{
            eValue = new JScriptException(JSError.NonClsException);
          }
          
          if (eValue != null) {
            Globals.ScopeStack.TrimToSize(i);
            Globals.CallContextStack.TrimToSize(j);
            if (this.handler_scope != null){
              this.handler_scope.SetParent(Globals.ScopeStack.Peek());
              Globals.ScopeStack.Push(this.handler_scope);
            }
            if (this.field != null)
              this.field.SetValue(Globals.ScopeStack.Peek(), eValue);
            bc = (Completion)this.handler.Evaluate();
          }
        }finally{
          Globals.ScopeStack.TrimToSize(i);
          Globals.CallContextStack.TrimToSize(j);
          if (this.finally_block != null){
            fc = (Completion)this.finally_block.Evaluate();
          }
        }
        if (bc == null || fc != null && (fc.Exit > 0 || fc.Continue > 0 || fc.Return))
          bc = fc;
        else{
          if (fc != null && fc.value is Missing)
            bc.value = fc.value;
        }
        Completion result = new Completion();
        result.Continue = bc.Continue - 1;
        result.Exit = bc.Exit - 1;
        result.Return = bc.Return;
        result.value = bc.value;
        return result;
      }

      internal override Context GetFirstExecutableContext(){
        return this.body.GetFirstExecutableContext();
      }
      
      public static Object JScriptExceptionValue(Object e, VsaEngine engine){
        if (engine == null){
          engine = new VsaEngine(true);
          engine.InitVsaEngine("JS7://Microsoft.JScript.Vsa.VsaEngine", new DefaultVsaSite());
        }
        ErrorConstructor originalError = engine.Globals.globalObject.originalError;
        if (e is JScriptException){
          Object value = ((JScriptException)e).value;
          if (value is Exception || value is Missing || (((JScriptException)e).Number&0xFFFF) != (int)JSError.UncaughtException)
            return originalError.Construct((Exception)e);
          return value; //The exception wraps a non-exception value
        }else if (e is StackOverflowException)
          return originalError.Construct(new JScriptException(JSError.OutOfStack));
        else if (e is OutOfMemoryException)
          return originalError.Construct(new JScriptException(JSError.OutOfMemory));
        return originalError.Construct(e);
      }
      
      internal override AST PartiallyEvaluate(){
        if (this.type != null){
          this.type.PartiallyEvaluate();
          ((JSVariableField)this.field).type = type;
        }else if (this.field is JSLocalField)
          ((JSLocalField)this.field).SetInferredType(Typeob.Object, null); //This should never give an error
        ScriptObject current_scope = Globals.ScopeStack.Peek();
        while (current_scope is WithObject) current_scope = current_scope.GetParent();
        FunctionScope scope = null;
        BitArray before = null;
        if (current_scope is FunctionScope){
          scope = (FunctionScope)current_scope;
          before = scope.DefinedFlags;
        }
        this.body = this.body.PartiallyEvaluate();
        if (this.handler != null){
          if (this.handler_scope != null)
            Globals.ScopeStack.Push(this.handler_scope);
          if (this.field is JSLocalField)
            ((JSLocalField)this.field).isDefined = true;
          this.handler = this.handler.PartiallyEvaluate();
          if (this.handler_scope != null)
            Globals.ScopeStack.Pop();
        }
        if (this.finally_block != null)
          this.finally_block = this.finally_block.PartiallyEvaluate();
        if (scope != null)
          scope.DefinedFlags = before;
        return this;
      }
      
      public static void PushHandlerScope(VsaEngine engine, String id, int scopeId){
        engine.PushScriptObject(new BlockScope(engine.ScriptObjectStackTop(), id, scopeId));
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        //This assumes that rtype == Void.class.
        bool savedInsideProtectedRegion = compilerGlobals.InsideProtectedRegion;
        compilerGlobals.InsideProtectedRegion = true;
        compilerGlobals.BreakLabelStack.Push(compilerGlobals.BreakLabelStack.Peek(0));
        compilerGlobals.ContinueLabelStack.Push(compilerGlobals.ContinueLabelStack.Peek(0));
        il.BeginExceptionBlock(); 
        if (this.finally_block != null){
          if (this.finallyHasControlFlowOutOfIt)
            il.BeginExceptionBlock();
          if (this.handler != null) 
            il.BeginExceptionBlock();
        }
        this.body.TranslateToIL(il, Typeob.Void);
        if (this.tryEndContext != null)
          this.tryEndContext.EmitLineInfo(il);
        if (this.handler != null){
          if (this.type == null){
            il.BeginCatchBlock(Typeob.Exception);
            this.handler.context.EmitLineInfo(il);
            this.EmitILToLoadEngine(il);
            il.Emit(OpCodes.Call, CompilerGlobals.jScriptExceptionValueMethod);
          }else{
            Type filterType = this.type.ToType();
            if (Typeob.Exception.IsAssignableFrom(filterType)){
              il.BeginCatchBlock(filterType);
              this.handler.context.EmitLineInfo(il);
            }
            else{
              il.BeginExceptFilterBlock();
              this.handler.context.EmitLineInfo(il);
              this.EmitILToLoadEngine(il);
              il.Emit(OpCodes.Call, CompilerGlobals.jScriptExceptionValueMethod);
              il.Emit(OpCodes.Isinst, filterType);
              il.Emit(OpCodes.Ldnull);
              il.Emit(OpCodes.Cgt_Un);
              il.BeginCatchBlock(null);
              this.EmitILToLoadEngine(il);
              il.Emit(OpCodes.Call, CompilerGlobals.jScriptExceptionValueMethod);
              Convert.Emit(this, il, Typeob.Object, filterType);
            }
          }
          Object tok = this.field is JSVariableField ? ((JSVariableField)this.field).GetMetaData() : this.field;
          if (tok is LocalBuilder)
            il.Emit(OpCodes.Stloc, (LocalBuilder)tok);
          else if (tok is FieldInfo)
            il.Emit(OpCodes.Stsfld, (FieldInfo)tok);
          else
            Convert.EmitLdarg(il, (short)tok);
            
          if (this.handler_scope != null){
            if (!this.handler_scope.isKnownAtCompileTime){ //I.e. eval or nested func
              this.EmitILToLoadEngine(il);
              il.Emit(OpCodes.Ldstr, this.fieldName);
              ConstantWrapper.TranslateToILInt(il, this.handler_scope.scopeId);
              il.Emit(OpCodes.Call, Typeob.Try.GetMethod("PushHandlerScope"));
              Globals.ScopeStack.Push(this.handler_scope);
              il.BeginExceptionBlock();
            }
            il.BeginScope(); // so that we can emit local scoped information for the handler variable
            if (this.context.document.debugOn)
              this.handler_scope.EmitLocalInfoForFields(il);
          }
          this.handler.TranslateToIL(il, Typeob.Void);
          if (this.handler_scope != null){
            il.EndScope(); 
            if (!this.handler_scope.isKnownAtCompileTime){ //I.e. eval or nested func
              il.BeginFinallyBlock();
              this.EmitILToLoadEngine(il);
              il.Emit(OpCodes.Call, CompilerGlobals.popScriptObjectMethod);
              il.Emit(OpCodes.Pop);
              Globals.ScopeStack.Pop();
              il.EndExceptionBlock();
            }
          }
          il.EndExceptionBlock();
        }
        if (this.finally_block != null){
          bool savedInsideFinally = compilerGlobals.InsideFinally;
          int savedFinallyStackTop = compilerGlobals.FinallyStackTop;
          compilerGlobals.InsideFinally = true;
          compilerGlobals.FinallyStackTop = compilerGlobals.BreakLabelStack.Size();
          il.BeginFinallyBlock();
          this.finally_block.TranslateToIL(il, Typeob.Void);
          il.EndExceptionBlock();
          compilerGlobals.InsideFinally = savedInsideFinally;
          compilerGlobals.FinallyStackTop = savedFinallyStackTop;
          if (this.finallyHasControlFlowOutOfIt){
            il.BeginCatchBlock(Typeob.BreakOutOfFinally);
            il.Emit(OpCodes.Ldfld, Typeob.BreakOutOfFinally.GetField("target"));
            // don't need to go to 0 in the loop because 0 is the outmost block (i.e. function body)
            // and that would generate a JIT assert because the jump is sometimes outside the function
            for (int i = compilerGlobals.BreakLabelStack.Size()-1, n = i; i > 0; i--){
              il.Emit(OpCodes.Dup);
              ConstantWrapper.TranslateToILInt(il, i);
              Label lab = il.DefineLabel();
              il.Emit(OpCodes.Blt_S, lab);
              il.Emit(OpCodes.Pop);
              if (savedInsideFinally && i < savedFinallyStackTop)
                il.Emit(OpCodes.Rethrow);
              else
                il.Emit(OpCodes.Leave, (Label)compilerGlobals.BreakLabelStack.Peek(n-i));
              il.MarkLabel(lab);
            }
            il.Emit(OpCodes.Pop);
            il.BeginCatchBlock(Typeob.ContinueOutOfFinally);
            il.Emit(OpCodes.Ldfld, Typeob.ContinueOutOfFinally.GetField("target"));
            // don't need to go to 0 in the loop because 0 is the outmost block (i.e. function body)
            for (int i = compilerGlobals.ContinueLabelStack.Size()-1, n = i; i > 0; i--){
              il.Emit(OpCodes.Dup);
              ConstantWrapper.TranslateToILInt(il, i);
              Label lab = il.DefineLabel();
              il.Emit(OpCodes.Blt_S, lab);
              il.Emit(OpCodes.Pop);
              if (savedInsideFinally && i < savedFinallyStackTop)
                il.Emit(OpCodes.Rethrow);
              else
                il.Emit(OpCodes.Leave, (Label)compilerGlobals.ContinueLabelStack.Peek(n-i));
              il.MarkLabel(lab);
            }
            il.Emit(OpCodes.Pop);
            ScriptObject scope = Globals.ScopeStack.Peek();
            while (scope != null && !(scope is FunctionScope))
              scope = scope.GetParent();
            if (scope != null && !savedInsideFinally){
              il.BeginCatchBlock(Typeob.ReturnOutOfFinally);
              il.Emit(OpCodes.Pop);
              il.Emit(OpCodes.Leave, ((FunctionScope)scope).owner.returnLabel);
            }
            il.EndExceptionBlock();
          }
        }
        compilerGlobals.InsideProtectedRegion = savedInsideProtectedRegion;
        compilerGlobals.BreakLabelStack.Pop();
        compilerGlobals.ContinueLabelStack.Pop();
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        this.body.TranslateToILInitializer(il);
        if (this.handler != null)
          this.handler.TranslateToILInitializer(il);
        if (this.finally_block != null)
          this.finally_block.TranslateToILInitializer(il);
      }

    }
  
}
