//------------------------------------------------------------------------------
// <copyright file="PrefixQName.cs" company="Microsoft">
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

    internal sealed class PrefixQName {
        public string Prefix;
        public string Name;
        public string Namespace;

        internal void ClearPrefix() {
            Prefix = string.Empty;
        }

        internal void SetQName(string qname) {
            PrefixQName.ParseQualifiedName(qname, out Prefix, out Name);
        }

        //
        // Parsing qualified names
        //

        private static string ParseNCName(string qname, ref int position) {
            int qnameLength = qname.Length;
            int nameStart = position;
            XmlCharType xmlCharType = XmlCharType.Instance;

            if (
                qnameLength == position ||                           // Zero length ncname
                ! xmlCharType.IsStartNCNameChar(qname[position])     // Start from invalid char
            ) {
                throw XsltException.Create(Res.Xslt_InvalidQName, qname);
            }

            position ++;

            while (position < qnameLength && xmlCharType.IsNCNameChar(qname[position])) {
                position ++;
            }

            return qname.Substring(nameStart, position - nameStart);
        }

        public static void ParseQualifiedName(string qname, out string prefix, out string local) {
            Debug.Assert(qname != null);
            prefix = string.Empty;
            local  = string.Empty;
            int position    = 0;

            local = ParseNCName(qname, ref position);

            if (position < qname.Length) {
                if (qname[position] == ':') {
                    position ++;
                    prefix = local;
                    local  = ParseNCName(qname, ref position);
                }

                if (position < qname.Length) {
                    throw XsltException.Create(Res.Xslt_InvalidQName, qname);
                }
            }
        }

        public static bool ValidatePrefix(string prefix) {
            if (prefix.Length == 0) {
                return false;
            }
            XmlCharType xmlCharType = XmlCharType.Instance;
            if (! xmlCharType.IsStartNCNameChar(prefix[0])) {
                return false;
            }
            for (int i = 1; i < prefix.Length; i ++) {
                if (! xmlCharType.IsNCNameChar(prefix[i])) {
                    return false;
                }
            }
            return true;
        }
    }
}
