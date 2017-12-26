//------------------------------------------------------------------------------
// <copyright file="Compiler.cs" company="Microsoft">
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
//------------------------------------------------------------------------------

using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Xml.XPath;
using System.Xml.Xsl.Qil;
using System.Xml.Xsl.XPath;

namespace System.Xml.Xsl.Xslt {
    using Res           = System.Xml.Utils.Res;
    using TypeFactory   = XmlQueryTypeFactory;
    using XmlILTrace    = System.Xml.Xsl.IlGen.XmlILTrace;

    internal enum XslVersion {
        Version10           = 0,
        ForwardsCompatible  = 1,
        Current             = Version10,
    }

    internal class Compiler {
        public XsltSettings         Settings;
        public bool                 IsDebug;

        public CompilerResults      CompilerResults;        // Results of the compilation
        public Stylesheet           PrincipalStylesheet;    // Principal stylesheet module
        public int                  CurrentPrecedence = 0;  // Decreases by 1 with each import
        public XslNode              StartApplyTemplates;
        public Scripts              Scripts;
        public Output               Output            = new Output();
        public List<VarPar>         ExternalPars      = new List<VarPar>();
        public List<VarPar>         GlobalVars        = new List<VarPar>();
        public List<WhitespaceRule> WhitespaceRules   = new List<WhitespaceRule>();
        public DecimalFormats       DecimalFormats    = new DecimalFormats();
        public Keys                 Keys              = new Keys();
        public List<ProtoTemplate>  AllTemplates      = new List<ProtoTemplate>();

        public Dictionary<QilName, VarPar>          AllGlobalVarPars  = new Dictionary<QilName, VarPar>();
        public Dictionary<QilName, Template>        NamedTemplates    = new Dictionary<QilName, Template>();
        public Dictionary<QilName, AttributeSet>    AttributeSets     = new Dictionary<QilName, AttributeSet>();
        public Dictionary<string,  NsAlias>         NsAliases         = new Dictionary<string,  NsAlias>();
        public Dictionary<QilName, XslFlags>        ModeFlags         = new Dictionary<QilName, XslFlags>();

        public Dictionary<QilName, List<QilFunction>> ApplyTemplatesFunctions = new Dictionary<QilName, List<QilFunction>>();

        public Compiler(XsltSettings settings) {
            Debug.Assert(CompilerResults == null, "Compiler cannot be reused");

            // Keep all intermediate files if tracing is enabled
            TempFileCollection tempFiles = settings.TempFiles ?? new TempFileCollection();
            if (XmlILTrace.IsEnabled) {
                tempFiles.KeepFiles = true;
            }

            Settings        = settings;
            IsDebug         = settings.IncludeDebugInformation;

            CompilerResults = new CompilerResults(tempFiles);
            Scripts         = new Scripts(this);
        }

        public CompilerResults Compile(object stylesheet, XmlResolver xmlResolver, out QilExpression qil) {
            Debug.Assert(stylesheet != null);
            Debug.Assert(PrincipalStylesheet == null, "Compiler cannot be reused");

            new XsltLoader().Load(this, stylesheet, xmlResolver);
            qil = QilGenerator.CompileStylesheet(this);
            return CompilerResults;
        }

        public Stylesheet CreateStylesheet() {
            Stylesheet sheet = new Stylesheet(this, CurrentPrecedence);
            if (CurrentPrecedence-- == 0) {
                PrincipalStylesheet = sheet;
            }
            return sheet;
        }

        public void ApplyNsAliases(ref string prefix, ref string nsUri) {
            NsAlias alias;
            if (NsAliases.TryGetValue(nsUri, out alias)) {
                nsUri  = alias.ResultNsUri;
                prefix = alias.ResultPrefix;
            }
        }

        // Returns true in case of redefinition
        public bool SetNsAlias(string ssheetNsUri, string resultNsUri, string resultPrefix, int importPrecedence) {
            NsAlias oldNsAlias;
            if (NsAliases.TryGetValue(ssheetNsUri, out oldNsAlias)) {
                // Namespace alias for this stylesheet namespace URI has already been defined
                Debug.Assert(importPrecedence <= oldNsAlias.ImportPrecedence, "Stylesheets must be processed in the order of decreasing import precedence");
                if (importPrecedence < oldNsAlias.ImportPrecedence || resultNsUri == oldNsAlias.ResultNsUri) {
                    // Either the identical definition or lower precedence - ignore it
                    return false;
                }
                // Recover by choosing the declaration that occurs later in the stylesheet
            }
            NsAliases[ssheetNsUri] = new NsAlias(resultNsUri, resultPrefix, importPrecedence);
            return oldNsAlias != null;
        }

