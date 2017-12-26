// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
// ===========================================================================
// File: symwrite.cpp
//
// Note: The various SymWriter_* and SymDocumentWriter_* are entry points
// called via PInvoke from the managed symbol wrapper used by managed languages
// to emit debug information (such as jscript)
// ===========================================================================

#include "pch.h"
#include "symwrite.h"

// ------------------------------------------------------------------------- 
// SymWriter class
// ------------------------------------------------------------------------- 

extern "C"
HRESULT SymWriter_Release(ISymUnmanagedWriter *pWriter)
{
    if (pWriter)
        pWriter->Release();;
    return NOERROR;
}

extern "C"
HRESULT SymWriter_GetWriter(VOID *ppWriter, VOID **ppOutWriter)
{
    HRESULT hr = NOERROR;
    IfFalseGo(ppOutWriter, E_INVALIDARG);

    *ppOutWriter = NULL;

    SymWriter *pWriter;
    if (ppWriter)
    {
        pWriter = *(SymWriter **)ppWriter;
        pWriter->AddRef();
        *ppOutWriter = pWriter;
    }
ErrExit:
    return hr;
}

//-----------------------------------------------------------
// NewSymWriter
// Static function used to create a new instance of SymWriter
//-----------------------------------------------------------
HRESULT SymWriter::NewSymWriter(const GUID& id, void **object)
{
    if (id != IID_ISymUnmanagedWriter)
        return (E_UNEXPECTED);

    SymWriter *writer = NEW(SymWriter());

    if (writer == NULL)
        return (E_OUTOFMEMORY);

    *object = (ISymUnmanagedWriter*)writer;
    writer->AddRef();

    return (S_OK);
}

//-----------------------------------------------------------
// SymWriter Constuctor
//-----------------------------------------------------------
SymWriter::SymWriter() :
    m_refCount(0),
    m_importer(NULL),
    m_openMethodToken(mdMethodDefNil),
    m_LargestMethodToken(mdMethodDefNil),
    m_pmethod(NULL),
    m_currentScope(-1),
    m_pDocsHead (NULL),
    m_hFile(NULL),
    m_pIStream(NULL),
    m_pStringPool(NULL),
    m_closed( false ),
    m_sortLines (false),
    m_sortMethodEntries(false)
{
    memset(m_szPath, 0, sizeof(m_szPath));
    memset(&ModuleLevelInfo, 0, sizeof(PDBInfo));
}

//-----------------------------------------------------------
// SymWriter QI
//-----------------------------------------------------------
COM_METHOD SymWriter::QueryInterface(REFIID riid, void **ppInterface)
{
    if (riid == IID_ISymUnmanagedWriter )
        *ppInterface = (ISymUnmanagedWriter*)this;
    /* ROTORTODO: Pretend that we do not implement ISymUnmanagedWriter2 to prevent C# compiler from using it.
       This is against COM rules since ISymUnmanagedWriter3 inherits from ISymUnmanagedWriter2.
    else if (riid == IID_ISymUnmanagedWriter2 )
        *ppInterface = (ISymUnmanagedWriter2*)this;
    */
    else if (riid == IID_ISymUnmanagedWriter3 )
        *ppInterface = (ISymUnmanagedWriter3*)this;
    else if (riid == IID_IUnknown)
        *ppInterface = (IUnknown*)(ISymUnmanagedWriter*)this;
    else
    {
        *ppInterface = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

//-----------------------------------------------------------
// SymWriter Destructor
//-----------------------------------------------------------
SymWriter::~SymWriter()
{
    if ( !m_closed )
        Close();
    RELEASE(m_pIStream);
    DELETE(m_pStringPool);
    RELEASE(m_importer);
}

//-----------------------------------------------------------
// SymWriter Initialize the SymWriter
//-----------------------------------------------------------
extern "C"
HRESULT SymWriter_Initialize(
    ISymUnmanagedWriter *pWriter,
    IUnknown *emitter,
    const WCHAR *szFilename,
    BOOL fFullBuild)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pWriter, E_INVALIDARG);
    hr = pWriter->Initialize(emitter, szFilename, NULL, fFullBuild);

ErrExit:
    return hr;
}

COM_METHOD SymWriter::Initialize
(   
    IUnknown *emitter,        // Emitter (IMetaData Emit/Import)
    const WCHAR *szFilename,  // FileName of the exe we're creating
    IStream *pIStream,        // Stream to store into
    BOOL fFullBuild           // Is this a full build or an incremental build
)
{
    HRESULT hr = S_OK;

    // Incremental compile not implemented in Rotor
    _ASSERTE(fFullBuild);

    if ( emitter == NULL )
        return E_INVALIDARG;


    if ( pIStream ) {
        m_pIStream = pIStream;
        pIStream->AddRef();
    } else {
        if ( szFilename == NULL )
            hr = E_INVALIDARG;
    }

    m_pStringPool = NEW(StgStringPool());
    m_pStringPool->InitNew();

    if ( szFilename ) {
        wchar_t fullpath[_MAX_PATH];
        wchar_t drive[_MAX_DRIVE];
        wchar_t dir[_MAX_DIR];
        wchar_t fname[_MAX_FNAME];
        _wsplitpath( szFilename, drive, dir, fname, NULL );
        _wmakepath( fullpath, drive, dir, fname, L"ildb" );
        wcsncpy( m_szPath, fullpath, _MAX_PATH );
    }

    if ( m_importer == NULL ) {
       // The emitter should also be a IMetaDataImport. Make sure...
        IfFailGo( emitter->QueryInterface(IID_IMetaDataImport,
                                             (void**)&m_importer) );
    }


ErrExit:
    return hr;
}

//-----------------------------------------------------------
// SymWriter Initialize2 the SymWriter
// Delegate to Initialize then use the szFullPathName param
//-----------------------------------------------------------
COM_METHOD SymWriter::Initialize2
(
    IUnknown *emitter,         // Emitter (IMetaData Emit/Import)
    const WCHAR *szTempPath,   // Location of the file
    IStream *pIStream,         // Stream to store into
    BOOL fFullBuild,           // Full build or not
    const WCHAR *szFullPathName   // Final destination of the ildb
)    
{
    HRESULT hr = S_OK;
    IfFailGo( Initialize( emitter, szTempPath, pIStream, fFullBuild ) );
    // We don't need the final location of the ildb

ErrExit:
    return hr;
}

//-----------------------------------------------------------
// SymWriter DefineDocument
//-----------------------------------------------------------
extern "C"
COM_METHOD SymWriter_DefineDocument(
    ISymUnmanagedWriter *pWriter,               // UnmanagedWriter
    const WCHAR *wcsUrl,
    const GUID *pLanguage,
    const GUID *pLanguageVendor,
    const GUID *pDocumentType,
    ISymUnmanagedDocumentWriter **ppRetVal)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pWriter, E_INVALIDARG);
    hr = pWriter->DefineDocument(
                wcsUrl,
                pLanguage,
                pLanguageVendor,
                pDocumentType,
                ppRetVal);
ErrExit:
    return hr;
}

