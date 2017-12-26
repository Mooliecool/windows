//------------------------------------------------------------------------------
// <copyright file="CacheOutputQuery.cs" company="Microsoft">
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
    using System.Xml.Xsl;
    using System.Collections.Generic;

    internal abstract class CacheOutputQuery : Query {
        internal Query input;
        // int count; -- we reusing it here
        protected List<XPathNavigator> outputBuffer;

        public CacheOutputQuery(Query input) {
            this.input = input;
            this.outputBuffer = new List<XPathNavigator>();
            this.count = 0;
        }
        protected CacheOutputQuery(CacheOutputQuery other) : base(other) {
            this.input        = Clone(other.input);
            this.outputBuffer = new List<XPathNavigator>(other.outputBuffer);
            this.count = other.count;
        }

        public override void Reset() {
            this.count = 0;
        }

        public override void SetXsltContext(XsltContext context){
            input.SetXsltContext(context);
        }

        public override object Evaluate(XPathNodeIterator context) {            
            outputBuffer.Clear();
            count = 0;
            return input.Evaluate(context);// This is trick. IDQuery needs this value. Otherwise we would return this.
                                            // All subclasses should and would anyway override thismethod and return this.
        }

        public override XPathNavigator Advance() {
            Debug.Assert(0 <= count && count <= outputBuffer.Count);
            if (count < outputBuffer.Count) {
                return outputBuffer[count++];
            }
            return null;
        }
        
        public override XPathNavigator Current { 
            get {
                Debug.Assert(0 <= count && count <= outputBuffer.Count);
                if (count == 0) {
                    return null;
                }
                return outputBuffer[count - 1];
            } 
        }

        public override XPathResultType StaticType { get { return XPathResultType.NodeSet; } }
        public override int CurrentPosition   { get { return count; } }
        public override int Count             { get { return outputBuffer.Count; } }
        public override QueryProps Properties { get { return QueryProps.Merge | QueryProps.Cached | QueryProps.Position | QueryProps.Count; } }

        public override void PrintQuery(XmlWriter w) {
            w.WriteStartElement(this.GetType().Name);
            input.PrintQuery(w);
            w.WriteEndElement();
        }
    }
}
