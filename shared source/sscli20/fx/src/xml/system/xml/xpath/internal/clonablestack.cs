//------------------------------------------------------------------------------
// <copyright file="ClonableStack.cs" company="Microsoft">
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

    internal sealed class ClonableStack<T> : System.Collections.Generic.List<T> {

        public ClonableStack() {}
        public ClonableStack(int capacity) : base(capacity) {}

        private ClonableStack(System.Collections.Generic.IEnumerable<T> collection) : base(collection) { }

        public void Push(T value) {
            base.Add(value);
        }

        public T Pop() {
            int last = base.Count - 1;
            T result = base[last];
            base.RemoveAt(last);
            return result;
        }

        public T Peek() {
            return base[base.Count - 1];
        }

        public ClonableStack<T> Clone() { return new ClonableStack<T>(this); }
    }
}
