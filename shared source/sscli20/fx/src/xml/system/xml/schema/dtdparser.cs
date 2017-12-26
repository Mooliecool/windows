//------------------------------------------------------------------------------
// <copyright file="DtdParser.cs" company="Microsoft">
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
using System.Xml.Schema;
using System.Diagnostics;
using System.Collections;
using System.Globalization;

namespace System.Xml {

    internal class DtdParser {
//
// Private types
//
        enum Token {
            CDATA           = XmlTokenizedType.CDATA,       // == 0
            ID              = XmlTokenizedType.ID,          // == 1
            IDREF           = XmlTokenizedType.IDREF,       // == 2
            IDREFS          = XmlTokenizedType.IDREFS,      // == 3
            ENTITY          = XmlTokenizedType.ENTITY,      // == 4
            ENTITIES        = XmlTokenizedType.ENTITIES,    // == 5
            NMTOKEN         = XmlTokenizedType.NMTOKEN,     // == 6
            NMTOKENS        = XmlTokenizedType.NMTOKENS,    // == 7
            NOTATION        = XmlTokenizedType.NOTATION,    // == 8
            None            ,
            PERef           ,
            AttlistDecl     ,
            ElementDecl     ,
            EntityDecl      ,
            NotationDecl    ,
            Comment         ,
            PI              ,
            CondSectionStart,
            CondSectionEnd  ,
            Eof             ,
            REQUIRED        ,
            IMPLIED         ,
            FIXED           ,
            QName           ,
            Name            ,
            Nmtoken         ,
            Quote,
            LeftParen       ,
            RightParen      ,
            GreaterThan     ,
            Or              ,
            LeftBracket     ,
            RightBracket    ,
            PUBLIC          ,
            SYSTEM          ,
            Literal         ,
            DOCTYPE         ,
            NData           ,
            Percent         ,
            Star            ,
            QMark           ,
            Plus            ,
            PCDATA          ,
            Comma           ,
            ANY             ,
            EMPTY           ,
            IGNORE          ,
            INCLUDE         ,
        }

        enum ScanningFunction {
            SubsetContent,
            Name,
            QName,
            Nmtoken,
            Doctype1,
            Doctype2,
            Element1,
            Element2,
            Element3,
            Element4,
            Element5,
            Element6,
            Element7,
            Attlist1,
            Attlist2,
            Attlist3,
            Attlist4,
            Attlist5,
            Attlist6,
            Attlist7,
            Entity1,
            Entity2,
            Entity3,
            Notation1,
            CondSection1,
            CondSection2,
            CondSection3,
            Literal,
            SystemId,
            PublicId1,
            PublicId2,
            ClosingTag,
            ParamEntitySpace,
            None,
        }

        enum LiteralType { 
            AttributeValue,
            EntityReplText,
            SystemOrPublicID
        }

        class UndeclaredNotation {
            internal string name;
            internal int lineNo;
            internal int linePos;
            internal UndeclaredNotation next;

            internal UndeclaredNotation( string name, int lineNo, int linePos ) { 
                this.name = name;
                this.lineNo = lineNo;
                this.linePos = linePos;
                this.next = null;
            }
        }

//
// Fields
//
        // connector to reader
        IDtdParserAdapter readerAdapter;

        // name table
        XmlNameTable nameTable;

        // final schema info
        SchemaInfo  schemaInfo;

        // XmlCharType instance
        XmlCharType xmlCharType = XmlCharType.Instance;

        // system & public id
        string      systemId = string.Empty;
        string      publicId = string.Empty;

        // flags
        bool    validate;
        bool    normalize;
        bool    supportNamespaces;
        bool    v1Compat;

        // cached character buffer
        char[]  chars;
        int     charsUsed;
        int     curPos;

        // scanning function for the next token
        ScanningFunction scanningFunction;
        ScanningFunction nextScaningFunction;
        ScanningFunction savedScanningFunction; // this one is used only for adding spaces around parameter entities

        // flag if whitespace seen before token
        bool    whitespaceSeen;

        // start position of the last token (for name and value)
        int     tokenStartPos;

        // colon position (for name)
        int     colonPos;

        // value of the internal subset
        BufferBuilder internalSubsetValueSb = null;
        string  internalSubsetValue = string.Empty;

        // entities
        int     externalEntitiesDepth = 0;
        int     currentEntityId = 0;
        int     nextEntityId = 1;
        int[]   condSectionEntityIds = null;

        // free-floating DTD support
        bool            freeFloatingDtd = false;
        bool            hasFreeFloatingInternalSubset = false;

        // misc
        BufferBuilder   stringBuilder;
        Hashtable       undeclaredNotations = null;
        int             condSectionDepth = 0;
        LineInfo        literalLineInfo = new LineInfo( 0, 0 );
        char            literalQuoteChar = '"';
        string          documentBaseUri = string.Empty;
        string          externalDtdBaseUri = string.Empty;

        const int CondSectionEntityIdsInitialSize = 2;

//
// Constructor
//

#if DEBUG
        static DtdParser() {
            //  The absolute numbering is utilized in attribute type parsing
            Debug.Assert( (int)Token.CDATA    == (int)XmlTokenizedType.CDATA    && (int)XmlTokenizedType.CDATA    == 0 );
            Debug.Assert( (int)Token.ID       == (int)XmlTokenizedType.ID       && (int)XmlTokenizedType.ID       == 1 );
            Debug.Assert( (int)Token.IDREF    == (int)XmlTokenizedType.IDREF    && (int)XmlTokenizedType.IDREF    == 2 );
            Debug.Assert( (int)Token.IDREFS   == (int)XmlTokenizedType.IDREFS   && (int)XmlTokenizedType.IDREFS   == 3 );
            Debug.Assert( (int)Token.ENTITY   == (int)XmlTokenizedType.ENTITY   && (int)XmlTokenizedType.ENTITY   == 4 );
            Debug.Assert( (int)Token.ENTITIES == (int)XmlTokenizedType.ENTITIES && (int)XmlTokenizedType.ENTITIES == 5 );
            Debug.Assert( (int)Token.NMTOKEN  == (int)XmlTokenizedType.NMTOKEN  && (int)XmlTokenizedType.NMTOKEN  == 6 );
            Debug.Assert( (int)Token.NMTOKENS == (int)XmlTokenizedType.NMTOKENS && (int)XmlTokenizedType.NMTOKENS == 7 );
            Debug.Assert( (int)Token.NOTATION == (int)XmlTokenizedType.NOTATION && (int)XmlTokenizedType.NOTATION == 8 );
        }
#endif

        internal DtdParser( IDtdParserAdapter readerAdapter ) {
            Debug.Assert( readerAdapter != null );
            this.readerAdapter = readerAdapter;
            
            nameTable = readerAdapter.NameTable;

            validate =  readerAdapter.DtdValidation;
            normalize = readerAdapter.Normalization;
            supportNamespaces = readerAdapter.Namespaces;
            v1Compat = readerAdapter.V1CompatibilityMode;
            
            schemaInfo = new SchemaInfo();
            schemaInfo.SchemaType = SchemaType.DTD;
    
            stringBuilder = new BufferBuilder();

            Uri baseUri = readerAdapter.BaseUri;
            if ( baseUri != null ) {
                documentBaseUri = baseUri.ToString();
            }
        }

        internal DtdParser( string baseUri, string docTypeName, string publicId, string systemId, string internalSubset, IDtdParserAdapter underlyingReader ) 
            : this( underlyingReader ) {

            if ( docTypeName == null || docTypeName.Length == 0 ) {
                throw XmlConvert.CreateInvalidNameArgumentException( docTypeName, "docTypeName" );
            }

            // check doctype name
            XmlConvert.VerifyName( docTypeName );
            int colonPos = docTypeName.IndexOf( ':' );
            if ( colonPos == -1 ) {
                schemaInfo.DocTypeName = new XmlQualifiedName( nameTable.Add( docTypeName ) );
            }
            else {
                schemaInfo.DocTypeName = new XmlQualifiedName( nameTable.Add( docTypeName.Substring( 0, colonPos ) ),
                                                               nameTable.Add( docTypeName.Substring( colonPos + 1 ) ) );
            }
            
            int i;
            // check system id
            if ( systemId != null && systemId.Length > 0 ) {
                if ( ( i = xmlCharType.IsOnlyCharData( systemId ) ) >= 0 ) {
                    ThrowInvalidChar( curPos, systemId[i] );
                }
                this.systemId = systemId;
            }

            // check public id
            if ( publicId != null && publicId.Length > 0 ) {
                if ( ( i = xmlCharType.IsPublicId( publicId ) ) >= 0 ) {
                    ThrowInvalidChar( curPos, publicId[i] );
                }
                this.publicId = publicId;
            }

            // init free-floating internal subset
            if ( internalSubset != null && internalSubset.Length > 0 ) {
                readerAdapter.PushInternalDtd( baseUri, internalSubset );
                hasFreeFloatingInternalSubset = true;
            }

            Uri baseUriOb = readerAdapter.BaseUri;
            if ( baseUriOb != null ) {
                documentBaseUri = baseUriOb.ToString();
            }

            freeFloatingDtd = true;
        }
        
//
// Internal static Parse method
//
        static internal SchemaInfo Parse( XmlNameTable nt, XmlNamespaceManager nsManager, bool namespaces, string baseUri, string docTypeName, 
                                          string publicId, string systemId, string internalSubset, bool useResolver, XmlResolver resolver ) {

            XmlParserContext pc = new XmlParserContext( nt, nsManager, null, null, null, null, baseUri, string.Empty, XmlSpace.None );
            XmlTextReaderImpl tr = new XmlTextReaderImpl( "", XmlNodeType.Element, pc );
            tr.Namespaces = namespaces;
            if ( useResolver ) {
                tr.XmlResolver = resolver;
            }

            return DtdParser.Parse( tr, baseUri, docTypeName, publicId, systemId, internalSubset ); 
        }

        static internal SchemaInfo Parse( XmlTextReaderImpl tr, string baseUri, string docTypeName, string publicId, string systemId, string internalSubset ) {
            XmlTextReaderImpl.DtdParserProxy dtdParserProxy = new XmlTextReaderImpl.DtdParserProxy( baseUri, docTypeName, publicId, systemId, internalSubset, tr ); 
            dtdParserProxy.Parse( false );
            return dtdParserProxy.DtdSchemaInfo;
        }
//
// Internal properties for readers
//
        internal string SystemID { 
            get { 
                return systemId;
            }
        }

        internal string PublicID {
            get { 
                return publicId;
            }
        }

        internal string InternalSubset {
            get { 
                return internalSubsetValue;
            }
        }

        internal SchemaInfo SchemaInfo {
            get { 
                return schemaInfo;
            }
        }

//
// Private properties
//
    private bool ParsingInternalSubset {
        get {
            return externalEntitiesDepth == 0;
        }
    }

    private bool IgnoreEntityReferences {
        get { 
            return scanningFunction == ScanningFunction.CondSection3;
        }
    }

    private bool SaveInternalSubsetValue { 
        get { 
            return readerAdapter.EntityStackLength == 0 && internalSubsetValueSb != null;
        }
    }

    private bool ParsingTopLevelMarkup {
        get {
            return scanningFunction == ScanningFunction.SubsetContent || 
                ( scanningFunction == ScanningFunction.ParamEntitySpace && savedScanningFunction == ScanningFunction.SubsetContent );
        }
    }

//
// Parsing methods
//
        internal void Parse( bool saveInternalSubset ) {
            if ( freeFloatingDtd ) {
                ParseFreeFloatingDtd();
            }
            else {
                ParseInDocumentDtd( saveInternalSubset );
            }

            schemaInfo.Finish();

            // check undeclared forward references
            if ( validate && undeclaredNotations != null ) {
                foreach ( UndeclaredNotation un in undeclaredNotations.Values ) {
                    UndeclaredNotation tmpUn = un;
                    while ( tmpUn != null ) {
                        SendValidationEvent( XmlSeverityType.Error, new XmlSchemaException( Res.Sch_UndeclaredNotation, un.name, BaseUriStr, (int)un.lineNo, (int)un.linePos ) );
                        tmpUn = tmpUn.next;
                    }
                }
            }
        }

