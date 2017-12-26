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
// File: ilgen.cpp
//
// Routines for generating il for a method
// ===========================================================================

#include "stdafx.h"

#if !DEBUG
#define dumpAllBlocks(x)
#define dumpAllBlocksContent(x)
#define initDumpingAllBlocks()
#endif

// We want to find out where we leak our temporaries...
#if DEBUG
#define allocTemporary(t, tk) allocTemporary(t, tk, __FILE__, __LINE__)
#endif

#define FREETEMP(temp) \
if (temp) { \
    freeTemporary(temp); \
    (temp) = NULL; \
}

// This is a macro instead of a function so that we can capture at what line it
#define STOREINTEMP(type, kind) \
    (storeLocal(allocTemporary(type, kind)))


// array of ilcodes...
const REFENCODING ILGENREC::ILcodes [] = {
#define OPDEF(id, name, pop, push, operand, type, len, b1, b2, cf) {b1, b2} ,
#include "opcode.def"
#undef OPDEF
};

/******************************************************************************
    Routines to map EXPRKINDs to IL opcodes.
******************************************************************************/
enum NumericOrderScheme {
    knosSigned,
    knosUnsigned,
    knosFloat
};

// The opcodes to emit based on ek and nos. Note that *psense is inverted iff
// (ek - EK_LT) is odd. If this needs to change to some non-trivial calculation,
// use a parallel table of bools to indicate whether *psense should be inverted.
static const ILCODE rgcodeCmp[] = {
  //  <           <=          >           >=
  CEE_CLT,    CEE_CGT,    CEE_CGT,    CEE_CLT,    // Signed
  CEE_CLT_UN, CEE_CGT_UN, CEE_CGT_UN, CEE_CLT_UN, // Unsigned
  CEE_CLT,    CEE_CGT_UN, CEE_CGT,    CEE_CLT_UN, // Float
};

__forceinline ILCODE CodeForCompare(EXPRKIND ek, TYPESYM * type, bool * psense)
{
    ASSERT(((int)*psense & ~1) == 0);

    if (ek <= EK_NE) {
        // EQ and NE don't depend on the nos.
        ASSERT(ek == EK_EQ || ek == EK_NE);
        if (ek == EK_NE)
            *psense ^= true;
        return CEE_CEQ;
    }



    int dek = ek - EK_LT;
    ASSERT(0 <= dek && dek < 4);

    // The sense is inverted for odd values of dek.
    *psense ^= (dek & 1);

    NumericOrderScheme nos;
    FUNDTYPE ft = type->fundType();

    if (ft == FT_R4 || ft == FT_R8)
        nos = knosFloat;
    else if (type->isUnsigned())
        nos = knosUnsigned;
    else
        nos = knosSigned;

    return FetchAtIndex(rgcodeCmp, dek + 4 * nos);
}
// The opcodes to emit based on ek, nos, and sense.
static const ILCODE rgcodeJmp[] = {
  //  <           <=          >           >=
  CEE_BLT,    CEE_BLE,    CEE_BGT,    CEE_BGE,    // Signed
  CEE_BGE,    CEE_BGT,    CEE_BLE,    CEE_BLT,    // Signed Invert
  CEE_BLT_UN, CEE_BLE_UN, CEE_BGT_UN, CEE_BGE_UN, // Unsigned
  CEE_BGE_UN, CEE_BGT_UN, CEE_BLE_UN, CEE_BLT_UN, // Unsigned Invert
  CEE_BLT,    CEE_BLE,    CEE_BGT,    CEE_BGE,    // Float
  CEE_BGE_UN, CEE_BGT_UN, CEE_BLE_UN, CEE_BLT_UN, // Float Invert
};

__forceinline ILCODE CodeForJump(EXPRKIND ek, TYPESYM * type, bool sense, ILCODE * pcodeRev)
{
    ASSERT(EK_EQ <= ek && ek <= EK_GE);
    ASSERT(((int)sense & ~1) == 0);

    if (ek <= EK_NE) {
        // EQ and NE don't depend on the nos.
        ASSERT(ek == EK_EQ || ek == EK_NE);
        *pcodeRev = CEE_ILLEGAL; // Not needed.
        return (ek - EK_EQ) ^ (int)sense ? CEE_BEQ : CEE_BNE_UN;
    }

    int dek = ek - EK_LT;
    ASSERT(0 <= dek && dek < 4);

    NumericOrderScheme nos;
    FUNDTYPE ft = type->fundType();

    if (ft == FT_R4 || ft == FT_R8)
        nos = knosFloat;
    else if (type->isUnsigned())
        nos = knosUnsigned;
    else
        nos = knosSigned;

    *pcodeRev = rgcodeJmp[dek + 8 * nos + 4 * (int) sense];
    return rgcodeJmp[dek + 8 * nos + 4 * (int) !sense];
}

// constructor, just use the local allocator...
ILGENREC::ILGENREC()
{
    allocator = &(compiler()->localSymAlloc);
}

// the method visible to the world...
void ILGENREC::compile(METHSYM * method, METHINFO * info, EXPR * tree) 
{
#if DEBUG
    if (compiler()->GetRegDWORD("SmallBlock")) {
        smallBlock = true;
    } else {
        smallBlock = false;
    }
#endif

    // save our context...
    this->method = method;
    this->cls = method->getClass();
    this->info = info;

    initDumpingAllBlocks();

    compileForEnc = !compiler()->options.m_fOPTIMIZATIONS;

    initFirstBB();

    BlobBldrNrHeap bldrLocalSlotArray(&compiler()->localSymAlloc);
    m_pbldrLocalSlotArray = &bldrLocalSlotArray;
    InitLocalsFromEnc();

    // initialize temporaries and locals and params
    temporaries = NULL;
    localScope = getLocalScope();
    if (localScope) {
        assignLocals(localScope);
    }
    assignParams();

    globalFieldCount = 0;
    curStack = 0;
    maxStack = 0;
    handlers = NULL;
    finallyNesting = 0;
    lastHandler = NULL;
    ehCount = 0;
    returnLocation = NULL;
    closeIndexUsed = false;
    blockedLeave = 0;
    origException = NULL;
    curDebugInfo = NULL;
    returnHandled = false;
    if (method->retType != compiler()->getBSymmgr().GetVoid() && (info->hasRetAsLeave || !compiler()->options.m_fOPTIMIZATIONS)) {
        retTemp = allocTemporary(method->retType, TK_RETURN);
    } else {
        retTemp = NULL;
    }

    extentCurrent.SetInvalid();
    nodeCurrent = NULL;

    // generate the prologue

    genPrologue(tree->asBLOCK());

    // generate the code
    genBlock(tree->asBLOCK());

    ASSERT(finallyNesting == 0);

    ASSERT(finallyNesting == 0);
    // do the COM+ magic to emit the method:
    SETLOCATIONSTAGE(EMITIL);

    unsigned codeSize = getFinalCodeSize();

    // make sure no temps leaked
#if DEBUG
    verifyAllTempsFree();
#endif

    DWORD dwFlags = 0;
    COR_ILMETHOD_FAT fatHeader;
    fatHeader.SetMaxStack(maxStack);
    fatHeader.SetCodeSize(codeSize);
    if (bldrLocalSlotArray.Length() > 0) {
        fatHeader.SetLocalVarSigTok(computeLocalSignature());
        dwFlags |= CorILMethod_InitLocals;
    } else {
        fatHeader.SetLocalVarSigTok(mdTokenNil);
    }
    fatHeader.SetFlags(dwFlags);

    COR_ILMETHOD_SECT_EH_CLAUSE_FAT * clauses = STACK_ALLOC(COR_ILMETHOD_SECT_EH_CLAUSE_FAT, ehCount);
    copyHandlers(clauses);

    bool moreSections = ehCount != 0;
    unsigned alignmentJunk;
    if (moreSections) {
        alignmentJunk = RoundUp4((int)codeSize) - codeSize;
        codeSize += alignmentJunk;
    } else {
        alignmentJunk = 0;
    }

    unsigned headerSize = COR_ILMETHOD::Size(&fatHeader, moreSections);
    unsigned ehSize = COR_ILMETHOD_SECT_EH::Size(ehCount, clauses);
    unsigned totalSize = headerSize + codeSize + ehSize;
    bool align = headerSize != 1;

    BYTE * buffer = (BYTE*) compiler()->emitter.EmitMethodRVA(method, totalSize, align ? 4 : 1);

    emitDebugInfo(codeSize - alignmentJunk, fatHeader.GetLocalVarSigTok());

#if DEBUG
    BYTE * bufferBeg, * endBuffer;

    bufferBeg = buffer;
    endBuffer = &buffer[totalSize];
#endif

    buffer += COR_ILMETHOD::Emit(headerSize, &fatHeader, moreSections, buffer);

    buffer = copyCode(buffer);

    memset (buffer, 0, alignmentJunk);
    buffer += alignmentJunk;
    
    buffer += COR_ILMETHOD_SECT_EH::Emit(ehSize, ehCount, clauses, false, buffer);

    ASSERT(buffer == endBuffer);
    m_pbldrLocalSlotArray = NULL;
}

// terminate the current bb with the given exit code & jump destination
void ILGENREC::endBB(ILCODE exitIL, BBLOCK * jumpDest, ILCODE exitILRev)
{
    // remember, if currently being emitted to, the bb has its exit set to CEE_ILLEGAL
    ASSERT(inlineBB.exitIL == CEE_ILLEGAL);

    inlineBB.exitIL = exitIL;
    if (exitIL == CEE_LEAVE) {
        ASSERT(jumpDest);
        if (jumpDest) {
            jumpDest->leaveTarget = true;
        }
    }
    inlineBB.exitILRev = exitILRev;
    if (exitIL == CEE_LEAVE) {
        inlineBB.leaveNesting = finallyNesting;
    }
#if DEBUG
    if (jumpDest) {
        inlineBB.jumpDest = jumpDest;
    } else {
        memset( &inlineBB.jumpDest, 0xCCCCCCCC, sizeof(BBLOCK*));
    }
#else
    inlineBB.jumpDest = jumpDest;
#endif

    // copy the code into a more permanent place...
    inlineBB.curLen = inlineBB.code - reusableBuffer;
    BYTE * newBuffer = ((BYTE*) allocator->Alloc(BB_PREBUFFER + inlineBB.curLen + BB_TOPOFF)) + BB_PREBUFFER;
    memcpy(newBuffer, reusableBuffer, inlineBB.curLen);
    inlineBB.code = newBuffer;

    // and finally code the bb info to its permanent mapping
    memcpy(currentBB, &inlineBB, sizeof(BBLOCK));
}


// initialzie the inline bb.  we only initialize the fields we know we
// are going to read before ending the bb.
inline void ILGENREC::initInlineBB()
{
    inlineBB.code = reusableBuffer;
    inlineBB.debugInfo = NULL;
    inlineBB.startsTry = false;
    inlineBB.endsFinally = false;
    inlineBB.startsCatchOrFinally = false;
    inlineBB.jumpIntoTry = false;
    inlineBB.gotoBlocked = false;
    inlineBB.tryNesting = 0;
    inlineBB.leaveNesting = 0;
    inlineBB.leaveTarget = false;
#if DEBUG
    inlineBB.exitIL = CEE_ILLEGAL;
    inlineBB.startOffset = (unsigned) -1;
    inlineBB.sym = NULL;
    inlineBB.curLen = 0xffffffff;
#endif
}


// create a new basic block and maybe make it the current one so that
// we can emit to it.
BBLOCK * ILGENREC::createNewBB(bool makeCurrent)
{
    BBLOCK * rval = (BBLOCK*) allocator->Alloc(sizeof(BBLOCK));
    rval->leaveTarget = false;
    if (makeCurrent) {
        ASSERT(inlineBB.exitIL != CEE_ILLEGAL);

        initInlineBB();
        currentBB = rval;
    }

    return rval;
}

// close the previuos bb and start a new one, either the one provided
// by next, or a brand new one...
// Return the now current BB
BBLOCK * ILGENREC::startNewBB(BBLOCK * next, ILCODE exitIL, BBLOCK * jumpDest, ILCODE exitILRev)
{
    endBB(exitIL, jumpDest, exitILRev);
    if (next) {
        currentBB->next = next;
        initInlineBB();
        currentBB = next;
        inlineBB.leaveTarget = next->leaveTarget;
    } else {
        BBLOCK * prev = currentBB;
        createNewBB(true);
        prev->next = currentBB;
    }
    return currentBB;
}

// this gets called if the inline bb gets too full, basically, we just
// didn't want to inline one extra param (the null)
void ILGENREC::flushBB()
{
    startNewBB(NULL);
}

// initialize the first bb before generating code
void ILGENREC::initFirstBB()
{
#if DEBUG
    inlineBB.exitIL = cee_next;
#endif
    firstBB = currentBB = createNewBB(true);
}

// emit a single opcode to e given buffer.  advance the buffer by the size of the opcode
__forceinline void ILGENREC::putOpcode(BYTE ** buffer, ILCODE opcode)
{
    ASSERT(opcode != CEE_ILLEGAL && opcode != CEE_UNUSED1 && opcode < cee_last);
    REFENCODING ref = FetchAtIndex(ILcodes, opcode);
    if (ref.b1 != 0xFF) {
        ASSERT(FetchAtIndex(ILcodesSize, opcode) == 2);
        *(REFENCODING*)(*buffer) = ref;
        (*buffer) += 2;
    } else {
        ASSERT(FetchAtIndex(ILcodesSize, opcode) == 1);
        (**buffer) = ref.b2;
        (*buffer)++;
    }
}

// emit, but to the default buffer from the inline bb
__forceinline void ILGENREC::putOpcode(ILCODE opcode)
{
    if (inlineBB.code > (reusableBuffer + BB_SIZE)) {
        flushBB();
    }
#if DEBUG
    if (smallBlock) {
        startNewBB(NULL);
    }
#endif
    putOpcode(&(inlineBB.code), opcode);
    curStack += ILStackOps[opcode];
    markStackMax();
}

// write a given value to the ilcode stream
void ILGENREC::putWORD(WORD w)
{
    SET_UNALIGNED_VAL16(inlineBB.code, w);
    inlineBB.code += sizeof(WORD);
}

void ILGENREC::putDWORD(DWORD dw)
{
    SET_UNALIGNED_VAL32(inlineBB.code, dw);
    inlineBB.code += sizeof(DWORD);
}

void ILGENREC::putCHAR(char c)
{
    (*(char*)(inlineBB.code)) = c;
    inlineBB.code += sizeof(char);
}

void ILGENREC::putQWORD(__int64 * qv)
{
    SET_UNALIGNED_VAL64(inlineBB.code, *qv);
    inlineBB.code += sizeof(__int64);
}

// return the scope just below the outermost param scope, if any
SCOPESYM * ILGENREC::getLocalScope()
{

    SYM * current = info->outerScope->firstChild;
    while (current) {
        if (current->isSCOPESYM() && !(current->asSCOPESYM()->scopeFlags & SF_ARGSCOPE)) {
            return current->asSCOPESYM();
        }
        current = current->nextChild;
    };

    return NULL;
}


void ILGENREC::initLocal(PSLOT slot, TYPESYM * type)
{
    if (!type) {
        type = slot->type;
        ASSERT(type);
    }
    
    if (type->fundType() != FT_STRUCT) { // || type->getPredefType() == PT_INTPTR) {
        genZero(NULL, type);
        dumpLocal(slot, true);
    } else {
        genSlotAddress(slot);
        putOpcode(CEE_INITOBJ);
        emitTypeToken(type);
    }

}

bool ILGENREC::isExprOptimizedAway(EXPR * tree)
{
AGAIN:
    switch (tree->kind) {       
    case EK_LOCAL:
        return tree->asLOCAL()->local->slot.type == NULL;
    case EK_FIELD:
        tree = tree->asFIELD()->object;
        if (tree) goto AGAIN;
        return false;
    default:
        return false;
    }
}

// get the list of locals from the EnC manager and pre-poluate the local slots
void ILGENREC::InitLocalsFromEnc()
{
}

// assign slots to all locals for a given scope and starting with
// a provided index.  returns the next available slot
void ILGENREC::assignLocals(SCOPESYM * scope)
{
    LOCVARSYM * loc;

    SYM * current = scope->firstChild;
    while (current) {
        switch (current->getKind()) {
        case SK_LOCVARSYM:
            loc = current->asLOCVARSYM();
            if (!loc->isConst && (!compiler()->options.m_fOPTIMIZATIONS || loc->slot.IsUsed()))
            {
                if (loc->fIsIteratorLocal) {
                    loc->slot.SetIndex((uint)-1);
                    loc->slot.type = NULL;
                }
                else {
                    TYPESYM * type = loc->type;
                    if (loc->slot.isPinned)
                        type = compiler()->getBSymmgr().GetPinnedType(type);
                    loc->slot.SetIndex(GetLocalSlot(loc->name, type));
                    loc->slot.type = loc->type;
                }
                loc->slot.isParam = false;
                loc->firstUsed.SetUninitialized();
            }
            else {
                loc->slot.type = NULL;
                loc->slot.SetIndex(0);
            }
            break;
        case SK_SCOPESYM:
            assignLocals(current->asSCOPESYM());
            break;
        case SK_ANONSCOPESYM:
        case SK_CACHESYM:
        case SK_LABELSYM:
            break;
        default:
            VSFAIL("Unexpected sym kind");
            break;
        }
        current = current->nextChild;
    }
}

uint ILGENREC::GetLocalSlot(NAME * name, TYPESYM * type)
{
    ASSERT(type != NULL && name != NULL);
    ASSERT(m_pbldrLocalSlotArray->Length() % sizeof(IlSlotInfo) == 0);

    // Normalize everything
    type = compiler()->getBSymmgr().SubstType(type, SubstTypeFlags::NormAll);
    if (!compiler()->options.m_fOPTIMIZATIONS) {
        // Look for an existing slot
        for (IlSlotInfo * pisi = (IlSlotInfo*)m_pbldrLocalSlotArray->Buffer(),
                * pisiLim = (IlSlotInfo*)(m_pbldrLocalSlotArray->Buffer() + m_pbldrLocalSlotArray->Length());
                pisi < pisiLim; pisi++)
        {
            if (pisi->name == name && pisi->type == type) {
                pisi->fIsUsed = true;
                return pisi->ilSlotNum;
            }
        }
    }
    // If none are found (or we're not reusing slots as is the case for optimized builds)
    // Just add a new one at the end of the list
    IlSlotInfo * pisi = (IlSlotInfo*)m_pbldrLocalSlotArray->AddBuf(sizeof(IlSlotInfo));
    pisi->name = name;
    pisi->type = type;
    pisi->fIsUsed = true;
    pisi->ilSlotNum = m_pbldrLocalSlotArray->Length() / sizeof(IlSlotInfo) - 1;
    return pisi->ilSlotNum;
}

uint ILGENREC::GetLocalSlot(TEMP_KIND tempKind, TYPESYM * type)
{
    ASSERT(type != NULL);
    ASSERT(m_pbldrLocalSlotArray->Length() % sizeof(IlSlotInfo) == 0);

    // Normalize everything
    type = compiler()->getBSymmgr().SubstType(type, SubstTypeFlags::NormAll);

    // In non-EnC builds this array only contains used items, so don't bother to search it
    if (compiler()->FEncBuild()) {
        // Look for an existing unused slot
        for (IlSlotInfo * pisi = (IlSlotInfo*)m_pbldrLocalSlotArray->Buffer(),
                * pisiLim = (IlSlotInfo*)(m_pbldrLocalSlotArray->Buffer() + m_pbldrLocalSlotArray->Length());
                pisi < pisiLim; pisi++)
        {
            if (pisi->name == NULL && pisi->tempKind == tempKind && pisi->type == type && !pisi->fIsUsed) {
                pisi->fIsUsed = true;
                return pisi->ilSlotNum;
            }
        }
    }

    // If none are found just add a new one at the end of the list
    IlSlotInfo * pisi = (IlSlotInfo*)m_pbldrLocalSlotArray->AddBuf(sizeof(IlSlotInfo));
    pisi->name = NULL;
    pisi->tempKind = tempKind;
    pisi->type = type;
    pisi->fIsUsed = true;
    pisi->ilSlotNum = m_pbldrLocalSlotArray->Length() / sizeof(IlSlotInfo) - 1;
    return pisi->ilSlotNum;
}

// assign slots to all parameters of the method being compiled...
void ILGENREC::assignParams()
{
    unsigned curSlot = 0;

    SYM * current = info->outerScope->firstChild;

    while (current) {
        if (current->isLOCVARSYM()) {
            LOCVARSYM * loc = current->asLOCVARSYM();
            loc->slot.type = loc->type;
            loc->slot.isParam = true;
            loc->slot.SetIndex(curSlot++);
            ASSERT(!loc->isThis || loc->slot.Index() == 0);
        }
        current = current->nextChild;
    }
}


// compute the signature of all locals, explicit and temporaries
mdToken ILGENREC::computeLocalSignature()
{
    // count the temps:
    unsigned temps = 0;
    TEMPBUCKET *bucket;
    for(bucket = temporaries; bucket; bucket = bucket->next) {
        for (int i = 0; i < TEMPBUCKETSIZE  && bucket->slots[i].type; ++temps, ++i);
    }

    int total = m_pbldrLocalSlotArray->Length() / sizeof(IlSlotInfo);

    // allocate enough space
    TYPESYM ** types = STACK_ALLOC(TYPESYM*, total);
#if DEBUG
    memset(types, 0, sizeof(TYPESYM*) * total);
#endif

    // collect the local types and temporaries
    TYPESYM ** current = types;
    IlSlotInfo * pisiLim = (IlSlotInfo*)(m_pbldrLocalSlotArray->Buffer() + m_pbldrLocalSlotArray->Length());
    for (IlSlotInfo * pisi = (IlSlotInfo*)m_pbldrLocalSlotArray->Buffer(); pisi < pisiLim; pisi++) {
        ASSERT(pisi->type != NULL);
        *current++ = pisi->type;
    }

    return compiler()->emitter.GetSignatureRef(compiler()->getBSymmgr().AllocParams(total, types));
}

void ILGENREC::handleReturn(bool addDebugInfo)
{
    if (returnLocation && !returnHandled && !retTemp) {
        startNewBB(returnLocation);
        returnHandled = true;
    }
    if (method->retType != compiler()->getBSymmgr().GetVoid()) curStack --;

    if (addDebugInfo) {
        if (closeIndexUsed) {
            openDebugInfo(SpecialDebugPoint::HiddenCode, EXF_NODEBUGINFO);
        } else {
            openDebugInfo(SpecialDebugPoint::CloseCurly, 0);
            
        }
    }

    putOpcode(CEE_RET);
}


void ILGENREC::genPrologue(EXPRBLOCK* tree)
{

    if (!tree->statements && !compiler()->options.m_fOPTIMIZATIONS && !shouldEmitNopForBlock(tree)) {
        putOpcode(CEE_NOP);
    }
}

void ILGENREC::genHashtableStringSwitchInit(EXPRSWITCH * tree)
{
    tree->hashtableToken = compiler()->emitter.GetGlobalFieldDef(method, ++globalFieldCount, compiler()->getBSymmgr().atsDictionary);

    BBLOCK * contBlock = createNewBB();

    putOpcode(CEE_VOLATILE);
    putOpcode(CEE_LDSFLD);
    putDWORD(tree->hashtableToken);
    startNewBB(NULL, CEE_BRTRUE, contBlock);
    curStack-=1;
    
    int labelCount = tree->labelCount;
    if (tree->flags & EXF_HASDEFAULT)
        labelCount--;
    ASSERT(labelCount >= 0);

    genIntConstant(labelCount);
    putOpcode(CEE_NEWOBJ);
    emitMethodToken(compiler()->getBSymmgr().methDictionaryCtor, compiler()->getBSymmgr().atsDictionary);
    curStack-=1;

    EXPRSWITCHLABEL ** start = tree->labels;
    EXPRSWITCHLABEL ** end = start + labelCount;

    int caseNumber = 0;

    while (start != end) {
        if (!start[0]->key->asCONSTANT()->isNull()) {
            putOpcode(CEE_DUP); // dup the hashtable...
            genString(start[0]->key->asCONSTANT()->getSVal());
            genIntConstant(caseNumber++);
            putOpcode(CEE_CALL);
            emitMethodToken(compiler()->getBSymmgr().methDictionaryAdd, compiler()->getBSymmgr().atsDictionary);
            curStack -= 3;
        }
        start ++;
    }

    putOpcode(CEE_VOLATILE);
    putOpcode(CEE_STSFLD);
    putDWORD(tree->hashtableToken);
   
    startNewBB(contBlock);

}



bool ILGENREC::shouldEmitNopForBlock(EXPRBLOCK * tree)
{
    if (tree->tree != NULL && tree->tree->kind == NK_BLOCK && tree->tree->asBLOCK()->iClose != -1 &&
        !(tree->flags & EXF_NODEBUGINFO) && !compiler()->options.m_fOPTIMIZATIONS && (info == NULL || !info->noDebugInfo))
    {
        BLOCKNODE * block = tree->tree->asBLOCK();
        if (block->pParent && block->pParent->kind == NK_TRY && !!(block->pParent->flags & NF_TRY_FINALLY) &&
            block->pStatements && block->pStatements->kind == NK_TRY && !!(block->pStatements->flags & NF_TRY_CATCH))
        {
            // If my iClose == the last catch's iClose, then this is really a try/catch/finally
            // and so we shouldn't emit the NOP
            BASENODE * pCatches = block->pStatements->asTRY()->pCatch;
            while (pCatches->kind == NK_LIST)
                pCatches = pCatches->asLIST()->p2;
            if (block->iClose == pCatches->asCATCH()->pBlock->iClose)
                return false;
        }
        return true;
    }
    return false;

}

void ILGENREC::emitNopForCurly(EXPRBLOCK * block, bool openCurly)
{
    openDebugInfo(block->tree->asBLOCK(), openCurly);
    putOpcode(CEE_NOP);
    closeDebugInfo();
    extentCurrent.SetInvalid();

}

// generate code for a block
void ILGENREC::genBlock(EXPRBLOCK * tree)
{
    // record debug info for scopes, so we can emit that later.
    if (TrackDebugInfo() && tree->scopeSymbol) {
        tree->scopeSymbol->debugBlockStart = currentBB;
        tree->scopeSymbol->debugOffsetStart = getCOffset();
    }
    const bool needNopsForCurlies = shouldEmitNopForBlock(tree);

    if (needNopsForCurlies) {
        emitNopForCurly(tree, true);
    }

    GenStmtChain(tree->statements);

    if (tree->flags & EXF_NEEDSRET) {
        handleReturn(true);
        startNewBB(NULL, CEE_RET);
        closeDebugInfo();
    } else if (needNopsForCurlies) {
        emitNopForCurly(tree, false);
    }

    if (TrackDebugInfo() && tree->scopeSymbol) {
        tree->scopeSymbol->debugBlockEnd = currentBB;
        tree->scopeSymbol->debugOffsetEnd = getCOffset();
    }
}


__forceinline bool ILGENREC::TrackDebugInfo()
{
	return (compiler()->options.m_fEMITDEBUGINFO && (info == NULL || !info->noDebugInfo));
}


long ILGENREC::getCloseIndex() 
{

    BLOCKNODE * block;

    if (!method) return -1;
    BASENODE * tree = method->getParseTree();
    if (!tree) return -1;

    switch (tree->kind) {
    case NK_ACCESSOR:
        return tree->asACCESSOR()->iClose;
    case NK_METHOD:
    case NK_CTOR:
    case NK_OPERATOR:
    case NK_DTOR:
        block = tree->asANYMETHOD()->pBody;
        if (!block) return -1;
        return block->iClose;

    default:
        return -1;
    }
    
}


