//------------------------------------------------------------------------------
// <copyright file="AvtEvent.cs" company="Microsoft">
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
    using System.Text;

    internal sealed class AvtEvent : TextEvent {
        private int key;

        public AvtEvent(int key) {
            Debug.Assert(key != Compiler.InvalidQueryKey);
            this.key = key;
        }

        public override bool Output(Processor processor, ActionFrame frame) {
            Debug.Assert(key != Compiler.InvalidQueryKey);
            return processor.TextEvent(processor.EvaluateString(frame, this.key));
        }

        public override string Evaluate(Processor processor, ActionFrame frame) {
            return processor.EvaluateString(frame, this.key);
        }
    }
}
