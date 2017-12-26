//------------------------------------------------------------------------------
// <copyright file="ResetableIterator.cs" company="Microsoft">
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

    internal abstract class ResetableIterator : XPathNodeIterator {
        // the best place for this constructors to be is XPathNodeIterator, to avoid DCR at this time let's ground them here
        public ResetableIterator() {
            base.count = -1;
        }
        protected ResetableIterator(ResetableIterator other) {
            base.count = other.count;
        }
        protected void ResetCount() { 
            base.count = -1; 
        }

        public abstract void Reset();
        public virtual bool MoveToPosition(int pos) {
            Reset();
            for(int i = CurrentPosition; i < pos ; i ++) {
                if(!MoveNext()) {
                    return false;
                }
            }
            return true;
        }

        // Contruct extension: CurrentPosition should return 0 if MoveNext() wasn't called after Reset()
        // (behavior is not defined for XPathNodeIterator)
        public abstract override int CurrentPosition { get; }
    }
}