long ILGENREC::getOpenIndex()
{
    if (!method) return -1;
    BASENODE * tree = method->getParseTree();
    if (!tree) return -1;

    switch (tree->kind) {
    case NK_ACCESSOR:
        return tree->asACCESSOR()->iOpen;
    case NK_METHOD:
    case NK_CTOR:
    case NK_DTOR:
    case NK_OPERATOR:
        return  tree->asANYMETHOD()->iOpen;

    default:
        return -1;
    }
}


void ILGENREC::openDebugInfo(SpecialDebugPointEnum e, int flags)
{

    if (!TrackDebugInfo() || (e == SpecialDebugPoint::HiddenCode  && !(flags & EXF_NODEBUGINFO))) return;

    createNewDebugInfo();
    emitDebugDataPoint(e, flags);
}

void ILGENREC::openDebugInfo(BLOCKNODE * block, bool openCurly)
{
    if (!TrackDebugInfo()) return;

    createNewDebugInfo();
    emitDebugDataPoint(block, openCurly);
}

void ILGENREC::openDebugInfo(BASENODE * tree, int flags)
{
    if (!TrackDebugInfo()) return;

    createNewDebugInfo();
    emitDebugDataPoint(tree, flags);
}

void ILGENREC::createNewDebugInfo()
{
    ASSERT(TrackDebugInfo());
    ASSERT(!curDebugInfo);
    curDebugInfo = (DEBUGINFO*) allocator->AllocZero(sizeof(DEBUGINFO));
    curDebugInfo->beginBlock = currentBB;
    curDebugInfo->extent.SetHiddenInvalidSource();
    curDebugInfo->prev = inlineBB.debugInfo;
    if (inlineBB.debugInfo) {
        inlineBB.debugInfo->next = curDebugInfo;
    }
    inlineBB.debugInfo = curDebugInfo;
    curDebugInfo->beginOffset = (unsigned short) getCOffset();

}

void ILGENREC::GenStmtChain(EXPRSTMT * tree)
{
    for (EXPRSTMT * stmt = tree; stmt; stmt = stmt->stmtNext)
        GenStatement(stmt);
}

void ILGENREC::GenStatement(EXPRSTMT * tree)
{
    if (!tree)
        return;

    if (!tree->FReachable()) {
        // Filter out unreachable statements. We need to process EK_SWITCH
        // and EK_SWITCHLABEL because they may contain reachable code, even
        // when they themselves are unreachable (because of constant switch
        // values and gotos).
        switch (tree->kind) {
        default:
            return;
        case EK_SWITCH:
        case EK_SWITCHLABEL:
        case EK_LABEL:
            break;
        }
    }

    SETLOCATIONNODE(tree->tree);

    if (TrackDebugInfo()) {
        switch (tree->kind) {
        case EK_BLOCK:
        case EK_SWITCHLABEL:
        case EK_TRY:
            break;
        default:
            openDebugInfo(tree->tree, tree->flags);
            break;
        }
    }

    switch (tree->kind) {
    case EK_STMTAS:
        genExpr(tree->asSTMTAS()->expression, false);
        break;
    case EK_RETURN:
        genReturn(tree->asRETURN());
        break;
    case EK_DECL:
        maybeEmitDebugLocalUsage(tree->tree, tree->asDECL()->sym);
        genExpr(tree->asDECL()->init, false);
        break;
    case EK_BLOCK:
        genBlock(tree->asBLOCK());
        break;
    case EK_GOTO:
        genGoto(tree->asGOTO());
        break;
    case EK_GOTOIF:
        genGotoIf(tree->asGOTOIF());
        break;
    case EK_SWITCHLABEL:
        genLabel(tree->asSWITCHLABEL());
        GenStmtChain(tree->asSWITCHLABEL()->statements);
        break;
    case EK_LABEL:
        genLabel(tree->asLABEL());
        break;
    case EK_SWITCH:
        genSwitch(tree->asSWITCH());
        break;
    case EK_THROW:
        genThrow(tree->asTHROW());
        break;
    case EK_TRY:
        genTry(tree->asTRY());
        break;
    case EK_NOOP:
        break;
    case EK_DEBUGNOOP:
        if (!compiler()->options.m_fOPTIMIZATIONS) {
            putOpcode(CEE_NOP);
        }
        break;
    default:
        VSFAIL("Bad stmt expr kind");
        break;
    }

    closeDebugInfo();
    ASSERT(curStack == 0);
}


void ILGENREC::closeDebugInfo() 
{

    if (!curDebugInfo) return;

    ASSERT(TrackDebugInfo());

#if DEBUG
    if (curDebugInfo->prev) {
        ASSERT(curDebugInfo->prev->next == curDebugInfo);
    }
#endif

    if (curDebugInfo->beginBlock == currentBB && curDebugInfo->beginOffset == getCOffset()) {
        if (curDebugInfo->prev) {
            curDebugInfo->prev->next = NULL;
        }
        if (curDebugInfo->beginBlock == currentBB) {
            ASSERT(inlineBB.debugInfo == curDebugInfo);
            inlineBB.debugInfo = curDebugInfo->prev;
        } else {
            ASSERT(curDebugInfo->beginBlock->debugInfo == curDebugInfo);
            curDebugInfo->beginBlock->debugInfo = curDebugInfo->prev;
        }
        curDebugInfo->beginBlock->debugInfo = curDebugInfo->prev;
        curDebugInfo = NULL;
        return;
    }
    curDebugInfo->endBlock = currentBB;
    curDebugInfo->endOffset = getCOffset();
    curDebugInfo = NULL;
}

__forceinline void ILGENREC::maybeEmitDebugLocalUsage(BASENODE * tree, LOCVARSYM * sym)
{
    if (TrackDebugInfo() && (sym->slot.type || sym->fIsIteratorLocal)) {
        emitDebugLocalUsage(tree, sym);
    }
}


INFILESYM * ILGENREC::getInfileFromTree(BASENODE * tree)
{
    NAME * inputFileName = tree->GetContainingFileName();
    INFILESYM * inputFile = compiler()->getBSymmgr().FindInfileSym(inputFileName);
    return inputFile;
}

SourceExtent __fastcall ILGENREC::getPosFromTree(BASENODE * tree, int flags)
{
    INFILESYM * infile = getInfileFromTree(tree);

    if (tree->kind == NK_FOR && (tree->flags & NF_FOR_FOREACH) && !(flags & EXF_USEORIGDEBUGINFO)) {
        return getSpecialPos(tree->asFOR()->iInKeyword, infile);
    }

    SourceExtent extent;

    extent.infile = infile;
    ASSERT(extent.infile);
    if (extent.infile) {        
        HRESULT hr = extent.infile->pData->GetExtentEx( tree, &extent.begin, &extent.end, EF_SINGLESTMT);
        ASSERT(hr == S_OK);
        if (FAILED(hr))
            extent.SetInvalid();
    }

    return extent;
}

SourceExtent __fastcall ILGENREC::getSpecialPos(SpecialDebugPointEnum e)
{
    long index;

    // Only called in debug path
    ASSERT(TrackDebugInfo());

    switch(e) {
    default:
        VSFAIL("Bad debug point!");
        __assume(0);
        // fall through
    case SpecialDebugPoint::HiddenCode:
        {
            SourceExtent extent;
            extent.SetHiddenInvalidSource();
            return extent;
        }

    case SpecialDebugPoint::OpenCurly:
        index = getOpenIndex();
        break;
    case SpecialDebugPoint::CloseCurly:
        index = getCloseIndex();
        break;
    }

    return getSpecialPos(index, method->getInputFile());
}

SourceExtent __fastcall ILGENREC::getSpecialPos(long index, INFILESYM * infile)
{
    LEXDATA         ld;
    SourceExtent extent;

    // Only called in debug path
    ASSERT(TrackDebugInfo());

    HRESULT hr = infile->pData->GetLexResults (&ld);
    ASSERT(SUCCEEDED(hr));
    if (FAILED(hr) || index == -1) {
        extent.SetHiddenInvalidSource();
    }
    else {
        extent.fNoDebugInfo = false;
        extent.infile = infile;
        extent.begin = ld.TokenAt(index);
        extent.end = ld.TokenAt(index).StopPosition();
    }

    return extent;
}

void ILGENREC::emitDebugLocalUsage(BASENODE * tree, LOCVARSYM * sym)
{

    // Only called in debug path
    ASSERT(TrackDebugInfo());

    if (tree != nodeCurrent) {
        nodeCurrent = tree;
        extentCurrent = getPosFromTree(tree, 0);
    }

    ASSERT(!extentCurrent.IsValidSource() || extentCurrent.infile == getInfileFromTree(sym->getParseTree()));

    if (extentCurrent.IsValidSource() || extentCurrent.begin < sym->firstUsed) {
        sym->firstUsed = extentCurrent.begin;
        sym->debugBlockFirstUsed = currentBB;
        sym->debugOffsetFirstUsed = getCOffset();
    }
}


void ILGENREC::emitDebugDataPoint(SpecialDebugPointEnum e, int flags)
{

    // Only called in debug path
    ASSERT(TrackDebugInfo());
    ASSERT(curDebugInfo);
    SourceExtent extent;

    switch(e) {
    case SpecialDebugPoint::HiddenCode:
        ASSERT(flags & EXF_NODEBUGINFO);
        extentCurrent.SetHiddenInvalidSource();
        nodeCurrent = NULL;
        setDebugDataPoint();
        return;
    case SpecialDebugPoint::OpenCurly:
    case SpecialDebugPoint::CloseCurly:
        extent = getSpecialPos(e);
        break;
    default:
        VSFAIL("Bad debug point!");
        __assume(0);
        return;
    }

    if (!extentCurrent.IsValidSource() || extent.end.IsUninitialized() || extentCurrent.end.IsUninitialized() || extent.end > extentCurrent.end) {
        extentCurrent = extent;
        nodeCurrent = NULL;
        if (flags & EXF_NODEBUGINFO) {
            extentCurrent.fNoDebugInfo = true;
        }
        else if (e == SpecialDebugPoint::CloseCurly) {
            closeIndexUsed = true;
        }
        setDebugDataPoint();
    }
}

void ILGENREC::emitDebugDataPoint(BLOCKNODE * block, bool openCurly)
{

    // Only called in debug path
    ASSERT(TrackDebugInfo());
    ASSERT(curDebugInfo);
    SourceExtent extent;
    if (openCurly) {
        extent = getSpecialPos(block->tokidx, getInfileFromTree(block));
    } else {
        extent = getSpecialPos(block->iClose, getInfileFromTree(block));
    }

    if (!extentCurrent.IsValidSource() || extentCurrent.end.IsUninitialized() || extent.end > extentCurrent.end) {
        extentCurrent = extent;
        nodeCurrent = NULL;
        setDebugDataPoint();
    }
}

void ILGENREC::emitDebugDataPoint(BASENODE * tree, int flags)
{

    // Only called in debug path
    ASSERT(TrackDebugInfo());
    ASSERT(curDebugInfo);

    if (!tree || (tree == nodeCurrent && !(flags & EXF_NODEBUGINFO))) return;

    // Only set this if we don't have a statement already
    if (!curDebugInfo->extent.IsValidSource()) {
        if (flags & EXF_LASTBRACEDEBUGINFO) {
            nodeCurrent = NULL;
            extentCurrent = getSpecialPos(SpecialDebugPoint::CloseCurly);
            if (!extentCurrent.IsValid())
                extentCurrent.SetHiddenInvalidSource();
        } else {
            nodeCurrent = tree;
            extentCurrent = getPosFromTree(tree, flags);
            if (flags & EXF_NODEBUGINFO) {
                extentCurrent.fNoDebugInfo = true;
                nodeCurrent = NULL;
            }
        }
        setDebugDataPoint();
    }
}


void ILGENREC::setDebugDataPoint()
{

    // Only called in debug path
    ASSERT(TrackDebugInfo());

    if (!curDebugInfo->extent.IsValidSource()) {
        curDebugInfo->extent = extentCurrent;
    }
    else if (extentCurrent.IsValidSource()) {
        ASSERT(!extentCurrent.end.IsUninitialized());
        ASSERT(!extentCurrent.begin.IsUninitialized());
        ASSERT(extentCurrent.infile == curDebugInfo->extent.infile);
        ASSERT(extentCurrent.fNoDebugInfo == curDebugInfo->extent.fNoDebugInfo);

        if (extentCurrent.end > curDebugInfo->extent.end || curDebugInfo->extent.end.IsUninitialized()) {
            curDebugInfo->extent.end = extentCurrent.end;
        }
        if (extentCurrent.begin < curDebugInfo->extent.begin || curDebugInfo->extent.begin.IsUninitialized()) {
            curDebugInfo->extent.begin = extentCurrent.begin;
        }
    }
}

void ILGENREC::emitDebugInfo(unsigned codeSize, mdToken tkLocalVarSig)
{
    if(!TrackDebugInfo()) return;

    // Count how many sequence points we have.
    int count = 0;
    BBLOCK *current;
    for (current = firstBB; current; current = current->next) {
        for (DEBUGINFO * debInfo = current->debugInfo; debInfo; debInfo = debInfo->prev) {
            count ++;
        }
    }

    //If none, don't emit any debug info.
    if (count == 0)
        return;

    // Begin emitting method debug info.
    compiler()->emitter.EmitDebugMethodInfoStart(method);


    // add one, for the possibility that we may have prologue code
    count++;

    // allocate arrays for offsets and extents.
    unsigned int * offsets;
    SourceExtent * extents;

    unsigned int allocSize = count * (sizeof(int) + sizeof(SourceExtent));
    
    if (allocSize > 0x00040000) { 
        // if we alloc more than 1/4 meg (stacksize is 1 meg by default) then don't use stack
        offsets = (unsigned int *) allocator->Alloc(count * sizeof(unsigned int));
        extents = (SourceExtent *) allocator->Alloc(count * sizeof(SourceExtent));
    } else {

        offsets = STACK_ALLOC(unsigned int, count);
        extents = STACK_ALLOC(SourceExtent, count);
    }

    // adjust back down...
    count--;

    int i = 0;
    unsigned prevEndOffset = 0;
    // traverse the sequence points.
    for (current = firstBB; current; current = current->next) {
        DEBUGINFO * debInfo = current->debugInfo;

        if (debInfo) {
            while(debInfo->prev) {
                ASSERT(debInfo->prev->next == debInfo);
                debInfo = debInfo->prev;
            }

            do {
                unsigned offset = (unsigned) debInfo->beginBlock->startOffset + debInfo->beginOffset;
                unsigned endoffset = (unsigned) debInfo->endBlock->startOffset + debInfo->endOffset;
                if (compileForEnc && !debInfo->alreadyAdjusted) {
                    if (debInfo->beginBlock->leaveTarget) {
                        offset += ILcodesSize[CEE_NOP];
                    }
                    if (debInfo->endBlock->leaveTarget && debInfo->endOffset != 0) {
                        endoffset += ILcodesSize[CEE_NOP];
                    }
                    debInfo->alreadyAdjusted = true;
                }
                ASSERT(endoffset >= offset);
                if (offset != endoffset && debInfo->extent.IsValid()) {

                    if (offset > 0 && i == 0) {
                        // we need to insert a record here for any prologue code
                        // we'll associate it w/ the opening {

                        SourceExtent extent = getSpecialPos(SpecialDebugPoint::OpenCurly);
                        if (extent.IsValidSource()) {
                            extents[0] = extent;
                            offsets[0] = 0;
                            i++;
                            count++;
                        }
                    }

                    // record IL offset.
                    offsets[i] = offset;
                    extents[i] = debInfo->extent;

                    // Dont actually emit this info if it is exactly the same as the previous one
                    if (i > 0 && ((extents[i-1] == extents[i] && extents[i-1].IsMergeAllowed() && extents[i].IsMergeAllowed()) || offsets[i] < prevEndOffset))
                    {
                        ASSERT(extents[i-1] == extents[i] && extents[i-1].IsMergeAllowed() && extents[i].IsMergeAllowed());
                        --count;
                    }
                    else
                    {
                        // advance to next entry.
                        ++i;
                    }
                } else {
                    // offset==endoffset -> no actual code emitted, so no sequence point.
                    // or begin == end -> no text to associate the code with, so no sequence point
                    --count;
                }
                prevEndOffset = endoffset;
                debInfo = debInfo->next;
            } while(debInfo);
        }
    }

    ASSERT(i == count);

    // And emit the sequence points.
    compiler()->emitter.EmitDebugBlock(method, count, offsets, extents);

    int cTemp = 0;
    // emit debug info for temporaries and deleted variables
    for (IlSlotInfo * pisi = (IlSlotInfo*)m_pbldrLocalSlotArray->Buffer(),
            * pisiLim = (IlSlotInfo*)(m_pbldrLocalSlotArray->Buffer() + m_pbldrLocalSlotArray->Length());
            pisi < pisiLim; pisi++)
    {
        WCHAR nameBuffer[MAX_IDENT_SIZE];
        if (!pisi->fIsUsed) {
            HRESULT hr; hr = StringCchPrintfW(nameBuffer, lengthof(nameBuffer), DELETED_NAME_PREFIX L"%04d", cTemp++);
            ASSERT (SUCCEEDED (hr));
        }
        else if (pisi->name == NULL) {
            HRESULT hr; hr = StringCchPrintfW(nameBuffer, lengthof(nameBuffer), TEMPORARY_NAME_PREFIX L"%d$%04d", pisi->tempKind, cTemp++);
            ASSERT (SUCCEEDED (hr));
        } 
        else {
            // Not a temporary or deleted local, so nothing to do
            continue;
        }

        compiler()->emitter.EmitDebugTemporary(pisi->type, nameBuffer, tkLocalVarSig, pisi->ilSlotNum);
    }


    if (localScope && localScope->tree) {
        // Emit information of local variables and their containing scopes.

        emitDebugScopesAndVars(localScope, codeSize, tkLocalVarSig);

    }

    compiler()->emitter.EmitDebugMethodInfoStop(method, codeSize);
}

void ILGENREC::emitDebugScopesAndVars(SCOPESYM * scope, unsigned codeSize, mdToken tkLocalVarSig)
{
    // Only called in debug path
    ASSERT(TrackDebugInfo());

    // Walk all locals declared in this scope. Emit scope information only if we're emitting
    // at least one variable. We can't do scopes and variables in a single loop because we need
    // to do an EmitDebugScopeStart (if one is needed) before doing any sub-scopes to get the
    // proper nesting of scopes.
    SCOPESYM * scopeOpened = NULL;
    unsigned beginOffset = 0;
    unsigned endOffset = codeSize;

    for (SYM * current = scope->firstChild; current; current = current->nextChild) {
        if (!current->isLOCVARSYM())
            continue;

        LOCVARSYM * loc = current->asLOCVARSYM();
        if (!loc->slot.type && !loc->isConst && !(loc->fIsIteratorLocal && loc->slot.IsUsed()))
            continue;

        // Open the current scope, if needed. If the current scope is the top-most scope
        // this isn't needed.
        if (!scopeOpened && scope != localScope) {
            // Opening a new scope.
            scopeOpened = scope;
            while (!scopeOpened->debugBlockStart && scopeOpened != localScope)
                scopeOpened = scopeOpened->parent->asSCOPESYM();

            if (scopeOpened == localScope)
                scopeOpened = NULL;  // don't reopen top-level scope.
            else {
                beginOffset = scopeOpened->debugBlockStart->startOffset;
                endOffset = scopeOpened->debugBlockEnd->startOffset;
                // This block/scope may have been wiped because it was unreachable
                // so be careful about emitting an offest past the end of the function
                if (scopeOpened->debugBlockStart->curLen < scopeOpened->debugOffsetStart)
                    beginOffset += (unsigned)scopeOpened->debugBlockStart->curLen;
                else
                    beginOffset += scopeOpened->debugOffsetStart;
                if (scopeOpened->debugBlockEnd->curLen < scopeOpened->debugOffsetEnd)
                    endOffset += (unsigned)scopeOpened->debugBlockEnd->curLen;
                else
                    endOffset += scopeOpened->debugOffsetEnd;

                ASSERT(beginOffset <= endOffset && endOffset <= codeSize);
                // Go ahead and emit nested variables even if the scope is zero length, we need all variables
                // recording in debug info for EnC.

                compiler()->emitter.EmitDebugScopeStart(beginOffset);
            }
        }

        if (loc->isConst) {
            // Emit the constant.
            compiler()->emitter.EmitDebugLocalConst(current->asLOCVARSYM());
        }
        else {
            // Emit the variable, along with the IL offsets it is valid for (from first use to end of scope).
            if (loc->debugBlockFirstUsed && !loc->fIsIteratorLocal) {
                compiler()->emitter.EmitDebugLocal(loc, tkLocalVarSig, loc->debugBlockFirstUsed->startOffset + loc->debugOffsetFirstUsed, endOffset);
            }
            else {
                // Unknown IL offsets: so use the whole scope
                compiler()->emitter.EmitDebugLocal(loc, tkLocalVarSig, beginOffset, endOffset);
            }
        }
    }

    // Walk all sub-scopes and emit them.
    for (SYM * current = scope->firstChild; current; current = current->nextChild) {
        if (current->isSCOPESYM()) {
            emitDebugScopesAndVars(current->asSCOPESYM(), codeSize, tkLocalVarSig);
        }
    }

    // If we opened a scope, close it.
    if (scopeOpened) {
        compiler()->emitter.EmitDebugScopeEnd(endOffset);
    }
}



bool ILGENREC::fitsInBucket(PSWITCHBUCKET bucket, unsigned __int64 key, unsigned newMembers)
{
    if (!bucket) return false;
    
    ASSERT((__int64)key > (__int64)bucket->lastMember);
    unsigned __int64 slots = key - bucket->firstMember;
    if (slots >= INT_MAX)
        return false;
    slots++;

    // Ensure > 50% table density
    return (bucket->members + newMembers) * 2 > slots;    
}

unsigned ILGENREC::mergeLastBucket(PSWITCHBUCKET * lastBucket)
{

    unsigned merged = 0;

    PSWITCHBUCKET currentBucket = *lastBucket;

AGAIN:
    PSWITCHBUCKET prevBucket = currentBucket->prevBucket;

    if (fitsInBucket(prevBucket, currentBucket->lastMember, currentBucket->members)) {
        *lastBucket = prevBucket;
        prevBucket->lastMember = currentBucket->lastMember;
        prevBucket->members += currentBucket->members;
        currentBucket = prevBucket;
        merged++;

        goto AGAIN;
    }

    return merged;
}


void ILGENREC::emitSwitchBucket(PSWITCHBUCKET bucket, PSLOT slot, BBLOCK * defBlock)
{
    // If this bucket holds 1 member only we dispense w/ the switch statement...
    if (bucket->members == 1) {
        // Use a simple compare...
        dumpLocal(slot, false);
        genExpr(bucket->labels[0]->key);
        curStack -= 2;
        startNewBB(NULL, CEE_BEQ, bucket->labels[0]->block = createNewBB());

        return;
    } 

    BBLOCK * guardBlock = emitSwitchBucketGuard(bucket->labels[bucket->members-1]->key, slot, false);
    if (guardBlock) {
        dumpLocal(slot, false);
        genExpr(bucket->labels[0]->key);
        curStack -= 2;

        ASSERT(bucket->labels[0]->key->type->fundType() != FT_U8);
        startNewBB(NULL, bucket->labels[0]->key->type->isUnsigned() ? CEE_BLT_UN : CEE_BLT, defBlock);
    }

    unsigned __int64 expectedKey = bucket->labels[0]->key->asCONSTANT()->getI64Value();

    dumpLocal(slot, false);
    // Ok, we now need to normalize the key to 0
    if (expectedKey != 0) {
        genExpr(bucket->labels[0]->key);
        putOpcode(CEE_SUB);
    }
    if (guardBlock) {
        putOpcode(CEE_CONV_I4);
    }
    curStack--;

    // Now, lets construct the target blocks...

    SWITCHDEST * switchDest = (SWITCHDEST*) allocator->Alloc(sizeof(SWITCHDEST) + sizeof(SWITCHDESTGOTO) * (1 + (int)(bucket->lastMember - bucket->firstMember)));

    unsigned slotNum = 0;
    for (unsigned i = 0; i < bucket->members; i++) {
AGAIN:
        switchDest->blocks[slotNum].jumpIntoTry = false;
        if (expectedKey == (unsigned __int64) bucket->labels[i]->key->asCONSTANT()->getI64Value()) {
            switchDest->blocks[slotNum++].dest = bucket->labels[i]->block = createNewBB();
            expectedKey++;
        } else {
            switchDest->blocks[slotNum++].dest = defBlock;
            expectedKey++;
            goto AGAIN;
        }
    }
    switchDest->count = slotNum;

    ASSERT(expectedKey == bucket->lastMember + 1);
    ASSERT(slotNum == 1 + (bucket->lastMember - bucket->firstMember));

    startNewBB(guardBlock, CEE_SWITCH, (BBLOCK*) switchDest);
    
}

BBLOCK * ILGENREC::emitSwitchBucketGuard(EXPR * key, PSLOT slot, bool force)
{
    
    FUNDTYPE ft;

    if (!force && (ft = key->type->underlyingType()->fundType()) != FT_I8 && ft != FT_U8) return NULL;

    dumpLocal(slot, false);
    genExpr(key);
    curStack -= 2;

    BBLOCK * rval;
    ASSERT(key->type->fundType() != FT_U8);
    startNewBB(NULL, key->type->isUnsigned() ? CEE_BGT_UN : CEE_BGT, rval = createNewBB());

    return rval;   
}

void ILGENREC::emitSwitchBuckets(PSWITCHBUCKET * array, unsigned first, unsigned last, PSLOT slot, BBLOCK * defBlock)
{
    if (first == last) {
        emitSwitchBucket(array[first], slot, defBlock);

        return;
    }
    unsigned mid = (last + first + 1) / 2;
    // This way (0 1 2 3) will produce a mid of 2 while
    // (0 1 2) will produce a mid of 1

    // Now, the first half is first to mid-1
    // and the second half is mid to last.

    // If the first half consists of only one bucket, then we will automatically fall into
    // the second half if we fail that switch.  Otherwise, however, we need to check 
    // ourselves which half we belong to:
    if (first != mid - 1) {

        EXPRSWITCHLABEL * lastLabel = array[mid-1]->labels[array[mid-1]->members - 1];
        EXPR * lastKey = lastLabel->key;

        BBLOCK * secondHalf = emitSwitchBucketGuard(lastKey, slot, true);
    
        emitSwitchBuckets(array, first, mid - 1, slot, defBlock);

        startNewBB(secondHalf, CEE_BR, defBlock);
    
    } else {
        emitSwitchBucket (array[first], slot, defBlock);
    }

    emitSwitchBuckets(array, mid, last, slot, defBlock);   

    startNewBB(NULL, CEE_BR, defBlock);
    
}


