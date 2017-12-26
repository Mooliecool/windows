//------------------------------------------------------------------------------
// <copyright file="IXmlSchemaInfo.cs" company="Microsoft">
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
// <owner current="true" primary="true">priyal</owner> 
//------------------------------------------------------------------------------

using System.Xml;
using System.Collections;

namespace System.Xml.Schema {

    /// <include file='doc\IXmlSchemaInfo.uex' path='docs/doc[@for="IXmlSchemaInfo"]/*' />
    public interface IXmlSchemaInfo {
        
        /// <include file='doc\IXmlSchemaInfo.uex' path='docs/doc[@for="IXmlSchemaInfo.Validity"]/*' />
        XmlSchemaValidity Validity { get; }

        /// <include file='doc\IXmlSchemaInfo.uex' path='docs/doc[@for="IXmlSchemaInfo.IsDefault"]/*' />
        bool IsDefault { get; }
        
        /// <include file='doc\IXmlSchemaInfo.uex' path='docs/doc[@for="IXmlSchemaInfo.IsNil"]/*' />
        bool IsNil { get; }

        /// <include file='doc\IXmlSchemaInfo.uex' path='docs/doc[@for="IXmlSchemaInfo.MemberType"]/*' />
        XmlSchemaSimpleType MemberType { get; }

        /// <include file='doc\IXmlSchemaInfo.uex' path='docs/doc[@for="IXmlSchemaInfo.SchemaType"]/*' />
        XmlSchemaType SchemaType { get; }

        /// <include file='doc\IXmlSchemaInfo.uex' path='docs/doc[@for="IXmlSchemaInfo.SchemaElement"]/*' />
        XmlSchemaElement SchemaElement { get; }

        /// <include file='doc\IXmlSchemaInfo.uex' path='docs/doc[@for="IXmlSchemaInfo.SchemaAttribute"]/*' />
        XmlSchemaAttribute SchemaAttribute { get; }
    }
}
