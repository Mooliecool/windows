
//------------------------------------------------------------------------------
// <copyright file="XmlTextReaderImpl.cs" company="Microsoft">
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
using System.Threading;
using System.Xml.Schema;
using System.Collections;
using System.Diagnostics;
using System.Globalization;
using System.Collections.Generic;

namespace System.Xml {

    internal partial class XmlTextReaderImpl : XmlReader, IXmlLineInfo, IXmlNamespaceResolver {
//
// Private helper types
//
        // ParsingFunction = what should the reader do when the next Read() is called
        enum ParsingFunction {
            ElementContent = 0,
            NoData,
            OpenUrl,
            SwitchToInteractive,
            SwitchToInteractiveXmlDecl,
            DocumentContent,
            MoveToElementContent,
            PopElementContext,
            PopEmptyElementContext,
            ResetAttributesRootLevel,
            Error,
            Eof,
            ReaderClosed,
            EntityReference,
            InIncrementalRead,
            FragmentAttribute,
            ReportEndEntity,
            AfterResolveEntityInContent,
            AfterResolveEmptyEntityInContent,
            XmlDeclarationFragment,
            GoToEof,
            PartialTextValue,

            // these two states must be last; see InAttributeValueIterator property
            InReadAttributeValue,
            InReadValueChunk,
            InReadContentAsBinary,
            InReadElementContentAsBinary,
        }

        enum ParsingMode {
            Full, 
            SkipNode,
            SkipContent,
        }

        enum EntityType {
            CharacterDec,
            CharacterHex,
            CharacterNamed,
            Expanded,
            ExpandedInAttribute,
            Skipped,
            Unexpanded,
            FakeExpanded,
        }

        enum EntityExpandType {
            OnlyGeneral,
            OnlyCharacter,
            All,
        }

        enum IncrementalReadState {
            // Following values are used in ReadText, ReadBase64 and ReadBinHex (V1 streaming methods)
            Text,
            StartTag,
            PI,
            CDATA,
            Comment,
            Attributes,
            AttributeValue,
            ReadData,
            EndElement,
            End,

            // Following values are used in ReadTextChunk, ReadContentAsBase64 and ReadBinHexChunk (V2 streaming methods)
            ReadValueChunk_OnCachedValue,
            ReadValueChunk_OnPartialValue,

            ReadContentAsBinary_OnCachedValue,
            ReadContentAsBinary_OnPartialValue,
            ReadContentAsBinary_End,
        }

//
// Fields
//
        // XmlCharType instance
        XmlCharType xmlCharType = XmlCharType.Instance;

        // current parsing state (aka. scanner data) 
        ParsingState ps;

        // parsing function = what to do in the next Read() (3-items-long stack, usually used just 2 level)
        ParsingFunction parsingFunction;
        ParsingFunction nextParsingFunction;
        ParsingFunction nextNextParsingFunction;

        // stack of nodes
        NodeData[] nodes;

        // current node
        NodeData curNode;

        // current index
        int  index = 0;

        // attributes info
        int  curAttrIndex = -1;
        int  attrCount;
        int  attrHashtable;
        int  attrDuplWalkCount;
        bool attrNeedNamespaceLookup;
        bool fullAttrCleanup;
        NodeData[] attrDuplSortingArray;

        // name table
        XmlNameTable nameTable;
        bool         nameTableFromSettings;

        // resolver
        XmlResolver         xmlResolver;

        // this is only for constructors that takes url 
        string              url = string.Empty;
        CompressedStack     compressedStack;

        // settings
        bool                normalize;
        bool                supportNamespaces = true;
        WhitespaceHandling  whitespaceHandling;
        bool                prohibitDtd;
        EntityHandling      entityHandling;
        bool                ignorePIs;
        bool                ignoreComments;
        bool                checkCharacters;
        int                 lineNumberOffset;
        int                 linePositionOffset;
        bool                closeInput;

        // this flag enables XmlTextReader backwards compatibility; 
        // when false, the reader has been created via XmlReader.Create
        bool                v1Compat;

        // namespace handling
        XmlNamespaceManager namespaceManager;
        string lastPrefix = string.Empty;

        // xml context (xml:space, xml:lang, default namespace)
        XmlContext              xmlContext;

        // stack of parsing states (=stack of entities)
        private ParsingState[]  parsingStatesStack;
        private int             parsingStatesStackTop = -1;

        // current node base uri and encoding
        string      reportedBaseUri;
        Encoding    reportedEncoding;

        // DTD
        DtdParserProxy  dtdParserProxy;

        // fragment parsing
        XmlNodeType         fragmentType = XmlNodeType.Document;
        bool                fragment;
        XmlParserContext    fragmentParserContext;

        // incremental read
        IncrementalReadDecoder      incReadDecoder;
        IncrementalReadState        incReadState;
        int                         incReadDepth;
        int                         incReadLeftStartPos;
        int                         incReadLeftEndPos;
        LineInfo                    incReadLineInfo;
        IncrementalReadCharsDecoder readCharsDecoder;
        BinHexDecoder               binHexDecoder;
        Base64Decoder               base64Decoder;

        // ReadAttributeValue helpers
        int     attributeValueBaseEntityId;
        bool    emptyEntityInAttributeResolved;

        // XmlValidatingReader helpers
        ValidationEventHandler  validationEventHandler;
        bool                    validatingReaderCompatFlag;
        bool                    addDefaultAttributesAndNormalize;
        XmlQualifiedName        qName;

        // misc
        BufferBuilder   stringBuilder;
        bool            rootElementParsed;
        bool            standalone;
        int             nextEntityId = 1;
        ParsingMode     parsingMode;
        ReadState       readState = ReadState.Initial;
        SchemaEntity    lastEntity;
        bool            afterResetState;
        int             documentStartBytePos;
        int             readValueOffset;

        // DOM helpers
        bool            disableUndeclaredEntityCheck;

        // Outer XmlReader exposed to the user - either XmlTextReader or XmlTextReaderImpl (when created via XmlReader.Create).
        // Virtual methods called from within XmlTextReaderImpl must be called on the outer reader so in case the user overrides
        // some of the XmlTextReader methods we will call the overriden version.
        XmlReader       outerReader;

//
// Atomized string constants
//
        private string Xml;
        private string XmlNs;

//
// Constants
//
        private const int MaxBytesToMove = 128;
        private const int ApproxXmlDeclLength = 80;
        private const int NodesInitialSize = 8;
        private const int InitialAttributesCount = 4;
        private const int InitialParsingStateStackSize = 2;
        private const int InitialParsingStatesDepth = 2;
        private const int DtdChidrenInitialSize = 2;
        private const int MaxByteSequenceLen = 6;  // max bytes per character
        private const int MaxAttrDuplWalkCount = 250;
        private const int MinWhitespaceLookahedCount = 4096;
 
        private const string XmlDeclarationBegining = "<?xml";

        internal const int SurHighStart = 0xd800;    
        internal const int SurHighEnd   = 0xdbff;
        internal const int SurLowStart  = 0xdc00;    
        internal const int SurLowEnd    = 0xdfff;

//
// Constructors
//
        internal XmlTextReaderImpl() {
            curNode = new NodeData();
            parsingFunction = ParsingFunction.NoData;
        }

        // Initializes a new instance of the XmlTextReaderImpl class with the specified XmlNameTable.
        // This constructor is used when creating XmlTextReaderImpl for V1 XmlTextReader
        internal XmlTextReaderImpl( XmlNameTable nt ) {
            Debug.Assert( nt != null );

            v1Compat = true;
            outerReader = this;

            nameTable = nt;
            nt.Add( string.Empty );

            xmlResolver = new XmlUrlResolver();

            Xml = nt.Add( "xml" );
            XmlNs = nt.Add( "xmlns" );

            Debug.Assert( index == 0 );
            nodes = new NodeData[ NodesInitialSize ];
            nodes[0] = new NodeData();
            curNode = nodes[0];

            stringBuilder = new BufferBuilder();
            xmlContext = new XmlContext();

            parsingFunction = ParsingFunction.SwitchToInteractiveXmlDecl;
            nextParsingFunction = ParsingFunction.DocumentContent;

            entityHandling = EntityHandling.ExpandCharEntities;
            whitespaceHandling = WhitespaceHandling.All;
            closeInput = true;

            ps.lineNo = 1;
            ps.lineStartPos = -1;
        }

        // This constructor is used when creating XmlTextReaderImpl reader via "XmlReader.Create(..)"
        private XmlTextReaderImpl( XmlResolver resolver, XmlReaderSettings settings, XmlParserContext context ) {
            v1Compat = false;
            outerReader = this;

            xmlContext = new XmlContext();        

            // create or get nametable and namespace manager from XmlParserContext
            XmlNameTable nt = settings.NameTable;
            if ( context == null ) {
                if ( nt == null ) {
                    nt = new NameTable();
                    Debug.Assert( nameTableFromSettings == false );
                }
                else {
                    nameTableFromSettings = true;
                }
                nameTable = nt;
                namespaceManager = new XmlNamespaceManager( nt );
            }
            else {
                SetupFromParserContext( context, settings );
                nt = nameTable;
            }

            nt.Add( string.Empty );
            Xml = nt.Add( "xml" );
            XmlNs = nt.Add( "xmlns" );

            xmlResolver = resolver;

            Debug.Assert( index == 0 );

            nodes = new NodeData[ NodesInitialSize ];
            nodes[0] = new NodeData();
            curNode = nodes[0];

            stringBuilder = new BufferBuilder();

            entityHandling = EntityHandling.ExpandEntities;
            whitespaceHandling = ( settings.IgnoreWhitespace ) ? WhitespaceHandling.Significant : WhitespaceHandling.All;
            normalize = true;
            ignorePIs = settings.IgnoreProcessingInstructions;
            ignoreComments = settings.IgnoreComments;
            checkCharacters = settings.CheckCharacters;
            lineNumberOffset = settings.LineNumberOffset;
            linePositionOffset = settings.LinePositionOffset;
            ps.lineNo = lineNumberOffset + 1;
            ps.lineStartPos = - linePositionOffset - 1;
            curNode.SetLineInfo( ps.LineNo - 1, ps.LinePos - 1 );
            prohibitDtd = settings.ProhibitDtd;

            fragmentParserContext = context;

            parsingFunction = ParsingFunction.SwitchToInteractiveXmlDecl;
            nextParsingFunction = ParsingFunction.DocumentContent;

            switch ( settings.ConformanceLevel ) { 
                case ConformanceLevel.Auto:
                    fragmentType = XmlNodeType.None;
                    fragment = true;
                    break;
                case ConformanceLevel.Fragment:
                    fragmentType = XmlNodeType.Element;
                    fragment = true;
                    break;
                case ConformanceLevel.Document:
                    fragmentType = XmlNodeType.Document;
                    break;
                default:
                    Debug.Assert( false );
                    goto case ConformanceLevel.Document;
            }
        }

        // Initializes a new instance of the XmlTextReaderImpl class with the specified stream, baseUri and nametable
        // This constructor is used when creating XmlTextReaderImpl for V1 XmlTextReader
        internal XmlTextReaderImpl( Stream input ) : this( string.Empty, input, new NameTable() ) {
        }
        internal XmlTextReaderImpl( Stream input, XmlNameTable nt ) : this( string.Empty, input, nt ) {
        }
        internal XmlTextReaderImpl( string url, Stream input ) : this( url, input, new NameTable() ) {
        }
        internal XmlTextReaderImpl( string url, Stream input, XmlNameTable nt ) : this( nt ) {
            namespaceManager = new XmlNamespaceManager( nt );
            if ( url == null || url.Length == 0 ) {
                InitStreamInput( input, null );
            }
            else {
                InitStreamInput( url, input, null );
            }
            reportedBaseUri = ps.baseUriStr;
            reportedEncoding = ps.encoding;
        }

        // Initializes a new instance of the XmlTextReaderImpl class with the specified TextReader, baseUri and XmlNameTable.
        // This constructor is used when creating XmlTextReaderImpl for V1 XmlTextReader
        internal XmlTextReaderImpl( TextReader input ) : this( string.Empty, input, new NameTable() ) {
        }
        internal XmlTextReaderImpl( TextReader input, XmlNameTable nt ) : this( string.Empty, input, nt ) {
        }
        internal XmlTextReaderImpl( string url, TextReader input ) : this( url, input, new NameTable() ) {
        }
        internal XmlTextReaderImpl( string url, TextReader input, XmlNameTable nt ) : this( nt ) {
            namespaceManager = new XmlNamespaceManager( nt );
            reportedBaseUri = ( url != null ) ? url : string.Empty;
            InitTextReaderInput( reportedBaseUri, input );
            reportedEncoding = ps.encoding;
        }

        // Initializes a new instance of XmlTextReaderImpl class for parsing fragments with the specified stream, fragment type and parser context
        // This constructor is used when creating XmlTextReaderImpl for V1 XmlTextReader
        internal XmlTextReaderImpl( Stream xmlFragment, XmlNodeType fragType, XmlParserContext context )
            :  this( ( context != null && context.NameTable != null ) ? context.NameTable : new NameTable() ) {

            Encoding enc = ( context != null ) ? context.Encoding : null;
            if ( context == null || context.BaseURI == null || context.BaseURI.Length == 0 ) {
                InitStreamInput( xmlFragment, enc );
            }
            else {
                InitStreamInput( xmlResolver.ResolveUri( null, context.BaseURI ), xmlFragment, enc );
            }
            InitFragmentReader( fragType, context, false );

            reportedBaseUri = ps.baseUriStr;
            reportedEncoding = ps.encoding;
        }

        // Initializes a new instance of XmlTextRreaderImpl class for parsing fragments with the specified string, fragment type and parser context
        // This constructor is used when creating XmlTextReaderImpl for V1 XmlTextReader
        internal XmlTextReaderImpl( string xmlFragment, XmlNodeType fragType, XmlParserContext context )
            : this( null == context || null == context.NameTable ? new NameTable() : context.NameTable ) {

            if ( context == null ) {
                InitStringInput( string.Empty, Encoding.Unicode, xmlFragment );
            }
            else {
                reportedBaseUri = context.BaseURI;
                InitStringInput( context.BaseURI, Encoding.Unicode, xmlFragment );
            }
            InitFragmentReader( fragType, context, false );
            reportedEncoding = ps.encoding;
        }

        // Following constructor assumes that the fragment node type == XmlDecl
        // We handle this node type separately because there is not real way to determine what the
        // "innerXml" of an XmlDecl is. This internal function is required by DOM. When(if) we handle/allow
        // all nodetypes in InnerXml then we should support them as part of fragment constructor as well.
        // Until then, this internal function will have to do.
        internal XmlTextReaderImpl( string xmlFragment, XmlParserContext context )
            : this(null == context || null == context.NameTable ? new NameTable() : context.NameTable ) {
            InitStringInput( ( context == null ) ? string.Empty : context.BaseURI, Encoding.Unicode, string.Concat( "<?xml ", xmlFragment, "?>" ) );
            InitFragmentReader( XmlNodeType.XmlDeclaration, context, true );
        }

        // Initializes a new instance of the XmlTextReaderImpl class with the specified url and XmlNameTable.
        // This constructor is used when creating XmlTextReaderImpl for V1 XmlTextReader
        public XmlTextReaderImpl( string url ) : this( url, new NameTable() ) {
        }
        public XmlTextReaderImpl( string url, XmlNameTable nt ): this( nt ) {
            if ( url == null ) {
                throw new ArgumentNullException( "url" );
            }
            if ( url.Length == 0 ) {
                throw new ArgumentException( Res.GetString( Res.Xml_EmptyUrl ), "url" );
            }
            namespaceManager = new XmlNamespaceManager( nt );

            compressedStack = CompressedStack.Capture();

            this.url = url;

            ps.baseUri = xmlResolver.ResolveUri( null, url );
            ps.baseUriStr = ps.baseUri.ToString();
            reportedBaseUri = ps.baseUriStr;

            parsingFunction = ParsingFunction.OpenUrl;
        }

        // Initializes a new instance of the XmlTextReaderImpl class with the specified arguments.
        // This constructor is used when creating XmlTextReaderImpl via XmlReader.Create
        internal XmlTextReaderImpl( Stream stream, byte[] bytes, int byteCount, XmlReaderSettings settings, Uri baseUri, string baseUriStr, 
                                    XmlParserContext context, bool closeInput ) 
            : this( settings.GetXmlResolver(), settings, context ) {

            Encoding enc = null;

            // get BaseUri and Encoding from XmlParserContext
            if ( context != null ) {
                if ( context.BaseURI != null && context.BaseURI.Length > 0 && 
                    !UriEqual( baseUri, baseUriStr, context.BaseURI, settings.GetXmlResolver() ) ) {
                    if ( baseUriStr.Length > 0 ) {
                        Throw( Res.Xml_DoubleBaseUri );
                    }
                    Debug.Assert( baseUri == null );
                    baseUriStr = context.BaseURI;
                }
                enc = context.Encoding;
            }

            // init ParsingState
            InitStreamInput( baseUri, baseUriStr, stream, bytes, byteCount, enc );

            this.closeInput = closeInput;

            reportedBaseUri = ps.baseUriStr;
            reportedEncoding = ps.encoding;

            // parse DTD
            if ( context != null && context.HasDtdInfo ) {
                if ( prohibitDtd ) {
                    ThrowWithoutLineInfo( Res.Xml_DtdIsProhibitedEx, string.Empty );
                }
                ParseDtdFromParserContext();
            }
        }

        // Initializes a new instance of the XmlTextReaderImpl class with the specified arguments.
        // This constructor is used when creating XmlTextReaderImpl via XmlReader.Create
        internal XmlTextReaderImpl( TextReader input, XmlReaderSettings settings, string baseUriStr, XmlParserContext context ) 
            : this( settings.GetXmlResolver(), settings, context ) {

            // get BaseUri from XmlParserContext
            if ( context != null ) {
                Debug.Assert( baseUriStr == string.Empty, "BaseURI can come either from XmlParserContext or from the constructor argument, not from both" );
                if ( context.BaseURI != null ) {
                    baseUriStr = context.BaseURI;
                }
            }

            // init ParsingState
            InitTextReaderInput( baseUriStr, input );

            this.closeInput = settings.CloseInput;

            reportedBaseUri = ps.baseUriStr;
            reportedEncoding = ps.encoding;

            // parse DTD
            if ( context != null && context.HasDtdInfo ) {
                if ( prohibitDtd ) {
                    ThrowWithoutLineInfo( Res.Xml_DtdIsProhibitedEx, string.Empty );
                }
                ParseDtdFromParserContext();
            }
        }

        // Initializes a new instance of the XmlTextReaderImpl class for fragment parsing.
        // This constructor is used by XmlBinaryReader for nested text XML
        internal XmlTextReaderImpl( string xmlFragment, XmlParserContext context, XmlReaderSettings settings )
            : this( null, settings, context ) {
            InitStringInput( string.Empty, Encoding.Unicode, xmlFragment );
            reportedBaseUri = ps.baseUriStr;
            reportedEncoding = ps.encoding;
        }

//
// XmlReader members
//
        // Returns the current settings of the reader
        public override XmlReaderSettings Settings { 
            get {
                if ( v1Compat ) {
                    return null;
                }

                XmlReaderSettings settings = new XmlReaderSettings();

                if ( nameTableFromSettings ) {
                    settings.NameTable = nameTable;
                }

                switch ( fragmentType ) {
                    case XmlNodeType.None:      settings.ConformanceLevel = ConformanceLevel.Auto;      break;
                    case XmlNodeType.Element:   settings.ConformanceLevel = ConformanceLevel.Fragment;  break;
                    case XmlNodeType.Document:  settings.ConformanceLevel = ConformanceLevel.Document;  break;
                    default:                    Debug.Assert( false );  goto case XmlNodeType.None;
                }
                settings.CheckCharacters = checkCharacters;
                settings.LineNumberOffset = lineNumberOffset;
                settings.LinePositionOffset = linePositionOffset;
                settings.IgnoreWhitespace = ( whitespaceHandling == WhitespaceHandling.Significant );
                settings.IgnoreProcessingInstructions = ignorePIs;
                settings.IgnoreComments = ignoreComments;
                settings.ProhibitDtd = prohibitDtd;

                settings.ReadOnly = true;
                return settings;
            }
        }

        // Returns the type of the current node.
        public override XmlNodeType NodeType { 
            get {
                return curNode.type;
            }
        }

        // Returns the name of the current node, including prefix.
        public override string Name { 
            get {
                return curNode.GetNameWPrefix( nameTable );
            }
        }

        // Returns local name of the current node (without prefix)
        public override string LocalName { 
            get {
                return curNode.localName;
            }
        }

        // Returns namespace name of the current node.
        public override string NamespaceURI { 
            get {
                return curNode.ns;
            }
        }

        // Returns prefix associated with the current node.
        public override string Prefix { 
            get {
                return curNode.prefix;
            }
        }

        // Returns true if the current node can have Value property != string.Empty.
        public override bool HasValue { 
            get {
                return XmlReader.HasValueInternal( curNode.type );
            }
        }

        // Returns the text value of the current node.
        public override string Value { 
            get {
                if ( parsingFunction >= ParsingFunction.PartialTextValue ) {
                    if ( parsingFunction == ParsingFunction.PartialTextValue ) {
                        FinishPartialValue();                   
                        parsingFunction = nextParsingFunction;
                    }
                    else {
                        FinishOtherValueIterator();
                    }
                }
                return curNode.StringValue;
            }
        }

        // Returns the depth of the current node in the XML element stack
        public override int Depth { 
            get {
                return curNode.depth;
            }
        }

        // Returns the base URI of the current node.
        public override string BaseURI { 
            get {
                return reportedBaseUri;
            }
        }

        // Returns true if the current node is an empty element (for example, <MyElement/>).
        public override bool IsEmptyElement { 
            get {
                return curNode.IsEmptyElement;
            }
        }

        // Returns true of the current node is a default attribute declared in DTD.
        public override bool IsDefault { 
            get {
                return curNode.IsDefaultAttribute;
            }
        }

        // Returns the quote character used in the current attribute declaration
        public override char QuoteChar { 
            get {
                return curNode.type == XmlNodeType.Attribute ? curNode.quoteChar : '"';
            }
        }

        // Returns the current xml:space scope.
        public override XmlSpace XmlSpace { 
            get {
                return xmlContext.xmlSpace;
            }
        }

        // Returns the current xml:lang scope.</para>
        public override string XmlLang { 
            get {
                return xmlContext.xmlLang;
            }
        }

        // Returns the current read state of the reader
        public override ReadState ReadState { 
            get {
                return readState;
            }
        }
        
        // Returns true if the reader reached end of the input data
        public override bool EOF { 
            get {
                return parsingFunction == ParsingFunction.Eof; 
            }
        }
        
        // Returns the XmlNameTable associated with this XmlReader
        public override XmlNameTable NameTable { 
            get {
                return nameTable; 
            }
        }

        // Returns true if the XmlReader knows how to resolve general entities
        public override bool CanResolveEntity  {
            get  {
                return true;
            }
        }

        // Returns the number of attributes on the current node.
        public override int AttributeCount { 
            get {
                return attrCount;
            }
        }

        // Returns value of an attribute with the specified Name
        public override string GetAttribute( string name ) {
            int i;
            if ( name.IndexOf( ':' ) == -1 ) {
                i = GetIndexOfAttributeWithoutPrefix( name );
            }
            else {
                i = GetIndexOfAttributeWithPrefix( name );
            }
            return ( i >= 0 ) ? nodes[i].StringValue : null;
        }

        // Returns value of an attribute with the specified LocalName and NamespaceURI
        public override string GetAttribute( string localName, string namespaceURI ) {
            namespaceURI = ( namespaceURI == null ) ? string.Empty : nameTable.Get( namespaceURI );
            localName = nameTable.Get( localName );
            for ( int i = index + 1; i < index + attrCount + 1; i++ ) {
                if ( Ref.Equal( nodes[i].localName, localName ) && Ref.Equal( nodes[i].ns, namespaceURI ) ) {
                    return nodes[i].StringValue;
                }
            }
            return null;
        }

        // Returns value of an attribute at the specified index (position)
        public override string GetAttribute( int i ) {
            if ( i < 0 || i >= attrCount ) {
                throw new ArgumentOutOfRangeException("i");
            }
            return nodes[index + i + 1].StringValue;
        }

        // Moves to an attribute with the specified Name
        public override bool MoveToAttribute( string name ) {
            int i;
            if ( name.IndexOf( ':' ) == -1 ) {
                i = GetIndexOfAttributeWithoutPrefix( name );
            }
            else {
                i = GetIndexOfAttributeWithPrefix( name );
            }

            if ( i >= 0 ) {
                if ( InAttributeValueIterator ) {
                    FinishAttributeValueIterator();
                }
                curAttrIndex = i - index - 1;
                curNode = nodes[i];
                return true;
            }
            else {
                return false;
            }
        }

        // Moves to an attribute with the specified LocalName and NamespceURI
        public override bool MoveToAttribute( string localName, string namespaceURI ) {
            namespaceURI = ( namespaceURI == null ) ? string.Empty : nameTable.Get( namespaceURI );
            localName = nameTable.Get( localName );
            for ( int i = index + 1; i < index + attrCount + 1; i++ ) {
                if ( Ref.Equal( nodes[i].localName, localName ) &&
                     Ref.Equal( nodes[i].ns, namespaceURI ) ) {
                    curAttrIndex = i - index - 1;
                    curNode = nodes[i];

                    if ( InAttributeValueIterator ) {
                        FinishAttributeValueIterator();
                    }
                    return true;
                }
            }
            return false;
        }

        // Moves to an attribute at the specified index (position)
        public override void MoveToAttribute( int i ) {
            if ( i < 0 || i >= attrCount ) {
                throw new ArgumentOutOfRangeException( "i" );
            }

            if ( InAttributeValueIterator ) {
                FinishAttributeValueIterator();
            }
            curAttrIndex = i;
            curNode = nodes[index + 1 + curAttrIndex];
        }

        // Moves to the first attribute of the current node
        public override bool MoveToFirstAttribute() {
            if ( attrCount == 0 ) {
                return false;
            }

            if ( InAttributeValueIterator ) {
                FinishAttributeValueIterator();
            }

            curAttrIndex = 0;
            curNode = nodes[index + 1];

            return true;
        }

        // Moves to the next attribute of the current node
        public override bool MoveToNextAttribute() {
            if ( curAttrIndex + 1 < attrCount ) {
                if ( InAttributeValueIterator ) {
                    FinishAttributeValueIterator();
                }
                curNode = nodes[ index + 1 + ++curAttrIndex  ];
                return true;
            }
            return false;
        }

        // If on attribute, moves to the element that contains the attribute node
        public override bool MoveToElement() {
            if ( InAttributeValueIterator ) {
                FinishAttributeValueIterator();
            }
            else if ( curNode.type != XmlNodeType.Attribute ) {
                return false;
            }
            curAttrIndex = -1;
            curNode = nodes[index];

            return true;
        }

        // Reads next node from the input data
        public override bool Read() {
            for (;;) {
                switch ( parsingFunction ) {
                    case ParsingFunction.ElementContent:
                        return ParseElementContent();
                    case ParsingFunction.DocumentContent:
                        return ParseDocumentContent();
                    case ParsingFunction.OpenUrl:
                        OpenUrl();
                        Debug.Assert( nextParsingFunction == ParsingFunction.DocumentContent );
                        goto case ParsingFunction.SwitchToInteractiveXmlDecl;
                    case ParsingFunction.SwitchToInteractive:
                        Debug.Assert( !ps.appendMode );
                        readState = ReadState.Interactive;
                        parsingFunction = nextParsingFunction;
                        continue;
                    case ParsingFunction.SwitchToInteractiveXmlDecl:
                        readState = ReadState.Interactive;
                        parsingFunction = nextParsingFunction;
                        if ( ParseXmlDeclaration( false ) ) {
                            reportedEncoding = ps.encoding;
                            return true;
                        }
                        reportedEncoding = ps.encoding;
                        continue;
                    case ParsingFunction.ResetAttributesRootLevel:
                        ResetAttributes();
                        curNode = nodes[index];
                        parsingFunction = ( index == 0 ) ? ParsingFunction.DocumentContent : ParsingFunction.ElementContent;
                        continue;
                    case ParsingFunction.MoveToElementContent:
                        ResetAttributes();
                        index++;
                        curNode = AddNode( index, index );
                        parsingFunction = ParsingFunction.ElementContent;
                        continue;
                    case ParsingFunction.PopElementContext:
                        PopElementContext();
                        parsingFunction = nextParsingFunction;
                        Debug.Assert( parsingFunction == ParsingFunction.ElementContent ||
                                      parsingFunction == ParsingFunction.DocumentContent );
                        continue;
                    case ParsingFunction.PopEmptyElementContext:
                        curNode = nodes[index];
                        Debug.Assert( curNode.type == XmlNodeType.Element );
                        curNode.IsEmptyElement = false;
                        ResetAttributes();
                        PopElementContext();
                        parsingFunction = nextParsingFunction;
                        continue;
                    case ParsingFunction.EntityReference:
                        parsingFunction = nextParsingFunction;
                        ParseEntityReference();
                        return true;
                    case ParsingFunction.ReportEndEntity:
                        SetupEndEntityNodeInContent();
                        parsingFunction = nextParsingFunction;
                        return true;
                    case ParsingFunction.AfterResolveEntityInContent:
                        curNode = AddNode( index, index );
                        reportedEncoding = ps.encoding;
                        reportedBaseUri = ps.baseUriStr;
                        parsingFunction = nextParsingFunction;
                        continue;
                    case ParsingFunction.AfterResolveEmptyEntityInContent:
                        curNode = AddNode( index, index );
                        curNode.SetValueNode( XmlNodeType.Text, string.Empty );
                        curNode.SetLineInfo( ps.lineNo, ps.LinePos );
                        reportedEncoding = ps.encoding;
                        reportedBaseUri = ps.baseUriStr;
                        parsingFunction = nextParsingFunction;
                        return true;
                    case ParsingFunction.InReadAttributeValue:
                        FinishAttributeValueIterator();
                        curNode = nodes[index];
                        continue;
                    case ParsingFunction.InIncrementalRead:
                        FinishIncrementalRead();
                        return true;
                    case ParsingFunction.FragmentAttribute:
                        return ParseFragmentAttribute();
                    case ParsingFunction.XmlDeclarationFragment:
                        ParseXmlDeclarationFragment();
                        parsingFunction = ParsingFunction.GoToEof;
                        return true;
                    case ParsingFunction.GoToEof:
                        OnEof();
                        return false;
                    case ParsingFunction.Error:
                    case ParsingFunction.Eof:
                    case ParsingFunction.ReaderClosed:
                        return false;
                    case ParsingFunction.NoData:
                        ThrowWithoutLineInfo( Res.Xml_MissingRoot );
                        return false;
                    case ParsingFunction.PartialTextValue:
                        SkipPartialTextValue();
                        continue;
                    case ParsingFunction.InReadValueChunk:
                        FinishReadValueChunk();
                        continue;
                    case ParsingFunction.InReadContentAsBinary:
                        FinishReadContentAsBinary();
                        continue;
                    case ParsingFunction.InReadElementContentAsBinary:
                        FinishReadElementContentAsBinary();
                        continue;
                    default:
                        Debug.Assert( false );
                        break;
                }
            }
        }

