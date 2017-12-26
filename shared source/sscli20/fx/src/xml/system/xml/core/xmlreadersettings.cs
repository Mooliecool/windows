//------------------------------------------------------------------------------
// <copyright file="XmlReaderSettings.cs" company="Microsoft">
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

using System.Xml;
using System.Xml.Schema;

namespace System.Xml {

    // XmlReaderSettings class specifies features of an XmlReader.
    public sealed class XmlReaderSettings {
//
// Fields
//
        // Nametable
        XmlNameTable    nameTable;

        // XmlResolver
        XmlResolver     xmlResolver;

        // Text settings
        int     lineNumberOffset;
        int     linePositionOffset;

        // Conformance settings
        ConformanceLevel    conformanceLevel;
        bool                checkCharacters;

        //Validation settings
        ValidationType          validationType;
        XmlSchemaValidationFlags validationFlags;
        XmlSchemaSet            schemas;
        ValidationEventHandler  valEventHandler;

        // Filtering settings
        bool    ignoreWhitespace;
        bool    ignorePIs;
        bool    ignoreComments;

        // security settings
        bool    prohibitDtd;

        // other settings
        bool    closeInput;

        // read-only flag
        bool isReadOnly;

//
// Constructor
//
        public XmlReaderSettings() {
            Reset();
        }

//
// Properties
//
        // Nametable
        public XmlNameTable NameTable { 
            get {
                return nameTable;
            }
            set {
                CheckReadOnly( "NameTable" );
                nameTable = value;
            }
        }

        // XmlResolver
        public XmlResolver XmlResolver { 
            set {
                CheckReadOnly( "XmlResolver" );
                xmlResolver = value;
            }
        }

        internal XmlResolver GetXmlResolver() {
            return xmlResolver;
        }

    // Text settings
        public int LineNumberOffset { 
            get {
                return lineNumberOffset;
            }
            set {
                CheckReadOnly( "LineNumberOffset" );
                if ( lineNumberOffset < 0 ) {
                    throw new ArgumentOutOfRangeException( "value" );
                }
                lineNumberOffset = value;
            }
        }

        public int LinePositionOffset { 
            get {
                return linePositionOffset;
            }
            set {
                CheckReadOnly( "LinePositionOffset" );
                if ( linePositionOffset < 0 ) {
                    throw new ArgumentOutOfRangeException( "value" );
                }
                linePositionOffset = value;
            }
        }

    // Conformance settings
        public ConformanceLevel ConformanceLevel {
            get {
                return conformanceLevel;
            }
            set {
                CheckReadOnly( "ConformanceLevel" );

                if ( (uint)value > (uint)ConformanceLevel.Document ) {
                    throw new ArgumentOutOfRangeException( "value" );
                }
                conformanceLevel = value;
            }
        }

        public bool CheckCharacters {
            get {
                return checkCharacters;
            }
            set {
                CheckReadOnly( "CheckCharacters" );
                checkCharacters = value;
            }
        }

        public ValidationType ValidationType {
            get {
                return validationType;
            }
            set {
                CheckReadOnly("ValidationType");

                if ( (uint)value > (uint)ValidationType.Schema ) {
                    throw new ArgumentOutOfRangeException( "value" );
                }
                validationType = value;
            }
        }

        public XmlSchemaValidationFlags ValidationFlags {
            get {
                return validationFlags;
            }
            set {
                CheckReadOnly("ValidationFlags");

                if ( (uint)value > (uint)( XmlSchemaValidationFlags.ProcessInlineSchema | XmlSchemaValidationFlags.ProcessSchemaLocation | 
                                           XmlSchemaValidationFlags.ReportValidationWarnings | XmlSchemaValidationFlags.ProcessIdentityConstraints |
                                           XmlSchemaValidationFlags.AllowXmlAttributes ) ) {
                    throw new ArgumentOutOfRangeException( "value" );
                }
                validationFlags = value;
            }
        }

        public XmlSchemaSet Schemas {
            get {
                if ( schemas == null ) {
                    schemas = new XmlSchemaSet();
                }
                return schemas;
            }
            set {
                CheckReadOnly( "Schemas" );
                schemas = value;
            }
        }

        public event ValidationEventHandler ValidationEventHandler {
            add {
                CheckReadOnly( "ValidationEventHandler" );
                valEventHandler += value;
            }
            remove {
                CheckReadOnly( "ValidationEventHandler" );
                valEventHandler -= value;
            }
        }

        // Filtering settings
        public bool IgnoreWhitespace {
            get {
                return ignoreWhitespace;
            }
            set {
                CheckReadOnly( "IgnoreWhitespace" );
                ignoreWhitespace = value;
            }
        }

        public bool IgnoreProcessingInstructions {
            get {
                return ignorePIs;
            }
            set {
                CheckReadOnly( "IgnoreProcessingInstructions" );
                ignorePIs = value;
            }
        }

        public bool IgnoreComments {
            get {
                return ignoreComments;
            }
            set {
                CheckReadOnly( "IgnoreComments" );
                ignoreComments = value;
            }
        }

        public bool ProhibitDtd {
            get {
                return prohibitDtd;
            }
            set {
                CheckReadOnly( "ProhibitDtd" );
                prohibitDtd = value;
            }
        }        

        public bool CloseInput {
            get {
                return closeInput;
            }
            set {
                CheckReadOnly( "CloseInput" );
                closeInput = value;
            }
        }        

//
// Public methods
//
        public void Reset() {
            CheckReadOnly( "Reset" );

            nameTable = null;
            xmlResolver = new XmlUrlResolver();
            lineNumberOffset = 0;
            linePositionOffset = 0;
            checkCharacters = true;
            conformanceLevel = ConformanceLevel.Document;

            schemas = null;
            validationType = ValidationType.None;
            validationFlags = XmlSchemaValidationFlags.ProcessIdentityConstraints;
            validationFlags |= XmlSchemaValidationFlags.AllowXmlAttributes;
            ignoreWhitespace = false;
            ignorePIs = false;
            ignoreComments = false;
            prohibitDtd = true;
            closeInput = false;

            isReadOnly = false;
        }

        public XmlReaderSettings Clone() {
            XmlReaderSettings clonedSettings = MemberwiseClone() as XmlReaderSettings;
            clonedSettings.isReadOnly = false;
            return clonedSettings;
        }
        
//
// Internal and private methods
//
        internal bool ReadOnly {
            get {
                return isReadOnly;
            }
            set {
                isReadOnly = value;
            }
        }
        
        internal ValidationEventHandler GetEventHandler() {
            return valEventHandler;
        }

        private void CheckReadOnly( string propertyName ) {
            if ( isReadOnly ) {
                throw new XmlException( Res.Xml_ReadOnlyProperty, "XmlReaderSettings." + propertyName );
            }
        }

        internal bool CanResolveExternals {
            get {
                return prohibitDtd == false && xmlResolver != null;
            }
        }

    }
}
