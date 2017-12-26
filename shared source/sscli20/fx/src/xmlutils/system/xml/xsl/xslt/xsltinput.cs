//------------------------------------------------------------------------------
// <copyright file="XsltInput.cs" company="Microsoft">
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

using System.Diagnostics;
using System.Text;
using System.Xml.XPath;

namespace System.Xml.Xsl.Xslt {
    using Res = System.Xml.Utils.Res;

    // This internal class is designed to help XSLT parser with reading stylesheet. We should be able to load SS from
    // XmlReader, URL. The simples way to load from URL and XmlReader is to load xml to XPathDocument and
    // use navigator over XPathDocument. To avoid intermediate storage XsltInput can work directly from reader and limits
    // navigation over SS nodes to XmlReader like MoveTo* functions.
    // document("") function in the case of XslInput(XmlReader) will read SS again. To be able to "reread" SS
    // we should be garanteed that Xml.Reader.BaseUri != "".

    //         a) Forward only, one pass.
    //         b) You should call MoveToFirstChildren on nonempty element node. (or may be skip)
    //         c) You should call MoveToParent when no more children is available.

    internal class XsltInput : IErrorHelper {
        private XmlReader           reader;
        private IXmlLineInfo        readerLineInfo;
        private bool                topLevelReader;
        private InputScopeManager   scopeManager;
        private KeywordsTable       atoms;
        private Compiler            compiler;

        private string              text;           // When text != null reader stays after all merged text nodes
        private bool                textIsWhite;    // true if all parts were WS or SWS

        // Cached properties. MoveTo* functions set them.
        private XPathNodeType       nodeType;
        private string              localName;
        private string              namespaceName;
        private string              value;

        public XsltInput(XmlReader reader, Compiler compiler) {
            Debug.Assert(reader != null);
            EnsureExpandEntities(reader);
            IXmlLineInfo xmlLineInfo = reader as IXmlLineInfo;

            this.reader         = reader;
            this.readerLineInfo = (xmlLineInfo != null && xmlLineInfo.HasLineInfo()) ? xmlLineInfo : null;
            this.topLevelReader = reader.ReadState == ReadState.Initial;
            this.scopeManager   = new InputScopeManager(reader.NameTable);
            this.atoms          = new KeywordsTable(reader.NameTable);
            this.compiler       = compiler;
            this.textIsWhite    = true;
            this.nodeType       = XPathNodeType.Root;
        }

        private static void EnsureExpandEntities(XmlReader reader) {
            XmlTextReader tr = reader as XmlTextReader;
            if (tr != null && tr.EntityHandling != EntityHandling.ExpandEntities) {
                Debug.Assert(tr.Settings == null, "XmlReader created with XmlReader.Create should always expand entities.");
                tr.EntityHandling = EntityHandling.ExpandEntities;
            }
        }

        public bool Start() {
            if (topLevelReader) {
                return MoveToNextSibling();
            } else {
                if (reader.ReadState != ReadState.Interactive) {
                    return false;
                }
                StepOnNodeRdr();
                if (nodeType == XPathNodeType.Comment || nodeType == XPathNodeType.ProcessingInstruction) {
                    return MoveToNextSibling();
                }
                return nodeType == XPathNodeType.Element;
            }
        }

        public void Finish() {
            scopeManager.CheckEmpty();

            // We may need to fix line info for the last end-tag
            reader.Read();
            FixLastLineInfo();

            if (topLevelReader) {
                while (reader.ReadState == ReadState.Interactive) {
                    reader.Skip();
                }
            }
        }

        private void SetCachedProperties() {
            nodeType        = ConvertNodeType(reader.NodeType);
            localName       = reader.LocalName;
            namespaceName   = reader.NamespaceURI;
            value           = reader.Value;

            if (nodeType == XPathNodeType.Attribute && IsNamespace(reader)) {
                nodeType = XPathNodeType.Namespace;
                namespaceName = string.Empty;
                if (localName == "xmlns") {
                    localName = string.Empty;
                }
            }
        }