        // Closes the input stream ot TextReader, changes the ReadState to Closed and sets all properties to zero/string.Empty
        public override void Close() {
            Close( closeInput );
        }

        // Skips the current node. If on element, skips to the end tag of the element.
        public override void Skip() {
            if ( readState != ReadState.Interactive )
                return;

            if ( InAttributeValueIterator ) {
                FinishAttributeValueIterator();
                curNode = nodes[index];
            }
            else {
                switch ( parsingFunction ) {
                    case ParsingFunction.InReadAttributeValue:
                        Debug.Assert( false );
                        break;
                    case ParsingFunction.InIncrementalRead:
                        FinishIncrementalRead();
                        break;
                    case ParsingFunction.PartialTextValue:
                        SkipPartialTextValue();
                        break;
                    case ParsingFunction.InReadValueChunk:
                        FinishReadValueChunk();
                        break;
                    case ParsingFunction.InReadContentAsBinary:
                        FinishReadContentAsBinary();
                        break;
                    case ParsingFunction.InReadElementContentAsBinary:
                        FinishReadElementContentAsBinary();
                        break;
                }
            }

            switch ( curNode.type ) {
                // skip subtree
                case XmlNodeType.Element:
                    if ( curNode.IsEmptyElement ) {
                        break;
                    }
                    int initialDepth = index;
                    parsingMode = ParsingMode.SkipContent;
                    // skip content
                    while ( outerReader.Read() && index > initialDepth ) ;
                    Debug.Assert( curNode.type == XmlNodeType.EndElement );
                    Debug.Assert( parsingFunction != ParsingFunction.Eof );
                    parsingMode = ParsingMode.Full;
                    break;
                case XmlNodeType.Attribute:
                    outerReader.MoveToElement();
                    goto case XmlNodeType.Element;
            }
            // move to following sibling node
            outerReader.Read();
            return;
        }

        // Returns NamespaceURI associated with the specified prefix in the current namespace scope.
        public override String LookupNamespace( String prefix ) {
            if ( !supportNamespaces ) {
                return null;
            }

            return namespaceManager.LookupNamespace( prefix );
        }

        // Iterates through the current attribute value's text and entity references chunks.
        public override bool ReadAttributeValue() {
            if ( parsingFunction != ParsingFunction.InReadAttributeValue ) {
                if ( curNode.type != XmlNodeType.Attribute ) {
                    return false;
                }
                if ( readState != ReadState.Interactive || curAttrIndex < 0 ) {
                    return false;
                }
                if ( parsingFunction == ParsingFunction.InReadValueChunk ) {
                    FinishReadValueChunk();
                }
                if ( parsingFunction == ParsingFunction.InReadContentAsBinary ) {
                    FinishReadContentAsBinary();
                }

                if ( curNode.nextAttrValueChunk == null || entityHandling == EntityHandling.ExpandEntities ) {
                    NodeData simpleValueNode = AddNode( index + attrCount + 1, curNode.depth + 1 );
                    simpleValueNode.SetValueNode( XmlNodeType.Text, curNode.StringValue );
                    simpleValueNode.lineInfo = curNode.lineInfo2;
                    simpleValueNode.depth = curNode.depth + 1;
                    simpleValueNode.nextAttrValueChunk = null;
                    curNode = simpleValueNode;
                }
                else {
                    curNode = curNode.nextAttrValueChunk;

                    // Place the current node at nodes[index + attrCount + 1]. If the node type
                    // is be EntityReference and user calls ResolveEntity, the associated EndEntity
                    // node will be constructed from the information stored there.
                    
                    // This will initialize the (index + attrCount + 1) place in nodes array
                    AddNode( index + attrCount + 1, index + 2 );
                    nodes[index + attrCount + 1] = curNode;

                    fullAttrCleanup = true;
                }
                nextParsingFunction = parsingFunction;
                parsingFunction = ParsingFunction.InReadAttributeValue;
                attributeValueBaseEntityId = ps.entityId;
                return true;
            }
            else {
                if ( ps.entityId == attributeValueBaseEntityId ) {
                    if ( curNode.nextAttrValueChunk != null ) {
                        curNode = curNode.nextAttrValueChunk;
                        nodes[index + attrCount + 1] = curNode;  // if curNode == EntityReference node, it will be picked from here by SetupEndEntityNodeInAttribute
                        return true;
                    }
                    return false;
                }
                else {
                    // expanded entity in attribute value
                    return ParseAttributeValueChunk();
                }
            }
        }

        // Resolves the current entity reference node
        public override void ResolveEntity() {
            if ( curNode.type != XmlNodeType.EntityReference ) {
                throw new InvalidOperationException( Res.GetString( Res.Xml_InvalidOperation ) );
            }

            Debug.Assert( parsingMode == ParsingMode.Full );

            // entity in attribute value
            if ( parsingFunction == ParsingFunction.InReadAttributeValue || 
                 parsingFunction == ParsingFunction.FragmentAttribute ) {
                switch ( HandleGeneralEntityReference( curNode.localName, true, true, curNode.LinePos ) ) {
                    case EntityType.ExpandedInAttribute:
                    case EntityType.Expanded:
                        if ( ps.charsUsed - ps.charPos == 0 ) {  // entity value == ""
                            emptyEntityInAttributeResolved = true;
                        }
                        break;
                    case EntityType.FakeExpanded:
                        emptyEntityInAttributeResolved = true;
                        break;
                    default:
                        Debug.Assert( false );
                        throw new XmlException( Res.Xml_InternalError );
                }
            }
           // entity in element content
            else {
                switch ( HandleGeneralEntityReference( curNode.localName, false, true, curNode.LinePos ) ) {
                    case EntityType.ExpandedInAttribute:
                    case EntityType.Expanded:
                        nextParsingFunction = parsingFunction;
                        if ( ps.charsUsed - ps.charPos == 0 && !ps.entity.IsExternal ) {  // empty internal entity value
                            parsingFunction = ParsingFunction.AfterResolveEmptyEntityInContent;
                        }
                        else {
                            parsingFunction = ParsingFunction.AfterResolveEntityInContent;
                        }
                        break;
                    case EntityType.FakeExpanded:
                        nextParsingFunction = parsingFunction;
                        parsingFunction = ParsingFunction.AfterResolveEmptyEntityInContent;
                        break;
                    default:
                        Debug.Assert( false );
                        throw new XmlException( Res.Xml_InternalError );
                }
            }
            ps.entityResolvedManually = true;
            index++;
        }

        internal XmlReader OuterReader {
            get {
                return outerReader;
            }
            set {
                Debug.Assert( value is XmlTextReader );
                outerReader = value;
            }
        }
        internal void MoveOffEntityReference() {
            if ( outerReader.NodeType == XmlNodeType.EntityReference && 
                 parsingFunction == ParsingFunction.AfterResolveEntityInContent ) {
                if ( !outerReader.Read() ) {
                    throw new InvalidOperationException( Res.GetString( Res.Xml_InvalidOperation ) );
                }
            }
        }

        public override string ReadString() {
            Debug.Assert( outerReader is XmlTextReaderImpl );
            MoveOffEntityReference();
            return base.ReadString();
        }

        public override bool CanReadBinaryContent {
            get { 
                return true;
            }
        }

        // Reads and concatenates content nodes, base64-decodes the results and copies the decoded bytes into the provided buffer
        public override int ReadContentAsBase64( byte[] buffer, int index, int count ) {
            // check arguments
            if ( buffer == null ) {
                throw new ArgumentNullException( "buffer" );
            }
            if ( count < 0 ) {
                throw new ArgumentOutOfRangeException( "count" );
            }
            if ( index < 0 ) {
                throw new ArgumentOutOfRangeException( "index" );
            }
            if ( buffer.Length - index < count ) {
                throw new ArgumentOutOfRangeException( "count" );
            }

            // if not the first call to ReadContentAsBase64 
            if ( parsingFunction == ParsingFunction.InReadContentAsBinary ) {
                // and if we have a correct decoder
                if ( incReadDecoder == base64Decoder ) {
                    // read more binary data
                    return ReadContentAsBinary( buffer, index, count );
                }
            }
            // first call of ReadContentAsBase64 -> initialize (move to first text child (for elements) and initialize incremental read state)
            else {
                if ( readState != ReadState.Interactive ) {
                    return 0;
                }
                if ( parsingFunction == ParsingFunction.InReadElementContentAsBinary ) {
                    throw new InvalidOperationException( Res.GetString( Res.Xml_MixingBinaryContentMethods ) );
                }
                if ( !XmlReader.CanReadContentAs( curNode.type ) ) {
                    throw CreateReadContentAsException( "ReadContentAsBase64" );
                }
                if ( !InitReadContentAsBinary() ) {
                    return 0;
                }
            }
    
            // setup base64 decoder
            InitBase64Decoder();

            // read binary data
            return ReadContentAsBinary( buffer, index, count );
        }

        
        // Reads and concatenates content nodes, binhex-decodes the results and copies the decoded bytes into the provided buffer
        public override int ReadContentAsBinHex( byte[] buffer, int index, int count ) {
            // check arguments
            if ( buffer == null ) {
                throw new ArgumentNullException( "buffer" );
            }
            if ( count < 0 ) {
                throw new ArgumentOutOfRangeException( "count" );
            }
            if ( index < 0 ) {
                throw new ArgumentOutOfRangeException( "index" );
            }
            if ( buffer.Length - index < count ) {
                throw new ArgumentOutOfRangeException( "count" );
            }

            // if not the first call to ReadContentAsBinHex 
            if ( parsingFunction == ParsingFunction.InReadContentAsBinary ) {
                // and if we have a correct decoder
                if ( incReadDecoder == binHexDecoder ) {
                    // read more binary data
                    return ReadContentAsBinary( buffer, index, count );
                }
            }
            // first call of ReadContentAsBinHex -> initialize (move to first text child (for elements) and initialize incremental read state)
            else {
                if ( readState != ReadState.Interactive ) {
                    return 0;
                }
                if ( parsingFunction == ParsingFunction.InReadElementContentAsBinary ) {
                    throw new InvalidOperationException( Res.GetString( Res.Xml_MixingBinaryContentMethods ) );
                }
                if ( !XmlReader.CanReadContentAs( curNode.type ) ) {
                    throw CreateReadContentAsException( "ReadContentAsBinHex" );
                }
                if ( !InitReadContentAsBinary() ) {
                    return 0;
                }
            }
    
            // setup binhex decoder (when in first ReadContentAsBinHex call or when mixed with ReadContentAsBase64)
            InitBinHexDecoder();

            // read binary data
            return ReadContentAsBinary( buffer, index, count );
        }

        // Reads and concatenates content of an element, base64-decodes the results and copies the decoded bytes into the provided buffer
        public override int ReadElementContentAsBase64( byte[] buffer, int index, int count ) {
            // check arguments
            if ( buffer == null ) {
                throw new ArgumentNullException( "buffer" );
            }
            if ( count < 0 ) {
                throw new ArgumentOutOfRangeException( "count" );
            }
            if ( index < 0 ) {
                throw new ArgumentOutOfRangeException( "index" );
            }
            if ( buffer.Length - index < count ) {
                throw new ArgumentOutOfRangeException( "count" );
            }

            // if not the first call to ReadContentAsBase64 
            if ( parsingFunction == ParsingFunction.InReadElementContentAsBinary ) {
                // and if we have a correct decoder
                if ( incReadDecoder == base64Decoder ) {
                    // read more binary data
                    return ReadElementContentAsBinary( buffer, index, count );
                }
            }
            // first call of ReadElementContentAsBase64 -> initialize 
            else {
                if ( readState != ReadState.Interactive ) {
                    return 0;
                }
                if ( parsingFunction == ParsingFunction.InReadContentAsBinary ) {
                    throw new InvalidOperationException( Res.GetString( Res.Xml_MixingBinaryContentMethods ) );
                }
                if ( curNode.type != XmlNodeType.Element ) {
                    throw CreateReadElementContentAsException( "ReadElementContentAsBinHex" );
                }
                if ( !InitReadElementContentAsBinary() ) {
                    return 0;
                }
            }
    
            // setup base64 decoder
            InitBase64Decoder();

            // read binary data
            return ReadElementContentAsBinary( buffer, index, count );
        }

        
        // Reads and concatenates content of an element, binhex-decodes the results and copies the decoded bytes into the provided buffer
        public override int ReadElementContentAsBinHex( byte[] buffer, int index, int count ) {
            // check arguments
            if ( buffer == null ) {
                throw new ArgumentNullException( "buffer" );
            }
            if ( count < 0 ) {
                throw new ArgumentOutOfRangeException( "count" );
            }
            if ( index < 0 ) {
                throw new ArgumentOutOfRangeException( "index" );
            }
            if ( buffer.Length - index < count ) {
                throw new ArgumentOutOfRangeException( "count" );
            }

            // if not the first call to ReadContentAsBinHex 
            if ( parsingFunction == ParsingFunction.InReadElementContentAsBinary ) {
                // and if we have a correct decoder
                if ( incReadDecoder == binHexDecoder ) {
                    // read more binary data
                    return ReadElementContentAsBinary( buffer, index, count );
                }
            }
            // first call of ReadContentAsBinHex -> initialize
            else {
                if ( readState != ReadState.Interactive ) {
                    return 0;
                }
                if ( parsingFunction == ParsingFunction.InReadContentAsBinary ) {
                    throw new InvalidOperationException( Res.GetString( Res.Xml_MixingBinaryContentMethods ) );
                }
                if ( curNode.type != XmlNodeType.Element ) {
                    throw CreateReadElementContentAsException( "ReadElementContentAsBinHex" );
                }
                if ( !InitReadElementContentAsBinary() ) {
                    return 0;
                }
            }
    
            // setup binhex decoder (when in first ReadContentAsBinHex call or when mixed with ReadContentAsBase64)
            InitBinHexDecoder();

            // read binary data
            return ReadElementContentAsBinary( buffer, index, count );
        }

        // Returns true if ReadValue is supported
        public override bool CanReadValueChunk {
            get {
                return true;
            }
        }

        // Iterates over Value property and copies it into the provided buffer
        public override int ReadValueChunk( char[] buffer, int index, int count ) {
            // throw on elements
            if ( !XmlReader.HasValueInternal( curNode.type ) ) {
                throw new InvalidOperationException( Res.GetString( Res.Xml_InvalidReadValueChunk, curNode.type ) ) ;
            }
            // check arguments
            if ( buffer == null ) {
                throw new ArgumentNullException( "buffer" );
            }
            if ( count < 0 ) {
                throw new ArgumentOutOfRangeException( "count" );
            }
            if ( index < 0 ) {
                throw new ArgumentOutOfRangeException( "index" );
            }
            if ( buffer.Length - index < count ) {
                throw new ArgumentOutOfRangeException( "count" );
            }

            // first call of ReadValueChunk -> initialize incremental read state
            if ( parsingFunction != ParsingFunction.InReadValueChunk ) {
                if ( readState != ReadState.Interactive ) {
                    return 0;
                }
                if ( parsingFunction == ParsingFunction.PartialTextValue ) {
                    incReadState = IncrementalReadState.ReadValueChunk_OnPartialValue;
                }
                else {
                    incReadState = IncrementalReadState.ReadValueChunk_OnCachedValue;
                    nextNextParsingFunction = nextParsingFunction;
                    nextParsingFunction = parsingFunction;
                }
                parsingFunction = ParsingFunction.InReadValueChunk;
                readValueOffset = 0;
            }

            if ( count == 0 ) {
                return 0;
            }

            // read what is already cached in curNode
            int readCount = 0;
            int read = curNode.CopyTo( readValueOffset, buffer, index + readCount, count - readCount );
            readCount += read;
            readValueOffset += read;

            if ( readCount == count ) {
                // take care of suppogate pairs spanning between buffers
                char ch = buffer[index + count - 1];
                if ( ch >= SurHighStart && ch <= SurHighEnd ) {
                    readCount--;
                    readValueOffset--;
                    if ( readCount == 0 ) {
                        Throw( Res.Xml_NotEnoughSpaceForSurrogatePair );
                    }
                }
                return readCount;
            }

            // if on partial value, read the rest of it
            if ( incReadState == IncrementalReadState.ReadValueChunk_OnPartialValue ) {
                curNode.SetValue( string.Empty );

                // read next chunk of text
                bool endOfValue = false;
                int startPos = 0;
                int endPos = 0;
                while ( readCount < count && !endOfValue ) {
                    int orChars = 0;
                    endOfValue = ParseText( out startPos, out endPos, ref orChars );

                    int copyCount = count - readCount;
                    if ( copyCount > endPos - startPos ) {
                        copyCount = endPos - startPos;
                    }
                    Buffer.BlockCopy( ps.chars, startPos * 2, buffer, ( index + readCount ) * 2, copyCount * 2 ); 

                    readCount += copyCount;
                    startPos += copyCount;
                }

				incReadState = endOfValue ? IncrementalReadState.ReadValueChunk_OnCachedValue : IncrementalReadState.ReadValueChunk_OnPartialValue;

                if ( readCount == count ) {
                    char ch = buffer[index + count - 1];
                    if ( ch >= SurHighStart && ch <= SurHighEnd ) {
                        readCount--;
                        startPos--;
                        if ( readCount == 0 ) {
                            Throw( Res.Xml_NotEnoughSpaceForSurrogatePair );
                        }
                    }
                }

                readValueOffset = 0;
                curNode.SetValue( ps.chars, startPos, endPos - startPos );
            }
            return readCount;
        }

//
// IXmlLineInfo members
//
        public bool HasLineInfo() {
            return true;
        }

        // Returns the line number of the current node
        public int LineNumber {
            get {
                return curNode.LineNo;
            }
        }

        // Returns the line position of the current node
        public int LinePosition { 
            get {
                return curNode.LinePos;
            }
        }

//
// IXmlNamespaceResolver members
//
        IDictionary<string,string> IXmlNamespaceResolver.GetNamespacesInScope( XmlNamespaceScope scope ) {
            return this.GetNamespacesInScope( scope );
        }

        string IXmlNamespaceResolver.LookupNamespace(string prefix) {
            return this.LookupNamespace( prefix );
        }

        string IXmlNamespaceResolver.LookupPrefix( string namespaceName ) {
            return this.LookupPrefix( namespaceName );
        }

    // Internal IXmlNamespaceResolver methods
        internal IDictionary<string,string> GetNamespacesInScope( XmlNamespaceScope scope ) {
            return namespaceManager.GetNamespacesInScope( scope );
        }

        // NOTE: there already is virtual method for "string LookupNamespace(string prefix)" 

        internal string LookupPrefix( string namespaceName ) {
            return namespaceManager.LookupPrefix( namespaceName );
        }

//
// XmlTextReader members
//
        // Disables or enables support of W3C XML 1.0 Namespaces
        internal bool Namespaces {
            get {
                Debug.Assert( v1Compat, "XmlTextReaderImpl.Namespaces property cannot be accessed on reader created via XmlReader.Create." );
                return supportNamespaces;
            }
            set {
                Debug.Assert( v1Compat, "XmlTextReaderImpl.Namespaces property cannot be changed on reader created via XmlReader.Create." );
                if ( readState != ReadState.Initial ) {
                    throw new InvalidOperationException( Res.GetString( Res.Xml_InvalidOperation ) );
                }
                supportNamespaces = value;
                if ( value ) {
                    if ( namespaceManager is NoNamespaceManager ) {
                        if ( fragment && fragmentParserContext != null && fragmentParserContext.NamespaceManager != null ) {
                            namespaceManager = fragmentParserContext.NamespaceManager;
                        }
                        else {
                            namespaceManager = new XmlNamespaceManager( nameTable );
                        }
                    }
                    xmlContext.defaultNamespace = namespaceManager.LookupNamespace( string.Empty );
                }
                else {
                    if ( !( namespaceManager is NoNamespaceManager ) ) {
                        namespaceManager = new NoNamespaceManager();
                    }
                    xmlContext.defaultNamespace = string.Empty;
                }
            }
        }

        // Enables or disables XML 1.0 normalization (incl. end-of-line normalization and normalization of attributes)
        internal bool Normalization {
            get {
                Debug.Assert( v1Compat, "XmlTextReaderImpl.Normalization property cannot be accessed on reader created via XmlReader.Create." );
                return normalize;
            }
            set {
                Debug.Assert( v1Compat, "XmlTextReaderImpl.Normalization property cannot be changed on reader created via XmlReader.Create." );
                if ( readState == ReadState.Closed ) {
                    throw new InvalidOperationException( Res.GetString( Res.Xml_InvalidOperation ) );
                }
                normalize = value;
                
                if ( ps.entity == null || ps.entity.IsExternal ) {
                    ps.eolNormalized = !value;
                }
            }
        }

        // Returns the Encoding of the XML document
        internal Encoding Encoding {
            get {
                return ( readState == ReadState.Interactive ) ? reportedEncoding : null;
            }
        }

        // Spefifies whitespace handling of the XML document, i.e. whether return all namespaces, only significant ones or none
        internal WhitespaceHandling WhitespaceHandling {
            get {
                Debug.Assert( v1Compat, "XmlTextReaderImpl.WhitespaceHandling property cannot be accessed on reader created via XmlReader.Create." );
                return whitespaceHandling;
            }
            set {
                Debug.Assert( v1Compat, "XmlTextReaderImpl.WhitespaceHandling property cannot be changed on reader created via XmlReader.Create." );
                if ( readState == ReadState.Closed ) {
                    throw new InvalidOperationException( Res.GetString( Res.Xml_InvalidOperation ) );
                }
                
                if ( (uint)value > (uint)WhitespaceHandling.None ) {
                    throw new XmlException( Res.Xml_WhitespaceHandling, string.Empty );
                }
                whitespaceHandling = value;
            }
        }

        // Specifies whether DTD is prohibited in the XML document. When true, an exceptio will be thrown when a DOCTYPE markup is encountered.
        internal bool ProhibitDtd {
            get {
                Debug.Assert( v1Compat, "XmlTextReaderImpl.ProhibitDtd property cannot be accessed on reader created via XmlReader.Create." );
                return prohibitDtd;
            }
            set {
                Debug.Assert( v1Compat, "XmlTextReaderImpl.ProhibitDtd property cannot be changed on reader created via XmlReader.Create." );
                prohibitDtd = value;
            }
        }

        // Spefifies whether general entities should be automatically expanded or not
        internal EntityHandling EntityHandling {
            get { 
                Debug.Assert( v1Compat, "XmlTextReaderImpl.EntityHandling property cannot be accessed on reader created via XmlReader.Create." );
                return entityHandling;
            }
            set {
                Debug.Assert( v1Compat, "XmlTextReaderImpl.EntityHandling property cannot be changed on reader created via XmlReader.Create." );
                if ( value != EntityHandling.ExpandEntities && value != EntityHandling.ExpandCharEntities ) {
                    throw new XmlException( Res.Xml_EntityHandling, string.Empty );
                }
                entityHandling = value;
            }
        }

        // Specifies XmlResolver used for opening the XML document and other external references
        internal XmlResolver XmlResolver {
            set {
                Debug.Assert( v1Compat, "XmlTextReaderImpl.XmlResolver property cannot be changed on reader created via XmlReader.Create." );
                xmlResolver = value;

                // invalidate all baseUris on the stack
                ps.baseUri = null;
                for ( int i = 0; i <= parsingStatesStackTop; i++ ) {
                    parsingStatesStack[i].baseUri = null;
                }
            }
        }

        // Reset the state of the reader so the reader is ready to parse another XML document from the same stream.
        internal void ResetState() {
            Debug.Assert( v1Compat, "XmlTextReaderImpl.ResetState cannot be called on reader created via XmlReader.Create." );

            if ( fragment ) {
                Throw( new InvalidOperationException( Res.GetString( Res.Xml_InvalidResetStateCall ) ) );
            }

            if ( readState == ReadState.Initial ) {
                return;
            }

            // Clear
            ResetAttributes();
            while ( namespaceManager.PopScope() );

            while ( InEntity ) {
                HandleEntityEnd( true );
            }

            // Init
            readState = ReadState.Initial;
            parsingFunction = ParsingFunction.SwitchToInteractiveXmlDecl;
            nextParsingFunction = ParsingFunction.DocumentContent;

            curNode = nodes[0];
            curNode.Clear( XmlNodeType.None );
            curNode.SetLineInfo( 0, 0 );
            index = 0;
            rootElementParsed = false;

            afterResetState = true;
        }

        // returns the remaining unparsed data as TextReader
        internal TextReader GetRemainder() {
            Debug.Assert( v1Compat, "XmlTextReaderImpl.GetRemainder cannot be called on reader created via XmlReader.Create." );

            Debug.Assert( stringBuilder.Length == 0 );
            switch ( parsingFunction ) { 
                case ParsingFunction.Eof:
                case ParsingFunction.ReaderClosed:
                    return new StringReader( string.Empty );
                case ParsingFunction.OpenUrl:
                    OpenUrl();
                    break;
                case ParsingFunction.InIncrementalRead:
                    if ( !InEntity ) {
                        stringBuilder.Append( ps.chars, incReadLeftStartPos, incReadLeftEndPos -  incReadLeftStartPos );
                    }
                    break;
            }

            while ( InEntity ) {
                HandleEntityEnd( true );
            }

            ps.appendMode = false;
            do {
                stringBuilder.Append( ps.chars, ps.charPos, ps.charsUsed - ps.charPos );
                ps.charPos = ps.charsUsed;
            } while ( ReadData() != 0 );

            OnEof();

            string remainer = stringBuilder.ToString();
            stringBuilder.Length = 0;
            return new StringReader( remainer );
        }

        // Reads the contents of an element including markup into a character buffer. Wellformedness checks are limited.
        // This method is designed to read large streams of embedded text by calling it successively.
        internal int ReadChars( char[] buffer, int index, int count ) {
            Debug.Assert( v1Compat, "XmlTextReaderImpl.ReadChars cannot be called on reader created via XmlReader.Create." );
            Debug.Assert( outerReader is XmlTextReader );

            if ( parsingFunction == ParsingFunction.InIncrementalRead ) {
                if ( incReadDecoder != readCharsDecoder ) { // mixing ReadChars with ReadBase64 or ReadBinHex
                    if ( readCharsDecoder == null ) {
                        readCharsDecoder = new IncrementalReadCharsDecoder();
                    }
                    readCharsDecoder.Reset();
                    incReadDecoder = readCharsDecoder;
                }
                return IncrementalRead( buffer, index, count );
            }
            else {
                if ( curNode.type != XmlNodeType.Element) {
                    return 0;
                }
                if ( curNode.IsEmptyElement ) {
                    outerReader.Read();
                    return 0;
                }

                if ( readCharsDecoder == null ) {
                    readCharsDecoder = new IncrementalReadCharsDecoder();
                }

                InitIncrementalRead( readCharsDecoder );
                return IncrementalRead( buffer, index, count );
            }
        }

        // Reads the contents of an element including markup and base64-decodes it into a byte buffer. Wellformedness checks are limited.
        // This method is designed to read base64-encoded large streams of bytes by calling it successively.
        internal int ReadBase64( byte[] array, int offset, int len ) {
            Debug.Assert( v1Compat, "XmlTextReaderImpl.ReadBase64 cannot be called on reader created via XmlReader.Create." );
            Debug.Assert( outerReader is XmlTextReader );

            if ( parsingFunction == ParsingFunction.InIncrementalRead ) {
                if ( incReadDecoder != base64Decoder ) { // mixing ReadBase64 with ReadChars or ReadBinHex
                    InitBase64Decoder();
                }
                return IncrementalRead( array, offset, len );
            }
            else {
                if ( curNode.type  != XmlNodeType.Element) {
                    return 0;
                }
                if ( curNode.IsEmptyElement ) {
                    outerReader.Read();
                    return 0;
                }

                if ( base64Decoder == null ) {
                    base64Decoder = new Base64Decoder();
                }

                InitIncrementalRead( base64Decoder );
                return IncrementalRead( array, offset, len );
            }        
        }

        // Reads the contents of an element including markup and binhex-decodes it into a byte buffer. Wellformedness checks are limited.
        // This method is designed to read binhex-encoded large streams of bytes by calling it successively.
        internal int ReadBinHex( byte[] array, int offset, int len ) {
            Debug.Assert( v1Compat, "XmlTextReaderImpl.ReadBinHex cannot be called on reader created via XmlReader.Create." );
            Debug.Assert( outerReader is XmlTextReader );

            if ( parsingFunction == ParsingFunction.InIncrementalRead ) {
                if ( incReadDecoder != binHexDecoder ) { // mixing ReadBinHex with ReadChars or ReadBase64
                    InitBinHexDecoder();
                }
                return IncrementalRead( array, offset, len );
            }
            else {
                if ( curNode.type  != XmlNodeType.Element) {
                    return 0;
                }
                if ( curNode.IsEmptyElement ) {
                    outerReader.Read();
                    return 0;
                }

                if ( binHexDecoder == null ) {
                    binHexDecoder = new BinHexDecoder();
                }

                InitIncrementalRead( binHexDecoder );
                return IncrementalRead( array, offset, len );
            }
        }

//
// Helpers for DtdParserProxy
//
        internal XmlNameTable DtdParserProxy_NameTable {
            get {
                return nameTable;
            }
        }

        internal XmlNamespaceManager DtdParserProxy_NamespaceManager { 
            get { 
                return namespaceManager; 
            }
        }

        internal bool DtdParserProxy_DtdValidation { 
            get {
                return DtdValidation;
            }
        }

        internal bool DtdParserProxy_Normalization { 
            get {
                return normalize;
            }
        }

        internal bool DtdParserProxy_Namespaces { 
            get {
                return supportNamespaces;
            }
        }

        internal bool DtdParserProxy_V1CompatibilityMode { 
            get {
                return v1Compat;
            }
        }
        
        internal Uri DtdParserProxy_BaseUri {
            get {
                if ( ps.baseUriStr.Length > 0 && ps.baseUri == null && xmlResolver != null ) {
                    ps.baseUri = xmlResolver.ResolveUri( null, ps.baseUriStr );
                }
                return ps.baseUri;
            }
        }
        
        internal bool DtdParserProxy_IsEof { 
            get {
                return ps.isEof; 
            }
        }

