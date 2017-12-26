//------------------------------------------------------------------------------
// <copyright file="XmlSchemaUse.cs" company="Microsoft">
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

    //nzeng: if change the enum, have to change xsdbuilder as well.
    /// <include file='doc\XmlSchemaUse.uex' path='docs/doc[@for="XmlSchemaUse"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public enum XmlSchemaUse {
        /// <include file='doc\XmlSchemaUse.uex' path='docs/doc[@for="XmlSchemaUse.None"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlIgnore]
        None,
        /// <include file='doc\XmlSchemaUse.uex' path='docs/doc[@for="XmlSchemaUse.Optional"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlEnum("optional")]
        Optional,
        /// <include file='doc\XmlSchemaUse.uex' path='docs/doc[@for="XmlSchemaUse.Prohibited"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlEnum("prohibited")]
        Prohibited,
        /// <include file='doc\XmlSchemaUse.uex' path='docs/doc[@for="XmlSchemaUse.Required"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlEnum("required")]
        Required,
    }
}