COM_METHOD SymWriter::DefineDocument(
    const WCHAR *wcsUrl,          // Document name
    const GUID *pLanguage,        // What Language we're compiling
    const GUID *pLanguageVendor,  // What vendor
    const GUID *pDocumentType,    // Type
    ISymUnmanagedDocumentWriter **ppRetVal // [out] Created DocumentWriter
)
{

    HRESULT hr = NOERROR;
    DocumentInfo* pDocument = NULL;
    SymDocumentWriter *sdw = NULL;
    DocWriter* pdoc = NULL;
    UINT32 DocumentEntry;
    ULONG UrlEntry;

    IfFalseGo(wcsUrl, E_INVALIDARG);
    IfFalseGo(ppRetVal, E_INVALIDARG);

    // Init out parameter
    *ppRetVal = NULL;

    DocumentEntry = m_MethodInfo.m_documents.count();
    IfNullGo(pDocument = m_MethodInfo.m_documents.next());
    memset(pDocument, 0, sizeof(DocumentInfo));

    // Create the new document writer.
    sdw = NEW(SymDocumentWriter(DocumentEntry, this));
    IfNullGo(sdw);

    pDocument->SetLanguage(*pLanguage);
    pDocument->SetLanguageVendor(*pLanguageVendor);
    pDocument->SetDocumentType(*pDocumentType);

    IfFailGo(m_pStringPool->AddStringW(wcsUrl, &UrlEntry));
    pDocument->SetUrlEntry(UrlEntry);

    pdoc = NEW(DocWriter);
    IfNullGo(pdoc);

    // Save the document writer but do not ref count this!
    // The SymDocumentWriter must call DocumentReleased on destruction
    pdoc->pdoc = sdw;
    pdoc->pnext = m_pDocsHead;
    m_pDocsHead = pdoc;
    pdoc = NULL;

    // Pass out the new ISymUnmanagedDocumentWriter.
    sdw->AddRef();
    *ppRetVal = (ISymUnmanagedDocumentWriter*)sdw;
    sdw = NULL;

ErrExit:
    DELETE(sdw);
    DELETE(pdoc);
    return hr;
}

//
// This document is ready to be released. Close it and eliminate it from our list.
//
HRESULT SymWriter::DocumentReleased(SymDocumentWriter *sdw)
{
    HRESULT hr = S_OK;

    DocWriter** ppdoc = &m_pDocsHead;

    while ( *ppdoc != NULL ) {
        if ( (*ppdoc)->pdoc == sdw ) {
            DocWriter* pdoc = *ppdoc;
            *ppdoc = pdoc->pnext;
            DELETE(pdoc);
            break;
        } else {
            ppdoc = &(*ppdoc)->pnext;
        }
    }

    return hr;
}

//-----------------------------------------------------------
// SymWriter DefineSrc
//-----------------------------------------------------------
HRESULT SymWriter::DefineSrc(
    UINT32 DocumentEntry,
    GUID  AlgorithmId,
    DWORD CheckSumSize,
    BYTE* pCheckSum,
    DWORD SourceSize,
    BYTE* pSource
)
{
    HRESULT hr = NOERROR;

    DocumentInfo* pDocument = NULL;
    IfFalseGo( CheckSumSize == 0 || pCheckSum, E_INVALIDARG);
    IfFalseGo( SourceSize == 0 || pSource, E_INVALIDARG);
    IfFalseGo( DocumentEntry < m_MethodInfo.m_documents.count(), E_INVALIDARG);

    pDocument = &m_MethodInfo.m_documents.m_array[DocumentEntry];

    if (pCheckSum)
    {
        pDocument->SetCheckSumEntry((int)m_MethodInfo.m_bytes.grab( CheckSumSize ));
        memcpy(&m_MethodInfo.m_bytes[pDocument->CheckSumEntry()], pCheckSum, CheckSumSize);
        pDocument->SetCheckSymSize(CheckSumSize);
    }

    if (pSource)
    {
        pDocument->SetSourceEntry((int)m_MethodInfo.m_bytes.grab(SourceSize));
        memcpy(&m_MethodInfo.m_bytes[pDocument->SourceEntry()], pSource, SourceSize);
        pDocument->SetSourceSize(SourceSize);
    }

    pDocument->SetAlgorithmId(AlgorithmId);

ErrExit:
    return hr;
}

//-----------------------------------------------------------
// SymWriter SetUserEntryPoint
//-----------------------------------------------------------
extern "C"
COM_METHOD SymWriter_SetUserEntryPoint(
    ISymUnmanagedWriter *pWriter,             // UnmanagedWriter
    mdMethodDef entryMethod)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pWriter, E_INVALIDARG);
    hr = pWriter->SetUserEntryPoint(entryMethod);

ErrExit:
    return hr;
}

COM_METHOD SymWriter::SetUserEntryPoint(mdMethodDef entryMethod)
{

    HRESULT hr = S_OK;

    // Make sure that an entry point hasn't already been set.
    if (ModuleLevelInfo.m_userEntryPoint == 0)
        ModuleLevelInfo.m_userEntryPoint = entryMethod;

    return hr;
}

//-----------------------------------------------------------
// SymWriter OpenMethod
// Get ready to get information about a new method
//-----------------------------------------------------------
extern "C"
COM_METHOD SymWriter_OpenMethod(
    ISymUnmanagedWriter *pWriter,             // UnmanagedWriter
    mdMethodDef method)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pWriter, E_INVALIDARG);
    hr = pWriter->OpenMethod(method);

ErrExit:
    return hr;
}
COM_METHOD SymWriter::OpenMethod(mdMethodDef method)
{

    HRESULT hr = S_OK;

    // We can only have one open method at a time.
    if (m_openMethodToken != mdMethodDefNil)
        return E_INVALIDARG;

    m_LargestMethodToken = max(method, m_LargestMethodToken);

    if (m_LargestMethodToken != method)
    {
        m_sortMethodEntries = true;
        // Check to see if we're trying to open a method we've already done
        unsigned i;
        for (i = 0; i < m_MethodInfo.m_methods.count(); i++)
        {
            if (m_MethodInfo.m_methods[i].MethodToken() == method)
            {
                return E_INVALIDARG;                
            }
        }
    }

    // Remember the token for this method.
    m_openMethodToken = method;

    IfNullGo( m_pmethod = m_MethodInfo.m_methods.next() );
    m_pmethod->SetMethodToken(m_openMethodToken);
    m_pmethod->SetStartScopes(m_MethodInfo.m_scopes.count());
    m_pmethod->SetStartVars(m_MethodInfo.m_vars.count());
    m_pmethod->SetStartUsing(m_MethodInfo.m_usings.count());
    m_pmethod->SetStartConstant(m_MethodInfo.m_constants.count());
    m_pmethod->SetStartDocuments(m_MethodInfo.m_documents.count());
    m_pmethod->SetStartSequencePoints(m_MethodInfo.m_auxSequencePoints.count());

    // Current scope index of -1 indicates no scopes defined in the method yet.
    m_currentScope = -1;

    // By default assume the lines are inserted in the correct order
    m_sortLines = false;

ErrExit:
    return hr;
}

COM_METHOD SymWriter::OpenMethod2(
    mdMethodDef method,
    ULONG32 isect,
    ULONG32 offset)
{
    // This symbol writer doesn't support section offsets
    _ASSERTE(FALSE);
    return E_NOTIMPL;
}