        private void ParseInDocumentDtd( bool saveInternalSubset ) {
            LoadParsingBuffer();

            scanningFunction = ScanningFunction.QName;
            nextScaningFunction = ScanningFunction.Doctype1;

            // doctype name
            if ( GetToken( false ) != Token.QName ) {
                OnUnexpectedError();
            }   
            schemaInfo.DocTypeName = GetNameQualified( true );

            // SYSTEM or PUBLIC id
            Token token = GetToken( false );
            if ( token == Token.SYSTEM || token == Token.PUBLIC ) {
                ParseExternalId( token, Token.DOCTYPE, out publicId, out systemId );
                token = GetToken( false);
            }

            switch ( token ) {
                case Token.LeftBracket:
                    if ( saveInternalSubset ) {
                        SaveParsingBuffer(); // this will cause saving the internal subset right from the point after '['
                        internalSubsetValueSb = new BufferBuilder();
                    }
                    ParseInternalSubset();
                    break;
                case Token.GreaterThan:
                    break;
                default:
                    OnUnexpectedError();
                    break;
            }
            SaveParsingBuffer();

            if ( systemId != null && systemId.Length > 0 ) {
                ParseExternalSubset();
            }
        }

        private void ParseFreeFloatingDtd() {

            if ( hasFreeFloatingInternalSubset ) {
                LoadParsingBuffer();
                ParseInternalSubset();
                SaveParsingBuffer();
            }

            if ( systemId != null && systemId.Length > 0 ) {
                ParseExternalSubset();
            }
        }

        private void ParseInternalSubset() {
            Debug.Assert( ParsingInternalSubset );
            ParseSubset();
        }

        private void ParseExternalSubset() {
            Debug.Assert( externalEntitiesDepth == 0 );

            // push external subset
            if ( !readerAdapter.PushExternalSubset( systemId, publicId ) ) {
                return;
            }

            Uri baseUri = readerAdapter.BaseUri;
            if ( baseUri != null ) {
                externalDtdBaseUri = baseUri.ToString();
            }

            externalEntitiesDepth++;
            LoadParsingBuffer();

            // parse
            ParseSubset();

#if DEBUG
            Debug.Assert( readerAdapter.EntityStackLength == 0 ||
                         ( freeFloatingDtd && readerAdapter.EntityStackLength == 1 ) );
#endif
        }

        private void ParseSubset() {
            int startTagEntityId;
            for (;;) {
                Token token = GetToken( false );
                startTagEntityId = currentEntityId;
                switch ( token ) {
                    case Token.AttlistDecl:
                        ParseAttlistDecl();
                        break;

                    case Token.ElementDecl:
                        ParseElementDecl();
                        break;

                    case Token.EntityDecl:
                        ParseEntityDecl();
                        break;

                    case Token.NotationDecl:
                        ParseNotationDecl();
                        break;

                    case Token.Comment:
                        ParseComment();
                        break;

                    case Token.PI:
                        ParsePI();
                        break;

                    case Token.CondSectionStart:
                        if ( ParsingInternalSubset ) {
                            Throw( curPos - 3, Res.Xml_InvalidConditionalSection ); // 3==strlen("<![")
                        }
                        ParseCondSection();
                        startTagEntityId = currentEntityId;
                        break;
                    case Token.CondSectionEnd:
                        if ( condSectionDepth > 0 ) {
                            condSectionDepth--;
                            if ( validate && currentEntityId != condSectionEntityIds[condSectionDepth] ) {
                                SendValidationEvent( curPos, XmlSeverityType.Error, Res.Sch_ParEntityRefNesting, string.Empty );
                            }
                        }
                        else {  
                            Throw( curPos - 3, Res.Xml_UnexpectedCDataEnd );
                        }
                        break;
                    case Token.RightBracket:
                        if ( ParsingInternalSubset ) {
                            if ( condSectionDepth != 0 ) {
                                Throw( curPos, Res.Xml_UnclosedConditionalSection );
                            }
                            // append the rest to internal subset value but not the closing ']'
                            if ( internalSubsetValueSb != null ) {
                                Debug.Assert( curPos > 0 && chars[curPos-1] == ']' );
                                SaveParsingBuffer( curPos - 1 );
                                internalSubsetValue = internalSubsetValueSb.ToString();
                                internalSubsetValueSb = null;
                            }
                            // check '>'
                            if ( GetToken( false ) != Token.GreaterThan ) {
                                ThrowUnexpectedToken( curPos, ">" );
                            }
#if DEBUG
                            // check entity nesting
                            Debug.Assert( readerAdapter.EntityStackLength == 0 || 
                                          ( freeFloatingDtd && readerAdapter.EntityStackLength == 1 ) );
#endif
                        }
                        else {
                            Throw( curPos, Res.Xml_ExpectDtdMarkup );
                        }
                        return;
                    case Token.Eof:
                        if ( ParsingInternalSubset && !freeFloatingDtd ) {
                            Throw( curPos, Res.Xml_IncompleteDtdContent );
                        }
                        if ( condSectionDepth != 0 ) {
                            Throw( curPos, Res.Xml_UnclosedConditionalSection );
                        }
                        return;
                    default:
                        Debug.Assert( false );
                        break;
                }

                Debug.Assert( scanningFunction == ScanningFunction.SubsetContent );

                if ( currentEntityId != startTagEntityId ) {
                    if ( validate ) {
                        SendValidationEvent( curPos, XmlSeverityType.Error, Res.Sch_ParEntityRefNesting, string.Empty );
                    }
                    else {
                        if ( !v1Compat ) {
                            Throw( curPos, Res.Sch_ParEntityRefNesting );
                        }
                    }
                }
            }
        }

        private void ParseAttlistDecl() {

            if ( GetToken( true ) != Token.QName ) {
                goto UnexpectedError;
            }

            // element name
            XmlQualifiedName elementName = GetNameQualified( true );
            SchemaElementDecl elementDecl = (SchemaElementDecl)schemaInfo.ElementDecls[elementName];
            if ( elementDecl == null ) {
                elementDecl = (SchemaElementDecl)schemaInfo.UndeclaredElementDecls[elementName];
                if ( elementDecl == null) {
                    elementDecl = new SchemaElementDecl( elementName, elementName.Namespace, SchemaType.DTD );
                    schemaInfo.UndeclaredElementDecls.Add( elementName, elementDecl );
                }
            }

            SchemaAttDef attrDef = null;
            for (;;) {
                switch ( GetToken( false ) ) {
                    case Token.QName:
                        XmlQualifiedName attrName = GetNameQualified( true );
                        attrDef = new SchemaAttDef( attrName, attrName.Namespace );
                        attrDef.IsDeclaredInExternal = !ParsingInternalSubset;
                        attrDef.LineNum = (int)LineNo;
                        attrDef.LinePos = (int)LinePos - ( curPos - tokenStartPos );
                        break;
                    case Token.GreaterThan:
                        if ( v1Compat ) {
                            // check xml:space and xml:lang
                            // BUG BUG: For backward compatibility, we check the correct type and values of the
                            // xml:space attribute only on the last attribute in the list.
                            // See Webdata bugs #97457 and #93935.
                            if ( attrDef != null && attrDef.Prefix.Length > 0 && attrDef.Prefix.Equals( "xml" ) && attrDef.Name.Name == "space" ) {
                                attrDef.Reserved = SchemaAttDef.Reserve.XmlSpace;
                                if ( attrDef.Datatype.TokenizedType != XmlTokenizedType.ENUMERATION ) {
                                    Throw( Res.Xml_EnumerationRequired, string.Empty, attrDef.LineNum, attrDef.LinePos );
                                }
                                if ( readerAdapter.EventHandler != null ) {
                                    attrDef.CheckXmlSpace( readerAdapter.EventHandler );
                                }
                            }
                        }
                        return;
                    default:
                        goto UnexpectedError;
                }

                ParseAttlistType( attrDef, elementDecl );
                ParseAttlistDefault( attrDef );

                // check xml:space and xml:lang
                if ( attrDef.Prefix.Length > 0 && attrDef.Prefix.Equals( "xml" ) ) {
                    if ( attrDef.Name.Name == "space" ) {
                        if ( v1Compat ) {
                            // BUG BUG: For backward compatibility, we check the correct type and values of the
                            // xml:space attribute only on the last attribute in the list, and mark it as reserved 
                            // only its value is correct (=prevent XmlTextReader from fhrowing on invalid value). 
                            // See Webdata bugs #98168, #97457 and #93935.
                            string val = attrDef.DefaultValueExpanded.Trim();
                            if ( val.Equals( "preserve" ) || val.Equals( "default" ) ) {
                                attrDef.Reserved = SchemaAttDef.Reserve.XmlSpace;
                            }
                        }
                        else {
                            attrDef.Reserved = SchemaAttDef.Reserve.XmlSpace;
                            if ( attrDef.Datatype.TokenizedType != XmlTokenizedType.ENUMERATION ) {
                                Throw( Res.Xml_EnumerationRequired, string.Empty, attrDef.LineNum, attrDef.LinePos );
                            }
                            if ( readerAdapter.EventHandler != null ) {
                                attrDef.CheckXmlSpace( readerAdapter.EventHandler );
                            }
                        }
                    }
                    else if ( attrDef.Name.Name == "lang" ) {
                        attrDef.Reserved = SchemaAttDef.Reserve.XmlLang;
                    }
                }

                // add attribute to element decl
                if ( elementDecl.GetAttDef( attrDef.Name ) == null ) {
                    elementDecl.AddAttDef( attrDef );
                }
            }

            UnexpectedError:
                OnUnexpectedError();
        }

        private void ParseAttlistType( SchemaAttDef attrDef, SchemaElementDecl elementDecl ) {
            Token token = GetToken( true );

            if ( token != Token.CDATA ) {
                elementDecl.HasNonCDataAttribute = true;
            }
            
            if ( IsAttributeValueType( token ) ) {
                attrDef.Datatype = XmlSchemaDatatype.FromXmlTokenizedType( (XmlTokenizedType)(int)token );
                attrDef.SchemaType = XmlSchemaType.GetBuiltInSimpleType( attrDef.Datatype.TypeCode );

                switch ( token ) {
                    case Token.NOTATION:
                        break;
                    case Token.ID:
                        if ( validate && elementDecl.IsIdDeclared ) {
                            SchemaAttDef idAttrDef = elementDecl.GetAttDef( attrDef.Name );
                            if ( idAttrDef == null || idAttrDef.Datatype.TokenizedType != XmlTokenizedType.ID ) {
                                SendValidationEvent( XmlSeverityType.Error, Res.Sch_IdAttrDeclared, elementDecl.Name.ToString() );
                            }
                        }
                        elementDecl.IsIdDeclared = true;
                        return;
                    default:
                        return;
                }
                // check notation constrains
                if ( validate ) {
                    if ( elementDecl.IsNotationDeclared ) {
                        SendValidationEvent( curPos - 8, XmlSeverityType.Error, Res.Sch_DupNotationAttribute, elementDecl.Name.ToString() ); // 8 == strlen("NOTATION")
                    }
                    else {
                        if ( elementDecl.ContentValidator != null && 
                            elementDecl.ContentValidator.ContentType == XmlSchemaContentType.Empty ) {
                            SendValidationEvent( curPos - 8, XmlSeverityType.Error, Res.Sch_NotationAttributeOnEmptyElement, elementDecl.Name.ToString() );// 8 == strlen("NOTATION")
                        }
                        elementDecl.IsNotationDeclared = true;
                    }
                }

                if ( GetToken( true ) != Token.LeftParen ) {
                    goto UnexpectedError;
                }

                // parse notation list
                if ( GetToken( false ) != Token.Name ) {
                    goto UnexpectedError;
                }
                for (;;) {
                    string notationName = GetNameString();
                    if ( schemaInfo.Notations[notationName] == null ) {
                        if ( undeclaredNotations == null ) {
                            undeclaredNotations = new Hashtable();
                        }
                        UndeclaredNotation un = new UndeclaredNotation( notationName, LineNo, LinePos - notationName.Length );
                        UndeclaredNotation loggedUn = (UndeclaredNotation)undeclaredNotations[notationName];
                        if ( loggedUn != null ) {
                            un.next = loggedUn.next;
                            loggedUn.next = un;
                        }
                        else {
                            undeclaredNotations.Add( notationName, un );
                        }
                    }
                    if ( validate && !v1Compat && attrDef.Values != null && attrDef.Values.Contains( notationName ) ) {
                        SendValidationEvent( XmlSeverityType.Error, new XmlSchemaException( Res.Xml_AttlistDuplNotationValue, notationName, BaseUriStr, (int)LineNo, (int)LinePos ) );
                    }
                    attrDef.AddValue( notationName );

                    switch ( GetToken( false ) ) {
                        case Token.Or:
                            if ( GetToken( false ) != Token.Name ) {
                                goto UnexpectedError;
                            }
                            continue;
                        case Token.RightParen:
                            return;
                        default:
                            goto UnexpectedError;
                    }
                }
            }
            else if ( token == Token.LeftParen ) {
                attrDef.Datatype = XmlSchemaDatatype.FromXmlTokenizedType( XmlTokenizedType.ENUMERATION );
                attrDef.SchemaType = XmlSchemaType.GetBuiltInSimpleType( attrDef.Datatype.TypeCode );

                // parse nmtoken list
                if ( GetToken( false ) != Token.Nmtoken ) 
                    goto UnexpectedError;
                attrDef.AddValue( GetNameString() );

                for (;;) {
                    switch ( GetToken( false ) ) {
                        case Token.Or:
                            if ( GetToken( false ) != Token.Nmtoken ) 
                                goto UnexpectedError;
                            string nmtoken = GetNmtokenString();
                            if ( validate && !v1Compat && attrDef.Values != null && attrDef.Values.Contains( nmtoken ) ) {
                                SendValidationEvent( XmlSeverityType.Error, new XmlSchemaException( Res.Xml_AttlistDuplEnumValue, nmtoken, BaseUriStr, (int)LineNo, (int)LinePos ) );
                            }
                            attrDef.AddValue( nmtoken );
                            break;
                        case Token.RightParen:
                            return;
                        default:
                            goto UnexpectedError;
                    }
                }
            }
            else {
                goto UnexpectedError;
            }

        UnexpectedError:
            OnUnexpectedError();
        }

