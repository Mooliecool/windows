//------------------------------------------------------------------------------
// <copyright file="XmlTextReaderHelpers.cs" company="Microsoft">
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
using System.Text;
using System.Security;
using System.Xml.Schema;
using System.Collections;
using System.Diagnostics;
using System.Globalization;
using System.Collections.Generic;

namespace System.Xml {

    internal partial class XmlTextReaderImpl  {

//
// ParsingState
//
        // Parsing state (aka. scanner data) - holds parsing buffer and entity input data information
        private struct ParsingState {
            // character buffer
            internal char[] chars;
            internal int    charPos;
            internal int    charsUsed;
            internal Encoding encoding;
            internal bool   appendMode;
            
            // input stream & byte buffer
            internal Stream  stream;
            internal Decoder decoder;
            internal byte[]  bytes;
            internal int     bytePos;
            internal int     bytesUsed;

            // input text reader
            internal TextReader textReader;

            // current line number & position
            internal int   lineNo;
            internal int   lineStartPos;

            // base uri of the current entity
            internal string baseUriStr;
            internal Uri    baseUri;

            // eof flag of the entity
            internal bool   isEof;
            internal bool   isStreamEof;

            // entity type & id
            internal SchemaEntity entity;
            internal int          entityId;

            // normalization
            internal bool   eolNormalized;

            // EndEntity reporting
            internal bool   entityResolvedManually;

            internal void Clear() {
                chars = null;
                charPos = 0;
                charsUsed = 0;
                encoding = null;
                stream = null;
                decoder = null;
                bytes = null;
                bytePos = 0;
                bytesUsed = 0;
                textReader = null;
                lineNo = 1;
                lineStartPos = -1;
                baseUriStr = string.Empty;
                baseUri = null;
                isEof = false;
                isStreamEof = false;
                eolNormalized = true;
                entityResolvedManually = false;
            }

            internal void Close( bool closeInput ) {
                if ( closeInput ) {
                    if ( stream != null ) {
                        stream.Close();
                    }
                    else if ( textReader != null ) {
                        textReader.Close();
                    }
                }
            }

            internal int LineNo {
                get {
                    return lineNo;
                }
            }

            internal int LinePos {
                get {
                    return charPos - lineStartPos;
                }
            }
        }

//
// XmlContext
//
        private class XmlContext  {
            internal XmlSpace xmlSpace;
            internal string xmlLang;
            internal string defaultNamespace;
            internal XmlContext previousContext;

            internal XmlContext() {
                xmlSpace = XmlSpace.None;
                xmlLang = string.Empty;
                defaultNamespace = string.Empty;
                previousContext = null;
            }

            internal XmlContext( XmlContext previousContext ) {
                this.xmlSpace = previousContext.xmlSpace;
                this.xmlLang = previousContext.xmlLang;
                this.defaultNamespace = previousContext.defaultNamespace;
                this.previousContext = previousContext;
            }
        }

//
// NoNamespaceManager
//
        private class NoNamespaceManager : XmlNamespaceManager {    
            public NoNamespaceManager() : base () {}
            public override string DefaultNamespace { get { return string.Empty; } }
            public override void PushScope() {}
            public override bool PopScope() { return false; }
            public override void AddNamespace( string prefix, string uri ) {}
            public override void RemoveNamespace( string prefix, string uri ) {}
            public override IEnumerator GetEnumerator() { return null; }
            public override IDictionary<string,string> GetNamespacesInScope( XmlNamespaceScope scope ) { return null; }
            public override string LookupNamespace( string prefix ) { return string.Empty; }
            public override string LookupPrefix( string uri ) { return null; }
            public override bool HasNamespace( string prefix ) { return false; }
        }

//
// DtdParserProxy: IDtdParserAdapter proxy for XmlTextReaderImpl
//
        internal class DtdParserProxy : IDtdParserAdapter {
        // Fields
            private XmlTextReaderImpl reader;
            private DtdParser dtdParser;
            private SchemaInfo schemaInfo;

