//------------------------------------------------------------------------------
// <copyright file="QilTargetType.cs" company="Microsoft">
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
    /// View over a Qil operator having two children, the second of which is a literal type.
    /// </summary>
    /// <remarks>
    /// Don't construct QIL nodes directly; instead, use the <see cref="QilFactory">QilFactory</see>.
    /// </remarks>
    internal class QilTargetType : QilBinary {

        //-----------------------------------------------
        // Constructor
        //-----------------------------------------------

        /// <summary>
        /// Construct a new node
        /// </summary>
        public QilTargetType(QilNodeType nodeType, QilNode expr, QilNode targetType) : base(nodeType, expr, targetType) {
        }


        //-----------------------------------------------
        // QilTargetType methods
        //-----------------------------------------------

        public QilNode Source {
            get { return Left; }
            set { Left = value; }
        }

        public XmlQueryType TargetType {
            get { return (XmlQueryType) ((QilLiteral) Right).Value; }
            set { ((QilLiteral) Right).Value = value; }
        }
    }
}

