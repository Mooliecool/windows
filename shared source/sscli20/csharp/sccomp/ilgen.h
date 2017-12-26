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
// File: ilgen.h
//
// Defines record used to generate il for a method
// ===========================================================================

#ifndef __ilgen_h__
#define __ilgen_h__

/////////////////////////////////////////////////////////////////////////////////////////

enum ILCODE {
#define OPDEF(id, name, pop, push, operand, type, len, b1, b2, cf) id,
#include "opcode.def"
#undef OPDEF
    cee_last,
    cee_next,
    cee_stop,
};

/////////////////////////////////////////////////////////////////////////////////////////

struct REFENCODING {
    BYTE b1;
    BYTE b2;
};

/////////////////////////////////////////////////////////////////////////////////////////

struct SourceExtent {
    INFILESYM * infile;
    POSDATA begin;
    POSDATA end;
    bool    fNoDebugInfo;
    bool fProhibitMerge;

    SourceExtent() { SetInvalid(); }
    bool IsValid() { return fNoDebugInfo || (infile != NULL); }
    bool IsValidSource() { return infile != NULL; }
    bool IsMergeAllowed() { return !fProhibitMerge; }
    void SetInvalid() { infile = NULL; begin.SetUninitialized(); end.SetUninitialized(); fNoDebugInfo = false; fProhibitMerge = false; }
    void SetHiddenInvalidSource() { infile = NULL; begin.SetUninitialized(); end.SetUninitialized(); fNoDebugInfo = true; }
    void ProhibitMerge() { fProhibitMerge = true; }
};

inline bool operator ==(const SourceExtent & extent1, const SourceExtent & extent2) {
    if (extent1.fNoDebugInfo && extent2.fNoDebugInfo)
        return true;
    return extent1.infile == extent2.infile && extent1.begin == extent2.begin &&
        extent1.end == extent2.end && extent1.fNoDebugInfo == extent2.fNoDebugInfo;
}

/////////////////////////////////////////////////////////////////////////////////////////

struct DEBUGINFO {
    SourceExtent extent;
    BBLOCK *endBlock;    
    BBLOCK *beginBlock;
    unsigned short beginOffset;
    unsigned short endOffset;
    DEBUGINFO *next;
    DEBUGINFO *prev;
    bool alreadyAdjusted;
};

/////////////////////////////////////////////////////////////////////////////////////////

struct SWITCHDESTGOTO {
    BBLOCK * dest;
    bool jumpIntoTry:1;
};

struct SWITCHDEST {
    DWORD count;
    SWITCHDESTGOTO blocks[0];  // this is actually of size "count"
};

/////////////////////////////////////////////////////////////////////////////////////////
struct HANDLERINFO;

struct BBLOCK {
    union {
        BBLOCK * jumpDest;
        SWITCHDEST * switchDest;
        SYM * sym;
    };
    BBLOCK * next;
    int order;
    union {
        unsigned startOffset;
        BBLOCK * markedWith;
        SWITCHDESTGOTO * markedWithSwitch;
    };
    BYTE * code;
    DEBUGINFO * debugInfo;

    size_t curLen;
    ILCODE exitIL : 16;
    ILCODE exitILRev : 16;

    // post op && op && pre op
    bool reachable:1; 
    // post op
    bool largeJump:1;
    // pre op && op
    bool gotoBlocked:1;
    bool jumpIntoTry:1;
    bool startsCatchOrFinally:1;
    bool endsFinally:1;
    bool startsTry:1;
    bool leaveTarget:1;

    int tryNesting;
    int leaveNesting;
    bool isNOP() {
        return exitIL == cee_next && !curLen && !startsTry;
    }
    bool isEmpty() {
        return exitIL == cee_next && !curLen;
    }
    void makeEmpty() {
        curLen = 0;
        exitIL = cee_next;
        debugInfo = NULL;
    }
    void FlipJump();

    // Size = 10 x 4 == 40 bytes... (on 32 bit machines, on 64 bit its more...)
};
#define BB_SIZE 1024
#define BB_TOPOFF 20
#define BB_PREBUFFER 10


/////////////////////////////////////////////////////////////////////////////////////////

struct HANDLERINFO {
    BBLOCK * tryBegin; // beginning of try
    BBLOCK * tryEnd;   // end of try
    BBLOCK * handBegin;  // beginning of handler
    BBLOCK * handEnd;  // end of handler
    TYPESYM * type;     // value of 1 indicates fault (instead of finally)
    HANDLERINFO * next;
    bool handlerShouldIncludeNOP;