//-----------------------------------------------------------
// compareAuxLines
// Used to sort SequencePoint
//-----------------------------------------------------------
int __cdecl SequencePoint::compareAuxLines(const void *elem1, const void *elem2 )
{
    SequencePoint* p1 = (SequencePoint*)elem1;
    SequencePoint* p2 = (SequencePoint*)elem2;
    return p1->Offset() - p2->Offset();
}

//-----------------------------------------------------------
// SymWriter CloseMethod
// We're done with this function, write it out.
//-----------------------------------------------------------
extern "C"
COM_METHOD SymWriter_CloseMethod(
    ISymUnmanagedWriter *pWriter)     // UnmanagedWriter
{
    HRESULT hr = NOERROR;
    IfFalseGo(pWriter, E_INVALIDARG);
    hr = pWriter->CloseMethod();

ErrExit:
    return hr;
}
COM_METHOD SymWriter::CloseMethod()
{

    HRESULT hr = S_OK;
    UINT32 CountOfSequencePoints;

    // Must have an open method.
    if (m_openMethodToken == mdMethodDefNil)
        return E_UNEXPECTED;

    // All scopes should be closed. -1 means nothing was ever opened,
    // -2 means that the top scope was closed.
    _ASSERTE((m_currentScope == -1) || (m_currentScope == -2));

    mdTypeDef classDef;
    WCHAR szNameMethod[1024];
    ULONG ccNm = 0;
    IfFailGo( m_importer->GetMethodProps(
                m_openMethodToken,  // The method for which to get props.
                &classDef,          // Put method's class here.
                szNameMethod,       // Put method's name here.
                1024,               // Size of szMethod buffer in wide chars.
                &ccNm,               // Put actual size here
                NULL,               // Put flags here.
                NULL,               // [OUT] point to the blob value of meta data
                NULL,               // [OUT] actual size of signature blob
                NULL,               // [OUT] codeRVA
                NULL ) );
    szNameMethod[1024-1] = 0;

    m_pmethod->SetEndScopes(m_MethodInfo.m_scopes.count());
    m_pmethod->SetEndVars(m_MethodInfo.m_vars.count());
    m_pmethod->SetEndUsing(m_MethodInfo.m_usings.count());
    m_pmethod->SetEndConstant(m_MethodInfo.m_constants.count());
    m_pmethod->SetEndDocuments(m_MethodInfo.m_documents.count());
    m_pmethod->SetEndSequencePoints(m_MethodInfo.m_auxSequencePoints.count());

    CountOfSequencePoints = m_pmethod->EndSequencePoints() - m_pmethod->StartSequencePoints();
     // Write any sequence points.
    if (CountOfSequencePoints > 0 ) {
        // sort the sequence points
        if ( m_sortLines )
        {
            qsort(&m_MethodInfo.m_auxSequencePoints[m_pmethod->StartSequencePoints()],
                  CountOfSequencePoints,
                  sizeof( SequencePoint ),
                  SequencePoint::compareAuxLines );
        }
    }

ErrExit:

    // All done with this method.
    m_openMethodToken = mdMethodDefNil;
    
    return hr;
}

//-----------------------------------------------------------
// SymWriter DefineSequencePoints
// Define the sequence points for this function
//-----------------------------------------------------------
extern "C"
COM_METHOD SymWriter_DefineSequencePoints(
    ISymUnmanagedWriter *pWriter,
    ISymUnmanagedDocumentWriter *document,
    ULONG32 spCount,
    ULONG32 offsets[],
    ULONG32 lines[],
    ULONG32 columns[],
    ULONG32 endLines[],
    ULONG32 endColumns[]
)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pWriter, E_INVALIDARG);
    hr = pWriter->DefineSequencePoints(
                document,
                spCount,
                offsets,
                lines, columns,
                endLines, endColumns);
ErrExit:
    return hr;
}

COM_METHOD SymWriter::DefineSequencePoints(
    ISymUnmanagedDocumentWriter *document,  // 
    ULONG32 spCount,        // Count of sequence points
    ULONG32 offsets[],      // Offsets
    ULONG32 lines[],        // Beginning Lines
    ULONG32 columns[],      // [optional] Columns
    ULONG32 endLines[],     // [optional] End Lines
    ULONG32 endColumns[]    // [optional] End Columns
)
{

    HRESULT hr = S_OK;
    DWORD docnum;

    // We must have a document, offsets, and lines.
    IfFalseGo(document && offsets && lines, E_INVALIDARG);
    // Must have some sequence points
    IfFalseGo(spCount != 0, E_INVALIDARG);
    // Must have an open method.
    IfFalseGo(m_openMethodToken != mdMethodDefNil, E_INVALIDARG);

    // Remember that we've loaded the sequence points and
    // which document they were for.
    docnum = (DWORD)((SymDocumentWriter *)document)->GetDocumentEntry();;

    // if sets of lines have been inserted out-of-order, remember to sort when emitting
    if ( m_MethodInfo.m_auxSequencePoints.count() > 0 && m_MethodInfo.m_auxSequencePoints[ m_MethodInfo.m_auxSequencePoints.count()-1 ].Offset() > offsets[0] )
        m_sortLines = true;

    // Copy the incomming arrays into the internal format.

    for ( UINT32 i = 0; i < spCount; i++)
    {
        SequencePoint* paux = m_MethodInfo.m_auxSequencePoints.next();
        paux->SetOffset(offsets[i]);
        paux->SetStartLine(lines[i]);
        paux->SetStartColumn(columns ? columns[i] : 0);
        // If no endLines specified, assume same as start
        paux->SetEndLine(endLines ? endLines[i] : lines[i]);
        paux->SetEndColumn(endColumns ? endColumns[i]: 0);
        paux->SetDocument(docnum);
    }

ErrExit:
    return hr;
}

//-----------------------------------------------------------
// SymWriter OpenScope
// Open a new scope for this function
//-----------------------------------------------------------
extern "C"
COM_METHOD SymWriter_OpenScope(ISymUnmanagedWriter *pWriter, ULONG32 startOffset, ULONG32 *pScopeID)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pWriter, E_INVALIDARG);
    hr = pWriter->OpenScope(startOffset, pScopeID);

ErrExit:
    return hr;
}

COM_METHOD SymWriter::OpenScope(ULONG32 startOffset, ULONG32 *scopeID)
{

    HRESULT hr = S_OK;

    // You can't open multiple top scopes. -1 indicates that this is
    // the first scope opened (we don't really care in this method.)
    // -2 indicates that the first scope has been closed.
    if (m_currentScope == -2)
        return E_FAIL;

    // Make sure the startOffset is within the current scope.
    if ((m_currentScope >= 0) &&
        (unsigned long)startOffset < m_MethodInfo.m_scopes[m_currentScope].StartOffset())
        return E_INVALIDARG;

    // Fill in the new scope.
    ULONG32 newScope = static_cast<ULONG32>( m_MethodInfo.m_scopes.count() );
    SymLexicalScope *sc;
    IfNullGo( sc = m_MethodInfo.m_scopes.next());
    sc->SetParentScope(m_currentScope); // parent is the current scope.
    sc->SetStartOffset(startOffset);
    sc->SetHasChildren(FALSE);
    sc->SetHasVars(FALSE);
    sc->SetEndOffset(0);

    // The current scope has a child now.
    if (m_currentScope >= 0)
        m_MethodInfo.m_scopes[m_currentScope].SetHasChildren(TRUE);

    // The new scope is now the current scope.
    m_currentScope = newScope;

    _ASSERTE(sizeof(m_currentScope) <= sizeof(long));

    // Pass out the "scope id", which is a _1_ based id for the scope.
    if (scopeID)
        *scopeID = (ULONG32)m_currentScope + 1;

ErrExit:
    return hr;
}

