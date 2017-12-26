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
#include "common.h"

#ifndef FJITONLY

#include "eetwain.h"
#include "dbginterface.h"

#define RETURN_ADDR_OFFS        1       // in DWORDS

#include "gcinfo.h"

#ifdef USE_GC_INFO_DECODER
#include "gcinfodecoder.h"
#endif


#define X86_INSTR_W_TEST_ESP            0x4485  // test [esp+N], eax
#define X86_INSTR_TEST_ESP_SIB          0x24
#define X86_INSTR_PUSH_0                0x6A    // push 00, entire instruction is 0x6A00
#define X86_INSTR_PUSH_IMM              0x68    // push NNNN,
#define X86_INSTR_W_PUSH_IND_IMM        0x35FF  // push [NNNN]
#define X86_INSTR_CALL_REL32            0xE8    // call rel32
#define X86_INSTR_W_CALL_IND_IMM        0x15FF  // call [addr32]
#define X86_INSTR_NOP                   0x90    // nop
#define X86_INSTR_INT3                  0xCC    // int3
#define X86_INSTR_HLT                   0xF4    // hlt
#define X86_INSTR_PUSH_EBP              0x55    // push ebp
#define X86_INSTR_W_MOV_EBP_ESP         0xEC8B  // mov ebp, esp
#define X86_INSTR_POP_ECX               0x59    // pop ecx
#define X86_INSTR_RET                   0xC2    // ret
#define X86_INSTR_w_LEA_ESP_EBP_BYTE_OFFSET     0x658d      // lea esp, [ebp-bOffset]
#define X86_INSTR_w_LEA_ESP_EBP_DWORD_OFFSET    0xa58d      // lea esp, [ebp-dwOffset]
#define X86_INSTR_JMP_NEAR_REL32     0xE9        // near jmp rel32
#define X86_INSTR_w_JMP_FAR_IND_IMM     0x25FF        // far jmp [addr32]

#ifndef USE_GC_INFO_DECODER


#ifdef  _DEBUG
// For dumping of verbose info.
#ifndef DACCESS_COMPILE
static  bool  trFixContext          = false;
#endif
static  bool  trEnumGCRefs          = false;
static  bool  dspPtr                = false; // prints the live ptrs as reported
#endif

// NOTE: enabling compiler optimizations, even for debug builds.  
// Comment this out in order to be able to fully debug methods here.
#if defined(_MSC_VER)
#pragma optimize("tgy", on)
#endif

inline unsigned decodeUnsigned(PTR_CBYTE& src)
{
    LEAF_CONTRACT;

    BYTE     byte  = *src++;
    unsigned value = byte & 0x7f;
    while (byte & 0x80) {
        byte    = *src++;
        value <<= 7;
        value  += byte & 0x7f;
    }
    return value;
}

inline int decodeSigned(PTR_CBYTE& src)
{
    LEAF_CONTRACT;

    BYTE     byte  = *src++;
    BYTE     first = byte;
    int      value = byte & 0x3f;
    while (byte & 0x80)
    {
        byte = *src++;
        value <<= 7;
        value += byte & 0x7f;
    }
    if (first & 0x40)
        value = -value;
    return value;
}

// Fast versions of the above, with one iteration of the loop unrolled
#define fastDecodeUnsigned(src) (((*(src) & 0x80) == 0) ? (unsigned) (*(src)++) : decodeUnsigned((src)))   
#define fastDecodeSigned(src) (((*(src) & 0xC0) == 0) ? (unsigned) (*(src)++) : decodeSigned((src)))

// Fast skipping past encoded integers
#define fastSkipUnsigned(src) { while ((*(src)++) & 0x80) { } }
#define fastSkipSigned(src) { while ((*(src)++) & 0x80) { } }


/*****************************************************************************
 *
 *  Decodes the methodInfoPtr and returns the decoded information
 *  in the hdrInfo struct.  The EIP parameter is the PC location
 *  within the active method.
 */
static size_t   crackMethodInfoHdr(LPVOID      methodInfoPtr,
                                   unsigned    curOffset,
                                   hdrInfo   * infoPtr)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    PTR_CBYTE table = PTR_CBYTE((TADDR)methodInfoPtr);
#if VERIFY_GC_TABLES
    _ASSERTE(*castto(table, unsigned short *)++ == 0xFEEF);
#endif

    infoPtr->methodSize = fastDecodeUnsigned(table);

    _ASSERTE(curOffset >= 0);
    _ASSERTE(curOffset <= infoPtr->methodSize);

    /* Decode the InfoHdr */

    InfoHdr header;
    table = decodeHeader(table, &header);

    BOOL hasArgTabOffset = FALSE;
    if (header.untrackedCnt == HAS_UNTRACKED)
    {
        hasArgTabOffset = TRUE;
        header.untrackedCnt = fastDecodeUnsigned(table);
    }

    if (header.varPtrTableSize == HAS_VARPTR)
    {
        hasArgTabOffset = TRUE;
        header.varPtrTableSize = fastDecodeUnsigned(table);
    }

    if (header.gsCookieOffset == HAS_GS_COOKIE_OFFSET)
    {
        header.gsCookieOffset = fastDecodeUnsigned(table);
    }

    if (header.syncStartOffset == HAS_SYNC_OFFSET)
    {
        header.syncStartOffset = decodeUnsigned(table);
        header.syncEndOffset = decodeUnsigned(table);

        _ASSERTE(header.syncStartOffset != INVALID_SYNC_OFFSET && header.syncEndOffset != INVALID_SYNC_OFFSET);
        _ASSERTE(header.syncStartOffset < header.syncEndOffset);
    }


    /* Some sanity checks on header */

    _ASSERTE( header.prologSize +
           (size_t)(header.epilogCount*header.epilogSize) <= infoPtr->methodSize);
    _ASSERTE( header.epilogCount == 1 || !header.epilogAtEnd);

    _ASSERTE( header.untrackedCnt <= header.argCount+header.frameSize);

    _ASSERTE( header.ebpSaved || !(header.ebpFrame || header.doubleAlign));
    _ASSERTE(!header.ebpFrame || !header.doubleAlign  );
    _ASSERTE( header.ebpFrame || !header.security     );
    _ASSERTE( header.ebpFrame || !header.handlers     );
    _ASSERTE( header.ebpFrame || !header.localloc     );
    _ASSERTE( header.ebpFrame || !header.editNcontinue);

    /* Initialize the infoPtr struct */

    infoPtr->argSize         = header.argCount * 4;
    infoPtr->ebpFrame        = header.ebpFrame;
    infoPtr->interruptible   = header.interruptible;

    infoPtr->prologSize      = header.prologSize;
    infoPtr->epilogSize      = header.epilogSize;
    infoPtr->epilogCnt       = header.epilogCount;
    infoPtr->epilogEnd       = header.epilogAtEnd;

    infoPtr->untrackedCnt    = header.untrackedCnt;
    infoPtr->varPtrTableSize = header.varPtrTableSize;
    infoPtr->gsCookieOffset  = header.gsCookieOffset;

    infoPtr->syncStartOffset = header.syncStartOffset;
    infoPtr->syncEndOffset   = header.syncEndOffset;

    infoPtr->doubleAlign     = header.doubleAlign;
    infoPtr->securityCheck   = header.security;
    infoPtr->handlers        = header.handlers;
    infoPtr->localloc        = header.localloc;
    infoPtr->editNcontinue   = header.editNcontinue;
    infoPtr->varargs         = header.varargs;
    infoPtr->profCallbacks   = header.profCallbacks;
    infoPtr->isSpeculativeStackWalk = false;

    /* Are we within the prolog of the method? */

    if  (curOffset < infoPtr->prologSize)
    {
        infoPtr->prologOffs = curOffset;
    }
    else
    {
        infoPtr->prologOffs = hdrInfo::NOT_IN_PROLOG;
    }

    /* Assume we're not in the epilog of the method */

    infoPtr->epilogOffs = hdrInfo::NOT_IN_EPILOG;

    /* Are we within an epilog of the method? */

    if  (infoPtr->epilogCnt)
    {
        unsigned epilogStart;

        if  (infoPtr->epilogCnt > 1 || !infoPtr->epilogEnd)
        {
#if VERIFY_GC_TABLES
            _ASSERTE(*castto(table, unsigned short *)++ == 0xFACE);
#endif
            epilogStart = 0;
            for (unsigned i = 0; i < infoPtr->epilogCnt; i++)
            {
                epilogStart += fastDecodeUnsigned(table);
                if  (curOffset > epilogStart &&
                     curOffset < epilogStart + infoPtr->epilogSize)
                {
                    infoPtr->epilogOffs = curOffset - epilogStart;
                }
            }
        }
        else
        {
            epilogStart = infoPtr->methodSize - infoPtr->epilogSize;

            if  (curOffset > epilogStart &&
                 curOffset < epilogStart + infoPtr->epilogSize)
            {
                infoPtr->epilogOffs = curOffset - epilogStart;
            }
        }

        infoPtr->syncEpilogStart = epilogStart;
    }

    unsigned argTabOffset = INVALID_ARGTAB_OFFSET;
    if (hasArgTabOffset)
    {
        argTabOffset = fastDecodeUnsigned(table);
    }
    infoPtr->argTabOffset    = argTabOffset;

    size_t frameDwordCount = header.frameSize;

    /* Set the rawStackSize to the number of bytes that it bumps ESP */

    infoPtr->rawStkSize = (UINT)(frameDwordCount * sizeof(size_t));

    /* Calculate the callee saves regMask and adjust stackSize to */
    /* include the callee saves register spills                   */

    unsigned savedRegs = RM_NONE;
    unsigned savedRegsCount = 0;

    if  (header.ediSaved)
    {
        savedRegsCount++;
        savedRegs |= RM_EDI;
    }
    if  (header.esiSaved)
    {
        savedRegsCount++;
        savedRegs |= RM_ESI;
    }
    if  (header.ebxSaved)
    {
        savedRegsCount++;
        savedRegs |= RM_EBX;
    }
    if  (header.ebpSaved)
    {
        savedRegsCount++;
        savedRegs |= RM_EBP;
    }

    infoPtr->savedRegMask = (RegMask)savedRegs;
    
    infoPtr->savedRegsCountExclFP = savedRegsCount;
    if (header.ebpFrame || header.doubleAlign)
    {
        _ASSERTE(header.ebpSaved);
        infoPtr->savedRegsCountExclFP = savedRegsCount - 1;
    }

    frameDwordCount += savedRegsCount;

    infoPtr->stackSize  =  (UINT)(frameDwordCount * sizeof(size_t));

    _ASSERTE(infoPtr->gsCookieOffset == INVALID_GS_COOKIE_OFFSET ||
             (infoPtr->gsCookieOffset < infoPtr->stackSize) &&
             ((header.gsCookieOffset % sizeof(void*)) == 0));
    
    return  PTR_TO_TADDR(table) - (TADDR)methodInfoPtr;
}

/*****************************************************************************/

// We do a "pop eax; jmp eax" to return from a fault or finally handler
const size_t END_FIN_POP_STACK = sizeof(TADDR);


// The offset (in bytes) from EBP for the secutiy object on the stack
inline size_t GetSecurityObjectOffset(hdrInfo * info)
{
    _ASSERTE(info->securityCheck && info->ebpFrame);

    unsigned position = info->savedRegsCountExclFP +
                        1;
    return position * sizeof(TADDR);
}

inline
size_t GetLocallocSPOffset(hdrInfo * info)
{
    _ASSERTE(info->localloc && info->ebpFrame);
    
    unsigned position = info->savedRegsCountExclFP +
                        info->securityCheck +
                        1;
    return position * sizeof(TADDR);
}

inline
size_t GetParamTypeArgOffset(hdrInfo * info)
{
    _ASSERTE(info->handlers && info->ebpFrame);
    
    unsigned position = info->savedRegsCountExclFP +
                        info->securityCheck +
                        info->localloc +
                        1;  // For CORINFO_GENERICS_CTXT_FROM_PARAMTYPEARG
    return position * sizeof(TADDR);
}

inline size_t GetStartShadowSPSlotsOffset(hdrInfo * info)
{
    WRAPPER_CONTRACT;
 
    _ASSERTE(info->handlers && info->ebpFrame);

    return GetParamTypeArgOffset(info) +
           sizeof(TADDR); // Slot for end-of-last-executed-filter
}

/*****************************************************************************
 *  Returns the start of the hidden slots for the shadowSP for functions
 *  with exception handlers. There is one slot per nesting level starting
 *  near Ebp and is zero-terminated after the active slots.
 */

inline
PTR_TADDR GetFirstBaseSPslotPtr(TADDR ebp, hdrInfo * info)
{
    LEAF_CONTRACT;

    _ASSERTE(info->handlers && info->ebpFrame);
    
    size_t offsetFromEBP = GetStartShadowSPSlotsOffset(info) 
                        + sizeof(TADDR); // to get to the *start* of the next slot
                        
    return PTR_TADDR(ebp - offsetFromEBP);
}

inline size_t GetEndShadowSPSlotsOffset(hdrInfo * info, unsigned maxHandlerNestingLevel)
{
    WRAPPER_CONTRACT;

    _ASSERTE(info->handlers && info->ebpFrame);

    unsigned numberOfShadowSPSlots = maxHandlerNestingLevel + 
                                     1 + // For zero-termination
                                     1; // For a filter (which can be active at the same time as a catch/finally handler

    return GetStartShadowSPSlotsOffset(info) +
           (numberOfShadowSPSlots * sizeof(TADDR));
}

// Returns the number of bytes at the beginning of the stack frame that shouldn't be
// modified by an EnC.  This is everything except the space for locals and temporaries.
inline size_t GetSizeOfFrameHeaderForEnC(hdrInfo * info)
{
    WRAPPER_CONTRACT;

    // See comment above Compiler::lvaAssignFrameOffsets() in src\jit\il\lclVars.cpp
    // for frame layout

    // EnC supports increasing the maximum handler nesting level by always
    // assuming that the max is MAX_EnC_HANDLER_NESTING_LEVEL. Methods with
    // a higher max cannot be updated by EnC

    // Take the offset (from EBP) of the last slot of the header, plus one for the EBP slot itself
    // to get the total size of the header.
    return sizeof(TADDR) + 
            GetEndShadowSPSlotsOffset(info, MAX_EnC_HANDLER_NESTING_LEVEL);
}


/*****************************************************************************
 *    returns the base frame pointer corresponding to the target nesting level.
 */

inline
TADDR GetOutermostBaseFP(TADDR ebp, hdrInfo * info)
{
    LEAF_CONTRACT;

    // we are not taking into account double alignment.  We are
    // safe because the jit currently bails on double alignment if there
    // are handles or localalloc
    _ASSERTE(!info->doubleAlign);
    if (info->localloc)
    {
        // If the function uses localloc we will fetch the ESP from the localloc
        // slot.
        PTR_TADDR pLocalloc = PTR_TADDR(ebp - GetLocallocSPOffset(info));

        return (*pLocalloc);
    }
    else
    {
        // Default, go back all the method's local stack size
        return ebp - info->stackSize + sizeof(int);
    }
}

/*****************************************************************************
 *
 *  For functions with handlers, checks if it is currently in a handler.
 *  Either of unwindESP or unwindLevel will specify the target nesting level.
 *  If unwindLevel is specified, info about the funclet at that nesting level
 *    will be returned. (Use if you are interested in a specific nesting level.)
 *  If unwindESP is specified, info for nesting level invoked before the stack
 *   reached unwindESP will be returned. (Use if you have a specific ESP value
 *   during stack walking.)
 *
 *  *pBaseSP is set to the base SP (base of the stack on entry to
 *    the current funclet) corresponding to the target nesting level.
 *  *pNestLevel is set to the nesting level of the target nesting level (useful
 *    if unwindESP!=IGNORE_VAL
 *  *pHasInnerFilter will be set to true (only when unwindESP!=IGNORE_VAL) if a filter
 *    is currently active, but the target nesting level is an outer nesting level.
 *  *pHadInnerFilter - was the last use of the frame to execute a filter.
 *    This mainly affects GC lifetime reporting.
 */

enum FrameType
{
    FR_NORMAL,              // Normal method frame - no exceptions currently active
    FR_FILTER,              // Frame-let of a filter
    FR_HANDLER,             // Frame-let of a callable catch/fault/finally

    FR_INVALID,             // Invalid frame (for speculative stackwalks)
};

enum { IGNORE_VAL = -1 };

FrameType   GetHandlerFrameInfo(hdrInfo   * info,
                                TADDR       frameEBP,
                                TADDR       unwindESP,
                                DWORD       unwindLevel,
                                TADDR     * pBaseSP = NULL,         /* OUT */
                                DWORD     * pNestLevel = NULL,      /* OUT */
                                bool      * pHasInnerFilter = NULL, /* OUT */
                                bool      * pHadInnerFilter = NULL) /* OUT */
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    _ASSERTE(info->ebpFrame && info->handlers);
    // One and only one of them should be IGNORE_VAL
    _ASSERTE((unwindESP == (TADDR) IGNORE_VAL) !=
           (unwindLevel == (DWORD) IGNORE_VAL));
    _ASSERTE(pHasInnerFilter == NULL || unwindESP != (TADDR) IGNORE_VAL);

    // Many of the conditions that we'd like to assert cannot be asserted in the case that we're
    // in the middle of a stackwalk seeded by a profiler, since such seeds can't be trusted
    // (profilers are external, untrusted sources).  So during profiler walks, we test the condition
    // and throw an exception if it's not met.  Otherwise, we just assert the condition. 
    #define FAIL_IF_SPECULATIVE_WALK(condition)         \
        if (info->isSpeculativeStackWalk)               \
        {                                               \
            if (!(condition))                           \
            {                                           \
                return FR_INVALID;                      \
            }                                           \
        }                                               \
        else                                            \
        {                                               \
            _ASSERTE(condition);                        \
        }

    PTR_TADDR pFirstBaseSPslot = GetFirstBaseSPslotPtr(frameEBP, info);
    TADDR  baseSP            = GetOutermostBaseFP(frameEBP, info);
    bool    nonLocalHandlers = false; // Are the funclets invoked by EE (instead of managed code itself)
    bool    hasInnerFilter   = false;
    bool    hadInnerFilter   = false;

    /* Get the last non-zero slot >= unwindESP, or lvl<unwindLevel.
       Also do some sanity checks */

    size_t lvl;
    PTR_TADDR pSlot;
    for(lvl = 0, pSlot = pFirstBaseSPslot;
        *pSlot && lvl < unwindLevel;
        pSlot--, lvl++)
    {
        // Filters cant have inner funclets
        FAIL_IF_SPECULATIVE_WALK(!(baseSP & ICodeManager::SHADOW_SP_IN_FILTER));

        TADDR curSlotVal = *pSlot;

        // The shadowSPs have to be less unless the stack has been unwound.
        FAIL_IF_SPECULATIVE_WALK(baseSP >  curSlotVal ||
               (baseSP == curSlotVal && pSlot == pFirstBaseSPslot));

        if (curSlotVal == LCL_FINALLY_MARK)
        {
            // Locally called finally
            baseSP -= sizeof(TADDR);
        }
        else
        {
            // Is this a funclet we unwound before (can only happen with filters) ?

            if (unwindESP != (TADDR) IGNORE_VAL &&
                unwindESP > END_FIN_POP_STACK +
                (curSlotVal & ~ICodeManager::SHADOW_SP_BITS))
            {
                // Filter cant have nested handlers
                FAIL_IF_SPECULATIVE_WALK((pSlot[0] & ICodeManager::SHADOW_SP_IN_FILTER) &&
                       (pSlot[-1] == 0));
                FAIL_IF_SPECULATIVE_WALK(!(baseSP & ICodeManager::SHADOW_SP_IN_FILTER));

                if (pSlot[0] & ICodeManager::SHADOW_SP_FILTER_DONE)
                    hadInnerFilter = true;
                else
                    hasInnerFilter = true;
                break;
            }

            nonLocalHandlers = true;
            baseSP = curSlotVal;
        }
    }

    if (unwindESP != (TADDR) IGNORE_VAL)
    {
        FAIL_IF_SPECULATIVE_WALK(baseSP >= unwindESP ||
               baseSP == unwindESP - sizeof(TADDR));  // About to locally call a finally

        if (baseSP < unwindESP)                       // About to locally call a finally
            baseSP = unwindESP;
    }
    else
    {
        FAIL_IF_SPECULATIVE_WALK(lvl == unwindLevel); // unwindLevel must be currently active on stack
    }

    if (pBaseSP)
        *pBaseSP = baseSP & ~ICodeManager::SHADOW_SP_BITS;

    if (pNestLevel)
    {
        *pNestLevel = (DWORD)lvl;
    }

    if (pHasInnerFilter)
        *pHasInnerFilter = hasInnerFilter;

    if (pHadInnerFilter)
        *pHadInnerFilter = hadInnerFilter;

    if (baseSP & ICodeManager::SHADOW_SP_IN_FILTER)
    {
        FAIL_IF_SPECULATIVE_WALK(!hasInnerFilter); // nested filters not allowed
        return FR_FILTER;
    }
    else if (nonLocalHandlers)
    {
        return FR_HANDLER;
    }
    else
    {
        return FR_NORMAL;
    }

    #undef FAIL_IF_SPECULATIVE_WALK
}

