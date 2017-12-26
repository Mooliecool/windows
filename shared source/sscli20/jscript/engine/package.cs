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

    public sealed class Package : AST{
      private String name;
      private ASTList classList;
      private PackageScope scope;

      internal Package(String name, AST id, ASTList classList, Context context)
        : base(context){
        this.name = name;
        this.classList = classList;
        this.scope = (PackageScope)Globals.ScopeStack.Peek();
        this.scope.owner = this;
        this.Engine.AddPackage(this.scope);
        Lookup simpleId = id as Lookup;
        if (simpleId != null)
          simpleId.EvaluateAsWrappedNamespace(true);
        else{
          Member qualifiedId = id as Member;
          if (qualifiedId != null)
            qualifiedId.EvaluateAsWrappedNamespace(true);
        }
      }
      
      internal override Object Evaluate(){
        Globals.ScopeStack.Push(this.scope);
        try{
          for (int i = 0, n = this.classList.count; i < n; i++)
            this.classList[i].Evaluate();
          return new Completion();
        }finally{
          Globals.ScopeStack.Pop();
        }
      }

      public static void JScriptPackage(String rootName, VsaEngine engine){
        GlobalScope scope = ((IActivationObject)engine.ScriptObjectStackTop()).GetGlobalScope();
        FieldInfo field = scope.GetLocalField(rootName);
        if (field == null)
          field = scope.AddNewField(rootName, Namespace.GetNamespace(rootName, engine), FieldAttributes.Public|FieldAttributes.Literal);
      }
      
      internal void MergeWith(Package p){
        for (int i = 0, n = p.classList.count; i < n; i++)
          this.classList.Append(p.classList[i]);
        this.scope.MergeWith(p.scope);
      }
      
      internal override AST PartiallyEvaluate(){
        this.scope.AddOwnName();
        Globals.ScopeStack.Push(this.scope);
        try{
          for (int i = 0, n = this.classList.count; i < n; i++)
            this.classList[i].PartiallyEvaluate();
          return this;
        }finally{
          Globals.ScopeStack.Pop();
        }
      }
      
      internal override void TranslateToIL(ILGenerator il, Type rtype){
        Globals.ScopeStack.Push(this.scope);
        for (int i = 0, n = this.classList.count; i < n; i++)
          this.classList[i].TranslateToIL(il, Typeob.Void);
        Globals.ScopeStack.Pop();
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        String root = this.name;
        int j = root.IndexOf('.');
        if (j > 0)
          root = root.Substring(0, j);
        il.Emit(OpCodes.Ldstr, root);
        this.EmitILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.jScriptPackageMethod);
        Globals.ScopeStack.Push(this.scope);
        for (int i = 0, n = this.classList.count; i < n; i++)
          this.classList[i].TranslateToILInitializer(il);
        Globals.ScopeStack.Pop();
      }

      internal override Context GetFirstExecutableContext(){
        return null;
      }
   }
}