//-----------------------------------------------------------
// SymWriter CloseScope
//-----------------------------------------------------------
extern "C"
COM_METHOD SymWriter_CloseScope(ISymUnmanagedWriter *pWriter, ULONG32 endOffset)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pWriter, E_INVALIDARG);
    hr = pWriter->CloseScope(endOffset);

ErrExit:
    return hr;
}
COM_METHOD SymWriter::CloseScope(
    ULONG32 endOffset // Closing offset of scope
)
{

    HRESULT hr = S_OK;

    // If we're closing after all scopes have been closed, then
    // someone has made a mistake.
    if (m_currentScope < 0)
        return E_FAIL;

    // Capture the end offset
    m_MethodInfo.m_scopes[m_currentScope].SetEndOffset(endOffset);

    // The current scope is now the parent scope.
    m_currentScope = m_MethodInfo.m_scopes[m_currentScope].ParentScope();

    // If we closed the top scope, then set m_currentScope to -2 to
    // indicate this.
    if (m_currentScope == -1)
        m_currentScope = -2;

    return hr;
}

//-----------------------------------------------------------
// SymWriter SetScopeRange
// Set the Start/End Offset for this scope
//-----------------------------------------------------------
extern "C"
HRESULT SymWriter_SetScopeRange(
    ISymUnmanagedWriter *pWriter,
    ULONG32 scopeID,      // ID for the scope
    ULONG32 startOffset,  // Start Offset
    ULONG32 endOffset     // End Offset
)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pWriter, E_INVALIDARG);
    hr = pWriter->SetScopeRange(scopeID,
                                startOffset,
                                endOffset);
ErrExit:
    return hr;
}

COM_METHOD SymWriter::SetScopeRange(
    ULONG32 scopeID,      // ID for the scope
    ULONG32 startOffset,  // Start Offset
    ULONG32 endOffset     // End Offset
)
{
    if (scopeID <= 0)
        return E_INVALIDARG;

    if (scopeID > m_MethodInfo.m_scopes.count() )
        return E_INVALIDARG;

    // Remember the new start and end offsets. Also remember that the
    // scopeID is _1_ based!!!
    SymLexicalScope *sc = &(m_MethodInfo.m_scopes[scopeID - 1]);
    sc->SetStartOffset(startOffset);
    sc->SetEndOffset(endOffset);

    return S_OK;
}

//-----------------------------------------------------------
// SymWriter DefineLocalVariable
//-----------------------------------------------------------
extern "C"
COM_METHOD SymWriter_DefineLocalVariable(
    ISymUnmanagedWriter *pWriter,
    const WCHAR *name,
    ULONG32 attributes,
    ULONG32 cSig,
    BYTE signature[],
    ULONG32 addrKind,
    ULONG32 addr1, ULONG32 addr2, ULONG32 addr3,
    ULONG32 startOffset, ULONG32 endOffset)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pWriter, E_INVALIDARG);
    hr = pWriter->DefineLocalVariable(
                name,
                attributes, cSig,
                signature,
                addrKind,
                addr1, addr2, addr3,
                startOffset, endOffset);
ErrExit:
    return hr;
}

COM_METHOD SymWriter::DefineLocalVariable(
    const WCHAR *name,    // Name of the variable
    ULONG32 attributes,   // Attributes for the var
    ULONG32 cSig,         // Signature for the variable
    BYTE signature[],
    ULONG32 addrKind,
    ULONG32 addr1, ULONG32 addr2, ULONG32 addr3,
    ULONG32 startOffset, ULONG32 endOffset)
{

    HRESULT hr = S_OK;
    ULONG NameEntry;

    // We must have a current scope.
    if (m_currentScope < 0)
        return E_FAIL;

    // We must have a name and a signature.
    if (!name || !signature)
        return E_INVALIDARG;

    if (cSig == 0)
        return E_INVALIDARG;

    // Make a new local variable and copy the data.
    SymVariable *var;
    IfNullGo( var = m_MethodInfo.m_vars.next());
    var->SetIsParam(FALSE);
    var->SetAttributes(attributes);
    var->SetAddrKind(addrKind);
    var->SetIsHidden(attributes & VAR_IS_COMP_GEN);
    var->SetAddr1(addr1);
    var->SetAddr2(addr2);
    var->SetAddr3(addr3);


    // Length of the sig?
    ULONG32 sigLen;
    sigLen = cSig;

    // Increase the signature length so we can stick a one byte
    // calling convention on the front of it.
    sigLen++;

    // Copy the name.
    IfFailGo(m_pStringPool->AddStringW(name, &NameEntry));
    var->SetName(NameEntry);

    // Copy the signature
    BYTE *s;
    var->SetSignature((int)m_MethodInfo.m_bytes.grab( sigLen ));
    var->SetSignatureSize(sigLen);
    s = &(m_MethodInfo.m_bytes[var->Signature()]);

    // For freaky symbol stores, we stick calling convention field
    // infront of the variable's signature.
    s[0] = IMAGE_CEE_CS_CALLCONV_FIELD;
    memcpy(s+1, signature, sigLen-1);

    // This var is in the current scope
    var->SetScope(m_currentScope);
    m_MethodInfo.m_scopes[m_currentScope].SetHasVars(TRUE);

    var->SetStartOffset(startOffset);
    var->SetEndOffset(endOffset);

ErrExit:
    return hr;
}

COM_METHOD SymWriter::DefineLocalVariable2(
    const WCHAR *name,
    ULONG32 attributes,
    mdSignature sigToken,
    ULONG32 addrKind,
    ULONG32 addr1, ULONG32 addr2, ULONG32 addr3,
    ULONG32 startOffset, ULONG32 endOffset)
{
    // This symbol writer doesn't support definiting signatures via tokens
    _ASSERTE(FALSE);
    return E_NOTIMPL;
}

//-----------------------------------------------------------
// SymWriter DefineParameter
//-----------------------------------------------------------
extern "C"
COM_METHOD SymWriter_DefineParameter(
    ISymUnmanagedWriter *pWriter,
    const WCHAR *name,
    ULONG32 attributes,
    ULONG32 sequence,
    ULONG32 addrKind,
    ULONG32 addr1, ULONG32 addr2, ULONG32 addr3)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pWriter, E_INVALIDARG);
    hr = pWriter->DefineParameter(
                name,
                attributes,
                sequence,
                addrKind, addr1, addr2, addr3);
