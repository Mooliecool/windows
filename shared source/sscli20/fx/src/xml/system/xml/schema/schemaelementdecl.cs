//------------------------------------------------------------------------------
// <copyright file="SchemaElementDecl.cs" company="Microsoft">
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

    using System;
    using System.Collections;
    using System.Diagnostics;

    internal sealed class SchemaElementDecl : SchemaDeclBase {
        ContentValidator contentValidator;      
        Hashtable attdefs = new Hashtable();
        Hashtable prohibitedAttributes = new Hashtable(); 
        ArrayList tmpDefaultAttdefs;
        SchemaAttDef[] defaultAttdefs;
        bool isAbstract = false;
        bool isNillable = false;
        XmlSchemaDerivationMethod block;
        bool isIdDeclared;
        bool isNotationDeclared;
        bool hasRequiredAttribute = false;
        bool hasNonCDataAttribute = false;
        XmlSchemaAnyAttribute anyAttribute;
        CompiledIdentityConstraint[] constraints;
        XmlSchemaElement schemaElement;
       
        public static readonly SchemaElementDecl Empty = new SchemaElementDecl();

        public SchemaElementDecl() {
        }
        
        public SchemaElementDecl(XmlSchemaDatatype dtype) {
            Datatype = dtype;
            contentValidator = ContentValidator.TextOnly;
        }

        public SchemaElementDecl(XmlQualifiedName name, String prefix, SchemaType schemaType) 
        : base(name, prefix) {
        }

        public static SchemaElementDecl CreateAnyTypeElementDecl() {
            SchemaElementDecl anyTypeElementDecl = new SchemaElementDecl();
            anyTypeElementDecl.Datatype = DatatypeImplementation.AnySimpleType.Datatype;
            return anyTypeElementDecl;
        }

        
        public SchemaElementDecl Clone() {
            return (SchemaElementDecl) MemberwiseClone();
        }

        public bool IsAbstract {
            get { return isAbstract;}
            set { isAbstract = value;}
        }

        public bool IsNillable {
            get { return isNillable;}
            set { isNillable = value;}
        }

        public XmlSchemaDerivationMethod Block {
             get { return block; }
             set { block = value; }
        }

        public bool IsIdDeclared {
            get { return isIdDeclared;}
            set { isIdDeclared = value;}
        }

        public bool IsNotationDeclared {
            get { return isNotationDeclared; }
            set { isNotationDeclared = value; }
        }

        public bool HasDefaultAttribute {
            get { return defaultAttdefs != null; }
        }

        public bool HasRequiredAttribute {
            get { return hasRequiredAttribute; }
            set { hasRequiredAttribute = value; }
        }

        public bool HasNonCDataAttribute {
            get { return hasNonCDataAttribute; }
            set { hasNonCDataAttribute = value; }
        }

        public ContentValidator ContentValidator {
            get { return contentValidator;}
            set { contentValidator = value;}
        }

        public XmlSchemaAnyAttribute AnyAttribute {
            get { return anyAttribute; }
            set { anyAttribute = value; }
        }

        public CompiledIdentityConstraint[] Constraints {
            get { return constraints; }
            set { constraints = value; }
        }
        
        public XmlSchemaElement SchemaElement {
            get { return schemaElement;}
            set { schemaElement = value;}
        }
        // add a new SchemaAttDef to the SchemaElementDecl
        public void AddAttDef(SchemaAttDef attdef) {
            attdefs.Add(attdef.Name, attdef);
            if (attdef.Presence == SchemaDeclBase.Use.Required || attdef.Presence == SchemaDeclBase.Use.RequiredFixed) {
                hasRequiredAttribute = true;
            }
            if (attdef.Presence == SchemaDeclBase.Use.Default || attdef.Presence == SchemaDeclBase.Use.Fixed) { //Not adding RequiredFixed here
                if (tmpDefaultAttdefs == null) {
                    tmpDefaultAttdefs = new ArrayList();
                }
                tmpDefaultAttdefs.Add(attdef);
            }
        }
        
        public void EndAddAttDef() {
            if (tmpDefaultAttdefs != null) {
                defaultAttdefs = (SchemaAttDef[])tmpDefaultAttdefs.ToArray(typeof(SchemaAttDef));
                tmpDefaultAttdefs = null;
            }
        }

        /*
         * Retrieves the attribute definition of the named attribute.
         * @param name  The name of the attribute.
         * @return  an attribute definition object; returns null if it is not found.
         */
        public SchemaAttDef GetAttDef(XmlQualifiedName qname) {
            return (SchemaAttDef)attdefs[qname];
        }

        public Hashtable AttDefs {
            get { return attdefs; }
        }

        public SchemaAttDef[] DefaultAttDefs {
            get { return defaultAttdefs; }
        }

        public Hashtable ProhibitedAttributes {
            get { return prohibitedAttributes; }
        }

        public void CheckAttributes(Hashtable presence, bool standalone) {
            foreach(SchemaAttDef attdef in attdefs.Values) {
                if (presence[attdef.Name] == null) {
                    if (attdef.Presence == SchemaDeclBase.Use.Required) {
                        throw new XmlSchemaException(Res.Sch_MissRequiredAttribute, attdef.Name.ToString());
                    }
                    else if (standalone && attdef.IsDeclaredInExternal && (attdef.Presence == SchemaDeclBase.Use.Default || attdef.Presence == SchemaDeclBase.Use.Fixed)) {
                        throw new XmlSchemaException(Res.Sch_StandAlone, string.Empty);
                    }
                }
            }
        }
    };
}