        private void MergeWhitespaceRules(Stylesheet sheet) {
            for (int idx = 0; idx <= 2; idx++) {
                sheet.WhitespaceRules[idx].Reverse();
                this.WhitespaceRules.AddRange(sheet.WhitespaceRules[idx]);
            }
            sheet.WhitespaceRules = null;
        }

        private void MergeAttributeSets(Stylesheet sheet) {
            foreach (QilName attSetName in sheet.AttributeSets.Keys) {
                AttributeSet attSet;
                if (!this.AttributeSets.TryGetValue(attSetName, out attSet)) {
                    this.AttributeSets[attSetName] = sheet.AttributeSets[attSetName];
                } else {
                    // Lower import precedence - insert before all previous definitions
                    attSet.MergeContent(sheet.AttributeSets[attSetName]);
                }
            }
            sheet.AttributeSets = null;
        }

        private void MergeGlobalVarPars(Stylesheet sheet) {
            foreach (VarPar var in sheet.GlobalVarPars) {
                Debug.Assert(var.NodeType == XslNodeType.Variable || var.NodeType == XslNodeType.Param);
                if (!AllGlobalVarPars.ContainsKey(var.Name)) {
                    if (var.NodeType == XslNodeType.Variable) {
                        GlobalVars.Add(var);
                    } else {
                        ExternalPars.Add(var);
                    }
                    AllGlobalVarPars[var.Name] = var;
                }
            }
            sheet.GlobalVarPars = null;
        }

        public void MergeWithStylesheet(Stylesheet sheet) {
            MergeWhitespaceRules(sheet);
            MergeAttributeSets(sheet);
            MergeGlobalVarPars(sheet);
        }

        public static string ConstructQName(string prefix, string localName) {
            if (prefix.Length == 0) {
                return localName;
            } else {
                return prefix + ':' + localName;
            }
        }

        public bool ParseQName(string qname, out string prefix, out string localName, IErrorHelper errorHelper) {
            Debug.Assert(qname != null);
            try {
                ValidateNames.ParseQNameThrow(qname, out prefix, out localName);
                return true;
            }
            catch (XmlException e) {
                errorHelper.ReportError(/*[XT_042]*/e.Message, null);
                prefix = PhantomNCName;
                localName = PhantomNCName;
                return false;
            }
        }

        public bool ParseNameTest(string nameTest, out string prefix, out string localName, IErrorHelper errorHelper) {
            Debug.Assert(nameTest != null);
            try {
                ValidateNames.ParseNameTestThrow(nameTest, out prefix, out localName);
                return true;
            }
            catch (XmlException e) {
                errorHelper.ReportError(/*[XT_043]*/e.Message, null);
                prefix = PhantomNCName;
                localName = PhantomNCName;
                return false;
            }
        }

        public void ValidatePiName(string name, IErrorHelper errorHelper) {
            Debug.Assert(name != null);
            try {
                ValidateNames.ValidateNameThrow(
                    /*prefix:*/string.Empty, /*localName:*/name, /*ns:*/string.Empty,
                    XPathNodeType.ProcessingInstruction, ValidateNames.Flags.AllExceptPrefixMapping
                );
            }
            catch (XmlException e) {
                errorHelper.ReportError(/*[XT_044]*/e.Message, null);
            }
        }

        public readonly string  PhantomNCName = "error";
        private int             phantomNsCounter = 0;

        public string CreatePhantomNamespace() {
            // Prepend invalid XmlChar to ensure this name would not clash with any namespace name in the stylesheet
            return "\0namespace" + phantomNsCounter++;
        }

        public bool IsPhantomNamespace(string namespaceName) {
            return namespaceName.Length > 0 && namespaceName[0] == '\0';
        }

        public bool IsPhantomName(QilName qname) {
            string nsUri = qname.NamespaceUri;
            return nsUri.Length > 0 && nsUri[0] == '\0';
        }

        // -------------------------------- Error Handling --------------------------------

        private int ErrorCount {
            get {
                return CompilerResults.Errors.Count;
            }
            set {
                Debug.Assert(value <= ErrorCount);
                for (int idx = ErrorCount - 1; idx >= value; idx--) {
                    CompilerResults.Errors.RemoveAt(idx);
                }
            }
        }

        private int savedErrorCount = -1;

        public void EnterForwardsCompatible() {
            Debug.Assert(savedErrorCount == -1, "Nested EnterForwardsCompatible calls");
            savedErrorCount = ErrorCount;
        }

