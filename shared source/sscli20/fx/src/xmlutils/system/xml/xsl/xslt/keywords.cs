//------------------------------------------------------------------------------
// <copyright file="Keywords.cs" company="Microsoft">
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

using System.Xml;

namespace System.Xml.Xsl.Xslt {

    internal class KeywordsTable {
        public string ApplyImports;
        public string ApplyTemplates;
        public string Assembly;
        public string Attribute;
        public string AttributeSet;
        public string CallTemplate;
        public string CaseOrder;
        public string CDataSectionElements;
        public string Choose;
        public string Comment;
        public string Copy;
        public string CopyOf;
        public string Count;
        public string DataType;
        public string DecimalFormat;
        public string DecimalSeparator;
        public string Digit;
        public string DisableOutputEscaping;
        public string DocTypePublic;
        public string DocTypeSystem;
        public string Element;
        public string Elements;
        public string Encoding;
        public string ExcludeResultPrefixes;
        public string ExtensionElementPrefixes;
        public string Fallback;
        public string ForEach;
        public string Format;
        public string From;
        public string GroupingSeparator;
        public string GroupingSize;
        public string Href;
        public string Id;
        public string If;
        public string ImplementsPrefix;
        public string Import;
        public string Include;
        public string Indent;
        public string Infinity;
        public string Key;
        public string Lang;
        public string Language;
        public string LetterValue;
        public string Level;
        public string Match;
        public string MediaType;
        public string Message;
        public string Method;
        public string MinusSign;
        public string Mode;
        public string Name;
        public string Namespace;
        public string NamespaceAlias;
        public string NaN;
        public string Number;
        public string OmitXmlDeclaration;
        public string Order;
        public string Otherwise;
        public string Output;
        public string Param;
        public string PatternSeparator;
        public string Percent;
        public string PerMille;
        public string PreserveSpace;
        public string Priority;
        public string ProcessingInstruction;
        public string ResultPrefix;
        public string Script;
        public string Select;
        public string Sort;
        public string Space;
        public string Standalone;
        public string StripSpace;
        public string Stylesheet;
        public string StylesheetPrefix;
        public string Template;
        public string Terminate;
        public string Test;
        public string Text;
        public string Transform;
        public string UrnMsxsl;
        public string UriXml;
        public string UriXsl;
        public string UriWdXsl;
        public string Use;
        public string UseAttributeSets;
        public string Using;
        public string Value;
        public string ValueOf;
        public string Variable;
        public string Version;
        public string When;
        public string WithParam;
        public string Xml;
        public string ZeroDigit;

        public KeywordsTable(XmlNameTable nt) {
            ApplyImports             = nt.Add("apply-imports");
            ApplyTemplates           = nt.Add("apply-templates");
            Assembly                 = nt.Add("assembly");
            Attribute                = nt.Add("attribute");
            AttributeSet             = nt.Add("attribute-set");
            CallTemplate             = nt.Add("call-template");
            CaseOrder                = nt.Add("case-order");
            CDataSectionElements     = nt.Add("cdata-section-elements");
            Choose                   = nt.Add("choose");
            Comment                  = nt.Add("comment");
            Copy                     = nt.Add("copy");
            CopyOf                   = nt.Add("copy-of");
            Count                    = nt.Add("count");
            DataType                 = nt.Add("data-type");
            DecimalFormat            = nt.Add("decimal-format");
            DecimalSeparator         = nt.Add("decimal-separator");
            Digit                    = nt.Add("digit");
            DisableOutputEscaping    = nt.Add("disable-output-escaping");
            DocTypePublic            = nt.Add("doctype-public");
            DocTypeSystem            = nt.Add("doctype-system");
            Element                  = nt.Add("element");
            Elements                 = nt.Add("elements");
            Encoding                 = nt.Add("encoding");
            ExcludeResultPrefixes    = nt.Add("exclude-result-prefixes");
            ExtensionElementPrefixes = nt.Add("extension-element-prefixes");
            Fallback                 = nt.Add("fallback");
            ForEach                  = nt.Add("for-each");
            Format                   = nt.Add("format");
            From                     = nt.Add("from");
            GroupingSeparator        = nt.Add("grouping-separator");
            GroupingSize             = nt.Add("grouping-size");
            Href                     = nt.Add("href");
            Id                       = nt.Add("id");
            If                       = nt.Add("if");
            ImplementsPrefix         = nt.Add("implements-prefix");
            Import                   = nt.Add("import");
            Include                  = nt.Add("include");
            Indent                   = nt.Add("indent");
            Infinity                 = nt.Add("infinity");
            Key                      = nt.Add("key");
            Lang                     = nt.Add("lang");
            Language                 = nt.Add("language");
            LetterValue              = nt.Add("letter-value");
            Level                    = nt.Add("level");
            Match                    = nt.Add("match");
            MediaType                = nt.Add("media-type");
            Message                  = nt.Add("message");
            Method                   = nt.Add("method");
            MinusSign                = nt.Add("minus-sign");
            Mode                     = nt.Add("mode");
            Name                     = nt.Add("name");
            Namespace                = nt.Add("namespace");
            NamespaceAlias           = nt.Add("namespace-alias");
            NaN                      = nt.Add("NaN");
            Number                   = nt.Add("number");
            OmitXmlDeclaration       = nt.Add("omit-xml-declaration");
            Otherwise                = nt.Add("otherwise");
            Order                    = nt.Add("order");
            Output                   = nt.Add("output");
            Param                    = nt.Add("param");
            PatternSeparator         = nt.Add("pattern-separator");
            Percent                  = nt.Add("percent");
            PerMille                 = nt.Add("per-mille");
            PreserveSpace            = nt.Add("preserve-space");
            Priority                 = nt.Add("priority");
            ProcessingInstruction    = nt.Add("processing-instruction");
            ResultPrefix             = nt.Add("result-prefix");
            Script                   = nt.Add("script");
            Select                   = nt.Add("select");
            Sort                     = nt.Add("sort");
            Space                    = nt.Add("space");
            Standalone               = nt.Add("standalone");
            StripSpace               = nt.Add("strip-space");
            Stylesheet               = nt.Add("stylesheet");
            StylesheetPrefix         = nt.Add("stylesheet-prefix");
            Template                 = nt.Add("template");
            Terminate                = nt.Add("terminate");
            Test                     = nt.Add("test");
            Text                     = nt.Add("text");
            Transform                = nt.Add("transform");
            UrnMsxsl                 = nt.Add(XmlReservedNs.NsMsxsl);
            UriXml                   = nt.Add(XmlReservedNs.NsXml);
            UriXsl                   = nt.Add(XmlReservedNs.NsXslt);
            UriWdXsl                 = nt.Add(XmlReservedNs.NsWdXsl);
            Use                      = nt.Add("use");
            UseAttributeSets         = nt.Add("use-attribute-sets");
            Using                    = nt.Add("using");
            Value                    = nt.Add("value");
            ValueOf                  = nt.Add("value-of");
            Variable                 = nt.Add("variable");
            Version                  = nt.Add("version");
            When                     = nt.Add("when");
            WithParam                = nt.Add("with-param");
            Xml                      = nt.Add("xml");
            ZeroDigit                = nt.Add("zero-digit");
        }
    }
}