#endif // !USE_GC_INFO_DECODER


#ifndef DACCESS_COMPILE

/*****************************************************************************
 *
 *  Setup context to enter an exception handler (a 'catch' block).
 *  This is the last chance for the runtime support to do fixups in
 *  the context before execution continues inside a filter, catch handler,
 *  or finally.
 */
void EECodeManager::FixContext( ContextType     ctxType,
                                EHContext      *ctx,
                                LPVOID          methodInfoPtr,
                                LPVOID          methodStart,
                                DWORD           nestingLevel,
                                OBJECTREF       thrownObject,
                                CodeManState   *pState,
                                size_t       ** ppShadowSP,
                                size_t       ** ppEndRegion)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    _ASSERTE((ctxType == FINALLY_CONTEXT) == (thrownObject == NULL));

#ifdef _X86_

    _ASSERTE(sizeof(CodeManStateBuf) <= sizeof(pState->stateBuf));
    CodeManStateBuf * stateBuf = (CodeManStateBuf*)pState->stateBuf;

    /* Extract the necessary information from the info block header */

    stateBuf->hdrInfoSize = (DWORD)crackMethodInfoHdr(methodInfoPtr,
                                       ExecutionManager::JitCodeToOffset((TADDR) ctx->Eip),
                                       &stateBuf->hdrInfoBody);
    pState->dwIsSet = 1;

#ifdef  _DEBUG
    if (trFixContext) {
        printf("FixContext [%s][%s] for %s.%s: ",
               stateBuf->hdrInfoBody.ebpFrame?"ebp":"   ",
               stateBuf->hdrInfoBody.interruptible?"int":"   ",
               "UnknownClass","UnknownMethod");
        fflush(stdout);
    }
#endif

    /* make sure that we have an ebp stack frame */

    _ASSERTE(stateBuf->hdrInfoBody.ebpFrame);
    _ASSERTE(stateBuf->hdrInfoBody.handlers);

    TADDR      baseSP;
    GetHandlerFrameInfo(&stateBuf->hdrInfoBody, ctx->Ebp,
                                ctxType == FILTER_CONTEXT ? ctx->Esp : IGNORE_VAL,
                                ctxType == FILTER_CONTEXT ? (DWORD) IGNORE_VAL : nestingLevel,
                                &baseSP,
                                &nestingLevel);

    _ASSERTE((size_t)ctx->Ebp >= baseSP && baseSP >= (size_t)ctx->Esp);

    ctx->Esp = (DWORD)baseSP;

    // EE will write Esp to **pShadowSP before jumping to handler

    PTR_TADDR pBaseSPslots =
        GetFirstBaseSPslotPtr(ctx->Ebp, &stateBuf->hdrInfoBody);
    *ppShadowSP = (size_t *)&pBaseSPslots[-(int) nestingLevel   ];
                   pBaseSPslots[-(int)(nestingLevel+1)] = 0; // Zero out the next slot

    // EE will write the end offset of the filter
    if (ctxType == FILTER_CONTEXT)
        *ppEndRegion = (size_t *)pBaseSPslots + 1;

    /*  This is just a simple assigment of throwObject to ctx->Eax,
        just pretend the cast goo isn't there.
     */

    *((OBJECTREF*)&(ctx->Eax)) = thrownObject;

#else // !_X86_
    _ASSERTE(!"@NYI - EECodeManager::FixContext (EETwain.cpp)");
#endif // _X86_
}





/*****************************************************************************/

bool        VarIsInReg(ICorDebugInfo::VarLoc varLoc)
{
    LEAF_CONTRACT;

    switch(varLoc.vlType)
    {
    case ICorDebugInfo::VLT_REG:
    case ICorDebugInfo::VLT_REG_REG:
    case ICorDebugInfo::VLT_REG_STK:
        return true;

    default:
        return false;
    }
}


#endif // #ifndef DACCESS_COMPILE

#ifdef USE_GC_INFO_DECODER
/*****************************************************************************
 *
 *  Is the function currently at a "GC safe point" ?
 */
bool EECodeManager::IsGcSafe( PREGDISPLAY     pContext,
                              LPVOID          methodInfoPtr,
                              ICodeInfo      *pCodeInfo,
                              unsigned        flags)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    unsigned          codeOffset;

    IJitManager* pEEJM;
    pEEJM = ExecutionManager::FindJitMan((BYTE*)GetControlPC(pContext));
    _ASSERTE(pEEJM);

    IJitManager::ScanFlag fJitManagerScanFlags = IJitManager::GetScanFlags();

    pEEJM->JitCodeToMethodInfo((BYTE*)GetControlPC(pContext), 
                               NULL, 
                               NULL,
                               (DWORD*)&(codeOffset), 
                               fJitManagerScanFlags);

#ifndef DACCESS_COMPILE
    BYTE* gcInfoAddr = (BYTE*) methodInfoPtr;

    GcInfoDecoder gcInfoDecoder(
            gcInfoAddr,
            DECODE_INTERRUPTIBILITY,
            codeOffset
            );

    return gcInfoDecoder.IsInterruptible();
#else
    DacNotImpl();
    return false;
#endif // #ifndef DACCESS_COMPILE
}




#else // !USE_GC_INFO_DECODER

/*****************************************************************************
 *
 *  Is the function currently at a "GC safe point" ?
 */
bool EECodeManager::IsGcSafe( PREGDISPLAY     pContext,
                              LPVOID          methodInfoPtr,
                              ICodeInfo      *pCodeInfo,
                              unsigned        flags)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    hdrInfo         info;
    BYTE    *       table;

    // Address where the method has been interrupted
    DWORD    *       breakPC = (DWORD *) *(pContext->pPC);

    /* Extract the necessary information from the info block header */

    DWORD offset;
    pCodeInfo->getJitManager()->JitCodeToMethodInfo((SLOT)breakPC, NULL, NULL, &offset);
    table = (BYTE *)crackMethodInfoHdr(methodInfoPtr,
                                       offset,
                                       &info);

    /* HACK: prevent interruption within prolog/epilog */

    if  (info.prologOffs != hdrInfo::NOT_IN_PROLOG || info.epilogOffs != hdrInfo::NOT_IN_EPILOG)
        return false;

#if VERIFY_GC_TABLES
    _ASSERTE(*castto(table, unsigned short *)++ == 0xBEEF);
#endif

    if  (!info.interruptible)
        return false;


    return true;
}


/*****************************************************************************/
static
PTR_CBYTE skipToArgReg(const hdrInfo& info, PTR_CBYTE table)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

#ifdef _DEBUG
    PTR_CBYTE tableStart = table;
#else
    if (info.argTabOffset != INVALID_ARGTAB_OFFSET)
    {
        return table + info.argTabOffset;
    }
#endif

    unsigned count;

#if VERIFY_GC_TABLES
    _ASSERTE(*castto(table, unsigned short *)++ == 0xBEEF);
#endif

    /* Skip over the untracked frame variable table */

    count = info.untrackedCnt;
    while (count-- > 0) {
        fastSkipSigned(table);
    }

#if VERIFY_GC_TABLES
    _ASSERTE(*castto(table, unsigned short *)++ == 0xCAFE);
#endif

    /* Skip over the frame variable lifetime table */

    count = info.varPtrTableSize;
    while (count-- > 0) {
        fastSkipUnsigned(table); fastSkipUnsigned(table); fastSkipUnsigned(table);
    }

#if VERIFY_GC_TABLES
    _ASSERTE(*castto(table, unsigned short *) == 0xBABE);
#endif

#ifdef _DEBUG
    if (info.argTabOffset != INVALID_ARGTAB_OFFSET)
    {
        CONSISTENCY_CHECK_MSGF((info.argTabOffset == (unsigned) (table - tableStart)), 
          ("table = %p, tableStart = %p, info.argTabOffset = %d", table, tableStart, info.argTabOffset));
    }
#endif

    return table;
}

/*****************************************************************************/

#define regNumToMask(regNum) RegMask(1<<regNum)

/*****************************************************************************
 Helper for scanArgRegTable() and scanArgRegTableI() for regMasks
 */

void *      getCalleeSavedReg(PREGDISPLAY pContext, regNum reg)
{
    LEAF_CONTRACT;

    switch (reg)
    {
        case REGI_EBP: return pContext->pEbp;
        case REGI_EBX: return pContext->pEbx;
        case REGI_ESI: return pContext->pEsi;
        case REGI_EDI: return pContext->pEdi;

        default: _ASSERTE(!"bad info.thisPtrResult"); return NULL;
    }
}

/*****************************************************************************
 These functions converts the bits in the GC encoding to RegMask
 */

inline
RegMask     convertCalleeSavedRegsMask(unsigned inMask) // EBP,EBX,ESI,EDI
{
    LEAF_CONTRACT;

    _ASSERTE((inMask & 0x0F) == inMask);

    unsigned outMask = RM_NONE;
    if (inMask & 0x1) outMask |= RM_EDI;
    if (inMask & 0x2) outMask |= RM_ESI;
    if (inMask & 0x4) outMask |= RM_EBX;
    if (inMask & 0x8) outMask |= RM_EBP;

    return (RegMask) outMask;
}

inline
RegMask     convertAllRegsMask(unsigned inMask) // EAX,ECX,EDX,EBX, EBP,ESI,EDI
{
    LEAF_CONTRACT;

    _ASSERTE((inMask & 0xEF) == inMask);

    unsigned outMask = RM_NONE;
    if (inMask & 0x01) outMask |= RM_EAX;
    if (inMask & 0x02) outMask |= RM_ECX;
    if (inMask & 0x04) outMask |= RM_EDX;
    if (inMask & 0x08) outMask |= RM_EBX;
    if (inMask & 0x20) outMask |= RM_EBP;
    if (inMask & 0x40) outMask |= RM_ESI;
    if (inMask & 0x80) outMask |= RM_EDI;

    return (RegMask)outMask;
}

/*****************************************************************************
 * scan the register argument table for the not fully interruptible case.
   this function is called to find all live objects (pushed arguments)
   and to get the stack base for EBP-less methods.

   NOTE: If info->argTabResult is NULL, info->argHnumResult indicates
         how many bits in argMask are valid
         If info->argTabResult is non-NULL, then the argMask field does
         not fit in 32-bits and the value in argMask meaningless.
         Instead argHnum specifies the number of (variable-lenght) elements
         in the array, and argTabBytes specifies the total byte size of the
         array. [ Note this is an extremely rare case ]
 */

static
unsigned scanArgRegTable(PTR_CBYTE     table,
                         unsigned     curOffs,
                         hdrInfo    * info)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    regNum    thisPtrReg    = REGI_NA;
#ifdef _DEBUG
    bool      isCall        = false;
#endif
    unsigned  regMask       = 0;    // EBP,EBX,ESI,EDI
    unsigned  argMask       = 0;
    unsigned  argHnum       = 0;
    const BYTE * argTab        = 0;
    unsigned  argTabBytes   = 0;
    unsigned  stackDepth    = 0;

    unsigned  iregMask      = 0;    // EBP,EBX,ESI,EDI
    unsigned  iargMask      = 0;
    unsigned  iptrMask      = 0;

#if VERIFY_GC_TABLES
    _ASSERTE(*castto(table, unsigned short *)++ == 0xBABE);
