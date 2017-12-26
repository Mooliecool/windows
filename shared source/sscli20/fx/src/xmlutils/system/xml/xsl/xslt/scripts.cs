//------------------------------------------------------------------------------
// <copyright file="Scripts.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
// <spec>http://devdiv/Documents/Whidbey/CLR/CurrentSpecs/BCL/CodeDom%20Activation.doc</spec>
//------------------------------------------------------------------------------

using System.CodeDom;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Configuration;
using System.Diagnostics;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Threading;
using System.Xml.Xsl.IlGen;
using System.Xml.Xsl.Runtime;
using System.Security.Permissions;

namespace System.Xml.Xsl.Xslt {
    using Res = System.Xml.Utils.Res;

    internal class ScriptClass {
        public string               ns;
        public CompilerInfo         compilerInfo;
        public StringCollection     refAssemblies = new StringCollection();
        public StringCollection     nsImports     = new StringCollection();
        public StringCollection     scriptFiles   = new StringCollection();
        public CodeTypeDeclaration  typeDecl;

        // These three fields are used to report a compile error when its position is outside
        // of all user code snippets in the generated temporary file
        public string               endFileName;
        public int                  endLine;
        public int                  endPos;

        public ScriptClass(string ns, CompilerInfo compilerInfo) {
            this.ns           = ns;
            this.compilerInfo = compilerInfo;
            this.typeDecl     = new CodeTypeDeclaration(GenerateUniqueClassName());
        }

        private static long scriptClassCounter = 0;

        private static string GenerateUniqueClassName() {
            return "Script" + Interlocked.Increment(ref scriptClassCounter);
        }

        public void AddScriptBlock(string source, string uriString, int lineNumber, int endLine, int endPos) {
            CodeSnippetTypeMember scriptSnippet = new CodeSnippetTypeMember(source);
            string fileName = SourceLineInfo.GetFileName(uriString);
            if (lineNumber > 0) {
                scriptSnippet.LinePragma = new CodeLinePragma(fileName, lineNumber);
                scriptFiles.Add(fileName);
            }
            typeDecl.Members.Add(scriptSnippet);

            this.endFileName  = fileName;
            this.endLine      = endLine;
            this.endPos       = endPos;
        }

        // The position of a compile error may be outside of all user code snippets (for example, in case of
        // unclosed '{'). In that case filename would be the name of the temporary file, and not the name
        // of the stylesheet file. Exposing the path of the temporary file is considered to be a security issue,
        // so here we check that filename is amongst user files
        public void FixFileName(CompilerError e) {
            string fileName = e.FileName;
            foreach (string scriptFile in scriptFiles) {
                if (fileName.Equals(scriptFile, StringComparison.OrdinalIgnoreCase)) {
                    // The error position is within one of user stylesheets, its filename may be reported
                    e.FileName = scriptFile;
                    return;
                }
            }

            // Error is outside user files, we should hide filename for security reasons.
            // Return filename and position of the end of the last script block for the given class.
            e.FileName  = this.endFileName;
            e.Line      = this.endLine;
            e.Column    = this.endPos;
        }

        public CompilerError CreateCompileExceptionError(Exception e) {
            string errorText = XslTransformException.CreateMessage(/*[XT_041]*/Res.Xslt_ScriptCompileException, e.Message);
            return new CompilerError(this.endFileName, this.endLine, this.endPos, /*errorNumber:*/string.Empty, errorText);
        }
    }

    internal class Scripts {
        private Compiler                  compiler;
        private List<ScriptClass>         scriptClasses = new List<ScriptClass>();
        private Dictionary<string, Type>  nsToType      = new Dictionary<string, Type>();
        private XmlExtensionFunctionTable extFuncs      = new XmlExtensionFunctionTable();

        public Scripts(Compiler compiler) {
            this.compiler = compiler;
        }

        public Dictionary<string, Type> ScriptClasses {
            get { return nsToType; }
        }

        public XmlExtensionFunction ResolveFunction(string name, string ns, int numArgs, IErrorHelper errorHelper) {
            Type type;
            if (nsToType.TryGetValue(ns, out type)) {
                try {
                    return extFuncs.Bind(name, ns, numArgs, type, XmlQueryRuntime.EarlyBoundFlags);
                }
                catch (XslTransformException e) {
                    errorHelper.ReportError(e.Message);
                }
            }
            return null;
        }