        public KeywordsTable Atoms        { get { return atoms; } }

        // Cached properties
        public XPathNodeType NodeType     { get { return nodeType;      } }
        public string        Value        { get { return value;         } }
        public string        LocalName    { get { return localName;     } }
        public string        NamespaceUri { get { return namespaceName; } }

        // Non-cached properties
        public string        Prefix         { get { return reader.Prefix;         } }
        public string        BaseUri        { get { return reader.BaseURI;        } }
        public string        QualifiedName  { get { return reader.Name;           } }
        public bool          IsEmptyElement { get { return reader.IsEmptyElement; } }

        // Resolve prefix, return null and report an error if not found
        public string LookupXmlNamespace(string prefix) {
            Debug.Assert(prefix != null);
            string nsUri = reader.LookupNamespace(prefix);
            if (nsUri != null) {
                return nsUri;
            }
            if (prefix.Length == 0) {
                return string.Empty;
            }
            ReportError(/*[XT0280]*/Res.Xslt_InvalidPrefix, prefix);
            return null;
        }

        public bool MoveToNextAttOrNs() {
            Debug.Assert(NodeType == XPathNodeType.Element
                || NodeType == XPathNodeType.Attribute
                || NodeType == XPathNodeType.Namespace);

            if (NodeType == XPathNodeType.Element) {
                if (!reader.MoveToFirstAttribute()) {
                    reader.MoveToElement();
                    return false;
                }
            } else {
                if (!reader.MoveToNextAttribute()) {
                    reader.MoveToElement();
                    nodeType = XPathNodeType.Element;
                    return false;
                }
            }
            SetCachedProperties();
            return true;
        }

        // Debug subsystem
        private enum Moves {
            Next, Child, Parent
        };

        private Moves lastMove   = Moves.Child;
        private bool  lastResult = true;

        [Conditional("DEBUG")]
        private void SetLastMove(Moves lastMove, bool lastResult) {
            this.lastMove   = lastMove;
            this.lastResult = lastResult;
        }
        // --------------------

        private void StepOnNodeRdr() {
            if (text == null) {
                SetCachedProperties();
            } else {
                value          = text;
                localName      = string.Empty;
                namespaceName  = string.Empty;
                nodeType = (
                    !textIsWhite   ? XPathNodeType.Text :
                    textPreserveWS ? XPathNodeType.SignificantWhitespace :
                    /*default:    */ XPathNodeType.Whitespace
                );
            }
            if (NodeType == XPathNodeType.Element) {
                scopeManager.PushScope();
            }
        }

        private void StepOffNode() {
            if (NodeType == XPathNodeType.Element) {
                scopeManager.PopScope();
            }
        }

        private bool MoveToFirstChildAny() {
            if (!reader.IsEmptyElement) {
                return ReadNextSibling();
            }
            nodeType = XPathNodeType.Element;
            return false;
        }

        public bool MoveToFirstChild() {
            Debug.Assert(lastResult, "The previous move must be successful");
            Debug.Assert(lastMove != Moves.Parent, "MoveToNextSibling() must be called after MoveToParent()");
            bool result = MoveToFirstChildAny();
            if (result) {
                // Pass comments and PIs
                if (NodeType == XPathNodeType.Comment || NodeType == XPathNodeType.ProcessingInstruction) {
                    result = MoveToNextSibling();
                    if (! result) {
                        MoveToParent();
                    }
                }
            }
            SetLastMove(Moves.Child, result);
            return result;
        }

