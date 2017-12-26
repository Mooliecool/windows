//------------------------------------------------------------------------------
// <copyright file="QilInvokeEarlyBound.cs" company="Microsoft">
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
using System.Reflection;

namespace System.Xml.Xsl.Qil {

    /// <summary>
    /// A function invocation node which reperesents a call to an early bound Clr function.
    /// </summary>
    internal class QilInvokeEarlyBound : QilTernary {

        //-----------------------------------------------
        // Constructor
        //-----------------------------------------------

        /// <summary>
        /// Construct a new node
        /// </summary>
        /// <param name="method">QilLiteral containing the Clr MethodInfo for the early bound function</param>
        public QilInvokeEarlyBound(QilNodeType nodeType, QilNode name, QilNode method, QilNode arguments, XmlQueryType resultType)
            : base(nodeType, name, method, arguments) {
            this.xmlType = resultType;
        }


        //-----------------------------------------------
        // QilInvokeEarlyBound methods
        //-----------------------------------------------

        public QilName Name {
            get { return (QilName) Left; }
            set { Left = value; }
        }

        public MethodInfo ClrMethod {
            get { return (MethodInfo) ((QilLiteral) Center).Value; }
            set { ((QilLiteral) Center).Value = value; }
        }

        public QilList Arguments {
            get { return (QilList) Right; }
            set { Right = value; }
        }
    }
}