        private void ParseAttlistDefault( SchemaAttDef attrDef ) {
            switch ( GetToken( true ) ) {
                case Token.REQUIRED:
                    attrDef.Presence = SchemaDeclBase.Use.Required;
                    return;
                case Token.IMPLIED:
                    attrDef.Presence = SchemaDeclBase.Use.Implied;
                    return;
                case Token.FIXED:
                    attrDef.Presence = SchemaDeclBase.Use.Fixed;
                    if ( GetToken( true ) != Token.Literal ) {
                        goto UnexpectedError;
                    }
                    break;
                case Token.Literal:
                    break;
                default:
                    goto UnexpectedError;
            }

            if ( validate && attrDef.Datatype.TokenizedType == XmlTokenizedType.ID ) {
                SendValidationEvent( curPos, XmlSeverityType.Error, Res.Sch_AttListPresence, string.Empty );
            }

            if ( attrDef.Datatype.TokenizedType != XmlTokenizedType.CDATA ) {
                // non-CDATA attribute type normalization - strip spaces
                attrDef.DefaultValueExpanded = GetValueWithStrippedSpaces();
            }
            else {
                attrDef.DefaultValueExpanded = GetValue();
            }
            attrDef.ValueLineNum = (int)literalLineInfo.lineNo;
            attrDef.ValueLinePos = (int)literalLineInfo.linePos + 1;

            DtdValidator.SetDefaultTypedValue( attrDef, readerAdapter );

            return;

        UnexpectedError:
            OnUnexpectedError();
        }

        private void ParseElementDecl() {

            // element name
            if ( GetToken( true ) != Token.QName ) {
                goto UnexpectedError;
            }

            // get schema decl for element
            SchemaElementDecl elementDecl = null;
            XmlQualifiedName name = GetNameQualified( true );
            elementDecl = (SchemaElementDecl)schemaInfo.ElementDecls[name];

            if ( elementDecl != null ) {
                if ( validate ) {
                    SendValidationEvent( curPos - name.Name.Length, XmlSeverityType.Error, Res.Sch_DupElementDecl, GetNameString() );
                }
            }
            else {
                if ( ( elementDecl = (SchemaElementDecl)schemaInfo.UndeclaredElementDecls[name] ) != null ) {
                    schemaInfo.UndeclaredElementDecls.Remove( name );
                }
                else {
                    elementDecl = new SchemaElementDecl( name, name.Namespace, SchemaType.DTD );
                }
                schemaInfo.ElementDecls.Add( name, elementDecl );
            }
            elementDecl.IsDeclaredInExternal = !ParsingInternalSubset;

            // content spec
            switch ( GetToken( true ) ) {
                case Token.EMPTY:
                    elementDecl.ContentValidator = ContentValidator.Empty;
                    goto End;
                case Token.ANY:
                    elementDecl.ContentValidator = ContentValidator.Any;
                    goto End;
                case Token.LeftParen:
                    int startParenEntityId = currentEntityId;
                    switch ( GetToken( false ) ) {
                        case Token.PCDATA:
                        {
                            ParticleContentValidator pcv = new ParticleContentValidator( XmlSchemaContentType.Mixed );
                            pcv.Start();
                            pcv.OpenGroup();

                            ParseElementMixedContent( pcv, startParenEntityId );

                            elementDecl.ContentValidator = pcv.Finish( true );
                            goto End;
                        }
                        case Token.None:
                        {
                            ParticleContentValidator pcv = null;
                            pcv = new ParticleContentValidator( XmlSchemaContentType.ElementOnly );
                            pcv.Start();
                            pcv.OpenGroup();

                            ParseElementOnlyContent( pcv, startParenEntityId );

                            elementDecl.ContentValidator = pcv.Finish( true );
                            goto End;
                        }
                        default:
                            goto UnexpectedError;
                    }
                default:
                    goto UnexpectedError;
            }
    End:
            if ( GetToken( false ) != Token.GreaterThan ) {
                ThrowUnexpectedToken( curPos, ">" );
            }
            return;

            UnexpectedError:
                OnUnexpectedError();
        }

        private void ParseElementOnlyContent( ParticleContentValidator pcv, int startParenEntityId ) {
            Token parsingSchema = Token.None;
            int connectorEntityId = startParenEntityId;
            int contentEntityId = -1;

            for (;;) {
                switch ( GetToken( false ) ) {
                    case Token.QName:
        
                        pcv.AddName( GetNameQualified(true), null );

                        if ( validate ) {
                            contentEntityId = currentEntityId;
                            if ( connectorEntityId > contentEntityId ) {  // entity repl.text ending with connector
                                SendValidationEvent( curPos, XmlSeverityType.Error, Res.Sch_ParEntityRefNesting, string.Empty );
                            }
                        }
                        ParseHowMany( pcv );
                        break;
                    case Token.LeftParen:
                        pcv.OpenGroup();
                        if ( validate ) {
                            contentEntityId = currentEntityId;
                            if ( connectorEntityId > contentEntityId ) {  // entity repl.text ending with connector
                                SendValidationEvent( curPos, XmlSeverityType.Error, Res.Sch_ParEntityRefNesting, string.Empty );
                            }
                        }
                        ParseElementOnlyContent( pcv, currentEntityId );
                        break;
                    case Token.GreaterThan:
                        Throw( curPos, Res.Xml_InvalidContentModel );
                        return;
                    default:
                        goto UnexpectedError;
                }

                switch ( GetToken( false ) ) {
                    case Token.Comma:
                        if ( parsingSchema == Token.Or ) {
                            Throw( curPos, Res.Xml_InvalidContentModel );
                        }
                        pcv.AddSequence();
                        parsingSchema = Token.Comma;
                        if ( validate ) {
                            connectorEntityId = currentEntityId;
                            if ( connectorEntityId > contentEntityId ) { // entity repl.text starting with connector
                                SendValidationEvent( curPos, XmlSeverityType.Error, Res.Sch_ParEntityRefNesting, string.Empty );
                            }
                        }
                        break;
                    case Token.Or:
                        if ( parsingSchema == Token.Comma ) {
                            Throw( curPos, Res.Xml_InvalidContentModel );
                        }
                        pcv.AddChoice();
                        parsingSchema = Token.Or;
                        if ( validate ) {
                            connectorEntityId = currentEntityId;
                            if ( connectorEntityId > contentEntityId ) { // entity repl.text starting with connector
                                SendValidationEvent( curPos, XmlSeverityType.Error, Res.Sch_ParEntityRefNesting, string.Empty );
                            }
                        }
                        break;
                    case Token.RightParen:
                        pcv.CloseGroup();
                        if ( validate && currentEntityId != startParenEntityId ) {
                            SendValidationEvent( curPos, XmlSeverityType.Error, Res.Sch_ParEntityRefNesting, string.Empty );
                        }
                        ParseHowMany( pcv );
                        return;
                    case Token.GreaterThan:
                        Throw( curPos, Res.Xml_InvalidContentModel );
                        return;
                    default:
                        goto UnexpectedError;
                }
            }

        UnexpectedError:
            OnUnexpectedError();
        }

        private void ParseHowMany( ParticleContentValidator pcv ) {
            switch ( GetToken( false ) ) {
                case Token.Star:
                    pcv.AddStar();
                    return;
                case Token.QMark:
                    pcv.AddQMark();
                    return;
                case Token.Plus:
                    pcv.AddPlus();
                    return;
                default:
                    return;
            }
        }

        private void ParseElementMixedContent( ParticleContentValidator pcv, int startParenEntityId ) {
            bool hasNames = false;
            int connectorEntityId = -1;
            int contentEntityId = currentEntityId;

            for (;;) {
                switch ( GetToken( false ) ) {
                    case Token.RightParen:
                        pcv.CloseGroup();
                        if ( validate && currentEntityId != startParenEntityId ) {
                            SendValidationEvent( curPos, XmlSeverityType.Error, Res.Sch_ParEntityRefNesting, string.Empty );
                        }
                        if ( GetToken( false ) == Token.Star && hasNames ) {
                            pcv.AddStar();
                        }
                        else if ( hasNames ) {
                            ThrowUnexpectedToken( curPos, "*" );
                        }
                        return;
                    case Token.Or:
                        if ( !hasNames ) {
                            hasNames = true;
                        }
                        else {
                            pcv.AddChoice();
                        }
                        if ( validate ) { 
                            connectorEntityId = currentEntityId;
                            if ( contentEntityId < connectorEntityId ) {  // entity repl.text starting with connector
                                SendValidationEvent( curPos, XmlSeverityType.Error, Res.Sch_ParEntityRefNesting, string.Empty );
                            }
                        }

                        if ( GetToken( false ) != Token.QName ) {
                            goto default;
                        }
                        
                        XmlQualifiedName name = GetNameQualified( true );
                        if ( pcv.Exists( name ) && validate ) {
                            SendValidationEvent( XmlSeverityType.Error, Res.Sch_DupElement, name.ToString() );
                        }
                        pcv.AddName( name, null );

                        if ( validate ) {
                            contentEntityId = currentEntityId;
                            if ( contentEntityId < connectorEntityId ) { // entity repl.text ending with connector
                                SendValidationEvent( curPos, XmlSeverityType.Error, Res.Sch_ParEntityRefNesting, string.Empty );
                            }
                        }
                        continue;
                    default:
                        OnUnexpectedError();
                        break;
                }
            }
        }

        private void ParseEntityDecl() {
            bool isParamEntity = false;
            SchemaEntity entity = null;

            // get entity name and type
            switch ( GetToken( true ) ) {
                case Token.Percent:
                    isParamEntity = true;
                    if ( GetToken( true ) != Token.Name ) {
                        goto UnexpectedError;
                    }
                    goto case Token.Name;
                case Token.Name:
                    // create entity object
                    XmlQualifiedName entityName = GetNameQualified( false );
                    entity = new SchemaEntity( entityName, isParamEntity );
                    
                    entity.BaseURI = BaseUriStr;
                    entity.DeclaredURI = ( externalDtdBaseUri.Length == 0 ) ? documentBaseUri : externalDtdBaseUri;

                    if ( isParamEntity ) {
                        if ( schemaInfo.ParameterEntities[ entityName ] == null ) {
                            schemaInfo.ParameterEntities.Add( entityName, entity );
                        }
                    }
                    else {
                        if ( schemaInfo.GeneralEntities[ entityName ] == null ) {
                            schemaInfo.GeneralEntities.Add( entityName, entity );
                        }
                    }
                    entity.DeclaredInExternal = !ParsingInternalSubset;
                    entity.IsProcessed = true;
                    break;
                default:
                    goto UnexpectedError;
            }

            Token token = GetToken( true );
            switch ( token ) {
                case Token.PUBLIC:
                case Token.SYSTEM:
                    string systemId;
                    string publicId;
                    ParseExternalId( token, Token.EntityDecl, out publicId, out systemId );
                    entity.IsExternal = true;
                    entity.Url = systemId;
                    entity.Pubid = publicId;

                    if ( GetToken( false ) == Token.NData ) {
                        if ( isParamEntity ) {
                            ThrowUnexpectedToken( curPos - 5, ">" ); // 5 == strlen("NDATA")
                        }
                        if ( !whitespaceSeen ) { 
                            Throw( curPos - 5, Res.Xml_ExpectingWhiteSpace, "NDATA" );
                        }

                        if ( GetToken( true ) != Token.Name ) {
                            goto UnexpectedError;
                        }
                        
                        entity.NData = GetNameQualified( false );
                        string notationName = entity.NData.Name;
                        if ( schemaInfo.Notations[ notationName ] == null ) {
                            if ( undeclaredNotations == null ) {
                                undeclaredNotations = new Hashtable();
                            }
                            UndeclaredNotation un = new UndeclaredNotation( notationName, LineNo, LinePos - notationName.Length );
                            UndeclaredNotation loggedUn = (UndeclaredNotation)undeclaredNotations[notationName];
                            if ( loggedUn != null ) {
                                un.next = loggedUn.next;
                                loggedUn.next = un;
                            }
                            else {
                                undeclaredNotations.Add( notationName, un );
                            }
                        }
                    }
                    break;
                case Token.Literal:
                    entity.Text = GetValue();
                    entity.Line = (int)literalLineInfo.lineNo;
                    entity.Pos = (int)literalLineInfo.linePos;
                    break;
                default:
                    goto UnexpectedError;
            }

            if ( GetToken( false ) == Token.GreaterThan ) {
                entity.IsProcessed = false;
                return;
            }

        UnexpectedError:
            OnUnexpectedError();
        }