        public bool MoveToNextSibling() {
#if DEBUG
            if (lastMove == Moves.Next) {
                Debug.Assert(lastResult, "MoveToParent() must be called after MoveToNextSibling() failed");
                Debug.Assert(NodeType != XPathNodeType.Element, "MoveToFirstChild() was not called on the element. Did you miss SkipNode()?");
            }
#endif
            bool result;
            do {
                StepOffNode();
                result = ReadNextSibling();
            } while (result && (NodeType == XPathNodeType.Comment || NodeType == XPathNodeType.ProcessingInstruction));
            SetLastMove(Moves.Next, result);
            return result;
        }

        public bool MoveToParent() {
            Debug.Assert(lastMove == Moves.Next && !lastResult, "All siblings must be visited before moving to parent");
            // We shouldn't call StepOffNode() here because we already left last node.
            bool result = true;         // Frankly, we should return false for the root
            SetLastMove(Moves.Parent, result);
            return result;
        }

        public void SkipNode() {
            if (NodeType == XPathNodeType.Element && MoveToFirstChild()) {
                do {
                    SkipNode();
                } while (MoveToNextSibling());
                MoveToParent();
            }
        }

        private bool ReadNextSiblingHelper() {
            if (text != null) {
                text = null;
                textIsWhite = true;
                return reader.NodeType != XmlNodeType.EndElement;
            } else {
                while (reader.Read()) {
                    switch (reader.NodeType) {
                    case XmlNodeType.Text:
                        textIsWhite = false;
                        goto case XmlNodeType.SignificantWhitespace;
                    case XmlNodeType.CDATA:
                        if (textIsWhite && ! IsWhitespace(reader.Value)) {
                            textIsWhite = false;
                        }
                        goto case XmlNodeType.SignificantWhitespace;
                    case XmlNodeType.Whitespace:
                    case XmlNodeType.SignificantWhitespace:
                        if (reader.Depth == 0 && text == null && textIsWhite) {
                            break;  // Ignore whitespace nodes on the root level
                        }
                        if (text == null) {
                            // First chunk of a text node
                            SaveTextInfo();
                        }
                        text += reader.Value;
                        break;
                    case XmlNodeType.EntityReference :
                        Debug.Assert(false, "Entity references should be resolved for us");
                        break;
                    case XmlNodeType.DocumentType:
                    case XmlNodeType.XmlDeclaration:
                        break;
                    default:
                        if (text != null) {
                            return true;
                        }
                        return reader.NodeType != XmlNodeType.EndElement;
                    }
                }
                return text != null;
            }
        }

        private bool ReadNextSibling() {
            bool result = ReadNextSiblingHelper();
            FixLastLineInfo();
            if (result) {
                StepOnNodeRdr();
                return true;
            }
            nodeType = XPathNodeType.Element;
            return false;
        }

        private static bool IsNamespace(XmlReader reader) {
            Debug.Assert(reader.NodeType == XmlNodeType.Attribute);
            return reader.Prefix == "xmlns" || reader.Prefix.Length == 0 && reader.LocalName == "xmlns";
        }

        private static bool IsWhitespace(string text) {
            return XmlCharType.Instance.IsOnlyWhitespace(text);
        }

        private static XPathNodeType[] XmlNodeType2XPathNodeType = new XPathNodeType[] {
            (XPathNodeType) (-1),                   // 0 = None,
            XPathNodeType.Element,                  // 1 = Element,s
            XPathNodeType.Attribute,                // 2 = Attribute,
            XPathNodeType.Text,                     // 3 = Text,
            XPathNodeType.Text,                     // 4 = CDATA,
            (XPathNodeType) (-1),                   // 5 = EntityReference,
            (XPathNodeType) (-1),                   // 6 = Entity,
            XPathNodeType.ProcessingInstruction,    // 7 = ProcessingInstruction,
            XPathNodeType.Comment,                  // 8 = Comment,
            (XPathNodeType) (-1),                   // 9 = Document,
            (XPathNodeType) (-1),                   // 10 = DocumentType,
            (XPathNodeType) (-1),                   // 11 = DocumentFragment,
            (XPathNodeType) (-1),                   // 12 = Notation,
            XPathNodeType.Whitespace,               // 13 = Whitespace,
            XPathNodeType.SignificantWhitespace,    // 14 = SignificantWhitespace,
            XPathNodeType.Element,                  // 15 = EndElement,
            (XPathNodeType) (-1),                   // 16 = EndEntity,
            (XPathNodeType) (-1),                   // 17 = XmlDeclaration
        };

