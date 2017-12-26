//------------------------------------------------------------------------------
// <copyright file="ChooseAction.cs" company="Microsoft">
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

    internal class ChooseAction : ContainerAction {
        internal override void Compile(Compiler compiler) {
            CompileAttributes(compiler);

            if (compiler.Recurse()) {
                CompileConditions(compiler);
                compiler.ToParent();
            }
        }

        private void CompileConditions(Compiler compiler) {
            NavigatorInput input = compiler.Input;
            bool when       = false;
            bool otherwise  = false;

            do {
                switch (input.NodeType) {
                case XPathNodeType.Element:
                    compiler.PushNamespaceScope();
                    string nspace = input.NamespaceURI;
                    string name   = input.LocalName;

                    if (Keywords.Equals(nspace, input.Atoms.XsltNamespace)) {
                        IfAction action = null;
                        if (Keywords.Equals(name, input.Atoms.When)) {
                            if (otherwise) {
                                throw XsltException.Create(Res.Xslt_WhenAfterOtherwise);
                            }
                            action = compiler.CreateIfAction(IfAction.ConditionType.ConditionWhen);
                            when = true;
                        }
                        else if (Keywords.Equals(name, input.Atoms.Otherwise)) {
                            if (otherwise) {
                                throw XsltException.Create(Res.Xslt_DupOtherwise);
                            }
                            action = compiler.CreateIfAction(IfAction.ConditionType.ConditionOtherwise);
                            otherwise = true;
                        }
                        else {
                            throw compiler.UnexpectedKeyword();
                        }
                        AddAction(action);
                    }
                    else {
                        throw compiler.UnexpectedKeyword();
                    }
                    compiler.PopScope();
                    break;

                case XPathNodeType.Comment:
                case XPathNodeType.ProcessingInstruction:
                case XPathNodeType.Whitespace:
                case XPathNodeType.SignificantWhitespace:
                    break;

                default:
                    throw XsltException.Create(Res.Xslt_InvalidContents, Keywords.s_Choose);
                }
            }
            while (compiler.Advance());
            if (! when) {
                throw XsltException.Create(Res.Xslt_NoWhen);
            }
        }
    }
}
