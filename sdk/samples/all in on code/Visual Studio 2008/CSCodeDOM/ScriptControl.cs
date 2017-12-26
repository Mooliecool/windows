/*********************************** Module Header ***********************************\
* Module Name:	ScriptControl.cs
* Project:		CSCodeDOM
* Copyright (c) Microsoft Corporation.
* 
* The CSCodeDOM project demonstrates how to use the .NET CodeDOM mechanism to enable
* dynamic souce code generation and compilation at runtime.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 6/14/2009 1:00 PM Jie Wang Created
* * 9/14/2009 11:40 AM Jie Wang Bug fixed in RunInternal method.
\*************************************************************************************/

#region Using directives
using System;
using System.IO;
using System.Text;
using System.CodeDom;
using System.CodeDom.Compiler;
using System.Reflection;
using System.Collections.Generic;
using Microsoft.CSharp;
using Microsoft.VisualBasic;
using Microsoft.JScript;
#endregion


namespace CSCodeDOM
{
    public sealed class ScriptControl : MarshalByRefObject
    {
        /// <summary>
        /// Languages supported by <see cref="ScriptControl" />.
        /// </summary>
        public enum Language
        {
            CSharp, VisualBasic, JScript
        }

        private const string ScriptAppDomainFriendlyName = "ScriptDomain";
        private const string ContainerNamespace = "ScriptContainerNamespace";
        private const string ContainerClassName = "ScriptContainer";
        private const string ScriptMethodName = "RunScript";
        private const string LanguageVersion = "v3.5";

        #region .ctor

        /// <summary>
        /// Creates an instance of ScriptControl.
        /// </summary>
        public ScriptControl()
        {
            this.TargetLanguage = Language.VisualBasic;
            this.RunInSeparateDomain = true;
            this.assemblyReferences = new List<string>();
            this.namespaceImports = new List<string>();
        }

        /// <summary>
        /// Creates an instance of ScriptControl.
        /// </summary>
        /// <param name="script">Script text.</param>
        /// <param name="language">Script language.</param>
        public ScriptControl(string script, Language language)
            : this()
        {
            this.Script = script;
            this.TargetLanguage = language;
        }

        #endregion

        #region Private members

        /// <summary>
        /// List of namespaces to be imported
        /// </summary>
        private List<string> namespaceImports;

        /// <summary>
        /// List of assemblies to be references
        /// </summary>
        private List<string> assemblyReferences;

        #endregion

        #region Public interface

        /// <summary>
        /// Runs the script stored in the <see cref="Script"/> property.
        /// </summary>
        /// <returns>The object returned by the script. If the script doesn't return 
        /// anything, an instance of System.Object is returned by default.</returns>
        public object Run()
        {
            object r;
            if (this.RunInSeparateDomain)   // We will run the script in a new AppDomain...
            {
                // Create a new domain for running the script.
                AppDomain scriptDomain = AppDomain.CreateDomain(ScriptAppDomainFriendlyName);
                // Create an instance of ScriptControl inside the new AppDomain.
                ScriptControl sc = (ScriptControl)scriptDomain.CreateInstanceAndUnwrap(
                    Assembly.GetExecutingAssembly().FullName, this.GetType().FullName);

                // Set the property values of the ScriptControl in the new AppDomain.
                // Making the values identical with current instance.
                sc.TargetLanguage = this.TargetLanguage;
                sc.Script = this.Script;

                // Add assembly references
                for (int i = 0; i < this.AssemblyReferences.Count; i++)
                {
                    sc.AddAssemblyReference(this.AssemblyReferences[i]);
                }

                // Add namespace imports.
                for (int i = 0; i < this.CodeNamespaceImports.Count; i++)
                {
                    sc.AddNamespaceImport(this.CodeNamespaceImports[i]);
                }

                // Except this one, other wise the call will end up as an infinite loop.
                sc.RunInSeparateDomain = false;
                // Call the Run method in the remote AppDomain and get the result.
                r = sc.Run();
                // We're done with the new AppDomain, unload it.
                AppDomain.Unload(scriptDomain);
            }
            else
            {
                // We will run the script in current AppDomain, call RunInternal directly.
                r = this.RunInternal();
            }
            return r;
        }

        /// <summary>
        /// Adds a namespace import.
        /// </summary>
        public void AddNamespaceImport(string ns)
        {
            if (this.CodeNamespaceImports.IndexOf(ns) < 0)
            {
                this.CodeNamespaceImports.Add(ns);
            }
        }

        /// <summary>
        /// Removes a namespace import.
        /// </summary>
        public void RemoveNamespaceImport(string ns)
        {
            this.CodeNamespaceImports.Remove(ns);
        }

        /// <summary>
        /// Adds an assembly reference.
        /// </summary>
        public void AddAssemblyReference(string asm)
        {
            if (this.AssemblyReferences.IndexOf(asm) < 0)
            {
                this.AssemblyReferences.Add(asm);
            }
        }

        /// <summary>
        /// Removes an assembly reference.
        /// </summary>
        public void RemoveAssemblyReference(string asm)
        {
            this.AssemblyReferences.Remove(asm);
        }

        /// <summary>
        /// Gets or sets the language.
        /// </summary>
        public Language TargetLanguage
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or sets the script.
        /// </summary>
        public string Script
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or sets whether the script should be run in a separate <see cref="AppDomain"/>.
        /// </summary>
        public bool RunInSeparateDomain
        {
            get;
            set;
        }

        /// <summary>
        /// Gets a list of namespaces to be imported.
        /// </summary>
        public List<string> CodeNamespaceImports
        {
            get { return this.namespaceImports; }
        }

        /// <summary>
        /// Gets a list of assemblies to be referenced.
        /// </summary>
        public List<string> AssemblyReferences
        {
            get { return this.assemblyReferences; }
        }

