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
//*****************************************************************************
// File: DebuggerModule.cpp
//
// Stuff for tracking DebuggerModules.
//
//*****************************************************************************

#include "stdafx.h"
#include "../inc/common.h"
#include "perflog.h"
#include "eeconfig.h" // This is here even for retail & free builds...
#include "vars.hpp"
#include <limits.h>
#include "ilformatter.h"
#include "memoryreport.h"
#include "debuginfostore.h"
#include "../../vm/methoditer.h"

#ifndef DACCESS_COMPILE

bool DbgIsSpecialILOffset(DWORD offset)
{
    LEAF_CONTRACT;

    return (offset == (ULONG) ICorDebugInfo::PROLOG ||
            offset == (ULONG) ICorDebugInfo::EPILOG ||
            offset == (ULONG) ICorDebugInfo::NO_MAPPING);
}

// Helper to use w/ the debug stores.
BYTE* InteropSafeNew(void * , size_t cBytes)
{
    BYTE * p = new (interopsafe, nothrow) BYTE[cBytes];
    return p;
}


//
// This is only fur internal debugging.
//
#ifdef LOGGING
static void _dumpVarNativeInfo(ICorJitInfo::NativeVarInfo* vni)
{
    WRAPPER_CONTRACT;

    LOG((LF_CORDB, LL_INFO1000000, "Var %02d: 0x%04x-0x%04x vlt=",
            vni->varNumber,
            vni->startOffset, vni->endOffset,
            vni->loc.vlType));

    switch (vni->loc.vlType)
    {
    case ICorJitInfo::VLT_REG:
        LOG((LF_CORDB, LL_INFO1000000, "REG reg=%d\n", vni->loc.vlReg.vlrReg));
        break;

    case ICorJitInfo::VLT_REG_BYREF:
        LOG((LF_CORDB, LL_INFO1000000, "REG_BYREF reg=%d\n", vni->loc.vlReg.vlrReg));
        break;

    case ICorJitInfo::VLT_STK:
        LOG((LF_CORDB, LL_INFO1000000, "STK reg=%d off=0x%04x (%d)\n",
             vni->loc.vlStk.vlsBaseReg,
             vni->loc.vlStk.vlsOffset,
             vni->loc.vlStk.vlsOffset));
        break;

    case ICorJitInfo::VLT_STK_BYREF:
        LOG((LF_CORDB, LL_INFO1000000, "STK_BYREF reg=%d off=0x%04x (%d)\n",
             vni->loc.vlStk.vlsBaseReg,
             vni->loc.vlStk.vlsOffset,
             vni->loc.vlStk.vlsOffset));
        break;

    case ICorJitInfo::VLT_REG_REG:
        LOG((LF_CORDB, LL_INFO1000000, "REG_REG reg1=%d reg2=%d\n",
             vni->loc.vlRegReg.vlrrReg1,
             vni->loc.vlRegReg.vlrrReg2));
        break;

    case ICorJitInfo::VLT_REG_STK:
        LOG((LF_CORDB, LL_INFO1000000, "REG_STK reg=%d basereg=%d off=0x%04x (%d)\n",
             vni->loc.vlRegStk.vlrsReg,
             vni->loc.vlRegStk.vlrsStk.vlrssBaseReg,
             vni->loc.vlRegStk.vlrsStk.vlrssOffset,
             vni->loc.vlRegStk.vlrsStk.vlrssOffset));
        break;

    case ICorJitInfo::VLT_STK_REG:
        LOG((LF_CORDB, LL_INFO1000000, "STK_REG basereg=%d off=0x%04x (%d) reg=%d\n",
             vni->loc.vlStkReg.vlsrStk.vlsrsBaseReg,
             vni->loc.vlStkReg.vlsrStk.vlsrsOffset,
             vni->loc.vlStkReg.vlsrStk.vlsrsOffset,
             vni->loc.vlStkReg.vlsrReg));
        break;

    case ICorJitInfo::VLT_STK2:
        LOG((LF_CORDB, LL_INFO1000000, "STK_STK reg=%d off=0x%04x (%d)\n",
             vni->loc.vlStk2.vls2BaseReg,
             vni->loc.vlStk2.vls2Offset,
             vni->loc.vlStk2.vls2Offset));
        break;

    case ICorJitInfo::VLT_FPSTK:
        LOG((LF_CORDB, LL_INFO1000000, "FPSTK reg=%d\n",
             vni->loc.vlFPstk.vlfReg));
        break;

    case ICorJitInfo::VLT_FIXED_VA:
        LOG((LF_CORDB, LL_INFO1000000, "FIXED_VA offset=%d (%d)\n",
             vni->loc.vlFixedVarArg.vlfvOffset,
             vni->loc.vlFixedVarArg.vlfvOffset));
        break;


    default:
        LOG((LF_CORDB, LL_INFO1000000, "???\n"));
        break;
    }
}
#endif


// It is entirely possbile that we have multiple sequence points for the
// same IL offset (because of funclets, optimization, etc.).  Just to be
// uniform in all cases, let's return the sequence point with the smallest
// native offset if fWantFirst is TRUE.
#define ADJUST_MAP_ENTRY(_map, _wantFirst)

DebuggerJitInfo::DebuggerJitInfo(DebuggerMethodInfo *minfo, MethodDesc *fd) :
    m_fd(fd),
    m_codePitched(false),
    m_jitComplete(false),
    m_illegalToTransitionFrom(FALSE),
    m_methodInfo(minfo),
    m_addrOfCode(NULL),
    m_sizeOfCode(0), m_prevJitInfo(NULL), m_nextJitInfo(NULL),
    m_lastIL(0),
    m_sequenceMap(NULL),
    m_sequenceMapCount(0), m_sequenceMapSorted(false),
    m_varNativeInfo(NULL), m_varNativeInfoCount(0),
    m_fAttemptInit(false)
    WIN64_ARG(m_rgFunclet(NULL))
    WIN64_ARG(m_funcletCount(0))
{
    WRAPPER_CONTRACT;


    _ASSERTE(minfo);
    m_encVersion = minfo->GetCurrentEnCVersion();
    _ASSERTE(m_encVersion >= CorDB_DEFAULT_ENC_FUNCTION_VERSION);
    LOG((LF_CORDB,LL_EVERYTHING, "DJI::DJI : created at 0x%x\n", this));

    // Debugger doesn't track LightWeight codegen methods.
    // We should never even be creating a DJI for one.
    _ASSERTE(!m_fd->IsDynamicMethod());
}
    
DebuggerILToNativeMap *DebuggerJitInfo::MapILOffsetToMapEntry(SIZE_T offset, BOOL *exact
                                                              WIN64_ARG(BOOL fWantFirst))
{
    LEAF_CONTRACT;

    // Ideally we should be able to assert this, since the binary search in this function
    // assumes that the sequence points are sorted by IL offset (NO_MAPPING, PROLOG, and EPILOG
    // are actually -1, -2, and -3, respectively).  However, the sequence points in pdb's use
    // -1 to mean "end of the method", which is different from our semantics of using 0.
    // _ASSERTE(offset != NO_MAPPING && offset != PROLOG && offset != EPILOG);

    //
    // Binary search for matching map element.
    //

    DebuggerILToNativeMap *mMin = GetSequenceMap();
    DebuggerILToNativeMap *mMax = mMin + GetSequenceMapCount();

    _ASSERTE(m_sequenceMapSorted);
    _ASSERTE( mMin < mMax ); //otherwise we have no code

    if (exact)
    {
        *exact = FALSE;
    }

    if (mMin)
    {
        while (mMin + 1 < mMax)
        {
            _ASSERTE(mMin>=m_sequenceMap);
            DebuggerILToNativeMap *mMid = mMin + ((mMax - mMin)>>1);
            _ASSERTE(mMid>=m_sequenceMap);

            if (offset == mMid->ilOffset)
            {
                if (exact)
                {
                    *exact = TRUE;
                }
                ADJUST_MAP_ENTRY(mMid, fWantFirst);
                return mMid;
            }
            else if (offset < mMid->ilOffset && mMid->ilOffset != (ULONG) ICorDebugInfo::PROLOG)
            {
                mMax = mMid;
            }
            else
            {
                mMin = mMid;
            }
        }

        if (exact && offset == mMin->ilOffset)
        {
            *exact = TRUE;
        }
        ADJUST_MAP_ENTRY(mMin, fWantFirst);
    }
    return mMin;
}

