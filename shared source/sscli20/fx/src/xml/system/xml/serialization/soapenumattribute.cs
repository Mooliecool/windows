//------------------------------------------------------------------------------
// <copyright file="SoapEnumAttribute.cs" company="Microsoft">
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

    using System;
    
    /// <include file='doc\SoapEnumAttribute.uex' path='docs/doc[@for="SoapEnumAttribute"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [AttributeUsage(AttributeTargets.Field)]
    public class SoapEnumAttribute : System.Attribute {
        string name; 

        /// <include file='doc\SoapEnumAttribute.uex' path='docs/doc[@for="SoapEnumAttribute.SoapEnumAttribute"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public SoapEnumAttribute() {
        }

        /// <include file='doc\SoapEnumAttribute.uex' path='docs/doc[@for="SoapEnumAttribute.SoapEnumAttribute1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public SoapEnumAttribute(string name) {
            this.name = name;
        }

        /// <include file='doc\SoapEnumAttribute.uex' path='docs/doc[@for="SoapEnumAttribute.Name"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string Name {
            get { return name == null ? string.Empty : name; }
            set { name = value; }
        }
    }
 }
