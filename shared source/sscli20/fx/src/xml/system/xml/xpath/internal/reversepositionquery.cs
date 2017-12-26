//------------------------------------------------------------------------------
// <copyright file="ReversePositionQuery.cs" company="Microsoft">
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

    internal sealed class ReversePositionQuery : ForwardPositionQuery {

        public ReversePositionQuery(Query input) : base(input) { }
        private ReversePositionQuery(ReversePositionQuery other) : base(other) { }
        
        public override XPathNodeIterator Clone() { return new ReversePositionQuery(this); }
        public override int CurrentPosition { get { return outputBuffer.Count - count + 1; } }
        public override QueryProps Properties { get { return base.Properties | QueryProps.Reverse; } }
    }
}





