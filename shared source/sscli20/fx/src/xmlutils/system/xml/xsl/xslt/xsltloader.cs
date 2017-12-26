//------------------------------------------------------------------------------
// <copyright file="XsltLoader.cs" company="Microsoft">
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

using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Diagnostics;
using System.Reflection;
using System.Text;
using System.IO;
using System.Xml.XPath;
using System.Xml.Xsl.Qil;

namespace System.Xml.Xsl.Xslt {
    using ContextInfo   = XsltInput.ContextInfo;
    using f             = AstFactory;
    using Res           = System.Xml.Utils.Res;
    using TypeFactory   = XmlQueryTypeFactory;

    internal class XsltLoader : IErrorHelper {
        private Compiler                compiler;
        private XmlResolver             xmlResolver;
        private QueryReaderSettings     readerSettings;
        private XsltInput               input;          // Current input stream
        private Stylesheet              curStylesheet;  // Current stylesheet
        private Template                curTemplate;    // Current template; used in XslApplyImports() only

        private static QilName          nullMode        = f.QName(string.Empty);

        public void Load(Compiler compiler, object stylesheet, XmlResolver xmlResolver) {
            Debug.Assert(compiler != null);
            this.compiler = compiler;
            this.xmlResolver = xmlResolver ?? XmlNullResolver.Singleton;

            XmlReader reader = stylesheet as XmlReader;
            if (reader != null) {
                readerSettings = new QueryReaderSettings(reader);
                LoadStylesheet(reader, /*include:*/false);
            } else {
                // We should take DefaultReaderSettings from Compiler.Settings.DefaultReaderSettings.

                string uri = stylesheet as string;
                if (uri != null) {
                    // If xmlResolver == null, then the original uri will be resolved using XmlUrlResolver
                    XmlResolver origResolver = xmlResolver ?? new XmlUrlResolver();
                    Uri resolvedUri = origResolver.ResolveUri(null, uri);
                    if (resolvedUri == null) {
                        throw new XslLoadException(Res.Xslt_CantResolve, uri);
                    }

                    readerSettings = new QueryReaderSettings(new NameTable());
                    using (reader = CreateReader(resolvedUri, origResolver)) {
                        LoadStylesheet(reader, /*include:*/false);
                    }
                } else {
                    IXPathNavigable navigable = stylesheet as IXPathNavigable;
                    if (navigable != null) {
                        reader = XPathNavigatorReader.Create(navigable.CreateNavigator());
                        readerSettings = new QueryReaderSettings(reader.NameTable);
                        LoadStylesheet(reader, /*include:*/false);
                    } else {
                        Debug.Fail("Should never get here");
                    }
                }
            }
            Process();
        }

        private void Process() {
            Debug.Assert(compiler.PrincipalStylesheet != null);
            compiler.StartApplyTemplates = f.ApplyTemplates(nullMode);
            ProcessOutputSettings();
            ProcessAttributeSets();
        }

        // Import/Include XsltInput management

        private HybridDictionary documentUriInUse = new HybridDictionary();

        private Uri ResolveUri(string relativeUri, string baseUri) {
            Uri resolvedBaseUri = (baseUri.Length != 0) ? xmlResolver.ResolveUri(null, baseUri) : null;
            Uri resolvedUri = xmlResolver.ResolveUri(resolvedBaseUri, relativeUri);
            if (resolvedUri == null) {
                throw new XslLoadException(Res.Xslt_CantResolve, relativeUri);
            }
            return resolvedUri;
        }

        private XmlReader CreateReader(Uri uri, XmlResolver xmlResolver) {
            object input = xmlResolver.GetEntity(uri, null, null);

            Stream stream = input as Stream;
            if (stream != null) {
                return readerSettings.CreateReader(stream, uri.ToString());
            }

            XmlReader reader = input as XmlReader;
            if (reader != null) {
                return reader;
            }

            IXPathNavigable navigable = input as IXPathNavigable;
            if (navigable != null) {
                return XPathNavigatorReader.Create(navigable.CreateNavigator());
            }

            throw new XslLoadException(Res.Xslt_CannotLoadStylesheet, uri.ToString(), input == null ? "null" : input.GetType().ToString());
        }

        private Stylesheet LoadStylesheet(Uri uri, bool include) {
            using (XmlReader reader = CreateReader(uri, this.xmlResolver)) {
                return LoadStylesheet(reader, include);
            }
        }

        private Stylesheet LoadStylesheet(XmlReader reader, bool include) {
            string baseUri = reader.BaseURI;
            Debug.Assert(!documentUriInUse.Contains(baseUri), "Circular references must be checked while processing xsl:include and xsl:import");
            documentUriInUse.Add(baseUri, null);

            Stylesheet  prevStylesheet  = curStylesheet;
            XsltInput   prevInput       = input;
            Stylesheet  thisStylesheet  = include ? curStylesheet : compiler.CreateStylesheet();

            input         = new XsltInput(reader, compiler);
            curStylesheet = thisStylesheet;

            try {
                LoadDocument();
                if (!include) {
                    compiler.MergeWithStylesheet(curStylesheet);

                    List<Uri> importHrefs = curStylesheet.ImportHrefs;
                    curStylesheet.Imports = new Stylesheet[importHrefs.Count];
                    // We can't reverce imports order. Template lookup relies on it after compilation
                    // Imports should be compiled in the reverse order
                    for (int i = importHrefs.Count - 1; 0 <= i; i--) {
                        curStylesheet.Imports[i] = LoadStylesheet(importHrefs[i], /*include:*/false);
                    }
                }
            }
            catch (XslLoadException) {
                throw;
            }
            catch (Exception e) {
                if (!XmlException.IsCatchableException(e)) {
                    throw;
                }
                XmlException ex = e as XmlException;
                if (ex != null) {
                    SourceLineInfo lineInfo = new SourceLineInfo(input.Uri, ex.LineNumber, ex.LinePosition, ex.LineNumber, ex.LinePosition);
                    throw new XslLoadException(ex, lineInfo);
                }
                input.FixLastLineInfo();
                throw new XslLoadException(e, input.BuildLineInfo());
            }
            finally {
                documentUriInUse.Remove(baseUri);
                input         = prevInput;
                curStylesheet = prevStylesheet;
            }
            return thisStylesheet;
        }

        private void LoadDocument() {
            if (!input.Start()) {
                ReportError(/*[XT_002]*/Res.Xslt_WrongStylesheetElement);
                return;
            }
            Debug.Assert(input.NodeType == XPathNodeType.Element);
            if (input.IsXsltNamespace()) {
                if (
                    input.IsKeyword(input.Atoms.Stylesheet) ||
                    input.IsKeyword(input.Atoms.Transform)
                ) {
                    LoadRealStylesheet();
                } else {
                    ReportError(/*[XT_002]*/Res.Xslt_WrongStylesheetElement);
                    input.SkipNode();
                }
            } else {
                LoadSimplifiedStylesheet();
            }
            input.Finish();
        }

        private void LoadSimplifiedStylesheet() {
            Debug.Assert(!input.IsXsltNamespace());
            Debug.Assert(curTemplate == null);

            // Prefix will be fixed later in LoadLiteralResultElement()
            curTemplate = f.Template(/*name:*/null, /*match:*/"/", /*mode:*/nullMode, /*priority:*/double.NaN, input.XslVersion);

            // This template has mode=null match="/" and no imports
            input.CanHaveApplyImports = true;
            XslNode lre = LoadLiteralResultElement(/*asStylesheet:*/true);
            if (lre != null) {
                SetLineInfo(curTemplate, lre.SourceLine);

                List<XslNode> content = new List<XslNode>();
                content.Add(lre);
                SetContent(curTemplate, content);
                if (!curStylesheet.AddTemplate(curTemplate)) {
                    Debug.Fail("AddTemplate() returned false for simplified stylesheet");
                }
            }
            curTemplate = null;
        }

        private void InsertExNamespaces(string value, ref NsDecl nsList, bool extensions) {
            if (value != null && value.Length != 0) {
                compiler.EnterForwardsCompatible();

                string[] list = XmlConvert.SplitString(value);
                for (int idx = 0; idx < list.Length; idx++) {
                    list[idx] = input.LookupXmlNamespace(list[idx] == "#default" ? string.Empty : list[idx]);
                }

                if (!compiler.ExitForwardsCompatible(input.ForwardCompatibility)) {
                    // There were errors in the list, ignore the whole list
                    return;
                }

                for (int idx = 0; idx < list.Length; idx++) {
                    if (list[idx] != null) {
                        nsList = new NsDecl(nsList, /*prefix:*/null, list[idx]);
                        if (extensions) {
                            input.AddExtensionNamespace(list[idx]);
                        }
                    }
                }
            }
        }

