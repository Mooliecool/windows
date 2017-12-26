//------------------------------------------------------------------------------
// <copyright file="CopyOfAction.cs" company="Microsoft">
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
    using MS.Internal.Xml.XPath;

    internal class CopyOfAction : CompiledAction {
        private const int ResultStored  = 2;
        private const int NodeSetCopied = 3;

        private int    selectKey   = Compiler.InvalidQueryKey;

        internal override void Compile(Compiler compiler) {
            CompileAttributes(compiler);
            CheckRequiredAttribute(compiler, selectKey != Compiler.InvalidQueryKey, Keywords.s_Select);
            CheckEmpty(compiler);
        }

        internal override bool CompileAttribute(Compiler compiler) {
            string name   = compiler.Input.LocalName;
            string value  = compiler.Input.Value;
            if (Keywords.Equals(name, compiler.Atoms.Select)) {
                this.selectKey = compiler.AddQuery(value);
            }
            else {
                return false;
            }

            return true;
        }

        internal override void Execute(Processor processor, ActionFrame frame) {
            Debug.Assert(processor != null && frame != null);

            switch (frame.State) {
            case Initialized:
                Debug.Assert(frame.NodeSet != null);
                Query query = processor.GetValueQuery(this.selectKey);
                object result = query.Evaluate(frame.NodeSet);

                if (result is XPathNodeIterator) {
                    processor.PushActionFrame(CopyNodeSetAction.GetAction(), new XPathArrayIterator(query));
                    frame.State = NodeSetCopied;
                    break;
                }

                XPathNavigator nav = result as XPathNavigator;
                if (nav != null) {
                    processor.PushActionFrame(CopyNodeSetAction.GetAction(), new XPathSingletonIterator(nav));
                    frame.State = NodeSetCopied;
                    break; 
                }

                string value = XmlConvert.ToXPathString(result);
                if (processor.TextEvent(value)) {
                    frame.Finished();
                } else {
                    frame.StoredOutput = value;
                    frame.State        = ResultStored;
                }
                break;

            case ResultStored:
                Debug.Assert(frame.StoredOutput != null);
                processor.TextEvent(frame.StoredOutput);
                frame.Finished();
                break;

            case NodeSetCopied:
                Debug.Assert(frame.State == NodeSetCopied);
                frame.Finished();
                break;
            }
        }
    }
}
