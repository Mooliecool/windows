//------------------------------------------------------------------------------
// <copyright file="QilLiteral.cs" company="Microsoft">
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
    /// View over a Qil atomic value literal (of any type).
    /// </summary>
    /// <remarks>
    /// Don't construct QIL nodes directly; instead, use the <see cref="QilFactory">QilFactory</see>.
    /// </remarks>
    internal class QilLiteral : QilNode {
        private object value;


        //-----------------------------------------------
        // Constructor
        //-----------------------------------------------

        /// <summary>
        /// Construct a new node
        /// </summary>
        public QilLiteral(QilNodeType nodeType, object value) : base(nodeType) {
            Value = value;
        }


        //-----------------------------------------------
        // QilLiteral methods
        //-----------------------------------------------

        public object Value {
            get { return this.value; }
            set { this.value = value; }
        }

        public static implicit operator string(QilLiteral literal) {
            return (string) literal.value;
        }

        public static implicit operator int(QilLiteral literal) {
            return (int) literal.value;
        }

        public static implicit operator long(QilLiteral literal) {
            return (long) literal.value;
        }

        public static implicit operator double(QilLiteral literal) {
            return (double) literal.value;
        }

        public static implicit operator decimal(QilLiteral literal) {
            return (decimal) literal.value;
        }

        public static implicit operator XmlQueryType(QilLiteral literal) {
            return (XmlQueryType) literal.value;
        }
    }
}
