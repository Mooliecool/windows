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

    public sealed class Import : AST{
      private String name;
      
      internal Import(Context context, AST name)
        : base(context) {
        if (name == null) //could happen if constructed while in error recovery mode
          return;
        WrappedNamespace ns = name.EvaluateAsWrappedNamespace(true);
        this.Engine.SetEnclosingContext(ns);
        this.name = ns.name;
      }

      internal override Object Evaluate(){
        return new Completion();
      }
      
      internal override AST PartiallyEvaluate(){
        return this;
      }
      
      public static void JScriptImport(String name, VsaEngine engine){
        int dotPos = name.IndexOf('.');
        String rootName = dotPos > 0 ? name.Substring(0, dotPos) : name;
        GlobalScope scope = ((IActivationObject)engine.ScriptObjectStackTop()).GetGlobalScope();
        FieldInfo field = scope.GetLocalField(rootName);
        if (field == null)
          field = scope.AddNewField(rootName, Namespace.GetNamespace(rootName, engine), FieldAttributes.Public|FieldAttributes.Literal);
        engine.SetEnclosingContext(new WrappedNamespace(name, engine, false));
      }

      internal override void TranslateToIL(ILGenerator il, Type rtype){
      }
      
      internal override void TranslateToILInitializer(ILGenerator il){
        il.Emit(OpCodes.Ldstr, this.name);
        this.EmitILToLoadEngine(il);
        il.Emit(OpCodes.Call, CompilerGlobals.jScriptImportMethod);
      }

    }

}
