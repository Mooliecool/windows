//------------------------------------------------------------------------------
// <copyright file="XmlSchemaAll.cs" company="Microsoft">
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

    using System.Collections;
    using System.Xml.Serialization;

    /// <include file='doc\XmlSchemaAll.uex' path='docs/doc[@for="XmlSchemaAll"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public class XmlSchemaAll : XmlSchemaGroupBase {
        XmlSchemaObjectCollection items = new XmlSchemaObjectCollection();

        /// <include file='doc\XmlSchemaAll.uex' path='docs/doc[@for="XmlSchemaAll.Items"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("element", typeof(XmlSchemaElement))]
        public override XmlSchemaObjectCollection Items {
            get { return items; }
        }

        internal override bool IsEmpty {
            get { return  base.IsEmpty || items.Count == 0; }
        } 

        internal override void SetItems(XmlSchemaObjectCollection newItems) {
            items = newItems;
        }
    }
}
