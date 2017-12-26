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

    public sealed class Block : AST{
      private Completion completion;
      private ArrayList list;
      
      internal Block(Context context)
        : base(context) {
        this.completion = new Completion();
        this.list = new ArrayList();
      }
      
      internal void Append(AST elem){
        this.list.Add(elem);
      }

      internal void ComplainAboutAnythingOtherThanClassOrPackage(){
        for (int i = 0, n = this.list.Count; i < n; i++){
          Object elem = this.list[i];
          if (elem is Class || elem is Package || elem is Import) continue;
          Block b = elem as Block;
          if (b != null && b.list.Count == 0) continue;
          Expression e = elem as Expression;
          if (e != null && e.operand is AssemblyCustomAttributeList) continue;
          ((AST)elem).context.HandleError(JSError.OnlyClassesAndPackagesAllowed);
          return;
        }
      }
      
      internal override Object Evaluate(){
        this.completion.Continue = 0;
        this.completion.Exit = 0;
        this.completion.value = null;
        for (int i = 0, n = this.list.Count; i < n; i++){
          AST elem = (AST)(this.list[i]);
          Object val;
          try{
            val = elem.Evaluate();
          }catch(JScriptException e){
            if (e.context == null){
              e.context = elem.context;
            }
            throw e;
          }
          Completion c = (Completion)val;
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
        }
        return this.completion;
      }

      internal void EvaluateStaticVariableInitializers(){
        for (int i = 0, n = this.list.Count; i < n; i++){
          Object elem = this.list[i];
          VariableDeclaration vard = elem as VariableDeclaration;
          if (vard != null && vard.field.IsStatic && !vard.field.IsLiteral){
            vard.Evaluate();
            continue;
          }
          StaticInitializer sinit = elem as StaticInitializer;
          if (sinit != null){
            sinit.Evaluate();
            continue;
          }
          Class cl = elem as Class;
          if (cl != null){
            cl.Evaluate();
            continue;
          }
          Constant cnst = elem as Constant;
          if (cnst != null && cnst.field.IsStatic){
            cnst.Evaluate();
            continue;
          }
          Block block = elem as Block;
          if (block != null)
            block.EvaluateStaticVariableInitializers();
        }
      }

      internal void EvaluateInstanceVariableInitializers(){
        for (int i = 0, n = this.list.Count; i < n; i++){
          Object elem = this.list[i];
          VariableDeclaration vard = elem as VariableDeclaration;
          if (vard != null && !vard.field.IsStatic && !vard.field.IsLiteral){
            vard.Evaluate();
            continue;
          }
          Block block = elem as Block;
          if (block != null)
            block.EvaluateInstanceVariableInitializers();
        }
      }
        
      internal override bool HasReturn(){
        for (int i = 0, n = this.list.Count; i < n; i++){
          AST elem = (AST)(this.list[i]);
          if (elem.HasReturn())
            return true;
        }
        return false;
      }
      
      internal void ProcessAssemblyAttributeLists(){
        for (int i = 0, n = this.list.Count; i < n; i++){
          Expression expr = this.list[i] as Expression;
          if (expr != null){
            AssemblyCustomAttributeList acl = expr.operand as AssemblyCustomAttributeList;
            if (acl != null) acl.Process();
          }
        }
      }

      internal void MarkSuperOKIfIsFirstStatement(){
        if (this.list.Count > 0 && this.list[0] is ConstructorCall)
          ((ConstructorCall)this.list[0]).isOK = true;
      }
      
      internal override AST PartiallyEvaluate(){
        for (int i = 0, n = this.list.Count; i < n; i++){
          AST elem = (AST)(this.list[i]);
          this.list[i] = elem.PartiallyEvaluate();
        }
        return this;
      }
        
      internal Expression ToExpression(){
        if (this.list.Count == 1 && this.list[0] is Expression)
          return (Expression)this.list[0];
        else
          return null;
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        //This assumes that rtype == Void.class
        Label lab = il.DefineLabel();
        compilerGlobals.BreakLabelStack.Push(lab);
        compilerGlobals.ContinueLabelStack.Push(lab);
        for (int i = 0, n = this.list.Count; i < n; i++){
          AST elem = (AST)(this.list[i]);
          elem.TranslateToIL(il, Typeob.Void);
        }
        il.MarkLabel(lab);
        compilerGlobals.BreakLabelStack.Pop();
        compilerGlobals.ContinueLabelStack.Pop();
      }
        
      internal override void TranslateToILInitializer(ILGenerator il){
        for (int i = 0, n = this.list.Count; i < n; i++){
          AST elem = (AST)(this.list[i]);
          elem.TranslateToILInitializer(il);
        }
      }
      
      internal void TranslateToILInitOnlyInitializers(ILGenerator il){
        for (int i = 0, n = this.list.Count; i < n; i++){
          Constant c = this.list[i] as Constant;
          if (c != null) c.TranslateToILInitOnlyInitializers(il);
        }
      }

      internal void TranslateToILInstanceInitializers(ILGenerator il){
        for (int i = 0, n = this.list.Count; i < n; i++){
          AST elem = (AST)(this.list[i]);
          if (elem is VariableDeclaration && !((VariableDeclaration)elem).field.IsStatic && !((VariableDeclaration)elem).field.IsLiteral){
            elem.TranslateToILInitializer(il);
            elem.TranslateToIL(il, Typeob.Void);
          }else if (elem is FunctionDeclaration && !((FunctionDeclaration)elem).func.isStatic)
            elem.TranslateToILInitializer(il);
          else if (elem is Constant && !((Constant)elem).field.IsStatic)
            elem.TranslateToIL(il, Typeob.Void);
          else if (elem is Block)
            ((Block)elem).TranslateToILInstanceInitializers(il);
        }
      }
        
      internal void TranslateToILStaticInitializers(ILGenerator il){
        for (int i = 0, n = this.list.Count; i < n; i++){
          AST elem = (AST)(this.list[i]);
          if (elem is VariableDeclaration && ((VariableDeclaration)elem).field.IsStatic ||
              elem is Constant && ((Constant)elem).field.IsStatic){
            elem.TranslateToILInitializer(il);
            elem.TranslateToIL(il, Typeob.Void);
          }else if (elem is StaticInitializer)
            elem.TranslateToIL(il, Typeob.Void);
          else if (elem is FunctionDeclaration && ((FunctionDeclaration)elem).func.isStatic)
            elem.TranslateToILInitializer(il);
          else if (elem is Class)
            elem.TranslateToIL(il, Typeob.Void);
          else if (elem is Block)
            ((Block)elem).TranslateToILStaticInitializers(il);
        }
      }

      internal override Context GetFirstExecutableContext(){
        for (int i = 0, n = this.list.Count; i < n; i++){
          AST elem = (AST)(this.list[i]);
          Context ctx;
          if ((ctx = elem.GetFirstExecutableContext()) != null)
            return ctx;
        }
        return null;
      }
    }
}