        #endregion

        #region Internal support methods

        /// <summary>
        /// The inner implementation of <see cref="Run"/>.
        /// </summary>
        private object RunInternal()
        {
            // Build the script into a class and compile it into an in-memory assembly.
            CompilerResults r = CompileCode(BuildClass(this.Script));
            Assembly asm = r.CompiledAssembly;

            // Now extract the method containing the script using reflection...
            Module[] modules = asm.GetModules(false);
            Type[] types = modules[0].GetTypes();

            for (int i = 0; i < types.Length; i++)
            {
                Type type = types[i];
                if (type.IsClass && type.Name == ContainerClassName)  // The class we're looking for.
                {
                    MethodInfo[] mis = type.GetMethods();
                    for (int j = 0; j < mis.Length; j++)
                    {
                        MethodInfo mi = mis[j];
                        if (mi.Name == ScriptMethodName)    // The method we're looking for.
                        {
                            return mi.Invoke(null, null);   // Call the method and return the result.
                        }
                    }
                }
            }

            // Should never be here.
            throw new ApplicationException("Script method not found.");
        }

        /// <summary>
        /// Build the Class source code using CodeDOM.
        /// </summary>
        /// <param name="snippet">Script text to be built into the class.</param>
        /// <returns>CodeDOM generated source.</returns>
        private string BuildClass(string snippet)
        {
            CodeNamespace ns = new CodeNamespace(ContainerNamespace);

            // Import namespaces.
            for (int i = 0; i < this.CodeNamespaceImports.Count; i++)
            {
                ns.Imports.Add(new CodeNamespaceImport(this.CodeNamespaceImports[i]));
            }

            // Create the class declaration.
            CodeTypeDeclaration cls = new CodeTypeDeclaration();
            cls.IsClass = true;
            cls.Name = ContainerClassName;
            cls.Attributes = MemberAttributes.Public;

            // Create the method.
            CodeMemberMethod method = new CodeMemberMethod();
            method.Name = ScriptMethodName;
            method.ReturnType = new CodeTypeReference(typeof(object));  // It will return an object.
            method.Attributes = MemberAttributes.Static | MemberAttributes.Public;
            method.Statements.Add(new CodeSnippetExpression(snippet));  // Add script code into the method.

            // Since we don't know if the script will return something or not, we will add 
            // this return statement to the end of the script code. This will not affect the 
            // script if the script does return something because in this case our return 
            // statement will never be executed. However, if the script doesn't include a 
            // return to the end of its code path, our return statement will return an 
            // instance of System.Object by default. Otherwise, a compile error will occur: 
            // not all code paths return a value.
            method.Statements.Add(new CodeMethodReturnStatement(new CodeObjectCreateExpression(
                typeof(object), new CodeExpression[] { })));

            cls.Members.Add(method);    // Add method to the class.
            ns.Types.Add(cls);          // Add class to the namespace.

            StringBuilder sb = new StringBuilder();
            StringWriter sw = null;
            CodeDomProvider provider = null;
            try
            {
                sw = new StringWriter(sb);
                provider = GetProvider();
                ICodeGenerator generator = provider.CreateGenerator(sw);
                // Generate the code and write to the stream.
                generator.GenerateCodeFromNamespace(ns, sw, new CodeGeneratorOptions());
                sw.Flush();
                sw.Close();
            }
            finally
            {
                if (provider != null) provider.Dispose();
                if (sw != null) sw.Dispose();
            }

            return sb.ToString();   // Return generated source code.
        }

        /// <summary>
        /// Returns an instance of CodeDomProvider according to the target language.
        /// </summary>
        private CodeDomProvider GetProvider()
        {
            Dictionary<string, string> cpOptions = new Dictionary<string, string>();

            switch (this.TargetLanguage)
            {
                case Language.CSharp:
                    cpOptions.Add("CompilerVersion", LanguageVersion);
                    return new CSharpCodeProvider(cpOptions);
                case Language.VisualBasic:
                    cpOptions.Add("CompilerVersion", LanguageVersion);
                    return new VBCodeProvider(cpOptions);
                case Language.JScript:
                    return new JScriptCodeProvider();
                default:
                    throw new NotSupportedException(string.Format(
                        "Target language {0} is not supported.", this.TargetLanguage));
            }
        }

        /// <summary>
        /// Returns an instance of <see cref="CompilerParameters"/> containing the compiler parameters.
        /// </summary>
        private CompilerParameters CreateCompilerParameters()
        {
            CompilerParameters cp = new CompilerParameters();
            cp.CompilerOptions = "/target:library";
            cp.IncludeDebugInformation = true;
            cp.GenerateExecutable = false;
            cp.GenerateInMemory = true;

            // Add assembly references.
            for (int i = 0; i < this.AssemblyReferences.Count; i++)
            {
                cp.ReferencedAssemblies.Add(this.AssemblyReferences[i]);
            }

            return cp;
        }

        /// <summary>
        /// Compiles the source code into assembly.
        /// </summary>
        /// <param name="source">Full source generated by <see cref="BuildClass"/> method.</param>
        private CompilerResults CompileCode(string source)
        {
            CodeDomProvider provider = GetProvider();
            CompilerParameters cp = CreateCompilerParameters();
            CompilerResults cr = provider.CompileAssemblyFromSource(cp, source);

            if (cr.Errors.Count > 0)
            {
                StringBuilder sb = new StringBuilder();
                foreach (CompilerError error in cr.Errors)
                {
                    sb.AppendLine("Compile Error: " + error.ErrorText);
                    sb.AppendLine();
                }
                throw new ApplicationException(sb.ToString());
            }
            return cr;
        }

        #endregion
    }
}