//------------------------------------------------------------------------------
// <copyright file="QilParameter.cs" company="Microsoft">
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
    /// View over a Qil parameter node.
    /// </summary>
    internal class QilParameter : QilIterator {
        private QilNode name;

        //-----------------------------------------------
        // Constructor
        //-----------------------------------------------

        /// <summary>
        /// Construct a parameter
        /// </summary>
        public QilParameter(QilNodeType nodeType, QilNode defaultValue, QilNode name, XmlQueryType xmlType) : base(nodeType, defaultValue) {
            this.name = name;
            this.xmlType = xmlType;
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
                    case 0: return Binding;
                    case 1: return this.name;
                    default: throw new IndexOutOfRangeException();
                }
            }
            set {
                switch (index) {
                    case 0: Binding = value; break;
                    case 1: this.name = value; break;
                    default: throw new IndexOutOfRangeException();
                }
            }
        }


        //-----------------------------------------------
        // QilParameter methods
        //-----------------------------------------------

        /// <summary>
        /// Default value expression of this parameter (may be null).
        /// </summary>
        public QilNode DefaultValue {
            get { return Binding; }
            set { Binding = value; }
        }

        /// <summary>
        /// Name of this parameter (may be null).
        /// </summary>
        public QilName Name {
            get { return (QilName) this.name; }
            set { this.name = value; }
        }
    }
}

