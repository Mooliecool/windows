//------------------------------------------------------------------------------
// <copyright file="BaseValidator.cs" company="Microsoft">
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

    using System.IO;
    using System.Diagnostics;
    using System.Text;
    using System.Collections;

#pragma warning disable 618

    internal class BaseValidator {
        XmlSchemaCollection schemaCollection;
        ValidationEventHandler eventHandler;
        XmlNameTable nameTable;
        SchemaNames schemaNames;
        PositionInfo positionInfo;
        XmlResolver xmlResolver;
        Uri baseUri;

        protected SchemaInfo schemaInfo;
        protected XmlValidatingReaderImpl reader;
        protected XmlQualifiedName elementName;
        protected ValidationState context;
        protected StringBuilder    textValue;
        protected string           textString;
        protected bool             hasSibling;
        protected bool             checkDatatype;

        public BaseValidator(BaseValidator other) {
            reader = other.reader;
            schemaCollection = other.schemaCollection;
            eventHandler = other.eventHandler;
            nameTable = other.nameTable;
            schemaNames = other.schemaNames;
            positionInfo = other.positionInfo;
            xmlResolver = other.xmlResolver;
            baseUri = other.baseUri;
            elementName = other.elementName;
        }

        public BaseValidator(XmlValidatingReaderImpl reader, XmlSchemaCollection schemaCollection, ValidationEventHandler eventHandler) {
            Debug.Assert(schemaCollection == null || schemaCollection.NameTable == reader.NameTable);
            this.reader = reader;
            this.schemaCollection = schemaCollection;
            this.eventHandler = eventHandler;
            nameTable = reader.NameTable;
            positionInfo = PositionInfo.GetPositionInfo(reader);
            elementName = new XmlQualifiedName();
        }

        public XmlValidatingReaderImpl Reader {
            get { return reader; }
        }

        public XmlSchemaCollection SchemaCollection {
            get { return schemaCollection; }
        }

        public XmlNameTable NameTable {
            get { return nameTable; }
        }
        
        public SchemaNames SchemaNames {
            get { 
                if (schemaNames != null) {
                    return schemaNames;
                }
                if (schemaCollection != null) {
                    schemaNames = schemaCollection.GetSchemaNames(nameTable);
                }
                else {
                    schemaNames = new SchemaNames(nameTable);
                }
                return schemaNames; 
            }
        }

        public PositionInfo PositionInfo {
            get { return positionInfo; }
        }

        public XmlResolver XmlResolver {
            get { return xmlResolver; }
            set { xmlResolver = value; }
        }

        public Uri BaseUri {
            get { return baseUri; }
            set { baseUri = value; }
        }

        public ValidationEventHandler EventHandler {
            get { return eventHandler; }
            set { eventHandler = value; }
        }

        public SchemaInfo SchemaInfo {
            get {
                return schemaInfo;
            }
            set {
                schemaInfo = value;
            }
        }

        public virtual bool PreserveWhitespace { 
            get {
                return false;
            }
        }

        public virtual void Validate() {
        }

        public virtual void CompleteValidation() {
        }
        
        public virtual object  FindId(string name) {
            return null;
        }
        
        public void ValidateText() {
            if (context.NeedValidateChildren) {
                if (context.IsNill) {
                    SendValidationEvent(Res.Sch_ContentInNill, XmlSchemaValidator.QNameString(context.LocalName, context.Namespace));
                    return;
                }
                ContentValidator contentValidator = context.ElementDecl.ContentValidator;
                XmlSchemaContentType contentType = contentValidator.ContentType;
                if (contentType == XmlSchemaContentType.ElementOnly) {
                    ArrayList names = contentValidator.ExpectedElements(context, false); 
                    if (names == null) {
                        SendValidationEvent(Res.Sch_InvalidTextInElement, XmlSchemaValidator.BuildElementName(context.LocalName, context.Namespace));
                    }
                    else {
                        Debug.Assert(names.Count > 0);
                        SendValidationEvent(Res.Sch_InvalidTextInElementExpecting, new string[] { XmlSchemaValidator.BuildElementName(context.LocalName, context.Namespace), XmlSchemaValidator.PrintExpectedElements(names, false) });
                    }
                }
                else if (contentType == XmlSchemaContentType.Empty) {
                    SendValidationEvent(Res.Sch_InvalidTextInEmpty, string.Empty);
                }
                if (checkDatatype) {
                    SaveTextValue(reader.Value);
                }
            }
        }
        
        public void ValidateWhitespace() {
            if (context.NeedValidateChildren) {
                XmlSchemaContentType contentType = context.ElementDecl.ContentValidator.ContentType;
                if (context.IsNill) {
                    SendValidationEvent(Res.Sch_ContentInNill, XmlSchemaValidator.QNameString(context.LocalName, context.Namespace));
                }
                if (contentType == XmlSchemaContentType.Empty) {
                    SendValidationEvent(Res.Sch_InvalidWhitespaceInEmpty, string.Empty);
                }
            }
        }

        private void SaveTextValue(string value) {
            if (textString.Length == 0) {
                textString = value;
            }
            else {
                if (!hasSibling) {
                    textValue.Append(textString);
                    hasSibling = true;
                }
                textValue.Append(value);
            }
        }

        protected void SendValidationEvent(string code) {
            SendValidationEvent(code, string.Empty);
        }

        protected void SendValidationEvent(string code, string[] args) {
            SendValidationEvent(new XmlSchemaException(code, args, reader.BaseURI, positionInfo.LineNumber, positionInfo.LinePosition));
        }

        protected void SendValidationEvent(string code, string arg) {
            SendValidationEvent(new XmlSchemaException(code, arg, reader.BaseURI, positionInfo.LineNumber, positionInfo.LinePosition));
        }

        protected void SendValidationEvent(string code, string arg1, string arg2) {
            SendValidationEvent(new XmlSchemaException(code, new string[] { arg1, arg2 }, reader.BaseURI, positionInfo.LineNumber, positionInfo.LinePosition));
        }

        protected void SendValidationEvent(XmlSchemaException e) {
            SendValidationEvent(e, XmlSeverityType.Error);
        }
        
        protected void SendValidationEvent(string code, string msg, XmlSeverityType severity) {
            SendValidationEvent(new XmlSchemaException(code, msg, reader.BaseURI, positionInfo.LineNumber, positionInfo.LinePosition), severity);
        }

        protected void SendValidationEvent(string code, string[] args, XmlSeverityType severity) {
            SendValidationEvent(new XmlSchemaException(code, args, reader.BaseURI, positionInfo.LineNumber, positionInfo.LinePosition), severity);
        }

        protected void SendValidationEvent(XmlSchemaException e, XmlSeverityType severity) {
            if (eventHandler != null) {
                eventHandler(reader, new ValidationEventArgs(e, severity));
            }
            else if (severity == XmlSeverityType.Error) {
                throw e;
            }
        }

        protected static void ProcessEntity(SchemaInfo sinfo, string name, object sender, ValidationEventHandler  eventhandler, string baseUri, int lineNumber, int linePosition) {
            SchemaEntity en = (SchemaEntity)sinfo.GeneralEntities[new XmlQualifiedName(name)];
            XmlSchemaException e = null;
            if (en == null) {
                // validation error, see xml spec [68]
                e = new XmlSchemaException(Res.Sch_UndeclaredEntity, name, baseUri, lineNumber, linePosition);                    
            }
            else if (en.NData.IsEmpty) {
                e = new XmlSchemaException(Res.Sch_UnparsedEntityRef, name, baseUri, lineNumber, linePosition);
            }
            if (e != null) {
                if (eventhandler != null) {
                    eventhandler(sender, new ValidationEventArgs(e));
                }
                else {
                    throw e;
                }
            }
        }

        public static BaseValidator CreateInstance(ValidationType valType, XmlValidatingReaderImpl reader, XmlSchemaCollection schemaCollection, ValidationEventHandler eventHandler, bool processIdentityConstraints) {
            switch(valType) {
                case ValidationType.XDR:
                    return new XdrValidator(reader, schemaCollection, eventHandler);

                case ValidationType.Schema:
                    return new XsdValidator(reader, schemaCollection, eventHandler);
                   
                case ValidationType.DTD:
                    return new DtdValidator(reader, eventHandler, processIdentityConstraints);
                    
                case ValidationType.Auto:
                    return new AutoValidator(reader, schemaCollection, eventHandler);

                case ValidationType.None:
                    return new BaseValidator(reader, schemaCollection, eventHandler);

                default:
                        break;
            }
            return null;
        }

    }
#pragma warning restore 618

}

        