        internal char[] DtdParserProxy_ParsingBuffer { 
            get {
                return ps.chars; 
            }
        }

        internal int DtdParserProxy_ParsingBufferLength { 
            get {
                return ps.charsUsed;
            }
        }

        internal int DtdParserProxy_CurrentPosition { 
            get {
                return ps.charPos; 
            }
            set {
                Debug.Assert( value >= 0 && value <= ps.charsUsed );
                ps.charPos = value;
            }
        }

        internal int DtdParserProxy_EntityStackLength { 
            get {
                return parsingStatesStackTop + 1;
            }
        }

        internal bool DtdParserProxy_IsEntityEolNormalized { 
            get {
                return ps.eolNormalized;
            } 
        }

        internal ValidationEventHandler DtdParserProxy_EventHandler { 
            get {
                return validationEventHandler;
            }
            set {
                validationEventHandler = value;
            }
        }

        internal void DtdParserProxy_OnNewLine( int pos ) {
            this.OnNewLine( pos );
        }

        internal int DtdParserProxy_LineNo { 
            get {
                return ps.LineNo; 
            }
        }

        internal int DtdParserProxy_LineStartPosition { 
            get {
                return ps.lineStartPos; 
            }
        }

        internal int DtdParserProxy_ReadData() {
            return this.ReadData();
        }

        internal void DtdParserProxy_SendValidationEvent( XmlSeverityType severity, XmlSchemaException exception ) {
            if ( DtdValidation ) {
                this.SendValidationEvent( severity, exception );
            }
        }

        internal int DtdParserProxy_ParseNumericCharRef( BufferBuilder internalSubsetBuilder ) {
            EntityType entType;
            return this.ParseNumericCharRef( true, internalSubsetBuilder, out entType );
        }

        internal int DtdParserProxy_ParseNamedCharRef( bool expand, BufferBuilder internalSubsetBuilder ) {
            return this.ParseNamedCharRef( expand, internalSubsetBuilder );
        }

        internal void DtdParserProxy_ParsePI( BufferBuilder sb ) {
            if ( sb == null ) {
                ParsingMode pm = parsingMode;
                parsingMode = ParsingMode.SkipNode;
                ParsePI( null );
                parsingMode = pm;
            }
            else {
                ParsePI( sb );
            }
        }
        
        internal void DtdParserProxy_ParseComment( BufferBuilder sb ) {
            Debug.Assert( parsingMode == ParsingMode.Full );

            try {
                if ( sb == null ) {
                    ParsingMode savedParsingMode = parsingMode;
                    parsingMode = ParsingMode.SkipNode;
                    ParseCDataOrComment( XmlNodeType.Comment );
                    parsingMode = savedParsingMode;
                }
                else {
                    NodeData originalCurNode = curNode;

                    curNode = AddNode( index + attrCount + 1, index );
                    ParseCDataOrComment( XmlNodeType.Comment );
                    curNode.CopyTo( sb );

                    curNode = originalCurNode;
                }
            }
            catch ( XmlException e ) {
                if ( e.ResString == Res.Xml_UnexpectedEOF && ps.entity != null ) {
                    SendValidationEvent( XmlSeverityType.Error, Res.Sch_ParEntityRefNesting, null, ps.LineNo, ps.LinePos );
                }   
                else {
                    throw;
                }
            }
        }

        internal bool DtdParserProxy_PushEntity( SchemaEntity entity, int entityId ) {
            if ( entity.IsExternal ) {
                if ( xmlResolver == null ) {
                    return false;
                }
                return PushExternalEntity( entity, entityId );
            }
            else {
                PushInternalEntity( entity, entityId );
                return true;
            }
        }

        internal bool DtdParserProxy_PopEntity( out SchemaEntity oldEntity, out int newEntityId ) {
            if ( parsingStatesStackTop == -1 ) {
                oldEntity = null;
                newEntityId = -1;
                return false;
            }
            oldEntity = ps.entity;
            PopEntity();
            newEntityId = ps.entityId;
            return true;
        }

        internal bool DtdParserProxy_PushExternalSubset( string systemId, string publicId ) {
            Debug.Assert( parsingStatesStackTop == -1 );
            Debug.Assert( ( systemId != null && systemId.Length > 0 ) || ( publicId != null && publicId.Length > 0 ) );

            if ( xmlResolver == null ) {
                return false;
            }

            if ( ps.baseUriStr.Length > 0 && ps.baseUri == null ) {
                ps.baseUri = xmlResolver.ResolveUri( null, ps.baseUriStr );
            }

            Uri extSubsetUri;
            Stream stream = null;
            if ( publicId == null || publicId.Length == 0 ) {
                extSubsetUri = xmlResolver.ResolveUri( ps.baseUri, systemId );
                try {
                    stream = OpenStream( extSubsetUri );
                }
                catch ( Exception e ) {
                    if ( v1Compat ) {
                        throw;
                    }
                    Throw( new XmlException( Res.Xml_ErrorOpeningExternalDtd, new string[] { extSubsetUri.ToString(), e.Message }, e, 0, 0 ) );
                }
            }
            else {
                try {
                    extSubsetUri = xmlResolver.ResolveUri( ps.baseUri, publicId );
                    stream = OpenStream( extSubsetUri );
                }
                catch ( Exception ) {
                    extSubsetUri = xmlResolver.ResolveUri( ps.baseUri, systemId );
                    try {
                        stream = OpenStream( extSubsetUri );
                    }
                    catch ( Exception e ) {
                        if ( v1Compat ) {
                            throw;
                        }
                        Throw( new XmlException( Res.Xml_ErrorOpeningExternalDtd, new string[] { extSubsetUri.ToString(), e.Message }, e, 0, 0 ) );
                    }
                }
            }

            if ( stream == null ) {
                ThrowWithoutLineInfo( Res.Xml_CannotResolveExternalSubset, new string[] { ( publicId != null ? publicId : string.Empty), systemId } ); 
            }

            PushParsingState();

            if ( v1Compat ) {
                InitStreamInput( extSubsetUri, stream, null );
            }
            else {
                InitStreamInput( extSubsetUri, stream, null );
            }

            ps.entity = null;
            ps.entityId = 0;

            Debug.Assert( ps.appendMode );
            int initialPos = ps.charPos;
            if ( v1Compat ) {
                EatWhitespaces( null );
            }
            if ( !ParseXmlDeclaration( true ) ) {
                ps.charPos = initialPos;
            }

            return true;        
        }

        internal void DtdParserProxy_PushInternalDtd( string baseUri, string internalDtd ) {
            Debug.Assert( parsingStatesStackTop == -1 );
            
            PushParsingState();

            InitStringInput( baseUri, Encoding.Unicode, internalDtd );

            ps.entity = null;
            ps.entityId = 0;
            ps.eolNormalized = false;
        }

        internal void DtdParserProxy_Throw( Exception e ) {
            this.Throw( e );
        }

        internal void DtdParserProxy_OnSystemId( string systemId, LineInfo keywordLineInfo, LineInfo systemLiteralLineInfo ) {
            NodeData attr = AddAttributeNoChecks( "SYSTEM", index ); 
            attr.SetValue( systemId );
            attr.lineInfo = keywordLineInfo;
            attr.lineInfo2 = systemLiteralLineInfo;
        }

        internal void DtdParserProxy_OnPublicId( string publicId, LineInfo keywordLineInfo, LineInfo publicLiteralLineInfo ) {
            NodeData attr = AddAttributeNoChecks( "PUBLIC", index ); 
            attr.SetValue( publicId );
            attr.lineInfo = keywordLineInfo;
            attr.lineInfo2 = publicLiteralLineInfo;
        }
        
//
// Throw methods: Sets the reader current position to pos, sets the error state and throws exception
//
        void Throw( int pos, string res, string arg ) {
            ps.charPos = pos;
            Throw( res, arg );
        }

        void Throw( int pos, string res, string[] args ) {
            ps.charPos = pos;
            Throw( res, args );
        }

        void Throw( int pos, string res ) {
            ps.charPos = pos;
            Throw( res, string.Empty );
        }

        void Throw( string res ) {
            Throw( res, string.Empty );
        }

        void Throw( string res, int lineNo, int linePos ) {
            Throw( new XmlException( res, string.Empty, lineNo, linePos, ps.baseUriStr ) );
        }

        void Throw( string res, string arg ) {
            Throw( new XmlException( res, arg, ps.LineNo, ps.LinePos, ps.baseUriStr ) );
        }

        void Throw( string res, string arg, int lineNo, int linePos ) {
            Throw( new XmlException( res, arg, lineNo, linePos, ps.baseUriStr ) );
        }

        void Throw( string res, string[] args ) {
            Throw( new XmlException( res, args, ps.LineNo, ps.LinePos, ps.baseUriStr ) );
        }

        void Throw( Exception e ) {
            SetErrorState();
            XmlException xmlEx = e as XmlException;
            if ( xmlEx != null ) {
                curNode.SetLineInfo( xmlEx.LineNumber, xmlEx.LinePosition );
            }
            throw e;
        } 

        void ReThrow( Exception e, int lineNo, int linePos ) {
            Throw( new XmlException( e.Message, (Exception)null, lineNo, linePos, ps.baseUriStr ) );
        }

        void ThrowWithoutLineInfo( string res ) {    
            Throw( new XmlException( res, string.Empty, ps.baseUriStr ) );
        }

        void ThrowWithoutLineInfo( string res, string arg ) {    
            Throw( new XmlException( res, arg, ps.baseUriStr ) );
        }

        void ThrowWithoutLineInfo( string res, string[] args ) {    
            Throw( new XmlException( res, args, ps.baseUriStr ) );
        }

        void ThrowInvalidChar( int pos, char invChar ) {
            // special-case binary XML stream wrapped in a TextReader
            if ( pos == 0 && curNode.type == XmlNodeType.None && ps.textReader != null && ps.charsUsed >= 2 && 
                ( ( ps.chars[0] == 0x1 && ps.chars[1] == 0x4 ) || ( ps.chars[0] == 0xdf || ps.chars[1] == 0xff ) ) ) {
                Throw( pos, Res.Xml_BinaryXmlReadAsText, XmlException.BuildCharExceptionStr( invChar ) );
            }
            else {
                Throw( pos, Res.Xml_InvalidCharacter, XmlException.BuildCharExceptionStr( invChar ) );
            }
        }

        private void SetErrorState() {
            parsingFunction = ParsingFunction.Error;
            readState = ReadState.Error;
        }

        void SendValidationEvent( XmlSeverityType severity, string code, string arg, int lineNo, int linePos ) {
            SendValidationEvent( severity, new XmlSchemaException( code, arg, ps.baseUriStr, lineNo, linePos ) );
        }

        void SendValidationEvent( XmlSeverityType severity, XmlSchemaException exception ) {
            if ( validationEventHandler != null ) {
                validationEventHandler( this, new ValidationEventArgs( exception, severity ) );
            }
        }

//
// Private implementation methods & properties
//
        private bool InAttributeValueIterator {
            get {
                return attrCount > 0 && parsingFunction >= ParsingFunction.InReadAttributeValue;
            }
        }

        private void FinishAttributeValueIterator() {
            Debug.Assert( InAttributeValueIterator );
            if ( parsingFunction == ParsingFunction.InReadValueChunk ) {
                FinishReadValueChunk();
            }
            else if ( parsingFunction == ParsingFunction.InReadContentAsBinary ) {
                FinishReadContentAsBinary();
            }
            if ( parsingFunction == ParsingFunction.InReadAttributeValue ) {
                while ( ps.entityId != attributeValueBaseEntityId ) {
                    HandleEntityEnd( false );
                }
                parsingFunction = nextParsingFunction;
                nextParsingFunction = ( index > 0 ) ? ParsingFunction.ElementContent : ParsingFunction.DocumentContent;
                emptyEntityInAttributeResolved = false;
            }
        }

        private bool DtdValidation {
            get { 
                return validationEventHandler != null;
            }
        }

        private void InitStreamInput( Stream stream, Encoding encoding ) {
            InitStreamInput( null, string.Empty, stream, null, 0, encoding );
        }

        private void InitStreamInput( string baseUriStr, Stream stream, Encoding encoding ) {
            Debug.Assert( baseUriStr != null );
            InitStreamInput( null, baseUriStr, stream, null, 0, encoding );
        }

        private void InitStreamInput( Uri baseUri, Stream stream, Encoding encoding ) {
            Debug.Assert( baseUri != null );
            InitStreamInput( baseUri, baseUri.ToString(), stream, null, 0, encoding );
        }

        private void InitStreamInput( Uri baseUri, string baseUriStr, Stream stream, Encoding encoding ) {
            InitStreamInput( baseUri, baseUriStr, stream, null, 0, encoding );
        }

        private void InitStreamInput( Uri baseUri, string baseUriStr, Stream stream, byte[] bytes, int byteCount, Encoding encoding ) {

            Debug.Assert( ps.charPos == 0 && ps.charsUsed == 0 && ps.textReader == null );
            Debug.Assert( baseUriStr != null );
            Debug.Assert( baseUri == null || ( baseUri.ToString().Equals( baseUriStr ) ) );

            ps.stream = stream;
            ps.baseUri = baseUri;
            ps.baseUriStr = baseUriStr;

            // take over the byte buffer allocated in XmlReader.Create, if available
            int bufferSize;
            if ( bytes != null ) {
                ps.bytes = bytes;
                ps.bytesUsed = byteCount;
                bufferSize = ps.bytes.Length;
            }
            else {
                // allocate the byte buffer 
                bufferSize = XmlReader.CalcBufferSize( stream );
                if ( ps.bytes == null || ps.bytes.Length < bufferSize ) {
                    ps.bytes = new byte[ bufferSize ];
                }
            }

            // allocate char buffer
            if ( ps.chars == null || ps.chars.Length < bufferSize + 1 ) {
                ps.chars = new char[ bufferSize + 1 ];
            }

            // make sure we have at least 4 bytes to detect the encoding (no preamble of System.Text supported encoding is longer than 4 bytes)
            ps.bytePos = 0;
            while ( ps.bytesUsed < 4 && ps.bytes.Length - ps.bytesUsed > 0 ) {
                int read = stream.Read( ps.bytes, ps.bytesUsed, ps.bytes.Length - ps.bytesUsed );
                if ( read == 0 ) {
                    ps.isStreamEof = true;
                    break;
                }
                ps.bytesUsed += read;
            } 

            // detect & setup encoding
            if ( encoding == null ) {
                encoding = DetectEncoding();
            }
            SetupEncoding( encoding );

            // eat preamble 
            byte[] preamble = ps.encoding.GetPreamble();
            int preambleLen = preamble.Length;
            int i;
            for ( i = 0; i < preambleLen && i < ps.bytesUsed; i++ ) {
                if ( ps.bytes[i] != preamble[i] ) {
                    break;
                }
            }
            if ( i == preambleLen ) {
                ps.bytePos = preambleLen; 
            }

            documentStartBytePos = ps.bytePos;

            ps.eolNormalized = !normalize;

            // decode first characters
            ps.appendMode = true;
            ReadData();
        }

        private void InitTextReaderInput( string baseUriStr, TextReader input ) {
            Debug.Assert( ps.charPos == 0 && ps.charsUsed == 0 && ps.stream == null );
            Debug.Assert( baseUriStr != null );

            ps.textReader = input;
            ps.baseUriStr = baseUriStr;
            ps.baseUri = null;
            
            if ( ps.chars == null ) {
                ps.chars = new char[ XmlReader.DefaultBufferSize + 1 ];
            }

            ps.encoding = Encoding.Unicode;
            ps.eolNormalized = !normalize;

            // read first characters
            ps.appendMode = true;
            ReadData();
        }

        private void InitStringInput( string baseUriStr, Encoding originalEncoding, string str ) {
            Debug.Assert( ps.stream == null && ps.textReader == null );
            Debug.Assert( ps.charPos == 0 && ps.charsUsed == 0 );
            Debug.Assert( baseUriStr != null );

            ps.baseUriStr = baseUriStr;
            ps.baseUri = null;

            int len = str.Length;
            ps.chars = new char[ len + 1 ];
            str.CopyTo( 0, ps.chars, 0, str.Length );
            ps.charsUsed = len;
            ps.chars[len] = (char)0;

            ps.encoding = originalEncoding;

            ps.eolNormalized = !normalize;
            ps.isEof = true;
        }

        private void InitFragmentReader( XmlNodeType fragmentType, XmlParserContext parserContext, bool allowXmlDeclFragment ) {
            
            fragmentParserContext = parserContext;

            if ( parserContext != null ) {
                if ( parserContext.NamespaceManager != null ) {
                    namespaceManager = parserContext.NamespaceManager;
                    xmlContext.defaultNamespace = namespaceManager.LookupNamespace( string.Empty );
                }
                else {
                    namespaceManager = new XmlNamespaceManager( nameTable );
                }

                ps.baseUriStr = parserContext.BaseURI;
                ps.baseUri = null;
                xmlContext.xmlLang = parserContext.XmlLang;
                xmlContext.xmlSpace = parserContext.XmlSpace;
            }
            else {
                namespaceManager = new XmlNamespaceManager( nameTable );
                ps.baseUriStr = string.Empty;
                ps.baseUri = null;
            }

            reportedBaseUri = ps.baseUriStr;

            switch ( fragmentType ) {
                case XmlNodeType.Attribute:
                    ps.appendMode = false;
                    parsingFunction = ParsingFunction.SwitchToInteractive;
                    nextParsingFunction = ParsingFunction.FragmentAttribute;
                    break;
                case XmlNodeType.Element:
                    Debug.Assert( parsingFunction == ParsingFunction.SwitchToInteractiveXmlDecl );
                    nextParsingFunction = ParsingFunction.DocumentContent;
                    break;
                case XmlNodeType.Document:
                    Debug.Assert( parsingFunction == ParsingFunction.SwitchToInteractiveXmlDecl );
                    Debug.Assert( nextParsingFunction == ParsingFunction.DocumentContent );
                    break;
                case XmlNodeType.XmlDeclaration:
                    if ( allowXmlDeclFragment ) {
                        ps.appendMode = false;
                        parsingFunction = ParsingFunction.SwitchToInteractive;
                        nextParsingFunction = ParsingFunction.XmlDeclarationFragment;
                        break;
                    }
                    else {
                        goto default;
                    }
                default:
                    Throw( Res.Xml_PartialContentNodeTypeNotSupportedEx, fragmentType.ToString() );
                    return;
            }
            this.fragmentType = fragmentType;
            this.fragment = true;
        }

        private void OpenUrl() {
            Debug.Assert( url != null && url.Length > 0 );
            Debug.Assert( compressedStack != null );

            XmlResolver tmpResolver;
            if ( ps.baseUri != null ) {
                Debug.Assert( xmlResolver != null );
                tmpResolver = xmlResolver;
            }
            else {
                tmpResolver = ( xmlResolver == null ) ? new XmlUrlResolver() : xmlResolver;
                ps.baseUri = tmpResolver.ResolveUri( null, url );
                ps.baseUriStr = ps.baseUri.ToString();
            }

            try {
                CompressedStack.Run( compressedStack, new ContextCallback( OpenUrlDelegate ), tmpResolver );
            }
            catch {
                SetErrorState();
                throw;
            }

            if ( ps.stream == null ) {
                ThrowWithoutLineInfo( Res.Xml_CannotResolveUrl, ps.baseUriStr );
            }
        
            InitStreamInput( ps.baseUri, ps.baseUriStr, ps.stream, null );
            reportedEncoding = ps.encoding;
        }

        void OpenUrlDelegate(object xmlResolver) {
            ps.stream = (Stream) ((XmlResolver)xmlResolver).GetEntity( ps.baseUri, null, typeof( Stream ) );
        }

        // Stream input only: detect encoding from the first 4 bytes of the byte buffer starting at ps.bytes[ps.bytePos]
        private Encoding DetectEncoding() {
            Debug.Assert( ps.bytes != null );
            Debug.Assert( ps.bytePos == 0 );

            if ( ps.bytesUsed < 2 ) {
                return null;
            }
            int first2Bytes = ps.bytes[0] << 8 | ps.bytes[1];
            int next2Bytes = ( ps.bytesUsed >= 4 ) ? ( ps.bytes[2] << 8 | ps.bytes[3] ) : 0;

            switch ( first2Bytes ) {
                case 0x0000:
                    switch ( next2Bytes ) {
                        case 0xFEFF:
                            return Ucs4Encoding.UCS4_Bigendian;
                        case 0x003C:
                            return Ucs4Encoding.UCS4_Bigendian;
                        case 0xFFFE:
                            return Ucs4Encoding.UCS4_2143;
                        case 0x3C00:
                            return Ucs4Encoding.UCS4_2143;
                    }
                    break;
                case 0xFEFF:
                    if ( next2Bytes == 0x0000 ) {
                        return Ucs4Encoding.UCS4_3412;
                    }
                    else {
                        return Encoding.BigEndianUnicode;
                    }
                case 0xFFFE:
                    if ( next2Bytes == 0x0000 ) {
                        return Ucs4Encoding.UCS4_Littleendian;
                    }
                    else {
                        return Encoding.Unicode;
                    }
                case 0x3C00:
                    switch ( next2Bytes ) {
                        case 0x0000:
                            return Ucs4Encoding.UCS4_Littleendian;
                        case 0x3F00:
                            return Encoding.Unicode;
                    }
                    break;
                case 0x003C:
                    switch ( next2Bytes ) {
                        case 0x0000:
                            return Ucs4Encoding.UCS4_3412;
                        case 0x003F:
                            return Encoding.BigEndianUnicode;
                    }
                    break;
                case 0x4C6F:
                    if ( next2Bytes == 0xA794 ) {
                        Throw( Res.Xml_UnknownEncoding, "ebcdic" );
                    }
                    break;
                case 0xEFBB:
                    if ( ( next2Bytes & 0xFF00 ) == 0xBF00 ) {
                        return new UTF8Encoding( true, true );
                    }
                    break;
            }
            // Default encoding is ASCII (using SafeAsciiDecoder) until we read xml declaration. 
            // If we set UTF8 encoding now, it will throw exceptions (=slow) when decoding non-UTF8-friendly 
            // characters after the xml declaration, which may be perfectly valid in the encoding 
            // specified in xml declaration.
            return null;
        }

        private void SetupEncoding( Encoding encoding ) {
            if ( encoding == null ) {
                Debug.Assert( ps.charPos == 0 );
                ps.encoding = Encoding.UTF8;
                ps.decoder = new SafeAsciiDecoder();
            }
            else {
                ps.encoding = encoding;
                switch ( ps.encoding.CodePage ) {
                    case 1200:
                        ps.decoder = new UTF16Decoder( false );
                        break;
                    case 1201:
                        ps.decoder = new UTF16Decoder( true );
                        break;
                    default:
                        ps.decoder = encoding.GetDecoder();
                        break;
                }
            }
        }

        // Switches the reader's encoding
        private void SwitchEncoding( Encoding newEncoding ) {
            if ( ( newEncoding.CodePage != ps.encoding.CodePage || ps.decoder is SafeAsciiDecoder ) && !afterResetState) {
                Debug.Assert( ps.stream != null );
                UnDecodeChars();
                ps.appendMode = false;
                SetupEncoding( newEncoding );
                ReadData();
            }
        }

        // Returns the Encoding object for the given encoding name, if the reader's encoding can be switched to that encoding.
        // Performs checks whether switching from current encoding to specified encoding is allowed.
        private Encoding CheckEncoding( string newEncodingName ) {
            // encoding can be switched on stream input only
            if ( ps.stream == null ) {
                return ps.encoding;
            }

             if ( 0 == String.Compare( newEncodingName, "ucs-2", StringComparison.OrdinalIgnoreCase ) || 
                 0 == String.Compare( newEncodingName, "utf-16", StringComparison.OrdinalIgnoreCase ) ||
                 0 == String.Compare( newEncodingName, "iso-10646-ucs-2", StringComparison.OrdinalIgnoreCase ) || 
                 0 == String.Compare( newEncodingName, "ucs-4", StringComparison.OrdinalIgnoreCase ) ) {
                if ( ps.encoding.CodePage != Encoding.BigEndianUnicode.CodePage && 
                     ps.encoding.CodePage != Encoding.Unicode.CodePage && 
                     0 != String.Compare( newEncodingName, "ucs-4", StringComparison.OrdinalIgnoreCase ) ) {
                    if ( afterResetState ) {
                        Throw( Res.Xml_EncodingSwitchAfterResetState, newEncodingName );
                    }
                    else {
                        ThrowWithoutLineInfo( Res.Xml_MissingByteOrderMark );
                    }
                }
                return ps.encoding;
            }

            Encoding newEncoding = null;
            if ( 0 == String.Compare( newEncodingName, "utf-8", StringComparison.OrdinalIgnoreCase ) ) {
                newEncoding = new UTF8Encoding( true, true );
            }
            else {
                try {
                    newEncoding = Encoding.GetEncoding( newEncodingName );
                    if ( newEncoding.CodePage == -1 ) {
                        Throw( Res.Xml_UnknownEncoding, newEncodingName );
                    }
                }
                catch ( NotSupportedException ) {
                    Throw( Res.Xml_UnknownEncoding, newEncodingName );
                }
                catch ( ArgumentException ) {
                    Throw( Res.Xml_UnknownEncoding, newEncodingName );
                }
                Debug.Assert( newEncoding.EncodingName != "UTF-8" );
            }

            // check for invalid encoding switches after ResetState
            if ( afterResetState && ps.encoding.CodePage != newEncoding.CodePage ) {
                Throw( Res.Xml_EncodingSwitchAfterResetState, newEncodingName );
            }

            return newEncoding;            
        }

        void UnDecodeChars() {
            Debug.Assert( ps.stream != null && ps.decoder != null && ps.bytes != null );
            Debug.Assert( ps.appendMode, "UnDecodeChars cannot be called after ps.appendMode has been changed to false" );

            ps.bytePos = documentStartBytePos; // byte position after preamble
            if ( ps.charPos > 0 ) {
                ps.bytePos += ps.encoding.GetByteCount( ps.chars, 0, ps.charPos );
            }
            ps.charsUsed = ps.charPos;
            ps.isEof = false; 
        }

        private void SwitchEncodingToUTF8() {
            SwitchEncoding( new UTF8Encoding( true, true ) );
        }

        // Reads more data to the character buffer, discarding already parsed chars / decoded bytes.
        int ReadData() {
            // Append Mode:  Append new bytes and characters to the buffers, do not rewrite them. Allocate new buffers
            //               if the current ones are full
            // Rewrite Mode: Reuse the buffers. If there is less than half of the char buffer left for new data, move 
            //               the characters that has not been parsed yet to the front of the buffer. Same for bytes.

            if ( ps.isEof ) {
                return 0;
            }

            int charsRead;
            if ( ps.appendMode ) {
                // the character buffer is full -> allocate a new one
                if ( ps.charsUsed == ps.chars.Length - 1 ) {
                    // invalidate node values kept in buffer - applies to attribute values only
                    for ( int i = 0; i < attrCount; i++ ) {
                        nodes[index + i + 1].OnBufferInvalidated();
                    }

                    char[] newChars = new char[ ps.chars.Length * 2 ];
                    Buffer.BlockCopy( ps.chars, 0, newChars, 0, ps.chars.Length * 2 );
                    ps.chars = newChars;
                }

                if ( ps.stream != null ) {
                    // the byte buffer is full -> allocate a new one
                    if ( ps.bytesUsed - ps.bytePos < MaxByteSequenceLen ) {
                        if ( ps.bytes.Length - ps.bytesUsed < MaxByteSequenceLen ) {
                            byte[] newBytes = new byte[ ps.bytes.Length * 2 ];
                            Buffer.BlockCopy( ps.bytes, 0, newBytes, 0, ps.bytesUsed );
                            ps.bytes = newBytes;
                        }
                    }
                }

                charsRead = ps.chars.Length - ps.charsUsed - 1;
                if ( charsRead > ApproxXmlDeclLength ) {
                    charsRead = ApproxXmlDeclLength;
                }
            }
            else {
                int charsLen = ps.chars.Length;
                if ( charsLen - ps.charsUsed <= charsLen/2 ) {
                    // invalidate node values kept in buffer - applies to attribute values only
                    for ( int i = 0; i < attrCount; i++ ) {
                        nodes[index + i + 1].OnBufferInvalidated();
                    }

                    // move unparsed characters to front, unless the whole buffer contains unparsed characters
                    int copyCharsCount = ps.charsUsed - ps.charPos;
                    if ( copyCharsCount < charsLen - 1 ) {
                        ps.lineStartPos = ps.lineStartPos - ps.charPos;
                        if ( copyCharsCount > 0 ) {
                            Buffer.BlockCopy( ps.chars, ps.charPos * 2, ps.chars, 0, copyCharsCount * 2 );
                        }
                        ps.charPos = 0;
                        ps.charsUsed = copyCharsCount;
                    }
                    else {
                        char[] newChars = new char[ ps.chars.Length * 2 ];
                        Buffer.BlockCopy( ps.chars, 0, newChars, 0, ps.chars.Length * 2 );
                        ps.chars = newChars;
                    }
                }

                if ( ps.stream != null ) {
                    // move undecoded bytes to the front to make some space in the byte buffer
                    int bytesLeft = ps.bytesUsed - ps.bytePos;
                    if ( bytesLeft <= MaxBytesToMove  ) {
                        if ( bytesLeft == 0 ) {
                            ps.bytesUsed = 0;
                        }
                        else {
                            Buffer.BlockCopy( ps.bytes, ps.bytePos, ps.bytes, 0, bytesLeft );
                            ps.bytesUsed = bytesLeft;
                        }
                        ps.bytePos = 0;
                    }
                }
                charsRead = ps.chars.Length - ps.charsUsed - 1;
            }

            if ( ps.stream != null ) {
                if ( !ps.isStreamEof ) {
                    // read new bytes
                    if ( ps.bytePos == ps.bytesUsed && ps.bytes.Length - ps.bytesUsed > 0 ) {
                        int read = ps.stream.Read( ps.bytes, ps.bytesUsed, ps.bytes.Length - ps.bytesUsed );
                        if ( read == 0 ) {
                            ps.isStreamEof = true;
                        }
                        ps.bytesUsed += read;
                    }
                }

                int originalBytePos = ps.bytePos;

                // decode chars
                charsRead = GetChars( charsRead );
                if ( charsRead == 0 && ps.bytePos != originalBytePos ) {
                    // GetChars consumed some bytes but it was not enough bytes to form a character -> try again
                    return ReadData();
                }
            }
            else if ( ps.textReader != null ) {
                // read chars
                charsRead = ps.textReader.Read( ps.chars, ps.charsUsed, ps.chars.Length - ps.charsUsed - 1 );  
                ps.charsUsed += charsRead;
            }
            else {
                charsRead = 0;
            }

            if ( charsRead == 0 ) {
                Debug.Assert ( ps.charsUsed < ps.chars.Length );
                ps.isEof = true;
            }
            ps.chars[ ps.charsUsed ] = (char)0;
            return charsRead;
        }

