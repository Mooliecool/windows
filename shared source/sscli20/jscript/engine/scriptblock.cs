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
    using System.Reflection;
    using System.Reflection.Emit;
    using System.Globalization;
        
    public class ScriptBlock : AST{
      private Block statement_block;
      private JSField[] fields;
      private GlobalScope own_scope;
      
      internal ScriptBlock(Context context, Block statement_block)
        : base(context) {
        this.statement_block = statement_block;
        this.own_scope = (GlobalScope)Engine.ScriptObjectStackTop();
        this.fields = null;
      }
      
      internal override Object Evaluate(){
        if (this.fields == null)
          this.fields = this.own_scope.GetFields();
        for (int i = 0, nf = this.fields.Length; i < nf; i++){
          FieldInfo field = this.fields[i];
          if (field is JSExpandoField) continue;
          Object val = field.GetValue(this.own_scope);
          if (val is FunctionObject){
            ((FunctionObject)val).engine = this.Engine;
            this.own_scope.AddFieldOrUseExistingField(field.Name, new Closure((FunctionObject)val), field.Attributes);
          }else if (val is ClassScope)
            this.own_scope.AddFieldOrUseExistingField(field.Name, val, field.Attributes);
          else
            this.own_scope.AddFieldOrUseExistingField(field.Name, Missing.Value, field.Attributes);
        }
        Object res = this.statement_block.Evaluate();
        if (res is Completion)
          res = ((Completion)res).value;
        return res;
      }

      internal void ProcessAssemblyAttributeLists(){
        this.statement_block.ProcessAssemblyAttributeLists();
      }
    
      internal override AST PartiallyEvaluate(){
        this.statement_block.PartiallyEvaluate();
        if (this.Engine.PEFileKind == PEFileKinds.Dll && this.Engine.doSaveAfterCompile)
          this.statement_block.ComplainAboutAnythingOtherThanClassOrPackage();
        this.fields = this.own_scope.GetFields();
        return this;
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        Expression expr = this.statement_block.ToExpression();
        if (expr != null)
          expr.TranslateToIL(il, rtype);
        else{
          this.statement_block.TranslateToIL(il, Typeob.Void);
          (new ConstantWrapper(null, this.context)).TranslateToIL(il, rtype);
        }
      }
      
      internal TypeBuilder TranslateToILClass (CompilerGlobals compilerGlobals){
        return TranslateToILClass(compilerGlobals, true);
      }

      internal TypeBuilder TranslateToILClass(CompilerGlobals compilerGlobals, bool pushScope){
        TypeBuilder classwriter = compilerGlobals.classwriter = 
          compilerGlobals.module.DefineType("JScript "+(this.Engine.classCounter++).ToString(CultureInfo.InvariantCulture), TypeAttributes.Public, Typeob.GlobalScope, null);
        compilerGlobals.classwriter.SetCustomAttribute(new CustomAttributeBuilder(CompilerGlobals.compilerGlobalScopeAttributeCtor, new Object[0]));
        if (null == compilerGlobals.globalScopeClassWriter)  
          compilerGlobals.globalScopeClassWriter = classwriter;
          
        //Define a constructor that calls the appropriate constructor on GlobalScope
        ConstructorBuilder cons = compilerGlobals.classwriter.DefineConstructor(MethodAttributes.Public, CallingConventions.Standard, new Type[]{Typeob.GlobalScope});
        ILGenerator il = cons.GetILGenerator();
        il.Emit(OpCodes.Ldarg_0);
        il.Emit(OpCodes.Ldarg_1);
        il.Emit(OpCodes.Dup);
        il.Emit(OpCodes.Ldfld, CompilerGlobals.engineField);
        il.Emit(OpCodes.Call, CompilerGlobals.globalScopeConstructor);
        il.Emit(OpCodes.Ret);
        
        //Define a method to contain the global code
        MethodBuilder mw = classwriter.DefineMethod("Global Code", MethodAttributes.Public, Typeob.Object, null);
        il = mw.GetILGenerator();
        
        if (this.Engine.GenerateDebugInfo){
          ScriptObject ns = this.own_scope.GetParent();
          while (ns != null){
            if (ns is WrappedNamespace && !((WrappedNamespace)ns).name.Equals(""))
              il.UsingNamespace(((WrappedNamespace)ns).name);
            ns = ns.GetParent();
          }
        }
      
        int startLine = this.context.StartLine;
        int startCol = this.context.StartColumn;
        //this.context.document.EmitLineInfo(il, startLine, startCol, startLine, startCol + 1); // NOTE: make the debugger stop at line 1 in the jscript source instead of in prolog code
        Context firstContext = this.GetFirstExecutableContext();
        if (firstContext != null)
          firstContext.EmitFirstLineInfo(il);

        if (pushScope) {
          //Script block for VsaStaticScript doesn't need to push object onto
          //the stack because the constructors for are stashed into the GlobalScope
          //object when they are constructed.
          
          //Emit code to push the scope onto the stack for use by eval...
          this.EmitILToLoadEngine(il);
          il.Emit(OpCodes.Ldarg_0);
          il.Emit(OpCodes.Call, CompilerGlobals.pushScriptObjectMethod);
        }

        this.TranslateToILInitializer(il);
        this.TranslateToIL(il, Typeob.Object);
    
        if (pushScope) {
          //... and corresponding code to pop it off.
          this.EmitILToLoadEngine(il);
          il.Emit(OpCodes.Call, CompilerGlobals.popScriptObjectMethod);
          il.Emit(OpCodes.Pop);
        }
        
        il.Emit(OpCodes.Ret);
        
        return classwriter;
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        int nf = this.fields.Length;
        if (nf > 0){
          for (int i = 0; i < nf; i++){
            JSGlobalField field = this.fields[i] as JSGlobalField;
            if (field == null) continue;
            Type t = field.FieldType;
            if ((field.IsLiteral && t != Typeob.ScriptFunction && t != Typeob.Type) || field.metaData != null){
              if ((t.IsPrimitive || t == Typeob.String || t.IsEnum) && field.metaData == null){
                FieldBuilder b = compilerGlobals.classwriter.DefineField(field.Name, t, field.Attributes);
                b.SetConstant(field.value);
              }
              continue;
            }
            if (field.value is FunctionObject && ((FunctionObject)field.value).suppressIL)
              continue;
            FieldBuilder fb = compilerGlobals.classwriter.DefineField(field.Name, t, 
              field.Attributes&(~(FieldAttributes.Literal|FieldAttributes.InitOnly))|FieldAttributes.Static);
            field.metaData = fb;
            field.WriteCustomAttribute(this.Engine.doCRS);
            //There is no need to store Closure objects in the fields corresponding to functions, since the initializer code
            //for functions takes care of it. Likewise for Classes.
          }
        }
        this.statement_block.TranslateToILInitializer(il);
      }

      internal override Context GetFirstExecutableContext(){
        return this.statement_block.GetFirstExecutableContext();
      }
    }
}