    bool IsTryCatch() { return type != NULL && type != (void*)1; }
    bool IsTryFinally() { return type == NULL; }
    bool IsTryFault() { return type == (void*)1; };
};

/////////////////////////////////////////////////////////////////////////////////////////

typedef LOCSLOTINFO * PSLOT;

/////////////////////////////////////////////////////////////////////////////////////////

static const int TEMPBUCKETSIZE = 16; 

struct TEMPBUCKET {
    LOCSLOTINFO slots[TEMPBUCKETSIZE] ; 
    TEMPBUCKET * next;
};

/////////////////////////////////////////////////////////////////////////////////////////

struct SWITCHBUCKET {
    unsigned __int64 firstMember;
    unsigned __int64 lastMember; // ie, member after last, so that (last-first)+1 == slots

    unsigned members; // actual members present in slots
    EXPRSWITCHLABEL ** labels; // starts w/ firstMember
    SWITCHBUCKET * prevBucket;

};
typedef SWITCHBUCKET * PSWITCHBUCKET;

/////////////////////////////////////////////////////////////////////////////////////////

class MARKREACHABLEINFO {
    struct BBSTACK {
        BBLOCK *  bbItem[8];
        BBSTACK * next;
    };

    NRHEAP *  allocator;
    NRMARK    markBase;
    BBSTACK * stack;
    BBSTACK * empty;
    int       stackIndex;

public:
    MARKREACHABLEINFO( NRHEAP * allocator);
    ~MARKREACHABLEINFO();
    void MarkAllReachableBB(BBLOCK * start);

    void Push(BBLOCK* block);
    BBLOCK * Pop();

};
/////////////////////////////////////////////////////////////////////////////////////////
struct IlSlotInfo {
    NAME * name;
    TYPESYM * type;
    uint ilSlotNum;
    TEMP_KIND tempKind;
    bool fIsUsed;
};

// For 64-bit we need to make sure that the struct will stay pointer-aligned
// even when allocated sequentially in an array
C_ASSERT((sizeof(IlSlotInfo) % sizeof(void*)) == 0);

/////////////////////////////////////////////////////////////////////////////////////////

namespace SpecialDebugPoint {
    enum _Type {
        HiddenCode,
        OpenCurly,
        CloseCurly,
    };
};
typedef SpecialDebugPoint::_Type SpecialDebugPointEnum;

/////////////////////////////////////////////////////////////////////////////////////////

#define NO_DEBUG_LINE   (0x00FEEFEE)

/////////////////////////////////////////////////////////////////////////////////////////
struct AccessTask {
    enum _Enum {
        Addr = 0x01, // Leave the "address" on the stack.
        Load = 0x02, // Load the value. Can combine with Addr.

        // Store the value, assuming the "address" is on the stack (below the value to store).
        // Not valid with Load or Addr.
        Store = 0x04
    };

#ifndef DEBUG
    typedef uint _EnumType;
#else
    typedef _Enum _EnumType;
    static bool FValid(_EnumType flags) { return flags && ((flags == Store) || !(flags & Store)); }
#endif // DEBUG
    static bool FAddr(_EnumType flags) { return !!(flags & Addr); }
    static bool FLoad(_EnumType flags) { return !!(flags & Load); }
    static bool FStore(_EnumType flags) { return !!(flags & Store); }
    static bool FAddrOnly(_EnumType flags) { return (flags == Addr); }
    static bool FAddrOrLoad(_EnumType flags) { return !!(flags & (Addr | Load)); }
    static bool FLoadOrStore(_EnumType flags) { return !!(flags & (Load | Store)); }
    static bool FDup(_EnumType flags) { return (flags == (Load | Addr)); }
};
DECLARE_FLAGS_TYPE(AccessTask);

struct AddrInfo {
    PSLOT slotStore;
    bool fIndirectArray;
    void Init() { memset(this, 0, sizeof(AddrInfo)); }
};


struct MultiOpInfo {
    AddrInfo addr;
    bool fNeedOld;
    byte cget;
    int cvalStack;
    PSLOT slot;
};


class ILGENREC {
public:

    void compile(METHSYM * method, METHINFO * info, EXPR * tree);
    ILGENREC();

    static bool callAsVirtual(METHSYM * meth, EXPR * object, bool isBaseCall);

private:
    METHSYM * method;
    METHINFO * info;
    SCOPESYM * localScope;
    AGGSYM * cls;
    HCEEFILE pFile;
#ifdef DEBUG
    bool privShouldDumpAllBlocks;
#endif

    NRHEAP * allocator;