void ILGENREC::genStringSwitch(EXPRSWITCH * tree)
{
    ASSERT(tree->arg->type->isPredefType(PT_STRING));

    int labelCount = tree->labelCount;
    if (tree->flags & EXF_HASDEFAULT)
        labelCount--;
    ASSERT(labelCount >= 0);

    EXPRSWITCHLABEL ** start = tree->labels;
    EXPRSWITCHLABEL ** end = start + labelCount;
    BBLOCK * defBlock = createNewBB();
    BBLOCK * nullBlock;
    if (tree->nullLabel) {
        tree->nullLabel->block = nullBlock = createNewBB();
    } else {
        nullBlock = defBlock;
    }

    if (tree->arg->kind != EK_CONSTANT) {

        if (start != end) {

            if (tree->flags & EXF_HASHTABLESWITCH) {
                ASSERT(!compiler()->FEncBuild());

                SWITCHDEST * switchDest = (SWITCHDEST*) allocator->Alloc(sizeof(SWITCHDEST) + sizeof(SWITCHDESTGOTO) * labelCount);

                switchDest->count = labelCount;


                genExpr(tree->arg);

                PSLOT slot = NULL;
                if (compileForEnc) {
                    slot = DumpToDurable(compiler()->GetReqPredefType(PT_STRING), false);
                } else {
                    putOpcode(CEE_DUP);
                    slot = STOREINTEMP(compiler()->GetReqPredefType(PT_STRING), TK_SHORTLIVED);
                }
                PSLOT slotInt = allocTemporary(compiler()->GetReqPredefType(PT_INT), TK_SHORTLIVED);

                startNewBB(NULL, CEE_BRFALSE, nullBlock);
                curStack--;
                
                genHashtableStringSwitchInit(tree);
                putOpcode(CEE_VOLATILE);
                putOpcode(CEE_LDSFLD);
                putDWORD(tree->hashtableToken);

                dumpLocal(slot, false);
                genSlotAddress(slotInt);
                putOpcode(CEE_CALL);
                emitMethodToken(compiler()->getBSymmgr().methDictionaryTryGetValue, compiler()->getBSymmgr().atsDictionary);
                curStack -= 2;
                startNewBB(NULL, CEE_BRFALSE, defBlock);
                curStack--;
                dumpLocal(slotInt, false);
                freeTemporary(slot);
                freeTemporary(slotInt);
                curStack--;
                int count = 0;
                while (start < end) {
                    if (start[0]->key->asCONSTANT()->isNull()) {
                        switchDest->count --;
                    } else {
                        switchDest->blocks[count].jumpIntoTry = false;
                        ASSERT(!start[0]->block);
                        switchDest->blocks[count++].dest = start[0]->block = createNewBB();
                    }
                    start ++;
                }
                startNewBB(NULL, CEE_SWITCH, (BBLOCK*) switchDest);

            } else {

                genExpr(tree->arg);
                PSLOT slot = NULL;
                if (compileForEnc) {
                    slot = DumpToDurable(compiler()->GetReqPredefType(PT_STRING), false);
                } else {
                    putOpcode(CEE_DUP);
                    slot = STOREINTEMP(compiler()->GetReqPredefType(PT_STRING), TK_SHORTLIVED);
                }
                startNewBB(NULL, CEE_BRFALSE, nullBlock);
                curStack--;

                while (start < end) {
                    if (!start[0]->key->asCONSTANT()->isNull()) {
                        dumpLocal(slot, false);
                        genString(start[0]->key->asCONSTANT()->getSVal());
                        putOpcode(CEE_CALL);
                        emitMethodToken(compiler()->getBSymmgr().methStringEquals);
                        startNewBB(NULL, CEE_BRTRUE, start[0]->block = createNewBB());
                        curStack -= 2;
                    }
                    start++;
                }
                freeTemporary(slot);
            }

        } else {
            genSideEffects(tree->arg);
        }

        if (tree->flags & EXF_HASDEFAULT) {
            startNewBB(NULL, CEE_BR, (*end)->block = defBlock);
        } else {
            startNewBB(NULL, CEE_BR, tree->breakLabel->block = defBlock);
        }
    }

    closeDebugInfo();

    if (tree->bodies) {
        GenStmtChain(tree->bodies);
    }
}


void ILGENREC::genSwitch(EXPRSWITCH *tree)
{
    ASSERT(!tree->FReachable() == (tree->arg->kind == EK_CONSTANT));

    if (tree->arg->type->isPredefType(PT_STRING)) {
        genStringSwitch(tree);
        return;
    }

    PSWITCHBUCKET lastBucket = NULL;
    unsigned bucketCount = 0;

    int labelCount = tree->labelCount;
    EXPRSWITCHLABEL * exprDefault = NULL;

    if (tree->flags & EXF_HASDEFAULT) {
        ASSERT(labelCount > 0);
        exprDefault = tree->labels[--labelCount];
    }
    if (tree->nullLabel) {
        ASSERT(labelCount > 0);
        ASSERT(tree->arg->type->isNUBSYM() && tree->nullLabel == tree->labels[labelCount - 1]);
        labelCount--;
    }

    SourceExtent extentSwitchOnly;
    extentSwitchOnly.SetInvalid();
    BBLOCK * defBlock = createNewBB();

    if (tree->arg->kind != EK_CONSTANT) {
        EXPRSWITCHLABEL ** pexprLim = tree->labels + labelCount;
        for (EXPRSWITCHLABEL ** pexprCur = tree->labels; pexprCur < pexprLim; pexprCur++) {
            ASSERT(*pexprCur && *pexprCur != tree->nullLabel && *pexprCur != exprDefault);

            // First, see if we fit in the last bucket, or if we need to start a new one...
            unsigned __int64 key = (*pexprCur)->key->asCONSTANT()->getI64Value();
            if (fitsInBucket(lastBucket, key, 1)) {
                lastBucket->lastMember = key;
                lastBucket->members ++;
                bucketCount -= mergeLastBucket(&lastBucket);
            } else {
                // create a new bucket...
                PSWITCHBUCKET newBucket = STACK_ALLOC(SWITCHBUCKET, 1);
                newBucket->firstMember = key;
                newBucket->lastMember = key;
                newBucket->labels = pexprCur;
                newBucket->members = 1;
                newBucket->prevBucket = lastBucket;
                lastBucket = newBucket;
                bucketCount++;
            }
        }

        // Ok, now to copy all this into an array so that we can do a binary traversal on it:

        PSWITCHBUCKET * bucketArray = STACK_ALLOC(PSWITCHBUCKET, bucketCount);

        for (int i = (int) bucketCount - 1; i >= 0; i--) {
            ASSERT(lastBucket);
            bucketArray[i] = lastBucket;
            lastBucket = lastBucket->prevBucket;
        }
        ASSERT(!lastBucket);

        if (bucketCount || tree->nullLabel) {
            PSLOT slot;

            // Deal with null.
            if (tree->arg->type->isNUBSYM()) {
                TYPESYM * typeTmp = tree->arg->type->StripNubs();
                AGGTYPESYM * ats = tree->arg->type->asNUBSYM()->GetAts();
                PSLOT slotNub = NULL;

                GenDupSafeMemAddr(tree->arg, &slotNub);
                putOpcode(CEE_DUP);
                putOpcode(CEE_CALL);
                emitMethodToken(compiler()->getBSymmgr().methNubGetValOrDef, ats);

                if (compileForEnc)
                    slot = DumpToDurable(typeTmp, false, false);
                else
                    slot = STOREINTEMP(typeTmp, TK_SHORTLIVED);

                putOpcode(CEE_CALL);
                emitMethodToken(compiler()->getBSymmgr().propNubHasValue->methGet, ats);
                if (slotNub)
                    freeTemporary(slotNub);

                BBLOCK * nullBlock;
                if (tree->nullLabel)
                    tree->nullLabel->block = nullBlock = createNewBB();
                else
                    nullBlock = defBlock;

                startNewBB(NULL, CEE_BRFALSE, nullBlock);
                curStack--;
            }
            else {
                TYPESYM * typeTmp = tree->arg->type;

                genExpr(tree->arg);
                ASSERT(!tree->nullLabel);
                if (compileForEnc)
                    slot = DumpToDurable(typeTmp, false, false);
                else 
                    slot = STOREINTEMP(typeTmp, TK_SHORTLIVED);
            }

            // Save the ending line/col so we don't include the case labels
            // when we emit the buckets
            if (curDebugInfo)
                extentSwitchOnly = curDebugInfo->extent;

            if (bucketCount)
                emitSwitchBuckets(bucketArray, 0, bucketCount - 1, slot, defBlock);

            freeTemporary(slot);
        } else {
            genSideEffects(tree->arg);
        }

        if (tree->flags & EXF_HASDEFAULT) {
            ASSERT(exprDefault);
            startNewBB(NULL, CEE_BR, exprDefault->block = defBlock);
        } else {
            startNewBB(NULL, CEE_BR, tree->breakLabel->block = defBlock);
        }
    }

    if (curDebugInfo) {
        if (extentSwitchOnly.IsValid()) {
            // Restore the extent info (overwritting whatever extra extents were set by emitSwitchBuckets)
            curDebugInfo->extent = extentSwitchOnly;
        }
        closeDebugInfo();
    }

    if (tree->bodies) {
        GenStmtChain(tree->bodies);
    }
}


// generate code for a label, this merely involves starting the right bblock.
void ILGENREC::genLabel(EXPRLABEL * tree)
{
    if (tree->block) {
        // if we got a block for this, then just make this the current block...
        startNewBB(tree->block);
    } else {
        startNewBB(NULL);
        tree->block = currentBB;
    }
}

// generate code for a goto...
void ILGENREC::genGoto(EXPRGOTO * tree)
{
    ASSERT(!(tree->flags & EXF_UNREALIZEDGOTO));

    BBLOCK * dest = tree->label->block;

    if (!dest) {
        dest = tree->label->block = createNewBB(false);
    }
    if (tree->flags & EXF_ASLEAVE) {
        if (tree->flags & EXF_FINALLYBLOCKED) {
            inlineBB.gotoBlocked = true;
            blockedLeave++;
        }
        startNewBB(NULL, CEE_LEAVE, dest);
    } else {
        startNewBB(NULL, CEE_BR, dest);
    }
}


// generate code for a gotoif...
void ILGENREC::genGotoIf(EXPRGOTOIF * tree)
{
    // this had to be taken care of by the def-use pass...
    ASSERT(!(tree->flags & EXF_UNREALIZEDGOTO));

    BBLOCK * dest = tree->label->block;
    
    if (!dest) {
        dest = tree->label->block = createNewBB();
    }

    if (compileForEnc) {
        EXPR::CONSTRESKIND crk;
        genCondExpr(tree->condition, tree->sense, &crk);
        bool dumpToStack = crk == EXPR::ConstNotConst;
        DumpToDurable(compiler()->GetReqPredefType(PT_BOOL), true, dumpToStack);
        ILCODE il;

        switch (crk) {
            case EXPR::ConstFalse:
                dest = NULL;
                il = cee_next;
                break;
            case EXPR::ConstTrue:
                il = CEE_BR;
                break;
            default:
                ASSERT(0);
            case EXPR::ConstNotConst:
                il = CEE_BRTRUE;
        }
        startNewBB(NULL, il, dest);
        if (il != cee_next) {
            curStack += ILStackOps[il];
            markStackMax();
        }
    } else {

        genCondBranch(tree->condition, dest, tree->sense);
    }
}

// generate a conditional (ie, boolean) expression...
// this will leave a value on the stack which conforms to sense, ie:
// (condition == sense)
void ILGENREC::genCondExpr(EXPR * condition, bool sense, EXPR::CONSTRESKIND * crk)
{

    if (crk) {
        *crk = EXPR::ConstNotConst;
    }

LRecurse:

#if DEBUG
    int prevStack = curStack;
    if (condition->flags & EXF_BINOP && condition->asBIN()->p1->kind == EK_WRAP)
        prevStack--;
#endif

    switch (condition->kind) {
    case EK_LOGAND:
        // remember that a == false is ~a, and so ~(a && b) is done as (~a || ~b)...
        if (!sense)
            goto LLogOr;

        {
LLogAnd:
            // we generate:
            // gotoif (a != sense) lab0
            // b == sense
            // goto labEnd
            // lab0
            // 0
            // labEnd
            BBLOCK * fallThrough = genCondBranch(condition->asBIN()->p1, createNewBB(), !sense);
            genCondExpr(condition->asBIN()->p2, sense);
            ASSERT(prevStack + 1 == curStack);
            curStack--;
            BBLOCK * labEnd = createNewBB(false);
            startNewBB(fallThrough, CEE_BR, labEnd);
            genIntConstant(0);
            startNewBB(labEnd);
        }
        break;

    case EK_LOGOR:
        // as above, ~(a || b) is (~a && ~b)
        if (!sense)
            goto LLogAnd;

        {
LLogOr:
            // we generate:
            // gotoif (a == sense) lab1
            // b == sense
            // goto labEnd
            // lab1
            // 1
            // labEnd
            BBLOCK * ldOne = genCondBranch(condition->asBIN()->p1, createNewBB(), sense);
            genCondExpr(condition->asBIN()->p2, sense);
            ASSERT(prevStack + 1 == curStack);
            curStack--;
            BBLOCK * fallThrough = createNewBB(false);
            startNewBB(ldOne, CEE_BR, fallThrough);
            genIntConstant(1);
            startNewBB(fallThrough);
        }
        break;

    case EK_CONSTANT:
        // Make sure that at most the low bit is set:
        ASSERT((condition->asCONSTANT()->getVal().iVal & ~1) == 0);
        ASSERT(((int)sense & ~1) == 0);
        int constant;
        constant = (((condition->asCONSTANT()->getVal().iVal &  1) == (sense & 1)) & 1);
        if (crk) {
            *crk = constant ? EXPR::ConstTrue : EXPR::ConstFalse;
        }
        genIntConstant(constant);
        break;

    case EK_LT:
    case EK_LE:
    case EK_GT:
    case EK_GE:
    case EK_EQ:
    case EK_NE:
        genExpr(condition->asBIN()->p1);
        genExpr(condition->asBIN()->p2);

        putOpcode(CodeForCompare(condition->kind, condition->asBIN()->p1->type, &sense));

        if (!sense) {
LNegate:
            putOpcode(CEE_LDC_I4_0);
            putOpcode(CEE_CEQ);
        }
        break;

    case EK_LOGNOT:
        condition = condition->asBIN()->p1;
        sense = !sense;
        goto LRecurse;

    default:
        genExpr(condition);
        if (crk) {
            *crk = condition->GetConstantResult();
            if (!sense && *crk != EXPR::ConstNotConst) {
                *crk = (*crk == EXPR::ConstTrue) ? EXPR::ConstFalse : EXPR::ConstTrue;
            }
        }
        if (!sense)
            goto LNegate;
        break;
    }
}


bool ILGENREC::isSimpleExpr(EXPR * condition, bool * sense)
{
    EXPR * op1 = condition->asBIN()->p1;
    EXPR * op2 = condition->asBIN()->p2;

    bool c1 = op1->kind == EK_CONSTANT;
    bool c2 = op2->kind == EK_CONSTANT;

    if (!c1 && !c2) return false;

    EXPR * constOp;
    EXPR * nonConstOp;

    if (c1) {
        constOp = op1;
        nonConstOp = op2;
    } else {
        constOp = op2;
        nonConstOp = op1;
    }

    FUNDTYPE ft = nonConstOp->type->fundType();
    if (ft == FT_NONE || (ft >= FT_I8 && ft <= FT_R8)) return false;

    bool isBool = nonConstOp->type->isPredefType(PT_BOOL);
    bool isZero = (ft == FT_I8 || ft == FT_U8) ? constOp->asCONSTANT()->getI64Value() == 0 : constOp->asCONSTANT()->getVal().iVal == 0;

    // bool is special, only it can be compared to true and false...
    if (!isBool && !isZero) {
        return false;
    }

    // if comparing to zero, flip the sense
    if (isZero) {
        *sense = !(*sense);
    }

    // if comparing != flip the sense
    if (condition->kind == EK_NE) {
        *sense = !(*sense);
    }

    genExpr(nonConstOp);

    return true;


}


// generate a jump to dest if condition == sense is true
BBLOCK * ILGENREC::genCondBranch(EXPR * condition, BBLOCK * dest, bool sense)
{
    ASSERT(dest);

    ILCODE ilcode = CEE_NOP;
    ILCODE ilcodeRev;
    EXPR::CONSTRESKIND crk;

AGAIN:

    ilcodeRev = CEE_ILLEGAL; // Default.

    switch(condition->kind) {
    case EK_LOGAND: {
        // see comment in genCondExpr
        if (!sense) goto LOGOR;
LOGAND:
        // we generate:
        // gotoif(a != sense) labFT
        // gotoif(b == sense) labDest
        // labFT
        BBLOCK * fallThrough = genCondBranch(condition->asBIN()->p1, createNewBB(), !sense);
        genCondBranch(condition->asBIN()->p2, dest, sense);
        startNewBB(fallThrough);
        return dest;
        }
    case EK_LOGOR:
        if (!sense) goto LOGAND;
LOGOR:
        // we generate:
        // gotoif(a == sense) labDest
        // gotoif(b == sense) labDest
        genCondBranch(condition->asBIN()->p1, dest, sense);
        condition = condition->asBIN()->p2;
        goto AGAIN;

    case EK_CONSTANT:
        // make sure that only the bool bits are set:
        ASSERT(condition->asCONSTANT()->getVal().iVal == 0 || condition->asCONSTANT()->getVal().iVal == 1);

        if (condition->asCONSTANT()->getVal().iVal == (int)sense) {
            ilcode = CEE_BR;
            break;
        } // otherwise this branch will never be taken, so just fall through...
        return dest;

    case EK_EQ:
    case EK_NE:
        if (isSimpleExpr(condition, &sense))
            goto SIMPLEBR;
        // Fall through
    case EK_LT:
    case EK_LE:
    case EK_GT:
    case EK_GE:
        genExpr(condition->asBIN()->p1);
        genExpr(condition->asBIN()->p2);

        ilcode = CodeForJump(condition->kind, condition->asBIN()->p1->type, sense, &ilcodeRev);
        break;

    case EK_LOGNOT:
        sense = !sense;
        condition = condition->asBIN()->p1;
        goto AGAIN;

    case EK_IS:
    case EK_AS:
        if (condition->asBIN()->p2->kind == EK_TYPEOF) {
            genExpr(condition->asBIN()->p1);
            putOpcode(CEE_ISINST);
            emitTypeToken(condition->asBIN()->p2->asTYPEOF()->sourceType);
            goto SIMPLEBR;
        }
        break;

    case EK_SEQUENCE:
        genSideEffects(condition->asBIN()->p1);
        condition = condition->asBIN()->p2;
        goto AGAIN;

    case EK_SEQREV:
        if (!condition->asBIN()->p2->hasSideEffects(compiler())) {
            condition = condition->asBIN()->p1;
            goto AGAIN;
        }
        goto DEFAULT;

    default:
DEFAULT:
        if ((crk = condition->GetConstantResult()) != EXPR::ConstNotConst) {
            genExpr(condition, false);
            if (EXPR::ConstantMatchesSense(crk, sense)) {
                ilcode = CEE_BR;
                break;
            } else {
                return dest;
            }
        }
        genExpr(condition);
SIMPLEBR:
        ilcode = sense ? CEE_BRTRUE : CEE_BRFALSE;
        break;
    }

    ASSERT(ilcodeRev == CEE_ILLEGAL || ILStackOps[ilcodeRev] == ILStackOps[ilcode]);
    startNewBB(NULL, ilcode, dest, ilcodeRev);

    // since we are not emitting the instruction, but rather are saving it in the bblock,
    // we need to manipulate the stack ourselves...
    curStack += ILStackOps[ilcode];
    markStackMax();

    return dest;
}



// generate code for a return statement
void ILGENREC::genReturn(EXPRRETURN * tree)
{
    if (tree->object) {
        genExpr(tree->object);
    }

    if (tree->flags & EXF_ASLEAVE || !compiler()->options.m_fOPTIMIZATIONS) {
        if (!returnLocation) {
            returnLocation = createNewBB();
            returnHandled = false;
        }
        if (retTemp) {
            dumpLocal(retTemp, true);
        } else if (tree->object) {
            // this could only happen if this is unreachable code...
            curStack--;
        }
        if (tree->flags & EXF_FINALLYBLOCKED) {
            inlineBB.gotoBlocked = true;
            blockedLeave++;
        }
        if (tree->flags & EXF_ASLEAVE) {
            startNewBB(NULL, CEE_LEAVE, returnLocation);
        } else {
            startNewBB(NULL, CEE_BR, returnLocation);
        }
    } else {

        handleReturn();
        startNewBB(NULL, CEE_RET);
    }
}


// generate a string constant
void ILGENREC::genString(CONSTVAL string)
{
    putOpcode(CEE_LDSTR);
    ULONG rva = compiler()->emitter.GetStringRef(string.strVal);
    putDWORD(rva);
}


void ILGENREC::genSwap(EXPRBINOP * tree, bool valUsed)
{

    if (!valUsed) {
        genSideEffects(tree);
        return;
    }

    genExpr(tree->p1);
    PSLOT slot = STOREINTEMP(tree->p2->type, TK_SHORTLIVED);
    genExpr(tree->p2);
    dumpLocal(slot, false);
    freeTemporary(slot);
}

// generate "is" or "as", depending on isAs.
void ILGENREC::genIs(EXPRBINOP * tree, bool valUsed, bool isAs)
{

    if (!valUsed) {
        genSideEffects(tree);
        return;
    }
    
    EXPR * e1 = tree->p1;
    EXPR * e2 = tree->p2;

    ASSERT(e2->kind == EK_TYPEOF);


    genExpr(e1);
    putOpcode(CEE_ISINST);
    emitTypeToken(e2->asTYPEOF()->sourceType);
    if (!isAs) {
        putOpcode(CEE_LDNULL);
        putOpcode(CEE_CGT_UN);
    } else if (tree->type->isNUBSYM()) {
        putOpcode(CEE_UNBOX_ANY);
        emitTypeToken(tree->type);
    }
}

void ILGENREC::genPtrAddr(EXPR * op, bool fNoConv, bool valUsed)
{
    // strings are special...
    if (op->type->isPINNEDSYM() && op->type->asPINNEDSYM()->baseType()->isPredefType(PT_STRING)) {
        genExpr(op);
        if (!valUsed) {
            putOpcode(CEE_POP);
            return;
        }
        putOpcode(CEE_CONV_I);
        putOpcode(CEE_DUP);
        BBLOCK * block = createNewBB();
        startNewBB(NULL, CEE_BRFALSE, block);
        curStack--;
        putOpcode(CEE_CALL);
        emitMethodToken(compiler()->getBSymmgr().methStringOffset);
        curStack++;
        markStackMax();
        putOpcode(CEE_ADD);
        startNewBB(block);
    } else {
        genMemoryAddress(op, NULL, true);
        if (!valUsed)
            putOpcode(CEE_POP);
        else if (!fNoConv)
            putOpcode(CEE_CONV_U);
    }
}


void ILGENREC::genTypeRefAny(EXPRBINOP * tree, bool valUsed)
{
    if (!valUsed) {
        genSideEffects(tree->p1);
        return;
    }

    genExpr(tree->p1);
    putOpcode(CEE_REFANYTYPE);
    putOpcode(CEE_CALL);
    emitMethodToken(tree->p2->asTYPEOF()->method);
}

void ILGENREC::genMakeRefAny(EXPRBINOP * tree, bool valUsed)
{
    if (!valUsed) {
        genSideEffects(tree->p1);
        return;
    }
    
    genMemoryAddress(tree->p1, NULL);
    putOpcode(CEE_MKREFANY);
    emitTypeToken(tree->p1->type);
}

