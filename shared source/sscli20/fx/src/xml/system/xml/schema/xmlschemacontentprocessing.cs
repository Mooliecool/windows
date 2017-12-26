//------------------------------------------------------------------------------
// <copyright file="XmlSchemaContentProcessing.cs" company="Microsoft">
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

namespace System.Xml.Schema {

    using System.Xml.Serialization;

    /// <include file='doc\XmlSchemaContentProcessing.uex' path='docs/doc[@for="XmlSchemaContentProcessing"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public enum XmlSchemaContentProcessing {
        /// <include file='doc\XmlSchemaContentProcessing.uex' path='docs/doc[@for="XmlSchemaContentProcessing.None"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlIgnore]
        None,
        /// <include file='doc\XmlSchemaContentProcessing.uex' path='docs/doc[@for="XmlSchemaContentProcessing.XmlEnum"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlEnum("skip")]
        Skip,
        /// <include file='doc\XmlSchemaContentProcessing.uex' path='docs/doc[@for="XmlSchemaContentProcessing.XmlEnum1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlEnum("lax")]
        Lax,
        /// <include file='doc\XmlSchemaContentProcessing.uex' path='docs/doc[@for="XmlSchemaContentProcessing.XmlEnum2"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlEnum("strict")]
        Strict
    }
}
