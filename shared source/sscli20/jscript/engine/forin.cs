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
    
    public sealed class ForIn : AST{
      private AST var;
      private AST initializer;
      private AST collection;
      private AST body;
      private Completion completion;
      private Context inExpressionContext;
        
      internal ForIn(Context context, AST var, AST initializer, AST collection, AST body)
        : base(context) {
        if (var != null){
          this.var = var;
          this.inExpressionContext = this.var.context.Clone();
        }else{
          VariableDeclaration decl = (VariableDeclaration)initializer;
          this.var = decl.identifier;
          if (decl.initializer == null)
            decl.initializer = new ConstantWrapper(null, null); //Suppress warning
          this.inExpressionContext = initializer.context.Clone();
        }
        this.initializer = initializer;
        this.collection = collection;
        this.inExpressionContext.UpdateWith(this.collection.context);
        this.body = body;
        this.completion = new Completion();
      }
    
      internal override Object Evaluate(){
        AST r = this.var;
        if (this.initializer != null)
          this.initializer.Evaluate();                                     
        this.completion.Continue = 0;
        this.completion.Exit = 0;
        this.completion.value = null;
        Object coll = Convert.ToForInObject(this.collection.Evaluate(), this.Engine);
        bool isScriptObject = coll is ScriptObject;
        IEnumerator e = null;
        try{
          e = JScriptGetEnumerator(coll);
        }catch(JScriptException x){
          x.context = this.collection.context;
          throw x;
        }
        while (e.MoveNext()){
          r.SetValue(e.Current);
          Completion c = (Completion)this.body.Evaluate();
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
        }
        return this.completion;
      }
      
      public static IEnumerator JScriptGetEnumerator(Object coll){
        if (coll is IEnumerator)
          return (IEnumerator)coll;
        if (coll is ScriptObject)
          return new ScriptObjectPropertyEnumerator((ScriptObject)coll);
        if (coll is Array){
          Array arr = (Array)coll;
          return new RangeEnumerator(arr.GetLowerBound(0), arr.GetUpperBound(0));
        }
        if (coll is IEnumerable){
          IEnumerator result =  ((IEnumerable)coll).GetEnumerator();
          if (result != null) return result;
          return new ScriptObjectPropertyEnumerator(new JSObject());
        }
        throw new JScriptException(JSError.NotCollection);
      }
    
      internal override AST PartiallyEvaluate(){
        this.var = this.var.PartiallyEvaluateAsReference();
        this.var.SetPartialValue(new ConstantWrapper(null, null));
        if (this.initializer != null)
          this.initializer = this.initializer.PartiallyEvaluate();
        this.collection = this.collection.PartiallyEvaluate();
        IReflect ctype = this.collection.InferType(null);
        if ((ctype is ClassScope && ((ClassScope)ctype).noExpando && !((ClassScope)ctype).ImplementsInterface(Typeob.IEnumerable)) ||
            (ctype != Typeob.Object && ctype is Type && !(Typeob.ScriptObject.IsAssignableFrom((Type)ctype)) &&
            !Typeob.IEnumerable.IsAssignableFrom((Type)ctype) && !Typeob.IConvertible.IsAssignableFrom((Type)ctype))
            &&!Typeob.IEnumerator.IsAssignableFrom((Type)ctype))
          this.collection.context.HandleError(JSError.NotCollection);
        ScriptObject current_scope = Globals.ScopeStack.Peek();
        while (current_scope is WithObject) current_scope = current_scope.GetParent();
        if (current_scope is FunctionScope){
          FunctionScope scope = (FunctionScope)current_scope;
          BitArray before = scope.DefinedFlags;
          this.body = this.body.PartiallyEvaluate();
          scope.DefinedFlags = before;
        }else
          this.body = this.body.PartiallyEvaluate();
        return this;
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        //This assumes that rtype == Void.class
        Label loop_start = il.DefineLabel();
        Label loop_end = il.DefineLabel();
        Label body = il.DefineLabel();
        compilerGlobals.BreakLabelStack.Push(loop_end);
        compilerGlobals.ContinueLabelStack.Push(loop_start);
        if (this.initializer != null){
          this.initializer.TranslateToIL(il, Typeob.Void);
        }
        this.inExpressionContext.EmitLineInfo(il);
        this.collection.TranslateToIL(il, Typeob.Object);
        this.EmitILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.toForInObjectMethod);
        il.Emit(OpCodes.Call, CompilerGlobals.jScriptGetEnumeratorMethod);
        LocalBuilder enumerator = il.DeclareLocal(Typeob.IEnumerator);
        il.Emit(OpCodes.Stloc, enumerator);
        il.Emit(OpCodes.Br, loop_start);
        il.MarkLabel(body);
        this.body.TranslateToIL(il, Typeob.Void);
        il.MarkLabel(loop_start);
        this.context.EmitLineInfo(il);
        il.Emit(OpCodes.Ldloc, enumerator);
        il.Emit(OpCodes.Callvirt, CompilerGlobals.moveNextMethod);
        il.Emit(OpCodes.Brfalse, loop_end);
        il.Emit(OpCodes.Ldloc, enumerator);
        il.Emit(OpCodes.Callvirt, CompilerGlobals.getCurrentMethod);
        Type vt = Convert.ToType(this.var.InferType(null));
        LocalBuilder val = il.DeclareLocal(vt);
        Convert.Emit(this, il, Typeob.Object, vt);
        il.Emit(OpCodes.Stloc, val);
        this.var.TranslateToILPreSet(il);
        il.Emit(OpCodes.Ldloc, val);
        this.var.TranslateToILSet(il);
        il.Emit(OpCodes.Br, body);
        il.MarkLabel(loop_end);
        compilerGlobals.BreakLabelStack.Pop();
        compilerGlobals.ContinueLabelStack.Pop();
      }
        
      internal override void TranslateToILInitializer(ILGenerator il){
        this.var.TranslateToILInitializer(il);
        if (this.initializer != null)
          this.initializer.TranslateToILInitializer(il);
        this.collection.TranslateToILInitializer(il);
        this.body.TranslateToILInitializer(il);
      }
    }
}