#endif

    unsigned scanOffs = 0;

    _ASSERTE(scanOffs <= info->methodSize);

    if (info->ebpFrame) {
  /*
      Encoding table for methods with an EBP frame and
                         that are not fully interruptible

      The encoding used is as follows:

      this pointer encodings:

         01000000          this pointer in EBX
         00100000          this pointer in ESI
         00010000          this pointer in EDI

      tiny encoding:

         0bsdDDDD
                           requires code delta     < 16 (4-bits)
                           requires pushed argmask == 0

           where    DDDD   is code delta
                       b   indicates that register EBX is a live pointer
                       s   indicates that register ESI is a live pointer
                       d   indicates that register EDI is a live pointer

      small encoding:

         1DDDDDDD bsdAAAAA

                           requires code delta     < 120 (7-bits)
                           requires pushed argmask <  64 (5-bits)

           where DDDDDDD   is code delta
                   AAAAA   is the pushed args mask
                       b   indicates that register EBX is a live pointer
                       s   indicates that register ESI is a live pointer
                       d   indicates that register EDI is a live pointer

      medium encoding

         0xFD aaaaaaaa AAAAdddd bseDDDDD

                           requires code delta     <    0x1000000000  (9-bits)
                           requires pushed argmask < 0x1000000000000 (12-bits)

           where    DDDDD  is the upper 5-bits of the code delta
                     dddd  is the low   4-bits of the code delta
                     AAAA  is the upper 4-bits of the pushed arg mask
                 aaaaaaaa  is the low   8-bits of the pushed arg mask
                        b  indicates that register EBX is a live pointer
                        s  indicates that register ESI is a live pointer
                        e  indicates that register EDI is a live pointer

      medium encoding with interior pointers

         0xF9 DDDDDDDD bsdAAAAAA iiiIIIII

                           requires code delta     < (8-bits)
                           requires pushed argmask < (5-bits)

           where  DDDDDDD  is the code delta
                        b  indicates that register EBX is a live pointer
                        s  indicates that register ESI is a live pointer
                        d  indicates that register EDI is a live pointer
                    AAAAA  is the pushed arg mask
                      iii  indicates that EBX,EDI,ESI are interior pointers
                    IIIII  indicates that bits is the arg mask are interior
                           pointers

      large encoding

         0xFE [0BSD0bsd][32-bit code delta][32-bit argMask]

                        b  indicates that register EBX is a live pointer
                        s  indicates that register ESI is a live pointer
                        d  indicates that register EDI is a live pointer
                        B  indicates that register EBX is an interior pointer
                        S  indicates that register ESI is an interior pointer
                        D  indicates that register EDI is an interior pointer
                           requires pushed  argmask < 32-bits

      large encoding  with interior pointers

         0xFA [0BSD0bsd][32-bit code delta][32-bit argMask][32-bit interior pointer mask]


                        b  indicates that register EBX is a live pointer
                        s  indicates that register ESI is a live pointer
                        d  indicates that register EDI is a live pointer
                        B  indicates that register EBX is an interior pointer
                        S  indicates that register ESI is an interior pointer
                        D  indicates that register EDI is an interior pointer
                           requires pushed  argmask < 32-bits
                           requires pushed iArgmask < 32-bits

      huge encoding        This is the only encoding that supports
                           a pushed argmask which is greater than
                           32-bits.

         0xFB [0BSD0bsd][32-bit code delta]
              [32-bit table count][32-bit table size]
              [pushed ptr offsets table...]

                       b   indicates that register EBX is a live pointer
                       s   indicates that register ESI is a live pointer
                       d   indicates that register EDI is a live pointer
                       B   indicates that register EBX is an interior pointer
                       S   indicates that register ESI is an interior pointer
                       D   indicates that register EDI is an interior pointer
                       the list count is the number of entries in the list
                       the list size gives the byte-lenght of the list
                       the offsets in the list are variable-length
  */
        while (scanOffs < curOffs)
        {
            iregMask =
            iargMask = 0;
            argTab = NULL;
#ifdef _DEBUG
            isCall = true;
#endif

            /* Get the next byte and check for a 'special' entry */

            unsigned encType = *table++;

            switch (encType)
            {
                unsigned    val, nxt;

            default:

                /* A tiny or small call entry */
                val = encType;
                if ((val & 0x80) == 0x00) {
                    if (val & 0x0F) {
                        /* A tiny call entry */
                        scanOffs += (val & 0x0F);
                        regMask   = (val & 0x70) >> 4;
                        argMask   = 0;
                        argHnum   = 0;
                    }
                    else {
                        /* This pointer liveness encoding */
                        regMask   = (val & 0x70) >> 4;
                        if (regMask == 0x1)
                            thisPtrReg = REGI_EDI;
                        else if (regMask == 0x2)
                            thisPtrReg = REGI_ESI;
                        else if (regMask == 0x4)
                            thisPtrReg = REGI_EBX;
                        else
                           _ASSERTE(!"illegal encoding for 'this' pointer liveness");
                    }
                }
                else {
                    /* A small call entry */
                    scanOffs += (val & 0x7F);
                    val       = *table++;
                    regMask   = val >> 5;
                    argMask   = val & 0x1F;
                    argHnum   = 5;
                }
                break;

            case 0xFD:  // medium encoding

                argMask   = *table++;
                val       = *table++;
                argMask  |= (val & 0xF0) << 4;
                argHnum   = 12;
                nxt       = *table++;
                scanOffs += (val & 0x0F) + ((nxt & 0x1F) << 4);
                regMask   = nxt >> 5;                   // EBX,ESI,EDI

                break;

            case 0xF9:  // medium encoding with interior pointers

                scanOffs   += *table++;
                val         = *table++;
                argMask     = val & 0x1F;
                argHnum     = 5;
                regMask     = val >> 5;
                val         = *table++;
                iargMask    = val & 0x1F;
                iregMask    = val >> 5;

                break;

            case 0xFE:  // large encoding
            case 0xFA:  // large encoding with interior pointers

                val         = *table++;
                regMask     = val & 0x7;
                iregMask    = val >> 4;
                scanOffs   += *PTR_DWORD(PTR_TO_TADDR(table));  table += sizeof(DWORD);
                argMask     = *PTR_DWORD(PTR_TO_TADDR(table));  table += sizeof(DWORD);
                argHnum     = 31;
                if (encType == 0xFA) // read iargMask
                {
                    iargMask = *PTR_DWORD(PTR_TO_TADDR(table)); table += sizeof(DWORD);
                }
                break;

            case 0xFB:  // huge encoding

                val         = *table++;
                regMask     = val & 0x7;
                iregMask    = val >> 4;
                scanOffs   += *PTR_DWORD(PTR_TO_TADDR(table)); table += sizeof(DWORD);
                argHnum     = *PTR_DWORD(PTR_TO_TADDR(table)); table += sizeof(DWORD);
                argTabBytes = *PTR_DWORD(PTR_TO_TADDR(table)); table += sizeof(DWORD);
                argTab      = table;                       table += argTabBytes;

                argMask     = 0xdeadbeef;
                break;

            case 0xFF:
                scanOffs = curOffs + 1;
                break;

            } // end case

            // iregMask & iargMask are subsets of regMask & argMask respectively

            _ASSERTE((iregMask & regMask) == iregMask);
            _ASSERTE((iargMask & argMask) == iargMask);

        } // end while

    }
    else {

/*
 *    Encoding table for methods without an EBP frame and are not fully interruptible
 *
 *               The encoding used is as follows:
 *
 *  push     000DDDDD                     ESP push one item with 5-bit delta
 *  push     00100000 [pushCount]         ESP push multiple items
 *  reserved 0011xxxx
 *  skip     01000000 [Delta]             Skip Delta, arbitrary sized delta
 *  skip     0100DDDD                     Skip small Delta, for call (DDDD != 0)
 *  pop      01CCDDDD                     ESP pop  CC items with 4-bit delta (CC != 00)
 *  call     1PPPPPPP                     Call Pattern, P=[0..79]
 *  call     1101pbsd DDCCCMMM            Call RegMask=pbsd,ArgCnt=CCC,
 *                                        ArgMask=MMM Delta=commonDelta[DD]
 *  call     1110pbsd [ArgCnt] [ArgMask]  Call ArgCnt,RegMask=pbsd,ArgMask
 *  call     11111000 [PBSDpbsd][32-bit delta][32-bit ArgCnt]
 *                    [32-bit PndCnt][32-bit PndSize][PndOffs...]
 *  iptr     11110000 [IPtrMask]          Arbitrary Interior Pointer Mask
 *  thisptr  111101RR                     This pointer is in Register RR
 *                                        00=EDI,01=ESI,10=EBX,11=EBP
 *  reserved 111100xx                     xx  != 00
 *  reserved 111110xx                     xx  != 00
 *  reserved 11111xxx                     xxx != 000 && xxx != 111(EOT)
 *
 *   The value 11111111 [0xFF] indicates the end of the table.
 *
 *  An offset (at which stack-walking is performed) without an explicit encoding
 *  is assumed to be a trivial call-site (no GC registers, stack empty before and
 *  after) to avoid having to encode all trivial calls.
 *
 * Note on the encoding used for interior pointers
 *
 *   The iptr encoding must immediately preceed a call encoding.  It is used to
 *   transform a normal GC pointer addresses into an interior pointers for GC purposes.
 *   The mask supplied to the iptr encoding is read from the least signicant bit
 *   to the most signicant bit. (i.e the lowest bit is read first)
 *
 *   p   indicates that register EBP is a live pointer
 *   b   indicates that register EBX is a live pointer
 *   s   indicates that register ESI is a live pointer
 *   d   indicates that register EDI is a live pointer
 *   P   indicates that register EBP is an interior pointer
 *   B   indicates that register EBX is an interior pointer
 *   S   indicates that register ESI is an interior pointer
 *   D   indicates that register EDI is an interior pointer
 *
 *   As an example the following sequence indicates that EDI.ESI and the 2nd pushed pointer
 *   in ArgMask are really interior pointers.  The pointer in ESI in a normal pointer:
 *
 *   iptr 11110000 00010011           => read Interior Ptr, Interior Ptr, Normal Ptr, Normal Ptr, Interior Ptr
 *   call 11010011 DDCCC011 RRRR=1011 => read EDI is a GC-pointer, ESI is a GC-pointer. EBP is a GC-pointer
 *                           MMM=0011 => read two GC-pointers arguments on the stack (nested call)
 *
 *   Since the call instruction mentions 5 GC-pointers we list them in the required order:
 *   EDI, ESI, EBP, 1st-pushed pointer, 2nd-pushed pointer
 *
 *   And we apply the Interior Pointer mask mmmm=10011 to the above five ordered GC-pointers
 *   we learn that EDI and ESI are interior GC-pointers and that the second push arg is an
 *   interior GC-pointer.
 */

        while (scanOffs <= curOffs)
        {
            unsigned callArgCnt;
            unsigned skip;
            unsigned newRegMask, inewRegMask;
            unsigned newArgMask, inewArgMask;
            unsigned oldScanOffs = scanOffs;

            if (iptrMask)
            {
                // We found this iptrMask in the previous iteration.
                // This iteration must be for a call. Set these variables
                // so that they are available at the end of the loop

                inewRegMask = iptrMask & 0x0F; // EBP,EBX,ESI,EDI
                inewArgMask = iptrMask >> 4;

                iptrMask    = 0;
            }
            else
            {
                // Zero out any stale values.

                inewRegMask =
                inewArgMask = 0;
            }

            /* Get the next byte and decode it */

            unsigned val = *table++;

#ifdef _DEBUG
            if (scanOffs != curOffs)
                isCall = false;
#endif

            /* Check pushes, pops, and skips */

            if  (!(val & 0x80)) {

                //  iptrMask can immediately precede only calls

                _ASSERTE(!inewRegMask & !inewArgMask);

                if (!(val & 0x40)) {

                    unsigned pushCount;

                    if (!(val & 0x20))
                    {
                        //
                        // push    000DDDDD                 ESP push one item, 5-bit delta
                        //
                        pushCount   = 1;
                        scanOffs   += val & 0x1f;
                    }
                    else
                    {
                        //
                        // push    00100000 [pushCount]     ESP push multiple items
                        //
                        _ASSERTE(val == 0x20);
                        pushCount = fastDecodeUnsigned(table);
                    }

                    if (scanOffs > curOffs)
                    {
                        scanOffs = oldScanOffs;
                        goto FINISHED;
                    }

                    stackDepth +=  pushCount;
                }
                else if ((val & 0x3f) != 0) {
                    //
                    //  pop     01CCDDDD         pop CC items, 4-bit delta
                    //
                    scanOffs   +=  val & 0x0f;
                    if (scanOffs > curOffs)
                    {
                        scanOffs = oldScanOffs;
                        goto FINISHED;
                    }
                    stackDepth -= (val & 0x30) >> 4;

                } else if (scanOffs < curOffs) {
                    //
                    // skip    01000000 [Delta]  Skip arbitrary sized delta
                    //
                    skip = fastDecodeUnsigned(table);
                    scanOffs += skip;
                }
                else // don't process a skip if we are already at curOffs
                    goto FINISHED;

                /* reset regs and args state since we advance past last call site */

                 regMask    =
                iregMask    = 0;
                 argMask    =
                iargMask    = 0;
                argHnum     = 0;

            }
            else /* It must be a call, thisptr, or iptr */
            {
                switch ((val & 0x70) >> 4) {
                default:    // case 0-4, 1000xxxx through 1100xxxx
                    //
                    // call    1PPPPPPP          Call Pattern, P=[0..79]
                    //
                    decodeCallPattern((val & 0x7f), &callArgCnt,
                                      &newRegMask, &newArgMask, &skip);
                    // If we've already reached curOffs and the skip amount
                    // is non-zero then we are done
                    if ((scanOffs == curOffs) && (skip > 0))
                        goto FINISHED;
                    // otherwise process this call pattern
                    scanOffs   += skip;
                    if (scanOffs > curOffs)
                        goto FINISHED;
#ifdef _DEBUG
                    isCall      = true;
#endif
                    regMask     = newRegMask;
                    argMask     = newArgMask;   argTab = NULL;
                    iregMask    = inewRegMask;
                    iargMask    = inewArgMask;
                    stackDepth -= callArgCnt;
                    argHnum     = 2;             // argMask is known to be <= 3
                    break;

                  case 5:
                    //
                    // call    1101RRRR DDCCCMMM  Call RegMask=RRRR,ArgCnt=CCC,
                    //                        ArgMask=MMM Delta=commonDelta[DD]
                    //
                    newRegMask  = val & 0xf;    // EBP,EBX,ESI,EDI
                    val         = *table++;     // read next byte
                    skip        = callCommonDelta[val>>6];
                    // If we've already reached curOffs and the skip amount
                    // is non-zero then we are done
                    if ((scanOffs == curOffs) && (skip > 0))
                        goto FINISHED;
                    // otherwise process this call encoding
                    scanOffs   += skip;
                    if (scanOffs > curOffs)
                        goto FINISHED;
#ifdef _DEBUG
                    isCall      = true;
#endif
                    regMask     = newRegMask;
                    iregMask    = inewRegMask;
                    callArgCnt  = (val >> 3) & 0x7;
                    stackDepth -= callArgCnt;
                    argMask     = (val & 0x7);  argTab = NULL;
                    iargMask    = inewArgMask;
                    argHnum     = 3;
                    break;

                  case 6:
                    //
                    // call    1110RRRR [ArgCnt] [ArgMask]
                    //                          Call ArgCnt,RegMask=RRR,ArgMask
                    //
#ifdef _DEBUG
                    isCall      = true;
#endif
                    regMask     = val & 0xf;    // EBP,EBX,ESI,EDI
                    iregMask    = inewRegMask;
                    callArgCnt = fastDecodeUnsigned(table);
                    stackDepth -= callArgCnt;
                    argMask = fastDecodeUnsigned(table);  argTab = NULL;
                    iargMask    = inewArgMask;
                    argHnum     = 31;
                    break;

                  case 7:
                    switch (val & 0x0C)
                    {
                      case 0x00:
                        //
                        //  iptr 11110000 [IPtrMask] Arbitrary Interior Pointer Mask
                        //
                        iptrMask = fastDecodeUnsigned(table);
                        break;

                      case 0x04:
                        {
                            static const regNum calleeSavedRegs[] =
                                { REGI_EDI, REGI_ESI, REGI_EBX, REGI_EBP };
                            thisPtrReg = calleeSavedRegs[val&0x3];
                        }
                        break;

                      case 0x08:
                        val         = *table++;
                        skip        = *PTR_DWORD(PTR_TO_TADDR(table)); table += sizeof(DWORD);
                        scanOffs   += skip;
                        if (scanOffs > curOffs)
                            goto FINISHED;
#ifdef _DEBUG
                        isCall      = true;
#endif
                        regMask     = val & 0xF;
                        iregMask    = val >> 4;
                        callArgCnt  = *PTR_DWORD(PTR_TO_TADDR(table)); table += sizeof(DWORD);
                        stackDepth -= callArgCnt;
                        argHnum     = *PTR_DWORD(PTR_TO_TADDR(table)); table += sizeof(DWORD);
                        argTabBytes = *PTR_DWORD(PTR_TO_TADDR(table)); table += sizeof(DWORD);
                        argTab      = table;
                        table      += argTabBytes;
                        break;

                      case 0x0C:
                        // end of the table
                        _ASSERTE(val==0xff);
                        goto FINISHED;

                      default:
                        _ASSERTE(!"reserved GC encoding");
                        break;
                    }
                    break;

                } // end switch

            } // end else (!(val & 0x80))

            // iregMask & iargMask are subsets of regMask & argMask respectively

            _ASSERTE((iregMask & regMask) == iregMask);
            _ASSERTE((iargMask & argMask) == iargMask);

        } // end while

    } // end else ebp-less frame

FINISHED:

    // iregMask & iargMask are subsets of regMask & argMask respectively

    _ASSERTE((iregMask & regMask) == iregMask);
    _ASSERTE((iargMask & argMask) == iargMask);

    if (scanOffs != curOffs)
    {
        /* must have been a boring call */
        info->regMaskResult  = RM_NONE;
        info->argMaskResult  = 0;
        info->iregMaskResult = RM_NONE;
        info->iargMaskResult = 0;
        info->argHnumResult  = 0;
        info->argTabResult   = NULL;
        info->argTabBytes    = 0;
    }
    else
    {
        info->regMaskResult     = convertCalleeSavedRegsMask(regMask);
        info->argMaskResult     = argMask;
        info->argHnumResult     = argHnum;
        info->iregMaskResult    = convertCalleeSavedRegsMask(iregMask);
        info->iargMaskResult    = iargMask;
        info->argTabResult      = argTab;
        info->argTabBytes       = argTabBytes;
        if ((stackDepth != 0) || (argMask != 0))
        {
            argMask = argMask;
        }
    }

#ifdef _DEBUG
    if (scanOffs != curOffs) {
        isCall = false;
    }
    _ASSERTE(thisPtrReg == REGI_NA || (!isCall || (regNumToMask(thisPtrReg) & info->regMaskResult)));
#endif
    info->thisPtrResult  = thisPtrReg;

    _ASSERTE(int(stackDepth) < INT_MAX); // check that it did not underflow
    return (stackDepth * sizeof(unsigned));
}


/*****************************************************************************
 * scan the register argument table for the fully interruptible case.
   this function is called to find all live objects (pushed arguments)
   and to get the stack base for fully interruptible methods.
   Returns size of things pushed on the stack for ESP frames
 */

static
unsigned scanArgRegTableI(PTR_CBYTE     table,
                          unsigned     curOffs,
                          hdrInfo   *  info)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    regNum thisPtrReg = REGI_NA;
    unsigned  ptrRegs    = 0;
    unsigned iptrRegs    = 0;
    unsigned  ptrOffs    = 0;
    unsigned  argCnt     = 0;

    ptrArgTP  ptrArgs(0);
    ptrArgTP iptrArgs(0);
    ptrArgTP  argHigh(0);

    bool      isThis     = false;
    bool      iptr       = false;

#if VERIFY_GC_TABLES
    _ASSERTE(*castto(table, unsigned short *)++ == 0xBABE);
#endif

  /*
      Encoding table for methods that are fully interruptible

      The encoding used is as follows:

          ptr reg dead        00RRRDDD    [RRR != 100]
          ptr reg live        01RRRDDD    [RRR != 100]

      non-ptr arg push        10110DDD                    [SSS == 110]
          ptr arg push        10SSSDDD                    [SSS != 110] && [SSS != 111]
          ptr arg pop         11CCCDDD    [CCC != 000] && [CCC != 110] && [CCC != 111]
      little delta skip       11000DDD    [CCC == 000]
      bigger delta skip       11110BBB                    [CCC == 110]

      The values used in the encodings are as follows:

        DDD                 code offset delta from previous entry (0-7)
        BBB                 bigger delta 000=8,001=16,010=24,...,111=64
        RRR                 register number (EAX=000,ECX=001,EDX=010,EBX=011,
                              EBP=101,ESI=110,EDI=111), ESP=100 is reserved
        SSS                 argument offset from base of stack. This is
                              redundant for frameless methods as we can
                              infer it from the previous pushes+pops. However,
                              for EBP-methods, we only report GC pushes, and
                              so we need SSS
        CCC                 argument count being popped (includes only ptrs for EBP methods)

      The following are the 'large' versions:

        large delta skip        10111000 [0xB8] , encodeUnsigned(delta)

        large     ptr arg push  11111000 [0xF8] , encodeUnsigned(pushCount)
        large non-ptr arg push  11111001 [0xF9] , encodeUnsigned(pushCount)
        large     ptr arg pop   11111100 [0xFC] , encodeUnsigned(popCount)
        large         arg dead  11111101 [0xFD] , encodeUnsigned(popCount) for caller-pop args.
                                                    Any GC args go dead after the call,
                                                    but are still sitting on the stack

        this pointer prefix     10111100 [0xBC]   the next encoding is a ptr live
                                                    or a ptr arg push
                                                    and contains the this pointer

        interior or by-ref      10111111 [0xBF]   the next encoding is a ptr live
             pointer prefix                         or a ptr arg push
                                                    and contains an interior
                                                    or by-ref pointer


        The value 11111111 [0xFF] indicates the end of the table.
  */

    /* Have we reached the instruction we're looking for? */

    while (ptrOffs <= curOffs)
    {
        unsigned    val;

        int         isPop;
        unsigned    argOfs;

        unsigned    regMask;

        // iptrRegs & iptrArgs are subsets of ptrRegs & ptrArgs respectively

        _ASSERTE((iptrRegs & ptrRegs) == iptrRegs);
        _ASSERTE((iptrArgs & ptrArgs) == iptrArgs);

        /* Now find the next 'life' transition */

        val = *table++;

        if  (!(val & 0x80))
        {
            /* A small 'regPtr' encoding */

            regNum       reg;

            ptrOffs += (val     ) & 0x7;
            if (ptrOffs > curOffs) {
                iptr = isThis = false;
                goto REPORT_REFS;
            }

            reg     = (regNum)((val >> 3) & 0x7);
            regMask = 1 << reg;         // EAX,ECX,EDX,EBX,---,EBP,ESI,EDI


            /* The register is becoming live/dead here */

            if  (val & 0x40)
            {
                /* Becomes Live */
                _ASSERTE((ptrRegs  &  regMask) == 0);

                ptrRegs |=  regMask;

                if  (isThis)
                {
                    thisPtrReg = reg;
                }
                if  (iptr)
                {
                    iptrRegs |= regMask;
                }
            }
            else
            {
                /* Becomes Dead */
                _ASSERTE((ptrRegs  &  regMask) != 0);

                ptrRegs &= ~regMask;

                if  (reg == thisPtrReg)
                {
                    thisPtrReg = REGI_NA;
                }
                if  (iptrRegs & regMask)
                {
                    iptrRegs &= ~regMask;
                }
            }
            iptr = isThis = false;
            continue;
        }

        /* This is probably an argument push/pop */

        argOfs = (val & 0x38) >> 3;

        /* 6 [110] and 7 [111] are reserved for other encodings */
        if  (argOfs < 6)
        {

            /* A small argument encoding */

            ptrOffs += (val & 0x07);
            if (ptrOffs > curOffs) {
                iptr = isThis = false;
                goto REPORT_REFS;
            }
            isPop    = (val & 0x40);

        ARG:

            if  (isPop)
            {
                if (argOfs == 0)
                    continue;           // little skip encoding

                /* We remove (pop) the top 'argOfs' entries */

                _ASSERTE(argOfs || argOfs <= argCnt);

                /* adjust # of arguments */

                argCnt -= argOfs;
                _ASSERTE(argCnt < MAX_PTRARG_OFS);

//              printf("[%04X] popping %u args: mask = %04X\n", ptrOffs, argOfs, (int)ptrArgs);

                do
                {
                    _ASSERTE(nonZero(argHigh));

                    /* Do we have an argument bit that's on? */

                    if  (intersect(ptrArgs, argHigh))
                    {
                        /* Turn off the bit */

                        setDiff(ptrArgs, argHigh);
                        setDiff(iptrArgs, argHigh);

                        /* We've removed one more argument bit */

                        argOfs--;
                    }
                    else if (info->ebpFrame)
                        argCnt--;
                    else /* !ebpFrame && not a ref */
                        argOfs--;

                    /* Continue with the next lower bit */

                    argHigh >>= 1;
                }
                while (argOfs);

                _ASSERTE(info->ebpFrame != 0         ||
                        !nonZero(argHigh)           ||
                        (argHigh == ptrArgTP(1 << (argCnt-1))));

                if (info->ebpFrame)
                {
                    while (!intersect(argHigh, ptrArgs) && (nonZero(argHigh)))
                        argHigh >>= 1;
                }

            }
            else
            {
                /* Add a new ptr arg entry at stack offset 'argOfs' */

                if  (argOfs >= MAX_PTRARG_OFS)
                {
                    _ASSERTE(!"@NYI: args pushed 'too deep'");
                }
                else
                {
                    /* For ESP-frames, all pushes are reported, and so
                       argOffs has to be consistent with argCnt */

                    _ASSERTE(info->ebpFrame || argCnt == argOfs);

                    /* store arg count */

                    argCnt  = argOfs + 1;
                    _ASSERTE((argCnt < MAX_PTRARG_OFS));

                    /* Compute the appropriate argument offset bit */

                    ptrArgTP argMask(1);
                    argMask <<= argOfs;

//                  printf("push arg at offset %02u --> mask = %04X\n", argOfs, (int)argMask);

                    /* We should never push twice at the same offset */

                    _ASSERTE(!intersect( ptrArgs, argMask));
                    _ASSERTE(!intersect(iptrArgs, argMask));

                    /* We should never push within the current highest offset */

                    _ASSERTE(argHigh < argMask);

                    /* This is now the highest bit we've set */

                    argHigh = argMask;

                    /* Set the appropriate bit in the argument mask */

                    ptrArgs |= argMask;

                    if (iptr)
                        iptrArgs |= argMask;
                }

                iptr = isThis = false;
            }
            continue;
        }
        else if (argOfs == 6)
        {
            if (val & 0x40) {
                /* Bigger delta  000=8,001=16,010=24,...,111=64 */
                ptrOffs += (((val & 0x07) + 1) << 3);
            }
            else {
                /* non-ptr arg push */
                _ASSERTE(!(info->ebpFrame));
                ptrOffs += (val & 0x07);
                if (ptrOffs > curOffs) {
                    iptr = isThis = false;
                    goto REPORT_REFS;
                }
                argHigh = ptrArgTP(1);
                argHigh <<= argCnt;
                argCnt++;
                _ASSERTE(argCnt < MAX_PTRARG_OFS);
            }
            continue;
        }

        /* argOfs was 7 [111] which is reserved for the larger encodings */

        _ASSERTE(argOfs==7);

        switch (val)
        {
        case 0xFF:
            iptr = isThis = false;
            goto REPORT_REFS;   // the method might loop !!!

        case 0xB8:
            val = fastDecodeUnsigned(table);
            ptrOffs += val;
            continue;

        case 0xBC:
            isThis = true;
            break;

        case 0xBF:
            iptr = true;
            break;

        case 0xF8:
        case 0xFC:
            isPop    = val & 0x04;
            argOfs = fastDecodeUnsigned(table);
            goto ARG;

        case 0xFD: {
            argOfs  = fastDecodeUnsigned(table);
            _ASSERTE(argOfs && argOfs <= argCnt);

            // Kill the top "argOfs" pointers.

            ptrArgTP    argMask(1);
            for(argMask <<= argCnt; argOfs; argMask >>= 1)
            {
                _ASSERTE(nonZero(argMask) && nonZero(ptrArgs)); // there should be remaining pointers

                if (intersect(ptrArgs, argMask))
                {
                    setDiff(ptrArgs, argMask);
                    setDiff(iptrArgs, argMask);
                    argOfs--;
                }
            }

            // For ebp-frames, need to find the next higest pointer for argHigh

            if (info->ebpFrame)
            {
                for(argHigh = ptrArgTP(0); nonZero(argMask); argMask >>= 1)
                {
                    if (intersect(ptrArgs, argMask)) {
                        argHigh = argMask;
                        break;
                    }
                }
            }
            } break;

        case 0xF9:
            argOfs = fastDecodeUnsigned(table);
            argCnt  += argOfs;
            break;

        default:
#ifdef _DEBUG
            printf("Unexpected special code %04X\n", val);
#endif
            _ASSERTE(!"");
        }
    }

    /* Report all live pointer registers */
