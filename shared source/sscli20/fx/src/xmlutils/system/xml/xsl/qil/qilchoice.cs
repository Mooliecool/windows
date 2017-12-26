//------------------------------------------------------------------------------
// <copyright file="QilLoop.cs" company="Microsoft">
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
using System.Collections;
using System.Diagnostics;
using System.Xml.Schema;
using System.Xml.Xsl;

namespace System.Xml.Xsl.Qil {

    /// <summary>
    /// View over a Qil choice operator.
    /// </summary>
    /// <remarks>
    /// Don't construct QIL nodes directly; instead, use the <see cref="QilFactory">QilFactory</see>.
    /// </remarks>
    internal class QilChoice : QilBinary {

        //-----------------------------------------------
        // Constructor
        //-----------------------------------------------

        /// <summary>
        /// Construct a new node
        /// </summary>
        public QilChoice(QilNodeType nodeType, QilNode expression, QilNode branches) : base(nodeType, expression, branches) {
        }


        //-----------------------------------------------
        // QilChoice methods
        //-----------------------------------------------

        public QilNode Expression {
            get { return Left; }
            set { Left = value; }
        }

        public QilList Branches {
            get { return (QilList) Right; }
            set { Right = value; }
        }
    }
}