ErrExit:
    return hr;
}
COM_METHOD SymWriter::DefineParameter(
    const WCHAR *name,    // Param name
    ULONG32 attributes,   // Attribute for the parameter
    ULONG32 sequence,
    ULONG32 addrKind,
    ULONG32 addr1, ULONG32 addr2, ULONG32 addr3)
{
    HRESULT hr = S_OK;
    ULONG NameEntry;

    // We must have a method.
    if (m_openMethodToken == mdMethodDefNil)
        return E_INVALIDARG;

    // We must have a name.
    if (!name)
        return E_INVALIDARG;

    SymVariable *var;
    IfNullGo( var = m_MethodInfo.m_vars.next());
    var->SetIsParam(TRUE);
    var->SetAttributes(attributes);
    var->SetAddrKind(addrKind);
    var->SetIsHidden(attributes & VAR_IS_COMP_GEN);
    var->SetAddr1(addr1);
    var->SetAddr2(addr2);
    var->SetAddr3(addr3);
    var->SetSequence(sequence);

    // Copy the name.
    IfFailGo(m_pStringPool->AddStringW(name, &NameEntry));
    var->SetName(NameEntry);

    // This var is in the current scope
    if ( m_currentScope > 0 )
        m_MethodInfo.m_scopes[m_currentScope].SetHasVars(TRUE);

    var->SetStartOffset(0);
    var->SetEndOffset(0);

ErrExit:
    return hr;
}

//-----------------------------------------------------------
// verifyConstTypes
// Verify that the type is a type we support
//-----------------------------------------------------------
static bool verifyConstTypes( DWORD vt )
{
    switch ( vt ) {
    case VT_UI8:
    case VT_I8:
    case VT_I4:
    case VT_UI1:    // value < LF_NUMERIC
    case VT_I2:
    case VT_R4:
    case VT_R8:
    case VT_BOOL:   // value < LF_NUMERIC
    case VT_DATE:
    case VT_BSTR:
    case VT_I1:
    case VT_UI2:
    case VT_UI4:
    case VT_INT:
    case VT_UINT:
    case VT_DECIMAL:
        return true;
    }
    return false;
}

//-----------------------------------------------------------
// SymWriter DefineConstant
//-----------------------------------------------------------
COM_METHOD SymWriter::DefineConstant(
    const WCHAR __RPC_FAR *name,
    VARIANT value,
    ULONG32 cSig,
    unsigned char __RPC_FAR signature[])
{
    HRESULT hr = S_OK;
    ULONG ValueBstr;
    ULONG Name;

    // currently we only support local constants

    // We must have a method.
    if (m_openMethodToken == mdMethodDefNil)
        return E_INVALIDARG;

    // We must have a name.
    if (!name)
        return E_INVALIDARG;

    //
    // Support byref decimal values
    //
    if ( (V_VT(&value)) == ( VT_BYREF | VT_DECIMAL ) ) {
        if ( V_DECIMALREF(&value) == NULL )
            return E_INVALIDARG;
        V_DECIMAL(&value) = *V_DECIMALREF(&value);
        V_VT(&value) = VT_DECIMAL;
    }

    // we only support non-ref constants
    if ( ( V_VT(&value) & VT_BYREF ) != 0 )
        return E_INVALIDARG;

    if ( !verifyConstTypes( V_VT(&value) ) )
        return E_INVALIDARG;

    // If it's a BSTR, we need to persist the Bstr as an entry into
    // the stringpool
    if (V_VT(&value) == VT_BSTR)
    {
        // Copy the bstrValue.
        IfFailGo(m_pStringPool->AddStringW(V_BSTR(&value), &ValueBstr));
        V_BSTR(&value) = NULL;
    }

    SymConstant *con;
    IfNullGo( con = m_MethodInfo.m_constants.next());
    con->SetValue(value, ValueBstr);


    // Copy the name.
    IfFailGo(m_pStringPool->AddStringW(name, &Name));
    con->SetName(Name);

    // Length of the sig?
    ULONG32 sigLen;
    sigLen = cSig;

    // Increase the signature length so we can stick a one byte
    // calling convention on the front of it.
    sigLen++;

    // Copy the signature
    BYTE *s;
    con->SetSignature((int)m_MethodInfo.m_bytes.grab( sigLen ));
    con->SetSignatureSize(sigLen);
    s = &(m_MethodInfo.m_bytes[con->Signature()]);

    // For freaky symbol stores, we stick calling convention field
    // infront of the variable's signature.
    s[0] = IMAGE_CEE_CS_CALLCONV_FIELD;
    memcpy(s+1, signature, sigLen-1);

    // This const is in the current scope
    con->SetParentScope(m_currentScope);
    m_MethodInfo.m_scopes[m_currentScope].SetHasVars(TRUE);

ErrExit:
    return hr;
}

COM_METHOD SymWriter::DefineConstant2(
    const WCHAR *name,
    VARIANT value,
    mdSignature sigToken)
{
    // This symbol writer doesn't support definiting signatures via tokens
    _ASSERTE(FALSE);
    return E_NOTIMPL;
}

//-----------------------------------------------------------
// SymWriter Abort
//-----------------------------------------------------------
COM_METHOD SymWriter::Abort(void)
{
    m_closed = true;
    return S_OK;
}

//-----------------------------------------------------------
// SymWriter DefineField
//-----------------------------------------------------------
extern "C"
COM_METHOD SymWriter_DefineField(
    ISymUnmanagedWriter *pWriter,
    mdTypeDef parent,
    const WCHAR *name,
    ULONG32 attributes,
    ULONG32 cSig,
    BYTE signature[],
    ULONG32 addrKind,
    ULONG32 addr1, ULONG32 addr2, ULONG32 addr3)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pWriter, E_INVALIDARG);
    hr = pWriter->DefineField(
                parent,
                name,
                attributes, cSig,
                signature,
                addrKind,
                addr1, addr2, addr3);
ErrExit:
    return hr;
}

COM_METHOD SymWriter::DefineField(
    mdTypeDef parent,
    const WCHAR *name,
    ULONG32 attributes,
    ULONG32 csig,
    BYTE signature[],
    ULONG32 addrKind,
    ULONG32 addr1, ULONG32 addr2, ULONG32 addr3)
{
    // This symbol store doesn't support extra random variable
    // definitions. 
    return S_OK;
}

//-----------------------------------------------------------
// SymWriter DefineGlobalVariable
//-----------------------------------------------------------
extern "C"
COM_METHOD SymWriter_DefineGlobalVariable(
    ISymUnmanagedWriter *pWriter,
    const WCHAR *name,
    ULONG32 attributes,
    ULONG32 csig,
    BYTE signature[],
    ULONG32 addrKind,
    ULONG32 addr1, ULONG32 addr2, ULONG32 addr3)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pWriter, E_INVALIDARG);
    hr = pWriter->DefineGlobalVariable(
                name,
                attributes, 
                csig, signature,
                addrKind,
                addr1, addr2, addr3);
ErrExit:
    return hr;
}
COM_METHOD SymWriter::DefineGlobalVariable(
    const WCHAR *name,
    ULONG32 attributes,
    ULONG32 csig,
    BYTE signature[],
    ULONG32 addrKind,
    ULONG32 addr1, ULONG32 addr2, ULONG32 addr3)
{
    // This symbol writer doesn't support global variables
    _ASSERTE(FALSE);
    return E_NOTIMPL;
}

COM_METHOD SymWriter::DefineGlobalVariable2(
    const WCHAR *name,
    ULONG32 attributes,
    mdSignature sigToken,
    ULONG32 addrKind,
    ULONG32 addr1, ULONG32 addr2, ULONG32 addr3)
{
    // This symbol writer doesn't support global variables
    _ASSERTE(FALSE);
    return E_NOTIMPL;
}

