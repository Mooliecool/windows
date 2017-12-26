//------------------------------------------------------------------------------
// <copyright file="XmlRawWriter.cs" company="Microsoft">
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
using System.IO;
using System.Diagnostics;
using System.Xml.XPath;
using System.Xml.Schema;
using System.Collections;

namespace System.Xml {


    /// <summary>
    /// Implementations of XmlRawWriter are intended to be wrapped by the XmlWellFormedWriter.  The
    /// well-formed writer performs many checks in behalf of the raw writer, and keeps state that the
    /// raw writer otherwise would have to keep.  Therefore, the well-formed writer will call the
    /// XmlRawWriter using the following rules, in order to make raw writers easier to implement:
    ///
    ///  1. The well-formed writer keeps a stack of element names, and always calls
    ///     WriteEndElement(string, string, string) instead of WriteEndElement().
    ///  2. The well-formed writer tracks namespaces, and will pass himself in via the
    ///     WellformedWriter property. It is used in the XmlRawWriter's implementation of IXmlNamespaceResolver.
    ///     Thus, LookupPrefix does not have to be implemented.
    ///  3. The well-formed writer tracks write states, so the raw writer doesn't need to.
    ///  4. The well-formed writer will always call StartElementContent.
    ///  5. The well-formed writer will always call WriteNamespaceDeclaration for namespace nodes,
    ///     rather than calling WriteStartAttribute().
    ///  6. The well-formed writer guarantees a well-formed document, including correct call sequences,
    ///     correct namespaces, and correct document rule enforcement.
    ///  7. All element and attribute names will be fully resolved and validated.  Null will never be
    ///     passed for any of the name parts.
    ///  8. The well-formed writer keeps track of xml:space and xml:lang.
    ///  9. The well-formed writer verifies NmToken, Name, and QName values and calls WriteString().
    /// </summary>
    internal abstract class XmlRawWriter : XmlWriter {
//
// Fields
//
        // base64 converter
        protected XmlRawWriterBase64Encoder base64Encoder;

        // namespace resolver
        protected IXmlNamespaceResolver resolver;

//
// Constants
//
        internal const int SurHighStart = 0xd800;    // 1101 10xx
        internal const int SurHighEnd   = 0xdbff;
        internal const int SurLowStart  = 0xdc00;    // 1101 11xx
        internal const int SurLowEnd    = 0xdfff;
        internal const int SurMask      = 0xfc00;    // 1111 11xx

//
// XmlWriter implementation
//
        // Raw writers do not have to track whether this is a well-formed document.
        public override void WriteStartDocument() {
            throw new InvalidOperationException( Res.GetString( Res.Xml_InvalidOperation ) );
        }

        public override void WriteStartDocument(bool standalone) {
            throw new InvalidOperationException( Res.GetString( Res.Xml_InvalidOperation ) );
        }

        public override void WriteEndDocument() {
            throw new InvalidOperationException( Res.GetString( Res.Xml_InvalidOperation ) );
        }

        public override void WriteDocType(string name, string pubid, string sysid, string subset) {
        }

        // Raw writers do not have to keep a stack of element names.
        public override void WriteEndElement() {
            throw new InvalidOperationException( Res.GetString( Res.Xml_InvalidOperation ) );
        }

        // Raw writers do not have to keep a stack of element names.
        public override void WriteFullEndElement() {
            throw new InvalidOperationException( Res.GetString( Res.Xml_InvalidOperation ) );
        }

        // By default, convert base64 value to string and call WriteString.
        public override void WriteBase64( byte[] buffer, int index, int count ) {
            if ( base64Encoder == null ) {
                base64Encoder = new XmlRawWriterBase64Encoder( this );
            }
            // Encode will call WriteRaw to write out the encoded characters
            base64Encoder.Encode( buffer, index, count );
        }

        // Raw writers do not have to keep track of namespaces.
        public override string LookupPrefix( string ns ) {
            throw new InvalidOperationException( Res.GetString( Res.Xml_InvalidOperation ) );
        }

        // Raw writers do not have to keep track of write states.
        public override WriteState WriteState {
            get {
                throw new InvalidOperationException( Res.GetString( Res.Xml_InvalidOperation ) );
            }
        }

        // Raw writers do not have to keep track of xml:space.
        public override XmlSpace XmlSpace { 
            get { throw new InvalidOperationException( Res.GetString( Res.Xml_InvalidOperation ) ); }
        }

        // Raw writers do not have to keep track of xml:lang.
        public override string XmlLang { 
            get { throw new InvalidOperationException( Res.GetString( Res.Xml_InvalidOperation ) ); }
        }