        public ScriptClass GetScriptClass(string ns, string language, IErrorHelper errorHelper) {
            CompilerInfo compilerInfo;
            try {
                compilerInfo = CodeDomProvider.GetCompilerInfo(language);
                Debug.Assert(compilerInfo != null);
            }
            catch (ConfigurationException) {
                // There is no CodeDom provider defined for this language
                errorHelper.ReportError(/*[XT_010]*/Res.Xslt_ScriptInvalidLanguage, language);
                return null;
            }

            foreach (ScriptClass scriptClass in scriptClasses) {
                if (ns == scriptClass.ns) {
                    if (!compilerInfo.Equals(scriptClass.compilerInfo)) {
                        errorHelper.ReportError(/*[XT_011]*/Res.Xslt_ScriptMixedLanguages, ns);
                        return null;
                    }
                    return scriptClass;
                }
            }

            ScriptClass newScriptClass = new ScriptClass(ns, compilerInfo);
            newScriptClass.typeDecl.TypeAttributes = TypeAttributes.Public;
            scriptClasses.Add(newScriptClass);
            return newScriptClass;
        }

        // ------------- CompileScripts() -----------

        public void CompileScripts() {
            for (int i = 0; i < scriptClasses.Count; i ++) {
                ScriptClass script = scriptClasses[i];
                Type clrType = CompileClass(script);
                if (clrType != null) {
                    nsToType.Add(script.ns, clrType);
                }
            }
        }

        // Namespaces we always import when compiling
        private static string[] defaultNamespaces = new string[] {
            "System",
            "System.Collections",
            "System.Text",
            "System.Text.RegularExpressions",
            "System.Xml",
            "System.Xml.Xsl",
            "System.Xml.XPath",
        };

        [PermissionSet(SecurityAction.Demand, Name = "FullTrust")]
        private Type CompileClass(ScriptClass script) {
            TempFileCollection      allTempFiles  = compiler.CompilerResults.TempFiles;
            CompilerErrorCollection allErrors     = compiler.CompilerResults.Errors;
            CodeDomProvider         provider;
            bool                    isVB          = false;

            try {
                provider = script.compilerInfo.CreateProvider();
            }
            catch (ConfigurationException e) {
                // The CodeDom provider type could not be located, or some error in machine.config
                allErrors.Add(script.CreateCompileExceptionError(e));
                return null;
            }

            CodeNamespace scriptNs = new CodeNamespace("System.Xml.Xsl.CompiledQuery");

            // Add #using directives
            foreach (string ns in defaultNamespaces) {
                scriptNs.Imports.Add(new CodeNamespaceImport(ns));
            }
            if (isVB) {
                scriptNs.Imports.Add(new CodeNamespaceImport("Microsoft.VisualBasic"));
            }
            foreach (string ns in script.nsImports) {
                scriptNs.Imports.Add(new CodeNamespaceImport(ns));
            }

            scriptNs.Types.Add(script.typeDecl);

            CodeCompileUnit unit = new CodeCompileUnit(); {
                unit.Namespaces.Add(scriptNs);
                unit.UserData["AllowLateBound"]             = true;  // Allow variables to be undeclared
                unit.UserData["RequireVariableDeclaration"] = false; // Allow variables to be declared untyped
                unit.AssemblyCustomAttributes.Add(new CodeAttributeDeclaration("System.Security.SecurityTransparentAttribute"));
            }

            CompilerParameters compilParams = new CompilerParameters();

            compilParams.ReferencedAssemblies.Add(typeof(System.Xml.Res).Assembly.Location);
            compilParams.ReferencedAssemblies.Add("System.dll");
            if (isVB) {
                compilParams.ReferencedAssemblies.Add("Microsoft.VisualBasic.dll");
            }
            foreach(string name in script.refAssemblies) {
                compilParams.ReferencedAssemblies.Add(name);
            }

            XsltSettings settings                 = compiler.Settings;
            compilParams.WarningLevel             = settings.WarningLevel >= 0 ? settings.WarningLevel : 4;
            compilParams.TreatWarningsAsErrors    = settings.TreatWarningsAsErrors;
            compilParams.IncludeDebugInformation  = settings.IncludeDebugInformation;
            compilParams.CompilerOptions          = script.compilerInfo.CreateDefaultCompilerParameters().CompilerOptions;

            bool keepFiles = (settings.IncludeDebugInformation || XmlILTrace.IsEnabled) && !settings.CheckOnly;

            compilParams.GenerateInMemory = settings.CheckOnly;

            // We need only .dll and .pdb, but there is no way to specify that
            compilParams.TempFiles.KeepFiles = keepFiles;

            CompilerResults results;

            try {
                results = provider.CompileAssemblyFromDom(compilParams, unit);
            }
            catch (ExternalException e) {
                // Compiler might have created temporary files
                results = new CompilerResults(compilParams.TempFiles);
                results.Errors.Add(script.CreateCompileExceptionError(e));
            }

            if (!settings.CheckOnly) {
                foreach (string fileName in results.TempFiles) {
                    allTempFiles.AddFile(fileName, allTempFiles.KeepFiles);
                }
            }
            foreach (CompilerError e in results.Errors) {
                script.FixFileName(e);
            }
            allErrors.AddRange(results.Errors);
            if (results.Errors.HasErrors) {
                return null;
            }
            return results.CompiledAssembly.GetType("System.Xml.Xsl.CompiledQuery." + script.typeDecl.Name);
        }
    }
}
