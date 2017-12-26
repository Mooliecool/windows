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

    using System.Diagnostics;
    using System;
    using System.IO;
    using System.Collections;
    using System.Reflection;
    using System.CodeDom;
    using System.CodeDom.Compiler;
    using System.Globalization;
    using System.Security;
    using System.Security.Permissions;
    using System.Text;
    using System.Text.RegularExpressions;
    using Microsoft.JScript.Vsa;

    [System.ComponentModel.DesignerCategory("code")]
    [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
    public sealed class JScriptCodeProvider: CodeDomProvider {
      private JSCodeGenerator generator;

      public JScriptCodeProvider() {
        generator = new JSCodeGenerator();
      }

      public override ICodeGenerator CreateGenerator() {
        return (ICodeGenerator)generator;
      }

      public override ICodeCompiler CreateCompiler() {
        return (ICodeCompiler)generator;
      }

      public override string FileExtension {
        get {
          return "js";
        }
      }
    }

    // JScript (ECMAScript) Code Generator.
    // [stesty] - This class used to be dependent on CSCodeGenerator but that dependency was removed
    //            when the source got moved over to the VSA tree.  Since we are still dependent on
    //            CodeCompiler, \\urtdist\builds\SRC\XXXX\DNA\CompMod\System\CodeDOM\Compiler\CodeCompiler.cs
    //            should be consulted when fixing new bugs here.
    //
    [System.ComponentModel.DesignerCategory("code")]
    internal sealed class JSCodeGenerator : CodeCompiler {
      private const int MaxLineLength = 80;

      private const GeneratorSupport LanguageSupport = GeneratorSupport.ArraysOfArrays |
                                                        GeneratorSupport.AssemblyAttributes |
                                                        GeneratorSupport.EntryPointMethod |
                                                        GeneratorSupport.MultidimensionalArrays |
                                                        GeneratorSupport.StaticConstructors |
                                                        GeneratorSupport.PublicStaticMembers |
                                                        GeneratorSupport.DeclareEnums | 
                                                        GeneratorSupport.DeclareInterfaces |
                                                        GeneratorSupport.TryCatchStatements ;

      private bool forLoopHack = false;
      private bool isArgumentList = true;

      private static Hashtable keywords;

      static JSCodeGenerator() {
        // build the keywords hashtable
        keywords = new Hashtable(150);
        Object obj = new Object();
        // a
        keywords["abstract"] = obj;
        keywords["assert"] = obj;
        // b
        keywords["boolean"] = obj;
        keywords["break"] = obj;
        keywords["byte"] = obj;
        // c
        keywords["case"] = obj;
        keywords["catch"] = obj;
        keywords["char"] = obj;
        keywords["class"] = obj;
        keywords["const"] = obj;
        keywords["continue"] = obj;
        // d
        keywords["debugger"] = obj;
        keywords["decimal"] = obj;
        keywords["default"] = obj;
        keywords["delete"] = obj;
        keywords["do"] = obj;
        keywords["double"] = obj;
        // e
        keywords["else"] = obj;
        keywords["ensure"] = obj;
        keywords["enum"] = obj;
        keywords["event"] = obj;
        keywords["export"] = obj;
        keywords["extends"] = obj;
        // f
        keywords["false"] = obj;
        keywords["final"] = obj;
        keywords["finally"] = obj;
        keywords["float"] = obj;
        keywords["for"] = obj;
        keywords["function"] = obj;
        // g
        keywords["get"] = obj;
        keywords["goto"] = obj;
        // i
        keywords["if"] = obj;
        keywords["implements"] = obj;
        keywords["import"] = obj;
        keywords["in"] = obj;
        keywords["instanceof"] = obj;
        keywords["int"] = obj;
        keywords["invariant"] = obj;
        keywords["interface"] = obj;
        keywords["internal"] = obj;
        // l
        keywords["long"] = obj;
        // n
        keywords["namespace"] = obj;
        keywords["native"] = obj;
        keywords["new"] = obj;
        keywords["null"] = obj;
        // p
        keywords["package"] = obj;
        keywords["private"] = obj;
        keywords["protected"] = obj;
        keywords["public"] = obj;
        // r
        keywords["require"] = obj;
        keywords["return"] = obj;
        // s
        keywords["sbyte"] = obj;
        keywords["scope"] = obj;
        keywords["set"] = obj;
        keywords["short"] = obj;
        keywords["static"] = obj;
        keywords["super"] = obj;
        keywords["switch"] = obj;
        keywords["synchronized"] = obj;
        // t
        keywords["this"] = obj;
        keywords["throw"] = obj;
        keywords["throws"] = obj;
        keywords["transient"] = obj;
        keywords["true"] = obj;
        keywords["try"] = obj;
        keywords["typeof"] = obj;
        // u
        keywords["use"] = obj;
        keywords["uint"] = obj;
        keywords["ulong"] = obj;
        keywords["ushort"] = obj;
        // v
        keywords["var"] = obj;
        keywords["void"] = obj;
        keywords["volatile"] = obj;
        // w
        keywords["while"] = obj;
        keywords["with"] = obj;
      }

      private string mainClassName = null;
      private string mainMethodName = null;
      private static Regex outputReg = new Regex(@"(([^(]+)(\(([0-9]+),([0-9]+)\))[ \t]*:[ \t]+)?(fatal )?(error|warning)[ \t]+([A-Z]+[0-9]+)[ \t]*:[ \t]*(.*)");

      protected override string CmdArgsFromParameters(CompilerParameters options) {
        StringBuilder sb = new StringBuilder(128);
        // Do not output the target type here because it can only appear once on the command line and we
        // would like the user to be able to specify a target type in CompilerParameters.CompilerOptions.

        // if '/' separates dir, use '-' as switch
        string cmdLineSwitch = Path.DirectorySeparatorChar == '/' ? "-" : "/" ;

        sb.Append(cmdLineSwitch + "utf8output ");

        object obj = new object();
        Hashtable refs = new Hashtable(20);
        foreach (string s in options.ReferencedAssemblies) {
          // we do not want to have more than one reference to any given assembly
          if (refs[s] == null) {
            refs[s] = obj;
            sb.Append(cmdLineSwitch + "r:");
            sb.Append("\"");
            sb.Append(s);
            sb.Append("\" ");
          }
        }
        sb.Append(cmdLineSwitch + "out:");
        sb.Append("\"");
        sb.Append(options.OutputAssembly);
        sb.Append("\" ");
        if (options.IncludeDebugInformation) {
          sb.Append(cmdLineSwitch + "d:DEBUG ");
          sb.Append(cmdLineSwitch + "debug+ ");
        } else {
          sb.Append(cmdLineSwitch + "debug- ");
        }
        if (options.TreatWarningsAsErrors) {
          sb.Append(cmdLineSwitch + "warnaserror ");
        }
        if (options.WarningLevel >= 0) {
          sb.Append(cmdLineSwitch + "w:" + options.WarningLevel.ToString(CultureInfo.InvariantCulture) + " ");
        }
        // We append options.CompilerOptions in JSInProcCompiler.ParseCompilerOptions

        return sb.ToString();
      }

      // Gets the name of the compiler executable.
      protected override string CompilerName { get { return "jsc.exe"; } }

      protected override string CreateEscapedIdentifier(string name) {
        if (IsKeyword(name)) {
          return "\\" + name;
        }
        return name;
      }

      protected override string CreateValidIdentifier(string name) {
        if (IsKeyword(name)) {
          return "$" + name;
        }
        return name;
      }

      // Gets the file extension to use for source files.
      protected override string FileExtension { get { return ".js"; } }

      protected override CompilerResults FromFileBatch(CompilerParameters options, string[] fileNames){
        string outputFile = options.TempFiles.AddExtension("out");
        CompilerResults results = new CompilerResults(options.TempFiles);

        if (options.OutputAssembly == null || options.OutputAssembly.Length == 0) {
          options.OutputAssembly = results.TempFiles.AddExtension("dll", !options.GenerateInMemory);
        }

        // For debug mode, we construct a command line for debugging with JSC.  The command line is
        // only partially formed at this stage; the rest of it will be added and written to a response
        // file by JSInProcCompiler.

        string partialCmdLine = null;
        if (options.IncludeDebugInformation){
          results.TempFiles.AddExtension("ildb");
          partialCmdLine = this.CmdArgsFromParameters(options);
        }

        results.NativeCompilerReturnValue = 0;
        try {
          JSInProcCompiler jsc = new JSInProcCompiler();
          results.NativeCompilerReturnValue = jsc.Compile(options, partialCmdLine, fileNames, outputFile);
        } catch {
          // internal compiler error
          results.NativeCompilerReturnValue = 10;
        }

        // parse the output looking for errors and warnings
        try {
          StreamReader compilerOutput = new StreamReader(outputFile);
          try {
            string line = compilerOutput.ReadLine();
            while (line != null) {
              results.Output.Add(line);
              ProcessCompilerOutputLine(results, line);
              line = compilerOutput.ReadLine();
            }
          } finally {
            compilerOutput.Close();
          }
        }catch(Exception e){
          // could not open output file -- provide some other error information
          results.Output.Add(JScriptException.Localize("No error output", CultureInfo.CurrentUICulture));
          results.Output.Add(e.ToString());
        }catch{
          // could not open output file -- provide some other error information
          results.Output.Add(JScriptException.Localize("No error output", CultureInfo.CurrentUICulture));
        }
        if ((results.NativeCompilerReturnValue == 0) && options.GenerateInMemory) {
          FileStream fs = new FileStream(options.OutputAssembly, FileMode.Open, FileAccess.Read, FileShare.Read);
          try {
            int fileLen = (int)fs.Length;
            byte[] b = new byte[fileLen];
            fs.Read(b, 0, fileLen);
            results.CompiledAssembly = Assembly.Load(b, null, options.Evidence);
          }finally {
            fs.Close();
          }
        }else{
          results.PathToAssembly = Path.GetFullPath(options.OutputAssembly);
        }
        results.Evidence = options.Evidence;
        return results;
      }

      protected override void GenerateArgumentReferenceExpression(CodeArgumentReferenceExpression e) {
        OutputIdentifier(e.ParameterName);
      }

      protected override void GenerateArrayCreateExpression(CodeArrayCreateExpression e) {
        CodeExpressionCollection init = e.Initializers;
        if (init.Count > 0) {
          Output.Write("[");
          Indent++;
          OutputExpressionList(init);
          Indent--;
          Output.Write("]");
        } else {
          Output.Write("new ");
          Output.Write(GetBaseTypeOutput(e.CreateType.BaseType));
          Output.Write("[");
          if (e.SizeExpression != null) {
            GenerateExpression(e.SizeExpression);
          } else {
            Output.Write(e.Size.ToString(CultureInfo.InvariantCulture));
          }
          Output.Write("]");
        }
      }

      protected override void GenerateArrayIndexerExpression(CodeArrayIndexerExpression e) {
        GenerateExpression(e.TargetObject);
        Output.Write("[");
        bool first = true;
        foreach(CodeExpression exp in e.Indices) {
          if (first) {
            first = false;
          } else {
            Output.Write(", ");
          }
          GenerateExpression(exp);
        }
        Output.Write("]");
      }

      private void GenerateAssemblyAttributes(CodeAttributeDeclarationCollection attributes) {
        if (attributes.Count == 0) return;
        IEnumerator en = attributes.GetEnumerator();
        while (en.MoveNext()) {
          Output.Write("[");
          Output.Write("assembly: ");
          CodeAttributeDeclaration current = (CodeAttributeDeclaration)en.Current;
          Output.Write(GetBaseTypeOutput(current.Name));
          Output.Write("(");
          bool firstArg = true;
          foreach (CodeAttributeArgument arg in current.Arguments) {
            if (firstArg) {
              firstArg = false;
            } else {
              Output.Write(", ");
            }
            OutputAttributeArgument(arg);
          }
          Output.Write(")");
          Output.Write("]");
          Output.WriteLine();
        }
      }

      // Generates code for the specified CodeDom based assignment statement representation.
      // 'e' indicates the statement to generate code for.
      protected override void GenerateAssignStatement(CodeAssignStatement e) {
        GenerateExpression(e.Left);
        Output.Write(" = ");
        GenerateExpression(e.Right);
        if (!forLoopHack) {
          Output.WriteLine(";");
        }
      }

      protected override void GenerateAttachEventStatement(CodeAttachEventStatement e) {
        GenerateExpression(e.Event.TargetObject);
        Output.Write(".add_");
        Output.Write(e.Event.EventName);
        Output.Write("(");
        GenerateExpression(e.Listener);
        Output.WriteLine(");");
      }

      // Generates code for the specified CodeDom based attribute block end representation.
      // 'attributes' indicates the attribute block end to generate code for.
      protected override void GenerateAttributeDeclarationsEnd(CodeAttributeDeclarationCollection attributes) {
      }

      // Generates code for the specified CodeDom based attribute block start representation.
      // 'attributes' indicates the attribute block to generate code for.
      protected override void GenerateAttributeDeclarationsStart(CodeAttributeDeclarationCollection attributes) {
      }

      // Generates code for the specified CodeDom based base reference expression
      // representation.
      // 'e' indicates the expression to generate code for.
      protected override void GenerateBaseReferenceExpression(CodeBaseReferenceExpression e) {
          Output.Write("super");
      }

      // Used to output the T part of new T[...]
      private string GetBaseTypeOutput(string baseType) {
        if (baseType.Length == 0) {
          return "void";
        } else if (string.Compare(baseType, "System.Byte", StringComparison.Ordinal) == 0) {
          return "byte";
        } else if (string.Compare(baseType, "System.Int16", StringComparison.Ordinal) == 0) {
          return "short";
        } else if (string.Compare(baseType, "System.Int32", StringComparison.Ordinal) == 0) {
          return "int";
        } else if (string.Compare(baseType, "System.Int64", StringComparison.Ordinal) == 0) {
          return "long";
        } else if (string.Compare(baseType, "System.SByte", StringComparison.Ordinal) == 0) {
          return "sbyte";
        } else if (string.Compare(baseType, "System.UInt16", StringComparison.Ordinal) == 0) {
          return "ushort";
        } else if (string.Compare(baseType, "System.UInt32", StringComparison.Ordinal) == 0) {
          return "uint";
        } else if (string.Compare(baseType, "System.UInt64", StringComparison.Ordinal) == 0) {
          return "ulong";
        } else if (string.Compare(baseType, "System.Decimal", StringComparison.Ordinal) == 0) {
          return "decimal";
        } else if (string.Compare(baseType, "System.Single", StringComparison.Ordinal) == 0) {
          return "float";
        } else if (string.Compare(baseType, "System.Double", StringComparison.Ordinal) == 0) {
          return "double";
        } else if (string.Compare(baseType, "System.Boolean", StringComparison.Ordinal) == 0) {
          return "boolean";
        } else if (string.Compare(baseType, "System.Char", StringComparison.Ordinal) == 0) {
          return "char";
        } else {
          // replace + with . for nested classes.
          baseType = baseType.Replace('+', '.');
          return CreateEscapedIdentifier(baseType);
        }
      }

      // Generates code for the specified CodeDom based cast expression representation.
      // 'e' indicates the expression to generate code for.
      protected override void GenerateCastExpression(CodeCastExpression e) {
        OutputType(e.TargetType);
        Output.Write("(");
        GenerateExpression(e.Expression);
        Output.Write(")");
      }

      protected override void GenerateComment(CodeComment e) {
        string commentText = e.Text;
        StringBuilder b = new StringBuilder(commentText.Length * 2);
        string commentPrefix = e.DocComment ? "///" : "//";
        // escape the comment text
        b.Append(commentPrefix);
        for (int i=0; i < commentText.Length; i++){
          switch (commentText[i]){
            // suppress '@' to prevent compiler directives in comments
            case '@':
              break;
            case '\r':
              if (i < commentText.Length - 1 && commentText[i+1] == '\n') {
                b.Append("\r\n" + commentPrefix);
                i++;
              }
              else {
                b.Append("\r" + commentPrefix);
              }
              break;
            case '\n':
              b.Append("\n" + commentPrefix);
              break;
            case '\u2028':
              b.Append("\u2028" + commentPrefix);
              break;
            case '\u2029':
              b.Append("\u2029" + commentPrefix);
              break;
            default:
              b.Append(commentText[i]);
              break;
          }
        }
        Output.WriteLine(b.ToString());
      }

      protected override void GenerateCompileUnitStart(CodeCompileUnit e) {
        Output.WriteLine("//------------------------------------------------------------------------------");
        Output.WriteLine("/// <autogenerated>");
        Output.WriteLine("///     This code was generated by a tool.");
        Output.WriteLine("///     Runtime Version: " + System.Environment.Version.ToString());
        Output.WriteLine("///");
        Output.WriteLine("///     Changes to this file may cause incorrect behavior and will be lost if ");
        Output.WriteLine("///     the code is regenerated.");
        Output.WriteLine("/// </autogenerated>");
        Output.WriteLine("//------------------------------------------------------------------------------");
        Output.WriteLine("");

        // Assembly attributes must be placed at the top level.
        if (e.AssemblyCustomAttributes.Count > 0) {
          GenerateAssemblyAttributes(e.AssemblyCustomAttributes);
          Output.WriteLine("");
        }
      }

      // Generates code for the specified CodeDom based if statement representation.
      // 'e' indicates the statement to generate code for.
      protected override void GenerateConditionStatement(CodeConditionStatement e) {
        Output.Write("if (");
        // Indent within the condition, in case they need to go on multiple lines.
        Indent += 2;
        GenerateExpression(e.Condition);
        Indent -= 2;
        Output.Write(")");
        OutputStartingBrace();            
        Indent++;
        GenerateStatements(e.TrueStatements);
        Indent--;

        if (e.FalseStatements.Count > 0) {
          Output.Write("}");
          if (Options.ElseOnClosing) {
            Output.Write(" ");
          } else {
            Output.WriteLine("");
          }
          Output.Write("else");
          OutputStartingBrace();
          Indent++;
          GenerateStatements(e.FalseStatements);
          Indent--;
        }
        Output.WriteLine("}");
      }


      // Generates code for the specified CodeDom based constructor representation.
      // 'e' indicates the constructor to generate code for.
      protected override void GenerateConstructor(CodeConstructor e, CodeTypeDeclaration c) {
        if (!(IsCurrentClass || IsCurrentStruct)) return;

        OutputMemberAccessModifier(e.Attributes);
        if (e.CustomAttributes.Count > 0) {
          OutputAttributeDeclarations(e.CustomAttributes);
        }
        Output.Write("function ");
        OutputIdentifier(CurrentTypeName);
        Output.Write("(");
        OutputParameters(e.Parameters);
        Output.Write(")");

        CodeExpressionCollection baseArgs = e.BaseConstructorArgs;
        CodeExpressionCollection thisArgs = e.ChainedConstructorArgs;

        OutputStartingBrace();
        Indent++;
        if (baseArgs.Count > 0) {
          Output.Write("super(");
          OutputExpressionList(baseArgs);
          Output.WriteLine(");");
        }

        if (thisArgs.Count > 0) {
          Output.Write("this(");
          OutputExpressionList(thisArgs);
          Output.WriteLine(");");
        }

        GenerateStatements(e.Statements);
        Output.WriteLine();
        Indent--;
        Output.WriteLine("}");
      }

      // Generates code for the specified CodeDom based delegate creation expression representation.
      // 'e' indicates the delegate creation expression to generate code for.
      protected override void GenerateDelegateCreateExpression(CodeDelegateCreateExpression e) {
        bool castToDelegate = (e.DelegateType != null);
        if (castToDelegate){
          OutputType(e.DelegateType);
          Output.Write("(");
        }
        GenerateExpression(e.TargetObject);
        Output.Write(".");
        OutputIdentifier(e.MethodName);
        if (castToDelegate)
          Output.Write(")");
      }

      // Generates code for the specified CodeDom based delegate invoke expression representation.
      // 'e' indicates the expression to generate code for.
      protected override void GenerateDelegateInvokeExpression(CodeDelegateInvokeExpression e) {
        if (e.TargetObject != null) {
          GenerateExpression(e.TargetObject);
        }
        Output.Write("(");
        OutputExpressionList(e.Parameters);
        Output.Write(")");
      }

      protected override void GenerateEntryPointMethod(CodeEntryPointMethod e, CodeTypeDeclaration c) {
        Output.Write("public static ");
        if (e.CustomAttributes.Count > 0) {
          OutputAttributeDeclarations(e.CustomAttributes);
        }
        Output.Write("function Main()");
        OutputStartingBrace();
        Indent++;
        GenerateStatements(e.Statements);
        Indent--;
        Output.WriteLine("}");
        mainClassName = CurrentTypeName;
        mainMethodName = "Main";
      }

      protected override void GenerateEvent(CodeMemberEvent e, CodeTypeDeclaration c) {
        throw new Exception(JScriptException.Localize("No event declarations", CultureInfo.CurrentUICulture));
      }

      protected override void GenerateEventReferenceExpression(CodeEventReferenceExpression e) {
        // JScript does not allow ob.foo where foo is an event name.  (This should never get
        // called, but we'll put an exception here just in case.)
        throw new Exception(JScriptException.Localize("No event references", CultureInfo.CurrentUICulture));
      }

      // Generates code for the specified CodeDom based method invoke statement representation.
      // 'e' indicates the statement to generate code for.
      protected override void GenerateExpressionStatement(CodeExpressionStatement e) {
        GenerateExpression(e.Expression);
        if (!forLoopHack) {
          Output.WriteLine(";");
        }
      }


      // Generates code for the specified CodeDom based member field representation.
      // 'e' indicates the field to generate code for.
      protected override void GenerateField(CodeMemberField e) {
        if (IsCurrentDelegate || IsCurrentInterface)
          throw new Exception(JScriptException.Localize("Only methods on interfaces", CultureInfo.CurrentUICulture));

        if (IsCurrentEnum) {
          OutputIdentifier(e.Name);
          if (e.InitExpression != null) {
            Output.Write(" = ");
            GenerateExpression(e.InitExpression);
          }
          Output.WriteLine(",");
        } else {
          OutputMemberAccessModifier(e.Attributes);
          if((e.Attributes & MemberAttributes.ScopeMask) == MemberAttributes.Static)
            Output.Write("static ");
          if (e.CustomAttributes.Count > 0) {
            OutputAttributeDeclarations(e.CustomAttributes);
            Output.WriteLine("");
          }
          if ((e.Attributes & MemberAttributes.Const) == MemberAttributes.Const) {
            if ((e.Attributes & MemberAttributes.ScopeMask) != MemberAttributes.Static)
              Output.Write("static ");
            Output.Write("const ");
          } else
            Output.Write("var ");
          OutputTypeNamePair(e.Type, e.Name);
          if (e.InitExpression != null) {
            Output.Write(" = ");
            GenerateExpression(e.InitExpression);
          }
          Output.WriteLine(";");
        }
      }

      // Generates code for the specified CodeDom based field reference expression representation.
      // 'e' indicates the expression to generate code for.
      protected override void GenerateFieldReferenceExpression(CodeFieldReferenceExpression e) {
        if (e.TargetObject != null) {
          GenerateExpression(e.TargetObject);
          Output.Write(".");
        }
        OutputIdentifier(e.FieldName);
      }

      protected override void GenerateGotoStatement(CodeGotoStatement e) {
        throw new Exception(JScriptException.Localize("No goto statements", CultureInfo.CurrentUICulture));
      }

      // Generates code for the specified CodeDom based indexer expression representation.
      // 'e' indicates the expression to generate code for.
      protected override void GenerateIndexerExpression(CodeIndexerExpression e) {
        GenerateExpression(e.TargetObject);
        Output.Write("[");
        bool first = true;
        foreach(CodeExpression exp in e.Indices) {            
          if (first) {
            first = false;
          } else {
            Output.Write(", ");
          }
          GenerateExpression(exp);
        }
        Output.Write("]");
      }

      // Generates code for the specified CodeDom based for loop statement representation.
      // 'e' indicates the statement to generate code for.
      protected override void GenerateIterationStatement(CodeIterationStatement e) {
        forLoopHack = true;
        Output.Write("for (");
        GenerateStatement(e.InitStatement);
        Output.Write("; ");
        GenerateExpression(e.TestExpression);
        Output.Write("; ");
        GenerateStatement(e.IncrementStatement);
        Output.Write(")");
        OutputStartingBrace();
        forLoopHack = false;
        Indent++;
        GenerateStatements(e.Statements);
        Indent--;
        Output.WriteLine("}");
      }

      protected override void GenerateLabeledStatement(CodeLabeledStatement e) {
        throw new Exception(JScriptException.Localize("No goto statements", CultureInfo.CurrentUICulture));
      }

      // Generates code for the specified CodeDom based line pragma start representation.
      // 'e' indicates the start position to generate code for.
      protected override void GenerateLinePragmaStart(CodeLinePragma e) {
        Output.WriteLine("");
        Output.WriteLine("//@cc_on");
        Output.Write("//@set @position(file=\"");

        // we need to escape "\" to become "\\", however regex requires the
        // _pattern_ to be escaped, therefore the replacement is "\\" --> "\\"...
        // since C# requires \ to be escaped we get the apparently meaningless
        // expression below...
        //
        Output.Write(Regex.Replace(e.FileName, "\\\\", "\\\\"));

        Output.Write("\";line=");
        Output.Write(e.LineNumber.ToString(CultureInfo.InvariantCulture));
        Output.WriteLine(")");
      }

      // Generates code for the specified CodeDom based line pragma end representation.
      // 'e' indicates the end position to generate code for.
      protected override void GenerateLinePragmaEnd(CodeLinePragma e) {
        Output.WriteLine("");
        Output.WriteLine("//@set @position(end)");
      }

      // Generates code for the specified CodeDom based member method representation.
      // 'e' indicates the method to generate code for.
      protected override void GenerateMethod(CodeMemberMethod e, CodeTypeDeclaration c) {
        if (!IsCurrentInterface) {
          if (e.PrivateImplementationType == null) {
            OutputMemberAccessModifier(e.Attributes);
            OutputMemberVTableModifier(e.Attributes);
            OutputMemberScopeModifier(e.Attributes);
          }
        } else {
          OutputMemberVTableModifier(e.Attributes);
        }
        if (e.CustomAttributes.Count > 0) {
          OutputAttributeDeclarations(e.CustomAttributes);
        }
        Output.Write("function ");
        if (e.PrivateImplementationType != null && !IsCurrentInterface) {
          Output.Write(e.PrivateImplementationType.BaseType);
          Output.Write(".");
        }
        OutputIdentifier(e.Name);
        Output.Write("(");
        // disable use of '&' in front of ref and out parameters (JScript doesn't allow them to be declared, only used)
        this.isArgumentList = false;
        try{
          OutputParameters(e.Parameters);
        }finally{
          this.isArgumentList = true;
        }
        Output.Write(")");
        if (e.ReturnType.BaseType.Length > 0 && string.Compare(e.ReturnType.BaseType, typeof(void).FullName, StringComparison.Ordinal) != 0) {
          Output.Write(" : ");
          OutputType(e.ReturnType);
        }
        if (!IsCurrentInterface 
          && (e.Attributes & MemberAttributes.ScopeMask) != MemberAttributes.Abstract) {
          OutputStartingBrace();
          Indent++;
          GenerateStatements(e.Statements);
          Indent--;
          Output.WriteLine("}");
        } else {
          Output.WriteLine(";");
        }
      }

      // Generates code for the specified CodeDom based method invoke expression representation.
      // 'e' indicates the expression to generate code for.
      protected override void GenerateMethodInvokeExpression(CodeMethodInvokeExpression e) {
        GenerateMethodReferenceExpression(e.Method);
        Output.Write("(");
        OutputExpressionList(e.Parameters);
        Output.Write(")");
      }

      protected override void GenerateMethodReferenceExpression(CodeMethodReferenceExpression e) {
        if (e.TargetObject != null) {
          if (e.TargetObject is CodeBinaryOperatorExpression) {
            Output.Write("(");
            GenerateExpression(e.TargetObject);
            Output.Write(")");
          } else {
            GenerateExpression(e.TargetObject);
          }
          Output.Write(".");
        }
        OutputIdentifier(e.MethodName);
      }

      // Generates code for the specified CodeDom based method return statement representation.
      // 'e' indicates the statement to generate code for.
      protected override void GenerateMethodReturnStatement(CodeMethodReturnStatement e) {
        Output.Write("return");
        if (e.Expression != null) {
          Output.Write(" ");
          GenerateExpression(e.Expression);
        }
        Output.WriteLine(";");
      }

      // Generates code for the specified CodeDom based namespace representation.
      // 'e' indicates the namespace to generate code for.
      protected override void GenerateNamespace(CodeNamespace e) {
        Output.WriteLine("//@cc_on");
        Output.WriteLine("//@set @debug(off)");
        Output.WriteLine("");

        GenerateNamespaceImports(e);
        Output.WriteLine("");

        GenerateCommentStatements(e.Comments);
        GenerateNamespaceStart(e);
        GenerateTypes(e);
        GenerateNamespaceEnd(e);
      }

      // Generates code for the specified CodeDom based namespace end representation.
      // 'e' indicates the namespace end to generate code for.
      protected override void GenerateNamespaceEnd(CodeNamespace e) {
        if (e.Name != null && e.Name.Length > 0) {
          Indent--;
          Output.WriteLine("}");
        }

        if (mainClassName != null) {
          if (e.Name != null){
            OutputIdentifier(e.Name);
            Output.Write(".");
          }
          OutputIdentifier(mainClassName);
          Output.Write(".");
          OutputIdentifier(mainMethodName);
          Output.WriteLine("();");
          mainClassName = null;
        }
      }

      // Generates code for the specified CodeDom based namespace import representation.
      // 'e' indicates the namespace import to generate code for.
      protected override void GenerateNamespaceImport(CodeNamespaceImport e) {
        Output.Write("import ");
        OutputIdentifier(e.Namespace);
        Output.WriteLine(";");
      }

      // Generates code for the specified CodeDom based namespace start representation.
      // 'e' indicates the namespace start to generate code for.
      protected override void GenerateNamespaceStart(CodeNamespace e) {
        if (e.Name != null && e.Name.Length > 0) {
          Output.Write("package ");
          OutputIdentifier(e.Name);
          OutputStartingBrace();
          Indent++;
        }
      }

      // Generates code for the specified CodeDom based object creation expression representation.
      // 'e' indicates the expression to generate code for.
      protected override void GenerateObjectCreateExpression(CodeObjectCreateExpression e) {
        Output.Write("new ");
        OutputType(e.CreateType);
        Output.Write("(");
        OutputExpressionList(e.Parameters);
        Output.Write(")");
      }

      protected override void GenerateParameterDeclarationExpression(CodeParameterDeclarationExpression e) {
        if (e.CustomAttributes.Count > 0) {
          CodeAttributeDeclaration attr = e.CustomAttributes[0];
          if (attr.Name == "ParamArrayAttribute")
            Output.Write("... ");
          else{
            throw new Exception(JScriptException.Localize("No parameter attributes", CultureInfo.CurrentUICulture));
//              Output.Write("[");
//              OutputAttributeDeclarations(current.CustomAttributes);
//              Output.Write("]");
          }
        }
        OutputDirection(e.Direction);
        OutputTypeNamePair(e.Type, e.Name);
      }

      // Generates code for the specified CodeDom based member property representation.
      // 'e' indicates the member property to generate code for.
      protected override void GenerateProperty(CodeMemberProperty e, CodeTypeDeclaration c) {
        if (!(IsCurrentClass || IsCurrentStruct || IsCurrentInterface)) return;

        if (e.HasGet) {
          if (!IsCurrentInterface) {
            if (e.PrivateImplementationType == null) {
              OutputMemberAccessModifier(e.Attributes);
              OutputMemberVTableModifier(e.Attributes);
              OutputMemberScopeModifier(e.Attributes);
            }
          } else {
            OutputMemberVTableModifier(e.Attributes);
          }
          if (e.CustomAttributes.Count > 0) {
            if (IsCurrentInterface)
              Output.Write("public ");
            OutputAttributeDeclarations(e.CustomAttributes);
            Output.WriteLine("");
          }
          Output.Write("function get ");
          if (e.PrivateImplementationType != null && !IsCurrentInterface) {
            Output.Write(e.PrivateImplementationType.BaseType);
            Output.Write(".");
          }
          OutputIdentifier(e.Name);
          if (e.Parameters.Count > 0)
            throw new Exception(JScriptException.Localize("No indexer declarations", CultureInfo.CurrentUICulture));
          Output.Write("() : ");
          OutputType(e.Type);
          if (IsCurrentInterface || (e.Attributes & MemberAttributes.ScopeMask) == MemberAttributes.Abstract) {
            Output.WriteLine(";");
          } else {
            OutputStartingBrace();
            Indent++;
            GenerateStatements(e.GetStatements);
            Indent--;
            Output.WriteLine("}");
          }
        }
        if (e.HasSet) {
          if (!IsCurrentInterface) {
            if (e.PrivateImplementationType == null) {
              OutputMemberAccessModifier(e.Attributes);
              OutputMemberVTableModifier(e.Attributes);
              OutputMemberScopeModifier(e.Attributes);
            }
          } else {
            OutputMemberVTableModifier(e.Attributes);
          }
          if (e.CustomAttributes.Count > 0 && !e.HasGet) {
            if (IsCurrentInterface)
              Output.Write("public ");
            OutputAttributeDeclarations(e.CustomAttributes);
            Output.WriteLine("");
          }
          Output.Write("function set ");
          if (e.PrivateImplementationType != null && !IsCurrentInterface) {
            Output.Write(e.PrivateImplementationType.BaseType);
            Output.Write(".");
          }
          OutputIdentifier(e.Name);
          Output.Write("(");
          OutputTypeNamePair(e.Type, "value");
          if (e.Parameters.Count > 0)
            throw new Exception(JScriptException.Localize("No indexer declarations", CultureInfo.CurrentUICulture));
          Output.Write(")");
          if (IsCurrentInterface || (e.Attributes & MemberAttributes.ScopeMask) == MemberAttributes.Abstract) {
            Output.WriteLine(";");
          } else {
            OutputStartingBrace();
            Indent++;
            GenerateStatements(e.SetStatements);
            Indent--;
            Output.WriteLine("}");
          }
        }
      }

      // Generates code for the specified CodeDom based property reference expression representation.
      // 'e' indicates the expression to generate code for.
      protected override void GeneratePropertyReferenceExpression(CodePropertyReferenceExpression e) {
        if (e.TargetObject != null) {
          GenerateExpression(e.TargetObject);
          Output.Write(".");
        }
        OutputIdentifier(e.PropertyName);
      }

      protected override void GeneratePropertySetValueReferenceExpression(CodePropertySetValueReferenceExpression e) {
        Output.Write("value");
      }

      protected override void GenerateRemoveEventStatement(CodeRemoveEventStatement e) {
          GenerateExpression(e.Event.TargetObject);
          Output.Write(".remove_");
          Output.Write(e.Event.EventName);
          Output.Write("(");
          GenerateExpression(e.Listener);
          Output.WriteLine(");");
      }

      protected override void GenerateSingleFloatValue(Single s) {
        Output.Write("float(");
        Output.Write(s.ToString(CultureInfo.InvariantCulture));
        Output.Write(")");
      }

      // Generates code for the specified CodeDom based snippet expression representation.
      // 'e' indicates the expression to generate code for.
      protected override void GenerateSnippetExpression(CodeSnippetExpression e) {
        Output.Write(e.Value);
      }

      // Generates code for the specified CodeDom based snippet class member representation.
      // 'e' indicates the expression to generate code for.
      protected override void GenerateSnippetMember(CodeSnippetTypeMember e) {
        Output.Write(e.Text);
      }

      protected override void GenerateSnippetStatement(CodeSnippetStatement e) {
        Output.WriteLine(e.Value);
      }

      // Generates code for the specified CodeDom based this reference expression representation.
      // 'e' indicates the expression to generate code for.
      protected override void GenerateThisReferenceExpression(CodeThisReferenceExpression e) {
        Output.Write("this");
      }

      // Generates code for the specified CodeDom based throw exception statement representation.
      // 'e' indicates the statement to generate code for.
      protected override void GenerateThrowExceptionStatement(CodeThrowExceptionStatement e) {
        Output.Write("throw");
        if (e.ToThrow != null) {
          Output.Write(" ");
          GenerateExpression(e.ToThrow);
        }
        Output.WriteLine(";");
      }

      protected override void GenerateTryCatchFinallyStatement(CodeTryCatchFinallyStatement e) {
        Output.Write("try");
        OutputStartingBrace();
        Indent++;
        GenerateStatements(e.TryStatements);
        Indent--;
        CodeCatchClauseCollection catches = e.CatchClauses;
        if (catches.Count > 0) {
          IEnumerator en = catches.GetEnumerator();
          while (en.MoveNext()) {
            Output.Write("}");
            if (Options.ElseOnClosing) {
              Output.Write(" ");
            } else {
              Output.WriteLine("");
            }
            CodeCatchClause current = (CodeCatchClause)en.Current;
            Output.Write("catch (");
            OutputIdentifier(current.LocalName);
            Output.Write(" : ");
            OutputType(current.CatchExceptionType);
            Output.Write(")");
            OutputStartingBrace();
            Indent++;
            GenerateStatements(current.Statements);
            Indent--;
          }
        }

        CodeStatementCollection finallyStatements = e.FinallyStatements;
        if (finallyStatements.Count > 0) {
          Output.Write("}");
          if (Options.ElseOnClosing) {
            Output.Write(" ");
          } else {
            Output.WriteLine("");
          }
          Output.Write("finally");
          OutputStartingBrace();
          Indent++;
          GenerateStatements(finallyStatements);
          Indent--;
        }
        Output.WriteLine("}");
      }

      // Generates code for the specified CodeDom based class constructor representation.
      // 'e' indicates the constructor to generate code for.
      protected override void GenerateTypeConstructor(CodeTypeConstructor e) {
        if (!(IsCurrentClass || IsCurrentStruct)) return;

        Output.Write("static ");
        OutputIdentifier(CurrentTypeName);
        OutputStartingBrace();
        Indent++;
        GenerateStatements(e.Statements);
        Indent--;
        Output.WriteLine("}");
      }

      // Generates code for the specified CodeDom based class end representation.
      // 'e' indicates the end of the class.
      protected override void GenerateTypeEnd(CodeTypeDeclaration e) {
        if (!IsCurrentDelegate) {
          Indent--;
          Output.WriteLine("}");
        }
      }

      // Generates code for the specified CodeDom based type of expression
      // representation.
      protected override void GenerateTypeOfExpression(CodeTypeOfExpression e) {
        OutputType(e.Type);
      }

      protected override string GetTypeOutput(CodeTypeReference typeRef) {
        string s;
        if (typeRef.ArrayElementType != null) {
          // Recurse up
          s = GetTypeOutput(typeRef.ArrayElementType);
        } else {
          s = GetBaseTypeOutput(typeRef.BaseType);
        }
        // Now spit out the array postfix
        if (typeRef.ArrayRank > 0) {
          char [] results = new char [typeRef.ArrayRank + 1];
          results[0] = '[';
          results[typeRef.ArrayRank] = ']';
          for (int i = 1; i < typeRef.ArrayRank; i++) {
            results[i] = ',';
          }
          s += new string(results);
        }               
        return s;
      }

      // Generates code for the specified CodeDom based class start representation.
      // 'e' indicates the start of the class.
      protected override void GenerateTypeStart(CodeTypeDeclaration e) {
        if (IsCurrentDelegate)
          throw new Exception(JScriptException.Localize("No delegate declarations", CultureInfo.CurrentUICulture));

        OutputTypeVisibility(e.TypeAttributes);
        if (e.CustomAttributes.Count > 0) {
          OutputAttributeDeclarations(e.CustomAttributes);
          Output.WriteLine("");
        }
        OutputTypeAttributes(e.TypeAttributes, IsCurrentStruct, IsCurrentEnum);
        OutputIdentifier(e.Name);

        if (IsCurrentEnum){
          if (e.BaseTypes.Count > 1)
            throw new Exception(JScriptException.Localize("Too many base types", CultureInfo.CurrentUICulture));
          if (e.BaseTypes.Count == 1){
            Output.Write(" : ");
            OutputType(e.BaseTypes[0]);
          }
        }else {
          bool first = true;
          bool second = false;
          foreach (CodeTypeReference typeRef in e.BaseTypes) {
            if (first) {
              Output.Write(" extends ");
              first = false;
              second = true;
            } else if (second) {
              Output.Write(" implements ");
              second = false;
            } else {
              Output.Write(", ");
            }                 
            OutputType(typeRef);
          }
        }
        OutputStartingBrace();
        Indent++;
      }

      // Generates code for the specified CodeDom based variable declaration statement representation.
      // 'e' indicates the statement to generate code for.
      protected override void GenerateVariableDeclarationStatement(CodeVariableDeclarationStatement e) {
        Output.Write("var ");
        OutputTypeNamePair(e.Type, e.Name);
        if (e.InitExpression != null) {
          Output.Write(" = ");
          GenerateExpression(e.InitExpression);
        }
        Output.WriteLine(";");
      }

      protected override void GenerateVariableReferenceExpression(CodeVariableReferenceExpression e) {
        OutputIdentifier(e.VariableName);
      }

      bool IsKeyword(string value) {
        return keywords.ContainsKey(value);
      }


      // Gets whether the specified value is a valid identifier.
      // 'value' is the string to test for validity as an identifier.
      protected override bool IsValidIdentifier(string value) {
        // identifiers must be 1 char or longer
        if (value == null || value.Length == 0) {
          return false;
        }

        VsaEngine engine = VsaEngine.CreateEngine();
        return engine.IsValidIdentifier(value);
      }

      // Gets the token used to represent 'null'.
      protected override string NullToken {
        get {
          return "null";
        }
      }

      // Generates code for the specified System.CodeDom.CodeAttributeBlock.
      protected override void OutputAttributeDeclarations(CodeAttributeDeclarationCollection attributes) {
        if (attributes.Count == 0) return;
        GenerateAttributeDeclarationsStart(attributes);
        IEnumerator en = attributes.GetEnumerator();
        while (en.MoveNext()) {
          CodeAttributeDeclaration current = (CodeAttributeDeclaration)en.Current;
          Output.Write(GetBaseTypeOutput(current.Name));
          Output.Write("(");

          bool firstArg = true;
          foreach (CodeAttributeArgument arg in current.Arguments) {
            if (firstArg) {
              firstArg = false;
            } else {
              Output.Write(", ");
            }
            OutputAttributeArgument(arg);
          }
          Output.Write(") ");
        }
        GenerateAttributeDeclarationsEnd(attributes);
      }

      protected override void OutputDirection(FieldDirection dir) {
        switch (dir) {
          case FieldDirection.In:
            break;
          case FieldDirection.Out:
          case FieldDirection.Ref:
            if (!this.isArgumentList)
              throw new Exception(JScriptException.Localize("No parameter direction", CultureInfo.CurrentUICulture));
            else
              Output.Write("&");
            break;
        }
      }

      protected override void OutputIdentifier(string ident) {
        Output.Write(CreateEscapedIdentifier(ident));
      }

      // Generates code for the specified CodeDom based member access modifier representation.
      // 'attributes' indicates the access modifier to generate code for.
      protected override void OutputMemberAccessModifier(MemberAttributes attributes) {
        switch (attributes & MemberAttributes.AccessMask) {
          case MemberAttributes.Assembly:
            Output.Write("internal ");
            break;
          case MemberAttributes.FamilyAndAssembly:
            Output.Write("internal ");
            break;
          case MemberAttributes.Family:
            Output.Write("protected ");
            break;
          case MemberAttributes.FamilyOrAssembly:
            Output.Write("protected internal ");
            break;
          case MemberAttributes.Public:
            Output.Write("public ");
            break;
          default:
            Output.Write("private ");
            break;
        }
      }

      // Generates code for the specified CodeDom based member scope modifier representation.
      // 'attributes' indicates the scope modifier to generate code for.</para>
      protected override void OutputMemberScopeModifier(MemberAttributes attributes) {
        switch (attributes & MemberAttributes.ScopeMask) {
          case MemberAttributes.Abstract:
            Output.Write("abstract ");
            break;
          case MemberAttributes.Final:
            Output.Write("final ");
            break;
          case MemberAttributes.Static:
            Output.Write("static ");
            break;
          case MemberAttributes.Override:
            Output.Write("override ");
            break;
          default:
            break;
        }
      }

      private void OutputMemberVTableModifier(MemberAttributes attributes) {
        switch (attributes & MemberAttributes.VTableMask) {
          case MemberAttributes.New:
            Output.Write("hide ");
            break;
        }
      }

      // Generates code for the specified CodeDom based parameter declaration expression representation.
      // 'parameters' indicates the parameter declaration expressions to generate code for.
      protected override void OutputParameters(CodeParameterDeclarationExpressionCollection parameters) {
        bool first = true;
        IEnumerator en = parameters.GetEnumerator();
        while (en.MoveNext()) {
          CodeParameterDeclarationExpression current = (CodeParameterDeclarationExpression)en.Current;
          if (first) {
            first = false;
          } else {
            Output.Write(", ");
          }
          GenerateExpression(current);
        }
      }

      private void OutputStartingBrace() {
        if (Options.BracingStyle == "C") {
          Output.WriteLine("");
          Output.WriteLine("{");
        } else {
          Output.WriteLine(" {");
        }
      }

      // Generates code for the specified CodeDom based return type representation.
      // 'typeRef' indicates the return type to generate code for.
      protected override void OutputType(CodeTypeReference typeRef) {
        Output.Write(GetTypeOutput(typeRef));
      }

      protected override void OutputTypeAttributes(TypeAttributes attributes, bool isStruct, bool isEnum) {
        if (isEnum) {
          Output.Write("enum ");
        } else {            
          switch (attributes & TypeAttributes.ClassSemanticsMask) {
            case TypeAttributes.Class:
              if ((attributes & TypeAttributes.Sealed) == TypeAttributes.Sealed) 
                Output.Write("final ");
              if ((attributes & TypeAttributes.Abstract) == TypeAttributes.Abstract)
                Output.Write("abstract ");
              Output.Write("class ");
              break;                
            case TypeAttributes.Interface:
              Output.Write("interface ");
              break;
          }     
        }   
      }

      // Generates code for the specified CodeDom based type name pair representation.
      // 'typeRef' indicates the type to generate code for.
      // 'name' name to generate code for.
      //
      // remarks: This is commonly used for variable declarations.
      protected override void OutputTypeNamePair(CodeTypeReference typeRef, string name) {
        OutputIdentifier(name);
        Output.Write(" : ");
        OutputType(typeRef);
      }

      private void OutputTypeVisibility(TypeAttributes attributes) {
        switch(attributes & TypeAttributes.VisibilityMask) {
          case TypeAttributes.NestedAssembly:
          case TypeAttributes.NestedFamANDAssem:
            Output.Write("internal static ");
            break;
          case TypeAttributes.NestedFamily:
            Output.Write("protected static ");
            break;
          case TypeAttributes.NestedFamORAssem:
            Output.Write("protected internal static ");
            break;
          case TypeAttributes.NotPublic:
            Output.Write("internal ");
            break;
          case TypeAttributes.NestedPrivate:
            Output.Write("private static ");
            break;
          case TypeAttributes.NestedPublic:
            Output.Write("public static ");
            break;
          default:
            Output.Write("public ");
            break;
        }
      }

      protected override void ProcessCompilerOutputLine(CompilerResults results, string line) {
        Match m = outputReg.Match(line);
        if (m.Success) {
          CompilerError ce = new CompilerError();
          // The location is optional since the error can not always be traced to a file.
          if (m.Groups[1].Success) {
            ce.FileName = m.Groups[2].Value;
            ce.Line = int.Parse(m.Groups[4].Value, CultureInfo.InvariantCulture);
            ce.Column = int.Parse(m.Groups[5].Value, CultureInfo.InvariantCulture);
          }
          if (string.Compare(m.Groups[7].Value, "warning", StringComparison.OrdinalIgnoreCase) == 0) {
            ce.IsWarning = true;
          }
          ce.ErrorNumber = m.Groups[8].Value;
          ce.ErrorText = m.Groups[9].Value;

          results.Errors.Add(ce);
        }
      }

      protected override string QuoteSnippetString(string value) {
        return QuoteSnippetStringCStyle(value);
      }

      // Provides conversion to C-style formatting with escape codes.
      private string QuoteSnippetStringCStyle(string value) {
        char[] chars = value.ToCharArray();
        StringBuilder b = new StringBuilder(value.Length+5);

        b.Append("\"");

        int nextBreak = MaxLineLength;
        for (int i=0; i<chars.Length; i++) {
          switch (chars[i]) {
            case '\r':
              b.Append("\\r");
              break;
            case '\t':
              b.Append("\\t");
              break;
            case '\"':
              b.Append("\\\"");
              break;
            case '\'':
              b.Append("\\\'");
              break;
            case '\\':
              b.Append("\\\\");
              break;
            case '\0':
              b.Append("\\0");
              break;
            case '\n':
              b.Append("\\n");
              break;
            case '\u2028':
              b.Append("\\u2028");
              break;
            case '\u2029':
              b.Append("\\u2029");
              break;
            default:
              b.Append(chars[i]);
              break;
          }

          // Insert a newline but only if we've reached max line, there are more characters, 
          // length, and we're not in the middle of a surrogate pair.
          if (i >= nextBreak && i+1 < chars.Length && (!IsSurrogateStart(chars[i]) || !IsSurrogateEnd(chars[i+1]))) {
            nextBreak = i+MaxLineLength;
            b.Append("\" + \r\n\"");
          }
        }

        b.Append("\"");

        return b.ToString();
      }
      
      private bool IsSurrogateStart(char c) {
        return 0xD800 <= c && c <= 0xDBFF; // Is code point the start (high surrogate) of a surrogate pair?
      
      }
      
      private bool IsSurrogateEnd(char c) {
        return 0xDC00 <= c && c <= 0xDFFF; // Is code point the end (i.e. low surrogate) of a surrogate pair?
      }

      protected override void GeneratePrimitiveExpression(CodePrimitiveExpression e) {
        if (null == e.Value) {
          Output.Write("undefined");
        }
        else if (e.Value is System.DBNull){
          Output.Write("null");
        }
        else if (e.Value is char) {
          GeneratePrimitiveChar((char)e.Value);
        }
        else {
          base.GeneratePrimitiveExpression(e);
        }
      }

      private void GeneratePrimitiveChar(char c) {
        Output.Write('\'');
        switch (c) {
        case '\r':
          Output.Write("\\r");
          break;
        case '\t':
          Output.Write("\\t");
          break;
        case '\"':
          Output.Write("\\\"");
          break;
        case '\'':
          Output.Write("\\\'");
          break;
        case '\\':
          Output.Write("\\\\");
          break;
        case '\0':
          Output.Write("\\0");
          break;
        case '\n':
          Output.Write("\\n");
          break;
        case '\u2028':
          Output.Write("\\u2028");
          break;
        case '\u2029':
          Output.Write("\\u2029");
          break;
        default:
          Output.Write(c);
          break;
        }
        Output.Write('\'');
      }


      protected override bool Supports(GeneratorSupport support) {
          return ((support & LanguageSupport) == support);
      }
    }
}
