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
// File: srcmod.cpp
//
// ===========================================================================

#include "stdafx.h"

#define DEFINE_TIMER_SWITCH "C# Compiler"
#define DBGOUT_MODULE DEFINE_TIMER_SWITCH

#include "timer.h"

#ifdef DEBUG
#include "dumpnode.h"
#define TOK(name, id, flags, type, stmtfunc, binop, unop, selfop, color) L ## #id,
const PCWSTR    rgszTokNames[] = {
#include "tokens.h"
NULL};
TOKENID Tok(int i) { return (TOKENID)i; }
PCWSTR TokenName(int i) { return rgszTokNames[i]; }
#endif

#if _MSC_VER
#pragma warning(error:4995)
#endif

VSDEFINE_SWITCH(gfNoIncrementalTokenization, "C# CodeSense", "Disable incremental tokenization");
VSDEFINE_SWITCH(gfRetokDbgOut, "C# CodeSense", "Incremental tokenization debug output spew");
VSDEFINE_SWITCH(gfBeepOnTopLevelParse, "C# CodeSense", "Beep on incremental parse failure (top level reparse)");
VSDEFINE_SWITCH(gfParanoidICCheck, "C# CodeSense", "Enable paranoid incremental tokenizer validation (SLOW)");
VSDEFINE_SWITCH(gfUnhandledExceptions, DBGOUT_MODULE, "Disable exception handling in lexer/parser");
VSDEFINE_SWITCH(gfTrackDataRefs, "C# CodeSense", "Track ICSSourceData references");

#ifdef DEBUG
#define RETOKSPEW(x) if (VSFSWITCH(gfRetokDbgOut)) { x; }
#define TRACKREF(x) if (VSFSWITCH(gfTrackDataRefs)) { x; }
#else
#define RETOKSPEW(x)
#define TRACKREF(x) 
#endif

////////////////////////////////////////////////////////////////////////////////
// IsKeywordDirective

const bool IsKeywordDirective [] = {
    #define PPKWD(n,id,d) d,
    #include "ppkwds.h"
};

const int THREAD_BLOCK_SIZE = 8;


////////////////////////////////////////////////////////////////////////////////
// Keep these 2 lists zero-terminated and up-to-date with any warnings reported
// from the lexer or parser.  They are used to determine whether or not 
// an incremental lex or parse error occured based on whether these warnings
// have been enabled or disabled via #pragma warning directives
// Also keep them sorted according to their error number
const WORD lexerWarnNumbers [] = {
    ErrorNumberFromID(WRN_WarningDirective),
    ErrorNumberFromID(WRN_IllegalPragma),
    ErrorNumberFromID(WRN_IllegalPPWarning),
    ErrorNumberFromID(WRN_BadRestoreNumber),
    ErrorNumberFromID(WRN_NonECMAFeature),
    ErrorNumberFromID(WRN_BadWarningNumber),
    ErrorNumberFromID(WRN_InvalidNumber),
    ErrorNumberFromID(WRN_FileNameTooLong),
    ErrorNumberFromID(WRN_IllegalPPChecksum),
    ErrorNumberFromID(WRN_EndOfPPLineExpected),
    ErrorNumberFromID(WRN_EmptyFileName),
    0
};

const WORD parserWarnNumbers [] = {
    ErrorNumberFromID(WRN_LowercaseEllSuffix),
    ErrorNumberFromID(WRN_PossibleMistakenNullStatement),
    ErrorNumberFromID(WRN_EmptySwitch),
    ErrorNumberFromID(WRN_NonECMAFeature),
    ErrorNumberFromID(WRN_GlobalAliasDefn),
    ErrorNumberFromID(WRN_AttributeLocationOnBadDeclaration),
    ErrorNumberFromID(WRN_InvalidAttributeLocation),
    0
};

////////////////////////////////////////////////////////////////////////////////
// CSourceLexer::CSourceLexer

CSourceLexer::CSourceLexer (CSourceModule *pMod) :
    CLexer (pMod->GetNameMgr(), pMod->GetOptions()->compatMode),
    m_pModule(pMod)
{
}

////////////////////////////////////////////////////////////////////////////////
// CSourceLexer::TrackLine

