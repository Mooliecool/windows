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
    
    public sealed class With : AST{
      private AST obj;
      private AST block;
      private Completion completion;
      private FunctionScope enclosing_function;
      
      internal With(Context context, AST obj, AST block)
        : base(context) {
        this.obj = obj;
        this.block = block;
        this.completion = new Completion();
        ScriptObject scope = Globals.ScopeStack.Peek();
        if (scope is FunctionScope)
          this.enclosing_function = (FunctionScope)scope;
        else
          this.enclosing_function = null;
      }
    
      internal override Object Evaluate(){
        try{
          JScriptWith(this.obj.Evaluate(), this.Engine);
        }catch(JScriptException e){
          e.context = this.obj.context;
          throw e;
        }
        Completion c = null;
        try{
          c = (Completion)this.block.Evaluate();
        }finally{
          Globals.ScopeStack.Pop();
        }
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
      
      public static Object JScriptWith(Object withOb, VsaEngine engine){
        Object ob = Convert.ToObject(withOb, engine);
        if (ob == null)
          throw new JScriptException(JSError.ObjectExpected);
        Globals glob = engine.Globals;
        glob.ScopeStack.GuardedPush(new WithObject(glob.ScopeStack.Peek(), ob));
        return ob;
      }
      
      internal override AST PartiallyEvaluate(){
        this.obj = this.obj.PartiallyEvaluate();
        WithObject wob;
        if (this.obj is ConstantWrapper){
          Object ob = Convert.ToObject(this.obj.Evaluate(), this.Engine);          
          wob = new WithObject(Globals.ScopeStack.Peek(), ob);
          if (ob is JSObject && ((JSObject)ob).noExpando)
            wob.isKnownAtCompileTime = true;
        }else
          wob = new WithObject(Globals.ScopeStack.Peek(), new JSObject(null, false));
        Globals.ScopeStack.Push(wob);
        try{
          this.block = this.block.PartiallyEvaluate();
        }finally{
          Globals.ScopeStack.Pop();
        }
        return this;
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        //This assumes that rtype == Void.class
        this.context.EmitLineInfo(il);
        Globals.ScopeStack.Push(new WithObject(Globals.ScopeStack.Peek(), new JSObject(null, false)));
        bool savedInsideProtectedRegion = compilerGlobals.InsideProtectedRegion;
        compilerGlobals.InsideProtectedRegion = true;
        Label lab = il.DefineLabel();
        compilerGlobals.BreakLabelStack.Push(lab);
        compilerGlobals.ContinueLabelStack.Push(lab);
        this.obj.TranslateToIL(il, Typeob.Object);
        this.EmitILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.jScriptWithMethod); // JScriptWith returns the with object as an 'Object' (used by the debugger EE)
        
        // define a local named 'with()' that the debugger EE will use to bind to the with object
        LocalBuilder withObj = null;
        if (context.document.debugOn){
          il.BeginScope(); // used by the debugger to mark a with block 
          withObj = il.DeclareLocal(Typeob.Object);
          withObj.SetLocalSymInfo("with()");
          il.Emit(OpCodes.Stloc, withObj);
        }else
          il.Emit(OpCodes.Pop);

        il.BeginExceptionBlock();
        this.block.TranslateToILInitializer(il);
        this.block.TranslateToIL(il, Typeob.Void);
        il.BeginFinallyBlock();
        if (context.document.debugOn){
          // null the local used by the debugger EE
          il.Emit(OpCodes.Ldnull);
          il.Emit(OpCodes.Stloc, withObj);
        }
        this.EmitILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.popScriptObjectMethod);
        il.Emit(OpCodes.Pop);
        il.EndExceptionBlock();
        if (context.document.debugOn)
          il.EndScope(); // used by the debugger to mark a with block 
        il.MarkLabel(lab);
        compilerGlobals.BreakLabelStack.Pop();
        compilerGlobals.ContinueLabelStack.Pop();
        compilerGlobals.InsideProtectedRegion = savedInsideProtectedRegion;
        Globals.ScopeStack.Pop();
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        this.obj.TranslateToILInitializer(il);
      }
    }
}
