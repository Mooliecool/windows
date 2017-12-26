//------------------------------------------------------------------------------
// <copyright file="XmlTypeMapping.cs" company="Microsoft">
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

    using System.Reflection;
    using System;

    /// <include file='doc\XmlTypeMapping.uex' path='docs/doc[@for="XmlTypeMapping"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public class XmlTypeMapping : XmlMapping {

        internal XmlTypeMapping(TypeScope scope, ElementAccessor accessor) : base(scope, accessor) { 
        }

        internal TypeMapping Mapping {
            get { return Accessor.Mapping; }
        }

        /// <include file='doc\XmlTypeMapping.uex' path='docs/doc[@for="XmlTypeMapping.TypeName"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string TypeName {
            get { return Mapping.TypeDesc.Name; }
        }

        /// <include file='doc\XmlTypeMapping.uex' path='docs/doc[@for="XmlTypeMapping.TypeFullName"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string TypeFullName {
            get { return Mapping.TypeDesc.FullName; }
        }

        /// <include file='doc\XmlTypeMapping.uex' path='docs/doc[@for="XmlTypeMapping.XsdTypeName"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string XsdTypeName {
            get { return Mapping.TypeName; }
        }

        /// <include file='doc\XmlTypeMapping.uex' path='docs/doc[@for="XmlTypeMapping.XsdTypeNamespace"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string XsdTypeNamespace {
            get { return Mapping.Namespace; }
        }
    }
}