        private void ParseNotationDecl() {
            // notation name
            if ( GetToken( true ) != Token.Name ) {
                OnUnexpectedError();
            }

            SchemaNotation notation = null;
            XmlQualifiedName notationName = GetNameQualified( false ); 
            if ( schemaInfo.Notations[ notationName.Name ] == null ) {
                if ( undeclaredNotations != null ) {
                    undeclaredNotations.Remove( notationName.Name );
                }
                notation = new SchemaNotation( notationName );
                schemaInfo.Notations.Add(notation.Name.Name, notation);
                
            }
            else {
                // duplicate notation
                if ( validate ) {
                    SendValidationEvent( curPos - notationName.Name.Length, XmlSeverityType.Error, Res.Sch_DupNotation, notationName.Name );
                }
            }

            // public / system id
            Token token = GetToken( true );
            if ( token == Token.SYSTEM || token == Token.PUBLIC ) {
                string notationPublicId, notationSystemId;
                ParseExternalId( token, Token.NOTATION, out notationPublicId, out notationSystemId );
                if ( notation != null ) {
                    notation.SystemLiteral = notationSystemId;
                    notation.Pubid = notationPublicId;
                }
            }
            else {
                OnUnexpectedError();
            }

            if ( GetToken( false ) != Token.GreaterThan ) 
                OnUnexpectedError();
        }

        private void ParseComment() {
            SaveParsingBuffer();
            try {
                if ( SaveInternalSubsetValue ) {
                    readerAdapter.ParseComment( internalSubsetValueSb );
                    internalSubsetValueSb.Append( "-->" );
                }
                else {
                    readerAdapter.ParseComment( null );
                }
            }
            catch ( XmlException e ) {
                if ( e.ResString == Res.Xml_UnexpectedEOF && currentEntityId != 0 ) {
                    SendValidationEvent( XmlSeverityType.Error, Res.Sch_ParEntityRefNesting, null );
                }
                else {
                    throw;
                }
            }
            LoadParsingBuffer();
        }

        private void ParsePI() {
            SaveParsingBuffer();
            if ( SaveInternalSubsetValue ) {
                readerAdapter.ParsePI( internalSubsetValueSb );
                internalSubsetValueSb.Append( "?>" );
            }
            else {
                readerAdapter.ParsePI( null );
            }
            LoadParsingBuffer();
        }

        private void ParseCondSection() {
            int csEntityId = currentEntityId;

            switch ( GetToken( false ) ) {
                case Token.INCLUDE:
                    if ( GetToken( false ) != Token.RightBracket ) {
                        goto default;
                    }
                    if ( validate && csEntityId != currentEntityId ) {
                        SendValidationEvent( curPos, XmlSeverityType.Error, Res.Sch_ParEntityRefNesting, string.Empty );
                    }

                    if ( validate ) {
                        if ( condSectionEntityIds == null ) {
                            condSectionEntityIds = new int[CondSectionEntityIdsInitialSize];
                        }
                        else if ( condSectionEntityIds.Length == condSectionDepth ) {
                            int[] tmp = new int[condSectionEntityIds.Length*2];
                            Array.Copy( condSectionEntityIds, 0, tmp, 0, condSectionEntityIds.Length );
                            condSectionEntityIds = tmp;
                        }
                        condSectionEntityIds[condSectionDepth] = csEntityId;
                    }
                    condSectionDepth++;
                    break;
                case Token.IGNORE:
                    if ( GetToken( false ) != Token.RightBracket ) {
                        goto default;
                    }
                    if ( validate && csEntityId != currentEntityId ) {
                        SendValidationEvent( curPos, XmlSeverityType.Error, Res.Sch_ParEntityRefNesting, string.Empty );
                    }
                    // the content of the ignore section is parsed & skipped by scanning function
                    if ( GetToken( false ) != Token.CondSectionEnd ) {
                        goto default;
                    }
                    if ( validate && csEntityId != currentEntityId ) {
                        SendValidationEvent( curPos, XmlSeverityType.Error, Res.Sch_ParEntityRefNesting, string.Empty );
                    }
                    break;
                default:
                    OnUnexpectedError();
                    break;
            }
        }

        private void ParseExternalId( Token idTokenType, Token declType, out string publicId, out string systemId ) {
            LineInfo keywordLineInfo = new LineInfo( LineNo, LinePos - 6 );
            publicId = null;
            systemId = null;

            if ( GetToken( true ) != Token.Literal ) {
                ThrowUnexpectedToken( curPos, "\"", "'" );
            }

            if ( idTokenType == Token.SYSTEM ) {
                systemId = GetValue();

                if ( systemId.IndexOf( '#' ) >= 0 ) {
                    Throw( curPos - systemId.Length - 1, Res.Xml_FragmentId, new string[] { systemId.Substring( systemId.IndexOf( '#' ) ), systemId } );
                }

                if ( declType == Token.DOCTYPE && !freeFloatingDtd ) {
                    literalLineInfo.linePos++;
                    readerAdapter.OnSystemId( systemId, keywordLineInfo, literalLineInfo );
                }
            }
            else {
                Debug.Assert( idTokenType == Token.PUBLIC );
                publicId = GetValue();

                // verify if it contains chars valid for public ids
                int i;
                if ( ( i = xmlCharType.IsPublicId( publicId ) ) >= 0 ) {
                    ThrowInvalidChar( curPos - 1 - publicId.Length + i, publicId[i] );
                }

                if ( declType == Token.DOCTYPE && !freeFloatingDtd ) {
                    literalLineInfo.linePos++;
                    readerAdapter.OnPublicId( publicId, keywordLineInfo, literalLineInfo );

                    if ( GetToken( false ) == Token.Literal ) {
                        if ( !whitespaceSeen ) {
                            Throw( Res.Xml_ExpectingWhiteSpace, new string( literalQuoteChar, 1 ), (int)literalLineInfo.lineNo, (int)literalLineInfo.linePos );
                        }
                        systemId = GetValue();
                        literalLineInfo.linePos++;
                        readerAdapter.OnSystemId( systemId, keywordLineInfo, literalLineInfo );
                    }
                    else {
                        ThrowUnexpectedToken( curPos, "\"", "'" );
                    }
                }
                else {
                    if ( GetToken( false ) == Token.Literal ) {
                        if ( !whitespaceSeen ) {
                            Throw( Res.Xml_ExpectingWhiteSpace, new string( literalQuoteChar, 1 ), (int)literalLineInfo.lineNo, (int)literalLineInfo.linePos );
                        }
                        systemId = GetValue();
                    }
                    else if ( declType != Token.NOTATION ) {
                        ThrowUnexpectedToken( curPos, "\"", "'" );
                    } 
                }
            }
        }
//
// Scanning methods - works directly with parsing buffer
//
        private Token GetToken( bool needWhiteSpace ) {
           whitespaceSeen = false;
            for (;;) {
                switch ( chars[curPos] ) {
                    case (char)0:
                        if ( curPos == charsUsed ) {
                            goto ReadData;
                        }
                        else {
                            ThrowInvalidChar( curPos, chars[curPos] );
                        }
                        break;
                    case (char)0xA:
                        whitespaceSeen = true;
                        curPos++;
                        readerAdapter.OnNewLine( curPos );
                        continue;
                    case (char)0xD:
                        whitespaceSeen = true;
                        if ( chars[curPos+1] == (char)0xA ) {
                            if ( normalize ) {
                                SaveParsingBuffer();          // EOL normalization of 0xD 0xA
                                readerAdapter.CurrentPosition++;
                            }
                            curPos += 2;
                        }
                        else if ( curPos+1 < charsUsed || readerAdapter.IsEof ) {
                            chars[curPos] = (char)0xA;             // EOL normalization of 0xD
                            curPos++;
                        }
                        else {
                            goto ReadData;
                        }
                        readerAdapter.OnNewLine( curPos );
                        continue;
                    case (char)0x9:
                    case (char)0x20:
                        whitespaceSeen = true;
                        curPos++;
                        continue;
                    case '%':
                        if ( charsUsed - curPos < 2 ) {
                            goto ReadData;
                        }
                        if ( !xmlCharType.IsWhiteSpace( chars[curPos+1] ) ) {
                            if ( IgnoreEntityReferences ) {
                                curPos++;
                            }
                            else {
                                HandleEntityReference( true, false, false );
                            }
                            continue;
                        }
                        goto default;
                    default:
                        if ( needWhiteSpace && !whitespaceSeen && scanningFunction != ScanningFunction.ParamEntitySpace ) { 
                            Throw( curPos, Res.Xml_ExpectingWhiteSpace, ParseUnexpectedToken( curPos ) );
                        }
                        tokenStartPos = curPos;
                    SwitchAgain:
                        switch ( scanningFunction ) {
                            case ScanningFunction.Name:              return ScanNameExpected();
                            case ScanningFunction.QName:             return ScanQNameExpected();
                            case ScanningFunction.Nmtoken:           return ScanNmtokenExpected();
                            case ScanningFunction.SubsetContent:     return ScanSubsetContent();
                            case ScanningFunction.Doctype1:          return ScanDoctype1();
                            case ScanningFunction.Doctype2:          return ScanDoctype2();
                            case ScanningFunction.Element1:          return ScanElement1();
                            case ScanningFunction.Element2:          return ScanElement2();
                            case ScanningFunction.Element3:          return ScanElement3();
                            case ScanningFunction.Element4:          return ScanElement4();
                            case ScanningFunction.Element5:          return ScanElement5();
                            case ScanningFunction.Element6:          return ScanElement6();
                            case ScanningFunction.Element7:          return ScanElement7();
                            case ScanningFunction.Attlist1:          return ScanAttlist1();
                            case ScanningFunction.Attlist2:          return ScanAttlist2();
                            case ScanningFunction.Attlist3:          return ScanAttlist3();
                            case ScanningFunction.Attlist4:          return ScanAttlist4();
                            case ScanningFunction.Attlist5:          return ScanAttlist5();
                            case ScanningFunction.Attlist6:          return ScanAttlist6();
                            case ScanningFunction.Attlist7:          return ScanAttlist7();
                            case ScanningFunction.Notation1:         return ScanNotation1();
                            case ScanningFunction.SystemId:          return ScanSystemId();
                            case ScanningFunction.PublicId1:         return ScanPublicId1();
                            case ScanningFunction.PublicId2:         return ScanPublicId2();
                            case ScanningFunction.Entity1:           return ScanEntity1();
                            case ScanningFunction.Entity2:           return ScanEntity2();
                            case ScanningFunction.Entity3:           return ScanEntity3();
                            case ScanningFunction.CondSection1:      return ScanCondSection1();
                            case ScanningFunction.CondSection2:      return ScanCondSection2();
                            case ScanningFunction.CondSection3:      return ScanCondSection3();
                            case ScanningFunction.ClosingTag:        return ScanClosingTag();
                            case ScanningFunction.ParamEntitySpace:
                                whitespaceSeen = true;
                                scanningFunction = savedScanningFunction;
                                goto SwitchAgain;
                            default:
                                Debug.Assert( false );
                                return Token.None;
                        }
                }
            ReadData:
                if ( readerAdapter.IsEof || ReadData() == 0 ) {
                    if ( HandleEntityEnd( false ) ) {
                        continue;
                    }
                    if ( scanningFunction == ScanningFunction.SubsetContent ) {
                        return Token.Eof;
                    }
                    else {
                        Throw( curPos, Res.Xml_IncompleteDtdContent );
                    }
                }
            }
        }