        private void LoadRealStylesheet() {
            string attVersion  ;
            string attExtension;
            string attExclude  ;
            string attId       ;

            Debug.Assert(input.IsXsltNamespace() && (input.IsKeyword(input.Atoms.Stylesheet) || input.IsKeyword(input.Atoms.Transform)));
            Debug.Assert(!input.ForwardCompatibility, "We shouldn't be in FC mode before we parsed stylesheet element");

            ContextInfo ctxInfo = input.GetAttributes(/*required:*/1,
                input.Atoms.Version                 , out attVersion  ,
                input.Atoms.ExtensionElementPrefixes, out attExtension,
                input.Atoms.ExcludeResultPrefixes   , out attExclude  ,
                input.Atoms.Id                      , out attId
            );
            if (attVersion == null) {
                input.SetVersion("1.0", input.Atoms.Version);
            }
            InsertExNamespaces(attExtension, ref ctxInfo.nsList, /*extensions:*/ true);
            InsertExNamespaces(attExclude  , ref ctxInfo.nsList, /*extensions:*/ false);
            if (attId != null) {
                // Do nothing here.
            }

            string elementName = input.QualifiedName;

            // Load top level elements:
            if (input.MoveToFirstChild()) {
                bool atTop = true;
                do {
                    bool isImport = false;
                    switch (input.NodeType) {
                    case XPathNodeType.Element:
                        if (input.IsXsltNamespace()) {
                            if (input.IsKeyword(input.Atoms.Import)) {
                                if (!atTop) {
                                    ReportError(/*[XT0200]*/Res.Xslt_NotAtTop, input.QualifiedName, elementName);
                                    input.SkipNode();
                                } else {
                                    isImport = true;
                                    LoadImport();
                                }
                            } else {
                                if (input.IsKeyword(input.Atoms.Include)) {
                                    LoadInclude();
                                } else if (input.IsKeyword(input.Atoms.StripSpace)) {
                                    LoadStripSpace(ctxInfo.nsList);
                                } else if (input.IsKeyword(input.Atoms.PreserveSpace)) {
                                    LoadPreserveSpace(ctxInfo.nsList);
                                } else if (input.IsKeyword(input.Atoms.Output)) {
                                    LoadOutput();
                                } else if (input.IsKeyword(input.Atoms.Key)) {
                                    LoadKey(ctxInfo.nsList);
                                } else if (input.IsKeyword(input.Atoms.DecimalFormat)) {
                                    LoadDecimalFormat(ctxInfo.nsList);
                                } else if (input.IsKeyword(input.Atoms.NamespaceAlias)) {
                                    LoadNamespaceAlias(ctxInfo.nsList);
                                } else if (input.IsKeyword(input.Atoms.AttributeSet)) {
                                    LoadAttributeSet(ctxInfo.nsList);
                                } else if (input.IsKeyword(input.Atoms.Variable)) {
                                    LoadGlobalVariableOrParameter(ctxInfo.nsList, XslNodeType.Variable);
                                } else if (input.IsKeyword(input.Atoms.Param)) {
                                    LoadGlobalVariableOrParameter(ctxInfo.nsList, XslNodeType.Param);
                                } else if (input.IsKeyword(input.Atoms.Template)) {
                                    LoadTemplate(ctxInfo.nsList);
                                } else {
                                    if (!input.ForwardCompatibility) {
                                        ReportError(/*[XT_003]*/Res.Xslt_UnexpectedElementQ, input.QualifiedName, elementName);
                                    }
                                    input.SkipNode();
                                }
                            }
                        } else if (input.IsNs(input.Atoms.UrnMsxsl) && input.IsKeyword(input.Atoms.Script)) {
                            LoadScript(ctxInfo.nsList);
                        } else if (input.IsNullNamespace()) {
                            ReportError(/*[XT0130]*/Res.Xslt_NullNsAtTopLevel, input.LocalName);
                            input.SkipNode();
                        } else {
                            // Ignoring non-recognized namespace per XSLT spec 2.2
                            input.SkipNode();
                        }
                        atTop = isImport;
                        break;

                    case XPathNodeType.Whitespace:
                    case XPathNodeType.SignificantWhitespace:
                        break;
                    default:
                        Debug.Assert(input.NodeType == XPathNodeType.Text);
                        ReportError(/*[XT0120]*/Res.Xslt_TextNodesNotAllowed, input.Atoms.Stylesheet);
                        break;
                    }
                } while (input.MoveToNextSibling());
                input.MoveToParent();
            }
        }

        private void LoadImport() {
            string attHref;
            ContextInfo ctxInfo = input.GetAttributes(/*required:*/1, input.Atoms.Href, out attHref);
            CheckNoContent();

            if (attHref == null) {
                return;
            }

            // Resolve href right away using the current BaseUri (it might change later)
            Uri uri = ResolveUri(attHref, input.BaseUri);

            // Check for circular references
            if (documentUriInUse.Contains(uri.ToString())) {
                ReportError(/*[XT0210]*/Res.Xslt_CircularInclude, attHref);
            } else {
                curStylesheet.ImportHrefs.Add(uri);
            }
        }

        private void LoadInclude() {
            string attHref;
            ContextInfo ctxInfo = input.GetAttributes(/*required:*/1, input.Atoms.Href, out attHref);
            CheckNoContent();

            if (attHref == null) {
                return;
            }

            Uri uri = ResolveUri(attHref, input.BaseUri);

            // Check for circular references
            if (documentUriInUse.Contains(uri.ToString())) {
                ReportError(/*[XT0180]*/Res.Xslt_CircularInclude, attHref);
            } else {
                LoadStylesheet(uri, /*include:*/ true);
            }
        }

        private void LoadStripSpace(NsDecl stylesheetNsList) {
            string attElements;
            ContextInfo ctxInfo = input.GetAttributes(/*required:*/1, input.Atoms.Elements, out attElements);
            ctxInfo.nsList = MergeNamespaces(ctxInfo.nsList, stylesheetNsList);

            ParseWhitespaceRules(attElements, false);
            CheckNoContent();
        }

        private void LoadPreserveSpace(NsDecl stylesheetNsList) {
            string attElements;
            ContextInfo ctxInfo = input.GetAttributes(/*required:*/1, input.Atoms.Elements, out attElements);
            ctxInfo.nsList = MergeNamespaces(ctxInfo.nsList, stylesheetNsList);

            ParseWhitespaceRules(attElements, true);
            CheckNoContent();
        }

        private void LoadOutput() {
            string attMethod               ;
            string attVersion              ;
            string attEncoding             ;
            string attOmitXmlDeclaration   ;
            string attStandalone           ;
            string attDocTypePublic        ;
            string attDocTypeSystem        ;
            string attCDataSectionElements ;
            string attIndent               ;
            string attMediaType            ;

            ContextInfo ctxInfo = input.GetAttributes(/*required:*/0,
                input.Atoms.Method              , out attMethod              ,
                input.Atoms.Version             , out attVersion             ,
                input.Atoms.Encoding            , out attEncoding            ,
                input.Atoms.OmitXmlDeclaration  , out attOmitXmlDeclaration  ,
                input.Atoms.Standalone          , out attStandalone          ,
                input.Atoms.DocTypePublic       , out attDocTypePublic       ,
                input.Atoms.DocTypeSystem       , out attDocTypeSystem       ,
                input.Atoms.CDataSectionElements, out attCDataSectionElements,
                input.Atoms.Indent              , out attIndent              ,
                input.Atoms.MediaType           , out attMediaType
            );

            Output output = compiler.Output;
            XmlWriterSettings settings = output.Settings;
            int currentPrec = compiler.CurrentPrecedence;
            TriState triState;

            if (attMethod != null && currentPrec >= output.MethodPrec) {
                compiler.EnterForwardsCompatible();
                XmlOutputMethod   outputMethod;
                XmlQualifiedName  method = ParseOutputMethod(attMethod, out outputMethod);
                if (compiler.ExitForwardsCompatible(input.ForwardCompatibility) && method != null) {
                    if (currentPrec == output.MethodPrec && !output.Method.Equals(method)) {
                        ReportWarning(/*[XT1560]*/Res.Xslt_AttributeRedefinition, input.Atoms.Method);
                    }
                    settings.OutputMethod = outputMethod;
                    output.Method = method;
                    output.MethodPrec = currentPrec;
                }
            }

            if (attVersion != null && currentPrec >= output.VersionPrec) {
                if (currentPrec == output.VersionPrec && output.Version != attVersion) {
                    ReportWarning(/*[XT1560]*/Res.Xslt_AttributeRedefinition, input.Atoms.Version);
                }
                output.Version = attVersion;
                output.VersionPrec = currentPrec;
            }

            if (attEncoding != null && currentPrec >= output.EncodingPrec) {
                try {
                    // Encoding.GetEncoding() should never throw NotSupportedException, only ArgumentException
                    Encoding encoding = Encoding.GetEncoding(attEncoding);
                    if (currentPrec == output.EncodingPrec && output.Encoding != attEncoding) {
                        ReportWarning(/*[XT1560]*/Res.Xslt_AttributeRedefinition, input.Atoms.Encoding);
                    }
                    settings.Encoding = encoding;
                    output.Encoding = attEncoding;
                    output.EncodingPrec = currentPrec;
                }
                catch (ArgumentException) {
                    if (!input.ForwardCompatibility) {
                        ReportWarning(/*[XT_004]*/Res.Xslt_InvalidEncoding, attEncoding);
                    }
                }
            }

            if (attOmitXmlDeclaration != null && currentPrec >= output.OmitXmlDeclarationPrec) {
                triState = ParseYesNo(attOmitXmlDeclaration, input.Atoms.OmitXmlDeclaration);
                if (triState != TriState.Unknown) {
                    bool omitXmlDeclaration = (triState == TriState.True);
                    if (currentPrec == output.OmitXmlDeclarationPrec && settings.OmitXmlDeclaration != omitXmlDeclaration) {
                        ReportWarning(/*[XT1560]*/Res.Xslt_AttributeRedefinition, input.Atoms.OmitXmlDeclaration);
                    }
                    settings.OmitXmlDeclaration = omitXmlDeclaration;
                    output.OmitXmlDeclarationPrec = currentPrec;
                }
            }

            if (attStandalone != null && currentPrec >= output.StandalonePrec) {
                triState = ParseYesNo(attStandalone, input.Atoms.Standalone);
                if (triState != TriState.Unknown) {
                    XmlStandalone standalone = (triState == TriState.True) ? XmlStandalone.Yes : XmlStandalone.No;
                    if (currentPrec == output.StandalonePrec && settings.Standalone != standalone) {
                        ReportWarning(/*[XT1560]*/Res.Xslt_AttributeRedefinition, input.Atoms.Standalone);
                    }
                    settings.Standalone = standalone;
                    output.StandalonePrec = currentPrec;
                }
            }

            if (attDocTypePublic != null && currentPrec >= output.DocTypePublicPrec) {
                if (currentPrec == output.DocTypePublicPrec && settings.DocTypePublic != attDocTypePublic) {
                    ReportWarning(/*[XT1560]*/Res.Xslt_AttributeRedefinition, input.Atoms.DocTypePublic);
                }
                settings.DocTypePublic = attDocTypePublic;
                output.DocTypePublicPrec = currentPrec;
            }

            if (attDocTypeSystem != null && currentPrec >= output.DocTypeSystemPrec) {
                if (currentPrec == output.DocTypeSystemPrec && settings.DocTypeSystem != attDocTypeSystem) {
                    ReportWarning(/*[XT1560]*/Res.Xslt_AttributeRedefinition, input.Atoms.DocTypeSystem);
                }
                settings.DocTypeSystem = attDocTypeSystem;
                output.DocTypeSystemPrec = currentPrec;
            }

            if (attCDataSectionElements != null && attCDataSectionElements.Length != 0) {
                // Do not check the import precedence, the effective value is the union of all specified values
                compiler.EnterForwardsCompatible();
                string[] qnames = XmlConvert.SplitString(attCDataSectionElements);
                List<XmlQualifiedName> list = new List<XmlQualifiedName>();
                for (int i = 0; i < qnames.Length; i++) {
                    list.Add(ResolveQName(/*ignoreDefaultNs:*/false, qnames[i]));
                }
                if (compiler.ExitForwardsCompatible(input.ForwardCompatibility)) {
                    output.CDataSectionElements.AddRange(list);
                }
            }

            if (attIndent != null && currentPrec >= output.IndentPrec) {
                triState = ParseYesNo(attIndent, input.Atoms.Indent);
                if (triState != TriState.Unknown) {
                    bool indent = (triState == TriState.True);
                    if (currentPrec == output.IndentPrec && settings.Indent != indent) {
                        ReportWarning(/*[XT1560]*/Res.Xslt_AttributeRedefinition, input.Atoms.Indent);
                    }
                    settings.Indent = indent;
                    output.IndentPrec = currentPrec;
                }
            }

            if (attMediaType != null && currentPrec >= output.MediaTypePrec) {
                if (currentPrec == output.MediaTypePrec && settings.MediaType != attMediaType) {
                    ReportWarning(/*[XT1560]*/Res.Xslt_AttributeRedefinition, input.Atoms.MediaType);
                }
                settings.MediaType = attMediaType;
                output.MediaTypePrec = currentPrec;
            }

            CheckNoContent();
        }

