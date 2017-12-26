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
// File: srcmod.h
//
// ===========================================================================

#ifndef __srcmod_h__
#define __srcmod_h__

#include "csiface.h"
#include "tokinfo.h"
#include "alloc.h"
#include "nodes.h"
#include "locks.h"
#include "namemgr.h"
#include "table.h"
#include "map.h"
#include "lexer.h"
#include "parser.h"
#include "array.h"
#include "objptr.h"

class CSourceModule;
class CError;

////////////////////////////////////////////////////////////////////////////////
// CSourceLexer
//
// This is the derivation of CLexer that is used by source modules to do
// the main lexing of a source file.

class CSourceLexer : public CLexer
{
private:
    CSourceModule   *m_pModule;

public:
    CSourceLexer (CSourceModule *pMod);

    // Context-specific implementation...
    void            TrackLine (PCWSTR pszNewLine);
    BOOL            ScanPreprocessorLine (PCWSTR &p);
    void            RecordCommentPosition (const POSDATA &pos);
    BOOL            RepresentNoiseTokens ();
    void            *TokenMemAlloc (CSTOKEN *pToken, size_t iSize);

protected:
    void ErrorPosArgs(long iLine, long iCol, long iExtent, int iErrorId, int carg, ErrArg * prgarg);
};

////////////////////////////////////////////////////////////////////////////////
// CSourceParser
//
// This is the derivation of CParser that is used by source modules for parsing.

class CSourceParser : public CParser
{
private:
    CSourceModule   *m_pModule;
    virtual void    ReportFeatureUse(DWORD dwFeatureId);

protected:
    virtual bool SupportsErrorSuppression() { return true; }
    virtual CErrorSuppression GetErrorSuppression();

public:
    CSourceParser (CSourceModule *pMod, int mid);

    // Virtuals from CParser
    void    *MemAlloc (long iSize);
    BOOL    CreateNewError (int iErrorId, int carg, ErrArg * prgarg, const POSDATA &posStart, const POSDATA &posEnd);
    void    AddToNodeTable (BASENODE *pNode);
    void    GetInteriorTree (CSourceData *pData, BASENODE *pNode, ICSInteriorTree **ppTree);
    HRESULT GetLexData(LEXDATA* pLexData);

    // Overrides
    virtual BASENODE *ParseSourceModule();
};


////////////////////////////////////////////////////////////////////////////////
// CModuleEventSource

class CModuleEventSource
{
public:
    void        FireOnModuleModified () { }
    void        FireOnStateTransitionsChanged (long iFirst, long iLast) { }
    void        FireOnCommentTableChanged (long iStart, long iOldEnd, long iNewEnd) { }
    void        FireOnBeginTokenization (long iFirstLine) { }
    void        FireOnEndTokenization (ICSSourceData *pData, long iFirst, long iLast, long iDelta) { }
    void        FireOnBeginParse (BASENODE *pNode) { }
    void        FireOnEndParse (BASENODE *pNode) { }
    void        FireReportErrors (ICSErrorContainer *pErrors) { }
};


class CController;
struct BASENODE;
class CInteriorNode;
class CSourceData;
class CErrorContainer;

enum PPTOKENID;

////////////////////////////////////////////////////////////////////////////////
// THREADDATAREF

struct THREADDATAREF
{
    DWORD       dwTid;
    long        iRef;
};

////////////////////////////////////////////////////////////////////////////////
// SKIPFLAGS -- flags directing behavior of, and indicating results of, SkipBlock()

enum SKIPFLAGS
{
    // Behavior direction flags
    SBF_INACCESSOR      = 0x00000001,   // The block to be skipped is a getter/setter

    // we're skipping over a block in a property.  So we may have something like:
    // public int T {
    //      private s
    //
    // In this case, the private s will look like the start of a new member.  But 
    // it's actually the start of a setter.  So we want to detect this and not treat
    // it as the start of a new member
    SBF_INPROPERTY      = 0x00000002,

    // Result flags
    SB_NORMALSKIP       = 0,            // Normal skip; no errors
    SB_NOTAMEMBER       = 0,            // Returned by ScanMemberDeclaration in failure case
    SB_TYPEMEMBER,                      // Detected a type member declaration (beginning at current token)
    SB_NAMESPACEMEMBER,                 // Detected a namespace member declaration (at current token)
    SB_ACCESSOR,                        // Detected an accessor (at current token)
    SB_ENDOFFILE,                       // Hit end of file (error reported)
};