        // Stream input only: read bytes from stream and decodes them according to the current encoding 
        int GetChars( int maxCharsCount ) {
            Debug.Assert( ps.stream != null && ps.decoder != null && ps.bytes != null );
            Debug.Assert( maxCharsCount <= ps.chars.Length - ps.charsUsed - 1 ); 
            
            // determine the maximum number of bytes we can pass to the decoder
            int bytesCount = ps.bytesUsed - ps.bytePos;
            if ( bytesCount == 0 ) {
                return 0;
            }
            
            int charsCount;
            bool completed;
            try {
                // decode chars
                ps.decoder.Convert( ps.bytes, ps.bytePos, bytesCount, ps.chars, ps.charsUsed, maxCharsCount, false, out bytesCount, out charsCount, out completed );
            }
            catch ( ArgumentException e ) {
                // Invalid character exception, lets get as many chars as we can:
                Debug.Assert( e.ParamName == null, "Expecting 'invalid character in given encoding' exception." );
                InvalidCharRecovery( ref bytesCount, out charsCount );
            }

            // move pointers and return
            ps.bytePos += bytesCount;
            ps.charsUsed += charsCount;
            Debug.Assert( maxCharsCount >= charsCount );
            return charsCount;
        }

        private void InvalidCharRecovery( ref int bytesCount, out int charsCount ) {
            int charsDecoded = 0;
            int bytesDecoded = 0;
            try {
                while ( bytesDecoded <  bytesCount ) {
                    int chDec;
                    int bDec;
                    bool completed;
                    ps.decoder.Convert( ps.bytes, ps.bytePos + bytesDecoded, 1, ps.chars, ps.charsUsed + charsDecoded, 1, false, out bDec, out chDec, out completed );
                    charsDecoded += chDec;
                    bytesDecoded += bDec;
                }
                Debug.Assert( false, "We should get an exception again." );
            }
            catch ( ArgumentException ) {
            }

            if ( charsDecoded == 0 ) {
                Throw( ps.charsUsed, Res.Xml_InvalidCharInThisEncoding );
            }
            charsCount = charsDecoded;
            bytesCount = bytesDecoded;
        }

        internal void Close( bool closeInput ) {
            if ( parsingFunction == ParsingFunction.ReaderClosed ) {
                return;
            }

            while ( InEntity ) {
                PopParsingState();
            }

            ps.Close( closeInput );

            curNode = NodeData.None;
            parsingFunction = ParsingFunction.ReaderClosed;
            reportedEncoding = null;
            reportedBaseUri = string.Empty;
            readState = ReadState.Closed;
            fullAttrCleanup = false;
            ResetAttributes();
        }

        void ShiftBuffer( int sourcePos, int destPos, int count ) {
            // PERF: Buffer.BlockCopy is faster than Array.Copy
            Buffer.BlockCopy( ps.chars, sourcePos * 2, ps.chars, destPos * 2, count * 2 );
        }

        // Parses the xml or text declaration and switched encoding if needed
        private bool ParseXmlDeclaration( bool isTextDecl ) {
            while ( ps.charsUsed - ps.charPos < 6 ) {  // minimum "<?xml "
                if ( ReadData() == 0 ) {
                    goto NoXmlDecl;
                }
            }

            if ( !XmlConvert.StrEqual( ps.chars, ps.charPos, 5, XmlDeclarationBegining ) ||
                 xmlCharType.IsNameChar( ps.chars[ps.charPos + 5] ) ) {
                goto NoXmlDecl;
            }

            if ( !isTextDecl ) {
                curNode.SetLineInfo( ps.LineNo, ps.LinePos + 2 );
                curNode.SetNamedNode( XmlNodeType.XmlDeclaration, Xml );
            }
            ps.charPos += 5;

            // parsing of text declarations cannot change global stringBuidler or curNode as we may be in the middle of a text node
            Debug.Assert( stringBuilder.Length == 0 || isTextDecl );
            BufferBuilder sb = isTextDecl ? new BufferBuilder() : stringBuilder;

            // parse version, encoding & standalone attributes
            int xmlDeclState = 0;   // <?xml (0) version='1.0' (1) encoding='__' (2) standalone='__' (3) ?>
            Encoding encoding = null;

            for (;;) {
                int originalSbLen = sb.Length;
                int wsCount = EatWhitespaces( xmlDeclState == 0 ? null : sb );

                // end of xml declaration
                if ( ps.chars[ps.charPos] == '?' ) {
                    sb.Length = originalSbLen;

                    if ( ps.chars[ps.charPos + 1] == '>' ) {
                        if ( xmlDeclState == 0 ) {
                            Throw( isTextDecl ? Res.Xml_InvalidTextDecl : Res.Xml_InvalidXmlDecl );
                        }

                        ps.charPos += 2;
                        if ( !isTextDecl ) {
                            curNode.SetValue( sb.ToString() );
                            sb.Length = 0;

                            nextParsingFunction = parsingFunction;
                            parsingFunction = ParsingFunction.ResetAttributesRootLevel;
                        }

                        // switch to encoding specified in xml declaration
                        if ( encoding == null ) {
                            if ( isTextDecl ) {
                                Throw( Res.Xml_InvalidTextDecl );
                            }
                            if ( afterResetState ) {
                                // check for invalid encoding switches to default encoding
                                int codePage = ps.encoding.CodePage;
                                if ( codePage != Encoding.UTF8.CodePage && codePage != Encoding.Unicode.CodePage &&
                                     codePage != Encoding.BigEndianUnicode.CodePage && !( ps.encoding is Ucs4Encoding ) ) {
                                    Throw( Res.Xml_EncodingSwitchAfterResetState, ( ps.encoding.GetByteCount( "A" ) == 1 ) ? "UTF-8" : "UTF-16" );
                                }
                            }
                            if ( ps.decoder is SafeAsciiDecoder ) {
                                SwitchEncodingToUTF8();
                            }
                        }
                        else {
                            SwitchEncoding( encoding );
                        }
                        ps.appendMode = false; 
                        return true;
                    }
                    else if ( ps.charPos + 1 == ps.charsUsed ) {
                        goto ReadData;
                    }
                    else {
                        ThrowUnexpectedToken( "'>'" );
                    }
                }

                if ( wsCount == 0 && xmlDeclState != 0 ) {
                    ThrowUnexpectedToken( "?>" );
                }
    
                // read attribute name            
                int nameEndPos = ParseName();

                NodeData attr = null;
                switch ( ps.chars[ps.charPos] ) {
                    case 'v':
                        if ( XmlConvert.StrEqual( ps.chars, ps.charPos, nameEndPos - ps.charPos, "version" ) && xmlDeclState == 0 ) {
                            if ( !isTextDecl ) {
                                attr = AddAttributeNoChecks( "version", 0 );
                            }
                            break;
                        }
                        goto default;
                    case 'e':
                        if ( XmlConvert.StrEqual( ps.chars, ps.charPos, nameEndPos - ps.charPos, "encoding" ) && 
                            ( xmlDeclState == 1 || ( isTextDecl && xmlDeclState == 0 ) ) ) {
                            if ( !isTextDecl ) {
                                attr = AddAttributeNoChecks( "encoding", 0 );
                            }
                            xmlDeclState = 1;
                            break;
                        }
                        goto default;
                    case 's':
                        if ( XmlConvert.StrEqual( ps.chars, ps.charPos, nameEndPos - ps.charPos, "standalone" ) &&
                             ( xmlDeclState == 1 || xmlDeclState == 2 ) && !isTextDecl ) {
                            if ( !isTextDecl ) {
                                attr = AddAttributeNoChecks( "standalone", 0 );
                            }
                            xmlDeclState = 2;
                            break;
                        }
                        goto default;
                    default:
                        Throw( isTextDecl ? Res.Xml_InvalidTextDecl : Res.Xml_InvalidXmlDecl );
                        break;
                }
                if ( !isTextDecl ) {
                    attr.SetLineInfo( ps.LineNo, ps.LinePos );
                }
                sb.Append( ps.chars, ps.charPos, nameEndPos - ps.charPos );
                ps.charPos = nameEndPos;

                // parse equals and quote char; 
                if ( ps.chars[ps.charPos] != '=' ) {
                    EatWhitespaces( sb );
                    if ( ps.chars[ps.charPos] != '=' ) {
                        ThrowUnexpectedToken( "=" );
                    }
                }
                sb.Append( '=' );
                ps.charPos++;

                char quoteChar = ps.chars[ps.charPos];
                if ( quoteChar != '"' && quoteChar != '\'' ) {
                    EatWhitespaces( sb );
                    quoteChar = ps.chars[ps.charPos];
                    if ( quoteChar != '"' && quoteChar != '\'' ) {
                        ThrowUnexpectedToken( "\"", "'" );
                    }
                }
                sb.Append( quoteChar );
                ps.charPos++;
                if ( !isTextDecl ) {
                    attr.quoteChar = quoteChar;
                    attr.SetLineInfo2( ps.LineNo, ps.LinePos );
                }

                // parse attribute value
                int pos = ps.charPos;
                char[] chars;
            Continue:
                chars = ps.chars;
                unsafe {
                    while ( ( (xmlCharType.charProperties[chars[pos]] & XmlCharType.fAttrValue) != 0) ) {
                        pos++;
                    }
                }

                if ( ps.chars[pos] == quoteChar ) {
                    switch ( xmlDeclState ) {
                        // version
                        case 0:
                            if ( XmlConvert.StrEqual( ps.chars, ps.charPos, pos - ps.charPos, "1.0" ) ) {
                                if ( !isTextDecl ) {
                                    attr.SetValue( ps.chars, ps.charPos, pos - ps.charPos );
                                }
                                xmlDeclState = 1;
                            }
                            else {
                                string badVersion = new string( ps.chars, ps.charPos, pos - ps.charPos );
                                Throw( Res.Xml_InvalidVersionNumber, badVersion );
                            }
                            break;
                        case 1:
                            string encName = new string( ps.chars, ps.charPos, pos - ps.charPos );
                            encoding = CheckEncoding( encName );
                            if ( !isTextDecl ) {
                                attr.SetValue( encName );
                            }
                            xmlDeclState = 2;
                            break;
                        case 2:
                            if ( XmlConvert.StrEqual( ps.chars, ps.charPos, pos - ps.charPos, "yes" ) ) {
                                this.standalone = true;
                            }
                            else if ( XmlConvert.StrEqual( ps.chars, ps.charPos, pos - ps.charPos, "no" ) ) {
                                this.standalone = false;
                            }
                            else {
                                Debug.Assert( !isTextDecl );
                                Throw( Res.Xml_InvalidXmlDecl, ps.LineNo, ps.LinePos - 1 );
                            }
                            if ( !isTextDecl ) {
                                attr.SetValue( ps.chars, ps.charPos, pos - ps.charPos );
                            }
                            xmlDeclState = 3;
                            break;
                        default:
                            Debug.Assert( false );
                            break;
                    }
                    sb.Append( chars, ps.charPos, pos - ps.charPos );
                    sb.Append( quoteChar );
                    ps.charPos = pos + 1;
                    continue;
                }
                else if ( pos == ps.charsUsed ) {
                    if ( ReadData() != 0 ) {
                        goto Continue;
                    }
                    else {
                        Throw( Res.Xml_UnclosedQuote );
                    }
                }
                else {
                    Throw( isTextDecl ? Res.Xml_InvalidTextDecl : Res.Xml_InvalidXmlDecl );
                }
                            
            ReadData:
                if ( ps.isEof || ReadData() == 0 ) {
                    Throw( Res.Xml_UnexpectedEOF1 );
                }
            }

        NoXmlDecl:
            // no xml declaration
            if ( !isTextDecl ) {
                parsingFunction = nextParsingFunction;
            }
            if ( afterResetState ) {
                // check for invalid encoding switches to default encoding
                int codePage = ps.encoding.CodePage;
                if ( codePage != Encoding.UTF8.CodePage && codePage != Encoding.Unicode.CodePage &&
                    codePage != Encoding.BigEndianUnicode.CodePage && !( ps.encoding is Ucs4Encoding ) ) {
                    Throw( Res.Xml_EncodingSwitchAfterResetState, ( ps.encoding.GetByteCount( "A" ) == 1 ) ? "UTF-8" : "UTF-16" );
                }
            }
            if ( ps.decoder is SafeAsciiDecoder ) {
                SwitchEncodingToUTF8();
            }
            ps.appendMode = false;
            return false;
        }

        // Parses the document content
        private bool ParseDocumentContent() {

            for (;;) {
                bool needMoreChars = false;
                int pos = ps.charPos;
                char[] chars = ps.chars;

                // some tag
                if ( chars[pos] == '<' ) {
                    needMoreChars = true;
                    if ( ps.charsUsed - pos < 4 ) // minimum  "<a/>"
                        goto ReadData;
                    pos++;
                    switch ( chars[pos] ) {
                        // processing instruction
                        case '?':
                            ps.charPos = pos + 1;
                            if ( ParsePI() ) {
                                return true;
                            }
                            continue;
                        case '!':
                            pos++;
                            if ( ps.charsUsed - pos < 2 ) // minimum characters expected "--"
                                goto ReadData;
                            // comment
                            if ( chars[pos] == '-' ) {
                                if ( chars[pos+1] == '-' ) {
                                    ps.charPos = pos + 2;
                                    if ( ParseComment() ) {
                                        return true;
                                    }
                                    continue;
                                }
                                else {
                                    ThrowUnexpectedToken( pos + 1, "-" );
                                }
                            }
                            // CDATA section
                            else if ( chars[pos] == '[' ) {
                                if ( fragmentType != XmlNodeType.Document ) {
                                    pos++;
                                    if ( ps.charsUsed - pos < 6 ) {
                                        goto ReadData;
                                    }
                                    if ( XmlConvert.StrEqual( chars, pos, 6, "CDATA[" ) ) {
                                        ps.charPos = pos + 6;
                                        ParseCData();
                                        if ( fragmentType == XmlNodeType.None ) {
                                            fragmentType = XmlNodeType.Element;
                                        }
                                        return true; 
                                    }
                                    else {
                                        ThrowUnexpectedToken( pos, "CDATA[" );
                                    }
                                }
                                else {
                                    Throw( ps.charPos, Res.Xml_InvalidRootData );
                                }
                            }
                            // DOCTYPE declaration
                            else {
                                if ( fragmentType == XmlNodeType.Document || fragmentType == XmlNodeType.None ) {
                                    fragmentType = XmlNodeType.Document;
                                    ps.charPos = pos;
                                    ParseDoctypeDecl();
                                    return true;
                                }
                                else {
                                    if ( ParseUnexpectedToken( pos ) == "DOCTYPE" ) {
                                        Throw( Res.Xml_BadDTDLocation );
                                    }
                                    else {
                                        ThrowUnexpectedToken( pos, "<!--", "<[CDATA[" ); 
                                    }
                                }
                            }
                            break;
                        case '/':
                            Throw( pos + 1, Res.Xml_UnexpectedEndTag );
                            break;
                        // document element start tag
                        default:
                            if ( rootElementParsed ) {
                                if ( fragmentType == XmlNodeType.Document ) {
                                    Throw( pos, Res.Xml_MultipleRoots );
                                }
                                if ( fragmentType == XmlNodeType.None ) {
                                    fragmentType = XmlNodeType.Element;
                                }
                            }
                            ps.charPos = pos;
                            rootElementParsed = true;
                            ParseElement();
                            return true;
                    }
                }
                else if ( chars[pos] == '&' ) {
                    if ( fragmentType == XmlNodeType.Document ) {
                        Throw( pos, Res.Xml_InvalidRootData );
                    }
                    else {
                        if ( fragmentType == XmlNodeType.None ) {
                            fragmentType = XmlNodeType.Element;
                        }
                        int i;
                        switch ( HandleEntityReference( false, EntityExpandType.OnlyGeneral, out i ) ) {
                            case EntityType.Unexpanded:
                                if ( parsingFunction == ParsingFunction.EntityReference ) {
                                    parsingFunction = nextParsingFunction;
                                }
                                ParseEntityReference();
                                return true;
                            case EntityType.CharacterDec:
                            case EntityType.CharacterHex:
                            case EntityType.CharacterNamed:
                                if ( ParseText() ) {
                                    return true;
                                }
                                continue;
                            default:
                                chars = ps.chars;
                                pos = ps.charPos;
                                continue;
                        }
                    }
                }
                // end of buffer
                else if ( pos == ps.charsUsed || ( v1Compat && chars[pos] == 0x0 ) ) {
                    goto ReadData;
                }
                // something else -> root level whitespaces
                else {
                    if ( fragmentType == XmlNodeType.Document ) {
                        if ( ParseRootLevelWhitespace() ) {
                            return true;
                        }
                    }
                    else {
                        if ( ParseText() ) {
                            if ( fragmentType == XmlNodeType.None && curNode.type == XmlNodeType.Text ) {
                                fragmentType = XmlNodeType.Element;
                            }
                            return true;
                        }
                    }
                    chars = ps.chars;
                    pos = ps.charPos;
                    continue;
                }

                Debug.Assert( pos == ps.charsUsed && !ps.isEof );

            ReadData:
                // read new characters into the buffer
                if ( ReadData() != 0 ) {
                    pos = ps.charPos;
                }
                else {
                    if ( needMoreChars ) {
                        Throw( Res.Xml_InvalidRootData );
                    }

                    if ( InEntity ) {
                        if ( HandleEntityEnd( true ) ) {
                            SetupEndEntityNodeInContent();
                            return true;
                        }
                        continue;
                    }
                    Debug.Assert( index == 0 );

                    if ( !rootElementParsed && fragmentType == XmlNodeType.Document ) {
                        ThrowWithoutLineInfo( Res.Xml_MissingRoot );
                    }
                    if ( fragmentType == XmlNodeType.None ) {
                        fragmentType = rootElementParsed ? XmlNodeType.Document : XmlNodeType.Element;
                    }
                    OnEof();
                    return false;
                }

                pos = ps.charPos;
                chars = ps.chars;
            }
        }
        
        // Parses element content
        private bool ParseElementContent() {

            for (;;) {
                int pos = ps.charPos;
                char[] chars = ps.chars;

                switch ( chars[pos] ) {
                    // some tag
                    case '<':
                        switch ( chars[pos+1] ) {
                            // processing instruction
                            case '?':
                                ps.charPos = pos + 2;
                                if ( ParsePI() ) {
                                    return true;
                                }
                                continue;
                            case '!':
                                pos += 2;
                                if ( ps.charsUsed - pos < 2 ) 
                                    goto ReadData;
                                // comment
                                if ( chars[pos] == '-' ) {
                                    if ( chars[pos+1] == '-' ) {
                                        ps.charPos = pos + 2;
                                        if ( ParseComment() ) {
                                            return true;
                                        }
                                        continue;
                                    }
                                    else {
                                        ThrowUnexpectedToken( pos + 1, "-" );
                                    }
                                }
                                // CDATA section
                                else if ( chars[pos] == '[' ) {
                                    pos++;
                                    if ( ps.charsUsed - pos < 6 ) {
                                        goto ReadData;
                                    }
                                    if ( XmlConvert.StrEqual( chars, pos, 6, "CDATA[" ) ) {
                                        ps.charPos = pos + 6;
                                        ParseCData();
                                        return true; 
                                    }
                                    else {
                                        ThrowUnexpectedToken( pos, "CDATA[" );
                                    }
                                }
                                else {

                                    if ( ParseUnexpectedToken( pos ) == "DOCTYPE" ) {
                                        Throw( Res.Xml_BadDTDLocation );
                                    }
                                    else {
                                        ThrowUnexpectedToken( pos, "<!--", "<[CDATA[" ); 
                                    }
                                }
                                break;
                            // element end tag
                            case '/':
                                ps.charPos = pos + 2;
                                ParseEndElement();
                                return true;
                            default:
                                // end of buffer
                                if ( pos+1 == ps.charsUsed ) {
                                    goto ReadData;
                                }
                                else {
                                    // element start tag
                                    ps.charPos = pos + 1;
                                    ParseElement();
                                    return true;
                                }
                        }
                        break;
                    case '&':
                        if ( ParseText() ) {
                            return true;
                        }
                        continue;
                    default:
                        // end of buffer
                        if ( pos == ps.charsUsed ) {
                            goto ReadData;
                        }
                        else {
                            // text node, whitespace or entity reference
                            if ( ParseText() ) {
                                return true;
                            }
                            continue;
                        }
                }

            ReadData:
                // read new characters into the buffer
                if ( ReadData() == 0 ) {
                    if ( ps.charsUsed - ps.charPos != 0 ) {
                        ThrowUnclosedElements();
                    }
                    if ( !InEntity ) {
                        if ( index == 0 && fragmentType != XmlNodeType.Document ) {
                            OnEof();
                            return false;
                        }
                        ThrowUnclosedElements();
                    }
                    if ( HandleEntityEnd( true ) ) {
                        SetupEndEntityNodeInContent();
                        return true;
                    }
                }
            }
        }

        private void ThrowUnclosedElements() {
            if ( index == 0 && curNode.type != XmlNodeType.Element ) {
                Throw( ps.charsUsed, Res.Xml_UnexpectedEOF1 );
            }
            else {
                int i = ( parsingFunction == ParsingFunction.InIncrementalRead ) ? index : index - 1;
                stringBuilder.Length = 0;
                for ( ; i >= 0; i-- ) {
                    NodeData el = nodes[i];
                    if ( el.type != XmlNodeType.Element ) {
                        continue;
                    }
                    stringBuilder.Append( el.GetNameWPrefix( nameTable ) );
                    if ( i > 0 ) {
                        stringBuilder.Append( ", " );
                    }
                    else {
                        stringBuilder.Append( "." );
                    }
                }
                Throw ( ps.charsUsed, Res.Xml_UnexpectedEOFInElementContent, stringBuilder.ToString() );
            }
        }

        // Parses the element start tag
        private void ParseElement() {
            int pos = ps.charPos;
            char[] chars = ps.chars;
            int colonPos = -1;

            curNode.SetLineInfo( ps.LineNo, ps.LinePos );

            // PERF: we intentionally don't call ParseQName here to parse the element name unless a special 
            // case occurs (like end of buffer, invalid name char)
        ContinueStartName:
            // check element name start char
            unsafe {
                if ( !( ( xmlCharType.charProperties[chars[pos]] & XmlCharType.fNCStartName ) != 0 ) ) {  
                    goto ParseQNameSlow;
                }
                pos++;
            }

        ContinueName:
            unsafe {
                // parse element name
                while ( ( ( xmlCharType.charProperties[chars[pos]] & XmlCharType.fNCName ) != 0 ) ) {
                    pos++;
                }
            }
            
            // colon -> save prefix end position and check next char if it's name start char
            if ( chars[pos] == ':' ) {
                if ( colonPos != -1 ) {
                    if ( supportNamespaces ) {
                        Throw( pos, Res.Xml_BadNameChar, XmlException.BuildCharExceptionStr( ':' ) );
                    }
                    else {
                        pos++;
                        goto ContinueName;
                    }
                }
                else {
                    colonPos = pos;
                    pos++;
                    goto ContinueStartName;
                }
            }
            else if ( pos < ps.charsUsed )  {
                goto SetElement;
            }

        ParseQNameSlow:
            pos = ParseQName( out colonPos );
            chars = ps.chars;
            
        SetElement:
            // push namespace context
            namespaceManager.PushScope();

            // init the NodeData class
            if ( colonPos == -1 || !supportNamespaces ) {
                curNode.SetNamedNode( XmlNodeType.Element, 
                                      nameTable.Add( chars, ps.charPos, pos - ps.charPos ) );
            }
            else {
                int startPos = ps.charPos;
                int prefixLen = colonPos - startPos;
                if ( prefixLen == lastPrefix.Length && XmlConvert.StrEqual( chars, startPos, prefixLen, lastPrefix ) ) {
                    curNode.SetNamedNode( XmlNodeType.Element, 
                                          nameTable.Add( chars, colonPos + 1, pos - colonPos - 1 ),
                                          lastPrefix,
                                          null );
                }
                else {
                    curNode.SetNamedNode( XmlNodeType.Element, 
                                          nameTable.Add( chars, colonPos + 1, pos - colonPos - 1 ),
                                          nameTable.Add( chars, ps.charPos, prefixLen ),
                                          null );
                    lastPrefix = curNode.prefix;
                }
            }

            char ch = chars[pos];
            // white space after element name -> there are probably some attributes
            bool isWs;  
            unsafe {
                isWs = ( ( xmlCharType.charProperties[ch] & XmlCharType.fWhitespace ) != 0 );
            }
            if ( isWs ) {
                ps.charPos = pos;
                ParseAttributes();
                return;
            }
            // no attributes
            else {
                // non-empty element
                if ( ch == '>' ) {
                    ps.charPos = pos + 1;
                    parsingFunction = ParsingFunction.MoveToElementContent;
                }
                // empty element
                else if ( ch == '/' ) {
                    if ( pos+1 == ps.charsUsed ) {
                        ps.charPos = pos;
                        if ( ReadData() == 0 ) {
                            Throw( pos, Res.Xml_UnexpectedEOF, ">" );
                        }
                        pos = ps.charPos;
                        chars = ps.chars;
                    }
                    if ( chars[pos+1] == '>' ) {
                        curNode.IsEmptyElement = true;
                        nextParsingFunction = parsingFunction;
                        parsingFunction = ParsingFunction.PopEmptyElementContext;
                        ps.charPos = pos + 2;
                    }
                    else {
                        ThrowUnexpectedToken( pos, ">" );
                    }
                }
                // something else after the element name
                else {
                    Throw( pos, Res.Xml_BadNameChar, XmlException.BuildCharExceptionStr( ch ) );
                }

                // add default attributes & strip spaces in attributes with type other than CDATA
                if ( addDefaultAttributesAndNormalize ) {
                    AddDefaultAttributesAndNormalize();
                }

                // lookup element namespace
                ElementNamespaceLookup();
            }
        }
        
        private void AddDefaultAttributesAndNormalize() {
            Debug.Assert( curNode.type == XmlNodeType.Element );
            SchemaElementDecl elementDecl;
            
            qName.Init( curNode.localName, curNode.prefix );
            SchemaInfo dtdSchemaInfo = dtdParserProxy.DtdSchemaInfo;
            if ( ( elementDecl = dtdSchemaInfo.GetElementDecl( qName ) ) == null ) {
                if ( ( elementDecl = (SchemaElementDecl)dtdSchemaInfo.UndeclaredElementDecls[qName] ) == null ) {
                    return;
                }
            }

            // fix non-CDATA attribute value
            if ( normalize && elementDecl.HasNonCDataAttribute ) {
                // go through the attributes and normalize it if not CDATA type
                for ( int i = index + 1; i < index + 1 + attrCount; i++ ) {
                    NodeData attr = nodes[i];
                    qName.Init( attr.localName, attr.prefix );
                    SchemaAttDef attrDef = elementDecl.GetAttDef( qName );
                    if ( attrDef != null && attrDef.SchemaType.Datatype.TokenizedType != XmlTokenizedType.CDATA ) {
                        if ( DtdValidation && standalone && attrDef.IsDeclaredInExternal ) {
                            // VC constraint:
                            // The standalone document declaration must have the value "no" if any external markup declarations
                            // contain declarations of attributes with values subject to normalization, where the attribute appears in
                            // the document with a value which will change as a result of normalization,
                            string oldValue = attr.StringValue;
                            attr.TrimSpacesInValue();

                            if ( oldValue != attr.StringValue ) {
                                SendValidationEvent( XmlSeverityType.Error, Res.Sch_StandAloneNormalization, attr.GetNameWPrefix( nameTable ), attr.LineNo, attr.LinePos );
                            }
                        }
                        else {
                            attr.TrimSpacesInValue();
                        }
                    }
                }
            }

            // add default attributes
            SchemaAttDef[] defaultAttributes = elementDecl.DefaultAttDefs;
            if ( defaultAttributes != null ) {
                int originalAttrCount = attrCount;
                for ( int i = 0; i < defaultAttributes.Length; i++ ) {
                    SchemaAttDef attrDef = defaultAttributes[i];
                    if ( AddDefaultAttribute( attrDef, true ) ) {
                        if ( DtdValidation && standalone && attrDef.IsDeclaredInExternal ) {
                            SendValidationEvent( XmlSeverityType.Error, Res.Sch_UnSpecifiedDefaultAttributeInExternalStandalone, attrDef.Name.Name, curNode.LineNo, curNode.LinePos );
                        }
                    }
                }

                if ( originalAttrCount == 0 && attrNeedNamespaceLookup ) {
                    AttributeNamespaceLookup();
                    attrNeedNamespaceLookup = false;
                }
            }
        }

        // parses the element end tag
        private void ParseEndElement() {
            // check if the end tag name equals start tag name
            NodeData startTagNode = nodes[index-1];

            int prefLen = startTagNode.prefix.Length;
            int locLen = startTagNode.localName.Length;

            while ( ps.charsUsed - ps.charPos < prefLen + locLen + 1 ) {
                if ( ReadData() == 0 ) {
                    break;
                }
            }

            int nameLen;
            char[] chars = ps.chars;
            if ( startTagNode.prefix.Length == 0 ) {
                if ( !XmlConvert.StrEqual( chars, ps.charPos, locLen, startTagNode.localName ) ) {
                    ThrowTagMismatch( startTagNode );
                }
                nameLen = locLen;
            }
            else {
                int colonPos = ps.charPos + prefLen;
                if ( !XmlConvert.StrEqual( chars, ps.charPos, prefLen, startTagNode.prefix ) ||
                        chars[colonPos] != ':' ||
                        !XmlConvert.StrEqual( chars, colonPos + 1, locLen, startTagNode.localName ) ) {
                    ThrowTagMismatch( startTagNode );
                }
                nameLen = locLen + prefLen + 1;
            }

            int pos;
            for (;;) {
                pos = ps.charPos + nameLen;
                chars = ps.chars;

                if ( pos == ps.charsUsed ) {
                    goto ReadData;
                }
                unsafe {
                    if ( ( ( (xmlCharType.charProperties[chars[pos]] & XmlCharType.fNCName) != 0 ) || (chars[pos]) == ':') ) {
                        ThrowTagMismatch(startTagNode);
                    }

                    // eat whitespaces
                    while ( ( (xmlCharType.charProperties[chars[pos]] & XmlCharType.fWhitespace ) != 0 ) ) {
                        pos++;
                    }
                }

                if ( chars[pos] == '>' ) {
                    break;
                }
                else if ( pos == ps.charsUsed ) {
                    goto ReadData;
                }
                else {
                    ThrowUnexpectedToken( pos, ">" );
                }

                Debug.Assert( false, "We should never get to this point." );

            ReadData:
                if ( ReadData() == 0 ) {
                    ThrowUnclosedElements();
                }
            }

            Debug.Assert( index > 0 );
            index--;
            curNode = nodes[index];

            // set the element data
            Debug.Assert( curNode == startTagNode );
            startTagNode.SetLineInfo( ps.LineNo, ps.LinePos );
            startTagNode.type = XmlNodeType.EndElement;

            ps.charPos = pos + 1;

            // set next parsing function
            nextParsingFunction = ( index > 0 ) ? parsingFunction : ParsingFunction.DocumentContent;
            parsingFunction = ParsingFunction.PopElementContext;
        }

