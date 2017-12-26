//------------------------------------------------------------------------------
// <copyright file="BeginEvent.cs" company="Microsoft">
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

namespace System.Xml.Xsl.XsltOld {
    using Res = System.Xml.Utils.Res;
    using System;
    using System.Diagnostics;
    using System.Xml;
    using System.Xml.XPath;

    internal class BeginEvent : Event {
        private XPathNodeType nodeType;
        private string        namespaceUri;
        private string        name;
        private string        prefix;
        private bool          empty;
        private object        htmlProps;
#if DEBUG
        private bool          replaceNSAliasesDone;
#endif
        
        public BeginEvent(Compiler compiler) {
            NavigatorInput input = compiler.Input;
            Debug.Assert(input != null);
            Debug.Assert(input.NodeType != XPathNodeType.Namespace);
            this.nodeType     = input.NodeType;
            this.namespaceUri = input.NamespaceURI;
            this.name         = input.LocalName;
            this.prefix       = input.Prefix;
            this.empty        = input.IsEmptyTag;
            if (nodeType ==  XPathNodeType.Element) {
                this.htmlProps = HtmlElementProps.GetProps(this.name);
            }
            else if (nodeType ==  XPathNodeType.Attribute) {
                this.htmlProps = HtmlAttributeProps.GetProps(this.name);
            }
        }

        public override void ReplaceNamespaceAlias(Compiler compiler){
#if DEBUG
            Debug.Assert(! replaceNSAliasesDone, "Second attempt to replace NS aliases!. This bad.");
            replaceNSAliasesDone = true;
#endif
            if (this.nodeType == XPathNodeType.Attribute && this.namespaceUri.Length == 0) {
                return ; // '#default' aren't apply to attributes.
            }
            NamespaceInfo ResultURIInfo = compiler.FindNamespaceAlias(this.namespaceUri);
            if (ResultURIInfo != null) {
                this.namespaceUri = ResultURIInfo.nameSpace;
                if (ResultURIInfo.prefix != null) {
                    this.prefix = ResultURIInfo.prefix; 
                }
            }
        }
        
        public override bool Output(Processor processor, ActionFrame frame) {
            return processor.BeginEvent(this.nodeType, this.prefix, this.name, this.namespaceUri, this.empty, this.htmlProps, false);
        }
    }
}
