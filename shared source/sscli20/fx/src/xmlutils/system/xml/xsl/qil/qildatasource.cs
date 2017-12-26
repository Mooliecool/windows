//------------------------------------------------------------------------------
// <copyright file="QilDataSource.cs" company="Microsoft">
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
using System.Xml.Schema;
using System.Diagnostics;

namespace System.Xml.Xsl.Qil {

    /// <summary>
    /// View over a Qil DataSource operator.
    /// </summary>
    /// <remarks>
    /// Don't construct QIL nodes directly; instead, use the <see cref="QilFactory">QilFactory</see>.
    /// </remarks>
    internal class QilDataSource : QilBinary {

        //-----------------------------------------------
        // Constructor
        //-----------------------------------------------

        /// <summary>
        /// Construct a new node
        /// </summary>
        public QilDataSource(QilNodeType nodeType, QilNode name, QilNode baseUri) : base(nodeType, name, baseUri) {
        }


        //-----------------------------------------------
        // QilDataSource methods
        //-----------------------------------------------

        public QilNode Name {
            get { return Left; }
            set { Left = value; }
        }

        public QilNode BaseUri {
            get { return Right; }
            set { Right = value; }
        }
    }
}