//-----------------------------------------------------------
// compareMethods
// Used to sort method entries
//-----------------------------------------------------------
int __cdecl SymMethodInfo::compareMethods(const void *elem1, const void *elem2 )
{
    SymMethodInfo* p1 = (SymMethodInfo*)elem1;
    SymMethodInfo* p2 = (SymMethodInfo*)elem2;
    return p1->m_MethodToken - p2->m_MethodToken;
}

//-----------------------------------------------------------
// SymWriter Close
//-----------------------------------------------------------
extern "C"
COM_METHOD SymWriter_Close(ISymUnmanagedWriter *pWriter)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pWriter, E_INVALIDARG);
    hr = pWriter->Close();
ErrExit:
    return hr;
}
COM_METHOD SymWriter::Close()
{
    HRESULT hr = Commit();
    m_closed = true;
    return hr;
}

//-----------------------------------------------------------
// SymWriter Commit
//-----------------------------------------------------------
COM_METHOD SymWriter::Commit(void)
{
    // Sort the entries if need be
    if (m_sortMethodEntries)
    {
        // First remap any tokens we need to
        if (m_MethodMap.count())
        {
            unsigned i;
            for (i = 0; i< m_MethodMap.count(); i++)
            {
                m_MethodInfo.m_methods[m_MethodMap[i].MethodEntry].SetMethodToken(m_MethodMap[i].m_MethodToken);
            }
        }

        // Now sort the array
        qsort(&m_MethodInfo.m_methods[0],
              m_MethodInfo.m_methods.count(),
              sizeof( SymMethodInfo ),
              SymMethodInfo::compareMethods );
        m_sortMethodEntries = false;
    }
    return WritePDB();
}

//-----------------------------------------------------------
// SymWriter SetSymAttribute
//-----------------------------------------------------------
extern "C"
COM_METHOD SymWriter_SetSymAttribute(
    ISymUnmanagedWriter *pWriter,
    mdToken parent,
    const WCHAR *name,
    ULONG32 cData,
    BYTE data[])
{
    HRESULT hr = NOERROR;
    IfFalseGo(pWriter, E_INVALIDARG);
    hr = pWriter->SetSymAttribute(parent, name, cData, data);
ErrExit:
    return hr;
}
COM_METHOD SymWriter::SetSymAttribute(
    mdToken parent,
    const WCHAR *name,
    ULONG32 cData,
    BYTE data[])
{
    // Setting attributes on the symbol isn't supported

    // ROTORTODO: #156785 in PS
    return S_OK;
}

//-----------------------------------------------------------
// SymWriter OpenNamespace
//-----------------------------------------------------------
extern "C"
COM_METHOD SymWriter_OpenNamespace(
    ISymUnmanagedWriter *pWriter,
    const WCHAR *name)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pWriter, E_INVALIDARG);
    hr = pWriter->OpenNamespace(name);
ErrExit:
    return hr;
}
COM_METHOD SymWriter::OpenNamespace(const WCHAR *name)
{
    // This symbol store doesn't support namespaces.
    return E_NOTIMPL;
}

//-----------------------------------------------------------
// SymWriter OpenNamespace
//-----------------------------------------------------------
extern "C"
COM_METHOD SymWriter_CloseNamespace(ISymUnmanagedWriter *pWriter)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pWriter, E_INVALIDARG);
    hr = pWriter->CloseNamespace();
ErrExit:
    return hr;
}
COM_METHOD SymWriter::CloseNamespace()
{
    // This symbol store doesn't support namespaces.
    return S_OK;
}

//-----------------------------------------------------------
// SymWriter UsingNamespace
// Add a Namespace to the list of namespace for this method
//-----------------------------------------------------------
extern "C"
COM_METHOD SymWriter_UsingNamespace(
    ISymUnmanagedWriter *pWriter,
    const WCHAR *fullName)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pWriter, E_INVALIDARG);
    hr = pWriter->UsingNamespace(fullName);
ErrExit:
    return hr;
}
COM_METHOD SymWriter::UsingNamespace(const WCHAR *fullName)
{
    HRESULT hr = S_OK;
    ULONG Name;

    // We must have a current scope.
    if (m_currentScope < 0)
        return E_FAIL;

    // We must have a name.
    if (!fullName)
        return E_INVALIDARG;


    SymUsingNamespace *use;
    IfNullGo( use = m_MethodInfo.m_usings.next());

    // Copy the name.
    IfFailGo(m_pStringPool->AddStringW(fullName, &Name));
    use->SetName(Name);

    use->SetParentScope(m_currentScope);

ErrExit:
    return hr;
}

//-----------------------------------------------------------
// SymWriter SetMethodSourceRange
//-----------------------------------------------------------
extern "C"
COM_METHOD SymWriter_SetMethodSourceRange(
    ISymUnmanagedWriter *pWriter,
    ISymUnmanagedDocumentWriter *startDoc,
    ULONG32 startLine,
    ULONG32 startColumn,
    ISymUnmanagedDocumentWriter *endDoc,
    ULONG32 endLine,
    ULONG32 endColumn)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pWriter, E_INVALIDARG);
    hr = pWriter->SetMethodSourceRange(startDoc, startLine, startColumn,
                                                      endDoc, endLine, endColumn);
ErrExit:
    return hr;
}
COM_METHOD SymWriter::SetMethodSourceRange(
    ISymUnmanagedDocumentWriter *startDoc,
    ULONG32 startLine,
    ULONG32 startColumn,
    ISymUnmanagedDocumentWriter *endDoc,
    ULONG32 endLine,
    ULONG32 endColumn)
{
    // This symbol store doesn't support source ranges.
    return E_NOTIMPL;
}

//-----------------------------------------------------------
// UnicodeToUTF8
// Translate the Unicode string to a UTF8 string
// Return the length in UTF8 of the Unicode string
//    Including NULL terminator
//-----------------------------------------------------------
inline int WINAPI UnicodeToUTF8(
    PCWSTR pUni,  // Unicode string
    PSTR pUTF8,   // [optional, out] Buffer for UTF8 string
    int cbUTF     // length of UTF8 buffer
)
{
    // Pass in the length including the NULL terminator
    int cchSrc = (int)wcslen(pUni)+1;
    return WideCharToMultiByte(CP_UTF8, 0, pUni, cchSrc, pUTF8, cbUTF, NULL, false);
}

