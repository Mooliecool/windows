//------------------------------------------------------------------------------
// <copyright file="NamespaceDecl.cs" company="Microsoft">
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
    using System.Xml;

    internal class NamespaceDecl {
        private string        prefix;
        private string        nsUri;
        private string        prevDefaultNsUri;
        private NamespaceDecl next;

        internal string Prefix {
            get { return this.prefix; }
        }

        internal string Uri {
            get { return this.nsUri; }
        }

        internal string PrevDefaultNsUri {
            get { return this.prevDefaultNsUri; }
        }

        internal NamespaceDecl Next {
            get { return this.next; }
        }

        internal NamespaceDecl(string prefix, string nsUri, string prevDefaultNsUri, NamespaceDecl next) {
            Init(prefix, nsUri, prevDefaultNsUri, next);
        }

        internal void Init(string prefix, string nsUri, string prevDefaultNsUri, NamespaceDecl next) {
            this.prefix           = prefix;
            this.nsUri            = nsUri;
            this.prevDefaultNsUri = prevDefaultNsUri;
            this.next             = next;
        }
    }
}