        private Token ScanSubsetContent() {
            for (;;) {
                switch ( chars[curPos] ) {
                    case '<':
                        switch ( chars[curPos+1] ) {
                            case '!':
                                switch ( chars[curPos+2] ) {
                                    case 'E':
                                        if ( chars[curPos+3] == 'L' ) {
                                            if ( charsUsed - curPos < 9 ) {
                                                goto ReadData;
                                            }
                                            if ( chars[curPos+4] != 'E' || chars[curPos+5] != 'M' ||
                                                 chars[curPos+6] != 'E' || chars[curPos+7] != 'N' || 
                                                 chars[curPos+8] != 'T' ) {
                                                Throw( curPos, Res.Xml_ExpectDtdMarkup );
                                            }
                                            curPos += 9;
                                            scanningFunction = ScanningFunction.QName;
                                            nextScaningFunction = ScanningFunction.Element1;
                                            return Token.ElementDecl;
                                        }
                                        else if ( chars[curPos+3] == 'N' ) {
                                            if ( charsUsed - curPos < 8 ) {
                                                goto ReadData;
                                            }
                                            if ( chars[curPos+4] != 'T' || chars[curPos+5] != 'I' || 
                                                 chars[curPos+6] != 'T' || chars[curPos+7] != 'Y' ) {
                                                Throw( curPos, Res.Xml_ExpectDtdMarkup );
                                            }
                                            curPos += 8;
                                            scanningFunction = ScanningFunction.Entity1;
                                            return Token.EntityDecl;
                                        }
                                        else {
                                            if ( charsUsed - curPos < 4 ) {
                                                goto ReadData;
                                            }
                                            Throw( curPos, Res.Xml_ExpectDtdMarkup );
                                            return Token.None;
                                        }

                                    case 'A':
                                        if ( charsUsed - curPos < 9 ) {
                                            goto ReadData;
                                        }
                                        if ( chars[curPos+3] != 'T' || chars[curPos+4] != 'T' || 
                                             chars[curPos+5] != 'L' || chars[curPos+6] != 'I' || 
                                             chars[curPos+7] != 'S' || chars[curPos+8] != 'T' ) {
                                            Throw( curPos, Res.Xml_ExpectDtdMarkup );
                                        }
                                        curPos += 9;
                                        scanningFunction = ScanningFunction.QName;
                                        nextScaningFunction = ScanningFunction.Attlist1;
                                        return Token.AttlistDecl;

                                    case 'N':
                                        if ( charsUsed - curPos < 10 ) {
                                            goto ReadData;
                                        }
                                        if ( chars[curPos+3] != 'O' || chars[curPos+4] != 'T' || 
                                             chars[curPos+5] != 'A' || chars[curPos+6] != 'T' || 
                                             chars[curPos+7] != 'I' || chars[curPos+8] != 'O' ||
                                             chars[curPos+9] != 'N' ) {
                                            Throw( curPos, Res.Xml_ExpectDtdMarkup );
                                        }
                                        curPos += 10;
                                        scanningFunction = ScanningFunction.Name;
                                        nextScaningFunction = ScanningFunction.Notation1;
                                        return Token.NotationDecl;

                                    case '[':
                                        curPos += 3;
                                        scanningFunction = ScanningFunction.CondSection1;
                                        return Token.CondSectionStart;
                                    case '-':
                                        if ( chars[curPos+3] == '-' ) {
                                            curPos += 4;
                                            return Token.Comment;
                                        }
                                        else {
                                            if ( charsUsed - curPos < 4 ) {
                                                goto ReadData;
                                            }
                                            Throw( curPos, Res.Xml_ExpectDtdMarkup );
                                            break;
                                        }
                                    default:
                                        if ( charsUsed - curPos < 3 ) {
                                            goto ReadData;
                                        }
                                        Throw( curPos + 2, Res.Xml_ExpectDtdMarkup );
                                        break;
                                }
                                break;
                            case '?':
                                curPos += 2;
                                return Token.PI;
                            default:
                                if ( charsUsed - curPos < 2 ) {
                                    goto ReadData;
                                }
                                Throw( curPos, Res.Xml_ExpectDtdMarkup );
                                return Token.None;
                        }
                        break;
                    case ']':
                        if ( charsUsed - curPos < 2 && !readerAdapter.IsEof ) {
                            goto ReadData;
                        }
                        if ( chars[curPos+1] != ']' ) {
                            curPos++;  
                            scanningFunction = ScanningFunction.ClosingTag;
                            return Token.RightBracket;
                        }
                        if ( charsUsed - curPos < 3 && !readerAdapter.IsEof ) {
                            goto ReadData;
                        }
                        if ( chars[curPos+1] == ']' && chars[curPos+2] == '>' ) {
                            curPos += 3;
                            return Token.CondSectionEnd;
                        }
                        goto default;
                    default:
                        if ( charsUsed - curPos == 0 ) {
                            goto ReadData;
                        }
                        Throw( curPos, Res.Xml_ExpectDtdMarkup );
                        break;
                }
            ReadData:
                if ( ReadData() == 0 ) {
                    Throw( charsUsed, Res.Xml_IncompleteDtdContent );
                }
            }
        }

        private Token ScanNameExpected() {
            ScanName();
            scanningFunction = nextScaningFunction;
            return Token.Name;
        }

        private Token ScanQNameExpected() {
            ScanQName();
            scanningFunction = nextScaningFunction;
            return Token.QName;
        }

        private Token ScanNmtokenExpected() {
            ScanNmtoken();
            scanningFunction = nextScaningFunction;
            return Token.Nmtoken;
        }

        private Token ScanDoctype1() {
            switch ( chars[curPos] ) {
                case 'P':
                    if ( !EatPublicKeyword() ) {
                        Throw( curPos, Res.Xml_ExpectExternalOrClose );
                    }
                    nextScaningFunction = ScanningFunction.Doctype2;
                    scanningFunction = ScanningFunction.PublicId1;
                    return Token.PUBLIC;
                case 'S':
                    if ( !EatSystemKeyword() ) {
                        Throw( curPos, Res.Xml_ExpectExternalOrClose );
                    }
                    nextScaningFunction = ScanningFunction.Doctype2;
                    scanningFunction = ScanningFunction.SystemId;
                    return Token.SYSTEM;
                case '[':
                    curPos++;
                    scanningFunction = ScanningFunction.SubsetContent;
                    return Token.LeftBracket;
                case '>':
                    curPos++;
                    scanningFunction = ScanningFunction.SubsetContent;
                    return Token.GreaterThan;
                default:
                    Throw( curPos, Res.Xml_ExpectExternalOrClose );
                    return Token.None;
            }
        }

        private Token ScanDoctype2() {
            switch ( chars[curPos] ) {
                case '[':
                    curPos++;
                    scanningFunction = ScanningFunction.SubsetContent;
                    return Token.LeftBracket;
                case '>':
                    curPos++;
                    scanningFunction = ScanningFunction.SubsetContent;
                    return Token.GreaterThan;
                default:
                    Throw( curPos, Res.Xml_ExpectSubOrClose );
                    return Token.None;
            }
        }

        private Token ScanClosingTag() {
            if ( chars[curPos] != '>' ) {
                ThrowUnexpectedToken( curPos, ">" );
            }
            curPos++;
            scanningFunction = ScanningFunction.SubsetContent;
            return Token.GreaterThan;
        }

        private Token ScanElement1() {
            for (;;) {
                switch ( chars[curPos] ) {
                    case '(':
                        scanningFunction = ScanningFunction.Element2;
                        curPos++;
                        return Token.LeftParen;
                    case 'E':
                        if ( charsUsed - curPos < 5 ) {
                            goto ReadData;
                        }
                        if ( chars[curPos+1] == 'M' && chars[curPos+2] == 'P' &&
                             chars[curPos+3] == 'T' && chars[curPos+4] == 'Y' ) {
                            curPos += 5;
                            scanningFunction = ScanningFunction.ClosingTag;
                            return Token.EMPTY;
                        }
                        goto default;
                    case 'A':
                        if ( charsUsed - curPos < 3 ) {
                            goto ReadData;
                        }
                        if ( chars[curPos+1] == 'N' && chars[curPos+2] == 'Y' ) {
                            curPos += 3;
                            scanningFunction = ScanningFunction.ClosingTag;
                            return Token.ANY;
                        }
                        goto default;
                    default:
                        Throw( curPos, Res.Xml_InvalidContentModel );
                        break;
                }
            ReadData:
                if ( ReadData() == 0 ) {
                    Throw( curPos, Res.Xml_IncompleteDtdContent );
                }
            }
        }
        
        private Token ScanElement2() {
            if ( chars[curPos] == '#' ) {
                while ( charsUsed - curPos < 7 ) {
                    if ( ReadData() == 0 ) {
                        Throw( curPos, Res.Xml_IncompleteDtdContent );
                    }
                }
                if ( chars[curPos+1] == 'P' && chars[curPos+2] == 'C' &&
                     chars[curPos+3] == 'D' && chars[curPos+4] == 'A' &&
                     chars[curPos+5] == 'T' && chars[curPos+6] == 'A' ) {
                    curPos += 7;
                    scanningFunction = ScanningFunction.Element6;
                    return Token.PCDATA;
                }
                else {
                    Throw( curPos + 1, Res.Xml_ExpectPcData );
                }
            }

            scanningFunction = ScanningFunction.Element3;
            return Token.None;
        }

        private Token ScanElement3() {
            switch ( chars[curPos] ) {
                case '(':
                    curPos++;
                    return Token.LeftParen;
                case '>':
                    curPos++;
                    scanningFunction = ScanningFunction.SubsetContent;
                    return Token.GreaterThan;
                default:
                    ScanQName();
                    scanningFunction = ScanningFunction.Element4;
                    return Token.QName;
            }
        }

        private Token ScanElement4() {
            scanningFunction = ScanningFunction.Element5;

            Token t;
            switch ( chars[curPos] ) {
                case '*':
                    t = Token.Star;
                    break;
                case '?':
                    t = Token.QMark;
                    break;
                case '+':
                    t = Token.Plus;
                    break;
                default:
                    return Token.None;
            }
            if ( whitespaceSeen ) {
                Throw( curPos, Res.Xml_ExpectNoWhitespace );
            }
            curPos++;
            return t;
        }

        private Token ScanElement5() {
            switch ( chars[curPos] ) {
                case ',':
                    curPos++;
                    scanningFunction = ScanningFunction.Element3;
                    return Token.Comma;
                case '|':
                    curPos++;
                    scanningFunction = ScanningFunction.Element3;
                    return Token.Or;
                case ')':
                    curPos++;
                    scanningFunction = ScanningFunction.Element4;
                    return Token.RightParen;
                case '>':
                    curPos++;
                    scanningFunction = ScanningFunction.SubsetContent;
                    return Token.GreaterThan;
                default:
                    Throw( curPos, Res.Xml_ExpectOp );
                    return Token.None;
            }
        }

        private Token ScanElement6() {
            switch ( chars[curPos] ) {
                case ')':
                    curPos++;
                    scanningFunction = ScanningFunction.Element7;
                    return Token.RightParen;
                case '|':
                    curPos++;
                    nextScaningFunction = ScanningFunction.Element6;
                    scanningFunction = ScanningFunction.QName;
                    return Token.Or;
                default:
                    ThrowUnexpectedToken( curPos, ")", "|" );
                    return Token.None;
            }
        }

        private Token ScanElement7() {
            scanningFunction = ScanningFunction.ClosingTag;
            if ( chars[curPos] == '*' && !whitespaceSeen ) {
                curPos++;
                return Token.Star;
            }
            return Token.None;
        }

        private Token ScanAttlist1() {
            switch ( chars[curPos] ) {
                case '>':
                    curPos++;
                    scanningFunction = ScanningFunction.SubsetContent;
                    return Token.GreaterThan;
                default:
                    if ( !whitespaceSeen ) {
                        Throw( curPos, Res.Xml_ExpectingWhiteSpace, ParseUnexpectedToken( curPos ) );
                    }
                    ScanQName();
                    scanningFunction = ScanningFunction.Attlist2;
                    return Token.QName;
            }
        }