REPORT_REFS:

    _ASSERTE((iptrRegs & ptrRegs) == iptrRegs); // iptrRegs is a subset of ptrRegs
    _ASSERTE((iptrArgs & ptrArgs) == iptrArgs); // iptrArgs is a subset of ptrArgs

    /* Save the current live register, argument set, and argCnt */

    info->regMaskResult  = convertAllRegsMask(ptrRegs);
    info->argMaskResult  = ptrArgs;
    info->argHnumResult  = 0;
    info->iregMaskResult = convertAllRegsMask(iptrRegs);
    info->iargMaskResult = iptrArgs;

    info->thisPtrResult  = thisPtrReg;
    _ASSERTE(thisPtrReg == REGI_NA || (regNumToMask(thisPtrReg) & info->regMaskResult));

    if (info->ebpFrame)
    {
        return 0;
    }
    else
    {
        _ASSERTE(int(argCnt) < INT_MAX); // check that it did not underflow
        return (argCnt * sizeof(unsigned));
    }
}

/*****************************************************************************/

unsigned GetPushedArgSize(hdrInfo * info, PTR_CBYTE table, DWORD curOffs)
{
    unsigned sz;

    if  (info->interruptible)
    {
        sz = scanArgRegTableI(skipToArgReg(*info, table),
                              curOffs,
                              info);
    }
    else
    {
        sz = scanArgRegTable(skipToArgReg(*info, table),
                             curOffs,
                             info);
    }

    return sz;
}

/*****************************************************************************/

inline
void    TRASH_CALLEE_UNSAVED_REGS(PREGDISPLAY pContext)
{
    LEAF_CONTRACT;

#ifdef _DEBUG
    /* This is not completely correct as we lose the current value, but
       it should not really be useful to anyone. */
    static DWORD s_badData = 0xDEADBEEF;
    pContext->pEax = pContext->pEcx = pContext->pEdx = &s_badData;
#endif //_DEBUG
}

/*****************************************************************************
 *  Sizes of certain i386 instructions which are used in the prolog/epilog
 */

// Can we use sign-extended byte to encode the imm value, or do we need a dword
#define CAN_COMPRESS(val)       ((INT8)(val) == (INT32)(val))

#define SZ_ADD_REG(val)         ( 2 +  (CAN_COMPRESS(val) ? 1 : 4))
#define SZ_AND_REG(val)         SZ_ADD_REG(val)
#define SZ_POP_REG              1
#define SZ_LEA(offset)          SZ_ADD_REG(offset)
#define SZ_MOV_REG_REG          2

bool IsMarkerInstr(BYTE val)
{
#ifdef _DEBUG
    return (val == X86_INSTR_INT3) || // Debugger might stomp with an int3
           (val == X86_INSTR_HLT && g_pConfig->GetGCStressLevel()); // GcCover might stomp with a Hlt
#else
    return false;
#endif
}

/* Check if the given instruction opcode is the one we expect.
   This is a "necessary" but not "sufficient" check as it ignores the check
   if the instruction is one of our special markers (for debugging and GcStress) */

bool CheckInstrByte(BYTE val, BYTE expectedValue)
{
    return ((val == expectedValue) || IsMarkerInstr(val));
}

/* Similar to CheckInstrByte(). Use this to check a masked opcode (ignoring
   optional bits in the opcode encoding).
   valPattern is the masked out value.
   expectedPattern is the mask value we expect.
   val is the actual instruction opcode
 */
bool CheckInstrBytePattern(BYTE valPattern, BYTE expectedPattern, BYTE val)
{
    _ASSERTE((valPattern & val) == valPattern);

    return ((valPattern == expectedPattern) || IsMarkerInstr(val));
}

/* Similar to CheckInstrByte() */

bool CheckInstrWord(WORD val, WORD expectedValue)
{
    return ((val == expectedValue) || IsMarkerInstr(val & 0xFF));
}

// Use this to check if the instruction at offset "walkOffset" has already
// been executed
// "actualHaltOffset" is the offset when the code was suspended
// It is assumed that there is linear control flow from offset 0 to "actualHaltOffset".
//
// This has been factored out just so that the intent of the comparison 
// is clear (compared to the opposite intent)

bool InstructionAlreadyExecuted(unsigned walkOffset, unsigned actualHaltOffset)
{
    return (walkOffset < actualHaltOffset);
}

// skips past a "arith REG, IMM"
inline unsigned SKIP_ARITH_REG(int val, PTR_CBYTE base, unsigned offset)
{
    LEAF_CONTRACT;

    unsigned delta = 0;
    if (val != 0)
    {
#ifdef _DEBUG
        // Confirm that arith instruction is at the correct place
        _ASSERTE(CheckInstrBytePattern(base[offset  ] & 0xFD, 0x81, base[offset]) &&
                 CheckInstrBytePattern(base[offset+1] & 0xC0, 0xC0, base[offset+1]));
        // only use DWORD form if needed
        _ASSERTE(((base[offset] & 2) != 0) == CAN_COMPRESS(val) ||
                 IsMarkerInstr(base[offset]));
#endif
        delta = 2 + (CAN_COMPRESS(val) ? 1 : 4);
    }
    return(offset + delta);
}

inline unsigned SKIP_PUSH_REG(PTR_CBYTE base, unsigned offset)
{
    LEAF_CONTRACT;

    // Confirm it is a push instruction
    _ASSERTE(CheckInstrBytePattern(base[offset] & 0xF8, 0x50, base[offset]));
    return(offset + 1);
}

inline unsigned SKIP_POP_REG(PTR_CBYTE base, unsigned offset)
{
    LEAF_CONTRACT;

    // Confirm it is a pop instruction
    _ASSERTE(CheckInstrBytePattern(base[offset] & 0xF8, 0x58, base[offset]));
    return(offset + 1);
}

inline unsigned SKIP_MOV_REG_REG(PTR_CBYTE base, unsigned offset)
{
    LEAF_CONTRACT;

    // Confirm it is a move instruction
    // Note that only the first byte may have been stomped on by IsMarkerInstr()
    // So we can check the second byte directly
    _ASSERTE(CheckInstrBytePattern(base[offset] & 0xFD, 0x89, base[offset]) &&
             (base[offset+1] & 0xC0) == 0xC0);
    return(offset + 2);
}

inline unsigned SKIP_LEA_ESP_EBP(int val, PTR_CBYTE base, unsigned offset)
{
    LEAF_CONTRACT;

#ifdef _DEBUG
    // Confirm it is the right instruction
    // Note that only the first byte may have been stomped on by IsMarkerInstr()
    // So we can check the second byte directly
    WORD wOpcode = *(PTR_WORD)base;
    _ASSERTE((CheckInstrWord(wOpcode, X86_INSTR_w_LEA_ESP_EBP_BYTE_OFFSET) &&
              (val == *(PTR_SBYTE)(base+2)) &&
              CAN_COMPRESS(val)) ||
             (CheckInstrWord(wOpcode, X86_INSTR_w_LEA_ESP_EBP_DWORD_OFFSET) &&
              (val == *(PTR_INT32)(base+2)) &&
              !CAN_COMPRESS(val)));
#endif

    unsigned delta = 2 + (CAN_COMPRESS(val) ? 1 : 4);
    return(offset + delta);
}

// May updates *pESP depending on haltOffset
// Returns the offset after the entire Enter callback call sequence (if present).
// Note that the return value may be greater than haltOffset.

unsigned SKIP_ENTER_PROF_CALLBACK(
        hdrInfo * info,
        PTR_CBYTE methodStart,
        unsigned walkOffset,
        unsigned haltOffset,
        unsigned * pESP)
{
// PROFILING_SUPPORTED is not defined for DAC build. Having the PROFILING_SUPPORTED ifdef here makes 
// SOS and windbg stacktraces garbage.
// #ifdef PROFILING_SUPPORTED

    // If profiler is active, we have following code
    //     push 0
    //     push 0
    //     push ProfilerContext (or push [ProfilerContext] or push 0)
    //     push MethodDesc      (or push [MethodDescPtr])
    //     call EnterNaked      (or call [EnterNakedPtr])
    // We need to adjust stack offset if haltOffset is after the push instruction.
    // We don't check if profiler is present because in ngen /prof cases,
    // prof-enabled ngened mscorlib image could be loaded.
    if (info->profCallbacks)
    {
        _ASSERTE(CheckInstrByte(methodStart[walkOffset], X86_INSTR_PUSH_0));

        walkOffset += 2; // skip the push 0 instruction

        // Unwind the pushed values
        if (walkOffset == haltOffset)
        {
            *pESP += (sizeof(TADDR));
        }

        _ASSERTE(CheckInstrByte(methodStart[walkOffset], X86_INSTR_PUSH_0));

        walkOffset += 2; // skip the push 0 instruction

        // Unwind the pushed values
        if (walkOffset == haltOffset)
        {
            *pESP += (sizeof(TADDR) * 2);
        }

        _ASSERTE(CheckInstrByte(methodStart[walkOffset], X86_INSTR_PUSH_0) ||
                 CheckInstrByte(methodStart[walkOffset], X86_INSTR_PUSH_IMM) ||
                 CheckInstrWord(*PTR_WORD(methodStart + walkOffset), X86_INSTR_W_PUSH_IND_IMM));

        //
        // Skip the profilerContext
        //
        if (methodStart[walkOffset] == X86_INSTR_PUSH_0)
        {
            walkOffset += 2;
        }
        else if (methodStart[walkOffset] == X86_INSTR_PUSH_IMM)
        {
            walkOffset += 5;
        }
        else if (*PTR_WORD(methodStart + walkOffset) == X86_INSTR_W_PUSH_IND_IMM)
        {
            walkOffset += 6;
        }
        else
        {
            _ASSERTE(!"Should not reach here!");
        }

        // Unwind the pushed values
        if (walkOffset == haltOffset)
        {
            *pESP += (sizeof(TADDR) * 3);
        }

        _ASSERTE(CheckInstrByte(methodStart[walkOffset], X86_INSTR_PUSH_IMM) ||
                 CheckInstrWord(*PTR_WORD(methodStart + walkOffset), X86_INSTR_W_PUSH_IND_IMM));

        //
        // Skip the method desc
        //
        if (methodStart[walkOffset] == X86_INSTR_PUSH_IMM)
        {
            walkOffset += 5;
        }
        else if (*PTR_WORD(methodStart + walkOffset) == X86_INSTR_W_PUSH_IND_IMM)
        {
            walkOffset += 6;
        }
        else
        {
            _ASSERTE(!"Should not reach here!");
        }

        // Unwind the pushed values
        if (walkOffset == haltOffset)
        {
            *pESP += (sizeof(TADDR) * 4);
        }

        _ASSERTE(CheckInstrByte(methodStart[walkOffset], X86_INSTR_CALL_REL32) ||
                 CheckInstrWord(*PTR_WORD(methodStart + walkOffset), X86_INSTR_W_CALL_IND_IMM));

        if (methodStart[walkOffset] == X86_INSTR_CALL_REL32)
        {
            walkOffset += 5;
        }
        else if (*PTR_WORD(methodStart + walkOffset) == X86_INSTR_W_CALL_IND_IMM)
        {
            walkOffset += 6;
        }
        else
        {
            _ASSERTE(!"Should not reach here!");
        }

    }

// #endif
    return walkOffset;
}


unsigned SKIP_ALLOC_FRAME(int size, PTR_CBYTE base, unsigned offset)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    _ASSERTE(size != 0);
    
    if (size == sizeof(void*))
    {
        // We do "push eax" instead of "sub esp,4"
        return (SKIP_PUSH_REG(base, offset));
    }

    if (size >= CORINFO_PAGE_SIZE)
    {
        if (size < (3 * CORINFO_PAGE_SIZE))
        {
            // add 7 bytes for one or two TEST EAX, [ESP+CORINFO_PAGE_SIZE]
            offset += (size / CORINFO_PAGE_SIZE) * 7;
        }
        else
        {
            //      xor eax, eax                2
            // loop:
            //      test [esp + eax], eax       3
            //      sub eax, 0x1000             5
            //      cmp EAX, -size              5
            //      jge loop                    2
            offset += 17;
        }
    }
    
    // sub ESP, size
    return (SKIP_ARITH_REG(size, base, offset));
}


#endif // !USE_GC_INFO_DECODER


#ifndef _X86_

void EECodeManager::EnsureCallerContextIsValid( PREGDISPLAY  pRD, StackwalkCacheEntry* pCacheEntry, ICodeInfo* pCodeInfo /*= NULL*/ )
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if( !pRD->IsCallerContextValid )
    {
        if (pCacheEntry != NULL)
        {
            // lightened schema: take stack unwind info from stackwalk cache
            QuickUnwindStackFrame(pRD, pCacheEntry, EnsureCallerStackFrameIsValid);
        }
        else
        {
            // We need to make a copy here (instead of switching the pointers), in order to preserve the current context
            *(pRD->pCallerContext) = *(pRD->pCurrentContext);
            AMD64_ONLY(*(pRD->pCallerContextPointers) = *(pRD->pCurrentContextPointers));

            KNONVOLATILE_CONTEXT_POINTERS *pCallerContextPointers = NULL;
            AMD64_ONLY(pCallerContextPointers = pRD->pCallerContextPointers);

            Thread::VirtualUnwindCallFrame(pRD->pCallerContext, pCallerContextPointers, pCodeInfo);
        }

        pRD->IsCallerContextValid = TRUE;
    }

    _ASSERTE( pRD->IsCallerContextValid );
}

size_t EECodeManager::GetCallerSp( PREGDISPLAY  pRD )
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    // Don't add usage of this field.  This is only temporary.
    // See ExceptionTracker::InitializeCrawlFrame() for more information.
    if (!pRD->IsCallerSPValid)
    {
        EnsureCallerContextIsValid(pRD, NULL);
    }
    return (size_t) (GetSP(pRD->pCallerContext));
}

#endif // !_X86_

/*
  *  Light unwind the current stack frame, using provided cache entry.
  *  pPC, Esp and pEbp of pContext are updated.
  */

// static
void EECodeManager::QuickUnwindStackFrame(PREGDISPLAY pRD, StackwalkCacheEntry *pCacheEntry, QuickUnwindFlag flag)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    _ASSERTE(pCacheEntry);
    _ASSERTE(GetControlPC(pRD) == (LPVOID)(pCacheEntry->IP));

#if defined(_X86_)
    _ASSERTE(flag == UnwindCurrentStackFrame);

    _ASSERTE(!pCacheEntry->fUseEbp || pCacheEntry->fUseEbpAsFrameReg);

    if (pCacheEntry->fUseEbpAsFrameReg)
    {
        _ASSERTE(pCacheEntry->fUseEbp);
        // EBP frame, update ESP through EBP, since ESPOffset may vary
        pRD->pEbp = PTR_DWORD((TADDR)*pRD->pEbp);
        pRD->Esp  = (TADDR)pRD->pEbp + sizeof(void*);
    }
    else
    {
        _ASSERTE(!pCacheEntry->fUseEbp);
        // ESP frame, update up to retAddr using ESPOffset
        pRD->Esp += pCacheEntry->ESPOffset;
    }
    pRD->PCTAddr  = (TADDR)pRD->Esp;
    pRD->pPC      = PTR_SLOT(pRD->PCTAddr);
    pRD->Esp     += sizeof(void*) + pCacheEntry->argSize;