void CSourceLexer::TrackLine (PCWSTR pszLine)
{
    m_pModule->TrackLine (this, pszLine);
    CLexer::TrackLine(pszLine);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceLexer::ErrorAtPosition

void CSourceLexer::ErrorPosArgs(long iLine, long iCol, long iExtent, int id, int carg, ErrArg * prgarg)
{
    m_pModule->ErrorAtPositionArgs(iLine, iCol, iExtent, id, carg, prgarg);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceLexer::ScanPreprocessorLine

BOOL CSourceLexer::ScanPreprocessorLine (PCWSTR &p)
{
    if (m_fPreproc)
    {
        m_pModule->ScanPreprocessorLine (this, p, m_fFirstOnLine);
        return TRUE;
    }

    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceLexer::RepresentNoiseTokens

BOOL CSourceLexer::RepresentNoiseTokens ()
{
    return m_pModule->RepresentNoiseTokens ();
}

////////////////////////////////////////////////////////////////////////////////
// CSourceLexer::TokenMemAlloc

void *CSourceLexer::TokenMemAlloc (CSTOKEN *pToken, size_t iSize)
{
    return m_pModule->TokenMemAlloc (pToken, iSize);
}


////////////////////////////////////////////////////////////////////////////////
// CSourceParser::CSourceParser

CSourceParser::CSourceParser (CSourceModule *pMod, int mid) :
    CParser(pMod->m_pController->GetNameMgr()),
    m_pModule(pMod)
{
}

////////////////////////////////////////////////////////////////////////////////

void *CSourceParser::MemAlloc (long iSize)
{
    return m_pModule->m_pActiveHeap->Alloc (iSize);
}

////////////////////////////////////////////////////////////////////////////////

CErrorSuppression CSourceParser::GetErrorSuppression()
{
    return CErrorSuppression(m_pModule->GetController());
}

////////////////////////////////////////////////////////////////////////////////

BOOL CSourceParser::CreateNewError (int iErrorId, int carg, ErrArg * prgarg, const POSDATA &posStart, const POSDATA &posEnd)
{
#ifdef DEBUG
    if (IsWarningID(iErrorId)) {
        WORD num = ErrorNumberFromID(iErrorId);
        for (int i = 0; parserWarnNumbers[i] != 0; i++) {
            if (parserWarnNumbers[i] == num)
                goto FOUND;
        }
        VSASSERT(!"Parser warning not in allowed list", "If you get this assert, you need to update parserWarnNumbers in srcmod.cpp to include this warning number");
FOUND:;
    }
#endif

    return m_pModule->CreateNewError(iErrorId, carg, prgarg, posStart, posEnd);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceParser::AddToNodeTable

void CSourceParser::AddToNodeTable (BASENODE *pNode)
{
    m_pModule->AddToNodeTable (pNode);
}

////////////////////////////////////////////////////////////////////////////////

void CSourceParser::GetInteriorTree(CSourceData *pData, BASENODE *pNode, ICSInteriorTree **ppTree)
{
    m_pModule->GetInteriorParseTree (pData, pNode, ppTree);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceParser::ReportFeatureUse

void CSourceParser::ReportFeatureUse(DWORD dwFeatureId)
{
    if (m_pModule->GetOptions()->IsNoneMode())
        return;

    switch (dwFeatureId) {
    
    // These are all errors
    case IDS_FeatureGenerics:
    case IDS_FeatureAnonDelegates:
    case IDS_FeatureGlobalNamespace:
    case IDS_FeatureFixedBuffer:
    case IDS_FeatureStaticClasses:
    case IDS_FeaturePartialTypes:
    case IDS_FeaturePropertyAccessorMods:
    case IDS_FeatureExternAlias:
    case IDS_FeatureIterators:
    case IDS_FeatureDefault:
    case IDS_FeatureNullable:
        if (m_pModule->GetOptions()->IsECMA1Mode())
        {
            BSTR szFeatureName = NULL;
            PAL_TRY
            {
                // This feature isn't allowed
                szFeatureName = CError::ComputeString( GetMessageDll(), dwFeatureId, NULL);
                Error(ERR_NonECMAFeature, szFeatureName);
            }
            PAL_FINALLY
            {
                SysFreeString (szFeatureName);
            }
            PAL_ENDTRY
        }
        break;

    // These are just warnings
    case IDS_FeatureModuleAttrLoc:
        if (m_pModule->GetOptions()->IsECMA1Mode())
        {
            BSTR szFeatureName = NULL;
            PAL_TRY
            {
                // This feature isn't allowed
                szFeatureName = CError::ComputeString( GetMessageDll(), dwFeatureId, NULL);
                Error(WRN_NonECMAFeature, szFeatureName);
            }
            PAL_FINALLY
            {
                SysFreeString (szFeatureName);
            }
            PAL_ENDTRY
        }
        break;

    default:
        ASSERT(!"Unknown Feature Id");
    }


    return;
}

////////////////////////////////////////////////////////////////////////////////

BASENODE *CSourceParser::ParseSourceModule()
{
    return CParser::ParseSourceModule();
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::CSourceModule

CSourceModule::CSourceModule (CController *pController, int mid) :
    CSourceModuleBase(pController),
    m_iDataRef(0),
    m_iThreadCount(0),
    m_dwTextCookie(0),
    m_pCCStack(NULL),
    m_dwVersion(0),
    m_iTokDelta(0),
    m_iTokDeltaFirst(0),
    m_iTokDeltaLast(0),
    m_iErrors(0),
    m_heap1(pController, false, ProtectedEntityFlags::ParseTree),
    m_heap2(pController, false, ProtectedEntityFlags::ParseTree),
    m_pActiveHeap(NULL),
    m_pHeapForFirstTokenization(NULL),
    m_pszSrcDir(NULL),
    m_iCurTok(0),
    m_pTokenErrors(NULL),
    m_pParseErrors(NULL),
    m_pCurrentErrors(NULL),
    m_fTextModified(FALSE),
    m_fEventsArrived(FALSE),
    m_fEditsReceived(FALSE),
    m_fTextStateKnown(FALSE),
    m_fInteriorHeapBusy(FALSE),
    m_fTokenizing(FALSE),
    m_fTokenized(FALSE),
    m_fFirstTokenization(TRUE),
    m_fParsingTop(FALSE),
    m_fParsedTop(FALSE),
    m_fTokensChanged(FALSE),
    m_fForceTopParse(FALSE),
    m_fParsingInterior(FALSE),
    m_fLimitExceeded(FALSE),
    m_fErrorOnCurTok(FALSE),
    m_fCCSymbolChange(FALSE),
    m_fParsingForErrors(FALSE),
    m_fCrashed(FALSE),
    m_fLeafCrashed(FALSE),
    m_fTrackLines(FALSE),
    m_iLastTokenRenamed (-1),
    m_pPreviousTokenName (NULL)
{
    m_pThreadDataRef = (THREADDATAREF *)VSAllocZero (THREAD_BLOCK_SIZE * sizeof (THREADDATAREF));
    if (!m_pThreadDataRef)
        pController->NoMemory();

    m_pParser = new CSourceParser (this, mid);
    if (!m_pParser)
        pController->NoMemory();

    // Initialize the top level heap pointer and pointer to markers
    m_pActiveTopLevelHeap= &m_heap1;
    m_pMarkInterior = &m_markInterior1;
    m_pMarkTokens = &m_markTokens1;

    // Initialize m_heap2 markers
    m_heap2.Mark(&m_markTokens2);
    m_heap2.Mark(&m_markInterior2);
    
    m_posEditOldEnd.iLine = 0;
    m_posEditOldEnd.iChar = 0;

    m_fAccumulateSize = FALSE;
    m_dwInteriorSize = 0;
    m_iInteriorTrees = 0;

    W_IsUnicodeSystem();
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::~CSourceModule

CSourceModule::~CSourceModule ()
{
    BOOL    fLocked = m_StateLock.LockedByMe ();

    InternalFlush ();

    // If we were connected to the text, it would have a ref on us...
    ASSERT (m_dwTextCookie == 0);

    if (m_pThreadDataRef)
        VSFree (m_pThreadDataRef);

    m_heap1.FreeHeap();
    m_heap2.FreeHeap();

    if (m_pTokenErrors != NULL)
        m_pTokenErrors->Release();

    if (m_pParseErrors != NULL)
        m_pParseErrors->Release();

    if (fLocked)
        m_StateLock.Release();

    if (m_pszSrcDir)
        m_pStdHeap->Free(m_pszSrcDir);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::CreateInstance        [static]
//
// A static construction function for a source module, whose ctor/dtor functions
// are private.

HRESULT CSourceModule::CreateInstance (CController *pController, ICSSourceText *pText, ICSSourceModule **ppModule, int mid)
{
    CSourceModule   *pObj = new CSourceModule (pController, mid);
    HRESULT         hr;

    if (pObj == NULL)
        return E_OUTOFMEMORY;

    if (SUCCEEDED (hr = pObj->Initialize (pText)))
    {
        *ppModule = pObj;
        (*ppModule)->AddRef();
    }
    else
    {
        delete pObj;
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::Initialize

HRESULT CSourceModule::Initialize (ICSSourceText *pText)
{
    CTinyGate   gate (&m_StateLock);
    HRESULT     hr;

    m_spSourceText = pText;

    if (CheckFlags (CCF_KEEPNODETABLES))
    {
        m_pNodeTable = new CNodeTable (GetNameMgr());
        m_pNodeArray = new CStructArray<KEYEDNODE> ();
        if (m_pNodeTable == NULL || m_pNodeArray == NULL)
            return E_OUTOFMEMORY;
    }

    if (CheckFlags (CCF_KEEPIDENTTABLES))
    {
        m_lex.pIdentTable = new CIdentTable (GetNameMgr());
        if (m_lex.pIdentTable == NULL)
            return E_OUTOFMEMORY;
    }

    if (SUCCEEDED (hr = CErrorContainer::CreateInstance (EC_TOKENIZATION, 0, &m_pTokenErrors)) &&
        SUCCEEDED (hr = CErrorContainer::CreateInstance (EC_TOPLEVELPARSE, 0, &m_pParseErrors)))
    {
        hr = GetTextAndConnect ();
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::AcceptEdits

void CSourceModule::AcceptEdits ()
{
    ASSERT (m_StateLock.LockedByMe ());

    m_fTokenized = FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ApplyDelta

inline void CSourceModule::ApplyDelta (POSDATA posX, POSDATA posCX, POSDATA posCY, POSDATA *pposY)
{
    pposY->iLine = posX.iLine + (posCY.iLine - posCX.iLine);
    if (pposY->iLine == posCY.iLine)
    {
        if (VSFSWITCH(gfParanoidICCheck))
        {
            ASSERT ((long)posX.iChar + (long)(posCY.iChar - posCX.iChar) < 1000);
            ASSERT ((long)posX.iChar + (long)(posCY.iChar - posCX.iChar) >= 0);
        }
        pposY->iChar = (long)posX.iChar + (long)(posCY.iChar - posCX.iChar);
    }
    else
        pposY->iChar = posX.iChar;

    if (VSFSWITCH (gfParanoidICCheck))
    {
        // Make sure we've arrived at a sane position
        ASSERT (pposY->iChar < 1000);
    }
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::OnEdit
//
// This event sink is called when our source text is modified.

STDMETHODIMP_(void) CSourceModule::OnEdit (POSDATA posStart, POSDATA posOldEnd, POSDATA posNewEnd)
{
    // We must merge the particulars of this edit with any prior edits, creating
    // a "conglomerate" edit.  This must be done in a serialized fashion
    // (because edits can occur on any thread) so we do it while holding the
    // state lock.
    CTinyGate   gate (&m_StateLock);

    if (m_fTextModified)
    {
        // Merge incoming edit data with old edit data here.
        // RULES:
        // 1) Position values are always associated with a buffer version.
        // 2) Comparison between position values is only allowed if their
        //    buffer version is the same.
        // 3) OldEnd and NewEnd (both stored and incoming) refer to the
        //    same position, but have different buffer versions.
        // 4) The incoming end position is associated with buffer versions
        //    n-1 (old) and n (new).
        // 5) The stored end position BEFORE THIS EDIT is associated with
        //    buffer versions 0 (old) and n-1 (new).
        // 6) The stored end position AFTER THIS EDIT should be associated
        //    with buffer versions 0 (old) and n (new).
        // 7) To transform a position P from buffer version of x to y, apply
        //    the delta between any position C(x) and C(y), ASSUMING that
        //    both positions P and C are affected by all edits between
        //    buffer versions x and y.
        // 8) The start position is relative to all buffer versions, because
        //    it precedes all edits (by definition)

        // First, the start position.  This one is easy, because it is not
        // complicated by buffer versioning -- it is always the "earliest"
        // of all incoming values.


        if (posStart < m_posEditStart)
            m_posEditStart = posStart;

        // Okay, now the end position.  We must make a choice between the
        // stored end position and the incoming end position.  Per rule #2,
        // we must use the stored NEW end and the incoming OLD end, both of
        // which are relative to buffer version n-1.
        if (m_posEditNewEnd > posOldEnd)
        {
            // We have chosen to keep the stored end because it occurs past
            // the incoming edit.  So, we need m_posEditOldEnd(0) and
            // m_posEditNewEnd(n).  Since m_posEditOldEnd is already relative
            // to buffer 0, it is unmodified.  Since m_posEditNewEnd is
            // relative to buffer n-1 (and we need n), apply to it the delta
            // between the incoming end position values, which are n-1 and n.
            ApplyDelta (m_posEditNewEnd, posOldEnd, posNewEnd, &m_posEditNewEnd);
        }
        else
        {
            // We have chosen to use the incoming end because it occurs past
            // the stored edit.  So, we need posOldEnd(0) and posNewEnd(n).
            // Since posNewEnd is already relative to buffer n, it is copied
            // unmodified.  Since posOldEnd is relative to buffer n-1 (and
            // we need 0), apply to it the delta between the stored end
            // position values, which are relative to 0 and n-1.
            ApplyDelta (posOldEnd, m_posEditNewEnd, m_posEditOldEnd, &m_posEditOldEnd);
            m_posEditNewEnd = posNewEnd;
        }

        if (VSFSWITCH (gfParanoidICCheck))
        {
            DBGOUT (("Edit accumulation: was:[(%d,%d) to (%d,%d)-(%d,%d)] in:[(%d,%d) to (%d,%d)-(%d,%d)] accum:[(%d,%d) to (%d,%d)-(%d,%d)]",
                    posOS.iLine, posOS.iChar, posOOE.iLine, posOOE.iChar, posONE.iLine, posONE.iChar,
                    posStart.iLine, posStart.iChar, posOldEnd.iLine, posOldEnd.iChar, posNewEnd.iLine, posNewEnd.iChar,
                    m_posEditStart.iLine, m_posEditStart.iChar, m_posEditOldEnd.iLine, m_posEditOldEnd.iChar, m_posEditNewEnd.iLine, m_posEditNewEnd.iChar));
        }
    }
    else
    {
        // First edit -- simply store edit data.  HOWEVER, check to see if this is
        // the first EVER edit.  If so, and the change data is the same was what
        // we already have, this is a delayed change event for the initial load, which
        // we've already seen.  In this case, make the change look like the current
        // contents were REPLACED, not inserted.
        if (!m_fEditsReceived)
        {
            if (posStart.IsZero() && posOldEnd.IsZero() && posNewEnd == m_posEditNewEnd)
                posOldEnd = posNewEnd;
        }

        m_posEditStart = posStart;
        m_posEditOldEnd = posOldEnd;
        m_posEditNewEnd = posNewEnd;
        if (VSFSWITCH (gfParanoidICCheck))
        {
            DBGOUT (("Fresh edit start: (%d,%d) to (%d,%d)-(%d,%d)", posStart.iLine, posStart.iChar, posOldEnd.iLine, posOldEnd.iChar, posNewEnd.iLine, posNewEnd.iChar));
        }
        m_fTextModified = TRUE;
    }

    m_fEventsArrived = TRUE;        // NOTE:  This one gets reset on each text acquisition
    m_fEditsReceived = TRUE;        // ...this one is only to detect the "fresh start" case

    // Check the data reference count -- if zero, we can "accept" this edit (allowing
    // the next request for data to retokenize/reparse, etc.), and broadcast the
    // "this module has changed" event.
    if (m_iDataRef == 0)
    {
        AcceptEdits ();
        gate.Release();
        m_EventSource.FireOnModuleModified();
    }
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::FindThreadIndex

long CSourceModule::FindThreadIndex (BOOL fMustExist)
{
    DWORD   dwTid = GetCurrentThreadId();

    // You MUST have already locked this object, since the return value is only
    // valid while the module is locked
    ASSERT (m_StateLock.LockedByMe());

    // Find this thread in the array -- add it if it isn't there
    long i;
    for (i=0; i<m_iThreadCount; i++)
        if (m_pThreadDataRef[i].dwTid == dwTid)
            return i;

    ASSERT (!fMustExist);

    // Not there.  Grow if necessary and add.
    if (m_iThreadCount % THREAD_BLOCK_SIZE == 0)
    {
        THREADDATAREF * ptdr = (THREADDATAREF *)VSRealloc(m_pThreadDataRef, SizeMul(m_iThreadCount + THREAD_BLOCK_SIZE, sizeof (THREADDATAREF)));
        if (!ptdr)
            m_pController->NoMemory();
        m_pThreadDataRef = ptdr;
        ZeroMemory(&m_pThreadDataRef[m_iThreadCount], THREAD_BLOCK_SIZE * sizeof(THREADDATAREF));
    }

    m_iThreadCount++;
    ASSERT (m_iThreadCount == i + 1);
    ASSERT (m_pThreadDataRef[i].iRef == 0);
    m_pThreadDataRef[i].dwTid = dwTid;
    return i;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ReleaseThreadIndex

void CSourceModule::ReleaseThreadIndex (long iIndex)
{
    ASSERT (m_StateLock.LockedByMe());
    ASSERT (m_pThreadDataRef[iIndex].dwTid == GetCurrentThreadId());
    ASSERT (m_pThreadDataRef[iIndex].iRef == 0);

    // Move the last thread in the array to this slot (if this isn't the last one)
    if (iIndex < m_iThreadCount - 1)
    {
        m_pThreadDataRef[iIndex] = m_pThreadDataRef[m_iThreadCount - 1];
        m_pThreadDataRef[m_iThreadCount - 1].iRef = 0;
    }

    m_iThreadCount--;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::AddDataRef

void CSourceModule::AddDataRef ()
{
    CTinyGate gate (&m_StateLock);

    // Bump the global count...
    m_iDataRef++;

    // ...as well as the per-thread count
    long    iThreadIdx = FindThreadIndex (FALSE);
    m_pThreadDataRef[iThreadIdx].iRef++;
    TRACKREF(DBGOUT(("ADD DATA REF    :  T-%4X TID:%04X TIDX:%1d MOD:%08X TRef:%d MRef:%d", GetTickCount() & 0xffff, GetCurrentThreadId(), iThreadIdx, this, m_pThreadDataRef[iThreadIdx].iRef, m_iDataRef)));
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ReleaseDataRef

void CSourceModule::ReleaseDataRef ()
{
    CTinyGate   gate (&m_StateLock);

    // Decrement the per-thread count...
    long    iThreadIdx = FindThreadIndex (TRUE);
    ASSERT (m_pThreadDataRef[iThreadIdx].iRef > 0);
    TRACKREF(DBGOUT(("RELEASE DATA REF:  T-%4X TID:%04X TIDX:%1d MOD:%08X TRef:%d MRef:%d", GetTickCount() & 0xffff, GetCurrentThreadId(), iThreadIdx, this, m_pThreadDataRef[iThreadIdx].iRef-1, m_iDataRef-1)));
    if (--(m_pThreadDataRef[iThreadIdx].iRef) == 0)
        ReleaseThreadIndex (iThreadIdx);

    ASSERT (m_iDataRef > 0);
    if (--m_iDataRef == 0)
    {
        if (m_fTextModified)
        {
            AcceptEdits ();
            gate.Release();
            m_EventSource.FireOnModuleModified();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::PrepareForCloning

void CSourceModule::PrepareForCloning(ICSSourceData *pData)
{
    InternalCreateTokenStream(pData);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::GetTextAndConnect

HRESULT CSourceModule::GetTextAndConnect ()
{
    ASSERT (m_StateLock.LockedByMe());

    HRESULT     hr;


    POSDATA     posEnd;

    // Now, grab the text.  Note that we must unlock our state bits while we
    // do this, because change events from the text also lock the state bits and
    // we could otherwise deadlock.  As such, we need to check to see if any change
    // events came in between our unlock and lock, and if so, grab the text again.
    do
    {
        m_fEventsArrived = FALSE;
        m_StateLock.Release();
        hr = m_spSourceText->GetText (&m_lex.pszSource, &posEnd);
        m_StateLock.Acquire();

#ifdef DEBUG
        if (m_fEventsArrived)
            RETOKSPEW (DBGOUT (("*** TEXT CHANGED DURING ICSSourceText::GetText()!  Re-obtaining text pointer...")));
#endif
    } while (m_fEventsArrived);
    m_lex.iTextLen = (long)wcslen (m_lex.pszSource);

    if (!m_fEditsReceived)
    {
        m_posEditNewEnd = posEnd;
        m_posEditOldEnd = posEnd;
        m_posEditStart.SetUninitialized();
    }

    // It's no longer modified
    m_fTextModified = FALSE;
    //m_fEditsReceived = FALSE;
    RETOKSPEW (DBGOUT (("TEXT REVERTED TO UNMODIFIED STATE")));
    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::WaitForTokenization
//
// This function checks to see if another thread is tokenizing the text, and if
// so, waits for it to complete and returns TRUE.  If nobody is tokenizing, this
// returns FALSE.

BOOL CSourceModule::WaitForTokenization ()
{
    ASSERT (m_StateLock.LockedByMe ());     // Must have state locked to call this!

    if (m_fTokenizing)
    {
        ASSERT (m_dwTokenizerThread != GetCurrentThreadId());
        while (!m_fTokenized)
        {
            m_StateLock.Release();
            Snooze ();
            m_StateLock.Acquire();
        }

        return TRUE;
    }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////
// CSourceModule::DetermineStartData
//
// For an incremental tokenization, this function determines the position and
// token index of where to start tokenizing, and the potential stopping token
// index and position (both relative to old and new text).

void CSourceModule::DetermineStartData (LEXDATABLOCK &old, MERGEDATA &md, BOOL *pfIncremental)
{
    if (m_posEditStart.IsUninitialized())
    {
        (*pfIncremental) = FALSE;
        return;
    }

    if (((int)m_posEditStart.iLine >= m_lex.iLines) ||
        ((int)m_posEditOldEnd.iLine >= m_lex.iLines))
    {
        (*pfIncremental) = FALSE;
        return;
    }

    // Get a pointer (in the new source buffer) to the first line involved in the
    // edit.  Because the beginning of the line can at most be the first character
    // involved in the edit, the OLD line offset table will work just fine.  Note
    // the the NEW TEXT MUST BE IN m_lex BY NOW!!!
    PCWSTR  pszEditLineNew = m_lex.pszSource + old.piLines[m_posEditStart.iLine];

    for (long iScan = -1; iScan >= -5 && (long)(m_posEditStart.iChar + iScan) >= 0; iScan--)
    {
        if (pszEditLineNew[m_posEditStart.iChar + iScan] == '\\' && pszEditLineNew[m_posEditStart.iChar + iScan + 1] == 'u')
        {
            RETOKSPEW (DBGOUT (("Backed up %d chars for possible unicode escape consideration", -iScan)));
            m_posEditStart.iChar += iScan;
            break;
        }
    }

    // Assume we will NOT start at 0, 0
    md.fStartAtZero = FALSE;

    // Find the first token to scan -- which is the last token w/ start position < the edit.
    // FindNearestToken may return a token index whose position == that given.  We can't
    // start there -- must start one before it.
    md.iTokStart = old.FindNearestPosition(m_posEditStart);
    if (md.iTokStart >= 0 && old.pTokens[md.iTokStart] == m_posEditStart)
        md.iTokStart--;

    // Check for special case -- TID_NUMBER followed by TID_DOT must be considered as a whole,
    // since it may now absorb into a single number.
    if (md.iTokStart > 0 && old.pTokens[md.iTokStart].Token() == TID_DOT && old.pTokens[md.iTokStart - 1].Token() == TID_NUMBER)
        md.iTokStart--;

    // We need to know the first non-noisy token in the old stream
    int iFirstNonNoisyToken = old.NextNonNoisyToken(0);

    if (md.iTokStart == -1 || md.iTokStart < iFirstNonNoisyToken)
    {
        RETOKSPEW (DBGOUT (("Change occurred prior to first non-noisy token; must rescan from beginning of file")));
        md.iTokStart = 0;
        md.fStartAtZero = true;
        md.posStart = POSDATA(0,0);
    }
    else
    {
        RETOKSPEW (DBGOUT (("Retokenization starts at token %d (%S), position (%d, %d)", md.iTokStart, rgszTokNames[old.pTokens[md.iTokStart].Token()], old.pTokens[md.iTokStart].iLine, old.pTokens[md.iTokStart].iChar)));

        // Start position is the position of the start token.
        md.posStart = old.pTokens[md.iTokStart];

        // It is NEVER the case that the edit and the retokenization start at the same place, unless
        // the edit occured at the beginning of the file.
        ASSERT (md.posStart < m_posEditStart);
    }

    md.tokFirst = (TOKENID)old.pTokens[md.iTokStart].Token();

    // Next find the (potentially) last token to scan -- which is the first token w/ start
    // position > the edit.
    md.iTokStop = old.FindNearestPosition(m_posEditOldEnd) + 1;
    if (md.iTokStop == old.iTokens)
    {
        RETOKSPEW (DBGOUT (("Change affects last token; must scan to end of file")));
        ASSERT (old.pTokens[old.iTokens - 1].Token() == TID_ENDFILE);
        md.posStopOld = old.pTokens[old.iTokens - 1];
    }
    else
    {
        // If this change includes the first non-noisy token in the file
        // We need to keep going until the next non-noisy token so that we'll
        // properly catch #defines
        if (iFirstNonNoisyToken >= md.iTokStart && iFirstNonNoisyToken < md.iTokStop)
        {
            md.iTokStop = old.NextNonNoisyToken(md.iTokStop);
        }

        ASSERT (md.iTokStop < old.iTokens);
        ASSERT (old.pTokens[md.iTokStop].StopPosition() >= m_posEditOldEnd);
        RETOKSPEW (DBGOUT (("Retokenization ends at token %d (%S), OLD position (%d, %d)", md.iTokStop, rgszTokNames[old.pTokens[md.iTokStop].Token()], old.pTokens[md.iTokStop].iLine, old.pTokens[md.iTokStop].iChar)));

        // pposStopOld is the stop position relative to the OLD buffer.  The
        // stop position is the position of the stop token (which, in a successful
        // incremental tokenization, will be the same token at the same location
        // relative to the NEW buffer).
        md.posStopOld = old.pTokens[md.iTokStop];
    }

    // Now that we have the stop position relative to the OLD buffer, we can use ApplyDelta
    // to calculate the position relative to the NEW buffer.
    ApplyDelta (md.posStopOld, m_posEditOldEnd, m_posEditNewEnd, &md.posStop);
}



////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ReconstructCCState

void CSourceModule::ReconstructCCState (LEXDATABLOCK &old, MERGEDATA &md, CCREC **ppCopy, NAMETAB *pOldTable)
{
    CCREC   *pCopyStack = NULL;

    ASSERT (m_pCCStack == NULL);

    // Starting with an empty stack, push/pop records based on the contents of the
    // OLD CC change array, up to md.posStart.iLine.  Note that this also copies
    // the CC change array to the NEW buffer.
    //
    // Also, while we create the starting stack, we also create a COPY of the
    // stack, which we "follow through" to md.posStop for comparison/validation later.
    ASSERT (m_lex.iCCStates == old.iCCStates);
    if (old.iCCStates > 0)
    {
        m_lex.pCCStates = (CCSTATE *)VSAlloc ((m_lex.iCCStates + 8) * sizeof (CCSTATE *));
        if (!m_lex.pCCStates)
            m_pController->NoMemory();

        for (m_lex.iCCStates=0; m_lex.iCCStates < old.iCCStates && md.posStart.iLine > old.pCCStates[m_lex.iCCStates].iLine; m_lex.iCCStates++)
        {
            m_lex.pCCStates[m_lex.iCCStates] = old.pCCStates[m_lex.iCCStates];
            if (old.pCCStates[m_lex.iCCStates].dwFlags & CCF_ENTER)
            {
                // Push a record onto both the "real" stack and our copy
                PushCCRecord (m_pCCStack, old.pCCStates[m_lex.iCCStates].dwFlags & ~CCF_ENTER);
                PushCCRecord (pCopyStack, old.pCCStates[m_lex.iCCStates].dwFlags & ~CCF_ENTER);
            }
            else
            {
                ASSERT (m_pCCStack != NULL);
                ASSERT (pCopyStack != NULL);

                // Pop a record from both stacks
                PopCCRecord (m_pCCStack);
                PopCCRecord (pCopyStack);
            }
        }

        md.iFirstAffectedCCState = m_lex.iCCStates;

        // Continue forming our copy of the CC stack
        for (long i=m_lex.iCCStates; i < old.iCCStates && md.posStopOld.iLine > old.pCCStates[i].iLine; i++)
        {
            if (old.pCCStates[i].dwFlags & CCF_ENTER)
                PushCCRecord (pCopyStack, old.pCCStates[i].dwFlags & ~CCF_ENTER);
            else
            {
                ASSERT (pCopyStack != NULL);
                PopCCRecord (pCopyStack);
            }
        }
    }
    else
    {
        // Don't hold on to the old copy so we don't delete it later...
        old.pCCStates = NULL;
        md.iFirstAffectedCCState = 0;
    }

    *ppCopy = pCopyStack;

    // Also, we must start with the correct CC symbol table.  If we are starting at a token > 0,
    // then we just use the old symbol table -- no further changes to it are possible, since a token
    // precedes the edit.  Otherwise, we must create a copy of the old table for comparison after
    // tokenization, and start with either the original (cmd-line provided) symbols if starting < 0,
    // or the old table (if starting at token 0).  If changes are made to the table during retok,
    // we must scan the remainder of the file.
    m_fCCSymbolChange = FALSE;
    if (md.iTokStart == 0)
    {
        // Keep a copy of the old table
        for (long iSym = 0; iSym < m_tableDefines.GetCount(); iSym++)
            pOldTable->Define (m_tableDefines.GetAt (iSym));

        if (md.fStartAtZero)
        {
            // Reset to "factory defaults"
            m_tableDefines.ClearAll();

            CComBSTR    sbstr(GetOptions()->m_sbstrCCSYMBOLS);

            if (sbstr)
            {
                WCHAR *pchContext = NULL;
                PWSTR   pszTok = wcstok_s (sbstr, L" ,;", &pchContext);

                while (pszTok != NULL)
                {
                    if (GetNameMgr()->IsValidIdentifier(pszTok, (int)wcslen (pszTok), GetOptions ()->compatMode, CheckIdentifierFlags::Simple))
                        m_tableDefines.Define (GetNameMgr()->AddString (pszTok));

                    pszTok = wcstok_s (NULL, L" ,;", &pchContext);
                }
            }

            // Assume that a change has happened -- we need to check the tables for equality after
            // tokenization
            m_fCCSymbolChange = TRUE;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::MergeTokenStream

void CSourceModule::MergeTokenStream (LEXDATABLOCK &old, MERGEDATA &md)
{
    long    iTokDelta = md.fIncrementalStop ? (m_lex.iTokens - (md.iTokStop - md.iTokStart)) : (m_lex.iTokens - (old.iTokens - md.iTokStart));

    // First, update the tokens (and positions).
    if (iTokDelta == 0)
    {
        // Check for token equality -- it is very possible that no token changes were made.
        // Note that some token values may stay the same but still require parsing work
        // (such as identifiers and literals), so watch for them.  We only do this if the
        // number of tokens we parsed was small (3 or less)
        if (md.fIncrementalStop && !md.fParseWarningsChanged && m_lex.iTokens <= 3)
        {
            BOOL    fRude = FALSE;

            for (long i=0; i<m_lex.iTokens; i++)
            {
                if (old.pTokens[md.iTokStart + i].Token() != m_lex.pTokens[i].Token() ||
                    m_lex.pTokens[i].Token() == TID_GREATER ||
                    CParser::GetTokenInfo (m_lex.pTokens[i].Token())->iLen == 0)
                {
                    fRude = TRUE;
                }
            }

            if (!fRude)
            {
                // Nice!  When we're done with this, we don't have to reparse ANYTHING!  (Happens
                // a lot when typing comments or whitespace)
                md.fStreamChanged = FALSE;
                RETOKSPEW (DBGOUT (("INCREMENTAL TOKENIZATION REALLY ROCKS:  No tokens actually changed!!!")));
            }
        }
    }
    else
    {
        // A token delta means we have to re-size the token stream
        const long iNewAllocTokens = old.iTokens + iTokDelta;
        RETAILVERIFY(iNewAllocTokens >= 1);
        if (old.iAllocTokens < iNewAllocTokens)
        {
            CSTOKEN *pNewTokens = (CSTOKEN *)m_pStdHeap->Realloc (old.pTokens, iNewAllocTokens * sizeof (CSTOKEN));
            if (pNewTokens == NULL)
            {
                m_pController->NoMemory();
                RETAILVERIFY(false);    // we shouldn't get here...
                return;
            }
            old.iAllocTokens = iNewAllocTokens;
            old.pTokens = pNewTokens;
        }
    }

    // Since we know the token delta, calculate the last-changed token index here.  Note that
    // m_iTokDelta is an accumulation -- multiple retokenizations can occur between parses.
    m_iTokDelta += iTokDelta;

    long        iLastChanged = md.iTokStop + iTokDelta - 1;     // -1 because md.iTokStop token doesn't change if md.fIncrementalStop

    if (iLastChanged < md.iTokStart)
        iLastChanged = md.iTokStart;

    m_iTokDeltaLast = max (iLastChanged, m_iTokDeltaLast + iTokDelta);

    // Any tokens from iTokStart to iTokStop - 1 (or the end if !fIncrementalStop) need to be checked for overhead freeing
    long        iLast = (md.fIncrementalStop ? md.iTokStop : old.iTokens);
    for (long i=md.iTokStart; i<iLast; i++)
    {
        if ((old.pTokens[i].iUserBits & TF_HEAPALLOCATED) && old.pTokens[i].HasOverhead())
            m_pStdHeap->Free (old.pTokens[i].Overhead());
    }
    
    // Move the old unaffected tokens, if any (and if necessary)
    if (md.fIncrementalStop && (iTokDelta != 0)) {
        memmove (old.pTokens + md.iTokStart + m_lex.iTokens, old.pTokens + md.iTokStop, (old.iTokens - md.iTokStop) * sizeof (CSTOKEN));
    }

    // Copy the new tokens/positions over
    memcpy (old.pTokens + md.iTokStart, m_lex.pTokens, m_lex.iTokens * sizeof (CSTOKEN));

    // Adjust positions of tokens following (if incremental stopped)
    if (md.fIncrementalStop)
    {
        // The token extra data we will be updating below may live on the original NRHEAP.  If that is the case, then
        // the original NRHEAP will most likely be marked as read only and so we need to make it writeable.
        // (They may live on the original NRHEAP if they were created during the first tokenization.)
        NRHeapWriteMaker makeWriteable(m_pHeapForFirstTokenization);

        // First, update all line AND column values of tokens on the same line as the end of the edit
        // (which is guaranteed to be <= md.posStop.iLine).
        long i;
        for (i = md.iTokStart + m_lex.iTokens; 
             (i < old.iTokens + iTokDelta) && (old.pTokens[i].iLine == md.posEditOldEnd.iLine);
             i++)
        {
            old.pTokens[i].UpdateStartAndEndPositions (md.iLineDelta, md.iCharDelta);
        }

        // Next, if there was a line delta, add it to all token position lines from here on out.
        if (md.iLineDelta != 0)
        {
            for (; i < old.iTokens + iTokDelta; i++)
            {
                // iCharDelta is "0" because we aren't on the same line anymore...
                old.pTokens[i].UpdateStartAndEndPositions (md.iLineDelta, 0);
            }
        }
    }

    old.iTokens += iTokDelta;

    // There!  Now put the old token stream back and whack the new one.
    m_pStdHeap->Free (m_lex.pTokens);
    //m_pStdHeap->Free (m_lex.pposTokens);
    m_lex.pTokens = old.pTokens;
    //m_lex.pposTokens = old.pposTokens;
    m_lex.iTokens = old.iTokens;
    m_lex.iAllocTokens = old.iAllocTokens;
}


////////////////////////////////////////////////////////////////////////////////
// CSourceModule::MergeCCState

void CSourceModule::MergeCCState (LEXDATABLOCK &old, MERGEDATA &md)
{
    long    iFirstAffected, iFirstUnaffected, iDelta;

    // Transitions.  These are small in number; binary searches not necessary.
    for (iFirstAffected = 0; iFirstAffected < old.iTransitionLines && old.piTransitionLines[iFirstAffected] < (long)md.posStart.iLine; iFirstAffected++)
        ;

    if (md.fIncrementalStop)
    {
        for (iFirstUnaffected = iFirstAffected; iFirstUnaffected < old.iTransitionLines && old.piTransitionLines[iFirstUnaffected] < (long)md.posStopOld.iLine; iFirstUnaffected++)
            ;
    }
    else
    {
        iFirstUnaffected = old.iTransitionLines;
    }

    iDelta = m_lex.iTransitionLines - (iFirstUnaffected - iFirstAffected);

    // Any transitions in the retokenized text means the transitions changed.
    if (iFirstUnaffected != iFirstAffected)
        md.fTransitionsChanged = TRUE;

    // Transition array grows in blocks of 8...
    if (iDelta > 0 && RoundUp8(old.iTransitionLines + iDelta) > RoundUp8(old.iTransitionLines))
    {
        long    iSize = RoundUp8(old.iTransitionLines + iDelta) * sizeof (long);
        old.piTransitionLines = (long *)((old.piTransitionLines == NULL) ? VSAlloc (iSize) : VSRealloc (old.piTransitionLines, iSize));
        if (!old.piTransitionLines)
            m_pController->NoMemory();
    }

    // We'll assume there's a line delta.  If the array is growing, move/update starting from the end.  
    // Otherwise move/update starting at the beginning.  Note that this refers to the unaffected 'third'.
    if (iDelta > 0)
    {
        for (long i=old.iTransitionLines - 1; i >= iFirstUnaffected; i--)
            old.piTransitionLines[i + iDelta] = old.piTransitionLines[i] + md.iLineDelta;
    }
    else if (iDelta < 0)
    {
        for (long i=iFirstUnaffected; i < old.iTransitionLines; i++)
            old.piTransitionLines[i + iDelta] = old.piTransitionLines[i] + md.iLineDelta;
    }
    else if (md.iLineDelta != 0)
    {
        // In this case the values aren't moving -- just update them
        for (long i=iFirstUnaffected; i < old.iTransitionLines; i++)  
            old.piTransitionLines[i] += md.iLineDelta;
    }

    // Copy the new ones into the array -- the space is there for them.
    memcpy (old.piTransitionLines + iFirstAffected, m_lex.piTransitionLines, m_lex.iTransitionLines * sizeof (long));
    old.iTransitionLines += iDelta;

    // Whack the new, replace with the updated old
    if (m_lex.piTransitionLines != NULL)
        VSFree (m_lex.piTransitionLines);
    m_lex.piTransitionLines = old.piTransitionLines;
    m_lex.iTransitionLines = old.iTransitionLines;

    // Update the conditional compilation state change array.  We will only have work to do
    // here if an incremental tokenization stopped short of the end of the file.
    if (md.fIncrementalStop)
    {
        // Find the first transition past the last scanned token
        while (md.iFirstAffectedCCState < old.iCCStates && old.pCCStates[md.iFirstAffectedCCState].iLine <= md.posStopOld.iLine)
            md.iFirstAffectedCCState++;

        if (md.iFirstAffectedCCState < old.iCCStates)
        {
            // Copy that one and all following to the new one (making sure it's big enough first), updating
            // the line using the line delta as we go.
            ASSERT (m_lex.pCCStates != NULL);
            CCSTATE * pccs = (CCSTATE *)VSRealloc (m_lex.pCCStates, SizeMul(m_lex.iCCStates + (old.iCCStates - md.iFirstAffectedCCState), sizeof (CCSTATE)));
            if (!pccs) {
                m_pController->NoMemory();
                VSFAIL("NoMemory shouldn't return!");
                return;
            }
            m_lex.pCCStates = pccs;
            for (; md.iFirstAffectedCCState < old.iCCStates; md.iFirstAffectedCCState++)
            {
                m_lex.pCCStates[m_lex.iCCStates] = old.pCCStates[md.iFirstAffectedCCState];
                m_lex.pCCStates[m_lex.iCCStates++].iLine += md.iLineDelta;
            }
        }
    }

    if (old.pCCStates != NULL)
        VSFree (old.pCCStates);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::MergeLineTable

void CSourceModule::MergeLineTable (LEXDATABLOCK &old, MERGEDATA &md)
{
    long    iFirstAffected, iFirstUnaffected;

    // The first affected line is the start line PLUS one.  The last one is the
    // stop line PLUS one -- and isn't really "unaffected", since we must account
    // for STREAM changes in the text.
    iFirstAffected = md.posStart.iLine + 1;
    iFirstUnaffected = md.fIncrementalStop ? md.posStopOld.iLine + 1 : old.iLines;

    // All lines up to the first affected one will be the same.  Note that we are
    // copying directly into the NEW table -- the span of lines that was just
    // tokenized will have already been updated.
    memcpy (m_lex.piLines, old.piLines, iFirstAffected * sizeof (long));

    // Now, update all lines from iFirstUnaffected to the end to account for
    // the stream delta.
    if (md.fIncrementalStop)
    {
        long    iStreamNewEnd = m_lex.piLines[md.posEditNewEnd.iLine] + md.posEditNewEnd.iChar;
        long    iStreamDelta = iStreamNewEnd - md.iStreamOldEnd;

        for (long i = iFirstUnaffected; i < old.iLines; i++)
            m_lex.piLines[i + md.iLineDelta] = old.piLines[i] + iStreamDelta;
    }

    // Whack the old copy
    m_pStdHeap->Free (old.piLines);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::UpdateRegionTable

void CSourceModule::UpdateRegionTable ()
{
    // We can have at most m_lex.iCCStates regions (and that's only if they are
    // all nested and unterminated).  Allocate twice as much as necessary, and
    // point the end array at the midpoint (these never grow -- one less allocation).
    long    iSize = 2 * m_lex.iCCStates * sizeof (LONG);
    m_lex.piRegionStart = (long *)(m_lex.piRegionStart == NULL ? m_pStdHeap->Alloc (iSize) : m_pStdHeap->Realloc (m_lex.piRegionStart, iSize));
    m_lex.piRegionEnd = m_lex.piRegionStart + m_lex.iCCStates;

    // We also need a little stack space
    long    *piStack = STACK_ALLOC (long, m_lex.iCCStates);
    long    iStack = 0;

    // Rip through the CC states and look for CCF_REGION enters and corresponding exits
    m_lex.iRegions = 0;
    for (long i=0; i<m_lex.iCCStates; i++)
    {
        DWORD   dwFlags = m_lex.pCCStates[i].dwFlags;

        if (dwFlags & CCF_REGION)
        {
            // Here's one we care about.
            if (dwFlags & CCF_ENTER)
            {
                // This is an enter.  Store the line number in the start array
                // at the current region slot, and push that region slot on the
                // stack for the next exit.
                m_lex.piRegionStart[m_lex.iRegions] = m_lex.pCCStates[i].iLine;
                StoreAtIndex(piStack, iStack++, m_lex.iCCStates, m_lex.iRegions++);
            }
            else
            {
                // This is an exit, so pop a slot off the stack and store the
                // line in the end array at that slot.
                ASSERT (iStack > 0);
                m_lex.piRegionEnd[(int)FetchAtIndex(piStack, --iStack, m_lex.iCCStates)] = m_lex.pCCStates[i].iLine;
            }
        }
    }

    // For anything left on the stack, use the line count as the 'end'
    while (iStack > 0)
        m_lex.piRegionEnd[(int)FetchAtIndex(piStack, --iStack, m_lex.iCCStates)] = m_lex.pTokens[m_lex.iTokens - 1].iLine;
}






////////////////////////////////////////////////////////////////////////////////
// CSourceModule::HandleExceptionFromTokenization

LONG CSourceModule::HandleExceptionFromTokenization (EXCEPTION_POINTERS * exceptionInfo, PVOID pv)
{
    //  1) The original call that caused tokenization to occur returns failure, and
    //  2) The state of the source module is such that another attempt to tokenize
    //     won't just crash again (which may not be possible)
    CSourceModule* pThis = (CSourceModule *)pv;

    pThis->ClearTokenizerThread();
    pThis->ResetTokenizedFlag();
    pThis->ResetTokenizingFlag();

    return EXCEPTION_CONTINUE_SEARCH;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::CreateTokenStream

void CSourceModule::CreateTokenStream (ICSSourceData *pData)
{
    if (VSFSWITCH (gfUnhandledExceptions))
    {
        InternalCreateTokenStream (pData);
    }
    else
    {
        PAL_TRY
        {
            InternalCreateTokenStream (pData);
        }
        PAL_EXCEPT_FILTER (HandleExceptionFromTokenization, this)
        {
            // NOTE:  We'll never get here...
        }
        PAL_ENDTRY
    }
}

//
//

void CSourceModule::InternalCreateTokenStream (ICSSourceData *pData)
{
    // Lock the state bits.  Once we determine that we actually need to do work,
    // we'll set appropriate state flags an unlock the state bits so other threads
    // can make the same determination.
    CTinyGate   gate (&m_StateLock);

    // If we're currently tokenized, we stop here.
    if (m_fTokenized)
        return;

    // We're not tokenized, but it's possible another thread is doing it already.
    // If this function returns TRUE, that is (was) the case -- the token stream
    // will have been updated by the other thread.
    if (WaitForTokenization ())
        return;

    // Okay, at this point we commit to tokenizing the current text.  Reflect that
    // in the state bits, and record our thread ID as the "tokenizer thread".  Also
    // start the timer for perf analysis.
    SetTokenizerThread ();
    m_fTokenizing = TRUE;

    m_pController->CloneCoreOptions(m_options);
    m_fFetchedOptions = true;

    // Validate some state flags -- we can't need a tokenization while someone
    // is currently doing any parsing work...
    ASSERT (!m_fParsingTop && !m_fParsingInterior);

    // Even in the event of a non-incremental tokenization, we need to make a copy
    // of the current lexer state.
    LEXDATABLOCK    old (m_lex);

    // If the text is modified, go grab the new version.  This also makes sure we
    // are connected to the events fired by the source text (if any).
    if (m_fTextModified)
        GetTextAndConnect ();

    BOOL    fIncremental;

    // There are many factors that contribute to whether or not we can be incremental.
    // The first one is whether the text state is known before we start.  The term
    // "text state" basically means whether or not we know precisely what has happened
    // to the text between now and the last time we tokenized (via m_posEdit* vars).
    // This implies that we actually have tokenization data from last time...
    fIncremental = m_fTextStateKnown;

    // After checking the "text state known" flag, we set it to TRUE here, instead of
    // when we are finished.  This prevents "missing" changes to the text that occur
    // (from another thread) while we're tokenizing
    m_fTextStateKnown = TRUE;

#ifdef DEBUG
    if (VSFSWITCH (gfNoIncrementalTokenization))
        fIncremental = FALSE;
#endif

    // Exceeding the line count or line length limits prevents incremental-ness
    if (m_fLimitExceeded)
    {
        fIncremental = FALSE;
        m_fLimitExceeded = FALSE;
    }

    // #pragma checksum also prohibit incremental-ness.
    if (fIncremental && (m_rgChecksum.Count() != 0))
        fIncremental = FALSE;

    // Okay, that's it for the absolutely-no-way-we-can-be-incremental checks.  Even
    // though we may end up lexing the entire source, if fIncremental is TRUE here,
    // the tokenization is considered incremental.  If we're NOT incremental, we
    // must flush any data from the last tokenization.
    if (!fIncremental)
    {
        RETOKSPEW (DBGOUT (("NON-INCREMENTAL TOKENIZATION!")));
        InternalFlush ();

        // Copy the flushed m_lex over to old to keep things sane...
        old = m_lex;

        // NOTE:  The identifier table isn't entirely accurate -- it may contain
        // identifiers that are no longer used.  We use this opportunity to empty
        // the identifier table.
        if (m_lex.pIdentTable != NULL)
            m_lex.pIdentTable->RemoveAll ();
    }
    else
    {
        // We must have data if this is incremental!
        ASSERT (m_lex.pTokens != NULL);
    }

    // We might need to keep the old line map active if we do incremental
    CLineMap oldLineMap(m_LineMap.GetAllocator());
    long     iLineMapCount = 0;

    MERGEDATA       md;
    if (fIncremental)
    {
        // Figure out where we can start and (try to) stop tokenizing.
        DetermineStartData (old, md, &fIncremental);
    }

    if (fIncremental)
    {
        // Figure out if we can do incremental with existing #line directives
        if (m_LineMap.ContainsEntry(md.posStart.iLine,  md.posStopOld.iLine))
        {
            m_LineMap.Clear();
            fIncremental = false;
        }
        else
        {
            // Copy line map into old one
            oldLineMap.Copy(&m_LineMap);

            // Delete the current one from the start of the incremental tokenization
            m_LineMap.RemoveFrom(md.posStart.iLine);

            // We keep the current count of remaing #line directives, so that
            // we can check later on if we found some in the new edited text.
            // If it's the case, we know we have to disable incremental tokenization
            // for the rest of the file
            iLineMapCount = m_LineMap.GetCount();
        }
    }

    // If we're still incremental, find out if we need a top reparse or not
    if (fIncremental)
    {
        // Find if we are changing an IDENTIFIER token
        long iTokenRenamed = -1;
           
        if (abs(md.iTokStop - md.iTokStart) <= 3)
        {
            for (int i = md.iTokStart; i < md.iTokStop; i++)
            {
                CSTOKEN tk = old.pTokens[i];
                if (tk.Token() == TID_IDENTIFIER && (old.pTokens[i] <= m_posEditStart && tk.StopPosition() >= m_posEditOldEnd))
                {
                    //DBGOUT( ("Only one token index=%d (Identifier: \"%S\") changed at pos:(line=%d, col=%d)",
                    //    tk.Token(), tk.Name()->text, tk.iLine, tk.iChar) );
                    iTokenRenamed = i;
                    break;
                }
            }
        }       

        // if no IDENTIFIER token changed, reset the state
        // otherwise update the name only if the token index hasn't changed
        if (iTokenRenamed == -1)
        {
            m_pPreviousTokenName = NULL;
            m_iLastTokenRenamed = -1;
        }
        else 
        {
            if (iTokenRenamed != m_iLastTokenRenamed)
            {
                m_pPreviousTokenName = old.pTokens[iTokenRenamed].Name();
                m_iLastTokenRenamed = iTokenRenamed;
            }
        }


        // Scan between iTokStop+1 and iTokStop-1 to see if any TID_OPEN/CLOSECURLY tokens exist.
        // If so, this retok will trigger a top-level reparse.  Note that we don't care about the
        // first changed token; it is compared later, and we don't care about the last token; it
        // must be the same or retokenization won't "stop incrementally".

        // The m_fForceTopParse is reset to FALSE when we actually do the top level reparse, so that
        // we don't miss multiple updates to the token stream (the last one not containing any
        // '{' or '}') before any parse is done.
        //m_fForceTopParse = FALSE; 
                                
        for (long i=md.iTokStart + 1; i<md.iTokStop; i++)
        {
            if (old.pTokens[i].Token() == TID_OPENCURLY || old.pTokens[i].Token() == TID_CLOSECURLY)
            {
                m_fForceTopParse = TRUE;
                break;
            }
        }
    }
    else
    {
        // A non-incremental tokenization will always start at ground zero...
        md.iTokStart = 0;
        md.fStartAtZero = TRUE;
        md.iTokStop = old.iTokens;
        md.posStart = POSDATA(0,0);
        m_iLastTokenRenamed = -1;
        m_pPreviousTokenName = NULL;

        // A non-incremental tokenization will also always force a top-level reparse.
        m_fForceTopParse = TRUE;
    }

    // This flag gets set to FALSE if an incremental tokenization doesn't stop at md.posStop correctly.
    md.fIncrementalStop = fIncremental && (md.iTokStop < old.iTokens);

    // This flag gets set when a #pragma warning forces a top-level parse (even if no other tokens change)
    md.fParseWarningsChanged = FALSE;

    // This is the line delta -- number of lines added/removed via edit
    md.iLineDelta = m_posEditNewEnd.iLine - m_posEditOldEnd.iLine;

    // This is the char delta -- which is NOT the number of characters added/removed,
    // but rather the change in char offset of the first unmodified character (which
    // may be on a different line now -- an insertion could render md.iCharDelta < 0).
    md.iCharDelta = m_posEditNewEnd.iChar - m_posEditOldEnd.iChar;

    // This is the stream-based character index of the first unmodified character,
    // relative to the OLD buffer.  We calculate the value relative to the NEW buffer
    // later (when we have a corresponding line table).
    md.iStreamOldEnd = fIncremental ? (old.piLines[m_posEditOldEnd.iLine] + m_posEditOldEnd.iChar) : 0;

    // For error replacement, we need the position of the end-of-file.
    md.posOldEOF = (old.pTokens == NULL) ? POSDATA(0,0) : old.pTokens[old.iTokens - 1];

    md.posEditOldEnd = m_posEditOldEnd;
    md.posEditNewEnd = m_posEditNewEnd;

    m_posEditOldEnd = m_posEditNewEnd;

    CErrorContainer *pErrorTemp;

    if (FAILED (m_pTokenErrors->Clone (&pErrorTemp)))
        m_pController->NoMemory();

    m_pTokenErrors->Release();
    m_pTokenErrors = pErrorTemp;

    ASSERT (m_pCurrentErrors == NULL);
    m_pCurrentErrors = m_pTokenErrors;

    m_pTokenErrors->BeginReplacement();

    //
    //
    gate.Release();



    CCREC       *pCopyStack = NULL;
    NAMETAB     OldTable (m_pController);

    ReconstructCCState (old, md, &pCopyStack, &OldTable);

    m_lex.iAllocLines = fIncremental ? old.iLines + md.iLineDelta : RoundUp32((m_lex.iTextLen >> 5) + 1);
    m_lex.piLines = (long *)m_pStdHeap->Alloc (m_lex.iAllocLines * sizeof (long));
    m_lex.iLines = (md.fStartAtZero) ? 0 : md.posStart.iLine;
    ASSERT (m_lex.iLines <= m_lex.iAllocLines);

    m_lex.iAllocTokens = m_lex.iAllocLines * 4;
    m_lex.pTokens = (CSTOKEN *)m_pStdHeap->Alloc (m_lex.iAllocTokens * sizeof (CSTOKEN));
    m_lex.iTokens = 0;
    
    if (old.pWarningMap != NULL)
    {
        m_lex.pWarningMap = new CWarningMap(m_pStdHeap);
        m_lex.pWarningMap->Copy( old.pWarningMap, -1, md.posStart.iLine);
        old.pWarningMap->Remove( -1, md.posStart.iLine);
    }
    else
    {
        ASSERT(m_lex.pWarningMap == NULL);
    }

    long    iTransitionCopy = old.iTransitionLines;
    long    *piTransitionCopy = (iTransitionCopy > 0) ? STACK_ALLOC (long, iTransitionCopy) : NULL;
    if (piTransitionCopy != NULL)
        memcpy (piTransitionCopy, old.piTransitionLines, iTransitionCopy * sizeof (long));

    m_lex.iTransitionLines = 0;
    m_lex.piTransitionLines = NULL;

    m_fTrackLines = TRUE;

    CSourceLexer    ctx (this);

    if (fIncremental)
    {
        ctx.m_iCurLine = md.posStart.iLine;
        ctx.m_pszCurLine = m_lex.pszSource + old.piLines[ctx.m_iCurLine];
        ctx.m_pszCurrent = ctx.m_pszCurLine + md.posStart.iChar;
    }
    else
    {
        ctx.m_pszCurLine = m_lex.pszSource;
        ctx.m_pszCurrent = m_lex.pszSource;
        ctx.m_iCurLine = 0;
    }

    m_lex.piLines[0] = 0;
    ctx.m_fPreproc = TRUE;
    ctx.m_fFirstOnLine = TRUE;

    if (md.iTokStart == 0 ||
        -1 == old.SkipNoisyTokens(md.iTokStart - 1, -1))
    {
        ctx.m_fTokensSeen = false;
    }
    else
    {
        ctx.m_fTokensSeen = true;
    }


    m_EventSource.FireOnBeginTokenization (ctx.m_iCurLine);


    //
    //

    while (TRUE)
    {
        if (m_lex.iTokens >= m_lex.iAllocTokens)
        {
            m_lex.iAllocTokens += RoundUp32(((m_lex.iTextLen - (long)(ctx.m_pszCurrent - m_lex.pszSource)) >> 5) + 1);
            m_lex.pTokens = (CSTOKEN *)m_pStdHeap->Realloc (m_lex.pTokens, (m_lex.iAllocTokens * sizeof (CSTOKEN)));
        }

        TOKENID     iTok = ctx.ScanToken (m_lex.pTokens + m_lex.iTokens);

        if (iTok == TID_IDENTIFIER && m_lex.pIdentTable != NULL)
        {
            NAME    *pName = m_lex.pTokens[m_lex.iTokens].Name();
            m_lex.pIdentTable->Add (pName, pName);
        }

        if (md.fIncrementalStop && m_lex.pTokens[m_lex.iTokens] >= md.posStop)
        {
            if (m_lex.pTokens[m_lex.iTokens].Token() == old.pTokens[md.iTokStop].Token() && m_lex.pTokens[m_lex.iTokens] == md.posStop)
            {
                if (m_fCCSymbolChange)
                {
                    if (OldTable.GetCount() == m_tableDefines.GetCount())
                    {
                        m_fCCSymbolChange = FALSE;
                        for (long i=0; i<OldTable.GetCount(); i++)
                            if (!m_tableDefines.IsDefined (OldTable.GetAt (i)))
                            {
                                m_fCCSymbolChange = TRUE;
                                break;
                            }
                    }

#ifdef DEBUG
                    if (m_fCCSymbolChange)
                        RETOKSPEW (DBGOUT (("CC SYMBOL TABLE CHANGED:  Tokenization must continue to end-of-file...")));
#endif
                }

                if (!m_fCCSymbolChange)
                {
                    if (!CompareCCStacks (m_pCCStack, pCopyStack))
                    {
                        RETOKSPEW (DBGOUT (("INCOMPATIBLE CC STATE CHANGE IN EDIT:  Tokenization must continue to end-of-file...")));
                        m_fCCSymbolChange = TRUE;
                    }
                }
                
                if (!m_fCCSymbolChange)
                {
                    if ((old.pWarningMap != NULL &&
                         old.pWarningMap->IsWarningChanged(lexerWarnNumbers, md.posStart.iLine, md.posEditOldEnd.iLine)) ||
                        (m_lex.pWarningMap != NULL &&
                         m_lex.pWarningMap->IsWarningChanged(lexerWarnNumbers, md.posStart.iLine, md.posStop.iLine)))
                    {
                        m_fCCSymbolChange = TRUE;
                        RETOKSPEW (DBGOUT (("#pragma warning change:  Tokenization must continue to end-of-file...")));
                    }
                    else if ((old.pWarningMap != NULL &&
                         old.pWarningMap->IsWarningChanged(parserWarnNumbers, md.posStart.iLine, md.posEditOldEnd.iLine)) ||
                        (m_lex.pWarningMap != NULL &&
                         m_lex.pWarningMap->IsWarningChanged(parserWarnNumbers, md.posStart.iLine, md.posStop.iLine)))
                    {
                        md.fParseWarningsChanged = TRUE;
                        m_fForceTopParse = TRUE;
                        RETOKSPEW (DBGOUT (("#pragma warning change:  Force a top-level re-parse")));
                    }

                }

                if (!m_fCCSymbolChange && (iLineMapCount == m_LineMap.GetCount()))
                {
                    if (iTok != TID_ENDFILE)
                    {
                        RETOKSPEW (DBGOUT (("INCREMENTAL TOKENIZATION ROCKS:  Scanned %d tokens instead of %d!", m_lex.iTokens + 1, old.iTokens + m_lex.iTokens - (md.iTokStop - md.iTokStart))));

                        m_lex.iLines = old.iLines + md.iLineDelta;
                        VSASSERT(m_lex.iLines >= 0, "Hmm... another bug with editing a binary file?!?!");

                        if ((m_lex.pTokens[m_lex.iTokens].iUserBits & (TF_OVERHEAD | TF_HEAPALLOCATED)) == (TF_OVERHEAD|TF_HEAPALLOCATED))
                            m_pStdHeap->Free (m_lex.pTokens[m_lex.iTokens].Overhead());
                        break;
                    }
                }
            }
            else
            {
                RETOKSPEW (DBGOUT (("STOP TOKEN/POSITION MISMATCH:  Tokenization must continue to end-of-file...")));
            }

            md.fIncrementalStop = FALSE;
            m_fForceTopParse = TRUE;
        }

        m_lex.iTokens++;

        if (iTok == TID_ENDFILE)
        {
            m_lex.iLines++;
            md.fIncrementalStop = FALSE;
            break;
        }

        if (ctx.m_fLimitExceeded)
        {
            md.fIncrementalStop = FALSE;
            break;
        }
    }

    m_fTrackLines = FALSE;


    //
    //

    md.fStreamChanged = TRUE;
    md.fTransitionsChanged = FALSE;
    md.iStartComment = -1;

    BOOL    fWhackOld = TRUE;

    if (old.pTokens != NULL)
    {
        if (md.fIncrementalStop || !md.fStartAtZero)
        {
            ASSERT (fIncremental);

            MergeTokenStream (old, md);

            MergeCCState (old, md);

            MergeLineTable (old, md);

            m_LineMap.Copy(&oldLineMap, ctx.m_iCurLine);
            m_LineMap.ApplyDelta(ctx.m_iCurLine, md.iLineDelta);
            fWhackOld = FALSE;
        }
        else
        {
        }
    }
    else
    {
        fWhackOld = FALSE;

        if (old.piTransitionLines != NULL)
            VSFree (old.piTransitionLines);
        if (old.pCCStates != NULL)
            VSFree (old.pCCStates);
    }

    if (fWhackOld)
    {
        for (long i=0; i<old.iTokens; i++)
        {
            if ((old.pTokens[i].iUserBits & (TF_OVERHEAD|TF_HEAPALLOCATED)) == (TF_OVERHEAD|TF_HEAPALLOCATED))
                m_pStdHeap->Free (old.pTokens[i].Overhead());
        }
        m_pStdHeap->Free (old.pTokens);
        m_pStdHeap->Free (old.piLines);
        if (old.pCCStates != NULL)
            VSFree (old.pCCStates);
        if (old.piTransitionLines != NULL)
            VSFree (old.piTransitionLines);
    }

    while (pCopyStack != NULL)
    {
        CCREC   *pTmp = pCopyStack->pPrev;
        VSFree (pCopyStack);
        pCopyStack = pTmp;
    }

    if (m_pCCStack != NULL)
    {
        if (!md.fIncrementalStop)
            ErrorAtPosition (m_lex.pTokens[m_lex.iTokens-1].iLine, m_lex.pTokens[m_lex.iTokens-1].iChar, 1, (m_pCCStack->dwFlags & CCF_REGION) ? ERR_EndRegionDirectiveExpected : ERR_EndifDirectiveExpected);

        while (PopCCRecord (m_pCCStack))
            ;
    }

    if (ctx.m_fLimitExceeded)
    {
        DBGOUT (("COMPILER LIMIT EXCEEDED!!!  Line too long, or too many lines..."));
        m_fLimitExceeded = TRUE;
        m_lex.iTokens = 1;
        m_lex.pTokens[0].iUserByte = (unsigned)TID_ENDFILE;
        m_lex.iTransitionLines = 0;
        m_lex.iRegions = 0;
        m_lex.iCCStates = 0;
    }

    UpdateRegionTable ();

    if (md.fIncrementalStop && old.pWarningMap)
    {
        m_lex.pWarningMap->Copy(old.pWarningMap, md.posStopOld.iLine, old.iLines+1, md.iLineDelta);
        old.pWarningMap->Remove(md.posStopOld.iLine);
    }

    if (old.pWarningMap)
    {
        old.pWarningMap->Clear();
        delete old.pWarningMap;
        old.pWarningMap = NULL;
    }

    //
    //
    gate.Acquire ();

    m_fTokenized = TRUE;
    m_fTokenizing = FALSE;
    if (m_fFirstTokenization)
    {
        m_pActiveTopLevelHeap->Mark(m_pMarkTokens);
        m_pHeapForFirstTokenization = m_pActiveTopLevelHeap;
        m_fFirstTokenization = FALSE;       
    }
    m_dwVersion++;
    ClearTokenizerThread();

    if (md.fStreamChanged)
    {
        m_fTokensChanged = TRUE;

        long    iFirstChanged;

        if (m_lex.pTokens[md.iTokStart].Token() != md.tokFirst || CParser::GetTokenInfo(md.tokFirst)->iLen == 0)
            iFirstChanged = md.iTokStart;
        else
            iFirstChanged = md.iTokStart + 1;

        if (iFirstChanged < m_iTokDeltaFirst)
            m_iTokDeltaFirst = iFirstChanged;

        if (!md.fIncrementalStop)
            m_iTokDeltaLast = m_lex.iTokens - 1;
    }

    HRESULT     hr;

    if (md.fIncrementalStop)
        hr = m_pTokenErrors->EndReplacement (md.posStart, md.posStopOld, md.posStop);
    else
        hr = m_pTokenErrors->EndReplacement (md.posStart, md.posOldEOF, md.posOldEOF);

    m_pCurrentErrors = NULL;
    pErrorTemp = m_pTokenErrors;

    //
    //
    gate.Release();

    if (hr != S_FALSE && SUCCEEDED(hr))
        m_EventSource.FireReportErrors (pErrorTemp);

    BOOL    fFire = FALSE;
    long    iTop;

    if (m_lex.iTransitionLines != iTransitionCopy)
    {
        fFire = TRUE;
        iTop = 0;
    }
    else
    {
        ASSERT (m_lex.iTransitionLines == 0 || piTransitionCopy != NULL);
        for (iTop=0; iTop < m_lex.iTransitionLines; iTop++)
        {
            if (piTransitionCopy[iTop] > (long)md.posEditOldEnd.iLine)
                piTransitionCopy[iTop] += md.iLineDelta;

            if (m_lex.piTransitionLines[iTop] != piTransitionCopy[iTop])
            {
                fFire = TRUE;
                break;
            }
        }
    }

    if (fFire)
    {
        long    iTopLine;

        ASSERT (piTransitionCopy != NULL || m_lex.piTransitionLines != NULL);
        iTopLine = (piTransitionCopy == NULL) ? m_lex.piTransitionLines[iTop] : (m_lex.piTransitionLines == NULL ? piTransitionCopy[iTop] : min (piTransitionCopy[iTop], m_lex.piTransitionLines[iTop]));
        m_EventSource.FireOnStateTransitionsChanged (iTopLine, m_lex.pTokens[m_lex.iTokens-1].iLine);
    }

    // Fire comment table change event as appropriate
    if (md.iStartComment != -1)
        m_EventSource.FireOnCommentTableChanged (md.iStartComment, md.iOldEndComment, md.iNewEndComment);

    // Fire end-tokenization event
    m_EventSource.FireOnEndTokenization (pData, md.posStart.iLine, md.fIncrementalStop ? md.posStop.iLine : m_lex.pTokens[m_lex.iTokens-1].iLine, md.iLineDelta);

    // If the source text object is capable of releasing the source text, let it do so
    if (SUCCEEDED(hr = m_spSourceText->ReleaseText ())) {
        m_lex.pszSource = NULL;
    }
    else if (hr != E_NOTIMPL) {
        m_pController->NoMemory();
    }
}


////////////////////////////////////////////////////////////////////////////////
// CSourceModule::TrackLine
//
// This function is called from the tokenizer when a physical end-of-line is
// found, to update the line table (growing it if necessary) and keep the lexer
// context up to date.

inline void CSourceModule::TrackLine (CLexer *pCtx, PCWSTR pszNewLine)
{
    // Lexer limit checks -- can't exceed the max sizes specified by POSDATA
    // structures (MAX_POS_LINE_LEN chars/line, MAX_POS_LINE lines)
    if (pCtx->m_iCurLine == MAX_POS_LINE - 1)
    {
        ErrorAtPosition (pCtx->m_iCurLine, 0, 1, ERR_TooManyLines, MAX_POS_LINE);
        pCtx->m_fLimitExceeded = TRUE;
    }
    else if (!pCtx->m_fThisLineTooLong && pszNewLine - pCtx->m_pszCurLine > MAX_POS_LINE_LEN + 1)        // (plus one for CRLF)
    {
        ErrorAtPosition (pCtx->m_iCurLine, MAX_POS_LINE_LEN - 1, 1, ERR_LineTooLong, MAX_POS_LINE_LEN);
        pCtx->m_fLimitExceeded = TRUE;
    }

    // Grow the array if needed
    if (m_fTrackLines && (pCtx->m_iCurLine == m_lex.iAllocLines - 1))
    {
        // Use guess to determine number of remaining lines
        m_lex.iAllocLines += RoundUp32(((m_lex.iTextLen - (long)(pCtx->m_pszCurrent - m_lex.pszSource)) >> 5) + 1);
        m_lex.piLines = (long *)m_pStdHeap->Realloc (m_lex.piLines, m_lex.iAllocLines * sizeof (PCWSTR));
    }

    pCtx->m_iCurLine++;
    if (m_fTrackLines)
        m_lex.piLines[m_lex.iLines = pCtx->m_iCurLine] = (long)(pszNewLine - m_lex.pszSource);
    else
        ASSERT (m_lex.piLines[pCtx->m_iCurLine] == pszNewLine - m_lex.pszSource);

    pCtx->m_pszCurLine = pszNewLine;
    pCtx->m_fFirstOnLine = TRUE;
}


////////////////////////////////////////////////////////////////////////////////
// CSourceModule::RepresentNoiseTokens

BOOL CSourceModule::RepresentNoiseTokens ()
{
    return CheckFlags (CCF_TRACKCOMMENTS);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::TokenMemAlloc

void *CSourceModule::TokenMemAlloc (CSTOKEN *pToken, size_t iSize)
{
    // If this is the first tokenization, we can use the NR heap
    if (m_fFirstTokenization)
        return m_pActiveTopLevelHeap->Alloc (iSize);

    // This is a special case we'll need to detect when removing this token later,
    // either via retokenization or teardown.
    pToken->iUserBits |= TF_HEAPALLOCATED;
    return m_pStdHeap->Alloc (iSize);
}



////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ScanPreprocessorLine

void CSourceModule::ScanPreprocessorLine (CLexer *pCtx, PCWSTR &p, BOOL fFirstOnLine)
{
    if (!fFirstOnLine)
    {
        // This is an error condition -- preprocessor directives must be first token
        ErrorAtPosition (pCtx->m_iCurLine, p - pCtx->m_pszCurLine, 1, ERR_BadDirectivePlacement);
        SkipRemainingPPTokens (p, pCtx);
    }
    else
    {
        // Scan the actual directive
        ScanPreprocessorDirective (pCtx, p);
    }

    // Make sure we end correctly...
    ScanPreprocessorLineTerminator (pCtx, p);

    // Scan to end-of-line
    while (TRUE)
    {
        if (IsEndOfLineChar(*p))
        {
            WCHAR   temp = *p;
            PCWSTR  pszTrack = ++p;

            if (temp == '\r')
            {
                if (*p == '\n')
                {
                    pszTrack = ++p;
                }

            }

            if (pszTrack != NULL)
                TrackLine (pCtx, pszTrack); // Only track physical (non-escaped) CR/CRLF guys
            break;
        }
        else if (*p == 0)
        {
            break;
        }

        p++;
    }
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ScanPreprocessorLineTerminator

void CSourceModule::ScanPreprocessorLineTerminator (CLexer *pCtx, PCWSTR &p)
{
    PCWSTR      pszStart;
    PPTOKENID   tok;
    NAME        *pName;

    tok = ScanPreprocessorToken (pCtx, p, pszStart, &pName, false);
    if (tok != PPT_EOL)
        ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, ERR_EndOfPPLineExpected);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ScanPreprocessorDirective

void CSourceModule::ScanPreprocessorDirective (CLexer *pCtx, PCWSTR &p)
{
    PCWSTR  pszStart;

    // Scan/skip the '#' character
    ASSERT (*p == '#');
    p++;

    NAME        *pName;
    PPTOKENID   ppTok;

    // Scan a PP token, handle accordingly
    ppTok = ScanPreprocessorToken (pCtx, p, pszStart, &pName, true);

    switch (ppTok)
    {
        case PPT_DEFINE:
        case PPT_UNDEF:
            // These can't happen beyond the first token!
            if (pCtx->m_fTokensSeen)
            {
                ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, ERR_PPDefFollowsToken);
                SkipRemainingPPTokens (p, pCtx);
                return;
            }

            ScanPreprocessorDeclaration (p, pCtx, ppTok == PPT_DEFINE);
            break;

        case PPT_ERROR:
        case PPT_WARNING:
            ScanPreprocessorControlLine (p, pCtx, ppTok == PPT_ERROR);
            break;

        case PPT_REGION:
            ScanPreprocessorRegionStart (p, pCtx);
            break;

        case PPT_ENDREGION:
            p = pszStart;
            ScanPreprocessorRegionEnd (p, pCtx);
            break;

        case PPT_IF:
            ScanPreprocessorIfSection (p, pCtx);
            break;

        case PPT_ELIF:
        case PPT_ELSE:
        case PPT_ENDIF:
            p = pszStart;
            ScanPreprocessorIfTrailer (p, pCtx);
            break;

        case PPT_LINE:
            ScanPreprocessorLineDecl (p, pCtx);
            break;

        case PPT_PRAGMA:
            if (GetOptions()->IsECMA1Mode())
            {
                BSTR szFeatureName = NULL;
                PAL_TRY
                {
                    // This feature isn't allowed
                    szFeatureName = CError::ComputeString( GetMessageDll(), IDS_FeaturePragma, NULL);
                    ErrorAtPosition(pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, ERR_NonECMAFeature, szFeatureName);
                }
                PAL_FINALLY
                {
                    SysFreeString (szFeatureName);
                }
                PAL_ENDTRY
            }
            ScanPreprocessorPragma (p, pCtx);
            break;

        default:
            ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, ERR_PPDirectiveExpected);
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ScanPreprocessorToken
//
// if allIds is true, any id-like pp-token is returned (if, define, region, true, ...)
// if allIds is false, only "true" and "false" are considered special tokens, and
// "if", "region", etc. are considered identifiers.

PPTOKENID CSourceModule::ScanPreprocessorToken (CLexer *pCtx, PCWSTR &p, PCWSTR &pszStart, NAME **ppName, bool allIds)
{
    WCHAR   ch, chSurrogate = L'\0', szTok[MAX_PPTOKEN_LEN + 1];
    BOOL    fHasEscapes = FALSE;

    // Skip whitespace
    while (IsWhitespaceChar(*p))
        p++;

    pszStart = p;

    // Check for end-of-line
    if (*p == 0 || IsEndOfLineChar(*p))
        return PPT_EOL;

    PWSTR   pszRun = szTok;

    switch (ch = *p++)
    {
        case '(':
            return PPT_OPENPAREN;

        case ')':
            return PPT_CLOSEPAREN;

        case ',':
            return PPT_COMMA;

        case '!':
            if (*p == '=')
            {
                p++;
                return PPT_NOTEQUAL;
            }
            return PPT_NOT;

        case '=':
            if (*p == '=')
            {
                p++;
                return PPT_EQUAL;
            }
            break;

        case '&':
            if (*p == '&')
            {
                p++;
                return PPT_AND;
            }
            break;

        case '|':
            if (*p == '|')
            {
                p++;
                return PPT_OR;
            }
            break;

        case '/':
            if (*p == '/')
            {
                p = pszStart;       // Don't go past EOL...
                return PPT_EOL;
            }
            break;

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            while (*p >= '0' && *p <= '9')
                p++;
            return PPT_NUMBER;

        case '\"':
            while (*p != '\"' && *p != 0 && !IsEndOfLineChar(*p))
                p++;
            if (*p == '\"') {
                p++;
                return PPT_STRING;
            }
            // Didn't end in a quote, so it's not a string
            p = pszStart + 1;
            break;

        case '\\':
            if (*p == L'u' || *p == L'U')
            {
                fHasEscapes = TRUE;
                ch = pCtx->ScanUnicodeEscape( p, &chSurrogate, FALSE);
                // Fall-through
            }
            else
            {
                break;
            }
        default:
        {
            // Only other thing can be identifiers, which are post-checked
            // for the token identifiers (if, define, etc.)
            if (!IsIdentifierChar (ch)) // BUG 424819 : Handle identifier chars > 0xFFFF via surrogate pairs
                break;

            PCWSTR q;
            do
            {
                if (chSurrogate != L'\0')
                {
                    fHasEscapes = TRUE;
                    // Keep these guys together
                    if (pszRun - szTok < MAX_PPTOKEN_LEN - 1)
                    {
                        *pszRun++ = ch;
                        *pszRun++ = chSurrogate;
                    }
                    else if (pszRun - szTok < MAX_PPTOKEN_LEN)
                    {
                        // We couldn't fit the 2-char surrgate, but we could fit a signle char, so don't pickup
                        // whatever's after the surrogate
                        *pszRun++ = L'\0';
                    }
                }
                else if (pszRun - szTok < MAX_PPTOKEN_LEN)
                    *pszRun++ = ch;

                q = p;
                ch = pCtx->NextChar (p, &chSurrogate);
            }
            while (IsIdentifierCharOrDigit (ch)); // BUG 424819 : Handle identifier chars > 0xFFFF via surrogate pairs

            p = q; // Backup to the previous character (because we read one more than we need to)
            *pszRun = 0;
            *ppName = GetNameMgr()->AddString (szTok);
            int i;
            if (!fHasEscapes && GetNameMgr()->IsPPKeyword (*ppName, &i)) {
                if (allIds || !IsKeywordDirective[i])
                    return (PPTOKENID)i;
            }

            return PPT_IDENTIFIER;
        }

    }

    // If you break out of the above switch, it is assumed you didn't recognize
    // anything...
    return PPT_UNKNOWN;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ScanPreprocessorDeclaration

void CSourceModule::ScanPreprocessorDeclaration (PCWSTR &p, CLexer *pCtx, BOOL fDefine)
{
    NAME    *pName;
    PCWSTR  pszStart;

    if (ScanPreprocessorToken (pCtx, p, pszStart, &pName, false) != PPT_IDENTIFIER)
    {
        ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, ERR_IdentifierExpected);
        return;
    }

    if (fDefine)
        m_tableDefines.Define (pName);
    else
        m_tableDefines.Undef (pName);
    m_fCCSymbolChange = TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ScanPreprocessorPragmaWarning

void CSourceModule::ScanPreprocessorPragmaWarning (PCWSTR &p, CLexer *pCtx)
{
    NAME    *pName;
    PCWSTR  pszStart;
    PPTOKENID tok;

    tok = ScanPreprocessorToken (pCtx, p, pszStart, &pName, true);
    
    if (tok == PPT_DISABLE || tok == PPT_RESTORE)
    {
        bool bDisable = (tok == PPT_DISABLE);
        bool bHadNumbers = false;
        WORD cntWarnings = 0;
        WORD numberStartBuffer[32]; // start with this buffer on stack
        WORD *numbers = numberStartBuffer;
        int cMaxNumbers = 32;
        
        pszStart = p;
        if (m_lex.pWarningMap == NULL)
        {
            m_lex.pWarningMap = new CWarningMap(m_pStdHeap);
        }
        
        if (ScanPreprocessorToken (pCtx, p, pszStart, &pName, false) != PPT_EOL)
        {
            p = pszStart;
            do 
            {
                long warn;
                if (ScanPreprocessorToken (pCtx, p, pszStart, &pName, false) == PPT_NUMBER)
                {
                    if (!ScanPreprocessorNumber (pszStart, warn, pCtx))
                        goto FAIL;

                    // Warning numbers are validated later during parsing
                    // so we can track the warnings easier
                    if ((WORD)warn != warn || !IsValidWarningNumber(warn))
                    {
                        WCHAR warnText[128];  // It's a valid long so it should fit
                        ASSERT(p - pszStart < 128);
                        StringCchCopyNW(warnText, lengthof(warnText), pszStart, p - pszStart);
                        ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, WRN_BadWarningNumber, warnText);
                    }
                    else if (!GetOptions()->IsNoWarnNumber((WORD)warn)) {
                        if (cntWarnings >= cMaxNumbers) {
                            WORD *tmp = STACK_ALLOC(WORD, cMaxNumbers*2);
                            memcpy(tmp, numbers, cMaxNumbers * sizeof(WORD));
                            cMaxNumbers *= 2;
                            numbers = tmp;
                        }
                        numbers[cntWarnings++] = (WORD)warn;
                    }
                    bHadNumbers = true;
                }
                else
                {
                    ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, WRN_InvalidNumber);
                    goto FAIL;
                }
            }
            while (ScanPreprocessorToken (pCtx, p, pszStart, &pName, false) == PPT_COMMA);
        }

        // Don't add an empty disable or restore directive unless the user specified it that way!
        if (cntWarnings != 0 || !bHadNumbers)
            m_lex.pWarningMap->AddWarning (pCtx->m_iCurLine, bDisable, cntWarnings, numbers);

        p = pszStart;
    }
    else
    {
        ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, WRN_IllegalPPWarning);
FAIL:
        SkipRemainingPPTokens (p, pCtx);
    }
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ScanPreprocessorPragma

void CSourceModule::ScanPreprocessorPragma (PCWSTR &p, CLexer *pCtx)
{
    NAME    *pName;
    PCWSTR  pszStart;
    PPTOKENID tok;

    tok = ScanPreprocessorToken (pCtx, p, pszStart, &pName, true);

    if (tok == PPT_WARNING)
    {
        ScanPreprocessorPragmaWarning( p, pCtx);
    }
    else if (tok == PPT_CHECKSUM)
    {
        ScanPreprocessorPragmaChecksum( p, pCtx);
    }
    else
    {
        ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, WRN_IllegalPragma);
        SkipRemainingPPTokens (p, pCtx);
    }

    // Check for the end of line here rather than the normal place so we can ensure no
    // errors are ever reported on a #pragma directive
    tok = ScanPreprocessorToken (pCtx, p, pszStart, &pName, false);
    if (tok != PPT_EOL) {
        ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, WRN_EndOfPPLineExpected);
        SkipRemainingPPTokens (p, pCtx);
    }

}



////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ScanPreprocessorPragmaChecksum

void CSourceModule::ScanPreprocessorPragmaChecksum (PCWSTR &p, CLexer *pCtx)
{
    NAME    *pFileName;
    NAME    *pTokenName;
    PCWSTR    pszTokenStart;
    BlobBldrMemHeap blobChecksum(m_pStdHeap);

    // Filename
    if (!ScanPreprocessorFilename (p, pCtx, &pFileName, true))
    {
FAIL:
        SkipRemainingPPTokens (p, pCtx);
        return;
    }

    // We should have gotten a filename, followed by whitespace and a string
    if (pFileName == NULL || !IsWhitespaceChar(*p))
    {
        ErrorAtPosition (pCtx->m_iCurLine, p - pCtx->m_pszCurLine, 1, WRN_IllegalPPChecksum);
        goto FAIL;
    }

    if (PPT_STRING != ScanPreprocessorToken( pCtx, p, pszTokenStart, &pTokenName, true))
    {
        ErrorAtPosition (pCtx->m_iCurLine, pszTokenStart - pCtx->m_pszCurLine, p - pszTokenStart, WRN_IllegalPPChecksum);
        goto FAIL;
    }


    // Parse the string into a GUID
    GUID     guidChecksumID;
    if (!GUIDFromString( pszTokenStart + 1, p - 1, &guidChecksumID))
    {
        ErrorAtPosition (pCtx->m_iCurLine, pszTokenStart - pCtx->m_pszCurLine, p - pszTokenStart, WRN_IllegalPPChecksum);
        goto FAIL;
    }

    // Whitespace and a string of bytes
    if (!IsWhitespaceChar(*p))
    {
        ErrorAtPosition (pCtx->m_iCurLine, p - pCtx->m_pszCurLine, 1, WRN_IllegalPPChecksum);
        goto FAIL;
    }

    if (PPT_STRING != ScanPreprocessorToken( pCtx, p, pszTokenStart, &pTokenName, true) || ((p - pszTokenStart) & 1) != 0)
    {
        ErrorAtPosition (pCtx->m_iCurLine, pszTokenStart - pCtx->m_pszCurLine, p - pszTokenStart, WRN_IllegalPPChecksum);
        goto FAIL;
    }

    // +/-1 to take into account the open and close quotes
    for (const WCHAR *  pszCur = pszTokenStart + 1; pszCur < p - 1; pszCur += 2)
    {
        if (!IsHexDigit(pszCur[0]) ||
            !IsHexDigit(pszCur[1]))
        {
            ErrorAtPosition (pCtx->m_iCurLine, pszCur - pCtx->m_pszCurLine, 2, WRN_IllegalPPChecksum);
            goto FAIL;
        }
        blobChecksum.Add((BYTE)((HexValue(pszCur[0]) << 4) | HexValue(pszCur[1])));
        if (blobChecksum.Error())
        {
            m_pController->NoMemory();
            goto FAIL;
        }
    }

    ChecksumData * checksum;
    if (FAILED(m_rgChecksum.Add(NULL, &checksum)))
    {
        // Out of memory
        m_pController->NoMemory();
        goto FAIL;
    }

    checksum->pFilename = pFileName;
    checksum->guidChecksumID = guidChecksumID;
    checksum->cbChecksumData = blobChecksum.Length();
    checksum->pbChecksumData = blobChecksum.Detach();
    checksum->pErrlocChecksum = new ERRLOC( this, POSDATA( pCtx->m_iCurLine, 0), POSDATA( pCtx->m_iCurLine, (long)(p - pCtx->m_pszCurLine)));
}
    
////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ScanPreprocessorControlLine

void CSourceModule::ScanPreprocessorControlLine (PCWSTR &p, CLexer *pCtx, BOOL fError)
{
    WCHAR   szBuf[501];  // 500 is the maximum size of the message we'll support.
    PWSTR   pszRun = szBuf;

    // The rest of this line (including any single-line comments) is the error/warning text.
    // First, skip leading whitespace
    while (IsWhitespaceChar(*p))
    {
        p++;
    }

    PCWSTR  pszStart = p;

    // Copy the rest of the line (up to limit chars) into szBuf
    while (TRUE)
    {
        if (*p == 0 || IsEndOfLineChar(*p))
            break;

        if (pszRun - szBuf < (int)lengthof(szBuf) - 1)
            *pszRun++ = *p++;
        else
            p++;
    }

    *pszRun = 0;

    ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, fError ? ERR_ErrorDirective : WRN_WarningDirective, szBuf);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ScanPreprocessorRegionStart

void CSourceModule::ScanPreprocessorRegionStart (PCWSTR &p, CLexer *pCtx)
{
    // This is pretty simply -- just push a record on the CC stack so that we
    // don't allow overlapping of regions with #if guys...
    PushCCRecord (m_pCCStack, CCF_REGION);
    MarkCCStateChange (pCtx->m_iCurLine, CCF_REGION|CCF_ENTER);

    // We don't care about the text following...
    SkipRemainingPPTokens (p, pCtx);
    return;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ScanPreprocessorRegionEnd

void CSourceModule::ScanPreprocessorRegionEnd (PCWSTR &p, CLexer *pCtx)
{
    NAME        *pName;
    PCWSTR      pszStart;
    PPTOKENID   tok;

    tok = ScanPreprocessorToken (pCtx, p, pszStart, &pName, true);
    ASSERT (tok == PPT_ENDREGION);

    // Make sure we're in the right kind of CC stack state
    if (m_pCCStack == NULL || (m_pCCStack->dwFlags & CCF_REGION) == 0)
    {
        ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, m_pCCStack == NULL ? ERR_UnexpectedDirective : ERR_EndifDirectiveExpected);
    }
    else
    {
        PopCCRecord (m_pCCStack);
        MarkCCStateChange (pCtx->m_iCurLine, CCF_REGION);
    }

    SkipRemainingPPTokens (p, pCtx);
    return;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ScanPreprocessorIfSection

void CSourceModule::ScanPreprocessorIfSection (PCWSTR &p, CLexer *pCtx)
{
    NAME    *pName;
    PCWSTR  pszStart;

    // First, scan and evaluate a preprocessor expression.   If TRUE, push an
    // appropriate state record and return
    if (EvalPreprocessorExpression (p, pCtx))
    {
        // Push a CC record -- it is NOT an else block.  Also mark a state
        // transition (to reconstruct this stack state at incremental tokenization
        // time)
        PushCCRecord (m_pCCStack, 0);
        MarkCCStateChange (pCtx->m_iCurLine, CCF_ENTER);
        return;
    }

    CCREC   *pSkipStack = NULL;

    // Mark this line as a transition (we're going from include to exclude)
    MarkTransition (pCtx->m_iCurLine);

    // Make sure there's no other gunk on the end of this line
    ScanPreprocessorLineTerminator (pCtx, p);

    while (TRUE)
    {
        // Scan for the next preprocessor directive
        ScanExcludedCode (p, pCtx);

        // Check for '#' (if not present, ScanExcludedCode will have reported the error...)
        if (*p != '#')
        {
            // Don't leak the now-useless skip stack entries
            while (pSkipStack != NULL)
                PopCCRecord (pSkipStack);
            return;     // NOTE:  No transition, so just return from here   
        }

        // Scan the directive
        p++;
        PPTOKENID   tok = ScanPreprocessorToken (pCtx, p, pszStart, &pName, true);

        if (pSkipStack != NULL &&
           (((pSkipStack->dwFlags & CCF_REGION) && (tok == PPT_ELSE || tok == PPT_ELIF || tok == PPT_ENDIF)) ||
            ((pSkipStack->dwFlags & CCF_REGION) == 0 && (tok == PPT_ENDREGION))))
        {
            // Bad mojo -- can't overlap #region/#endregion and #if/#else/elif/#endif blocks
            ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, (pSkipStack->dwFlags & CCF_REGION) ? ERR_EndRegionDirectiveExpected : ERR_EndifDirectiveExpected);
            ScanAndIgnoreDirective (tok, pszStart, p, pCtx);
            continue;
        }

        if (tok == PPT_ELSE)
        {
            if (pSkipStack == NULL)
            {
                // Push a CC record -- we ARE in the else block now.  Also mark
                // a CC state transition to enable stack reconstruction.  Then break
                // out of the while loop to mark this as a transition line
                PushCCRecord (m_pCCStack, CCF_ELSE);
                MarkCCStateChange (pCtx->m_iCurLine, CCF_ELSE|CCF_ENTER);
                break;
            }

            ASSERT ((pSkipStack->dwFlags & CCF_REGION) == 0);
        }


        if (tok == PPT_ELIF)
        {
            if (pSkipStack == NULL)
            {
                if (EvalPreprocessorExpression (p, pCtx))
                {
                    // This is the same as an IF block -- it is NOT an "else" block.
                    // Record the state change for reconstruction on retokenization.
                    // Break from the loop, which will mark this line as a transition
                    // and return.
                    PushCCRecord (m_pCCStack, 0);
                    MarkCCStateChange (pCtx->m_iCurLine, CCF_ENTER);
                    break;
                }

                // Make sure the line ends "cleanly" and then go scan another excluded block
                ScanPreprocessorLineTerminator (pCtx, p);
                continue;
            }

            ASSERT ((pSkipStack->dwFlags & CCF_REGION) == 0);
        }

        if (tok == PPT_IF)
        {
            // This is the beginning of another if-section.  Push an 'if' record on to
            // our skip stack, and then scan (and ignore) the directive to make sure it
            // is syntactically correct.
            PushCCRecord (pSkipStack, 0);
            ScanAndIgnoreDirective (tok, pszStart, p, pCtx);
            continue;
        }

        if (tok == PPT_ENDIF)
        {
            // Here's the end of an if-section (but not necessarily ours).  If we
            // are currently at the same depth as when we started, we're done.
            // Otherwise, pop the top record off of our skip stack and continue.
            if (pSkipStack != NULL)
            {
                PopCCRecord (pSkipStack);
                ScanAndIgnoreDirective (tok, pszStart, p, pCtx);
                continue;
            }

            // This was our endif.  Break out, mark this line as transition.
            break;
        }

        if (tok == PPT_REGION)
        {
            // Here's the beginning of a region.  We don't care about it, but to
            // enforce correct nesting, we push a record on our skip stack
            PushCCRecord (pSkipStack, CCF_REGION);
            ScanAndIgnoreDirective (tok, pszStart, p, pCtx);
            continue;
        }

        if (tok == PPT_ENDREGION)
        {
            // In order to be correct this must match the topmost record on
            // our skip stack
            if (pSkipStack == NULL || (pSkipStack->dwFlags & CCF_REGION) == 0)
                ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, pSkipStack != NULL ? ERR_EndifDirectiveExpected : ERR_UnexpectedDirective);
            else
                PopCCRecord (pSkipStack);
        }

        // Unrecognized/ignored directive/token -- skip it, checking it for errors first
        ScanAndIgnoreDirective (tok, pszStart, p, pCtx);
    }

    // Mark this line as another transition (from exclude to include)
    MarkTransition (pCtx->m_iCurLine);

    ASSERT (pSkipStack == NULL);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ScanPreprocessorIfTrailer

void CSourceModule::ScanPreprocessorIfTrailer (PCWSTR &p, CLexer *pCtx)
{
    NAME        *pName;
    PCWSTR      pszStart;
    PPTOKENID   tok = ScanPreprocessorToken (pCtx, p, pszStart, &pName, true);

    // Here, we check to see if we are currently in a CC state.  If we aren't,
    // the directive found is unexpected
    if (m_pCCStack == NULL || m_pCCStack->dwFlags & CCF_REGION)
    {
        ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, m_pCCStack != NULL ? ERR_EndRegionDirectiveExpected : ERR_UnexpectedDirective);

        // Scan the rest of the tokens off so we don't get more errors.
        SkipRemainingPPTokens (p, pCtx);
        return;
    }

    CCREC   *pSkipStack = NULL;

    // This line marks the state change (exit).  Do that now.
    MarkCCStateChange (pCtx->m_iCurLine, 0);

    // If this is not a PPT_ENDIF, we need to skip excluded code blocks
    if (tok != PPT_ENDIF)
    {
        // We're transitioning here...
        MarkTransition (pCtx->m_iCurLine);

        while (TRUE)
        {
            if (pSkipStack != NULL &&
               (((pSkipStack->dwFlags & CCF_REGION) && (tok == PPT_ELSE || tok == PPT_ELIF || tok == PPT_ENDIF)) ||
                ((pSkipStack->dwFlags & CCF_REGION) == 0 && (tok == PPT_ENDREGION))))
            {
                // Bad mojo -- can't overlap #region/#endregion and #if/#else/elif/#endif blocks
                ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, (pSkipStack->dwFlags & CCF_REGION) ? ERR_EndRegionDirectiveExpected : ERR_EndifDirectiveExpected);
            }
            else
            {
                // If there's nothing on our skip stack, we need to sniff this directive a bit...
                if (pSkipStack == NULL)
                {
                    ASSERT (m_pCCStack != NULL && (m_pCCStack->dwFlags & CCF_REGION) == 0);

                    // If we were in the "else" block, we can't see any more else or elif blocks!
                    if ((m_pCCStack->dwFlags & CCF_ELSE) && (tok == PPT_ELSE || tok == PPT_ELIF))
                        ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, ERR_UnexpectedDirective);

                    // #endregion is also bogus here...
                    if (tok == PPT_ENDREGION)
                        ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, ERR_UnexpectedDirective);

                    // Is this the else?  If so, mark it in the state record
                    if (tok == PPT_ELSE)
                        m_pCCStack->dwFlags |= CCF_ELSE;
                }

                // Handle the directives appropriately -- we care about #if/#region, and #endif/#endregion
                if (tok == PPT_IF || tok == PPT_REGION)
                {
                    // Push an appropriate record on the skip stack.
                    PushCCRecord (pSkipStack, tok == PPT_REGION ? CCF_REGION : 0);
                }
                else if (tok == PPT_ENDIF || tok == PPT_ENDREGION)
                {
                    // If the skip stack is empty and this was an #endif, we're done.
                    if (tok == PPT_ENDIF && pSkipStack == NULL)
                        break;

                    // Make sure the stack corresponds to the directive
                    if (pSkipStack == NULL || (tok == PPT_ENDIF && (pSkipStack->dwFlags & CCF_REGION)) || (tok == PPT_ENDREGION && (pSkipStack->dwFlags & CCF_REGION) == 0))
                    {
                        short   iErr;

                        if (pSkipStack == NULL)
                            iErr = (m_pCCStack->dwFlags & CCF_REGION) ? ERR_EndRegionDirectiveExpected : ERR_EndifDirectiveExpected;
                        else
                            iErr = (pSkipStack->dwFlags & CCF_REGION) ? ERR_EndRegionDirectiveExpected : ERR_EndifDirectiveExpected;

                        ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, iErr);
                    }
                    else
                    {
                        PopCCRecord (pSkipStack);
                    }
                }
            }

            // Scan and ignore the rest of whatever this directive was
            ScanAndIgnoreDirective (tok, pszStart, p, pCtx);

            // Continue scanning excluded code for another directive
            ScanExcludedCode (p, pCtx);

            // Check for '#' (if not present, ScanExcludedCode will have reported the error...)
            if (*p != '#')
            {
                // Don't leak the now worthless skip stack entries...
                while (pSkipStack != NULL)
                    PopCCRecord (pSkipStack);

                return;     // Don't mark the reverse transition...
            }

            // Scan the new directive and continue
            p++;
            tok = ScanPreprocessorToken (pCtx, p, pszStart, &pName, true);
        }

        // Transition back out...
        MarkTransition (pCtx->m_iCurLine);
    }

    // Okay... pop the state record and return...
    PopCCRecord (m_pCCStack);

    ASSERT (pSkipStack == NULL);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ScanPreprocessorNumber

bool CSourceModule::ScanPreprocessorNumber (PCWSTR p, long &number, CLexer *pCtx)
{
    bool isValidNum = true;
    number = 0;

    while (*p >= '0' && *p <= '9')
    {
        long oldnumber = (number == 0 ? -1 : number);
        number = number * 10 + (*p - '0');
        if (number <= oldnumber && isValidNum)
        {
            ErrorAtPosition (pCtx->m_iCurLine, p - pCtx->m_pszCurLine,  0, ERR_IntOverflow);
            isValidNum = false;
            // Keep parsing the number so we don't get multiple bogus errors
        }
        p++;
    }

    return isValidNum;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ScanPreprocessorFilename
// return true if there was either no filename or if there was a complete filename
// false indicates an error or warning was reported
bool CSourceModule::ScanPreprocessorFilename (PCWSTR &p, CLexer *pCtx, NAME ** ppFilename, bool fPragma)
{
    WCHAR  szBuf[MAX_PATH];
    PCWSTR pszFilename = p;
    NAME * pName = NULL;

    *ppFilename = NULL;

    if (!IsWhitespace(*p) || PPT_STRING != ScanPreprocessorToken( pCtx, p, pszFilename, &pName, false))
    {
        // No string so no filename, backup and return
        p = pszFilename;
        return true;
    }

    if (FAILED(StringCchCopyNW(szBuf, lengthof(szBuf), pszFilename + 1, p - (pszFilename + 2)))) {
        ErrorAtPosition(pCtx->m_iCurLine, pszFilename - pCtx->m_pszCurLine, p - pszFilename, fPragma ? WRN_FileNameTooLong : ERR_FileNameTooLong);
        return false;
    }

    if (szBuf[0] == L'\0') {
        ErrorAtPosition(pCtx->m_iCurLine, pszFilename - pCtx->m_pszCurLine, p - pszFilename, WRN_EmptyFileName);
    }
    else {
         NAME* cached_name = NULL;


        if (!cached_name)
        {
            // now make the filename fully-qualified
            // If it is relative, assume it is relative to the source file
            // So start by getting the source file's path (without filename)
            if (m_pszSrcDir == NULL)
            {
                WCHAR szNewDir[MAX_PATH];
                PCWSTR szBSlash, szFSlash;
                PCWSTR szSrcFile;

                m_spSourceText->GetName (&szSrcFile);
                szBSlash = wcsrchr(szSrcFile, L'\\');
                szFSlash = wcsrchr(szSrcFile, L'/');
                if (szBSlash < szFSlash)
                    szBSlash = szFSlash;
                if (szBSlash == NULL)
                    szBSlash = szSrcFile + wcslen(szSrcFile);
                if (FAILED(StringCchCopyNW( szNewDir, lengthof(szNewDir), szSrcFile, szBSlash + 1 - szSrcFile)))
                {
                    // Error is probably that the full name is too long.
                    ErrorAtPosition(pCtx->m_iCurLine, pszFilename - pCtx->m_pszCurLine, p - pszFilename, fPragma ? WRN_FileNameTooLong : ERR_FileNameTooLong);
                }
                m_pszSrcDir = m_pStdHeap->AllocStr(szNewDir);
            }

            WCHAR formatted_name[MAX_PATH];

            // Now relativize it (this handles UNCs, URLs, and relative paths)
            if (!MakePath (m_pszSrcDir, szBuf, WCBuffer(formatted_name)))
            {
                ErrorAtPosition(pCtx->m_iCurLine, pszFilename - pCtx->m_pszCurLine, p - pszFilename, fPragma ? WRN_FileNameTooLong : ERR_FileNameTooLong);
                return false;
            }

            NAME * key, * val;
            //stash both key and value somewhere with long lifetime
            key = GetNameMgr()->AddString( szBuf);
            val = GetNameMgr()->AddString( formatted_name);
            cached_name = val;
        }


        ASSERT (cached_name);
        *ppFilename = cached_name;
        return true;
    }

    *ppFilename = GetNameMgr()->AddString( szBuf);
    return true;
}


////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ScanPreprocessorLineDecl

void CSourceModule::ScanPreprocessorLineDecl (PCWSTR &p, CLexer *pCtx)
{
    PCWSTR  pszStart = NULL;
    NAME   *pName = NULL;


    // The rest of this line (including any single-line comments) is the line and optional filename

    // First check for  "default" (this will skip past whitespace)
    PPTOKENID tok = ScanPreprocessorToken(pCtx, p, pszStart, &pName, false);
    if (tok == PPT_DEFAULT) 
    {
        PCWSTR w_original = NULL;
        m_spSourceText->GetName (&w_original);
        pName = GetNameMgr()->AddString( w_original);
        m_LineMap.AddMap(pCtx->m_iCurLine, true/*fDefaultLine*/, pCtx->m_iCurLine + 1, pName);
    } 
    else if (tok == PPT_HIDDEN) 
    {
        // and "hidden"
        m_LineMap.HideLines(pCtx->m_iCurLine);
    } 
    else 
    {
        bool    isValidNum = (tok == PPT_NUMBER);
        long    newLine = 0;

        if (isValidNum)
        {
            // get the line number
            isValidNum = ScanPreprocessorNumber( pszStart, newLine, pCtx);
            if (isValidNum && (newLine < 1 || newLine > MAX_POS_LINE))
                ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine,  0, ERR_InvalidLineNumber);
        }
        else
        {
            // Reset to before the token
            p = pszStart;
        }

        if (!isValidNum)
        {
            ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine,  0, ERR_InvalidLineNumber);
            goto FAIL;
        }

        // do we have a filename?
        if (ScanPreprocessorFilename( p, pCtx, &pName, false))
        {
            // Subtract 1 because lines are Zero based in the Lexer/Parser
            m_LineMap.AddMap(pCtx->m_iCurLine, false/*fDefaultLine*/, newLine - 1, pName);

            if (pName == NULL) {
                // Check for the EOL so we can give a better error message
                pszStart = NULL;
                pName = NULL;
                if (PPT_EOL != ScanPreprocessorToken (pCtx, p, pszStart, &pName, false))
                {
                    ErrorAtPosition(pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, ERR_MissingPPFile);

                    goto FAIL;
                }
            }
        }
        else
        {
FAIL:
            // Skip over these, so we don't get multiple errors
            SkipRemainingPPTokens(p, pCtx);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::SkipRemainingPPTokens

void CSourceModule::SkipRemainingPPTokens (PCWSTR &p, CLexer *pCtx)
{
    PCWSTR      pszStart;
    NAME        *pName;
    PPTOKENID   tok;

    do
        tok = ScanPreprocessorToken (pCtx, p, pszStart, &pName, false);
    while (tok != PPT_EOL);
}


////////////////////////////////////////////////////////////////////////////////
// CSourceModule::EvalPreprocessorExpression

BOOL CSourceModule::EvalPreprocessorExpression (PCWSTR &p, CLexer *pCtx, PPTOKENID tokOpPrec)
{
    NAME        *pName;
    PCWSTR      pszStart;
    PPTOKENID   tok;
    BOOL        fTerm;

    // First, we need a term -- possibly prefixed with a ! operator
    tok = ScanPreprocessorToken (pCtx, p, pszStart, &pName, false);
    switch (tok)
    {
        case PPT_NOT:
            fTerm = !EvalPreprocessorExpression (p, pCtx, tok);
            break;

        case PPT_OPENPAREN:
            fTerm = EvalPreprocessorExpression (p, pCtx);   // (default (lowest) precedence)
            if (ScanPreprocessorToken (pCtx, p, pszStart, &pName, false) != PPT_CLOSEPAREN)
            {
                ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, ERR_CloseParenExpected);
                p = pszStart;
            }
            break;

        case PPT_TRUE:
        case PPT_FALSE:
            fTerm = (tok == PPT_TRUE);
            break;

        case PPT_IDENTIFIER:
            fTerm = m_tableDefines.IsDefined (pName);
            break;

        default:
            ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, ERR_InvalidPreprocExpr);
            SkipRemainingPPTokens (p, pCtx);
            return TRUE;
    }

    // Okay, now look for an operator.
    tok = ScanPreprocessorToken (pCtx, p, pszStart, &pName, false);
    if (tok >= PPT_OR && tok <= PPT_NOTEQUAL)
    {
        // We'll only 'take' this operator if it is of equal or higher precedence
        // than that which we were given
        if (tok < tokOpPrec)
        {
            p = pszStart;       // Back up to the token again so we don't skip it
        }
        else
        {
            BOOL    fRHS = EvalPreprocessorExpression (p, pCtx, tok == PPT_NOTEQUAL ? PPT_EQUAL : tok);

            switch (tok)
            {
                case PPT_OR:        fTerm = fTerm || fRHS;          break;
                case PPT_AND:       fTerm = fTerm && fRHS;          break;
                case PPT_EQUAL:     fTerm = (!fTerm) == (!fRHS);    break;
                case PPT_NOTEQUAL:  fTerm = (!fTerm) != (!fRHS);    break;
                default:
                    VSFAIL ("Unrecognized preprocessor expression operator!");
            }
        }
    }
    else
    {
        // Not an operator, so put the token back!
        p = pszStart;
    }

    return fTerm;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ScanExcludedCode

void CSourceModule::ScanExcludedCode (PCWSTR &p, CLexer *pCtx)
{
    // Simply skip everything until we see another '#' character as the first
    // non-white character of a line, or until we hit end of file.  If we DO
    // hit end-of-file first, issue an error
    while (TRUE)
    {
        WCHAR   ch = *p;
        if (IsEndOfLineChar(ch))
        {
            WCHAR   temp = ch;
            PCWSTR  pszTrack = ++p;
            ch = *p;

            if (temp == '\r')
            {
                if (*p == '\n')
                {
                    pszTrack = ++p;
                    ch = *p;
                }
            }

            if (pszTrack != NULL)
            {
                TrackLine (pCtx, pszTrack); // Only track the line if it's a physical LF (non-escaped)
                // Skip whitespace
                while(IsWhitespaceChar(ch))
                {
                    ch = *(++p);
                }
                // If this is a '#', we're done
                if (ch == '#')
                {
                    return;
                }
            }
        }
        else if (ch == 0)
        {
            ErrorAtPosition (pCtx->m_iCurLine, p - pCtx->m_pszCurLine, 1, ERR_EndifDirectiveExpected);
            return;
        }
        else
        {
            p++;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ScanAndIgnoreDirective

void CSourceModule::ScanAndIgnoreDirective (PPTOKENID tok, PCWSTR pszStart, PCWSTR &p, CLexer *pCtx)
{
    if (tok == PPT_IF || tok == PPT_ELIF)
    {
        EvalPreprocessorExpression (p, pCtx);
    }
    else if (tok == PPT_ERROR || tok == PPT_WARNING || tok == PPT_REGION || tok == PPT_ENDREGION || tok == PPT_LINE || tok == PPT_PRAGMA)
    {
        SkipRemainingPPTokens (p, pCtx);
    }
    else if (tok == PPT_DEFINE || tok == PPT_UNDEF)
    {
        NAME    *pName;

        tok = ScanPreprocessorToken (pCtx, p, pszStart, &pName, false);
        if (tok != PPT_IDENTIFIER)
        {
            ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, ERR_IdentifierExpected);
            SkipRemainingPPTokens (p, pCtx);
        }
    }
    else if (tok != PPT_ELSE && tok != PPT_ENDIF && tok != PPT_ENDREGION)
    {
        ErrorAtPosition (pCtx->m_iCurLine, pszStart - pCtx->m_pszCurLine, p - pszStart, ERR_PPDirectiveExpected);
        SkipRemainingPPTokens (p, pCtx);
    }

    // Make sure the line ends cleanly
    ScanPreprocessorLineTerminator (pCtx, p);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::PushCCRecord

void CSourceModule::PushCCRecord (CCREC * & pStack, DWORD dwFlags)
{
    CCREC   *pNew = (CCREC *)VSAlloc (sizeof (CCREC));
    if (!pNew)
        m_pController->NoMemory();
    pNew->pPrev = pStack;
    pStack = pNew;
    pNew->dwFlags = dwFlags;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::PopCCRecord

BOOL CSourceModule::PopCCRecord (CCREC * &pStack)
{
    if (pStack != NULL)
    {
        CCREC   *pTmp = pStack;
        pStack = pStack->pPrev;
        VSFree (pTmp);
        return TRUE;
    }

    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::CompareCCStacks

BOOL CSourceModule::CompareCCStacks (CCREC *pOne, CCREC *pTwo)
{
    if (pOne == pTwo)
        return TRUE;

    if (pOne == NULL || pTwo == NULL)
        return FALSE;

    return (pOne->dwFlags == pTwo->dwFlags) && CompareCCStacks (pOne->pPrev, pTwo->pPrev);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::MarkTransition

void CSourceModule::MarkTransition (long iLine)
{
    /* NOTE:  This was taken out for colorization purposes...

    // Optimization:  Two consecutive transitions cancel each other out...
    if (m_lex.iTransitionLines > 0 && ((m_lex.piTransitionLines[m_lex.iTransitionLines - 1]) == (iLine - 1)))
    {
        m_lex.iTransitionLines--;
        return;
    }
    */

    // Make sure there's room
    if ((m_lex.iTransitionLines & 7) == 0)
    {
        size_t iSize = SizeMul(m_lex.iTransitionLines + 8, sizeof (long));
        m_lex.piTransitionLines = (long *)((m_lex.piTransitionLines == NULL) ? VSAlloc (iSize) : VSRealloc (m_lex.piTransitionLines, iSize));
        if (!m_lex.piTransitionLines)
            m_pController->NoMemory();
    }

    ASSERT (m_lex.piTransitionLines[m_lex.iTransitionLines] != *(long *)"END!");
    m_lex.piTransitionLines[m_lex.iTransitionLines++] = iLine;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::MarkCCStateChange

void CSourceModule::MarkCCStateChange (long iLine, DWORD dwFlags)
{
    // Make sure there's room
    if ((m_lex.iCCStates & 7) == 0)
    {
        size_t iSize = SizeMul(m_lex.iCCStates + 8, sizeof (long));
        m_lex.pCCStates = (CCSTATE *) ((m_lex.pCCStates == NULL) ? VSAlloc (iSize) : VSRealloc (m_lex.pCCStates, iSize));
        if (!m_lex.pCCStates)
            m_pController->NoMemory();
    }

    m_lex.pCCStates[m_lex.iCCStates].dwFlags = dwFlags;
    m_lex.pCCStates[m_lex.iCCStates++].iLine = iLine;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::FinalRelease

void CSourceModule::FinalRelease ()
{
    BOOL    fLocked = m_StateLock.Acquire ();

    if (VSFSWITCH (gfUnhandledExceptions))
    {
        delete this;

        // Because of the above, this object is unlocked in the dtor...
        fLocked = FALSE;
    }
    else
    {
        PAL_TRY
        {
            delete this;

            // Because of the above, this object is unlocked in the dtor...
            fLocked = FALSE;
        }
        EXCEPT_EXCEPTION
        {
            CLEANUP_STACK // handle stack overflow
            m_pController->HandleException (GetExceptionCode());
        }
        PAL_ENDTRY
    }

    if (fLocked)
    {
        ASSERT (m_StateLock.LockedByMe());
        m_StateLock.Release();
    }
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::AddRef

STDMETHODIMP_(ULONG) CSourceModule::AddRef ()
{
    return InternalAddRef();
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::Release

STDMETHODIMP_(ULONG) CSourceModule::Release ()
{
    return InternalRelease();
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::QueryInterface

STDMETHODIMP CSourceModule::QueryInterface (REFIID riid, void **ppObj)
{
    *ppObj = NULL;

    if (riid == IID_IUnknown || riid == IID_ICSSourceModule)
    {
        *ppObj = (ICSSourceModule *)this;
        ((ICSSourceModule *)this)->AddRef();
        return S_OK;
    }
    else if (riid == IID_ICSSourceTextEvents)
    {
        *ppObj = (ICSSourceTextEvents *)this;
        ((ICSSourceTextEvents *)this)->AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CSourceModule::GetSourceData (BOOL fBlockForNewest, ICSSourceData **ppData)
{
    CTinyGate   gate (&m_StateLock);
    BOOL        fNewest = (m_iDataRef == 0 || !m_fTextModified);
    BOOL        fCheckThisThread = TRUE;

    while (fBlockForNewest && m_iDataRef > 0 && m_fTextModified)
    {
        // We must wait until all outstanding source data objects are released.
        if (fCheckThisThread)
        {
            long    iThreadIdx = FindThreadIndex (FALSE);

            // We must also check first to see if the calling thread holds such an
            // object, and if so, fail.
            if (m_pThreadDataRef[iThreadIdx].iRef != 0)
            {
                DBGOUT (("HEY!  ICSSourceModule::GetSourceData(TRUE) called while outstanding data ref held by same thread!  BAD!"));
                VSFAIL("HEY!  ICSSourceModule::GetSourceData(TRUE) called while outstanding data ref held by same thread!  BAD!");
                if (m_spSourceText != NULL) {
                    m_spSourceText->ReportExceptionalOperation();
                }

                return E_FAIL;
            }
            fCheckThisThread = FALSE;
        }

        // Okay, we know two things:  There is at least one outstanding data
        // ref, and we don't have it.  We need to unlock, wait until the data ref
        // count goes to zero, and lock again.                                           
        m_StateLock.Release();
        BOOL    fFirst = TRUE;
        while (m_iDataRef > 0)
        {
            if (fFirst)
            {
                Sleep (0);
                fFirst = FALSE;
            }
            else
            {
                Snooze ();
            }
        }
        m_StateLock.Acquire();
        fNewest = TRUE;
    }

    HRESULT     hr = CSourceData::CreateInstance (this, ppData);
    return SUCCEEDED (hr) && !fNewest ? S_FALSE : hr;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::GetSourceText

STDMETHODIMP CSourceModule::GetSourceText (ICSSourceText **ppSrcText)
{
    (*ppSrcText) = m_spSourceText;
    (*ppSrcText)->AddRef();
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::Flush

STDMETHODIMP CSourceModule::Flush ()
{
    CTinyGate   gate (&m_StateLock);

    if (m_iDataRef > 0)
    {
        VSFAIL ("Can't flush source module with outstanding data refs!");
        return E_FAIL;
    }

    InternalFlush ();
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::InternalFlush

void CSourceModule::InternalFlush ()
{
    ASSERT (m_StateLock.LockedByMe());

    CSourceModuleBase::InternalFlush();

    for (int i = 0; i < m_rgChecksum.Count(); i++)
    {
        m_pStdHeap->Free(m_rgChecksum[i].pbChecksumData);
        m_rgChecksum[i].pbChecksumData = NULL;
        m_rgChecksum[i].cbChecksumData = 0;
        delete m_rgChecksum[i].pErrlocChecksum;
        m_rgChecksum[i].pErrlocChecksum = NULL;
    }

    m_rgChecksum.ClearAll();

    m_pActiveTopLevelHeap->FreeHeap();
    m_pActiveHeap = m_pActiveTopLevelHeap;
    m_pActiveTopLevelHeap->Mark(m_pMarkInterior);
    m_pActiveTopLevelHeap->Mark(m_pMarkTokens);
    if (m_pHeapForFirstTokenization == m_pActiveTopLevelHeap) {
        m_pHeapForFirstTokenization = NULL;
    }
    m_pTreeTop = NULL;
    m_fParsedTop = FALSE;
    m_fTokenized = FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::DisconnectTextEvents

STDMETHODIMP CSourceModule::DisconnectTextEvents ()
{
    VSFAIL ("not implemented");
    return E_NOTIMPL;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::AdviseChangeEvents

STDMETHODIMP CSourceModule::AdviseChangeEvents (ICSSourceModuleEvents *pSink, DWORD_PTR *pdwCookie)
{
    VSFAIL ("not implemented");
    return E_NOTIMPL;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::UnadviseChangeEvents

STDMETHODIMP CSourceModule::UnadviseChangeEvents (DWORD_PTR dwCookie)
{
    VSFAIL ("not implemented");
    return E_NOTIMPL;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::GetParseDiffQ

STDMETHODIMP CSourceModule::GetParseDiffQ(EVENTNODE **ppEventQ)
{
    VSFAIL ("not implemented");
    return E_NOTIMPL;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::GetMemoryConsumption

STDMETHODIMP CSourceModule::GetMemoryConsumption (long *piTopTree, long *piInteriorTrees, long *piInteriorNodes)
{
    // Calculate bytes consumed by top-level parse tree (assumes no interior tree exists)
    *piTopTree = (long)m_heap1.CalcCommittedSize() + (long)m_heap2.CalcCommittedSize();

    // Return size consumed by interior nodes (assumes ParseForErrors() was called first)
    *piInteriorTrees = (long)m_dwInteriorSize;

    // Return number of interior trees parsed
    *piInteriorNodes = m_iInteriorTrees;
    return S_OK;
}

HRESULT CSourceModuleBase::OnRename(NAME * pNewName)
{
    CTinyGate gate(&m_StateLock);
    if (!m_pTreeTop) return S_OK;

    {
        WriteToggler allowWrite(ProtectedEntityFlags::ParseTree, m_pTreeTop->GetRootNamespace()->pNameSourceFile);
        m_pTreeTop->GetRootNamespace()->pNameSourceFile = pNewName;
    }

    return S_OK;
}


void CSourceModule::ParseSourceModuleWorker()
{
    NRHeapWriteAllower allowWrite(m_pActiveTopLevelHeap);

    // Must build parse tree
    m_EventSource.FireOnBeginParse (NULL);
    m_pTreeTop = m_pParser->ParseSourceModule ();
    m_EventSource.FireOnEndParse (NULL);

    // Mark the allocator -- back to here is where we rewind when
    // parsing a new interior node
    m_pActiveTopLevelHeap->Mark(m_pMarkInterior);
}

void CSourceModule::UpdateTokenIndexesWrapper(NRHEAP * pLeafHeap, BASENODE *pNode, BASENODE *pChild)
{
    NRHeapWriteMaker makeWriteable(pLeafHeap);
    UpdateTokenIndexes (pNode, pChild);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ParseTopLevel

HRESULT CSourceModule::ParseTopLevel (CSourceData *pData, BASENODE **ppNode, BOOL fCreateParseDiffs)
{
    HRESULT hr = S_OK;

    if (m_fCrashed)
        return E_FAIL;

    // Make sure the token stream is up-to-date...
    if (VSFSWITCH (gfUnhandledExceptions))
    {
        CreateTokenStream (pData);
    }
    else
    {
        PAL_TRY
        {
            CreateTokenStream (pData);
        }
        PAL_EXCEPT_FILTER(ExceptionFilter, this)
        {
            CLEANUP_STACK // handle stack overflow
            m_pController->HandleException (GetExceptionCode());
            VSFAIL ("YIPES!  CRASH DURING TOKENIZATION!");
            ClearTokenizerThread();
            m_fCrashed = TRUE;
            m_fTokenizing = FALSE;
            m_fTokenized = FALSE;
            hr = E_FAIL;
        }
        PAL_ENDTRY

        if (FAILED(hr))
            return hr;
    }

    // Now, check the state to see if we need to do any parsing work.
    BOOL    fLocked = m_StateLock.Acquire();

    // ParseTopLevel cannot be called while state is locked!
    ASSERT (fLocked);

    // Get the source file name, if we can, to pass to the parser.
    PCWSTR szFileName;
    if (FAILED(m_spSourceText->GetName(&szFileName)))
        szFileName = L"";

    if (m_fParsingTop)
    {
        // Someone else is currently parsing the top, so wait until they are done.
        ASSERT (m_dwParserThread != GetCurrentThreadId());
        while (m_fParsingTop)
        {
            m_StateLock.Release();
            Snooze ();
            m_StateLock.Acquire();
        }
    ASSERT (m_fParsedTop);
    }
    
    m_pParser->SetInputData (szFileName, m_lex.pTokens, /*m_lex.pposTokens,*/ m_lex.iTokens, m_lex.pszSource, m_lex.piLines, m_lex.iLines);

    // Check for incremental parsing possibility.  This is only possible if
    // we actually HAVE a parsed tree, and m_fTokensChanged, and the range of
    // changed tokens 1) falls entirely within an interior node, and 2) contains
    // no TID_OPENCURLY or TID_CLOSECURLY tokens.
    if (m_fParsedTop && m_fTokensChanged)
    {
        BOOL        fMustReparse = m_fForceTopParse;// || m_fExtraOpenCurly || m_fExtraCloseCurly;
        BASENODE    *pLeaf = NULL;
        m_fForceTopParse = FALSE;

        ASSERT (m_pTreeTop != NULL);

        if (!fMustReparse)
        {
            // Look at all the tokens from m_iTokDeltaFirst to m_iTokDeltaLast.
            // If there are any TID_*CURLY tokens, we must reparse the top.
            for (long i=m_iTokDeltaFirst; i<= m_iTokDeltaLast; i++)
                if (m_lex.TokenAt(i).Token() == TID_OPENCURLY || m_lex.TokenAt(i).Token() == TID_CLOSECURLY)
                {
                    DBGOUT (("PARSING TOP LEVEL: Token delta contains an open/close curly"));
                    fMustReparse = TRUE;
                    break;
                }
        }
        else
        {
            DBGOUT (("PARSING TOP LEVEL:  Tokenization wasn't properly incremental, or rescanned an open/close curly"));
        }

        if (!fMustReparse)
        {
            // Find the leaf node (WITHOUT parsing interiors) that contains the
            // first token, and see if it is 1) an interior node that 2) also
            // contains the last token.
            pLeaf = m_pParser->FindLeafIgnoringTokenStream(m_lex.TokenAt(m_iTokDeltaFirst), m_pTreeTop, NULL, NULL);
            if (pLeaf != NULL && pLeaf->InGroup (NG_INTERIOR))
            {
                if (pLeaf->kind == NK_ACCESSOR)
                    fMustReparse = (m_iTokDeltaLast >= pLeaf->asACCESSOR()->iClose + m_iTokDelta) || (m_iTokDeltaFirst <= pLeaf->asACCESSOR()->iOpen);
                else
                    fMustReparse = (m_iTokDeltaLast >= pLeaf->asANYMETHOD()->iClose + m_iTokDelta) || (m_iTokDeltaFirst <= pLeaf->asANYMETHOD()->iOpen);

                if (fMustReparse)
                {
                    DBGOUT (("PARSING TOP LEVEL: Token delta begins prior to open or extends beyond close of interior node"));
                }

                MarkInteriorAsParsed(pLeaf, false);
            }
            else
            {
                DBGOUT (("PARSING TOP LEVEL: Token delta begins outside of interior node"));
                fMustReparse = TRUE;
            }
        }

        if (fMustReparse)
        {
            m_pActiveTopLevelHeap->Free (m_pMarkTokens);
            m_pActiveHeap = m_pActiveTopLevelHeap;
            m_pTreeTop = NULL;
            m_fParsedTop = FALSE;
        }
        else
        {
            DBGOUT (("KEEPING TOP-LEVEL PARSE TREE!!!!"));

            // The tree is okay -- we might need to update token indexes in the nodes
            // from "here down".
            ASSERT (pLeaf != NULL);
            if (m_iTokDelta != 0)
            {
                // From "here down" means we need to pass a child to UpdateTokenIndexes.
                // Just pass the leaf again -- it doesn't have to be a real child of pLeaf.
                STARTTIMER ("UpdateTokenIndexes");
                NRHEAP * pLeafHeap;
                if (m_heap1.IsAddressInHeap(pLeaf)) {
                    pLeafHeap = &m_heap1;
                } else {
                    ASSERT(m_heap2.IsAddressInHeap(pLeaf));
                    pLeafHeap = &m_heap2;
                }
                UpdateTokenIndexesWrapper(pLeafHeap, pLeaf, pLeaf);
                STOPTIMER ();
            }
        }
    }

    m_fTokensChanged = FALSE;
    m_iTokDelta = 0;
    m_iTokDeltaLast = 0;
    m_iTokDeltaFirst = m_lex.iTokens;

    if (!m_fParsedTop)
    {
        // It's not done, so we need to do it.
        ASSERT (!m_fParsingTop);
        m_fParsingTop = TRUE;
        SetParserThread ();

        PAL_TRY
        {
            m_pActiveHeap = m_pActiveTopLevelHeap;
            if (m_pNodeTable)
            {
                m_pNodeTable->RemoveAll();
                m_pNodeArray->ClearAll();
            }

            CErrorContainer *pNew;
            if (FAILED (hr = CErrorContainer::CreateInstance (EC_TOPLEVELPARSE, 0, &pNew))) {
                VSFAIL ("Out of memory");
                pNew = NULL;
            }

            long    iOldErrorCount = m_pParseErrors->Count();
            m_pParseErrors->Release();
            m_pParseErrors = pNew;
            ASSERT (m_pCurrentErrors == NULL);
            m_pCurrentErrors = m_pParseErrors;

            // Release state lock while we parse in case another thread
            // wants the token stream (for example)
            m_StateLock.Release();

            if (VSFSWITCH (gfUnhandledExceptions))
            {
                ParseSourceModuleWorker();
            }
            else
            {
                PAL_TRY
                {
                    ParseSourceModuleWorker();
                }
                PAL_EXCEPT_FILTER(ExceptionFilter, this)
                {
                    CLEANUP_STACK // handle stack overflow
                    if (GetExceptionCode() == EXCEPTION_STACK_OVERFLOW) {
                        POSDATA pd = m_pParser->CurTokenPos();
                        this->ErrorAtPosition(pd.iLine, pd.iChar, 0, FTL_StackOverflow, m_pParser->GetTokenText(m_pParser->CurTokenIndex()));
                        m_pController->ReportErrorsToHost(m_pCurrentErrors);
                    }
                    else {
                        m_pController->HandleException (GetExceptionCode());
                    }
                    m_fCrashed = TRUE;
                    hr = E_FAIL;
                }
                PAL_ENDTRY
            }

            // If necessary, fire the top-level-parse-errors-changed event before
            // locking the state bits again
            if (iOldErrorCount + m_pCurrentErrors->Count() > 0)
            {
                PAL_TRY
                {
                    m_EventSource.FireReportErrors (m_pParseErrors);
                }
                PAL_EXCEPT_FILTER(ExceptionFilter, this)
                {
                    CLEANUP_STACK // handle stack overflow
                    m_pController->HandleException (GetExceptionCode());
                    hr = E_FAIL;
                }
                PAL_ENDTRY
            }

            // We're done parsing the top level, so reflect this in the state
            fLocked = m_StateLock.Acquire();
            ASSERT (fLocked);
            m_pCurrentErrors = NULL;
        }
        PAL_FINALLY
        {
            ClearParserThread ();
            m_fParsingTop = FALSE;
            m_fParsedTop = SUCCEEDED (hr);
        }
        PAL_ENDTRY
    }

    // We're done with the state lock
    m_StateLock.Release();


    *ppNode = m_pTreeTop;

    return (*ppNode == NULL) ? E_FAIL : S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::GetErrors

HRESULT CSourceModule::GetErrors (CSourceData *pData, ERRORCATEGORY iCategory, ICSErrorContainer **ppErrors)
{
    HRESULT hr;
    BASENODE *pTopNode;
    if (FAILED (hr = ParseTopLevel (pData, &pTopNode)))
    {
        return hr;
    }

    CTinyGate   gate (&m_StateLock);

    if (iCategory == EC_TOKENIZATION)
    {
        *ppErrors = m_pTokenErrors;
    }
    else if (iCategory == EC_TOPLEVELPARSE)
    {
        *ppErrors = m_pParseErrors;
    }
    else
    {
        return E_INVALIDARG;
    }

    (*ppErrors)->AddRef();
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::UpdateTokenIndex

void CSourceModule::UpdateTokenIndex (long &iToken)
{
    // Assert that it's right not to check for "-1" token index
    //
    // By definition, m_iTokDelta is always <= m_iTokDeltaLast:
    // . In the case we're adding tokens, the opposite would mean we're adding tokens
    //   before the start of the token stream (e.g. we're adding 10 tokens, and the
    //   last affected token index in the new stream is < 10)
    // . In the case we're removing tokens, the token delta is negative, so there is
    //   no way it can be greater the last updated token index (an array index is 
    //   always >= 0!)
    ASSERT(m_iTokDelta <= m_iTokDeltaLast);
    VSIMPLIES(iToken == -1, iToken + m_iTokDelta <= m_iTokDeltaLast, 
        "Oops. We missed something obvious here");

    if (iToken + m_iTokDelta > m_iTokDeltaLast) 
    { 
        iToken += m_iTokDelta; 
    } 
    else 
    { 
        ASSERT (iToken < m_iTokDeltaFirst); 
    } 
    
    ASSERT(iToken == -1 || (iToken >= 0 && iToken < m_lex.TokenCount()));
}


////////////////////////////////////////////////////////////////////////////////
// CSourceModule::UpdateTokenIndexes
//
// NOTE:  To update the token positions of JUST pNode, do NOT pass a child
// pointer.  Otherwise, this update only subnodes of pNode that are "beyond"
// pChild, and also any such nodes in pNode's parent.

#define UPDTOK(i) UpdateTokenIndex(i)

void CSourceModule::UpdateTokenIndexes (BASENODE *pNode, BASENODE *pChild)
{
    if (pNode == NULL)
        return;

    NRHEAP * owningHeap = NULL;

    switch (pNode->kind)
    {
        case NK_METHOD:
        case NK_CTOR:
        case NK_DTOR:
        case NK_OPERATOR:
            if (pNode->asANYMETHOD()->pInteriorNode) {
                owningHeap = pNode->asANYMETHOD()->pInteriorNode->GetAllocationHeap();
            }
            break;
        case NK_ACCESSOR:
            if (pNode->asACCESSOR()->pInteriorNode) {
                owningHeap = pNode->asACCESSOR()->pInteriorNode->GetAllocationHeap();
            }
        default:
            break;
    }

    NRHeapWriteMaker makeWriteable(owningHeap);

    switch (pNode->kind)
    {
    case NK_NAME:
        if (pNode->flags & NF_NAME_MISSING)
        {
            // The token index of the missing name node is one prior to where it
            // was supposed to be.  Thus, it may be == m_iTokDeltaFirst.
            if (pNode->tokidx + m_iTokDelta > m_iTokDeltaLast)
                pNode->tokidx += m_iTokDelta;
            else 
                ASSERT(pNode->tokidx <= m_iTokDeltaFirst); 
            ASSERT(pNode->tokidx == -1 || (pNode->tokidx >= 0 && pNode->tokidx < m_lex.TokenCount()));
        }
        else
            UPDTOK(pNode->tokidx);
        break;

    case NK_LIST:
        {
            BINOPNODE   *pOp = pNode->asLIST();
            if (pChild == NULL)
            {
                // We're going back down the tree.  Update all of our children.
                CListIterator list(pNode->asLIST());
                BASENODE *pNodeCur;
                while (NULL != (pNodeCur = list.Next()))
                {
                    UpdateTokenIndexes(pNodeCur, NULL);
                }
            }
            else
            {
                // We're going up the tree.
                // We don't want to call on P1, since we know that it must come before P2 in the token stream,
                // so it's effectively a no-op.
                // We'll call on p2, if we didn't just come from it.
                if (pChild != pOp->p2)
                {
                    ASSERT(pChild == pOp->p1);
                    UpdateTokenIndexes (pOp->p2, NULL);
                }
            }
        }
        UPDTOK(pNode->tokidx);
        break;

    case NK_CLASS:
    case NK_ENUM:
    case NK_INTERFACE:
    case NK_STRUCT:
        // FIRST_NODEs must be handled directly.
        {
            AGGREGATENODE   *pClass = pNode->asAGGREGATE();

            MEMBERNODE  *pMbr = pClass->pMembers;

            if (pChild != NULL)
            {
                // Run the list of members to see if we find pChild
                for ( ; pMbr != NULL && pMbr != pChild; pMbr = pMbr->pNext)
                    ;

                // If we didn't find it, we need to update them all.  If we did, we DON'T want to
                // update it again.
                if (pMbr == NULL)
                    pMbr = pClass->pMembers;
                else
                    pMbr = pMbr->pNext;
            }

            for ( ; pMbr != NULL; pMbr = pMbr->pNext)
                UpdateTokenIndexes (pMbr, NULL);
        }
        goto LChildren;

    case NK_BLOCK:
        // FIRST_NODEs must be handled directly.
        {
            BLOCKNODE * nodeBlock = pNode->asBLOCK();
            STATEMENTNODE * nodeStmt = nodeBlock->pStatements;

            if (pChild)
            {
                // Run the list of members to see if we find pChild
                for (nodeStmt = nodeBlock->pStatements; nodeStmt && nodeStmt != pChild; nodeStmt = nodeStmt->pNext)
                    ;

                // If we didn't find it, we need to update them all.  If we did, we DON'T want to
                // update it again.
                if (!nodeStmt)
                    nodeStmt = nodeBlock->pStatements;
                else
                    nodeStmt = nodeStmt->pNext;
            }

            for ( ; nodeStmt; nodeStmt = nodeStmt->pNext)
                UpdateTokenIndexes (nodeStmt, NULL);
        }
        goto LChildren;

    default:
LChildren:
        for (AllChildNodeIterator children(pNode); children.MoveNext(); )
        {
            BASENODE *pCurrent = children.Current();
            if (pChild != pCurrent)
                UpdateTokenIndexes(pCurrent, NULL);
        }

        for (IndexNodeIterator indexes(pNode); indexes.MoveNext(); )
        {
            UPDTOK(indexes.Current());
        }
        UPDTOK(pNode->tokidx);
        break;
    }

    // If we were given a child, update our parent
    if (pChild != NULL)
        UpdateTokenIndexes (pNode->pParent, pNode);

}


////////////////////////////////////////////////////////////////////////////////
// CSourceModule::GetInteriorParseTree

HRESULT CSourceModule::GetInteriorParseTree (CSourceData *pData, BASENODE *pNode, ICSInteriorTree **ppTree)
{
    HRESULT     hr = S_OK;
    BOOL        fLocked = m_StateLock.Acquire();

    ASSERT (fLocked);       // Can't call this when the state is locked!

    // Can't parse interior node without having already done the top level!
    if (!m_fParsedTop)
    {
        if (fLocked)
            m_StateLock.Release();
        return E_INVALIDARG;
    }

    // We're done with the state lock for now
    if (fLocked)
        m_StateLock.Release();

    if (VSFSWITCH (gfUnhandledExceptions))
    {
        // Create an interior tree object.  It will call back into us to do
        // the parse (if necessary; or grab the existing tree node if already
        // there).
        hr = CSourceModuleBase::GetInteriorParseTree(pData, pNode, ppTree);
    }
    else
    {
        PAL_TRY
        {
            // Create an interior tree object.  It will call back into us to do
            // the parse (if necessary; or grab the existing tree node if already
            // there).
            hr = CSourceModuleBase::GetInteriorParseTree(pData, pNode, ppTree);
        }
        PAL_EXCEPT_FILTER(ExceptionFilter, this)
        {
            CLEANUP_STACK // handle stack overflow
            m_pController->HandleException (GetExceptionCode());
            hr = E_FAIL;
        }
        PAL_ENDTRY
    }

    return hr;
}


void CSourceModule::GetInteriorNodeWorker(CInteriorNode ** ppStoredNode, BASENODE * pNode)
{
   // No, so we need to create one.  Mark ourselves as parsing an
    // interior node in the state bits
    m_fParsingInterior = TRUE;
    SetInteriorParserThread();

    // If no interior node is currently using the built-in heap, we'll
    // use that.  Otherwise, we need the new interior node to use its own heap.
    if (m_fInteriorHeapBusy)
    {
        CSecondaryInteriorNode  *pIntNode = CSecondaryInteriorNode::CreateInstance (this, m_pController, pNode);
        {
            WriteToggler allowWrite(ProtectedEntityFlags::ParseTree, *ppStoredNode);
            *ppStoredNode = pIntNode;
        }
        m_pActiveHeap = pIntNode->GetAllocationHeap();
        DBGOUT (("SECONDARY INTERIOR NODE CREATED!"));
    }
    else
    {
        {
            CPrimaryInteriorNode * pIntNode = CPrimaryInteriorNode::CreateInstance (this, pNode);
            WriteToggler allowWrite(ProtectedEntityFlags::ParseTree, *ppStoredNode);
            *ppStoredNode = pIntNode;
        }
        m_fInteriorHeapBusy = TRUE;
        m_pActiveHeap = m_pActiveTopLevelHeap;
        m_pActiveTopLevelHeap->Free(m_pMarkInterior);
    }
}

void CSourceModule::ParseInteriorWorker(BASENODE * pNode)
{
       m_EventSource.FireOnBeginParse (pNode);
        ParseInteriorNode (pNode);
        m_EventSource.FireOnEndParse (pNode);
        if (m_fParsingForErrors) {
            MarkInteriorAsParsed(pNode, true);
        }
 }

void CSourceModule::MarkInteriorAsParsed(BASENODE * pNode, bool markAsParsed)
{
    WriteToggler allowWrite(ProtectedEntityFlags::ParseTree, *pNode);
    if (markAsParsed) {
        pNode->other |= NFEX_INTERIOR_PARSED;
    } else {
        pNode->other &= ~NFEX_INTERIOR_PARSED;
    }

}
////////////////////////////////////////////////////////////////////////////////
// CSourceModule::GetInteriorNode

HRESULT CSourceModule::GetInteriorNode (CSourceData *pData, BASENODE *pNode, CInteriorNode **ppNode)
{
    if (pNode == NULL) {
        return E_INVALIDARG;
    }

    BOOL    fLocked;

    // Re-obtain the state lock
    fLocked = m_StateLock.Acquire ();

    ASSERT (fLocked);       // Can't call this while state lock is held
    ASSERT (m_fParsedTop);  // Must have already parsed the top (still)

    // Wait until no other thread is parsing an interior node
    while (m_fParsingInterior)
    {
        ASSERT (m_dwInteriorParserThread != GetCurrentThreadId());
        m_StateLock.Release();
        Sleep (1);
        m_StateLock.Acquire();
    }

    CInteriorNode   **ppStoredNode = GetInteriorNodeAddress(pNode);
    if (ppStoredNode == NULL)
    {
        VSFAIL ("Bogus node type passed to CSourceModule::GetInteriorNode()!");
        m_StateLock.Release();
        return E_FAIL;
    }


    HRESULT     hr = S_OK;

    // Has this node been parsed already?  (If so, it will have an interior
    // node object...)
    if (*ppStoredNode != NULL)
    {
        // The node has been parsed.  Before releasing the state lock, addref the
        // interior node to protect it from final release below...
        *ppNode = *ppStoredNode;
        (*ppNode)->AddRef();

        // Check to see if it has been marked as being parsed for errors.
        // If not, then we need to fire the event now (obviously the node has been
        // parsed, it just hasn't had errors reported yet.
        if ((pNode->other & NFEX_INTERIOR_PARSED) == 0)
        {
            // Mark the node as being parsed first, while the state is still 
            // locked.
            MarkInteriorAsParsed(pNode, true);

            // Release the state lock before reporting the errors.   The errors
            // are safe because the interior node already has our ref.
            m_StateLock.Release();
            m_EventSource.FireReportErrors ((*ppStoredNode)->GetErrorContainer());
        }
        else
        {
            m_StateLock.Release();
        }

        return S_OK;
    }

    GetInteriorNodeWorker(ppStoredNode, pNode);

    DWORD   dwSizeBase = 0;
    if (m_fAccumulateSize)
        dwSizeBase = (DWORD)m_pActiveTopLevelHeap->CalcCommittedSize();

    // Have the interior node create an error container for itself
    ASSERT (m_pCurrentErrors == NULL);
    m_pCurrentErrors = (*ppStoredNode)->CreateErrorContainer();

    // Do the interior parse, releasing the state lock first
    m_StateLock.Release();
    if (VSFSWITCH (gfUnhandledExceptions))
    {
        ParseInteriorWorker(pNode);
    }
    else
    {
        PAL_TRY
        {
            ParseInteriorWorker(pNode);
        }
        PAL_EXCEPT_FILTER(ExceptionFilter, this)
        {
            CLEANUP_STACK // handle stack overflow
            m_pController->HandleException (GetExceptionCode());
            hr = E_FAIL;
        }
        PAL_ENDTRY
    }

    // Fire error reporting event before locking state bits.  Note that
    // we fire this event regardless of whether there were errors or not.
    // The host uses this event to clear other errors in cases of
    // incremental parsing.
    if (m_fParsingForErrors)
        m_EventSource.FireReportErrors (m_pCurrentErrors);

    m_StateLock.Acquire();
    m_pCurrentErrors = NULL;
    m_pActiveHeap = NULL;

    m_fParsingInterior = FALSE;
    ClearInteriorParserThread();

    if (m_fAccumulateSize)
    {
        m_dwInteriorSize += (DWORD)(m_pActiveTopLevelHeap->CalcCommittedSize() - dwSizeBase);
        m_iInteriorTrees++;
    }

    // NOTE:  The state lock is still acquired at this point.  The state lock
    // is used for serialization to the interior nodes' ref counts.
    *ppNode = *ppStoredNode;
    (*ppNode)->AddRef();
    m_StateLock.Release();
    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ParseForErrors

HRESULT CSourceModule::ParseForErrors (CSourceData *pData)
{
    HRESULT     hr;
    BASENODE    *pTopNode;

    STARTTIMER ("parse for errors");

    m_fParsingForErrors = TRUE;

    // First, parse the top level.
    if (FAILED (hr = ParseTopLevel (pData, &pTopNode)))
    {
        STOPTIMER();
        m_fParsingForErrors = FALSE;
        return hr;
    }

    // Now, iterate through the tree and parse all interior nodes.


    m_dwInteriorSize = 0;
    m_iInteriorTrees = 0;

    hr = ParseInteriorsForErrors (pData, pTopNode);


    STOPTIMER();
    m_fParsingForErrors = FALSE;
    return hr;
}



////////////////////////////////////////////////////////////////////////////////
// CSourceModule::FindLeafNodeEx

HRESULT CSourceModule::FindLeafNodeEx (CSourceData *pData, const POSDATA pos, ExtentFlags flags, BASENODE **ppNode, ICSInteriorTree **ppTree)
{
    HRESULT     hr = E_FAIL;

    if (m_fLeafCrashed)
        return E_FAIL;

    if (VSFSWITCH (gfUnhandledExceptions))
    {
        hr = CSourceModuleBase::FindLeafNodeEx(pData, pos, flags, ppNode, ppTree);
    }
    else
    {
        PAL_TRY
        {
            hr = CSourceModuleBase::FindLeafNodeEx(pData, pos, flags, ppNode, ppTree);
        }
        PAL_EXCEPT_FILTER(ExceptionFilter, this)
        {
            CLEANUP_STACK // handle stack overflow
            m_pController->HandleException (GetExceptionCode());
            m_fLeafCrashed = TRUE;
            hr = E_FAIL;
        }
        PAL_ENDTRY
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::IsUpToDate

HRESULT CSourceModule::IsUpToDate (BOOL *pfTokenized, BOOL *pfTopParsed)
{
    HRESULT     hr = ((!m_fTextModified) && m_fTokenized) ? S_OK : S_FALSE;

    if (pfTokenized != NULL)
        *pfTokenized = m_fTokenized;

    if (pfTopParsed != NULL)
        *pfTopParsed = m_fParsedTop;

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::UnsafeGetLexData
HRESULT CSourceModule::UnsafeGetLexData(LEXDATA* pLexData)
{
    return GetLexResults(NULL, pLexData);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::GetLexResults

HRESULT CSourceModule::GetLexResults (CSourceData *pData, LEXDATA *pLexData)
{
    HRESULT     hr = E_FAIL;

    if (VSFSWITCH (gfUnhandledExceptions))
    {
        CreateTokenStream (pData);
        *pLexData = m_lex;
        hr = S_OK;
    }
    else
    {
        PAL_TRY
        {
            CreateTokenStream (pData);
            *pLexData = m_lex;
            hr = S_OK;
        }
        PAL_EXCEPT_FILTER(ExceptionFilter, this)
        {
            CLEANUP_STACK // handle stack overflow
            m_pController->HandleException (GetExceptionCode());
            hr = E_FAIL;
        }
        PAL_ENDTRY
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::GetSingleTokenPos

HRESULT CSourceModule::GetSingleTokenPos (CSourceData *pData, long iToken, POSDATA *pposToken)
{
    CreateTokenStream (pData);
    return CSourceModuleBase::GetSingleTokenPos (pData, iToken, pposToken);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::GetSingleTokenData

HRESULT CSourceModule::GetSingleTokenData (CSourceData *pData, long iToken, TOKENDATA *pTokenData)
{
    CreateTokenStream (pData);
    return CSourceModuleBase::GetSingleTokenData (pData, iToken, pTokenData);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::IsInsideComment

HRESULT CSourceModule::IsInsideComment (CSourceData *pData, const POSDATA &pos, BOOL *pfInComment)
{ 
    HRESULT     hr = S_OK;

    if (VSFSWITCH (gfUnhandledExceptions))
    {
        CreateTokenStream(pData);
        hr = CSourceModuleBase::IsInsideComment (pData, pos, pfInComment);
    }
    else
    {
        PAL_TRY
        {
            CreateTokenStream(pData);
            hr = CSourceModuleBase::IsInsideComment (pData, pos, pfInComment);
        }
        PAL_EXCEPT_FILTER(ExceptionFilter, this)
        {
            CLEANUP_STACK // handle stack overflow
            m_pController->HandleException (GetExceptionCode());
            hr = E_FAIL;
        }
        PAL_ENDTRY
    }

    return hr;
}



////////////////////////////////////////////////////////////////////////////////
// CSourceModule::GetLastRenamedTokenIndex

HRESULT CSourceModule::GetLastRenamedTokenIndex(long *piTokIdx, NAME **ppPreviousName)
{
    CTinyGate gate(&m_StateLock);
    *piTokIdx       = m_iLastTokenRenamed;
    *ppPreviousName = m_pPreviousTokenName;
    return S_OK;
}


HRESULT CSourceModule::ResetRenamedTokenData()
{
    CTinyGate gate(&m_StateLock);
    m_iLastTokenRenamed = -1;
    m_pPreviousTokenName = NULL;
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::CreateNewError
//
// This is the error/warning creation function.  All forms of errors and warnings
// from the lexer and parser come through here.  Also, all forms of errors and
// warnings from the lexer/parser have exactly one location, so it is provided
// here.

BOOL CSourceModule::CreateNewError (int iErrorId, int carg, ErrArg * prgarg, const POSDATA &posStart, const POSDATA &posEnd)
{
    // Since we're going to use the error container, we need to grab the state
    // lock for this.
    CTinyGate gate(&m_StateLock);

    // We may not have an error container -- if not, then we ignore errors.
    if (m_pCurrentErrors == NULL)
        return FALSE;

    CComPtr<ICSError> spICSError;
    CError    * pCError = NULL;
    HRESULT     hr = S_OK;

    // Create an arg array manually using the type information in the ErrArgs.
    PCWSTR * prgpsz = STACK_ALLOC(PCWSTR, carg);
    PCWSTR * ppsz = prgpsz;

    for (ErrArg * parg = prgarg; parg < prgarg + carg; parg++) {
        ConvertAndCleanArgToStack(parg, ppsz);
        ppsz++;
    }

    // Create the error object
    if (FAILED (hr = m_pController->CreateError(iErrorId, prgpsz, &pCError)))
        return FALSE;     // Hmm...
    
    // The error is created with a zero reference count.
    // This addrefs it and does the correct error count handling.
    if (pCError) {
        hr = (static_cast<IUnknown *>(pCError))->QueryInterface (IID_ICSError, (void **)&spICSError);
    }
    ASSERT (SUCCEEDED(hr));

    // This may not have created anything (for example, if the error was a
    // warning that doesn't meet the warning level criteria...)
    if (hr == S_FALSE)
        return FALSE;

    ASSERT (pCError != NULL);

    PCWSTR  pszFileName;
    PCWSTR  pszMapFileName;
    POSDATA mapStart(posStart), mapEnd(posEnd);

    MapLocation (&mapStart, &pszMapFileName, NULL, NULL);
    MapLocation (&mapEnd, (NAME**)NULL, NULL, NULL);

    if (FAILED (hr = m_spSourceText->GetName (&pszFileName)) ||
        FAILED (hr = pCError->AddLocation (pszFileName, &posStart, &posEnd, pszMapFileName, &mapStart, &mapEnd)))
    {
    }
    
    if (IsWarningDisabled(pCError))
        return FALSE;

    m_iErrors++;

    if (m_pController->FErrorsSuppressed() && pCError->Kind() != ERROR_FATAL)
        return FALSE;

    // Now, add this error to the current error container.
    m_pCurrentErrors->AddError (pCError);
    return (pCError->Kind() != ERROR_WARNING);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::ErrorAtPositionArgs
//
// Only called by lexer or preprocessor

void CSourceModule::ErrorAtPositionArgs(long iLine, long iCol, long iExtent, int id, int carg, ErrArg * prgarg)
{
    POSDATA pos(iLine, iCol);
    POSDATA posEnd(iLine, iCol + max (iExtent, 1));

#ifdef DEBUG
    if (IsWarningID(id)) {
        WORD num = ErrorNumberFromID(id);
        for (int i = 0; lexerWarnNumbers[i] != 0; i++) {
            if (lexerWarnNumbers[i] == num)
                goto FOUND;
        }
        VSASSERT(!"Lexer warning not in allowed list", "If you get this assert, you need to update lexerWarnNumbers in srcmod.cpp to include this warning number");
FOUND:;
    }
#endif

    CreateNewError(id, carg, prgarg, pos, posEnd);
}

void CSourceModule::ErrorAtPosition(LONG_PTR iLine, LONG_PTR iCol, LONG_PTR iExtent, int id, ErrArg a)
{
    ErrorAtPositionArgs((long)iLine, (long)iCol, (long)iExtent, id, 1, &a);
}

void *CSourceModule::AllocFromActiveHeap(size_t iSize)
{
    return m_pActiveTopLevelHeap->Alloc (iSize);
}





////////////////////////////////////////////////////////////////////////////////
// CSourceModule::GetICSSourceModule

HRESULT CSourceModule::GetICSSourceModule (ICSSourceModule **ppModule)
{
    (*ppModule) = this;
    (*ppModule)->AddRef();
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::Clone

HRESULT CSourceModule::Clone (ICSSourceData *pData, CSourceModuleBase **ppClone)
{
    VSFAIL ("not implemented");
    return E_NOTIMPL;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::GetMemoryConsumptionPrivate

HRESULT CSourceModule::GetMemoryConsumptionPrivate(long *piTopTree, long *piInteriorTrees, long *piInteriorNodes)
{
    return GetMemoryConsumption(piTopTree, piInteriorTrees, piInteriorNodes);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::CloneSourceText

void CSourceModule::CloneSourceText (ICSSourceText **ppClone)
{
    if (FAILED (m_spSourceText->CloneInMemory (ppClone)))
        m_pController->NoMemory();
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModule::CloneLexData

void CSourceModule::CloneLexData(MEMHEAP *pStdHeap, PCWSTR pszText, LEXDATABLOCK &lex)
{
    m_lex.CloneTo(m_pController, pStdHeap, pszText, lex);
}



////////////////////////////////////////////////////////////////////////////////
// ExceptionFilter
LONG CSourceModule::ExceptionFilter (EXCEPTION_POINTERS *pExceptionInfo, PVOID pv)
{
    CSourceModule* pThis = (CSourceModule *)pv;

    WatsonOperationKindEnum howToReportWatsons = WatsonOperationKind::Queue;
    WCHAR bugreport[MAX_PATH];
    bugreport[0] = L'\0';
    if (pThis->m_pController)
    {
        pThis->m_pController->SetExceptionData (pExceptionInfo);
        howToReportWatsons = pThis->m_pController->GetWatsonFlags();
        CComPtr<ICSCommandLineCompilerHost> cmdHost;
        if (SUCCEEDED(pThis->m_pController->GetHost()->QueryInterface(IID_ICSCommandLineCompilerHost, (void**)&cmdHost)) && cmdHost) {
            if (FAILED(cmdHost->GetBugReportFileName(bugreport, lengthof(bugreport))))
                bugreport[0] = L'\0';
        }
    }

    return EXCEPTION_EXECUTE_HANDLER;
}



////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::CSourceModuleBase

CSourceModuleBase::CSourceModuleBase(CController *pController) :
    m_iRef(0),
    m_pController(pController),
    m_pParser(NULL),
    m_LineMap(pController->GetStandardHeap()),
    m_tableDefines(pController),
    m_pTreeTop(NULL),
    m_pNodeTable(NULL),
    m_pNodeArray(NULL)
{
    m_pStdHeap = pController->GetStandardHeap ();

    ZeroMemory (&m_lex, sizeof (m_lex));

    // Must ref the controller!
    pController->AddRef();
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::~CSourceModuleBase

CSourceModuleBase::~CSourceModuleBase()
{
    InternalFlush();

    m_spSourceText.Release();

    if (m_lex.piTransitionLines != NULL)
        VSFree (m_lex.piTransitionLines);

    if (m_lex.pCCStates != NULL)
        VSFree (m_lex.pCCStates);

    if (m_lex.pIdentTable)
        delete m_lex.pIdentTable;

    if (m_lex.pWarningMap)
    {
        m_lex.pWarningMap->Clear();
        delete m_lex.pWarningMap;
    }

    m_tableDefines.ClearAll (TRUE);

    if (m_pNodeTable)
        delete m_pNodeTable;

    if (m_pNodeArray)
        delete m_pNodeArray;

    delete m_pParser;

    m_pController->Release();
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::InternalFlush

void CSourceModuleBase::InternalFlush ()
{
    if (m_lex.piLines != NULL)
    {
        m_pStdHeap->Free (m_lex.piLines);
        m_lex.piLines = NULL;
    }

    if (m_lex.pTokens != NULL)
    {
        for (long i=0; i<m_lex.iTokens; i++)
            if (m_lex.pTokens[i].HasOverhead() && (m_lex.pTokens[i].iUserBits & TF_HEAPALLOCATED))
                m_pStdHeap->Free (m_lex.pTokens[i].Overhead());

        m_pStdHeap->Free (m_lex.pTokens);
        m_lex.pTokens = NULL;
    }

    if (m_lex.piRegionStart != NULL)
    {
        m_pStdHeap->Free (m_lex.piRegionStart);
        m_lex.piRegionStart = NULL;
    }
    
    if (m_lex.pWarningMap != NULL)
    {
        m_lex.pWarningMap->Clear();
        delete m_lex.pWarningMap;
        m_lex.pWarningMap = NULL;
    }

    m_LineMap.Clear();

    if (m_pNodeTable)
    {
        m_pNodeTable->RemoveAll();
        m_pNodeArray->ClearAll();
    }

    m_lex.iTokens = 0;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::InternalAddRef

ULONG CSourceModuleBase::InternalAddRef ()
{
    return InterlockedIncrement (&m_iRef);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::InternalRelease

ULONG CSourceModuleBase::InternalRelease ()
{
    ULONG   iNew = InterlockedDecrement (&m_iRef);
    if (iNew == 0)
        FinalRelease();

    return iNew;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::FinalRelease

void CSourceModuleBase::FinalRelease()
{
    delete this;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::MapLocation
//
// Does NOT allocate memory for ppszFilename, it's just a pointer
// If no mapping changes the filename, this will use ICSSourceText->GetName()
// to supply the ppszFilename

void CSourceModuleBase::MapLocation (POSDATA *pos, PCWSTR *ppszFilename, bool *pbIsHidden, bool *pbIsMapped)
{
    if (ppszFilename != NULL) 
    {
        NAME * name = NULL;
        m_LineMap.Map(pos, &name, pbIsHidden, pbIsMapped);
        if (name != NULL)
            (*ppszFilename) = name->text;
        else
            (*ppszFilename) = GetFileName();
    }
    else 
    {
        m_LineMap.Map(pos, NULL, pbIsHidden, pbIsMapped);
    }
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::MapLocation
//
// If no previous mapping changes the filename, then pnameFile is
// left unchanged (this is different behaviour from the other MapLocation)

void CSourceModuleBase::MapLocation(long *line, NAME ** pnameFile, bool *pbIsHidden, bool *pbIsMapped)
{
    *line = m_LineMap.Map(*line, pnameFile, pbIsHidden, pbIsMapped);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::MapLocation
//
// If no previous mapping changes the filename, then pnameFile is
// left unchanged (this is different behaviour from the other MapLocation)

void CSourceModuleBase::MapLocation(POSDATA *pos, NAME ** pnameFile, bool *pbIsHidden, bool *pbIsMapped)
{
    m_LineMap.Map(pos, pnameFile, pbIsHidden, pbIsMapped);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::hasMap

BOOL CSourceModuleBase::hasMap() 
{    
    return !m_LineMap.IsEmpty();
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::GetFileName

PCWSTR CSourceModuleBase::GetFileName()
{
    HRESULT hr;
    PCWSTR pszFileName;
    if (FAILED (hr = m_spSourceText->GetName (&pszFileName)))
        return NULL;
    return pszFileName;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::IsSymbolDefined

BOOL CSourceModuleBase::IsSymbolDefined(PNAME symbol) 
{    
    return m_tableDefines.IsDefined(symbol);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::ParseInteriorNode

void CSourceModuleBase::ParseInteriorNode (BASENODE *pNode)
{
    NRHEAP* activeHeap = GetActiveHeap();
    NRHeapWriteMaker makeWriteable(activeHeap);
    NRHeapWriteAllower allowWrite(activeHeap);

    switch (pNode->kind)
    {
        case NK_METHOD:
        case NK_CTOR:
        case NK_DTOR:
        case NK_OPERATOR:
        {
            METHODBASENODE  *pMethod = pNode->asANYMETHOD();

            m_pParser->Rewind (pMethod->iOpen);
            if (m_pParser->CurToken() == TID_OPENCURLY) {
                BLOCKNODE * temp = m_pParser->ParseBlock (pMethod)->asBLOCK();
                WriteToggler toggler(ProtectedEntityFlags::ParseTree, pMethod->pBody);
                pMethod->pBody = temp;
            }
            break;
        }

        case NK_ACCESSOR:
        {
            ACCESSORNODE    *pAccessor = pNode->asACCESSOR();

            m_pParser->Rewind (pAccessor->iOpen);
            if (m_pParser->CurToken() == TID_OPENCURLY) {
                BLOCKNODE * temp = m_pParser->ParseBlock (pAccessor)->asBLOCK();
                WriteToggler toggler(ProtectedEntityFlags::ParseTree, pAccessor->pBody);
                pAccessor->pBody = temp;
            }
            break;
        }

        default:
        {
            ASSERT(!"Unknown interior node");
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::ParseInteriorsForErrors

HRESULT CSourceModuleBase::ParseInteriorsForErrors (CSourceData *pData, BASENODE *pNode)
{
    if (pNode == NULL)
        return S_OK;

    switch (pNode->kind)
    {
        case NK_NAMESPACE:
            return ParseInteriorsForErrors (pData, pNode->asNAMESPACE()->pElements);

        case NK_LIST:
        {
            CListIterator list(pNode->asLIST());
            BASENODE *pNodeCur;
            while (NULL != (pNodeCur = list.Next()))
            {
                HRESULT     hr;
                if (FAILED (hr = ParseInteriorsForErrors (pData, pNodeCur)))
                    return hr;
            }
            return S_OK;
        }

        case NK_CLASS:
        case NK_STRUCT:
        {
            HRESULT     hr;

            for (MEMBERNODE *p = pNode->asAGGREGATE()->pMembers; p != NULL; p = p->pNext)
            {
                if (p->kind == NK_NESTEDTYPE)
                {
                    if (FAILED (hr = ParseInteriorsForErrors (pData, p->asNESTEDTYPE()->pType)))
                        return hr;
                    continue;
                }

                if (p->kind != NK_CTOR && p->kind != NK_METHOD &&
                    p->kind != NK_PROPERTY && p->kind != NK_OPERATOR && p->kind != NK_INDEXER &&
                    p->kind != NK_DTOR)
                    continue;

                ICSInteriorTree *pTree;

                if (p->kind == NK_PROPERTY || p->kind == NK_INDEXER)
                {
                    // Before getting an interior parse tree, check each node to see if it
                    // has already been parsed for errors (NF_INTERIOR_PARSED)
                    if (p->asANYPROPERTY()->pGet != NULL && (p->asANYPROPERTY()->pGet->other & NFEX_INTERIOR_PARSED) == 0)
                    {
                        if (FAILED (hr = pData->GetInteriorParseTree (p->asANYPROPERTY()->pGet, &pTree)))
                            return hr;
                        pTree->Release();
                    }
                    if (p->asANYPROPERTY()->pSet != NULL && (p->asANYPROPERTY()->pSet->other & NFEX_INTERIOR_PARSED) == 0)
                    {
                        if (FAILED (hr = pData->GetInteriorParseTree (p->asANYPROPERTY()->pSet, &pTree)))
                            return hr;
                        pTree->Release();
                    }
                }
                else if ((p->other & NFEX_INTERIOR_PARSED) == 0)
                {
                    if (FAILED (hr = pData->GetInteriorParseTree (p, &pTree)))
                        return hr;
                    pTree->Release();
                }
            }
            break;
        }

        default:
            break;
    }

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::GetExtent

HRESULT CSourceModuleBase::GetExtent (BASENODE *pNode, POSDATA *pposStart, POSDATA *pposEnd, ExtentFlags flags)
{
    CBasenodeLookupCache cache;
    return m_pParser->GetExtent (cache, pNode, flags, pposStart, pposEnd);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::GetTokenExtent

HRESULT CSourceModuleBase::GetTokenExtent (BASENODE *pNode, long *piFirst, long *piLast)
{
    m_pParser->GetTokenExtent (pNode, piFirst, piLast, EF_FULL);
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::GetDocComment

HRESULT CSourceModuleBase::GetDocComment (BASENODE *pNode, long *piComment, long *piCount)
{
    // In order to have a doc comment, this node must be a type or member.  If it
    // isn't, look up the parent chain until it is.
    while (!pNode->InGroup (NG_TYPE|NG_FIELD|NG_METHOD|NG_PROPERTY) && pNode->kind != NK_ENUMMBR && pNode->kind != NK_VARDECL)
    {
        pNode = pNode->pParent;
        if (pNode == NULL)
            return E_INVALIDARG;
    }

    BASENODE    *pAttr = NULL;

    // Go from VARDECL to parent.
    if (pNode->kind == NK_VARDECL)
    {
        pNode = pNode->asVARDECL()->pParent;
        while (pNode->kind == NK_LIST)
            pNode = pNode->pParent;
    }

    // Get the position of the first token of this thing -- which should include
    // its attribute if it has one
    if (pNode->InGroup (NG_AGGREGATE))
        pAttr = pNode->asAGGREGATE()->pAttr;
    else if (pNode->kind == NK_DELEGATE)
        pAttr = pNode->asDELEGATE()->pAttr;
    else if (pNode->InGroup (NG_FIELD|NG_METHOD|NG_PROPERTY) || pNode->kind == NK_ENUMMBR)
        pAttr = pNode->asANYMEMBER()->pAttr;

    long    iFirstToken, iLastToken;
    BOOL    fMissing, fFound = FALSE;;

    // Find the first token of the node
    iFirstToken = m_pParser->GetFirstToken ((pAttr != NULL) ? pAttr : pNode, EF_FULL, &fMissing);

    // Starting from there, find the first doc comment amonst the noise tokens the precede it.
    for (iLastToken = iFirstToken--; 
        iFirstToken >= 0 && (CParser::GetTokenInfo (m_lex.TokenAt(iFirstToken).Token())->dwFlags & TFF_NOISE); 
        iFirstToken--)
    {
        if (m_lex.TokenAt(iFirstToken).Token() == TID_DOCCOMMENT || 
            m_lex.TokenAt(iFirstToken).Token() == TID_MLDOCCOMMENT)
        {
            fFound = TRUE;
            break;
        }
    }

    // If we found one, continue searching backwards until we find a non-doccomment token.
    if (!fFound)
        return E_FAIL;

    long    iFirstComment = iFirstToken, iLastComment;

    for (iLastComment = iFirstComment--; iFirstComment >= 0; iFirstComment--)
    {
        if (m_lex.TokenAt(iFirstComment).Token() != TID_DOCCOMMENT &&
            m_lex.TokenAt(iFirstComment).Token() != TID_MLDOCCOMMENT)
        {
            break;
        }
    }
    iFirstComment++;
/*
    UNDONE: Everett code ensures there are no intervening non-comment lines
    Whidbey doesn't do that checking here anymore...  See CSourceModule::GetDocCommentText
*/

    // Our doc comment runs from iFirstComment to iLastComment
    *piComment = iFirstComment;
    *piCount = (iLastComment - iFirstComment + 1);
    return S_OK;
}

CInteriorNode **CSourceModuleBase::GetInteriorNodeAddress(BASENODE *pNode)
{
    ASSERT (pNode->InGroup (NG_INTERIOR));

    // Determine the location of the interior node pointer in the given
    // container node.
    switch (pNode->kind)
    {
        case NK_METHOD:
        case NK_CTOR:
        case NK_DTOR:
        case NK_OPERATOR:
            return &(pNode->asANYMETHOD()->pInteriorNode);
            break;
        case NK_ACCESSOR:
            return &(pNode->asACCESSOR()->pInteriorNode);
            break;
        default:
            return NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::GetDocCommentText
//
// Allocates memory from pStdHeap for the heap.  Free it yourself
// or call FreeDocCommentText.  Sets *ppszText = NULL if there is
// no DocComment

HRESULT CSourceModuleBase::GetDocCommentText(BASENODE *pNode, __deref_out PWSTR *ppszText, long *piFirstComment, long * piLastComment, CXMLMap * srcMap, long * piLines, PCWSTR pszIndent)
{
    HRESULT     hr = S_OK;
    long        iStart = 0, iLen = 0, i, iBlockStart, x, iPrevLine;
    size_t      iTextLen = 0;
    int       * iRemoveChars = NULL;
    int         iAddChars = 2; // for the "\r\n"
    LEXDATA   * lexComments = NULL;
    ICSLexer ** lex = NULL;
    int         lineCount;
    WCHAR     * p = NULL;
    size_t      cchLeft;
    size_t      len;

    ASSERT (ppszText != NULL);
    *ppszText = NULL;

    // E_FAIL means no doc comment, other FAILED(hr)'s mean something bad.
    if (FAILED (hr = GetDocComment (pNode, &iStart, &iLen))) 
        return (hr == E_FAIL) ? S_FALSE : hr;

    if (piLastComment)
        *piLastComment = iStart + (iLen - 1);

    if (pszIndent != NULL)
        iAddChars += (int)wcslen(pszIndent);        

    iRemoveChars = (int*)m_pStdHeap->Alloc(sizeof(int) * iLen);
    lexComments = (LEXDATA*)m_pStdHeap->Alloc(sizeof(LEXDATA) * iLen);
    lex = (ICSLexer**)m_pStdHeap->AllocZero(sizeof(ICSLexer*) * iLen);

    // Scan each comment to see if we can remove the first whitespace
    // Work back-wards so we can detect any intervening whitespace
    iBlockStart = x = (iLen - 1);
    iPrevLine = -1;
    for (i = iStart + x; i >= iStart; i--, x--)
    {
        long iEndLine;
        PCWSTR pszLine = m_lex.TokenAt(i).DocLiteral()->szText;
        if (m_lex.TokenAt(i).Token() == TID_MLDOCCOMMENT) {
            if (FAILED(hr = CTextLexer::CreateInstance(GetNameMgr(), GetOptions()->compatMode, &lex[x])))
                goto CLEANUP;
            if (FAILED(hr = lex[x]->SetInput(m_lex.TokenAt(i).DocLiteral()->szText, -1)))
                goto CLEANUP;
            if (FAILED(hr = lex[x]->GetLexResults(&lexComments[x])))
                goto CLEANUP;
            iEndLine = lexComments[x].iLines + m_lex.TokenAt(i).iLine - 1;
        } else {
            ASSERT(m_lex.TokenAt(i).Token() == TID_DOCCOMMENT);
            iEndLine = m_lex.TokenAt(i).iLine;
            memset (&lexComments[x], 0, sizeof(lexComments[x]));
        }

        if (iPrevLine != -1 && 
            iPrevLine != iEndLine && (iPrevLine - 1) != iEndLine) {
            // This doc comment starts on a non-contigous line, so stop here
            i++;
            x++;
            iLen -= i - iStart;
            iStart = i;
            memmove(iRemoveChars, iRemoveChars + x, sizeof(*iRemoveChars) * iLen);
            memmove(lexComments, lexComments + x, sizeof(*lexComments) * iLen);
            memmove(lex, lex + x, sizeof(*lex) * iLen);
            break;
        } else {
            iPrevLine = m_lex.TokenAt(i).iLine;
        }

        if (m_lex.TokenAt(i).Token() == TID_DOCCOMMENT) {
            // Single-line comment
            if (x == iBlockStart || iRemoveChars[x+1] == 4)
            {
                if (IsWhitespace (pszLine[3]))
                    iRemoveChars[x] = 4;
                else
                {
                    // This comment doesn't lead with whitespace
                    // So we can't strip anything from this whole block
                    for (int y = iBlockStart; y >= x; y--) {
                        ASSERT(x == y || iRemoveChars[y] == 4);
                        iTextLen++;
                        iRemoveChars[y] = 3;
                    }
                }
            } else
                iRemoveChars[x] = 3;
            iTextLen += (iAddChars - iRemoveChars[x]) + m_lex.TokenAt(i).Length();
        } else {
            // Multi-line comment
            ASSERT(m_lex.TokenAt(i).Token() == TID_MLDOCCOMMENT && lexComments[x].pszSource != NULL);
            // The next single-line comment is the start of a new 'block' of comments
            iBlockStart = x - 1;

            // If a multi-line doc comment is all on one line, treat it just like a
            // a single line comment (except it does break the block of single-line comments)
            if (lexComments[x].iLines == 1) {
                if (IsWhitespace (pszLine[3]))
                    iRemoveChars[x] = 4;
                else
                    iRemoveChars[x] = 3;
                iTextLen += (iAddChars - iRemoveChars[x]) + m_lex.TokenAt(i).Length();
            } else {
                // Guess how many characters (but make sure it's always bigger
                iTextLen += iAddChars * (lexComments[x].iLines) +
                    wcslen(m_lex.TokenAt(i).DocLiteral()->szText);
                iRemoveChars[x] = 0;
            }
        }
    }

    if (piFirstComment)
        *piFirstComment = iStart;

    iAddChars -= 2; // for the "/r/n"

    // Allocate and copy in the text
    ++iTextLen; // Add one for NULL.
    *ppszText = (PWSTR)m_pStdHeap->Alloc(sizeof(WCHAR) * (iTextLen + 1)); // Add one for assert checking below
    p = *ppszText;
    *p = L'\0';

    cchLeft = iTextLen + 1; // always contains the amount of buffer left after p.
    len = 0;
    x = 0;
    *p = L'\0';
    
    // All the code that copies into *ppszText has been updated to use the "safe string" functions
    // to ensure that a buffer overflow can't occur, even if there is a logic error somewhere and
    // we didn't allocate enough memory after all (e.g., the calculation of iTextLen was somehow
    // flawed.) The equivalent code is commented out immediately above.
    lineCount = 0;
    for (i = iStart; i < (iStart + iLen); i++, x++)
    {
        PCWSTR pszLine = m_lex.TokenAt(i).DocLiteral()->szText;
        m_lex.TokenAt(i).iUserBits |= TF_USEDCOMMENT;
        if (iRemoveChars[x] != 0)
        {
            ASSERT(iRemoveChars[x] == 3 || iRemoveChars[x] == 4);
            len = (long)m_lex.TokenAt(i).Length() - iRemoveChars[x];
            if (pszLine[1] == '*')
                len -= 2; // Don't copy the trailing "*/"

            //wcscpy (p, pszIndent);
            //p += iAddChars;
            if (pszIndent)
                StringCchCopyExW(p, cchLeft, pszIndent, &p, &cchLeft, 0);

            //wcsncpy (p, m_lex.pTokens[i].DocLiteral()->szText + iRemoveChars[x], len);
            //p += len;
            StringCchCopyNExW(p, cchLeft, m_lex.TokenAt(i).DocLiteral()->szText + iRemoveChars[x], len, &p, &cchLeft, 0);
            
            //*p++ = '\r';
            //*p++ = '\n';
            StringCchCopyExW(p, cchLeft, L"\r\n", &p, &cchLeft, 0);

            if (srcMap != NULL) {
                srcMap->AddMap(lineCount, m_lex.TokenAt(i).iLine, m_lex.TokenAt(i).iChar + iRemoveChars[x] - iAddChars);
            }

            ++lineCount;
        }
        else
        {
            PCWSTR pszPattern = NULL;
            long cntPattern = 0;
            bool bSkipFirst = false;
            unsigned long j;
            unsigned long last_line = (lexComments[x].iLines - 1);

            // If the first line has nothing but whitespace, then skip it entirely
            if (lexComments[x].IsLineWhitespaceAfter( 0, 3))
                bSkipFirst = true;

            // If the last line has nothing but whitespace, then skip it entirely
            if (lexComments[x].IsLineWhitespaceBefore(last_line, lexComments[x].GetLineLength(last_line) - 2)) {
                // The whole last line is whitespace, so ignore it (except of course the trailing "*/"
                last_line--;
            }

            // Find the 'pattern' if one exists and then strip it.
            // to find the pattern, find a common "whitespace*whitespace" on
            // every line except the first and maybe the last
            j = 1;
            if (j <= last_line) {
                // setup the 'pattern'
                HRESULT hr = FindDocCommentPattern( &lexComments[x], 1, &cntPattern, &pszPattern);
                if (hr == S_OK) {
                    // found a 'pattern', so match it on each line
                    while (++j <= last_line)
                    {
                        hr = MatchDocCommentPattern( lexComments[x].TextAt(j, 0), pszPattern, &cntPattern);
                        if (hr == E_FAIL) {
                            // No matchable pattern (or sub-pattern)
                            break;
                        }
                    }
                }
            }

            // cntPattern now holds the count of common characters that we can safely
            // strip from all lines except the first

            // Do the first line
            if (!bSkipFirst) {
                pszLine = lexComments[x].TextAt(0, 3);
                if (IsWhitespaceChar(*pszLine))
                    pszLine++;
                len = lexComments[x].TextAt(1,0) - pszLine;
                
                //wcscpy (p, pszIndent);
                //p += iAddChars;
                if (pszIndent)
                    StringCchCopyExW(p, cchLeft, pszIndent, &p, &cchLeft, 0);
                
                //wcsncpy (p, pszLine, len);
                //p += len;
                StringCchCopyNExW(p, cchLeft, pszLine, len, &p, &cchLeft, 0);

                if (srcMap != NULL) {
                    srcMap->AddMap(lineCount, m_lex.TokenAt(i).iLine, (long)(m_lex.TokenAt(i).iChar + (pszLine - lexComments[x].TextAt(0, 0)) - iAddChars));
                }

                ++lineCount;
            }
            
            // Do all the 'middle' lines
            for (j = 1; j < (unsigned long)(lexComments[x].iLines - 1); j++)
            {
                pszLine = lexComments[x].TextAt(j, cntPattern);
                len = lexComments[x].TextAt(j+1, 0) - pszLine;
                
                //wcscpy (p, pszIndent);
                //p += iAddChars;
                if (pszIndent)
                    StringCchCopyExW(p, cchLeft, pszIndent, &p, &cchLeft, 0);
                
                //wcsncpy (p, pszLine, len);
                //p += len;
                StringCchCopyNExW(p, cchLeft, pszLine, len, &p, &cchLeft, 0);

                if (srcMap != NULL) {
                    srcMap->AddMap(lineCount, m_lex.TokenAt(i).iLine + j, cntPattern - iAddChars);
                }

                ++lineCount;
            }

            // Do the last line (same as middle line except how it ends)
            // unless it's all whitespace and can be skipped
            if (last_line == (unsigned)(lexComments[x].iLines - 1)) {
                pszLine = lexComments[x].TextAt(last_line, cntPattern);
                // Remove the '*' and '/' from the length
                len = lexComments[x].GetLineLength(last_line) - (2 + cntPattern);
                
                //wcscpy (p, pszIndent);
                //p += iAddChars;
                if (pszIndent)
                    StringCchCopyExW(p, cchLeft, pszIndent, &p, &cchLeft, 0);
                
                //wcsncpy (p, pszLine, len);
                //p += len;
                StringCchCopyNExW(p, cchLeft, pszLine, len, &p, &cchLeft, 0);
                
                //*p++ = L'\r';
                //*p++ = L'\n';
                StringCchCopyExW(p, cchLeft, L"\r\n", &p, &cchLeft, 0);

                if (srcMap != NULL) {
                    srcMap->AddMap(lineCount, m_lex.TokenAt(i).iLine + j, cntPattern - iAddChars);
                }

                ++lineCount;
            }

        }

    }
    //*p = L'\0';
    ASSERT((p - *ppszText) + cchLeft == iTextLen + 1);  // Make sure that cchLeft was updated correctly throughout.
    ASSERT(cchLeft > 1);
    ASSERT(srcMap == NULL || srcMap->Count() == lineCount);

    if (piLines)
        *piLines = lineCount;

CLEANUP:
    ASSERT(iRemoveChars != NULL);
    m_pStdHeap->Free(iRemoveChars);
    ASSERT(lexComments != NULL);
    m_pStdHeap->Free(lexComments);
    ASSERT(lex != NULL);
    for (x = 0; x < iLen; x++) {
        if (lex[x] != NULL)
            lex[x]->Release();
    }
    m_pStdHeap->Free(lex);

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::FreeDocCommentText
//      Frees memory allocated in GetDocCommentText() from pStdHeap

HRESULT CSourceModuleBase::FreeDocCommentText(__deref_inout PWSTR *ppszText)
{
    ASSERT(ppszText != NULL);
    if (*ppszText != NULL)
        m_pStdHeap->Free(*ppszText);
    *ppszText = NULL;
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::GetSingleTokenData

HRESULT CSourceModuleBase::GetSingleTokenData (CSourceData *pData, long iToken, TOKENDATA *pTokenData) 
{    
    if (iToken < 0 || iToken >= m_lex.TokenCount())
        return E_INVALIDARG;

    CSTOKEN &rToken = m_lex.TokenAt(iToken);
    pTokenData->dwFlags = rToken.iUserBits;
    pTokenData->iLength = rToken.Length();
    pTokenData->iToken = rToken.Token();
    pTokenData->pName = (rToken.Token() == TID_IDENTIFIER) ? rToken.Name() : NULL;
    pTokenData->posTokenStart = rToken;
    pTokenData->posTokenStop = rToken.StopPosition();
    return S_OK;;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::GetSingleTokenPos

HRESULT CSourceModuleBase::GetSingleTokenPos (CSourceData *pData, long iToken, POSDATA *pposToken)
{
    if (iToken < 0 || iToken >= m_lex.TokenCount())
        return E_INVALIDARG;

    (*pposToken) = m_lex.TokenAt(iToken);
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::GetTokenText

HRESULT CSourceModuleBase::GetTokenText (long iTokenId, PCWSTR *ppszText, long *piLen)
{
    if (piLen != NULL)
        *piLen = 0;
    if (ppszText != NULL)
        *ppszText = L"";

    if (iTokenId < 0 || iTokenId >= TID_NUMTOKENS)
        return E_INVALIDARG;

    // If the stored length is 0, it's unknown.  Caller must use GetSingleTokenData
    // on a specific token index to get the desired data
    if (CParser::GetTokenInfo ((TOKENID)iTokenId)->iLen == 0)
        return S_FALSE;

    if (piLen != NULL)
        *piLen = CParser::GetTokenInfo ((TOKENID)iTokenId)->iLen;
    if (ppszText != NULL)
        *ppszText = CParser::GetTokenInfo ((TOKENID)iTokenId)->pszText;

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::GetInteriorParseTree

HRESULT CSourceModuleBase::GetInteriorParseTree (CSourceData *pData, BASENODE *pNode, ICSInteriorTree **ppTree)
{
    // Create an interior tree object.  It will call back into us to do
    // the parse (if necessary; or grab the existing tree node if already
    // there).
    return CInteriorTree::CreateInstance (pData, pNode, ppTree);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::LookupNode

HRESULT CSourceModuleBase::LookupNode (CSourceData *pData, NAME *pKey, long iOrdinal, BASENODE **ppNode, long *piGlobalOrdinal)
{
    HRESULT hr;
    BASENODE *pTopNode;
    if (FAILED (hr = ParseTopLevel(pData, &pTopNode)))
    {
        return hr;
    }

    CTinyGate   gate (&m_StateLock);

    if (m_pNodeTable == NULL)
        return E_FAIL;      // Not available -- must create compiler with CCF_KEEPNODETABLES

    // First, the easier check -- global-orginal value.
    if (pKey == NULL)
    {
        if (iOrdinal < 0 || iOrdinal >= m_pNodeArray->Count())
            return E_INVALIDARG;

        if (piGlobalOrdinal != NULL)
            *piGlobalOrdinal = iOrdinal;        // Well, duh...

        *ppNode = (*m_pNodeArray)[iOrdinal].pNode;
        return S_OK;
    }

    // Okay, we have a key.  Find a node chain in the table.
    NODECHAIN   *pChain = m_pNodeTable->Find (pKey);

    // Pay respect to the key-ordinal
    while (pChain != NULL && iOrdinal--)
    {
        if (pChain != NULL)
            pChain = pChain->pNext;
    }

    if (pChain != NULL)
    {
        *ppNode = (*m_pNodeArray)[pChain->iGlobalOrdinal].pNode;
        if (piGlobalOrdinal != NULL)
            *piGlobalOrdinal = pChain->iGlobalOrdinal;
        return S_OK;
    }

    // Node not found...
    return E_FAIL;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::GetNodeKeyOrdinal

HRESULT CSourceModuleBase::GetNodeKeyOrdinal (CSourceData *pData, BASENODE *pNode, NAME **ppKey, long *piKeyOrdinal)
{
    CTinyGate   gate (&m_StateLock);

    if (m_pNodeTable == NULL)
        return E_FAIL;      // Not available -- must create compiler with CCF_KEEPNODETABLES

    NAME    *pKey;
    HRESULT hr;

    // First, build the node key and look up the node chain in the table
    if (FAILED (hr = pNode->BuildKey (GetNameMgr(), TRUE, true /* we want the <,,,> */, &pKey)))
        return hr;

    long        iKeyOrd = 0;

    // Find the given node in the chain
    NODECHAIN *pChain;
    for (pChain = m_pNodeTable->Find (pKey); pChain != NULL; pChain = pChain->pNext)
    {
        if (pNode == (*m_pNodeArray)[pChain->iGlobalOrdinal].pNode)
            break;
        iKeyOrd++;
    }

    if (pChain == NULL)
        return E_FAIL;      // Huh?  Maybe not a keyed node, but then BuildKey would have failed!

    if (piKeyOrdinal != NULL)
        *piKeyOrdinal = iKeyOrd;
    if (ppKey != NULL)
        *ppKey = pKey;

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::GetGlobalKeyArray

HRESULT CSourceModuleBase::GetGlobalKeyArray (CSourceData *pData, KEYEDNODE *pKeyedNodes, long iSize, long *piCopied)
{
    CTinyGate   gate (&m_StateLock);

    if (m_pNodeTable == NULL)
        return E_FAIL;      // Not available -- must create compiler with CCF_KEEPNODETABLES

    // Caller just wants the size?
    if (pKeyedNodes == NULL)
    {
        if (piCopied != NULL)
            *piCopied = m_pNodeArray->Count();
        return S_OK;
    }

    // Copy over the amount requested, or the total number of nodes, whichever
    // is smaller...
    long    iCopy = min (iSize, m_pNodeArray->Count());

    memcpy (pKeyedNodes, m_pNodeArray->Base(), iCopy * sizeof (KEYEDNODE));
    if (piCopied != NULL)
        *piCopied = iCopy;

    return iCopy == m_pNodeArray->Count() ? S_OK : S_FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::IsInsideComment

HRESULT CSourceModuleBase::IsInsideComment (CSourceData *pData, const POSDATA &pos, BOOL *pfInComment)
{
    *pfInComment = FALSE;

    // Find the nearest token
    long    i = m_lex.FindNearestPosition(pos);

    if (i < 0)
    {
        *pfInComment = FALSE;
        return S_OK;

    }

    CSTOKEN &tk = m_lex.TokenAt(i);
    if (tk == pos || !tk.IsComment())
    {
        // We're either not in the comment token, or it's not a comment token
        *pfInComment = FALSE;
    }
    else
    {
        // It's a comment token, but we may be beyond it.
        if (tk.Token() == TID_MLCOMMENT || tk.Token() == TID_MLDOCCOMMENT)
        {
            *pfInComment = (tk.StopPosition() > pos);
        }
        else
        {
            *pfInComment = (tk.iLine == pos.iLine);
        }
    }

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::FindDocCommentPattern
//
// iLine - Pass in the line to look at
// piChar - Pass out the character index of the '*'
// piPatternCnt - Pass out the length of the pattern
// pszPattern - Pass out the pattern string (assumed to be cchPattern buffer size)
// Return S_OK if a pattern was found, or S_FALSE if no pattern

HRESULT CSourceModuleBase::FindDocCommentPattern (LEXDATA *pLex, unsigned long iLine, long *piPatternCnt, PCWSTR * pszPattern)
{
    long iChar = 0;
    HRESULT hr = pLex->FindFirstNonWhiteChar(iLine, &iChar); 
    // Keep iChar as the position of the star within the pattern
    if (hr == S_OK) {
        PCWSTR p = pLex->pszSource + pLex->piLines[iLine] + iChar;
        if (*p != L'*') {
            // no pattern on the first line
            goto NO_PATTERN;
        }
        PCWSTR pend = pLex->pszSource + pLex->piLines[iLine] + pLex->GetLineLength(iLine);
        p++;
        while (p < pend && IsWhitespaceChar(*p))
            p++;
        *piPatternCnt = (long)(p - (pLex->pszSource + pLex->piLines[iLine]));
        *pszPattern = pLex->pszSource + pLex->piLines[iLine];
        return S_OK;
    } else {
        // an empty line (or all whitespace)
NO_PATTERN:
        *piPatternCnt = 0;
        *pszPattern = L"";
        return S_FALSE;
    }
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::MatchDocCommentPattern
//
// pszLine - Pass in the comment line to check
// pszPattern - Pass in the pattern string
// piPatternCnt - Pass in/out the length of the pattern (it might change)
// Return S_OK if the full pattern was found,
// or S_FALSE if a smaller pattern was found (must include '*')
// or E_FAIL if not pattern was found

HRESULT CSourceModuleBase::MatchDocCommentPattern (PCWSTR pszLine, PCWSTR pszPattern, long *piPatternCnt)
{
    const long len = *piPatternCnt;

    if (wcsncmp(pszLine, pszPattern, len) == 0)
        return S_OK;

    long k;
    bool bSeenStar = false;
    for (k = 0; pszLine[k] == pszPattern[k] && k < len; k++) {
        if (pszLine[k] == L'*')
            bSeenStar = true;
    }
    if (bSeenStar == true) {
        *piPatternCnt = k;
        return S_FALSE;
    } else {
        *piPatternCnt = 0;
        return E_FAIL;
    }
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::GetDocCommentXML

HRESULT CSourceModuleBase::GetDocCommentXML (BASENODE *pNode, BSTR *pbstrDoc)
{
    HRESULT hr;
    PWSTR wszText = NULL;

    if (FAILED(hr = this->GetDocCommentText (pNode, &wszText)))
        return hr;

    // hr = S_FALSE is there is no doc comment
    CStringBuilder sb;
    if (hr != S_FALSE)
        sb.Append(wszText);
    hr = this->FreeDocCommentText(&wszText);
    ASSERT(SUCCEEDED(hr));
    return sb.CreateBSTR (pbstrDoc);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::FindLeafNode

HRESULT CSourceModuleBase::FindLeafNode (CSourceData *pData, const POSDATA pos, BASENODE **ppNode, ICSInteriorTree **ppTree)
{
    return FindLeafNodeEx(pData, pos, EF_FULL, ppNode, ppTree);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::FindLeafNodeEx

HRESULT CSourceModuleBase::FindLeafNodeEx (CSourceData *pData, const POSDATA pos, ExtentFlags flags, BASENODE **ppNode, ICSInteriorTree **ppTree)
{
    HRESULT hr;

    BASENODE    *pTopNode;

    if (ppTree != NULL)
        *ppTree = NULL;

    *ppNode = NULL;

    if (SUCCEEDED (hr = ParseTopLevel (pData, &pTopNode)))
    {
        // Search this node for the "lowest" containing the given position
        CBasenodeLookupCache cache;
        BASENODE    *pLeaf = m_pParser->FindLeafEx (cache, pos, pTopNode, ppTree == NULL ? NULL : pData, flags, ppTree);

        *ppNode = pLeaf;
        hr = pLeaf ? S_OK : E_FAIL;
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::FindLeafNodeForToken

HRESULT CSourceModuleBase::FindLeafNodeForToken (CSourceData *pData, long iToken, BASENODE **ppNode, ICSInteriorTree **ppTree)
{
    return FindLeafNodeForTokenEx(pData, iToken, EF_FULL, ppNode, ppTree);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::FindLeafNodeForTokenEx

HRESULT CSourceModuleBase::FindLeafNodeForTokenEx (CSourceData *pData, long iToken, ExtentFlags flags, BASENODE **ppNode, ICSInteriorTree **ppTree)
{
    HRESULT hr;
    BASENODE* pTopNode;
    if (FAILED (hr = ParseTopLevel (pData, &pTopNode)))
    {
        return hr;
    }

    return m_pParser->FindLeafNodeForTokenEx(iToken, pTopNode, pData, flags, ppNode, ppTree);
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::IsWarningDisabled
//
// This checks a warning against any #pragma warning directives and disables/eats
// the warning if appropriate

BOOL CSourceModuleBase::IsWarningDisabled (CError * pError)
{
    // Check to see if warning is disabled in source
    if (m_lex.pWarningMap != NULL && 
        !m_lex.pWarningMap->IsEmpty() &&
        pError->WasWarning() && 
        pError->LocationCount() > 0)
    {

        // It's a warning that can be disabled  and it has at least 1 location
        // Check to see if that warning number is disabled at the first/primary location
        PCWSTR pszFileName = NULL;
        POSDATA posStart, posEnd;

        if (SUCCEEDED(pError->GetUnmappedLocationAt( 0, &pszFileName, &posStart, &posEnd)) &&
            pszFileName != NULL && 
            !posStart.IsUninitialized() && 
            !posEnd.IsUninitialized())
        {

            if (m_lex.pWarningMap->IsWarningDisabled(pError->ID(), posStart.iLine, posEnd.iLine))
            {
                // Don't report the warning because it's been disabled
                return TRUE;
            }
        }
    }

    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::GetChecksum
// Get the MD5 hash for PDB check sum

HRESULT CSourceModuleBase::GetChecksum(Checksum * checksum)
{
    HRESULT hr = E_NOTIMPL;
    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::MapSourceLine

HRESULT CSourceModuleBase::MapSourceLine(CSourceData *pData, long iLine, long *piMappedLine, PCWSTR *ppszFilename, BOOL *pbIsHidden)
{    
    POSDATA pos;
    bool    fIsHidden;
    bool    fIsMapped;

    pos.iLine = iLine;
    pos.iChar = 0;

    MapLocation(&pos, ppszFilename, &fIsHidden, &fIsMapped);

    if (piMappedLine)
        *piMappedLine = pos.iLine;
    
    if (pbIsHidden)
        *pbIsHidden = (fIsHidden == true);

    // S_OK indicates the line is mapped, S_FALSE indicates it's the actual source line
    return fIsMapped ? S_OK : S_FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::GetLexResults

HRESULT CSourceModuleBase::GetLexResults (CSourceData *pData, LEXDATA *pLexData)
{
    *pLexData = m_lex;
    return S_OK;
}


////////////////////////////////////////////////////////////////////////////////
// CSourceModuleBase::AddToNodeTable

void CSourceModuleBase::AddToNodeTable (BASENODE *pNode)
{
    if (m_pNodeTable == NULL)
        return;

    CTinyGate   gate (&m_StateLock);

    // Add the node to the array, so we know its global ordinal
    NODECHAIN   *pChain = (NODECHAIN *)AllocFromActiveHeap (sizeof (NODECHAIN));
    KEYEDNODE   *pKeyedNode;

    if (FAILED (m_pNodeArray->Add (&pChain->iGlobalOrdinal, &pKeyedNode)))
        return;     // Oh, well...

    pChain->pNext = NULL;
    pKeyedNode->pNode = pNode;

    DWORD_PTR   dwCookie;
    HRESULT     hr = S_OK;

    // Build the key for this node and find/add its node chain
    if (SUCCEEDED (pNode->BuildKey (GetNameMgr(), TRUE, true /* we want the <,,,> */, &pKeyedNode->pKey)) &&
        SUCCEEDED (hr = m_pNodeTable->FindOrAdd (pKeyedNode->pKey, &dwCookie)))
    {
        // The new one must go on the END of the node chain.
        if (hr == S_FALSE)
        {
            // This is the first one (most common case by huge margin...)
            m_pNodeTable->SetData (dwCookie, pChain);
        }
        else
        {
            NODECHAIN   *pExisting = m_pNodeTable->GetData (dwCookie);

            // Find the end and add it
            if (pExisting == NULL)
                return;     // WHAT?!?

            while (pExisting->pNext != NULL)
                pExisting = pExisting->pNext;

            pExisting->pNext = pChain;
        }
    }
}

class CHeapTokenAllocator : public ITokenAllocator
{
public:
    CHeapTokenAllocator(MEMHEAP *pStdHeap) : m_pStdHeap(pStdHeap) {}
    void * AllocateMemory(size_t size) { return m_pStdHeap->Alloc(size); }
    void Free(void *p) { m_pStdHeap->Free(p); }

private:
    MEMHEAP *m_pStdHeap;
};

////////////////////////////////////////////////////////////////////////////////
// LEXDATABLOCK::CloneTo

void LEXDATABLOCK::CloneTo(CController *pController, MEMHEAP *pStdHeap, PCWSTR pszCloneSourceText, LEXDATABLOCK &lexClone)
{
    // pTokens
    TOKENSTREAM tokenstream;
    CHeapTokenAllocator alloc (pStdHeap);
    CloneTokenStream(&alloc, 0, iTokens, iAllocTokens, &tokenstream);
    lexClone.pTokens = tokenstream.pTokens;
    lexClone.iTokens = tokenstream.iTokens;
    lexClone.iAllocTokens = iAllocTokens;

    lexClone.pszSource = pszCloneSourceText;

    // piLines
    {
        lexClone.piLines = NULL;
        lexClone.iLines = iLines;

        long    iSize = iLines * sizeof (long);
        if (iSize > 0)
        {
            lexClone.piLines = (long *)pStdHeap->Alloc (iSize);
            memcpy(lexClone.piLines, piLines, iSize);
        }
    }

    // transition lines
    {
        lexClone.piTransitionLines = NULL;
        lexClone.iTransitionLines = iTransitionLines;

        long    iSize = iTransitionLines * sizeof (long);
        if (iSize > 0)
        {
            lexClone.piTransitionLines = (long *)VSAlloc (iSize);
            if (!lexClone.piTransitionLines)
                pController->NoMemory();

            memcpy(lexClone.piTransitionLines, piTransitionLines, iSize);
        }
    }

    // pIdentTable
    {
        lexClone.pIdentTable = NULL;

        if (pIdentTable != NULL)
        {
            lexClone.pIdentTable = new CIdentTable (pIdentTable->GetNameTable());
            if (lexClone.pIdentTable == NULL)
                pController->NoMemory();

            if (FAILED (lexClone.pIdentTable->CopyContentsFrom (pIdentTable)))
                pController->NoMemory();
        }
    }

    // piRegionStart
    {
        lexClone.piRegionStart = NULL;
        lexClone.piRegionEnd = NULL;
        lexClone.iRegions = iRegions;

        long    iSize = 2 * iCCStates * sizeof (LONG);
        if (iSize > 0)
        {
            lexClone.piRegionStart = (long *)pStdHeap->Alloc (iSize);
            lexClone.piRegionEnd = lexClone.piRegionStart + iCCStates;

            memcpy(lexClone.piRegionStart, piRegionStart, iSize);
        }
    }

    if (pWarningMap != NULL)
    {
        // Well, don't care about warnings for now...
    }

    // DONE with LEXDATA members... Take care of LEXDATABLOCK ones
    lexClone.iTextLen = iTextLen;
    lexClone.iAllocTokens = iTokens;
    lexClone.iAllocLines = iLines;
    lexClone.iAllocComments = iAllocComments;

    // ccstates
    {
        lexClone.pCCStates = NULL;
        lexClone.iCCStates = iCCStates;

        long    iSize = iCCStates * sizeof (CCSTATE *);
        if (iSize > 0)
        {
            lexClone.pCCStates = (CCSTATE *)VSAlloc (iSize);
            if (!lexClone.pCCStates)
                pController->NoMemory();

            memcpy(lexClone.pCCStates, pCCStates, iSize);
        }
    }
}