// generate a generic expression
void ILGENREC::genExpr(EXPR * tree, bool valUsed)
{
AGAIN:

    if (!tree) {
        return;
    }

    switch (tree->kind) {
    case EK_CONCAT:
        if (tree->flags & EXF_UNREALIZEDCONCAT) {
            if (!valUsed)
                return;
            // This occurs in unreachable code, so just gen the first operand.
            tree = tree->asCONCAT()->list;
            if (tree->kind == EK_LIST)
                tree = tree->asBIN()->p1;
        }
        else
            tree = tree->asCONCAT()->list;
        goto AGAIN;

    case EK_WRAP:
        if (tree->asWRAP()->slot) {
            dumpLocal(tree->asWRAP()->slot, false);
            if (!tree->asWRAP()->doNotFree) {
                freeTemporary(tree->asWRAP()->slot);
            }
        } else if (tree->asWRAP()->expr && tree->asWRAP()->expr->kind == EK_WRAP) {
            if (tree->asWRAP()->expr->asWRAP()->slot && tree->asWRAP()->expr->asWRAP()->doNotFree) {
                FREETEMP(tree->asWRAP()->expr->asWRAP()->slot);
            }
        }
        return;

    case EK_USERLOGOP: {
        genExpr(tree->asUSERLOGOP()->opX);
        putOpcode(CEE_DUP);
        genExpr(tree->asUSERLOGOP()->callTF);
        BBLOCK * target = createNewBB();
        startNewBB(NULL, CEE_BRTRUE, target);
        curStack--;
        genExpr(tree->asUSERLOGOP()->callOp);
        startNewBB(target);
        break;
        }
    case EK_DBLQMARK: {
        EXPR * exprTest = tree->asDBLQMARK()->exprTest;

        genExpr(exprTest);
        putOpcode(CEE_DUP);
        if (exprTest->type->isTYVARSYM()) {
            putOpcode(CEE_BOX);
            emitTypeToken(exprTest->type);
        }

        BBLOCK * bbConv = createNewBB();
        BBLOCK * bbDone = createNewBB();

        startNewBB(NULL, CEE_BRTRUE, bbConv);
        curStack--;
        putOpcode(CEE_POP);
        genExpr(tree->asDBLQMARK()->exprElse);

        BBLOCK * bbElse = currentBB;
        startNewBB(bbConv, CEE_BR, bbDone);
        genExpr(tree->asDBLQMARK()->exprConv);

        if (inlineBB.code == reusableBuffer && currentBB == bbConv) {
            // No code emitted for the conversion so we don't need bbDone.
            ASSERT(bbElse->exitIL == CEE_BR);
            ASSERT(bbElse->jumpDest == bbDone);
            bbElse->exitIL = cee_next;
            bbElse->jumpDest = NULL;
        }
        else
            startNewBB(bbDone);
        break;
        }
    case EK_SAVE:
        ASSERT(valUsed);
        ASSERT(tree->asBIN()->p2->kind == EK_WRAP || tree->asBIN()->p2->kind == EK_FIELD);
        if (tree->asBIN()->p2->kind == EK_FIELD) {
            AddrInfo addr;
            int cvalStack = genAddr(tree->asBIN()->p2, addr);
            ASSERT(cvalStack >= 0);

            genExpr(tree->asBIN()->p1);
            PSLOT slot = NULL;
            putOpcode(CEE_DUP);
            if (cvalStack)
                slot = STOREINTEMP(tree->asBIN()->p1->type, TK_SHORTLIVED);
            genStore(tree->asBIN()->p2, addr);
            if (slot) {
                dumpLocal(slot, false);
                freeTemporary(slot);
            }
        }
        else {
            genExpr(tree->asBIN()->p1);
            putOpcode(CEE_DUP);
            tree->asBIN()->p2->asWRAP()->slot = STOREINTEMP(tree->asBIN()->p1->type, tree->asBIN()->p2->asWRAP()->tempKind);
        }
        return;
    case EK_LOCALLOC:
        genExpr(tree->asBIN()->p1);
        putOpcode(CEE_LOCALLOC);
        break;
    case EK_QMARK:
        genQMark(tree->asBIN(), valUsed);
        return;
    case EK_SWAP:
        genSwap(tree->asBIN(), valUsed);
        return;
    case EK_IS:
        genIs(tree->asBIN(), valUsed, false);
        return;
    case EK_AS:
        genIs(tree->asBIN(), valUsed, true);
        return;
    case EK_CAST:
        genCast(tree->asCAST(), valUsed);
        return;
    case EK_MAKERA:
        genMakeRefAny(tree->asBIN(), valUsed);
        return;
    case EK_TYPERA:
        genTypeRefAny(tree->asBIN(), valUsed);
        return;
    case EK_FUNCPTR:
        if (tree->asFUNCPTR()->mwi.Meth()->isVirtual && !tree->asFUNCPTR()->mwi.Meth()->getClass()->isSealed &&
            tree->asFUNCPTR()->object->type->fundType() == FT_REF && !(tree->flags & EXF_BASECALL))
        {
            putOpcode(CEE_DUP);
            putOpcode(CEE_LDVIRTFTN);
        }
        else
            putOpcode(CEE_LDFTN);
#if USAGEHACK
        tree->asFUNCPTR()->mwi.Meth()->isUsed = true;
#endif
        emitMethodToken(tree->asFUNCPTR()->mwi.Meth(), tree->asFUNCPTR()->mwi.Type(), tree->asFUNCPTR()->mwi.TypeArgs());
        break;
    case EK_TYPEOF:
        if (valUsed) {
            putOpcode(CEE_LDTOKEN);
            if (tree->flags & EXF_OPENTYPE) {
                TYPESYM * type = tree->asTYPEOF()->sourceType;
                ASSERT(type->isAGGTYPESYM() && type->asAGGTYPESYM()->typeArgsAll->size > 0 &&
                    type->asAGGTYPESYM()->typeArgsAll->Item(type->asAGGTYPESYM()->typeArgsAll->size - 1)->isUNITSYM());
                mdToken tok = compiler()->emitter.GetAggRef(type->getAggregate(), false);
#if DEBUG
                inlineBB.sym = type->getAggregate();
#endif
                putDWORD(tok);
            }
            else
                emitTypeToken(tree->asTYPEOF()->sourceType);
            putOpcode(CEE_CALL);
            emitMethodToken(tree->asTYPEOF()->method);
        }
        return;
    case EK_SIZEOF:
        if (valUsed) {
            genSizeOf(tree->asSIZEOF()->sourceType);
        }
        return;
    case EK_ARRINIT:
        genArrayInit(tree->asARRINIT(), valUsed);
        return;
    case EK_CONSTANT:
        if (!valUsed) return;
        switch(tree->type->fundType ()) {
        case FT_I1:
        case FT_U1:
        case FT_I2:
        case FT_U2:
        case FT_I4:
        case FT_U4:
            genIntConstant(tree->asCONSTANT()->getVal().iVal); // OK for U4, since IL treats them the same.
            break;

        case FT_R4:
            // REVIER_CONSIDER: should we use genDoubleConstant ever for R4s if the constant
            // has greater precision than an R4. We do constant fold to greater precision
            // so it might be available.
            genFloatConstant((float) * tree->asCONSTANT()->getVal().doubleVal);
            break;
        case FT_R8:
            genDoubleConstant(tree->asCONSTANT()->getVal().doubleVal);
            break;
        case FT_I8:
        case FT_U8:
            genLongConstant(tree->asCONSTANT()->getVal().longVal);
            break;
        case FT_REF:
            if (tree->asCONSTANT()->getSVal().strVal) {
                // this must be a string...
                genString(tree->asCONSTANT()->getSVal());
            } else {
                putOpcode(CEE_LDNULL);
            }
            break;

        case FT_STRUCT:
            ASSERT(tree->type->isPredefType(PT_DECIMAL));
            genDecimalConstant(tree->asCONSTANT()->getVal().decVal);
            break;

        default:
            ASSERT(!"bad constant type");
        }
        break;
    case EK_CALL:
        genCall(tree->asCALL(), valUsed);
        return;
    case EK_NOOP:
        return;
    case EK_DELIM:
        return;
    case EK_MULTI:
        GenMultiOp(tree->asMULTI(), valUsed);
        return;
    case EK_MULTIGET:
        GenMultiGet(tree->asMULTIGET()->multi, valUsed);
        return;

    // Temp management.
    case EK_STTMP:
        ASSERT(!tree->asSTTMP()->slot);
        genExpr(tree->asSTTMP()->src, true);
        if (valUsed)
            putOpcode(CEE_DUP);
        tree->asSTTMP()->slot = STOREINTEMP(tree->asSTTMP()->src->type, TK_SHORTLIVED);
        return;
    case EK_LDTMP:
        ASSERT(tree->asLDTMP()->tmp->slot);
        if (valUsed)
            dumpLocal(tree->asLDTMP()->tmp->slot, false);
        return;
    case EK_FREETMP:
        ASSERT(!valUsed);
        ASSERT(tree->asFREETMP()->tmp->slot);
        FREETEMP(tree->asFREETMP()->tmp->slot);
        return;

    case EK_LIST:
        // this kind of loop takes less code if we know that the elements
        // are also expressions
        genExpr(tree->asBIN()->p1, valUsed);
        tree = tree->asBIN()->p2;
        goto AGAIN;
    case EK_NEWARRAY:
        genNewArray(tree->asBIN());
        break;
    case EK_SEQUENCE:
        genSideEffects(tree->asBIN()->p1);
        tree = tree->asBIN()->p2;
        goto AGAIN;
    case EK_SEQREV:
        if (valUsed)
            genExpr(tree->asBIN()->p1);
        else
            genSideEffects(tree->asBIN()->p1);
        genSideEffects(tree->asBIN()->p2);
        return;
    case EK_ARGS:
        putOpcode(CEE_ARGLIST);
        break;
    case EK_LOCAL:
        if (!tree->asLOCAL()->local->slot.isRefParam) {
            dumpLocal(&(tree->asLOCAL()->local->slot), false);
            break;
        }
    case EK_INDIR:
    case EK_VALUERA:
    case EK_PROP:
    case EK_ARRINDEX:
    case EK_FIELD:
        genLoad(tree);
        break;

    case EK_ZEROINIT:
        genZeroInit(tree->asZEROINIT(), valUsed);
        return;
    case EK_ADDR:
        genPtrAddr(tree->asBIN()->p1, !!(tree->flags & EXF_ADDRNOCONV), valUsed);
        return;
    case EK_ASSG: {
        EXPR * op1 = tree->asBIN()->p1;
        EXPR * op2 = tree->asBIN()->p2;

        if (isExprOptimizedAway(op1)) {
            if (valUsed) {
                genExpr(op2);
            } else {
                genSideEffects(op2);
            }
            return;
        }

        AddrInfo addr;

        int cvalStack = genAddr(op1, addr);
        ASSERT(cvalStack >= 0);

        genExpr(op2);
        PSLOT slot = NULL;
        if (valUsed) {
            putOpcode(CEE_DUP);
            if (cvalStack)
                slot = STOREINTEMP(op2->type, TK_SHORTLIVED);
        }

        genStore(op1, addr);

        if (slot) {
            dumpLocal(slot, false);
            freeTemporary(slot);
        }
        return;
    }
    case EK_ANONMETH:
    case EK_MEMGRP:
        // We should have given an error
        ASSERT(compiler()->FAbortCodeGen(0));
        putOpcode(CEE_LDNULL); // Just to keep the stack happy
        return;
    default:
        if (tree->flags & EXF_BINOP) {
            genBinopExpr(tree->asBIN(), valUsed);
        } else {
            ASSERT(!"bad expr type");
        }
    }

    if (!valUsed) {
        putOpcode(CEE_POP);
    }
}

void ILGENREC::genZero(BASENODE * tree, TYPESYM * type)
{
    switch (type->fundType()) {
    case FT_REF:
        putOpcode(CEE_LDNULL);
        break;
    case FT_PTR:
        genIntConstant(0);
        putOpcode(CEE_CONV_U);
        break;
    case FT_STRUCT:
        ASSERT(type->isPredefType(PT_INTPTR) || type->isPredefType(PT_UINTPTR));
        genIntConstant(0);
        putOpcode(CEE_CONV_I);
        break;
    default:
        type = type->underlyingType();
        ASSERT(type->isPredefined());
        EXPRCONSTANT expr;
        expr.kind = EK_CONSTANT;

        CONSTVAL constVal = compiler()->getBSymmgr().GetPredefZero((PREDEFTYPE)(type->getPredefType()));
        expr.getVal() = (CONSTVALNS&)constVal;

        expr.setType(type);
        expr.tree = tree;
        genExpr(&expr);
        break;
    }
}

void ILGENREC::genZeroInit(EXPRZEROINIT * tree, bool valUsed)
{
    EXPR * op = tree->p1;

    if (op && isExprOptimizedAway(op)) {
        op = NULL;
    }

    if (!op && !valUsed)
        return;

    TYPESYM * type = tree->type;

    switch (type->fundType()) {
    case FT_VAR:
        // Type variables always follow FT_STRUCT - even with a class bound.
        // Fall through....
    case FT_STRUCT:
        if (valUsed) {
            AddrInfo addr;
            int cvalStack = 0;

            if (op) {
                cvalStack = genAddr(op, addr);
                ASSERT(cvalStack >= 0);
            }

            PSLOT slot = allocTemporary(type, TK_SHORTLIVED);
            genSlotAddress(slot);
            putOpcode(CEE_INITOBJ);
            emitTypeToken(type);
            dumpLocal(slot, false);

            if (op) {
                if (!cvalStack)
                    putOpcode(CEE_DUP);
                genStore(op, addr);
                if (cvalStack)
                    dumpLocal(slot, false);
            }
            freeTemporary(slot);
        }
        else {
            ASSERT(op);
            PSLOT slot = NULL;
            genMemoryAddress(op, &slot);
            ASSERT(!slot);
            putOpcode(CEE_INITOBJ);
            emitTypeToken(type);
        }
        break;

    default:
        if (op) {
            AddrInfo addr;

            int cvalStack = genAddr(op, addr);
            ASSERT(cvalStack >= 0);

            genZero(tree->tree, type);
            if (valUsed && !cvalStack)
                putOpcode(CEE_DUP);

            genStore(op, addr);

            if (valUsed && cvalStack)
                genZero(tree->tree, type);
        } else {
            ASSERT(valUsed);
            genZero(tree->tree, type);
        }
        break;
    }
}


void ILGENREC::writeArrayValues1(BYTE * buffer, EXPR * tree)
{
    EXPRLOOP(tree, elem)
        EXPRCONSTANT * exprConst = elem->GetConst()->asCONSTANT();
        if (exprConst) {
            *buffer = (BYTE) (exprConst->getVal().uiVal & 0xff);
        } else {
            *buffer = 0;
        }
        buffer += 1;
    ENDLOOP;

}

void ILGENREC::writeArrayValues2(BYTE * buffer, EXPR * tree)
{
    EXPRLOOP(tree, elem)
        EXPRCONSTANT * exprConst = elem->GetConst()->asCONSTANT();
        if (exprConst) {
            *((WORD*)buffer) = VAL16((WORD) (exprConst->getVal().uiVal & 0xffff));
        } else {
            *((WORD*)buffer) = (WORD) 0;
        }
        buffer += 2;
    ENDLOOP;

}

void ILGENREC::writeArrayValues4(BYTE * buffer, EXPR * tree)
{
    EXPRLOOP(tree, elem)
        EXPRCONSTANT * exprConst = elem->GetConst()->asCONSTANT();
        if (exprConst) {
            *((DWORD*)buffer) = VAL32((DWORD) (exprConst->getVal().uiVal));
        } else {
            *((DWORD*)buffer) = (DWORD) 0;
        }
        buffer += 4;
    ENDLOOP;

}

void ILGENREC::writeArrayValues8(BYTE * buffer, EXPR * tree)
{
    EXPRLOOP(tree, elem)
        EXPRCONSTANT * exprConst = elem->GetConst()->asCONSTANT();
        if (exprConst) {
            *((unsigned __int64 *)buffer) = VAL64((unsigned __int64) *(exprConst->getVal().ulongVal));
        } else {
            *((unsigned __int64 *)buffer) = (unsigned __int64) 0;
        }
        buffer += 8;
    ENDLOOP;
}

void ILGENREC::writeArrayValuesD(BYTE * buffer, EXPR * tree)
{
    ASSERT(sizeof(double) == 8);
    EXPRLOOP(tree, elem)
        EXPRCONSTANT * exprConst = elem->GetConst()->asCONSTANT();
        if (exprConst) {
            __int64 tmp = VAL64(*exprConst->getVal().ulongVal);
           *(double*)buffer = (double &)tmp;
        } else {
            *((double*)buffer) = (double) 0.0;
        }
        buffer += 8;
    ENDLOOP;
}

void ILGENREC::writeArrayValuesF(BYTE * buffer, EXPR * tree)
{
    ASSERT(sizeof(float) == 4);
    EXPRLOOP(tree, elem)
        EXPRCONSTANT * exprConst = elem->GetConst()->asCONSTANT();
        if (exprConst) {
            float tmpfloat = (float)*exprConst->getVal().doubleVal;
            __int32 tmp = VAL32(*reinterpret_cast<const __int32 *>(&tmpfloat));
            *((float*)buffer) = (float &)tmp;
        } else {
            *((float*)buffer) = (float) 0.0;
        }
        buffer += 4;
    ENDLOOP;
}

void ILGENREC::genArrayInitConstant(EXPRARRINIT * tree, TYPESYM * elemType, bool valUsed)
{
    int rank = tree->type->asARRAYSYM()->rank;

    int size, initSize = size = compiler()->getBSymmgr().GetAttrArgSize(elemType->getPredefType());

    ASSERT(initSize > 0);

    for (int i = 0; i < rank; i++) {
        size = size * tree->dimSizes[i];
    }

    BYTE * buffer;
    mdToken token = compiler()->emitter.GetGlobalFieldDef(method, ++globalFieldCount, (unsigned) size, &buffer);
    
    switch (initSize) {
    case 1:
        writeArrayValues1(buffer, tree->args);
        break;
    case 2:
        writeArrayValues2(buffer, tree->args);
        break;
    case 4:
        if (elemType->asAGGTYPESYM()->fundType() == FT_R4) {
            writeArrayValuesF(buffer, tree->args);
        } else {
            writeArrayValues4(buffer, tree->args);
        }
        break;
    case 8:
        if (elemType->asAGGTYPESYM()->fundType() == FT_R8) {
            writeArrayValuesD(buffer, tree->args);
        } else {
            writeArrayValues8(buffer, tree->args);
        }
        break;
    default:
        ASSERT(0);
    }

    if (valUsed || !(tree->flags & EXF_ARRAYALLCONST)) {
        putOpcode(CEE_DUP);
    }

    putOpcode(CEE_LDTOKEN);
    putDWORD(token);
    putOpcode(CEE_CALL);
    emitMethodToken(compiler()->getBSymmgr().methInitArray);
    curStack -= 2;

}


void ILGENREC::genArrayInit(EXPRARRINIT *tree, bool valUsed)
{

    ARRAYSYM *arrType = tree->type->asARRAYSYM();
    int rank = arrType->rank;
    TYPESYM * elemType = arrType->elementType();

    PSLOT wrappedValue = NULL;

    if (tree->flags & EXF_PUSH_OP_FIRST) {
        // This only occurs for string/object concatenation.
        ASSERT(rank == 1 && tree->args);
        ASSERT(elemType->isPredefType(PT_STRING) || elemType->isPredefType(PT_OBJECT));

        EXPR * exprFirst = tree->args->kind == EK_LIST ? tree->args->asBIN()->p1 : tree->args;
        genExpr(exprFirst);
        wrappedValue = STOREINTEMP(elemType, TK_SHORTLIVED);
    }

    for (int i = 0; i < rank; i++) {
        genIntConstant(tree->dimSizes[i]);
    }
    if (rank == 1) {
        putOpcode(CEE_NEWARR);
        emitTypeToken(elemType);
    } else {
        genArrayCall(arrType, rank, ARRAYMETH_CTOR);
    }

    if (!tree->args) {
        // If there are no args, our code gen should be just like
        // EK_NEWARRAY.
        return;
    }

    if (tree->flags & (EXF_ARRAYCONST | EXF_ARRAYALLCONST)) {
        ASSERT(!compiler()->FEncBuild());
        genArrayInitConstant(tree, elemType, valUsed);
        if (tree->flags & EXF_ARRAYALLCONST) {
            return;
        }
    }

    PSLOT slot = allocTemporary(tree->type, TK_SHORTLIVED);
    dumpLocal(slot, true);

    if (tree->args) {

        ILCODE il = ILarrayStore[elemType->fundType()];

        int *rows;
        rows = STACK_ALLOC_ZERO(int, rank);
    
        bool fStruct = (elemType->fundType() == FT_STRUCT);

        EXPRLOOP(tree->args, elem)
            EXPR * exprConst = elem->GetConst();
            if (wrappedValue || !exprConst || !exprConst->isZero(true) && !(tree->flags & EXF_ARRAYCONST)) {
                dumpLocal(slot, false);
                for (int i = 0; i < rank; i ++)
                    genIntConstant(rows[i]);

                if (fStruct) {
                    if (rank == 1) {
                        putOpcode(CEE_LDELEMA);
                        emitTypeToken(elemType);
                    }
                    else
                        genArrayCall(arrType, rank, ARRAYMETH_LOADADDR);
                }

                if (wrappedValue) {
                    dumpLocal(wrappedValue, false);
                    FREETEMP(wrappedValue);
                }
                else
                    genExpr(elem);

                if (fStruct) {
                    putOpcode(CEE_STOBJ);
                    emitTypeToken(elemType);
                }
                else if (rank == 1 && il != CEE_ILLEGAL) {
                    putOpcode(il);
                    if (il == CEE_STELEM)
                        emitTypeToken(elemType);
                }
                else
                    genArrayCall(arrType, rank, ARRAYMETH_STORE);
            }
            else
                genSideEffects(elem);

            int row = rank - 1;
            while(true) {
                rows[row]++;
                if (rows[row] == tree->dimSizes[row]) {
                    rows[row] = 0;
                    if (row == 0) {
                        ASSERT(_nd == NULL);
                        goto DONE;
                    }
                    row--;
                } else {
                    break;
                }
            }        
        ENDLOOP;
    }    
DONE:

    if (valUsed) dumpLocal(slot, false);

    freeTemporary(slot);
}


void ILGENREC::genCast(EXPRCAST * tree, bool valUsed)
{
    int key = tree->flags & EXF_CAST_ALL;

    // The only valid combination with more than one bit set is (EXF_FORCE_BOX | EXF_REFCHECK).
    ASSERT(!(key & (key - 1)) || key == (EXF_FORCE_BOX | EXF_REFCHECK));

    // Short-cut for unused expressions - simply generate the side effects
    // of the expression instead.
    if (!valUsed && !(tree->flags & (EXF_CHECKOVERFLOW | EXF_UNBOX | EXF_REFCHECK | EXF_FORCE_UNBOX))) {
        genSideEffects(tree->p1);
        return;
    }

    ILCODE il;

    TYPESYM * fromType = tree->p1->type->underlyingType();
    TYPESYM * toType = tree->type->underlyingType();

    // FORCE_BOX may be used with a REFCHECK (for type variable to interface cast).
    if (key == EXF_BOX || (key & EXF_FORCE_BOX)) {
        genExpr(tree->p1);
        putOpcode(CEE_BOX);
        emitTypeToken(tree->p1->type);
        key &= ~(EXF_BOX | EXF_FORCE_BOX);
        ASSERT(key == 0 || key == EXF_REFCHECK);
        if (key == EXF_REFCHECK)
            goto LRefCheck;
    } else if (key == EXF_INDEXEXPR) {
        genExpr(tree->p1);
        if (toType->fundType() == FT_U4) {
            putOpcode(CEE_CONV_U);
        } else if (toType->fundType() == FT_I8) {
            putOpcode(CEE_CONV_OVF_I);
        } else {
            ASSERT(toType->fundType() == FT_U8);
            putOpcode(CEE_CONV_OVF_I_UN);
        }
    } else if (key == EXF_FORCE_UNBOX) {
        genExpr(tree->p1);  
        putOpcode(CEE_UNBOX_ANY);
        emitTypeToken(tree->type);
    } else if (key == EXF_STATIC_CAST) {
        PSLOT slot = allocTemporary(toType, TK_SHORTLIVED);
        genExpr(tree->p1);
        dumpLocal(slot, true);
        dumpLocal(slot, false);
        freeTemporary(slot);
    } else {
        // This is null being cast to a pointer, change this into a zero instead...
        if (toType->isPTRSYM() && !fromType->isPTRSYM() && tree->p1->type->fundType() == FT_REF && tree->p1->GetConst()) {
            genSideEffects(tree->p1);
            genIntConstant(0);
            fromType = compiler()->GetReqPredefType(PT_UINT, false);
        } else {
            genExpr(tree->p1);
        }

        switch (key) {
        case EXF_UNBOX:
            putOpcode(CEE_UNBOX_ANY);
            emitTypeToken(tree->type);
            break;
        case EXF_REFCHECK:
LRefCheck:
            putOpcode(CEE_CASTCLASS);
            emitTypeToken(toType);
            break;
        default:
            unsigned int toPredef = COMPILER::getPredefIndex(toType);
            unsigned int fromPredef = COMPILER::getPredefIndex(fromType);
            if (fromPredef == UNDEFINEDINDEX || toPredef == UNDEFINEDINDEX) {
                break;
            }
            if (tree->flags & EXF_CHECKOVERFLOW) {
                il = simpleTypeConversionsOvf[fromPredef][toPredef];
            } else {
                il = simpleTypeConversions[fromPredef][toPredef];
            }
            ASSERT( il != CEE_ILLEGAL);
            if (il != cee_next) {
                putOpcode(il);
                if (il == CEE_CONV_R_UN) {
                    il = simpleTypeConversionsEx[fromPredef][toPredef];
                    ASSERT(il != CEE_ILLEGAL && il != cee_next);
                    putOpcode(il);
                }
            }
        }
    }

    if (!valUsed) putOpcode(CEE_POP);
}


void ILGENREC::genQMark(EXPRBINOP * tree, bool valUsed)
{
    if (!valUsed) {
        genSideEffects(tree);
        return;
    }

    if (tree->type->isInterfaceType()) {
        EXPR * p1 = tree->p2->asBIN()->p1;
        EXPR * p2 = tree->p2->asBIN()->p2;
        ASSERT(p1->type == tree->type && p2->type == tree->type);

        if (p1->kind == EK_CAST && p2->kind == EK_CAST && !(p1->flags & EXF_CAST_ALL) && !(p2->flags & EXF_CAST_ALL)) {
            // We only need to static cast one of the two.
            p2->flags |= EXF_STATIC_CAST;
        }
    }

    BBLOCK * trueBranch = genCondBranch(tree->p1, createNewBB(), true);
    tree = tree->p2->asBINOP();

    BBLOCK * fallThrough = createNewBB();

    genExpr(tree->p2);
    startNewBB(trueBranch, CEE_BR, fallThrough);
    genExpr(tree->p1);
    curStack--;
    startNewBB(fallThrough);

}

// generate code to create an array...
void ILGENREC::genNewArray(EXPRBINOP * tree)
{
    ARRAYSYM *arrType = tree->type->asARRAYSYM();
    int rank = arrType->rank;
    TYPESYM * elemType = arrType->elementType();

    int oldStack = curStack;

    genExpr(tree->p1);
    if (rank == 1) {
        putOpcode(CEE_NEWARR);
        emitTypeToken(elemType);

        return;
    }

    ASSERT(curStack > oldStack);
    genArrayCall(arrType, curStack - oldStack,ARRAYMETH_CTOR);
}

void ILGENREC::GenMultiGet(EXPRMULTI * tree, bool valUsed)
{
    ASSERT(tree->pinfo);

    MultiOpInfo & info = *tree->pinfo;

    ASSERT(!info.slot);
    ASSERT(!info.cget);
    info.cget++;

    // Stack level shouldn't have changed since GenMultiOp called genExpr.
    ASSERT(info.cvalStack == curStack);

    if (!info.fNeedOld && !valUsed) {
        genAccess(tree->left, AccessTask::Addr | AccessTask::Load, info.addr);
        putOpcode(CEE_POP);
        return;
    }

    genAccess(tree->left, AccessTask::Addr | AccessTask::Load, info.addr);

    // There should be at least a value on the stack - and possibly an "address"
    ASSERT(curStack > info.cvalStack);

    if (info.fNeedOld) {
        int cval = 1;
        if (valUsed) {
            putOpcode(CEE_DUP);
            cval++;
        }
        if (curStack - info.cvalStack > cval) {
            // There's an "address" on the stack so store the old value in a temp.
            info.slot = allocTemporary(tree->left->type, TK_SHORTLIVED);
            dumpLocal(info.slot, true);
        }
    }
}

void ILGENREC::GenMultiOp(EXPRMULTI * tree, bool valUsed)
{
    MultiOpInfo info;

    tree->pinfo = &info;

    info.addr.Init();
    info.fNeedOld = valUsed && (tree->flags & EXF_ISPOSTOP);
    info.cget = 0;
    info.cvalStack = curStack;
    info.slot = NULL;

    genExpr(tree->op);

    // GenMultiGet should have been called exactly once.
    ASSERT(info.cget == 1);

    // There should at least be a value on the stack (for assignment) and
    // possibly an "address"
    ASSERT(curStack > info.cvalStack);

    if (valUsed && !info.fNeedOld) {
        // Need the new value.
        ASSERT(!info.slot);

        putOpcode(CEE_DUP);
        if (curStack - info.cvalStack > 2) {
            // There's an "address" on the stack so store the new value in a temp.
            info.slot = allocTemporary(tree->left->type, TK_SHORTLIVED);
            dumpLocal(info.slot, true);
        }
    }

    genStore(tree->left, info.addr);

    if (info.slot) {
        dumpLocal(info.slot, false);
        freeTemporary(info.slot);
    }

    tree->pinfo = NULL;
}

// generate a standard binary operation...
void ILGENREC::genBinopExpr(EXPRBINOP * tree, bool valUsed)
{
    ILCODE ilcode;

    switch (tree->kind) {
    case EK_LOGAND:
    case EK_LOGOR:
    case EK_LOGNOT:
    case EK_LT:
    case EK_LE:
    case EK_GT:
    case EK_GE:
    case EK_EQ:
    case EK_NE:
        genCondExpr(tree, true);
        return;

    case EK_ADD:
    case EK_SUB:
    case EK_MUL:
    case EK_DIV:
    case EK_MOD:
    case EK_NEG:
    case EK_BITAND: 
    case EK_BITOR:
    case EK_BITXOR:
    case EK_BITNOT:
    case EK_LSHIFT:
    case EK_RSHIFT:
    case EK_ARRLEN:
        if (tree->flags & EXF_CHECKOVERFLOW) {
            if (tree->p1->type->isUnsigned()) {
                ilcode = ILarithInstrUNOvf[tree->kind - EK_ADD];
            } else {
                ilcode = ILarithInstrOvf[tree->kind - EK_ADD];
            } 
        } else {
            if (tree->p1->type->isUnsigned()) {
                ilcode = ILarithInstrUN[tree->kind - EK_ADD];
            } else {
                ilcode = ILarithInstr[tree->kind - EK_ADD];
            }
        }
        break;

    case EK_UPLUS:
        // This is a non-op (we just need to emit the numeric promotion)
        genExpr(tree->p1);
        ASSERT(!tree->p2);
        return;

    default:
        ASSERT(!"bad binop expr"); ilcode = CEE_ILLEGAL;
    }

    genExpr(tree->p1);
    genExpr(tree->p2);

    putOpcode(ilcode);

    if (ilcode == CEE_LDLEN) {
        putOpcode(CEE_CONV_I4);
    }
}

// generate an integer constant by using the smallest possible ilcode
void ILGENREC::genIntConstant(int val)
{
    ASSERT(CEE_LDC_I4_M1 + 9 == CEE_LDC_I4_8);
    if (val >= -1 && val <= 8) {
        putOpcode((ILCODE) (CEE_LDC_I4_0 + val));
        return;
    }

    if (val == (char) (0xFF & val)) {
        putOpcode(CEE_LDC_I4_S);
        putCHAR((char)val);
        return;
    }

    putOpcode(CEE_LDC_I4);
    putDWORD((DWORD)val);
}


void ILGENREC::genFloatConstant(float val)
{
    putOpcode(CEE_LDC_R4);
    putDWORD(*(DWORD*)(&val));
}

void ILGENREC::genDoubleConstant(double * val)
{
    putOpcode(CEE_LDC_R8);
    putQWORD((__int64 *)val);
}