#else  // !_X86_ && !_AMD64_
    PORTABILITY_ASSERT("EECodeManager::QuickUnwindStackFrame is not implemented on this platform.");
#endif // !_X86_ && !_AMD64_
}

/*****************************************************************************/
#ifdef _X86_ // UnwindStackFrame
/*****************************************************************************/

const RegMask CALLEE_SAVED_REGISTERS_MASK[] =
{
    RM_EDI, // first register to be pushed
    RM_ESI,
    RM_EBX,
    RM_EBP  // last register to be pushed
};

const SIZE_T REGDISPLAY_OFFSET_OF_CALLEE_SAVED_REGISTERS[] =
{
    offsetof(REGDISPLAY, pEdi), // first register to be pushed
    offsetof(REGDISPLAY, pEsi),
    offsetof(REGDISPLAY, pEbx),
    offsetof(REGDISPLAY, pEbp)  // last register to be pushed
};

/*****************************************************************************/

void UnwindEspFrameEpilog(
        PREGDISPLAY pContext, 
        hdrInfo * info,
        PTR_CBYTE epilogBase,
        unsigned flags)
{
    _ASSERTE(info->epilogOffs != hdrInfo::NOT_IN_EPILOG);
    _ASSERTE(!info->ebpFrame && !info->doubleAlign);
    _ASSERTE(info->epilogOffs > 0);
    
    int offset = 0;
    unsigned ESP = pContext->Esp;

    if (info->rawStkSize)
    {
        if (!InstructionAlreadyExecuted(offset, info->epilogOffs))
        {
            /* We have NOT executed the "ADD ESP, FrameSize", 
               so manually adjust stack pointer */
            ESP += info->rawStkSize;
        }

        // We have already popped off the frame (excluding the callee-saved registers)
        
        if (epilogBase[0] == X86_INSTR_POP_ECX)
        {
            // We may use "POP ecx" for doing "ADD ESP, 4", 
            // or we may not (in the case of JMP epilogs)
            _ASSERTE(info->rawStkSize == sizeof(void*));
            offset = SKIP_POP_REG(epilogBase, offset);
        }
        else
        {
            // "add esp, rawStkSize"
            offset = SKIP_ARITH_REG(info->rawStkSize, epilogBase, offset);
        }
    }
    
    /* Remaining callee-saved regs are at ESP. Need to update
       regsMask as well to exclude registers which have already been popped. */

    const RegMask regsMask = info->savedRegMask;

    /* Increment "offset" in steps to see which callee-saved
       registers have already been popped */

    for (unsigned i = NumItems(CALLEE_SAVED_REGISTERS_MASK); i > 0; i--)
    {
        RegMask regMask = CALLEE_SAVED_REGISTERS_MASK[i - 1];

        if (!(regMask & regsMask))
            continue;

        if (!InstructionAlreadyExecuted(offset, info->epilogOffs))
        {
            /* We have NOT yet popped off the register.
               Get the value from the stack if needed */
            if ((flags & UpdateAllRegs) || (regMask == RM_EBP))
            {
                SIZE_T offsetOfRegPtr = REGDISPLAY_OFFSET_OF_CALLEE_SAVED_REGISTERS[i - 1];
                *(LPVOID*)(PBYTE(pContext) + offsetOfRegPtr) = PTR_DWORD((TADDR)ESP);
            }

            /* Adjust ESP */
            ESP += sizeof(void*);
        }
        
        offset = SKIP_POP_REG(epilogBase, offset);
    }
    
    //CEE_JMP generates an epilog similar to a normal CEE_RET epilog except for the last instruction
    _ASSERTE(CheckInstrBytePattern(epilogBase[offset] & X86_INSTR_RET, X86_INSTR_RET, epilogBase[offset]) //ret
        || CheckInstrBytePattern(epilogBase[offset], X86_INSTR_JMP_NEAR_REL32, epilogBase[offset]) //jmp ret32
        || CheckInstrWord(*PTR_WORD(epilogBase + offset), X86_INSTR_w_JMP_FAR_IND_IMM)); //jmp [addr32]

    /* Finally we can set pPC */
    pContext->PCTAddr = (TADDR)ESP;
    pContext->pPC = PTR_SLOT(pContext->PCTAddr);

    pContext->Esp = ESP;
}

/*****************************************************************************/

void UnwindEbpDoubleAlignFrameEpilog(
        PREGDISPLAY pContext, 
        hdrInfo * info, 
        PTR_CBYTE epilogBase, 
        unsigned flags)
{
    _ASSERTE(info->epilogOffs != hdrInfo::NOT_IN_EPILOG);
    _ASSERTE(info->ebpFrame || info->doubleAlign);
    
    _ASSERTE(info->argSize < 0x10000); // "ret" only has a 2 byte operand

   /* See how many instructions we have executed in the
      epilog to determine which callee-saved registers
      have already been popped */
    int offset = 0;
   
    unsigned ESP = pContext->Esp;

    bool needMovEspEbp = false;

    if (info->doubleAlign)
    {
        // add esp, rawStkSize

        if (!InstructionAlreadyExecuted(offset, info->epilogOffs))
            ESP += info->rawStkSize;
        _ASSERTE(info->rawStkSize != 0);
        offset = SKIP_ARITH_REG(info->rawStkSize, epilogBase, offset);

        // We also need "mov esp, ebp" after popping the callee-saved registers
        needMovEspEbp = true;
    }
    else
    {
        bool needLea = false;

        if (info->localloc)
        {
            // ESP may be variable if a localloc was actually executed. We will reset it.
            //    lea esp, [ebp-calleeSavedRegs]

            needLea = true;
        }
        else if (info->savedRegsCountExclFP == 0)
        {
            // We will just generate "mov esp, ebp" and be done with it.
            
            if (info->rawStkSize != 0)
            {
                needMovEspEbp = true;
            }
        }
        else if  (info->rawStkSize == 0)
        {
            // do nothing before popping the callee-saved registers
        }
        else if (info->rawStkSize == sizeof(void*))
        {
            // "pop ecx" will make ESP point to the callee-saved registers           
            if (!InstructionAlreadyExecuted(offset, info->epilogOffs))
                ESP += sizeof(void*);
            offset = SKIP_POP_REG(epilogBase, offset);
        }
        else
        {
            // We need to make ESP point to the callee-saved registers
            //    lea esp, [ebp-calleeSavedRegs]
            
            needLea = true;
        }

        if (needLea)
        {
            // lea esp, [ebp-calleeSavedRegs]

            unsigned calleeSavedRegsSize = info->savedRegsCountExclFP * sizeof(void*); 

            if (!InstructionAlreadyExecuted(offset, info->epilogOffs))
                ESP = (*pContext->pEbp) - calleeSavedRegsSize;
            
            offset = SKIP_LEA_ESP_EBP(-int(calleeSavedRegsSize), epilogBase, offset);
        }
    }

    for (unsigned i = NumItems(CALLEE_SAVED_REGISTERS_MASK) - 1; i > 0; i--)
    {
        RegMask regMask = CALLEE_SAVED_REGISTERS_MASK[i - 1];
        _ASSERTE(regMask != RM_EBP);

        if ((info->savedRegMask & regMask) == 0)
            continue;
        
        if (!InstructionAlreadyExecuted(offset, info->epilogOffs))
        {
            if (flags & UpdateAllRegs)
            {
                SIZE_T offsetOfRegPtr = REGDISPLAY_OFFSET_OF_CALLEE_SAVED_REGISTERS[i - 1];
                *(LPVOID*)(PBYTE(pContext) + offsetOfRegPtr) = PTR_DWORD((TADDR)ESP);
            }
            ESP += sizeof(void*);
        }

        offset = SKIP_POP_REG(epilogBase, offset);
    }
    
    if (needMovEspEbp)
    {
        if (!InstructionAlreadyExecuted(offset, info->epilogOffs))
            ESP = *pContext->pEbp;
            
        offset = SKIP_MOV_REG_REG(epilogBase, offset);
    }

    // Have we executed the pop EBP?
    if (!InstructionAlreadyExecuted(offset, info->epilogOffs))
    {
        pContext->pEbp = PTR_DWORD(TADDR(ESP));
        ESP += sizeof(void*);
    }
    offset = SKIP_POP_REG(epilogBase, offset);

    pContext->PCTAddr = (TADDR)ESP;
    pContext->pPC = PTR_SLOT(pContext->PCTAddr);

    pContext->Esp = ESP;
}

//****************************************************************************
// This is the value ESP is incremented by on doing a "return"

inline SIZE_T ESPIncrOnReturn(hdrInfo * info)
{
    return sizeof(void *) + // pop off the return address
           // Note varargs is caller-popped
           (info->varargs ? 0 : info->argSize);
}

/*****************************************************************************/

void UnwindEpilog(
        PREGDISPLAY pContext, 
        hdrInfo * info, 
        PTR_CBYTE epilogBase, 
        unsigned flags)
{
    _ASSERTE(info->epilogOffs != hdrInfo::NOT_IN_EPILOG);
    // _ASSERTE(flags & ActiveStackFrame);
    _ASSERTE(info->epilogOffs > 0);

    if  (info->ebpFrame || info->doubleAlign)
    {
        UnwindEbpDoubleAlignFrameEpilog(pContext, info, epilogBase, flags);
    }
    else
    {
        UnwindEspFrameEpilog(pContext, info, epilogBase, flags);
    }

#ifdef _DEBUG    
    if (flags & UpdateAllRegs)
        TRASH_CALLEE_UNSAVED_REGS(pContext);
#endif

    /* Now adjust stack pointer */

    pContext->Esp += ESPIncrOnReturn(info);
}

/*****************************************************************************/

void UnwindEspFrameProlog(
        PREGDISPLAY pContext, 
        hdrInfo * info, 
        PTR_CBYTE methodStart, 
        unsigned flags)
{
    /* we are in the middle of the prolog */
    _ASSERTE(info->prologOffs != hdrInfo::NOT_IN_PROLOG);
    _ASSERTE(!info->ebpFrame && !info->doubleAlign);

    unsigned offset = 0;
    
#ifdef _DEBUG
    // If the first two instructions are 'nop, int3', then  we will
    // assume that is from a JitHalt operation and skip past it
    if (methodStart[0] == X86_INSTR_NOP)
    {
        _ASSERTE(CheckInstrByte(methodStart[1], X86_INSTR_INT3));
        offset += 2;
    }
#endif

    const DWORD curOffs = info->prologOffs;
    unsigned ESP = pContext->Esp;

    offset = SKIP_ENTER_PROF_CALLBACK(info, methodStart, offset, curOffs, (unsigned*)&ESP);
    
    // Find out how many callee-saved regs have already been pushed

    unsigned regsMask = RM_NONE;
    PTR_DWORD savedRegPtr = PTR_DWORD((TADDR)ESP);

    for (unsigned i = 0; i < NumItems(CALLEE_SAVED_REGISTERS_MASK); i++)
    {
        RegMask regMask = CALLEE_SAVED_REGISTERS_MASK[i];

        if (!(info->savedRegMask & regMask))
            continue;
        
        if (InstructionAlreadyExecuted(offset, curOffs))
        {
            ESP += sizeof(void*);
            regsMask    |= regMask;
        }
        
        offset = SKIP_PUSH_REG(methodStart, offset);
    }

    if (info->rawStkSize)
    {
        offset = SKIP_ALLOC_FRAME(info->rawStkSize, methodStart, offset);

        // Note that this assumes that only the last instruction in SKIP_ALLOC_FRAME
        // actually updates ESP
        if (InstructionAlreadyExecuted(offset, curOffs + 1))
        {
            savedRegPtr += (info->rawStkSize / sizeof(DWORD));
            ESP += info->rawStkSize;
        }
    }
    
    //
    // Stack probe checks here 
    //
    
    // Poison the value, we don't set it properly at the end of the prolog
    INDEBUG(offset = 0xCCCCCCCC);


    // Always restore EBP
    if (regsMask & RM_EBP)
        pContext->pEbp = savedRegPtr++;

    if (flags & UpdateAllRegs)
    {
        if (regsMask & RM_EBX)
            pContext->pEbx = savedRegPtr++;
        if (regsMask & RM_ESI)
            pContext->pEsi = savedRegPtr++;
        if (regsMask & RM_EDI)
            pContext->pEdi = savedRegPtr++;

        TRASH_CALLEE_UNSAVED_REGS(pContext);
    }


    pContext->Esp = ESP;
}

/*****************************************************************************/

void UnwindEspFrame(
        PREGDISPLAY pContext, 
        hdrInfo * info, 
        PTR_CBYTE table, 
        PTR_CBYTE methodStart,
        DWORD curOffs,
        unsigned flags)
{
    _ASSERTE(!info->ebpFrame && !info->doubleAlign);
    _ASSERTE(info->epilogOffs == hdrInfo::NOT_IN_EPILOG);

    unsigned ESP = pContext->Esp;

    
    if (info->prologOffs != hdrInfo::NOT_IN_PROLOG)
    {
        if (info->prologOffs != 0) // Do nothing for the very start of the method
        {
            UnwindEspFrameProlog(pContext, info, methodStart, flags);
            ESP = pContext->Esp;
        }
    }
    else
    {
        /* we are past the prolog, ESP has been set above */

        // Are there any arguments pushed on the stack?
        
        ESP += GetPushedArgSize(info, table, curOffs);

        ESP += info->rawStkSize;

        const RegMask regsMask = info->savedRegMask;

        for (unsigned i = NumItems(CALLEE_SAVED_REGISTERS_MASK); i > 0; i--)
        {
            RegMask regMask = CALLEE_SAVED_REGISTERS_MASK[i - 1];

            if ((regMask & regsMask) == 0)
                continue;
            
            SIZE_T offsetOfRegPtr = REGDISPLAY_OFFSET_OF_CALLEE_SAVED_REGISTERS[i - 1];
            *(LPVOID*)(PBYTE(pContext) + offsetOfRegPtr) = PTR_DWORD((TADDR)ESP);

            ESP += sizeof(unsigned);
        }
    }

    /* we can now set the (address of the) return address */

    pContext->PCTAddr = (TADDR)ESP;
    pContext->pPC = PTR_SLOT(pContext->PCTAddr);

    /* Now adjust stack pointer */

    pContext->Esp = ESP + ESPIncrOnReturn(info);
}


/*****************************************************************************/

void UnwindEbpDoubleAlignFrameProlog(
        PREGDISPLAY pContext, 
        hdrInfo * info, 
        PTR_CBYTE methodStart, 
        unsigned flags)
{
    _ASSERTE(info->prologOffs != hdrInfo::NOT_IN_PROLOG);
    _ASSERTE(info->ebpFrame || info->doubleAlign);
    
    DWORD offset = 0;
    
#ifdef _DEBUG
    // If the first two instructions are 'nop, int3', then  we will
    // assume that is from a JitHalt operation and skip past it
    if (methodStart[0] == X86_INSTR_NOP)
    {
        _ASSERTE(CheckInstrByte(methodStart[1], X86_INSTR_INT3));
        offset += 2;
    }
#endif

    /* Check for the case where EBP has not been updated yet. */

    const DWORD curOffs = info->prologOffs;

    offset = SKIP_ENTER_PROF_CALLBACK(info, methodStart, offset, curOffs, (unsigned*)&pContext->Esp);

    // If we have still not excecuted "push ebp; mov ebp, esp", then we need to
    // report the frame relative to ESP
    
    if (!InstructionAlreadyExecuted(offset + 1, curOffs))
    {
        _ASSERTE(CheckInstrByte(methodStart [offset], X86_INSTR_PUSH_EBP) ||
                 CheckInstrWord(*PTR_WORD(methodStart + offset), X86_INSTR_W_MOV_EBP_ESP));

        /* If we're past the "push ebp", adjust ESP to pop EBP off */

        if  (curOffs == (offset + 1))
            pContext->Esp += sizeof(TADDR);

        /* Stack pointer points to return address */

        pContext->PCTAddr = (TADDR)pContext->Esp;
        pContext->pPC = PTR_SLOT(pContext->PCTAddr);

        /* EBP and callee-saved registers still have the correct value */
        
        return;
    }

    // We are atleast after the "push ebp; mov ebp, esp"

    offset = SKIP_MOV_REG_REG(methodStart,
                SKIP_PUSH_REG(methodStart, offset));
    
    /* At this point, EBP has been set up. The caller's ESP and the return value
       can be determined using EBP. Since we are still in the prolog,
       we need to know our exact location to determine the callee-saved registers */
       
    const unsigned curEBP = *pContext->pEbp;
    
    if (flags & UpdateAllRegs)
    {        
        PTR_DWORD pSavedRegs = PTR_DWORD((TADDR)curEBP);

        /* make sure that we align ESP just like the method's prolog did */
        if  (info->doubleAlign)
        {
            // "and esp,-8"
            offset = SKIP_ARITH_REG(-8, methodStart, offset);
            if (curEBP & 0x04)
            {
                pSavedRegs--;
#ifdef _DEBUG
                if (dspPtr) printf("EnumRef: dblalign ebp: %08X\n", curEBP);
#endif
            }
        }

        /* Increment "offset" in steps to see which callee-saved
           registers have been pushed already */

        for (unsigned i = 0; i < NumItems(CALLEE_SAVED_REGISTERS_MASK) - 1; i++)
        {
            RegMask regMask = CALLEE_SAVED_REGISTERS_MASK[i];
            _ASSERTE(regMask != RM_EBP);

            if ((info->savedRegMask & regMask) == 0)
                continue;
            
            if (InstructionAlreadyExecuted(offset, curOffs))
            {
                SIZE_T offsetOfRegPtr = REGDISPLAY_OFFSET_OF_CALLEE_SAVED_REGISTERS[i];
                *(LPVOID*)(PBYTE(pContext) + offsetOfRegPtr) = --pSavedRegs;
            }

            // "push reg"
            offset = SKIP_PUSH_REG(methodStart, offset) ;
        }

        TRASH_CALLEE_UNSAVED_REGS(pContext);
    }
    
    /* The caller's saved EBP is pointed to by our EBP */

    pContext->pEbp = PTR_DWORD((TADDR)curEBP);
    pContext->Esp = DWORD(TADDR(pContext->pEbp + 1));
    
    /* Stack pointer points to return address */

    pContext->PCTAddr = (TADDR)pContext->Esp;
    pContext->pPC = PTR_SLOT(pContext->PCTAddr);
}

/*****************************************************************************/

