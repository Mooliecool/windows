//------------------------------------------------------------------------------
// <copyright file="CompiledAction.cs" company="Microsoft">
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

    internal abstract class CompiledAction : Action {
        internal abstract void Compile(Compiler compiler);

        internal virtual bool CompileAttribute(Compiler compiler) {
            return false;
        }
        
        public void CompileAttributes(Compiler compiler) {
            NavigatorInput input   = compiler.Input;
            string         element = input.LocalName;

            if (input.MoveToFirstAttribute()) {
                do {
                    if (! Keywords.Equals(input.NamespaceURI, input.Atoms.Empty)) continue;

                    try {
                        if (CompileAttribute(compiler) == false) {
                            throw XsltException.Create(Res.Xslt_InvalidAttribute, input.LocalName, element);
                        }
                    }catch {
                        if (! compiler.ForwardCompatibility) {
                            throw;
                        }
                        else {
                            // In ForwardCompatibility mode we ignoreing all unknown or incorrect attributes
                            // If it's mandatory attribute we'l notice it absents later.
                        }
                    }
                }
                while (input.MoveToNextAttribute());
                input.ToParent();
            }
        }

        // For perf reason we precalculating AVTs at compile time.
        // If we can do this we set original AVT to null
        internal static string PrecalculateAvt(ref Avt avt) {
            string result = null;
            if(avt != null && avt.IsConstant) {
                result = avt.Evaluate(null, null);
                avt = null;
            }
            return result;
        }

        public void CheckEmpty(Compiler compiler) {
            // Really EMPTY means no content at all, but the sake of compatibility with MSXML we allow whitespaces
            string elementName = compiler.Input.Name;
            if (compiler.Recurse()) {
                do {
                    // Note: <![CDATA[ ]]> will be reported as XPathNodeType.Text
                    XPathNodeType nodeType = compiler.Input.NodeType;
                    if (
                        nodeType != XPathNodeType.Whitespace            &&
                        nodeType != XPathNodeType.Comment               &&
                        nodeType != XPathNodeType.ProcessingInstruction
                    ) {
                        throw XsltException.Create(Res.Xslt_NotEmptyContents, elementName);
                    }
                }
                while (compiler.Advance());
                compiler.ToParent();
            }
        }

        public void CheckRequiredAttribute(Compiler compiler, object attrValue, string attrName) {
            CheckRequiredAttribute(compiler, attrValue != null, attrName);
        }

        public void CheckRequiredAttribute(Compiler compiler, bool attr, string attrName) {
            if (! attr) {
                throw XsltException.Create(Res.Xslt_MissingAttribute, attrName);
            }
        }
    }
}