        private Token ScanAttlist2() {
            for (;;) {
                switch ( chars[curPos] ) {
                    case '(':
                        curPos++;
                        scanningFunction = ScanningFunction.Nmtoken;
                        nextScaningFunction = ScanningFunction.Attlist5;
                        return Token.LeftParen;
                    case 'C':
                        if ( charsUsed - curPos < 5 )
                            goto ReadData;
                        if ( chars[curPos+1] != 'D' || chars[curPos+2] != 'A' ||
                             chars[curPos+3] != 'T' || chars[curPos+4] != 'A' ) {
                            Throw( curPos, Res.Xml_InvalidAttributeType1 );
                        }
                        curPos += 5;
                        scanningFunction = ScanningFunction.Attlist6;
                        return Token.CDATA;
                    case 'E':
                        if ( charsUsed - curPos < 9 )
                            goto ReadData;
                        scanningFunction = ScanningFunction.Attlist6;
                        if ( chars[curPos+1] != 'N' || chars[curPos+2] != 'T' ||
                             chars[curPos+3] != 'I' || chars[curPos+4] != 'T' ) {
                            Throw( curPos, Res.Xml_InvalidAttributeType );
                        }
                        switch ( chars[curPos+5] ) {
                            case 'I':
                                if ( chars[curPos+6] != 'E' || chars[curPos+7] != 'S' ) {
                                    Throw( curPos, Res.Xml_InvalidAttributeType );
                                }
                                curPos += 8;
                                return Token.ENTITIES;
                            case 'Y':
                                curPos += 6;
                                return Token.ENTITY;
                            default:
                                Throw( curPos, Res.Xml_InvalidAttributeType );
                                break;
                        }
                        break;
                    case 'I':
                        if ( charsUsed - curPos < 6 )
                            goto ReadData;
                        scanningFunction = ScanningFunction.Attlist6;
                        if ( chars[curPos+1] != 'D' ) {
                            Throw( curPos, Res.Xml_InvalidAttributeType );
                        }

                        if ( chars[curPos+2] != 'R' ) {
                            curPos += 2;
                            return Token.ID;
                        }

                        if ( chars[curPos+3] != 'E' || chars[curPos+4] != 'F' ) {
                            Throw( curPos, Res.Xml_InvalidAttributeType );
                        }

                        if ( chars[curPos+5] != 'S' ) {
                            curPos += 5;
                            return Token.IDREF;
                        }
                        else {
                            curPos += 6;
                            return Token.IDREFS;
                        }
                    case 'N':
                        if ( charsUsed - curPos < 8 && !readerAdapter.IsEof ) {
                            goto ReadData;
                        }
                        switch ( chars[curPos+1] ) {
                            case 'O':
                                if ( chars[curPos+2] != 'T' || chars[curPos+3] != 'A' || 
                                     chars[curPos+4] != 'T' || chars[curPos+5] != 'I' || 
                                     chars[curPos+6] != 'O' || chars[curPos+7] != 'N' ) {
                                    Throw( curPos, Res.Xml_InvalidAttributeType );
                                }
                                curPos += 8;
                                scanningFunction = ScanningFunction.Attlist3;
                                return Token.NOTATION;
                            case 'M':
                                if ( chars[curPos+2] != 'T' || chars[curPos+3] != 'O' || 
                                     chars[curPos+4] != 'K' || chars[curPos+5] != 'E' || 
                                    chars[curPos+6] != 'N' ) {
                                    Throw( curPos, Res.Xml_InvalidAttributeType );
                                }
                                scanningFunction = ScanningFunction.Attlist6;

                                if ( chars[curPos+7] == 'S' ) {
                                    curPos += 8;
                                    return Token.NMTOKENS;
                                }
                                else {
                                    curPos += 7;
                                    return Token.NMTOKEN;
                                }
                            default:
                                Throw( curPos, Res.Xml_InvalidAttributeType );
                                break;
                        }
                        break;
                    default:
                        Throw( curPos, Res.Xml_InvalidAttributeType );
                        break;
                }

            ReadData:
                if ( ReadData() == 0 ) {
                    Throw( curPos, Res.Xml_IncompleteDtdContent );
                }
            }
        }

        private Token ScanAttlist3() {
            if ( chars[curPos] == '(' ) {
                curPos++;
                scanningFunction = ScanningFunction.Name;
                nextScaningFunction = ScanningFunction.Attlist4;
                return Token.LeftParen;
            }
            else {
                ThrowUnexpectedToken( curPos, "(" );
                return Token.None;
            }
        }

        private Token ScanAttlist4() {
            switch ( chars[curPos] ) {
                case ')':
                    curPos++;
                    scanningFunction = ScanningFunction.Attlist6;
                    return Token.RightParen;
                case '|':
                    curPos++;
                    scanningFunction = ScanningFunction.Name;
                    nextScaningFunction = ScanningFunction.Attlist4;
                    return Token.Or;
                default:
                    ThrowUnexpectedToken( curPos, ")", "|" );
                    return Token.None;
            }
        }

        private Token ScanAttlist5() {
            switch ( chars[curPos] ) {
                case ')':
                    curPos++;
                    scanningFunction = ScanningFunction.Attlist6;
                    return Token.RightParen;
                case '|':
                    curPos++;
                    scanningFunction = ScanningFunction.Nmtoken;
                    nextScaningFunction = ScanningFunction.Attlist5;
                    return Token.Or;
                default:
                    ThrowUnexpectedToken( curPos, ")", "|" );
                    return Token.None;
                    
            }
        }

        private Token ScanAttlist6() {
            for (;;) {
                switch ( chars[curPos] ) {
                    case '"':
                    case '\'':
                        ScanLiteral( LiteralType.AttributeValue );
                        scanningFunction = ScanningFunction.Attlist1;
                        return Token.Literal;
                    case '#':
                        if ( charsUsed - curPos < 6 )
                            goto ReadData;
                        switch ( chars[curPos+1] ) {
                            case 'R':
                                if ( charsUsed - curPos < 9 )
                                    goto ReadData;
                                if ( chars[curPos+2] != 'E' || chars[curPos+3] != 'Q' ||
                                     chars[curPos+4] != 'U' || chars[curPos+5] != 'I' ||
                                     chars[curPos+6] != 'R' || chars[curPos+7] != 'E' ||
                                     chars[curPos+8] != 'D' ) {
                                    Throw( curPos, Res.Xml_ExpectAttType );
                                }
                                curPos += 9;
                                scanningFunction = ScanningFunction.Attlist1;
                                return Token.REQUIRED;
                            case 'I':
                                if ( charsUsed - curPos < 8 )
                                    goto ReadData;
                                if ( chars[curPos+2] != 'M' || chars[curPos+3] != 'P' ||
                                     chars[curPos+4] != 'L' || chars[curPos+5] != 'I' ||
                                     chars[curPos+6] != 'E' || chars[curPos+7] != 'D' ) {
                                    Throw( curPos, Res.Xml_ExpectAttType );
                                }
                                curPos += 8;
                                scanningFunction = ScanningFunction.Attlist1;
                                return Token.IMPLIED;
                            case 'F':
                                if ( chars[curPos+2] != 'I' || chars[curPos+3] != 'X' ||
                                     chars[curPos+4] != 'E' || chars[curPos+5] != 'D' ) {
                                    Throw( curPos, Res.Xml_ExpectAttType );
                                }
                                curPos += 6;
                                scanningFunction = ScanningFunction.Attlist7;
                                return Token.FIXED;
                            default:
                                Throw( curPos, Res.Xml_ExpectAttType );
                                break;
                        }
                        break;
                    default:
                        Throw( curPos, Res.Xml_ExpectAttType );
                        break;
                }
            ReadData:
                if ( ReadData() == 0 ) {
                    Throw( curPos, Res.Xml_IncompleteDtdContent );
                }
            }
        }

        private Token ScanAttlist7() {
            switch ( chars[curPos] ) {
                case '"':
                case '\'':
                    ScanLiteral( LiteralType.AttributeValue );
                    scanningFunction = ScanningFunction.Attlist1;
                    return Token.Literal;
                default:
                    Debug.Assert( false, "We should never get to this point." );
                    ThrowUnexpectedToken( curPos, "\"", "'" );
                    return Token.None;
            }
        }

        private Token ScanLiteral( LiteralType literalType ) {
            Debug.Assert( chars[curPos] == '"' || chars[curPos] == '\'' );
            
            char quoteChar = chars[curPos];
            char replaceChar = ( literalType == LiteralType.AttributeValue ) ? (char)0x20 : (char)0xA;
            int startQuoteEntityId = currentEntityId;

            literalLineInfo.Set( LineNo, LinePos );

            curPos++;
            tokenStartPos = curPos;

            stringBuilder.Length = 0;

            for (;;) {
                unsafe {
                    while ( ( xmlCharType.charProperties[chars[curPos]] & XmlCharType.fAttrValue ) != 0 && chars[curPos] != '%' ) {
                        curPos++;
                    }
                }

                if ( chars[curPos] == quoteChar && currentEntityId == startQuoteEntityId ) {
                    if ( stringBuilder.Length > 0 ) {
                        stringBuilder.Append( chars, tokenStartPos, curPos - tokenStartPos );
                    }
                    curPos++;
                    literalQuoteChar = quoteChar;
                    return Token.Literal;
                }

                int tmp1 = curPos - tokenStartPos;
                if ( tmp1 > 0 ) {
                    stringBuilder.Append( chars, tokenStartPos, tmp1 );
                    tokenStartPos = curPos;
                }

                switch ( chars[curPos] ) {
                    case '"':
                    case '\'':
                    case '>':
                        curPos++;
                        continue;
                    // eol
                    case (char)0xA:
                        curPos++;
                        if ( normalize ) {
                            stringBuilder.Append( replaceChar );        // For attributes: CDATA normalization of 0xA
                            tokenStartPos = curPos;
                        }
                        readerAdapter.OnNewLine( curPos );
                        continue;
                    case (char)0xD:
                        if ( chars[curPos+1] == (char)0xA ) {
                            if ( normalize ) {
                                if ( literalType == LiteralType.AttributeValue ) {
                                    stringBuilder.Append( readerAdapter.IsEntityEolNormalized ? "\u0020\u0020" : "\u0020" ); // CDATA normalization of 0xD 0xA
                                }
                                else {
                                    stringBuilder.Append( readerAdapter.IsEntityEolNormalized ? "\u000D\u000A" : "\u000A" ); // EOL normalization of 0xD 0xA									
                                }
                                tokenStartPos = curPos + 2;

                                SaveParsingBuffer();          // EOL normalization of 0xD 0xA in internal subset value
                                readerAdapter.CurrentPosition++;
                            }
                            curPos += 2;
                        }
                        else if ( curPos+1 == charsUsed ) {
                            goto ReadData;
                        }
                        else {
                            curPos++;
                            if ( normalize ) {
                                stringBuilder.Append( replaceChar ); // For attributes: CDATA normalization of 0xD and 0xD 0xA
                                tokenStartPos = curPos;              // For entities:   EOL normalization of 0xD and 0xD 0xA
                            }
                        }
                        readerAdapter.OnNewLine( curPos );
                        continue;
                    // tab
                    case (char)0x9:
                        if ( literalType == LiteralType.AttributeValue && normalize ) {
                            stringBuilder.Append( (char)0x20 );      // For attributes: CDATA normalization of 0x9
                            tokenStartPos++;
                        }
                        curPos++;
                        continue;
                    // attribute values cannot contain '<'
                    case '<':
                        if ( literalType == LiteralType.AttributeValue ) {
                            Throw( curPos, Res.Xml_BadAttributeChar, XmlException.BuildCharExceptionStr( '<' ) );
                        }
                        curPos++;
                        continue;
                    // parameter entity reference
                    case '%':
                        if ( literalType != LiteralType.EntityReplText ) {
                            curPos++;
                            continue;
                        }
                        HandleEntityReference( true, true, literalType == LiteralType.AttributeValue );
                        tokenStartPos = curPos;
                        continue;
                    // general entity reference
                    case '&':
                        if ( literalType == LiteralType.SystemOrPublicID ) {
                            curPos++;
                            continue;
                        }
                        if ( curPos + 1 == charsUsed ) {
                            goto ReadData;
                        }
                        // numeric characters reference
                        if ( chars[curPos + 1] == '#' ) {
                            SaveParsingBuffer();
                            int endPos = readerAdapter.ParseNumericCharRef( SaveInternalSubsetValue ? internalSubsetValueSb : null );
                            LoadParsingBuffer();
                            stringBuilder.Append( chars, curPos, endPos - curPos );
                            readerAdapter.CurrentPosition = endPos;
                            tokenStartPos = endPos;
                            curPos = endPos;
                            continue;
                        }
                        else {
                            // general entity reference
                            SaveParsingBuffer();
                            if ( literalType == LiteralType.AttributeValue ) {
                                int endPos = readerAdapter.ParseNamedCharRef( true, SaveInternalSubsetValue ? internalSubsetValueSb : null );
                                LoadParsingBuffer();

                                if ( endPos >= 0 ) {
                                    stringBuilder.Append( chars, curPos, endPos - curPos );
                                    readerAdapter.CurrentPosition = endPos;
                                    tokenStartPos = endPos;
                                    curPos = endPos;
                                    continue;
                                }
                                else {
                                    HandleEntityReference( false, true, true );
                                    tokenStartPos = curPos;
                                }
                                continue;
                            }
                            else {
                                int endPos = readerAdapter.ParseNamedCharRef( false, null );
                                LoadParsingBuffer();
    
                                if ( endPos >= 0 ) {
                                    tokenStartPos = curPos;
                                    curPos = endPos;
                                    continue;
                                }
                                else {
                                    stringBuilder.Append( '&' );
                                    curPos++;
                                    tokenStartPos = curPos;
                                    // Bypass general entities in entity values
                                    XmlQualifiedName entityName = ScanEntityName();
                                    VerifyEntityReference( entityName, false, false, false );
                                }
                                continue;
                            }
                        }
                    default:
                        // end of buffer
                        if ( curPos == charsUsed ) {
                            goto ReadData;
                        }
                        // surrogate chars
                        else { 
                            char ch = chars[curPos];
                            if ( ch >= 0xd800 && ch <=0xdbff ) {
                                if ( curPos + 1 == charsUsed ) {
                                    goto ReadData;
                                }
                                curPos++;
                                if ( chars[curPos] >= 0xdc00 && chars[curPos] <= 0xdfff) {
                                    curPos++;
                                    continue;
                                }
                            }
                            ThrowInvalidChar( curPos, ch );
                            return Token.None;
                        }
                }

            ReadData:
                Debug.Assert( curPos - tokenStartPos == 0 );

                // read new characters into the buffer
                if ( readerAdapter.IsEof || ReadData() == 0 ) {
                    if ( literalType == LiteralType.SystemOrPublicID || !HandleEntityEnd( true ) ) {
                        Throw( curPos, Res.Xml_UnclosedQuote );
                    }
                }
                tokenStartPos = curPos;
            }
        }