bool UnwindEbpDoubleAlignFrame(
        PREGDISPLAY pContext, 
        hdrInfo * info, 
        PTR_CBYTE methodStart, 
        unsigned flags,
        StackwalkCacheUnwindInfo  *pUnwindInfo) // out-only, perf improvement
{
    _ASSERTE(info->ebpFrame || info->doubleAlign);

    const unsigned curESP =  pContext->Esp;
    const unsigned curEBP = *pContext->pEbp;

    /* First check if we are in a filter (which is obviously after the prolog) */

    if (info->handlers && info->prologOffs == hdrInfo::NOT_IN_PROLOG)
    {
        TADDR baseSP;

        FrameType frameType = GetHandlerFrameInfo(info, curEBP,
                                                  curESP, (DWORD) IGNORE_VAL,
                                                  &baseSP);

        /* If we are in a filter, we only need to unwind the funclet stack.
           For catches/finallies, the normal handling will
           cause the frame to be unwound all the way up to ebp skipping
           other frames above it. This is OK, as those frames will be
           dead. Also, the EE will detect that this has happened and it
           will handle any EE frames correctly.
         */

        if (frameType == FR_INVALID)
        {
            return false;
        }

        if (frameType == FR_FILTER)
        {
            pContext->PCTAddr = baseSP;
            pContext->pPC = PTR_SLOT(pContext->PCTAddr);

            pContext->Esp = (DWORD)(baseSP + sizeof(TADDR));

         // pContext->pEbp = same as before;

#ifdef _DEBUG
            /* The filter has to be called by the VM. So we dont need to
               update callee-saved registers.
             */

            if (flags & UpdateAllRegs)
            {
                static DWORD s_badData = 0xDEADBEEF;

                pContext->pEax = pContext->pEbx = pContext->pEcx =
                pContext->pEdx = pContext->pEsi = pContext->pEdi = &s_badData;
            }
#endif

            if (pUnwindInfo)
            {
                // The filter funclet is like an ESP-framed-method.
                pUnwindInfo->fUseEbp = FALSE;
                pUnwindInfo->fUseEbpAsFrameReg = FALSE;
            }

            return true;
        }
    }

    //
    // Prolog of an EBP method
    //
    
    if (info->prologOffs != hdrInfo::NOT_IN_PROLOG)
    {
        UnwindEbpDoubleAlignFrameProlog(pContext, info, methodStart, flags);
        
        /* Now adjust stack pointer. */

        pContext->Esp += ESPIncrOnReturn(info);
        return true;
    }

    if (flags & UpdateAllRegs)
    {
        // Get to the first callee-saved register
        PTR_DWORD pSavedRegs = PTR_DWORD((TADDR)curEBP);
        
        if (info->doubleAlign && (curEBP & 0x04))
            pSavedRegs--;

        for (unsigned i = 0; i < NumItems(CALLEE_SAVED_REGISTERS_MASK) - 1; i++)
        {
            RegMask regMask = CALLEE_SAVED_REGISTERS_MASK[i];
            if ((info->savedRegMask & regMask) == 0)
                continue;
            
            SIZE_T offsetOfRegPtr = REGDISPLAY_OFFSET_OF_CALLEE_SAVED_REGISTERS[i];
            *(LPVOID*)(PBYTE(pContext) + offsetOfRegPtr) = --pSavedRegs;
        }
    }

    /* The caller's ESP will be equal to EBP + retAddrSize + argSize. */

    pContext->Esp = (DWORD)(curEBP + sizeof(curEBP) + ESPIncrOnReturn(info));

    /* The caller's saved EIP is right after our EBP */

    pContext->PCTAddr = (TADDR)curEBP + RETURN_ADDR_OFFS * sizeof(TADDR);
    pContext->pPC = PTR_SLOT(pContext->PCTAddr);

    /* The caller's saved EBP is pointed to by our EBP */

    pContext->pEbp = PTR_DWORD((TADDR)curEBP);

    return true;
}

/*****************************************************************************
 *
 *  Unwind the current stack frame, i.e. update the virtual register
 *  set in pContext. This will be similar to the state after the function
 *  returns back to caller (IP points to after the call, Frame and Stack
 *  pointer has been reset, callee-saved registers restored (if UpdateAllRegs),
 *  callee-unsaved registers are trashed.
 *  Returns success of operation.
 */

/*        
            */
bool EECodeManager::UnwindStackFrame(PREGDISPLAY     pContext,
                                     LPVOID          methodInfoPtr,
                                     ICodeInfo      *pCodeInfo,
                                     unsigned        flags,
                                     CodeManState   *pState,
                                     StackwalkCacheUnwindInfo  *pUnwindInfo /* out-only, perf improvement */)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    // Address where the method has been interrupted
    size_t           breakPC = (size_t) *(pContext->pPC);

    /* Extract the necessary information from the info block header */
    PTR_CBYTE methodStart = PTR_CBYTE((TADDR)pCodeInfo->getStartAddress());
    DWORD    curOffs;
    pCodeInfo->getJitManager()->JitCodeToMethodInfo((SLOT) breakPC, NULL, NULL, &curOffs);

    _ASSERTE(sizeof(CodeManStateBuf) <= sizeof(pState->stateBuf));
    CodeManStateBuf * stateBuf = (CodeManStateBuf*)pState->stateBuf;

    if (pState->dwIsSet == 0)
    {
        /* Extract the necessary information from the info block header */

        stateBuf->hdrInfoSize = (DWORD)crackMethodInfoHdr(methodInfoPtr,
                                                          curOffs,
                                                          &stateBuf->hdrInfoBody);
    }

    PTR_CBYTE table = PTR_CBYTE((TADDR)methodInfoPtr) + stateBuf->hdrInfoSize;

    hdrInfo * info = &stateBuf->hdrInfoBody;

    info->isSpeculativeStackWalk = ((flags & SpeculativeStackwalk) != 0);
    
    if (pUnwindInfo != NULL)
    {
        pUnwindInfo->securityObjectOffset = 0;
        if (info->securityCheck)
        {
            _ASSERTE(info->ebpFrame);
            SIZE_T securityObjectOffset = (GetSecurityObjectOffset(info) / sizeof(void*));
            _ASSERTE(securityObjectOffset != 0);
            pUnwindInfo->securityObjectOffset = DWORD(securityObjectOffset);
        }
            
        pUnwindInfo->fUseEbpAsFrameReg = info->ebpFrame;
        pUnwindInfo->fUseEbp = ((info->savedRegMask & RM_EBP) != 0);
    }

    if  (info->epilogOffs != hdrInfo::NOT_IN_EPILOG)
    {
        /*---------------------------------------------------------------------
         *  First, handle the epilog
         */

        PTR_CBYTE epilogBase = (PTR_CBYTE) (breakPC - info->epilogOffs);
        UnwindEpilog(pContext, info, epilogBase, flags);
    }
    else if (!info->ebpFrame && !info->doubleAlign)
    {
        /*---------------------------------------------------------------------
         *  Now handle ESP frames
         */
         
        UnwindEspFrame(pContext, info, table, methodStart, curOffs, flags);
        return true;
    }
    else
    {
        /*---------------------------------------------------------------------
         *  Now we know that have an EBP frame
         */

        if (!UnwindEbpDoubleAlignFrame(pContext, info, methodStart, flags, pUnwindInfo))
            return false;
    }

    /*
    // Ensure isLegalManagedCodeCaller succeeds for speculative stackwalks.
    // (We just assert this below for non-speculative stackwalks.)
    //
    FAIL_IF_SPECULATIVE_WALK(isLegalManagedCodeCaller(taGetControlPC(pContext)));
    */

    return true;
}

/*****************************************************************************/
#else // _X86_ - UnwindStackFrame
/*****************************************************************************/

bool EECodeManager::UnwindStackFrame(PREGDISPLAY     pContext,
                                     LPVOID          methodInfoPtr,
                                     ICodeInfo      *pCodeInfo,
                                     unsigned        flags,
                                     CodeManState   *pState,
                                     StackwalkCacheUnwindInfo  *pUnwindInfo /* out-only, perf improvement */)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;


    _ASSERTE(pCodeInfo != NULL);
    Thread::VirtualUnwindCallFrame(pContext, pCodeInfo);
    return true;    
}

/*****************************************************************************/
#endif // _X86_ - UnwindStackFrame
/*****************************************************************************/

INDEBUG(void* forceStack1;)



#if defined(_X86_)

/*****************************************************************************
 *
 *  Enumerate all live object references in that function using
 *  the virtual register set.
 *  Returns success of operation.
 */

bool EECodeManager::EnumGcRefs( PREGDISPLAY     pContext,
                                LPVOID          methodInfoPtr,
                                ICodeInfo      *pCodeInfo,
                                unsigned        curOffs,
                                unsigned        flags,
                                GCEnumCallback  pCallBack,
                                LPVOID          hCallBack)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    INDEBUG(forceStack1 = &curOffs;)            // So I can see this in fastchecked.
    unsigned  EBP     = *pContext->pEbp;
    unsigned  ESP     =  pContext->Esp;

    unsigned  ptrOffs;

    unsigned  count;

    hdrInfo   info;
    PTR_CBYTE table = PTR_CBYTE((TADDR)methodInfoPtr);
    //unsigned  curOffs = *pContext->pPC - (int)methodStart;

    /* Extract the necessary information from the info block header */

    table += crackMethodInfoHdr(methodInfoPtr,
                                curOffs,
                                &info);

    _ASSERTE( curOffs <= info.methodSize);

#ifdef  _DEBUG
//    if ((methodInfoPtr == (void*)0x37760d0) && (curOffs == 0x264))
//        __asm int 3;

    if (trEnumGCRefs) {
        static unsigned lastESP = 0;
        unsigned        diffESP = ESP - lastESP;
        if (diffESP > 0xFFFF) {
            printf("------------------------------------------------------\n");
        }
        lastESP = ESP;
        printf("EnumGCRefs [%s][%s] at %s.%s + 0x%03X:\n",
               info.ebpFrame?"ebp":"   ",
               info.interruptible?"int":"   ",
               "UnknownClass","UnknownMethod", curOffs);
        fflush(stdout);
    }
#endif

    /* Are we in the prolog or epilog of the method? */

    if (info.prologOffs != hdrInfo::NOT_IN_PROLOG || 
        info.epilogOffs != hdrInfo::NOT_IN_EPILOG)
    {

        // Under normal circumstances the system will not suspend a thread
        // if it is in the prolog or epilog of the function.   However ThreadAbort
        // exception or stack overflows can cause EH to happen in a prolog.
        // Once in the handler, a GC can happen, so we can get to this code path.
        // However since we are tearing down this frame, we don't need to report
        // anything and we can simply return.

        _ASSERTE(flags & ExecutionAborted);
        return true;
    }