        /*
            Default values for method="xml" :   version="1.0"   indent="no"     media-type="text/xml"
            Default values for method="html":   version="4.0"   indent="yes"    media-type="text/html"
            Default values for method="text":                                   media-type="text/plain"
        */
        private void ProcessOutputSettings() {
            Output output = compiler.Output;
            XmlWriterSettings settings    = output.Settings;
            settings.CDataSectionElements = output.CDataSectionElements;

            // version is ignored, indent="no" by default
            if (settings.OutputMethod == XmlOutputMethod.Html && output.IndentPrec == Output.NeverDeclaredPrec) {
                settings.Indent = true;
            }
            if (output.MediaTypePrec == Output.NeverDeclaredPrec) {
                settings.MediaType =
                    settings.OutputMethod == XmlOutputMethod.Xml  ? "text/xml" :
                    settings.OutputMethod == XmlOutputMethod.Html ? "text/html" :
                    settings.OutputMethod == XmlOutputMethod.Text ? "text/plain" : null;
            }
        }

        private void AttributeSetsDfs(AttributeSet attSet) {
            Debug.Assert(attSet != null);
            switch (attSet.CycleCheck) {
            case CycleCheck.NotStarted:
                attSet.CycleCheck = CycleCheck.Processing;
                foreach (QilName qname in attSet.UsedAttributeSets) {
                    AttributeSet usedAttSet;
                    if (!compiler.AttributeSets.TryGetValue(qname, out usedAttSet)) {
                        // Prevent reporting the same error twice. The error will be reported in QilGenerator
                        // while compiling this attribute set.
                        //Debug.Assert(attSet.Content[0].SourceLine != null);
                        //compiler.ReportError(/*[XT0710]*/attSet.Content[0].SourceLine, Res.Xslt_NoAttributeSet, qname.QualifiedName);
                    } else {
                        AttributeSetsDfs(usedAttSet);
                    }
                }
                attSet.CycleCheck = CycleCheck.Completed;
                break;
            case CycleCheck.Completed:
                break;
            default:
                Debug.Assert(attSet.CycleCheck == CycleCheck.Processing);
                Debug.Assert(attSet.Content[0].SourceLine != null);
                compiler.ReportError(/*[XT0720]*/attSet.Content[0].SourceLine, Res.Xslt_CircularAttributeSet, attSet.Name.QualifiedName);
                break;
            }
        }

        private void ProcessAttributeSets() {
            // Check attribute sets for circular references using dfs marking method
            foreach (AttributeSet attSet in compiler.AttributeSets.Values) {
                AttributeSetsDfs(attSet);
            }
        }

        private void LoadKey(NsDecl stylesheetNsList) {
            string attName ;
            string attMatch;
            string attUse  ;

            ContextInfo ctxInfo = input.GetAttributes(/*required:*/3,
                input.Atoms.Name , out attName,
                input.Atoms.Match, out attMatch,
                input.Atoms.Use  , out attUse
            );
            ctxInfo.nsList = MergeNamespaces(ctxInfo.nsList, stylesheetNsList);
            CheckNoContent();

            QilName keyName = CreateXPathQName(attName);
            Key key = (Key) SetInfo(f.Key(keyName, attMatch, attUse, input.XslVersion), null, ctxInfo);

            if (compiler.Keys.Contains(keyName)) {
                // Add to the list of previous definitions
                compiler.Keys[keyName].Add(key);
            } else {
                // First definition of key with that name
                List<Key> defList = new List<Key>();
                defList.Add(key);
                compiler.Keys.Add(defList);
            }
        }

        private void LoadDecimalFormat(NsDecl stylesheetNsList) {
            const int NumAttrs = 11, NumCharAttrs = 8, NumSignAttrs = 7;

            string[] attValues = new string[NumAttrs];
            string[] attNames  = new string[NumAttrs] {
                input.Atoms.DecimalSeparator ,
                input.Atoms.GroupingSeparator,
                input.Atoms.Percent          ,
                input.Atoms.PerMille         ,
                input.Atoms.ZeroDigit        ,
                input.Atoms.Digit            ,
                input.Atoms.PatternSeparator ,
                input.Atoms.MinusSign        ,
                input.Atoms.Infinity         ,
                input.Atoms.NaN              ,
                input.Atoms.Name             ,
            };

            ContextInfo ctxInfo = input.GetAttributes(/*required:*/0, NumAttrs, attNames, attValues);
            ctxInfo.nsList = MergeNamespaces(ctxInfo.nsList, stylesheetNsList);

            // Apply default values
            char[] DefaultValues = DecimalFormatDecl.Default.Characters;
            char[] characters = new char[NumCharAttrs];
            Debug.Assert(NumCharAttrs == DefaultValues.Length);
            int idx;

            for (idx = 0; idx < NumCharAttrs; idx++) {
                characters[idx] = ParseCharAttribute(attValues[idx], DefaultValues[idx], attNames[idx]);
            }

            string attInfinity = attValues[idx++];
            string attNaN      = attValues[idx++];
            string attName     = attValues[idx++];
            Debug.Assert(idx == NumAttrs);

            if (attInfinity == null) {
                attInfinity = DecimalFormatDecl.Default.InfinitySymbol;
            }
            if (attNaN == null) {
                attNaN = DecimalFormatDecl.Default.NanSymbol;
            }

            // Check all NumSignAttrs signs are distinct
            for (int i = 0; i < NumSignAttrs; i++) {
                for (int j = i+1; j < NumSignAttrs; j++) {
                    if (characters[i] == characters[j]) {
                        ReportError(/*[XT1300]*/Res.Xslt_DecimalFormatSignsNotDistinct, attNames[i], attNames[j]);
                        break;
                    }
                }
            }

            XmlQualifiedName name;
            if (attName == null) {
                // Use name="" for the default decimal-format
                name = new XmlQualifiedName();
            } else {
                compiler.EnterForwardsCompatible();
                name = ResolveQName(/*ignoreDefaultNs:*/true, attName);
                if (!compiler.ExitForwardsCompatible(input.ForwardCompatibility)) {
                    name = new XmlQualifiedName();
                }
            }

            if (compiler.DecimalFormats.Contains(name)) {
                // Check all attributes have the same values
                DecimalFormatDecl format = compiler.DecimalFormats[name];
                for (idx = 0; idx < NumCharAttrs; idx++) {
                    if (characters[idx] != format.Characters[idx]) {
                        ReportError(/*[XT1290]*/Res.Xslt_DecimalFormatRedefined, attNames[idx], char.ToString(characters[idx]));
                    }
                }
                if (attInfinity != format.InfinitySymbol) {
                    ReportError(/*[XT1290]*/Res.Xslt_DecimalFormatRedefined, attNames[idx], attInfinity);
                }
                idx++;
                if (attNaN != format.NanSymbol) {
                    ReportError(/*[XT1290]*/Res.Xslt_DecimalFormatRedefined, attNames[idx], attNaN);
                }
                idx++;
                Debug.Assert(name.Equals(format.Name));
                idx++;
                Debug.Assert(idx == NumAttrs);
            } else {
                // Add format to the global collection
                DecimalFormatDecl format = new DecimalFormatDecl(name, attInfinity, attNaN, new string(characters));
                compiler.DecimalFormats.Add(format);
            }
            CheckNoContent();
        }

