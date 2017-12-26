//------------------------------------------------------------------------------
// <copyright file="EarlyBoundInfo.cs" company="Microsoft">
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

namespace System.Xml.Xsl.Runtime {

    /// <summary>
    /// This class contains information about early bound function objects.
    /// </summary>
    internal sealed class EarlyBoundInfo {
        private string namespaceUri;            // Namespace Uri mapped to these early bound functions
        private ConstructorInfo constrInfo;     // Constructor for the early bound function object

        public EarlyBoundInfo(string namespaceUri, ConstructorInfo constrInfo) {
            Debug.Assert(namespaceUri != null && constrInfo != null);
            this.namespaceUri = namespaceUri;
            this.constrInfo = constrInfo;
        }

        /// <summary>
        /// Get the Namespace Uri mapped to these early bound functions.
        /// </summary>
        public string NamespaceUri { get { return this.namespaceUri; } }

        /// <summary>
        /// Return the Clr Type of the early bound object
        /// </summary>
        public Type EarlyBoundType { get { return this.constrInfo.DeclaringType; } }

        /// <summary>
        /// Create an instance of the early bound object.
        /// </summary>
        public object CreateObject() { return this.constrInfo.Invoke(new object[] {}); }

        /// <summary>
        /// Override Equals method so that EarlyBoundInfo to implement value comparison.
        /// </summary>
        public override bool Equals(object obj) {
            EarlyBoundInfo info = obj as EarlyBoundInfo;
            if (info == null)
                return false;

            return this.namespaceUri == info.namespaceUri && this.constrInfo == info.constrInfo;
        }

        /// <summary>
        /// Override GetHashCode since Equals is overriden.
        /// </summary>
        public override int GetHashCode() {
            return this.namespaceUri.GetHashCode();
        }
    }
}