#ifdef _DEBUG
#define CHK_AND_REPORT_REG(doIt, iptr, regName)                         \
        if  (doIt)                                                      \
        {                                                               \
            if (dspPtr)                                                 \
                printf("    Live pointer register %s: ", #regName);     \
                pCallBack(hCallBack,                                    \
                          (OBJECTREF*)(pContext->p##regName),           \
                          (iptr ? GC_CALL_INTERIOR : 0)                 \
                          | CHECK_APP_DOMAIN );                         \
        }
#else // !_DEBUG
#define CHK_AND_REPORT_REG(doIt, iptr, regName)                         \
        if  (doIt)                                                      \
                pCallBack(hCallBack,                                    \
                          (OBJECTREF*)(pContext->p##regName),           \
                          (iptr ? GC_CALL_INTERIOR : 0)                 \
                          | CHECK_APP_DOMAIN );                         \

#endif // _DEBUG

    /* What kind of a frame is this ? */

    FrameType   frameType = FR_NORMAL;
    TADDR       baseSP = 0;

    if (info.handlers)
    {
        _ASSERTE(info.ebpFrame);

        bool    hasInnerFilter, hadInnerFilter;
        frameType = GetHandlerFrameInfo(&info, EBP,
                                        ESP, (DWORD) IGNORE_VAL,
                                        &baseSP, NULL,
                                        &hasInnerFilter, &hadInnerFilter);
        _ASSERTE(frameType != FR_INVALID);

        /* If this is the parent frame of a filter which is currently
           executing, then the filter would have enumerated the frame using
           the filter PC.
         */

        if (hasInnerFilter)
            return true;

        /* If are in a try and we had a filter execute, we may have reported
           GC refs from the filter (and not using the try's offset). So
           we had better use the filter's end offset, as the try is
           effectively dead and its GC ref's would be stale */

        if (hadInnerFilter)
        {
            PTR_TADDR pFirstBaseSPslot = GetFirstBaseSPslotPtr(EBP, &info);
            curOffs = (unsigned)pFirstBaseSPslot[1] - 1;
            _ASSERTE(curOffs < info.methodSize);

            /* Extract the necessary information from the info block header */

            table = PTR_CBYTE((TADDR)methodInfoPtr);

            table += crackMethodInfoHdr(methodInfoPtr,
                                        curOffs,
                                        &info);
        }
    }


    bool        willContinueExecution = !(flags & ExecutionAborted);
    unsigned    pushedSize = 0;

    /* if we have been interrupted we don't have to report registers/arguments
     * because we are about to lose this context anyway.
     * Alas, if we are in a ebp-less method we have to parse the table
     * in order to adjust ESP.
     *
     * Note that we report "this" for all methods, even if
     * noncontinuable, because because of the off chance they may be
     * synchronized and we have to release the monitor on unwind. This
     * could conceivably be optimized, but it turns out to be more
     * expensive to check whether we're synchronized (which involves
     * consulting metadata) than to just report "this" all the time in
     * our most important scenarios.
     */

    if  (info.interruptible)
    {
        pushedSize = scanArgRegTableI(skipToArgReg(info, table), curOffs, &info);

        RegMask   regs  = info.regMaskResult;
        RegMask  iregs  = info.iregMaskResult;
        ptrArgTP  args  = info.argMaskResult;
        ptrArgTP iargs  = info.iargMaskResult;

        _ASSERTE((!nonZero(args) || pushedSize != 0) || info.ebpFrame);
        _ASSERTE((args & iargs) == iargs);
        // Only synchronized methods and generic code that accesses
        // the type context via "this" need to report "this".
        // If its reported for other methods, its probably
        // done incorrectly. So flag such cases.
        _ASSERTE(info.thisPtrResult == REGI_NA ||
                 pCodeInfo->IsSynchronized() ||
                 pCodeInfo->AcquiresInstMethodTableFromThis());

            /* now report registers and arguments if we are not interrupted */

        if  (willContinueExecution)
        {

            /* Propagate unsafed registers only in "current" method */
            /* If this is not the active method, then the callee wil
             * trash these registers, and so we wont need to report them */

            if (flags & ActiveStackFrame)
            {
                CHK_AND_REPORT_REG(regs & RM_EAX, iregs & RM_EAX, Eax);
                CHK_AND_REPORT_REG(regs & RM_ECX, iregs & RM_ECX, Ecx);
                CHK_AND_REPORT_REG(regs & RM_EDX, iregs & RM_EDX, Edx);
            }

            CHK_AND_REPORT_REG(regs & RM_EBX, iregs & RM_EBX, Ebx);
            CHK_AND_REPORT_REG(regs & RM_EBP, iregs & RM_EBP, Ebp);
            CHK_AND_REPORT_REG(regs & RM_ESI, iregs & RM_ESI, Esi);
            CHK_AND_REPORT_REG(regs & RM_EDI, iregs & RM_EDI, Edi);
            _ASSERTE(!(regs & RM_ESP));

            /* Report any pending pointer arguments */

            DWORD * pPendingArgFirst;       // points **AT** first parameter
            if (!info.ebpFrame)
            {
                // -sizeof(void*) because we want to point *AT* first parameter
                pPendingArgFirst = (DWORD *)(size_t)(ESP + pushedSize - sizeof(void*));
            }
            else
            {
                _ASSERTE(willContinueExecution);

                if (info.handlers)
                {
                    // -sizeof(void*) because we want to point *AT* first parameter
                    pPendingArgFirst = (DWORD *)(size_t)(baseSP - sizeof(void*));
                }
                else if (info.localloc)
                {
                    baseSP = *(DWORD *)(size_t)(EBP - GetLocallocSPOffset(&info));
                    // -sizeof(void*) because we want to point *AT* first parameter
                    pPendingArgFirst = (DWORD *)(size_t) (baseSP - sizeof(void*));
                }
                else
                {
                    // Note that 'info.stackSize includes the size for pushing EBP, but EBP is pushed
                    // BEFORE EBP is set from ESP, thus (EBP - info.stackSize) actually points past
                    // the frame by one DWORD, and thus points *AT* the first parameter

                    pPendingArgFirst = (DWORD *)(size_t)(EBP - info.stackSize);
                }
            }

            if  (nonZero(args))
            {
                unsigned   i = 0;
                ptrArgTP   b(1);
                for (; nonZero(args) && (i < MAX_PTRARG_OFS); i += 1, b <<= 1)
                {
                    if  (intersect(args,b))
                    {
                        unsigned    argAddr = (unsigned)(size_t)(pPendingArgFirst - i);
                        bool        iptr    = false;

                        setDiff(args, b);
                        if (intersect(iargs,b))
                        {
                            setDiff(iargs, b);
                            iptr   = true;
                        }

#ifdef _DEBUG
                        if (dspPtr)
                        {
                            printf("    Pushed ptr arg  [E");
                            if  (info.ebpFrame)
                                printf("BP-%02XH]: ", EBP - argAddr);
                            else
                                printf("SP+%02XH]: ", argAddr - ESP);
                        }
#endif
                        _ASSERTE(true == GC_CALL_INTERIOR);
                        pCallBack(hCallBack, (OBJECTREF *)(size_t)argAddr,
                                  (int)iptr | CHECK_APP_DOMAIN);
                    }
                }
            }
        }
        else
        {
            // Is "this" enregistered. If so, report it as we might need to
            // release the monitor for synchronized methods.
            // Else, it is on the stack and will be reported below.

            if (info.thisPtrResult != REGI_NA)
            {
                // Synchronized methods and methods satisfying
                // MethodDesc::AcquiresInstMethodTableFromThis (i.e. those
                // where "this" is reported in thisPtrResult) are
                // not supported on value types.
                _ASSERTE((regNumToMask(info.thisPtrResult) & info.iregMaskResult)== 0);

                void * thisReg = getCalleeSavedReg(pContext, info.thisPtrResult);
                pCallBack(hCallBack, (OBJECTREF *)thisReg,
                          CHECK_APP_DOMAIN);
            }
        }
    }
    else /* not interruptible */
    {
        pushedSize = scanArgRegTable(skipToArgReg(info, table), curOffs, &info);

        RegMask    regMask = info.regMaskResult;
        RegMask   iregMask = info.iregMaskResult;
        unsigned   argMask = info.argMaskResult;
        unsigned  iargMask = info.iargMaskResult;
        unsigned   argHnum = info.argHnumResult;
        PTR_CBYTE argTab  = (PTR_CBYTE) info.argTabResult;

        // Only synchronized methods and generic code that accesses
        // the type context via "this" need to report "this".
        // If its reported for other methods, its probably
        // done incorrectly. So flag such cases.
        _ASSERTE(info.thisPtrResult == REGI_NA ||
                 pCodeInfo->IsSynchronized() ||
                 pCodeInfo->AcquiresInstMethodTableFromThis());


        /* now report registers and arguments if we are not interrupted */

        if  (willContinueExecution)
        {

            /* Report all live pointer registers */

            CHK_AND_REPORT_REG(regMask & RM_EDI, iregMask & RM_EDI, Edi);
            CHK_AND_REPORT_REG(regMask & RM_ESI, iregMask & RM_ESI, Esi);
            CHK_AND_REPORT_REG(regMask & RM_EBX, iregMask & RM_EBX, Ebx);
            CHK_AND_REPORT_REG(regMask & RM_EBP, iregMask & RM_EBP, Ebp);

            /* Esp cant be reported */
            _ASSERTE(!(regMask & RM_ESP));
            /* No callee-trashed registers */
            _ASSERTE(!(regMask & RM_CALLEE_TRASHED));
            /* EBP can't be reported unless we have an EBP-less frame */
            _ASSERTE(!(regMask & RM_EBP) || !(info.ebpFrame));

            /* Report any pending pointer arguments */

            if (argTab != 0)
            {
                unsigned    lowBits, stkOffs, argAddr, val;

                // argMask does not fit in 32-bits
                // thus arguments are reported via a table
                // Both of these are very rare cases

                do
                {
                    val = fastDecodeUnsigned(argTab);

                    lowBits = val &  OFFSET_MASK;
                    stkOffs = val & ~OFFSET_MASK;
                    _ASSERTE((lowBits == 0) || (lowBits == byref_OFFSET_FLAG));

                    argAddr = ESP + stkOffs;
#ifdef _DEBUG
                    if (dspPtr)
                        printf("    Pushed %sptr arg at [ESP+%02XH]",
                               lowBits ? "iptr " : "", stkOffs);
#endif
                    _ASSERTE(byref_OFFSET_FLAG == GC_CALL_INTERIOR);
                    pCallBack(hCallBack, (OBJECTREF *)(size_t)argAddr,
                              lowBits | CHECK_APP_DOMAIN);
                }
                while(--argHnum);

                _ASSERTE(info.argTabResult + info.argTabBytes == argTab);
            }
            else
            {
                unsigned    argAddr = ESP;

                while (argMask)
                {
                    _ASSERTE(argHnum-- > 0);

                    if  (argMask & 1)
                    {
                        bool     iptr    = false;

                        if (iargMask & 1)
                            iptr = true;
#ifdef _DEBUG
                        if (dspPtr)
                            printf("    Pushed ptr arg at [ESP+%02XH]",
                                   argAddr - ESP);
#endif
                        _ASSERTE(true == GC_CALL_INTERIOR);
                        pCallBack(hCallBack, (OBJECTREF *)(size_t)argAddr,
                                  (int)iptr | CHECK_APP_DOMAIN);
                    }

                    argMask >>= 1;
                    iargMask >>= 1;
                    argAddr  += 4;
                }

            }

        }
        else
        {
            // Is "this" enregistered. If so, report it as we will need to
            // release the monitor. Else, it is on the stack and will be
            // reported below.

            // For partially interruptible code, info.thisPtrResult will be
            // the last known location of "this". So the compiler needs to
            // generate information which is correct at every point in the code,
            // not just at call sites.

            if (info.thisPtrResult != REGI_NA)
            {
                // Synchronized methods on value types are not supported
                _ASSERTE((regNumToMask(info.thisPtrResult) & info.iregMaskResult)== 0);

                void * thisReg = getCalleeSavedReg(pContext, info.thisPtrResult);
                pCallBack(hCallBack, (OBJECTREF *)thisReg,
                          CHECK_APP_DOMAIN);
            }
        }

    } //info.interruptible

    /* compute the argument base (reference point) */

    unsigned    argBase;

    if (info.ebpFrame)
        argBase = EBP;
    else
        argBase = ESP + pushedSize;

#if VERIFY_GC_TABLES
    _ASSERTE(*castto(table, unsigned short *)++ == 0xBEEF);
#endif

    unsigned ptrAddr;
    unsigned lowBits;


    /* Process the untracked frame variable table */

    count = info.untrackedCnt;
    int lastStkOffs = 0;
    while (count-- > 0)
    {
        int stkOffs = fastDecodeSigned(table);
        stkOffs = lastStkOffs - stkOffs;
        lastStkOffs = stkOffs;

        _ASSERTE(0 == ~OFFSET_MASK % sizeof(void*));

        lowBits  =   OFFSET_MASK & stkOffs;
        stkOffs &=  ~OFFSET_MASK;

        ptrAddr = argBase + stkOffs;
        if (info.doubleAlign && stkOffs >= int(info.stackSize - sizeof(void*))) {
            // We encode the arguments as if they were ESP based variables even though they aren't
            // If this frame would have ben an ESP based frame,   This fake frame is one DWORD
            // smaller than the real frame because it did not push EBP but the real frame did.
            // Thus to get the correct EBP relative offset we have to ajust by info.stackSize-sizeof(void*)
            ptrAddr = EBP + (stkOffs-(info.stackSize - sizeof(void*)));
        }

#ifdef  _DEBUG
        if (dspPtr)
        {
            printf("    Untracked %s%s local at [E",
                        (lowBits & pinned_OFFSET_FLAG) ? "pinned " : "",
                        (lowBits & byref_OFFSET_FLAG)  ? "byref"   : "");

            int   dspOffs = ptrAddr;
            char  frameType;

            if (info.ebpFrame) {
                dspOffs   -= EBP;
                frameType  = 'B';
            }
            else {
                dspOffs   -= ESP;
                frameType  = 'S';
            }

            if (dspOffs < 0)
                printf("%cP-%02XH]: ", frameType, -dspOffs);
            else
                printf("%cP+%02XH]: ", frameType, +dspOffs);
        }
#endif

        _ASSERTE((pinned_OFFSET_FLAG == GC_CALL_PINNED) &&
               (byref_OFFSET_FLAG  == GC_CALL_INTERIOR));
        pCallBack(hCallBack, (OBJECTREF*)(size_t)ptrAddr, lowBits | CHECK_APP_DOMAIN);
    }

#if VERIFY_GC_TABLES
    _ASSERTE(*castto(table, unsigned short *)++ == 0xCAFE);
#endif

    /* Process the frame variable lifetime table */
    count = info.varPtrTableSize;

    /* If we are not in the active method, we are currently pointing
     * to the return address; at the return address stack variables
     * can become dead if the call the last instruction of a try block
     * and the return address is the jump around the catch block. Therefore
     * we simply assume an offset inside of call instruction.
     */

    unsigned newCurOffs;

    if (willContinueExecution)
    {
        newCurOffs = (flags & ActiveStackFrame) ?  curOffs    // after "call"
                                                :  curOffs-1; // inside "call"
    }
    else
    {
        /* However if ExecutionAborted, then this must be one of the
         * ExceptionFrames. Handle accordingly
         */
        _ASSERTE(!(flags & AbortingCall) || !(flags & ActiveStackFrame));

        newCurOffs = (flags & AbortingCall) ? curOffs-1 // inside "call"
                                            : curOffs;  // at faulting instr, or start of "try"
    }

    ptrOffs    = 0;

    while (count-- > 0)
    {
        int       stkOffs;
        unsigned  begOffs;
        unsigned  endOffs;

        stkOffs = fastDecodeUnsigned(table);
        begOffs  = ptrOffs + fastDecodeUnsigned(table);
        endOffs  = begOffs + fastDecodeUnsigned(table);

        _ASSERTE(0 == ~OFFSET_MASK % sizeof(void*));

        lowBits  =   OFFSET_MASK & stkOffs;
        stkOffs &=  ~OFFSET_MASK;

        if (info.ebpFrame) {
            stkOffs = -stkOffs;
            _ASSERTE(stkOffs < 0);
        }
        else {
            _ASSERTE(stkOffs >= 0);
        }

        ptrAddr = argBase + stkOffs;

        /* Is this variable live right now? */

        if (newCurOffs >= begOffs)
        {
            if (newCurOffs <  endOffs)
            {
#ifdef  _DEBUG
                if (dspPtr) {
                    printf("    Frame %s%s local at [E",
                           (lowBits & byref_OFFSET_FLAG) ? "byref "   : "",
                           (lowBits & this_OFFSET_FLAG)  ? "this-ptr" : "");
                    
                    int  dspOffs = ptrAddr;
                    char frameType;
                    
                    if (info.ebpFrame) {
                        dspOffs   -= EBP;
                        frameType  = 'B';
                    }
                    else {
                        dspOffs   -= ESP;
                        frameType  = 'S';
                    }
                    
                    if (dspOffs < 0)
                        printf("%cP-%02XH]: ", frameType, -dspOffs);
                    else
                        printf("%cP+%02XH]: ", frameType, +dspOffs);
                }
#endif
                _ASSERTE(byref_OFFSET_FLAG == GC_CALL_INTERIOR);
                pCallBack(hCallBack, (OBJECTREF*)(size_t)ptrAddr,
                          (lowBits & byref_OFFSET_FLAG) | CHECK_APP_DOMAIN);

            }
        }
        // exit loop early if start of live range is beyond PC, as ranges are sorted by lower bound
        else break;

        ptrOffs  = begOffs;
    }


#if VERIFY_GC_TABLES
    _ASSERTE(*castto(table, unsigned short *)++ == 0xBABE);
#endif

    /* Are we a varargs function, if so we have to report all args
       except 'this' (note that the GC tables created by the x86 jit
       do not contain ANY arguments except 'this' (even if they
       were statically declared */

    if (info.varargs) {
#ifndef DACCESS_COMPILE
        LOG((LF_GCINFO, LL_INFO100, "Reporting incoming vararg GC refs\n"));

        BYTE* argsStart;

        if (info.ebpFrame || info.doubleAlign)
            argsStart = ((BYTE*)(size_t)EBP) + 2* sizeof(void*);                 // pushed EBP and retAddr
        else
            argsStart = ((BYTE*)(size_t)argBase) + info.stackSize + sizeof(void*);   // ESP + locals + retAddr

#ifdef _DEBUG
        // Note that I really want to say hCallBack is a GCCONTEXT, but this is pretty close
        extern void GcEnumObject(LPVOID pData, OBJECTREF *pObj, DWORD flags);
        _ASSERTE((void*) GcEnumObject == pCallBack);
#endif
        GCCONTEXT   *pCtx = (GCCONTEXT *) hCallBack;

        // For varargs, look up the signature using the varArgSig token passed on the stack
        VASigCookie* varArgSig = *((VASigCookie**) argsStart);
        MetaSig msig(varArgSig->mdVASig, 
                     SigParser::LengthOfSig(varArgSig->mdVASig),
                     varArgSig->pModule, 
                     NULL, 
                     NULL);

        promoteArgs(argsStart, &msig, pCtx, 0, 0, false);
#else
        DacNotImpl();
#endif
    }

    return true;
}


#elif defined(USE_GC_INFO_DECODER)  // !defined(_X86_)


/*****************************************************************************
 *
 *  Enumerate all live object references in that function using
 *  the virtual register set.
 *  Returns success of operation.
 */
bool EECodeManager::EnumGcRefs( PREGDISPLAY     pRD,
                                LPVOID          methodInfoPtr,
                                ICodeInfo      *pCodeInfo,
                                unsigned        curOffs,
                                unsigned        flags,
                                GCEnumCallback  pCallBack,
                                LPVOID          hCallBack)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    LOG((LF_GCINFO, LL_INFO1000, "Reporting GC refs at offset %04x.\n", curOffs));


#ifndef DACCESS_COMPILE

    BYTE* gcInfoAddr = (BYTE*) methodInfoPtr;


#ifdef _DEBUG
    if (flags & ActiveStackFrame)
    {
        GcInfoDecoder _gcInfoDecoder(
                            gcInfoAddr,
                            DECODE_INTERRUPTIBILITY,
                            curOffs
                            );
        _ASSERTE(_gcInfoDecoder.IsInterruptible());
    }
#endif


    BOOL reportScratchSlots;

    // We report scratch slots only for leaf frames.
    // A frame is non-leaf if we are executing a call, or a fault occurred in the function.
    // The only case in which we need to report scratch slots for a non-leaf frame
    //   is when execution has to be resumed at the point of interruption (via ResumableFrame)
    _ASSERTE( sizeof( BOOL ) >= sizeof( ActiveStackFrame ) );
    reportScratchSlots = (flags & ActiveStackFrame);

    GcInfoDecoder gcInfoDecoder(
                        gcInfoAddr,
                        GcInfoDecoderFlags (DECODE_GC_LIFETIMES | DECODE_SECURITY_OBJECT | DECODE_VARARG),
                        curOffs
                        );

    if (!gcInfoDecoder.EnumerateLiveSlots(
                        pRD,
                        reportScratchSlots,
                        flags,
                        pCallBack,
                        hCallBack
                        ))
    {
        return false;
    }


    INT32 spOffset = gcInfoDecoder.GetSecurityObjectStackSlot();
    if( spOffset != NO_SECURITY_OBJECT )
    {
        if(curOffs >= gcInfoDecoder.GetSecurityObjectValidRangeStart()
            && curOffs < gcInfoDecoder.GetSecurityObjectValidRangeEnd())
        {
            /* Report the "security object" */
            OBJECTREF* pSlot = (OBJECTREF*) (spOffset + GetCallerSp(pRD));
            pCallBack(hCallBack, pSlot, CHECK_APP_DOMAIN);
        }
    }

    if (gcInfoDecoder.GetIsVarArg())
    {

        LOG((LF_GCINFO, LL_INFO100, "Reporting incoming vararg GC refs\n"));

        CORINFO_SIG_HANDLE hSig;
        DWORD cbSigSize;
        CORINFO_MODULE_HANDLE hModule;
        pCodeInfo->getMethodSig(&hSig, &cbSigSize, &hModule);

        // We won't be parsing the argument encodings in the signature, so
        // generics don't matter.  These casts assume that we're talking to an
        // EECodeInfo.
        SigTypeContext typeContext; // An empty SigTypeContext is OK - loads the generic type
        MetaSig msigFindVASig((PCCOR_SIGNATURE)hSig, 
                              cbSigSize,
                              (Module*)hModule, 
                              &typeContext);

        // Find the offset of the VASigCookie.  It's offsets are relative to
        // the base of a FramedMethodFrame.
        int argsOffset =   ArgIterator::GetVASigCookieOffset(&msigFindVASig)
                         - FramedMethodFrame::GetOffsetOfArgs();


        BYTE* prevSP     = (BYTE*) GetCallerSp(pRD);
        BYTE* curSP      = (BYTE*) GetSP(pRD->pCurrentContext);


        _ASSERTE(prevSP + argsOffset >= curSP);

#ifdef _DEBUG

        // Note that I really want to say hCallBack is a GCCONTEXT, but this is pretty close
        extern void GcEnumObject(LPVOID pData, OBJECTREF *pObj, DWORD flags);
        _ASSERTE((void*) GcEnumObject == pCallBack);
#endif
        GCCONTEXT   *pCtx = (GCCONTEXT *) hCallBack;

        // For varargs, look up the signature using the varArgSig token passed on the stack
        VASigCookie* varArgSig = *((VASigCookie**) (prevSP + argsOffset));
        MetaSig msig(varArgSig->mdVASig, 
                     SigParser::LengthOfSig(varArgSig->mdVASig),
                     varArgSig->pModule, 
                     NULL, 
                     NULL);

        //
        // @NICE: all these #ifdefs are really confusing, surely there's a better way...?
        //


#ifdef _X86_
        // For the X86 target the JIT does not report any of the fixed args for a varargs method
        // So we report the fixed args via the promoteArgs call below
        promoteArgs(prevSP, &msig, pCtx, argsOffset, 0, false);
#else
        // For the 64-bit platforms the JITs do report the fixed args of a varargs method
        // So we must tell promoteArgs to skip to the end of the fixed args
        promoteArgs(prevSP, &msig, pCtx, argsOffset, 0, true);
#endif
    }

    return true;

#else
    DacNotImpl();
    return false;
#endif // #ifndef DACCESS_COMPILE
}

#else // !defined(_X86_) && !defined(_GC_INFO_ENCODER_)

bool EECodeManager::EnumGcRefs( PREGDISPLAY     pContext,
                                LPVOID          methodInfoPtr,
                                ICodeInfo      *pCodeInfo,
                                unsigned        curOffs,
                                unsigned        flags,
                                GCEnumCallback  pCallBack,
                                LPVOID          hCallBack)
{
    PORTABILITY_ASSERT("EECodeManager::EnumGcRefs is not implemented on this platform.");
    return false;
}

#endif // _X86_

/*****************************************************************************
 *
 *  Return the address of the local security object reference
 *  using data that was previously cached before in UnwindStackFrame
 *  using StackwalkCacheUnwindInfo
 */

OBJECTREF* EECodeManager::GetAddrOfSecurityObjectFromCachedInfo(PREGDISPLAY pRD, StackwalkCacheUnwindInfo * stackwalkCacheUnwindInfo)
{
    LEAF_CONTRACT;
#ifdef _X86_
    size_t securityObjectOffset = stackwalkCacheUnwindInfo->securityObjectOffset;
    _ASSERTE(securityObjectOffset != 0);
    // We pretend that filters are ESP-based methods in UnwindEbpDoubleAlignFrame().
    // Hence we cannot enforce this assert.
    // _ASSERTE(stackwalkCacheUnwindInfo->fUseEbpAsFrameReg);
    return (OBJECTREF *) (size_t) (DWORD(*pRD->pEbp) - (securityObjectOffset * sizeof(void*)));
#else
    PORTABILITY_ASSERT("EECodeManager::GetAddrOfSecurityObjectFromContext is not implemented on this platform.");
    return NULL;
#endif
}

OBJECTREF* EECodeManager::GetAddrOfSecurityObject(CrawlFrame *pCF)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    REGDISPLAY*   pRD         = pCF->GetRegisterSet();
    IJitManager*   pJitMan    = pCF->GetJitManager();
    METHODTOKEN   methodToken = pCF->GetMethodToken();
    unsigned      relOffset   = pCF->GetRelOffset();
    CodeManState* pState      = pCF->GetCodeManState();

    LPVOID methodInfoPtr = pJitMan->GetGCInfo(methodToken);

    _ASSERTE(sizeof(CodeManStateBuf) <= sizeof(pState->stateBuf));
    CodeManStateBuf * stateBuf = (CodeManStateBuf*)pState->stateBuf;

#if defined(_X86_)
    /* Extract the necessary information from the info block header */
    stateBuf->hdrInfoSize = (DWORD)crackMethodInfoHdr(methodInfoPtr,
                                                      relOffset,
                                                      &stateBuf->hdrInfoBody);

    pState->dwIsSet = 1;
    if  (stateBuf->hdrInfoBody.securityCheck)
    {
        _ASSERTE(stateBuf->hdrInfoBody.ebpFrame);
        if(stateBuf->hdrInfoBody.prologOffs == hdrInfo::NOT_IN_PROLOG &&
                stateBuf->hdrInfoBody.epilogOffs == hdrInfo::NOT_IN_EPILOG)
        {
            return (OBJECTREF *)(size_t)(((DWORD)*pRD->pEbp) - GetSecurityObjectOffset(&stateBuf->hdrInfoBody));
        }
    }
#elif defined(USE_GC_INFO_DECODER)

#ifndef DACCESS_COMPILE
    BYTE* gcInfoAddr = (BYTE*) methodInfoPtr;

    GcInfoDecoder gcInfoDecoder(
            gcInfoAddr,
            DECODE_SECURITY_OBJECT,
            0
            );

    INT32 spOffset = gcInfoDecoder.GetSecurityObjectStackSlot();
    if( spOffset != NO_SECURITY_OBJECT )
    {
        UINT_PTR uCallerSP = GetCallerSp(pRD);

        if (pCF->IsFunclet())
        {
            if (!pCF->IsFilterFunclet())
            {
                // Cannot retrieve the security object for a non-filter funclet.
                return NULL;
            }

            DWORD    dwParentOffset  = 0;
            UINT_PTR uParentCallerSP = 0;

            // If this is a filter funclet, retrieve the information of the parent method
            // and use that to find the security object.
            ExceptionTracker::FindParentStackFrameForSecurityObject(pCF, &dwParentOffset, &uParentCallerSP);

            relOffset = dwParentOffset;
            uCallerSP = uParentCallerSP;
        }

        if(relOffset >= gcInfoDecoder.GetSecurityObjectValidRangeStart()
            && relOffset < gcInfoDecoder.GetSecurityObjectValidRangeEnd())
        {
            OBJECTREF* pSlot = (OBJECTREF*) (spOffset + uCallerSP);
            return pSlot;
        }
    }
#else
    DacNotImpl();
    return NULL;
#endif // #ifndef DACCESS_COMPILE

#else // !_X86_ && !USE_GC_INFO_DECODER
    PORTABILITY_ASSERT("EECodeManager::GetAddrOfSecurityObject is not implemented on this platform.");
#endif

    return NULL;
}

