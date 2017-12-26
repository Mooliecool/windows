//------------------------------------------------------------------------------
// <copyright file="XslCompiledTransform.cs" company="Microsoft">
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
// <spec>http://webdata/xml/specs/XslCompiledTransform.xml</spec>
//------------------------------------------------------------------------------

using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Xml.XPath;
using System.Xml.Xsl.Qil;
using System.Xml.Xsl.Runtime;
using System.Xml.Xsl.XPath;
using System.Xml.Xsl.Xslt;
using System.Security.Permissions;

namespace System.Xml.Xsl {
#if ! HIDE_XSL
    using XsltCompiler = System.Xml.Xsl.Xslt.Compiler;

    //----------------------------------------------------------------------------------------------------
    //  Clarification on null values in this API:
    //      stylesheet, stylesheetUri   - cannot be null
    //      settings                    - if null, XsltSettings.Default will be used
    //      stylesheetResolver          - if null, XmlNullResolver will be used for includes/imports.
    //                                    However, if the principal stylesheet is given by its URI, that
    //                                    URI will be resolved using XmlUrlResolver (for compatibility
    //                                    with XslTransform and XmlReader).
    //      documentResolver            - if null, XmlNullResolver will be used
    //      input, inputUri             - cannot be null
    //      arguments                   - null means no arguments
    //      results, resultsFile        - cannot be null
    //----------------------------------------------------------------------------------------------------

    public sealed class XslCompiledTransform {
        // Options of compilation
        private bool                enableDebug     = false;

        // Results of compilation
        private CompilerResults     compilerResults = null;
        private XmlWriterSettings   outputSettings  = null;
        private QilExpression       qil             = null;
        private XmlCommand          command         = null;

        public XslCompiledTransform() {}

        public XslCompiledTransform(bool enableDebug) {
            this.enableDebug = enableDebug;
        }

        /// <summary>
        /// This function is called on every recompilation to discard all previous results
        /// </summary>
        private void Reset() {
            compilerResults = null;
            outputSettings  = null;
            qil             = null;
            command         = null;
        }

        internal CompilerErrorCollection Errors {
            get { return compilerResults != null ? compilerResults.Errors : null; }
        }

        /// <summary>
        /// Writer settings specified in the stylesheet
        /// </summary>
        public XmlWriterSettings OutputSettings {
            get { return outputSettings; }
        }

        public TempFileCollection TemporaryFiles {
            [PermissionSet(SecurityAction.LinkDemand, Name="FullTrust")]
            get { return compilerResults != null ? compilerResults.TempFiles : null; }
        }

        //------------------------------------------------
        // Load methods
        //------------------------------------------------

        public void Load(XmlReader stylesheet) {
            Reset();
            LoadInternal(stylesheet, XsltSettings.Default, new XmlUrlResolver());
        }

        public void Load(XmlReader stylesheet, XsltSettings settings, XmlResolver stylesheetResolver) {
            Reset();
            LoadInternal(stylesheet, settings, stylesheetResolver);
        }

        public void Load(IXPathNavigable stylesheet) {
            Reset();
            LoadInternal(stylesheet, XsltSettings.Default, new XmlUrlResolver());
        }

        public void Load(IXPathNavigable stylesheet, XsltSettings settings, XmlResolver stylesheetResolver) {
            Reset();
            LoadInternal(stylesheet, settings, stylesheetResolver);
        }

        public void Load(string stylesheetUri) {
            Reset();
            if (stylesheetUri == null) {
                throw new ArgumentNullException("stylesheetUri");
            }
            LoadInternal(stylesheetUri, XsltSettings.Default, new XmlUrlResolver());
        }

        public void Load(string stylesheetUri, XsltSettings settings, XmlResolver stylesheetResolver) {
            Reset();
            if (stylesheetUri == null) {
                throw new ArgumentNullException("stylesheetUri");
            }
            LoadInternal(stylesheetUri, settings, stylesheetResolver);
        }

        private CompilerResults LoadInternal(object stylesheet, XsltSettings settings, XmlResolver stylesheetResolver) {
            if (stylesheet == null) {
                throw new ArgumentNullException("stylesheet");
            }
            if (settings == null) {
                settings = XsltSettings.Default;
            }
            CompileToQil(stylesheet, settings, stylesheetResolver);
            CompilerError error = FirstError();
            if (error != null) {
                throw new XslLoadException(error);
            }
            if (!settings.CheckOnly) {
                CompileIlFromQil(settings);
            }
            return compilerResults;
        }

        private void CompileToQil(object stylesheet, XsltSettings settings, XmlResolver stylesheetResolver) {
            settings.IncludeDebugInformation |= enableDebug;
            XsltCompiler compiler = new XsltCompiler(settings);
            compilerResults = compiler.Compile(stylesheet, stylesheetResolver, out qil);
        }

        /// <summary>
        /// Returns the first compiler error except warnings
        /// </summary>
        private CompilerError FirstError() {
            foreach (CompilerError error in compilerResults.Errors) {
                if (!error.IsWarning) {
                    return error;
                }
            }
            return null;
        }

        private void CompileIlFromQil(XsltSettings settings) {
            command = new XmlILGenerator().Generate(qil, settings.AssemblyName);
            // Set outputSettings only if compilation was successful
            outputSettings = qil.DefaultWriterSettings;
            qil = null;
        }

        //------------------------------------------------
        // Transform methods which take an IXPathNavigable
        //------------------------------------------------

        public void Transform(IXPathNavigable input, XmlWriter results) {
            CheckCommand();
            CheckInput(input);
            command.Execute(input, new XmlUrlResolver(), (XsltArgumentList)null, results);
        }