        private XmlQualifiedName ScanEntityName() {
            try {
                ScanName();
            }
            catch ( XmlException e ) {
                Throw( Res.Xml_ErrorParsingEntityName, string.Empty, e.LineNumber, e.LinePosition );
            }

            if ( chars[curPos] != ';' ) {
                ThrowUnexpectedToken( curPos, ";" );
            }
            XmlQualifiedName entityName = GetNameQualified( false );
            curPos++;

            return entityName;
        }

        private Token ScanNotation1() {
            switch ( chars[curPos] ) {
                case 'P':
                    if ( !EatPublicKeyword() ) {
                        Throw( curPos, Res.Xml_ExpectExternalOrClose );
                    }
                    nextScaningFunction = ScanningFunction.ClosingTag;
                    scanningFunction = ScanningFunction.PublicId1;
                    return Token.PUBLIC;
                case 'S':
                    if ( !EatSystemKeyword() ) {
                        Throw( curPos, Res.Xml_ExpectExternalOrClose );
                    }
                    nextScaningFunction = ScanningFunction.ClosingTag;
                    scanningFunction = ScanningFunction.SystemId;
                    return Token.SYSTEM;
                default:
                    Throw( curPos, Res.Xml_ExpectExternalOrPublicId );
                    return Token.None;
            }
        }

        private Token ScanSystemId() {
            if ( chars[curPos] != '"' && chars[curPos] != '\'' ) {
                ThrowUnexpectedToken( curPos, "\"", "'" );
            }

            ScanLiteral( LiteralType.SystemOrPublicID );

            scanningFunction = nextScaningFunction;
            return Token.Literal;
        }

        private Token ScanEntity1() {
            if ( chars[curPos] == '%' ) {
                curPos++;
                nextScaningFunction = ScanningFunction.Entity2;
                scanningFunction = ScanningFunction.Name;
                return Token.Percent;
            }
            else {
                ScanName();
                scanningFunction = ScanningFunction.Entity2;
                return Token.Name;
            }
        }

        private Token ScanEntity2() {
            switch ( chars[curPos] ) {
                case 'P':
                    if ( !EatPublicKeyword() ) {
                        Throw( curPos, Res.Xml_ExpectExternalOrClose );
                    }
                    nextScaningFunction = ScanningFunction.Entity3;
                    scanningFunction = ScanningFunction.PublicId1;
                    return Token.PUBLIC;
                case 'S':
                    if ( !EatSystemKeyword() ) {
                        Throw( curPos, Res.Xml_ExpectExternalOrClose );
                    }
                    nextScaningFunction = ScanningFunction.Entity3;
                    scanningFunction = ScanningFunction.SystemId;
                    return Token.SYSTEM;

                case '"':
                case '\'':
                    ScanLiteral( LiteralType.EntityReplText );
                    scanningFunction = ScanningFunction.ClosingTag;
                    return Token.Literal;
                default:
                    Throw( curPos, Res.Xml_ExpectExternalIdOrEntityValue );
                    return Token.None;
            }
        }

        private Token ScanEntity3() {
            if ( chars[curPos] == 'N' ) {
                while ( charsUsed - curPos < 5 ) {
                    if ( ReadData() == 0 ) {
                        goto End;
                    }
                }
                if ( chars[curPos+1] == 'D' && chars[curPos+2] == 'A' && 
                     chars[curPos+3] == 'T' && chars[curPos+4] == 'A' ) {
                    curPos += 5;
                    scanningFunction = ScanningFunction.Name;
                    nextScaningFunction = ScanningFunction.ClosingTag;
                    return Token.NData;
                }
            }
        End:
            scanningFunction = ScanningFunction.ClosingTag;
            return Token.None;
        }

        private Token ScanPublicId1() {
            if ( chars[curPos] != '"' && chars[curPos] != '\'' ) {
                ThrowUnexpectedToken( curPos, "\"", "'" );
            }

            ScanLiteral( LiteralType.SystemOrPublicID );

            scanningFunction = ScanningFunction.PublicId2;
            return Token.Literal;
        }

        private Token ScanPublicId2() {
            if ( chars[curPos] != '"' && chars[curPos] != '\'' ) {
                scanningFunction = nextScaningFunction;
                return Token.None;
            }

            ScanLiteral( LiteralType.SystemOrPublicID );
            scanningFunction = nextScaningFunction;

            return Token.Literal;
        }

        private Token ScanCondSection1() {
            if ( chars[curPos] != 'I' ) {
                Throw( curPos, Res.Xml_ExpectIgnoreOrInclude );
            }
            curPos++;

            for (;;) {
                if ( charsUsed - curPos < 5 ) { 
                    goto ReadData;
                }
                switch ( chars[curPos] ) {
                    case 'N':
                        if ( charsUsed - curPos < 6 ) { 
                            goto ReadData;
                        }
                        if ( chars[curPos+1] != 'C' || chars[curPos+2] != 'L' ||
                             chars[curPos+3] != 'U' || chars[curPos+4] != 'D' || 
                             chars[curPos+5] != 'E' || xmlCharType.IsNameChar( chars[curPos+6] ) ) {
                            goto default;
                        }
                        nextScaningFunction = ScanningFunction.SubsetContent;
                        scanningFunction = ScanningFunction.CondSection2;
                        curPos += 6;
                        return Token.INCLUDE;
                    case 'G':
                        if ( chars[curPos+1] != 'N' || chars[curPos+2] != 'O' ||
                             chars[curPos+3] != 'R' || chars[curPos+4] != 'E' ||
                             xmlCharType.IsNameChar( chars[curPos+5] ) ) {
                            goto default;
                        }
                        nextScaningFunction = ScanningFunction.CondSection3;
                        scanningFunction = ScanningFunction.CondSection2;
                        curPos += 5;
                        return Token.IGNORE;
                    default:
                        Throw( curPos - 1, Res.Xml_ExpectIgnoreOrInclude );
                        return Token.None;
                }
            ReadData:
                if ( ReadData() == 0 ) {
                    Throw( curPos, Res.Xml_IncompleteDtdContent );
                }
            }
        }

        private Token ScanCondSection2() {
            if ( chars[curPos] != '[' ) {
                ThrowUnexpectedToken( curPos, "[" );
            }
            curPos++;
            scanningFunction = nextScaningFunction;
            return Token.RightBracket;
        }

        private Token ScanCondSection3() {
            int ignoreSectionDepth = 0;

            // skip ignored part
            for (;;) {
                unsafe {
                    while ( ( xmlCharType.charProperties[chars[curPos]] & XmlCharType.fText ) != 0 && chars[curPos] != ']' ) {
                        curPos++;
                    }
                }

                switch ( chars[curPos] ) {
                    case '"':
                    case '\'':
                    case (char)0x9:
                    case '&':
                        curPos++;
                        continue;
                    // eol
                    case (char)0xA:
                        curPos++;
                        readerAdapter.OnNewLine( curPos );
                        continue;
                    case (char)0xD:
                        Debug.Assert( !ParsingInternalSubset ); // no need to normalize
                        if ( chars[curPos+1] == (char)0xA ) {
                            curPos += 2;
                        }
                        else if ( curPos+1 < charsUsed || readerAdapter.IsEof ) {
                            curPos++;
                        }
                        else {
                            goto ReadData;
                        }
                        readerAdapter.OnNewLine( curPos );
                        continue;
                    case '<':
                        if ( charsUsed - curPos < 3 ) {
                            goto ReadData;
                        }
                        if ( chars[curPos+1] != '!' || chars[curPos+2] != '[' ) {
                            curPos++;
                            continue;
                        }
                        ignoreSectionDepth++;
                        curPos += 3;
                        continue;
                    case ']':
                        if ( charsUsed - curPos < 3 ) {
                            goto ReadData;
                        }
                        if ( chars[curPos+1] != ']' || chars[curPos+2] != '>' ) {
                            curPos++;
                            continue;
                        }
                        if ( ignoreSectionDepth > 0 ) {
                            ignoreSectionDepth--;
                            curPos += 3;
                            continue;
                        }
                        else {
                            curPos += 3;
                            scanningFunction = ScanningFunction.SubsetContent;
                            return Token.CondSectionEnd;
                        }
                    default:
                        // end of buffer
                        if ( curPos == charsUsed ) {
                            goto ReadData;
                        }
                        // surrogate chars
                        else { 
                            char ch = chars[curPos];
                            if ( ch >= 0xd800 && ch <=0xdbff ) {
                                if ( curPos + 1 == charsUsed ) {
                                    goto ReadData;
                                }
                                curPos++;
                                if ( chars[curPos] >= 0xdc00 && chars[curPos] <= 0xdfff) {
                                    curPos++;
                                    continue;
                                }
                            }
                            ThrowInvalidChar( curPos, ch );
                            return Token.None;
                        }
                }

            ReadData:
                // read new characters into the buffer
                if ( readerAdapter.IsEof || ReadData() == 0 ) {
                    if ( HandleEntityEnd( false ) ) {
                        continue;
                    }
                    Throw( curPos, Res.Xml_UnclosedConditionalSection );
                }
                tokenStartPos = curPos;
            }
        }

        private void ScanName() {
            ScanQName( false );
        }

        private void ScanQName() {
            ScanQName( supportNamespaces );
        }

        private void ScanQName( bool isQName ) {
            tokenStartPos = curPos;
            int colonOffset = -1;

            for (;;) {
                unsafe {
                    if ( ( xmlCharType.charProperties[chars[curPos]] & XmlCharType.fNCStartName ) == 0 && chars[curPos] != ':' ) {
                        if ( curPos == charsUsed ) {
                            if ( ReadDataInName() ) {
                                continue;
                            }
                            Throw( curPos, Res.Xml_UnexpectedEOF, "Name" );
                        }
                        else if ( chars[curPos] != ':' || supportNamespaces ) {
                            Throw( curPos, Res.Xml_BadStartNameChar, XmlException.BuildCharExceptionStr( chars[curPos] ) );
                        }
                    }
                }
                curPos++;

            ContinueName:
                unsafe {
                    while ( ( xmlCharType.charProperties[chars[curPos]] & XmlCharType.fNCName ) != 0 ) {
                        curPos++;
                    }
                }

                if ( chars[curPos] == ':' ) {
                    if ( isQName ) {
                        if ( colonOffset != -1 ) {
                            Throw( curPos, Res.Xml_BadNameChar, XmlException.BuildCharExceptionStr( ':' ));
                        }
                        colonOffset = curPos - tokenStartPos;
                        curPos++;
                        continue;
                    }
                    else {
                        curPos++;
                        goto ContinueName;
                    }
                }
                // end of buffer
                else if ( charsUsed - curPos == 0 ) {
                    if ( ReadDataInName() ) {
                        goto ContinueName;
                    }
                    if ( tokenStartPos == curPos ) {
                        Throw( curPos, Res.Xml_UnexpectedEOF, "Name" );
                    }
                }
                // end of name
                colonPos = ( colonOffset == -1 ) ? -1 : tokenStartPos + colonOffset;
                return;
            }
        }

        private bool ReadDataInName() {
            int offset = curPos - tokenStartPos;
            curPos = tokenStartPos;
            bool newDataRead = ( ReadData() != 0 );
            tokenStartPos = curPos;
            curPos += offset;
            return newDataRead;
        }