        private void ThrowTagMismatch( NodeData startTag ) {
            if ( startTag.type == XmlNodeType.Element ) { 
                // parse the bad name
                int colonPos;
                int endPos = ParseQName( out colonPos );

                string[] args = new string[3];
                args[0] = startTag.GetNameWPrefix( nameTable );
                args[1] = startTag.lineInfo.lineNo.ToString(CultureInfo.InvariantCulture);
                args[2] = new string( ps.chars, ps.charPos, endPos - ps.charPos );
                Throw( Res.Xml_TagMismatch, args );
            }
            else {
                Debug.Assert( startTag.type == XmlNodeType.EntityReference );
                Throw( Res.Xml_UnexpectedEndTag );
            }
        }

        // Reads the attributes
        private void ParseAttributes() {
            int pos = ps.charPos;
            char[] chars = ps.chars;
            NodeData attr = null;

            Debug.Assert( attrCount == 0 );

            for (;;) {
                // eat whitespaces
                int lineNoDelta = 0;
                char tmpch0;
                unsafe {
                    while ( ( ( xmlCharType.charProperties[tmpch0 = chars[pos]] & XmlCharType.fWhitespace ) != 0 ) ) {
                        if ( tmpch0 == (char)0xA ) {
                            OnNewLine( pos + 1 );
                            lineNoDelta++;
                        }
                        else if ( tmpch0 == (char)0xD ) {
                            if ( chars[pos+1] == (char)0xA ) {
                                OnNewLine( pos + 2 );
                                lineNoDelta++;
                                pos++;
                            }
                            else if ( pos+1 != ps.charsUsed ) {
                                OnNewLine( pos + 1 );
                                lineNoDelta++;
                            }
                            else {
                                ps.charPos = pos;
                                goto ReadData;
                            }
                        }
                        pos++;
                    }
                }

                char tmpch1;
                bool isNCStartName;
                unsafe {
                    isNCStartName = ( ( xmlCharType.charProperties[tmpch1 = chars[pos]] & XmlCharType.fNCStartName ) != 0 );
                }
                if ( !isNCStartName ) {
                    // element end
                    if ( tmpch1 == '>' ) {
                        Debug.Assert( curNode.type == XmlNodeType.Element );
                        ps.charPos = pos + 1;
                        parsingFunction = ParsingFunction.MoveToElementContent;
                        goto End;
                    }
                    // empty element end
                    else if ( tmpch1 == '/' ) {
                        Debug.Assert( curNode.type == XmlNodeType.Element );
                        if ( pos+1 == ps.charsUsed ) {
                            goto ReadData;
                        }
                        if ( chars[pos+1] == '>' ) {
                            ps.charPos = pos + 2;
                            curNode.IsEmptyElement = true;
                            nextParsingFunction = parsingFunction;
                            parsingFunction = ParsingFunction.PopEmptyElementContext;
                            goto End;
                        }
                        else {
                            ThrowUnexpectedToken( pos + 1, ">" );
                        }
                    }
                    else if ( pos == ps.charsUsed ) {
                        goto ReadData;
                    }
                    else if ( tmpch1 != ':' || supportNamespaces ) {
                        Throw( pos, Res.Xml_BadStartNameChar, XmlException.BuildCharExceptionStr( tmpch1 ) );
                    }
                }

                if ( pos == ps.charPos ) {
                    Throw( Res.Xml_ExpectingWhiteSpace, ParseUnexpectedToken() );
                }
                ps.charPos = pos;

                // save attribute name line position
                int attrNameLinePos = ps.LinePos;

#if DEBUG
                int attrNameLineNo = ps.LineNo;
#endif

                // parse attribute name
                int colonPos = -1;
    
                // PERF: we intentionally don't call ParseQName here to parse the element name unless a special 
                // case occurs (like end of buffer, invalid name char)
                pos++; // start name char has already been checked

                // parse attribute name
            ContinueParseName:
                char tmpch2;
                unsafe {
                    while ( ( ( xmlCharType.charProperties[tmpch2 = chars[pos]] & XmlCharType.fNCName ) != 0 ) ) {
                        pos++;
                    }
                }
                
                // colon -> save prefix end position and check next char if it's name start char
                if ( tmpch2 == ':' ) {
                    if ( colonPos != -1 ) {
                        if ( supportNamespaces ) {
                            Throw( pos, Res.Xml_BadNameChar, XmlException.BuildCharExceptionStr( ':' ));
                        }
                        else {
                            pos++;
                            goto ContinueParseName;
                        }
                    }
                    else {
                        colonPos = pos;
                        pos++;
                        unsafe {
                            if ( ( ( xmlCharType.charProperties[chars[pos]] & XmlCharType.fNCStartName ) != 0 ) ) {
                                pos++;
                                goto ContinueParseName;
                            }
                        }
                        pos = ParseQName( out colonPos );
                        chars = ps.chars;
                    }
                }
                else if ( pos == ps.charsUsed ) {
                    pos = ParseQName( out colonPos );
                    chars = ps.chars;
                }

                attr = AddAttribute( pos, colonPos );
                attr.SetLineInfo( ps.LineNo, attrNameLinePos );

#if DEBUG
                Debug.Assert( attrNameLineNo == ps.LineNo );
#endif

                // parse equals and quote char; 
                if ( chars[pos] != '=' ) {
                    ps.charPos = pos;
                    EatWhitespaces( null );
                    pos = ps.charPos;
                    if ( chars[pos] != '=' ) {
                        ThrowUnexpectedToken( "=" );
                    }
                }
                pos++;

                char quoteChar = chars[pos];
                if ( quoteChar != '"' && quoteChar != '\'' ) {
                    ps.charPos = pos;
                    EatWhitespaces( null );
                    pos = ps.charPos;
                    quoteChar = chars[pos];
                    if ( quoteChar != '"' && quoteChar != '\'' ) {
                        ThrowUnexpectedToken( "\"", "'" );
                    }
                }
                pos++;
                ps.charPos = pos;

                attr.quoteChar = quoteChar;
                attr.SetLineInfo2( ps.LineNo, ps.LinePos );

                // parse attribute value
                char tmpch3;
                unsafe {
                    while ( ( ( xmlCharType.charProperties[tmpch3 = chars[pos]] & XmlCharType.fAttrValue ) != 0 ) ) {
                        pos++;
                    }
                }

                if ( tmpch3 == quoteChar ) {
#if DEBUG
                    if ( normalize ) {
                        string val = new string( chars, ps.charPos, pos - ps.charPos );
                        Debug.Assert( val == XmlComplianceUtil.CDataNormalize( val ), "The attribute value is not CDATA normalized!" ); 
                    }
#endif
                    attr.SetValue( chars, ps.charPos, pos - ps.charPos );
                    pos++;
                    ps.charPos = pos;
                }
                else {
                    ParseAttributeValueSlow( pos, quoteChar, attr );
                    pos = ps.charPos;
                    chars = ps.chars;
                }

                // handle special attributes:
                if ( attr.prefix.Length == 0 ) {
                    // default namespace declaration
                    if ( Ref.Equal( attr.localName, XmlNs ) ) {
                        OnDefaultNamespaceDecl( attr );
                    }
                }
                else {
                    // prefixed namespace declaration
                    if ( Ref.Equal( attr.prefix, XmlNs ) ) {
                        OnNamespaceDecl( attr );
                    }
                    // xml: attribute
                    else if ( Ref.Equal( attr.prefix, Xml ) ) {
                        OnXmlReservedAttribute( attr );
                    }
                }
                continue;

            ReadData:
                ps.lineNo -= lineNoDelta;
                if ( ReadData() != 0 ) {
                    pos = ps.charPos;
                    chars = ps.chars;
                }
                else {
                    ThrowUnclosedElements();
                }
            }

        End:
            if ( addDefaultAttributesAndNormalize ) {
                AddDefaultAttributesAndNormalize();
            }
            // lookup namespaces: element
            ElementNamespaceLookup();

            // lookup namespaces: attributes
            if ( attrNeedNamespaceLookup ) {
                AttributeNamespaceLookup();
                attrNeedNamespaceLookup = false;
            }

            // check duplicate attributes
            if ( attrDuplWalkCount >= MaxAttrDuplWalkCount ) {
                AttributeDuplCheck();
            }
        }

        private void ElementNamespaceLookup() {
            Debug.Assert( curNode.type == XmlNodeType.Element );
            if ( curNode.prefix.Length == 0 ) {
                curNode.ns = xmlContext.defaultNamespace;
            }
            else {
                curNode.ns = LookupNamespace( curNode );
            }
        }

        private void AttributeNamespaceLookup() {
            for ( int i = index + 1; i < index + attrCount + 1; i++ ) {
                NodeData at = nodes[i];
                if ( at.type == XmlNodeType.Attribute && at.prefix.Length > 0 ) {
                    at.ns = LookupNamespace( at );
                }
            }
        }

        private void AttributeDuplCheck() {
            if ( attrCount < MaxAttrDuplWalkCount ) {
                for ( int i = index + 1; i < index + 1 + attrCount; i++ ) {
                    NodeData attr1 = nodes[i];
                    for ( int j = i + 1; j < index + 1 + attrCount; j++ ) {
                        if ( Ref.Equal( attr1.localName, nodes[j].localName ) && Ref.Equal( attr1.ns, nodes[j].ns ) ) {
                            Throw( Res.Xml_DupAttributeName, nodes[j].GetNameWPrefix( nameTable ), nodes[j].LineNo, nodes[j].LinePos );
                        }
                    }
                }
            }
            else {
                if ( attrDuplSortingArray == null || attrDuplSortingArray.Length < attrCount ) {
                    attrDuplSortingArray = new NodeData[attrCount];
                }
                Array.Copy( nodes, index + 1, attrDuplSortingArray, 0, attrCount );
                Array.Sort( attrDuplSortingArray, 0, attrCount );

                NodeData attr1 = attrDuplSortingArray[0];
                for ( int i = 1; i < attrCount; i++ ) {
                    NodeData attr2 = attrDuplSortingArray[i];
                    if ( Ref.Equal( attr1.localName, attr2.localName ) && Ref.Equal( attr1.ns, attr2.ns ) ) {
                        Throw( Res.Xml_DupAttributeName, attr2.GetNameWPrefix( nameTable ), attr2.LineNo, attr2.LinePos );
                    }
                    attr1 = attr2;
                }
            }
        }
        
        private void OnDefaultNamespaceDecl( NodeData attr ) {
            if ( !supportNamespaces ) {
                return;
            }
            
            string ns = attr.GetAtomizedValue( nameTable );
            attr.SetValue( ns );
            attr.ns = nameTable.Add( XmlReservedNs.NsXmlNs );

            if ( !curNode.xmlContextPushed ) {
                PushXmlContext();
            }
            xmlContext.defaultNamespace = ns;

            AddNamespace( string.Empty, ns, attr );
        }

        private void OnNamespaceDecl( NodeData attr ) {
            if ( !supportNamespaces ) {
                return;
            }
            string ns = attr.GetAtomizedValue( nameTable );
            if ( ns.Length == 0 ) {
                Throw( Res.Xml_BadNamespaceDecl, attr.lineInfo2.lineNo, attr.lineInfo2.linePos - 1 ); 
            }
            attr.SetValue( ns );
            AddNamespace( attr.localName, ns, attr );
        }

        private void OnXmlReservedAttribute( NodeData attr ) {
            switch ( attr.localName ) {
                // xml:space
                case "space":
                    if ( !curNode.xmlContextPushed ) {
                        PushXmlContext();
                    }
                    switch ( XmlConvert.TrimString(attr.StringValue) ) {
                        case "preserve":
                            xmlContext.xmlSpace = XmlSpace.Preserve;
                            break;
                        case "default":
                            xmlContext.xmlSpace = XmlSpace.Default;
                            break;
                        default:
                            Throw( Res.Xml_InvalidXmlSpace, attr.StringValue, attr.lineInfo.lineNo, attr.lineInfo.linePos );
                            break;
                    }
                    break;
                // xml:lang
                case "lang":
                    if ( !curNode.xmlContextPushed ) {
                        PushXmlContext();
                    }
                    xmlContext.xmlLang = attr.StringValue;
                    break;
            }
        }

        private void ParseAttributeValueSlow( int curPos, char quoteChar, NodeData attr ) {
            int pos = curPos;
            char[] chars = ps.chars;
            int attributeBaseEntityId = ps.entityId;
            int valueChunkStartPos = 0;
            LineInfo valueChunkLineInfo = new LineInfo( ps.lineNo, ps.LinePos );
            NodeData lastChunk = null;

            Debug.Assert( stringBuilder.Length == 0 );

            for (;;) {
                // parse the rest of the attribute value
                unsafe {
                    while ( ( ( xmlCharType.charProperties[chars[pos]] & XmlCharType.fAttrValue ) != 0 ) ) {
                        pos++;
                    }
                }

                if ( pos - ps.charPos > 0 ) {
                    stringBuilder.Append( chars, ps.charPos, pos - ps.charPos );
                    ps.charPos = pos;
                }

                if ( chars[pos] == quoteChar && attributeBaseEntityId == ps.entityId ) {
                    break;
                }
                else {
                    switch ( chars[pos] ) {
                        // eol
                        case (char)0xA:
                            pos++;
                            OnNewLine( pos );
                            if ( normalize ) {
                                stringBuilder.Append( (char)0x20 );  // CDATA normalization of 0xA
                                ps.charPos++;
                            }
                            continue;
                        case (char)0xD:
                            if ( chars[pos+1] == (char)0xA ) {
                                pos += 2;
                                if ( normalize ) {
                                    stringBuilder.Append( ps.eolNormalized ? "\u0020\u0020" : "\u0020" ); // CDATA normalization of 0xD 0xA
                                    ps.charPos = pos;
                                }
                            }
                            else if ( pos+1 < ps.charsUsed || ps.isEof ) { 
                                pos++;
                                if ( normalize ) {
                                    stringBuilder.Append( (char)0x20 );  // CDATA normalization of 0xD and 0xD 0xA
                                    ps.charPos = pos;
                                }
                            } 
                            else {
                                goto ReadData;
                            }
                            OnNewLine( pos );
                            continue;
                        // tab
                        case (char)0x9:
                            pos++;
                            if ( normalize ) {
                                stringBuilder.Append( (char)0x20 );  // CDATA normalization of 0x9
                                ps.charPos++;
                            }
                            continue;
                        case '"':
                        case '\'':
                        case '>':
                            pos++;
                            continue;
                        // attribute values cannot contain '<'
                        case '<':
                            Throw( pos, Res.Xml_BadAttributeChar, XmlException.BuildCharExceptionStr( '<' ) );
                            break;
                        // entity referece
                        case '&':
                            if ( pos - ps.charPos > 0 ) {
                                stringBuilder.Append( chars, ps.charPos, pos - ps.charPos );
                            }
                            ps.charPos = pos;

                            int enclosingEntityId = ps.entityId;
                            LineInfo entityLineInfo = new LineInfo( ps.lineNo, ps.LinePos + 1 );
                            switch ( HandleEntityReference( true, EntityExpandType.All, out pos ) ) {
                                case EntityType.CharacterDec:
                                case EntityType.CharacterHex:
                                case EntityType.CharacterNamed:
                                    break;
                                case EntityType.Unexpanded:
                                    if ( parsingMode == ParsingMode.Full && ps.entityId == attributeBaseEntityId ) {
                                        // construct text value chunk
                                        int valueChunkLen = stringBuilder.Length - valueChunkStartPos;
                                        if ( valueChunkLen > 0 ) {
                                            NodeData textChunk = new NodeData();
                                            textChunk.lineInfo = valueChunkLineInfo;
                                            textChunk.depth = attr.depth + 1;
                                            textChunk.SetValueNode( XmlNodeType.Text, stringBuilder.ToString( valueChunkStartPos, valueChunkLen ) );
                                            AddAttributeChunkToList( attr, textChunk, ref lastChunk );
                                        }

                                        // parse entity name
                                        ps.charPos++;
                                        string entityName = ParseEntityName();
                                             
                                        // construct entity reference chunk
                                        NodeData entityChunk = new NodeData();
                                        entityChunk.lineInfo = entityLineInfo;
                                        entityChunk.depth = attr.depth + 1;
                                        entityChunk.SetNamedNode( XmlNodeType.EntityReference, entityName );
                                        AddAttributeChunkToList( attr, entityChunk, ref lastChunk );

                                        // append entity ref to the attribute value
                                        stringBuilder.Append( '&' );
                                        stringBuilder.Append( entityName );
                                        stringBuilder.Append( ';' );

                                        // update info for the next attribute value chunk
                                        valueChunkStartPos = stringBuilder.Length;
                                        valueChunkLineInfo.Set( ps.LineNo, ps.LinePos );

                                        fullAttrCleanup = true;
                                    }
                                    else {
                                        ps.charPos++;
                                        ParseEntityName();
                                    }
                                    pos = ps.charPos;
                                    break;
                                case EntityType.ExpandedInAttribute:
                                    if ( parsingMode == ParsingMode.Full && enclosingEntityId == attributeBaseEntityId  ) {
                                        
                                        // construct text value chunk
                                        int valueChunkLen = stringBuilder.Length - valueChunkStartPos;
                                        if ( valueChunkLen > 0 ) {
                                            NodeData textChunk = new NodeData();
                                            textChunk.lineInfo = valueChunkLineInfo;
                                            textChunk.depth = attr.depth + 1;
                                            textChunk.SetValueNode( XmlNodeType.Text, stringBuilder.ToString( valueChunkStartPos, valueChunkLen ) );
                                            AddAttributeChunkToList( attr, textChunk, ref lastChunk );
                                        }

                                        // construct entity reference chunk
                                        NodeData entityChunk = new NodeData();
                                        entityChunk.lineInfo = entityLineInfo;
                                        entityChunk.depth = attr.depth + 1;
                                        entityChunk.SetNamedNode( XmlNodeType.EntityReference, ps.entity.Name.Name );
                                        AddAttributeChunkToList( attr, entityChunk, ref lastChunk );

                                        fullAttrCleanup = true;

                                        // Note: info for the next attribute value chunk will be updated once we
                                        // get out of the expanded entity
                                    }
                                    pos = ps.charPos;
                                    break;
                                default:
                                    pos = ps.charPos;
                                    break;
                            }
                            chars = ps.chars;
                            continue;
                        default:
                            // end of buffer
                            if ( pos == ps.charsUsed ) {
                                goto ReadData;
                            }
                            // surrogate chars
                            else { 
                                char ch = chars[pos];
                                if ( ch >= SurHighStart && ch <= SurHighEnd ) {
                                    if ( pos + 1 == ps.charsUsed ) {
                                        goto ReadData;
                                    }
                                    pos++;
                                    if ( chars[pos] >= SurLowStart && chars[pos] <= SurLowEnd ) {
                                        pos++;
                                        continue;
                                    }
                                }
                                ThrowInvalidChar( pos, ch );
                                break;
                            }
                    }
                }
            
            ReadData:
                // read new characters into the buffer
                if ( ReadData() == 0 ) {
                    if ( ps.charsUsed - ps.charPos > 0 ) {
                        if ( ps.chars[ps.charPos] != (char)0xD ) {
                            Debug.Assert( false, "We should never get to this point." );
                            Throw( Res.Xml_UnexpectedEOF1 );
                        }
                        Debug.Assert( ps.isEof );
                    }
                    else {
                        if ( !InEntity ) {
                            if ( fragmentType == XmlNodeType.Attribute ) {
                                if ( attributeBaseEntityId != ps.entityId ) {
                                    Throw( Res.Xml_EntityRefNesting );
                                }
                                break;
                            }
                            Throw( Res.Xml_UnclosedQuote );
                        }
                        if ( HandleEntityEnd( true ) ) { // no EndEntity reporting while parsing attributes
                            Debug.Assert( false );
                            Throw( Res.Xml_InternalError );
                        }
                        // update info for the next attribute value chunk
                        if ( attributeBaseEntityId == ps.entityId ) {
                            valueChunkStartPos = stringBuilder.Length;
                            valueChunkLineInfo.Set( ps.LineNo, ps.LinePos );
                        }
                    }
                }

                pos = ps.charPos;
                chars = ps.chars;
            }

            if ( attr.nextAttrValueChunk != null ) {
                // construct last text value chunk
                int valueChunkLen = stringBuilder.Length - valueChunkStartPos;
                if ( valueChunkLen > 0 ) {
                    NodeData textChunk = new NodeData();
                    textChunk.lineInfo = valueChunkLineInfo;
                    textChunk.depth = attr.depth + 1;
                    textChunk.SetValueNode( XmlNodeType.Text, stringBuilder.ToString( valueChunkStartPos, valueChunkLen ) );
                    AddAttributeChunkToList( attr, textChunk, ref lastChunk );
                }
            }

            ps.charPos = pos + 1;

            attr.SetValue( stringBuilder.ToString() );
            stringBuilder.Length = 0;
        }

        private void AddAttributeChunkToList( NodeData attr, NodeData chunk, ref NodeData lastChunk ) {
            if ( lastChunk == null ) {
                Debug.Assert( attr.nextAttrValueChunk == null );
                lastChunk = chunk;
                attr.nextAttrValueChunk = chunk;
            }
            else {
                lastChunk.nextAttrValueChunk = chunk;
                lastChunk = chunk;
            }
        }

        // Parses text or white space node.
        // Returns true if a node has been parsed and its data set to curNode. 
        // Returns false when a white space has been parsed and ignored (according to current whitespace handling) or when parsing mode is not Full.
        // Also returns false if there is no text to be parsed.
        private bool ParseText() {
            int startPos;
            int endPos;
            int orChars = 0;

            // skip over the text if not in full parsing mode
            if ( parsingMode != ParsingMode.Full ) {
                while ( !ParseText( out startPos, out endPos, ref orChars ) );
                goto IgnoredNode;
            }

            curNode.SetLineInfo( ps.LineNo, ps.LinePos ); 
            Debug.Assert( stringBuilder.Length == 0 );
            
            // the whole value is in buffer
            if ( ParseText( out startPos, out endPos, ref orChars ) ) {
                if ( endPos - startPos == 0 ) {
                    goto IgnoredNode;
                }
                XmlNodeType nodeType = GetTextNodeType( orChars );
                if ( nodeType == XmlNodeType.None ) {
                    goto IgnoredNode;
                }
                Debug.Assert( endPos - startPos > 0 );
                curNode.SetValueNode( nodeType, ps.chars, startPos, endPos - startPos );
                return true;
            }
            // only piece of the value was returned
            else {
                // V1 compatibility mode -> cache the whole value
                if ( v1Compat ) {
                    do {
                        stringBuilder.Append( ps.chars, startPos, endPos - startPos );
                    } while ( !ParseText( out startPos, out endPos, ref orChars ) );
                    stringBuilder.Append( ps.chars, startPos, endPos - startPos );

                    Debug.Assert( stringBuilder.Length > 0 );

                    XmlNodeType nodeType = GetTextNodeType( orChars );
                    if ( nodeType == XmlNodeType.None ) {
                        stringBuilder.Length = 0;
                        goto IgnoredNode;
                    }

                    curNode.SetValueNode( nodeType, stringBuilder.ToString() );
                    stringBuilder.Length = 0;
                    return true;
                }
                // V2 reader -> do not cache the whole value yet, read only up to 4kB to decide whether the value is a whitespace
                else {
                    bool fullValue = false;

                    // if it's a partial text value, not a whitespace -> return
                    if ( orChars > 0x20 ) {
                        Debug.Assert( endPos - startPos > 0 );
                        curNode.SetValueNode( XmlNodeType.Text, ps.chars, startPos, endPos - startPos );
                        nextParsingFunction = parsingFunction;
                        parsingFunction = ParsingFunction.PartialTextValue;
                        return true;
                    }

                    // partial whitespace -> read more data (up to 4kB) to decide if it is a whitespace or a text node
                    stringBuilder.Append( ps.chars, startPos, endPos - startPos );
                    do {
                        fullValue = ParseText( out startPos, out endPos, ref orChars );
                        stringBuilder.Append( ps.chars, startPos, endPos - startPos );
                    } while ( !fullValue && orChars <= 0x20 && stringBuilder.Length < MinWhitespaceLookahedCount );

                    // determine the value node type
                    XmlNodeType nodeType = ( stringBuilder.Length < MinWhitespaceLookahedCount ) ? GetTextNodeType( orChars ) : XmlNodeType.Text;
                    if ( nodeType == XmlNodeType.None ) {
                        // ignored whitespace -> skip over the rest of the value unless we already read it all
                        stringBuilder.Length = 0;
                        if ( !fullValue ) {
                            while ( !ParseText( out startPos, out endPos, ref orChars ) );
                        }
                        goto IgnoredNode;
                    }
                    // set value to curNode
                    curNode.SetValueNode( nodeType, stringBuilder.ToString() );
                    stringBuilder.Length = 0;

                    // change parsing state if the full value was not parsed
                    if ( !fullValue ) {
                        nextParsingFunction = parsingFunction;
                        parsingFunction = ParsingFunction.PartialTextValue;
                    }
                    return true;
                }
            }

        IgnoredNode:
            // ignored whitespace at the end of manually resolved entity
            if ( parsingFunction == ParsingFunction.ReportEndEntity ) {
                SetupEndEntityNodeInContent();
                parsingFunction = nextParsingFunction;
                return true;
            }
            else if ( parsingFunction == ParsingFunction.EntityReference ) {
                parsingFunction = nextNextParsingFunction;
                ParseEntityReference();
                return true;
            }
            return false;
        }

        // Parses a chunk of text starting at ps.charPos. 
        //   startPos .... start position of the text chunk that has been parsed (can differ from ps.charPos before the call)
        //   endPos ...... end position of the text chunk that has been parsed (can differ from ps.charPos after the call)
        //   ourOrChars .. all parsed character bigger or equal to 0x20 or-ed (|) into a single int. It can be used for whitespace detection 
        //                 (the text has a non-whitespace character if outOrChars > 0x20).
        // Returns true when the whole value has been parsed. Return false when it needs to be called again to get a next chunk of value.
        private bool ParseText( out int startPos, out int endPos, ref int outOrChars ) {
            char[] chars = ps.chars;
            int pos = ps.charPos;
            int rcount = 0;
            int rpos = -1;
            int orChars = outOrChars;
            char c;

            for (;;) {
                // parse text content
                unsafe {
                    while ( ( ( xmlCharType.charProperties[c = chars[pos]] & XmlCharType.fText ) != 0 ) ) {
                        orChars |= (int)c;
                        pos++;
                    }
                }

                switch ( c ) {
                    case (char)0x9:
                        pos++;
                        continue;
                    // eol
                    case (char)0xA:
                        pos++;
                        OnNewLine( pos );
                        continue;
                    case (char)0xD:
                        if ( chars[pos+1] == (char)0xA ) {
                            if ( !ps.eolNormalized && parsingMode == ParsingMode.Full ) {
                                if ( pos - ps.charPos > 0 ) {
                                    if ( rcount == 0 ) { 
                                        rcount = 1;
                                        rpos = pos;
                                    }
                                    else {
                                        ShiftBuffer( rpos + rcount, rpos, pos - rpos - rcount );
                                        rpos = pos - rcount;
                                        rcount++;
                                    }
                                }
                                else {
                                    ps.charPos++;
                                }
                            }
                            pos += 2;
                        }
                        else if ( pos + 1 < ps.charsUsed || ps.isEof ) {
                            if ( !ps.eolNormalized ) {
                                chars[pos] = (char)0xA;             // EOL normalization of 0xD
                            }
                            pos++;
                        }
                        else {
                            goto ReadData;
                        }
                        OnNewLine( pos );
                        continue;
                    // some tag 
                    case '<':
                        goto ReturnPartialValue;
                    // entity reference
                    case '&':
                        // try to parse char entity inline
                        int charRefEndPos, charCount;
                        EntityType entityType;
                        if ( ( charRefEndPos = ParseCharRefInline( pos, out charCount, out entityType ) ) > 0 ) {
                            if ( rcount > 0 ) {
                                ShiftBuffer( rpos + rcount, rpos, pos - rpos - rcount );
                            }
                            rpos = pos - rcount;
                            rcount += ( charRefEndPos - pos - charCount );
                            pos = charRefEndPos;

                            if ( !xmlCharType.IsWhiteSpace(chars[charRefEndPos-charCount]) ||
                                 ( v1Compat && entityType == EntityType.CharacterDec ) ) {
                                orChars |= 0xFF;
                            }
                        }
                        else {
                            if ( pos > ps.charPos ) {
                                goto ReturnPartialValue;
                            }
                            switch ( HandleEntityReference( false, EntityExpandType.All, out pos ) ) {
                                case EntityType.Unexpanded:
                                    // make sure we will report EntityReference after the text node
                                    nextParsingFunction = parsingFunction;
                                    parsingFunction = ParsingFunction.EntityReference;
                                    // end the value (returns nothing)
                                    goto NoValue;
                                case EntityType.CharacterDec:
                                    if ( !v1Compat ) {
                                        goto case EntityType.CharacterHex;
                                    }
                                    orChars |= 0xFF;
                                    break;
                                case EntityType.CharacterHex:
                                case EntityType.CharacterNamed:
                                    if ( !xmlCharType.IsWhiteSpace(ps.chars[pos-1]) ) {
                                        orChars |= 0xFF;
                                    }
                                    break;
                                default:
                                    pos = ps.charPos;
                                    break;
                            }
                            chars = ps.chars;
                        }
                        continue;
                    case ']':
                        if ( ps.charsUsed - pos < 3 && !ps.isEof ) {
                            goto ReadData;
                        }
                        if ( chars[pos+1] == ']' && chars[pos+2] == '>' ) {
                            Throw( pos, Res.Xml_CDATAEndInText );
                        }
                        orChars |= ']';
                        pos++;
                        continue;
                    default:
                        // end of buffer
                        if ( pos == ps.charsUsed ) {
                            goto ReadData;
                        }
                        // surrogate chars
                        else {
                            char ch = chars[pos];
                            if ( ch >= SurHighStart && ch <= SurHighEnd ) {
                                if ( pos + 1 == ps.charsUsed ) {
                                    goto ReadData;
                                }
                                pos++;
                                if ( chars[pos] >= SurLowStart && chars[pos] <= SurLowEnd ) {
                                    pos++;
                                    orChars |= ch;
                                    continue;
                                }
                            }
                            int offset = pos - ps.charPos;
                            if ( ZeroEndingStream( pos ) ) {
                                chars = ps.chars;
                                pos = ps.charPos + offset;
                                goto ReturnPartialValue;
                            }
                            else {
                                ThrowInvalidChar( ps.charPos + offset, ch );
                            }
                            break;
                        }
                }

            ReadData:
                if ( pos > ps.charPos ) {
                    goto ReturnPartialValue;
                }
                // read new characters into the buffer 
                if ( ReadData() == 0 ) {
                    if ( ps.charsUsed - ps.charPos > 0 ) {
                        if ( ps.chars[ps.charPos] != (char)0xD ) {
                            Debug.Assert( false, "We should never get to this point." );
                            Throw( Res.Xml_UnexpectedEOF1 );
                        }
                        Debug.Assert( ps.isEof );
                    }
                    else {
                        if ( !InEntity ) {
                            // end the value (returns nothing)
                            goto NoValue;
                        }
                        if ( HandleEntityEnd( true ) ) {
                            // report EndEntity after the text node
                            nextParsingFunction = parsingFunction;
                            parsingFunction = ParsingFunction.ReportEndEntity;
                            // end the value (returns nothing)
                            goto NoValue;
                        }
                    }
                }
                pos = ps.charPos;
                chars = ps.chars;
                continue;
            }
        NoValue:
            startPos = endPos = pos;
            return true;

        ReturnPartialValue:
            if ( parsingMode == ParsingMode.Full && rcount > 0 ) {
                ShiftBuffer( rpos + rcount, rpos, pos - rpos - rcount );
            }
            startPos = ps.charPos;
            endPos = pos - rcount;
            ps.charPos = pos;
            outOrChars = orChars;
            return c == '<';
        }