        private static XPathNodeType ConvertNodeType(XmlNodeType xmlNodeType) {
            XPathNodeType xpathNodeType = XmlNodeType2XPathNodeType[ (int)xmlNodeType ];

            if (xpathNodeType == (XPathNodeType) (-1)) {
                Debug.Fail("We must not step on this node");
                return XPathNodeType.All;
            }
            return xpathNodeType;
        }

        // -------------------- Keywords testing --------------------

        public bool IsNs(string ns)               { return Ref.Equal(ns, NamespaceUri); }
        public bool IsKeyword(string kwd)         { return Ref.Equal(kwd, LocalName);  }
        public bool IsXsltNamespace()             { return IsNs(atoms.UriXsl); }
        public bool IsNullNamespace()             { return IsNs(string.Empty); }
        public bool IsXsltAttribute(string kwd)   { return IsKeyword(kwd) && IsNullNamespace(); }

        // -------------------- Scope Management --------------------
        // See private class InputScopeManager bellow.
        // InputScopeManager handles some flags and values with respect of scope level where they as defined.
        // To parse XSLT style sheet we need the folloing values:
        //  ForwardCompatibility -- this flag is set when xsl:version!='1.0'.
        //  CanHaveApplyImports  -- we allow xsl:apply-templates instruction to apear in any template with match!=null, but not inside xsl:for-each
        //                          so it can't be inside global variable and has initial value = false
        //  ExtentionNamespace   -- is defined by extension-element-prefixes attribute on LRE or xsl:stylesheet

        public bool CanHaveApplyImports {
            get { return scopeManager.CanHaveApplyImports;  }
            set { scopeManager.CanHaveApplyImports = value; }
        }

        public void AddExtensionNamespace(string uri) { scopeManager.AddExtensionNamespace(uri); }
        public bool IsExtensionNamespace (string uri) { return scopeManager.IsExtensionNamespace(uri); }

        public bool ForwardCompatibility {
            get { return scopeManager.ForwardCompatibility; }
        }

        public XslVersion XslVersion {
            get { return scopeManager.ForwardCompatibility ? XslVersion.ForwardsCompatible : XslVersion.Current; }
        }

        public void SetVersion(string version, string attName) {
            double versionNum = XPathConvert.StringToDouble(version);
            if (double.IsNaN(versionNum)) {
                ReportError(/*[XT0110]*/Res.Xslt_InvalidAttrValue, attName, version);
                versionNum = 1.0;
            }
            scopeManager.ForwardCompatibility = (versionNum != 1.0);
        }

        // --------------- GetAtributes(...) -------------------------
        // All Xslt Instructions allows fixed set of attributes in null-ns, no in XSLT-ns and any in other ns.
        // In ForwardCompatibility mode we should ignore any of this problems.
        // We not use these functions for parseing LiteralResultElement and xsl:stylesheet

        private string[] names  = new string[10];
        private string[] values = new string[10];

        public ContextInfo GetAttributes() {
            return GetAttributes(0, 0, names, values);
        }

        public ContextInfo GetAttributes(int required, string name, out string value) {
            names[0] = name;
            ContextInfo ctxInfo = GetAttributes(required, 1, names, values);
            value = values[0];
            return ctxInfo;
        }

        public ContextInfo GetAttributes(int required, string name0, out string value0, string name1, out string value1) {
            names[0] = name0;
            names[1] = name1;
            ContextInfo ctxInfo = GetAttributes(required, 2, names, values);
            value0 = values[0];
            value1 = values[1];
            return ctxInfo;
        }