//-----------------------------------------------------------
// SymWriter GetDebugCVInfo
// Get the size and potentially the debug info
//-----------------------------------------------------------
COM_METHOD SymWriter::GetDebugCVInfo(
    ULONG32 cbBuf,    // [optional] Size of buf
    ULONG32 *pcbBuf,  // [out] Size needed for the DebugInfo
    BYTE buf[])       // [optional, out] Buffer for DebugInfo
{

    if ( m_szPath == NULL || *m_szPath == 0 )
        return E_UNEXPECTED;

    // We need to change the .ildb extension to .pdb to be
    // compatible with VS7
    wchar_t fullpath[_MAX_PATH];
    wchar_t drive[_MAX_DRIVE];
    wchar_t dir[_MAX_DIR];
    wchar_t fname[_MAX_FNAME];
    _wsplitpath( m_szPath, drive, dir, fname, NULL );
    _wmakepath( fullpath, drive, dir, fname, L"pdb" );

    // Includes the Null Terminator
    int Utf8Length = UnicodeToUTF8( fullpath, NULL, 0 );
    if ( cbBuf == 0 && pcbBuf != NULL ) {   // just checking for the size
        *pcbBuf = ULONG32(sizeof( RSDSI ) + Utf8Length);
        return S_OK;
    }

    if ( cbBuf < ULONG32(sizeof( RSDSI ) + Utf8Length) ) {    // not enough space
        return E_INVALIDARG;
    }

    if ( buf == NULL ) {
        return E_INVALIDARG;
    }

    RSDSI* pRsdsi = (RSDSI*)buf;
    pRsdsi->dwSig = VAL32(0x53445352); // "SDSR";
    pRsdsi->guidSig = ILDB_VERSION_GUID;
    SwapGuid(&(pRsdsi->guidSig));
    // Age of 0 represent VC6.0 format so make sure it's 1
    pRsdsi->age = VAL32(1);
    UnicodeToUTF8( fullpath, pRsdsi->szPDB, Utf8Length );
    if ( pcbBuf )
        *pcbBuf = ULONG32(sizeof( RSDSI ) + Utf8Length);
    return S_OK;
}

//-----------------------------------------------------------
// SymWriter GetDebugInfo
// Get the size and potentially the debug info
//-----------------------------------------------------------
COM_METHOD SymWriter::GetDebugInfo(
    IMAGE_DEBUG_DIRECTORY *pIDD,  // [out] IDD to fill in
    DWORD cData,                  // [optional] size of data
    DWORD *pcData,                // [optional, out] return needed size for DebugInfo
    BYTE data[])                  // [optional] Buffer to store into
{
    HRESULT hr = S_OK;
    if ( cData == 0 && pcData != NULL ) {   // just checking for the size
        return GetDebugCVInfo( cData, (UINT *)pcData, data );
    }
    if ( pIDD == NULL )
        return E_INVALIDARG;

    DWORD cTheData = 0;
    IfFailGo( GetDebugCVInfo( cData, (UINT *)&cTheData, data ) );

    memset( pIDD, 0, sizeof( *pIDD ) );
    pIDD->Type = VAL32(IMAGE_DEBUG_TYPE_CODEVIEW);
    pIDD->SizeOfData = VAL32(cTheData);

    if ( pcData ) {
        *pcData = cTheData;
    }

ErrExit:
    return hr;
}

COM_METHOD SymWriter::RemapToken(mdToken oldToken, mdToken newToken)
{
    HRESULT hr = NOERROR;
    if (oldToken != newToken)
    {
        // We only care about methods
        if ((TypeFromToken(oldToken) == mdtMethodDef) ||
            (TypeFromToken(newToken) == mdtMethodDef))
        {
            // Make sure they are both methods
            _ASSERTE(TypeFromToken(newToken) == mdtMethodDef);
            _ASSERTE(TypeFromToken(oldToken) == mdtMethodDef);

            // Make sure we sort before saving
            m_sortMethodEntries = true;

            // Check to see if we're trying to map a token we know about
            unsigned i;
            for (i = 0; i < m_MethodInfo.m_methods.count(); i++)
            {
                if (m_MethodInfo.m_methods[i].MethodToken() == oldToken)
                {
                    // Remember the map, we need to actually do the actual
                    // mapping later because we might already have a function
                    // with a token 'newToken'
                    SymMap *pMethodMap;
                    IfNullGo( pMethodMap = m_MethodMap.next() );
                    pMethodMap->m_MethodToken = newToken;
                    pMethodMap->MethodEntry = i;                        
                    break;
                }
            }
        }
    }
ErrExit:
    return hr;
}

//-----------------------------------------------------------
// SymWriter Write
// Write the information to a file or to a stream
//-----------------------------------------------------------
COM_METHOD SymWriter::Write(void *pData, DWORD SizeOfData)
{
    HRESULT hr = NOERROR;
    DWORD NumberOfBytesWritten;
    if (m_pIStream)
    {
        IfFailGo(m_pIStream->Write(pData,
                                   SizeOfData,
                                   &NumberOfBytesWritten));
    }
    else
    {
        // Write out a signature to recognize that we're an ildb
        if (!WriteFile(m_hFile, pData, SizeOfData, &NumberOfBytesWritten, NULL))
            return HrFromWin32(GetLastError());
    }
    _ASSERTE(NumberOfBytesWritten == SizeOfData);
ErrExit:
    return hr;
}

//-----------------------------------------------------------
// SymWriter WriteStringPool
// Write the information to a file or to a stream
//-----------------------------------------------------------
COM_METHOD SymWriter::WriteStringPool()
{
    IStream *pIStream = NULL;
    BYTE *pStreamMem = NULL;

    HRESULT hr = NOERROR;
    if (m_pIStream)
    {
        IfFailGo(m_pStringPool->PersistToStream(m_pIStream));
    }
    else
    {
        LARGE_INTEGER disp = { {0, 0} };
        DWORD NumberOfBytes;
        DWORD SizeOfData;
        STATSTG statStg;

        IfFailGo(CreateStreamOnHGlobal(NULL,
                                       TRUE,
                                       &pIStream));

        IfFailGo(m_pStringPool->PersistToStream(pIStream));

        IfFailGo(pIStream->Stat(&statStg, STATFLAG_NONAME));
        SizeOfData = statStg.cbSize.u.LowPart;

        IfFailGo(pIStream->Seek(disp, STREAM_SEEK_SET, NULL));

        pStreamMem = NEW(BYTE[SizeOfData]);
        IfFailGo(pIStream->Read(pStreamMem, SizeOfData, &NumberOfBytes));

        if (!WriteFile(m_hFile, pStreamMem, SizeOfData, &NumberOfBytes, NULL))
            return HrFromWin32(GetLastError());

        _ASSERTE(NumberOfBytes == SizeOfData);

    }
ErrExit:
    RELEASE(pIStream);
    DELETEARRAY(pStreamMem);
    return hr;
}

