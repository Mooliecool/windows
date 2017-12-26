//------------------------------------------------------------------------------
// <copyright file="XmlWriterSettings.cs" company="Microsoft">
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

using System;
using System.Collections;
using System.Text;
using System.Xml;
using System.Xml.Schema;


namespace System.Xml {

    public enum XmlOutputMethod {
        Xml         = 0,    // Use Xml 1.0 rules to serialize
        Html        = 1,    // Use Html rules specified by Xslt specification to serialize
        Text        = 2,    // Only serialize text blocks
        AutoDetect  = 3,    // Choose between Xml and Html output methods at runtime (using Xslt rules to do so)
    }

    internal enum XmlStandalone {
        // Do not change the constants - XmlBinaryWriter depends in it
        Omit    = 0,
        Yes     = 1,
        No      = 2,
    }

    /// <summary>
    /// Three-state logic enumeration.
    /// </summary>
    internal enum TriState {
        Unknown = -1,
        False = 0,
        True = 1,
    };


    // XmlReaderSettings class specifies features of an XmlWriter.
    public sealed class XmlWriterSettings {
//
// Fields
//
        // Text settings
        Encoding            encoding;
        bool                omitXmlDecl;
        NewLineHandling     newLineHandling;
        string              newLineChars;
        TriState            indent;
        string              indentChars;
        bool                newLineOnAttributes;
        bool                closeOutput;

        // Conformance settings
        ConformanceLevel conformanceLevel;
        bool             checkCharacters;

        // Xslt settings
        XmlOutputMethod outputMethod;
        IList           cdataSections;
        bool            mergeCDataSections;
        string          mediaType;
        string          docTypeSystem;
        string          docTypePublic;
        XmlStandalone   standalone;
        bool            autoXmlDecl;

        // read-only flag
        bool    isReadOnly;

//
// Constructor
//
        public XmlWriterSettings() {
            Reset();
        }

//
// Properties
//
    // Text
        public Encoding Encoding {
            get { 
                return encoding; 
            }
            set {
                CheckReadOnly( "Encoding" );
                encoding = value;
            }
        }

        // True if an xml declaration should *not* be written.
        public bool OmitXmlDeclaration {
            get { 
                return omitXmlDecl; 
            }
            set {
                CheckReadOnly( "OmitXmlDeclaration" );
                omitXmlDecl = value;
            }
        }

        // See NewLineHandling enum for details.
        public NewLineHandling NewLineHandling {
            get {
                return newLineHandling;
            }
            set {
                CheckReadOnly("NewLineHandling");

                if ( (uint)value > (uint)NewLineHandling.None ) {
                    throw new ArgumentOutOfRangeException( "value" );
                }
                newLineHandling = value;
            }
        }

        // Line terminator string. By default, this is a carriage return followed by a line feed ("\r\n").
        public string NewLineChars {
            get { 
                return newLineChars; 
            }
            set {
                CheckReadOnly( "NewLineChars" );

                if ( value == null ) {
                    throw new ArgumentNullException( "value" );
                }
                newLineChars = value;
            }
        }

        // True if output should be indented using rules that are appropriate to the output rules (i.e. Xml, Html, etc).
        public bool Indent {
            get { 
                return indent == TriState.True;
            }
            set {
                CheckReadOnly( "Indent" );
                indent = value ? TriState.True : TriState.False;
            }
        }

        // Characters to use when indenting. This is usually tab or some spaces, but can be anything.
        public string IndentChars {
            get { 
                return indentChars; 
            }
            set {
                CheckReadOnly( "IndentChars" );

                if ( value == null ) {
                    throw new ArgumentNullException("value");
                }
                indentChars = value;
            }
        }

        // Whether or not indent attributes on new lines.
        public bool NewLineOnAttributes {
            get { 
                return newLineOnAttributes; 
            }
            set {
                CheckReadOnly( "NewLineOnAttributes" );
                newLineOnAttributes = value;
            }
        }

        // Whether or not the XmlWriter should close the underlying stream or TextWriter when Close is called on the XmlWriter.
        public bool CloseOutput {
            get { 
                return closeOutput; 
            }
            set {
                CheckReadOnly( "CloseOutput" );
                closeOutput = value;
            }
        }