        public ContextInfo GetAttributes(int required,
            string name0, out string value0,
            string name1, out string value1,
            string name2, out string value2
        ) {
            names[0] = name0;
            names[1] = name1;
            names[2] = name2;
            ContextInfo ctxInfo = GetAttributes(required, 3, names, values);
            value0 = values[0];
            value1 = values[1];
            value2 = values[2];
            return ctxInfo;
        }

        public ContextInfo GetAttributes(int required,
            string name0, out string value0,
            string name1, out string value1,
            string name2, out string value2,
            string name3, out string value3
        ) {
            names[0] = name0;
            names[1] = name1;
            names[2] = name2;
            names[3] = name3;
            ContextInfo ctxInfo = GetAttributes(required, 4, names, values);
            value0 = values[0];
            value1 = values[1];
            value2 = values[2];
            value3 = values[3];
            return ctxInfo;
        }

        public ContextInfo GetAttributes(int required,
            string name0, out string value0,
            string name1, out string value1,
            string name2, out string value2,
            string name3, out string value3,
            string name4, out string value4
        ) {
            names[0] = name0;
            names[1] = name1;
            names[2] = name2;
            names[3] = name3;
            names[4] = name4;
            ContextInfo ctxInfo = GetAttributes(required, 5, names, values);
            value0 = values[0];
            value1 = values[1];
            value2 = values[2];
            value3 = values[3];
            value4 = values[4];
            return ctxInfo;
        }

        public ContextInfo GetAttributes(int required,
            string name0, out string value0,
            string name1, out string value1,
            string name2, out string value2,
            string name3, out string value3,
            string name4, out string value4,
            string name5, out string value5,
            string name6, out string value6,
            string name7, out string value7,
            string name8, out string value8
        ) {
            names[0] = name0;
            names[1] = name1;
            names[2] = name2;
            names[3] = name3;
            names[4] = name4;
            names[5] = name5;
            names[6] = name6;
            names[7] = name7;
            names[8] = name8;
            ContextInfo ctxInfo = GetAttributes(required, 9, names, values);
            value0 = values[0];
            value1 = values[1];
            value2 = values[2];
            value3 = values[3];
            value4 = values[4];
            value5 = values[5];
            value6 = values[6];
            value7 = values[7];
            value8 = values[8];
            return ctxInfo;
        }

        public ContextInfo GetAttributes(int required,
            string name0, out string value0,
            string name1, out string value1,
            string name2, out string value2,
            string name3, out string value3,
            string name4, out string value4,
            string name5, out string value5,
            string name6, out string value6,
            string name7, out string value7,
            string name8, out string value8,
            string name9, out string value9
        ) {
            names[0] = name0;
            names[1] = name1;
            names[2] = name2;
            names[3] = name3;
            names[4] = name4;
            names[5] = name5;
            names[6] = name6;
            names[7] = name7;
            names[8] = name8;
            names[9] = name9;
            ContextInfo ctxInfo = GetAttributes(required, 10, names, values);
            value0 = values[0];
            value1 = values[1];
            value2 = values[2];
            value3 = values[3];
            value4 = values[4];
            value5 = values[5];
            value6 = values[6];
            value7 = values[7];
            value8 = values[8];
            value9 = values[9];
            return ctxInfo;
        }