void ILGENREC::genDecimalConstant(DECIMAL * val)
{
    AGGSYM * aggDec = compiler()->GetOptPredefAgg(PT_DECIMAL);
    ASSERT(aggDec);
    METHSYM *meth = compiler()->getBSymmgr().LookupAggMember(compiler()->namemgr->GetPredefName(PN_CTOR), aggDec, MASK_ALL)->asMETHSYM();
    AGGTYPESYM *int32Sym = compiler()->GetReqPredefType(PT_INT);
    int countArgs = 0;
    
    // check if we can call a simple constructor
    if (!DECIMAL_SCALE(*val) && !DECIMAL_HI32(*val) && (!(DECIMAL_MID32(*val) & 0x80000000) || (DECIMAL_SIGN(*val) && DECIMAL_MID32(*val) == 0x80000000 && DECIMAL_LO32(*val) == 0))) {
        if (!DECIMAL_MID32(*val) && (!(DECIMAL_LO32(*val) & 0x80000000) || (DECIMAL_SIGN(*val) && DECIMAL_LO32(*val) == 0x80000000))) {
            int val32 = DECIMAL_LO32(*val);
            if (DECIMAL_SIGN(*val)) {
                val32 = -val32;
            }
            genIntConstant(val32);

            while (meth->params->size != 1 || meth->params->Item(0) != int32Sym) {
                meth = meth->nextSameName->asMETHSYM();
            }
        } else {
            __int64 val64;
            val64 = ((ULONGLONG)DECIMAL_MID32(*val) << 32) | DECIMAL_LO32(*val);
            if (DECIMAL_SIGN(*val)) {
                val64 = -val64;
            }
            genLongConstant(&val64);

            AGGTYPESYM *int64Sym = compiler()->GetReqPredefType(PT_LONG);
            while (meth->params->size != 1 || meth->params->Item(0) != int64Sym) {
                meth = meth->nextSameName->asMETHSYM();
            }
        }
        countArgs = 1;
    } else {

        // new Decimal(lo, mid, hi, sign, scale)
        genIntConstant(DECIMAL_LO32(*val));
        genIntConstant(DECIMAL_MID32(*val));
        genIntConstant(DECIMAL_HI32(*val));
        genIntConstant(DECIMAL_SIGN(*val));
        genIntConstant(DECIMAL_SCALE(*val));

        AGGTYPESYM *bool32Sym = compiler()->GetReqPredefType(PT_BOOL);
        AGGTYPESYM *byte32Sym = compiler()->GetReqPredefType(PT_BYTE);
        while (meth->params->size != 5 || meth->params->Item(0) != int32Sym || meth->params->Item(1) != int32Sym ||
                meth->params->Item(2) != int32Sym || meth->params->Item(3) != bool32Sym || meth->params->Item(4) != byte32Sym) {
            meth = meth->nextSameName->asMETHSYM();
            ASSERT(meth);
        }
        countArgs = 5;
    }

    putOpcode(CEE_NEWOBJ);
    emitMethodToken(meth);

    curStack -= countArgs;
}

// generate a long, but try to do it as an int if it fits...
void ILGENREC::genLongConstant(__int64 * val)
{
    if ((__int32)((*val) & 0xFFFFFFFF) == *val) {
        genIntConstant((__int32)*val);
        putOpcode(CEE_CONV_I8);
    } else if ((unsigned __int32)((*val) & 0xFFFFFFFF) == *(unsigned __int64*)val) {
        genIntConstant((__int32)*val);
        putOpcode(CEE_CONV_U8);
    } else {
        putOpcode(CEE_LDC_I8);
        putQWORD(val);
    }
}

unsigned ILGENREC::getArgCount(EXPR * args)
{
    unsigned rval = 0;
    EXPRLOOP(args, arg)
        rval++;
    ENDLOOP;
    
    return rval;
}


void ILGENREC::emitRefValue(EXPRBINOP * tree)
{
    genExpr(tree->p1);
    putOpcode(CEE_REFANYVAL);
    emitTypeToken(tree->p2->asTYPEOF()->sourceType);
}


void ILGENREC::genAccess(EXPR * tree, AccessTaskEnum flags, AddrInfo & addr)
{
    ASSERT(AccessTask::FValid(flags));

    if (!AccessTask::FStore(flags))
        addr.Init();

    switch (tree->kind) {
    default:
        ASSERT(!"bad addr expr");
        return;

    case EK_FIELD:
        genFieldAccess(tree->asFIELD(), flags, addr);
        return;

    case EK_PROP:
        genPropAccess(tree->asPROP(), flags, addr);
        return;

    case EK_ARRINDEX:
        genArrayAccess(tree->asBIN(), flags, addr);
        return;

    case EK_WRAP:
        if (!AccessTask::FLoadOrStore(flags)) {
            ASSERT(AccessTask::FAddrOnly(flags));
            return;
        }
        if (!tree->asWRAP()->slot)
            tree->asWRAP()->slot = allocTemporary(tree->asWRAP()->type, tree->asWRAP()->tempKind);
        dumpLocal(tree->asWRAP()->slot, AccessTask::FStore(flags));
        return;

    case EK_LDTMP:
        ASSERT(tree->asLDTMP()->tmp->slot);
        if (!AccessTask::FLoadOrStore(flags)) {
            ASSERT(AccessTask::FAddrOnly(flags));
            return;
        }
        dumpLocal(tree->asLDTMP()->tmp->slot, AccessTask::FStore(flags));
        return;

    case EK_LOCAL:
        {
            ASSERT(!tree->asLOCAL()->local->isConst);
            PSLOT slot = &tree->asLOCAL()->local->slot;
            if (!slot->isRefParam) {
                if (AccessTask::FLoadOrStore(flags))
                    dumpLocal(slot, AccessTask::FStore(flags));
                return;
            }

            if (AccessTask::FAddrOrLoad(flags)) {
                dumpLocal(slot, false);
                if (AccessTask::FDup(flags))
                    putOpcode(CEE_DUP);
            }
        }
        // Indirect.
        break;

    case EK_INDIR:
        if (AccessTask::FAddrOrLoad(flags)) {
            genExpr(tree->asBIN()->p1);
            if (AccessTask::FDup(flags))
                putOpcode(CEE_DUP);
        }
        // Indirect.
        break;

    case EK_VALUERA:
        if (AccessTask::FAddrOrLoad(flags)) {
            emitRefValue(tree->asBIN());
            if (AccessTask::FDup(flags))
                putOpcode(CEE_DUP);
        }
        // Indirect.
        break;
    }

    ASSERT(!addr.slotStore);

    if (!AccessTask::FLoadOrStore(flags)) {
        ASSERT(AccessTask::FAddrOnly(flags));
        return;
    }

    // This assumes the address is on the stack.
    FUNDTYPE ft = tree->type->fundType();
    ILCODE code = (AccessTask::FStore(flags) ? ILGENREC::ILstackStore : ILGENREC::ILstackLoad)[ft];
    putOpcode(code);
    if (code == CEE_LDOBJ || code == CEE_STOBJ)
        emitTypeToken(tree->type);
}


void ILGENREC::genFieldAccess(EXPRFIELD * tree, AccessTaskEnum flags, AddrInfo & addr)
{
    ASSERT(AccessTask::FValid(flags));
    ASSERT(!addr.slotStore || AccessTask::FStore(flags));

    MEMBVARSYM * field = tree->fwt.Field();
    ASSERT(!!tree->object == !field->isStatic);

    if (!field->isStatic && AccessTask::FAddrOrLoad(flags)) {

        if (flags == AccessTask::Load && field->getClass()->IsValueType()) {
            if (tree->fwt.Type() == field->type) {
                // This handles int32 loading its m_value field (also of type int32!).
                genExpr(tree->object);
                return;
            }
            if (!(tree->flags & EXF_LVALUE) && tree->object->GetSeqVal()->kind != EK_LOCAL && !field->getClass()->IsCLRAmbigStruct())
                genExpr(tree->object);
            else
                genObjectPtr(tree->object, field, &addr.slotStore);
        }
        else {
            genObjectPtr(tree->object, field, &addr.slotStore);
            if (AccessTask::FDup(flags))
                putOpcode(CEE_DUP);
        }
    }

    if (!AccessTask::FLoadOrStore(flags)) {
        ASSERT(AccessTask::FAddrOnly(flags));
        return;
    }

    if (field->isVolatile)
        putOpcode(CEE_VOLATILE);

    if (field->isStatic)
        putOpcode(AccessTask::FStore(flags) ? CEE_STSFLD : CEE_LDSFLD);
    else
        putOpcode(AccessTask::FStore(flags) ? CEE_STFLD : CEE_LDFLD);

    emitFieldToken(field, tree->fwt.Type());

    if (!AccessTask::FAddr(flags) && addr.slotStore) {
        FREETEMP(addr.slotStore);
    }
}


void ILGENREC::genPropAccess(EXPRPROP * tree, AccessTaskEnum flags, AddrInfo & addr)
{
    ASSERT(AccessTask::FValid(flags));
    ASSERT(!addr.slotStore || AccessTask::FStore(flags));

    PROPSYM * propSlot = tree->pwtSlot.Prop();
    EXPR * obj = tree->object;
    bool fConstrained = false;
    EXPR * args = tree->args;

    // ASSERT(type && type->getAggregate() == prop->getClass());
    ASSERT(!!obj == !propSlot->isStatic);

    if (tree->flags & EXF_CONSTRAINED) {
        ASSERT(!(tree->flags & EXF_BASECALL));
        ASSERT(!propSlot->isStatic);
        ASSERT(obj->kind == EK_CAST &&
            (obj->asCAST()->p1->type->isTYVARSYM() ||
                obj->asCAST()->p1->type->IsValType()));
        obj = obj->asCAST()->p1;
        fConstrained = true;
    }

    if (AccessTask::FAddrOrLoad(flags)) {
        // Generate the "address" on the stack. The address consists of:
        //
        // 1) The object if there is one. If (fConstrained), we need a memory address,
        //    not the value.
        // 2) The args (for indexers).
        //
        // If FDup(flags), we need to duplicate the entire "address" on the stack.
        if (fConstrained)
            genMemoryAddress(obj, &addr.slotStore, false, true);
        else if (!propSlot->isStatic)
            genObjectPtr(obj, propSlot, &addr.slotStore);

        if (AccessTask::FDup(flags)) {
            genArgsDup(obj, fConstrained, args, false);
        }
        else {
            genExpr(args);
        }
    }

    if (!AccessTask::FLoadOrStore(flags)) {
        ASSERT(AccessTask::FAddrOnly(flags));
        return;
    }

    MethWithType mwt = AccessTask::FStore(flags) ? tree->mwtSet : tree->mwtGet;
    ASSERT(mwt.Sym() && mwt.Sym()->isMETHSYM() && !mwt.Meth()->isStatic == !propSlot->isStatic);
#if USAGEHACK
    mwt.Meth()->isUsed = true;
#endif

    if (fConstrained) {
        ASSERT(obj->type->isTYVARSYM() || obj->type->IsValType());
        putOpcode(CEE_CONSTRAINED);
        emitTypeToken(obj->type);
        putOpcode(CEE_CALLVIRT);
    }
    else if (callAsVirtual(mwt.Meth(), obj, !!(tree->flags & EXF_BASECALL)))
        putOpcode(CEE_CALLVIRT);
    else
        putOpcode(CEE_CALL);

    emitMethodToken(mwt.Meth(), mwt.Type());

    if (compileForEnc && AccessTask::FStore(flags)) {
        putOpcode(CEE_NOP);
    }

    curStack -= mwt.Meth()->params->size;
    if (!mwt.Meth()->isStatic)
        curStack--;
    ASSERT(curStack >= 0);
    if (AccessTask::FLoad(flags)) {
        curStack++;
        markStackMax();
    }

    if (!AccessTask::FAddr(flags) && addr.slotStore) {
        FREETEMP(addr.slotStore);
    }
}


void ILGENREC::genArrayAccess(EXPRBINOP * tree, AccessTaskEnum flags, AddrInfo & addr)
{
    ASSERT(AccessTask::FValid(flags));
    ASSERT(!addr.slotStore);
    ASSERT(tree->kind == EK_ARRINDEX);
    ASSERT(tree->type == tree->p1->type->asARRAYSYM()->elementType());

    ARRAYSYM * typeArray = tree->p1->type->asARRAYSYM();
    bool fValueType = tree->type->isStructOrEnum();
    FUNDTYPE ft = tree->type->fundType();

    ASSERT(getArgCount(tree->p2) == (unsigned)typeArray->rank);

    if (AccessTask::FAddrOrLoad(flags)) {
        ASSERT(!addr.fIndirectArray);

        // Generate the "address" on the stack. The address is either:
        //
        // 1) A memory address when addr.fIndirectArray is set to true. This is when
        //    the element type is a value type and FDup is true or when the element type
        //    is a non-trivial struct. Duplicating a single memory address is much more
        //    efficient than duplicating the other address form.
        //
        // 2) The object followed by the indices. We always use this form for reference
        //    types, since ldelema causes a type check which may fail when our assignment
        //    may very well succeed.

        // Use indirection only on value types since ldelema on a reference type array
        // causes a type check (because of array covariance).
        addr.fIndirectArray = fValueType && (AccessTask::FDup(flags) || ft == FT_STRUCT);

        // Load the array
        genExpr(tree->p1);

        if (addr.fIndirectArray) {
            genExpr(tree->p2);
            if (typeArray->rank == 1) {
                putOpcode(CEE_LDELEMA);
                emitTypeToken(tree->type);
            }
            else {
                genArrayCall(typeArray, typeArray->rank, ARRAYMETH_LOADADDR);
            }
            if (AccessTask::FDup(flags))
                putOpcode(CEE_DUP);
        }
        else if (AccessTask::FDup(flags))
            genArgsDup(tree->p1, false, tree->p2, true);
        else
            genExpr(tree->p2);
    }

    if (!AccessTask::FLoadOrStore(flags)) {
        ASSERT(AccessTask::FAddrOnly(flags));
        return;
    }

    if (addr.fIndirectArray) {
        putOpcode(AccessTask::FStore(flags) ? CEE_STOBJ : CEE_LDOBJ);
        emitTypeToken(tree->type);
    }
    else if (typeArray->rank == 1) {
        ILCODE code;

        if (AccessTask::FStore(flags)) {
            code = ILGENREC::ILarrayStore[ft];
            putOpcode(code);
            if (code == CEE_STELEM)
                emitTypeToken(tree->type);
        }
        else {
            code = ILGENREC::ILarrayLoad[ft];
            putOpcode(code);
            if (code == CEE_LDELEM)
                emitTypeToken(tree->type); 
        }
    }
    else {
        genArrayCall(typeArray, typeArray->rank, AccessTask::FStore(flags) ? ARRAYMETH_STORE : ARRAYMETH_LOAD);
    }

    ASSERT(!addr.slotStore);
}


void ILGENREC::genArgsDup(EXPR * obj, bool fConstrained, EXPR * args, bool fArray)
{
    // The object should be on the stack coming into this (if there is one).
    // The stack value is assumed to be indirect iff fConstrained or obj->type
    // is a value type.
    if (obj)
        putOpcode(CEE_DUP);

    if (!args)
        return;

    PSLOT slotObj = NULL;

    // Duplicate and store the object if the object addr is not constant.
    if (obj) {
        TYPESYM * type = obj->type;
        if (fConstrained || type->isStructOrEnum()) {
            ASSERT(!fArray);
            type = compiler()->getBSymmgr().GetParamModifier(type, false);
        }
        slotObj = allocTemporary(type, TK_SHORTLIVED);
        dumpLocal(slotObj, true);
    }

    // Count the number of non-constant args
    int cslot = 0;

    EXPRLOOP(args, arg)
        if (!arg->GetConst())
            cslot++;
    ENDLOOP;

    PSLOT * prgslot = (cslot > 0) ? (PSLOT *)STACK_ALLOC(PSLOT, cslot) : NULL;

    int islot = 0;
    EXPRLOOP(args, arg)
        genExpr(arg);
        if (arg->GetConst())
            continue;

        putOpcode(CEE_DUP);
        TYPESYM * type = fArray ? compiler()->getBSymmgr().GetNaturalIntSym() : arg->type;
        prgslot[islot++] = STOREINTEMP(type, TK_SHORTLIVED);
    ENDLOOP;
    ASSERT(islot == cslot);

    if (obj)
        dumpLocal(slotObj, false);

    islot = 0;
    EXPRLOOP(args, arg)
        if (arg->GetConst()) {
            // Don't gen the side effects again - just the value.
            genExpr(arg->GetConst());
        }
        else
            dumpLocal(prgslot[islot++], false);
    ENDLOOP;
    ASSERT(islot == cslot);

    if (slotObj)
        freeTemporary(slotObj);
    for (islot = 0; islot < cslot; islot++)
        freeTemporary(prgslot[islot]);
}


void ILGENREC::genArrayCall(ARRAYSYM * array, int args, ARRAYMETHOD meth)
{
    curStack -= args; // (1 for obj pointer)

    ILCODE il = CEE_CALL;

    switch (meth) {
    case ARRAYMETH_LOAD:
    case ARRAYMETH_LOADADDR:
    case ARRAYMETH_GETAT:
        break;
    case ARRAYMETH_STORE:
        curStack -= 2;
        break;
    case ARRAYMETH_CTOR:
        il = CEE_NEWOBJ;
    default:
        break;
    }
    markStackMax();
    putOpcode(il);
    emitArrayMethodToken(array, meth);
}

METHSYM * ILGENREC::getVarArgMethod(METHSYM * sym, EXPR * args)
{
    ASSERT(sym->isVarargs);
    ASSERT(!sym->isFAKEMETHSYM());

    unsigned realCount = getArgCount(args);

    unsigned fixedCount = sym->params->size - 1;

    ASSERT(realCount >= fixedCount);

    TYPESYM ** sig;

    if (realCount > fixedCount) {
        realCount++; // for the sentinel
    }

    sig = STACK_ALLOC(TYPESYM*, realCount);
    sym->params->CopyItems(0, fixedCount, sig);

    if (realCount > fixedCount) {

        TYPESYM ** sigNew = sig + fixedCount;

        *sigNew = compiler()->getBSymmgr().GetArglistSym();
        sigNew++;

        int originalCount = fixedCount;
        EXPRLOOP(args, arg)
            originalCount--;
            if (originalCount < 0) {
                if (arg->type == compiler()->getBSymmgr().GetNullType()) {
                    *sigNew = compiler()->GetReqPredefType(PT_OBJECT);
                } else {
                    *sigNew = arg->type;
                }
                sigNew++;
            }
        ENDLOOP;
    }

    TypeArray * ptaSig = compiler()->getBSymmgr().AllocParams(realCount, sig);

    ASSERT(sym->getKind() == SK_METHSYM);

    FAKEMETHSYM * previous = compiler()->getBSymmgr().LookupAggMember(sym->name, sym->getClass(), MASK_FAKEMETHSYM)->asFAKEMETHSYM();

    while (previous && (previous->parentMethSym != sym || previous->retType != sym->retType || previous->params != ptaSig)) {
        previous = compiler()->getBSymmgr().LookupNextSym(previous, sym->parent, MASK_FAKEMETHSYM)->asFAKEMETHSYM();
    }

    if (previous) return previous;

    previous = compiler()->getBSymmgr().CreateGlobalSym(SK_FAKEMETHSYM, sym->name, sym->parent)->asFAKEMETHSYM();

    sym->copyInto(previous, NULL, compiler());
    previous->parentMethSym = sym;
    previous->params = ptaSig;

    return previous;
}


__forceinline PSLOT ILGENREC::storeLocal(PSLOT slot)
{
    dumpLocal(slot, true);
    return slot;
}

// store or load a local or param.  we try to use the smallest
// opcode available...
void ILGENREC::dumpLocal(LOCSLOTINFO * slot, bool store)
{
    ASSERT(slot->HasIndex());

    dumpLocal(slot->Index(), slot->isParam, store);

    if (!store && slot->isPinned) {
        putOpcode(CEE_CONV_I);
    }
}

void ILGENREC::dumpLocal(int islot, bool isParam, bool store)
{
    ASSERT(store == false || store == true);

    ILCODE ilcode;

    int idx1 = (isParam ? 2 : 0) + (int) store;
    if (islot < 4) {
        ilcode = ILGENREC::ILlsTiny[idx1][islot];
        if (ilcode == CEE_ILLEGAL) goto USE_S;
        putOpcode(ilcode);
    } else if (islot <= 0xFF) {
USE_S:
        putOpcode(ILGENREC::ILlsTiny[idx1][4]);
        putCHAR((char)islot);
    } else {
        ASSERT(islot <= 0xffff);
        putOpcode(ILGENREC::ILlsTiny[idx1][5]);
        putWORD((WORD)islot);
    }

}

TEMPBUCKET * ILGENREC::AllocBucket()
{
    TEMPBUCKET * bucket = (TEMPBUCKET *) allocator->AllocZero(sizeof(TEMPBUCKET));
    if (!temporaries) {
        temporaries = bucket;
    }
    else {
        TEMPBUCKET * temp = temporaries;
        while (temp->next) temp = temp->next;
        temp->next = bucket;
    }
    return bucket;
}

#if DEBUG
// verify that all temporaries have been deallocated
void ILGENREC::verifyAllTempsFree()
{
    TEMPBUCKET * bucket = temporaries;
    while (bucket) {
        for (int i = 0; i < TEMPBUCKETSIZE ; i++) {
            ASSERT(!bucket->slots[i].isTaken);
        }
        bucket = bucket->next;
    }
}

// allocate a new temporary, and record where from when in debug
#undef allocTemporary
PSLOT ILGENREC::allocTemporary(TYPESYM * type, TEMP_KIND tempKind, PCSTR file, unsigned line)
#else
PSLOT ILGENREC::allocTemporary(TYPESYM * type, TEMP_KIND tempKind)
#endif
{
    if (type->isNULLSYM()) {
        type = compiler()->GetReqPredefType(PT_OBJECT);
    }

    for (TEMPBUCKET * bucket = temporaries; true; bucket = bucket->next) {
        bool fMustFindInThisBucket = false;
        if (!bucket) { 
            fMustFindInThisBucket = true;
            bucket = AllocBucket();
        }
        for (int i = 0; i < TEMPBUCKETSIZE; i++) {
            // Only use this slot if it's new (type == NULL) OR we're not optimizing and the slot 'matches'
            // Or we're getting close to the 64-locals limit that the JIT tracks
            if (!bucket->slots[i].type || (!bucket->slots[i].isTaken &&
                     bucket->slots[i].type == type && bucket->slots[i].tempKind == tempKind &&
                     (!compiler()->options.m_fOPTIMIZATIONS || (m_pbldrLocalSlotArray->Length() / sizeof(IlSlotInfo)) > 60)))
            {
                bucket->slots[i].isTaken = true;

                if (!bucket->slots[i].HasIndex())
                    bucket->slots[i].SetIndex(GetLocalSlot(tempKind, type));

                bucket->slots[i].isTemporary = true;
                bucket->slots[i].type = type;
                bucket->slots[i].tempKind = tempKind;
#if DEBUG
                bucket->slots[i].lastFile = file;
                bucket->slots[i].lastLine = line;
#endif
                return &(bucket->slots[i]);
            }
        }
        ASSERT(!fMustFindInThisBucket);
    }
}

// reset the define now that we defined the functions
#if DEBUG
#define allocTemporary(t, tk) allocTemporary(t, tk, __FILE__, __LINE__)
#endif


// free the given temporary
__forceinline void ILGENREC::freeTemporary(PSLOT slot)
{
    ASSERT(slot->isTaken && !slot->isParam && slot->type && slot->isTemporary);
    slot->isTaken = false;
}

PSLOT ILGENREC::DumpToDurable(TYPESYM * type, bool fFree, bool dumpToStack)
{
    PSLOT temp = STOREINTEMP( type, TK_DURABLE);
    closeDebugInfo();
    openDebugInfo(SpecialDebugPoint::HiddenCode, EXF_NODEBUGINFO);
    if (dumpToStack) {
        dumpLocal(temp, false);
    }
    if (!fFree)
        return temp;

    freeTemporary(temp);
    return NULL;
}

// emit MD tokens of various types
void ILGENREC::emitFieldToken(MEMBVARSYM * sym, AGGTYPESYM *methodInType)
{
    mdToken tok;

    ASSERT(methodInType && methodInType->getAggregate() == sym->getClass());

    // GENERICS: emit a field ref with a typeSpec parent whenever we use a field in a generic class.
    if (methodInType->typeArgsAll->size) {
        tok = compiler()->emitter.GetMembVarRef(sym, methodInType);
    }
    else {
        tok = compiler()->emitter.GetMembVarRef(sym);
    }
#if DEBUG
//    ASSERT(!inlineBB.sym);
    inlineBB.sym = sym;
#endif
    putDWORD(tok);
}


void ILGENREC::emitMethodToken(METHSYM * sym, AGGTYPESYM *methodInType, TypeArray *pMethArgs)
{
    if (!methodInType) {
        methodInType = sym->getClass()->getThisType();
        ASSERT(!methodInType->typeArgsAll->size);
    }
    ASSERT(sym->typeVars->size == (pMethArgs ? pMethArgs->size : 0));

    mdToken tok = compiler()->emitter.GetMethodRef(sym, methodInType, pMethArgs);

#if DEBUG
    inlineBB.sym = sym;
#endif
    putDWORD(tok);
}

void ILGENREC::emitTypeToken(TYPESYM * sym)
{
    mdToken tok = compiler()->emitter.GetTypeRef(sym, false);
#if DEBUG
    inlineBB.sym = sym;
#endif
    putDWORD(tok);
}

void ILGENREC::emitArrayMethodToken(ARRAYSYM * sym, ARRAYMETHOD methodId)
{
    mdToken tok = compiler()->emitter.GetArrayMethodRef(sym, methodId);
#if DEBUG
    inlineBB.sym = sym;
#endif
    putDWORD(tok);
}