        // When in ParsingState.PartialTextValue, this method parses and caches the rest of the value and stores it in curNode.
        void FinishPartialValue() {
            Debug.Assert( stringBuilder.Length == 0 );
            Debug.Assert( parsingFunction == ParsingFunction.PartialTextValue ||
                          ( parsingFunction == ParsingFunction.InReadValueChunk && incReadState == IncrementalReadState.ReadValueChunk_OnPartialValue ) );

            curNode.CopyTo( readValueOffset, stringBuilder );

            int startPos;
            int endPos;
            int orChars = 0;
            while ( !ParseText( out startPos, out endPos, ref orChars ) ) {
                stringBuilder.Append( ps.chars, startPos, endPos - startPos );
            }
            stringBuilder.Append( ps.chars, startPos, endPos - startPos );

            Debug.Assert( stringBuilder.Length > 0 );
            curNode.SetValue( stringBuilder.ToString() );
            stringBuilder.Length = 0;
        }

        void FinishOtherValueIterator() {
            switch ( parsingFunction ) {
                case ParsingFunction.InReadAttributeValue:
                    // do nothing, correct value is already in curNode
                    break;
                case ParsingFunction.InReadValueChunk:
                    if ( incReadState == IncrementalReadState.ReadValueChunk_OnPartialValue ) {
                        FinishPartialValue();
                        incReadState = IncrementalReadState.ReadValueChunk_OnCachedValue;
                    }
                    else {
                        if ( readValueOffset > 0 ) {
                            curNode.SetValue( curNode.StringValue.Substring( readValueOffset ) );
                            readValueOffset = 0;
                        }
                    }
                    break;
                case ParsingFunction.InReadContentAsBinary:
                case ParsingFunction.InReadElementContentAsBinary:
                    switch ( incReadState ) {
                        case IncrementalReadState.ReadContentAsBinary_OnPartialValue:
                            FinishPartialValue();
                            incReadState = IncrementalReadState.ReadContentAsBinary_OnCachedValue;
                            break;
                        case IncrementalReadState.ReadContentAsBinary_OnCachedValue:
                            if ( readValueOffset > 0 ) {
                                curNode.SetValue( curNode.StringValue.Substring( readValueOffset ) );
                                readValueOffset = 0;
                            }
                            break;
                        case IncrementalReadState.ReadContentAsBinary_End:
                            curNode.SetValue( string.Empty );
                            break;
                    }
                    break;
            }
        }

        // When in ParsingState.PartialTextValue, this method skips over the rest of the partial value.
        void SkipPartialTextValue() {
            Debug.Assert( parsingFunction == ParsingFunction.PartialTextValue || parsingFunction == ParsingFunction.InReadValueChunk || 
                          parsingFunction == ParsingFunction.InReadContentAsBinary || parsingFunction == ParsingFunction.InReadElementContentAsBinary );
            int startPos;
            int endPos;
            int orChars = 0;

            parsingFunction = nextParsingFunction;
            while ( !ParseText( out startPos, out endPos, ref orChars ) );
        }

        void FinishReadValueChunk() {
            Debug.Assert( parsingFunction == ParsingFunction.InReadValueChunk );

            readValueOffset = 0;
            if ( incReadState == IncrementalReadState.ReadValueChunk_OnPartialValue ) {
                Debug.Assert( ( index > 0 ) ? nextParsingFunction == ParsingFunction.ElementContent : nextParsingFunction == ParsingFunction.DocumentContent );
                SkipPartialTextValue();
            }
            else {
                parsingFunction = nextParsingFunction;
                nextParsingFunction = nextNextParsingFunction;
            }
        }

        void FinishReadContentAsBinary() {
            Debug.Assert( parsingFunction == ParsingFunction.InReadContentAsBinary || parsingFunction == ParsingFunction.InReadElementContentAsBinary );

            readValueOffset = 0;
            if ( incReadState == IncrementalReadState.ReadContentAsBinary_OnPartialValue ) {
                Debug.Assert( ( index > 0 ) ? nextParsingFunction == ParsingFunction.ElementContent : nextParsingFunction == ParsingFunction.DocumentContent );
                SkipPartialTextValue();
            }
            else {
                parsingFunction = nextParsingFunction;
                nextParsingFunction = nextNextParsingFunction;
            }
            if ( incReadState != IncrementalReadState.ReadContentAsBinary_End ) {
                while ( MoveToNextContentNode( true ) );
            }
        }

        void FinishReadElementContentAsBinary() {
            FinishReadContentAsBinary();

            if ( curNode.type != XmlNodeType.EndElement ) {
                Throw( Res.Xml_InvalidNodeType, curNode.type.ToString() );
            }
            // move off the end element
            outerReader.Read();
        }

        private bool ParseRootLevelWhitespace() {
            Debug.Assert( stringBuilder.Length == 0 );

            XmlNodeType nodeType = GetWhitespaceType();

            if ( nodeType == XmlNodeType.None ) {
                EatWhitespaces( null );
                if ( ps.chars[ps.charPos] == '<' || ps.charsUsed - ps.charPos == 0 || ZeroEndingStream( ps.charPos ) ) {
                    return false;
                }
            }
            else {
                curNode.SetLineInfo( ps.LineNo, ps.LinePos ); 
                EatWhitespaces( stringBuilder );
                if ( ps.chars[ps.charPos] == '<' || ps.charsUsed - ps.charPos == 0 || ZeroEndingStream( ps.charPos ) ) {
                    if ( stringBuilder.Length > 0 ) {
                        curNode.SetValueNode( nodeType, stringBuilder.ToString() );
                        stringBuilder.Length = 0;
                        return true;
                    }
                    return false;
                }
            }

            if ( xmlCharType.IsCharData( ps.chars[ps.charPos] ) ) {
                Throw( Res.Xml_InvalidRootData );
            }
            else {
                ThrowInvalidChar( ps.charPos, ps.chars[ps.charPos] );
            }
            return false;
        }

        private void ParseEntityReference() {
            Debug.Assert( ps.chars[ps.charPos] == '&' );
            ps.charPos++;

            curNode.SetLineInfo( ps.LineNo, ps.LinePos );
            curNode.SetNamedNode( XmlNodeType.EntityReference, ParseEntityName() );
        }
        
        private EntityType HandleEntityReference( bool isInAttributeValue, EntityExpandType expandType, out int charRefEndPos ) {
            Debug.Assert( ps.chars[ps.charPos] == '&' );

            if ( ps.charPos + 1 == ps.charsUsed ) {
                if ( ReadData() == 0 ) {
                    Throw( Res.Xml_UnexpectedEOF1 );
                }
            }
            
            // numeric characters reference
            if ( ps.chars[ps.charPos+1] == '#' ) {
                EntityType entityType;
                charRefEndPos = ParseNumericCharRef( expandType != EntityExpandType.OnlyGeneral, null, out entityType );
                Debug.Assert( entityType == EntityType.CharacterDec || entityType == EntityType.CharacterHex );
                return entityType;
            }
            // named reference
            else {
                // named character reference
                charRefEndPos = ParseNamedCharRef( expandType != EntityExpandType.OnlyGeneral, null );
                if ( charRefEndPos >= 0 ) {
                    return EntityType.CharacterNamed;
                }

                // general entity reference
                if ( expandType != EntityExpandType.OnlyCharacter &&
                     ( entityHandling == EntityHandling.ExpandEntities ||
                       ( isInAttributeValue && validatingReaderCompatFlag ) ) ) { 
                    // NOTE: XmlValidatingReader compatibility mode: expand all entities in attribute values
                    // general entity reference
                    int endPos;

                    ps.charPos++;
                    int savedLinePos = ps.LinePos;
                    try {
                        endPos = ParseName();
                    }
                    catch ( XmlException ) {
                        Throw( Res.Xml_ErrorParsingEntityName, ps.LineNo, savedLinePos );
                        return EntityType.Skipped;
                    }

                    // check ';'
                    if ( ps.chars[endPos] != ';' ) {
                        ThrowUnexpectedToken( endPos, ";" );
                    }

                    int entityLinePos = ps.LinePos;
                    string entityName = nameTable.Add( ps.chars, ps.charPos, endPos - ps.charPos );
                    ps.charPos = endPos + 1;
                    charRefEndPos = -1;

                    EntityType entType = HandleGeneralEntityReference( entityName, isInAttributeValue, false, entityLinePos );
                    reportedBaseUri = ps.baseUriStr;
                    reportedEncoding = ps.encoding;
                    return entType;
                }
                else {
                    return EntityType.Unexpanded;
                }
            }
        }

        // returns true == continue parsing
        // return false == unexpanded external entity, stop parsing and return
        private EntityType HandleGeneralEntityReference( string name, bool isInAttributeValue, bool pushFakeEntityIfNullResolver, int entityStartLinePos ) {
            SchemaEntity entity = null;
            XmlQualifiedName qName = new XmlQualifiedName( name );

            if ( dtdParserProxy == null && fragmentParserContext != null && fragmentParserContext.HasDtdInfo && !prohibitDtd ) {
                ParseDtdFromParserContext();
            }

            if ( dtdParserProxy == null || 
                 ( ( entity = (SchemaEntity) dtdParserProxy.DtdSchemaInfo.GeneralEntities[qName] ) == null ) ) { 
                if ( disableUndeclaredEntityCheck ) {
                    entity = new SchemaEntity( new XmlQualifiedName( name ), false );
                    entity.Text = string.Empty;
                }
                else {
                    Throw( Res.Xml_UndeclaredEntity, name, ps.LineNo, entityStartLinePos );
                }
            }

            if ( entity.IsProcessed ) {
                Throw( Res.Xml_RecursiveGenEntity, name, ps.LineNo, entityStartLinePos );
            }

            if ( !entity.NData.IsEmpty ) {
                if ( disableUndeclaredEntityCheck ) {
                    entity = new SchemaEntity( new XmlQualifiedName( name ), false );
                    entity.Text = string.Empty;
                }
                else {
                    Throw( Res.Xml_UnparsedEntityRef, name, ps.LineNo, entityStartLinePos ); 
                }
            }

            if ( standalone && entity.DeclaredInExternal ) {
                Throw( Res.Xml_ExternalEntityInStandAloneDocument, entity.Name.Name, ps.LineNo, entityStartLinePos );
            }

            if ( entity.IsExternal ) {
                if ( isInAttributeValue ) {
                    Throw( Res.Xml_ExternalEntityInAttValue, name, ps.LineNo, entityStartLinePos );
                    return EntityType.Skipped;
                }

                if ( parsingMode == ParsingMode.SkipContent ) {
                    return EntityType.Skipped;
                }

                if ( xmlResolver == null ) {
                    if ( pushFakeEntityIfNullResolver ) {
                        PushExternalEntity( entity, ++nextEntityId );
                        curNode.entityId = ps.entityId;
                        return EntityType.FakeExpanded;
                    }
                    return EntityType.Skipped;
                }
                else {
                    PushExternalEntity( entity, ++nextEntityId );
                    curNode.entityId = ps.entityId;
                    return ( isInAttributeValue && validatingReaderCompatFlag ) ? EntityType.ExpandedInAttribute : EntityType.Expanded;
                }
            }
            else {
                if ( parsingMode == ParsingMode.SkipContent ) {
                    return EntityType.Skipped;
                }

                int entityId = nextEntityId++;
                PushInternalEntity( entity, entityId );

                curNode.entityId = entityId;
                return ( isInAttributeValue && validatingReaderCompatFlag ) ? EntityType.ExpandedInAttribute : EntityType.Expanded;
            }
        }

        private bool InEntity {
            get {
                return parsingStatesStackTop >= 0;
            }
        }

        // return true if EndEntity node should be reported. The entity is stored in lastEntity.
        private bool HandleEntityEnd( bool checkEntityNesting ) {
            if ( parsingStatesStackTop == -1 ) {
                Debug.Assert( false );
                Throw( Res.Xml_InternalError );
            }

            if ( ps.entityResolvedManually ) {
                index--;

                if ( checkEntityNesting ) {
                    if ( ps.entityId != nodes[index].entityId ) {
                        Throw( Res.Xml_IncompleteEntity );
                    }
                }

                lastEntity = ps.entity;  // save last entity for the EndEntity node

                PopEntity();
                curNode.entityId = ps.entityId;
                return true;
            }
            else {
                if ( checkEntityNesting ) {
                    if ( ps.entityId != nodes[index].entityId ) {
                        Throw( Res.Xml_IncompleteEntity );
                    }
                }

                PopEntity();
                curNode.entityId = ps.entityId;

                reportedEncoding = ps.encoding;
                reportedBaseUri = ps.baseUriStr;
                return false;
            }
        }

        private void SetupEndEntityNodeInContent() {
            Debug.Assert( lastEntity != null );

            reportedEncoding = ps.encoding;
            reportedBaseUri = ps.baseUriStr;

            curNode = nodes[index];
            Debug.Assert( curNode.depth == index );
            curNode.SetNamedNode( XmlNodeType.EndEntity, lastEntity.Name.Name );
            curNode.lineInfo.Set( ps.lineNo, ps.LinePos - 1 );

            if ( index == 0 && parsingFunction == ParsingFunction.ElementContent ) {
                parsingFunction = ParsingFunction.DocumentContent;
            }
        }

        private void SetupEndEntityNodeInAttribute() {
            curNode = nodes[index + attrCount + 1];
            Debug.Assert( curNode.type == XmlNodeType.EntityReference );
            Debug.Assert( Ref.Equal( lastEntity.Name.Name, curNode.localName ) );
            curNode.lineInfo.linePos += curNode.localName.Length;
            curNode.type = XmlNodeType.EndEntity;
        }

        private bool ParsePI() {
            return ParsePI( null );
        }

        // Parses processing instruction; if piInDtdStringBuilder != null, the processing instruction is in DTD and
        // it will be saved in the passed string builder (target, whitespace & value).
        private bool ParsePI( BufferBuilder piInDtdStringBuilder ) {
            if ( parsingMode == ParsingMode.Full ) {
                curNode.SetLineInfo( ps.LineNo, ps.LinePos );
            }

            Debug.Assert( stringBuilder.Length == 0 );

            // parse target name
            int nameEndPos = ParseName();
            string target = nameTable.Add( ps.chars, ps.charPos, nameEndPos - ps.charPos );

            if ( string.Compare( target, "xml", StringComparison.OrdinalIgnoreCase ) == 0 ) {
                Throw( target.Equals( "xml" ) ? Res.Xml_XmlDeclNotFirst : Res.Xml_InvalidPIName, target );
            }
            ps.charPos = nameEndPos;

            if ( piInDtdStringBuilder == null ) {
                if ( !ignorePIs && parsingMode == ParsingMode.Full ) {
                    curNode.SetNamedNode( XmlNodeType.ProcessingInstruction, target );
                }
            }
            else {
                piInDtdStringBuilder.Append( target );
            }

            // check mandatory whitespace
            char ch = ps.chars[ps.charPos];
            Debug.Assert( ps.charPos < ps.charsUsed );
            if ( EatWhitespaces( piInDtdStringBuilder ) == 0 ) {
                if ( ps.charsUsed - ps.charPos < 2 ) {
                    ReadData();
                }
                if ( ch != '?' || ps.chars[ps.charPos+1] != '>' ) {
                    Throw( Res.Xml_BadNameChar, XmlException.BuildCharExceptionStr( ch ) );
                }
            }

            // scan processing instruction value
            int startPos, endPos;
            if ( ParsePIValue( out startPos, out endPos ) ) {
                if ( piInDtdStringBuilder == null ) {
                    if ( ignorePIs ) {
                        return false;
                    }
                    if ( parsingMode == ParsingMode.Full ) {
                        curNode.SetValue( ps.chars, startPos, endPos - startPos );
                    }
                }
                else {
                    piInDtdStringBuilder.Append( ps.chars, startPos, endPos - startPos );
                }
            }
            else {
                BufferBuilder sb;
                if ( piInDtdStringBuilder == null ) {
                    if ( ignorePIs || parsingMode != ParsingMode.Full ) {
                        while ( !ParsePIValue( out startPos, out endPos ) );
                        return false;
                    }
                    sb = stringBuilder;
                    Debug.Assert( stringBuilder.Length == 0 );
                }
                else {
                    sb = piInDtdStringBuilder;
                }

                do {
                    sb.Append( ps.chars, startPos, endPos - startPos );
                } while ( !ParsePIValue( out startPos, out endPos ) );
                sb.Append( ps.chars, startPos, endPos - startPos );

                if ( piInDtdStringBuilder == null ) {
                    curNode.SetValue( stringBuilder.ToString() );
                    stringBuilder.Length = 0;
                }
            }
            return true;
        }

        private bool ParsePIValue( out int outStartPos, out int outEndPos ) {
            // read new characters into the buffer
            if ( ps.charsUsed - ps.charPos < 2 ) {
                if ( ReadData() == 0 ) {
                    Throw( ps.charsUsed, Res.Xml_UnexpectedEOF, "PI" );
                }
            }

            int pos = ps.charPos;
            char[] chars = ps.chars;
            int rcount = 0;
            int rpos = -1;
            for (;;) {
                unsafe {
                    while ( ( ( xmlCharType.charProperties[chars[pos]] & XmlCharType.fText ) != 0 ) &&
                        chars[pos] != '?' ) { 
                        pos++;
                    }
                }

                switch ( chars[pos] ) {
                    // possibly end of PI
                    case '?':
                        if ( chars[pos+1] == '>' ) {
                            if ( rcount > 0 ) {
                                Debug.Assert( !ps.eolNormalized );
                                ShiftBuffer( rpos + rcount, rpos, pos - rpos - rcount );
                                outEndPos = pos - rcount;
                            }
                            else {
                                outEndPos = pos;
                            }
                            outStartPos = ps.charPos;
                            ps.charPos = pos + 2;
                            return true;
                        }
                        else if ( pos+1 == ps.charsUsed ) {
                            goto ReturnPartial;
                        }
                        else {
                            pos++;
                            continue;
                        }
                    // eol
                    case (char)0xA:
                        pos++;
                        OnNewLine( pos );
                        continue;
                    case (char)0xD:
                        if ( chars[pos+1] == (char)0xA ) {
                            if ( !ps.eolNormalized && parsingMode == ParsingMode.Full ) {
                                // EOL normalization of 0xD 0xA
                                if ( pos - ps.charPos > 0 ) {
                                    if ( rcount == 0 ) { 
                                        rcount = 1;
                                        rpos = pos;
                                    }
                                    else {
                                        ShiftBuffer( rpos + rcount, rpos, pos - rpos - rcount );
                                        rpos = pos - rcount;
                                        rcount++;
                                    }
                                }
                                else {
                                    ps.charPos++;
                                }
                            }
                            pos += 2;
                        }
                        else if ( pos+1 < ps.charsUsed || ps.isEof ) {
                            if ( !ps.eolNormalized ) {
                                chars[pos] = (char)0xA;             // EOL normalization of 0xD
                            }
                            pos++;
                        }
                        else {
                            goto ReturnPartial;
                        }
                        OnNewLine( pos );
                        continue;
                    case '<':
                    case '&':
                    case ']':
                    case (char)0x9:
                        pos++;
                        continue;
                    default:
                        // end of buffer
                        if ( pos == ps.charsUsed ) {
                            goto ReturnPartial;
                        }
                        // surrogate characters
                        else {
                            char ch = chars[pos];
                            if ( ch >= SurHighStart && ch <= SurHighEnd ) {
                                if ( pos + 1 == ps.charsUsed ) {
                                    goto ReturnPartial;
                                }
                                pos++;
                                if ( chars[pos] >= SurLowStart && chars[pos] <= SurLowEnd ) {
                                    pos++;
                                    continue;
                                }
                            }
                            ThrowInvalidChar( pos, ch );
                            break;
                        }
                }
                
            }
            
        ReturnPartial:
            if ( rcount > 0 ) {
                ShiftBuffer( rpos + rcount, rpos, pos - rpos - rcount );
                outEndPos = pos - rcount;
            }
            else {
                outEndPos = pos;
            }
            outStartPos = ps.charPos;
            ps.charPos = pos;
            return false;
        }

        private bool ParseComment() {
            if ( ignoreComments ) {
                ParsingMode oldParsingMode = parsingMode;
                parsingMode = ParsingMode.SkipNode;
                ParseCDataOrComment( XmlNodeType.Comment );
                parsingMode = oldParsingMode;
                return false;
            }
            else {
                ParseCDataOrComment( XmlNodeType.Comment );
                return true;
            }
        }

        private void ParseCData() {
            ParseCDataOrComment( XmlNodeType.CDATA );
        }

        // Parses CDATA section or comment
        private void ParseCDataOrComment( XmlNodeType type ) {
            int startPos, endPos;

            if ( parsingMode == ParsingMode.Full ) {
                curNode.SetLineInfo( ps.LineNo, ps.LinePos );
                Debug.Assert( stringBuilder.Length == 0 );
                if ( ParseCDataOrComment( type, out startPos, out endPos ) ) {
                    curNode.SetValueNode( type, ps.chars, startPos, endPos - startPos );
                }
                else {
                    do {
                        stringBuilder.Append( ps.chars, startPos, endPos - startPos );
                    } while ( !ParseCDataOrComment( type, out startPos, out endPos ) );
                    stringBuilder.Append( ps.chars, startPos, endPos - startPos );
                    curNode.SetValueNode( type, stringBuilder.ToString() );
                    stringBuilder.Length = 0;
                }
            }
            else {
                while ( !ParseCDataOrComment( type, out startPos, out endPos ) ) ;
            }
        }

        // Parses a chunk of CDATA section or comment. Returns true when the end of CDATA or comment was reached.
        private bool ParseCDataOrComment( XmlNodeType type, out int outStartPos, out int outEndPos ) {
            if ( ps.charsUsed - ps.charPos < 3 ) {
                // read new characters into the buffer
                if ( ReadData() == 0 ) {
                    Throw( Res.Xml_UnexpectedEOF, ( type == XmlNodeType.Comment ) ? "Comment" : "CDATA" );
                }
            }

            int pos = ps.charPos;
            char[] chars = ps.chars;
            int rcount = 0;
            int rpos = -1;
            char stopChar = ( type == XmlNodeType.Comment ) ? '-' : ']';
            
            for (;;) {
                unsafe {
                    while ( ( ( xmlCharType.charProperties[chars[pos]] & XmlCharType.fText ) != 0 ) && 
                        chars[pos] != stopChar ) 
                        pos++;
                }

                // posibbly end of comment or cdata section
                if ( chars[pos] == stopChar ) {
                    if ( chars[pos+1] == stopChar ) {
                        if ( chars[pos+2] == '>' ) {
                            if ( rcount > 0 ) {
                                Debug.Assert( !ps.eolNormalized );
                                ShiftBuffer( rpos + rcount, rpos, pos - rpos - rcount );
                                outEndPos = pos - rcount;
                            }
                            else {
                                outEndPos = pos;
                            }
                            outStartPos = ps.charPos;
                            ps.charPos = pos + 3;
                            return true;
                        }
                        else if ( pos+2 == ps.charsUsed ) {
                            goto ReturnPartial;
                        }
                        else if ( type == XmlNodeType.Comment ) {
                            Throw( pos, Res.Xml_InvalidCommentChars );
                        }
                    }
                    else if ( pos+1 == ps.charsUsed ) {
                        goto ReturnPartial;
                    }
                    pos++;
                    continue;
                }
                else {
                    switch ( chars[pos] ) {
                    // eol
                    case (char)0xA:
                        pos++;
                        OnNewLine( pos );
                        continue;
                    case (char)0xD:
                        if ( chars[pos+1] == (char)0xA ) {
                            // EOL normalization of 0xD 0xA - shift the buffer
                            if ( !ps.eolNormalized && parsingMode == ParsingMode.Full ) {
                                if ( pos - ps.charPos > 0 ) {
                                    if ( rcount == 0 ) { 
                                        rcount = 1;
                                        rpos = pos;
                                    }
                                    else {
                                        ShiftBuffer( rpos + rcount, rpos, pos - rpos - rcount );
                                        rpos = pos - rcount;
                                        rcount++;
                                    }
                                }
                                else {
                                    ps.charPos++;
                                }
                            }
                            pos += 2;
                        }
                        else if ( pos+1 < ps.charsUsed || ps.isEof ) {
                            if ( !ps.eolNormalized ) {
                                chars[pos] = (char)0xA;             // EOL normalization of 0xD
                            }
                            pos++;
                        }
                        else {
                            goto ReturnPartial;
                        }
                        OnNewLine( pos );
                        continue;
                    case '<':
                    case '&':
                    case ']':
                    case (char)0x9:
                        pos++;
                        continue;
                    default:
                        // end of buffer
                        if ( pos == ps.charsUsed ) {
                            goto ReturnPartial;
                        }
                        // surrogate characters
                        char ch = chars[pos];
                        if ( ch >= SurHighStart && ch <= SurHighEnd ) {
                            if ( pos + 1 == ps.charsUsed ) {
                                goto ReturnPartial;
                            }
                            pos++;
                            if ( chars[pos] >= SurLowStart && chars[pos] <= SurLowEnd ) {
                                pos++;
                                continue;
                            }
                        }
                        ThrowInvalidChar( pos, ch );
                        break;
                    }
                }
            
            ReturnPartial:
                if ( rcount > 0 ) {
                    ShiftBuffer( rpos + rcount, rpos, pos - rpos - rcount );
                    outEndPos = pos - rcount;
                }
                else {
                    outEndPos = pos;
                }
                outStartPos = ps.charPos;

                ps.charPos = pos;
                return false; // false == parsing of comment or CData section is not finished yet, must be called again
            }
        }

        // Parses DOCTYPE declaration
        private void ParseDoctypeDecl() {
            if ( prohibitDtd ) {
                ThrowWithoutLineInfo( v1Compat ? Res.Xml_DtdIsProhibited : Res.Xml_DtdIsProhibitedEx, string.Empty );
            }

            // parse 'DOCTYPE'
            while ( ps.charsUsed - ps.charPos < 8 ) {
                if ( ReadData() == 0 ) {
                    Throw( Res.Xml_UnexpectedEOF, "DOCTYPE" );
                }
            }
            if ( !XmlConvert.StrEqual( ps.chars, ps.charPos, 7, "DOCTYPE" ) ) {
                ThrowUnexpectedToken( ( !rootElementParsed && dtdParserProxy == null ) ? "DOCTYPE" : "<!--" );
            }
            if ( !xmlCharType.IsWhiteSpace( ps.chars[ps.charPos + 7] ) ) {
                Throw( Res.Xml_ExpectingWhiteSpace, ParseUnexpectedToken( ps.charPos + 7 ) );
            }

            if ( dtdParserProxy != null ) {
                Throw( ps.charPos - 2, Res.Xml_MultipleDTDsProvided );  // position just before <!DOCTYPE
            }
            if ( rootElementParsed ) {
                Throw( ps.charPos - 2, Res.Xml_DtdAfterRootElement );
            }

            ps.charPos += 8;

            EatWhitespaces( null );
            curNode.SetLineInfo( ps.LineNo, ps.LinePos );

            // parse DTD
            dtdParserProxy = new DtdParserProxy( this );
            dtdParserProxy.Parse( true );

            SchemaInfo dtdSchemaInfo = dtdParserProxy.DtdSchemaInfo;

            if ( ( validatingReaderCompatFlag || !v1Compat ) && ( dtdSchemaInfo.HasDefaultAttributes || dtdSchemaInfo.HasNonCDataAttributes ) ) {
                addDefaultAttributesAndNormalize = true;
                qName = new XmlQualifiedName();
            }

            curNode.SetNamedNode( XmlNodeType.DocumentType, dtdSchemaInfo.DocTypeName.ToString() );
            curNode.SetValue( dtdParserProxy.InternalDtdSubset );

            nextParsingFunction = parsingFunction;
            parsingFunction = ParsingFunction.ResetAttributesRootLevel;
        }

