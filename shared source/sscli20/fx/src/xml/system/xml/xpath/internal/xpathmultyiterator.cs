//------------------------------------------------------------------------------
// <copyright file="XPathMultyIterator.cs" company="Microsoft">
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

    internal class XPathMultyIterator: ResetableIterator {
        protected ResetableIterator[] arr;
        protected int                 firstNotEmpty;
        protected int                 position;

        public XPathMultyIterator(ArrayList inputArray) {
            // NOTE: We do not clone the passed inputArray supposing that it is not changed outside of this class
            this.arr = new ResetableIterator[inputArray.Count];
            for (int i = 0; i < this.arr.Length; i ++) {
                this.arr[i] = new XPathArrayIterator((ArrayList) inputArray[i]);
            }
            Init();
        }

        private void Init() {
            for (int i = 0; i < arr.Length; i ++) {
                Advance(i);
            }
            for (int i = arr.Length - 2; firstNotEmpty <= i; ) {
                if (SiftItem(i)) {
                    i --;
                }
            }
        }

        // returns false is iterator at pos reached it's end & as a result head of the array may be moved
        bool Advance(int pos) {
            if (! arr[pos].MoveNext()) {
                if (firstNotEmpty != pos) {
                    ResetableIterator empty = arr[pos];
                    Array.Copy(arr, firstNotEmpty, arr, firstNotEmpty + 1, pos - firstNotEmpty);
                    arr[firstNotEmpty] = empty;
                }
                firstNotEmpty ++;
                return false;
            }
            return true;
        }


        // Invariant: a[i] < a[i+1] for i > item
        // returns flase is head of the list was moved & as a result consistancy of list depends on head consistancy.
        bool SiftItem(int item) {
            Debug.Assert(firstNotEmpty <= item && item < arr.Length);
            ResetableIterator it = arr[item];
            while (item + 1 < arr.Length) {
                XmlNodeOrder order = Query.CompareNodes(it.Current, arr[item + 1].Current);
                if (order == XmlNodeOrder.Before) {
                    break;
                }
                if (order == XmlNodeOrder.After) {
                    arr[item] = arr[item + 1];
                    //arr[item + 1] = it;
                    item ++;
                } else { // Same
                    arr[item] = it;
                    if (! Advance(item)) {
                        return false;
                    }
                    it = arr[item];
                }
            }
            arr[item] = it;
            return true;
        }

        public override void Reset() {
            firstNotEmpty = 0;
            position      = 0;
            for (int i = 0; i < arr.Length; i ++) {
                arr[i].Reset();
            }
            Init();
        }

        public XPathMultyIterator(XPathMultyIterator it) {
            this.arr           = (ResetableIterator[]) it.arr.Clone();
            this.firstNotEmpty = it.firstNotEmpty;
            this.position      = it.position;
        }

        public override XPathNodeIterator Clone() {
            return new XPathMultyIterator(this);
        }

        public override XPathNavigator Current {
            get {
                Debug.Assert(position != 0, "MoveNext() wasn't called");
                Debug.Assert(firstNotEmpty < arr.Length, "MoveNext() returned false");
                return arr[firstNotEmpty].Current;
            }
        }

        public override int CurrentPosition { get { return position; } }

        public override bool MoveNext() {
            // NOTE: MoveNext() may be called even if the previous call to MoveNext() returned false, SQLBUDT 330810
            if (firstNotEmpty >= arr.Length) {
                return false;
            }
            if (position != 0) {
                if (Advance(firstNotEmpty)) {
                    SiftItem(firstNotEmpty);
                }
                if (firstNotEmpty >= arr.Length) {
                    return false;
                }
            }
            position ++;
            return true;
        }
    }
}