void DebuggerJitInfo::InitILToNativeOffsetIterator(ILToNativeOffsetIterator &iterator, SIZE_T ilOffset)
{
    WRAPPER_CONTRACT;

    iterator.Init(this, ilOffset);
}


DebuggerJitInfo::NativeOffset DebuggerJitInfo::MapILOffsetToNative(DebuggerJitInfo::ILOffset ilOffset)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    NativeOffset resultOffset;

    DebuggerILToNativeMap *map = MapILOffsetToMapEntry(ilOffset.m_ilOffset, &(resultOffset.m_fExact));

        PREFIX_ASSUME( map != NULL );
        LOG((LF_CORDB, LL_INFO10000, "DJI::MILOTN: ilOff 0x%x to nat 0x%x exact:0x%x (Entry IL Off:0x%x)\n",
             ilOffset.m_ilOffset, map->nativeStartOffset, resultOffset.m_fExact, map->ilOffset));

        resultOffset.m_nativeOffset = map->nativeStartOffset;


    return resultOffset;
}


DebuggerJitInfo::ILToNativeOffsetIterator::ILToNativeOffsetIterator()
{
    LEAF_CONTRACT;

    m_dji = NULL;
    m_currentILOffset.m_ilOffset = INVALID_IL_OFFSET;
    WIN64_ONLY(m_currentILOffset.m_funcletIndex = PARENT_METHOD_INDEX);
}

void DebuggerJitInfo::ILToNativeOffsetIterator::Init(DebuggerJitInfo* dji, SIZE_T ilOffset)
{
    WRAPPER_CONTRACT;

    m_dji = dji;
    m_currentILOffset.m_ilOffset = ilOffset;
    WIN64_ONLY(m_currentILOffset.m_funcletIndex = PARENT_METHOD_INDEX);

    m_currentNativeOffset = m_dji->MapILOffsetToNative(m_currentILOffset);
}

bool DebuggerJitInfo::ILToNativeOffsetIterator::IsAtEnd()
{
    LEAF_CONTRACT;

    return (m_currentILOffset.m_ilOffset == INVALID_IL_OFFSET);
}

SIZE_T DebuggerJitInfo::ILToNativeOffsetIterator::Current(BOOL* pfExact)
{
    LEAF_CONTRACT;

    if (pfExact != NULL)
    {
        *pfExact = m_currentNativeOffset.m_fExact;
    }
    return m_currentNativeOffset.m_nativeOffset;
}

SIZE_T DebuggerJitInfo::ILToNativeOffsetIterator::CurrentAssertOnlyOne(BOOL* pfExact)
{
    WRAPPER_CONTRACT;

    SIZE_T nativeOffset = Current(pfExact);

    Next();
    _ASSERTE(IsAtEnd());

    return nativeOffset;
}

void DebuggerJitInfo::ILToNativeOffsetIterator::Next()
{
    m_currentILOffset.m_ilOffset = INVALID_IL_OFFSET;
}



// SIZE_T DebuggerJitInfo::MapSpecialToNative():  Maps something like
//      a prolog to a native offset.
// CordDebugMappingResult mapping:  Mapping type to be looking for.
// SIZE_T which:  Which one.                                                  

SIZE_T DebuggerJitInfo::MapSpecialToNative(CorDebugMappingResult mapping,
                                           SIZE_T which,
                                           BOOL *pfAccurate)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(NULL != pfAccurate);
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO10000, "DJI::MSTN map:0x%x which:0x%x\n", mapping, which));

    bool fFound;
    SIZE_T  cFound = 0;

    DebuggerILToNativeMap *m = GetSequenceMap();
    DebuggerILToNativeMap *mEnd = m + GetSequenceMapCount();
    if (m)
    {
        while(m < mEnd)
        {
            _ASSERTE(m>=GetSequenceMap());

            fFound = false;

            if (DbgIsSpecialILOffset(m->ilOffset))
                cFound++;

            if (cFound == which)
            {
                _ASSERTE( (mapping == MAPPING_PROLOG &&
                    m->ilOffset == (SIZE_T) ICorDebugInfo::PROLOG) ||
                          (mapping == MAPPING_EPILOG &&
                    m->ilOffset == (SIZE_T) ICorDebugInfo::EPILOG) ||
                          ((mapping == MAPPING_NO_INFO || mapping == MAPPING_UNMAPPED_ADDRESS) &&
                    m->ilOffset == (SIZE_T) ICorDebugInfo::NO_MAPPING)
                        );

                (*pfAccurate) = TRUE;
                LOG((LF_CORDB, LL_INFO10000, "DJI::MSTN found mapping to nat:0x%x\n",
                    m->nativeStartOffset));
                return m->nativeStartOffset;
            }
            m++;
        }
    }

    LOG((LF_CORDB, LL_INFO10000, "DJI::MSTN No mapping found :(\n"));
    (*pfAccurate) = FALSE;

    return 0;
}


// void DebuggerJitInfo::MapILRangeToMapEntryRange():   MIRTMER
// calls MapILOffsetToNative for the startOffset (putting the
// result into start), and the endOffset (putting the result into end).
// SIZE_T startOffset:  IL offset from beginning of function.
// SIZE_T endOffset:  IL offset from beginngin of function,
// or zero to indicate that the end of the function should be used.
// DebuggerILToNativeMap **start:  Contains start & end
// native offsets that correspond to startOffset.  Set to NULL if
// there is no mapping info.
// DebuggerILToNativeMap **end:  Contains start & end native
// offsets that correspond to endOffset. Set to NULL if there
// is no mapping info.
void DebuggerJitInfo::MapILRangeToMapEntryRange(SIZE_T startOffset,
                                                SIZE_T endOffset,
                                                DebuggerILToNativeMap **start,
                                                DebuggerILToNativeMap **end)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO1000000,
         "DJI::MIRTMER: IL 0x%04x-0x%04x\n",
         startOffset, endOffset));

    if (GetSequenceMapCount() == 0)
    {
        *start = NULL;
        *end = NULL;
        return;
    }

    *start = MapILOffsetToMapEntry(startOffset);

    //
    // end points to the last range that endOffset maps to, not past
    // the last range.
    // We want to return the last IL, and exclude the epilog
    if (endOffset == 0)
    {
        *end = GetSequenceMap() + GetSequenceMapCount() - 1;
        _ASSERTE(*end>=m_sequenceMap);

        while ( ((*end)->ilOffset == (ULONG) ICorDebugInfo::EPILOG||
                (*end)->ilOffset == (ULONG) ICorDebugInfo::NO_MAPPING)
               && (*end) > m_sequenceMap)
        {
            (*end)--;
            _ASSERTE(*end>=m_sequenceMap);

        }
    }
    else
        *end = MapILOffsetToMapEntry(endOffset - 1, NULL
                                     WIN64_ARG(FALSE));

    _ASSERTE(*end>=m_sequenceMap);


    LOG((LF_CORDB, LL_INFO1000000,
         "DJI::MIRTMER: IL 0x%04x-0x%04x --> 0x%04x 0x%08x-0x%08x\n"
         "                               --> 0x%04x 0x%08x-0x%08x\n",
         startOffset, endOffset,
         (*start)->ilOffset,
         (*start)->nativeStartOffset, (*start)->nativeEndOffset,
         (*end)->ilOffset,
         (*end)->nativeStartOffset, (*end)->nativeEndOffset));
}


