//------------------------------------------------------------------------------
// <copyright file="newinstructionaction.cs" company="Microsoft">
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

    internal class NewInstructionAction : ContainerAction {
        string name;
        string parent;
        bool fallback;
        
        internal override void Compile(Compiler compiler) {
            XPathNavigator nav = compiler.Input.Navigator.Clone();
            name = nav.Name;
            nav.MoveToParent();
            parent = nav.Name;
            if (compiler.Recurse()) {
                CompileSelectiveTemplate(compiler);
                compiler.ToParent();
            }
        }

        internal void CompileSelectiveTemplate(Compiler compiler){
            NavigatorInput input = compiler.Input;
            do{
                if (Keywords.Equals(input.NamespaceURI, input.Atoms.XsltNamespace) &&
                    Keywords.Equals(input.LocalName, input.Atoms.Fallback)){
                    fallback = true;
                    if (compiler.Recurse()){
                        CompileTemplate(compiler);
                        compiler.ToParent();
                    }
                }
            }
            while (compiler.Advance());
        }
        
       internal override void Execute(Processor processor, ActionFrame frame) {
            Debug.Assert(processor != null && frame != null);

            switch (frame.State) {
            case Initialized:
                if (!fallback) {
                    throw XsltException.Create(Res.Xslt_UnknownExtensionElement, this.name);
                }
                if (this.containedActions != null && this.containedActions.Count > 0) {
                    processor.PushActionFrame(frame);
                    frame.State = ProcessingChildren;
                    break;
                }
                else goto case ProcessingChildren;
            case ProcessingChildren:
                frame.Finished();
                break;

            default:
                Debug.Fail("Invalid Container action execution state");
                break;
            }
        }
    }
}