        private void LoadNamespaceAlias(NsDecl stylesheetNsList) {
            string attStylesheetPrefix;
            string attResultPrefix    ;

            ContextInfo ctxInfo = input.GetAttributes(/*required:*/2,
                input.Atoms.StylesheetPrefix, out attStylesheetPrefix,
                input.Atoms.ResultPrefix    , out attResultPrefix
            );
            ctxInfo.nsList = MergeNamespaces(ctxInfo.nsList, stylesheetNsList);
            CheckNoContent();

            string stylesheetNsUri  = null;
            string resultNsUri      = null;

            if (attStylesheetPrefix == null) {
                // Attribute is missing
            } else if (attStylesheetPrefix.Length == 0) {
                ReportError(/*[XT_005]*/Res.Xslt_EmptyNsAlias, input.Atoms.StylesheetPrefix);
            } else {
                if (attStylesheetPrefix == "#default") {
                    attStylesheetPrefix = string.Empty;
                }
                stylesheetNsUri = input.LookupXmlNamespace(attStylesheetPrefix);
            }

            if (attResultPrefix == null) {
                // Attribute is missing
            } else if (attResultPrefix.Length == 0) {
                ReportError(/*[XT_005]*/Res.Xslt_EmptyNsAlias, input.Atoms.ResultPrefix);
            } else {
                if (attResultPrefix == "#default") {
                    attResultPrefix = string.Empty;
                }
                resultNsUri = input.LookupXmlNamespace(attResultPrefix);
            }

            if (stylesheetNsUri == null || resultNsUri == null) {
                // At least one of attributes is missing or invalid
                return;
            }
            if (compiler.SetNsAlias(stylesheetNsUri, resultNsUri, attResultPrefix, curStylesheet.ImportPrecedence)) {
                // Namespace alias redefinition
                ReportWarning(/*[XT0810]*/Res.Xslt_DupNsAlias, stylesheetNsUri);
            }
        }

        private void LoadAttributeSet(NsDecl stylesheetNsList) {
            string attName            ;
            string attUseAttributeSets;

            ContextInfo ctxInfo = input.GetAttributes(/*required:*/1,
                input.Atoms.Name            , out attName            ,
                input.Atoms.UseAttributeSets, out attUseAttributeSets
            );
            ctxInfo.nsList = MergeNamespaces(ctxInfo.nsList, stylesheetNsList);

            QilName attSetName = CreateXPathQName(attName);
            AttributeSet attSet;
            if (!curStylesheet.AttributeSets.TryGetValue(attSetName, out attSet)) {
                // First definition for attSetName within this stylesheet
                curStylesheet.AttributeSets[attSetName] = attSet = f.AttributeSet(attSetName);
                if (!compiler.AttributeSets.ContainsKey(attSetName)) {
                    // First definition for attSetName overall, adding it to the list here
                    // to ensure stable order of prototemplate functions in QilExpression
                    compiler.AllTemplates.Add(attSet);
                }
            }

            List<XslNode> content = ParseUseAttributeSets(attUseAttributeSets, ctxInfo.lineInfo);
            foreach (XslNode useAttSet in content) {
                Debug.Assert(useAttSet.NodeType == XslNodeType.UseAttributeSet);
                attSet.UsedAttributeSets.Add(useAttSet.Name);
            }

            /* Process children */
            if (input.MoveToFirstChild()) {
                do {
                    switch (input.NodeType) {
                    case XPathNodeType.Element:
                        // Only xsl:attribute's are allowed here
                        if (input.IsXsltNamespace() && input.IsKeyword(input.Atoms.Attribute)) {
                            AddInstruction(content, XslAttribute());
                        } else {
                            ReportError(/*[XT_006]*/Res.Xslt_UnexpectedElement, input.QualifiedName, input.Atoms.AttributeSet);
                            input.SkipNode();
                        }
                        break;
                    case XPathNodeType.Whitespace:
                    case XPathNodeType.SignificantWhitespace:
                        break;
                    default:
                        Debug.Assert(input.NodeType == XPathNodeType.Text);
                        ReportError(/*[XT_006]*/Res.Xslt_TextNodesNotAllowed, input.Atoms.AttributeSet);
                        break;
                    }
                } while (input.MoveToNextSibling());
                input.MoveToParent();
            }
            attSet.AddContent(SetInfo(f.List(), LoadEndTag(content), ctxInfo));
        }

        private void LoadGlobalVariableOrParameter(NsDecl stylesheetNsList, XslNodeType nodeType) {
            VarPar var = XslVarPar(nodeType);
            // Preserving namespaces to parse content later
            var.Namespaces = MergeNamespaces(var.Namespaces, stylesheetNsList);
            if (!curStylesheet.AddVarPar(var)) {
                ReportError(/*[XT0630]*/Res.Xslt_DupGlobalVariable, var.Name.QualifiedName);
            }
        }

        //: http://www.w3.org/TR/xslt#section-Defining-Template-Rules
        private void LoadTemplate(NsDecl stylesheetNsList) {
            Debug.Assert(curTemplate == null);
            string attMatch   ;
            string attName    ;
            string attPriority;
            string attMode    ;

            ContextInfo ctxInfo = input.GetAttributes(/*required:*/0,
                input.Atoms.Match   , out attMatch   ,
                input.Atoms.Name    , out attName    ,
                input.Atoms.Priority, out attPriority,
                input.Atoms.Mode    , out attMode
            );
            ctxInfo.nsList = MergeNamespaces(ctxInfo.nsList, stylesheetNsList);
            if (attMatch == null) {
                if (attName == null) {
                    ReportError(/*[XT_007]*/Res.Xslt_BothMatchNameAbsent);
                }
                if (attMode != null) {
                    ReportError(/*[XT_008]*/Res.Xslt_ModeWithoutMatch);
                    attMode = null;
                }
                if (attPriority != null) {
                    // In XSLT 2.0 this is an error
                    ReportWarning(/*[XT_008]*/Res.Xslt_PriorityWithoutMatch);
                }
            }

            QilName tmplName = null;
            if (attName != null) {
                compiler.EnterForwardsCompatible();
                tmplName = CreateXPathQName(attName);
                if (!compiler.ExitForwardsCompatible(input.ForwardCompatibility)) {
                    tmplName = null;
                }
            }

            double priority = double.NaN;
            if (attPriority != null) {
                priority = XPathConvert.StringToDouble(attPriority);
                if (double.IsNaN(priority) && !input.ForwardCompatibility) {
                    ReportError(/*[XT0530]*/Res.Xslt_InvalidAttrValue, input.Atoms.Priority, attPriority);
                }
            }

            curTemplate = f.Template(tmplName, attMatch, ParseMode(attMode), priority, input.XslVersion);

            // Template without match considered to not have mode and can't call xsl:apply-imports
            input.CanHaveApplyImports = (attMatch != null);

            SetInfo(curTemplate,
                LoadEndTag(LoadInstructions(InstructionFlags.AllowParam)), ctxInfo
            );

            if (!curStylesheet.AddTemplate(curTemplate)) {
                ReportError(/*[XT0660]*/Res.Xslt_DupTemplateName, curTemplate.Name.QualifiedName);
            }
            curTemplate = null;
        }

        private void LoadScript(NsDecl stylesheetNsList) {
            string attLanguage        ;
            string attImplementsPrefix;

            ContextInfo ctxInfo = input.GetAttributes(/*required:*/1,
                input.Atoms.ImplementsPrefix, out attImplementsPrefix,
                input.Atoms.Language        , out attLanguage
            );
            ctxInfo.nsList = MergeNamespaces(ctxInfo.nsList, stylesheetNsList);

            string scriptNs = null;
            if (attImplementsPrefix == null) {
                // Attribute is missing
            } else if (attImplementsPrefix.Length == 0) {
                ReportError(/*[XT_009]*/Res.Xslt_EmptyAttrValue, input.Atoms.ImplementsPrefix, attImplementsPrefix);
            } else {
                scriptNs = input.LookupXmlNamespace(attImplementsPrefix);
                if (scriptNs == XmlReservedNs.NsXslt) {
                    ReportError(/*[XT_036]*/Res.Xslt_ScriptXsltNamespace);
                    scriptNs = null;
                }
            }

            if (scriptNs == null) {
                scriptNs = compiler.CreatePhantomNamespace();
            }
            if (attLanguage == null) {
                attLanguage = "jscript";
            }

            if (! compiler.Settings.EnableScript) {
                compiler.Scripts.ScriptClasses[scriptNs] = null;
                input.SkipNode();
                return;
            }

            ScriptClass     scriptClass;
            StringBuilder   scriptCode  = new StringBuilder();
            string          uriString   = input.Uri;
            int             lineNumber  = 0;
            int             lastEndLine = 0;

            scriptClass = compiler.Scripts.GetScriptClass(scriptNs, attLanguage, (IErrorHelper)this);
            if (scriptClass == null) {
                input.SkipNode();
                return;
            }

            if (input.MoveToFirstChild()) {
                do {
                    switch (input.NodeType) {
                    case XPathNodeType.Text:
                        int startLine = input.StartLine;
                        int endLine   = input.EndLine;
                        if (scriptCode.Length == 0) {
                            lineNumber = startLine;
                        } else if (lastEndLine < startLine) {
                            scriptCode.Append('\n', startLine - lastEndLine);
                        }
                        scriptCode.Append(input.Value);
                        lastEndLine = endLine;
                        break;
                    case XPathNodeType.Element:
                        if (input.IsNs(input.Atoms.UrnMsxsl) && (input.IsKeyword(input.Atoms.Assembly) || input.IsKeyword(input.Atoms.Using))) {
                            if (scriptCode.Length != 0) {
                                ReportError(/*[XT_012]*/Res.Xslt_ScriptNotAtTop, input.QualifiedName);
                                input.SkipNode();
                            }
                            if (input.IsKeyword(input.Atoms.Assembly)) {
                                LoadMsAssembly(scriptClass);
                            } else if (input.IsKeyword(input.Atoms.Using)) {
                                LoadMsUsing(scriptClass);
                            }
                        } else {
                            ReportError(/*[XT_012]*/Res.Xslt_UnexpectedElementQ, input.QualifiedName, "msxsl:script");
                            input.SkipNode();
                        }
                        break;
                    default:
                        Debug.Assert(
                            input.NodeType == XPathNodeType.SignificantWhitespace ||
                            input.NodeType == XPathNodeType.Whitespace
                        );
                        // Skip leading whitespaces
                        if (scriptCode.Length != 0) {
                            goto case XPathNodeType.Text;
                        }
                        break;
                    }
                } while (input.MoveToNextSibling());
                input.MoveToParent();
            }

            if (scriptCode.Length == 0) {
                lineNumber = input.StartLine;
            }
            scriptClass.AddScriptBlock(scriptCode.ToString(), uriString, lineNumber, input.StartLine, input.StartPos);
        }

