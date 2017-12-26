//------------------------------------------------------------------------------
// <copyright file="ParentQuery.cs" company="Microsoft">
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

namespace MS.Internal.Xml.XPath {
    using System;
    using System.Xml;
    using System.Xml.XPath;
    using System.Diagnostics;
    using System.Globalization;
    using System.Collections.Generic;

    internal sealed class ParentQuery : CacheAxisQuery {

        public ParentQuery(Query  qyInput, string Name, string Prefix, XPathNodeType Type) : base(qyInput, Name, Prefix, Type) {}
        private ParentQuery(ParentQuery other) : base(other) { }

        public override object Evaluate(XPathNodeIterator context) {
            base.Evaluate(context);

            XPathNavigator input;
            while ((input = qyInput.Advance()) != null) {
                input = input.Clone();

                if (input.MoveToParent()) {
                    if (matches(input)) {
                        Insert(outputBuffer, input);
                    }
                }
            }
            return this;
        }

        public override XPathNodeIterator Clone() { return new ParentQuery(this); }        
    }
}
