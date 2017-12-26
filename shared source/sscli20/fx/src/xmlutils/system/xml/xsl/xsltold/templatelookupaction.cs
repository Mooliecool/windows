//------------------------------------------------------------------------------
// <copyright file="TemplateLookupAction.cs" company="Microsoft">
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

    internal class TemplateLookupAction : Action {
        protected XmlQualifiedName mode;
        protected Stylesheet       importsOf;

        internal void Initialize(XmlQualifiedName mode, Stylesheet importsOf) {
            this.mode      = mode;
            this.importsOf = importsOf;
        }

        internal override void Execute(Processor processor, ActionFrame frame) {
            Debug.Assert(processor != null && frame != null);
            Debug.Assert(frame.State == Initialized);

            Action action = null;

            if (this.mode != null) {
                action = importsOf == null 
                    ? processor.Stylesheet.FindTemplate(processor, frame.Node, this.mode)
                    : importsOf.FindTemplateImports(processor, frame.Node, this.mode);
            }
            else {
                action = importsOf == null 
                    ? processor.Stylesheet.FindTemplate(processor, frame.Node)
                    : importsOf.FindTemplateImports(processor, frame.Node);
            }

            // Built-int template rules
            if (action == null) {
                action = BuiltInTemplate(frame.Node);
            }

            // Jump
            if (action != null) {
                frame.SetAction(action);
            }
            else {
                frame.Finished();
            }
        }

        internal Action BuiltInTemplate(XPathNavigator node) {
            Debug.Assert(node != null);
            Action action = null;

            switch (node.NodeType) {
            //  <xsl:template match="*|/" [mode="?"]>
            //    <xsl:apply-templates [mode="?"]/>
            //  </xsl:template>
            case XPathNodeType.Element:
            case XPathNodeType.Root:
                action = ApplyTemplatesAction.BuiltInRule(this.mode);
                break;
            //  <xsl:template match="text()|@*">
            //    <xsl:value-of select="."/>
            //  </xsl:template>
            case XPathNodeType.Attribute:
            case XPathNodeType.Whitespace:
            case XPathNodeType.SignificantWhitespace:
            case XPathNodeType.Text:
                action = ValueOfAction.BuiltInRule();
                break;
            // <xsl:template match="processing-instruction()|comment()"/>
            case XPathNodeType.ProcessingInstruction:
            case XPathNodeType.Comment:
                // Empty action;
                break;
            case XPathNodeType.All:
                // Ignore the rest
                break;
            }

            return action;
        }
    }

    internal class TemplateLookupActionDbg : TemplateLookupAction {
        internal override void Execute(Processor processor, ActionFrame frame) {
            Debug.Assert(processor != null && frame != null);
            Debug.Assert(frame.State == Initialized);
            Debug.Assert(processor.Debugger != null);

            Action action = null;

            if (this.mode == Compiler.BuiltInMode) {
                // mode="*" -- use one from debuggerStack
                this.mode = processor.GetPrevioseMode();
                Debug.Assert(this.mode != Compiler.BuiltInMode);
            }
            processor.SetCurrentMode(this.mode);

            if (this.mode != null) {
                action = importsOf == null 
                    ? processor.Stylesheet.FindTemplate(processor, frame.Node, this.mode)
                    : importsOf.FindTemplateImports(processor, frame.Node, this.mode);
            }
            else {
                action = importsOf == null 
                    ? processor.Stylesheet.FindTemplate(processor, frame.Node)
                    : importsOf.FindTemplateImports(processor, frame.Node);
            }

            // Built-int template rules
            if (action == null && processor.RootAction.builtInSheet != null) {
                action = processor.RootAction.builtInSheet.FindTemplate(processor, frame.Node, Compiler.BuiltInMode);
            }
            if (action == null) {
                action = BuiltInTemplate(frame.Node);
            }

            // Jump
            if (action != null) {
                frame.SetAction(action);
            }
            else {
                frame.Finished();
            }
        }
    }
}