// DWORD DebuggerJitInfo::MapNativeOffsetToIL():   Given a native
//  offset for the DebuggerJitInfo, compute
//  the IL offset from the beginning of the same method.
// Returns: Offset of the IL instruction that contains
//  the native offset,
// SIZE_T nativeOffset:  [IN] Native Offset
// CorDebugMappingResult *map:  [OUT] explains the
//  quality of the matching & special cases
// SIZE_T which:  It's possible to have multiple EPILOGs, or
//  multiple unmapped regions within a method.  This opaque value
//  specifies which special region we're talking about.  This
//  param has no meaning if map & (MAPPING_EXACT|MAPPING_APPROXIMATE)
//  Basically, this gets handed back to MapSpecialToNative, later.
DWORD DebuggerJitInfo::MapNativeOffsetToIL(DWORD nativeOffset,
                                            CorDebugMappingResult *map,
                                            DWORD *which)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(map != NULL);
        PRECONDITION(which != NULL);
    }
    CONTRACTL_END;

    (*which) = 0;
    DebuggerILToNativeMap *m = GetSequenceMap();
    DebuggerILToNativeMap *mEnd = m + GetSequenceMapCount();

    LOG((LF_CORDB,LL_INFO10000,"DJI::MNOTI: nativeOffset = 0x%x\n", nativeOffset));

    if (m)
    {
        while (m < mEnd)
        {
            _ASSERTE(m>=m_sequenceMap);

#ifdef LOGGING
            if (m->ilOffset == (SIZE_T) ICorDebugInfo::PROLOG )
                LOG((LF_CORDB,LL_INFO10000,"DJI::MNOTI: m->natStart:0x%x m->natEnd:0x%x il:PROLOG\n", m->nativeStartOffset, m->nativeEndOffset));
            else if (m->ilOffset == (SIZE_T) ICorDebugInfo::EPILOG )
                LOG((LF_CORDB,LL_INFO10000,"DJI::MNOTI: m->natStart:0x%x m->natEnd:0x%x il:EPILOG\n", m->nativeStartOffset, m->nativeEndOffset));
            else if (m->ilOffset == (SIZE_T) ICorDebugInfo::NO_MAPPING)
                LOG((LF_CORDB,LL_INFO10000,"DJI::MNOTI: m->natStart:0x%x m->natEnd:0x%x il:NO MAP\n", m->nativeStartOffset, m->nativeEndOffset));
            else
                LOG((LF_CORDB,LL_INFO10000,"DJI::MNOTI: m->natStart:0x%x m->natEnd:0x%x il:0x%x\n", m->nativeStartOffset, m->nativeEndOffset, m->ilOffset));
#endif // LOGGING

            if (m->ilOffset == (ULONG) ICorDebugInfo::PROLOG ||
                m->ilOffset == (ULONG) ICorDebugInfo::EPILOG ||
                m->ilOffset == (ULONG) ICorDebugInfo::NO_MAPPING)
            {
                (*which)++;
            }

            if (nativeOffset >= m->nativeStartOffset
                && ((m->nativeEndOffset == 0 &&
                    m->ilOffset != (ULONG) ICorDebugInfo::PROLOG)
                     || nativeOffset < m->nativeEndOffset))
            {
                SIZE_T ilOff = m->ilOffset;

                if( m->ilOffset == (ULONG) ICorDebugInfo::PROLOG )
                {
                    ilOff = 0;
                    (*map) = MAPPING_PROLOG;
                    LOG((LF_CORDB,LL_INFO10000,"DJI::MNOTI: MAPPING_PROLOG\n"));

                }
                else if (m->ilOffset == (ULONG) ICorDebugInfo::NO_MAPPING)
                {
                    ilOff = 0;
                    (*map) = MAPPING_UNMAPPED_ADDRESS ;
                    LOG((LF_CORDB,LL_INFO10000,"DJI::MNOTI:MAPPING_"
                        "UNMAPPED_ADDRESS\n"));
                }
                else if( m->ilOffset == (ULONG) ICorDebugInfo::EPILOG )
                {
                    ilOff = m_lastIL;
                    (*map) = MAPPING_EPILOG;
                    LOG((LF_CORDB,LL_INFO10000,"DJI::MNOTI:MAPPING_EPILOG\n"));
                }
                else if (nativeOffset == m->nativeStartOffset)
                {
                    (*map) = MAPPING_EXACT;
                    LOG((LF_CORDB,LL_INFO10000,"DJI::MNOTI:MAPPING_EXACT\n"));
                }
                else
                {
                    (*map) = MAPPING_APPROXIMATE;
                    LOG((LF_CORDB,LL_INFO10000,"DJI::MNOTI:MAPPING_"
                        "APPROXIMATE\n"));
                }

                return ilOff;
            }
            m++;
        }
    }

    (*map) = MAPPING_NO_INFO;
    LOG((LF_CORDB,LL_INFO10000,"DJI::MNOTI:NO_INFO\n"));
    return 0;
}

/******************************************************************************
 *
 ******************************************************************************/
DebuggerJitInfo::~DebuggerJitInfo()
{
    TRACE_FREE(m_sequenceMap);
    if (m_sequenceMap != NULL)
    {
        DeleteInteropSafe(((BYTE *)m_sequenceMap));
    }

    TRACE_FREE(m_varNativeInfo);
    if (m_varNativeInfo != NULL)
    {
        DeleteInteropSafe(m_varNativeInfo);
    }



#ifdef _DEBUG
    // Trash pointers to garbage.
    // Don't null out since there may be runtime checks against NULL.
    // Set to a non-null random pointer value that will cause an immediate AV on deref.
    m_fd = (MethodDesc*) 0x1;
    m_methodInfo = (DebuggerMethodInfo*) 0x1;
    m_prevJitInfo = (DebuggerJitInfo*) 0x01;
    m_nextJitInfo = (DebuggerJitInfo*) 0x01;
#endif


    LOG((LF_CORDB,LL_EVERYTHING, "DJI::~DJI : deleted at 0x%p\n", this));
}


// Lazy initialize the Debugger-Jit-Info
void DebuggerJitInfo::LazyInitBounds()
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(ThisMaybeHelperThread());
        PRECONDITION(!g_pDebugger->HasDebuggerDataLock());
    } CONTRACTL_END;

    if (m_fAttemptInit)
    {
        return;
    }
    m_fAttemptInit = true;

    EX_TRY
    {

        // Should have already been jitted
        _ASSERTE(this->m_jitComplete);

        MethodDesc * mdesc = this->m_fd;

        DebugInfoRequest request;

        _ASSERTE(this->m_addrOfCode != NULL); // must have address to disambguate the Enc cases.
        // Caller already resolved generics when they craeted the DJI, so we don't need to repeat.
        // Note the MethodDesc may not yet have the jitted info, so we'll also use the starting address we got in the jit complete callback.
        request.InitFromStartingAddr(mdesc, this->m_addrOfCode);


        // Bounds info.
        ULONG32 cMap;
        ICorDebugInfo::OffsetMapping *pMap = NULL;

        HRESULT hBounds = g_pDebugInfoStore->GetBoundaries(
            request,
            InteropSafeNew, NULL, // allocator
            &cMap, &pMap);

        if (SUCCEEDED(hBounds))
        {
            this->SetBoundaries(cMap, pMap);
        }


        // Set var info.
        ULONG32 cVars;
        ICorDebugInfo::NativeVarInfo *pVars = NULL;

        HRESULT hVars = g_pDebugInfoStore->GetVars(
            request,
            InteropSafeNew, NULL,
            &cVars, &pVars);

        if (SUCCEEDED(hVars))
        {
            this->SetVars(cVars, pVars);
        }

    }
    EX_CATCH
    {
        // Just swallow the exception. The DJI maps may or may-not be intialized,
        // but they should still be in a consistent state, so we should be ok.
    }
    EX_END_CATCH(SwallowAllExceptions)
}

/******************************************************************************
 * SetVars() takes ownership of pVars
 ******************************************************************************/
void DebuggerJitInfo::SetVars(ULONG32 cVars, ICorDebugInfo::NativeVarInfo *pVars)
{
    LEAF_CONTRACT;

    if (m_varNativeInfo)
    {
        return;
    }

    m_varNativeInfo = pVars;
    m_varNativeInfoCount = cVars;

    LOG((LF_CORDB, LL_INFO1000000, "D::sV: var count is %d\n",
         m_varNativeInfoCount));

#ifdef LOGGING
    for (unsigned int i = 0; i < m_varNativeInfoCount; i++)
    {
        ICorJitInfo::NativeVarInfo* vni = &(m_varNativeInfo[i]);
        _dumpVarNativeInfo(vni);
    }
#endif
}