        private void ScanNmtoken() {
            tokenStartPos = curPos;

            for (;;) {
                unsafe {
                    while ( ( xmlCharType.charProperties[chars[curPos]] & XmlCharType.fNCName ) != 0 || chars[curPos] == ':' ) {
                        curPos++;
                    }
                }

                if ( chars[curPos] != 0 ) {
                    if ( curPos - tokenStartPos == 0 ) {
                        Throw( curPos, Res.Xml_BadNameChar, XmlException.BuildCharExceptionStr( chars[curPos] ) );
                    }
                    return;
                }

                int len = curPos - tokenStartPos;
                curPos = tokenStartPos;
                if ( ReadData() == 0 ) {
                    if ( len > 0 ) {
                        tokenStartPos = curPos;
                        curPos += len;
                        return;
                    }
                    Throw( curPos, Res.Xml_UnexpectedEOF, "NmToken" );
                }
                tokenStartPos = curPos;
                curPos += len;
            }
        }

        private bool EatPublicKeyword() { 
            Debug.Assert( chars[curPos] == 'P' );
            while ( charsUsed - curPos < 6 ) {
                if ( ReadData() == 0 ) {
                    return false;
                }
            }
            if ( chars[curPos+1] != 'U' || chars[curPos+2] != 'B' ||
                 chars[curPos+3] != 'L' || chars[curPos+4] != 'I' ||
                 chars[curPos+5] != 'C' ) {
                return false;
            }
            curPos += 6;
            return true;
        }

        private bool EatSystemKeyword() { 
            Debug.Assert( chars[curPos] == 'S' );
            while ( charsUsed - curPos < 6 ) {
                if ( ReadData() == 0 ) {
                    return false;
                }
            }
            if ( chars[curPos+1] != 'Y' || chars[curPos+2] != 'S' ||
                 chars[curPos+3] != 'T' || chars[curPos+4] != 'E' ||
                 chars[curPos+5] != 'M' ) {
                return false;
            }
            curPos += 6;
            return true;
        }

//
// Scanned data getters
//
        private XmlQualifiedName GetNameQualified( bool canHavePrefix ) {
            Debug.Assert( curPos - tokenStartPos > 0 );
            if ( colonPos == -1 ) {
                return new XmlQualifiedName( nameTable.Add( chars, tokenStartPos, curPos - tokenStartPos ) );
            }
            else {
                if ( canHavePrefix ) {
                    return new XmlQualifiedName( nameTable.Add( chars, colonPos + 1, curPos - colonPos - 1 ),
                                                 nameTable.Add( chars, tokenStartPos, colonPos - tokenStartPos ) );
                }
                else {
                    Throw( tokenStartPos, Res.Xml_ColonInLocalName, GetNameString() );
                    return null;
                }
            }
        }

        private string GetNameString() {
            Debug.Assert( curPos - tokenStartPos > 0 );
            return new string( chars, tokenStartPos, curPos - tokenStartPos );
        }

        private string GetNmtokenString() {
            return GetNameString();
        }

        private string GetValue() {
            if ( stringBuilder.Length == 0 ) {
                return new string( chars, tokenStartPos, curPos - tokenStartPos - 1 );
            }
            else {
                return stringBuilder.ToString();
            }
        }

        private string GetValueWithStrippedSpaces() {
            Debug.Assert( curPos == 0 || chars[curPos-1] == '"' || chars[curPos-1] == '\'' );
            if ( stringBuilder.Length == 0 ) {
                int len = curPos - tokenStartPos - 1;
                XmlComplianceUtil.StripSpaces( chars, tokenStartPos, ref len );
                return new string( chars, tokenStartPos, len );
            }
            else {
                return XmlComplianceUtil.StripSpaces( stringBuilder.ToString() );
            }
        }
//
// Parsing buffer maintainance methods
//
        int ReadData() {
            SaveParsingBuffer();
            int charsRead = readerAdapter.ReadData();
            LoadParsingBuffer();
            return charsRead;
        }

        private void LoadParsingBuffer() {
            chars = readerAdapter.ParsingBuffer;
            charsUsed = readerAdapter.ParsingBufferLength;
            curPos = readerAdapter.CurrentPosition;
        }

        private void SaveParsingBuffer() {
            SaveParsingBuffer( curPos );
        }

        private void SaveParsingBuffer( int internalSubsetValueEndPos ) {
            if ( SaveInternalSubsetValue ) {
                Debug.Assert( internalSubsetValueSb != null );

                int readerCurPos = readerAdapter.CurrentPosition;
                if ( internalSubsetValueEndPos - readerCurPos > 0 ) {
                    internalSubsetValueSb.Append( chars, readerCurPos, internalSubsetValueEndPos - readerCurPos );
                }
            }
            readerAdapter.CurrentPosition = curPos;
        }

//
// Entity handling
//
        private bool HandleEntityReference( bool paramEntity, bool inLiteral, bool inAttribute ) {
            Debug.Assert( chars[curPos] == '&' || chars[curPos] == '%' );
            curPos++;
            return HandleEntityReference( ScanEntityName(), paramEntity, inLiteral, inAttribute );
        }

        private bool HandleEntityReference( XmlQualifiedName entityName, bool paramEntity, bool inLiteral, bool inAttribute ) {
            Debug.Assert( chars[curPos-1] == ';' );

            SaveParsingBuffer();
            if ( paramEntity && ParsingInternalSubset && !ParsingTopLevelMarkup ) {
                Throw( curPos - entityName.Name.Length - 1, Res.Xml_InvalidParEntityRef );
            }

            SchemaEntity entity = VerifyEntityReference( entityName, paramEntity, true, inAttribute );
            if ( entity == null ) {
                return false;
            }
            if ( entity.IsProcessed ) {
                Throw( curPos - entityName.Name.Length - 1, paramEntity ? Res.Xml_RecursiveParEntity : Res.Xml_RecursiveGenEntity, entityName.Name );
            }

            int newEntityId = nextEntityId++;

            if ( entity.IsExternal ) {
                if ( !readerAdapter.PushEntity( entity, newEntityId ) ) {
                    return false;
                }
                externalEntitiesDepth++;
            }
            else {
                if ( entity.Text.Length == 0 ) {
                    return false;
                }
                if ( !readerAdapter.PushEntity( entity, newEntityId ) ) {
                    return false;
                }
            }
            currentEntityId = newEntityId;

            if ( paramEntity && !inLiteral && scanningFunction != ScanningFunction.ParamEntitySpace ) {
                savedScanningFunction = scanningFunction;
                scanningFunction = ScanningFunction.ParamEntitySpace;
            }

            LoadParsingBuffer();
            return true;
        }

        private bool HandleEntityEnd( bool inLiteral ) {
            SaveParsingBuffer();
            
            SchemaEntity oldEntity;
            if ( !readerAdapter.PopEntity( out oldEntity, out currentEntityId ) ) {
                return false;
            }
            LoadParsingBuffer();

            if ( oldEntity == null  ) {
                // external subset popped
                Debug.Assert( !ParsingInternalSubset || freeFloatingDtd );
                Debug.Assert( currentEntityId == 0 );
                if ( scanningFunction == ScanningFunction.ParamEntitySpace ) {
                    scanningFunction = savedScanningFunction;
                }
                return false;
            }

            if ( oldEntity.IsExternal ) {
                externalEntitiesDepth--;
            }

            if ( !inLiteral && scanningFunction != ScanningFunction.ParamEntitySpace ) {
                savedScanningFunction = scanningFunction;
                scanningFunction = ScanningFunction.ParamEntitySpace;
            }

            return true;
        }

        private SchemaEntity VerifyEntityReference( XmlQualifiedName entityName, bool paramEntity, bool mustBeDeclared, bool inAttribute ) {
            Debug.Assert( chars[curPos-1] == ';' );

            SchemaEntity entity;
            if ( paramEntity ) {
                entity = (SchemaEntity)schemaInfo.ParameterEntities[entityName];
            }
            else {
                entity = (SchemaEntity)schemaInfo.GeneralEntities[entityName];
            }

            if ( entity == null ) {
                if ( paramEntity ) {
                    if ( validate ) {
                        SendValidationEvent( curPos - entityName.Name.Length - 1, XmlSeverityType.Error, Res.Xml_UndeclaredParEntity, entityName.Name );
                    }
                }
                else if ( mustBeDeclared ) {
                    if ( !ParsingInternalSubset ) {
                        SendValidationEvent( curPos - entityName.Name.Length - 1, XmlSeverityType.Error, Res.Xml_UndeclaredEntity, entityName.Name );
                    }
                    else {
                        Throw( curPos - entityName.Name.Length - 1, Res.Xml_UndeclaredEntity, entityName.Name );
                    }
                }
                return null;
            }

            if ( !entity.NData.IsEmpty ) {
                Throw( curPos - entityName.Name.Length - 1, Res.Xml_UnparsedEntityRef, entityName.Name ); 
            }

            if ( inAttribute && entity.IsExternal ) {
                Throw( curPos - entityName.Name.Length - 1, Res.Xml_ExternalEntityInAttValue, entityName.Name );
            }

            return entity;
        }

//
// Helper methods and properties
//
        private void SendValidationEvent( int pos, XmlSeverityType severity, string code, string arg ) {
            Debug.Assert( validate );
            SendValidationEvent( severity, new XmlSchemaException( code, arg, BaseUriStr, (int)LineNo, (int)LinePos + ( pos - curPos ) ) );
        }

        private void SendValidationEvent( XmlSeverityType severity, string code, string arg ) {
            Debug.Assert( validate );
            SendValidationEvent( severity, new XmlSchemaException( code, arg, BaseUriStr, (int)LineNo, (int)LinePos ) );
        }

        private void SendValidationEvent( XmlSeverityType severity, XmlSchemaException e ) {
            Debug.Assert( validate );
            readerAdapter.SendValidationEvent( severity, e );
        }

        private bool IsAttributeValueType( Token token ) {
            return (int)token >= (int)Token.CDATA && (int)token <= (int)Token.NOTATION;
        }

        private int LineNo {
            get {
                return readerAdapter.LineNo;
            }
        }

        private int LinePos {
            get {
                return curPos - readerAdapter.LineStartPosition;
            }
        }

        private string BaseUriStr {
            get {
                Uri tmp = readerAdapter.BaseUri;
                return ( tmp != null ) ? tmp.ToString() : string.Empty;
            }
        }

        private void OnUnexpectedError() {
            Debug.Assert( false, "This is an unexpected error that should have been handled in the ScanXXX methods." );
            Throw( curPos, Res.Xml_InternalError );
        }

        void Throw( int curPos, string res ) {
            Throw( curPos, res, string.Empty );
        }

        void Throw( int curPos, string res, string arg ) {
            this.curPos = curPos;
            Uri baseUri = readerAdapter.BaseUri;
            readerAdapter.Throw( new XmlException( res, arg, (int)LineNo, (int)LinePos, baseUri == null ? null : baseUri.ToString() ) );
        }
        void Throw( int curPos, string res, string[] args ) {
            this.curPos = curPos;
            Uri baseUri = readerAdapter.BaseUri;
            readerAdapter.Throw( new XmlException( res, args, (int)LineNo, (int)LinePos, baseUri == null ? null : baseUri.ToString() ) );
        }

        void Throw( string res, string arg, int lineNo, int linePos ) {
            Uri baseUri = readerAdapter.BaseUri;
            readerAdapter.Throw( new XmlException( res, arg, (int)lineNo, (int)linePos, baseUri == null ? null : baseUri.ToString() ) );
        }

        void ThrowInvalidChar( int pos, char invChar ) {
            Throw( pos, Res.Xml_InvalidCharacter, XmlException.BuildCharExceptionStr( invChar ) );
        }

        private void ThrowUnexpectedToken( int pos, string expectedToken ) {
            ThrowUnexpectedToken( pos, expectedToken, null );
        }

        private void ThrowUnexpectedToken( int pos, string expectedToken1, string expectedToken2 ) {
            string unexpectedToken = ParseUnexpectedToken( pos );
            if ( expectedToken2 != null ) {
                Throw( curPos, Res.Xml_UnexpectedTokens2, new string[3] { unexpectedToken, expectedToken1, expectedToken2 } );
            }
            else {
                Throw( curPos, Res.Xml_UnexpectedTokenEx, new string[2] { unexpectedToken, expectedToken1 } );
            }
        }

        private string ParseUnexpectedToken( int startPos ) {
            if ( xmlCharType.IsNCNameChar( chars[startPos] ) ) {
                int endPos = startPos + 1;
                while ( xmlCharType.IsNCNameChar( chars[endPos] ) ) {
                    endPos++;
                }
                return new string( chars, startPos, endPos - startPos );
            }
            else {
                Debug.Assert( startPos < charsUsed );
                return new string( chars, startPos, 1 );
            }
        }
    }
}