// generate the sideeffects of an expression
void ILGENREC::genSideEffects(EXPR * tree)
{
    EXPRBINOP * colon;
    BBLOCK * labEnd;

AGAIN:

    if (!tree) return;

    if (tree->flags & (EXF_ASSGOP | EXF_CHECKOVERFLOW)) {
        genExpr(tree, false);
        return;
    }
    if (tree->flags & EXF_BINOP) {

        bool sense;
        switch (tree->kind) {
        case EK_IS:
        case EK_AS:
        case EK_SWAP:
            genSideEffects(tree->asBIN()->p1);
            tree = tree->asBIN()->p2;
            goto AGAIN;
        case EK_LOGAND: 
            sense = false;
            // for (a && b) and (a || b) we generate a and then b depending on the 
            // result of a.

            // if b has no sideefects, then we generate se(a)
DOCONDITIONAL:

            if (tree->asBIN()->p2->hasSideEffects(compiler())) {
                labEnd = genCondBranch(tree->asBIN()->p1, createNewBB(), sense);
                genSideEffects(tree->asBIN()->p2);
                startNewBB(labEnd);
                return;
            } else {
                tree = tree->asBIN()->p1;
                goto AGAIN;
            }
                        
        case EK_LOGOR: 
            sense = true;
            goto DOCONDITIONAL;

        case EK_QMARK:
            colon = tree->asBIN()->p2->asBINOP();
            if ((colon->p1 && colon->p1->hasSideEffects( compiler())) ||
                (colon->p2 && colon->p2->hasSideEffects( compiler()))) {
                BBLOCK * labTrue = genCondBranch(tree->asBIN()->p1, createNewBB(), true);
                BBLOCK * labFallThrough = createNewBB();
                genSideEffects(colon->p2);
                startNewBB(labTrue, CEE_BR, labFallThrough);
                genSideEffects(colon->p1);
                startNewBB(labFallThrough);
                return;
            } else {
                tree = tree->asBIN()->p1;
                goto AGAIN;
            }
            break;

        default:
            break;
        }

        genSideEffects(tree->asBIN()->p1);
        tree = tree->asBIN()->p2;
        goto AGAIN;
    }

    switch (tree->kind) {
    case EK_ZEROINIT:
        if (tree->hasSideEffects( compiler())) {
            genZeroInit(tree->asZEROINIT(), false);
        }
        break;
    case EK_CONCAT:
    case EK_PROP:
        genExpr(tree, false);
        break;
    case EK_CALL:
        genCall(tree->asCALL(), false);
        break;
    case EK_NOOP:
        break;
    case EK_DELIM:
        break;
    case EK_FIELD:
        if (tree->hasSideEffects( compiler())) {
            genExpr(tree, false);
            return;
        } else {
            tree = tree->asFIELD()->object;
            goto AGAIN;
        }
    case EK_ARRINIT:
        tree = tree->asARRINIT()->args;
        goto AGAIN;
    case EK_CAST:
        if (tree->flags & (EXF_BOX | EXF_UNBOX | EXF_FORCE_UNBOX| EXF_CHECKOVERFLOW | EXF_REFCHECK)) {
            genExpr(tree, false);
        } else {
            tree = tree->asCAST()->p1;
            goto AGAIN;
        }
        // fall through otherwise...
    case EK_LOCAL:
    case EK_CONSTANT:
    case EK_FUNCPTR:
    case EK_TYPEOF:
    case EK_SIZEOF:
    case EK_LDTMP:
        return;

    case EK_MULTIGET:
        GenMultiGet(tree->asMULTIGET()->multi, false);
        return;

    case EK_WRAP:
        if (tree->asWRAP()->expr && tree->asWRAP()->expr->kind == EK_WRAP && 
            tree->asWRAP()->expr->asWRAP()->slot && tree->asWRAP()->expr->asWRAP()->doNotFree)
        {
            FREETEMP(tree->asWRAP()->expr->asWRAP()->slot);
        }
        return;

    case EK_DBLQMARK: {
        EXPR * exprTest = tree->asDBLQMARK()->exprTest;
        EXPR * exprConv = tree->asDBLQMARK()->exprConv;
        bool fConv = exprConv->kind != EK_WRAP && (exprConv->kind != EK_CAST || exprConv->asCAST()->p1->kind != EK_WRAP) &&
            exprConv->hasSideEffects(compiler());
        bool fElse = tree->asDBLQMARK()->exprElse->hasSideEffects(compiler());

        if (fConv == fElse) {
            if (fConv)
                genExpr(tree, false);
            else
                genSideEffects(exprTest);
            return;
        }

        genExpr(exprTest);
        if (fConv)
            putOpcode(CEE_DUP);
        if (exprTest->type->isTYVARSYM()) {
            putOpcode(CEE_BOX);
            emitTypeToken(exprTest->type);
        }

        if (fElse) {
            BBLOCK * bbDone = createNewBB();
            startNewBB(NULL, CEE_BRTRUE, bbDone);
            curStack--;
            genSideEffects(tree->asDBLQMARK()->exprElse);
            startNewBB(bbDone);
        }
        else {
            ASSERT(fConv);
            BBLOCK * bbDone = createNewBB();
            startNewBB(NULL, CEE_BRFALSE, bbDone);
            curStack--;
            genExpr(tree->asDBLQMARK()->exprConv);
            startNewBB(bbDone);
            putOpcode(CEE_POP);
        }
        break;
        }

    default:
        VSFAIL("bad expr");
    }
}

void ILGENREC::genSlotAddress(PSLOT slot, bool ptrAddr)
{
    ASSERT(slot->HasIndex());
    if (slot->isRefParam || (!ptrAddr && (slot->type && slot->type->isPTRSYM()))) {
        dumpLocal(slot, false);
        return;
    }

    ASSERT((slot->Index() & ~0xFFFF) == 0);
    int size = 1;
    if ((slot->Index() & ~0xFF) == 0) {
        size = 0; // so its small
    }
    putOpcode(ILaddrLoad[slot->isParam][size]);
    if (size) {
        putWORD((WORD)slot->Index());
    } else {
        putCHAR((char)slot->Index());
    }
}


void ILGENREC::genSizeOf(TYPESYM * typ)
{
    putOpcode(CEE_SIZEOF);
    emitTypeToken(typ);
}

void ILGENREC::genMemoryAddress(EXPR * tree, PSLOT * pslot, bool ptrAddr, bool fReadOnly)
{
    TYPESYM * type;

LRestart:
    switch (tree->kind) {
    case EK_WRAP:
        if (!tree->asWRAP()->slot) {
            if (tree->asWRAP()->needEmptySlot) {
                tree->asWRAP()->slot = allocTemporary(tree->type, tree->asWRAP()->tempKind);
            } else {
                // we need to get the thing from the stack and load its address
                tree->asWRAP()->slot = STOREINTEMP(tree->type, tree->asWRAP()->tempKind);
                *pslot = tree->asWRAP()->slot;
            }
        }
        genSlotAddress(tree->asWRAP()->slot);
        return;
    case EK_LDTMP:
        ASSERT(tree->asLDTMP()->tmp->slot);
        genSlotAddress(tree->asLDTMP()->tmp->slot);
        return;
    case EK_STTMP:
        ASSERT(!tree->asSTTMP()->slot);
        genExpr(tree, false);
        ASSERT(tree->asSTTMP()->slot);
        genSlotAddress(tree->asSTTMP()->slot);
        return;
    case EK_LOCAL:
        ASSERT(!tree->asLOCAL()->local->isConst);
        genSlotAddress(&(tree->asLOCAL()->local->slot), ptrAddr);
        return;
    case EK_VALUERA:
        emitRefValue(tree->asBIN());
        return;
    case EK_INDIR:
        genExpr(tree->asBIN()->p1);
        return;
    case EK_FIELD:
        if (!(tree->flags & EXF_LVALUE)) {
            ASSERT(!ptrAddr);
            genExpr(tree);
            *pslot = STOREINTEMP(tree->type, TK_SHORTLIVED);
            genSlotAddress(*pslot);
            return;
        }
        if (tree->asFIELD()->fwt.Field()->isStatic) {
            ASSERT(!tree->asFIELD()->object);
            putOpcode(CEE_LDSFLDA);
        } else {
            genObjectPtr(tree->asFIELD()->object, tree->asFIELD()->fwt.Field(), pslot);
            if (!ptrAddr && tree->asFIELD()->object->type->isPTRSYM()) {
                if (tree->asFIELD()->fwt.Field()->isVolatile)
                    putOpcode(CEE_VOLATILE);
                putOpcode(CEE_LDFLD);
            } else {
                ASSERT(tree->asFIELD()->fCheckedMarshalByRef);
                putOpcode(CEE_LDFLDA);
            }
        }
        emitFieldToken(tree->asFIELD()->fwt.Field(), tree->asFIELD()->fwt.Type());
        return;
    case EK_ARRINDEX:
        genExpr(tree->asBIN()->p1);
        genExpr(tree->asBIN()->p2);
        if ((type = tree->asBIN()->p1->type)->asARRAYSYM()->rank == 1) {
            if (fReadOnly && (type->asARRAYSYM()->elementType()->isTYVARSYM() || type->asARRAYSYM()->elementType()->IsRefType()))
                putOpcode(CEE_READONLY);
            putOpcode(CEE_LDELEMA);
            emitTypeToken(type->asARRAYSYM()->elementType()); 
        } else {
            genArrayCall(type->asARRAYSYM(), type->asARRAYSYM()->rank ? type->asARRAYSYM()->rank : getArgCount(tree->asBIN()->p2), ARRAYMETH_LOADADDR);
        }
        return;
    case EK_SEQUENCE:
        genSideEffects(tree->asBIN()->p1);
        tree = tree->asBIN()->p2;
        goto LRestart;
    case EK_SEQREV:
        genMemoryAddress(tree->asBIN()->p1, pslot, ptrAddr, fReadOnly);
        genSideEffects(tree->asBIN()->p2);
        return;
    default:
        type = tree->type;
        if (type->isPARAMMODSYM()) {
            type = type->asPARAMMODSYM()->paramType();
            ASSERT(tree->kind == EK_PROP);
        }
        genExpr(tree);
        *pslot = STOREINTEMP(type, TK_SHORTLIVED);
        genSlotAddress(*pslot);
        return;
    }
}


/***************************************************************************************************
    Generate a memory address that is safe to duplicate on the stack without concern for
    concurrency issues.
***************************************************************************************************/
void ILGENREC::GenDupSafeMemAddr(EXPR * tree, PSLOT * pslot)
{
    TYPESYM * type;

LRestart:
    switch (tree->kind) {
    case EK_WRAP:
    case EK_LDTMP:
    case EK_STTMP:
    case EK_LOCAL:
        genMemoryAddress(tree, pslot);
        return;
    case EK_SEQUENCE:
        genSideEffects(tree->asBIN()->p1);
        tree = tree->asBIN()->p2;
        goto LRestart;
    case EK_SEQREV:
        GenDupSafeMemAddr(tree->asBIN()->p1, pslot);
        genSideEffects(tree->asBIN()->p2);
        return;
    default:
        type = tree->type;
        if (type->isPARAMMODSYM()) {
            type = type->asPARAMMODSYM()->paramType();
            ASSERT(tree->kind == EK_PROP);
        }
        genExpr(tree);
        *pslot = STOREINTEMP(type, TK_SHORTLIVED);
        genSlotAddress(*pslot);
        return;
    }
}


// see notes at places where this is called as to why I've made it more lenient.
__forceinline bool ILGENREC::needsBoxing(PARENTSYM * parent, TYPESYM * object)
{
    ASSERT(object->isStructOrEnum());
    if (object->isNUBSYM())
        return !parent->isAGGSYM() || !parent->asAGGSYM()->isPredefAgg(PT_G_OPTIONAL);
    return (parent != object->getAggregate());
}


void ILGENREC::genObjectPtr(EXPR * object, SYM * member, PSLOT * pslot)
{
    ASSERT(object);
    ASSERT(!object->type->isTYVARSYM());
    ASSERT(!pslot || !*pslot);

    if (!object->type->isStructOrEnum()) {
        genExpr(object);
    }
    else if (needsBoxing(member->parent, object->type)) {
        genExpr(object);
        putOpcode(CEE_BOX);
        emitTypeToken(object->type);
    }
    else {
        genMemoryAddress(object, pslot);
    }
}


void ILGENREC::emitRefParam(EXPR * arg, PSLOT * curSlot)
{

    if (arg->kind == EK_LOCAL && !arg->asLOCAL()->local->slot.type) {
        curSlot[0] = allocTemporary(arg->type->parent->asTYPESYM(), TK_SHORTLIVED);
        initLocal(curSlot[0], arg->type->parent->asTYPESYM());
        genSlotAddress(curSlot[0]);
    } else {
        PSLOT slot = NULL;
        *curSlot = NULL;
        genMemoryAddress(arg, &slot, true);
        ASSERT(!slot);
    }
}


// Should we use the "call" or "callvirt" opcode? We use "callvirt" even on non-virtual
// instance methods (exception base calls or structs) to get a null check.
bool ILGENREC::callAsVirtual(METHSYM * meth, EXPR * object, bool isBaseCall)
{
    ASSERT(!meth->isStatic == !!object);
    if (meth->isStatic || isBaseCall || (object->flags & EXF_CANTBENULL) && !meth->isVirtual)
        return false;

    FUNDTYPE ft = object->type->fundType();
    return (ft == FT_REF || ft == FT_VAR);
}


// generate code for a function call
void ILGENREC::genCall(EXPRCALL * tree, bool valUsed)
{
    METHSYM * func = tree->mwi.Meth();
    AGGTYPESYM * aggType = tree->mwi.Type();

    ASSERT(aggType && aggType->getAggregate() == func->getClass());

#if USAGEHACK
    func->isUsed = true;
#endif

    EXPR * object = tree->object;
    
    bool isNewObjCall = !!(tree->flags & EXF_NEWOBJCALL);
    bool isBaseCall = !!(tree->flags & EXF_BASECALL);
    bool isStructAssgCall = !!(tree->flags & EXF_NEWSTRUCTASSG);
    bool isImplicitStructAssgCall = !!(tree->flags & EXF_IMPLICITSTRUCTASSG);
    bool hasRefParams = !!(tree->flags & EXF_HASREFPARAM);
    bool needsCopy = false;
    EXPRLOCAL * exprDumpAddr = NULL; 
    bool wasStructAssgCall = false;
    bool fConstrained = false;

    if (object && object->kind == EK_FIELD && object->asFIELD()->fwt.Field()->isReadOnly && !(object->flags & EXF_LVALUE)) {
        needsCopy = true;
    }

    if (func->isVarargs) {
        func = getVarArgMethod(func, tree->args)->asFMETHSYM();
        ASSERT(aggType && aggType->getAggregate() == func->getClass());
    }

    PSLOT slotObject = NULL;

    bool retIsVoid = tree->type == compiler()->getBSymmgr().GetVoid();
    if (func->isStatic || isNewObjCall) {
        ASSERT(!object);
    }
    else if (isStructAssgCall || isImplicitStructAssgCall)  {
        if (!isExprOptimizedAway(object)) {
            if (isImplicitStructAssgCall) {
                if (object->kind != EK_LOCAL || object->asLOCAL()->local->slot.aliasPossible) {
                    wasStructAssgCall = 1;
                    isStructAssgCall = 0;
                    isNewObjCall = 1;
                } else {
                    isStructAssgCall = 1;
                }
            }
            genObjectPtr(object, func, &slotObject);
            if (valUsed) {
                ASSERT(object);
                if (object->GetSeqVal()->kind == EK_LOCAL) {
                    // this is a problem... the verifier might complain that we are duping an
                    // uninitialized local's address
                    exprDumpAddr = object->GetSeqVal()->asLOCAL();
                } else {
                    putOpcode(CEE_DUP);
                }
            }

            retIsVoid = true;

        } else {
            // if this is an assignment to a local which is not used, then don't assign...
            object = NULL;
            isNewObjCall = 1;
            isStructAssgCall = 0;
        }
    }
    else if (tree->flags & EXF_CONSTRAINED) {
        // Use the constrained prefix
        ASSERT(object->kind == EK_CAST &&
            (object->asCAST()->p1->type->isTYVARSYM() ||
                object->asCAST()->p1->type->IsValType()));
        ASSERT(!isBaseCall);
        object = object->asCAST()->p1;
        genMemoryAddress(object, &slotObject, false, true); // Readonly for array element access
        fConstrained = true;
    }
    else {
        genObjectPtr(object, func, &slotObject);
    }

    int stackPrev = curStack;

    PSLOT * tempSlots = NULL;
    unsigned int slotCount = 0;

    if (hasRefParams) {
        // If the arguments being passed by ref are optimized away, we need temporary slots for them...
        PSLOT * curSlot = tempSlots = STACK_ALLOC(PSLOT, func->params->size);
    
        EXPRLOOP(tree->args, arg)
            if (arg->type->isPARAMMODSYM()) {
                emitRefParam(arg, curSlot);
            } else {
                genExpr(arg);
                *curSlot = NULL;
            }
            curSlot++;
        ENDLOOP;

        slotCount = (unsigned)(curSlot - tempSlots);

    } else {
        genExpr(tree->args);
    }

    int stackDiff;

    if (!func->isVarargs) {
        stackDiff = func->params->size;
    } else {
        stackDiff = curStack - stackPrev;
    }

    if (!func->isStatic && !isNewObjCall) stackDiff ++;  // adjust for this pointer

    ILCODE ilcode;
    if (fConstrained) {
        ASSERT(object->type->isTYVARSYM() || object->type->IsValType());
        putOpcode(CEE_CONSTRAINED);
        emitTypeToken(object->type);
        ilcode = CEE_CALLVIRT;
    }
    else if (isNewObjCall)
        ilcode = CEE_NEWOBJ;
    else if (callAsVirtual(func, object, isBaseCall))
        ilcode = CEE_CALLVIRT;
    else
        ilcode = CEE_CALL;

    putOpcode(ilcode);

    emitMethodToken(func, aggType, tree->mwi.TypeArgs());

    // eat the arguments off the stack
    curStack -= stackDiff;
    ASSERT(curStack >= 0);

    if (func->retType != compiler()->getBSymmgr().GetVoid()) {
        curStack++;
        markStackMax();
    }
    if (wasStructAssgCall) {
        putOpcode(CEE_STOBJ);
        emitTypeToken(object->type);
    }
    // The difference between the above check (of funcrettype) and here, 
    // (of tree type) is that call exprs have a type of non-void for new_obj calls
    // since they denote a value of a certain type, while the constructor itself has
    // a return type of void...
    if ((isStructAssgCall || wasStructAssgCall) && valUsed) {
        if (exprDumpAddr) {
            genSlotAddress(&exprDumpAddr->local->slot);
        }
        putOpcode(CEE_LDOBJ);
        emitTypeToken(object->type);
    } else if (!valUsed) {
        // so, if val is not used, and we had something, then we need to pop it...
        if (!retIsVoid) {
            putOpcode(CEE_POP);
        }
        else if (!wasStructAssgCall && !compiler()->options.m_fOPTIMIZATIONS &&
            (info == NULL || !info->noDebugInfo) && !(tree->flags & EXF_NODEBUGINFO)) {
            putOpcode(CEE_NOP);
        }
    }

    if (slotObject) {
        freeTemporary(slotObject);
    }

    if (tempSlots) {
        for (unsigned i = 0; i < slotCount; i++) {
            if (tempSlots[i]) {
                freeTemporary(tempSlots[i]);
            }
        }
    }
}


void ILGENREC::copyHandlers(COR_ILMETHOD_SECT_EH_CLAUSE_FAT * clauses)
{
    for (HANDLERINFO * hi = handlers;hi; hi = hi->next) {
        // is this protected block even reachable? 
        if (!hi->tryBegin) {          
            ehCount--;
            continue;
        }
        int nopspace = hi->handlerShouldIncludeNOP ? ILcodesSize[CEE_NOP] : 0;
        clauses->SetTryOffset(hi->tryBegin->startOffset);
        clauses->SetTryLength(hi->tryEnd->next->startOffset - hi->tryBegin->startOffset);
        clauses->SetHandlerOffset(hi->handBegin->startOffset - nopspace);
        clauses->SetHandlerLength(hi->handEnd->next->startOffset - hi->handBegin->startOffset + nopspace);
        if (hi->IsTryFinally()) {
            clauses->SetFlags((CorExceptionFlag) (COR_ILEXCEPTION_CLAUSE_FINALLY | COR_ILEXCEPTION_CLAUSE_OFFSETLEN));
            clauses->SetClassToken(NULL);
        } else if (hi->IsTryFault()) {
            clauses->SetFlags((CorExceptionFlag) (COR_ILEXCEPTION_CLAUSE_FAULT | COR_ILEXCEPTION_CLAUSE_OFFSETLEN));
            clauses->SetClassToken(NULL);
        } else {
            clauses->SetFlags(COR_ILEXCEPTION_CLAUSE_OFFSETLEN);
            clauses->SetClassToken(compiler()->emitter.GetTypeRef(hi->type, false));
        }

        clauses++;
    }
}


HANDLERINFO * ILGENREC::createHandler(BBLOCK * tryBegin, BBLOCK * tryEnd, BBLOCK * handBegin, TYPESYM * type)
{
    ehCount++;
    HANDLERINFO * handler = (HANDLERINFO*)allocator->Alloc(sizeof(HANDLERINFO));
    handler->tryBegin = tryBegin;
    handler->tryEnd = tryEnd;
    handler->handBegin = handBegin;
    handler->type = type;
    handler->next = NULL;
    handler->handlerShouldIncludeNOP = false;

    if (lastHandler) {
        lastHandler->next = handler;
        lastHandler = handler;
    } else {
        lastHandler = handlers = handler;
    }

    return handler;

}

void ILGENREC::genTry(EXPRTRY * tree)
{

    if (tree->flags & EXF_REMOVEFINALLY) {
        ASSERT(tree->flags & EXF_ISFINALLY);
        genBlock(tree->tryblock);
        genBlock(tree->handlers->asBLOCK());
        return;
    }

    // start : is top of the try block
    BBLOCK * tryBegin = startNewBB(NULL);
    if (tree->flags & EXF_ISFINALLY) {
        finallyNesting++;
        inlineBB.tryNesting = finallyNesting;
    }
    inlineBB.startsTry = true;
    genBlock(tree->tryblock);

    // end is after the end of the protected block
    BBLOCK * tryEnd;
    if (tree->flags & EXF_ISFINALLY) {

        BBLOCK * afterFinally = createNewBB();

        tryEnd = startNewBB(NULL); // this will point to the CEE_LEAVE...

        //inlineBB.mayRemove = true; // this is a synthetic goto, which may point beyond the end of code... 

        if (tree->IsFinallyBlocked()) {
            inlineBB.gotoBlocked = true; // this refers to the following leave
            blockedLeave++;
        }

        closeDebugInfo();  // close the info for the try block
        
        openDebugInfo(SpecialDebugPoint::HiddenCode, EXF_NODEBUGINFO);
        BBLOCK * handBegin = startNewBB(NULL, CEE_LEAVE, afterFinally);
        closeDebugInfo();
        // and this will point to the finally block following the CEE_LEAVE...

        inlineBB.startsCatchOrFinally = true;
        genBlock(tree->handlers->asBLOCK());

        HANDLERINFO * hand = createHandler(tryBegin, tryEnd, handBegin, tree->flags & EXF_ISFAULT ? (TYPESYM*)1 : NULL);
        
        hand->handEnd = startNewBB(NULL);
        // this points to the CEE_ENDFINALLY...
    
        openDebugInfo(SpecialDebugPoint::HiddenCode, EXF_NODEBUGINFO);
        inlineBB.endsFinally = true;
        putOpcode(CEE_ENDFINALLY);
        closeDebugInfo();
        startNewBB(afterFinally, CEE_ENDFINALLY);

        finallyNesting--;
    } else {

        // fallthrough is after the handler
        BBLOCK * fallThrough = createNewBB();

        tryEnd = startNewBB(NULL);
        // this points to the CEE_LEAVE

        //inlineBB.mayRemove = true; 

        // close the info for the try block
        closeDebugInfo();  

        openDebugInfo(SpecialDebugPoint::HiddenCode, EXF_NODEBUGINFO);
        startNewBB(NULL, CEE_LEAVE, fallThrough);
        closeDebugInfo();
        // Current is now after the CEE_LEAVE...

        STMTLOOP(tree->handlers, hand)
            EXPRHANDLER * catchHandler = hand->asHANDLER();
            BBLOCK * handBegin = currentBB;
            inlineBB.startsCatchOrFinally = true;

            curStack++;
            markStackMax();

            openDebugInfo(catchHandler->tree, 0);  // emit debug info for the catch.

            bool usedParam = catchHandler->param && catchHandler->param->slot.type;
            ASSERT(catchHandler->param == NULL || catchHandler->param->movedToField == NULL || usedParam);
            if (usedParam) {
                maybeEmitDebugLocalUsage(catchHandler->tree, catchHandler->param);
                if (catchHandler->param->type->isTYVARSYM()) {
                    putOpcode(CEE_UNBOX_ANY);
                    emitTypeToken(catchHandler->param->type);
                }
                dumpLocal(&(catchHandler->param->slot), true);
            } else {
                putOpcode(CEE_POP);
            }

            closeDebugInfo();  // finish debug info for the catch.

            genBlock(catchHandler->handlerBlock);

            HANDLERINFO * handler = createHandler(tryBegin, tryEnd, handBegin, catchHandler->type);
            handler->handEnd = startNewBB(NULL);
            // this will now point to the CEE_LEAVE...

            //inlineBB.mayRemove = true;  

            closeDebugInfo();  // finish debug info for the catch.

            openDebugInfo(SpecialDebugPoint::HiddenCode, EXF_NODEBUGINFO);
            startNewBB(NULL, CEE_LEAVE, fallThrough);
            closeDebugInfo();
        ENDLOOP;

        startNewBB(fallThrough);
    }
}


void ILGENREC::genThrow(EXPRTHROW * tree)
{
    if (tree->object) {
        genExpr(tree->object);
        putOpcode(CEE_THROW);
    } else {
        putOpcode(CEE_RETHROW);
    }
    startNewBB(NULL, cee_stop);
}

__forceinline unsigned ILGENREC::computeSwitchSize(BBLOCK * block)
{
    ASSERT(block->exitIL == CEE_SWITCH);

    return ILcodesSize[CEE_SWITCH] + sizeof(DWORD) + (sizeof(int) * block->switchDest->count);
}

// calculate the size of our code and set the correct bb offsets.
unsigned ILGENREC::getFinalCodeSize()
{

    if (returnLocation && !returnHandled) {
        startNewBB(returnLocation);
        if (closeIndexUsed) {
            openDebugInfo(SpecialDebugPoint::HiddenCode, EXF_NODEBUGINFO);
        } else {
            openDebugInfo(SpecialDebugPoint::CloseCurly, 0);
        }
        if (retTemp) {
            dumpLocal(retTemp, false);
            freeTemporary(retTemp);
        }
        putOpcode(CEE_RET);
        closeDebugInfo();
    } else if (retTemp) {
        freeTemporary(retTemp);
    }

    endBB(cee_stop, NULL);
    currentBB->next = NULL;

    optimizeGotos();

    size_t curoffset = 0;
    BBLOCK * curBB = firstBB;

    do {
        if (curBB->tryNesting == -1) curoffset += ILcodesSize[CEE_NOP];
        curBB->startOffset = (unsigned) curoffset;
        curBB->largeJump = 0;
        curoffset += curBB->curLen;
        switch(curBB->exitIL) {
        case CEE_RET:
        case cee_stop:
        case CEE_ENDFINALLY:
        case cee_next: 
            break;
        case CEE_SWITCH:
            curoffset += computeSwitchSize(curBB);
            break;
        case CEE_LEAVE:
            if (curBB->jumpDest->tryNesting == -1) {
                curBB->leaveNesting = -1;
            }
            // fallthrough
        default:
            ASSERT(curBB->exitIL < cee_last);
            curBB->largeJump = 1;
            // assume a large offset...
            curoffset += FetchAtIndex(ILcodesSize, curBB->exitIL) + 4;
        }
        curBB = curBB->next;
    } while (curBB);

    unsigned delta;
    do {
        curBB = firstBB;
        delta = 0;
        do {
            curBB->startOffset -= delta;
            if (curBB->largeJump) {
                int noopSpace;
                noopSpace = (curBB->leaveNesting == -1) ? ILcodesSize[CEE_NOP] : 0;
                int offset;
                ILCODE newOpcode = getShortOpcode(curBB->exitIL);
                if (curBB->jumpDest->startOffset > curBB->startOffset) {
                    // forward jump
                    offset = (int)(curBB->jumpDest->startOffset - curBB->startOffset -
                        delta - ILcodesSize[curBB->exitIL] - 4 - curBB->curLen - noopSpace);
                } else {
                    // backward jump
                    offset = (int)(curBB->jumpDest->startOffset - 
                        (curBB->startOffset + curBB->curLen + ILcodesSize[newOpcode] + 1 + noopSpace));

                }
                if (offset == (char) (offset & 0xff)) {
                    // this fits!!!
                    delta += (ILcodesSize[curBB->exitIL] - ILcodesSize[newOpcode]) + 3;
                    curBB->exitIL = newOpcode;
                    curBB->largeJump = false;
                }
            }
            curBB = curBB->next;
        } while (curBB);
        curoffset -= delta;
    } while (delta);

    #if DEBUG
        if (compiler()->GetRegDWORD("After")) {
            dumpAllBlocksContent(L"all done in get final code size");
        }
    #endif
    dumpAllBlocks(L"Final");

    return (unsigned)curoffset;
}

BYTE * ILGENREC::copySwitchInstruction(BYTE * outBuffer, BBLOCK * block)
{
    ASSERT(block->exitIL == CEE_SWITCH);

    putOpcode(&outBuffer, CEE_SWITCH);

    SET_UNALIGNED_VAL32(outBuffer, block->switchDest->count);
    outBuffer += sizeof(DWORD);

    unsigned instrSize = computeSwitchSize(block);

    for (unsigned i= 0; i < block->switchDest->count; i++) {
        SET_UNALIGNED_VAL32(outBuffer, computeJumpOffset(block, block->switchDest->blocks[i].dest, instrSize));
        outBuffer += sizeof(int);
    }

    return outBuffer;
}