CHECK DebuggerJitInfo::Check() const
{
    LEAF_CONTRACT;

    CHECK_OK;
}

// Invariants for a DebuggerJitInfo
// These should always be true at any well defined point.
CHECK DebuggerJitInfo::Invariant() const
{
    LEAF_CONTRACT;
    CHECK((m_sequenceMapCount == 0) == (m_sequenceMap == NULL));
    CHECK(m_methodInfo != NULL);
    CHECK(m_fd != NULL);

    CHECK_OK;
}


/******************************************************************************
 * SetBoundaries() takes ownership of pMap
 ******************************************************************************/
void DebuggerJitInfo::SetBoundaries(ULONG32 cMap, ICorDebugInfo::OffsetMapping *pMap)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
    }
    CONTRACTL_END;

    _ASSERTE((cMap == 0) == (pMap == NULL));

    if (cMap == 0)
        return;

    if (m_sequenceMap)
    {
        return;
    }

    SIZE_T ilLast = 0;
#ifdef _DEBUG
    // We assume that the map is sorted by native offset
    if (cMap > 1)
    {
        for(ICorDebugInfo::OffsetMapping * pEntry = pMap;
            pEntry < (pMap + cMap - 1);
            pEntry++)
        {
            _ASSERTE(pEntry->nativeOffset <= (pEntry+1)->nativeOffset);
        }
    }
#endif //_DEBUG

    //
    //
    m_sequenceMap = (DebuggerILToNativeMap *)new (interopsafe) DebuggerILToNativeMap[cMap];
    _ASSERTE(m_sequenceMap != NULL); // we'll throw on null

    m_sequenceMapCount = cMap;

    DebuggerILToNativeMap *m = m_sequenceMap;

    // For the instrumented-IL case, we need to remove all duplicate entries.
    // So we keep a record of the last old IL offset. If the current old IL
    // offset is the same as the last old IL offset, we remove it.
    // Pick a unique initial value (-10) so that the 1st doesn't accidentally match.
    int ilPrevOld = -10;

    //
    for(ULONG32 idxJitMap = 0; idxJitMap < cMap; idxJitMap++)
    {
        const ICorDebugInfo::OffsetMapping * const pMapEntry = &pMap[idxJitMap];
        _ASSERTE(m >= m_sequenceMap);
        _ASSERTE(m < m_sequenceMap + m_sequenceMapCount);

        ilLast = max( (int)ilLast, (int)pMapEntry->ilOffset );

        // Simply copy everything over, since we translate to
        // CorDebugMappingResults immediately prior to handing
        // back to user...
        m->nativeStartOffset    = pMapEntry->nativeOffset;
        m->ilOffset             = pMapEntry->ilOffset;
        m->source               = pMapEntry->source;

        // Keep in mind that if we have an instrumented code translation
        // table, we may have asked for completely different IL offsets
        // than the user thinks we did.....

        // If we did instrument, then we can't have any sequence points that
        // are "in-between" the old-->new map that the profiler gave us.
        // Ex, if map is:
        // (6 old -> 36 new)
        // (8 old -> 50 new)
        // And the jit gives us an entry for 44 new, that will map back to 6 old.
        // Since the map can only have one entry for 6 old, we remove 44 new.
        if (m_methodInfo->HasInstrumentedILMap())
        {
            int ilThisOld = m_methodInfo->TranslateToInstIL(pMapEntry->ilOffset, bInstrumentedToOriginal);

            if (ilThisOld == ilPrevOld)
            {
                // If this translated to the same old IL offset as the last entry,
                // then this is "in between". Skip it.
                m_sequenceMapCount--; // one less seq point in the DJI's map
                continue;
            }
            m->ilOffset = ilThisOld;
            ilPrevOld = ilThisOld;
        }

        if (m > m_sequenceMap && (m-1)->ilOffset == m->ilOffset)
        {
            // JIT gave us an extra entry (probably zero), so mush
            // it into the one we've already got.
            m_sequenceMapCount--;
            continue;
        }

        // Move to next entry in the debugger's table
        m++;
    } // end for

    DeleteInteropSafe(pMap);

    _ASSERTE(m == m_sequenceMap + m_sequenceMapCount);

    m_lastIL = ilLast;

    // Set nativeEndOffset in debugger's il->native map
    // Do this before we resort by IL.
    unsigned int i;
    for(i = 0; i < m_sequenceMapCount - 1; i++)
    {
        m_sequenceMap[i].nativeEndOffset = m_sequenceMap[i+1].nativeStartOffset;
    }

    m_sequenceMap[i].nativeEndOffset = 0;
    m_sequenceMap[i].source = (ICorDebugInfo::SourceTypes)
                ((DWORD) m_sequenceMap[i].source |
                (DWORD)ICorDebugInfo::NATIVE_END_OFFSET_UNKNOWN);

    // Now resort by IL.
    MapSortIL isort(m_sequenceMap, m_sequenceMapCount);

    isort.Sort();

    m_sequenceMapSorted = true;

    LOG((LF_CORDB, LL_INFO100000, "D::sB: boundary count is %d\n",
         m_sequenceMapCount));

#ifdef LOGGING
    for (unsigned int i = 0; i < m_sequenceMapCount; i++)
    {
        if( m_sequenceMap[i].ilOffset ==
            (SIZE_T) ICorDebugInfo::PROLOG )
            LOG((LF_CORDB, LL_INFO1000000,
                 "D::sB: PROLOG               --> 0x%08x -- 0x%08x",
                 m_sequenceMap[i].nativeStartOffset,
                 m_sequenceMap[i].nativeEndOffset));
        else if ( m_sequenceMap[i].ilOffset ==
                  (SIZE_T) ICorDebugInfo::EPILOG )
            LOG((LF_CORDB, LL_INFO1000000,
                 "D::sB: EPILOG              --> 0x%08x -- 0x%08x",
                 m_sequenceMap[i].nativeStartOffset,
                 m_sequenceMap[i].nativeEndOffset));
        else if ( m_sequenceMap[i].ilOffset ==
                  (SIZE_T) ICorDebugInfo::NO_MAPPING )
            LOG((LF_CORDB, LL_INFO1000000,
                 "D::sB: NO MAP              --> 0x%08x -- 0x%08x",
                 m_sequenceMap[i].nativeStartOffset,
                 m_sequenceMap[i].nativeEndOffset));
        else
            LOG((LF_CORDB, LL_INFO1000000,
                 "D::sB: 0x%04x (Real:0x%04x) --> 0x%08x -- 0x%08x",
                 m_sequenceMap[i].ilOffset,
                 m_methodInfo->TranslateToInstIL(m_sequenceMap[i].ilOffset,
                                   bOriginalToInstrumented),
                 m_sequenceMap[i].nativeStartOffset,
                 m_sequenceMap[i].nativeEndOffset));

        LOG((LF_CORDB, LL_INFO1000000, " Src:0x%x\n", m_sequenceMap[i].source));

    }
#endif //LOGGING
}

