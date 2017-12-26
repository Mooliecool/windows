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
    using Microsoft.Vsa;
    using System.Globalization;
    using System.IO;
    using System.CodeDom;
    using System.Collections.Specialized;

    internal class VsaStaticCode : VsaItem, IVsaCodeItem {
      internal Context codeContext;
      private Type compiledClass;
      private ScriptBlock block;

      internal VsaStaticCode(VsaEngine engine, string itemName, VsaItemFlag flag)
        : base(engine, itemName, VsaItemType.Code, flag) {
        this.compiledClass = null;
        this.codeContext = new Context(new DocumentContext(this), "");
      }

      public void AddEventSource(string eventSourceName, string eventSourceType) {
        // JScript does not support adding event sources (user must add an AppGlobal instead and manually hook up event handlers)
        if (this.engine == null)
          throw new VsaException(VsaError.EngineClosed);
        throw new System.NotSupportedException();
      }

      public System.CodeDom.CodeObject CodeDOM {
        get {
          if (this.engine == null)
            throw new VsaException(VsaError.EngineClosed);
          throw new VsaException(VsaError.CodeDOMNotAvailable);
        }
      }

      public void AppendSourceText(string SourceCode)
      {
        if (this.engine == null)
          throw new VsaException(VsaError.EngineClosed);
        if (SourceCode == null || SourceCode.Length == 0) return;
        this.codeContext.SetSourceContext(this.codeContext.document, this.codeContext.source_string + SourceCode);
        this.compiledClass = null;
        this.isDirty = true;
        this.engine.IsDirty = true;
      }

      internal override void CheckForErrors() {
        if (this.compiledClass == null) {
          JSParser p = new JSParser(this.codeContext);
          AST prog = (ScriptBlock)p.Parse();
          //prog.PartiallyEvaluate();
        }
      }

      internal override void Close() {
        base.Close();
        this.codeContext = null;
        this.compiledClass = null;
      }

      internal override Type GetCompiledType() {
        TypeBuilder tb = this.compiledClass as TypeBuilder;
        if (tb != null) this.compiledClass = tb.CreateType();
        return this.compiledClass;
      }

      public override String Name{ 
        set{
          base.Name = value;
          if (this.codebase == null){
            // The codeContext document was named according to the engine's RootMoniker and the item name
            string rootMoniker = this.engine.RootMoniker;
            this.codeContext.document.documentName = rootMoniker + (rootMoniker.EndsWith("/", StringComparison.Ordinal) ? "" : "/") + this.name;
          }
        }
      }

      internal void Parse() {
        if (this.block == null && this.compiledClass == null) {
          GlobalScope glob = (GlobalScope)this.engine.GetGlobalScope().GetObject();
          //Provide for the possibility of forward references to declarations in code blocks yet to come.
          glob.evilScript = !glob.fast || this.engine.GetStaticCodeBlockCount() > 1;
          this.engine.Globals.ScopeStack.Push(glob);
          try{
            JSParser p = new JSParser(this.codeContext);
            this.block = (ScriptBlock)p.Parse();
            if (p.HasAborted)
              this.block = null;
          }finally{
            this.engine.Globals.ScopeStack.Pop();
          }
        }
      }

      internal void ProcessAssemblyAttributeLists(){
        if(this.block == null) return;
        this.block.ProcessAssemblyAttributeLists();
      }

      internal void PartiallyEvaluate(){
        if (this.block != null && this.compiledClass == null){
          GlobalScope glob = (GlobalScope)this.engine.GetGlobalScope().GetObject();
          this.engine.Globals.ScopeStack.Push(glob);
          try{
            this.block.PartiallyEvaluate();
            if (this.engine.HasErrors && !this.engine.alwaysGenerateIL)
              throw new EndOfFile();
          }finally{
            this.engine.Globals.ScopeStack.Pop();
          }
        }
      }

      internal override void Remove() {
        if (this.engine == null)
          throw new VsaException(VsaError.EngineClosed);
        base.Remove();
      }

      public void RemoveEventSource(string eventSourceName)
      {
        if (this.engine == null)
          throw new VsaException(VsaError.EngineClosed);
        throw new System.NotSupportedException();
      }

      internal override void Reset() {
        this.compiledClass = null;
        this.block = null;
        // We reset codeContext because the DocumentContext holds a reference to engine.compilerGlobals
        // (which we must have set to null before calling Reset on the VsaItems).
        this.codeContext = new Context(new DocumentContext(this), this.codeContext.source_string);
      }

      internal override void Run() {
        if (this.compiledClass != null) {
          GlobalScope scriptBlockScope = (GlobalScope)Activator.CreateInstance(this.GetCompiledType(), new Object[]{this.engine.GetGlobalScope().GetObject()});
          this.engine.Globals.ScopeStack.Push(scriptBlockScope);
          try {
            MethodInfo main = this.compiledClass.GetMethod("Global Code");
            try{
              main.Invoke(scriptBlockScope, null);
            }catch(TargetInvocationException e){
              throw e.InnerException;
            }
          }finally{
            this.engine.Globals.ScopeStack.Pop();
          }
        }
      }

      public override void SetOption(String name, Object value){
        if (this.engine == null)
          throw new VsaException(VsaError.EngineClosed);
        if (0 == String.Compare(name, "codebase", StringComparison.OrdinalIgnoreCase)){
          this.codebase = (string)value;
          this.codeContext.document.documentName = this.codebase;
        }else
          throw new VsaException(VsaError.OptionNotSupported);
        this.isDirty = true;
        this.engine.IsDirty = true;
      }

      public Object SourceContext { 
        get {
          return null;
        } 
        
        set {
        } 
      }

      public String SourceText { 
        get {
          if (this.engine == null)
            throw new VsaException(VsaError.EngineClosed);
          return this.codeContext.source_string;
        } 
        
        set {
          if (this.engine == null)
            throw new VsaException(VsaError.EngineClosed);
          this.codeContext.SetSourceContext(this.codeContext.document, (value == null ? "": value));
          this.compiledClass = null;
          this.isDirty = true;
          this.engine.IsDirty = true;
        }
      }

      internal void TranslateToIL(){
        if (this.block != null && this.compiledClass == null){
          GlobalScope glob = (GlobalScope)this.engine.GetGlobalScope().GetObject();
          this.engine.Globals.ScopeStack.Push(glob);
          try{
            this.compiledClass = this.block.TranslateToILClass(this.engine.CompilerGlobals, false);
          }finally{
            this.engine.Globals.ScopeStack.Pop();
          }
        }
      }

    }

}
