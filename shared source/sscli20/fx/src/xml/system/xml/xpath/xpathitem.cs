//------------------------------------------------------------------------------
// <copyright file="XPathItem.cs" company="Microsoft">
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
// <owner current="true" primary="true">akimball</owner> 
//------------------------------------------------------------------------------
using System;
using System.Collections;
using System.Xml.Schema;

namespace System.Xml.XPath {
    /// <summary>
    /// Base class for XPathNavigator and XmlAtomicValue.
    /// </summary>
    public abstract class XPathItem {
        /// <summary>
        /// True if this item is a node, and not an atomic value.
        /// </summary>
        public abstract bool IsNode { get; }

        /// <summary>
        /// Returns Xsd type of atomic value, or of node's content.
        /// </summary>
        public abstract XmlSchemaType XmlType { get; }

        /// <summary>
        /// Typed and untyped value accessors.
        /// </summary>
        public abstract string Value { get; }
        public abstract object TypedValue { get; }
        public abstract Type ValueType { get; }
        public abstract bool ValueAsBoolean { get; }
        public abstract DateTime ValueAsDateTime { get; }
        public abstract double ValueAsDouble { get; }
        public abstract int ValueAsInt { get; }
        public abstract long ValueAsLong { get; }
        public virtual object ValueAs(Type returnType) { return ValueAs(returnType, null); }
        public abstract object ValueAs(Type returnType, IXmlNamespaceResolver nsResolver);
    }
}