// Init a DJI after it's jitted.
void DebuggerJitInfo::Init(TADDR newAddress, SIZE_T sizeOfCode)
{
    // Shouldn't initialize while holding the lock b/c intialzing may call functions that lock,
    // and thus we'd have a locking violation.
    _ASSERTE(!g_pDebugger->HasDebuggerDataLock());

    this->m_addrOfCode = (ULONG_PTR)PTR_TO_CORDB_ADDRESS((BYTE*) newAddress);
    this->m_sizeOfCode =  sizeOfCode;
    this->m_jitComplete = true;
    this->m_codePitched = false;
    this->m_codeRegionInfo.InitializeFromStartAddress((CORDB_ADDRESS_TYPE *) CORDB_ADDRESS_TO_PTR(this->m_addrOfCode));
    this->m_encVersion = this->m_methodInfo->GetCurrentEnCVersion();

    _ASSERTE(this->m_codeRegionInfo.getSizeOfTotalCode() == this->m_sizeOfCode);


    LOG((LF_CORDB,LL_INFO10000,"De::JITCo:Got DJI 0x%p(V %d),"
         "Hot section from 0x%p to 0x%p "
         "Cold section from 0x%p to 0x%p "
         "varCount=%d  seqCount=%d\n",
         this, this->m_encVersion,
         this->m_codeRegionInfo.getAddrOfHotCode(),
         this->m_codeRegionInfo.getAddrOfHotCode() + this->m_codeRegionInfo.getSizeOfHotCode(),
         this->m_codeRegionInfo.getAddrOfColdCode(),
         this->m_codeRegionInfo.getAddrOfColdCode() + this->m_codeRegionInfo.getSizeOfColdCode(),
         (ULONG)this->m_addrOfCode,
         (ULONG)this->m_addrOfCode+(ULONG)this->m_sizeOfCode,
         this->GetVarNativeInfoCount(),
         this->GetSequenceMapCount()));

#if defined(LOGGING)
    for (unsigned int i = 0; i < this->GetSequenceMapCount(); i++)
    {
        LOG((LF_CORDB, LL_INFO10000, "De::JITCo: seq map 0x%x - "
             "IL offset 0x%x native start offset 0x%x native end offset 0x%x source 0x%x\n",
             i, this->GetSequenceMap()[i].ilOffset,
             this->GetSequenceMap()[i].nativeStartOffset,
             this->GetSequenceMap()[i].nativeEndOffset,
             this->GetSequenceMap()[i].source));
    }
#endif // LOGGING

}

/******************************************************************************
 *
 ******************************************************************************/
ICorDebugInfo::SourceTypes DebuggerJitInfo::GetSrcTypeFromILOffset(SIZE_T ilOffset)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    BOOL exact = FALSE;
    DebuggerILToNativeMap *pMap = MapILOffsetToMapEntry(ilOffset, &exact);

    LOG((LF_CORDB, LL_INFO100000, "DJI::GSTFILO: for il 0x%x, got entry 0x%p,"
        "(il 0x%x) nat 0x%x to 0x%x, SourceTypes 0x%x, exact:%x\n", ilOffset, pMap,
        pMap->ilOffset, pMap->nativeStartOffset, pMap->nativeEndOffset, pMap->source,
        exact));

    if (!exact)
    {
        return ICorDebugInfo::SOURCE_TYPE_INVALID;
    }

    return pMap->source;
}

/******************************************************************************
 *
 ******************************************************************************/
DebuggerMethodInfo::~DebuggerMethodInfo()
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
        DESTRUCTOR_CHECK;
    }
    CONTRACTL_END;

    TRACE_FREE(m_rgInstrumentedILMap);

    if (m_rgInstrumentedILMap != NULL)
        CoTaskMemFree(m_rgInstrumentedILMap);

    DeleteJitInfoList();

    LOG((LF_CORDB,LL_EVERYTHING, "DMI::~DMI : deleted at 0x%p\n", this));
}

// Translate between old & new offsets (w/ respect to Instrumented IL).

// Don't interpolate
SIZE_T DebuggerMethodInfo::TranslateToInstIL(SIZE_T offOrig, bool fOrigToInst)
{
    LEAF_CONTRACT;

    SIZE_T iMap;
    // some negative IL offsets have special meaning. Don't translate
    // those (just return as is). See ICorDebugInfo::MappingTypes
    if (m_cInstrumentedILMap == 0 || ((int) offOrig < 0))
            return offOrig;

    // This assumes:
    // - map is sorted in increasing order by both old & new
    // - round down.
    if (fOrigToInst)
    {
        // Translate: old --> new
        for(iMap = 1; iMap < m_cInstrumentedILMap; iMap++)
        {
            if (offOrig < m_rgInstrumentedILMap[iMap].oldOffset)
                return m_rgInstrumentedILMap[iMap-1].newOffset;
        }
        return m_rgInstrumentedILMap[iMap - 1].newOffset;
    }
    else
    {
        // Translate: new --> old
        for(iMap = 1; iMap < m_cInstrumentedILMap; iMap++)
        {
            if (offOrig < m_rgInstrumentedILMap[iMap].newOffset)
                return m_rgInstrumentedILMap[iMap-1].oldOffset;
        }
        return m_rgInstrumentedILMap[iMap - 1].oldOffset;
    }
}

/******************************************************************************
 * Constructor for DebuggerMethodInfo
 ******************************************************************************/
DebuggerMethodInfo::DebuggerMethodInfo(Module *module, mdMethodDef token) :
        m_currentEnCVersion(CorDB_DEFAULT_ENC_FUNCTION_VERSION),
        m_module(module),
        m_token(token),
        m_prevMethodInfo(NULL),
        m_nextMethodInfo(NULL),
        m_latestJitInfo(NULL),
        m_cInstrumentedILMap(0), m_rgInstrumentedILMap(NULL)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        WRAPPER(THROWS);
        WRAPPER(GC_TRIGGERS);
        CONSTRUCTOR_CHECK;
    }
    CONTRACTL_END;
 
    LOG((LF_CORDB,LL_EVERYTHING, "DMI::DMI : created at 0x%p\n", this));

    _ASSERTE(g_pDebugger->HasDebuggerDataLock());

    DebuggerModule * pModule = GetPrimaryModule();

    m_fJMCStatus = false;

    // If there's no module, then this isn't a JMC function.
    // This can happen since DMIs are created for debuggable code, and
    // Modules are only created if a debugger is actually attached.
    if (pModule != NULL)
    {
        // Use the accessor so that we keep the module's count properly updated.
        SetJMCStatus(pModule->LookupMethodDefaultJMCStatus(this));
    }
 }


/******************************************************************************
 * Get the primary debugger module for this DMI. This is 1:1 w/ an EE Module.
 ******************************************************************************/
DebuggerModule* DebuggerMethodInfo::GetPrimaryModule()
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(g_pDebugger->HasDebuggerDataLock());

    DebuggerModuleTable * pTable = g_pDebugger->GetModuleTable();

    // If we're tracking debug info but no debugger's attached, then
    // we won't have a table for the modules yet.
    if (pTable == NULL)
        return NULL;

    DebuggerModule * pModule = pTable->GetModule(GetRuntimeModule());
    if (pModule == NULL)
    {
        // We may be missing the module even if we have the table.
        // 1.) If there's no debugger attached (so we're not getting ModuleLoad events).
        // 2.) If we're asking for this while in DllMain of the module we're currently loading,
        //     we won't have gotten the ModuleLoad event yet.
        return NULL;
    }

    // Only give back primary modules...
    DebuggerModule * p2 = pModule->GetPrimaryModule();
    _ASSERTE(p2 != NULL);

    return p2;
}

/******************************************************************************
 * Get the runtime module for this DMI
 ******************************************************************************/
Module * DebuggerMethodInfo::GetRuntimeModule()
{
    LEAF_CONTRACT;

    return m_module;
}

/*
 * FindOrCreateInitAndAddJitInfo
 *
 * This routine allocates a new DJI, adding it to the DMI.
 *
 * Parameters:
 *   fd - the method desc to create a DJI for.
 *
 * Returns
 *   A pointer to the created DJI, or NULL.
 *
 */

DebuggerJitInfo *DebuggerMethodInfo::FindOrCreateInitAndAddJitInfo(MethodDesc* fd)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(fd != NULL);

    // This will grab the latest EnC version.
    TADDR addr = (TADDR) g_pEEInterface->GetFunctionAddress(fd);
    SIZE_T size = (SIZE_T) g_pEEInterface->GetFunctionSize(fd);

    if ((addr == NULL) || (size == 0))
        return NULL;

    // Check the lsit to see if we've already populated an entry for this JitInfo.
    // If we didn't have a JitInfo before, lazily create it now.
    // We don't care if we were prejitted or not.
    //
    // We haven't got the lock yet so we'll repeat this lookup once
    // we've taken the lock.
    DebuggerJitInfo * pCheck = m_latestJitInfo;
    while(pCheck != NULL)
    {
        if ((pCheck->m_fd == fd) && (pCheck->m_addrOfCode == addr))
        {
            // Found!
            _ASSERTE(pCheck->m_sizeOfCode == size);
            return pCheck;
        }

        pCheck = pCheck->m_prevJitInfo;
    }


    // CreateInitAndAddJitInfo takes a lock and checks the list again, which
    // makes this  thread-safe.
    return CreateInitAndAddJitInfo(fd, addr, size);
}

