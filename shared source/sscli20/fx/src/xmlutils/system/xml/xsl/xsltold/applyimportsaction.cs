//------------------------------------------------------------------------------
// <copyright file="ApplyImportsAction.cs" company="Microsoft">
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

    internal class ApplyImportsAction : CompiledAction {
        private XmlQualifiedName   mode;
        private Stylesheet         stylesheet;
        private const int    TemplateProcessed = 2;
        internal override void Compile(Compiler compiler) {
            CheckEmpty(compiler);
            if (! compiler.CanHaveApplyImports) {
                throw XsltException.Create(Res.Xslt_ApplyImports);                
            }
            this.mode = compiler.CurrentMode;
            this.stylesheet = compiler.CompiledStylesheet;
        }

        internal override void Execute(Processor processor, ActionFrame frame) {
            Debug.Assert(processor != null && frame != null);
            switch (frame.State) {
            case Initialized:
                processor.PushTemplateLookup(frame.NodeSet, this.mode, /*importsOf:*/this.stylesheet);
                frame.State = TemplateProcessed;
                break;
            case TemplateProcessed:
                frame.Finished();
                break;
            }
        }
    }
}