        public void Transform(IXPathNavigable input, XsltArgumentList arguments, XmlWriter results) {
            CheckCommand();
            CheckInput(input);
            command.Execute(input, new XmlUrlResolver(), arguments, results);
        }

        public void Transform(IXPathNavigable input, XsltArgumentList arguments, TextWriter results) {
            CheckCommand();
            CheckInput(input);
            command.Execute(input, new XmlUrlResolver(), arguments, results);
        }

        public void Transform(IXPathNavigable input, XsltArgumentList arguments, Stream results) {
            CheckCommand();
            CheckInput(input);
            command.Execute(input, new XmlUrlResolver(), arguments, results);
        }

        //------------------------------------------------
        // Transform methods which take an XmlReader
        //------------------------------------------------

        public void Transform(XmlReader input, XmlWriter results) {
            CheckCommand();
            CheckInput(input);
            command.Execute(input, new XmlUrlResolver(), (XsltArgumentList)null, results);
        }

        public void Transform(XmlReader input, XsltArgumentList arguments, XmlWriter results) {
            CheckCommand();
            CheckInput(input);
            command.Execute(input, new XmlUrlResolver(), arguments, results);
        }

        public void Transform(XmlReader input, XsltArgumentList arguments, TextWriter results) {
            CheckCommand();
            CheckInput(input);
            command.Execute(input, new XmlUrlResolver(), arguments, results);
        }

        public void Transform(XmlReader input, XsltArgumentList arguments, Stream results) {
            CheckCommand();
            CheckInput(input);
            command.Execute(input, new XmlUrlResolver(), arguments, results);
        }

        public void Transform(XmlReader input, XsltArgumentList arguments, XmlWriter results, XmlResolver documentResolver) {
            CheckCommand();
            CheckInput(input);
            command.Execute(input, documentResolver, arguments, results);
        }

        //------------------------------------------------
        // Transform methods which take a uri
        //------------------------------------------------

        public void Transform(string inputUri, string resultsFile) {
            CheckCommand();
            // SQLBUDT 276415: Prevent wiping out the content of the input file if the output file is the same
            using (XmlReader input = CreateReader(inputUri)) {
                if (resultsFile == null) {
                    throw new ArgumentNullException("resultsFile");
                }
                using (FileStream output = new FileStream(resultsFile, FileMode.Create, FileAccess.Write)) {
                    command.Execute(input, new XmlUrlResolver(), (XsltArgumentList)null, output);
                }
            }
        }

        public void Transform(string inputUri, XmlWriter results) {
            CheckCommand();
            using (XmlReader input = CreateReader(inputUri)) {
                command.Execute(input, new XmlUrlResolver(), (XsltArgumentList)null, results);
            }
        }

        public void Transform(string inputUri, XsltArgumentList arguments, XmlWriter results) {
            CheckCommand();
            using (XmlReader input = CreateReader(inputUri)) {
                command.Execute(input, new XmlUrlResolver(), arguments, results);
            }
        }

        public void Transform(string inputUri, XsltArgumentList arguments, TextWriter results) {
            CheckCommand();
            using (XmlReader input = CreateReader(inputUri)) {
                command.Execute(input, new XmlUrlResolver(), arguments, results);
            }
        }

        public void Transform(string inputUri, XsltArgumentList arguments, Stream results) {
            CheckCommand();
            using (XmlReader input = CreateReader(inputUri)) {
                command.Execute(input, new XmlUrlResolver(), arguments, results);
            }
        }

        //------------------------------------------------
        // Helper methods
        //------------------------------------------------

        private void CheckCommand() {
            if (command == null) {
                throw new InvalidOperationException(Res.GetString(Res.Xslt_NoStylesheetLoaded));
            }
        }

        private void CheckInput(object input) {
            if (input == null) {
                throw new System.ArgumentNullException("input");
            }
        }

        private XmlReader CreateReader(string inputUri) {
            if (inputUri == null) {
                throw new ArgumentNullException("inputUri");
            }
            XmlReaderSettings rs = new XmlReaderSettings();
            rs.ProhibitDtd = true;
            return XmlReader.Create(inputUri, rs);
        }

        //------------------------------------------------
        // Test suites entry points
        //------------------------------------------------

        private QilExpression TestCompile(object stylesheet, XsltSettings settings, XmlResolver stylesheetResolver) {
            Reset();
            CompileToQil(stylesheet, settings, stylesheetResolver);
            return qil;
        }

        private void TestGenerate(XsltSettings settings) {
            Debug.Assert(qil != null, "You must compile to Qil first");
            CompileIlFromQil(settings);
        }

        private void Transform(string inputUri, XsltArgumentList arguments, XmlWriter results, XmlResolver documentResolver) {
            ((XmlILCommand)command).Execute(inputUri, documentResolver, arguments, results);
        }

        internal static void PrintQil(object qil, XmlWriter xw, bool printComments, bool printTypes, bool printLineInfo) {
            QilExpression qilExpr = (QilExpression)qil;
            QilXmlWriter.Options options = QilXmlWriter.Options.None;
            QilValidationVisitor.Validate(qilExpr);
            if (printComments) options |= QilXmlWriter.Options.Annotations;
            if (printTypes) options |= QilXmlWriter.Options.TypeInfo;
            if (printLineInfo) options |= QilXmlWriter.Options.LineInfo;
            QilXmlWriter qw = new QilXmlWriter(xw, options);
            qw.ToXml(qilExpr);
            xw.Flush();
        }
    }
#endif // ! HIDE_XSL
}