// Create a DJI around a method-desc. The EE already has all the information we need for a DJI,
// the DJI just serves as a cache of the information for the debugger.
// Caller makes no guarantees about whether the DJI is already in the table. (Caller should avoid this if
// it knows it's in the table, but b/c we can't expect caller to synchronize w/ the other threads).
DebuggerJitInfo *DebuggerMethodInfo::CreateInitAndAddJitInfo(MethodDesc* fd, TADDR startAddr, SIZE_T sizeBytes)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(!g_pDebugger->HasDebuggerDataLock());
    }
    CONTRACTL_END;

    _ASSERTE(fd != NULL);

    // May or may-not be jitted, that's why we passed in the start addr & size explicitly.
    _ASSERTE(startAddr != NULL);
    _ASSERTE(sizeBytes != NULL);


    // No support for light-weight codegen methods.
    if (fd->IsDynamicMethod())
    {
        return NULL;
    }


    DebuggerJitInfo *dji = new (interopsafe) DebuggerJitInfo(this, fd);
    _ASSERTE(dji != NULL); // throws on oom error

    _ASSERTE(dji->m_methodInfo == this); // this should be set

    TRACE_ALLOC(dji);

    // Init may take locks that violate the debugger-data lock, so we can't init while we hold that lock.
    // But we can't init after we add it to the table and release the lock b/c another thread may pick
    // if up in the uninitialized state.
    // So we initialize a private copy of the DJI before we take the debugger-data lock.
    dji->Init(startAddr, sizeBytes);

    dji->m_nextJitInfo = NULL;

    //
    //
    {
        Debugger::DebuggerDataLockHolder debuggerDataLockHolder(g_pDebugger);

        // We need to ensure that another thread didn't go in and add this exact same DJI?
        {
            DebuggerJitInfo * pCheck = m_latestJitInfo;
            while(pCheck != NULL)
            {
                if ((pCheck->m_fd == dji->m_fd) && (pCheck->m_addrOfCode == dji->m_addrOfCode))
                {
                    // Found!
                    _ASSERTE(pCheck->m_sizeOfCode == dji->m_sizeOfCode);
                    DeleteInteropSafe(dji);
                    return pCheck;
                }

                pCheck = pCheck->m_prevJitInfo;
            }

        }

        // We know it's not in the table. Go add it!
        DebuggerJitInfo *djiPrev = m_latestJitInfo;

        LOG((LF_CORDB,LL_INFO10000,"DMI:CAAJI: current head of dji list:0x%08x\n", djiPrev));

        if (djiPrev != NULL)
        {
            dji->m_prevJitInfo = djiPrev;
            djiPrev->m_nextJitInfo = dji;

            m_latestJitInfo = dji;

            LOG((LF_CORDB,LL_INFO10000,"DMI:CAAJI: DJI version 0x%04x for %s\n",
                 GetCurrentEnCVersion(),
                 dji->m_fd->m_pszDebugMethodName));
        }
        else
        {
            m_latestJitInfo = dji;
        }

    } // DebuggerDataLockHolder out of scope - release implied

    // We've now added a new DJI into the table and released the lock. Thus any other thread
    // can come and use our DJI. Good thing we inited the DJI _before_ adding it to the table.

    LOG((LF_CORDB,LL_INFO10000,"DMI:CAAJI: new head of dji list:0x%08x\n", m_latestJitInfo));

    return dji;
}

/*
 * DeleteJitInfo
 *
 * This routine remove a DJI from the DMI's list and deletes the memory.
 *
 * Parameters:
 *   dji - The DJI to nuke.
 *
 * Returns
 *   None.
 *
 */

void DebuggerMethodInfo::DeleteJitInfo(DebuggerJitInfo *dji)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    Debugger::DebuggerDataLockHolder debuggerDataLockHolder(g_pDebugger);

    LOG((LF_CORDB,LL_INFO10000,"DMI:DJI: dji:0x%08x\n", dji));

    DebuggerJitInfo *djiPrev = dji->m_prevJitInfo;

    if (djiPrev != NULL)
    {
        djiPrev->m_nextJitInfo = dji->m_nextJitInfo;
    }

    if (dji->m_nextJitInfo != NULL)
    {
        dji->m_nextJitInfo->m_prevJitInfo = djiPrev;
    }
    else
    {
        //
        // This DJI is the head of the list
        //
        _ASSERTE(m_latestJitInfo == dji);

        m_latestJitInfo = djiPrev;
    }

    TRACE_FREE(dji);

    DeleteInteropSafe(dji);

    // DebuggerDataLockHolder out of scope - release implied
}

/*
 * DeleteJitInfoList
 *
 * This routine removes all the DJIs from the current DMI.
 *
 * Parameters:
 *   None.
 *
 * Returns
 *   None.
 *
 */

void DebuggerMethodInfo::DeleteJitInfoList(void)
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    Debugger::DebuggerDataLockHolder debuggerDataLockHolder(g_pDebugger);

    while(m_latestJitInfo != NULL)
    {
        DeleteJitInfo(m_latestJitInfo);
    }

    // DebuggerDataLockHolder out of scope - release implied
}


// Iterate through all existing DJIs. See header for expected usage.
DebuggerMethodInfo::DJIIterator::DJIIterator()
{
    LEAF_CONTRACT;

    m_pCurrent = NULL;
}

bool DebuggerMethodInfo::DJIIterator::IsAtEnd()
{
    LEAF_CONTRACT;

    return m_pCurrent == NULL;
}

DebuggerJitInfo * DebuggerMethodInfo::DJIIterator::Current()
{
    LEAF_CONTRACT;

    return m_pCurrent;
}

void DebuggerMethodInfo::DJIIterator::Next()
{
    LEAF_CONTRACT;

    PREFIX_ASSUME(m_pCurrent != NULL); // IsAtEnd() should have caught this.
    m_pCurrent = m_pCurrent->m_prevJitInfo;
}


/******************************************************************************
 * Return true iff this method is jitted
 ******************************************************************************/
bool DebuggerMethodInfo::HasJitInfos()
{
    LEAF_CONTRACT;
    _ASSERTE(g_pDebugger->HasDebuggerDataLock());
    return (m_latestJitInfo != NULL);
}

/******************************************************************************
 * Return true iff this has been EnCed since last time function was jitted.
 ******************************************************************************/
bool DebuggerMethodInfo::HasMoreRecentEnCVersion()
{
    LEAF_CONTRACT;
    return ((m_latestJitInfo != NULL) &&
            (m_currentEnCVersion > m_latestJitInfo->m_encVersion));
}

/******************************************************************************
 * Updated the instrumented-IL map
 ******************************************************************************/
void DebuggerMethodInfo::SetInstrumentedILMap(COR_IL_MAP * pMap, SIZE_T cEntries)
{
    if (m_rgInstrumentedILMap != NULL)
    {
        CoTaskMemFree(m_rgInstrumentedILMap);
    }

    m_cInstrumentedILMap = cEntries;
    m_rgInstrumentedILMap =  pMap;
}

/******************************************************************************
 * Get the JMC status for a given function.
 ******************************************************************************/
bool DebuggerMethodInfo::IsJMCFunction()
{
    LEAF_CONTRACT;
    return m_fJMCStatus;
}

/******************************************************************************
 * Set the JMC status to a given value
 ******************************************************************************/