        // Constructors
            internal DtdParserProxy( XmlTextReaderImpl reader ) {
                this.reader = reader;
                this.dtdParser = new DtdParser( this );
            }

            internal DtdParserProxy( XmlTextReaderImpl reader, SchemaInfo schemaInfo ) {
                this.reader = reader;
                this.schemaInfo = schemaInfo;
            }

            internal DtdParserProxy( string baseUri, string docTypeName, string publicId, string systemId, 
                                     string internalSubset, XmlTextReaderImpl reader ) {
                this.reader = reader;
                this.dtdParser = new DtdParser( baseUri, docTypeName, publicId, systemId, internalSubset, this );
            }


        // DtdParser proxies
            internal void Parse( bool saveInternalSubset ) {
                if ( dtdParser == null ) {
                    throw new InvalidOperationException();
                }
                dtdParser.Parse( saveInternalSubset );
            }

            internal SchemaInfo DtdSchemaInfo {
                get {
                    return dtdParser != null ? dtdParser.SchemaInfo : schemaInfo;
                }
            }

            internal string InternalDtdSubset {
                get {
                    if ( dtdParser == null ) {
                        throw new InvalidOperationException();
                    }
                    return dtdParser.InternalSubset;
                }
            }

        // IDtdParserAdapter proxies
            XmlNameTable IDtdParserAdapter.NameTable { 
                get { return reader.DtdParserProxy_NameTable; } 
            }
            
            XmlNamespaceManager IDtdParserAdapter.NamespaceManager {
                get { return reader.DtdParserProxy_NamespaceManager; }
            }
            
            bool IDtdParserAdapter.DtdValidation { 
                get { return reader.DtdParserProxy_DtdValidation; } 
            }
            
            bool IDtdParserAdapter.Normalization { 
                get { return reader.DtdParserProxy_Normalization; } 
            }
            
            bool IDtdParserAdapter.Namespaces { 
                get { return reader.DtdParserProxy_Namespaces; } 
            }
            
            bool IDtdParserAdapter.V1CompatibilityMode { 
                get { return reader.DtdParserProxy_V1CompatibilityMode; } 
            }
            
            Uri IDtdParserAdapter.BaseUri { 
                get { return reader.DtdParserProxy_BaseUri; } 
            }
            
            bool IDtdParserAdapter.IsEof { 
                get { return reader.DtdParserProxy_IsEof; } 
            }
            
            char[] IDtdParserAdapter.ParsingBuffer { 
                get { return reader.DtdParserProxy_ParsingBuffer; } 
            }
            
            int IDtdParserAdapter.ParsingBufferLength { 
                get { return reader.DtdParserProxy_ParsingBufferLength; } 
            }
            
            int IDtdParserAdapter.CurrentPosition { 
                get { return reader.DtdParserProxy_CurrentPosition; } 
                set { reader.DtdParserProxy_CurrentPosition = value; } 
            }

            int IDtdParserAdapter.EntityStackLength { 
                get { return reader.DtdParserProxy_EntityStackLength; } 
            }
            
            bool IDtdParserAdapter.IsEntityEolNormalized { 
                get { return reader.DtdParserProxy_IsEntityEolNormalized; } 
            }
            
            ValidationEventHandler IDtdParserAdapter.EventHandler { 
                get { return reader.DtdParserProxy_EventHandler; } 
                set { reader.DtdParserProxy_EventHandler = value; } 
            }
            
            void IDtdParserAdapter.OnNewLine( int pos ) { 
                reader.DtdParserProxy_OnNewLine( pos ); 
            }
            
            int IDtdParserAdapter.LineNo { 
                get { return reader.DtdParserProxy_LineNo; } 
            }
            
            int IDtdParserAdapter.LineStartPosition { 
                get { return reader.DtdParserProxy_LineStartPosition; } 
            }
            
