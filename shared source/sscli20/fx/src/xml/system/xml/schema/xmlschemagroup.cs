//------------------------------------------------------------------------------
// <copyright file="XmlSchemaGroup.cs" company="Microsoft">
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

    /// <include file='doc\XmlSchemaGroup.uex' path='docs/doc[@for="XmlSchemaGroup"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public class XmlSchemaGroup : XmlSchemaAnnotated {
        string name;        
        XmlSchemaGroupBase particle;
        XmlSchemaParticle canonicalParticle;
        XmlQualifiedName qname = XmlQualifiedName.Empty; 
        XmlSchemaGroup redefined;
        int selfReferenceCount;

        /// <include file='doc\XmlSchemaGroup.uex' path='docs/doc[@for="XmlSchemaGroup.Name"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlAttribute("name")]
        public string Name { 
            get { return name; }
            set { name = value; }
        }

        /// <include file='doc\XmlSchemaGroup.uex' path='docs/doc[@for="XmlSchemaGroup.Particle"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlElement("choice", typeof(XmlSchemaChoice)),
         XmlElement("all", typeof(XmlSchemaAll)),
         XmlElement("sequence", typeof(XmlSchemaSequence))]
        public XmlSchemaGroupBase Particle {
            get { return particle; }
            set { particle = value; }
        }

        [XmlIgnore]
        public XmlQualifiedName QualifiedName {
            get { return qname; }
        }

        [XmlIgnore]
        internal XmlSchemaParticle CanonicalParticle {
            get { return canonicalParticle; }
            set { canonicalParticle = value; }
        }

        [XmlIgnore]
        internal XmlSchemaGroup Redefined {
            get { return redefined; }
            set { redefined = value; }
        }

        [XmlIgnore]
        internal int SelfReferenceCount {
            get { return selfReferenceCount; }
            set { selfReferenceCount = value; }
        }

        [XmlIgnore]
        internal override string NameAttribute {
            get { return Name; }
            set { Name = value; }
        }

        internal void SetQualifiedName(XmlQualifiedName value) { 
            qname = value;
        }

        internal override XmlSchemaObject Clone() {
            XmlSchemaGroup newGroup = (XmlSchemaGroup)MemberwiseClone();
            if (XmlSchemaComplexType.HasParticleRef(this.particle)) {
                newGroup.particle = XmlSchemaComplexType.CloneParticle(this.particle) as XmlSchemaGroupBase;
            }
            newGroup.canonicalParticle = XmlSchemaParticle.Empty;
            return newGroup;
        }
    }
}
