//------------------------------------------------------------------------------
// <copyright file="XmlArrayItemAttributes.cs" company="Microsoft">
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
    using System.Reflection;
    using System.Collections;
    using System.ComponentModel;

   /// <include file='doc\XmlArrayItemAttributes.uex' path='docs/doc[@for="XmlArrayItemAttributes"]/*' />
   /// <devdoc>
   ///    <para>[To be supplied.]</para>
   /// </devdoc>
   public class XmlArrayItemAttributes : CollectionBase {
        
        /// <include file='doc\XmlArrayItemAttributes.uex' path='docs/doc[@for="XmlArrayItemAttributes.this"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlArrayItemAttribute this[int index] {
            get { return (XmlArrayItemAttribute)List[index]; }
            set { List[index] = value; }
        }
        
        /// <include file='doc\XmlArrayItemAttributes.uex' path='docs/doc[@for="XmlArrayItemAttributes.Add"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int Add(XmlArrayItemAttribute attribute) {
            return List.Add(attribute);
        }
        
        /// <include file='doc\XmlArrayItemAttributes.uex' path='docs/doc[@for="XmlArrayItemAttributes.Insert"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Insert(int index, XmlArrayItemAttribute attribute) {
            List.Insert(index, attribute);
        }
        
        /// <include file='doc\XmlArrayItemAttributes.uex' path='docs/doc[@for="XmlArrayItemAttributes.IndexOf"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int IndexOf(XmlArrayItemAttribute attribute) {
            return List.IndexOf(attribute);
        }
        
        /// <include file='doc\XmlArrayItemAttributes.uex' path='docs/doc[@for="XmlArrayItemAttributes.Contains"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool Contains(XmlArrayItemAttribute attribute) {
            return List.Contains(attribute);
        }
        
        /// <include file='doc\XmlArrayItemAttributes.uex' path='docs/doc[@for="XmlArrayItemAttributes.Remove"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Remove(XmlArrayItemAttribute attribute) {
            List.Remove(attribute);
        }
        
        /// <include file='doc\XmlArrayItemAttributes.uex' path='docs/doc[@for="XmlArrayItemAttributes.CopyTo"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void CopyTo(XmlArrayItemAttribute[] array, int index) {
            List.CopyTo(array, index);
        }
        
    }

}
