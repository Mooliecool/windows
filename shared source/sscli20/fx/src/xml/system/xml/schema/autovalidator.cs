//------------------------------------------------------------------------------
// <copyright file="AutoValidator.cs" company="Microsoft">
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
//------------------------------------------------------------------------------

namespace System.Xml.Schema {

    using System.Diagnostics;
    using System.Globalization;

#pragma warning disable 618

    internal class AutoValidator : BaseValidator {
        const string x_schema = "x-schema";

        public AutoValidator(XmlValidatingReaderImpl reader, XmlSchemaCollection schemaCollection, ValidationEventHandler eventHandler) : base(reader, schemaCollection, eventHandler) {
            schemaInfo = new SchemaInfo();
        } 
        
        public override bool PreserveWhitespace { 
            get { return false; }
        }
        
        public override void Validate() {
            ValidationType valType = DetectValidationType();
            switch(valType) {
                case ValidationType.XDR:
                    reader.Validator = new XdrValidator(this);
                    reader.Validator.Validate();
                    break;

                case ValidationType.Schema:
                    reader.Validator = new XsdValidator(this);
                    reader.Validator.Validate();
                    break;

                case ValidationType.Auto:
                    break;
            }
        } 
        
        public override void CompleteValidation() {} 

        public override object  FindId(string name) {
            return null;
        } 

        private ValidationType DetectValidationType() {
            //Type not yet detected : Check in Schema Collection
            if (reader.Schemas != null && reader.Schemas.Count > 0) {
                 XmlSchemaCollectionEnumerator enumerator = reader.Schemas.GetEnumerator();
                 while (enumerator.MoveNext()) {
                    XmlSchemaCollectionNode node = enumerator.CurrentNode;
                    SchemaInfo schemaInfo = node.SchemaInfo;
                    if(schemaInfo.SchemaType == SchemaType.XSD)
                        return ValidationType.Schema;
                    else if(schemaInfo.SchemaType == SchemaType.XDR)
                        return ValidationType.XDR;
                 }
            }

            if (reader.NodeType == XmlNodeType.Element) {
                SchemaType schemaType = SchemaNames.SchemaTypeFromRoot(reader.LocalName, reader.NamespaceURI);
                if (schemaType == SchemaType.XSD) {
                    return ValidationType.Schema;
                }
                else if (schemaType == SchemaType.XDR) {
                    return ValidationType.XDR;
                }
                else {
                    int count = reader.AttributeCount;
                    for (int i = 0; i < count ; i++) {
                        reader.MoveToAttribute(i);
                        string objectNs = reader.NamespaceURI;
                        string objectName = reader.LocalName;
                        if (Ref.Equal(objectNs, SchemaNames.NsXmlNs)) {
                            if(XdrBuilder.IsXdrSchema(reader.Value)) {
                                reader.MoveToElement();
                                return ValidationType.XDR;
                            }
                        }
                        else if (Ref.Equal(objectNs, SchemaNames.NsXsi)) {
                            reader.MoveToElement();
                            return ValidationType.Schema;
                        }
                        else if (Ref.Equal(objectNs, SchemaNames.QnDtDt.Namespace) && Ref.Equal(objectName, SchemaNames.QnDtDt.Name)) {
                            reader.SchemaTypeObject = XmlSchemaDatatype.FromXdrName(reader.Value);
                            reader.MoveToElement();
                            return ValidationType.XDR;
                        }
                    } //end of for
                    if(count > 0) {
                        reader.MoveToElement();
                    }
                }
            }
            return ValidationType.Auto;
        }    
        
    }
#pragma warning restore 618

}

        