            int IDtdParserAdapter.ReadData() { 
                return reader.DtdParserProxy_ReadData(); 
            }
            
            void IDtdParserAdapter.SendValidationEvent( XmlSeverityType severity, XmlSchemaException exception ) { 
                reader.DtdParserProxy_SendValidationEvent( severity, exception ); 
            }
            
            int IDtdParserAdapter.ParseNumericCharRef( BufferBuilder internalSubsetBuilder ) { 
                return reader.DtdParserProxy_ParseNumericCharRef( internalSubsetBuilder ); 
            }
            
            int IDtdParserAdapter.ParseNamedCharRef( bool expand, BufferBuilder internalSubsetBuilder ) { 
                return reader.DtdParserProxy_ParseNamedCharRef( expand, internalSubsetBuilder ); 
            }
            
            void IDtdParserAdapter.ParsePI( BufferBuilder sb ) { 
                reader.DtdParserProxy_ParsePI( sb ); 
            }
            
            void IDtdParserAdapter.ParseComment( BufferBuilder sb ) { 
                reader.DtdParserProxy_ParseComment( sb ); 
            }
            
            bool IDtdParserAdapter.PushEntity( SchemaEntity entity, int entityId ) { 
                return reader.DtdParserProxy_PushEntity( entity, entityId ); 
            }
            
            bool IDtdParserAdapter.PopEntity( out SchemaEntity oldEntity, out int newEntityId ) { 
                return reader.DtdParserProxy_PopEntity( out oldEntity, out newEntityId ); 
            }
            
            bool IDtdParserAdapter.PushExternalSubset( string systemId, string publicId ) { 
                return reader.DtdParserProxy_PushExternalSubset( systemId, publicId ); 
            }
            
            void IDtdParserAdapter.PushInternalDtd( string baseUri, string internalDtd ) { 
                reader.DtdParserProxy_PushInternalDtd( baseUri, internalDtd ); 
            }
            
            void IDtdParserAdapter.Throw( Exception e ) { 
                reader.DtdParserProxy_Throw( e ); 
            }
            
            void IDtdParserAdapter.OnSystemId( string systemId, LineInfo keywordLineInfo, LineInfo systemLiteralLineInfo ) { 
                reader.DtdParserProxy_OnSystemId( systemId, keywordLineInfo, systemLiteralLineInfo ); 
            }

            void IDtdParserAdapter.OnPublicId( string publicId, LineInfo keywordLineInfo, LineInfo publicLiteralLineInfo ) { 
                reader.DtdParserProxy_OnPublicId( publicId, keywordLineInfo, publicLiteralLineInfo ); 
            }
        }

//
// NodeData
//
        private class NodeData : IComparable  {
            // static instance with no data - is used when XmlTextReader is closed
            static NodeData s_None;

            // NOTE: Do not use this property for reference comparison. It may not be unique.
            internal static NodeData None {
                get {
                    if ( s_None == null ) {
                        // no locking; s_None is immutable so it's not a problem that it may get initialized more than once
                        s_None = new NodeData();
                    }
                    return s_None;
                }
            }

            // type
            internal XmlNodeType type;

            // name
            internal string localName;
            internal string prefix;
            internal string ns;
            internal string nameWPrefix;

            // value:
            // value == null -> the value is kept in the 'chars' buffer starting at valueStartPos and valueLength long
            string value;          
            char[] chars;
            int    valueStartPos;   
            int    valueLength;
            
            // main line info
            internal LineInfo   lineInfo;

            // second line info
            internal LineInfo   lineInfo2;

            // quote char for attributes
            internal char   quoteChar;

            // depth
            internal int    depth;

            // empty element / default attribute
            bool   isEmptyOrDefault;

            // entity id
            internal int    entityId;

            // helper members
            internal bool   xmlContextPushed;

