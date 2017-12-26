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
    using System.Globalization;
        
    public sealed class FunctionExpression : AST{
      private FunctionObject func;
      private String name;
      private JSVariableField field;
      private LocalBuilder func_local;
      private static int uniqueNumber = 0;
    
      internal FunctionExpression(Context context, AST id, ParameterDeclaration[] formal_parameters, TypeExpression return_type, Block body, FunctionScope own_scope, FieldAttributes attributes)
        : base(context){
        if (attributes != (FieldAttributes)0){
          this.context.HandleError(JSError.SyntaxError);
          attributes = (FieldAttributes)0;
        }
        ScriptObject enclosingScope = Globals.ScopeStack.Peek();
        this.name = id.ToString();
        if (this.name.Length == 0)
          this.name = "anonymous "+(uniqueNumber++).ToString(CultureInfo.InvariantCulture);
        else
          this.AddNameTo(enclosingScope);
        this.func = new FunctionObject(this.name, formal_parameters, return_type, body, own_scope, enclosingScope, this.context, MethodAttributes.Static|MethodAttributes.Public);
      }

      void AddNameTo(ScriptObject enclosingScope){
        while (enclosingScope is WithObject) //Can only happen at run time and only if there is an eval
          enclosingScope = enclosingScope.GetParent();
        FieldInfo field = ((IActivationObject)enclosingScope).GetLocalField(this.name);
        if (field != null) return;
        if (enclosingScope is ActivationObject)
          if (enclosingScope is FunctionScope)
            field = ((ActivationObject)enclosingScope).AddNewField(this.name, null, FieldAttributes.Public);
          else
            field = ((ActivationObject)enclosingScope).AddNewField(this.name, null, FieldAttributes.Public|FieldAttributes.Static);
        else
          field = ((StackFrame)enclosingScope).AddNewField(this.name, null, FieldAttributes.Public);
        JSLocalField lfield = field as JSLocalField;
        if (lfield != null){
          // emit debug info for the local only if this block of code is in a section that has debug set
          lfield.debugOn = this.context.document.debugOn;
          lfield.isDefined = true;
        }
        this.field = (JSVariableField)field;
      }
    
      internal override Object Evaluate(){
        if( VsaEngine.executeForJSEE )
          throw new JScriptException(JSError.NonSupportedInDebugger);
        ScriptObject enclosingScope = Globals.ScopeStack.Peek();
        this.func.own_scope.SetParent(enclosingScope);
        Closure result = new Closure(this.func);
        if (this.field != null)
          this.field.value = result;
        return result;
      }

      internal override IReflect InferType(JSField inference_target){
        return Typeob.ScriptFunction;
      }
      
      public static FunctionObject JScriptFunctionExpression(RuntimeTypeHandle handle, String name, String method_name, String[] formal_params, JSLocalField[] fields,
      bool must_save_stack_locals, bool hasArgumentsObject, String text, VsaEngine engine){
        Type t = Type.GetTypeFromHandle(handle);
        FunctionObject result = new FunctionObject(t, name, method_name, formal_params, fields, must_save_stack_locals, hasArgumentsObject, text, engine);
        return result;
      }
      
      internal override AST PartiallyEvaluate(){
        ScriptObject enclosingScope = Globals.ScopeStack.Peek();
        if (ClassScope.ScopeOfClassMemberInitializer(enclosingScope) != null) {
          this.context.HandleError(JSError.MemberInitializerCannotContainFuncExpr);
          return this;
        }
        ScriptObject scope = enclosingScope;
        while (scope is WithObject || scope is BlockScope) scope = scope.GetParent();
        FunctionScope fscope = scope as FunctionScope;
        if (fscope != null) fscope.closuresMightEscape = true;
        if (scope != enclosingScope)
          this.func.own_scope.SetParent(new WithObject(new JSObject(), this.func.own_scope.GetGlobalScope()));
        this.func.PartiallyEvaluate();
        return this;
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        //This is executed every time the function expression is executed and constructs a new closure for every execution.
        if (rtype == Typeob.Void)
          return;
        il.Emit(OpCodes.Ldloc, this.func_local);
        il.Emit(OpCodes.Newobj, CompilerGlobals.closureConstructor);
        Convert.Emit(this, il, Typeob.Closure, rtype);
        if (this.field != null){
          il.Emit(OpCodes.Dup);
          Object tok = this.field.GetMetaData();
          if (tok is LocalBuilder)
            il.Emit(OpCodes.Stloc, (LocalBuilder)tok);
          else
            il.Emit(OpCodes.Stsfld, (FieldInfo)tok);
        }
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        //This is executed only once for a given scope. It creates a FunctionObject and stores it in a private local.
        //The FunctionObject is then used to construct a new closure every time the function expression is evaluated.
        //This way, what is a method to IL, ends up as a ScriptFunction object that behaves just like an ECMAScript function.
        this.func.TranslateToIL(compilerGlobals);
        this.func_local = il.DeclareLocal(Typeob.FunctionObject);
        il.Emit(OpCodes.Ldtoken, this.func.classwriter);
        il.Emit(OpCodes.Ldstr, this.name);
        il.Emit(OpCodes.Ldstr, this.func.GetName());
        int n = this.func.formal_parameters.Length;
        ConstantWrapper.TranslateToILInt(il, n);
        il.Emit(OpCodes.Newarr, Typeob.String);
        for (int i = 0; i < n; i++){
          il.Emit(OpCodes.Dup);
          ConstantWrapper.TranslateToILInt(il, i);
          il.Emit(OpCodes.Ldstr, this.func.formal_parameters[i]);
          il.Emit(OpCodes.Stelem_Ref);
        }
        n = this.func.fields.Length;
        ConstantWrapper.TranslateToILInt(il, n);
        il.Emit(OpCodes.Newarr, Typeob.JSLocalField);
        for (int i = 0; i < n; i++){
          JSLocalField field = this.func.fields[i];
          il.Emit(OpCodes.Dup);
          ConstantWrapper.TranslateToILInt(il, i);
          il.Emit(OpCodes.Ldstr, field.Name);
          il.Emit(OpCodes.Ldtoken, field.FieldType);
          ConstantWrapper.TranslateToILInt(il, field.slotNumber);
          il.Emit(OpCodes.Newobj, CompilerGlobals.jsLocalFieldConstructor);
          il.Emit(OpCodes.Stelem_Ref);
        }
        if (this.func.must_save_stack_locals)
          il.Emit(OpCodes.Ldc_I4_1);
        else
          il.Emit(OpCodes.Ldc_I4_0);
        if (this.func.hasArgumentsObject)
          il.Emit(OpCodes.Ldc_I4_1);
        else
          il.Emit(OpCodes.Ldc_I4_0);
        il.Emit(OpCodes.Ldstr, this.func.ToString());
        this.EmitILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.jScriptFunctionExpressionMethod);
        il.Emit(OpCodes.Stloc, this.func_local);
      }
    }
}
