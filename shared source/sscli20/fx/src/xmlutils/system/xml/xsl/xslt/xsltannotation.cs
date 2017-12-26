//------------------------------------------------------------------------------
// <copyright file="XsltAnnotation.cs" company="Microsoft">
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
using System.Reflection;
using System.Xml.Xsl.Qil;

namespace System.Xml.Xsl.Xslt {

    /// <summary>
    /// Several annotations are created and attached to Qil nodes during Xslt compilation.
    /// </summary>
    internal class XsltAnnotation : ListBase<object> {
        private object arg0, arg1, arg2;


        //-----------------------------------------------
        // Constructor
        //-----------------------------------------------

        /// <summary>
        /// Create and initialize XsltAnnotation for the specified node.
        /// Allow properties to be read and written.
        /// </summary>
        public static XsltAnnotation Write(QilNode nd) {
            XsltAnnotation ann = nd.Annotation as XsltAnnotation;

            if (ann == null) {
                ann = new XsltAnnotation();
                nd.Annotation = ann;
            }

            return ann;
        }

        private XsltAnnotation() {
        }


        //-----------------------------------------------
        // ListBase implementation
        //-----------------------------------------------

        /// <summary>
        /// List of annotations can be updated.
        /// </summary>
        public override bool IsReadOnly {
            get { return false; }
        }

        /// <summary>
        /// Return the count of sub-annotations maintained by this annotation.
        /// </summary>
        public override int Count {
            get { return 3; }
        }

        /// <summary>
        /// Return the annotation at the specified index.
        /// </summary>
        public override object this[int index] {
            get {
                switch (index) {
                    case 0: return this.arg0;
                    case 1: return this.arg1;
                    case 2: return this.arg2;
                }

                throw new IndexOutOfRangeException();
            }
            set {
                switch (index) {
                    case 0: this.arg0 = value; return;
                    case 1: this.arg1 = value; return;
                    case 2: this.arg2 = value; return;
                }
                throw new IndexOutOfRangeException();
            }
        }
    }
}