//-----------------------------------------------------------
// SymWriter WritePDB
// Write the PDB information to a file or to a stream
//-----------------------------------------------------------
COM_METHOD SymWriter::WritePDB()
{

    HRESULT hr = NOERROR;
    GUID ildb_guid = ILDB_VERSION_GUID;

    // Make sure the ModuleLevelInfo is set
    ModuleLevelInfo.m_CountOfVars = VAL32(m_MethodInfo.m_vars.count());
    ModuleLevelInfo.m_CountOfBytes = VAL32(m_MethodInfo.m_bytes.count());
    ModuleLevelInfo.m_CountOfUsing = VAL32(m_MethodInfo.m_usings.count());
    ModuleLevelInfo.m_CountOfScopes = VAL32(m_MethodInfo.m_scopes.count());
    ModuleLevelInfo.m_CountOfMethods = VAL32(m_MethodInfo.m_methods.count());
    if (m_pStringPool)
    {
        DWORD dwSaveSize;
        IfFailGo(m_pStringPool->GetSaveSize(&dwSaveSize));
        ModuleLevelInfo.m_CountOfStringBytes = VAL32(dwSaveSize);
    }
    else
    {
        ModuleLevelInfo.m_CountOfStringBytes = 0;
    }
    ModuleLevelInfo.m_CountOfConstants = VAL32(m_MethodInfo.m_constants.count());
    ModuleLevelInfo.m_CountOfDocuments = VAL32(m_MethodInfo.m_documents.count());
    ModuleLevelInfo.m_CountOfSequencePoints = VAL32(m_MethodInfo.m_auxSequencePoints.count());

    // Open the file
    if (m_pIStream == NULL)
    {
        // We need to open the output file.
        m_hFile = CreateFile(m_szPath,
                            GENERIC_WRITE,
                            0,
                            NULL,
                            CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);

        if (m_hFile == INVALID_HANDLE_VALUE)
        {
            IfFailGo(HrFromWin32(GetLastError()));
        }
    }

#if _DEBUG
    // We need to make sure the Variant entry in the constants is 8 byte
    // aligned so make sure everything up to the there is aligned correctly
    if ((ILDB_SIGNATURE_SIZE % 8) || 
        (sizeof(PDBInfo) % 8) ||
        (sizeof(GUID) % 8))
    {
        _ASSERTE(!"We need to safe the data in an aligned format");
    }
#endif

    // Write out a signature to recognize that we're an ildb
    IfFailGo(Write((void *)ILDB_SIGNATURE, ILDB_SIGNATURE_SIZE));
    // Write out a guid representing the version
    SwapGuid(&ildb_guid);
    IfFailGo(Write((void *)&ildb_guid, sizeof(GUID)));

    // Now we need to write the Project level 
    IfFailGo(Write(&ModuleLevelInfo, sizeof(PDBInfo)));

    // Now we have to write out each array as appropriate
    IfFailGo(Write(m_MethodInfo.m_constants.m_array, sizeof(SymConstant) * m_MethodInfo.m_constants.count()));

    // These members are all 4 byte aligned
    IfFailGo(Write(m_MethodInfo.m_methods.m_array, sizeof(SymMethodInfo) * m_MethodInfo.m_methods.count()));
    IfFailGo(Write(m_MethodInfo.m_scopes.m_array, sizeof(SymLexicalScope) * m_MethodInfo.m_scopes.count()));
    IfFailGo(Write(m_MethodInfo.m_vars.m_array, sizeof(SymVariable) * m_MethodInfo.m_vars.count()));
    IfFailGo(Write(m_MethodInfo.m_usings.m_array, sizeof(SymUsingNamespace) * m_MethodInfo.m_usings.count()));
    IfFailGo(Write(m_MethodInfo.m_auxSequencePoints.m_array, sizeof(SequencePoint) * m_MethodInfo.m_auxSequencePoints.count()));
    IfFailGo(Write(m_MethodInfo.m_documents.m_array, sizeof(DocumentInfo) * m_MethodInfo.m_documents.count()));
    IfFailGo(Write(m_MethodInfo.m_bytes.m_array, sizeof(BYTE) * m_MethodInfo.m_bytes.count()));
    IfFailGo(WriteStringPool());

ErrExit:
    if (m_hFile)
        CloseHandle(m_hFile);
    return hr;
}

/* ------------------------------------------------------------------------- *
 * SymDocumentWriter class
 * ------------------------------------------------------------------------- */
extern "C"
HRESULT SymDocumentWriter_Release(ISymUnmanagedDocumentWriter *pDocumentWriter)
{
    if (pDocumentWriter)
        pDocumentWriter->Release();;
    return NOERROR;
}

SymDocumentWriter::SymDocumentWriter(
    UINT32 DocumentEntry,
    SymWriter  *pEmitter
) :
    m_refCount ( 0 ),
    m_DocumentEntry ( DocumentEntry ),
    m_pEmitter( pEmitter ),
    m_pcheckSum( 0 ),
    m_source( NULL ), 
    m_written( false )
{

    _ASSERTE(pEmitter);
    m_pEmitter->AddRef();
}

SymDocumentWriter::~SymDocumentWriter()
{
    Close();

    if ( m_pcheckSum )
        DELETEARRAY(m_pcheckSum);

    if ( m_source )
        DELETEARRAY(m_source);
}

COM_METHOD SymDocumentWriter::QueryInterface(REFIID riid, void **ppInterface)
{
    if (riid == IID_ISymUnmanagedDocumentWriter)
        *ppInterface = (ISymUnmanagedDocumentWriter*)this;
    else if (riid == IID_IUnknown)
        *ppInterface = (IUnknown*)(ISymUnmanagedDocumentWriter*)this;
    else
    {
        *ppInterface = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

//-----------------------------------------------------------
// SymDocumentWriter SetSource
//-----------------------------------------------------------
extern "C"
COM_METHOD SymDocumentWriter_SetSource(ISymUnmanagedDocumentWriter *pDocumentWriter,
                                       ULONG32 sourceSize,
                                       BYTE source[])
{
    HRESULT hr = NOERROR;
    IfFalseGo(pDocumentWriter, E_INVALIDARG);
    hr = pDocumentWriter->SetSource(sourceSize, source);

ErrExit:
    return hr;
}
COM_METHOD SymDocumentWriter::SetSource(ULONG32 sourceSize,
                                        BYTE source[])
{
    if ( sourceSize > 0 && source == NULL )
        return E_INVALIDARG;

    m_source = NEW(BYTE[ sourceSize ]);
    if ( m_source == NULL )
        return E_OUTOFMEMORY;

    memcpy( m_source, source, sourceSize );
    m_SourceSize = sourceSize;

    return E_FAIL;
}

//-----------------------------------------------------------
// SymDocumentWriter Close
//-----------------------------------------------------------
HRESULT SymDocumentWriter::Close()
{
    HRESULT hr = NOERROR;
    if ( m_written )
        return NOERROR;    // already written, just ignore


    hr = m_pEmitter->DefineSrc(m_DocumentEntry,
                               m_AlgorithmId,
                               m_CheckSumSize,
                               m_pcheckSum,
                               m_SourceSize,
                               m_source );

    m_written = true;

    m_pEmitter->DocumentReleased( this );
    RELEASE(m_pEmitter);

    return hr;
}

//-----------------------------------------------------------
// SymDocumentWriter SetCheckSum
//-----------------------------------------------------------
extern "C"
COM_METHOD SymDocumentWriter_SetCheckSum(
    ISymUnmanagedDocumentWriter *pDocumentWriter,
    GUID algorithmId,
    ULONG32 checkSumSize,
    BYTE checkSum[])
{
    HRESULT hr = NOERROR;
    IfFalseGo(pDocumentWriter, E_INVALIDARG);
    hr = pDocumentWriter->SetCheckSum(algorithmId, checkSumSize, checkSum);

ErrExit:
    return hr;
}
COM_METHOD SymDocumentWriter::SetCheckSum(GUID algorithmId,
                                          ULONG32 checkSumSize,
                                          BYTE checkSum[])
{
    if ( m_written )
        return E_UNEXPECTED;

    m_AlgorithmId = algorithmId;
    SwapGuid(&m_AlgorithmId);
    m_CheckSumSize = checkSumSize;
    if ( checkSumSize > 0 ) {
        m_pcheckSum = NEW(BYTE[ checkSumSize ]);
        if ( m_pcheckSum == NULL )
            return E_OUTOFMEMORY;
        memcpy( m_pcheckSum, checkSum, checkSumSize );
    }
    return S_OK;
}