    BBLOCK * firstBB;
    BBLOCK * currentBB;
    BBLOCK   inlineBB;
    
    HANDLERINFO * handlers;
    HANDLERINFO * lastHandler;
    PSLOT origException;
    BBLOCK * returnLocation;
    bool returnHandled;
    unsigned blockedLeave;
    unsigned ehCount;
    bool closeIndexUsed;
    bool compileForEnc;

    DEBUGINFO * curDebugInfo;

    bool TrackDebugInfo();
    SourceExtent extentCurrent;
    BASENODE * nodeCurrent;

    TEMPBUCKET * temporaries;
    PSLOT retTemp;
    int globalFieldCount;

    int curStack;
    int maxStack;

    int finallyNesting;

    void markStackMax() {
        if (maxStack < curStack) maxStack = curStack;
        ASSERT(maxStack >= 0 && curStack >= 0);
    }

    long getOpenIndex();
    long getCloseIndex();
    SCOPESYM * getLocalScope();
    METHSYM * getVarArgMethod(METHSYM * sym, EXPR * args);
    TYPESYM ** getLocalTypes(TYPESYM ** array, SCOPESYM * scope);
    void assignLocals(SCOPESYM * scope);
    void initLocal(PSLOT slot, TYPESYM * type = NULL);
    void assignParams();
    mdToken computeLocalSignature();
    INFILESYM * getInfileFromTree(BASENODE * tree);
    SourceExtent __fastcall getPosFromTree(BASENODE * tree, int flags);
    SourceExtent __fastcall getSpecialPos(SpecialDebugPointEnum e);
    SourceExtent __fastcall getSpecialPos(long index, INFILESYM * infile);

    void InitLocalsFromEnc();
    uint GetLocalSlot(NAME * name, TYPESYM * type);
    uint GetLocalSlot(TEMP_KIND tempKind, TYPESYM * type);
    BlobBldrNrHeap * m_pbldrLocalSlotArray;

    BBLOCK * createNewBB(bool makeCurrent = false);
    BBLOCK * startNewBB(BBLOCK * next, ILCODE exitIL = cee_next, BBLOCK * jumpDest = NULL, ILCODE exitILRev = CEE_ILLEGAL);
    void endBB(ILCODE exitIL, BBLOCK * jumpDest, ILCODE exitILRev = CEE_ILLEGAL);
    void initInlineBB();
    void initFirstBB();
    void flushBB();
    unsigned getCOffset() { return (unsigned)(inlineBB.code - (reusableBuffer)); }
    void closeDebugInfo();
    void emitDebugDataPoint(BASENODE * tree, int flags);
    void emitDebugDataPoint(SpecialDebugPointEnum e, int flags);
    void emitDebugDataPoint(BLOCKNODE * tree, bool openCurly);
    void setDebugDataPoint();
    void emitDebugLocalUsage(BASENODE * tree, LOCVARSYM * sym);
    void maybeEmitDebugLocalUsage(BASENODE * tree, LOCVARSYM * sym);
    void openDebugInfo(BASENODE * tree, int flags);
    void openDebugInfo(SpecialDebugPointEnum e, int flags);
    void openDebugInfo(BLOCKNODE *block, bool openCurly);
    void emitNopForCurly(EXPRBLOCK * block, bool openCurly);
    void createNewDebugInfo();
    static bool fitsInBucket(PSWITCHBUCKET bucket, unsigned __int64 key, unsigned newMembers);
    static unsigned mergeLastBucket(PSWITCHBUCKET * lastBucket);
    void emitSwitchBuckets(PSWITCHBUCKET * array, unsigned first, unsigned last, PSLOT slot, BBLOCK * defBlock);
    void emitSwitchBucket(PSWITCHBUCKET bucket, PSLOT slot, BBLOCK * defBlock);
    BBLOCK * emitSwitchBucketGuard(EXPR * key, PSLOT slot, bool force);
    void emitDebugInfo(unsigned codeSize, mdToken tkLocalVarSig);
    void emitDebugScopesAndVars(SCOPESYM * scope, unsigned codeSize, mdToken tkLocalVarSig);

    void putOpcode(ILCODE opcode);
    void putOpcode(BYTE ** buffer, ILCODE opcode);
    void putDWORD(DWORD dw);
    void putWORD(WORD w);
    void putCHAR(char c);
    void putQWORD(__int64 * qw);

