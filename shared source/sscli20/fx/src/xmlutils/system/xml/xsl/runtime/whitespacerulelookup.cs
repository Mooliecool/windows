//------------------------------------------------------------------------------
// <copyright file="WhitespaceRuleLookup.cs" company="Microsoft">
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
using System;
using System.Xml;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using MS.Internal.Xml;
using System.Xml.Xsl.Qil;

namespace System.Xml.Xsl.Runtime {

    /// <summary>
    /// This class keeps a list of whitespace rules in order to determine whether whitespace children of particular
    /// elements should be stripped.
    /// </summary>
    internal class WhitespaceRuleLookup {
        private Hashtable qnames;
        private ArrayList wildcards;
        private InternalWhitespaceRule ruleTemp;
        private XmlNameTable nameTable;

        public WhitespaceRuleLookup() {
            this.qnames = new Hashtable();
            this.wildcards = new ArrayList();
        }

        /// <summary>
        /// Create a new lookup internal class from the specified WhitespaceRules.  Use the name table to atomize all
        /// names for fast comparison.
        /// </summary>
        public WhitespaceRuleLookup(IList<WhitespaceRule> rules) : this() {
            WhitespaceRule rule;
            InternalWhitespaceRule ruleInternal;
            Debug.Assert(rules != null);

            for (int i = rules.Count - 1; i >= 0; i--) {
                // Make a copy of each rule, but atomize each name
                rule = rules[i];
                ruleInternal = new InternalWhitespaceRule(rule.LocalName, rule.NamespaceName, rule.PreserveSpace, -i);

                if (rule.LocalName == null || rule.NamespaceName == null) {
                    // Wildcard, so add to wildcards array
                    this.wildcards.Add(ruleInternal);
                }
                else {
                    // Exact name, so add to hashtable
                    this.qnames[ruleInternal] = ruleInternal;
                }
            }

            // Create a temporary (not thread-safe) InternalWhitespaceRule used for lookups
            this.ruleTemp = new InternalWhitespaceRule();
        }

        /// <summary>
        /// Atomize all names contained within the whitespace rules with respect to "nameTable".
        /// </summary>
        public void Atomize(XmlNameTable nameTable) {
            // If names are already atomized with respect to "nameTable", no need to do it again
            if (nameTable != this.nameTable) {
                this.nameTable = nameTable;

                foreach (InternalWhitespaceRule rule in this.qnames.Values)
                    rule.Atomize(nameTable);

                foreach (InternalWhitespaceRule rule in this.wildcards)
                    rule.Atomize(nameTable);
            }
        }

        /// <summary>
        /// Return true if elements of the specified name should have whitespace children stripped.
        /// NOTE: This method is not thread-safe.  Different threads should create their own copy of the
        /// WhitespaceRuleLookup object.  This allows all names to be atomized according to a private NameTable.
        /// </summary>
        public bool ShouldStripSpace(string localName, string namespaceName) {
            InternalWhitespaceRule qnameRule, wildcardRule;
            Debug.Assert(this.nameTable != null && this.ruleTemp != null);
            Debug.Assert(localName != null && (object) this.nameTable.Get(localName) == (object) localName);
            Debug.Assert(namespaceName != null && (object) this.nameTable.Get(namespaceName) == (object) namespaceName);

            this.ruleTemp.Init(localName, namespaceName, false, 0);

            // Lookup name in qnames table
            // If found, the name will be stripped unless there is a preserve wildcard with higher priority
            qnameRule = this.qnames[this.ruleTemp] as InternalWhitespaceRule;

            for (int pos = this.wildcards.Count; pos-- != 0;) {
                wildcardRule = this.wildcards[pos] as InternalWhitespaceRule;

                if (qnameRule != null) {
                    // If qname priority is greater than any subsequent wildcard's priority, then we're done
                    if (qnameRule.Priority > wildcardRule.Priority)
                        return !qnameRule.PreserveSpace;

                    if (qnameRule.PreserveSpace == wildcardRule.PreserveSpace)
                        continue;
                }

                if (wildcardRule.LocalName == null || (object) wildcardRule.LocalName == (object) localName) {
                    if (wildcardRule.NamespaceName == null || (object) wildcardRule.NamespaceName == (object) namespaceName) {
                        // Found wildcard match, so we're done (since wildcards are in priority order)
                        return !wildcardRule.PreserveSpace;
                    }
                }
            }

            return (qnameRule != null && !qnameRule.PreserveSpace);
        }

        private class InternalWhitespaceRule : WhitespaceRule {
            private int priority;       // Relative priority of this test
            private int hashCode;       // Cached hashcode

            public InternalWhitespaceRule() {
            }

            public InternalWhitespaceRule(string localName, string namespaceName, bool preserveSpace, int priority) {
                Init(localName, namespaceName, preserveSpace, priority);
            }

            public void Init(string localName, string namespaceName, bool preserveSpace, int priority) {
                base.Init(localName, namespaceName, preserveSpace);
                this.priority = priority;

                if (localName != null && namespaceName != null) {
                    this.hashCode = localName.GetHashCode();
                }
            }

            public void Atomize(XmlNameTable nameTable) {
                if (LocalName != null)
                    LocalName = nameTable.Add(LocalName);

                if (NamespaceName != null)
                    NamespaceName = nameTable.Add(NamespaceName);
            }

            public int Priority {
                get { return this.priority; }
            }

            public override int GetHashCode() {
                return this.hashCode;
            }

            public override bool Equals(object obj) {
                Debug.Assert(obj is InternalWhitespaceRule);
                InternalWhitespaceRule that = obj as InternalWhitespaceRule;

                Debug.Assert(LocalName != null && that.LocalName != null);
                Debug.Assert(NamespaceName != null && that.NamespaceName != null);
                return (object) LocalName == (object) LocalName && (object) NamespaceName == (object) that.NamespaceName;
            }
        }
    }
}