void DebuggerMethodInfo::SetJMCStatus(bool fStatus)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(g_pDebugger->HasDebuggerDataLock());

    // First check if this is a no-op.
    // Do this first b/c there may be some cases where we don't have a DebuggerModule
    // yet but are still calling SetJMCStatus(false), like if we detach before attach is complete.
    bool fOldStatus = IsJMCFunction();

    if (fOldStatus == fStatus)
    {
        // if no change, then there's nothing to do.
        LOG((LF_CORDB,LL_EVERYTHING, "DMI::SetJMCStatus: %p, keeping old status, %d\n", this, fStatus));
        return;
    }

    // For a perf-optimization, our Module needs to know if it has any user
    // code. If it doesn't, it shouldn't dispatch through the JMC probes.
    // So modules keep a count of # of JMC functions - if the count is 0, the
    // module can set is JMC probe flag to 0 and skip the JMC probes.
    DebuggerModule * pModule = this->GetPrimaryModule();

    // If we're toggling JMC status, we'd better have a debugger module.
    // If the underlying appdomain has been unloaded underneath us, then we'll
    // lose our primary module. However, AD unload also sets the DMI's jmc-status to false,
    // so we shouldn't be here in a process-unload case.
    PREFIX_ASSUME(pModule != NULL);

    // Must do all JMC tracking on the primary module (b/c of shared modules).
    DebuggerModule * pPrimary = pModule->GetPrimaryModule();

    // Update the module's count.
    if (!fStatus)
    {
        LOG((LF_CORDB,LL_EVERYTHING, "DMI::SetJMCStatus: %p, changing to non-user code\n", this));
        _ASSERTE(pPrimary->HasAnyJMCFunctions());
        pPrimary->DecJMCFuncCount();
    }
    else
    {
        LOG((LF_CORDB,LL_EVERYTHING, "DMI::SetJMCStatus: %p, changing to user code\n", this));
        pPrimary->IncJMCFuncCount();
        _ASSERTE(pPrimary->HasAnyJMCFunctions());
    }

    m_fJMCStatus = fStatus;

    // We should update our module's JMC status...
    g_pDebugger->UpdateModuleJMCFlag(pPrimary, DebuggerController::GetTotalMethodEnter() != 0);

}

// Get an iterator that will go through ALL native code-blobs (DJI).
// This is EnC/ Generics / Prejit aware.
void DebuggerMethodInfo::IterateAllDJIs(AppDomain * pAppDomain, DebuggerMethodInfo::DJIIterator * pEnum)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(pEnum != NULL);
    _ASSERTE(pAppDomain != NULL);

    // Esnure we have DJIs for everything.
    CreateDJIsForNativeBlobs(pAppDomain);

    pEnum->m_pCurrent = m_latestJitInfo;
}

// Bring the DJI cache up to date.
void DebuggerMethodInfo::CreateDJIsForNativeBlobs(AppDomain * pAppDomain)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    // If we're not stopped, then it's possible new native blobs are being created underneath us.
    _ASSERTE(g_pDebugger->IsStopped());

    LoadedMethodDescIterator it(pAppDomain, m_module, m_token);
    while(it.Next())
    {
        MethodDesc * pDesc = it.Current();
        if (!pDesc->HasNativeCode())
        {
            continue;
        }

        // We just ask for the DJI to ensure that it's lazily created.
        // This should only fail in an oom scenario.
        DebuggerJitInfo * djiTest = g_pDebugger->GetLatestJitInfoFromMethodDesc(pDesc);
        if (djiTest == NULL)
        {
            // We're oom. Give up.
            ThrowOutOfMemory();
            return;
        }
    }
}


/*
 * GetLatestJitInfo
 *
 * This routine returns the lastest DJI we have for a particular DMI.
 * DJIs are lazily created.
 * Parameters:
 *   None.
 *
 * Returns
 *   a possibly NULL pointer to a DJI.
 *
 */

#ifdef LOGGING
// For logging purposes, provide a non-initializing accessor.
DebuggerJitInfo* DebuggerMethodInfo::Logging_GetLatestJitInfo()
{
    return m_latestJitInfo;
}
#endif


DebuggerMethodInfoTable::DebuggerMethodInfoTable() : CHashTableAndData<CNewZeroData>(101)
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        GC_NOTRIGGER;

        CONSTRUCTOR_CHECK;
    }
    CONTRACTL_END;

    SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;
    HRESULT hr = NewInit(101, sizeof(DebuggerMethodInfoEntry), 101);

    if (FAILED(hr))
    {
        ThrowWin32(hr);
    }
}

HRESULT DebuggerMethodInfoTable::AddMethodInfo(Module *pModule,
                   mdMethodDef token,
                   DebuggerMethodInfo *mi)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;

        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(CheckPointer(mi));
    }
    CONTRACTL_END;

   LOG((LF_CORDB, LL_INFO1000, "DMIT::AMI Adding dmi:0x%x Mod:0x%x tok:"
        "0x%x nVer:0x%x\n", mi, pModule, token, mi->GetCurrentEnCVersion()));

   _ASSERTE(mi != NULL);

    _ASSERTE(g_pDebugger->HasDebuggerDataLock());

    HRESULT hr = OverwriteMethodInfo(pModule, token, mi, TRUE);
    if (hr == S_OK)
        return hr;

    DebuggerMethodInfoKey dmik;
    dmik.pModule = pModule;
    dmik.token = token;

    DebuggerMethodInfoEntry *dmie =
        (DebuggerMethodInfoEntry *) Add(HASH(&dmik));

    if (dmie != NULL)
    {
        dmie->key.pModule = pModule;
        dmie->key.token = token;
        dmie->mi = mi;

        LOG((LF_CORDB, LL_INFO1000, "DMIT::AJI: mod:0x%x tok:0%x ",
            pModule, token));
        return S_OK;
    }

    ThrowOutOfMemory();
    return S_OK;
}

HRESULT DebuggerMethodInfoTable::OverwriteMethodInfo(Module *pModule,
                         mdMethodDef token,
                         DebuggerMethodInfo *mi,
                         BOOL fOnlyIfNull)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;

        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(CheckPointer(mi));
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO1000, "DMIT::OJI: dmi:0x%x mod:0x%x tok:0x%x\n", mi,
        pModule, token));

    _ASSERTE(g_pDebugger->HasDebuggerDataLock());

    DebuggerMethodInfoKey dmik;
    dmik.pModule = pModule;
    dmik.token = token;

    DebuggerMethodInfoEntry *entry
      = (DebuggerMethodInfoEntry *) Find(HASH(&dmik), KEY(&dmik));
    if (entry != NULL)
    {
        if ( (fOnlyIfNull &&
              entry->mi == NULL) ||
             !fOnlyIfNull)
        {
            entry->mi = mi;

            LOG((LF_CORDB, LL_INFO1000, "DMIT::OJI: mod:0x%x tok:0x%x remap"
                "nVer:0x%x\n", pModule, token, entry->nVersionLastRemapped));
            return S_OK;
        }
    }

    return E_FAIL;
}

// pModule is being destroyed - remove any entries that belong to it.  Why?
// (a) Correctness: the module can be reloaded at the same address,
//      which will cause accidental matches with our hashtable (indexed by
//      {Module*,mdMethodDef}
// (b) Perf: don't waste the memory!
void DebuggerMethodInfoTable::ClearMethodsOfModule(Module *pModule)
{
    WRAPPER_CONTRACT;

    _ASSERTE(g_pDebugger->HasDebuggerDataLock()); 

    LOG((LF_CORDB, LL_INFO1000000, "CMOM:mod:0x%x (%S)\n", pModule
        ,pModule->GetDebugName()));

    HASHFIND info;

    DebuggerMethodInfoEntry *entry
      = (DebuggerMethodInfoEntry *) FindFirstEntry(&info);
    while(entry != NULL)
    {
        Module *pMod = entry->key.pModule ;
        if (pMod == pModule)
        {
            // This method actually got mitted, at least
            // once - remove all version info.
            while(entry->mi != NULL)
            {
                DeleteEntryDMI(entry);
            }

            Delete(HASH(&(entry->key)), (HASHENTRY*)entry);
        }

        entry = (DebuggerMethodInfoEntry *) FindNextEntry(&info);
    }
}

void DebuggerMethodInfoTable::RemoveMethodInfo(Module *pModule, mdMethodDef token)
{
    WRAPPER_CONTRACT;

//        CHECK_DMI_TABLE;
    LOG((LF_CORDB, LL_INFO1000000, "RMI:removing mod:0x%x tok:0x%x\n", pModule, token));

    _ASSERTE(g_pDebugger->HasDebuggerDataLock());

    DebuggerMethodInfoKey dmik;
    dmik.pModule = pModule;
    dmik.token = token;

    DebuggerMethodInfoEntry *entry
      = (DebuggerMethodInfoEntry *) Find(HASH(&dmik), KEY(&dmik));

    _ASSERTE(entry != NULL); // it had better be in there!

//        LOG((LF_CORDB,LL_INFO1000000, "Remove entry 0x%x for %s::%s\n",
//            entry, fd->m_pszDebugClassName, fd->m_pszDebugMethodName));

    if (entry != NULL) // if its not, we fail gracefully in a free build
    {
        LOG((LF_CORDB, LL_INFO1000000, "DMI::RMI: for got 0x%x prev:0x%x\n",
            entry->mi, (entry->mi?entry->mi->m_prevMethodInfo:0)));

        // If we remove the hash table entry, we'll lose
        // the version number info, which would be bad.
        // Also, since this is called to undo a failed JIT operation,we
        // shouldn't mess with the version number.
        DeleteEntryDMI(entry);
    }

//        CHECK_DMI_TABLE;
}

