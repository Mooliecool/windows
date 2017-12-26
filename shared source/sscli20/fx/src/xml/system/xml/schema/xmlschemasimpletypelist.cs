//------------------------------------------------------------------------------
// <copyright file="XmlSchemaSimpleTypeList.cs" company="Microsoft">
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

    /// <include file='doc\XmlSchemaSimpleTypeList.uex' path='docs/doc[@for="XmlSchemaSimpleTypeList"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public class XmlSchemaSimpleTypeList : XmlSchemaSimpleTypeContent {
        XmlQualifiedName itemTypeName = XmlQualifiedName.Empty; 
        XmlSchemaSimpleType itemType;
        XmlSchemaSimpleType baseItemType; //Compiled
        
        /// <include file='doc\XmlSchemaSimpleTypeList.uex' path='docs/doc[@for="XmlSchemaSimpleTypeList.ItemTypeName"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("itemType")]
        public XmlQualifiedName ItemTypeName { 
            get { return itemTypeName; }
            set { itemTypeName = (value == null ? XmlQualifiedName.Empty : value); }
        }

        /// <include file='doc\XmlSchemaSimpleTypeList.uex' path='docs/doc[@for="XmlSchemaSimpleTypeList.BaseType"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("simpleType", typeof(XmlSchemaSimpleType))]
        public XmlSchemaSimpleType ItemType {
            get { return itemType; }
            set { itemType = value; }
        }
        
        //Compiled
        /// <include file='doc\XmlSchemaSimpleTypeList.uex' path='docs/doc[@for="XmlSchemaSimpleTypeList.BaseItemType"]/*' />
        [XmlIgnore]
        public XmlSchemaSimpleType BaseItemType {
            get { return baseItemType; }
            set { baseItemType = value; }
        }

        internal override XmlSchemaObject Clone() {
            XmlSchemaSimpleTypeList newList = (XmlSchemaSimpleTypeList)MemberwiseClone();
            newList.ItemTypeName = itemTypeName.Clone();
            return newList;
        }
    }

}