        public ContextInfo GetAttributes(int required, int number, string[] names, string[] values) {
            Debug.Assert(reader.NodeType == XmlNodeType.Element);
            for (int i = 0; i < number; i ++) {
                values[i] = null;
            }
            string elementName = QualifiedName;
            ContextInfo ctxInfo = new ContextInfo(this);
            compiler.EnterForwardsCompatible();

            while (MoveToNextAttOrNs()) {
                if (nodeType == XPathNodeType.Namespace) {
                    ctxInfo.AddNamespace(this);
                    continue;
                }
                Debug.Assert(nodeType == XPathNodeType.Attribute);
                ctxInfo.AddAttribute(this);
                bool found = false;
                for (int i = 0; i < number; i ++) {
                    if (IsXsltAttribute(names[i])) {
                        found = true;
                        values[i] = Value;

                        // There are only two XSL elements, xsl:stylesheet/xsl:transform and xsl:output, capable
                        // of having 'version' attribute.  And only for the first one this attribute is required.
                        if (Ref.Equal(names[i], Atoms.Version) && i < required) {
                            SetVersion(Value, Atoms.Version);
                        }
                        break;
                    }
                }
                if (!found) {
                    // An element from the XSLT namespace may have any attribute not from the XSLT namespace,
                    // provided that the expanded-name of the attribute has a non-null namespace URI.
                    // For example, it may be 'xml:space'.
                    if (IsNullNamespace() || IsXsltNamespace()) {
                        ReportError(/*[XT0090]*/Res.Xslt_InvalidAttribute, QualifiedName, elementName);
                    }
                }
            }

            // Ignore invalid attributes if forwards-compatible behavior is enabled. Note that invalid
            // attributes may encounter before ForwardCompatibility flag is set to true. For example,
            // <xsl:stylesheet unknown="foo" version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"/>
            compiler.ExitForwardsCompatible(ForwardCompatibility);

            // Report missing mandatory attributes
            for (int i = 0; i < required; i ++) {
                if (values[i] == null) {
                    ReportError(/*[XT_001]*/Res.Xslt_MissingAttribute, names[i]);
                }
            }
            ctxInfo.Finish(this);
            return ctxInfo;
        }

        // ----------------------- ISourceLineInfo -----------------------

        private SourceLineInfo  lastLineInfo  = null;

        public string Uri { get { return reader.BaseURI; } }

        public int StartLine {
            get {
                if (readerLineInfo == null) {
                    return 0;
                }
                if (OnTextNode) {
                    return textStartLine;
                } else {
                    return readerLineInfo.LineNumber;
                }
            }
        }

        public int StartPos {
            get {
                if (readerLineInfo == null) {
                    return 0;
                }
                if (OnTextNode) {
                    return textStartPos;
                } else {
                    return readerLineInfo.LinePosition - PositionAdjustment(reader.NodeType);
                }
            }
        }

        public int EndLine {
            get {
                if (readerLineInfo == null) {
                    return 0;
                }
                return readerLineInfo.LineNumber;
            }
        }

        public int EndPos {
            get {
                if (readerLineInfo == null) {
                    return 0;
                }
                int pos = readerLineInfo.LinePosition;
                if (OnTextNode) {
                    return pos - PositionAdjustment(reader.NodeType);
                } else {
                    // This value will be fixed later
                    return pos + 1;
                }
            }
        }

        private bool OnTextNode { get { return text != null; } }

        private static int PositionAdjustment(XmlNodeType nt) {
            switch (nt) {
            case XmlNodeType.Element:
                return 1;   // "<"
            case XmlNodeType.CDATA:
                return 9;   // "<![CDATA["
            case XmlNodeType.ProcessingInstruction:
                return 2;   // "<?"
            case XmlNodeType.Comment:
                return 4;   // "<!--"
            case XmlNodeType.EndElement:
                return 2;   // "</"
            default:
                return 0;
            }
        }

        private void SaveTextInfo() {
            Debug.Assert(!OnTextNode);
            textPreserveWS = reader.XmlSpace == XmlSpace.Preserve;
            textStartLine  = StartLine;
            textStartPos   = StartPos;
        }

        private bool    textPreserveWS;
        private int     textStartLine;
        private int     textStartPos;

