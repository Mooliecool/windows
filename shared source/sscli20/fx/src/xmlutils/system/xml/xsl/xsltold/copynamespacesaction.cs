//------------------------------------------------------------------------------
// <copyright file="CopyNamespacesAction.cs" company="Microsoft">
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

    internal sealed class CopyNamespacesAction : Action {
        private const int BeginEvent    = 2;
        private const int TextEvent     = 3;
        private const int EndEvent      = 4;
        private const int Advance       = 5;

        private static CopyNamespacesAction s_Action = new CopyNamespacesAction();

        internal static CopyNamespacesAction GetAction() {
            Debug.Assert(s_Action != null);
            return s_Action;
        }

        internal override void Execute(Processor processor, ActionFrame frame) {
            Debug.Assert(processor != null && frame != null);

            while (processor.CanContinue) {
                switch (frame.State) {
                case Initialized:
                    if (frame.Node.MoveToFirstNamespace(XPathNamespaceScope.ExcludeXml) == false) {
                        frame.Finished();
                        break;
                    }

                    frame.State   = BeginEvent;
                    goto case BeginEvent;

                case BeginEvent:
                    Debug.Assert(frame.State == BeginEvent);
                    Debug.Assert(frame.Node.NodeType == XPathNodeType.Namespace);

                    if (processor.BeginEvent(XPathNodeType.Namespace, null, frame.Node.LocalName, frame.Node.Value, false) == false) {
                        // This one wasn't output
                        break;
                    }
                    frame.State = EndEvent;
                    continue;

                case EndEvent:
                    Debug.Assert(frame.State == EndEvent);
                    Debug.Assert(frame.Node.NodeType == XPathNodeType.Namespace);

                    if (processor.EndEvent(XPathNodeType.Namespace) == false) {
                        // This one wasn't output
                        break;
                    }
                    frame.State = Advance;
                    continue;

                case Advance:
                    Debug.Assert(frame.State == Advance);
                    Debug.Assert(frame.Node.NodeType == XPathNodeType.Namespace);

                    if (frame.Node.MoveToNextNamespace(XPathNamespaceScope.ExcludeXml)) {
                        frame.State = BeginEvent;
                        continue;
                    }
                    else {
                        frame.Node.MoveToParent();
                        frame.Finished();
                        break;
                    }
                }
                break;
            }// while
        }
    }
}
