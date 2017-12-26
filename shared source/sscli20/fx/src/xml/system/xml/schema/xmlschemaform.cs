//------------------------------------------------------------------------------
// <copyright file="XmlSchemaForm.cs" company="Microsoft">
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

    // if change the enum, have to change xsdbuilder as well.
    /// <include file='doc\XmlSchemaForm.uex' path='docs/doc[@for="XmlSchemaForm"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public enum XmlSchemaForm {
        /// <include file='doc\XmlSchemaForm.uex' path='docs/doc[@for="XmlSchemaForm.XmlEnum"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlIgnore]
        None,
        /// <include file='doc\XmlSchemaForm.uex' path='docs/doc[@for="XmlSchemaForm.XmlEnum1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlEnum("qualified")]
        Qualified,
        /// <include file='doc\XmlSchemaForm.uex' path='docs/doc[@for="XmlSchemaForm.XmlEnum2"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlEnum("unqualified")]
        Unqualified,
    }
}
