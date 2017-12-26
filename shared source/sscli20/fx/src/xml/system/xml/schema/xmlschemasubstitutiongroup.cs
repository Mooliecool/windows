//------------------------------------------------------------------------------
// <copyright file="XmlSchemaSubstitutionGroup.cs" company="Microsoft">
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

    internal class XmlSchemaSubstitutionGroup : XmlSchemaObject {
        ArrayList membersList = new ArrayList();
        XmlQualifiedName examplar = XmlQualifiedName.Empty;

        [XmlIgnore]
        internal ArrayList Members {
            get { return membersList; }
        } 

        [XmlIgnore]
        internal XmlQualifiedName Examplar {
            get { return examplar; }
            set { examplar = value; }
        }
    }

    internal class XmlSchemaSubstitutionGroupV1Compat : XmlSchemaSubstitutionGroup {
        XmlSchemaChoice choice = new XmlSchemaChoice();

        [XmlIgnore]
        internal XmlSchemaChoice Choice {
            get { return choice; }
        }          

    }
}