/*****************************************************************************
 *
 *  Returns "this" pointer if it is a non-static method
 *  AND the object is still alive.
 *  Returns NULL in all other cases.
 */
OBJECTREF EECodeManager::GetInstance( PREGDISPLAY    pContext,
                                      LPVOID         methodInfoPtr,
                                      ICodeInfo *    pCodeInfo,
                                      unsigned       relOffset)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        if (!IsGCSpecialThread()) {MODE_COOPERATIVE;} else {MODE_ANY;}
    } CONTRACTL_END;

#ifdef _X86_
    PTR_CBYTE    table = PTR_CBYTE((TADDR)methodInfoPtr);
    hdrInfo     info;
    unsigned    stackDepth;
    size_t      argBase;
    unsigned    count;

    /* Extract the necessary information from the info block header */

    table += crackMethodInfoHdr(methodInfoPtr,
                                relOffset,
                                &info);

    // We do not have accurate information in the prolog or the epilog
    if (info.prologOffs != hdrInfo::NOT_IN_PROLOG ||
        info.epilogOffs != hdrInfo::NOT_IN_EPILOG)
    {
        return NULL;
    }

    if  (info.interruptible)
    {
        stackDepth = scanArgRegTableI(skipToArgReg(info, table), (unsigned)relOffset, &info);
    }
    else
    {
        stackDepth = scanArgRegTable (skipToArgReg(info, table), (unsigned)relOffset, &info);
    }

    if (info.ebpFrame)
    {
        _ASSERTE(stackDepth == 0);
        argBase = *pContext->pEbp;
    }
    else
    {
        argBase =  pContext->Esp + stackDepth;
    }

    // Only synchronized methods and generic code that accesses
    // the type context via "this" need to report "this".
    // If its reported for other methods, its probably
    // done incorrectly. So flag such cases.
    _ASSERTE(info.thisPtrResult == REGI_NA ||
             pCodeInfo->IsSynchronized() ||
             pCodeInfo->AcquiresInstMethodTableFromThis());

    if (info.thisPtrResult != REGI_NA)
    {
        return ObjectToOBJECTREF(*(Object **)getCalleeSavedReg(pContext, info.thisPtrResult));
    }

#if VERIFY_GC_TABLES
    _ASSERTE(*castto(table, unsigned short *)++ == 0xBEEF);
#endif

    /* Parse the untracked frame variable table */

    /* The 'this' pointer can never be located in the untracked table */
    /* as we only allow pinned and byrefs in the untracked table      */

    count = info.untrackedCnt;
    while (count-- > 0)
    {
        fastSkipSigned(table);
    }

    /* Look for the 'this' pointer in the frame variable lifetime table     */

    count = info.varPtrTableSize;
    unsigned tmpOffs = 0;
    while (count-- > 0)
    {
        unsigned varOfs = fastDecodeUnsigned(table);
        unsigned begOfs = tmpOffs + fastDecodeUnsigned(table);
        unsigned endOfs = begOfs + fastDecodeUnsigned(table);
        _ASSERTE(!info.ebpFrame || (varOfs!=0));
        /* Is this variable live right now? */
        if (((unsigned)relOffset >= begOfs) && ((unsigned)relOffset < endOfs))
        {
            /* Does it contain the 'this' pointer */
            if (varOfs & this_OFFSET_FLAG)
            {
                unsigned ofs = varOfs & ~OFFSET_MASK;

                /* Tracked locals for EBP frames are always at negative offsets */

                if (info.ebpFrame)
                    argBase -= ofs;
                else
                    argBase += ofs;

                return (OBJECTREF)(size_t)(*(DWORD *)argBase);
            }
        }
        tmpOffs = begOfs;
    }

#if VERIFY_GC_TABLES
    _ASSERTE(*castto(table, unsigned short *) == 0xBABE);
#endif

#else
    PORTABILITY_ASSERT("Port: EECodeManager::GetInstance is not implemented on this platform.");
#endif // _X86_
    return NULL;
}


/*****************************************************************************
 *
 *  Returns the extra argument passed to to shared generic code if it is still alive.
 *  Returns NULL in all other cases.
 */
void *EECodeManager::GetParamTypeArg(PREGDISPLAY     pContext,
                                     LPVOID          methodInfoPtr,
                                     ICodeInfo      *pCodeInfo,
                                     unsigned        relOffset)

{
    WRAPPER_CONTRACT;

    _ASSERTE(pCodeInfo->RequiresInstArg());

#ifdef _X86_

    /* Extract the necessary information from the info block header */
    hdrInfo     info;
    PTR_CBYTE    table = PTR_CBYTE((TADDR)methodInfoPtr);
    table += crackMethodInfoHdr(methodInfoPtr,
                                relOffset,
                                &info);

    if (!info.handlers || 
        info.prologOffs != hdrInfo::NOT_IN_PROLOG || 
        info.epilogOffs != hdrInfo::NOT_IN_EPILOG)
    {
        return NULL;
    }

    const PBYTE fp = (const PBYTE)(size_t(GetRegdisplayFP(pContext)));
    return *(void**)(fp - GetParamTypeArgOffset(&info));

#else
    PORTABILITY_ASSERT("Port: EECodeManager::GetInstance is not implemented on this platform.");
    return NULL;
#endif // _X86_
}


/*****************************************************************************/

void * EECodeManager::GetGSCookieAddr(PREGDISPLAY     pContext,
                                      LPVOID          methodInfoPtr,
                                      ICodeInfo     * pCodeInfo,
                                      unsigned        relOffset,
                                      CodeManState  * pState)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    _ASSERTE(sizeof(CodeManStateBuf) <= sizeof(pState->stateBuf));
    CodeManStateBuf * stateBuf = (CodeManStateBuf*)pState->stateBuf;

#if defined(_X86_)
    /* Extract the necessary information from the info block header */
    hdrInfo * info = &stateBuf->hdrInfoBody;
    stateBuf->hdrInfoSize = (DWORD)crackMethodInfoHdr(methodInfoPtr,
                                                      relOffset,
                                                      info);

    pState->dwIsSet = 1;
    
    if (info->prologOffs != hdrInfo::NOT_IN_PROLOG ||
        info->epilogOffs != hdrInfo::NOT_IN_EPILOG ||
        info->gsCookieOffset == INVALID_GS_COOKIE_OFFSET)
    {
        return NULL;
    }
    
    if  (info->ebpFrame)
    {
        return PVOID(SIZE_T((DWORD(*pContext->pEbp) - info->gsCookieOffset)));
    }
    else
    {
        PTR_CBYTE table = PTR_CBYTE((TADDR)methodInfoPtr) + stateBuf->hdrInfoSize;       
        unsigned argSize = GetPushedArgSize(info, table, relOffset);
        
        return PVOID(SIZE_T(pContext->Esp + argSize + info->gsCookieOffset));
    }

#else
    PORTABILITY_WARNING("EECodeManager::GetGSCookieAddr is not implemented on this platform.");
    return NULL;
#endif
}


/*****************************************************************************
 *
 *  Returns true if the given IP is in the given method's prolog or epilog.
 */
bool EECodeManager::IsInPrologOrEpilog(DWORD        relPCoffset,
                                       LPVOID       methodInfoPtr,
                                       size_t*      prologSize)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

#ifndef USE_GC_INFO_DECODER
    hdrInfo info;

    crackMethodInfoHdr(methodInfoPtr, relPCoffset, &info);

    if (prologSize)
        *prologSize = info.prologSize;

    return ((info.prologOffs != hdrInfo::NOT_IN_PROLOG) || 
            (info.epilogOffs != hdrInfo::NOT_IN_EPILOG));
#else // USE_GC_INFO_DECODER
    _ASSERTE(!"@NYI - EECodeManager::IsInPrologOrEpilog (EETwain.cpp)");
    return false;
#endif // USE_GC_INFO_DECODER
}

/*****************************************************************************
 *
 *  Returns true if the given IP is in the synchronized region of the method (valid for synchronized functions only)
*/
bool  EECodeManager::IsInSynchronizedRegion(
                DWORD           relOffset,
                LPVOID          methodInfoPtr,
                unsigned        flags)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

#ifndef USE_GC_INFO_DECODER
    hdrInfo info;

    crackMethodInfoHdr(methodInfoPtr, relOffset, &info);

    // We should be called only for synchronized methods
    _ASSERTE(info.syncStartOffset != INVALID_SYNC_OFFSET && info.syncEndOffset != INVALID_SYNC_OFFSET);

    _ASSERTE(info.syncStartOffset < info.syncEndOffset);
    _ASSERTE(info.epilogCnt <= 1);
    _ASSERTE(info.epilogCnt == 0 || info.syncEndOffset <= info.syncEpilogStart);

    return (info.syncStartOffset < relOffset && relOffset < info.syncEndOffset) ||
        (info.syncStartOffset == relOffset && (flags & ActiveStackFrame)) ||
        // Synchronized methods have at most one epilog. The epilog does not have to be at the end of the method though.
        // Everything after the epilog is also in synchronized region.
        (info.epilogCnt != 0 && info.syncEpilogStart + info.epilogSize <= relOffset);
#else // USE_GC_INFO_DECODER
    _ASSERTE(!"@NYI - EECodeManager::IsInSynchronizedRegion (EETwain.cpp)");
    return false;
#endif // USE_GC_INFO_DECODER
}

/*****************************************************************************
 *
 *  Returns the size of a given function.
 */
size_t EECodeManager::GetFunctionSize(LPVOID  methodInfoPtr)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

#if defined(_X86_)
    hdrInfo info;

    crackMethodInfoHdr(methodInfoPtr, 0, &info);

    return info.methodSize;
#elif defined(USE_GC_INFO_DECODER)

    BYTE* gcInfoAddr = (BYTE*) methodInfoPtr;

    GcInfoDecoder gcInfoDecoder(
            gcInfoAddr,
            DECODE_CODE_LENGTH,
            0
            );

    UINT32 codeLength = gcInfoDecoder.GetCodeLength();
    _ASSERTE( codeLength > 0 );
    return codeLength;

#else // !_X86_ && !USE_GC_INFO_DECODER
    PORTABILITY_ASSERT("EECodeManager::GetFunctionSize is not implemented on this platform.");
    return 0;
#endif


}

/*****************************************************************************
 *
 *  Returns the size of the frame of the given function.
 */
unsigned int EECodeManager::GetFrameSize(LPVOID  methodInfoPtr)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

#ifndef USE_GC_INFO_DECODER
    hdrInfo info;

    crackMethodInfoHdr(methodInfoPtr, 0, &info);

    // currently only used by E&C callers need to know about doubleAlign
    // in all likelyhood
    _ASSERTE(!info.doubleAlign);
    return info.stackSize;
#else // USE_GC_INFO_DECODER
    PORTABILITY_ASSERT("EECodeManager::GetFrameSize is not implemented on this platform.");
    return false;
#endif // USE_GC_INFO_DECODER
}

#ifndef DACCESS_COMPILE

/*****************************************************************************/

const BYTE* EECodeManager::GetFinallyReturnAddr(PREGDISPLAY pReg)
{
    LEAF_CONTRACT;

#ifdef _X86_
    return *(const BYTE**)(size_t)(GetRegdisplaySP(pReg));
#else
    PORTABILITY_ASSERT("EECodeManager::GetFinallyReturnAddr is not implemented on this platform.");
    return NULL;
#endif
}

BOOL EECodeManager::IsInFilter(void *methodInfoPtr,
                              unsigned offset,
                              PCONTEXT pCtx,
                              DWORD curNestLevel)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

#ifdef _X86_

    /* Extract the necessary information from the info block header */

    hdrInfo     info;

    crackMethodInfoHdr(methodInfoPtr,
                       offset,
                       &info);

    /* make sure that we have an ebp stack frame */

    _ASSERTE(info.ebpFrame);
    _ASSERTE(info.handlers);

    TADDR       baseSP;
    DWORD       nestingLevel;

    FrameType   frameType = GetHandlerFrameInfo(&info, pCtx->Ebp,
                                                pCtx->Esp, (DWORD) IGNORE_VAL,
                                                &baseSP, &nestingLevel);
    _ASSERTE(frameType != FR_INVALID);

//    _ASSERTE(nestingLevel == curNestLevel);

    return frameType == FR_FILTER;

#else
    PORTABILITY_ASSERT("EECodeManager::IsInFilter is not implemented on this platform.");
    return FALSE;
#endif
}


BOOL EECodeManager::LeaveFinally(void *methodInfoPtr,
                                unsigned offset,
                                PCONTEXT pCtx)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

#ifdef _X86_

    hdrInfo info;

    crackMethodInfoHdr(methodInfoPtr,
                       offset,
                       &info);

    DWORD       nestingLevel;
    GetHandlerFrameInfo(&info, pCtx->Ebp, pCtx->Esp, (DWORD) IGNORE_VAL, NULL, &nestingLevel);

    // Compute an index into the stack-based table of esp values from
    // each level of catch block.
    PTR_TADDR pBaseSPslots = GetFirstBaseSPslotPtr(pCtx->Ebp, &info);
    PTR_TADDR pPrevSlot    = pBaseSPslots - (nestingLevel - 1);

    /* Currently, LeaveFinally() is not used if the finally is invoked in the
       second pass for unwinding. So we expect the finally to be called locally */
    _ASSERTE(*pPrevSlot == LCL_FINALLY_MARK);

    *pPrevSlot = 0; // Zero out the previous shadow ESP

    pCtx->Esp += sizeof(TADDR); // Pop the return value off the stack
    return TRUE;
#else
    PORTABILITY_ASSERT("EEJitManager::LeaveFinally is not implemented on this platform.");
    return FALSE;
#endif
}

void EECodeManager::LeaveCatch(void *methodInfoPtr,
                                unsigned offset,
                                PCONTEXT pCtx)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

#ifdef _X86_

#ifdef _DEBUG
    TADDR       baseSP;
    DWORD       nestingLevel;
    bool        hasInnerFilter;
    hdrInfo     info;

    crackMethodInfoHdr(methodInfoPtr, offset, &info);
    GetHandlerFrameInfo(&info, pCtx->Ebp, pCtx->Esp, (DWORD) IGNORE_VAL,
                        &baseSP, &nestingLevel, &hasInnerFilter);
//    _ASSERTE(frameType == FR_HANDLER);
//    _ASSERTE(pCtx->Esp == baseSP);
#endif

    return;

#else // !_X86_
    PORTABILITY_ASSERT("EECodeManager::LeaveCatch is not implemented on this platform.");
    return;
#endif // _X86_
}


#endif // #ifndef DACCESS_COMPILE

#ifdef DACCESS_COMPILE

void EECodeManager::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    DAC_ENUM_VTHIS();
}

#endif // #ifdef DACCESS_COMPILE


#ifndef DACCESS_COMPILE

/*
 *  GetAmbientSP
 *
 *  This function computes the zero-depth stack pointer for the given nesting
 *  level within the method given.  Nesting level is the the depth within
 *  try-catch-finally blocks, and is zero based.  It is up to the caller to
 *  supply a valid nesting level value.
 *
 */

TADDR EECodeManager::GetAmbientSP(EHContext      *ctx,
                                  LPVOID          methodInfoPtr,
                                  DWORD           nestingLevel,
                                  ICodeInfo      *pCodeInfo,
                                  CodeManState   *pState)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

#ifdef _X86_

    _ASSERTE(sizeof(CodeManStateBuf) <= sizeof(pState->stateBuf));
    CodeManStateBuf * stateBuf = (CodeManStateBuf*)pState->stateBuf;
    PTR_CBYTE table = PTR_CBYTE((TADDR)methodInfoPtr);

    /* Extract the necessary information from the info block header */

    stateBuf->hdrInfoSize = (DWORD)crackMethodInfoHdr(methodInfoPtr,
                                       ExecutionManager::JitCodeToOffset(PTR_TO_TADDR((INT_PTR)ctx->Eip)),
                                       &stateBuf->hdrInfoBody);
    table += stateBuf->hdrInfoSize;

    pState->dwIsSet = 1;

#ifdef  _DEBUG
    if (trFixContext) {
        printf("GetAmbientSP [%s][%s] for %s.%s: ",
               stateBuf->hdrInfoBody.ebpFrame?"ebp":"   ",
               stateBuf->hdrInfoBody.interruptible?"int":"   ",
               "UnknownClass","UnknownMethod");
        fflush(stdout);
    }
#endif

    if ((stateBuf->hdrInfoBody.prologOffs != hdrInfo::NOT_IN_PROLOG) ||
        (stateBuf->hdrInfoBody.epilogOffs != hdrInfo::NOT_IN_EPILOG))
    {
        return NULL;
    }

    /* make sure that we have an ebp stack frame */

    if (stateBuf->hdrInfoBody.handlers)
    {
        _ASSERTE(stateBuf->hdrInfoBody.ebpFrame);

        TADDR      baseSP;
        GetHandlerFrameInfo(&stateBuf->hdrInfoBody,
                            ctx->Ebp,
                            (DWORD) IGNORE_VAL,
                            nestingLevel,
                            &baseSP);

        _ASSERTE(((size_t)ctx->Ebp >= baseSP) && (baseSP >= (size_t)ctx->Esp));

        return baseSP;
    }

    _ASSERTE(nestingLevel == 0);

    if (stateBuf->hdrInfoBody.ebpFrame)
    {
        return GetOutermostBaseFP(ctx->Ebp, &stateBuf->hdrInfoBody);
    }

    TADDR baseSP = ctx->Esp;
    DWORD curOffs;
    pCodeInfo->getJitManager()->JitCodeToMethodInfo((BYTE *)(size_t)(ctx->Eip), NULL, NULL, &curOffs);
    if  (stateBuf->hdrInfoBody.interruptible)
    {
        baseSP += scanArgRegTableI(skipToArgReg(stateBuf->hdrInfoBody, table),
                                   curOffs,
                                   &stateBuf->hdrInfoBody);
    }
    else
    {
        baseSP += scanArgRegTable(skipToArgReg(stateBuf->hdrInfoBody, table),
                                  curOffs,
                                  &stateBuf->hdrInfoBody);
    }

    return baseSP;

#else // !_X86_
    PORTABILITY_ASSERT("EECodeManager::GetAmbientSP is not implemented on this platform.");
    return 0;
#endif // _X86_
}


#endif // #ifdef DACCESS_COMPILE

#endif // FJITONLY

