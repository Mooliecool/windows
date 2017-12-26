//------------------------------------------------------------------------------
// <copyright file="QilReference.cs" company="Microsoft">
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
    /// View over a Qil node which is the target of a reference (functions, variables, parameters).
    /// </summary>
    internal class QilReference : QilNode {
        private const int MaxDebugNameLength = 1023;

        private string debugName;

        //-----------------------------------------------
        // Constructor
        //-----------------------------------------------

        /// <summary>
        /// Construct a reference
        /// </summary>
        public QilReference(QilNodeType nodeType) : base(nodeType) {
        }


        //-----------------------------------------------
        // QilReference methods
        //-----------------------------------------------

        /// <summary>
        /// Name of this reference, preserved for debugging (may be null).
        /// </summary>
        public string DebugName {
            get { return this.debugName; }
            set {
                if (value.Length > MaxDebugNameLength)
                    value = value.Substring(0, MaxDebugNameLength);

                this.debugName = value;
            }
        }
    }
}
