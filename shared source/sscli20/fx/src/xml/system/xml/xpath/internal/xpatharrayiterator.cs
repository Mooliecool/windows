//------------------------------------------------------------------------------
// <copyright file="XPathArrayIterator.cs" company="Microsoft">
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
    using System.Collections;

    [DebuggerDisplay("Position={CurrentPosition}, Current={index < 1 ? null : (object) new System.Xml.XPath.XPathNavigator.DebuggerDisplayProxy(Current)}")]
    internal class XPathArrayIterator : ResetableIterator {
        protected IList     list;
        protected int       index;

        public XPathArrayIterator(IList list) {
            this.list = list;
        }

        public XPathArrayIterator(XPathArrayIterator it) {
            this.list = it.list;
            this.index = it.index;
        }

        public XPathArrayIterator(XPathNodeIterator nodeIterator) {
            this.list = new ArrayList();
            while (nodeIterator.MoveNext()) {
                this.list.Add(nodeIterator.Current.Clone());
            }
        }

        public IList AsList {
            get { return this.list; }
        }

        public override XPathNodeIterator Clone() {
            return new XPathArrayIterator(this);
        }

        public override XPathNavigator Current {
            get {
                Debug.Assert(index <= list.Count);

                if (index < 1)
                    throw new InvalidOperationException(Res.GetString(Res.Sch_EnumNotStarted, string.Empty));

                return (XPathNavigator) list[index - 1];
            }
        }

        public override int CurrentPosition { get { return index; } }
        public override int Count           { get { return list.Count; } }

        public override bool MoveNext() {
            Debug.Assert(index <= list.Count);
            if (index == list.Count) {
                return false;
            }
            index++;
            return true;
        }

        public override void Reset() {
            index = 0;
        }

        public override IEnumerator GetEnumerator() {
            return list.GetEnumerator();
        }
    }
}
