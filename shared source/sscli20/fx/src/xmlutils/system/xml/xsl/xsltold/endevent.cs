//------------------------------------------------------------------------------
// <copyright file="EndEvent.cs" company="Microsoft">
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

    internal class EndEvent : Event {
        private XPathNodeType nodeType;
        
        internal EndEvent(XPathNodeType nodeType) {
            Debug.Assert(nodeType != XPathNodeType.Namespace);
            this.nodeType = nodeType;
        }

        public override bool Output(Processor processor, ActionFrame frame) {
            return processor.EndEvent(this.nodeType);
        }
    }
}