    // Conformance
        // See ConformanceLevel enum for details.
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

        // Whether or not to check content characters that they are valid XML characters.
        public bool CheckCharacters {
            get {
                return checkCharacters;
            }
            set {
                CheckReadOnly( "CheckCharacters" );
                checkCharacters = value;
            }
        }

//
// Public methods
//
        public void Reset() {
            encoding = Encoding.UTF8;
            omitXmlDecl = false;
            newLineHandling = NewLineHandling.Replace;
            newLineChars = "\r\n";
            indent = TriState.Unknown;
            indentChars = "  ";
            newLineOnAttributes = false;
            closeOutput = false;

            conformanceLevel = ConformanceLevel.Document;
            checkCharacters = true;

            outputMethod = XmlOutputMethod.Xml;
            cdataSections = null;
            mergeCDataSections = false;
            mediaType = null;
            docTypeSystem = null;
            docTypePublic = null;
            standalone = XmlStandalone.Omit;

            isReadOnly = false;
        }

        // Deep clone all settings (except read-only, which is always set to false).  The original and new objects
        // can now be set independently of each other.
        public XmlWriterSettings Clone() {
            XmlWriterSettings clonedSettings = MemberwiseClone() as XmlWriterSettings;

            // Deep clone shared settings that are not immutable
            if ( cdataSections != null ) {
                clonedSettings.cdataSections = new ArrayList( cdataSections );
            }

            // Read-only setting is always false on clones
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

        // Specifies the method (Html, Xml, etc.) that will be used to serialize the result tree.
        public XmlOutputMethod OutputMethod {
            get { 
                return outputMethod; 
            }
            internal set {
                outputMethod = value;
            }
        }

        // Set of XmlQualifiedNames that identify any elements that need to have text children wrapped in CData sections.
        internal IList CDataSectionElements {
            get {
                if ( cdataSections == null ) {
                    cdataSections = new ArrayList();
                }
                return cdataSections;
            }
            set {
                CheckReadOnly( "CDataSectionElements" );

                if ( value == null ) {
                    throw new ArgumentNullException("value");
                }
                cdataSections = value;
            }
        }

        internal bool MergeCDataSections {
            get {
                return mergeCDataSections;
            }
            set {
                CheckReadOnly( "MergeCDataSections" );
                mergeCDataSections = value;
            }
        }

        // Used in Html writer when writing Meta element.  Null denotes the default media type.
        internal string MediaType {
            get {
                return mediaType;
            }
            set {
                CheckReadOnly( "MediaType" );
                mediaType = value;
            }
        }

        // System Id in doc-type declaration.  Null denotes the absence of the system Id.
        internal string DocTypeSystem {
            get {
                return docTypeSystem;
            }
            set {
                CheckReadOnly( "DocTypeSystem" );
                docTypeSystem = value;
            }
        }

        // Public Id in doc-type declaration.  Null denotes the absence of the public Id.
        internal string DocTypePublic {
            get {
                return docTypePublic;
            }
            set {
                CheckReadOnly( "DocTypePublic" );
                docTypePublic = value;
            }
        }

        // Yes for standalone="yes", No for standalone="no", and Omit for no standalone.
        internal XmlStandalone Standalone {
            get {
                return standalone;
            }
            set {
                CheckReadOnly( "Standalone" );
                standalone = value;
            }
        }

        // True if an xml declaration should automatically be output (no need to call WriteStartDocument)
        internal bool AutoXmlDeclaration {
            get {
                return autoXmlDecl;
            }
            set {
                CheckReadOnly( "AutoXmlDeclaration" );
                autoXmlDecl = value;
            }
        }

        // If TriState.Unknown, then Indent property was not explicitly set.  In this case, the AutoDetect output
        // method will default to Indent=true for Html and Indent=false for Xml.
        internal TriState InternalIndent {
            get {
                return indent;
            }
        }

        internal bool IsQuerySpecific {
            get {
                return cdataSections != null || docTypePublic != null ||
                       docTypeSystem != null || standalone == XmlStandalone.Yes;
            }
        }

        private void CheckReadOnly( string propertyName ) {
            if ( isReadOnly ) {
                throw new XmlException( Res.Xml_ReadOnlyProperty, "XmlWriterSettings." + propertyName );
            }
        }
    }
}