        public ISourceLineInfo BuildLineInfo() {
            bool onAttribute = (nodeType == XPathNodeType.Attribute);
            if (lastLineInfo != null && !onAttribute) {
                Debug.Assert(
                    Uri       == lastLineInfo.Uri       &&
                    StartLine == lastLineInfo.StartLine &&
                    StartPos  == lastLineInfo.StartPos  &&
                    EndLine   == lastLineInfo.EndLine   &&
                    EndPos    == lastLineInfo.EndPos
                );
                return lastLineInfo;
            }
            SourceLineInfo lineInfo = new SourceLineInfo(Uri, StartLine, StartPos, EndLine, EndPos);
            if (!OnTextNode && !onAttribute) {
                lastLineInfo = lineInfo;
            }
            return lineInfo;
        }

        public void FixLastLineInfo() {
            if (lastLineInfo != null) {
                lastLineInfo.SetEndLinePos(StartLine, StartPos);
                lastLineInfo = null;
            }
        }

        // ---------------------- Error Handling ----------------------

        public void ReportError(string res, params string[] args) {
            compiler.ReportError(BuildLineInfo(), res, args);
        }

        public void ReportWarning(string res, params string[] args) {
            compiler.ReportWarning(BuildLineInfo(), res, args);
        }

        // ---------------------- InputScopeManager ----------------------

        private class InputScopeManager {
            private enum ScopeFlags {
                ForwardCompatibility = 0x1,
                CanHaveApplyImports  = 0x2,
                NsExtension          = 0x4,

                InheritedFlags = ForwardCompatibility | CanHaveApplyImports,
            }

            private struct ScopeRecord {
                public int        scopeCount;
                public ScopeFlags scopeFlags;
                public string     nsUri;
            }

            XmlNameTable nameTable;
            ScopeRecord[] records = new ScopeRecord[32];
            int lastRecord = 0;
            int lastScopes = 0;  // This is cash of records[lastRecord].scopeCount field;
                                // most often we will have PushScope()/PopScope pare over the same record.
                                // It has sence to avoid adresing this field through array access.

            public InputScopeManager(XmlNameTable nameTable) {
                this.nameTable = nameTable;
                records[0].scopeFlags = 0;
            }

            public void PushScope() {
                lastScopes ++;
            }

            public void PopScope() {
                if (0 < lastScopes) {
                    lastScopes--;
                } else {
                    do {
                        lastRecord--;
                    } while (records[lastRecord].scopeCount == 0);
                    lastScopes = records[lastRecord].scopeCount;
                    lastScopes--;
                }
            }

            private void AddRecord() {
                records[lastRecord].scopeCount = lastScopes;
                lastRecord ++;
                if (lastRecord == records.Length) {
                    ScopeRecord[] newRecords = new ScopeRecord[lastRecord * 2];
                    Array.Copy(records, 0, newRecords, 0, lastRecord);
                    records = newRecords;
                }
                lastScopes = 0;
            }

            private void SetFlag(bool value, ScopeFlags flag) {
                Debug.Assert(flag == (flag & ScopeFlags.InheritedFlags) && (flag & (flag - 1)) == 0 && flag != 0);
                ScopeFlags lastFlags  =  records[lastRecord].scopeFlags;
                if (((lastFlags & flag) != 0) != value) {
                    //         lastScopes == records[lastRecord].scopeCount; // we know this because we are cashing it.
                    bool canReuseLastRecord = lastScopes == 0;                             // last record is from last scope
                    if (! canReuseLastRecord) {
                        AddRecord();
                        lastFlags = lastFlags & ScopeFlags.InheritedFlags;
                    }
                    records[lastRecord].scopeFlags = lastFlags ^ flag;
                }
            }

            public bool ForwardCompatibility {
                get { return (records[lastRecord].scopeFlags & ScopeFlags.ForwardCompatibility) != 0; }
                set { SetFlag(value, ScopeFlags.ForwardCompatibility) ;}
            }

            public bool CanHaveApplyImports {
                get { return (records[lastRecord].scopeFlags & ScopeFlags.CanHaveApplyImports) != 0; }
                set { SetFlag(value, ScopeFlags.CanHaveApplyImports) ;}
            }