    void genPrologue(EXPRBLOCK * tree);
    void genBlock(EXPRBLOCK * tree);
    bool shouldEmitNopForBlock(EXPRBLOCK * tree);
    void GenStmtChain(EXPRSTMT * tree);
    void GenStatement(EXPRSTMT * tree);
    void genExpr(EXPR * tree, bool valUsed = true);
    void genBinopExpr(EXPRBINOP* tree, bool valUsed = true);
    void genString(CONSTVAL string);
    void genCall(EXPRCALL * tree, bool valUsed);
    void emitRefParam(EXPR * arg, PSLOT * curSlot);
    void genSideEffects(EXPR * tree);

    void genAccess(EXPR * tree, AccessTaskEnum flags, AddrInfo & addr);
    void genLoad(EXPR * tree) {
        AddrInfo addr;
        genAccess(tree, AccessTask::Load, addr);
    }

    // Returns the stack delta (which will always be >= 0).
    int genAddr(EXPR * tree, AddrInfo & addr) {
        int cvalStack = curStack;
        genAccess(tree, AccessTask::Addr, addr);
        ASSERT(curStack >= cvalStack);
        return curStack - cvalStack;
    }
    void genStore(EXPR * tree, AddrInfo & addr) {
        genAccess(tree, AccessTask::Store, addr);
#ifdef DEBUG
        addr.slotStore = (PSLOT)(UINT_PTR)0xC3C3C3C3;
#endif // DEBUG
    }

    void genFieldAccess(EXPRFIELD * tree, AccessTaskEnum flags, AddrInfo & addr);
    void genPropAccess(EXPRPROP * tree, AccessTaskEnum flags, AddrInfo & addr);
    void genArrayAccess(EXPRBINOP * tree, AccessTaskEnum flags, AddrInfo & addr);
    void genArgsDup(EXPR * obj, bool fConstrained, EXPR * args, bool fArray);

    unsigned getArgCount(EXPR * args);
    void genMemoryAddress(EXPR * tree, PSLOT * pslot, bool ptrAddr = false, bool fReadOnly = false);
    void GenDupSafeMemAddr(EXPR * tree, PSLOT * pslot);
    void genSlotAddress(PSLOT slot, bool ptrAddr = false);
    void genReturn(EXPRRETURN * tree);
    void genGoto(EXPRGOTO * tree);
    void genGotoIf(EXPRGOTOIF * tree);
    void genLabel(EXPRLABEL * tree);
    BBLOCK * genCondBranch(EXPR * condition, BBLOCK * dest, bool sense);
    bool isSimpleExpr(EXPR * condition, bool * sense);
    void genCondExpr(EXPR * condition, bool sense, EXPR::CONSTRESKIND * crk = NULL);
    void genNewArray(EXPRBINOP * tree);
    void genSwitch(EXPRSWITCH * tree);
    void genHashtableStringSwitchInit(EXPRSWITCH * tree);
    void genStringSwitch(EXPRSWITCH * tree);
    void genTry(EXPRTRY * tree);
    void genThrow(EXPRTHROW * tree);
    HANDLERINFO * createHandler(BBLOCK * tryBegin, BBLOCK * tryEnd, BBLOCK * handBegin, TYPESYM * type);
    void handleReturn(bool addDebugInfo = false);

    void GenMultiOp(EXPRMULTI * tree, bool valUsed = true);
    void GenMultiGet(EXPRMULTI * tree, bool valUsed = true);

    void genQMark(EXPRBINOP * tree, bool valUsed);
    void genArrayInit(EXPRARRINIT * tree, bool valUsed);
    void genArrayInitConstant(EXPRARRINIT * tree, TYPESYM * elemType, bool valUsed);
    static void writeArrayValues1(BYTE * buffer, EXPR * tree);
    static void writeArrayValues2(BYTE * buffer, EXPR * tree);
    static void writeArrayValues4(BYTE * buffer, EXPR * tree);
    static void writeArrayValues8(BYTE * buffer, EXPR * tree);
    static void writeArrayValuesD(BYTE * buffer, EXPR * tree);
    static void writeArrayValuesF(BYTE * buffer, EXPR * tree);
    void genCast(EXPRCAST * tree, bool valUsed);
    void genObjectPtr(EXPR * object, SYM * member, PSLOT * pslot);
    void genIs(EXPRBINOP * tree, bool valUsed, bool isAs);
    void genZeroInit(EXPRZEROINIT * tree, bool valUsed);
    void genArrayCall(ARRAYSYM * array, int args, ARRAYMETHOD meth);
    void genFlipParams(TYPESYM * elem, unsigned count);
    void genPtrAddr(EXPR * op, bool fNoConv, bool valUsed);
    void genSizeOf(TYPESYM * typ);

