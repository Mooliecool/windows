//------------------------------------------------------------------------------
// <copyright file="XmlMemberMapping.cs" company="Microsoft">
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
    using System.CodeDom;
    using System.CodeDom.Compiler;

    /// <include file='doc\XmlMemberMapping.uex' path='docs/doc[@for="XmlMemberMapping"]/*' />
    /// <internalonly/>
    public class XmlMemberMapping {
        MemberMapping mapping;

        internal XmlMemberMapping(MemberMapping mapping) {
            this.mapping = mapping;
        }

        internal MemberMapping Mapping {
            get { return mapping; }
        }

        internal Accessor Accessor {
            get { return mapping.Accessor; }
        }

        /// <include file='doc\XmlMemberMapping.uex' path='docs/doc[@for="XmlMemberMapping.Any"]/*' />
        public bool Any {
            get { return Accessor.Any; }
        }

        /// <include file='doc\XmlMemberMapping.uex' path='docs/doc[@for="XmlMemberMapping.ElementName"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string ElementName { 
            get { return Accessor.UnescapeName(Accessor.Name); }
        }

        /// <include file='doc\XmlMemberMapping.uex' path='docs/doc[@for="XmlMemberMapping.XsdElementName"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string XsdElementName { 
            get { return Accessor.Name; }
        }

        /// <include file='doc\XmlMemberMapping.uex' path='docs/doc[@for="XmlMemberMapping.Namespace"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string Namespace {
            get { return Accessor.Namespace; }
        }

        /// <include file='doc\XmlMemberMapping.uex' path='docs/doc[@for="XmlMemberMapping.MemberName"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string MemberName {
            get { return mapping.Name; }
        }

        /// <include file='doc\XmlMemberMapping.uex' path='docs/doc[@for="XmlMemberMapping.TypeName"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string TypeName {
            get { return Accessor.Mapping != null ? Accessor.Mapping.TypeName : String.Empty; }
        }

        /// <include file='doc\XmlMemberMapping.uex' path='docs/doc[@for="XmlMemberMapping.TypeNamespace"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string TypeNamespace {
            get { return Accessor.Mapping != null ? Accessor.Mapping.Namespace : null; }
        }

        /// <include file='doc\XmlMemberMapping.uex' path='docs/doc[@for="XmlMemberMapping.TypeFullName"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string TypeFullName {
            get { return mapping.TypeDesc.FullName; }
        }

        /// <include file='doc\XmlMemberMapping.uex' path='docs/doc[@for="XmlMemberMapping.CheckSpecified"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool CheckSpecified {
            get { return mapping.CheckSpecified; }
        }

        internal bool IsNullable {
            get { return mapping.IsNeedNullable; }
        }

        /// <include file='doc\XmlMemberMapping.uex' path='docs/doc[@for="XmlMemberMapping.GenerateTypeName"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string GenerateTypeName(CodeDomProvider codeProvider) {
            return mapping.GetTypeName(codeProvider);
        }
    }
}