            public void AddExtensionNamespace(string uri) {
                Debug.Assert(uri != null);

                uri = nameTable.Add(uri);

                ScopeFlags lastFlags  =  records[lastRecord].scopeFlags;
                //         lastScopes == records[lastRecord].scopeCount; // we know this because we are cashing it.
                bool canReuseLastRecord = (
                    lastScopes == 0 &&                                   // last record is from last scope
                    (lastFlags & ScopeFlags.NsExtension) == 0            // only flag fields are used in this record
                );
                if (! canReuseLastRecord) {
                    AddRecord();
                    lastFlags = lastFlags & ScopeFlags.InheritedFlags;
                }
                records[lastRecord].nsUri = uri;
                records[lastRecord].scopeFlags = lastFlags | ScopeFlags.NsExtension;
            }

            public bool IsExtensionNamespace(string nsUri) {
                for (int record = lastRecord; 0 <= record; record --) {
                    if (
                        (records[record].scopeFlags & ScopeFlags.NsExtension) != 0 &&
                        (records[record].nsUri == nsUri)
                    ) {
                        return true;
                    }
                }
                return false;
            }

            [Conditional("DEBUG")]
            public void CheckEmpty() {
                PopScope();
                Debug.Assert(lastRecord == 0 && lastScopes == 0, "StepOnNode() and StepOffNode() calls have been unbalanced");
            }
        }

        // -------------------------------- ContextInfo ------------------------------------

        internal class ContextInfo {
            public NsDecl           nsList;
            public ISourceLineInfo  lineInfo;       // Line info for whole start tag
            public ISourceLineInfo  elemNameLi;     // Line info for element name
            public ISourceLineInfo  endTagLi;       // Line info for end tag or '/>'
            private int             elemNameLength;

            public ContextInfo(XsltInput input) {
                lineInfo = input.BuildLineInfo();
                elemNameLength = input.QualifiedName.Length;
            }

            public void AddAttribute(XsltInput input) {
            }

            public void AddNamespace(XsltInput input) {
                if (Ref.Equal(input.LocalName, input.Atoms.Xml)) {
                    Debug.Assert(input.Value == XmlReservedNs.NsXml, "XmlReader must check binding for 'xml' prefix");
                } else {
                    nsList = new NsDecl(nsList, input.LocalName, input.Value);
                }
            }

            public void Finish(XsltInput input) {
            }

            public void SaveExtendedLineInfo(XsltInput input) {
                elemNameLi = new SourceLineInfo(
                    lineInfo.Uri,
                    lineInfo.StartLine, lineInfo.StartPos + 1,  // "<"
                    lineInfo.StartLine, lineInfo.StartPos + 1 + elemNameLength
                );

                if (!input.IsEmptyElement) {
                    Debug.Assert(input.reader.NodeType == XmlNodeType.EndElement);
                    endTagLi = input.BuildLineInfo();
                } else {
                    Debug.Assert(input.reader.NodeType == XmlNodeType.Element);
                    endTagLi = new EmptyElementEndTag(lineInfo);
                }
            }

            // We need this wrapper class because elementTagLi is not yet calculated
            internal class EmptyElementEndTag : ISourceLineInfo {
                private ISourceLineInfo elementTagLi;

                public EmptyElementEndTag(ISourceLineInfo elementTagLi) {
                    this.elementTagLi = elementTagLi;
                }

                public string Uri       { get { return elementTagLi.Uri;        } }
                public int StartLine    { get { return elementTagLi.EndLine;    } }
                public int StartPos     { get { return elementTagLi.EndPos - 2; } }
                public int EndLine      { get { return elementTagLi.EndLine;    } }
                public int EndPos       { get { return elementTagLi.EndPos;     } }
                public bool IsNoSource  { get { return elementTagLi.IsNoSource; } }
            }
        }
    }
}
