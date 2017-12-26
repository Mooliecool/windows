//------------------------------------------------------------------------------
// <copyright file="SchemaInfo.cs" company="Microsoft">
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
    
    internal enum AttributeMatchState {
        AttributeFound,
        AnyIdAttributeFound,
        UndeclaredElementAndAttribute,
        UndeclaredAttribute,
        AnyAttributeLax,
        AnyAttributeSkip,
        ProhibitedAnyAttribute,
        ProhibitedAttribute,
        AttributeNameMismatch,
        ValidateAttributeInvalidCall,
    }

    internal class SchemaInfo {
        SchemaType schemaType;
        Hashtable targetNamespaces = new Hashtable();
        Hashtable elementDecls = new Hashtable();
        Hashtable undeclaredElementDecls = new Hashtable();
        Hashtable elementDeclsByType = new Hashtable();
        Hashtable attributeDecls = new Hashtable();

        Hashtable generalEntities;
        Hashtable parameterEntities;
        Hashtable notations;
        XmlQualifiedName docTypeName = XmlQualifiedName.Empty;
        int errorCount;
        bool hasNonCDataAttributes = false;
        bool hasDefaultAttributes = false;

        public SchemaInfo() {
            schemaType = SchemaType.None;
        }

        public SchemaType SchemaType {
            get { return schemaType;}
            set { schemaType = value;}
        }

        public Hashtable TargetNamespaces {
            get { return targetNamespaces; }
        }

        public Hashtable ElementDecls {
            get { return elementDecls; }
        }

        public Hashtable UndeclaredElementDecls {
            get { return undeclaredElementDecls; }
        }
        
        public Hashtable ElementDeclsByType {
            get { return elementDeclsByType; }
        }

        public Hashtable AttributeDecls {
            get { return attributeDecls; }
        }

        public Hashtable GeneralEntities {
            get {
                if (this.generalEntities == null) {
                    this.generalEntities = new Hashtable();
                }
                return this.generalEntities; 
            }
        }

        public Hashtable ParameterEntities {
            get {
                if (this.parameterEntities == null) {
                    this.parameterEntities = new Hashtable();
                }
                return this.parameterEntities; 
            }
        }

        public Hashtable Notations {
            get {
                if (this.notations == null) {
                    this.notations = new Hashtable();
                }
                return this.notations; 
            }
        }

        public XmlQualifiedName DocTypeName {
            get { return docTypeName;}
            set { docTypeName = value;}
        }

        public int ErrorCount {
            get { return errorCount; }
            set { errorCount = value; }
        }

        public SchemaElementDecl GetElementDecl(XmlQualifiedName qname) {
            return (SchemaElementDecl)elementDecls[qname];
        }
        
        public SchemaElementDecl GetTypeDecl(XmlQualifiedName qname) {
            return (SchemaElementDecl)elementDeclsByType[qname];
        }
        
        public XmlSchemaElement GetElement(XmlQualifiedName qname) {
            SchemaElementDecl ed = GetElementDecl(qname);
            if (ed != null) {
                return ed.SchemaElement;
            }
            return null;
        }
        
        public XmlSchemaAttribute GetAttribute(XmlQualifiedName qname) {
            SchemaAttDef attdef = (SchemaAttDef)attributeDecls[qname];
            if (attdef != null) {
                return attdef.SchemaAttribute;
            }
            return null;
        }
        
        public XmlSchemaElement GetType(XmlQualifiedName qname) {
            SchemaElementDecl ed = GetElementDecl(qname);
            if (ed != null) {
                return ed.SchemaElement;
            }
            return null;
        }

        public bool HasSchema(string ns) {
            return targetNamespaces[ns] != null;
        }
        
        public bool Contains(string ns) {
            return targetNamespaces[ns] != null;
        }

        public SchemaAttDef GetAttributeXdr(SchemaElementDecl ed, XmlQualifiedName qname) {
            SchemaAttDef attdef = null;
            if (ed != null) {
                attdef = ed.GetAttDef(qname);;
                if (attdef == null) {
                    if (!ed.ContentValidator.IsOpen || qname.Namespace.Length == 0) {
                        throw new XmlSchemaException(Res.Sch_UndeclaredAttribute, qname.ToString());
                    }
                    attdef = (SchemaAttDef)attributeDecls[qname];
                    if (attdef == null && targetNamespaces.Contains(qname.Namespace)) {
                        throw new XmlSchemaException(Res.Sch_UndeclaredAttribute, qname.ToString());
                    }
                }
            }
            return attdef;
        }


        public SchemaAttDef GetAttributeXsd(SchemaElementDecl ed, XmlQualifiedName qname, XmlSchemaObject partialValidationType, out AttributeMatchState attributeMatchState) {
            SchemaAttDef attdef = null;
            attributeMatchState = AttributeMatchState.UndeclaredAttribute;
            if (ed != null) {
                attdef = ed.GetAttDef(qname);
                if (attdef != null) {
                    attributeMatchState = AttributeMatchState.AttributeFound;
                    return attdef;
                }
                XmlSchemaAnyAttribute any = ed.AnyAttribute;
                if (any != null) {
                    if (!any.NamespaceList.Allows(qname)) {
                        attributeMatchState = AttributeMatchState.ProhibitedAnyAttribute;
                    }
                    else if (any.ProcessContentsCorrect != XmlSchemaContentProcessing.Skip) {
                        attdef = (SchemaAttDef)attributeDecls[qname];
                        if (attdef != null) {
                            if (attdef.Datatype.TypeCode == XmlTypeCode.Id) { //anyAttribute match whose type is ID
                                attributeMatchState = AttributeMatchState.AnyIdAttributeFound;
                            }
                            else {
                                attributeMatchState = AttributeMatchState.AttributeFound;
                            }
                        }
                        else if (any.ProcessContentsCorrect == XmlSchemaContentProcessing.Lax) {
                            attributeMatchState = AttributeMatchState.AnyAttributeLax;
                        }
                    }
                    else {
                        attributeMatchState = AttributeMatchState.AnyAttributeSkip;
                    }
                }
                else if (ed.ProhibitedAttributes[qname] != null) {
                    attributeMatchState = AttributeMatchState.ProhibitedAttribute;
                }
            }
            else if (partialValidationType != null) {
                XmlSchemaAttribute attr = partialValidationType as XmlSchemaAttribute;
                if (attr != null) {
                    if (qname.Equals(attr.QualifiedName)) {
                        attdef = attr.AttDef;
                        attributeMatchState = AttributeMatchState.AttributeFound;
                    }
                    else {
                        attributeMatchState = AttributeMatchState.AttributeNameMismatch;
                    }
                }
                else {
                    attributeMatchState = AttributeMatchState.ValidateAttributeInvalidCall;
                }
            }
            else {
                attdef = (SchemaAttDef)attributeDecls[qname];
                if (attdef != null) {
                    attributeMatchState = AttributeMatchState.AttributeFound;
                }
                else {
                    attributeMatchState = AttributeMatchState.UndeclaredElementAndAttribute;
                }
            }
            return attdef;
        }

        public SchemaAttDef GetAttributeXsd(SchemaElementDecl ed, XmlQualifiedName qname, ref bool skip) {
            AttributeMatchState attributeMatchState;

            SchemaAttDef attDef = GetAttributeXsd(ed, qname, null, out attributeMatchState);
            switch(attributeMatchState) {
                case AttributeMatchState.UndeclaredAttribute:
                    throw new XmlSchemaException(Res.Sch_UndeclaredAttribute, qname.ToString());

                case AttributeMatchState.ProhibitedAnyAttribute:
                case AttributeMatchState.ProhibitedAttribute:
                    throw new XmlSchemaException(Res.Sch_ProhibitedAttribute, qname.ToString());

                case AttributeMatchState.AttributeFound:
                case AttributeMatchState.AnyAttributeLax:
                case AttributeMatchState.UndeclaredElementAndAttribute:
                    break;

                case AttributeMatchState.AnyAttributeSkip:
                    skip = true;
                    break;

                default:
                    Debug.Assert(false);
                    break;
            }
            return attDef;
        }
            
        public void Add(SchemaInfo sinfo, ValidationEventHandler eventhandler) {
            if (schemaType == SchemaType.None) {
                schemaType = sinfo.SchemaType;
            }
            else if (schemaType != sinfo.SchemaType) {
                if (eventhandler != null) {
                    eventhandler(this, new ValidationEventArgs(new XmlSchemaException(Res.Sch_MixSchemaTypes, string.Empty)));
                }
                return;
            }

            foreach(string tns in sinfo.TargetNamespaces.Keys) {
                if (!targetNamespaces.ContainsKey(tns)) {
                    targetNamespaces.Add(tns, true);
                }
            }

            foreach(DictionaryEntry entry in sinfo.elementDecls) {
                if (!elementDecls.ContainsKey(entry.Key)) {
                    elementDecls.Add(entry.Key, entry.Value);
                }
            }
            foreach(DictionaryEntry entry in sinfo.elementDeclsByType) {
                if (!elementDeclsByType.ContainsKey(entry.Key)) {
                    elementDeclsByType.Add(entry.Key, entry.Value);
                }   
            }
            foreach (SchemaAttDef attdef in sinfo.AttributeDecls.Values) {
                if (!attributeDecls.ContainsKey(attdef.Name)) {
                    attributeDecls.Add(attdef.Name, attdef);
                }
            }
            foreach (SchemaNotation notation in sinfo.Notations.Values) {
                if (!Notations.ContainsKey(notation.Name.Name)) {
                    Notations.Add(notation.Name.Name, notation);
                }
            }

        }

        public void Finish() {
            Hashtable elementHT = elementDecls;
            for ( int i = 0; i < 2; i++ ) {
                foreach ( SchemaElementDecl e in elementHT.Values ) {
                    e.EndAddAttDef();
                    if ( e.HasNonCDataAttribute ) {
                        hasNonCDataAttributes = true;
                    }
                    if ( e.DefaultAttDefs != null ) {
                        hasDefaultAttributes = true;
                    }
                }
                elementHT = undeclaredElementDecls;
            }
        }

        internal bool HasDefaultAttributes {
            get {
                return hasDefaultAttributes;
            }
            set {
                hasDefaultAttributes = value;
            }
        }

        internal bool HasNonCDataAttributes {
            get {
                return hasNonCDataAttributes;
            }
            set {
                hasNonCDataAttributes = value;
            }
        }
    }
}