////////////////////////////////////////////////////////////////////////////////
// CCFLAGS
//
// Flags indicating nuances of CC state changes and stack records

enum CCFLAGS
{
    CCF_REGION  = 0x0001,           // Record is for #region (as opposed to #if)
    CCF_ELSE    = 0x0002,           // Indicates an #else block (implies !CCF_REGION)
    CCF_ENTER   = 0x0004,           // State change only -- indicates entrance (push) as opposed to exit (pop)
};

////////////////////////////////////////////////////////////////////////////////
// CCREC -- conditional compilation stack record.  This includes #region
// directives as well.

struct CCREC
{
    DWORD       dwFlags;            // CCF_* flags
    CCREC       *pPrev;             // Previous state record
};

////////////////////////////////////////////////////////////////////////////////
// CCSTATE -- conditional compilation state change record

struct CCSTATE
{
    unsigned long   dwFlags:3;      // CCF_* flags
    unsigned long   iLine:29;       // Line number of CC directive causing change
};


////////////////////////////////////////////////////////////////////////////////
// CNodeTable, NODECHAIN

struct NODECHAIN
{
    long        iGlobalOrdinal;     // Index of this node in global-ordinal array
    NODECHAIN   *pNext;             // Next node with the same key (key-ordinal + 1)
};

typedef CTable<NODECHAIN>   CNodeTable;

////////////////////////////////////////////////////////////////////////////////
// LEXDATABLOCK
//
// NOTE:  This structure is used by the source module to represent all data
// calculated and/or generated by lexing.  It derives from LEXDATA, which is the
// exposed version, and also includes some extra goo.

struct LEXDATABLOCK : public LEXDATA
{
    // Source text
    long        iTextLen;

    // Token stream
    long        iAllocTokens;

    // Line offset table
    long        iAllocLines;

    // Comment table
    long        iAllocComments;

    // CC states
    CCSTATE     *pCCStates;
    long        iCCStates;

    void CloneTo(CController *pController, MEMHEAP *pStdHeap, PCWSTR pszCloneSourceText, LEXDATABLOCK &lexClone);
};

////////////////////////////////////////////////////////////////////////////////
// MERGEDATA

struct MERGEDATA
{
    long        iTokStart;              // First token in token stream to be scanned
    long        iTokStop;               // Last token to be rescanned (in incremental retok is successful)
    BOOL        fStartAtZero;           // TRUE if scanner is starting at 0,0
    POSDATA     posStart;               // Position to start scanning at
    TOKENID     tokFirst;               // Token ID of iTokStart (old)
    POSDATA     posStop;                // Position (relative to NEW buffer) of stop token (if retok is successful)
    POSDATA     posStopOld;             // Same as above, only relative to OLD buffer

    POSDATA     posEditOldEnd;          // Copy of m_posEditOldEnd
    POSDATA     posEditNewEnd;          // Copy of m_posEditNewEnd
    POSDATA     posOldEOF;              // Position of TID_ENDFILE (old buffer)

    BOOL        fIncrementalStop;       // TRUE if retokenization stopped successfully
    long        iLineDelta;             // Change in line count
    long        iCharDelta;             // Change in character position of first unmodified character
    long        iStreamOldEnd;          // STREAM position of first unmodified character (old)

    long        iFirstAffectedCCState;  // First CC state record affected by change (calculated in ReconstructCCStack)
    BOOL        fStreamChanged;         // TRUE if token stream changed
    BOOL        fTransitionsChanged;    // TRUE if transition lines have changed
    BOOL        fParseWarningsChanged;  // TRUE if a parser warning was enabled or disabled

    long        iStartComment;          // First changed comment
    long        iOldEndComment;         // First unchanged comment (in old table)
    long        iNewEndComment;         // First unchanged comment (in new table)
};

////////////////////////////////////////////////////////////////////////////////
// ChecksumData

struct ChecksumData
{
    NAME *      pFilename;
    GUID        guidChecksumID;
    BYTE *      pbChecksumData;
    int         cbChecksumData;
    ERRLOC *    pErrlocChecksum;
};

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase
//
//  Base class for a source module as seen by a CSourceData object.
//  This allows a CSourceData object to be cloned to an immutable source module,
//  allowing a non-locking ICSSourceData object to be created.