        private void LoadMsAssembly(ScriptClass scriptClass) {
            string attName, attHref;
            input.GetAttributes(/*required:*/0,
                input.Atoms.Name,   out attName,
                input.Atoms.Href,   out attHref
            );

            string asmLocation = null;

            if (attName != null) {
                if (attHref != null) {
                    ReportError(/*[XT_046]*/Res.Xslt_AssemblyBothNameHrefPresent);
                } else {
                    try {
                        asmLocation = Assembly.Load(attName).Location;
                    }
                    catch {
                        AssemblyName asmName = new AssemblyName(attName);

                        // If the assembly is simply named, let CodeDomProvider and Fusion resolve it
                        byte[] publicKeyToken = asmName.GetPublicKeyToken();
                        if ((publicKeyToken == null || publicKeyToken.Length == 0) && asmName.Version == null) {
                            asmLocation = asmName.Name + ".dll";
                        } else {
                            throw;
                        }
                    }
                }
            } else if (attHref != null) {
                asmLocation = Assembly.LoadFrom(ResolveUri(attHref, input.BaseUri).ToString()).Location;
            } else {
                ReportError(/*[XT_045]*/Res.Xslt_AssemblyBothNameHrefAbsent);
            }

            if (asmLocation != null) {
                scriptClass.refAssemblies.Add(asmLocation);
            }
            CheckNoContent();
        }

        private void LoadMsUsing(ScriptClass scriptClass) {
            string attNamespace;
            input.GetAttributes(/*required:*/1, input.Atoms.Namespace, out attNamespace);

            if (attNamespace != null) {
                scriptClass.nsImports.Add(attNamespace);
            }
            CheckNoContent();
        }


        private enum InstructionFlags {
            NoParamNoSort = 0x00,
            AllowParam    = 0x01,
            AllowSort     = 0x02,
        }

        private List<XslNode> LoadInstructions() {
            return LoadInstructions(new List<XslNode>(), InstructionFlags.NoParamNoSort);
        }

        private List<XslNode> LoadInstructions(InstructionFlags flags) {
            return LoadInstructions(new List<XslNode>(), flags);
        }

        private List<XslNode> LoadInstructions(List<XslNode> content) {
            return LoadInstructions(content, InstructionFlags.NoParamNoSort);
        }

        private List<XslNode> LoadInstructions(List<XslNode> content, InstructionFlags flags) {
            string parentName = input.QualifiedName;

            if (input.MoveToFirstChild()) {
                bool    atTop = true;
                XslNode result;

                do {
                    switch (input.NodeType) {
                    case XPathNodeType.Element:
                        string nspace = input.NamespaceUri;
                        string name   = input.LocalName;
                        if (nspace == input.Atoms.UriXsl) {
                            bool error = false;
                            if (Ref.Equal(name, input.Atoms.Param)) {
                                if ((flags & InstructionFlags.AllowParam) == 0) {
                                    ReportError(/*[XT_013]*/Res.Xslt_UnexpectedElementQ, input.QualifiedName, parentName);
                                    error = true;
                                } else if (!atTop) {
                                    // xsl:param's must precede any other children of xsl:template
                                    ReportError(/*[XT_014]*/Res.Xslt_NotAtTop, input.QualifiedName, parentName);
                                    error = true;
                                }
                            } else if (Ref.Equal(name, input.Atoms.Sort)) {
                                if ((flags & InstructionFlags.AllowSort) == 0) {
                                    ReportError(/*[XT_013]*/Res.Xslt_UnexpectedElementQ, input.QualifiedName, parentName);
                                    error = true;
                                } else if (!atTop) {
                                    // xsl:sort's must precede any other children of xsl:for-each
                                    ReportError(/*[XT_014]*/Res.Xslt_NotAtTop, input.QualifiedName, parentName);
                                    error = true;
                                }
                            } else {
                                atTop = false;
                            }
                            if (error) {
                                atTop = false;
                                input.SkipNode();
                                continue;
                            }
                            result = (
                                Ref.Equal(name, input.Atoms.ApplyImports         ) ? XslApplyImports() :
                                Ref.Equal(name, input.Atoms.ApplyTemplates       ) ? XslApplyTemplates() :
                                Ref.Equal(name, input.Atoms.CallTemplate         ) ? XslCallTemplate() :
                                Ref.Equal(name, input.Atoms.Copy                 ) ? XslCopy() :
                                Ref.Equal(name, input.Atoms.CopyOf               ) ? XslCopyOf() :
                                Ref.Equal(name, input.Atoms.Fallback             ) ? XslFallback() :
                                Ref.Equal(name, input.Atoms.If                   ) ? XslIf() :
                                Ref.Equal(name, input.Atoms.Choose               ) ? XslChoose() :
                                Ref.Equal(name, input.Atoms.ForEach              ) ? XslForEach() :
                                Ref.Equal(name, input.Atoms.Message              ) ? XslMessage() :
                                Ref.Equal(name, input.Atoms.Number               ) ? XslNumber() :
                                Ref.Equal(name, input.Atoms.ValueOf              ) ? XslValueOf() :
                                Ref.Equal(name, input.Atoms.Comment              ) ? XslComment() :
                                Ref.Equal(name, input.Atoms.ProcessingInstruction) ? XslProcessingInstruction() :
                                Ref.Equal(name, input.Atoms.Text                 ) ? XslText() :
                                Ref.Equal(name, input.Atoms.Element              ) ? XslElement() :
                                Ref.Equal(name, input.Atoms.Attribute            ) ? XslAttribute() :
                                Ref.Equal(name, input.Atoms.Variable             ) ? XslVarPar(XslNodeType.Variable) :
                                Ref.Equal(name, input.Atoms.Param                ) ? XslVarPar(XslNodeType.Param) :
                                Ref.Equal(name, input.Atoms.Sort                 ) ? XslSort() :
                                /*default:*/                                         LoadUnknownXsltInstruction(parentName)
                            );
                        } else {
                            atTop = false;
                            result = LoadLiteralResultElement(/*asStylesheet:*/false);
                        }
                        break;
                    case XPathNodeType.SignificantWhitespace:
                        result = SetLineInfo(f.Text(input.Value), input.BuildLineInfo());
                        break;
                    case XPathNodeType.Whitespace:
                        continue;
                    default:
                        Debug.Assert(input.NodeType == XPathNodeType.Text);
                        atTop = false;
                        goto case XPathNodeType.SignificantWhitespace;
                    }
                    AddInstruction(content, result);
                } while (input.MoveToNextSibling());
                input.MoveToParent();
            }
            return content;
        }

        // http://www.w3.org/TR/xslt#apply-imports
        private XslNode XslApplyImports() {
            ContextInfo ctxInfo = input.GetAttributes();
            if (!input.CanHaveApplyImports) {
                ReportError(/*[XT_015]*/Res.Xslt_InvalidApplyImports);
                input.SkipNode();
                return null;
            }

            CheckNoContent();
            return SetInfo(f.ApplyImports(/*Mode:*/curTemplate.Mode, curStylesheet, input.XslVersion), null, ctxInfo);
        }

        // http://www.w3.org/TR/xslt#section-Applying-Template-Rules
        private XslNode XslApplyTemplates() {
            string attSelect;
            string attMode  ;
            ContextInfo ctxInfo = input.GetAttributes(/*required:*/0,
                input.Atoms.Select, out attSelect,
                input.Atoms.Mode  , out attMode
            );
            if (attSelect == null) {
                attSelect = "node()";
            }
            QilName mode = ParseMode(attMode);

            List<XslNode> content = new List<XslNode>();
            /* Process children */
            if (input.MoveToFirstChild()) {
                do {
                    switch (input.NodeType) {
                    case XPathNodeType.Element:
                        if (input.IsXsltNamespace()) {
                            if (input.IsKeyword(input.Atoms.WithParam)) {
                                XslNode withParam = XslVarPar(XslNodeType.WithParam);
                                CheckWithParam(content, withParam);
                                AddInstruction(content, withParam);
                                break;
                            } else if (input.IsKeyword(input.Atoms.Sort)) {
                                AddInstruction(content, XslSort());
                                break;
                            }
                        }
                        ReportError(/*[XT_016]*/Res.Xslt_UnexpectedElement, input.QualifiedName, input.Atoms.ApplyTemplates);
                        input.SkipNode();
                        break;
                    case XPathNodeType.Whitespace:
                    case XPathNodeType.SignificantWhitespace:
                        break;
                    default:
                        Debug.Assert(input.NodeType == XPathNodeType.Text);
                        ReportError(/*[XT_016]*/Res.Xslt_TextNodesNotAllowed, input.Atoms.ApplyTemplates);
                        break;
                    }
                } while (input.MoveToNextSibling());
                input.MoveToParent();
            }

            ctxInfo.SaveExtendedLineInfo(input);
            return SetInfo(f.ApplyTemplates(mode, attSelect, ctxInfo, input.XslVersion),
                content, ctxInfo
            );
        }