void DebuggerMethodInfoTable::DeleteEntryDMI(DebuggerMethodInfoEntry *entry)
{
    LEAF_CONTRACT;

    DebuggerMethodInfo *dmiPrev = entry->mi->m_prevMethodInfo;
    TRACE_FREE(entry->mi);
    DeleteInteropSafe(entry->mi);
    entry->mi = dmiPrev;
    if ( dmiPrev != NULL )
        dmiPrev->m_nextMethodInfo = NULL;
}

#endif // #ifndef DACCESS_COMPILE

DebuggerJitInfo *DebuggerJitInfo::GetJitInfoByAddress(const BYTE *pbAddr )
{
    CONTRACTL
    {
        SO_INTOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    DebuggerJitInfo *dji = this;

#ifdef LOGGING
    LOG((LF_CORDB,LL_INFO10000,"DJI:GJIBA finding DJI "
            "corresponding to addr 0x%p, starting with 0x%p\n", pbAddr, dji));
#endif //LOGGING

    // If it's not NULL, but not in the range m_addrOfCode to end of function,
    //  then get the previous one.
    while( dji != NULL &&
           !CodeRegionInfo::GetCodeRegionInfo(dji).IsMethodAddress(pbAddr))
    {
        LOG((LF_CORDB,LL_INFO10000,"DJI:GJIBA: pbAddr 0x%p is not in code "
            "0x%p (size:0x%p)\n", pbAddr, dji->m_addrOfCode,
            dji->m_sizeOfCode));
        dji = dji->m_prevJitInfo;
    }

#ifdef LOGGING
    if (dji == NULL)
    {
        LOG((LF_CORDB,LL_INFO10000,"DJI:GJIBA couldn't find a DJI "
            "corresponding to addr 0x%p\n", pbAddr));
    }
#endif //LOGGING
    return dji;
}

PTR_DebuggerJitInfo DebuggerMethodInfo::GetLatestJitInfo(MethodDesc *mdesc)
{
    // dac checks ngen'ed image content first, so
    // only check for existing JIT info.
#ifndef DACCESS_COMPILE

    CONTRACTL
    {
        SO_INTOLERANT;
        THROWS;
        CALLED_IN_DEBUGGERDATALOCK_HOLDER_SCOPE_MAY_GC_TRIGGERS_CONTRACT;
        PRECONDITION(!g_pDebugger->HasDebuggerDataLock());
    }
    CONTRACTL_END;


    if (m_latestJitInfo && m_latestJitInfo->m_fd == mdesc && !m_latestJitInfo->m_fd->HasClassOrMethodInstantiation())
        return m_latestJitInfo;

    // This ensures that there is an entry in the DJI list for this particular MethodDesc.
    // in the case of generic code it may not be the first entry in the list.
    FindOrCreateInitAndAddJitInfo(mdesc);

#endif // #ifndef DACCESS_COMPILE

    return m_latestJitInfo;
}

DebuggerMethodInfo *DebuggerMethodInfoTable::GetMethodInfo(Module *pModule, mdMethodDef token)
{
    WRAPPER_CONTRACT;


    if (this == NULL)
        return NULL;

    DebuggerMethodInfoKey dmik;
    dmik.pModule = PTR_Module(PTR_HOST_TO_TADDR(pModule));
    dmik.token = token;

    DebuggerMethodInfoEntry *entry = PTR_DebuggerMethodInfoEntry
        (PTR_HOST_TO_TADDR(Find(HASH(&dmik), KEY(&dmik))));
    if (entry == NULL )
        return NULL;
    else
    {
        LOG((LF_CORDB, LL_INFO1000, "DMI::GMI: for methodDef 0x%x, got 0x%x prev:0x%x\n",
            token, entry->mi, (entry->mi?entry->mi->m_prevMethodInfo:0)));
        return entry->mi;
    }
}


DebuggerMethodInfo *DebuggerMethodInfoTable::GetFirstMethodInfo(HASHFIND *info)
{
    CONTRACT(DebuggerMethodInfo*)
    {
        NOTHROW;
        GC_NOTRIGGER;

        PRECONDITION(CheckPointer(info));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    _ASSERTE(g_pDebugger->HasDebuggerDataLock());

    DebuggerMethodInfoEntry *entry = PTR_DebuggerMethodInfoEntry
        (PTR_HOST_TO_TADDR(FindFirstEntry(info)));
    if (entry == NULL)
        RETURN NULL;
    else
        RETURN entry->mi;
}

DebuggerMethodInfo *DebuggerMethodInfoTable::GetNextMethodInfo(HASHFIND *info)
{
    CONTRACT(DebuggerMethodInfo*)
    {
        NOTHROW;
        GC_NOTRIGGER;

        PRECONDITION(CheckPointer(info));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    _ASSERTE(g_pDebugger->HasDebuggerDataLock());

    DebuggerMethodInfoEntry *entry = PTR_DebuggerMethodInfoEntry
        (PTR_HOST_TO_TADDR(FindNextEntry(info)));

    // We may have incremented the version number
    // for methods that never got JITted, so we should
    // pretend like they don't exist here.
    while (entry != NULL &&
           entry->mi == NULL)
    {
        entry = PTR_DebuggerMethodInfoEntry
            (PTR_HOST_TO_TADDR(FindNextEntry(info)));
    }

    if (entry == NULL)
        RETURN NULL;
    else
        RETURN entry->mi;
}



#ifdef DACCESS_COMPILE
void
DebuggerMethodInfoEntry::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    // This structure is in an array in the hash
    // so the 'this' is implicitly enumerated by the
    // array enum in CHashTable.

    if (key.pModule.IsValid())
    {
        key.pModule->EnumMemoryRegions(flags, true);
    }

    while (mi.IsValid())
    {
        mi->EnumMemoryRegions(flags);
        mi = mi->m_prevMethodInfo;
    }
}

void
DebuggerMethodInfo::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    DAC_ENUM_DTHIS();

    if (m_module.IsValid())
    {
        m_module->EnumMemoryRegions(flags, true);
    }

    PTR_DebuggerJitInfo jitInfo = m_latestJitInfo;
    while (jitInfo.IsValid())
    {
        jitInfo->EnumMemoryRegions(flags);
        jitInfo = jitInfo->m_prevJitInfo;
    }
}

void
DebuggerJitInfo::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    DAC_ENUM_DTHIS();

    if (m_fd.IsValid())
    {
        m_fd->EnumMemoryRegions(flags);
    }
    if (m_methodInfo.IsValid())
    {
        m_methodInfo->EnumMemoryRegions(flags);
    }
    DacEnumMemoryRegion(PTR_TO_TADDR(GetSequenceMap()),
                        GetSequenceMapCount() * sizeof(DebuggerILToNativeMap));
    DacEnumMemoryRegion(PTR_TO_TADDR(GetVarNativeInfo()),
                        GetVarNativeInfoCount() *
                        sizeof(ICorDebugInfo::NativeVarInfo));
}


void DebuggerMethodInfoTable::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;

    DAC_ENUM_VTHIS();
    CHashTableAndData<CNewZeroData>::EnumMemoryRegions(flags);

    for (ULONG i = 0; i < m_iEntries; i++)
    {
        DebuggerMethodInfoEntry* entry =
            PTR_DebuggerMethodInfoEntry(PTR_HOST_TO_TADDR(EntryPtr(i)));
        entry->EnumMemoryRegions(flags);
    }
}
#endif // #ifdef DACCESS_COMPILE


