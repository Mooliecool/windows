//------------------------------------------------------------------------------
// <copyright file="IXmlSerializable.cs" company="Microsoft">
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
// <owner current="true" primary="true">ElenaK</owner>                                                                
//------------------------------------------------------------------------------

namespace System.Xml.Serialization {

    using System.Xml.Schema;

    /// <include file='doc\IXmlSerializable.uex' path='docs/doc[@for="IXmlSerializable"]/*' />
    ///<internalonly/>
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public interface IXmlSerializable {
        /// <include file='doc\IXmlSerializable.uex' path='docs/doc[@for="IXmlSerializable.GetSchema"]/*' />
        XmlSchema GetSchema();
        /// <include file='doc\IXmlSerializable.uex' path='docs/doc[@for="IXmlSerializable.ReadXml"]/*' />
        void ReadXml(XmlReader reader);
        /// <include file='doc\IXmlSerializable.uex' path='docs/doc[@for="IXmlSerializable.WriteXml"]/*' />
        void WriteXml(XmlWriter writer);
    }

}