        // http://www.w3.org/TR/xslt#named-templates
        private XslNode XslCallTemplate() {
            string attName;
            ContextInfo ctxInfo = input.GetAttributes(/*required:*/1, input.Atoms.Name, out attName);

            List<XslNode> content = new List<XslNode>();
            /* Process children */
            if (input.MoveToFirstChild()) {
                do {
                    switch (input.NodeType) {
                    case XPathNodeType.Element:
                        if (input.IsXsltNamespace() && input.IsKeyword(input.Atoms.WithParam)) {
                            XslNode withParam = XslVarPar(XslNodeType.WithParam);
                            CheckWithParam(content, withParam);
                            AddInstruction(content, withParam);
                        } else {
                            ReportError(/*[XT_017]*/Res.Xslt_UnexpectedElement, input.QualifiedName, input.Atoms.CallTemplate);
                            input.SkipNode();
                        }
                        break;
                    case XPathNodeType.Whitespace:
                    case XPathNodeType.SignificantWhitespace:
                        break;
                    default:
                        Debug.Assert(input.NodeType == XPathNodeType.Text);
                        ReportError(/*[XT_017]*/Res.Xslt_TextNodesNotAllowed, input.Atoms.CallTemplate);
                        break;
                    }
                } while (input.MoveToNextSibling());
                input.MoveToParent();
            }

            ctxInfo.SaveExtendedLineInfo(input);
            return SetInfo(f.CallTemplate(CreateXPathQName(attName), ctxInfo),
                content, ctxInfo
            );
        }

        private XslNode XslCopy() {
            string attUseAttributeSets;
            ContextInfo ctxInfo = input.GetAttributes(/*required:*/0, input.Atoms.UseAttributeSets, out attUseAttributeSets);

            List<XslNode> content = ParseUseAttributeSets(attUseAttributeSets, ctxInfo.lineInfo);
            return SetInfo(f.Copy(), LoadEndTag(LoadInstructions(content)), ctxInfo);
        }

        private XslNode XslCopyOf() {
            string attSelect;
            ContextInfo ctxInfo = input.GetAttributes(/*required:*/1, input.Atoms.Select, out attSelect);

            CheckNoContent();
            return SetInfo(f.CopyOf(attSelect, input.XslVersion), null, ctxInfo);
        }

        // http://www.w3.org/TR/xslt#fallback
        // See LoadFallbacks() for real fallback implementation
        private XslNode XslFallback() {
            input.GetAttributes();
            input.SkipNode();
            return null;
        }

        private XslNode XslIf() {
            string attTest;
            ContextInfo ctxInfo = input.GetAttributes(/*required:*/1, input.Atoms.Test, out attTest);

            return SetInfo(f.If(attTest, input.XslVersion), LoadInstructions(), ctxInfo);
        }

        private XslNode XslChoose() {
            ContextInfo ctxInfo = input.GetAttributes();

            List<XslNode> content   = new List<XslNode>();
            bool        otherwise = false;
            bool        when      = false;

            if (input.MoveToFirstChild()) {
                do {
                    switch (input.NodeType) {
                    case XPathNodeType.Element:
                        XslNode node = null;
                        if (Ref.Equal(input.NamespaceUri, input.Atoms.UriXsl)) {
                            if (Ref.Equal(input.LocalName, input.Atoms.When)) {
                                if (otherwise) {
                                    ReportError(/*[XT_018]*/Res.Xslt_WhenAfterOtherwise);
                                    input.SkipNode();
                                    continue;
                                } else {
                                    when = true;
                                    node = XslIf();
                                }
                            } else if (Ref.Equal(input.LocalName, input.Atoms.Otherwise)) {
                                if (otherwise) {
                                    ReportError(/*[XT_019]*/Res.Xslt_DupOtherwise);
                                    input.SkipNode();
                                    continue;
                                } else {
                                    otherwise = true;
                                    node = XslOtherwise();
                                }
                            }
                        }
                        if (node == null) {
                            ReportError(/*[XT_020]*/Res.Xslt_UnexpectedElement, input.QualifiedName, input.Atoms.Choose);
                            input.SkipNode();
                            continue;
                        }
                        AddInstruction(content, node);
                        break;
                    case XPathNodeType.Whitespace:
                    case XPathNodeType.SignificantWhitespace:
                        break;
                    default:
                        Debug.Assert(input.NodeType == XPathNodeType.Text);
                        ReportError(/*[XT_020]*/Res.Xslt_TextNodesNotAllowed, input.Atoms.Choose);
                        break;
                    }
                } while (input.MoveToNextSibling());
                input.MoveToParent();
            }
            if (!when) {
                ReportError(/*[XT_021]*/Res.Xslt_NoWhen);
            }
            return SetInfo(f.Choose(), content, ctxInfo);
        }

        private XslNode XslOtherwise() {
            ContextInfo ctxInfo = input.GetAttributes();
            return SetInfo(f.Otherwise(), LoadInstructions(), ctxInfo);
        }

        private XslNode XslForEach() {
            string attSelect;
            ContextInfo ctxInfo = input.GetAttributes(/*required:*/1, input.Atoms.Select, out attSelect);

            // The current template rule becomes null, so we must not allow xsl:apply-import's within this element
            input.CanHaveApplyImports = false;
            List<XslNode> content = LoadInstructions(InstructionFlags.AllowSort);
            ctxInfo.SaveExtendedLineInfo(input);

            return SetInfo(f.ForEach(attSelect, ctxInfo, input.XslVersion),
                content, ctxInfo
            );
        }

        // http://www.w3.org/TR/xslt#message
        private XslNode XslMessage() {
            string attTerminate;
            ContextInfo ctxInfo = input.GetAttributes(/*required:*/0, input.Atoms.Terminate, out attTerminate);

            bool terminate = ParseYesNo(attTerminate, /*attName:*/input.Atoms.Terminate) == TriState.True;
            return SetInfo(f.Message(terminate), LoadEndTag(LoadInstructions()), ctxInfo);
        }

        // http://www.w3.org/TR/xslt#number
        private XslNode XslNumber() {
            string attLevel            ;
            string attCount            ;
            string attFrom             ;
            string attValue            ;
            string attFormat           ;
            string attLang             ;
            string attLetterValue      ;
            string attGroupingSeparator;
            string attGroupingSize     ;

            ContextInfo ctxInfo = input.GetAttributes(/*required:*/0,
                input.Atoms.Level            , out attLevel            ,
                input.Atoms.Count            , out attCount            ,
                input.Atoms.From             , out attFrom             ,
                input.Atoms.Value            , out attValue            ,
                input.Atoms.Format           , out attFormat           ,
                input.Atoms.Lang             , out attLang             ,
                input.Atoms.LetterValue      , out attLetterValue      ,
                input.Atoms.GroupingSeparator, out attGroupingSeparator,
                input.Atoms.GroupingSize     , out attGroupingSize
            );

            // Default values for xsl:number :  level="single"  format="1"
            NumberLevel level;

            switch (attLevel) {
            case "single"  :  level = NumberLevel.Single  ; break;
            case "multiple":  level = NumberLevel.Multiple; break;
            case "any"     :  level = NumberLevel.Any     ; break;
            default:
                if (attLevel != null && !input.ForwardCompatibility) {
                    ReportError(/*[XT_022]*/Res.Xslt_InvalidAttrValue, input.Atoms.Level, attLevel);
                }
                goto case "single";
            }

            if (attFormat == null) {
                attFormat = "1";
            }

            CheckNoContent();
            return SetInfo(
                f.Number(level, attCount, attFrom, attValue,
                    attFormat, attLang, attLetterValue, attGroupingSeparator, attGroupingSize,
                    input.XslVersion
                ),
                null, ctxInfo
            );
        }

        // http://www.w3.org/TR/xslt#value-of
        private XslNode XslValueOf() {
            string attSelect               ;
            string attDisableOutputEscaping;
            ContextInfo ctxInfo = input.GetAttributes(/*required:*/1,
                input.Atoms.Select               , out attSelect,
                input.Atoms.DisableOutputEscaping, out attDisableOutputEscaping
            );

            bool doe = ParseYesNo(attDisableOutputEscaping, /*attName:*/ input.Atoms.DisableOutputEscaping) == TriState.True;

            CheckNoContent();
            return SetInfo(f.XslNode(doe ? XslNodeType.ValueOfDoe : XslNodeType.ValueOf, null, attSelect, input.XslVersion),
                null, ctxInfo
            );
        }

        // xsl:variable     http://www.w3.org/TR/xslt#local-variables
        // xsl:param        http://www.w3.org/TR/xslt#element-param
        // xsl:with-param   http://www.w3.org/TR/xslt#element-with-param
        private VarPar XslVarPar(XslNodeType nodeType) {
            Debug.Assert(
                nodeType == XslNodeType.Variable  && input.LocalName == input.Atoms.Variable  ||
                nodeType == XslNodeType.Param     && input.LocalName == input.Atoms.Param     ||
                nodeType == XslNodeType.WithParam && input.LocalName == input.Atoms.WithParam
            );
            string attName  ;
            string attSelect;
            ContextInfo ctxInfo = input.GetAttributes(/*required:*/1,
                input.Atoms.Name  , out attName,
                input.Atoms.Select, out attSelect
            );

            List<XslNode> content = LoadInstructions();
            // Load the end tag only if the content is not empty
            if (content.Count != 0) {
                content = LoadEndTag(content);
            }

            if (attSelect != null && content.Count != 0) {
                ReportError(/*[XT0620]*/Res.Xslt_VariableCntSel2, attName);
            }

            VarPar result = f.VarPar(nodeType, CreateXPathQName(attName), attSelect, input.XslVersion);
            SetInfo(result, content, ctxInfo);
            return result;
        }

