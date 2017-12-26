//------------------------------------------------------------------------------
// <copyright file="ForwardPositionQuery.cs" company="Microsoft">
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

    internal class ForwardPositionQuery : CacheOutputQuery {
        
        public ForwardPositionQuery(Query input) : base(input) {
            Debug.Assert(input != null);
        }
        protected ForwardPositionQuery(ForwardPositionQuery other) : base(other) { }       
        
        public override object Evaluate(XPathNodeIterator context) {
            base.Evaluate(context);

            XPathNavigator node;
            while ((node = base.input.Advance()) != null) {
                outputBuffer.Add(node.Clone());
            }

            return this;
        }

        public override XPathNavigator MatchNode(XPathNavigator context) {
            return input.MatchNode(context);
        }

        public override XPathNodeIterator Clone() { return new ForwardPositionQuery(this); }
    }
}





