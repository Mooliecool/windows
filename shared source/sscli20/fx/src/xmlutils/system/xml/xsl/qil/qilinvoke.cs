//------------------------------------------------------------------------------
// <copyright file="QilInvoke.cs" company="Microsoft">
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
using System.Xml.Schema;
using System.Xml.Xsl;

namespace System.Xml.Xsl.Qil {

    /// <summary>
    /// A function invocation node which represents a call to a Qil functions.
    /// </summary>
    internal class QilInvoke : QilBinary {

        //-----------------------------------------------
        // Constructor
        //-----------------------------------------------

        /// <summary>
        /// Construct a new node
        /// </summary>
        public QilInvoke(QilNodeType nodeType, QilNode function, QilNode arguments) : base(nodeType, function, arguments) {
        }


        //-----------------------------------------------
        // QilInvoke methods
        //-----------------------------------------------

        public QilFunction Function {
            get { return (QilFunction) Left; }
            set { Left = value; }
        }

        public QilList Arguments {
            get { return (QilList) Right; }
            set { Right = value; }
        }
    }
}