        // http://www.w3.org/TR/xslt#section-Creating-Comments
        private XslNode XslComment() {
            ContextInfo ctxInfo = input.GetAttributes();
            return SetInfo(f.Comment(),
                LoadEndTag(LoadInstructions()), ctxInfo
            );
        }

        // http://www.w3.org/TR/xslt#section-Creating-Processing-Instructions
        private XslNode XslProcessingInstruction() {
            string attName;
            ContextInfo ctxInfo = input.GetAttributes(/*required:*/1, input.Atoms.Name, out attName);
            if (attName == null) {
                attName = compiler.PhantomNCName;
            }
            return SetInfo(f.PI(attName, input.XslVersion),
                LoadEndTag(LoadInstructions()), ctxInfo
            );
        }

        // http://www.w3.org/TR/xslt#section-Creating-Text
        private XslNode XslText() {
            string attDisableOutputEscaping;
            ContextInfo ctxInfo = input.GetAttributes(/*required:*/0,input.Atoms.DisableOutputEscaping, out attDisableOutputEscaping);

            bool doe = ParseYesNo(attDisableOutputEscaping, /*attName:*/ input.Atoms.DisableOutputEscaping) == TriState.True;
            SerializationHints hints = doe ? SerializationHints.DisableOutputEscaping : SerializationHints.None;

            // We are not using StringBuilder here because in most cases there will be just one text node.
            List<XslNode> content = new List<XslNode>();

            // xsl:text may contain multiple child text nodes separated by comments and PIs, which are ignored by XsltInput
            if (input.MoveToFirstChild()) {
                do {
                    switch (input.NodeType) {
                    case XPathNodeType.Text:
                    case XPathNodeType.Whitespace:
                    case XPathNodeType.SignificantWhitespace:
                        content.Add(f.Text(input.Value, hints));
                        break;
                    default:
                        Debug.Assert(input.NodeType == XPathNodeType.Element);
                        ReportError(/*[XT_023]*/Res.Xslt_UnexpectedElement, input.QualifiedName, input.Atoms.Text);
                        input.SkipNode();
                        break;
                    }
                } while (input.MoveToNextSibling());
                input.MoveToParent();
            }

            // Empty xsl:text elements will be ignored
            return SetInfo(f.List(), content, ctxInfo);
        }

        // http://www.w3.org/TR/xslt#section-Creating-Elements-with-xsl:element
        private XslNode XslElement() {
            string attName            ;
            string attNamespace       ;
            string attUseAttributeSets;
            ContextInfo ctxInfo = input.GetAttributes(/*required:*/1,
                input.Atoms.Name            , out attName            ,
                input.Atoms.Namespace       , out attNamespace       ,
                input.Atoms.UseAttributeSets, out attUseAttributeSets
            );

            if (attName == null) {
                attName = compiler.PhantomNCName;
            }
            if (attNamespace == XmlReservedNs.NsXmlNs) {
                ReportError(/*[XT_024]*/Res.Xslt_ReservedNS, attNamespace);
            }
            List<XslNode> content = ParseUseAttributeSets(attUseAttributeSets, ctxInfo.lineInfo);
            return SetInfo(f.Element(attName, attNamespace, input.XslVersion),
                LoadEndTag(LoadInstructions(content)), ctxInfo
            );
        }

        // http://www.w3.org/TR/xslt#creating-attributes
        private XslNode XslAttribute() {
            string attName     ;
            string attNamespace;
            ContextInfo ctxInfo = input.GetAttributes(/*required:*/1,
                input.Atoms.Name     , out attName     ,
                input.Atoms.Namespace, out attNamespace
            );

            if (attName == null) {
                attName = compiler.PhantomNCName;
            }
            if (attNamespace == XmlReservedNs.NsXmlNs) {
                ReportError(/*[XT_024]*/Res.Xslt_ReservedNS, attNamespace);
            }
            return SetInfo(f.Attribute(attName, attNamespace, input.XslVersion),
                LoadEndTag(LoadInstructions()), ctxInfo
            );
        }

        // http://www.w3.org/TR/xslt#sorting
        private XslNode XslSort() {
            string attSelect   ;
            string attLang     ;
            string attDataType ;
            string attOrder    ;
            string attCaseOrder;
            ContextInfo ctxInfo = input.GetAttributes(/*required:*/0,
                input.Atoms.Select   , out attSelect   ,
                input.Atoms.Lang     , out attLang     ,
                input.Atoms.DataType , out attDataType ,
                input.Atoms.Order    , out attOrder    ,
                input.Atoms.CaseOrder, out attCaseOrder
            );

            if (attSelect == null) {
                attSelect = ".";
            }
            CheckNoContent();
            return SetInfo(f.Sort(attSelect, attLang, attDataType, attOrder, attCaseOrder, input.XslVersion),
                null, ctxInfo
            );
        }

        // http://www.w3.org/TR/xslt#literal-result-element
        private XslNode LoadLiteralResultElement(bool asStylesheet) {
            Debug.Assert(input.NodeType == XPathNodeType.Element);
            string prefix   = input.Prefix;
            string name     = input.LocalName;
            string nsUri    = input.NamespaceUri;

            string version      = null;
            string extPrefixes  = null;
            string exclPrefixes = null;
            string useAttrSets  = null;
            string versionQName = null;

            List<XslNode> content = new List<XslNode>();
            ContextInfo ctxInfo = new ContextInfo(input);

            /* Process literal attributes */
            while (input.MoveToNextAttOrNs()) {
                if (input.NodeType == XPathNodeType.Namespace) {
                    ctxInfo.AddNamespace(input);
                } else {
                    Debug.Assert(input.NodeType == XPathNodeType.Attribute);
                    ctxInfo.AddAttribute(input);
                    if (input.IsXsltNamespace()) {
                        if (input.LocalName == input.Atoms.Version) {
                            version = input.Value;
                            versionQName = input.QualifiedName;
                        } else if (input.LocalName == input.Atoms.ExtensionElementPrefixes) {
                            extPrefixes = input.Value;
                        } else if (input.LocalName == input.Atoms.ExcludeResultPrefixes) {
                            exclPrefixes = input.Value;
                        } else if (input.LocalName == input.Atoms.UseAttributeSets) {
                            useAttrSets = input.Value;
                        } else {
                            // just ignore it
                        }
                    } else {
                        XslNode att = f.LiteralAttribute(f.QName(input.LocalName, input.NamespaceUri, input.Prefix), input.Value, input.XslVersion);
                        // QilGenerator takes care of AVTs, and needs line info
                        AddInstruction(content, SetLineInfo(att, ctxInfo.lineInfo));
                    }
                }
            }
            ctxInfo.Finish(input);

            if (version != null) {
                // Enable forwards-compatible behavior if version attribute is not "1.0"
                input.SetVersion(version, versionQName);
            } else {
                if (asStylesheet) {
                    if (Ref.Equal(nsUri, input.Atoms.UriWdXsl) && Ref.Equal(name, input.Atoms.Stylesheet)) {
                        ReportError(/*[XT_025]*/Res.Xslt_WdXslNamespace);
                    } else {
                        ReportError(/*[XT0150]*/Res.Xslt_WrongStylesheetElement);
                    }
                    input.SkipNode();
                    return null;
                }
            }

            // Parse xsl:extension-element-prefixes attribute (now that forwards-compatible mode is known)
            InsertExNamespaces(extPrefixes, ref ctxInfo.nsList, /*extensions:*/true);

            XslNode result;

            // Now we can determine whether this element is an extension element (spec section 14.1)
            if (input.IsExtensionNamespace(nsUri)) {
                // This is not a literal result element, so drop all attributes we have collected
                content = LoadFallbacks(name);
                result = f.List();
            } else {
                // Parse xsl:exclude-result-prefixes attribute (now that it's known this is a literal result element)
                InsertExNamespaces(exclPrefixes, ref ctxInfo.nsList, /*extensions:*/false);

                // Insert all attribute sets at the beginning of content
                content.InsertRange(0, ParseUseAttributeSets(useAttrSets, ctxInfo.lineInfo));

                content = LoadEndTag(LoadInstructions(content));
                result = f.LiteralElement(f.QName(name, nsUri, prefix));
            }

            return SetInfo(result, content, ctxInfo);
        }

        private void CheckWithParam(List<XslNode> content, XslNode withParam) {
            Debug.Assert(content != null && withParam != null);
            Debug.Assert(withParam.NodeType == XslNodeType.WithParam);
            foreach (XslNode node in content) {
                if (node.NodeType == XslNodeType.WithParam && node.Name.Equals(withParam.Name)) {
                    ReportError(/*[XT0670]*/Res.Xslt_DuplicateWithParam, withParam.Name.QualifiedName);
                    break;
                }
            }
        }

        private static void AddInstruction(List<XslNode> content, XslNode instruction) {
            Debug.Assert(content != null);
            if (instruction != null) {
                content.Add(instruction);
            }
        }

        private List<XslNode> LoadEndTag(List<XslNode> content) {
            Debug.Assert(content != null);
            if (compiler.IsDebug && !input.IsEmptyElement) {
                AddInstruction(content, SetLineInfo(f.Nop(), input.BuildLineInfo()));
            }
            return content;
        }

        private XslNode LoadUnknownXsltInstruction(string parentName) {
            if (!input.ForwardCompatibility) {
                ReportError(/*[XT_026]*/Res.Xslt_UnexpectedElementQ, input.QualifiedName, parentName);
                input.SkipNode();
                return null;
            } else {
                ContextInfo ctxInfo = input.GetAttributes();
                List<XslNode> fallbacks = LoadFallbacks(input.LocalName);
                return SetInfo(f.List(), fallbacks, ctxInfo);
            }
        }

