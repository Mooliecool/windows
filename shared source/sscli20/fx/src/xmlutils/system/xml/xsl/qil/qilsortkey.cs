//------------------------------------------------------------------------------
// <copyright file="QilSortKey.cs" company="Microsoft">
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
    /// View over a Qil SortKey operator.
    /// </summary>
    /// <remarks>
    /// Don't construct QIL nodes directly; instead, use the <see cref="QilFactory">QilFactory</see>.
    /// </remarks>
    internal class QilSortKey : QilBinary {

        //-----------------------------------------------
        // Constructor
        //-----------------------------------------------

        /// <summary>
        /// Construct a new node
        /// </summary>
        public QilSortKey(QilNodeType nodeType, QilNode key, QilNode collation) : base(nodeType, key, collation) {
        }


        //-----------------------------------------------
        // QilSortKey methods
        //-----------------------------------------------

        public QilNode Key {
            get { return Left; }
            set { Left = value; }
        }

        public QilNode Collation {
            get { return Right; }
            set { Right = value; }
        }
    }
}
