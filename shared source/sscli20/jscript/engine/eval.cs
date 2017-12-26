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
    using System.Security.Permissions;


    public sealed class Eval : AST{
      private AST operand;
      private AST unsafeOption;
      private FunctionScope enclosingFunctionScope;

      internal Eval(Context context, AST operand, AST unsafeOption)
        : base(context) {
        this.operand = operand;
        this.unsafeOption = unsafeOption;

        ScriptObject enclosingScope = Globals.ScopeStack.Peek();
        ((IActivationObject)enclosingScope).GetGlobalScope().evilScript = true;
        if (enclosingScope is ActivationObject)
          ((ActivationObject)enclosingScope).isKnownAtCompileTime = this.Engine.doFast;
        if (enclosingScope is FunctionScope){
          this.enclosingFunctionScope = (FunctionScope)enclosingScope;
          this.enclosingFunctionScope.mustSaveStackLocals = true;
          ScriptObject scope = (ScriptObject)this.enclosingFunctionScope.GetParent();
          while (scope != null){
            FunctionScope fscope = scope as FunctionScope;
            if (fscope != null){
              fscope.mustSaveStackLocals = true;
              fscope.closuresMightEscape = true;
            }
            scope = (ScriptObject)scope.GetParent();
          }
        }else
          this.enclosingFunctionScope = null;
      }

      internal override void CheckIfOKToUseInSuperConstructorCall(){
        this.context.HandleError(JSError.NotAllowedInSuperConstructorCall);
      }

      internal override Object Evaluate(){
        if( VsaEngine.executeForJSEE )
          throw new JScriptException(JSError.NonSupportedInDebugger);
        Object v = this.operand.Evaluate();
        Object u = null;
        if (this.unsafeOption != null)
          u = this.unsafeOption.Evaluate();
        Globals.CallContextStack.Push(new CallContext(this.context, null, new Object[]{v, u}));
        try{
          try{
            return JScriptEvaluate(v, u, this.Engine);
          }catch(JScriptException e){
            if (e.context == null){
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

      public static Object JScriptEvaluate(Object source, VsaEngine engine){
        if (Convert.GetTypeCode(source) != TypeCode.String)
          return source;
        return Eval.DoEvaluate(source, engine, true);
      }

      public static Object JScriptEvaluate(Object source, Object unsafeOption, VsaEngine engine){
        if (Convert.GetTypeCode(source) != TypeCode.String)
          return source;
        bool isUnsafe = false;
        if (Convert.GetTypeCode(unsafeOption) == TypeCode.String){
          if (((IConvertible)unsafeOption).ToString() == "unsafe")
            isUnsafe = true;
        }
        return Eval.DoEvaluate(source, engine, isUnsafe);
      }

      private static Object DoEvaluate(Object source, VsaEngine engine, bool isUnsafe){
        if (engine.doFast)
          engine.PushScriptObject(new BlockScope(engine.ScriptObjectStackTop()));
        try{
          Context context = new Context(new DocumentContext("eval code", engine), ((IConvertible)source).ToString());
          JSParser p = new JSParser(context);
          if (!isUnsafe)
            new SecurityPermission(SecurityPermissionFlag.Execution).PermitOnly();
          return ((Completion)p.ParseEvalBody().PartiallyEvaluate().Evaluate()).value;
        }finally{
          if (engine.doFast)
            engine.PopScriptObject();
        }

      }

      internal override AST PartiallyEvaluate(){
        VsaEngine engine = this.Engine;
        ScriptObject scope = Globals.ScopeStack.Peek();
        
        ClassScope cscope = ClassScope.ScopeOfClassMemberInitializer(scope);
        if (null != cscope) {
          if (cscope.inStaticInitializerCode) 
            cscope.staticInitializerUsesEval = true;
          else 
            cscope.instanceInitializerUsesEval = true;
        }
          
        if (engine.doFast)
          engine.PushScriptObject(new BlockScope(scope));
        else{
          while (scope is WithObject || scope is BlockScope){
            if (scope is BlockScope)
              ((BlockScope)scope).isKnownAtCompileTime = false;
            scope = scope.GetParent();
          }
        }
        try{
          this.operand = this.operand.PartiallyEvaluate();
          if (this.unsafeOption != null)
            this.unsafeOption = this.unsafeOption.PartiallyEvaluate();
          if (this.enclosingFunctionScope != null && this.enclosingFunctionScope.owner == null)
            this.context.HandleError(JSError.NotYetImplemented);
          return this;
        }finally{
          if (engine.doFast)
            this.Engine.PopScriptObject();
        }
      }

      internal override void TranslateToIL(ILGenerator il, Type rtype){
        if (this.enclosingFunctionScope != null && this.enclosingFunctionScope.owner != null)
          this.enclosingFunctionScope.owner.TranslateToILToSaveLocals(il);
        this.operand.TranslateToIL(il, Typeob.Object);
        MethodInfo evaluateMethod = null;
        ConstantWrapper cw = this.unsafeOption as ConstantWrapper;
        if (cw != null){
          string s = cw.value as string;
          if (s != null && s == "unsafe")
            evaluateMethod = CompilerGlobals.jScriptEvaluateMethod1;
        }
        if (evaluateMethod == null){
          evaluateMethod = CompilerGlobals.jScriptEvaluateMethod2;
          if (this.unsafeOption == null)
            il.Emit(OpCodes.Ldnull);
          else
            this.unsafeOption.TranslateToIL(il, Typeob.Object);
        }
        this.EmitILToLoadEngine(il);
        il.Emit(OpCodes.Call, evaluateMethod);
        Convert.Emit(this, il, Typeob.Object, rtype);
        if (this.enclosingFunctionScope != null && this.enclosingFunctionScope.owner != null)
          this.enclosingFunctionScope.owner.TranslateToILToRestoreLocals(il);
      }

      internal override void TranslateToILInitializer(ILGenerator il){
        this.operand.TranslateToILInitializer(il);
        if (this.unsafeOption != null)
          this.unsafeOption.TranslateToILInitializer(il);
      }
    }
}
