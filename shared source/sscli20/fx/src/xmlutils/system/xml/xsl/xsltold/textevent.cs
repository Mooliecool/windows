//------------------------------------------------------------------------------
// <copyright file="TextEvent.cs" company="Microsoft">
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

    internal class TextEvent : Event {
        private string text;
        
        protected TextEvent() {}

        public TextEvent(string text) {
            Debug.Assert(text != null);
            this.text = text;
        }

        public TextEvent(Compiler compiler) {
            NavigatorInput input = compiler.Input;
            Debug.Assert(input.NodeType == XPathNodeType.Text || input.NodeType == XPathNodeType.SignificantWhitespace);
            this.text = input.Value;
        }

        public override bool Output(Processor processor, ActionFrame frame) {
            return processor.TextEvent(this.text);
        }

        public virtual string Evaluate(Processor processor, ActionFrame frame) {
            return this.text;
        }
    }
}
