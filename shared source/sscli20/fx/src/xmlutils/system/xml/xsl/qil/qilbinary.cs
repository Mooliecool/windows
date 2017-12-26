//------------------------------------------------------------------------------
// <copyright file="QilBinary.cs" company="Microsoft">
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
using System.Collections.Generic;
using System.Diagnostics;

namespace System.Xml.Xsl.Qil {

    /// <summary>
    /// View over a Qil operator having two children.
    /// </summary>
    /// <remarks>
    /// Don't construct QIL nodes directly; instead, use the <see cref="QilFactory">QilFactory</see>.
    /// </remarks>
    internal class QilBinary : QilNode {
        private QilNode left, right;


        //-----------------------------------------------
        // Constructor
        //-----------------------------------------------

        /// <summary>
        /// Construct a new node
        /// </summary>
        public QilBinary(QilNodeType nodeType, QilNode left, QilNode right) : base(nodeType) {
            this.left = left;
            this.right = right;
        }


        //-----------------------------------------------
        // IList<QilNode> methods -- override
        //-----------------------------------------------

        public override int Count {
            get { return 2; }
        }

        public override QilNode this[int index] {
            get {
                switch (index) {
                    case 0: return this.left;
                    case 1: return this.right;
                    default: throw new IndexOutOfRangeException();
                }
            }
            set {
                switch (index) {
                    case 0: this.left = value; break;
                    case 1: this.right = value; break;
                    default: throw new IndexOutOfRangeException();
                }
            }
        }


        //-----------------------------------------------
        // QilBinary methods
        //-----------------------------------------------

        public QilNode Left {
            get { return this.left; }
            set { this.left = value; }
        }

        public QilNode Right {
            get { return this.right; }
            set { this.right = value; }
        }
    }
}
