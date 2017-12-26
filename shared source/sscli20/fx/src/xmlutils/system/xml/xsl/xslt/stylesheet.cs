//------------------------------------------------------------------------------
// <copyright file="Stylesheet.cs" company="Microsoft">
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

using System.Collections.Generic;
using System.Diagnostics;
using System.Xml.Xsl.Qil;

namespace System.Xml.Xsl.Xslt {

    internal class Stylesheet {
        private Compiler            compiler;
        public List<Uri>            ImportHrefs         = new List<Uri>();
        public Stylesheet[]         Imports             = null;
        public List<Template>       Templates           = new List<Template>();
        public List<XslNode>        GlobalVarPars       = new List<XslNode>();

        // xsl:attribute-set/@name -> AttributeSet
        public Dictionary<QilName, AttributeSet>        AttributeSets   = new Dictionary<QilName, AttributeSet>();

        // xsl:template/@mode -> list of @match'es
        public Dictionary<QilName, List<TemplateMatch>> TemplateMatches = new Dictionary<QilName, List<TemplateMatch>>();

        // mode -> xsl:apply-import functions for that mode
        public Dictionary<QilName, List<QilFunction>>   ApplyImportsFunctions = new Dictionary<QilName, List<QilFunction>>();

        private int                importPrecedence;
        private int                orderNumber          = 0;

        /*
            WhitespaceRules[0] - rules with default priority  0
            WhitespaceRules[1] - rules with default priority -0.25
            WhitespaceRules[2] - rules with default priority -0.5
        */
        public List<WhitespaceRule>[]   WhitespaceRules = new List<WhitespaceRule>[3];

        public Stylesheet(Compiler compiler, int importPrecedence) {
            this.compiler         = compiler;
            this.importPrecedence = importPrecedence;

            WhitespaceRules[0] = new List<WhitespaceRule>();
            WhitespaceRules[1] = new List<WhitespaceRule>();
            WhitespaceRules[2] = new List<WhitespaceRule>();
        }

        public int ImportPrecedence { get { return importPrecedence; } }

        public void AddWhitespaceRule(int index, WhitespaceRule rule) {
            WhitespaceRules[index].Add(rule);
        }

        public bool AddVarPar(VarPar var) {
            Debug.Assert(var.NodeType == XslNodeType.Variable || var.NodeType == XslNodeType.Param);
            Debug.Assert(var.Name.NamespaceUri != null, "Name must be resolved in XsltLoader");
            foreach (XslNode prevVar in GlobalVarPars) {
                if (prevVar.Name.Equals(var.Name)) {
                    // [ERR XT0630] It is a static error if a stylesheet contains more than one binding
                    // of a global variable with the same name and same import precedence, unless it also
                    // contains another binding with the same name and higher import precedence.
                    return compiler.AllGlobalVarPars.ContainsKey(var.Name);
                }
            }
            GlobalVarPars.Add(var);
            return true;
        }

        public bool AddTemplate(Template template) {
            Debug.Assert(template.ImportPrecedence == 0);

            template.ImportPrecedence = this.importPrecedence;
            template.OrderNumber      = this.orderNumber++;

            compiler.AllTemplates.Add(template);

            if (template.Name != null) {
                Template old;
                if (!compiler.NamedTemplates.TryGetValue(template.Name, out old)) {
                    compiler.NamedTemplates[template.Name] = template;
                } else {
                    Debug.Assert(template.ImportPrecedence <= old.ImportPrecedence, "Global objects are processed in order of decreasing import precedence");
                    if (old.ImportPrecedence == template.ImportPrecedence) {
                        return false;
                    }
                }
            }

            if (template.Match != null) {
                Templates.Add(template);
            }
            return true;
        }

        public void AddTemplateMatch(Template template, QilLoop filter) {
            List<TemplateMatch> matchesForMode;
            if (!TemplateMatches.TryGetValue(template.Mode, out matchesForMode)) {
                matchesForMode = TemplateMatches[template.Mode] = new List<TemplateMatch>();
            }
            matchesForMode.Add(new TemplateMatch(template, filter));
        }

        public void SortTemplateMatches() {
            foreach (QilName mode in TemplateMatches.Keys) {
                TemplateMatches[mode].Sort(TemplateMatch.Comparer);
            }
        }
    }
}
