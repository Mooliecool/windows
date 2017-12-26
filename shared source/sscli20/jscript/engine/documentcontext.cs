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
  using Microsoft.Vsa;
  using System.Reflection;
  using System.Reflection.Emit;
  using System.Collections;
  using System.Diagnostics.SymbolStore;

  public class DocumentContext{
    internal String documentName;
    internal ISymbolDocumentWriter documentWriter;
    internal int startLine;
    internal int startCol;
    internal int lastLineInSource;
    internal VsaItem sourceItem;
    internal VsaEngine engine; // This may be null
    internal bool debugOn;
    private CompilerGlobals _compilerGlobals;
    private SimpleHashtable reportedVariables;

    private bool checkForFirst;
    private int firstStartLine;
    private int firstStartCol;
    private int firstEndLine;
    private int firstEndCol;

    internal static readonly Guid language = new Guid("3a12d0b6-c26c-11d0-b442-00a0244a1dd2");
    internal static readonly Guid vendor = new Guid("994b45c4-e6e9-11d2-903f-00c04fa302a1");
    
    internal DocumentContext(String name, VsaEngine engine){
      this.documentName = name;
      this.documentWriter = null;
      this.startLine = 0;
      this.startCol = 0;
      this.lastLineInSource = 0;
      this.sourceItem = null;
      this.engine = engine;
      this.debugOn = null == engine ? false : engine.GenerateDebugInfo;
      this._compilerGlobals = null;
      this.reportedVariables = null;
      this.checkForFirst = false;
    }

    internal DocumentContext(VsaItem sourceItem){
      if (sourceItem.codebase != null)
        this.documentName = sourceItem.codebase;
      else{
        string rootMoniker = sourceItem.engine.RootMoniker;
        this.documentName = rootMoniker + (rootMoniker.EndsWith("/", StringComparison.Ordinal) ? "" : "/") + sourceItem.Name;
      }
      this.documentWriter = null;
      this.startLine = 0;
      this.startCol = 0;
      this.lastLineInSource = 0;
      this.sourceItem = sourceItem;
      this.engine = sourceItem.engine;
      this.debugOn = null == engine ? false : engine.GenerateDebugInfo;
      this._compilerGlobals = null;
      this.checkForFirst = false;
    }
    
    internal DocumentContext(String documentName, int startLine, int startCol, int lastLineInSource, VsaItem sourceItem){
      this.documentName = documentName;
      this.documentWriter = null;
      this.startLine = startLine;
      this.startCol = startCol;
      this.lastLineInSource = lastLineInSource;
      this.sourceItem = sourceItem;
      this.engine = sourceItem.engine;
      this.debugOn = null == engine ? false : engine.GenerateDebugInfo;
      this._compilerGlobals = null;
      this.checkForFirst = false;
    }

    internal CompilerGlobals compilerGlobals{
      get{
        if (this._compilerGlobals == null)
          this._compilerGlobals = this.engine.CompilerGlobals;
        return this._compilerGlobals;
      }
    }
    
    internal void EmitLineInfo(ILGenerator ilgen, int line, int column, int endLine, int endColumn) {
      if (debugOn) {
        if (checkForFirst && line == this.firstStartLine && column == this.firstStartCol && endLine == this.firstEndLine
            && endColumn == this.firstEndCol)
          checkForFirst = false;
        else{
          if (this.documentWriter == null) 
            this.documentWriter = this.GetSymDocument(documentName);
          ilgen.MarkSequencePoint(this.documentWriter, this.startLine + line - this.lastLineInSource, this.startCol + column + 1,
                  this.startLine - this.lastLineInSource + endLine, this.startCol + endColumn + 1);
        }
      }
    }

    internal void EmitFirstLineInfo(ILGenerator ilgen, int line, int column, int endLine, int endColumn) {
      this.EmitLineInfo(ilgen, line, column, endLine, endColumn);
      this.checkForFirst = true;
      this.firstStartLine = line;
      this.firstStartCol = column;
      this.firstEndLine = endLine;
      this.firstEndCol = endColumn;
    }


    private ISymbolDocumentWriter GetSymDocument(String documentName){
      SimpleHashtable documents = this.compilerGlobals.documents;
      Object document = documents[documentName];
      if (null == document) {
        document = _compilerGlobals.module.DefineDocument(this.documentName, DocumentContext.language, DocumentContext.vendor, Guid.Empty);
        documents[documentName] = document;
      }
      return (ISymbolDocumentWriter)document;
    }

    //---------------------------------------------------------------------------------------
    // HandleError
    //
    //  Handle an error. There are two actions that can be taken when an error occurs:
    //    - throwing an exception with no recovering action (eval case)
    //    - notify the host that an error occurred and let the host decide whether or not
    //      parsing has to continue (the host returns true when parsing has to continue)
    //---------------------------------------------------------------------------------------
    
    internal void HandleError(JScriptException error) {
      if (this.sourceItem == null) //Called from eval
        if (error.Severity == 0)
          throw error;
        else
          return;
      if (!this.sourceItem.engine.OnCompilerError(error))
        throw new EndOfFile(); // this exception terminates the parser
    }
    
    internal bool HasAlreadySeenErrorFor(String varName){
      if (this.reportedVariables == null)
        this.reportedVariables = new SimpleHashtable(8);
      else if (this.reportedVariables[varName] != null)
        return true;
      this.reportedVariables[varName] = varName;
      return false;
    }
  
  }
  
}