int ILGENREC::computeJumpOffset(BBLOCK * from, BBLOCK * to, unsigned instrSize)
{
    int offset;
    if (to->startOffset > from->startOffset) {
        // forward jump
        offset = (int)(to->startOffset - from->startOffset - instrSize - from->curLen);
    } else {
        // backward jump
        offset = (int)(to->startOffset - (from->startOffset + from->curLen + instrSize));
    }
    return offset;
}

BYTE * ILGENREC::copyCode(BYTE * outBuffer)
{
    for (BBLOCK * curBB = firstBB;curBB;curBB = curBB->next) {
        ASSERT(curBB->reachable || !compiler()->options.m_fOPTIMIZATIONS || (curBB->curLen == 0 && (curBB->exitIL == cee_next || curBB->exitIL == cee_stop)));
        if (!curBB->reachable) continue;

        if (curBB->tryNesting == -1) {
            putOpcode(&outBuffer, CEE_NOP);
        }

        memcpy(outBuffer, curBB->code, curBB->curLen);
        outBuffer += curBB->curLen;

        switch (curBB->exitIL) {
        case CEE_RET:
        case cee_stop:
        case CEE_ENDFINALLY:
        case cee_next: 
            break;
        case CEE_SWITCH:
            outBuffer = copySwitchInstruction(outBuffer, curBB);
            break;
        default:
            ASSERT(curBB->exitIL < cee_last);
            putOpcode(&outBuffer, curBB->exitIL);
            unsigned opSize = FetchAtIndex(ILcodesSize, curBB->exitIL);
            unsigned addrSize = curBB->largeJump ? 4 : 1;
            int offset = computeJumpOffset(curBB, curBB->jumpDest, addrSize + opSize);
            if (curBB->leaveNesting == -1) {
                offset -= ILcodesSize[CEE_NOP];
            }
            if (curBB->largeJump) {
                ASSERT(offset != (char) (0xff & offset));
                SET_UNALIGNED_VAL32(outBuffer, offset);
                outBuffer += 4;
            } else {
                ASSERT(offset == (char) (0xff & offset));
                *(char*)outBuffer = (char) (0xff & offset);
                outBuffer ++;
            }
        }
    }

    return outBuffer;
}


// return the short ilcode form for a given jump instruction.
ILCODE ILGENREC::getShortOpcode(ILCODE longOpcode)
{

    switch (longOpcode) {

    case CEE_BEQ : longOpcode = CEE_BEQ_S; break;
    case CEE_BGE : longOpcode = CEE_BGE_S; break;
    case CEE_BGT : longOpcode = CEE_BGT_S; break;
    case CEE_BLE : longOpcode = CEE_BLE_S; break;
    case CEE_BLT : longOpcode = CEE_BLT_S; break;
    
    case CEE_BGE_UN : longOpcode = CEE_BGE_UN_S; break;
    case CEE_BGT_UN : longOpcode = CEE_BGT_UN_S; break;
    case CEE_BLE_UN : longOpcode = CEE_BLE_UN_S; break;
    case CEE_BLT_UN : longOpcode = CEE_BLT_UN_S; break;
    case CEE_BNE_UN : longOpcode = CEE_BNE_UN_S; break;

    case CEE_BRTRUE: longOpcode = CEE_BRTRUE_S; break;
    case CEE_BRFALSE: longOpcode = CEE_BRFALSE_S; break;
    case CEE_BR: longOpcode = CEE_BR_S; break;
    case CEE_LEAVE: longOpcode = CEE_LEAVE_S; break;
    default: ASSERT (!"bad jump opcode"); return CEE_ILLEGAL;
    }

    return longOpcode;
}

// mark a given block as visited, and reuturn true if this is the first time for that block
__forceinline bool ILGENREC::markAsVisited(BBLOCK * block)
{
    if (block->reachable) return false;

    block->reachable = true;

    return true;
}

// mark all reachable blocks starting from start using the provided marking function
void ILGENREC::markAllReachableBB(BBLOCK * start)
{
    MARKREACHABLEINFO marker( allocator);
    do {
        marker.MarkAllReachableBB(start);
        start = marker.Pop();
    } while (start != NULL);
}

MARKREACHABLEINFO::MARKREACHABLEINFO( NRHEAP * allocator)
{
    this->allocator = allocator;
    this->allocator->Mark( &markBase );
    stack = NULL;
    empty = NULL;
    stackIndex = lengthof(stack->bbItem);
}

MARKREACHABLEINFO::~MARKREACHABLEINFO()
{
    // Release our stack
    allocator->Free( &markBase );
}

void MARKREACHABLEINFO::Push(BBLOCK * block)
{
    if (stackIndex == lengthof(stack->bbItem)) {
        BBSTACK * next;
        if (empty != NULL) {
            // Re-use a block off of the empty list
            next = empty;
            empty = empty->next;
        } else {
            next = (BBSTACK*)allocator->Alloc(sizeof(BBSTACK));
        }
        next->next = stack;
        stack = next;
        stackIndex = 0;
    }

    stack->bbItem[stackIndex++] = block;
}

BBLOCK * MARKREACHABLEINFO::Pop()
{
    if (stack == NULL || (stackIndex == 0 && stack->next == NULL))
        return NULL;

    stackIndex--;
    if (stackIndex < 0) {
        BBSTACK * temp = stack->next;
        stack->next = empty;
        empty = stack;
        stack = temp;
        stackIndex = lengthof(stack->bbItem) - 1;
    }
    return stack->bbItem[stackIndex];
}

#if _MSC_FULL_VER < 140030615
#endif

// mark all reachable blocks starting from start using the provided marking function
void MARKREACHABLEINFO::MarkAllReachableBB(BBLOCK * start)
{

AGAIN:
    if (!ILGENREC::markAsVisited(start)) return;

    switch(start->exitIL) {
    case cee_stop:
    case CEE_ENDFINALLY:
    case CEE_RET: return;
    case cee_next:
        ASSERT(start->next);
        start = start->next;
        goto AGAIN;
    case CEE_LEAVE:
        if (start->gotoBlocked) {
            break;
        }
    case CEE_BR:
        ASSERT(start->jumpDest);
        start = start->jumpDest;
        goto AGAIN;
    case CEE_SWITCH:
        for (unsigned i = 0; i < start->switchDest->count; i++) {
            Push(start->switchDest->blocks[i].dest);
        }
        ASSERT(start->next);
        start = start->next;
        goto AGAIN;
    default:
        Push(start->jumpDest);
        ASSERT(start->next);
        start = start->next;
        goto AGAIN;
    }
}


void BBLOCK::FlipJump()
{
    if (exitILRev == CEE_ILLEGAL) {
        // Calculate it from exitIL.
        switch (exitIL) {
        case CEE_BRFALSE:   exitILRev = CEE_BRTRUE;     break;
        case CEE_BRTRUE:    exitILRev = CEE_BRFALSE;    break;
        case CEE_BEQ:       exitILRev = CEE_BNE_UN;     break;
        case CEE_BNE_UN:    exitILRev = CEE_BEQ;        break;
        case CEE_BLT:       exitILRev = CEE_BGE;        break;
        case CEE_BLE:       exitILRev = CEE_BGT;        break;
        case CEE_BGT:       exitILRev = CEE_BLE;        break;
        case CEE_BGE:       exitILRev = CEE_BLT;        break;
        case CEE_BLT_UN:    exitILRev = CEE_BGE_UN;     break;
        case CEE_BLE_UN:    exitILRev = CEE_BGT_UN;     break;
        case CEE_BGT_UN:    exitILRev = CEE_BLE_UN;     break;
        case CEE_BGE_UN:    exitILRev = CEE_BLT_UN;     break;
        default:
            ASSERT(!"bad jump");
            break;
        }
    }

    // Swap them.
    ILCODE code = exitIL;
    exitIL = exitILRev;
    exitILRev = code;
}


#if DEBUG

int ILGENREC::FindBlockInList(BBLOCK **list, int count, BBLOCK * block)
{
    for (int i = 0; i < count; i++) {
        if (list[i] == block)
            return i;
    }
    return -1;
}

ILCODE ILGENREC::findInstruction(BYTE b1, BYTE b2)
{
    ILCODE il = (ILCODE) 0;
    do {
        if (ILcodes[il].b1 == b1 && ILcodes[il].b2 == b2) return il;
        il = (ILCODE) (il + 1);
    } while(true);
}

void ILGENREC::dumpAllBlocks(PCWSTR label) {

    if (!shouldDumpAllBlocks()) return;

    wprintf(L"******** %s\n", label);

    wprintf(L"%s : %s\n", cls->name->text, method->name->text);


    int count = 0;
    BBLOCK *current;
    for (current = firstBB; current; current = current->next) {
        count++;
    }

    BBLOCK ** list = STACK_ALLOC(BBLOCK*, count);

    int i;
    for (i = 0, current = firstBB; current; current = current->next, i++) {
        list[i] = current;
    }

    for (i = 0, current = firstBB; current; current = current->next, i++) {
        bool tryEnd = false;
        bool handlerEnd = false;
        for (HANDLERINFO * hi = handlers; hi; hi = hi->next) {
            if (!hi->tryBegin) break;
            if (hi->tryBegin == current) {
                wprintf (L"TRY: ");
            }
            if (hi->tryEnd == current) {
                tryEnd = true;
            }
            if (hi->handBegin == current) {
                wprintf (L"HAND: ");
            }
            if (hi->handEnd == current) {
                handlerEnd = true;
            }
        }
        wprintf(L"Block %d : %p : ", FindBlockInList(list, count, current), current);
        if (current->exitIL == cee_next && current->curLen == 0) {
            wprintf(L"EMPTY \n");
        } else {
            wprintf(L"Size(%d) : ", current->curLen);
            bool doJump = true;
            switch (current->exitIL) {
            case cee_next:
                wprintf(L"cee_next");
                doJump = false;
                break;
            case cee_stop:
                wprintf(L"cee_stop");
                doJump = false;
                break;
            case CEE_SWITCH:
            case CEE_RET:
                doJump = false;
            default:
                wprintf(L"%s ", FetchAtIndex(ILnames, current->exitIL));
            }
            if (doJump) {
                wprintf(L" --> %d", FindBlockInList(list, count, current->jumpDest));
            }
            if (tryEnd) {
                wprintf(L" TRYEND");
            }
            if (handlerEnd) {
                wprintf(L" HANDEND");
            }
            wprintf(L"\n");
        }
    }
}

void ILGENREC::initDumpingAllBlocks()
{
    privShouldDumpAllBlocks = false;

    if (!cls->name || !method->name) {
        return;
    }

    if (!compiler()->IsRegString(L"*", L"BlockClass") && !compiler()->IsRegString(cls->name->text, L"BlockClass")) {
        return;
    }

    if (!compiler()->IsRegString(L"*", L"BlockMethod") && !compiler()->IsRegString(method->name->text, L"BlockMethod")) {
        return;
    }

    privShouldDumpAllBlocks = true;
}

void ILGENREC::dumpAllBlocksContent(PCWSTR label) {

    if (!shouldDumpAllBlocks()) return;

    wprintf(L"******** %s\n", label);
    wprintf(L"\n\n%s : %s\n", cls->name->text, method->name->text);


    unsigned count = 0;
    BBLOCK *current;
    for (current = firstBB; current; current = current->next) {
        count++;
    }

    BBLOCK ** list = STACK_ALLOC(BBLOCK*, count);

    unsigned i;
    for (i = 0, current = firstBB; current; current = current->next, i++) {
        list[i] = current;
    }

    DEBUGINFO * curDI = NULL;
    DWORD lastOff = (DWORD) -1;

    for (i = 0, current = firstBB; current; current = current->next, i++) {

        if (current->startOffset != (unsigned) -1 && current->startOffset != lastOff) {
            wprintf(L"[%d] ", current->startOffset);
            lastOff = current->startOffset;
        }

GETCDI:
        if (!curDI) {
            curDI = current->debugInfo;
            if (curDI) {
                while (curDI->prev) curDI = curDI->prev;
            }
        } else {
            if (curDI->endBlock == current) {
                if (curDI->endOffset == 0) {
                    curDI = NULL;
                    goto GETCDI;
                }
            }
        }

        wprintf(L"%d : ", i);
        BYTE *pb = current->code;
        size_t length = current->curLen;

        // If necessary for debugging in the future, we could dump entire instruction stream including typed instruction arguments
        if (length > 0) {
            BYTE b = pb[0];
            BYTE b2;
            if (b == 0xfe) {
                b2 = pb[1];
            } else {
                b2 = b;
                b = 0xff;
            }
            ILCODE ilcode = findInstruction(b, b2);
            pb += ILcodesSize[ilcode];
            length -= ILcodesSize[ilcode];
            wprintf(L"%s\n", ILnames[ilcode]);
        }
        if (current->curLen) {
            if (curDI) {
                if (curDI->extent.IsValid()) {
                    if (curDI->extent.fNoDebugInfo)
                        wprintf(L"[ no debug info ]\n");
                    else
                        wprintf(L"[%ls (%ld, %ld) - (%ld, %ld)]\n", curDI->extent.infile->name->text, curDI->extent.begin.iLine +1 ,
                            curDI->extent.begin.iChar +1 , curDI->extent.end.iLine+1 , curDI->extent.end.iChar+1);
                }
                else {
                    wprintf(L"[ invalid debug info ]\n");
                }
                
                if (curDI->endBlock == current && curDI->endOffset == current->curLen) {
                    curDI = curDI->next;
                    if (!curDI) {
                        curDI = current->debugInfo;
                    }
                }
            }
        }

        if (current->exitIL == cee_next && current->curLen == 0) {
            if (current->sym && current->sym != (SYM *)I64(0xCCCCCCCCCCCCCCCC)) {
                wprintf(L"%s", compiler()->ErrSym(current->sym));
            }
        } else {
            bool doJump = true;
            bool noToken = false;
            switch (current->exitIL) {
            case cee_next:
            case cee_stop:
            case CEE_RET:
                doJump = false;
                break;
            case CEE_SWITCH:
                doJump = false;
                noToken = true;
            default:
                wprintf(L"%s ", ILnames[current->exitIL]);
            }
            if (doJump) {
                wprintf(L" --> %d ", FindBlockInList(list, count, current->jumpDest));
                if (curDI) {
                    if (curDI->extent.IsValid()) {
                        if (curDI->extent.fNoDebugInfo)
                            wprintf(L"[ no debug info ]\n");
                        else
                            wprintf(L"[%ls (%ld, %ld) - (%ld, %ld)]\n", curDI->extent.infile->name->text, curDI->extent.begin.iLine +1 ,
                                curDI->extent.begin.iChar +1 , curDI->extent.end.iLine+1 , curDI->extent.end.iChar+1);
                    }
                    else {
                        wprintf(L"[ invalid debug info ]\n");
                    }
                } else {
                    wprintf(L"\n");
                }
            } else {
                if (!noToken && !doJump && current->sym && current->sym != (SYM *)I64(0xCCCCCCCCCCCCCCCC)) {
                    wprintf(L"%s\n", compiler()->ErrSym(current->sym));
                } else {
                    wprintf(L"\n");
                }
            }

        }

        if (curDI && curDI->endBlock == current) {
            curDI = NULL;
        }
    }

    // dump handlers
    for (HANDLERINFO * hi = handlers; hi; hi = hi->next) {
        wprintf(L"handler: start %d end %d hand %d last %d\n",
            FindBlockInList(list, count, hi->tryBegin),
            FindBlockInList(list, count, hi->tryEnd),
            FindBlockInList(list, count, hi->handBegin),
            FindBlockInList(list, count, hi->handEnd));
    }
}


#endif

void __fastcall ILGENREC::optimizeBranchesToNext()
{
    
    // branches to next block:
    for (BBLOCK * current = firstBB; current->next; current = current->next) {
        BBLOCK * target;
        switch(current->exitIL) {
        case CEE_RET:
        case cee_next:
        case CEE_ENDFINALLY:
        case CEE_SWITCH:
        case cee_stop:
        case CEE_LEAVE:
            break;
        default:
            target = current->next;
            do {
                if (current->jumpDest == target) {
                    if (current->exitIL != CEE_BR) {
                        //
                        // here we have a conditional branch with equal
                        // destination addresses
                        //
                        BYTE * offset = current->curLen + current->code;
                        // it's too late now to do any better :-( ...
                        putOpcode (&offset, CEE_POP);
                        if (current->exitIL != CEE_BRTRUE && current->exitIL != CEE_BRFALSE) {
                            putOpcode (&offset, CEE_POP);
                            current->curLen += ILcodesSize[CEE_POP];
                        }
                        current->curLen += ILcodesSize[CEE_POP];
                    }
                    current->exitIL = cee_next;
                    break;
                }
                if (target->isNOP()) {
                    target = target->next;
                } else {
                    break;
                }
            } while (true);
        }
    }

}

void __fastcall ILGENREC::optimizeBranchesOverBranches()
{

    BBLOCK *current;
    // cond branches over branches:
    // gotoif A, lab1             gotoif !a, lab2
    // goto lab2        --->      nop
    // lab1:                      lab1:
    for (current = firstBB; current->next; current = current->next) {
        BBLOCK * target;
        BBLOCK * next;
        switch(current->exitIL) {
        case CEE_RET:
        case cee_next:
        case cee_stop:
        case CEE_BR:
        case CEE_ENDFINALLY:
        case CEE_SWITCH:
        case CEE_LEAVE:
            break;
        default:

            next = current->next;
            while (next->isNOP()) next = next->next;
            if (next->exitIL == CEE_BR && !next->curLen) {
                target = next->next;
                if (target) {
                    while (target->isNOP()) target = target->next;
                    if (current->jumpDest == target) {
                        current->jumpDest = next->jumpDest;
                        // we wipe the branch:
                        next->exitIL = cee_next;
                        current->FlipJump();
                    }
                }
            }
        }
    }
}

inline int AdvanceToNonNOP(BBLOCK **ppb) {
    int rval = 0;
    while((*ppb)->isNOP()) {
        if (ppb[0]->next->startsTry) {
            rval = -1;
        }
        *ppb = (*ppb)->next;
    }
    return rval;
}

void __fastcall ILGENREC::optimizeBranchesToNOPs()
{
   BBLOCK *current;
   unsigned i;
 
   for (current = firstBB; current->next; current = current->next) {
        switch(current->exitIL) {
        case CEE_RET:
        case cee_next:
        case cee_stop:
        case CEE_ENDFINALLY:
            break;
        case CEE_SWITCH:
            for (i = 0; i < current->switchDest->count; i++) {
                if (AdvanceToNonNOP(&current->switchDest->blocks[i].dest)) {
                    current->switchDest->blocks[i].jumpIntoTry = true;
                }
            }
            break;
        default: 
            if (AdvanceToNonNOP(&current->jumpDest)) {
                current->jumpIntoTry = true;
            }
        }
   }

   // must advance handlers past NOPs as well
   // so that handlers don't point to bogusly unreachable blocks
   for (HANDLERINFO * hi = handlers; hi; hi = hi->next) {
      AdvanceToNonNOP(&hi->tryBegin);
      AdvanceToNonNOP(&hi->tryEnd);
      AdvanceToNonNOP(&hi->handBegin);
      AdvanceToNonNOP(&hi->handEnd);
   }
}


void ILGENREC::optimizeBranchesToBranches()
{
    BBLOCK * current;
    unsigned i;

    // branches to branches and branches to ret
    for (current = firstBB; current->next; current = current->next) {
        switch(current->exitIL) {
        case CEE_RET:
        case cee_next:
        case cee_stop:
        case CEE_ENDFINALLY:
            break;
        case CEE_SWITCH:
            // Need to examine if any of the cases go to a br, in which case they canbe
            // redirected furher...
            for (i = 0; i < current->switchDest->count; i++) {
AGAINSW:
                BBLOCK * targetBlock = current->switchDest->blocks[i].dest;
                if (!targetBlock->curLen) {
                    if (&(current->switchDest->blocks[i]) == targetBlock->markedWithSwitch) {
                        // protect against cycles > 1 in size
                        targetBlock->jumpDest = targetBlock;
                        continue;
                    }
                    if (targetBlock->exitIL == CEE_BR && targetBlock->jumpDest != targetBlock) {
                        if (!current->switchDest->blocks[i].jumpIntoTry) {
                            targetBlock->markedWithSwitch = &(current->switchDest->blocks[i]);
                            current->switchDest->blocks[i].jumpIntoTry = targetBlock->jumpIntoTry;
                            current->switchDest->blocks[i].dest = targetBlock->jumpDest;
                            goto AGAINSW;
                        }
                    }
                }
            }
            break;
        default:
            // We need to catch extended cycles.  we do this by writing current into the code field of the
            // destination if we suck it in.  since we only suck in empty blocks we don't overwrite any code
AGAINBR:
            if (!current->jumpDest->curLen) {
                if (current == current->jumpDest->markedWith) {
                    // we sucked this in already... which means that we hit a cycle, so we might as well
                    // emit a jump to ourselves...
                    current->jumpDest->jumpDest = current->jumpDest;
                    break;
                }
                if (current->jumpDest->exitIL == CEE_BR && current->jumpDest->jumpDest != current->jumpDest) {
                    if (!current->jumpIntoTry) {
                        if (current->jumpDest->jumpIntoTry) {
                            current->jumpIntoTry = true;
                        }
                        //  we suck in the destination, and mark it as sucked in
                        current->jumpDest->markedWith = current;
                        current->jumpDest = current->jumpDest->jumpDest;
                        goto AGAINBR;
                    }
                }
            }
            if (current->exitIL == CEE_BR && current->jumpDest->exitIL == CEE_RET && current->jumpDest->curLen == ILcodesSize[CEE_RET]) {
                current->exitIL = CEE_RET;
                BYTE * offset = current->curLen + current->code;
                putOpcode (&offset, CEE_RET);
                current->curLen += ILcodesSize[CEE_RET];
            }
        }
    }

    // INVARIANT: No br instruction has br as its immediate target
    dumpAllBlocks(L"Before opt br to next");

    optimizeBranchesToNext();
    dumpAllBlocks(L"After opt br to next");

}



// optimize branches