        // Raw writers do not have to verify NmToken values.
        public override void WriteNmToken( string name ) {
            throw new InvalidOperationException( Res.GetString( Res.Xml_InvalidOperation ) );
        }

        // Raw writers do not have to verify Name values.
        public override void WriteName( string name ) {
            throw new InvalidOperationException( Res.GetString( Res.Xml_InvalidOperation ) );
        }

        // Raw writers do not have to verify QName values.
        public override void WriteQualifiedName( string localName, string ns ) {
            throw new InvalidOperationException( Res.GetString( Res.Xml_InvalidOperation ) );
        }

        // Forward call to WriteString(string).
        public override void WriteCData( string text ) {
            WriteString( text );
        }

        // Forward call to WriteString(string).
        public override void WriteCharEntity( char ch ) {
            WriteString( new string( new char[] {ch} ) );
        }

        // Forward call to WriteString(string).
        public override void WriteSurrogateCharEntity( char lowChar, char highChar ) {
            WriteString( new string( new char[] {lowChar, highChar} ) );
        }

        // Forward call to WriteString(string).
        public override void WriteWhitespace( string ws ) {
            WriteString( ws );
        }

        // Forward call to WriteString(string).
        public override void WriteChars( char[] buffer, int index, int count ) {
            WriteString( new string( buffer, index, count ) );
        }

        // Forward call to WriteString(string).
        public override void WriteRaw(char[] buffer, int index, int count) {
            WriteString( new string( buffer, index, count ) );
        }

        // Forward call to WriteString(string).
        public override void WriteRaw(string data) {
            WriteString( data );
        }

        // Override in order to handle Xml simple typed values and to pass resolver for QName values
        public override void WriteValue(object value) {
            if ( value == null ) {
                throw new ArgumentNullException( "value" );
            }
            WriteString(XmlUntypedConverter.Untyped.ToString( value, resolver ) );
        }

        // Override in order to handle Xml simple typed values and to pass resolver for QName values
        public override void WriteValue( string value ) {
            WriteString( value );
        }

        // Copying to XmlRawWriter is not currently supported.
        public override void WriteAttributes( XmlReader reader, bool defattr ) {
            throw new InvalidOperationException( Res.GetString( Res.Xml_InvalidOperation ) );
        }

        public override void WriteNode( XmlReader reader, bool defattr ) {
            throw new InvalidOperationException( Res.GetString( Res.Xml_InvalidOperation ) );
        }

        public override void WriteNode( System.Xml.XPath.XPathNavigator navigator, bool defattr ) {
            throw new InvalidOperationException( Res.GetString( Res.Xml_InvalidOperation ) );
        }

//
// XmlRawWriter methods and properties
//
        // Get and set the namespace resolver that's used by this RawWriter to resolve prefixes.
        internal virtual IXmlNamespaceResolver NamespaceResolver  {
            get {
                return resolver;
            }
            set {
                resolver = value;
            }
        }

        // Write the xml declaration.  This must be the first call.
        internal virtual void WriteXmlDeclaration( XmlStandalone standalone ) {
        }
        internal virtual void WriteXmlDeclaration( string xmldecl ) {
        }

        // Called after an element's attributes have been enumerated, but before any children have been
        // enumerated.  This method must always be called, even for empty elements.
        internal abstract void StartElementContent();

        // WriteEndElement() and WriteFullEndElement() overloads, in which caller gives the full name of the
        // element, so that raw writers do not need to keep a stack of element names.  This method should
        // always be called instead of WriteEndElement() or WriteFullEndElement() without parameters.
        internal abstract void WriteEndElement( string prefix, string localName, string ns );
        internal virtual void WriteFullEndElement( string prefix, string localName, string ns ) {
            WriteEndElement( prefix, localName, ns );
        }

        internal virtual void WriteQualifiedName( string prefix, string localName, string ns ) {
            if ( prefix.Length != 0 ) {
                WriteString( prefix );
                WriteString( ":" );
            }
            WriteString( localName );
        }

        // This method must be called instead of WriteStartAttribute() for namespaces.
        internal abstract void WriteNamespaceDeclaration( string prefix, string ns );

        // This is called when the remainder of a base64 value should be output.
        internal virtual void WriteEndBase64() {
            // The Flush will call WriteRaw to write out the rest of the encoded characters
            base64Encoder.Flush();
        }

        internal virtual void Close( WriteState currentState ) {
            Close();
        }
    }
}