class CSourceModuleBase : public IUnknown
{
private:
    LONG                        m_iRef;                         // Ref count

protected:
    // Owner
    CController *               m_pController;                  // Owning controller

    // Heaps
    MEMHEAP *                   m_pStdHeap;                     // Standard heap (from controller)

    // Parser data
    CParser *                   m_pParser;                      // This would be our parser...

    // State lock
    CTinyLock                   m_StateLock;                    // Lock for state data

    // Lexer data
    LEXDATABLOCK                m_lex;
    CLineMap                    m_LineMap;                      // Map of source lines to #lines
    NAMETAB                     m_tableDefines;                 // Table of defined preprocessor symbols

    // Source text and edit data
    CComPtr<ICSSourceText>      m_spSourceText;                 // Source text object

    // Data accumulated during parse
    BASENODE *                  m_pTreeTop;                     // Top of parse tree
    CNodeTable *                m_pNodeTable;                   // Table of key-indexable nodes
    CStructArray<KEYEDNODE> *   m_pNodeArray;                   // Array of key-indexable nodes

    CCoreOptionData m_options;
    bool m_fFetchedOptions;

protected:
    ULONG InternalAddRef();
    ULONG InternalRelease();
    virtual void  InternalFlush();
    virtual void  FinalRelease();

    CInteriorNode **GetInteriorNodeAddress(BASENODE *pNode);
    HRESULT         ParseInteriorsForErrors (CSourceData *pData, BASENODE *pNode);
    void            ParseInteriorNode (BASENODE *pNode);

public:
    CSourceModuleBase(CController *pController);
    virtual ~CSourceModuleBase();

    // Access to our state lock for external serialization
    CTinyLock   *GetStateLock () { return &m_StateLock; }

    CController *GetController() { return m_pController; }

    // Some accessor functions for things we get from the controller
    NAMEMGR         *GetNameMgr ();
    BOOL            CheckFlags (DWORD dwFlags);
    CCoreOptionData     *GetOptions()
    { 
        ASSERT(m_fFetchedOptions);
        return &m_options;
    }

    //
    // Virtual methods
    //
    virtual void    MapLocation(POSDATA *pos, PCWSTR *ppszFilename, bool *pbIsHidden, bool *pbIsMapped);
    virtual void    MapLocation(POSDATA *pos, NAME **pnameFile, bool *pbIsHidden, bool *pbIsMapped);
    virtual void    MapLocation(long *line, NAME **pnameFile, bool *pbIsHidden, bool *pbIsMapped);
    virtual BOOL    hasMap();

    virtual PCWSTR  GetFileName();

    virtual BOOL    IsSymbolDefined(PNAME symbol);

    virtual HRESULT GetDocCommentText (BASENODE *pNode, __deref_out PWSTR *ppszText, long *piFirstComment = NULL, long *piLastComment = NULL, CXMLMap * srcMap = NULL, long * piLines = NULL, PCWSTR pszIndent = NULL);
    virtual HRESULT FreeDocCommentText (__deref_inout PWSTR *ppszText);
    virtual HRESULT FindDocCommentPattern (LEXDATA *pLex, unsigned long iLine, long *piPatternCnt, PCWSTR * pszPattern);
    virtual HRESULT MatchDocCommentPattern (PCWSTR pszLine, PCWSTR pszPattern, long *piPatternCnt);
    virtual HRESULT GetInteriorParseTree (CSourceData *pData, BASENODE *pNode, ICSInteriorTree **ppTree);
    virtual HRESULT LookupNode (CSourceData *pData, NAME *pKey, long iOrdinal, BASENODE **ppNode, long *piGlobalOrdinal);
    virtual HRESULT GetNodeKeyOrdinal (CSourceData *pData, BASENODE *pNode, NAME **ppKey, long *piKeyOrdinal);
    virtual HRESULT GetGlobalKeyArray (CSourceData *pData, KEYEDNODE *pKeyedNodes, long iSize, long *piCopied);
    virtual HRESULT GetSingleTokenData (CSourceData *pData, long iToken, TOKENDATA *pTokenData);
    virtual HRESULT GetSingleTokenPos (CSourceData *pData, long iToken, POSDATA *pposToken);
    virtual HRESULT IsInsideComment (CSourceData *pData, const POSDATA &pos, BOOL *pfInComment);
    virtual HRESULT GetLexResults (CSourceData *pData, LEXDATA *pLexData);
    virtual HRESULT GetTokenText (long iTokenId, PCWSTR *ppszText, long *piLength);
    virtual HRESULT FindLeafNode (CSourceData *pData, const POSDATA pos, BASENODE **ppNode, ICSInteriorTree **ppTree);
    virtual HRESULT FindLeafNodeEx (CSourceData *pData, const POSDATA pos, ExtentFlags flags, BASENODE **ppNode, ICSInteriorTree **ppTree);
    virtual HRESULT FindLeafNodeForToken (CSourceData *pData, long iToken, BASENODE **ppNode, ICSInteriorTree **ppTree);
    virtual HRESULT FindLeafNodeForTokenEx (CSourceData *pData, long iToken, ExtentFlags flags, BASENODE **ppNode, ICSInteriorTree **ppTree);
    virtual HRESULT GetDocComment (BASENODE *pNode, long *piComment, long *piCount);
    virtual HRESULT GetDocCommentXML (BASENODE *pNode, BSTR *pbstrDoc);
    virtual HRESULT GetExtent (BASENODE *pNode, POSDATA *pposStart, POSDATA *pposEnd, ExtentFlags flags);
    virtual HRESULT GetTokenExtent (BASENODE *pNode, long *piFirst, long *piLast);
    virtual BOOL    IsWarningDisabled(CError * pError);
    virtual HRESULT GetChecksum(Checksum * checksum);
    virtual HRESULT MapSourceLine (CSourceData *pData, long iLine, long *piMappedLine, PCWSTR *ppszFilename, BOOL *pbIsHidden);
    virtual void    AddToNodeTable (BASENODE *pNode);
    virtual HRESULT OnRename(NAME * pNewName);