void ILGENREC::optimizeGotos()
{



#if DEBUG
    if (compiler()->GetRegDWORD("Before")) {
        dumpAllBlocksContent(L"Before");
    }
#endif

    BBLOCK * current;

    int order = 0;
    for (current = firstBB; current; current = current->next) {
        current->order = order++;
    }

    dumpAllBlocks(L"after order assigned");

    if (compileForEnc) {
        for (current = firstBB; current; current = current->next) {
            if (current->leaveTarget) {
                current->curLen += ILcodesSize[CEE_NOP];
                current->code -= ILcodesSize[CEE_NOP];
                BYTE * offset = current->code;
                putOpcode(&offset, CEE_NOP);
                if (TrackDebugInfo()) {
                    DEBUGINFO * nopDebugInfo; nopDebugInfo = (DEBUGINFO*) allocator->AllocZero(sizeof(DEBUGINFO));
                    nopDebugInfo->beginBlock = current;
                    nopDebugInfo->extent.SetHiddenInvalidSource();
                    nopDebugInfo->extent.ProhibitMerge();
                    nopDebugInfo->beginOffset = (unsigned short) 0;
                    nopDebugInfo->endBlock = current;
                    nopDebugInfo->endOffset = ILcodesSize[CEE_NOP];
                    nopDebugInfo->alreadyAdjusted = true;
                    DEBUGINFO * first = current->debugInfo;
                    while (first && first->prev) first = first->prev;
                    if (first) {
                        nopDebugInfo->next = first;
                        first->prev = nopDebugInfo;
                    } else {
                        current->debugInfo = nopDebugInfo;
                    }
                }
            }
        }
    }

    dumpAllBlocks(L"before br to nop");
    optimizeBranchesToNOPs();
    dumpAllBlocks(L"after br to nop");

    // INVARIANT:  all br instructions have actual targets that cannot be skipped.
    bool redoBranchesToBranches = false;
    if (compiler()->options.m_fOPTIMIZATIONS) {

REDOBRANCHESTOBRANCHES:
        redoBranchesToBranches = false;
        ASSERT(compiler()->options.m_fOPTIMIZATIONS);

        optimizeBranchesToBranches();
        // INVARIANT: No br instruction has an offset of 0.

    }


    for (current = firstBB; current; current = current->next) {
        current->reachable = false;
    }

    // mark all normally reachable blocks:
    markAllReachableBB(firstBB);

    // now, mark all blocks reachable from reachable exception handlers...

    dumpAllBlocks(L"after mark all reachable");

    unsigned unreachCountOld = ehCount;

EXCEPAGAIN:

    unsigned unreachable = 0;

    HANDLERINFO *hi;
    for (hi = handlers; hi; hi = hi->next) {
        bool reached = false;
        for (current = hi->tryBegin; current && current != hi->tryEnd->next; current = current->next) {
            if (current->reachable) {
                reached = true;
                if (!hi->handBegin->reachable) {        
                    markAllReachableBB(hi->handBegin);
                }
                break;
            }
        }
        if (!reached) {
            unreachable++;
        }
    }

    if (unreachable && (unreachable != unreachCountOld)) {
        unreachCountOld = unreachable;
        goto EXCEPAGAIN;
    }

    ASSERT(unreachable <= ehCount);

    bool redoTryFinally;
    
REDOTRYFINALLY:
    redoTryFinally = false;

    // now, remove handlers for empty bodies if optimizing
    if (compiler()->options.m_fOPTIMIZATIONS) {

        bool changed = false;
        
DOHANDLERS:

        if (changed) {
            redoTryFinally = true;

            changed = false;
        }

        for (hi = handlers; hi; hi = hi->next) {

            if (!hi->tryBegin) continue;

            BBLOCK * start = hi->tryBegin;
            while(start != hi->tryEnd && (start->isEmpty() || !start->reachable)) {
                start = start->next;
            }
            if (start == hi->tryEnd) {


                // ok, this try block is empty, so we have one of 2 cases, if we optimize:

                // case one: its a catch, so we transform the catch into a nop...
                // case 3: its a fault, so we also transform the handler to a nop...
                if (!hi->IsTryFinally()) {
                    changed = true;
                    // if it's a catch, then it will never be executed, so
                    // it's sufficient if we skip it entirely...
                    start = hi->handBegin;
                    while (start != hi->handEnd->next) {
                        start->makeEmpty();
                        start = start->next;
                    }

                    // also, eliminate the CEE_LEAVE at the end of the try block, as
                    // it is the same as a fallthrough now (since there is no catch for it to
                    // jump over...
                    ASSERT(hi->tryEnd->exitIL == CEE_LEAVE || hi->tryEnd->exitIL == CEE_BR || hi->tryEnd->exitIL == cee_next);
                    if (hi->tryEnd->exitIL == CEE_LEAVE) {
                        hi->tryEnd->exitIL = CEE_BR;
                        // since this enables more BR to BR:
                        redoBranchesToBranches = true;
                    }

                    hi->tryBegin = NULL;

                // case 2: its a finally:
                } else {
                    /*
                    // if it's a finally, then we convert it to a normal block, by
                    // removing the END_FINALLY instruction...
                    ASSERT(start->exitIL == CEE_LEAVE);
                    start->makeEmpty();

                    // remove the CEE_ENDFINALLY instruction:
                    hi->handEnd->makeEmpty();
                   

                    // in both cases, we need to make sure that we don't emit EIT information
                    // for this try:
                    hi->tryBegin = NULL;
                    */
                    // Saddly, we can do nothing here... the problem is that the CLR has this rule that an async
                    // thread abort exception is not allowed to abort a thread while its in a finally,
                    // so finalies are significant even if the try had no code...
                    // so, there is only one thing to do:
                    goto TRYHANDLER;
                }
            } else {
                // non empty try:

                // the trybody had code in it, buf if the finally or fault body has no code
                // we can optimize it away...
                if (!hi->IsTryCatch()) {
TRYHANDLER:
                    BBLOCK * start = hi->handBegin;
                    while (start != hi->handEnd && (start->isEmpty() || !start->reachable)) {
                        start = start->next;
                    }
                    if (start == hi->handEnd) {

                        // also, remove the CEE_LEAVE as well as the CEE_ENDFINALLY instrs
                        hi->handEnd->makeEmpty();

                        // BUT, before removing the leave, check that the leave doesn't jump over
                        // code, in which case it cannot be removed  (this is possible since leaves
                        // suck up branches.)  So, if there is code we are jumping over that we
                        // would now fall into, make the leave into a branch instead.
                        if (hi->tryEnd->reachable) {
                            if (hi->tryEnd->exitIL == CEE_LEAVE) {
                                start = hi->tryEnd->next;
                                while (start && (!start->reachable || start->isEmpty()) && start != hi->tryEnd->jumpDest) {
                                    start = start->next;
                                }
                                if (start == hi->tryEnd->jumpDest) {
                                    hi->tryEnd->makeEmpty();
                                } else {
                                    hi->tryEnd->exitIL = CEE_BR;
                                    // since this enables more BR to BR:
                                    redoBranchesToBranches = true;
                                }
                            } else {
                                ASSERT(hi->tryEnd->exitIL == cee_next);
                            }
                        } 

                
                        // no code, in finally block, so just mark it as unreachable and we
                        // will not emit EIT info for it
                        hi->tryBegin = NULL;

                        changed = true;

                    }
                }
            }
        }

        if (changed) goto DOHANDLERS;
        if (redoBranchesToBranches) goto REDOBRANCHESTOBRANCHES;

    } else {

        // if not optimizing, we will merely remove unreachable trys


        for (hi = handlers; hi; hi = hi->next) {

            if (!hi->handBegin->reachable) {
                hi->tryBegin = NULL; // this effectively removes the try...
            }
        }
    }

    dumpAllBlocks(L"before leave adjustment");

    // we need to insure that those leave's don't go into outer space...
    for (current = firstBB; current && blockedLeave; current = current->next) {
        if (current->gotoBlocked) {
            blockedLeave--;
            if (current->reachable) {
                BBLOCK * temp = current->jumpDest;
                while (temp && !temp->startsCatchOrFinally && !temp->endsFinally && (!temp->reachable || (!temp->curLen && (temp->exitIL == cee_next || temp->exitIL == cee_stop)))) {
                    temp = temp->next;
                }
                if (!temp || temp->startsCatchOrFinally || temp->endsFinally) {
                    // ok, this leave either points to lala land, or into a catch or finally (also illegal),
                    // or, crosses a finally boundary (some false positives here, buts that's ok)
                    // so we need to make it point to some sane piece of code, namely an infinite loop...

                    current->jumpDest->exitIL = CEE_BR;
                    current->jumpDest->curLen = 0;
                    current->jumpDest->jumpDest = current->jumpDest;
                    current->jumpDest->reachable = true;
                    current->jumpDest->debugInfo = NULL;
                }
            }
        }
    }

    // now, suck up all unreachable blocks from this list...
    // [Well, actually, all we do is wipe the code and the exit instruction...]
    for (current = firstBB; current; current = current->next) {
        if (!current->reachable) {
            current->makeEmpty();
        }
    }

    // if this were unreachable, it could have been wiped, so let's reset it just in case
    if (currentBB->exitIL == cee_next) {
        currentBB->exitIL = cee_stop;
    }
    dumpAllBlocks(L"after unreach opts");

    if (compiler()->options.m_fOPTIMIZATIONS) {
        // now that we no longer have unreachable code, optimize branches over branches...
        optimizeBranchesOverBranches();
        dumpAllBlocks(L"after br over br");

        // Now, we might have enabled more branches to next which used to be
        // branches over dead code... so let's do that

        optimizeBranchesToNext();

        dumpAllBlocks(L"after br to next");
        // at this point there are no other opts to perform...
        // since branchesToNext removes branches, there are no new cases of
        // branchesToBranches or branchesToRet which can be optimized...

        if (redoTryFinally) goto REDOTRYFINALLY;

    }

    // Now, scan for leaves which needs NOPS instrted to properle leave the try
    // (leaves to first instruction of try)

    for (current = firstBB; current; current = current->next) {
        if (current->reachable && current->exitIL == CEE_LEAVE && current->leaveNesting > 0) {
            if (current->jumpDest->tryNesting > 0 && current->leaveNesting >= current->jumpDest->leaveNesting) {
                if (current->jumpDest->tryNesting == current->leaveNesting) {
                    // if they are equal we want to eliminate the case where the leave just
                    // goes to a following block, in which case the nop is not necessary
                    if (current->jumpDest->order > current->order) {
                        goto NEXTLEAVE;
                    }
                }
                BBLOCK * target = current->jumpDest;
                while (target->next && target->isNOP()) {
                    target = target->next;
                }
                // This might leave the nop in no-man's land (after the try, but before the handler)
                // That situation can occur if we advance the handlers to the same BB and 
                // set the nop before that point.  Let's detect this situation and fix it.
                for (HANDLERINFO * hi = handlers; hi; hi = hi->next) {
                    if (hi->tryBegin && hi->tryEnd && hi->handBegin && hi->handEnd) {
                        if (target->order > hi->tryEnd->order && target->order <= hi->handBegin->order) {
                            // This is the rare but bad situation...
                            hi->handlerShouldIncludeNOP = true;
                        }
                    }
                }
                target->tryNesting = -1;
            }
        }
NEXTLEAVE:;
    }

    // useful peephole opt:  sequences such as CEE_RET CEE_RET are equivalent to CEE_RET
    if (compiler()->options.m_fOPTIMIZATIONS) {
        for (current = firstBB; current; current = current->next) {
            if (current->exitIL == CEE_RET) {
                BBLOCK *next = current->next;
                while (next && next->isNOP()) {
                    next = next->next;
                }
                if (next && next->exitIL == CEE_RET && next->curLen == ILcodesSize[CEE_RET]) {
                    current->exitIL = cee_next;
                    current->curLen -= ILcodesSize[CEE_RET];
                }
            }
        }
    }



}

// array of instruction sizes:
const BYTE ILGENREC::ILcodesSize [] = {
#define OPDEF(id, name, pop, push, operand, type, len, b1, b2, cf) len ,
#include "opcode.def"
#undef OPDEF
};

#if DEBUG
const PWSTR ILGENREC::ILnames[] = {
#define OPDEF(id, name, pop, push, operand, type, len, b1, b2, cf) L ## name,
#include "opcode.def"
#undef OPDEF
};
#endif

const ILCODE ILGENREC::ILaddrLoad [2][2] =
{
    { CEE_LDLOCA_S, CEE_LDLOCA, },
    { CEE_LDARGA_S, CEE_LDARGA, },
};

// array of load store instructions.  if an inline version is not present
// we indicate this with CEE_ILLEGAL which means that we have to use the
// _S version
const ILCODE ILGENREC::ILlsTiny [4][6] =
{
    { CEE_LDLOC_0, CEE_LDLOC_1, CEE_LDLOC_2, CEE_LDLOC_3, CEE_LDLOC_S, CEE_LDLOC, },
    { CEE_STLOC_0, CEE_STLOC_1, CEE_STLOC_2, CEE_STLOC_3, CEE_STLOC_S, CEE_STLOC, },
    { CEE_LDARG_0, CEE_LDARG_1, CEE_LDARG_2, CEE_LDARG_3, CEE_LDARG_S, CEE_LDARG, },
    { CEE_ILLEGAL, CEE_ILLEGAL, CEE_ILLEGAL, CEE_ILLEGAL, CEE_STARG_S, CEE_STARG, },
};

const ILCODE ILGENREC::ILarithInstr[EK_ARRLEN - EK_ADD + 1] = {
    CEE_ADD,    // EK_ADD
    CEE_SUB,    // EK_SUB
    CEE_MUL,    // EK_MUL
    CEE_DIV,    // EK_DIV
    CEE_REM,    // EK_MOD
    CEE_NEG,    // EK_NEG
    CEE_ILLEGAL,// EK_UPLUS
    
    CEE_AND,    // EK_BITAND
    CEE_OR,     // EK_BITOR
    CEE_XOR,    // EK_BITXOR
    CEE_NOT,    // EK_BITNOT

    CEE_SHL,    // EK_LSHIFT
    CEE_SHR,    // EK_RSHIFT
    CEE_LDLEN,  // EK_ARRLEN
};

const ILCODE ILGENREC::ILarithInstrUN[EK_ARRLEN - EK_ADD + 1] = {
    CEE_ADD,    // EK_ADD
    CEE_SUB,    // EK_SUB
    CEE_MUL,    // EK_MUL
    CEE_DIV_UN, // EK_DIV
    CEE_REM_UN, // EK_MOD
    CEE_NEG,    // EK_NEG
    CEE_ILLEGAL,// EK_UPLUS
    
    CEE_AND,    // EK_BITAND
    CEE_OR,     // EK_BITOR
    CEE_XOR,    // EK_BITXOR
    CEE_NOT,    // EK_BITNOT

    CEE_SHL,    // EK_LSHIFT
    CEE_SHR_UN, // EK_RSHIFT
    CEE_LDLEN,  // EK_ARRLEN
};

const ILCODE ILGENREC::ILarithInstrOvf[EK_ARRLEN - EK_ADD + 1] = {
    CEE_ADD_OVF,// EK_ADD
    CEE_SUB_OVF,// EK_SUB
    CEE_MUL_OVF,// EK_MUL
    CEE_DIV,    // EK_DIV
    CEE_REM,    // EK_MOD
    CEE_NEG,    // EK_NEG
    CEE_ILLEGAL,// EK_UPLUS

    CEE_AND,    // EK_BITAND
    CEE_OR,     // EK_BITOR
    CEE_XOR,    // EK_BITXOR
    CEE_NOT,    // EK_BITNOT

    CEE_SHL,    // EK_LSHIFT
    CEE_SHR,    // EK_RSHIFT
    CEE_LDLEN,  // EK_ARRLEN
};

const ILCODE ILGENREC::ILarithInstrUNOvf[EK_ARRLEN - EK_ADD + 1] = {
    CEE_ADD_OVF_UN,// EK_ADD
    CEE_SUB_OVF_UN,// EK_SUB
    CEE_MUL_OVF_UN,// EK_MUL
    CEE_DIV_UN, // EK_DIV
    CEE_REM_UN, // EK_MOD
    CEE_NEG,    // EK_NEG
    CEE_ILLEGAL,// EK_UPLUS
    
    CEE_AND,    // EK_BITAND
    CEE_OR,     // EK_BITOR
    CEE_XOR,    // EK_BITXOR
    CEE_NOT,    // EK_BITNOT

    CEE_SHL,    // EK_LSHIFT
    CEE_SHR_UN, // EK_RSHIFT
    CEE_LDLEN,  // EK_ARRLEN
};

const ILCODE ILGENREC::ILstackLoad[FT_COUNT] = {
CEE_ILLEGAL,  //    FT_NONE,        // No fundemental type
CEE_LDIND_I1, //    FT_I1,
CEE_LDIND_I2, //    FT_I2,
CEE_LDIND_I4, //    FT_I4,
CEE_LDIND_U1, //    FT_U1,
CEE_LDIND_U2, //    FT_U2,
CEE_LDIND_U4, //    FT_U4,
CEE_LDIND_I8, //    FT_I8,
CEE_LDIND_I8, //    FT_U8,          // integral types
CEE_LDIND_R4, //    FT_R4,
CEE_LDIND_R8, //    FT_R8,          // floating types
CEE_LDIND_REF,//    FT_REF,         // reference type
CEE_LDOBJ  ,  //    FT_STRUCT,      // structure type
CEE_LDIND_I,  //    FT_PTR
CEE_LDOBJ,    //    FT_VAR          // type variable
};


const ILCODE ILGENREC::ILstackStore[FT_COUNT] = {
CEE_ILLEGAL,   //    FT_NONE,        // No fundemental type
CEE_STIND_I1, //    FT_I1,
CEE_STIND_I2, //    FT_I2,
CEE_STIND_I4, //    FT_I4,
CEE_STIND_I1, //    FT_U1,
CEE_STIND_I2, //    FT_U2,
CEE_STIND_I4, //    FT_U4,
CEE_STIND_I8, //    FT_I8,
CEE_STIND_I8, //    FT_U8,          // integral types
CEE_STIND_R4, //    FT_R4,
CEE_STIND_R8, //    FT_R8,          // floating types
CEE_STIND_REF,//    FT_REF,         // reference type
CEE_STOBJ  ,  //    FT_STRUCT,      // structure type
CEE_STIND_I,
CEE_STOBJ,    //    FT_VAR,         // type variable
};

const ILCODE ILGENREC::ILarrayLoad[FT_COUNT] = {
CEE_ILLEGAL,   //    FT_NONE,        // No fundemental type
CEE_LDELEM_I1, //    FT_I1,
CEE_LDELEM_I2, //    FT_I2,
CEE_LDELEM_I4, //    FT_I4,
CEE_LDELEM_U1, //    FT_U1,
CEE_LDELEM_U2, //    FT_U2,
CEE_LDELEM_U4, //    FT_U4,
CEE_LDELEM_I8, //    FT_I8,
CEE_LDELEM_I8, //    FT_U8,          // integral types
CEE_LDELEM_R4, //    FT_R4,
CEE_LDELEM_R8, //    FT_R8,          // floating types
CEE_LDELEM_REF,//    FT_REF,         // reference type
CEE_ILLEGAL,   //    FT_STRUCT,      // structure type
CEE_LDELEM_I,  //    FT_PTR
CEE_LDELEM     //    FT_VAR,         // generics
};

const ILCODE ILGENREC::ILarrayStore[FT_COUNT] = {
CEE_ILLEGAL,   //    FT_NONE,        // No fundemental type
CEE_STELEM_I1, //    FT_I1,
CEE_STELEM_I2, //    FT_I2,
CEE_STELEM_I4, //    FT_I4,
CEE_STELEM_I1, //    FT_U1,
CEE_STELEM_I2, //    FT_U2,
CEE_STELEM_I4, //    FT_U4,
CEE_STELEM_I8, //    FT_I8,
CEE_STELEM_I8, //    FT_U8,          // integral types
CEE_STELEM_R4, //    FT_R4,
CEE_STELEM_R8, //    FT_R8,          // floating types
CEE_STELEM_REF,//    FT_REF,         // reference type
CEE_ILLEGAL,   //    FT_STRUCT,      // structure type
CEE_STELEM_I,  //    FT_PTR  // FIXED BUG!!!!
CEE_STELEM     //    FT_VAR,         // generics
};


#define NOP cee_next
#define U1 CEE_CONV_U1
#define U2 CEE_CONV_U2
#define U4 CEE_CONV_U4
#define U8 CEE_CONV_U8
#define I1 CEE_CONV_I1
#define I2 CEE_CONV_I2
#define I4 CEE_CONV_I4
#define I8 CEE_CONV_I8
#define R4 CEE_CONV_R4
#define R8 CEE_CONV_R8
#define UR CEE_CONV_R_UN
#define ILL CEE_ILLEGAL
#define U CEE_CONV_U
#define I CEE_CONV_I

const ILCODE ILGENREC::simpleTypeConversions[NUM_EXT_TYPES][NUM_EXT_TYPES] = {
//        to: BYTE  I2    I4    I8    FLT   DBL     DEC     CHAR    BOOL   SBYTE   U2     U4     U8     I    U
/* from */
/* BYTE */ {  NOP  ,NOP  ,NOP  ,U8   ,R4   ,R8     ,ILL    ,NOP    ,ILL   ,I1     ,NOP   ,NOP   ,U8,   U  ,  U  },
/*   I2 */ {  U1   ,NOP  ,NOP  ,I8   ,R4   ,R8     ,ILL    ,U2     ,ILL   ,I1     ,U2    ,NOP   ,I8,   I  ,  I  },
/*   I4 */ {  U1   ,I2   ,NOP  ,I8   ,R4   ,R8     ,ILL    ,U2     ,ILL   ,I1     ,U2    ,NOP   ,I8,   I  ,  I  },
/*   I8 */ {  U1   ,I2   ,I4   ,NOP  ,R4   ,R8     ,ILL    ,U2     ,ILL   ,I1     ,U2    ,U4    ,NOP,  I  ,  U  },
/*  FLT */ {  U1   ,I2   ,I4   ,I8   ,R4   ,R8     ,ILL    ,U2     ,ILL   ,I1     ,U2    ,U4    ,U8,   I  ,  U  },
/*  DBL */ {  U1   ,I2   ,I4   ,I8   ,R4   ,R8     ,ILL    ,U2     ,ILL   ,I1     ,U2    ,U4    ,U8,   I  ,  U  },
/*  DEC */ {  ILL  ,ILL  ,ILL  ,ILL  ,ILL  ,ILL    ,ILL    ,ILL    ,ILL   ,ILL    ,ILL   ,ILL   ,ILL,  ILL,  ILL},
/* CHAR */ {  U1   ,I2   ,NOP  ,U8   ,R4   ,R8     ,ILL    ,NOP    ,ILL   ,I1     ,NOP   ,NOP   ,U8,   U  ,  U  },
/* BOOL */ {  ILL  ,ILL  ,ILL  ,ILL  ,ILL  ,ILL    ,ILL    ,ILL    ,ILL   ,ILL    ,ILL   ,ILL   ,ILL,  ILL,  ILL},
/*SBYTE */ {  U1   ,NOP  ,NOP  ,I8   ,R4   ,R8     ,ILL    ,U2     ,ILL   ,NOP    ,U2    ,NOP   ,I8,   I  ,  I  },
/*   U2 */ {  U1   ,I2   ,NOP  ,U8   ,R4   ,R8     ,ILL    ,NOP    ,ILL   ,I1     ,NOP   ,NOP   ,U8,   U  ,  U  },
/*   U4 */ {  U1   ,I2   ,NOP  ,U8   ,UR   ,UR     ,ILL    ,U2     ,ILL   ,I1     ,U2    ,NOP   ,U8,   U  ,  U  },
/*   U8 */ {  U1   ,I2   ,I4   ,NOP  ,UR   ,UR     ,ILL    ,U2     ,ILL   ,I1     ,U2    ,U4    ,NOP,  I,    U  },
/*    I */ {  U1   ,I2   ,I4   ,I8   ,R4   ,R8     ,ILL    ,U2     ,ILL   ,I1     ,U2    ,U4    ,I8,   NOP,  NOP},
/*    U */ {  U1   ,I2   ,I4   ,U8   ,UR   ,UR     ,ILL    ,U2     ,ILL   ,I1     ,U2    ,U4    ,U8,   NOP,  NOP}
};

const ILCODE ILGENREC::simpleTypeConversionsEx[NUM_EXT_TYPES][NUM_EXT_TYPES] = {
//        to: BYTE    I2      I4      I8      FLT     DBL     DEC     CHAR    BOOL     SBYTE     U2     U4     U8     I       U
/* from */
/* BYTE */ {  ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL     ,ILL      ,ILL   ,ILL   ,ILL,   ILL   ,ILL  },
/*   I2 */ {  ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL     ,ILL      ,ILL   ,ILL   ,ILL,   ILL   ,ILL  },
/*   I4 */ {  ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL     ,ILL      ,ILL   ,ILL   ,ILL,   ILL   ,ILL  },
/*   I8 */ {  ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL     ,ILL      ,ILL   ,ILL   ,ILL,   ILL   ,ILL  },
/*  FLT */ {  ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL     ,ILL      ,ILL   ,ILL   ,ILL,   ILL   ,ILL  },
/*  DBL */ {  ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL     ,ILL      ,ILL   ,ILL   ,ILL,   ILL   ,ILL  },
/*  DEC */ {  ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL     ,ILL      ,ILL   ,ILL   ,ILL,   ILL   ,ILL  },
/* CHAR */ {  ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL     ,ILL      ,ILL   ,ILL   ,ILL,   ILL   ,ILL  },
/* BOOL */ {  ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL     ,ILL      ,ILL   ,ILL   ,ILL,   ILL   ,ILL  },
/*SBYTE */ {  ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL     ,ILL      ,ILL   ,ILL   ,ILL,   ILL   ,ILL  },
/*   U2 */ {  ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL     ,ILL      ,ILL   ,ILL   ,ILL,   ILL   ,ILL  },
/*   U4 */ {  ILL    ,ILL    ,ILL    ,ILL    ,R4     ,R8     ,ILL    ,ILL    ,ILL     ,ILL      ,ILL   ,ILL   ,ILL,   ILL   ,ILL  },
/*   U8 */ {  ILL    ,ILL    ,ILL    ,ILL    ,R4     ,R8     ,ILL    ,ILL    ,ILL     ,ILL      ,ILL   ,ILL   ,ILL,   ILL   ,ILL  },
/*    I */ {  ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL    ,ILL     ,ILL      ,ILL   ,ILL   ,ILL,   ILL   ,ILL  },
/*    U */ {  ILL    ,ILL    ,ILL    ,ILL    ,R4     ,R8     ,ILL    ,ILL    ,ILL     ,ILL      ,ILL   ,ILL   ,ILL,   ILL   ,ILL  }
};

#undef NOP
#undef U1
#undef U2
#undef U4
#undef U8
#undef I1
#undef I2
#undef I4
#undef I8
#undef R4
#undef R8
#undef UR
#undef ILL
#undef U
#undef I

#define NOP cee_next
#define U1  CEE_CONV_OVF_U1 // signed to U?
#define U2  CEE_CONV_OVF_U2
#define U4  CEE_CONV_OVF_U4
#define U8  CEE_CONV_OVF_U8
#define U CEE_CONV_OVF_U
#define U1U CEE_CONV_OVF_U1_UN // unsigned to U?
#define U2U CEE_CONV_OVF_U2_UN
#define U4U CEE_CONV_OVF_U4_UN
#define U8U CEE_CONV_OVF_U8_UN
#define UU CEE_CONV_OVF_U_UN
#define I1  CEE_CONV_OVF_I1 // signed to I?
#define I2  CEE_CONV_OVF_I2
#define I4  CEE_CONV_OVF_I4
#define I8  CEE_CONV_OVF_I8
#define I CEE_CONV_OVF_I
#define I1U CEE_CONV_OVF_I1_UN // unsigned to I?
#define I2U CEE_CONV_OVF_I2_UN
#define I4U CEE_CONV_OVF_I4_UN
#define I8U CEE_CONV_OVF_I8_UN
#define IU CEE_CONV_OVF_I_UN
#define U8N CEE_CONV_U8   // no overflow check
#define I8N CEE_CONV_I8 
#define _IN CEE_CONV_I // IN is already defined to something...
#define UN CEE_CONV_U
#define R4 CEE_CONV_R4
#define R8 CEE_CONV_R8
#define UR CEE_CONV_R_UN
#define ILL CEE_ILLEGAL

const ILCODE ILGENREC::simpleTypeConversionsOvf[NUM_EXT_TYPES][NUM_EXT_TYPES] = {
//        to: BYTE  I2    I4    I8    FLT   DBL   DEC   CHAR  BOOL   SBYTE   U2   U4   U8       I      U
/* from */
/* BYTE */ {  NOP  ,NOP  ,NOP  ,U8N  ,R4   ,R8   ,ILL  ,NOP  ,ILL   ,I1U    ,NOP ,NOP ,U8N   ,  UN  ,  UN },
/*   I2 */ {  U1   ,NOP  ,NOP  ,I8N  ,R4   ,R8   ,ILL  ,U2   ,ILL   ,I1     ,U2  ,U4  ,U8    , _IN  ,  U  },
/*   I4 */ {  U1   ,I2   ,NOP  ,I8N  ,R4   ,R8   ,ILL  ,U2   ,ILL   ,I1     ,U2  ,U4  ,U8    , _IN  ,  U  },
/*   I8 */ {  U1   ,I2   ,I4   ,NOP  ,R4   ,R8   ,ILL  ,U2   ,ILL   ,I1     ,U2  ,U4  ,U8    ,  I   ,  U  },
/*  FLT */ {  U1   ,I2   ,I4   ,I8   ,R4   ,R8   ,ILL  ,U2   ,ILL   ,I1     ,U2  ,U4  ,U8    ,  I   ,  U  },
/*  DBL */ {  U1   ,I2   ,I4   ,I8   ,R4   ,R8   ,ILL  ,U2   ,ILL   ,I1     ,U2  ,U4  ,U8    ,  I   ,  U  },
/*  DEC */ {  ILL  ,ILL  ,ILL  ,ILL  ,ILL  ,ILL  ,ILL  ,ILL  ,ILL   ,ILL    ,ILL ,ILL ,ILL   ,  ILL ,  ILL},
/* CHAR */ {  U1U  ,I2U  ,NOP  ,I8N  ,R4   ,R8   ,ILL  ,NOP  ,ILL   ,I1U    ,NOP ,NOP ,U8N   ,  UN  ,  UN },
/* BOOL */ {  ILL  ,ILL  ,ILL  ,ILL  ,ILL  ,ILL  ,ILL  ,ILL  ,ILL   ,ILL    ,ILL ,ILL ,ILL   ,  ILL ,  ILL},
/*SBYTE */ {  U1   ,NOP  ,NOP  ,I8N  ,R4   ,R8   ,ILL  ,U2   ,ILL   ,NOP    ,U2  ,U4  ,U8    , _IN  ,  U  },
/*   U2 */ {  U1U  ,I2U  ,NOP  ,I8N  ,R4   ,R8   ,ILL  ,NOP  ,ILL   ,I1U    ,NOP ,NOP ,U8N   ,  UN  ,  UN },
/*   U4 */ {  U1U  ,I2U  ,I4U  ,I8U  ,UR   ,UR   ,ILL  ,U2U  ,ILL   ,I1U    ,U2U ,NOP ,U8N   ,  IU  ,  UN },
/*   U8 */ {  U1U  ,I2U  ,I4U  ,I8U  ,UR   ,UR   ,ILL  ,U2U  ,ILL   ,I1U    ,U2U ,U4U ,NOP   ,  IU  ,  UU },
/*    I */ {  U1   ,I2   ,I4   ,I8N  ,R4   ,R8   ,ILL  ,U2   ,ILL   ,I1     ,U2  ,U4  ,U8    ,  NOP ,  U  },
/*    U */ {  U1U  ,I2U  ,I4U  ,I8U  ,UR   ,UR   ,ILL  ,U2U  ,ILL   ,I1U    ,U2U ,U4U ,U8N   ,  IU  ,  NOP}
};

#undef NOP
#undef U1
#undef U2
#undef U4
#undef U8
#undef U1U
#undef U2U
#undef U4U
#undef U8U
#undef I1
#undef I2
#undef I4
#undef I8
#undef I1U
#undef I2U
#undef I4U
#undef I8U
#undef R4
#undef R8
#undef UR
#undef IN
#undef UN
#undef I
#undef U
#undef IU
#undef UU
#undef ILL


// stack behaviour of operations
const int ILGENREC::ILStackOps [] = {
#define Pop0 0
#define Pop1 1
#define PopI 1
#define PopR4 1
#define PopR8 1
#define PopI8 1
#define PopRef 1
#define VarPop 0
#define Push0 0
#define Push1 1
#define PushI 1
#define PushR4 1
#define PushR8 1
#define PushI8 1
#define PushRef 1
#define VarPush 0
#define OPDEF(id, name, pop, push, operand, type, len, b1, b2, cf) (push) - (pop) ,
#include "opcode.def"
#undef Pop0
#undef Pop1
#undef PopI
#undef PopR4
#undef PopR8
#undef PopI8
#undef PopRef
#undef VarPop
#undef Push0
#undef Push1
#undef PushI
#undef PushR4
#undef PushR8
#undef PushI8
#undef PushRef
#undef VarPush
#undef OPDEF
};
