//------------------------------------------------------------------------------
// <copyright file="ConformanceLevel.cs" company="Microsoft">
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

namespace System.Xml {

    public enum ConformanceLevel {
        // With conformance level Auto an XmlReader or XmlWriter automatically determines whether in incoming XML is an XML fragment or document.
        Auto = 0,

        // Conformance level for XML fragment. An XML fragment can contain any node type that can be a child of an element,
        // plus it can have a single XML declaration as its first node
        Fragment = 1,

        // Conformance level for XML document as specified in XML 1.0 Specification
        Document = 2,
    }
}