    //
    // Abstract methods
    //
    virtual void    AddDataRef() PURE;
    virtual void    ReleaseDataRef() PURE;
    virtual void *  AllocFromActiveHeap(size_t iSize) PURE;
    virtual HRESULT GetMemoryConsumptionPrivate(long *piTopTree, long *piInteriorTrees, long *piInteriorNodes) PURE;
    virtual HRESULT GetICSSourceModule (ICSSourceModule **ppModule) PURE;
    virtual HRESULT Clone (ICSSourceData *pData, CSourceModuleBase **ppClone) PURE;
    virtual HRESULT GetInteriorNode (CSourceData *pData, BASENODE *pNode, CInteriorNode **ppInteriorNode) PURE;
    virtual HRESULT GetErrors (CSourceData *pData, ERRORCATEGORY iCategory, ICSErrorContainer **ppErrors) PURE;
    virtual HRESULT ParseTopLevel (CSourceData *pData, BASENODE **ppNode, BOOL fCreateParseDiffs = FALSE) PURE;
    virtual HRESULT IsUpToDate (BOOL *pfTokenized, BOOL *pfTopParsed) PURE;
    virtual HRESULT GetLastRenamedTokenIndex (long *piTokIdx, NAME **ppPreviousName) PURE;
    virtual HRESULT ResetRenamedTokenData () PURE;
    virtual HRESULT ParseForErrors (CSourceData *pData) PURE;
    virtual NRHEAP * GetActiveHeap() PURE;
};

typedef CSmartPtr<CSourceModuleBase> CSourceModuleBasePtr;

////////////////////////////////////////////////////////////////////////////////
// CSourceModule inlined accessors

__forceinline NAMEMGR *CSourceModuleBase::GetNameMgr () 
{ 
    return m_pController->GetNameMgr(); 
}

__forceinline BOOL CSourceModuleBase::CheckFlags (DWORD dwFlags) 
{ 
    return m_pController->CheckFlags (dwFlags); 
}


////////////////////////////////////////////////////////////////////////////////
// CSourceModule
//
// This is the object that implements ICSSourceModule.  It also holds all of
// the data to which callers can access ONLY through ICSSourceData objects.
//
// Clearly, there is a LOT of state contained in a source module.  Most of this
// has to do with the complexities involved in incremental lexing, compounded by
// free-threadedness.
//
// When adding any new state/member variables, please try to conform to the
// organization of logical groupings, and be aware of multi-threaded access and
// whether or not your change affects incremental lexing.

