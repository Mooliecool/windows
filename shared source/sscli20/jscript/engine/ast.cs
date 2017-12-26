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
    
    public abstract class AST{
      internal Context context;
     
      internal AST(Context context){
        this.context = context;
      }
      
      internal virtual void CheckIfOKToUseInSuperConstructorCall(){
      }
      
      internal CompilerGlobals compilerGlobals{
        get{
          return this.context.document.compilerGlobals;
        }
      }
      
      internal virtual bool Delete(){
        return true; //This is according to spec, but not backwards compatible with JS5. The latter throws an exception.
      }

      //Run up the scope chain until a FunctionScope or GlobalScope/StackFrame is encountered.
      //For a FunctionScope, defer to the owner.
      //For a ClassScope, get the engine from a CRS static, or make a new engine
      //For the GlobalScope, load the corresponding field of the this object.

      internal void EmitILToLoadEngine(ILGenerator il){
        ScriptObject scope = this.Engine.ScriptObjectStackTop();
        while (scope != null && (scope is WithObject || scope is BlockScope))
          scope = scope.GetParent();
        if (scope is FunctionScope)
            ((FunctionScope)scope).owner.TranslateToILToLoadEngine(il);
          /*
            else //inside a static initializer
            if (this.Engine.doCRS)        
              il.Emit(OpCodes.Ldsfld, CompilerGlobals.contextEngineField);
            else
              il.Emit(OpCodes.Call, CompilerGlobals.createVsaEngine);*/
        else if (scope is ClassScope) //Inside a static initializer routine
          if (this.Engine.doCRS)        
            il.Emit(OpCodes.Ldsfld, CompilerGlobals.contextEngineField);
          else{
            if (this.context.document.engine.PEFileKind == PEFileKinds.Dll){
              il.Emit(OpCodes.Ldtoken, ((ClassScope)scope).GetTypeBuilder());
              il.Emit(OpCodes.Call, CompilerGlobals.createVsaEngineWithType);
            }else
              il.Emit(OpCodes.Call, CompilerGlobals.createVsaEngine);
          }
        else{
          //NOTE: a StackFrame is ecountered at compile time only when there is an eval. 
          //Eval does not translate to IL so we should only get here if the scope is a GlobalScope
          Debug.Assert(scope is GlobalScope);
          il.Emit(OpCodes.Ldarg_0);
          il.Emit(OpCodes.Ldfld, CompilerGlobals.engineField);
        }
      }
      
      internal VsaEngine Engine{
        get{
          return this.context.document.engine;
        }
      }
      
      internal abstract Object Evaluate();
      
      internal virtual LateBinding EvaluateAsLateBinding(){
        return new LateBinding(null, this.Evaluate(), VsaEngine.executeForJSEE);
      }
    
      internal virtual WrappedNamespace EvaluateAsWrappedNamespace(bool giveErrorIfNameInUse){
        throw new JScriptException(JSError.InternalError, this.context);
      }      
      
      internal Globals Globals{
        get{
          return this.context.document.engine.Globals;
        }
      }
      
      internal virtual bool HasReturn(){
        return false;
      }
      
      internal virtual IReflect InferType(JSField inference_target){
        return Typeob.Object;
      }
      
    
      internal virtual void InvalidateInferredTypes(){
        //No need to do anything at this level.
      }
      
      internal virtual bool OkToUseAsType(){
        return false;
      }
      
      internal abstract AST PartiallyEvaluate();

      internal virtual AST PartiallyEvaluateAsCallable(){
        return new CallableExpression(this.PartiallyEvaluate());
      }
      
      internal virtual AST PartiallyEvaluateAsReference(){
        return this.PartiallyEvaluate();
      }

      internal virtual void ResolveCall(ASTList args, IReflect[] argIRs, bool constructor, bool brackets){
        throw new JScriptException(JSError.InternalError, this.context);
      }
  
      internal virtual Object ResolveCustomAttribute(ASTList args, IReflect[] argIRs, AST target){
        throw new JScriptException(JSError.InternalError, this.context);
      }
      
      internal virtual void SetPartialValue(AST partial_value){
        this.context.HandleError(JSError.IllegalAssignment);
      }
      
      internal virtual void SetValue(Object value){
        this.context.HandleError(JSError.IllegalAssignment);
      }
      
      internal virtual void TranslateToConditionalBranch(ILGenerator il, bool branchIfTrue, Label label, bool shortForm){
        //This method is overridden in all interesting cases.
        IReflect ir = this.InferType(null);
        if (ir != Typeob.Object && ir is Type){
          String op = branchIfTrue ? "op_True" : "op_False";
          MethodInfo meth = ir.GetMethod(op, BindingFlags.ExactBinding|BindingFlags.Public|BindingFlags.Static, null, new Type[]{(Type)ir}, null);
          if (meth != null){
            this.TranslateToIL(il, (Type)ir);
            il.Emit(OpCodes.Call, meth);
            il.Emit(OpCodes.Brtrue, label);
            return;
          }
        }
        Type t = Convert.ToType(ir);
        this.TranslateToIL(il, t);
        Convert.Emit(this, il, t, Typeob.Boolean, true);
        if (branchIfTrue)
          il.Emit(shortForm ? OpCodes.Brtrue_S : OpCodes.Brtrue, label);
        else
          il.Emit(shortForm ? OpCodes.Brfalse_S : OpCodes.Brfalse, label);
      }
    
      internal abstract void TranslateToIL(ILGenerator il, Type rtype);

      internal virtual void TranslateToILCall(ILGenerator il, Type rtype, ASTList args, bool construct, bool brackets){
        throw new JScriptException(JSError.InternalError, this.context);
      }
      
      internal virtual void TranslateToILDelete(ILGenerator il, Type rtype){
        if (rtype != Typeob.Void){
          il.Emit(OpCodes.Ldc_I4_1); //This is according to spec, but not backwards compatible with JS5. The latter throws an exception.
          Convert.Emit(this, il, Typeob.Boolean, rtype);
        }
      }
      
      internal virtual void TranslateToILInitializer(ILGenerator il){
        throw new JScriptException(JSError.InternalError, this.context);
      }
    
      internal virtual void TranslateToILPreSet(ILGenerator il){
        throw new JScriptException(JSError.InternalError, this.context);
      }

      internal virtual void TranslateToILPreSet(ILGenerator il, ASTList args){
        this.TranslateToIL(il, Typeob.Object);
        args.TranslateToIL(il, Typeob.ArrayOfObject);
      }
      
      internal virtual void TranslateToILPreSetPlusGet(ILGenerator il){
        throw new JScriptException(JSError.InternalError, this.context);
      }

      internal virtual void TranslateToILPreSetPlusGet(ILGenerator il, ASTList args, bool inBrackets){
        il.Emit(OpCodes.Ldnull); //Put a dummy this obj on the stack for CallValue's sake
        this.TranslateToIL(il, Typeob.Object);
        il.Emit(OpCodes.Dup);
        LocalBuilder savedOb = il.DeclareLocal(Typeob.Object);
        il.Emit(OpCodes.Stloc, savedOb);
        args.TranslateToIL(il, Typeob.ArrayOfObject);
        il.Emit(OpCodes.Dup);
        LocalBuilder savedArgs = il.DeclareLocal(Typeob.ArrayOfObject);
        il.Emit(OpCodes.Stloc, savedArgs);
        il.Emit(OpCodes.Ldc_I4_0);
        if (inBrackets)
          il.Emit(OpCodes.Ldc_I4_1);
        else
          il.Emit(OpCodes.Ldc_I4_0);
        this.EmitILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.callValueMethod);
        LocalBuilder savedResult = il.DeclareLocal(Typeob.Object);
        il.Emit(OpCodes.Stloc, savedResult);
        il.Emit(OpCodes.Ldloc, savedOb);
        il.Emit(OpCodes.Ldloc, savedArgs);
        il.Emit(OpCodes.Ldloc, savedResult);
      }

      internal void TranslateToILSet(ILGenerator il){
        this.TranslateToILSet(il, null);
      }
      
      internal virtual void TranslateToILSet(ILGenerator il, AST rhvalue){
        if (rhvalue != null)
          rhvalue.TranslateToIL(il, Typeob.Object);
        il.Emit(OpCodes.Call, CompilerGlobals.setIndexedPropertyValueStaticMethod);
      }
    
      internal virtual Object TranslateToILReference(ILGenerator il, Type rtype){
        this.TranslateToIL(il, rtype);
        LocalBuilder tok = il.DeclareLocal(rtype);
        il.Emit(OpCodes.Stloc, tok);
        il.Emit(OpCodes.Ldloca, tok);
        return tok;
      }

      internal virtual Context GetFirstExecutableContext(){
        return this.context;
      }


    }
}