        // Returns true if no errors were suppressed
        public bool ExitForwardsCompatible(bool fwdCompat) {
            Debug.Assert(savedErrorCount != -1, "ExitForwardsCompatible without EnterForwardsCompatible");
            if (fwdCompat && ErrorCount > savedErrorCount) {
                ErrorCount = savedErrorCount;
                Debug.Assert((savedErrorCount = -1) < 0);
                return false;
            }
            Debug.Assert((savedErrorCount = -1) < 0);
            return true;
        }

        public CompilerError CreateError(ISourceLineInfo lineInfo, string res, params string[] args) {
            return new CompilerError(
                lineInfo.Uri, lineInfo.StartLine, lineInfo.StartPos, /*errorNumber:*/string.Empty,
                /*errorText:*/XslTransformException.CreateMessage(res, args)
            );
        }

        public void ReportError(ISourceLineInfo lineInfo, string res, params string[] args) {
            CompilerError error = CreateError(lineInfo, res, args);
            CompilerResults.Errors.Add(error);
        }

        public void ReportWarning(ISourceLineInfo lineInfo, string res, params string[] args) {
            int warningLevel = 1;
            if (0 <= Settings.WarningLevel && Settings.WarningLevel < warningLevel) {
                // Ignore warning
                return;
            }
            CompilerError error = CreateError(lineInfo, res, args);
            if (Settings.TreatWarningsAsErrors) {
                error.ErrorText = XslTransformException.CreateMessage(Res.Xslt_WarningAsError, error.ErrorText);
                CompilerResults.Errors.Add(error);
            } else {
                error.IsWarning = true;
                CompilerResults.Errors.Add(error);
            }
        }
    }

    internal class Output {
        public XmlWriterSettings        Settings              = new XmlWriterSettings();
        public List<XmlQualifiedName>   CDataSectionElements  = new List<XmlQualifiedName>();
        public string                   Version;
        public string                   Encoding;
        public XmlQualifiedName         Method;

        // All the xsl:output elements occurring in a stylesheet are merged into a single effective xsl:output element.
        // We store the import precedence of each attribute value to catch redefinitions with the same import precedence.
        public const int NeverDeclaredPrec  = int.MinValue;
        public int MethodPrec               = NeverDeclaredPrec;
        public int VersionPrec              = NeverDeclaredPrec;
        public int EncodingPrec             = NeverDeclaredPrec;
        public int OmitXmlDeclarationPrec   = NeverDeclaredPrec;
        public int StandalonePrec           = NeverDeclaredPrec;
        public int DocTypePublicPrec        = NeverDeclaredPrec;
        public int DocTypeSystemPrec        = NeverDeclaredPrec;
        public int IndentPrec               = NeverDeclaredPrec;
        public int MediaTypePrec            = NeverDeclaredPrec;

        public Output() {
            Settings.OutputMethod = XmlOutputMethod.AutoDetect;
            Settings.AutoXmlDeclaration = true;
            Settings.ConformanceLevel = ConformanceLevel.Auto;
            Settings.MergeCDataSections = true;
        }
    }

    internal class DecimalFormats : KeyedCollection<XmlQualifiedName, DecimalFormatDecl> {
        protected override XmlQualifiedName GetKeyForItem(DecimalFormatDecl format) {
            return format.Name;
        }
    }

    internal class DecimalFormatDecl {
        public readonly XmlQualifiedName    Name;
        public readonly string              InfinitySymbol;
        public readonly string              NanSymbol;
        public readonly char[]              Characters;

        public static DecimalFormatDecl Default = new DecimalFormatDecl(new XmlQualifiedName(), "Infinity", "NaN", ".,%\u20300#;-");

        public DecimalFormatDecl(XmlQualifiedName name, string infinitySymbol, string nanSymbol, string characters) {
            Debug.Assert(characters.Length == 8);
            this.Name           = name;
            this.InfinitySymbol = infinitySymbol;
            this.NanSymbol      = nanSymbol;
            this.Characters     = characters.ToCharArray();
        }
    }

    internal class NsAlias {
        public readonly string  ResultNsUri;
        public readonly string  ResultPrefix;
        public readonly int     ImportPrecedence;

        public NsAlias(string resultNsUri, string resultPrefix, int importPrecedence) {
            this.ResultNsUri      = resultNsUri;
            this.ResultPrefix     = resultPrefix;
            this.ImportPrecedence = importPrecedence;
        }
    }
}
