//------------------------------------------------------------------------------
// <copyright file="QilIterator.cs" company="Microsoft">
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
using System;
using System.Diagnostics;

namespace System.Xml.Xsl.Qil {

    /// <summary>
    /// View over a Qil iterator node (For or Let).
    /// </summary>
    internal class QilIterator : QilReference {
        private QilNode binding;

        //-----------------------------------------------
        // Constructor
        //-----------------------------------------------

        /// <summary>
        /// Construct an iterator
        /// </summary>
        public QilIterator(QilNodeType nodeType, QilNode binding) : base(nodeType) {
            Binding = binding;
        }


        //-----------------------------------------------
        // IList<QilNode> methods -- override
        //-----------------------------------------------

        public override int Count {
            get { return 1; }
        }

        public override QilNode this[int index] {
            get { if (index != 0) throw new IndexOutOfRangeException(); return this.binding; }
            set { if (index != 0) throw new IndexOutOfRangeException(); this.binding = value; }
        }


        //-----------------------------------------------
        // QilIterator methods
        //-----------------------------------------------

        /// <summary>
        /// Expression which is bound to the iterator.
        /// </summary>
        public QilNode Binding {
            get { return this.binding; }
            set { this.binding = value; }
        }
    }
}