        private int EatWhitespaces( BufferBuilder sb ) {
            int pos = ps.charPos;
            int wsCount = 0;
            char[] chars = ps.chars;

            for (;;) {
                for (;;) {
                    switch ( chars[pos] ) {
                        case (char)0xA:
                            pos++;
                            OnNewLine( pos );
                            continue;
                        case (char)0xD:
                            if ( chars[pos+1] == (char)0xA ) {
                                int tmp1 = pos - ps.charPos;
                                if ( sb != null && !ps.eolNormalized ) {
                                    if ( tmp1 > 0 ) {
                                        sb.Append( chars, ps.charPos, tmp1 );
                                        wsCount += tmp1;
                                    }
                                    ps.charPos = pos + 1;
                                }
                                pos += 2;
                            }
                            else if ( pos+1 < ps.charsUsed || ps.isEof ) {
                                if ( !ps.eolNormalized ) {
                                    chars[pos] = (char)0xA;             // EOL normalization of 0xD
                                }
                                pos++;
                            }
                            else {
                                goto ReadData;
                            }
                            OnNewLine( pos );
                            continue;
                        case (char)0x9:
                        case (char)0x20:
                            pos++;
                            continue;
                        default:
                            if ( pos == ps.charsUsed ) {
                                goto ReadData;
                            }
                            else {
                                int tmp2 = pos - ps.charPos;
                                if ( tmp2 > 0 ) {
                                    if ( sb != null  ) {
                                        sb.Append( ps.chars, ps.charPos, tmp2 );
                                    }
                                    ps.charPos = pos;
                                    wsCount += tmp2;
                                }
                                return wsCount;
                            }
                    }
                }

            ReadData:
                int tmp3 = pos - ps.charPos;
                if ( tmp3 > 0 ) {
                    if ( sb != null  ) {
                        sb.Append( ps.chars, ps.charPos, tmp3 );
                    }
                    ps.charPos = pos;
                    wsCount += tmp3;
                }

                if ( ReadData() == 0 ) {
                    if ( ps.charsUsed - ps.charPos == 0 ) {
                        return wsCount;
                    }
                    if ( ps.chars[ps.charPos] != (char)0xD ) {
                        Debug.Assert( false, "We should never get to this point." );
                        Throw( Res.Xml_UnexpectedEOF1 );
                    }
                    Debug.Assert( ps.isEof );
                }
                pos = ps.charPos;
                chars = ps.chars;
            }
        }

        private int ParseCharRefInline( int startPos, out int charCount, out EntityType entityType ) {
            Debug.Assert( ps.chars[startPos] == '&' );
            if ( ps.chars[startPos + 1] == '#' ) {
                return ParseNumericCharRefInline( startPos, true, null, out charCount, out entityType );
            }
            else {
                charCount = 1;
                entityType = EntityType.CharacterNamed;
                return ParseNamedCharRefInline( startPos, true, null );
            }
        }

        // Parses numeric character entity reference (e.g. &#32; &#x20;).
        //      - replaces the last one or two character of the entity reference (';' and the character before) with the referenced 
        //        character or surrogates pair (if expand == true)
        //      - returns position of the end of the character reference, that is of the character next to the original ';'
        //      - if (expand == true) then ps.charPos is changed to point to the replaced character
        private int ParseNumericCharRef( bool expand, BufferBuilder internalSubsetBuilder, out EntityType entityType ) {
            for (;;) {
                int newPos;
                int charCount;
                switch ( newPos = ParseNumericCharRefInline( ps.charPos, expand, internalSubsetBuilder, out charCount, out entityType ) ) {
                    case -2:
                        // read new characters in the buffer
                        if ( ReadData() == 0 ) {
                            Throw( Res.Xml_UnexpectedEOF );
                        }
                        Debug.Assert( ps.chars[ps.charPos] == '&' );
                        continue;
                    default:
                        if ( expand ) {
                            ps.charPos = newPos - charCount;
                        }
                        return newPos;
                }
            }
        }

        // Parses numeric character entity reference (e.g. &#32; &#x20;).
        // Returns -2 if more data is needed in the buffer
        // Otherwise 
        //      - replaces the last one or two character of the entity reference (';' and the character before) with the referenced 
        //        character or surrogates pair (if expand == true)
        //      - returns position of the end of the character reference, that is of the character next to the original ';'
        private int ParseNumericCharRefInline( int startPos, bool expand, BufferBuilder internalSubsetBuilder, out int charCount, out EntityType entityType ) {
            Debug.Assert( ps.chars[startPos] == '&' && ps.chars[startPos + 1] == '#' );

            int val;
            int pos;
            char[] chars;

            val = 0;
            string badDigitExceptionString = null;
            chars = ps.chars;
            pos = startPos + 2;
            charCount = 0;

            if ( chars[pos] == 'x' ) {
                pos++;
                badDigitExceptionString = Res.Xml_BadHexEntity;
                for (;;) {
                    char ch = chars[pos];
                    if ( ch >= '0' && ch <= '9' )
                        val = val * 16 + ch - '0';
                    else if ( ch >= 'a' && ch <= 'f' )
                        val = val * 16 + 10 + ch - 'a';
                    else if ( ch >= 'A' && ch <= 'F' )
                        val = val * 16 + 10 + ch - 'A';
                    else 
                        break;
                    pos++;
                }
                entityType = EntityType.CharacterHex;
            }
            else if ( pos < ps.charsUsed ) {
                badDigitExceptionString = Res.Xml_BadDecimalEntity;
                while ( chars[pos] >= '0' && chars[pos] <= '9' ) {
                    val = val * 10 + chars[pos] - '0';
                    pos++;
                }
                entityType = EntityType.CharacterDec;
            }
            else {
                // need more data in the buffer
                entityType = EntityType.Unexpanded;
                return -2;
            }
                
            if ( chars[pos] != ';' ) {
                if ( pos == ps.charsUsed ) {
                    // need more data in the buffer
                    return -2;
                }
                else {
                    Throw( pos, badDigitExceptionString );
                }
            }

            // simple character
            if ( val <= char.MaxValue ) {
                char ch = (char)val;
                if ( ( !xmlCharType.IsCharData(ch) || ( ch >= SurLowStart && ch <= 0xdeff ) ) &&
                     ( ( v1Compat && normalize ) || (!v1Compat && checkCharacters ) ) ) { 
                    ThrowInvalidChar( (ps.chars[ps.charPos + 2] == 'x') ? ps.charPos + 3 : ps.charPos + 2, ch );
                }

                if ( expand ) {
                    if ( internalSubsetBuilder != null ) {
                        internalSubsetBuilder.Append( ps.chars, ps.charPos, pos - ps.charPos + 1 );
                    }
                    chars[pos] = ch;
                }
                charCount = 1;
                return pos + 1;
            }
            // surrogate
            else {
                int v = val - 0x10000;
                int low = SurLowStart + v % 1024;
                int high = SurHighStart + v / 1024;

                if ( normalize ) {
                    char ch = (char)high;
                    if ( ch >= SurHighStart && ch <= SurHighEnd ) {
                        ch = (char)low;
                        if ( ch >=SurLowStart && ch <= SurLowEnd ) {
                            goto Return;
                        }
                    }
                    ThrowInvalidChar( (ps.chars[ps.charPos + 2] == 'x') ? ps.charPos + 3 : ps.charPos + 2, (char)val );
                }

            Return:
                Debug.Assert( pos > 0 );
                if ( expand ) {
                    if ( internalSubsetBuilder != null ) {
                        internalSubsetBuilder.Append( ps.chars, ps.charPos, pos - ps.charPos + 1 );
                    }
                    chars[pos-1] = (char)high;
                    chars[pos] = (char)low;
                }
                charCount = 2;
                return pos + 1;
            }
        }

        // Parses named character entity reference (&amp; &apos; &lt; &gt; &quot;).
        // Returns -1 if the reference is not a character entity reference.
        // Otherwise 
        //      - replaces the last character of the entity reference (';') with the referenced character (if expand == true)
        //      - returns position of the end of the character reference, that is of the character next to the original ';'
        //      - if (expand == true) then ps.charPos is changed to point to the replaced character
        private int ParseNamedCharRef( bool expand, BufferBuilder internalSubsetBuilder ) {
            for (;;) {
                int newPos;
                switch ( newPos = ParseNamedCharRefInline( ps.charPos, expand, internalSubsetBuilder ) ) {
                    case -1:
                        return -1;
                    case -2:
                        // read new characters in the buffer
                        if ( ReadData() == 0 ) {
                            return -1;
                        }
                        Debug.Assert( ps.chars[ps.charPos] == '&' );
                        continue;
                    default:
                        if ( expand ) {
                            ps.charPos = newPos - 1;
                        }
                        return newPos;
                }
            }
        }

        // Parses named character entity reference (&amp; &apos; &lt; &gt; &quot;).
        // Returns -1 if the reference is not a character entity reference.
        // Returns -2 if more data is needed in the buffer
        // Otherwise 
        //      - replaces the last character of the entity reference (';') with the referenced character (if expand == true)
        //      - returns position of the end of the character reference, that is of the character next to the original ';'
        private int ParseNamedCharRefInline( int startPos, bool expand, BufferBuilder internalSubsetBuilder ) {
            Debug.Assert( startPos < ps.charsUsed );
            Debug.Assert( ps.chars[startPos] == '&' );
            Debug.Assert( ps.chars[startPos + 1] != '#' );

            int pos = startPos + 1;
            char[] chars = ps.chars;
            char ch;

            switch ( chars[pos] ) {
                // &apos; or &amp; 
                case 'a':
                    pos++;
                    // &amp;
                    if ( chars[pos] == 'm' ) {
                        if ( ps.charsUsed - pos >= 3 ) {
                            if ( chars[pos+1] == 'p' && chars[pos+2] == ';' ) {
                                pos += 3;
                                ch = '&';
                                goto FoundCharRef;
                            }
                            else {
                                return -1;
                            }
                        }
                    }
                    // &apos;
                    else if ( chars[pos] == 'p' ) {
                        if ( ps.charsUsed - pos >= 4 ) {
                            if ( chars[pos+1] == 'o' && chars[pos+2] == 's' &&
                                    chars[pos+3] == ';' ) {
                                pos += 4;
                                ch = '\'';
                                goto FoundCharRef;
                            }
                            else {
                                return -1;
                            }
                        }
                    }
                    else if ( pos < ps.charsUsed ) {
                        return -1;
                    }
                    break;
                // &guot;
                case 'q':
                    if ( ps.charsUsed - pos >= 5 ) {
                        if ( chars[pos+1] == 'u' && chars[pos+2] == 'o' &&
                                chars[pos+3] == 't' && chars[pos+4] == ';' ) {
                            pos += 5;
                            ch = '"';
                            goto FoundCharRef;
                        }
                        else {
                            return -1;
                        }
                    }
                    break;
                // &lt;
                case 'l':
                    if ( ps.charsUsed - pos >= 3 ) {
                        if ( chars[pos+1] == 't' && chars[pos+2] == ';' ) {
                            pos += 3;
                            ch = '<';
                            goto FoundCharRef;
                        }
                        else {
                            return -1;
                        }
                    }
                    break;
                // &gt;
                case 'g':
                    if ( ps.charsUsed - pos >= 3 ) {
                        if ( chars[pos+1] == 't' && chars[pos+2] == ';' ) {
                            pos += 3;
                            ch = '>';
                            goto FoundCharRef;
                        }
                        else {
                            return -1;
                        }
                    }
                    break;
                default:
                    return -1;
            }

            // need more data in the buffer
            return -2;

        FoundCharRef:
            Debug.Assert( pos > 0 );
            if ( expand ) {
                if ( internalSubsetBuilder != null ) {
                    internalSubsetBuilder.Append( ps.chars, ps.charPos, pos - ps.charPos );
                }
                ps.chars[pos-1] = ch;
            }
            return pos;
        }

        private int ParseName() {
            int colonPos;
            return ParseQName( false, 0, out colonPos );
        }

        private int ParseQName( out int colonPos ) {
            return ParseQName( true, 0, out colonPos );
        }

        private int ParseQName( bool isQName, int startOffset, out int colonPos ) {
            int colonOffset = -1;
            int pos = ps.charPos + startOffset;

        ContinueStartName:
            char[] chars = ps.chars;

            // start name char
            unsafe {
                if ( !( ( xmlCharType.charProperties[chars[pos]] & XmlCharType.fNCStartName ) != 0 ) ) {
                    if ( pos == ps.charsUsed ) {
                        if ( ReadDataInName( ref pos ) ) {
                            goto ContinueStartName;
                        }
                        Throw( pos, Res.Xml_UnexpectedEOF, "Name" );
                    }
                    if ( chars[pos] != ':' || supportNamespaces ) {
                        Throw( pos, Res.Xml_BadStartNameChar, XmlException.BuildCharExceptionStr( chars[pos] ) );
                    }
                }
            }
            pos++;

        ContinueName:
            // parse name
            unsafe {
                while ( ( ( xmlCharType.charProperties[chars[pos]] & XmlCharType.fNCName ) != 0 ) ) {
                    pos++;
                }
            }

            // colon
            if ( chars[pos] == ':' ) {
                if ( ( colonOffset != -1 || !isQName ) && supportNamespaces ) {
                    Throw( pos, Res.Xml_BadNameChar, XmlException.BuildCharExceptionStr( ':' ));
                }
                colonOffset = pos - ps.charPos;
                pos++;
                goto ContinueStartName;
            }
            // end of buffer
            else if ( pos == ps.charsUsed ) {
                if ( ReadDataInName( ref pos ) ) {
                    chars = ps.chars;
                    goto ContinueName;
                }
                Throw( pos, Res.Xml_UnexpectedEOF, "Name" );
            }

            // end of name
            colonPos = ( colonOffset == -1 ) ? -1 : ps.charPos + colonOffset;
            return pos;
        }

        private bool ReadDataInName( ref int pos ) {
            int offset = pos - ps.charPos;
            bool newDataRead = ( ReadData() != 0 );
            pos = ps.charPos + offset;
            return newDataRead;
        }

        private string ParseEntityName() {
            int endPos;
            try {
                endPos = ParseName();
            }
            catch ( XmlException ) {
                Throw( Res.Xml_ErrorParsingEntityName );
                return null;
            }

            // check ';'
            if ( ps.chars[endPos] != ';' ) {
                Throw( Res.Xml_ErrorParsingEntityName );
            }

            string entityName = nameTable.Add( ps.chars, ps.charPos, endPos - ps.charPos );
            ps.charPos = endPos + 1;
            return entityName;
        }


        private NodeData AddNode( int nodeIndex, int nodeDepth ) {
            Debug.Assert( nodeIndex < nodes.Length );
            Debug.Assert( nodes[nodes.Length - 1] == null );

            NodeData n = nodes[nodeIndex];
            if ( n != null ) {
                n.depth = nodeDepth;
                return n;
            }
            return AllocNode( nodeIndex, nodeDepth );
        }

        private NodeData AllocNode( int nodeIndex, int nodeDepth ) {
            Debug.Assert( nodeIndex < nodes.Length );
            if ( nodeIndex >= nodes.Length - 1 ) {
                NodeData[] newNodes = new NodeData[nodes.Length * 2];
                Array.Copy( nodes, 0, newNodes, 0, nodes.Length );
                nodes = newNodes;
            }
            Debug.Assert( nodeIndex < nodes.Length );

            NodeData node = nodes[nodeIndex];
            if ( node == null ) {
                node = new NodeData();
                nodes[nodeIndex] = node;
            }
            node.depth = nodeDepth;
            return node;
        }

        private NodeData AddAttributeNoChecks( string name, int attrDepth ) {
            NodeData newAttr = AddNode( index + attrCount + 1, attrDepth );
            newAttr.SetNamedNode( XmlNodeType.Attribute, nameTable.Add( name ) );
            attrCount++;
            return newAttr;
        }

        private NodeData AddAttribute( int endNamePos, int colonPos ) {
            // setup attribute name
            if ( colonPos == -1 || !supportNamespaces ) {
                string localName = nameTable.Add( ps.chars, ps.charPos, endNamePos - ps.charPos );
                return AddAttribute( localName, string.Empty, localName );
            }
            else {
                attrNeedNamespaceLookup = true;
                int startPos = ps.charPos;
                int prefixLen = colonPos - startPos;
                if ( prefixLen == lastPrefix.Length && XmlConvert.StrEqual( ps.chars, startPos, prefixLen, lastPrefix ) ) {
                    return AddAttribute( nameTable.Add( ps.chars, colonPos + 1, endNamePos - colonPos - 1 ),
                                         lastPrefix,
                                         null );
                }
                else {
                    string prefix = nameTable.Add( ps.chars, startPos, prefixLen );
                    lastPrefix = prefix;
                    return AddAttribute( nameTable.Add( ps.chars, colonPos + 1, endNamePos - colonPos - 1 ),
                                         prefix,
                                         null );
                }
            }
        }

        private NodeData AddAttribute( string localName, string prefix, string nameWPrefix ) {
            NodeData newAttr = AddNode( index + attrCount + 1, index + 1 );
            
            // set attribute name
            newAttr.SetNamedNode( XmlNodeType.Attribute, localName, prefix, nameWPrefix );

            // pre-check attribute for duplicate: hash by first local name char
            int attrHash = 1 << ( localName[0] & 0x1F );
            if ( ( attrHashtable & attrHash ) == 0 ) {
                attrHashtable |= attrHash;
            }
            else {
                // there are probably 2 attributes beginning with the same letter -> check all previous 
                // attributes
                if ( attrDuplWalkCount < MaxAttrDuplWalkCount ) {
                    attrDuplWalkCount++;
                    for ( int i = index + 1; i < index + attrCount + 1; i++ ) {
                        NodeData attr = nodes[i];
                        Debug.Assert( attr.type == XmlNodeType.Attribute );
                        if ( Ref.Equal( attr.localName, newAttr.localName ) ) {
                            attrDuplWalkCount = MaxAttrDuplWalkCount;
                            break;
                        }
                    }
                }
            }

            attrCount++;
            return newAttr;        
        }

        private void PopElementContext() {
            // pop namespace context
            namespaceManager.PopScope();
            
            // pop xml context
            if ( curNode.xmlContextPushed ) {
                PopXmlContext();
            }
        }

        private void OnNewLine( int pos ) {
            ps.lineNo++;
            ps.lineStartPos = pos - 1;
        }

        private void OnEof() {
            Debug.Assert( ps.isEof );
            curNode = nodes[0];
            curNode.Clear( XmlNodeType.None );
            curNode.SetLineInfo( ps.LineNo, ps.LinePos );

            parsingFunction = ParsingFunction.Eof;
            readState = ReadState.EndOfFile;
            
            reportedEncoding = null;
        }

        private string LookupNamespace( NodeData node ) {
            string ns = namespaceManager.LookupNamespace( node.prefix );
            if ( ns != null ) {
                return ns;
            }
            else {
                Throw( Res.Xml_UnknownNs, node.prefix, node.LineNo, node.LinePos );
                return null;
            }
        }

        private void AddNamespace( string prefix, string uri, NodeData attr ) {
            if ( uri == XmlReservedNs.NsXmlNs ) {
                if ( Ref.Equal( prefix, XmlNs ) ) {
                    Throw( Res.Xml_XmlnsPrefix, (int)attr.lineInfo2.lineNo, (int)attr.lineInfo2.linePos );
                }
                else {
                    Throw( Res.Xml_NamespaceDeclXmlXmlns, prefix, (int)attr.lineInfo2.lineNo, (int)attr.lineInfo2.linePos );
                }
            }
            else if ( uri == XmlReservedNs.NsXml ) {
                if ( !Ref.Equal( prefix, Xml ) && !v1Compat ) {
                    Throw( Res.Xml_NamespaceDeclXmlXmlns, prefix, (int)attr.lineInfo2.lineNo, (int)attr.lineInfo2.linePos );
                }
            }
            if ( uri.Length == 0 && prefix.Length > 0 ) {
                Throw( Res.Xml_BadNamespaceDecl, (int)attr.lineInfo.lineNo, (int)attr.lineInfo.linePos );
            }
    
            try {
                namespaceManager.AddNamespace( prefix, uri );
            }
            catch ( ArgumentException e ) { 
                Debug.Assert( e.ParamName == null, "Expecting ArgumentException with message Xml_XmlPrefix or Xml_XmlnsPrefix." );
                ReThrow( e, (int)attr.lineInfo.lineNo, (int)attr.lineInfo.linePos );
            }
#if DEBUG
            if ( prefix.Length == 0 ) {
                Debug.Assert( xmlContext.defaultNamespace == uri );
            }
#endif
        }

        private void ResetAttributes() {
            if ( fullAttrCleanup ) {
                FullAttributeCleanup(); 
            }
            curAttrIndex = -1;
            attrCount = 0;
            attrHashtable = 0;
            attrDuplWalkCount = 0;
        }

        private void FullAttributeCleanup() {
            for ( int i = index + 1; i < index + attrCount + 1; i++ ) {
                NodeData attr = nodes[i];
                attr.nextAttrValueChunk = null;
                attr.IsDefaultAttribute = false;
            }
            fullAttrCleanup = false;
        }

        private void PushXmlContext() {
            xmlContext = new XmlContext( xmlContext );
            curNode.xmlContextPushed = true;
        }

        private void PopXmlContext() {
            Debug.Assert( curNode.xmlContextPushed );
            xmlContext = xmlContext.previousContext;
            curNode.xmlContextPushed = false;
        }

        // Returns the whitespace node type according to the current whitespaceHandling setting and xml:space
        private XmlNodeType GetWhitespaceType() {
            if ( whitespaceHandling != WhitespaceHandling.None ) {
                if ( xmlContext.xmlSpace == XmlSpace.Preserve ) {
                    return XmlNodeType.SignificantWhitespace;
                }
                if ( whitespaceHandling == WhitespaceHandling.All ) {
                    return XmlNodeType.Whitespace;
                }
            }
            return XmlNodeType.None;
        }    

        private XmlNodeType GetTextNodeType( int orChars ) {
            if ( orChars > 0x20 ) {
                return XmlNodeType.Text;
            }
            else {
                return GetWhitespaceType();
            }
        }

        // returns true if real entity has been pushed, false if fake entity (=empty content entity)
        private bool PushExternalEntity( SchemaEntity entity, int entityId ) {
            Debug.Assert( entity.IsExternal );

            if ( xmlResolver != null ) {

                Uri entityBaseUri = ( entity.BaseURI.Length > 0 ) ? xmlResolver.ResolveUri( null, entity.BaseURI ): null;
                Uri entityUri = xmlResolver.ResolveUri( entityBaseUri, entity.Url );
                Stream stream = null;
                try {
                    stream = OpenStream( entityUri );
                }
                catch ( Exception e ) {
                    if ( v1Compat ) {
                        throw;
                    }
                    Throw( new XmlException( Res.Xml_ErrorOpeningExternalEntity, new string[] { entityUri.ToString(), e.Message }, e, 0, 0 ) );
                }

                if ( stream == null ) {
                    Throw( Res.Xml_CannotResolveEntity, entity.Name.Name );
                }

                PushParsingState();

                if ( v1Compat ) {
                    InitStreamInput( entityUri, stream, null );
                }
                else {
                    InitStreamInput( entityUri, stream, null );
                }

                ps.entity = entity;
                ps.entityId = entityId;
                entity.IsProcessed = true;

                Debug.Assert( ps.appendMode );
                int initialPos = ps.charPos;
                if ( v1Compat ) {
                    EatWhitespaces( null );
                }
                if ( !ParseXmlDeclaration( true ) ) {
                    ps.charPos = initialPos;
                }
                return true;
            }
            else {
                Encoding enc = ps.encoding;

                PushParsingState();
                InitStringInput( entity.Url, enc, string.Empty );

                ps.entity = entity;
                ps.entityId = entityId;

                return false;
            }
        }

        private void PushInternalEntity( SchemaEntity entity, int entityId ) {
            Debug.Assert( !entity.IsExternal );

            Encoding enc = ps.encoding;

            PushParsingState();

            InitStringInput( ( entity.DeclaredURI != null ) ? entity.DeclaredURI : string.Empty, enc, entity.Text );

            ps.entity = entity;
            ps.entityId = entityId;
            ps.lineNo = entity.Line;
            ps.lineStartPos = - entity.Pos - 1;

            ps.eolNormalized = true;
            entity.IsProcessed = true;
        }

        private void PopEntity() {
            if ( ps.entity != null ) {
                ps.entity.IsProcessed = false;
            }
            if ( ps.stream != null ) {
                ps.stream.Close();
            }
            PopParsingState();
            curNode.entityId = ps.entityId;
        }

        private void PushParsingState() {
            if ( parsingStatesStack == null ) {
                parsingStatesStack = new ParsingState[ InitialParsingStatesDepth ];
                Debug.Assert( parsingStatesStackTop == -1 );
            }
            else if ( parsingStatesStackTop + 1 == parsingStatesStack.Length ) {
                ParsingState[] newParsingStateStack = new ParsingState[ parsingStatesStack.Length * 2 ];
                Array.Copy( parsingStatesStack, 0, newParsingStateStack, 0, parsingStatesStack.Length );
                parsingStatesStack = newParsingStateStack;
            }
            parsingStatesStackTop++;
            parsingStatesStack[ parsingStatesStackTop ] = ps;
            
            ps.Clear();
        }

        private void PopParsingState() {
            Debug.Assert( parsingStatesStackTop >= 0 );
            ps.Close( true );
            ps = parsingStatesStack[ parsingStatesStackTop-- ];
        }

        private void InitIncrementalRead( IncrementalReadDecoder decoder ) {
            ResetAttributes();

            decoder.Reset();
            incReadDecoder = decoder;
            incReadState = IncrementalReadState.Text;
            incReadDepth = 1;
            incReadLeftStartPos = ps.charPos;
            incReadLineInfo.Set( ps.LineNo, ps.LinePos );

            parsingFunction = ParsingFunction.InIncrementalRead;
        }

        private int IncrementalRead( Array array, int index, int count ) {
            if ( array == null ) {
                throw new ArgumentNullException( ( incReadDecoder is IncrementalReadCharsDecoder ) ? "buffer" : "array" );
            }
            if ( count < 0 ) {
                throw new ArgumentOutOfRangeException( ( incReadDecoder is IncrementalReadCharsDecoder ) ? "count" : "len" );
            }
            if ( index < 0 ) {
                throw new ArgumentOutOfRangeException( ( incReadDecoder is IncrementalReadCharsDecoder ) ? "index" : "offset" );
            }
            if ( array.Length - index < count ) {
                throw new ArgumentException( ( incReadDecoder is IncrementalReadCharsDecoder ) ? "count" : "len" );
            }

            if ( count == 0 ) {
                return 0;
            }

            curNode.lineInfo = incReadLineInfo;

            incReadDecoder.SetNextOutputBuffer( array, index, count );
            IncrementalRead();
            return incReadDecoder.DecodedCount;
        }

