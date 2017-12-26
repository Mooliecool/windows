//------------------------------------------------------------------------------
// <copyright file="CommentAction.cs" company="Microsoft">
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

    internal class CommentAction : ContainerAction {
        internal override void Compile(Compiler compiler) {
            CompileAttributes(compiler);

            if (compiler.Recurse()) {
                CompileTemplate(compiler);
                compiler.ToParent();
            }
        }

        internal override void Execute(Processor processor, ActionFrame frame) {
            Debug.Assert(processor != null && frame != null);

            switch (frame.State) {
            case Initialized:
                if (processor.BeginEvent(XPathNodeType.Comment, string.Empty, string.Empty, string.Empty, false) == false) {
                    // Come back later
                    break;
                }

                processor.PushActionFrame(frame);
                frame.State = ProcessingChildren;
                break;                              // Allow children to run

            case ProcessingChildren:
                if (processor.EndEvent(XPathNodeType.Comment) == false) {
                    break;
                }

                frame.Finished();
                break;

            default:
                Debug.Fail("Invalid IfAction execution state");
    		    break;
            }
        }
    }
}