class CSourceModule :
    public CSourceModuleBase,
    public ICSSourceModule,
    public ICSSourceTextEvents
{
    friend class CSourceLexer;
    friend class CSourceParser;

    enum { COMMENT_BLOCK_SIZE = 32 };       // NOTE:  Must be power of 2...

private:
    // Reference counting (per-thread and global)
    long            m_iDataRef;                     // Data ref count (outstanding ICSSourceData object count)
    THREADDATAREF   *m_pThreadDataRef;              // Per-thread data ref counts
    long            m_iThreadCount;                 // Number of threads represented in array

    // Source text and edit data
    DWORD_PTR       m_dwTextCookie;                 // Event cookie for edit events from text
    POSDATA         m_posEditStart;                 // Accumulated edit start point
    POSDATA         m_posEditOldEnd;                // Accumulated edit old end point
    POSDATA         m_posEditNewEnd;                // Accumulated edit new end point

    // CC data
    CCREC           *m_pCCStack;                    // Stack of CC records

    // Incremental lexing data
    DWORD           m_dwVersion;                    // "Version" of the current token stream
    long            m_iTokDelta;                    // Change in token count (if m_fTokensChanged)
    long            m_iTokDeltaFirst;               // First changed token (if m_fTokensChanged)
    long            m_iTokDeltaLast;                // Last changed token (in NEW stream) (if m_fTokensChanged)

    // Data accumulated during parse
    CModuleEventSource          m_EventSource;      // Event source
    long                        m_iErrors;          // Used ONLY to detect errors report during some time span...

    // Heaps
    NRHEAP          m_heap1;                        // Heap for allocating nodes
    NRHEAP          m_heap2;                        // Heap for allocating nodes (2 NRHEAPS required to persist old parse tree - since NRHEAP is a "no release" allocator)
    NRMARK          m_markInterior1;                // Start point for interior node parse
    NRMARK          m_markInterior2;                // Start point for interior node parse
    NRMARK          m_markTokens1;                  // Start point for top-level node parse
    NRMARK          m_markTokens2;                  // Start point for top-level node parse
    NRHEAP          *m_pActiveTopLevelHeap;         // "Active" top-level allocation heap
    NRHEAP          *m_pActiveHeap;                 // "Active" allocation heap
    NRHEAP          *m_pHeapForFirstTokenization;        // Heap that was used for the first tokenization
    NRMARK          *m_pMarkInterior;               // "Active" start point for interior node parse
    NRMARK          *m_pMarkTokens;                 // "Active" start point for top-level parse

    // #line data
    PWSTR           m_pszSrcDir;                    // Directory of source file (used only for LineMap)
    CStructArray<ChecksumData>
                    m_rgChecksum;                   // Array of #pragma checksum lines

    // Parser data
    long            m_iCurTok;                      // Current token

    // Error tracking
    CErrorContainer *m_pTokenErrors;                // Container for EC_TOKENIZATION errors
    CErrorContainer *m_pParseErrors;                // Container for EC_TOPLEVELPARSE errors
    CErrorContainer *m_pCurrentErrors;              // Depending on mode, points to one of the above or an interior parse container

    // Bits/Flags/etc...
    //
    // These bits are protected by m_StateLock
    unsigned        m_fTextModified:1;              // TRUE if text has been modified since last tokenization
    unsigned        m_fEventsArrived:1;             // TRUE if text change events have arrived since last attempt to acquire text
    unsigned        m_fEditsReceived:1;             // TRUE if text change events have arrived since creation
    unsigned        m_fTextStateKnown:1;            // TRUE if mods to text are known in m_posEdit* vars (i.e. no disconnects since last tokenization)
    unsigned        m_fInteriorHeapBusy:1;          // TRUE if a CPrimaryInteriorNode exists
    unsigned        m_fTokenizing:1;                // TRUE if a thread has committed to tokenizing current text
    unsigned        m_fTokenized:1;                 // TRUE if token stream for current text is available
    unsigned        m_fFirstTokenization:1;         // TRUE if this is the first tokenization for this module (token allocs can come from NR heap)
    unsigned        m_fParsingTop:1;                // TRUE if a thread has committed to parsing the top level
    unsigned        m_fParsedTop:1;                 // TRUE if the top level parse tree for the current text is available
    unsigned        m_fTokensChanged:1;             // TRUE if token stream has changed since m_fParsedTop was set
    unsigned        m_fForceTopParse:1;             // TRUE if rescanned tokens contained { or } tokens
    unsigned        m_fParsingInterior:1;           // TRUE if a thread is parsing an interior node

    unsigned        m_bitpad:19;                    // This pad ensures that mods to the unprotected bits don't whack the protected ones

    // These bits are NOT PROTECTED and must be kept on the "other side" of the pad
    unsigned        m_fLimitExceeded:1;             // TRUE if line count/length limit exceeded on last tokenization
    unsigned        m_fErrorOnCurTok:1;             // TRUE if an error has been reported on the current token
    unsigned        m_fCCSymbolChange:1;            // TRUE if a CC symbol was defined/undefined
    unsigned        m_fParsingForErrors:1;          // TRUE if in ParseForError call only
    unsigned        m_fCrashed:1;                   // TRUE if exception happend during parse/lex
    unsigned        m_fLeafCrashed:1;               // TRUE if exception happened during FindLeafNode
    unsigned        m_fTrackLines;                  // TRUE if line tracking should occur (first time tokenization; creates line table)
                                                    // Note: Removed from bitset to avoid concurrency issues with fields above

    // Memory consumption tracking                                                  
    BOOL            m_fAccumulateSize;              // Set this if size accumulation is desired (during parse for errors)
    DWORD           m_dwInteriorSize;               // Accumulation of interior parse tree space
    long            m_iInteriorTrees;               // Number of interior trees parsed (during parse for errors)

    // Token index to keep track of renaming events...
    long            m_iLastTokenRenamed;            // Index of the last token that was renamed
    NAME *          m_pPreviousTokenName;

#ifdef _DEBUG
    // Debug-only data
    DWORD           m_dwTokenizerThread;            // These three values are used for asserting thread behavior during parsing...
    DWORD           m_dwParserThread;
    DWORD           m_dwInteriorParserThread;
    void            SetTokenizerThread () { m_dwTokenizerThread = GetCurrentThreadId(); }
    void            ClearTokenizerThread () { m_dwTokenizerThread = 0; }
    void            SetParserThread () { m_dwParserThread = GetCurrentThreadId(); }
    void            ClearParserThread () { m_dwParserThread = 0; }
    void            SetInteriorParserThread () { m_dwInteriorParserThread = GetCurrentThreadId(); }
    void            ClearInteriorParserThread () { m_dwInteriorParserThread = 0; }
#else
    void            SetTokenizerThread () {}
    void            ClearTokenizerThread () {}
    void            SetParserThread () {}
    void            ClearParserThread () {}
    void            SetInteriorParserThread () {}
    void            ClearInteriorParserThread () {}
#endif

    // Tokenization routines
    BOOL            WaitForTokenization ();
    //long            FindNearestPosition (POSDATA *ppos, long iSize, const POSDATA &pos);
    void            DetermineStartData (LEXDATABLOCK &old, MERGEDATA &md, BOOL *pfIncremental);
    void            ReconstructCCState (LEXDATABLOCK &old, MERGEDATA &md, CCREC **ppCopy, NAMETAB *pOldTable);
    void            MergeTokenStream (LEXDATABLOCK &old, MERGEDATA &md);
    void            MergeCommentTable (LEXDATABLOCK &old, MERGEDATA &md);
    void            MergeCCState (LEXDATABLOCK &old, MERGEDATA &md);
    void            MergeLineTable (LEXDATABLOCK &old, MERGEDATA &md);
    void            UpdateRegionTable ();
    static LONG     HandleExceptionFromTokenization (EXCEPTION_POINTERS * exceptionInfo, PVOID pv);
    void            InternalCreateTokenStream (ICSSourceData *pData);
    void            CreateTokenStream (ICSSourceData *pData);
    HRESULT         GetTextAndConnect ();
    void            InternalFlush ();
    void            AcceptEdits ();
    void            TrackLine (CLexer *pCtx, PCWSTR pszNewLine);
    void            RecordCommentPosition (const POSDATA &pos);
    BOOL            RepresentNoiseTokens ();
    void            *TokenMemAlloc (CSTOKEN *pToken, size_t iSize);

    void            ScanPreprocessorLine (CLexer *pCtx, PCWSTR &pszCur, BOOL fFirstOnLine);
    void            ScanPreprocessorLineTerminator (CLexer *pCtx, PCWSTR &pszCur);
    void            ScanPreprocessorDirective (CLexer *pCtx, PCWSTR &pszCur);
    PPTOKENID       ScanPreprocessorToken (CLexer *pCtx, PCWSTR &p, PCWSTR &pszStart, NAME **ppName, bool allIds);
    bool            ScanPreprocessorNumber (PCWSTR p, long & number, CLexer *pCtx);
    bool            ScanPreprocessorFilename (PCWSTR &p, CLexer *pCtx, NAME ** ppFilename, bool fPragma);
    void            ScanPreprocessorPragma (PCWSTR &p, CLexer *pCtx);
    void            ScanPreprocessorPragmaWarning (PCWSTR &p, CLexer *pCtx);
    void            ScanPreprocessorPragmaChecksum (PCWSTR &p, CLexer *pCtx);
    void            ScanPreprocessorDeclaration (PCWSTR &p, CLexer *pCtx, BOOL fDefine);
    void            ScanPreprocessorControlLine (PCWSTR &p, CLexer *pCtx, BOOL fError);
    void            ScanPreprocessorRegionStart (PCWSTR &p, CLexer *pCtx);
    void            ScanPreprocessorRegionEnd (PCWSTR &p, CLexer *pCtx);
    void            ScanPreprocessorIfSection (PCWSTR &p, CLexer *pCtx);
    void            ScanPreprocessorIfTrailer (PCWSTR &p, CLexer *pCtx);
    void            ScanPreprocessorLineDecl (PCWSTR &p, CLexer *pCtx);
    BOOL            EvalPreprocessorExpression (PCWSTR &p, CLexer *pCtx, PPTOKENID tokPrecedence = PPT_OR);
    void            ScanExcludedCode (PCWSTR &p, CLexer *pCtx);
    void            ScanAndIgnoreDirective (PPTOKENID tok, PCWSTR pszStart, PCWSTR &p, CLexer *pCtx);
    void            SkipRemainingPPTokens (PCWSTR &p, CLexer *pCtx);

    void            PushCCRecord (CCREC * &pStack, DWORD dwFlags);
    BOOL            PopCCRecord (CCREC * &pStack);
    BOOL            CompareCCStacks (CCREC *pOne, CCREC *pTwo);
    void            MarkTransition (long iLine);
    void            MarkCCStateChange (long iLine, DWORD dwFlags);
    void            ApplyDelta (POSDATA posX, POSDATA posCX, POSDATA posCY, POSDATA *pposY);
    void            UpdateTokenIndexesWrapper (NRHEAP * heap, BASENODE *pNode, BASENODE *pChild);
    void            UpdateTokenIndexes (BASENODE *pNode, BASENODE *pChild);
    void            UpdateTokenIndex (long &iToken);
    void            GetInteriorNodeWorker(CInteriorNode ** ppStoredNode, BASENODE * pNode);
    void            ParseInteriorWorker(BASENODE * pNode);
    void            ParseSourceModuleWorker();
    void            MarkInteriorAsParsed(BASENODE * pNode, bool markAsParsed);

    BOOL            CreateNewError (int iErrorId, int carg, ErrArg * prgarg, const POSDATA &posStart, const POSDATA &posEnd);

    // Use LONG_PTR's here so we don't have to cast when calculating positions based on pointer arithmetic
    void ErrorAtPositionArgs(long iLine, long iCol, long iExtent, int id, int carg, ErrArg * prgarg);
    void ErrorAtPosition(LONG_PTR iLine, LONG_PTR iCol, LONG_PTR iExtent, int id) { ErrorAtPositionArgs((long)iLine, (long)iCol, (long)iExtent, id, 0, NULL); }
    void ErrorAtPosition(LONG_PTR iLine, LONG_PTR iCol, LONG_PTR iExtent, int id, ErrArg a);

    static LONG     ExceptionFilter (EXCEPTION_POINTERS * exceptionInfo, PVOID pv);

    NAME            *BuildNodeKey (BASENODE *pNode);

    CSourceModule (CController *pController, int mid);
    ~CSourceModule ();


public:
    static  HRESULT CreateInstance (CController *pController, ICSSourceText *pText, ICSSourceModule **ppStream, int mid);

    HRESULT Initialize (ICSSourceText *pText);

    // Source data reference management
    void    AddDataRef ();
    void    ReleaseDataRef ();
    long    FindThreadIndex (BOOL fMustExist);
    void    ReleaseThreadIndex (long iIndex);

    // CSourceModuleBase
    void    FinalRelease();
    HRESULT GetICSSourceModule (ICSSourceModule **ppModule);
    HRESULT Clone (ICSSourceData *pData, CSourceModuleBase **ppClone);
    HRESULT GetMemoryConsumptionPrivate(long *piTopTree, long *piInteriorTrees, long *piInteriorNodes);
    void *  AllocFromActiveHeap(size_t iSize);
    void    PrepareForCloning(ICSSourceData *pData);
    virtual NRHEAP * GetActiveHeap() { return m_pActiveHeap; }

    // Source data accessors
    HRESULT GetLexResults (CSourceData *pData, LEXDATA *pLexData);
    HRESULT UnsafeGetLexData (LEXDATA *pLexData);
    HRESULT GetSingleTokenData (CSourceData *pData, long iToken, TOKENDATA *pTokenData);
    HRESULT GetSingleTokenPos (CSourceData *pData, long iToken, POSDATA *pposToken);
    HRESULT IsInsideComment (CSourceData *pData, const POSDATA &pos, BOOL *pfInComment);
    HRESULT InternalIsInsideComment (CSourceData *pData, const POSDATA &pos, BOOL *pfInComment);
    HRESULT ParseTopLevel (CSourceData *pData, BASENODE **ppNode, BOOL fCreateParseDiffs = FALSE);
    HRESULT GetErrors (CSourceData *pData, ERRORCATEGORY iCategory, ICSErrorContainer **ppErrors);
    HRESULT GetInteriorParseTree (CSourceData *pData, BASENODE *pNode, ICSInteriorTree **ppTree);
    HRESULT ParseForErrors (CSourceData *pData);
    HRESULT FindLeafNodeEx (CSourceData *pData, const POSDATA pos, ExtentFlags flags, BASENODE **ppNode, ICSInteriorTree **ppTree);
    long    GetFirstToken (BASENODE *pNode, BOOL *pfMissingName);
    long    GetLastToken (BASENODE *pNode, BOOL *pfMissingName);
    STATEMENTNODE   *GetLastStatement (STATEMENTNODE *pStmt);

    HRESULT IsUpToDate (BOOL *pfTokenized, BOOL *pfTopParsed);
    HRESULT GetInteriorNode (CSourceData *pData, BASENODE *pNode, CInteriorNode **ppInteriorNode);
    void    ResetHeapBusyFlag () { m_fInteriorHeapBusy = FALSE; }
    void    ResetTokenizedFlag () { m_fTokenized = FALSE; }
    void    ResetTokenizingFlag () { m_fTokenizing = FALSE; }
    BOOL    ReportError (int iErrorId, ERRORKIND errKind, int iLine, int iCol, int iExtent, PCWSTR pszText);
    BOOL    IsSymbolDefined(PNAME symbol) { return m_tableDefines.IsDefined(symbol); }
    HRESULT GetLastRenamedTokenIndex(long *piTokIdx, NAME **ppPreviousName);
    HRESULT ResetRenamedTokenData();

    bool    hasChecksums() { return m_rgChecksum.Count() > 0; }
    int     CountChecksums() { return m_rgChecksum.Count(); }
    ChecksumData * GetChecksum(int index) { return m_rgChecksum.GetAt(index); }

    void    CloneSourceText (ICSSourceText **ppClone);
    void    CloneLexData(MEMHEAP *pStdHeap, PCWSTR pszText, LEXDATABLOCK &lex);

    // Parse diffs (CodeModel eventing)
    HRESULT GenerateParseDiffs(BASENODE *pOldTree, BASENODE*pNewTree, EVENTNODE **ppEventNode);
    void    FreeParseDiffQ();

    // IUnknown
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();
    STDMETHOD(QueryInterface)(REFIID riid, void **ppObj);

    // ICSSourceModule
    STDMETHOD(GetSourceData)(BOOL fBlockForNewest, ICSSourceData **ppData);
    STDMETHOD(GetSourceText)(ICSSourceText **ppSrcText);
    STDMETHOD(Flush)();
    STDMETHOD(DisconnectTextEvents)();
    STDMETHOD(GetParseDiffQ)(EVENTNODE **);
    STDMETHOD(GetMemoryConsumption)(long *piTopTree, long *piInteriorTrees, long *piInteriorNodes);
    STDMETHOD(AdviseChangeEvents)(ICSSourceModuleEvents *pSink, DWORD_PTR *pdwCookie);
    STDMETHOD(UnadviseChangeEvents)(DWORD_PTR dwCookie);

    // ICSSourceTextEvents
    STDMETHOD_(void, OnEdit)(POSDATA posStart, POSDATA posOldEnd, POSDATA posNewEnd);
};


#endif //__srcmod_h__