        private int IncrementalRead() {
            int charsDecoded = 0;

        OuterContinue:
            int charsLeft = incReadLeftEndPos - incReadLeftStartPos;
            if ( charsLeft > 0 ) {
                int count;
                try {
                    count = incReadDecoder.Decode( ps.chars, incReadLeftStartPos, charsLeft );
                }
                catch ( XmlException e ) {
                    ReThrow( e, (int)incReadLineInfo.lineNo, (int)incReadLineInfo.linePos );
                    return 0;
                }
                if ( count < charsLeft ) {
                    incReadLeftStartPos += count;
                    incReadLineInfo.linePos += count; // we have never more then 1 line cached
                    return count;
                }
                else {
                    incReadLeftStartPos = 0;
                    incReadLeftEndPos = 0;
                    incReadLineInfo.linePos += count;
                    if ( incReadDecoder.IsFull ) {
                        return count;
                    }
                }
            }

            int startPos = 0;
            int pos = 0;

            for (;;) {

                switch ( incReadState ) {
                    case IncrementalReadState.Text:
                    case IncrementalReadState.Attributes:
                    case IncrementalReadState.AttributeValue:
                        break;
                    case IncrementalReadState.PI:
                        if ( ParsePIValue( out startPos, out pos ) ) {
                            Debug.Assert( XmlConvert.StrEqual( ps.chars, ps.charPos - 2, 2, "?>" ) );
                            ps.charPos -= 2;
                            incReadState = IncrementalReadState.Text;
                        }
                        goto Append;
                    case IncrementalReadState.Comment:
                        if ( ParseCDataOrComment( XmlNodeType.Comment, out startPos, out pos ) ) {
                            Debug.Assert( XmlConvert.StrEqual( ps.chars, ps.charPos - 3, 3, "-->" ) );
                            ps.charPos -= 3;
                            incReadState = IncrementalReadState.Text;
                        }
                        goto Append;
                    case IncrementalReadState.CDATA:
                        if ( ParseCDataOrComment( XmlNodeType.CDATA, out startPos, out pos ) ) {
                            Debug.Assert( XmlConvert.StrEqual( ps.chars, ps.charPos - 3, 3, "]]>" ) );
                            ps.charPos -= 3;
                            incReadState = IncrementalReadState.Text;
                        }
                        goto Append;
                    case IncrementalReadState.EndElement:
                        parsingFunction = ParsingFunction.PopElementContext;
                        nextParsingFunction = ( index > 0 || fragmentType != XmlNodeType.Document ) ? ParsingFunction.ElementContent
																								    : ParsingFunction.DocumentContent;
                        outerReader.Read();
                        incReadState = IncrementalReadState.End;
                        goto case IncrementalReadState.End;
                    case IncrementalReadState.End:
                        return charsDecoded;
                    case IncrementalReadState.ReadData:
                        if ( ReadData() == 0 ) {
                            ThrowUnclosedElements();
                        }
                        incReadState = IncrementalReadState.Text;
                        startPos = ps.charPos;
                        pos = startPos;
                        break;
                    default:
                        Debug.Assert( false );
                        break;
                }
                Debug.Assert( incReadState == IncrementalReadState.Text || 
                              incReadState == IncrementalReadState.Attributes ||
                              incReadState == IncrementalReadState.AttributeValue );

                char[] chars = ps.chars;
                startPos = ps.charPos;
                pos = startPos;

                for (;;) {
                    incReadLineInfo.Set( ps.LineNo, ps.LinePos );

                    char c;
                    unsafe {
                        if ( incReadState == IncrementalReadState.Attributes ) {
                            while ( ( ( xmlCharType.charProperties[c = chars[pos]] & XmlCharType.fAttrValue ) != 0 ) && c != '/' ) {
                                pos++;
                            }
                        }
                        else {
                            while ( ( ( xmlCharType.charProperties[c = chars[pos]] & XmlCharType.fAttrValue ) != 0 ) ) {
                                pos++;
                            }
                        }
                    }

                    if ( chars[pos] == '&' || chars[pos] == (char)0x9 ) {
                        pos++;
                        continue;
                    }

                    if ( pos - startPos > 0 ) {
                        goto AppendAndUpdateCharPos;
                    }

                    switch ( chars[pos] ) {
                        // eol
                        case (char)0xA:
                            pos++;
                            OnNewLine( pos );
                            continue;
                        case (char)0xD:
                            if ( chars[pos+1] == (char)0xA ) {
                                pos += 2;
                            }
                            else if ( pos+1 < ps.charsUsed ) {
                                pos++;
                            }
                            else {
                                goto ReadData;
                            }
                            OnNewLine( pos );
                            continue;
                        // some tag 
                        case '<':
                            if ( incReadState != IncrementalReadState.Text ) {
                                pos++;
                                continue;
                            }
                            if ( ps.charsUsed - pos < 2 ) {
                                goto ReadData;
                            }
                            switch ( chars[pos+1] ) {
                                // pi
                                case '?':
                                    pos += 2;
                                    incReadState = IncrementalReadState.PI;
                                    goto AppendAndUpdateCharPos;
                                // comment
                                case '!':
                                    if ( ps.charsUsed - pos < 4 ) {
                                        goto ReadData;
                                    }
                                    if ( chars[pos+2] == '-' && chars[pos+3] == '-' ) {
                                        pos += 4;
                                        incReadState = IncrementalReadState.Comment;
                                        goto AppendAndUpdateCharPos;
                                    }
                                    if ( ps.charsUsed - pos < 9 ) {
                                        goto ReadData;
                                    }
                                    if ( XmlConvert.StrEqual( chars, pos + 2, 7, "[CDATA[" ) ) {
                                        pos += 9;
                                        incReadState = IncrementalReadState.CDATA;
                                        goto AppendAndUpdateCharPos;
                                    }
                                    else {
                                        ;//Throw( );
                                    }
                                    break;
                                // end tag
                                case '/':
                                {
                                    Debug.Assert( ps.charPos - pos == 0 );
                                    int colonPos;
                                    int endPos = ParseQName( true, 2, out colonPos );
                                    if ( XmlConvert.StrEqual( chars, ps.charPos + 2, endPos - ps.charPos - 2, curNode.GetNameWPrefix( nameTable ) ) &&
                                        ( ps.chars[endPos] == '>' || xmlCharType.IsWhiteSpace( ps.chars[endPos] ) ) ) {

                                        if ( --incReadDepth > 0 ) {
                                            pos = endPos + 1;
                                            continue;
                                        }

                                        ps.charPos = endPos;
                                        if ( xmlCharType.IsWhiteSpace( ps.chars[endPos] ) ) {
                                            EatWhitespaces( null );
                                        }
                                        if ( ps.chars[ps.charPos] != '>' ) {
                                            ThrowUnexpectedToken( ">" );
                                        }
                                        ps.charPos++;

                                        incReadState = IncrementalReadState.EndElement;
                                        goto OuterContinue;
                                    }
                                    else {
                                        pos = endPos;
                                        continue;
                                    }
                                }
                                // start tag
                                default:
                                {
                                    Debug.Assert( ps.charPos - pos == 0 );
                                    int colonPos;
                                    int endPos = ParseQName( true, 1, out colonPos );
                                    if ( XmlConvert.StrEqual( ps.chars, ps.charPos + 1, endPos - ps.charPos - 1, curNode.localName ) &&
                                        ( ps.chars[endPos] == '>' || ps.chars[endPos] == '/' || xmlCharType.IsWhiteSpace( ps.chars[endPos] ) ) ) {
                                        incReadDepth++;
                                        incReadState = IncrementalReadState.Attributes;
                                        pos = endPos;
                                        goto AppendAndUpdateCharPos;
                                    }
                                    pos = endPos;
                                    startPos = ps.charPos;
                                    chars = ps.chars;
                                    continue;
                                }
                            }
                            break;
                        // end of start tag
                        case '/':
                            if ( incReadState == IncrementalReadState.Attributes ) {
                                if ( ps.charsUsed - pos < 2 ) {
                                    goto ReadData;
                                }
                                if ( chars[pos+1] == '>' ) {
                                    incReadState = IncrementalReadState.Text;
                                    incReadDepth--;
                                }
                            }
                            pos++;
                            continue;
                        // end of start tag
                        case '>':
                            if ( incReadState == IncrementalReadState.Attributes ) {
                                incReadState = IncrementalReadState.Text;
                            }
                            pos++;
                            continue;
                        case '"':
                        case '\'':
                            switch ( incReadState ) {
                                case IncrementalReadState.AttributeValue:
                                    if ( chars[pos] == curNode.quoteChar ) {
                                        incReadState = IncrementalReadState.Attributes;
                                    }
                                    break;
                                case IncrementalReadState.Attributes:
                                    curNode.quoteChar = chars[pos];
                                    incReadState = IncrementalReadState.AttributeValue;
                                    break;
                            }
                            pos++;
                            continue;
                        default:
                            // end of buffer
                            if ( pos == ps.charsUsed ) {
                                goto ReadData;
                            }
                            // surrogate chars or invalid chars are ignored
                            else {
                                pos++;
                                continue;
                            }
                    }
                }

            ReadData:
                incReadState = IncrementalReadState.ReadData;

            AppendAndUpdateCharPos:
                ps.charPos = pos;

            Append:
                // decode characters
                int charsParsed = pos - startPos;
                if ( charsParsed > 0 ) {
                    int count;
                    try {
                        count = incReadDecoder.Decode( ps.chars, startPos, charsParsed );
                    }
                    catch ( XmlException e ) {
                        ReThrow( e, (int)incReadLineInfo.lineNo, (int)incReadLineInfo.linePos );
                        return 0;
                    }
                    Debug.Assert( count == charsParsed || incReadDecoder.IsFull, "Check if decoded consumed all characters unless it's full." );
                    charsDecoded += count;
                    if ( incReadDecoder.IsFull ) {
                        incReadLeftStartPos = startPos + count;
                        incReadLeftEndPos = pos;
                        incReadLineInfo.linePos += count; // we have never more than 1 line cached
                        return charsDecoded;
                    }
                }
            }
        }

        private void FinishIncrementalRead() {
            incReadDecoder = new IncrementalReadDummyDecoder();
            IncrementalRead();
            Debug.Assert( IncrementalRead() == 0, "Previous call of IncrementalRead should eat up all characters!" );
            incReadDecoder = null;
        }

        private bool ParseFragmentAttribute() {
            Debug.Assert( fragmentType == XmlNodeType.Attribute );

            // if first call then parse the whole attribute value
            if ( curNode.type == XmlNodeType.None ) {
                curNode.type = XmlNodeType.Attribute;
                curAttrIndex = 0;
                ParseAttributeValueSlow( ps.charPos, '"', curNode );
            }
            else {
                parsingFunction = ParsingFunction.InReadAttributeValue;
            }

            // return attribute value chunk
            if ( ReadAttributeValue() ) {
                Debug.Assert( parsingFunction == ParsingFunction.InReadAttributeValue );
                parsingFunction = ParsingFunction.FragmentAttribute;
                return true;
            }
            else {
                OnEof();
                return false;
            }
        }

        private bool ParseAttributeValueChunk() {
            char[] chars = ps.chars;
            int pos  = ps.charPos;

            curNode = AddNode( index + attrCount + 1, index + 2 );
            curNode.SetLineInfo( ps.LineNo, ps.LinePos );

            if ( emptyEntityInAttributeResolved ) {
                curNode.SetValueNode( XmlNodeType.Text, string.Empty );
                emptyEntityInAttributeResolved = false;
                return true;
            }

            Debug.Assert( stringBuilder.Length == 0 );

            for (;;) {
                unsafe {
                    while ( ( ( xmlCharType.charProperties[chars[pos]] & XmlCharType.fAttrValue ) != 0 ) ) 
                        pos++;
                }

                switch ( chars[pos] ) {
                    // eol D
                    case (char)0xD:
                        Debug.Assert( ps.eolNormalized, "Entity replacement text for attribute values should be EOL-normalized!" );
                        pos++;
                        continue;
                    // eol A, tab
                    case (char)0xA:
                    case (char)0x9:
                        if ( normalize ) {
                            chars[pos] = (char)0x20;  // CDATA normalization of 0xA and 0x9
                        }
                        pos++;
                        continue;
                    case '"':
                    case '\'':
                    case '>':
                        pos++;
                        continue;
                    // attribute values cannot contain '<'
                    case '<':
                        Throw( pos, Res.Xml_BadAttributeChar, XmlException.BuildCharExceptionStr( '<' ) );
                        break;
                    // entity reference
                    case '&':
                        if ( pos - ps.charPos > 0 ) {
                            stringBuilder.Append( chars, ps.charPos, pos - ps.charPos );
                        }
                        ps.charPos = pos;

                        // expand char entities but not general entities 
                        switch ( HandleEntityReference( true, EntityExpandType.OnlyCharacter, out pos ) ) {
                            case EntityType.CharacterDec:
                            case EntityType.CharacterHex:
                            case EntityType.CharacterNamed:
                                chars = ps.chars;                            
                                if ( normalize && xmlCharType.IsWhiteSpace( chars[ps.charPos] ) && pos - ps.charPos == 1 ) {
                                    chars[ps.charPos] = (char)0x20;  // CDATA normalization of character references in entities
                                }
                                break;
                            case EntityType.Unexpanded:
                                if ( stringBuilder.Length == 0 ) {
                                    curNode.lineInfo.linePos++;
                                    ps.charPos++;
                                    curNode.SetNamedNode( XmlNodeType.EntityReference, ParseEntityName() );
                                    return true;
                                }
                                else {
                                    goto ReturnText;
                                }
                            default:
                                Debug.Assert( false, "We should never get to this point." );
                                break;
                        }
                        chars = ps.chars;
                        continue;
                    default:
                        // end of buffer
                        if ( pos == ps.charsUsed ) {
                            goto ReadData;
                        }
                        // surrogate chars
                        else { 
                            char ch = chars[pos];
                            if ( ch >= SurHighStart && ch <= SurHighEnd ) {
                                if ( pos + 1 == ps.charsUsed ) {
                                    goto ReadData;
                                }
                                pos++;
                                if ( chars[pos] >= SurLowStart && chars[pos] <= SurLowEnd ) {
                                    pos++;
                                    continue;
                                }
                            }
                            ThrowInvalidChar( pos, ch );
                            break;
                        }
                }
            
            ReadData:
                if ( pos - ps.charPos > 0 ) {
                    stringBuilder.Append( chars, ps.charPos, pos - ps.charPos );
                    ps.charPos = pos;
                }
                // read new characters into the buffer
                if ( ReadData() == 0 ) {
                    if ( stringBuilder.Length > 0 ) {
                        goto ReturnText;
                    }
                    else {
                        if ( HandleEntityEnd( false ) ) {
                            SetupEndEntityNodeInAttribute();
                            return true;
                        }
                        else {
                            Debug.Assert( false, "We should never get to this point." );
                        }
                    }
                }

                pos = ps.charPos;
                chars = ps.chars;
            }

        ReturnText:
            if ( pos - ps.charPos > 0 ) {
                stringBuilder.Append( chars, ps.charPos, pos - ps.charPos );
                ps.charPos = pos;
            }
            curNode.SetValueNode( XmlNodeType.Text, stringBuilder.ToString() );
            stringBuilder.Length = 0;
            return true;
        }

        private void ParseXmlDeclarationFragment() {
            try {
                ParseXmlDeclaration( false );
            }
            catch ( XmlException e ) {
                ReThrow( e, e.LineNumber, e.LinePosition - 6 ); // 6 == strlen( "<?xml " );
            }
        }

        private void ThrowUnexpectedToken( int pos, string expectedToken ) {
            ThrowUnexpectedToken( pos, expectedToken, null );
        }

        private void ThrowUnexpectedToken( string expectedToken1 ) {
            ThrowUnexpectedToken( expectedToken1, null );
        }

        private void ThrowUnexpectedToken( int pos, string expectedToken1, string expectedToken2 ) {
            ps.charPos = pos;
            ThrowUnexpectedToken( expectedToken1, expectedToken2 );
        }

        private void ThrowUnexpectedToken( string expectedToken1, string expectedToken2 ) {
            string unexpectedToken = ParseUnexpectedToken();
            if ( expectedToken2 != null ) {
                Throw( Res.Xml_UnexpectedTokens2, new string[3] { unexpectedToken, expectedToken1, expectedToken2 } );
            }
            else {
                Throw( Res.Xml_UnexpectedTokenEx, new string[2] { unexpectedToken, expectedToken1 } );
            }
        }

        private string ParseUnexpectedToken( int pos ) {
            ps.charPos = pos;
            return ParseUnexpectedToken();
        }

        private string ParseUnexpectedToken() {
            if ( xmlCharType.IsNCNameChar( ps.chars[ps.charPos] ) ) {
                int pos = ps.charPos + 1;
                while ( xmlCharType.IsNCNameChar( ps.chars[pos] ) ) {
                    pos++;
                }
                return new string( ps.chars, ps.charPos, pos - ps.charPos );
            }
            else {
                Debug.Assert( ps.charPos < ps.charsUsed );
                return new string( ps.chars, ps.charPos, 1 );
            }
        }

        private int GetIndexOfAttributeWithoutPrefix( string name ) {
            name = nameTable.Get( name );
            if ( name == null ) {
                return -1;
            }
            for ( int i = index + 1; i < index + attrCount + 1; i++ ) {
                if ( Ref.Equal( nodes[i].localName, name ) && nodes[i].prefix.Length == 0 ) {
                    return i;
                }
            }
            return -1;
        }

        private int GetIndexOfAttributeWithPrefix( string name ) {
            name = nameTable.Add( name );
            if ( name == null ) {
                return -1;
            }
            for ( int i = index + 1; i < index + attrCount + 1; i++ ) {
                if ( Ref.Equal( nodes[i].GetNameWPrefix( nameTable ), name ) ) {
                    return i;
                }
            }
            return -1;
        }

        private Stream OpenStream( Uri uri ) {
            // resolve the stream
            return (Stream)xmlResolver.GetEntity( uri, null, typeof( Stream ) );
        }

        // This method is used to enable parsing of zero-terminated streams. The old XmlTextReader implementation used 
        // to parse such streams, we this one needs to do that as well. 
        // If the last characters decoded from the stream is 0 and the stream is in EOF state, this method will remove 
        // the character from the parsing buffer (decrements ps.charsUsed).
        // Note that this method calls ReadData() which may change the value of ps.chars and ps.charPos.
        private bool ZeroEndingStream( int pos ) {
            if ( v1Compat && pos == ps.charsUsed - 1 && ps.chars[pos] == (char)0 && ReadData() == 0 && ps.isStreamEof ) {
                ps.charsUsed--;
                return true;
            }
            return false;
        }

        private void ParseDtdFromParserContext() {
            Debug.Assert( dtdParserProxy == null && fragmentParserContext != null && fragmentParserContext.HasDtdInfo );

            dtdParserProxy = new DtdParserProxy( fragmentParserContext.BaseURI, fragmentParserContext.DocTypeName, fragmentParserContext.PublicId,
                                                 fragmentParserContext.SystemId, fragmentParserContext.InternalSubset, this );
            dtdParserProxy.Parse( false );

            SchemaInfo dtdSchemaInfo = dtdParserProxy.DtdSchemaInfo;
            if ( ( validatingReaderCompatFlag || !v1Compat ) && ( dtdSchemaInfo.HasDefaultAttributes || dtdSchemaInfo.HasNonCDataAttributes ) ) {
                addDefaultAttributesAndNormalize = true;
                qName = new XmlQualifiedName();
            }
        }

        bool InitReadContentAsBinary() {
            Debug.Assert( parsingFunction != ParsingFunction.InReadContentAsBinary );

            if ( parsingFunction == ParsingFunction.InReadValueChunk ) {
                throw new InvalidOperationException( Res.GetString( Res.Xml_MixingReadValueChunkWithBinary ) );
            }
            if ( parsingFunction == ParsingFunction.InIncrementalRead ) {
                throw new InvalidOperationException( Res.GetString( Res.Xml_MixingV1StreamingWithV2Binary ) );
            }

            if ( !XmlReader.IsTextualNode( curNode.type ) ) {
                if ( !MoveToNextContentNode( false ) ) {
                    return false;
                }
            }

            SetupReadContentAsBinaryState( ParsingFunction.InReadContentAsBinary );
            incReadLineInfo.Set( curNode.LineNo, curNode.LinePos );
            return true;
        }

        bool InitReadElementContentAsBinary() {
            Debug.Assert( parsingFunction != ParsingFunction.InReadElementContentAsBinary );
            Debug.Assert( curNode.type == XmlNodeType.Element );

            bool isEmpty = curNode.IsEmptyElement;

            // move to content or off the empty element
            outerReader.Read();
            if ( isEmpty ) {
                return false;
            }

            // make sure we are on a content node
            if ( !MoveToNextContentNode( false ) ) {
                if ( curNode.type != XmlNodeType.EndElement ) {
                    Throw( Res.Xml_InvalidNodeType, curNode.type.ToString() );
                }
                // move off end element
                outerReader.Read();
                return false;
            }
            SetupReadContentAsBinaryState( ParsingFunction.InReadElementContentAsBinary );
            incReadLineInfo.Set( curNode.LineNo, curNode.LinePos );
            return true;
        }

        bool MoveToNextContentNode( bool moveIfOnContentNode ) {
            do {
                switch ( curNode.type ) {
                    case XmlNodeType.Attribute:
                        return !moveIfOnContentNode;
                    case XmlNodeType.Text:
                    case XmlNodeType.Whitespace:
                    case XmlNodeType.SignificantWhitespace:
                    case XmlNodeType.CDATA:
                        if ( !moveIfOnContentNode ) {
                            return true;
                        }
                        break;
                    case XmlNodeType.ProcessingInstruction:
                    case XmlNodeType.Comment:
                    case XmlNodeType.EndEntity:
                        // skip comments, pis and end entity nodes
                        break;
                    case XmlNodeType.EntityReference:
                        outerReader.ResolveEntity();
                        break;
                    default:
                        return false;
                }
                moveIfOnContentNode = false;
            } while ( outerReader.Read() );
            return false;
        }

        void SetupReadContentAsBinaryState( ParsingFunction inReadBinaryFunction ) {
            if ( parsingFunction == ParsingFunction.PartialTextValue ) {
                incReadState = IncrementalReadState.ReadContentAsBinary_OnPartialValue;
            }
            else {
                incReadState = IncrementalReadState.ReadContentAsBinary_OnCachedValue;
                nextNextParsingFunction = nextParsingFunction;
                nextParsingFunction = parsingFunction;
            }
            readValueOffset = 0;
            parsingFunction = inReadBinaryFunction;
        }

        void SetupFromParserContext( XmlParserContext context, XmlReaderSettings settings ) {
            Debug.Assert( context != null );

            // setup nameTable
            XmlNameTable nt = settings.NameTable;
            nameTableFromSettings = ( nt != null );

            // get name table from namespace manager in XmlParserContext, if available; 
            if ( context.NamespaceManager != null ) {
                // must be the same as XmlReaderSettings.NameTable, or null
                if ( nt != null && nt != context.NamespaceManager.NameTable ) {
                    throw new XmlException( Res.Xml_NametableMismatch );
                }
                // get the namespace manager from context
                namespaceManager = context.NamespaceManager;
                xmlContext.defaultNamespace = namespaceManager.LookupNamespace( string.Empty );

                // get the nametable from ns manager
                nt = namespaceManager.NameTable;

                Debug.Assert( nt != null );
                Debug.Assert( context.NameTable == null || context.NameTable == nt, "This check should have been done in XmlParserContext constructor." );
            } 
            // get name table directly from XmlParserContext
            else if ( context.NameTable != null ) {
                // must be the same as XmlReaderSettings.NameTable, or null
                if ( nt != null && nt != context.NameTable ) {
                    throw new XmlException( Res.Xml_NametableMismatch );
                }
                nt = context.NameTable;
            }
            // no nametable provided -> create a new one
            else if ( nt == null ) {
                nt = new NameTable();
                Debug.Assert( nameTableFromSettings == false );
            }
            nameTable = nt;

            // make sure we have namespace manager
            if ( namespaceManager == null ) {
                namespaceManager = new XmlNamespaceManager( nt );
            }

            // copy xml:space and xml:lang
            xmlContext.xmlSpace = context.XmlSpace;
            xmlContext.xmlLang = context.XmlLang;
        }

//
// Internal properties for XmlValidatingReader
//
        internal SchemaInfo DtdSchemaInfo {
            get {
                return dtdParserProxy == null ? null : dtdParserProxy.DtdSchemaInfo;
            }
            set {
                Debug.Assert( dtdParserProxy == null );

                if ( value != null ) {
                    dtdParserProxy = new DtdParserProxy( this, value );
                    if ( ( validatingReaderCompatFlag || !v1Compat ) && ( value.HasDefaultAttributes || value.HasNonCDataAttributes ) ) {
                        addDefaultAttributesAndNormalize = true;
                        qName = new XmlQualifiedName();
                    }
                }
                else {
                    dtdParserProxy = null;
                }
            }
        }

        internal bool XmlValidatingReaderCompatibilityMode { 
            set {
                validatingReaderCompatFlag = value;
            }
        }

        internal ValidationEventHandler ValidationEventHandler {
            set {
                validationEventHandler = value;
            }
        }

        internal XmlNodeType FragmentType {
            get {
                return fragmentType;
            }
        }

        internal void ChangeCurrentNodeType( XmlNodeType newNodeType ) {
            Debug.Assert( curNode.type == XmlNodeType.Whitespace && newNodeType == XmlNodeType.SignificantWhitespace, "Incorrect node type change!" );
            curNode.type = newNodeType;
        }

        internal XmlResolver GetResolver() {
            return xmlResolver;
        }

        internal object InternalSchemaType {
            get {
                return curNode.schemaType;
            }
            set {
                curNode.schemaType = value;
            }
        }

        internal object InternalTypedValue {
            get {
                return curNode.typedValue;
            }
            set {
                curNode.typedValue = value;
            }
        }

        internal bool StandAlone {
            get {
                return standalone;
            }
        }

        internal override XmlNamespaceManager NamespaceManager {
            get {
                return namespaceManager;
            }
        }

        internal bool V1Compat {
            get {
                return v1Compat;
            }
        }

        internal ConformanceLevel V1ComformanceLevel {
            get {
                return fragmentType == XmlNodeType.Element ? ConformanceLevel.Fragment : ConformanceLevel.Document;
            }
        }

        internal bool AddDefaultAttribute( SchemaAttDef attrDef, bool definedInDtd ) {
            string localName = attrDef.Name.Name;
            string prefix = attrDef.Prefix;
            string ns = attrDef.Name.Namespace;

            // DTD default attribute
            if ( definedInDtd ) {
                if ( prefix.Length > 0 ) {
                    attrNeedNamespaceLookup = true;
                }
            }
            // XSD/XDR default attribute
            else {
                // atomize namesspace - Xsd Validator does not need to have the same nametable
                ns = nameTable.Add( ns );
                if ( prefix.Length == 0 && ns.Length > 0 ) {
                    prefix = namespaceManager.LookupPrefix( ns );

                    Debug.Assert( prefix != null );
                    if ( prefix == null ) {
                        prefix = string.Empty;
                    }
                }
            }

            // atomize names - Xsd Validator does not need to have the same nametable
            localName = nameTable.Add( localName );
            prefix = nameTable.Add( prefix );

            // check for duplicates
            for ( int i = index + 1; i < index + 1 + attrCount; i++ ) {
                if ( (object)nodes[i].localName == (object)localName && 
                     ( ( (object)nodes[i].prefix == (object)prefix ) || ( (object)nodes[i].ns == (object)ns  &&  ns != null ) ) ) {
                    return false;
                }
            }

            // check the default attribute value if it has a valid value according to its type
            if ( definedInDtd && DtdValidation && !attrDef.DefaultValueChecked ) {
                attrDef.CheckDefaultValue( dtdParserProxy.DtdSchemaInfo, dtdParserProxy );
            }

            // setup the attribute 
            NodeData attr = AddAttribute( localName, prefix, prefix.Length > 0 ? null : localName );
            if ( !definedInDtd ) {
                Debug.Assert( ns != null );
                attr.ns = ns;
            }
            attr.SetValue( attrDef.DefaultValueExpanded );
            attr.IsDefaultAttribute = true;
            attr.schemaType = ( attrDef.SchemaType == null ) ? (object)attrDef.Datatype : (object)attrDef.SchemaType;
            attr.typedValue = attrDef.DefaultValueTyped;
            attr.lineInfo.Set( attrDef.LineNum, attrDef.LinePos );
            attr.lineInfo2.Set( attrDef.ValueLineNum, attrDef.ValueLinePos );

            // handle special attributes:
            if ( attr.prefix.Length == 0 ) {
                // default namespace declaration
                if ( Ref.Equal( attr.localName, XmlNs ) ) {
                    OnDefaultNamespaceDecl( attr );
                    if ( !definedInDtd ) {
                        // change element default namespace
                        Debug.Assert( nodes[index].type == XmlNodeType.Element );
                        if ( nodes[index].prefix.Length == 0 ) {
                            nodes[index].ns = xmlContext.defaultNamespace;
                        }
                    }
                }
            }
            else {
                // prefixed namespace declaration
                if ( Ref.Equal( attr.prefix, XmlNs ) ) {
                    OnNamespaceDecl( attr );
                    if ( !definedInDtd ) {
                        // change namespace of current element and attributes
                        string pref = attr.localName;
                        Debug.Assert( nodes[index].type == XmlNodeType.Element );
                        for ( int i = index; i < index + attrCount + 1; i++ ) {
                            if ( nodes[i].prefix.Equals( pref ) ) {
                                nodes[i].ns = namespaceManager.LookupNamespace( pref );
                            }
                        }
                    }
                }
                // xml: attribute
                else {
                    if ( attrDef.Reserved != SchemaAttDef.Reserve.None ) {
                        OnXmlReservedAttribute( attr );
                    }
                }
            }

            fullAttrCleanup = true;
            return true;
        }

        internal bool DisableUndeclaredEntityCheck {
            set { 
                disableUndeclaredEntityCheck = value; 
            }
        }

        int ReadContentAsBinary( byte[] buffer, int index, int count ) { 
            Debug.Assert( incReadDecoder != null );

            if ( incReadState == IncrementalReadState.ReadContentAsBinary_End ) {
                return 0;
            }

            incReadDecoder.SetNextOutputBuffer( buffer, index, count );

            for (;;) {
                // read what is already cached in curNode
                int charsRead = 0;
                try {
                    charsRead = curNode.CopyToBinary( incReadDecoder, readValueOffset );
                }
                // add line info to the exception
                catch ( XmlException e ) {
                    curNode.AdjustLineInfo( readValueOffset, ps.eolNormalized, ref incReadLineInfo );
                    ReThrow( e, incReadLineInfo.lineNo, incReadLineInfo.linePos );
                }
                readValueOffset += charsRead;

                if ( incReadDecoder.IsFull ) {
                    return incReadDecoder.DecodedCount;
                }

                // if on partial value, read the rest of it
                if ( incReadState == IncrementalReadState.ReadContentAsBinary_OnPartialValue ) {
                    curNode.SetValue( string.Empty );

                    // read next chunk of text
                    bool endOfValue = false;
                    int startPos = 0;
                    int endPos = 0;
                    while ( !incReadDecoder.IsFull && !endOfValue ) {
                        int orChars = 0;

                        // store current line info and parse more text
                        incReadLineInfo.Set( ps.LineNo, ps.LinePos );
                        endOfValue = ParseText( out startPos, out endPos, ref orChars );

                        try {
                            charsRead = incReadDecoder.Decode( ps.chars, startPos, endPos - startPos ); 
                        }
                        // add line info to the exception
                        catch ( XmlException e ) {
                            ReThrow( e, incReadLineInfo.lineNo, incReadLineInfo.linePos);
                        }
                        startPos += charsRead;
                    }
                    incReadState = endOfValue ? IncrementalReadState.ReadContentAsBinary_OnCachedValue : IncrementalReadState.ReadContentAsBinary_OnPartialValue;
                    readValueOffset = 0;

                    if ( incReadDecoder.IsFull ) {
                        curNode.SetValue( ps.chars, startPos, endPos - startPos );
                        // adjust line info for the chunk that has been already decoded
                        AdjustLineInfo( ps.chars, startPos - charsRead, startPos, ps.eolNormalized, ref incReadLineInfo );
                        curNode.SetLineInfo( incReadLineInfo.lineNo, incReadLineInfo.linePos );
                        return incReadDecoder.DecodedCount;
                    }
                }

                // reset to normal state so we can call Read() to move forward
                ParsingFunction tmp = parsingFunction;
                parsingFunction = nextParsingFunction;
                nextParsingFunction = nextNextParsingFunction;

                // move to next textual node in the element content; throw on sub elements
                if ( !MoveToNextContentNode( true ) ) {
                    SetupReadContentAsBinaryState( tmp );
                    incReadState = IncrementalReadState.ReadContentAsBinary_End;
                    return incReadDecoder.DecodedCount;
                }
                SetupReadContentAsBinaryState( tmp );
                incReadLineInfo.Set( curNode.LineNo, curNode.LinePos );
            }
        }

        int ReadElementContentAsBinary( byte[] buffer, int index, int count ) { 
            if ( count == 0 ) {
                return 0;
            }
            int decoded = ReadContentAsBinary( buffer, index, count );
            if ( decoded > 0 ) {
                return decoded;
            }

            // if 0 bytes returned check if we are on a closing EndElement, throw exception if not
            if ( curNode.type != XmlNodeType.EndElement ) {
                throw new XmlException( Res.Xml_InvalidNodeType, curNode.type.ToString(), this as IXmlLineInfo );
            }

            // reset state
            parsingFunction = nextParsingFunction;
            nextParsingFunction = nextNextParsingFunction;
            Debug.Assert( parsingFunction != ParsingFunction.InReadElementContentAsBinary );

            // move off the EndElement
            outerReader.Read();
            return 0;
        }

        void InitBase64Decoder() {
            if ( base64Decoder == null ) {
                base64Decoder = new Base64Decoder();
            }
            else {
                base64Decoder.Reset();
            }
            incReadDecoder = base64Decoder;
        }

        void InitBinHexDecoder() {
            if ( binHexDecoder == null ) {
                binHexDecoder = new BinHexDecoder();
            }
            else {
                binHexDecoder.Reset();
            }
            incReadDecoder = binHexDecoder;
        }

        bool UriEqual( Uri uri1, string uri1Str, string uri2Str, XmlResolver resolver ) {
            if ( uri1 == null || resolver == null ) {
                return uri1Str == uri2Str;
            }
            Uri uri2 = resolver.ResolveUri( null, uri2Str );
            return uri1.Equals(uri2);
        }

        static internal void AdjustLineInfo( char[] chars, int startPos, int endPos, bool isNormalized, ref LineInfo lineInfo ) {
            int lastNewLinePos = -1;
            int i = startPos;
            while ( i < endPos ) {
                switch ( chars[i] ) {
                    case '\n':
                        lineInfo.lineNo++;
                        lastNewLinePos = i;
                        break;
                    case '\r':
                        if ( isNormalized ) {
                            break;
                        }
                        lineInfo.lineNo++;
                        lastNewLinePos = i;
                        if ( i + 1 < endPos && chars[i+1] == '\n' ) {
                            i++;
                            lastNewLinePos++;
                        }
                        break;
                }
                i++;
            }
            if ( lastNewLinePos >= 0 ) {
                lineInfo.linePos = endPos - lastNewLinePos;
            }
        }
    }
}