            // attribute value chunks
            internal NodeData nextAttrValueChunk;

            // type info
            internal object schemaType;
            internal object typedValue;

            internal NodeData() {
                Clear( XmlNodeType.None );
                xmlContextPushed = false;
            }

            internal int LineNo { 
                get { 
                    return lineInfo.lineNo;
                }
            }

            internal int LinePos { 
                get { 
                    return lineInfo.linePos;
                }
            }

            internal bool IsEmptyElement {
                get {
                    return type == XmlNodeType.Element && isEmptyOrDefault;
                }
                set {
                    Debug.Assert( type == XmlNodeType.Element );
                    isEmptyOrDefault = value;
                }
            }

            internal bool IsDefaultAttribute {
                get {
                    return type == XmlNodeType.Attribute && isEmptyOrDefault;
                }
                set {
                    Debug.Assert( type == XmlNodeType.Attribute );
                    isEmptyOrDefault = value;
                }
            }

            internal bool ValueBuffered {
                get { 
                    return value == null;
                }
            }

            internal string StringValue {
                get {
                    Debug.Assert( valueStartPos >= 0 || this.value != null, "Value not ready." );

                    if ( this.value == null ) {
                        this.value = new string( chars, valueStartPos, valueLength );
                    }
                    return this.value;
                }
            }

            internal void TrimSpacesInValue() {
                if ( ValueBuffered ) {
                    XmlComplianceUtil.StripSpaces( chars, valueStartPos, ref valueLength );
                }
                else {
                    value = XmlComplianceUtil.StripSpaces( value );
                }
            }

            internal void Clear( XmlNodeType type ) {
                this.type = type;
                ClearName();
                value = string.Empty;  
                valueStartPos = -1;
                nameWPrefix = string.Empty;
                schemaType = null;
                typedValue = null;
            }

            internal void ClearName() {
                localName = string.Empty;
                prefix = string.Empty;
                ns = string.Empty;
                nameWPrefix = string.Empty;
            }

            internal void SetLineInfo( int lineNo, int linePos ) {
                lineInfo.Set( lineNo, linePos );
            }

            internal void SetLineInfo2( int lineNo, int linePos ) {
                lineInfo2.Set( lineNo, linePos );
            }

            internal void SetValueNode( XmlNodeType type, string value ) {
                Debug.Assert( value != null );

                this.type = type;
                ClearName();
                this.value = value;
                this.valueStartPos = -1;
            }

            internal void SetValueNode( XmlNodeType type, char[] chars, int startPos, int len ) {
                this.type = type;
                ClearName();
                
                this.value = null;
                this.chars = chars;
                this.valueStartPos = startPos;
                this.valueLength = len;
            }

            internal void SetNamedNode( XmlNodeType type, string localName ) {
                SetNamedNode( type, localName, string.Empty, localName );
            }

            internal void SetNamedNode( XmlNodeType type, string localName, string prefix, string nameWPrefix ) {
                Debug.Assert( localName != null );
                Debug.Assert( localName.Length > 0 );

                this.type = type;
                this.localName = localName;
                this.prefix = prefix;
                this.nameWPrefix = nameWPrefix;
                this.ns = string.Empty;
                this.value = string.Empty;
                this.valueStartPos = -1;
            }

            internal void SetValue( string value ) {
                this.valueStartPos = -1;
                this.value = value;
            }

            internal void SetValue( char[] chars, int startPos, int len ) {
                this.value = null;
                this.chars = chars;
                this.valueStartPos = startPos;
                this.valueLength = len;
            }

            internal void OnBufferInvalidated() {
                if ( this.value == null ) {
                    Debug.Assert( valueStartPos != -1 );
                    Debug.Assert( chars != null );
                    this.value = new string( chars, valueStartPos, valueLength );
                }
                valueStartPos = -1;
            }

