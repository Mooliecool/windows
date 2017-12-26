//------------------------------------------------------------------------------
// <copyright file="QilInvokeLateBound.cs" company="Microsoft">
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
    /// A function invocation node which reperesents a call to an late bound function.
    /// </summary>
    internal class QilInvokeLateBound : QilBinary {

        //-----------------------------------------------
        // Constructor
        //-----------------------------------------------

        /// <summary>
        /// Construct a new node
        /// </summary>
        public QilInvokeLateBound(QilNodeType nodeType, QilNode name, QilNode arguments) : base(nodeType, name, arguments) {
        }


        //-----------------------------------------------
        // QilInvokeLateBound methods
        //-----------------------------------------------

        public QilName Name {
            get { return (QilName) Left; }
            set { Left = value; }
        }

        public QilList Arguments {
            get { return (QilList) Right; }
            set { Right = value; }
        }
    }
}