        private List<XslNode> LoadFallbacks(string instrName) {
            List<XslNode> fallbacksArray = new List<XslNode>();
            /* Process children */
            if (input.MoveToFirstChild()) {
                do {
                    if (
                        Ref.Equal(input.NamespaceUri, input.Atoms.UriXsl  ) &&
                        Ref.Equal(input.LocalName   , input.Atoms.Fallback)
                    ) {
                        ContextInfo ctxInfo = input.GetAttributes();
                        fallbacksArray.Add(SetInfo(f.List(), LoadInstructions(), ctxInfo));
                    } else {
                        input.SkipNode();
                    }
                } while (input.MoveToNextSibling());
                input.MoveToParent();
            }

            // Generate runtime error if there is no fallbacks
            if (fallbacksArray.Count == 0) {
                fallbacksArray.Add(
                    f.Error(XslLoadException.CreateMessage(input.BuildLineInfo(), Res.Xslt_UnknownExtensionElement, instrName))
                );
            }
            return fallbacksArray;
        }

        // ------------------ little helper methods ---------------------

        // Suppresses errors if FCB is enabled
        private QilName ParseMode(string qname) {
            if (qname == null) {
                return nullMode;
            }
            // mode is always optional attribute
            compiler.EnterForwardsCompatible();
            QilName mode = CreateXPathQName(qname);
            if (!compiler.ExitForwardsCompatible(input.ForwardCompatibility)) {
                mode = nullMode;
            }
            return mode;
        }

        // Does not suppress errors
        private void ResolveQName(bool ignoreDefaultNs, string qname, out string localName, out string namespaceName, out string prefix) {
            if (qname == null) {
                // That means stylesheet is incorrect
                prefix = compiler.PhantomNCName;
                localName = compiler.PhantomNCName;
                namespaceName = compiler.CreatePhantomNamespace();
                return;
            }
            if (!compiler.ParseQName(qname, out prefix, out localName, (IErrorHelper)this)) {
                namespaceName = compiler.CreatePhantomNamespace();
                return;
            }
            if (ignoreDefaultNs && prefix.Length == 0) {
                namespaceName = string.Empty;
            } else {
                namespaceName = input.LookupXmlNamespace(prefix);
                if (namespaceName == null) {
                    namespaceName = compiler.CreatePhantomNamespace();
                }
            }
        }

        // Does not suppress errors
        private QilName CreateXPathQName(string qname) {
            string prefix, localName, namespaceName;
            ResolveQName(/*ignoreDefaultNs:*/true, qname, out localName, out namespaceName, out prefix);
            return f.QName(localName, namespaceName, prefix);
        }

        // Does not suppress errors
        private XmlQualifiedName ResolveQName(bool ignoreDefaultNs, string qname) {
            string prefix, localName, namespaceName;
            ResolveQName(ignoreDefaultNs, qname, out localName, out namespaceName, out prefix);
            return new XmlQualifiedName(localName, namespaceName);
        }

        // Does not suppress errors
        private void ParseWhitespaceRules(string elements, bool preserveSpace) {
            if (elements != null && elements.Length != 0) {
                string[] tokens = XmlConvert.SplitString(elements);
                for (int i = 0; i < tokens.Length; i++) {
                    string prefix, localName, namespaceName;
                    if (!compiler.ParseNameTest(tokens[i], out prefix, out localName, (IErrorHelper)this)) {
                        namespaceName = compiler.CreatePhantomNamespace();
                    } else if (prefix == null || prefix.Length == 0) {
                        namespaceName = prefix;
                    } else {
                        namespaceName = input.LookupXmlNamespace(prefix);
                        if (namespaceName == null) {
                            namespaceName = compiler.CreatePhantomNamespace();
                        }
                    }
                    int index = (
                        (localName     == null ? 1 : 0) +
                        (namespaceName == null ? 1 : 0)
                    );
                    curStylesheet.AddWhitespaceRule(index, new WhitespaceRule(localName, namespaceName, preserveSpace));
                }
            }
        }

        // Does not suppress errors.  In case of error, null is returned.
        private XmlQualifiedName ParseOutputMethod(string attValue, out XmlOutputMethod method) {
            string prefix, localName, namespaceName;
            ResolveQName(/*ignoreDefaultNs:*/true, attValue, out localName, out namespaceName, out prefix);
            method = XmlOutputMethod.AutoDetect;

            if (compiler.IsPhantomNamespace(namespaceName)) {
                return null;
            } else if (prefix.Length == 0) {
                switch (localName) {
                case "xml"  : method = XmlOutputMethod.Xml;  break;
                case "html" : method = XmlOutputMethod.Html; break;
                case "text" : method = XmlOutputMethod.Text; break;
                default:
                    ReportError(/*[XT1570]*/Res.Xslt_InvalidAttrValue, input.Atoms.Method, attValue);
                    return null;
                }
            } else {
                if (!input.ForwardCompatibility) {
                    ReportWarning(/*[XT1570]*/Res.Xslt_InvalidMethod, attValue);
                }
            }
            return new XmlQualifiedName(localName, namespaceName);
        }

        // Suppresses errors if FCB is enabled
        private List<XslNode> ParseUseAttributeSets(string useAttributeSets, ISourceLineInfo lineInfo) {
            List<XslNode> result = new List<XslNode>();
            if (useAttributeSets != null && useAttributeSets.Length != 0) {
                compiler.EnterForwardsCompatible();

                string[] qnames = XmlConvert.SplitString(useAttributeSets);
                for (int idx = 0; idx < qnames.Length; idx++) {
                    AddInstruction(result, SetLineInfo(f.UseAttributeSet(CreateXPathQName(qnames[idx])), lineInfo));
                }

                if (!compiler.ExitForwardsCompatible(input.ForwardCompatibility)) {
                    // There were errors in the list, ignore the whole list
                    result = new List<XslNode>();
                }
            }
            return result;
        }

        // Suppresses errors if FCB is enabled
        private TriState ParseYesNo(string val, string attName) {
            switch (val) {
            case null : return TriState.Unknown;
            case "yes": return TriState.True;
            case "no" : return TriState.False;
            default:
                if (!input.ForwardCompatibility) {
                    ReportError(/*[XT_028]*/Res.Xslt_BistateAttribute, attName, "yes", "no");
                }
                return TriState.Unknown;
            }
        }

        // Suppresses errors if FCB is enabled
        private char ParseCharAttribute(string attValue, char defaultValue, string attName) {
            if (attValue == null) {
                return defaultValue;
            } else if (attValue.Length != 1) {
                if (!input.ForwardCompatibility) {
                    ReportError(/*[XT_029]*/Res.Xslt_CharAttribute, attName);
                }
                return defaultValue;
            }
            return attValue[0];
        }

        private void CheckNoContent() {
            string  elementName = input.QualifiedName;
            bool    error       = false;

            // Really EMPTY means no content at all, but for the sake of compatibility with MSXML we allow whitespaces
            if (input.MoveToFirstChild()) {
                do {
                    // NOTE: XPathNodeType.SignificantWhitespace are not allowed here
                    if (input.NodeType != XPathNodeType.Whitespace) {
                        if (!error) {
                            ReportError(/*[XT0260]*/Res.Xslt_NotEmptyContents, elementName);
                            error = true;
                        }
                        input.SkipNode();
                    }
                } while (input.MoveToNextSibling());
                input.MoveToParent();
            }
        }

        private static XslNode SetLineInfo(XslNode node, ISourceLineInfo lineInfo) {
            Debug.Assert(node != null);
            node.SourceLine = lineInfo;
            return node;
        }

        private static void SetContent(XslNode node, List<XslNode> content) {
            Debug.Assert(node != null);
            if (content != null && content.Count == 0) {
                content = null; // Actualy we can reuse this ArayList.
            }
            node.SetContent(content);
        }

        private static XslNode SetInfo(XslNode to, List<XslNode> content, ContextInfo info) {
            Debug.Assert(to != null);
            to.Namespaces = info.nsList;
            SetContent(to, content);
            SetLineInfo(to, info.lineInfo);
            return to;
        }

        // NOTE! We inverting namespace order that is irelevant for namespace of the same node, but
        // for included styleseets we don't keep stylesheet as a node and adding it's namespaces to
        // each toplevel element by MergeNamespaces().
        // Namespaces of stylesheet can be overriden in template and to make this works correclety we
        // should attache them after NsDec of top level elements.
        // Toplevel element almost never contais NsDecl and in practice node duplication will not happened, but if they have
        // we should copy NsDecls of stylesheet localy in toplevel elements.
        private static NsDecl MergeNamespaces(NsDecl thisList, NsDecl parentList) {
            if (parentList == null) {
                return thisList;
            }
            if (thisList == null) {
                return parentList;
            }
            // Clone all nodes and attache them to nodes of thisList;
            while (parentList != null) {
                bool duplicate = false;
                for (NsDecl tmp = thisList; tmp != null; tmp = tmp.Prev) {
                    if (Ref.Equal(tmp.Prefix, parentList.Prefix) && (
                        tmp.Prefix != null ||           // Namespace declaration
                        tmp.NsUri == parentList.NsUri   // Extension or excluded namespace
                    )) {
                        duplicate = true;
                        break;
                    }
                }
                if (!duplicate) {
                    thisList = new NsDecl(thisList, parentList.Prefix, parentList.NsUri);
                }
                parentList = parentList.Prev;
            }
            return thisList;
        }

        // -------------------------------- IErrorHelper --------------------------------

        public void ReportError(string res, params string[] args) {
            compiler.ReportError(input.BuildLineInfo(), res, args);
        }

        public void ReportWarning(string res, params string[] args) {
            compiler.ReportWarning(input.BuildLineInfo(), res, args);
        }
    }
}
