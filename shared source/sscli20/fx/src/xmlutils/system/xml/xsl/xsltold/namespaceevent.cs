//------------------------------------------------------------------------------
// <copyright file="NameSpaceEvent.cs" company="Microsoft">
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

    internal class NamespaceEvent : Event {
        private string namespaceUri;
        private string name;
        
        public NamespaceEvent(NavigatorInput input) {
            Debug.Assert(input != null);
            Debug.Assert(input.NodeType == XPathNodeType.Namespace);
            this.namespaceUri = input.Value;
            this.name         = input.LocalName;
        }

        public override void ReplaceNamespaceAlias(Compiler compiler){
            if (this.namespaceUri.Length != 0) { // Do we need to check this for namespace?
                NamespaceInfo ResultURIInfo = compiler.FindNamespaceAlias(this.namespaceUri);
                if (ResultURIInfo != null) {
                    this.namespaceUri = ResultURIInfo.nameSpace;
                    if (ResultURIInfo.prefix != null) {
                        this.name = ResultURIInfo.prefix;
                    }
                }
            }
        }
        
        public override bool Output(Processor processor, ActionFrame frame) {
            bool res;
            res = processor.BeginEvent(XPathNodeType.Namespace, /*prefix:*/null, this.name, this.namespaceUri, /*empty:*/false);
            Debug.Assert(res); // Namespace node as any other attribute can't fail because it doesn't signal record change
            res = processor.EndEvent(XPathNodeType.Namespace);
            Debug.Assert(res);
            return true;
        }
    }
}