            internal string GetAtomizedValue( XmlNameTable nameTable ) {
                if ( this.value == null ) {
                    Debug.Assert( valueStartPos != -1 );
                    Debug.Assert( chars != null );
                    return nameTable.Add( chars, valueStartPos, valueLength );
                }
                else {
                    return nameTable.Add( this.value );
                }
            }

            internal void CopyTo( BufferBuilder sb ) {
                CopyTo( 0, sb );
            }

            internal void CopyTo( int valueOffset, BufferBuilder sb ) {
                if ( value == null ) {
                    Debug.Assert( valueStartPos != -1 );
                    Debug.Assert( chars != null );
                    sb.Append( chars, valueStartPos + valueOffset, valueLength - valueOffset );
                }
                else {
                    if ( valueOffset <= 0 ) {
                        sb.Append( value );
                    }
                    else {
                        sb.Append( value, valueOffset, value.Length - valueOffset );
                    }
                }
            }

            internal int CopyTo( int valueOffset, char[] buffer, int offset, int length ) {
                if ( value == null ) {
                    Debug.Assert( valueStartPos != -1 );
                    Debug.Assert( chars != null );
                    int copyCount = valueLength - valueOffset;
                    if ( copyCount > length ) {
                        copyCount = length;
                    }
                    Buffer.BlockCopy( chars, ( valueStartPos + valueOffset ) * 2, buffer, offset * 2, copyCount * 2 );
                    return copyCount;
                }
                else {
                    int copyCount = value.Length - valueOffset;
                    if ( copyCount > length ) {
                        copyCount = length;
                    }
                    value.CopyTo( valueOffset, buffer, offset, copyCount );
                    return copyCount;
                }
            }

            internal int CopyToBinary( IncrementalReadDecoder decoder, int valueOffset ) {
                if ( value == null ) {
                    Debug.Assert( valueStartPos != -1 );
                    Debug.Assert( chars != null );
                    return decoder.Decode( chars, valueStartPos + valueOffset, valueLength - valueOffset );
                }
                else {
                    return decoder.Decode( value, valueOffset, value.Length - valueOffset );
                }
            }

            internal void AdjustLineInfo( int valueOffset, bool isNormalized, ref LineInfo lineInfo ) {
                if ( valueOffset == 0 ) {
                    return;
                }
                if ( valueStartPos != -1 ) {
                    XmlTextReaderImpl.AdjustLineInfo( chars, valueStartPos, valueStartPos + valueOffset, isNormalized, ref lineInfo );
                }
                else {
                    char[] chars = value.ToCharArray( 0, valueOffset );
                    XmlTextReaderImpl.AdjustLineInfo( chars, 0, chars.Length, isNormalized, ref lineInfo );
                }
            }

            // This should be inlined by JIT compiler
            internal string GetNameWPrefix( XmlNameTable nt ) { 
                if ( nameWPrefix != null ) {
                    return nameWPrefix;
                }
                else {
                    return CreateNameWPrefix( nt );
                }
            }

            internal string CreateNameWPrefix( XmlNameTable nt ) { 
                Debug.Assert( nameWPrefix == null );
                if ( prefix.Length == 0 ) {
                    nameWPrefix = localName;
                }
                else {
                    nameWPrefix = nt.Add( string.Concat( prefix, ":", localName ) );
                }
                return nameWPrefix;
            }

            int IComparable.CompareTo( object obj ) {
                NodeData other = obj as NodeData;
                if ( other != null ) {
                    if ( Ref.Equal( localName, other.localName ) ) {
                        if ( Ref.Equal( ns, other.ns ) ) {
                            return 0;
                        }
                        else {
                            return string.CompareOrdinal( ns, other.ns );
                        }
                    }
                    else {
                        return string.CompareOrdinal( localName, other.localName );
                    }
                }
                else {
                    Debug.Assert( false, "We should never get to this point." );
                    return GetHashCode().CompareTo( other.GetHashCode() );
                }
            }
        }
    }
}