    void genIntConstant(int val);
    void genFloatConstant(float val);
    void genDoubleConstant(double *val);
    void genDecimalConstant(DECIMAL * val);
    void genLongConstant(__int64 *val);
    void genZero(BASENODE * tree, TYPESYM * type);
    void genSwap(EXPRBINOP * tree, bool valUsed);
    
    void emitRefValue(EXPRBINOP * tree);
    void genMakeRefAny(EXPRBINOP * tree, bool valUsed);
    void genTypeRefAny(EXPRBINOP * tree, bool valUsed);

    void dumpLocal(PSLOT slot, bool store);
    void dumpLocal(int slot, bool isParam, bool store);
    PSLOT storeLocal(PSLOT slot);

    bool isExprOptimizedAway(EXPR *tree);

#if DEBUG
    void initDumpingAllBlocks();
    bool shouldDumpAllBlocks() { return privShouldDumpAllBlocks; }
    void dumpAllBlocks(PCWSTR label);
    void dumpAllBlocksContent(PCWSTR label);
    int FindBlockInList(BBLOCK ** list, int count, BBLOCK * block);
    ILCODE findInstruction(BYTE b1, BYTE b2);

    void  verifyAllTempsFree();
    PSLOT allocTemporary(TYPESYM * type, TEMP_KIND tempKind, PCSTR file, unsigned line);
#else
    PSLOT allocTemporary(TYPESYM * type, TEMP_KIND tempKind);
#endif
    TEMPBUCKET * AllocBucket();
    void freeTemporary(PSLOT slot);
    PSLOT DumpToDurable(TYPESYM * type, bool fFree, bool dumpToStack = true);

    void emitFieldToken(MEMBVARSYM * sym, AGGTYPESYM *methodInType = NULL);
    void emitMethodToken(METHSYM * sym, AGGTYPESYM *methodInType = NULL, TypeArray *pMethArgs = NULL);
    void emitTypeToken(TYPESYM * sym);
    void emitArrayMethodToken(ARRAYSYM * sym, ARRAYMETHOD methodId);

    bool needsBoxing(PARENTSYM * parent, TYPESYM * object);
    friend class MARKREACHABLEINFO;
    static bool markAsVisited(BBLOCK * block);
    void markAllReachableBB(BBLOCK * start);
    void __fastcall optimizeBranchesToNOPs();
    void __fastcall optimizeBranchesToNext();
    void __fastcall optimizeBranchesOverBranches();
    void __fastcall optimizeBranchesToBranches();
    void optimizeGotos();
    unsigned getFinalCodeSize();
    unsigned computeSwitchSize(BBLOCK * block);
    BYTE * copySwitchInstruction(BYTE * outBuffer, BBLOCK * block);
    BYTE * copyCode(BYTE * outBuffer);
    int    computeJumpOffset(BBLOCK * from, BBLOCK * to, unsigned instrSize);
    void copyHandlers(COR_ILMETHOD_SECT_EH_CLAUSE_FAT* clauses);
    
    ILCODE getShortOpcode(ILCODE longOpcode);

    COMPILER * compiler();
    static const REFENCODING ILcodes[cee_last];
    static const int ILStackOps[cee_last];
    static const BYTE ILcodesSize[cee_last];
    static const ILCODE ILlsTiny[4][6];
    static const ILCODE ILarithInstr[EK_ARRLEN - EK_ADD + 1];
    static const ILCODE ILarithInstrUN[EK_ARRLEN - EK_ADD + 1];
    static const ILCODE ILarithInstrOvf[EK_ARRLEN - EK_ADD + 1];
    static const ILCODE ILarithInstrUNOvf[EK_ARRLEN - EK_ADD + 1];
    static const ILCODE ILstackLoad[FT_COUNT];
    static const ILCODE ILstackStore[FT_COUNT];
    static const ILCODE ILarrayLoad[FT_COUNT];
    static const ILCODE ILarrayStore[FT_COUNT];
    static const ILCODE ILaddrLoad[2][2];
    BYTE reusableBuffer[BB_PREBUFFER + BB_SIZE + BB_TOPOFF];
    static const ILCODE simpleTypeConversions[NUM_EXT_TYPES][NUM_EXT_TYPES];
    static const ILCODE simpleTypeConversionsOvf[NUM_EXT_TYPES][NUM_EXT_TYPES];
    static const ILCODE simpleTypeConversionsEx[NUM_EXT_TYPES][NUM_EXT_TYPES];

#if DEBUG
    static const PWSTR ILnames[cee_last];

    bool smallBlock;
#endif
};

#endif // __ilgen_h__
