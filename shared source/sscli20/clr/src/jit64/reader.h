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
//
// READER.H
//
// File describes interface between reader and client.
//

#ifndef _READER_H
#define _READER_H

#include <stdio.h> // for printf(...)

// These turn on Reader SSA until we decide to make it more permanent.
#define READER_ESSA 7
// turn this on to fully enable Reader SSA
//#define READER_ESSA_RENAME 11

#include "cor.h"
#include "corjit.h"
#include "malloc.h"         // for _alloca
#include "readerenum.h"
#include "gverify.h"
#include "jit64.h"

#define JIT64_BUILD
#include "palclr.h"

extern int _cdecl dbprint(const char *form, ...);

#define CC_CALL_INTERCEPT

// as defined in src\vm\vars.hpp
#define MAX_CLASSNAME_LENGTH 1024

#ifndef COUNTOF
#define COUNTOF(a) (sizeof(a) / sizeof(*a))
#endif

// Forward declarations for client defined structures
class GenIR;           // Compiler dependent IR production
class IRNode;          // Your own beloved compiler intermediate representation
class ReaderStack;

class FlowGraphNode;
class FlowGraphEdgeList;
class BranchList;
class ReaderBitVector;
struct EHRegion;

struct tagGCLAYOUT_STRUCT
{
    INT     nGCPtrs;
    BYTE    gcLayout[0];
};

typedef struct tagGCLAYOUT_STRUCT GCLAYOUT_STRUCT, *PGCLAYOUT_STRUCT;

#define GCLS_NUMGCPTRS(x) ((x)->nGCPtrs)
#define GCLS_GCLAYOUT(x)  ((x)->gcLayout)

#define GCLAYOUT_BASE_PTR       1   
#define GCLAYOUT_INTERIOR_PTR   2

extern HRESULT verLastError;

// Structure used to pass argument information from rdrCall
// to GenIR_GenCall.
struct callArgTriple{
    IRNode*              argNode;
    CorInfoType          argType;
    CORINFO_CLASS_HANDLE argClass;
};

struct FlowGraphNodeList {
    FlowGraphNode *block;
    FlowGraphNodeList* next;
};

struct FlowGraphNodeWorkList {
    FlowGraphNode *block;
    FlowGraphNodeWorkList* next;
    FlowGraphNode * parent;
};

class VerifyWorkList;
class VerificationState;

typedef enum {
    READER_ALIGN_NATURAL = (unsigned char)~0, // Default natural alignment
    READER_ALIGN_UNKNOWN = 0,
    READER_ALIGN_1       = 1,
    READER_ALIGN_2       = 2,
    READER_ALIGN_4       = 4,
    READER_ALIGN_8       = 8,
} ReaderAlignType;

// Used to denote a special symbol when creating syms
typedef enum {
    READER_NOT_SPECIAL_SYMBOL = 0,
    READER_THIS_PTR,
    READER_UNMODIFIED_THIS_PTR,
    READER_VARARGS_TOKEN,
    READER_INST_PARAM,
    READER_SECURITY_OBJECT,
    READER_GENERICS_CONTEXT
} ReaderSpecialSymbolType;

// Types of pointers.
typedef enum {
    READER_PTR_NOT_GC = 0,
    READER_PTR_GC_BASE,
    READER_PTR_GC_INTERIOR
} ReaderPtrType;

typedef enum {
    READER_LOCAL_VERIFICATION_EXCEPTION,
    READER_GLOBAL_VERIFICATION_EXCEPTION,
} ReaderExceptionType;

typedef enum {
  MONITOR_TRY_FINALLY_REGION = 0, 
  SECURITY_CALLOUT_TRY_FINALLY_REGION 
} TryFinallyRegionType; 

class ReaderException
{
public:
    ReaderExceptionType type;
};

class LabelOffsetList;

// The TryRegion graph allows us to build a region tree that captures the 
// lexical information from the EIT before we begin reading the MSIL opcodes.
// Thus we build the try-region tree, then the flow graph, then fill in the
// the flow graph nodes with IL.

struct EHRegion;
struct EHRegionList;
struct FgData;

//
// Reader Stack
//

typedef struct IRNodeListElement* IRNodeList;

// A node for the push down list we call the reader stack. The 
// intent is that the stack is only accessed via push and pop.
class ReaderStackNode;
class ReaderStackIterator;
class ReaderBase;

class ReaderStack 
{
public:
    virtual IRNode* pop (void) = 0;
    virtual void push (IRNode* newVal, IRNode **newIR) = 0;
    virtual void clearStack (void) = 0;
    virtual bool empty(void) = 0;
    virtual void assertEmpty(void) = 0;
    virtual int  depth() = 0;

    // For iteration, implement/use as needed by the client
    virtual IRNode* getIterator(ReaderStackIterator**) = 0;
    virtual IRNode* iteratorGetNext (ReaderStackIterator**) = 0;
    virtual void    iteratorReplace(ReaderStackIterator**, IRNode*) = 0;
    virtual IRNode* getReverseIterator(ReaderStackIterator**) = 0;
    virtual IRNode* getReverseIteratorFromDepth(ReaderStackIterator**, int depth) = 0;
    virtual IRNode* reverseIteratorGetNext(ReaderStackIterator**) = 0;

#if defined(_DEBUG)
    virtual void print() = 0;
#endif

    // Creates stack copy, used in non-empty stacks across flow, etc.
    virtual ReaderStack* copy() = 0;
};

class ReaderCallTargetData {
    friend class ReaderBase;
public:
    __forceinline mdToken getMethodToken() { return targetMethodToken; }
    __forceinline CORINFO_METHOD_HANDLE getMethodHandle() { return targetMethodHandle; }
    __forceinline DWORD getMethodAttribs() { return targetMethodAttribs; };
    __forceinline CORINFO_SIG_INFO *getSigInfo() { return &sigInfo; };
    __forceinline CORINFO_CALL_INFO *getCallInfo() { return fCallInfoValid ? &callInfo : NULL; }
    __forceinline IRNode *getIndirectionCellNode() { return indirectionCellNode; }
    __forceinline IRNode *getCallTargetNode() { return callTargetNode; }

    DWORD   getClassAttribs();
    CORINFO_CLASS_HANDLE getClassHandle();

    IRNode *getMethodHandleNode(IRNode **newIR);
    IRNode *getClassHandleNode(IRNode **newIR);
    IRNode *getTypeContextNode(IRNode **newIR);

    __forceinline bool hasThis() { return fThisPtr; }
    __forceinline bool isJmp() { return fJmp; }
    __forceinline bool isTailCall() { return fTailCall; }
    __forceinline bool isRecursiveTailCall() { return fRecursiveTailCall; }
    __forceinline bool isUnmarkedTailCall() { return fUnmarkedTailCall; }
    __forceinline bool isStubDispatch() { return indirectionCellNode ? true : false; }
    __forceinline bool isIndirect() { return fIndirect; }
    __forceinline bool isTrueDirect() { return getCallInfo()->kind==CORINFO_CALL; }
    __forceinline bool isNewObj() { return fNewObj; }
    __forceinline bool needsNullCheck() { return fNeedsNullCheck; }
    __forceinline bool usesMethodDesc() { return fUsesMethodDesc; }
    __forceinline bool needsThisInSecretRegister() { return fThisInSecretRegister; }
    __forceinline bool isReadonlyCall() { return fReadonlyCall; }

    __forceinline mdToken getLoadFtnToken() { return loadFtnToken; }

    __forceinline bool isBasicCall() { return !fIndirect && !fNewObj && !isJmp() && isTrueDirect(); }
    __forceinline CorInfoIntrinsics getCorInstrinsic() { return corIntrinsicId; }

    void setOptimizedDelegateCtor(CORINFO_METHOD_HANDLE newTargetMethodHandle);
    DelegateCtorArgs * getDelegateCtorData() { return pCtorArgs; }

private:
    ReaderBase *reader;
    mdToken targetMethodToken;
    mdToken constraintToken;
    mdToken loadFtnToken;
    bool fThisPtr;
    bool fJmp;
    bool fTailCall;
    bool fRecursiveTailCall;
    bool fUnmarkedTailCall;
    bool fClassAttribsValid;
    bool fCallInfoValid;
    bool fCallVirt;
    bool fIndirect;
    bool fNewObj;
    bool fNeedsNullCheck;
    bool fUsesMethodDesc;
    bool fOptimizedDelegateCtor;
    bool fThisInSecretRegister;
    bool fReadonlyCall;

    CorInfoIntrinsics corIntrinsicId;
    DWORD   targetMethodAttribs;
    DWORD   targetClassAttribs;
    CORINFO_METHOD_HANDLE targetMethodHandle;
    CORINFO_CLASS_HANDLE targetClassHandle;
    CORINFO_SIG_INFO sigInfo;
    CORINFO_CALL_INFO callInfo;
    DelegateCtorArgs *pCtorArgs;

    IRNode *targetMethodHandleNode;
    IRNode *targetClassHandleNode;
    IRNode *indirectionCellNode;
    IRNode *callTargetNode;

#if defined(_DEBUG)
    char targetName[MAX_CLASSNAME_LENGTH];
#endif

    __forceinline void setIndirectionCellNode(IRNode *n) { indirectionCellNode = n; }
    void fillTargetInfo();

    // Private constructor: only called by friend class ReaderBase
    ReaderCallTargetData(ReaderBase *reader, 
                         mdToken targetToken, mdToken constraintToken, 
                         mdToken loadFtnToken,
                         bool fTailCall, bool fUnmarkedTailCall, bool fReadonlyCall, 
                         ReaderBaseNS::CallOpcode opcode);
};

// Interface to GenIR defined EHRegion structure
// Implementation Supplied by Jit Client
EHRegion*            RgnAllocateRegion(GenIR* genIR);
EHRegionList*        RgnAllocateRegionList(GenIR* genIR);
EHRegionList*        RgnListGetNext(EHRegionList* ehRegionList);
void                 RgnListSetNext(EHRegionList* ehRegionList,EHRegionList* next);
EHRegion*            RgnListGetRgn(EHRegionList* ehRegionList);
void                 RgnListSetRgn(EHRegionList* ehRegionList,EHRegion* rgn);
ReaderBaseNS::RegionKind RgnGetRegionType(EHRegion* ehRegion);
void                 RgnSetRegionType(EHRegion* ehRegion,ReaderBaseNS::RegionKind type);
DWORD                RgnGetStartMSILOffset(EHRegion* ehRegion);
void                 RgnSetStartMSILOffset(EHRegion* ehRegion,DWORD offset);
DWORD                RgnGetEndMSILOffset(EHRegion* ehRegion);
void                 RgnSetEndMSILOffset(EHRegion* ehRegion,DWORD offset);
IRNode*              RgnGetHead(EHRegion* ehRegion);
void                 RgnSetHead(EHRegion* ehRegion, IRNode* head);
IRNode*              RgnGetLast(EHRegion* ehRegion);
void                 RgnSetLast(EHRegion* ehRegion, IRNode* last);
bool                 RgnGetIsLive(EHRegion *ehRegion);
void                 RgnSetIsLive(EHRegion *ehRegion, bool live);
void                 RgnSetParent(EHRegion *ehRegion,EHRegion* parent);
EHRegion*            RgnGetParent(EHRegion *ehRegion);
void                 RgnSetChildList(EHRegion *ehRegion,EHRegionList* children);
EHRegionList*        RgnGetChildList(EHRegion *ehRegion);
bool                 RgnGetHasNonLocalFlow(EHRegion *ehRegion);
void                 RgnSetHasNonLocalFlow(EHRegion *ehRegion,bool nonLocalFlow);
IRNode*              RgnGetEndOfClauses(EHRegion* ehRegion);
void                 RgnSetEndOfClauses(EHRegion* ehRegion,IRNode* node);
IRNode*              RgnGetTryBodyEnd(EHRegion* ehRegion);
void                 RgnSetTryBodyEnd(EHRegion* ehRegion,IRNode* node);
ReaderBaseNS::TryKind RgnGetTryType(EHRegion* ehRegion);
void                 RgnSetTryType(EHRegion* ehRegion,ReaderBaseNS::TryKind type);
int                  RgnGetTryCanonicalExitOffset(EHRegion* tryRegion);
void                 RgnSetTryCanonicalExitOffset(EHRegion* tryRegion, int offset);
EHRegion*            RgnGetExceptFilterRegion(EHRegion* ehRegion);
void                 RgnSetExceptFilterRegion(EHRegion* ehRegion,EHRegion* filterRegion);
EHRegion*            RgnGetExceptTryRegion(EHRegion* ehRegion);
void                 RgnSetExceptTryRegion(EHRegion* ehRegion, EHRegion* tryRegion);
bool                 RgnGetExceptUsesExCode(EHRegion* ehRegion);
void                 RgnSetExceptUsesExCode(EHRegion* ehRegion, bool usesExceptionCode);
EHRegion*            RgnGetFilterTryRegion(EHRegion* ehRegion);
void                 RgnSetFilterTryRegion(EHRegion* ehRegion, EHRegion* tryRegion);
EHRegion*            RgnGetFilterHandlerRegion(EHRegion* ehRegion);
void                 RgnSetFilterHandlerRegion(EHRegion* ehRegion, EHRegion* handler);
EHRegion*            RgnGetFinallyTryRegion(EHRegion* finallyRegion);
void                 RgnSetFinallyTryRegion(EHRegion* finallyRegion, EHRegion* tryRegion);
bool                 RgnGetFinallyEndIsReachable(EHRegion* finallyRegion);
void                 RgnSetFinallyEndIsReachable(EHRegion* finallyRegion, bool isReachable);
EHRegion*            RgnGetFaultTryRegion(EHRegion* faultRegion);
void                 RgnSetFaultTryRegion(EHRegion* faultRegion,EHRegion* tryRegion);
EHRegion*            RgnGetCatchTryRegion(EHRegion* catchRegion);
void                 RgnSetCatchTryRegion(EHRegion* catchRegion,EHRegion* tryRegion);
mdToken              RgnGetCatchClassToken(EHRegion* catchRegion);
void                 RgnSetCatchClassToken(EHRegion* catchRegion, mdToken token);



// Interface to GenIR defined Flow Graph structures.
// Implementation Supplied by Jit Client
FlowGraphNode*       FgNodeGetNext (FlowGraphNode* fgNode);
bool                 FgNodeIsVisited(FlowGraphNode* fgNode);
void                 FgNodeSetVisited(FlowGraphNode* fgNode,bool visited);
EHRegion*            FgNodeGetRegion(FlowGraphNode* fgNode);
void                 FgNodeSetRegion(FlowGraphNode* fgNode,EHRegion* ehRegion);
FlowGraphEdgeList*   FgNodeGetSuccessorList(FlowGraphNode* fgNode);
FlowGraphEdgeList*   FgNodeGetPredecessorList(FlowGraphNode* fgNode);

// Get the special block-start placekeeping node
IRNode*              FgNodeGetStartIRNode(FlowGraphNode* fgNode);

// Get the first non-placekeeping node in block
IRNode*              FgNodeGetStartInsertIRNode(FlowGraphNode* fgNode);

// Get the special block-end placekeeping node
IRNode*              FgNodeGetEndIRNode(FlowGraphNode* fgNode);

// Get the last non-placekeeping node in block
IRNode*              FgNodeGetEndInsertIRNode(FlowGraphNode* fgNode);

IRNode*              FgNodeGetEndIRInsertionPoint(FlowGraphNode* fgNode);
unsigned int         FgNodeGetStartMSILOffset(FlowGraphNode* fg);
void                 FgNodeSetIBCNotReal(FlowGraphNode* fg);
void                 FgNodeSetStartMSILOffset(FlowGraphNode* fg, unsigned int offset);
unsigned int         FgNodeGetEndMSILOffset(FlowGraphNode* fg);
void                 FgNodeSetEndMSILOffset(FlowGraphNode* fgNode, unsigned int offset);

GLOBALVERIFYDATA *   FgNodeGetGlobalVerifyData(FlowGraphNode* fg);
void                 FgNodeSetGlobalVerifyData(FlowGraphNode* fg, GLOBALVERIFYDATA * gvData);

void                 FgNodeSetOperandStack(FlowGraphNode* fg, ReaderStack* stack);
ReaderStack*         FgNodeGetOperandStack(FlowGraphNode* fg);

unsigned             FgNodeGetBlockNum(FlowGraphNode* fg);

FlowGraphEdgeList*   FgEdgeListGetNextSuccessor(FlowGraphEdgeList* fgEdge);
FlowGraphEdgeList*   FgEdgeListGetNextPredecessor(FlowGraphEdgeList* fgEdge);
FlowGraphNode*       FgEdgeListGetSource(FlowGraphEdgeList* fgEdge);
FlowGraphNode*       FgEdgeListGetSink(FlowGraphEdgeList* fgEdge);
bool                 FgEdgeListIsNominal(FlowGraphEdgeList* fgEdge);
bool                 FgEdgeListIsFake(FlowGraphEdgeList* fgEdge);
void                 FgEdgeListMakeFake(FlowGraphEdgeList* fgEdge);

FlowGraphEdgeList*   FgEdgeListGetNextSuccessorActual(FlowGraphEdgeList* fgEdge);
FlowGraphEdgeList*   FgEdgeListGetNextPredecessorActual(FlowGraphEdgeList* fgEdge);
FlowGraphEdgeList*   FgNodeGetSuccessorListActual(FlowGraphNode* fg);
FlowGraphEdgeList*   FgNodeGetPredecessorListActual(FlowGraphNode* fg);

// Interface to GenIR defined IRNode structure
// Implementation Supplied by Jit Client

IRNode*              IRNodeGetNext(IRNode* node);
bool                 IRNodeIsBranch(IRNode* node); 

IRNode*              IRNodeGetInsertPointAfterMSILOffset(IRNode* node, unsigned int offset);
IRNode*              IRNodeGetInsertPointBeforeMSILOffset(IRNode* node, unsigned int offset);
IRNode*              IRNodeGetFirstLabelOrInstrNodeInEnclosingBlock(IRNode* handlerStartNode);
unsigned int         IRNodeGetMSILOffset(IRNode* node);
void                 IRNodeLabelSetMSILOffset(IRNode* node, unsigned int labelMSILOffset);
void                 IRNodeBranchSetMSILOffset(IRNode* branchNode, unsigned int offset);
void                 IRNodeExceptSetMSILOffset(IRNode* branchNode, unsigned int offset);
void                 IRNodeInsertBefore(IRNode* insertionPointTuple, IRNode* newNode);
void                 IRNodeInsertAfter(IRNode* insertionPointTuple, IRNode* newNode);
void                 IRNodeSetRegion(IRNode* node, EHRegion* region);
EHRegion*            IRNodeGetRegion(IRNode* node);
FlowGraphNode*       IRNodeGetEnclosingBlock(IRNode* node);
bool                 IRNodeIsLabel(IRNode* node);
bool                 IRNodeIsEHFlowAnnotation(IRNode* node);

// Interface to GenIR defined BranchList structure
// Implementation Supplied by Jit Client.
BranchList*          BranchListGetNext(BranchList* branchList);
IRNode*              BranchListGetIRNode(BranchList* branchList);


class VerificationBranchInfo
{
public:
    unsigned srcOffset;
    unsigned targetOffset;
    IRNode  *branchOp;
    bool     isLeave;

    VerificationBranchInfo *next;
};

ReaderBaseNS::OPCODE ParseMSILOpcode(unsigned char* curMsilPtr, unsigned char** operandPtrPtr, unsigned int* increment, ReaderBase *reader);
ReaderBaseNS::OPCODE ParseMSILOpcodeSafely( unsigned char* buf, unsigned int currOffset, unsigned int bufSize, unsigned char** operand, unsigned int* nextOffset, ReaderBase *reader, bool reportError);


class ReaderBase
{
    friend class ReaderCallTargetData;

public:
    // Public because it is read and written for inlining support.
    CORINFO_METHOD_INFO* m_methodInfo;

    // The reader's operand stack. Public because of inlining and debug prints
    ReaderStack*         m_readerStack;

    // Public for debug printing
    EHRegion*            m_currentRegion;

    FlowGraphNode*       m_currentFgNode;

    bool                 m_hasLocAlloc;

private:
    // Private data (not available to derived client class)
    ICorJitInfo*         m_jitInfo;
    unsigned             m_flags;   // original flags that were passed to compileMethod

    // SEQUENCE POINT Info
    ReaderBitVector*     m_customSequencePoints;

    // EH Info
    CORINFO_EH_CLAUSE   *m_ehClauseInfo; // raw eh clause info
    EHRegion*            m_ehRegionTree;
    EHRegionList*        m_allRegionList;

    // Fg Info - unused after fg is built

    // labelList array is an ordered array of labelOffsetList*. Each entry
    // is a pointer to a labelOffsetList which will contain at most
    // LABEL_LIST_ARRAY_STRIDE elements. The labelOffsetLists are maintained
    // in order, which helps later insertion of labels into tuple-stream.
    LabelOffsetList**    m_labelListArray;
    unsigned             m_labelListArraySize;

    VerificationBranchInfo *m_branchesToVerify;

    // Block array, maps fg node blocknum to optional block data
    FgData**               m_blockArray;

    // Verification Info
public:
    bool                 m_verificationNeeded;
    bool                 m_isVerifiableCode; // valid only if m_verificationNeeded is set
    bool                 m_verIsInstantiationOfFailedGeneric;
    bool                 m_needsRuntimeCallout;
    CorInfoCanSkipVerificationResult   m_canSkipVerificationResult; 
private:
    bool                 m_verTrackObjCtorInitState;
    bool                 m_verThisInitialized;
    unsigned             m_numVerifyParams;
    unsigned             m_numVerifyAutos;
    bool                 m_thisPtrModified;
    vertype *            m_paramVerifyMap;
    vertype *            m_autoVerifyMap;
    mdToken              m_verLastToken;
    ReaderBitVector     *m_legalTargetOffsets;

protected:
    // peverify needs to squirrel away some info on method init to use later
    void *      m_peverifyErrorHandler;
    void *      m_peverifyThis;
public:
#define ERROR_MSG_SIZE 4096
    WCHAR       extended_error_message[ERROR_MSG_SIZE];
protected:

private:
    // Global Verification Info
    unsigned short *     m_gvStackPop;
    unsigned short *     m_gvStackPush;
    PGLOBALVERIFYDATA    m_gvWorklistHead;
    PGLOBALVERIFYDATA    m_gvWorklistTail;

#ifdef READER_ESSA
    // ESSA info
    int                  m_ssaNoParms;
    int                  m_ssaNoLocals;
#endif


public:
    bool                 m_areInlining;
    ReaderBase(ICorJitInfo* jitInfo, CORINFO_METHOD_INFO* methodInfo,unsigned flags);

    // Main Reader Entry
    void MSILToIR(void);

    // Call GenIR_CreateSym for each param and auto. Also return function bytecode start and length.
    void               initParamsAndAutos(unsigned int nParam, unsigned int nAuto);
    void               handleNonEmptyStack(FlowGraphNode* fg,IRNode** newIR,bool *fmbassign);

    // Needed by inlining so public
    FlowGraphNode*     buildFlowGraph(FlowGraphNode** ppFgTail);
    FlowGraphNode*     fgSplitBlock(FlowGraphNode* block, unsigned int offset, IRNode* node);
    CORINFO_ARG_LIST_HANDLE argListNext(CORINFO_ARG_LIST_HANDLE,
                                        CORINFO_SIG_INFO*,
                                        bool fIsLocalArgList,
                                        CorInfoType * pCorType = NULL,
                                        CORINFO_CLASS_HANDLE * phClass = NULL,
                                        bool * pfIsPinned = NULL);
    void               buildUpParams(unsigned int numParams);
    void               buildUpAutos (unsigned int numAutos);
#if defined(_DEBUG)
    // Debug-only reader function to print range of MSIL.
    void               printMSIL(BYTE* pbuf, unsigned startOffset, unsigned endOffset);
#endif

    void               GetMSILInstrStackDelta(ReaderBaseNS::OPCODE opcode, 
                                              BYTE * operand, 
                                              unsigned short * pop, 
                                              unsigned short * push);

private:
    bool               isUnmarkedTailCall(BYTE *buf, unsigned int bufSize, unsigned int nextOffset, mdToken token);
    bool               isUnmarkedTailCallHelper(BYTE *buf, unsigned int bufSize, unsigned int nextOffset, mdToken token);

    // Reduce given block from MSIL to IR
    void               readBytesForFlowGraphNode(FlowGraphNode* fg, bool isVerifyOnly);
public:
    void               initVerifyInfo(void);
 private:
    void               setupBlockForEH(IRNode** newIR);

    void               clearStack(IRNode** newIR);


    bool               IsOffsetInstrStart(unsigned int offset);

    // SEQUENCE POINTS
    void               getCustomequencePoints();

    //
    // FlowGraph
    //


    FlowGraphNode*     fgBuildBasicBlocksFromBytes(BYTE *buf, unsigned int bufSize);
    void               fgBuildPhase1(FlowGraphNode* fg,BYTE* buf,unsigned bufSize);
    void               fgAttachGlobalVerifyData(FlowGraphNode * headBlock, int ssaNoParams, 
                                      int ssaNoLocals);
    void               fgAddArcs(FlowGraphNode *headBlock);
    IRNode*            fgAddCaseToCaseList(IRNode* switchNode, IRNode* labelNode,
                                           unsigned int element);
    FlowGraphNodeWorkList* fgAppendUnvisitedSuccToWorklist(FlowGraphNodeWorkList* worklist,
                                                           FlowGraphNode* currBlock);
    void               fgDeleteBlockAndNodes(FlowGraphNode* block);
    void               fgEnsureEnclosingRegionBeginsWithLabel(IRNode* handlerStartNode);
    EHRegion*          fgGetRegionFromMSILOffset(unsigned int offset);
    FlowGraphNode*     fgInsertLabel(unsigned int offset, IRNode* label, FlowGraphNode* startBlock);
    void               fgInsertLabels(void);
    void               fgInsertTryEnd(EHRegion* ehRegion);
    void               fgInsertBeginRegionExceptionNode(unsigned int offset,IRNode* tupInsert);
    void               fgInsertEndRegionExceptionNode(unsigned int offset, IRNode* tupInsert);
    void               fgInsertEHAnnotations(EHRegion* region);
    IRNode*            fgMakeBranch(IRNode* labelNode, IRNode* blockNode, unsigned int offset,
                                    bool fIsConditional, bool fIsNominal);
    IRNode*            fgMakeEndFinally(IRNode* blockNode, unsigned int offset, bool fIsLexicalEnd);
    void               fgRemoveUnusedBlocks(FlowGraphNode* pFgHead, FlowGraphNode *pFgTail);
    unsigned int       fgGetRegionCanonicalExitOffset(EHRegion* tr);

    // DomInfo - get and set properties of dominators
    void               initBlockArray(unsigned blockCount);
    void*              DomInfo_GetInfoFromDominator(FlowGraphNode* fg, unsigned key1, void* key2, void* key3, void* key4, bool requireSameRegion,
                                                    void *(FgData::*pmfn)(unsigned key1, void* key2, void* key3, void* key4));

    FgData*            DomInfo_GetBlockData(FlowGraphNode *fg, bool doCreate);
    IRNode*            DomInfo_DominatorDefinesSharedStaticBase(FlowGraphNode* fg, CorInfoHelpFunc& helperID, void* moduleID, void* classID,
                                                                mdToken typeRef, bool *pfNoCtor);
    void               DomInfo_RecordSharedStaticBaseDefine(FlowGraphNode* fg, CorInfoHelpFunc helperID, void* moduleID, void* classID, IRNode* basePtr);
    bool               DomInfo_DominatorHasClassInit(FlowGraphNode *fg, mdToken typeRef);
    void               DomInfo_RecordClassInit(FlowGraphNode *fg, mdToken typeRef);

public:
    IRNode*            GetThreadControlBlock(IRNode **newIR);
    IRNode*            DomInfo_DominatorHasTCB(FlowGraphNode *fg);
    void               DomInfo_RecordTCB(FlowGraphNode *fg, IRNode* pTCB);

private:
    LabelOffsetList*   fgAddLabelMSILOffset(IRNode **pLabelTup,unsigned int targetOffset);
    bool               fgLeaveIsNonLocal(FlowGraphNode* fg, unsigned int leaveOffset, 
                                         unsigned int leaveTarget, bool* endsWithNonLocalGoto);

    // =============================================================================
    // =============================================================================
    // =======    EIT Verification   ===============================================
    // =============================================================================
    // =============================================================================

    // these types are for EIT verification only
    struct EHNodeDsc;
    struct EITVerBasicBlock;
    struct EHblkDsc;
    typedef struct EHNodeDsc* pEHNodeDsc;
    typedef UINT32 IL_OFFSET;

    EHNodeDsc* m_ehnTree;                    // root of the tree comprising the EHnodes.
    EHNodeDsc* m_ehnNext;                    // root of the tree comprising the EHnodes.
    EITVerBasicBlock *m_verBBList;
    UINT       m_verBBCount;
    EHblkDsc   *m_compHndBBTab;
    IL_OFFSET  verInstrStartOffset;
    ReaderBaseNS::OPCODE verInstrOpcode;
    bool fNeedsByrefReturnCheck;
   

    
    void VerifyEIT(); // the entry point
    EITVerBasicBlock *verEITAddBlock(IL_OFFSET start, IL_OFFSET end);
    EITVerBasicBlock *verLookupBB(UINT32 x);

    void verInitEHTree(unsigned numEHClauses);
    void verInsertEhNode(CORINFO_EH_CLAUSE* clause, EHblkDsc* handlerTab);
    void verInsertEhNodeInTree(EHNodeDsc** ppRoot,  EHNodeDsc* node);
    void verInsertEhNodeParent(EHNodeDsc** ppRoot,  EHNodeDsc*  node);
    void verCheckNestingLevel(EHNodeDsc* root);
    void verDispHandlerTab();

    inline IL_OFFSET    ebdTryEndOffs(EHblkDsc * ehBlk);
    inline unsigned     ebdTryEndBlkNum(EHblkDsc * ehBlk);
    inline IL_OFFSET    ebdHndEndOffs(EHblkDsc * ehBlk);
    inline unsigned     ebdHndEndBlkNum(EHblkDsc * ehBlk);

    // =============================================================================
    // =======  EHRegion Builder       =============================================
    // =============================================================================

    void               rgnCreateRegionTree(void);
    void               rgnPushRegionChild(EHRegion* parent,EHRegion* child);
public:
    EHRegion*          rgnMakeRegion(ReaderBaseNS::RegionKind type, EHRegion* parent,
                                      EHRegion* regionRoot, EHRegionList** allRegionList);
private:

    // //////////////////////////////////////////////////////////////////////
    //                           Verification methods
    //
    // Opcode specific verification routines that "throw(verErr)" to insert
    // throw into the native code stream. You need the flowgraph node for the
    // throw object, the opcode for factoring and the curPtr to pick up the 
    // operand to any MSIL opcodes like the token for CEE_CALL etc.
    //
    // NOTE These could all be factored out into another class
    // //////////////////////////////////////////////////////////////////////

public:



    void VerifyCompatibleWith(const vertype &a, const vertype &b);
    void VerifyEqual(const vertype &a, const vertype &b);
    void VerifyAndReportFound(int cond, const vertype &v, HRESULT message);
    void VerifyAndReportFound(int cond, const vertype &v, __in __in_z char *message);
    void VerifyIsNumberType(const vertype &v);
    void VerifyIsIntegerType(const vertype &v);
    void VerifyIsObjRef(const vertype &v);
    void VerifyIsByref(const vertype &v);
    void VerifyIsBoxable(const vertype &v);
    void VerifyIsNotUnmanaged(const vertype &v);
    void VerifyTypeIsValid(const vertype &v);


    void PrintVerificationErrorMessage(VerErrType type, __in __in_z char *message, const vertype *expected, const vertype *encountered, mdToken token, bool andThrow);
    void PrintVerificationErrorMessage(VerErrType type, HRESULT message, const vertype *expected, const vertype *encountered, mdToken token, bool andThrow);
    void VerifyOrReturn(int cond, HRESULT code);
    void GVerifyOrReturn(int cond, HRESULT msg);
    void verGlobalError(HRESULT msg);
    void VerifyOrReturn(int cond, __in __in_z char *message);
    void GVerifyOrReturn(int cond, __in __in_z char *message);
    void verGlobalError(__in __in_z char *message);

    static LONG EEJITFilter(PEXCEPTION_POINTERS pExceptionPointers, LPVOID lpvParam);
    static LONG FieldFilter(PEXCEPTION_POINTERS pExceptionPointers, LPVOID lpvParam);

    CORINFO_CLASS_HANDLE         verifyClass(CORINFO_CLASS_HANDLE cls);
    CORINFO_METHOD_HANDLE        verifyMethodHandle(CORINFO_METHOD_HANDLE method);
    ptrdiff_t                    verifyStringHandle(ptrdiff_t val, void** ppIndir);

protected:
    // Client defined function to force verification.
    bool                verifyNeedsVerification(CorInfoCanSkipVerificationResult *);
    VerificationState * verifyInitializeBlock(FlowGraphNode *, unsigned int ilOffset);
    VerificationState * verCreateNewVState(unsigned maxstack, unsigned numLocals, bool fInitLocals, bool fThisInited);
    void                verifyFinishBlock(VerificationState* vstate, FlowGraphNode *);

    void verInitCurrentState();

    void VerifyRecordBranchForVerification(IRNode *branch, unsigned srcOffset, 
                                           unsigned targetOffset, bool isLeave);

    void VerifyRecordLocalType(int num, CorInfoType type, CORINFO_CLASS_HANDLE hclass);
    void VerifyRecordParamType(int num, CorInfoType type, CORINFO_CLASS_HANDLE hclass, bool makeByRef, bool isThis);
    void VerifyRecordParamType(int num, CORINFO_SIG_INFO* sig, CORINFO_ARG_LIST_HANDLE args);
    void VerifyRecordLocalType(int num, CORINFO_SIG_INFO* sig, CORINFO_ARG_LIST_HANDLE args);
    void VerifyPushExceptionObject(VerificationState *vstate, mdToken);
    void VerifyFieldAccess(VerificationState *vstate, ReaderBaseNS::OPCODE opcode,mdToken token);
    BOOL verIsCallToInitThisPtr(CORINFO_CLASS_HANDLE context, 
                                CORINFO_CLASS_HANDLE target);
    void VerifyLoadElemA(VerificationState *vstate, bool readOnlyPrefix, mdToken tok);
    void VerifyLoadElem(VerificationState *vstate, ReaderBaseNS::OPCODE opcode, mdToken token);
    void VerifyLoadConstant(VerificationState *vstate, ReaderBaseNS::OPCODE opcode);
    void VerifyStoreObj(VerificationState *vstate, mdToken tok);
    void VerifyLoadObj(VerificationState *vstate, mdToken tok);
    void VerifyStloc(VerificationState *vstate, unsigned locnum);
    void VerifyIsInst(VerificationState *vstate, mdToken tok);
    void VerifyCastClass(VerificationState *vstate, mdToken token);
    void VerifyBox(VerificationState *vstate, mdToken tok);
    void VerifyLoadAddr(VerificationState *vstate);
    void VerifyLoadToken(VerificationState *vstate, mdToken tok);
    void VerifyUnbox(VerificationState *vstate, mdToken typeRef);
    void VerifyStoreElemRef(VerificationState *vstate);
    void VerifyLdarg(VerificationState *vstate, unsigned locnum, ReaderBaseNS::OPCODE opcode);
    void VerifyStarg(VerificationState *vstate, unsigned locnum);
    void VerifyLdloc(VerificationState *vstate, unsigned locnum, ReaderBaseNS::OPCODE opcode);
    void VerifyStoreElem(VerificationState *vstate, ReaderBaseNS::StElemOpcode, mdToken tok);
    void VerifyLoadLen(VerificationState *vstate);
    void VerifyDup(VerificationState *vstate, const BYTE *codeAddr);
    void VerifyEndFilter(VerificationState *vstate, DWORD msilOffset);
    void VerifyInitObj(VerificationState *vstate, mdToken token);
    void VerifyCall(VerificationState *vstate, ReaderBaseNS::OPCODE opcode, 
                    bool tailCall, bool readOnlyCall, bool constraintCall, 
                    bool thisPossiblyModified,
                    mdToken constraintTypeRef, mdToken tok);
    void VerifyCpObj(VerificationState *vstate, mdToken token);
    void VerifyNewObj(VerificationState *vstate, ReaderBaseNS::OPCODE opcode, bool isTail, mdToken tok, const BYTE *codeAddr);
    void VerifyBoolBranch(VerificationState *vstate, unsigned int nextOffset, unsigned intTargetOffset);
    void VerifyLoadNull(VerificationState* vstate);
    void VerifyLoadStr(VerificationState *vstate, mdToken tok);
    void VerifyIntegerBinary(VerificationState *vstate);
    void VerifyBinary(VerificationState *vstate, ReaderBaseNS::OPCODE opcode);
    void VerifyShift(VerificationState *vstate);
    void VerifyReturn(VerificationState *vstate, EHRegion *region);
    void VerifyEndFinally(VerificationState *vstate);
    void VerifyThrow(VerificationState *vstate);
    void VerifyLoadFtn(VerificationState *vstate, ReaderBaseNS::OPCODE opcode, 
                       mdToken memberRef, const BYTE *codeAddr);
    void VerifyNewArr(VerificationState *vstate, mdToken token);
    void VerifyLoadIndirect(VerificationState *vstate, ReaderBaseNS::LdIndirOpcode opcode);
    void VerifyStoreIndir(VerificationState *vstate, ReaderBaseNS::StIndirOpcode opcode);
    void VerifyConvert(VerificationState *vstate, ReaderBaseNS::ConvOpcode opcode);
    void VerifyCompare(VerificationState *vstate, ReaderBaseNS::OPCODE opcode);
    void VerifyUnary(VerificationState *vstate,ReaderBaseNS::UnaryOpcode opcode);
    void VerifyPop(VerificationState *vstate);    
    void VerifyArgList(VerificationState *vstate);
    void VerifyCkFinite(VerificationState *vstate);
    void VerifyFailure(VerificationState *vstate);
    void VerifyToken(mdToken token);
    void VerifyRefAnyVal(VerificationState *vstate, mdToken typeRef);
    void VerifyRefAnyType(VerificationState *vstate);
    void VerifyUnboxAny(VerificationState *vstate, mdToken typeRef);
    void VerifySwitch(VerificationState *vstate);
    void VerifyMkRefAny(VerificationState *vstate, mdToken typeRef);
    void VerifySizeOf(VerificationState *vstate, mdToken typeRef);
    void VerifyRethrow(VerificationState *vstate, EHRegion *region);
    void VerifyTail(VerificationState *vstate, EHRegion *region);
    void VerifyConstrained(VerificationState *vstate, mdToken typeDefOrRefOrSpec);
    void VerifyReadOnly(VerificationState *vstate);
    void VerifyVolatile(VerificationState *vstate);
    void VerifyUnaligned(VerificationState *vstate, ReaderAlignType alignment);
    void VerifyPrefixConsumed(VerificationState *vstate, ReaderBaseNS::OPCODE opcode);
    void VerifyLeave(VerificationState *vstate);

    void VerifyBranchTarget(VerificationState *vstate, FlowGraphNode *currentFGNode, 
                            EHRegion *srcRegion, unsigned int targetOffset, bool isLeave);
    void VerifyReturnFlow(unsigned int srcOffset);

    void VerifyFallThrough(VerificationState *vs, FlowGraphNode *fg);

    bool verCheckDelegateCreation(ReaderBaseNS::OPCODE opcode,
                                  VerificationState *vstate,
                                  const BYTE *codeAddr, mdMemberRef &targetMemberRef,
                                  vertype ftnType, vertype objType);
    
    void verVerifyCall (ReaderBaseNS::OPCODE    opcode,
                        mdToken                 memberRef,
                        bool                    tailCall,
                        const BYTE*             codeAddr,
                        VerificationState *     vstate
                        );

    void verPushExceptionObject(VerificationState *vstate, mdToken tok);
    void verVerifyField(CORINFO_FIELD_HANDLE fldHnd, CORINFO_CLASS_HANDLE enclosingClassHnd, const vertype* tiThis, 
                        unsigned fieldFlags, BOOL mutator);
    void VerifyIsDirectCallToken(mdToken token);
    void VerifyIsCallToken(mdToken token);
    bool verIsValueClass(CORINFO_CLASS_HANDLE clsHnd);
    bool verIsBoxedValueType(const vertype &v);
    static bool verIsCallToken(mdToken token);
    bool verIsValClassWithStackPtr(CORINFO_CLASS_HANDLE clsHnd);
    bool verIsGenericTypeVar(CORINFO_CLASS_HANDLE clsHnd);
    void verDumpType(const vertype &v);
    bool verNeedsCtorTrack();
    void VerifyIsClassToken(mdToken token);
    void VerifyIsFieldToken(mdToken token);

    vertype verVerifySTIND(const vertype& ptr, const vertype& value, ti_types instrType);
    vertype verVerifyLDIND(const vertype& ptr, ti_types instrType );

    // methods
    vertype verGetArrayElemType(vertype ti);
    vertype verMakeTypeInfo(CORINFO_CLASS_HANDLE clsHnd);
    vertype verMakeTypeInfo(CorInfoType ciType, CORINFO_CLASS_HANDLE clsHnd);
    vertype verParseArgSigToTypeInfo(CORINFO_SIG_INFO* sig, CORINFO_ARG_LIST_HANDLE args);

    CORINFO_CLASS_HANDLE verGetExactMethodClass(mdMemberRef token, CORINFO_METHOD_HANDLE method);
    CORINFO_CLASS_HANDLE verGetExactFieldClass(mdMemberRef token, CORINFO_FIELD_HANDLE method);
    
    CORINFO_CLASS_HANDLE getTokenTypeAsHandle (unsigned metaTOK, CORINFO_MODULE_HANDLE scope);


    CORINFO_METHOD_HANDLE  eeFindHelper        (ptrdiff_t helper);
    CorInfoHelpFunc        eeGetHelperNum      (CORINFO_METHOD_HANDLE  method);
    CORINFO_FIELD_HANDLE   eeFindJitDataOffs   (ptrdiff_t dataOffs);
    ptrdiff_t              eeGetJitDataOffs    (CORINFO_FIELD_HANDLE  field);
    bool                   eeIsNativeMethod    (CORINFO_METHOD_HANDLE method);
    CORINFO_METHOD_HANDLE  eeMarkNativeTarget  (CORINFO_METHOD_HANDLE method);
    CORINFO_METHOD_HANDLE  eeGetMethodHandleForNative (CORINFO_METHOD_HANDLE method);
    unsigned eeGetMethodAttribs (CORINFO_METHOD_HANDLE    methodHandle);
    void     eeGetMethodSig     (CORINFO_METHOD_HANDLE  methHnd,
                                 CORINFO_SIG_INFO*      sigRet,
                                 bool                   giveUp,
                                 CORINFO_CLASS_HANDLE   owner=NULL);
    void     eeGetCallSiteSig   (unsigned sigTok,
                                 CORINFO_MODULE_HANDLE scope,
                                 CORINFO_METHOD_HANDLE context,
                                 CORINFO_SIG_INFO*     sigRet,
                                 bool                  giveUp = true);
        
    void VerifyIsSDArray(const vertype &ti);
    bool verIsByRefLike(const vertype& ti);
    bool verIsSafeToReturnByRef(const vertype& ti);
    BOOL verIsBoxable(const vertype& ti);
    CORINFO_CLASS_HANDLE verGetClassHandle(mdToken tok, 
                                           CorInfoTokenKind tokenKind=CORINFO_TOKENKIND_Default);

public:
    // ///////////////////////////////////////////////////////////////////////////
    //                           VOS opcode methods
    // These methods are available to help implement codegen for cee opcodes.
    // These methods are implemented using the optional client methods declared
    // at the end of this file.
    // ///////////////////////////////////////////////////////////////////////////

    IRNode* rdrCall(ReaderBaseNS::CallOpcode opcode, 
                    mdToken token, mdToken constraintTypeRef,
                    mdToken loadFtnToken,
                    bool fReadonlyCallPrefix, bool fTailCallPrefix, 
                    bool fIsUnmarkedTailCall,
                    IRNode** callNode, IRNode** newIR);

private:
    void rdrMakeCallTargetNode(ReaderCallTargetData *callTargetData, IRNode **thisPtr, IRNode **newIR);
    IRNode* rdrGetDirectCallTarget(ReaderCallTargetData *callTargetData, IRNode **newIR);
    IRNode* rdrGetCodePointerLookupCallTarget(ReaderCallTargetData *callTargetData, IRNode **newIR);
    IRNode* rdrGetIndirectVirtualCallTarget(ReaderCallTargetData *callTargetData, IRNode **thisPtr, IRNode **newIR);
    IRNode* rdrGetVirtualStubCallTarget(ReaderCallTargetData *callTargetData, IRNode **newIR);
    IRNode* rdrGetVirtualTableCallTarget(ReaderCallTargetData *callTargetData, IRNode **thisPtr, IRNode **newIR);

    // Delegate invoke and delegate construct optimizations
    bool    rdrCallIsDelegateInvoke(ReaderCallTargetData *callTargetData);
    bool    rdrCallIsDelegateConstruct(ReaderCallTargetData *callTargetData);
    IRNode* rdrGetDelegateInvokeTarget(ReaderCallTargetData *callTargetData, IRNode **thisPtr, IRNode **newIR);

    // Called by castClass/isInst code (in fg build and in builder)
    bool    rdrCanOptimizeCastClass(mdToken token, CorInfoHelpFunc *pHelperId);
    bool    rdrCanOptimizeIsInst(mdToken token, CorInfoHelpFunc *pHelperId);

public:
    void    rdrCallFieldHelper(mdToken token,bool fLoad,
                               IRNode* dst,   // dst node if this is a load, otherwise NULL
                               IRNode* tupObj, IRNode* tupValue,
                               ReaderAlignType classAlignment,
                               bool fVolatile, IRNode** newIR);
    void    rdrCallWriteBarrierHelper(IRNode* arg1, IRNode* arg2,
                                      ReaderAlignType alignment, bool fVolatile, IRNode** newIR,
                                      mdToken token, bool nonValueClass, bool fValueIsPointer, bool fField);
    void    rdrCallWriteBarrierHelperForReturnValue(IRNode* arg1, IRNode* arg2, IRNode** newIR, mdToken token);
    IRNode* rdrGetFieldAddress(mdToken token,IRNode* ptObj,bool fBaseIsGCObj,bool fBaseIsThis,
                               bool* fNullCheckAdded,IRNode** newIR);

   virtual bool GenIR_DisableCastClassOptimization() = 0;
   virtual bool GenIR_DisableIsInstOptimization() = 0;
   virtual bool GenIR_IsTmpVarAndReg( IRNode *val ) = 0;

public:
    // //////////////////////////////////////////////////////////////////////////
    // Metadata Accessors
    // //////////////////////////////////////////////////////////////////////////

    // routines to map token to handle.
    CORINFO_METHOD_HANDLE getMethodHandle(mdToken token);
    CORINFO_METHOD_HANDLE getMethodHandle(mdToken token, CORINFO_METHOD_HANDLE context, CORINFO_MODULE_HANDLE scope);
    CORINFO_CLASS_HANDLE  getClassHandle(mdToken token);
    CORINFO_CLASS_HANDLE  getClassHandle(mdToken token, CORINFO_METHOD_HANDLE context, CORINFO_MODULE_HANDLE scope);
    CORINFO_CLASS_HANDLE  getClassHandle(mdToken token, CORINFO_METHOD_HANDLE context, CORINFO_MODULE_HANDLE scope, CorInfoTokenKind tokenKind);

    InfoAccessType        constructStringLiteral(mdToken token,void **ppInfo);

    CORINFO_MODULE_HANDLE getClassModuleForStatics(CORINFO_CLASS_HANDLE clsHnd);
    void *                getEmbedModuleDomainID(CORINFO_MODULE_HANDLE mdlHnd,bool *pfIndirect);
    void *                getEmbedClassDomainID(CORINFO_CLASS_HANDLE clsHnd,bool *pfIndirect);

    CORINFO_METHOD_HANDLE embedMethodHandle(CORINFO_METHOD_HANDLE methodHandle, bool *pfIndirect);
    CORINFO_CLASS_HANDLE  embedClassHandle(CORINFO_CLASS_HANDLE classHandle, bool *pfIndirect);
    CORINFO_FIELD_HANDLE  embedFieldHandle(CORINFO_FIELD_HANDLE fieldHandle, bool* pfIndirect);

    void                  getCallSiteSignature(CORINFO_METHOD_HANDLE hMethod,mdToken token,
                                               CORINFO_SIG_INFO* pSig,bool* pHasThis);
    void                  getCallSiteSignature(CORINFO_METHOD_HANDLE hMethod,mdToken token,
                                               CORINFO_SIG_INFO* pSig,bool* pHasThis,
                                               CORINFO_METHOD_HANDLE context, CORINFO_MODULE_HANDLE scope);

    // Gets a CALL_INFO with only the virtual call kind filled in
    void                  getVirtualCallKind(mdToken methodToken,
                                             mdToken constraintToken,
                                             CORINFO_CALL_INFO *pResult);

    // Gets a CALL_INFO with all virutal call info filled in
    void                  getCallInfo(mdToken methodToken,
                                      mdToken constraintToken,
                                      bool fVirtual,
                                      CORINFO_CALL_INFO *pResult);

    void                  getCallInfo(mdToken methodToken,
                                      mdToken constraintToken,
                                      bool fVirtual,
                                      CORINFO_CALL_INFO *pResult, 
                                      CORINFO_METHOD_HANDLE context,
                                      CORINFO_MODULE_HANDLE scope);

    unsigned int          getClassNumInstanceFields(CORINFO_CLASS_HANDLE hClass);
    CORINFO_FIELD_HANDLE  getFieldInClass(CORINFO_CLASS_HANDLE hClass, unsigned int ordinal);
    CorInfoType           getFieldInfo(CORINFO_CLASS_HANDLE hClass, unsigned int ordinal, unsigned int* fieldOffset, CORINFO_CLASS_HANDLE* hFieldClass);

    // Properties of current method.
    bool                  isZeroInitLocals(void);
    unsigned int          getCurrentMethodNumAutos(void);
    CORINFO_METHOD_HANDLE getCurrentMethodHandle(void);
    CORINFO_CLASS_HANDLE  getCurrentMethodClass(void);
    unsigned              getCurrentMethodHash(void);
    DWORD                 getCurrentMethodAttribs(void);
    char*                 getCurrentMethodName(const char** modName);
    void                  getCurrentMethodSigData(CorInfoCallConv* conv,
                                                  CorInfoType* returnType,
                                                  CORINFO_CLASS_HANDLE* returnClass,
                                                  int* totalILArgs,bool* isVarArg,
                                                  bool* hasThis,unsigned __int8* retSig);

    // Get entry point for function (used *only* by direct calls)
    void                 getFunctionEntryPoint(CORINFO_METHOD_HANDLE    ftn,   
                                                  InfoAccessType        requestedAccessType, 
                                                  CORINFO_CONST_LOOKUP *pResult,
                                                  CORINFO_ACCESS_FLAGS  accessFlags);

    // Get entry point for function (used by ldftn, ldvirtftn)
    void                 getFunctionFixedEntryPointInfo(mdToken token,
                                                        CORINFO_LOOKUP *pResult);

    //
    // Module
    //
    CORINFO_MODULE_HANDLE  getCurrentModuleHandle(void);

    //
    // Properties of current jitinfo.
    // These functions assume the context of the current module and method info.
    //

    // Finds name of MemberRef or MethodDef token
    void findNameOfToken(mdToken token, __out_ecount(cchBuffer) char * szBuffer, size_t cchBuffer);

    //
    // class
    //
public:
    CORINFO_CLASS_HANDLE getMethodClass   (CORINFO_METHOD_HANDLE handle);
    unsigned long        getMethodVTableOffset(CORINFO_METHOD_HANDLE handle);
    const char*          getClassName     (CORINFO_CLASS_HANDLE hClass);
    INT                  getClassCustomAttribute(CORINFO_CLASS_HANDLE clsHnd, LPCSTR attrib, const BYTE** ppVal);
    PGCLAYOUT_STRUCT     getClassGCLayout (CORINFO_CLASS_HANDLE hClass);
    DWORD                getClassAttribs  (CORINFO_CLASS_HANDLE hClass);
    DWORD                getClassAttribs  (CORINFO_CLASS_HANDLE hClass, CORINFO_METHOD_HANDLE context);
    unsigned int         getClassSize     (CORINFO_CLASS_HANDLE hClass);
    CorInfoType          getClassType     (CORINFO_CLASS_HANDLE hClass);
    unsigned __int8 *    getValueClassSig (CORINFO_CLASS_HANDLE hClass);
    void                 getClassType     (CORINFO_CLASS_HANDLE hClass, DWORD attribs, 
                                            CorInfoType* pCorInfoType, unsigned int* pSize,unsigned __int8** pRetSig);
    BOOL                 canInlineTypeCheckWithObjectVTable(CORINFO_CLASS_HANDLE hClass);
    bool                 accessStaticFieldRequiresClassConstructor(CORINFO_FIELD_HANDLE);

    // Class Alignment
private:
    unsigned int         getClassAlignmentRequirement(CORINFO_CLASS_HANDLE);
    void*                getMethodSync(bool *fIndirect);

public:
    ReaderAlignType      getMinimumClassAlignment(CORINFO_CLASS_HANDLE hClass, ReaderAlignType alignment);

    CorInfoHelpFunc      getNewArrHelper(CORINFO_CLASS_HANDLE hndElementType);

    void*                getAddrOfCaptureThreadGlobal(bool* pfIndirect);

    //
    // field
    //
public:
    CORINFO_FIELD_HANDLE getFieldHandle   (mdToken token);
    CORINFO_FIELD_HANDLE getFieldHandle   (mdToken token, CORINFO_METHOD_HANDLE context, CORINFO_MODULE_HANDLE scope);
    const char*          getFieldName     (CORINFO_FIELD_HANDLE,const char** pModName);
    CORINFO_CLASS_HANDLE getFieldClass    (CORINFO_FIELD_HANDLE);
    DWORD                getFieldAttribs  (CORINFO_FIELD_HANDLE hField,
                                           CORINFO_ACCESS_FLAGS flags);
    CorInfoType          getFieldType     (CORINFO_FIELD_HANDLE, CORINFO_CLASS_HANDLE *hClass,
                                           CORINFO_CLASS_HANDLE owner=NULL /* Optional: for verification */);
    CorInfoHelpFunc      getFieldHelper   (CORINFO_FIELD_HANDLE, CorInfoFieldAccess helperId);
    CorInfoHelpFunc      getSharedStaticBaseHelper(CORINFO_FIELD_HANDLE, bool fRuntimeLookup);
    CorInfoHelpFunc      getSharedCCtorHelper(CORINFO_CLASS_HANDLE);
    CORINFO_CLASS_HANDLE  getTypeForBox(CORINFO_CLASS_HANDLE  cls); 
    CorInfoHelpFunc      getBoxHelper(CORINFO_CLASS_HANDLE);
    CorInfoHelpFunc      getUnBoxHelper(CORINFO_CLASS_HANDLE, BOOL*);
    unsigned int         getFieldOffset   (CORINFO_FIELD_HANDLE);

    mdToken              getMemberParent  (mdToken token);
    mdToken              getMemberParent  (CORINFO_MODULE_HANDLE context, mdToken token);

    void*                getStaticFieldAddress(CORINFO_FIELD_HANDLE fieldHandle, ULONG* offset, bool* fBoxed,
                                                bool* fIndirect);

    //
    // method
    //
    char*                getMethodName    (CORINFO_METHOD_HANDLE,const char** modName);
    DWORD                getMethodAttribs(CORINFO_METHOD_HANDLE handle);
    DWORD                getMethodAttribs(CORINFO_METHOD_HANDLE handle, CORINFO_METHOD_HANDLE context);
    void                 setMethodAttribs(CORINFO_METHOD_HANDLE handle, CorInfoMethodRuntimeFlags flag);

    BOOL                 checkMethodModifier(CORINFO_METHOD_HANDLE hMethod, LPCSTR modifier, BOOL fOptional);
    mdToken              getMethodDefFromMethod(CORINFO_METHOD_HANDLE handle);
    void                 getMethodSig     (CORINFO_METHOD_HANDLE handle,CORINFO_SIG_INFO *sig);
    const char*          getMethodRefInfo (CORINFO_METHOD_HANDLE handle,CorInfoCallConv *pConv,
                                           CorInfoType *pCorType,CORINFO_CLASS_HANDLE *pRetTypeClass,
                                           const char **pModName);
    void                 getMethodSigData (CorInfoCallConv* conv,CorInfoType* returnType,
                                           CORINFO_CLASS_HANDLE* returnClass,int* totalILArgs,
                                           bool* isVarArg,bool* hasThis,unsigned __int8* retSig);
    void                 getMethodInfo(CORINFO_METHOD_HANDLE handle, CORINFO_METHOD_INFO *info);
        
    bool                 isPrimitiveType(CORINFO_CLASS_HANDLE handle);
    static bool          isPrimitiveType(CorInfoType CorInfoType);

#if defined(CC_CALL_INTERCEPT)
    CorInfoIsCallAllowedResult  isCallAllowedByHost (CORINFO_METHOD_HANDLE callerHandleHandle, 
                                                     CORINFO_METHOD_HANDLE methodHandle, 
                                                     CORINFO_CALL_ALLOWED_INFO * callAllowedByHostResult);
    void                 checkCallAuthorization (mdToken, CORINFO_METHOD_HANDLE, IRNode** newIR);
#endif
    bool                 canTailCall(CORINFO_METHOD_HANDLE target,bool hasThis,bool isTailPrefix);
    CorInfoInline        canInline(CORINFO_METHOD_HANDLE caller,CORINFO_METHOD_HANDLE target,DWORD *pRestrictions);
    CORINFO_ARG_LIST_HANDLE getArgNext(CORINFO_ARG_LIST_HANDLE args);
    CorInfoTypeWithMod getArgType(CORINFO_SIG_INFO *sig, CORINFO_ARG_LIST_HANDLE args, CORINFO_CLASS_HANDLE *vcTypeRet);
    CORINFO_CLASS_HANDLE getArgClass (CORINFO_SIG_INFO *sig, CORINFO_ARG_LIST_HANDLE args);
    CORINFO_CLASS_HANDLE getBuiltinClass(CorInfoClassId classId);
    CorInfoType getChildType (CORINFO_CLASS_HANDLE clsHnd, CORINFO_CLASS_HANDLE *clsRet);
    BOOL isSDArray(CORINFO_CLASS_HANDLE hClass);
    unsigned getArrayRank(CORINFO_CLASS_HANDLE hClass);

    void*                getHelperDescr   (CorInfoHelpFunc helpFuncId,bool *fIndirect,InfoAccessModule* pAccessModule);
    CorInfoHelpFunc      getNewHelper(CORINFO_CLASS_HANDLE classHandle, unsigned classToken, CORINFO_MODULE_HANDLE tokenContext);
    CorInfoHelpFunc      getNewHelper(CORINFO_CLASS_HANDLE classHandle, CORINFO_METHOD_HANDLE context, unsigned classToken, CORINFO_MODULE_HANDLE tokenContext);

    void*                getVarArgsHandle(CORINFO_SIG_INFO *pSig, bool *fIndirect);

    void*                getJustMyCodeHandle(CORINFO_METHOD_HANDLE handle, bool *fIndirect);
    
    void*                getCookieForPInvokeCalliSig(CORINFO_SIG_INFO *sigTarget, bool* fIndirect);
    void*                getAddressOfPInvokeFixup(CORINFO_METHOD_HANDLE methodHandle, InfoAccessType *accessType);
    void *               getPInvokeUnmanagedTarget(CORINFO_METHOD_HANDLE methodHandle);

    bool                 pInvokeMarshalingRequired(CORINFO_METHOD_HANDLE mHandle, CORINFO_SIG_INFO *sig);

    // Get a node that can be passed to the sync method helpers.
   IRNode*               rdrGetCritSect(IRNode **newIR);

    //
    // Support for filtering runtime-thrown exceptions
    //
    int                  __RuntimeFilter(struct _EXCEPTION_POINTERS *pExceptionPointers);

    // ESSA force override in the client that inherits the reader base.
    // Will revisit this decision but for now its simply in the client.

    virtual void         Ssa_LocAddressTaken(unsigned int idx) = 0;
    virtual void         Ssa_ArgAddressTaken(unsigned int idx) = 0;
    virtual void         Ssa_VirtPhiPass(FlowGraphNode* h) = 0;
    virtual void         Ssa_OptRenameInit() = 0;
    virtual void         Ssa_CreateDefMap(int *ssaParamCnt, int *ssaLocalCnt) = 0;
    virtual void         Ssa_AddDefPoint(unsigned int def, int offset, bool isLocal) = 0;
    virtual int          Ssa_HshMSILScalar(unsigned int opcode, BYTE* operand, FlowGraphNode *fg) = 0;
    virtual void         Ssa_LookAheadEdge(FlowGraphNode * curr, FlowGraphNode * succ, FlowGraphEdgeList * edge) = 0;
    virtual void         Ssa_OptPop(FlowGraphNode *block) = 0;
    virtual void         Ssa_PushPhis(FlowGraphNode *block) = 0;

    // KINTERF
    virtual void         Itf_RegisterNumberOfIndirs(int numIndirs) = 0;

    // ////////////////////////////////////////////////////////////////////
    //             IL generation methods supplied by the clients
    //
    // All pure virtual routines must be implemented by the client, non-pure
    // virtual routines have a default implementation in the reader, but can
    // be overloaded if necessary.
    // /////////////////////////////////////////////////////////////////////

    // MSIL Routines - client defined routines that are invoked by the reader.
    //                 One will be called for each msil opcode.

    virtual void           GenIR_OpcodeDebugPrint(BYTE* pbuf, unsigned startOffset, unsigned endOffset) = 0;

    // Used for testing, client can force verification.
    virtual bool           verForceVerification(void) = 0;

    virtual bool           GenIR_Abs(IRNode* arg1, IRNode** retVal, IRNode** newIR) = 0;

    virtual IRNode*        GenIR_ArgList(IRNode** newIR) = 0;
    virtual IRNode*        GenIR_InstParam(IRNode** newIR) = 0;
    virtual IRNode*        GenIR_ThisObj(IRNode** newIR) = 0;
    virtual void           GenIR_BoolBranch(ReaderBaseNS::BoolBranchOpcode opcode,
                                            IRNode* arg1,IRNode** newIR) = 0;
    virtual IRNode*        GenIR_Box(mdToken token, IRNode* arg1,IRNode** newIR);
    virtual IRNode*        GenIR_BinaryOp(ReaderBaseNS::BinaryOpcode opcode,
                                          IRNode*        arg1,
                                          IRNode*        arg2, 
                                          IRNode**       newIR) = 0;
    virtual void           GenIR_Branch(IRNode** newIR) = 0;
    virtual void           GenIR_Break(IRNode** newIR);
    virtual IRNode*        GenIR_Call(ReaderBaseNS::CallOpcode opcode, mdToken token,
                                    mdToken constraintTypeRef, bool readOnlyPrefix,
                                    bool tailCallPrefix, bool isUnmarkedTailCall, IRNode** newIR) = 0;
    virtual IRNode*        GenIR_CastClass(mdToken token, IRNode* arg1, IRNode** newIR,
                                    CorInfoHelpFunc helperId = /*DEFAULT*/ CORINFO_HELP_UNDEF);
    virtual IRNode*        GenIR_IsInst(mdToken token,IRNode* arg2,IRNode** newIR,
                                    CorInfoHelpFunc helperId = /*DEFAULT*/ CORINFO_HELP_UNDEF);

    virtual void           GenIR_CastClassOptimized(mdToken token, IRNode** newIR);
    virtual void           GenIR_IsInstOptimized(mdToken token, IRNode** newIR);
    virtual void           GenIR_OptimizedCast(mdToken token, CorInfoHelpFunc helperId, IRNode** newIR);

    virtual IRNode*        GenIR_CkFinite(IRNode* arg1,IRNode** newIR) = 0;
    virtual IRNode*        GenIR_Cmp(ReaderBaseNS::CmpOpcode opode,
                                     IRNode* arg1,IRNode* arg2,IRNode** newIR) = 0;
    virtual void           GenIR_CondBranch(ReaderBaseNS::CondBranchOpcode opcode,
                                            IRNode*        arg1,
                                            IRNode*        arg2,
                                            IRNode**       newIR) = 0;
    virtual IRNode*        GenIR_Conv(ReaderBaseNS::ConvOpcode opcode,
                                     IRNode* arg1,IRNode** newIR) = 0;
    virtual void           GenIR_CpBlk(IRNode *byteCnt, IRNode* sourceAddress,
                                       IRNode* destAddress, ReaderAlignType alignment, bool volatile,
                                       IRNode** newIR);
    virtual void           GenIR_CpObj(mdToken token, IRNode* arg1,IRNode* arg2, 
                                       ReaderAlignType alignment, bool fVolatile,
                                       IRNode** newIR);
    virtual void           GenIR_Dup(IRNode* opr, IRNode** result1, IRNode** result2,
                                     IRNode** newIR) = 0;
    virtual void           GenIR_EndFilter(IRNode* arg1, IRNode** newIR) = 0;
    virtual IRNode*        GenIR_GetStaticFieldAddress(mdToken token, IRNode** newIR);
    virtual IRNode*        GenIR_GetFieldAddress(mdToken token, IRNode* ptObj, bool fObjIsThis,
                                                 bool* fNullCheckAdded, IRNode** newIR) = 0;
    virtual void           GenIR_InitBlk(IRNode* numBytes, IRNode* valuePerByte,
                                         IRNode* destAddress, ReaderAlignType alignment,
                                         bool volatile, IRNode** newIR);
    virtual void           GenIR_InitObj(mdToken token,IRNode* arg2,IRNode** newIR);
    virtual void           GenIR_InsertThrow(CorInfoHelpFunc throwHelper,unsigned int offset,IRNode **newIR);
    virtual void           GenIR_Jmp(ReaderBaseNS::CallOpcode opcode,mdToken token, bool hasThis,
                                       bool hasVarArg, IRNode** newIR) = 0;

    virtual void           GenIR_Leave(unsigned int targetOffset,
                                       bool isNonLocal,bool endsWithNonLocalGoto,IRNode **newIR) = 0;
#ifdef READER_ESSA_RENAME
    virtual IRNode*        GenIR_LoadArg(unsigned int argOrdinal, bool isJmp, IRNode** newIR, int ssaIdx, FlowGraphNode* block) = 0;
    virtual IRNode*        GenIR_LoadLocal(unsigned int argOrdinal, IRNode** newIR, int ssaIdx, FlowGraphNode* block) = 0;
#else
    virtual IRNode*        GenIR_LoadArg(unsigned int argOrdinal, bool isJmp, IRNode** newIR) = 0;
    virtual IRNode*        GenIR_LoadLocal(unsigned int argOrdinal, IRNode** newIR) = 0;
#endif
    virtual IRNode*        GenIR_LoadArgAddress(unsigned int argOrdinal,IRNode** newIR) = 0;
    virtual IRNode*        GenIR_LoadLocalAddress(unsigned int locOrdinal,IRNode** newIR) = 0;
    virtual IRNode*        GenIR_LoadConstantI4(int constant,IRNode** newIR) = 0;
    virtual IRNode*        GenIR_LoadConstantI8(__int64 constant,IRNode** newIR) = 0;
    virtual IRNode*        GenIR_LoadConstantI(size_t constant,IRNode** newIR) = 0;
    virtual IRNode*        GenIR_LoadConstantR4(float value,IRNode** newIR) = 0;
    virtual IRNode*        GenIR_LoadConstantR8(double value,IRNode** newIR) = 0;
    virtual IRNode*        GenIR_LoadElem( ReaderBaseNS::LdElemOpcode opcode,
                                         mdToken token, IRNode* arg1, IRNode* arg2,
                                         IRNode** newIR) = 0;
    virtual IRNode*        GenIR_LoadElemA(mdToken token, IRNode* arg1, IRNode* arg2,
                                         bool fReadOnly, IRNode** newIR) = 0;
    virtual IRNode*        GenIR_LoadField(mdToken token,
                                          IRNode* arg1,ReaderAlignType alignment,
                                          bool fVolatile,IRNode** newIR) = 0;
    virtual IRNode*        GenIR_LoadFuncptr(mdToken token,CORINFO_LOOKUP *pLookup,IRNode** newIR) = 0;
    virtual IRNode*        GenIR_LoadIndir(ReaderBaseNS::LdIndirOpcode opcode,
                                             IRNode* addr,ReaderAlignType alignement,
                                             bool fVolatile,bool fInterfReadOnly,IRNode** newIR);
    virtual IRNode*        GenIR_LoadNull(IRNode** newIR) = 0;
    virtual IRNode*        GenIR_LocalAlloc(IRNode* arg, bool zeroInit, IRNode** newIR) = 0;
    virtual IRNode*        GenIR_LoadFieldAddress( mdToken token,IRNode* ptObj,
                                                 IRNode** newIR) = 0;
    virtual IRNode*        GenIR_LoadLen(IRNode* arg1,IRNode** newIR) = 0;
    virtual bool           GenIR_ArrayAddress(CORINFO_SIG_INFO *sig, IRNode **retVal, IRNode **newIR) = 0;
    virtual IRNode*        GenIR_LoadStringLen(IRNode* arg1,IRNode** newIR) = 0;
    virtual IRNode*        GenIR_ArrayGetDimLength(IRNode* arg1, IRNode* arg2, CORINFO_CALL_INFO *callInfo, IRNode** newIR) = 0;
    virtual IRNode*        GenIR_LoadObj(mdToken token,IRNode* arg1,ReaderAlignType alignmentPrefix,
                                         bool fVolatile, bool isField, IRNode** newIR);
    virtual IRNode*        GenIR_LoadAndBox(mdToken token,IRNode* addr,ReaderAlignType alignmentPrefix,IRNode** newIR);
    virtual IRNode*        GenIR_LoadStaticField( mdToken token,
                                                bool fVolatile,IRNode** newIR) = 0;
    virtual IRNode*        GenIR_LoadStr(mdToken token,IRNode** newIR);
    virtual IRNode*        GenIR_LoadToken(mdToken token,IRNode** newIR);
    virtual IRNode*        GenIR_LoadVirtFunc(IRNode* arg1, 
                                             mdToken token,IRNode** newIR) = 0;
    virtual IRNode*        GenIR_LoadPrimitiveType(IRNode* addr, CorInfoType CorInfoType,
                                ReaderAlignType alignment, bool fVolatile,
                                bool fInterfConst, IRNode** newIR) = 0;
    virtual IRNode*        GenIR_LoadNonPrimitiveObj(IRNode* addr, CORINFO_CLASS_HANDLE classHandle,
                                ReaderAlignType alignment, bool fVolatile, IRNode** newIR) = 0;
    virtual IRNode*        GenIR_MakeRefAny(mdToken token,IRNode* object,
                                           IRNode** newIR) = 0;
    virtual IRNode*        GenIR_NewArr(mdToken token,IRNode* arg1,
                                       IRNode** newIR) = 0;
    virtual IRNode*        GenIR_NewObj(mdToken token, mdToken loadFtnToken, IRNode** newIR) = 0;
    virtual void           GenIR_Pop( IRNode* opr, IRNode** newIR) = 0;
    virtual IRNode*        GenIR_RefAnyType(IRNode* arg1,IRNode** newIR) = 0;
    virtual IRNode*        GenIR_RefAnyVal(IRNode* val,
                                          mdToken token,IRNode** newIR);
    virtual void           GenIR_Rethrow(IRNode** newIR) = 0; 
    virtual void           GenIR_Return(IRNode* opr,bool synchronousMethod,bool needsByrefReturnCheck, IRNode** newIR) = 0;
    virtual IRNode*        GenIR_Shift(ReaderBaseNS::ShiftOpcode opcode,
                                      IRNode* shiftAmount,IRNode* shiftOperand,IRNode** newIR) = 0;
    virtual IRNode*        GenIR_Sizeof(mdToken token,
                                       IRNode** newIR) = 0;
#ifdef READER_ESSA_RENAME
    virtual void           GenIR_StoreArg(unsigned int locOrdinal,
                                         IRNode* arg1,ReaderAlignType alignment,bool fVolatile,IRNode** newIR,
                                         int ssaIdx, FlowGraphNode * block) = 0;
#else
    virtual void           GenIR_StoreArg(unsigned int locOrdinal,
                                         IRNode* arg1,ReaderAlignType alignment,bool fVolatile,IRNode** newIR) = 0;
#endif
    virtual void           GenIR_StoreElem(ReaderBaseNS::StElemOpcode opcode,mdToken token,
                                          IRNode* arg1, IRNode* arg2, IRNode* arg3,IRNode** newIR) = 0;
    virtual void           GenIR_StoreElemRefAny(IRNode* value,IRNode* index,IRNode* obj,IRNode** newIR);
    virtual void           GenIR_StoreField(mdToken token,IRNode* arg1,
                                           IRNode* arg2,ReaderAlignType alignment,
                                           bool fVolatile,IRNode** newIR) = 0;
    virtual void           GenIR_StoreIndir(ReaderBaseNS::StIndirOpcode opcode,
                                              IRNode* arg1, IRNode* arg2, ReaderAlignType alignment,
                                              bool volatilePrefix,IRNode** newIR);
    virtual void           GenIR_StorePrimitiveType(IRNode* value, IRNode* addr, CorInfoType CorInfoType,
                                ReaderAlignType alignment, bool fVolatile, IRNode** newIR) = 0;
#ifdef READER_ESSA_RENAME
    virtual void           GenIR_StoreLocal(unsigned int locOrdinal,
                                           IRNode* arg1,ReaderAlignType alignment,bool fVolatile,IRNode** newIR,
                                           int ssaIdx, FlowGraphNode * block) = 0;
#else
    virtual void           GenIR_StoreLocal(unsigned int locOrdinal,
                                           IRNode* arg1,ReaderAlignType alignment,bool fVolatile,IRNode** newIR) = 0;
#endif
    virtual void           GenIR_StoreObj(mdToken token,IRNode* arg1,
                                         IRNode* arg2,ReaderAlignType alignment,
                                         bool fVolatile,bool fField,IRNode** newIR);
    virtual void           GenIR_StoreStaticField(mdToken token,
                                                 IRNode* arg1,bool fVolatile,IRNode** newIR) = 0;
    virtual IRNode*        GenIR_StringGetChar(IRNode* arg1,IRNode* arg2,IRNode** newIR) = 0;
    virtual bool           GenIR_Sqrt(IRNode* arg1, IRNode** retVal, IRNode** newIR) = 0;
    virtual void           GenIR_Switch(IRNode* opr, IRNode** newIR) = 0;
    virtual void           GenIR_Throw( IRNode* arg1, IRNode** newIR) = 0;
    virtual IRNode*        GenIR_UnaryOp(ReaderBaseNS::UnaryOpcode opcode,
                                        IRNode* arg1,IRNode** newIR) = 0;
    virtual IRNode*        GenIR_Unbox(mdToken token, IRNode* arg1,IRNode** newIR);
    virtual IRNode*        GenIR_UnboxAny(mdToken token, IRNode* arg1,
                                         ReaderAlignType alignmentPrefix,
                                         bool fVolatilePrefix, IRNode** newIR);
    virtual void           GenIR_Nop(IRNode **newIR) = 0;

    virtual IRNode*        GenIR_getAddressOfInterfaceVtable(IRNode *tupVtable, 
                                         CORINFO_CLASS_HANDLE classHandle, IRNode **newIR) = 0;

    virtual void           GenIR_InsertIBCAnnotations() = 0;
    virtual IRNode*        GenIR_InsertIBCAnnotation(FlowGraphNode *node, unsigned int count, unsigned int offset) = 0;

    // Insert class constructor
    virtual void           GenIR_InsertClassConstructor(IRNode **newIR);

    //
    // REQUIRED Client Helper Routines.
    //

    // Base calls to alert client it needs a security check
    virtual void           GenIR_MethodNeedsSecurityCheck() = 0;

    // Base calls to alert client it needs keep generics context alive
    virtual void           GenIR_MethodNeedsToKeepAliveGenericsContext(bool keepGenericsCtxtAlive) = 0;

    // Called to instantiate an empty reader stack.
    virtual ReaderStack*   GenIR_CreateStack(int maxStack, ReaderBase * gen) = 0;

    // Called when reader begins processing method.
    virtual void           GenIR_ReaderPrePass(BYTE* pBuf, unsigned int numBytes) = 0;

    // Called between building the flow graph and inserting the IR
    virtual void           GenIR_ReaderMiddlePass(void) = 0;

    // Called when reader has finished processing method.
    virtual void           GenIR_ReaderPostPass(bool fImportOnly) = 0;

    // Called at the start of block processing
    virtual void           GenIR_BeginFlowGraphNode(FlowGraphNode* fg,
                                                   unsigned int currOffset,
                                                   bool isVerifyOnly) = 0;
    // Called at the end of block processing.
    virtual void           GenIR_EndFlowGraphNode(FlowGraphNode* fg,
                                                 unsigned int currOffset, IRNode **newIR) = 0;

    // Used to maintain operand stack.
    virtual void           GenIR_MaintainOperandStack(IRNode** opr1,IRNode** opr2,IRNode** newIR) = 0;
    virtual void           GenIR_AssignToSuccessorStackNode( FlowGraphNode*,IRNode* dst,
                                                           IRNode* src, IRNode** newIR, bool*) = 0;
//    virtual ReaderStackNode* GenIR_CopyStackList(ReaderStackNode* stack) = 0;
    virtual bool           GenIR_TypesCompatible(IRNode* src1, IRNode* src2) = 0;

    virtual void           GenIR_RemoveStackInterference(IRNode** newIR) = 0;

    virtual void           GenIR_RemoveStackInterferenceForLocalStore(unsigned int opcode, unsigned int ordianl, IRNode** newIR) = 0;

    // Remove all IRNodes from block (for verification error processing.)
    virtual void           GenIR_ClearCurrentBlock(IRNode** newIR) = 0;

    // Called when an assert occurs (debug only)
    static void            GenIR_DebugError(const char* filename, unsigned lineNumber, const char* s);
    
    // Notify client of alignment problem
    virtual void           GenIR_VerifyStaticAlignment(void* p, CorInfoType corInfoType, unsigned minClassAlign) = 0;

    // non-debug fatal error (verification badcode, jit can't continue, etc...)
    static void            GenIR_Fatal(int errnum);

    // Query the runtime/compiler about code-generation information
    virtual bool           GenIR_GenerateDebugCode() { return false; }
    virtual bool           GenIR_GenerateDebugInfo() { return false; }

    virtual void*          getTempMemory(unsigned bytes) = 0; // Allocate temporary (reader lifetime) memory
    virtual void*          getProcMemory(unsigned bytes) = 0; // Allocate procedure-lifetime memory


    virtual EHRegion*      RgnAllocateRegion(void) = 0;
    virtual EHRegionList*  RgnAllocateRegionList(void) = 0;


    //
    // REQUIRED Flow and Region Graph Manipulation Routines
    //
    virtual FlowGraphNode* GenIR_FgPrePhase(FlowGraphNode* fg) = 0;
    virtual void           GenIR_FgPostPhase(void) = 0;
    virtual FlowGraphNode* GenIR_FgGetHeadBlock(void) = 0;
    virtual FlowGraphNode* GenIR_FgGetTailBlock(void) = 0;
    virtual unsigned       GenIR_FgGetBlockCount(void) = 0;
    virtual FlowGraphNode* GenIR_FgNodeGetIDom(FlowGraphNode* fg) = 0;


    virtual IRNode*        FgNodeFindStartLabel(FlowGraphNode* block) = 0;

    virtual BranchList*    GenIR_FgGetLabelBranchList(IRNode* labelNode) = 0;

    virtual void           GenIR_InsertHandlerAnnotation(EHRegion* handlerRegion) = 0;
    virtual void           GenIR_InsertRegionAnnotation(IRNode* regionStartTuple,
                                                       IRNode* regionEndTuple) = 0;
    virtual void           GenIR_FgAddLabelToBranchList(IRNode* labelNode, 
                                                       IRNode* branchNode) = 0;
    virtual void           GenIR_FgAddArc(IRNode* branchNode, FlowGraphNode *source,
                                         FlowGraphNode *sink) = 0;
    virtual bool           GenIR_FgBlockHasFallThrough(FlowGraphNode* block) = 0;
    virtual bool           GenIR_FgBlockIsRegionEnd(FlowGraphNode* block) = 0;
    virtual void           GenIR_FgDeleteBlock(FlowGraphNode* block) = 0;
    virtual void           GenIR_FgDeleteEdge(FlowGraphEdgeList* arc) = 0;
    virtual void           GenIR_FgDeleteNodesFromBlock(FlowGraphNode* block) = 0;

    // Returns true iff client considers the CALL/JMP recursive and wants a
    // loop back-edge rather than a forward edge to the exit label.
    virtual bool           GenIR_FgOptRecurse(ReaderBaseNS::CallOpcode, mdToken token, bool fIsUnmarkedTailCall) = 0;

    // Returns true if node (the start of a new eh region) cannot be the start of a block.
    virtual bool           GenIR_FgEHRegionStartRequiresBlockSplit(IRNode* node) = 0;

    virtual bool           GenIR_FgIsExceptRegionStartNode(IRNode* node) = 0;
    virtual FlowGraphNode* GenIR_FgSplitBlock(FlowGraphNode* block, IRNode* node) = 0;
    virtual void           GenIR_FgSetBlockToRegion(FlowGraphNode* block,EHRegion* region, unsigned int lastOffset) = 0;
    virtual IRNode*        GenIR_FgMakeBranch(IRNode* labelTup, IRNode* insertTup,
                                             unsigned int currentOffset, bool fIsConditional,
                                             bool fIsNominal) = 0;
    virtual IRNode*        GenIR_FgMakeEndFinally(IRNode* insertTup, unsigned int currentOffset,
                                                  bool fIsLexicalEnd) = 0;

    // turns an unconditional branch to the entry label into a fall-through
    // or a branch to the exit label, depending on whether it was a recursive
    // jmp or tail.call.
    virtual void           GenIR_FgRevertRecursiveBranch(IRNode* branchTup) = 0;

    virtual IRNode*        GenIR_FgMakeSwitch(IRNode* defaultLabel, IRNode* insert) = 0;
    virtual IRNode*        GenIR_FgMakeThrow(IRNode* insert) = 0;
    virtual IRNode*        GenIR_FgMakeRethrow(IRNode* insert) = 0;
    virtual IRNode*        GenIR_FgAddCaseToCaseList(IRNode* switchNode,IRNode* labelNode,
                                                    unsigned element) = 0;
    virtual void           GenIR_InsertEHAnnotationNode(IRNode* insertionPointNode,
                                                       IRNode* insertNode) = 0;
    virtual IRNode*        GenIR_MakeLabel(unsigned int targetOffset, EHRegion* region) = 0;
    virtual void           GenIR_MarkAsEHLabel(IRNode* labelNode) = 0;
    virtual IRNode*        GenIR_MakeTryEndNode(void) = 0;
    virtual IRNode*        GenIR_MakeRegionStartNode(ReaderBaseNS::RegionKind regionType) = 0;
    virtual IRNode*        GenIR_MakeRegionEndNode(ReaderBaseNS::RegionKind regionType) = 0;

    // Hook to permit client to record call information
    virtual void           GenIR_FgCall(ReaderBaseNS::OPCODE, mdToken token, unsigned msilOffset, IRNode* block, bool fCanInline) = 0;

    // Given a block and offset, find the first node in the block that comes after the given offset, and
    // insert the label before that node.
    virtual void           GenIR_InsertLabelInBlock(FlowGraphNode* block,
                                                    unsigned int offset,IRNode* newLabel) = 0;
    virtual IRNode*        GenIR_FindBlockSplitPointAfterNode(IRNode* node) = 0;
    virtual IRNode*        GenIR_ExitLabel(void) = 0;
    virtual IRNode*        GenIR_EntryLabel(void) = 0;

    // Function is passed a try region, and is expected to return the first label or instruction
    // after the region.
    virtual IRNode*        GenIR_FindTryRegionEndOfClauses(EHRegion* tryRegion) = 0;

    virtual bool           GenIR_IsCall(IRNode** newIR) = 0;
    virtual bool           GenIR_IsRegionStartBlock(FlowGraphNode* fg) = 0;
    virtual bool           GenIR_IsRegionEndBlock(FlowGraphNode* fg) = 0;

    // Create a symbol node that will be used to represent the stack-incoming exception object
    // upon entry to funclets.
    virtual IRNode*        GenIR_MakeExceptionObject(IRNode** newIR) = 0;


    // //////////////////////////////////////////////////////////////////////////
    // Client Supplied Helper Routines, required by VOS support
    // //////////////////////////////////////////////////////////////////////////

    // Asks GenIR to make operand value accessible by address, and return a node that references
    // the incoming operand by address.
    virtual IRNode*        GenIR_AddressOfLeaf(IRNode* leaf,IRNode** newIR) = 0;
    virtual IRNode*        GenIR_AddressOfValue(IRNode* leaf,IRNode** newIR) = 0;

    virtual void           GenIR_EnableGC(IRNode* threadBase, IRNode** newIR) = 0;
    virtual void           GenIR_DisableGC(IRNode* threadBase, IRNode** newIR) = 0;

    // Helper callback used by rdrCall to emit call code.
    virtual IRNode*        GenIR_GenCall(ReaderCallTargetData *callTargetInfo,
                                          callArgTriple* argArray, unsigned int numArgs,
                                          IRNode** callNode, IRNode** newIR) = 0;

    virtual bool           GenIR_CanMakeDirectCall(ReaderCallTargetData *callTargetData) = 0;

    // Generate call to helper
    virtual IRNode*        GenIR_CallHelper(CorInfoHelpFunc helperID,IRNode* dst, IRNode** newIR, 
                                    IRNode* arg1 = NULL, /*optinal: args to helper (up to 3) */
                                    IRNode* arg2 = NULL,
                                    IRNode* arg3 = NULL,
                                    ReaderAlignType alignment = READER_ALIGN_UNKNOWN /*optinal: alignment */,
                                    bool fVolatile = false /* optional: volatile */,
                                    bool fNoCtor = false /* optional: is this known not to invoke the static .cctor */,
                                    bool fCanMoveUp = false /* optional: is this a relaxed intrinsic that can move up */ ) = 0;

    // Generate special generics helper that might need to insert flow
    virtual IRNode*        GenIR_CallRuntimeHandleHelper(CorInfoHelpFunc helper,
                                      IRNode* arg1, IRNode* arg2,
                                      IRNode* arg3,IRNode* arg4,IRNode *arg5,
                                      IRNode* nullCheckArg, IRNode** newIR) = 0;

    virtual IRNode*        GenIR_ConvertToHelperArgumentType(IRNode* opr,
                                                            unsigned int destinationSize,
                                                            IRNode** newIR) = 0;

    virtual IRNode*        GenIR_GenNullCheck(IRNode* node,
                                             IRNode** newIR) = 0;
    virtual void           GenIR_CreateSym(int num, bool isAuto, CorInfoType corType,
                                          CORINFO_CLASS_HANDLE hClass, bool fIsPinned,
                         ReaderSpecialSymbolType type = READER_NOT_SPECIAL_SYMBOL /* optional */) = 0;

    virtual IRNode*        GenIR_DerefAddress(IRNode* address, bool fDstIsGCPtr,
                                             IRNode** newIR) = 0;

    virtual IRNode*        GenIR_GetHelperCallAddress(CorInfoHelpFunc helperId,
                                                     IRNode** newIR) = 0;

    virtual IRNode*        GenIR_HandleToIRNode(mdToken token, void* embedHandle, void* realHandle,
                                                bool fIndirect, bool fReadOnly, bool fRelocatable, bool fCallTarget,
                                                IRNode** newIR,
                                                bool fIsFrozenObject = false /*default value: not a frozen object*/
                                                ) = 0;

    // Create an operand that will be used to hold a pointer.
    virtual IRNode*        GenIR_MakePtrDstGCOperand(bool fInteriorGC) = 0;
    virtual IRNode*        GenIR_MakePtrNode(ReaderPtrType ptrType = READER_PTR_NOT_GC) = 0;
    virtual IRNode*        GenIR_MakeStackTypeNode(IRNode* node) = 0;
    virtual IRNode*        GenIR_MakeCallReturnNode(CORINFO_SIG_INFO* sig,
                                                    unsigned* pHiddenMBParamSize,
                                                    PGCLAYOUT_STRUCT *ppGcLayoutStruct) = 0;
    
    virtual IRNode* GenIR_MakeDirectCallTargetNode(CORINFO_METHOD_HANDLE hMethod, void *codeAddr)= 0;

    // Called once region tree has been built.
    virtual void           GenIR_SetEHInfo(EHRegion* ehRegionTree, EHRegionList* ehRegionList) = 0;

    // Line number info
    virtual void           GenIR_SequencePoint(int offset, ReaderBaseNS::OPCODE prevOp, IRNode **newIR);
    virtual void           GenIR_SetSequencePoint(unsigned int,ICorDebugInfo::SourceTypes, IRNode **newIR) = 0;
    virtual bool           GenIR_NeedSequencePoints() = 0;
    virtual void           GenIR_SetOptimizedCastUsed() = 0;

    // Used to turn token into handle/IRNode
    virtual IRNode*        GenIR_GenericTokenToNode(mdToken token,
                                                    IRNode** newIR,
                                                    bool mustRestoreHandle = false /* Optional */,
                                                    CORINFO_GENERIC_HANDLE* pStaticHandle = NULL /* Optional */,
                                                    bool* pRuntimeLookup = NULL /* Optional */,
                                                    CorInfoTokenKind tokenKind = CORINFO_TOKENKIND_Default /* Optional */,
                                                    bool needResult = true /* Optional */);

    virtual IRNode*        GenIR_RuntimeLookupToNode(CORINFO_RUNTIME_LOOKUP_KIND kind,
                                                     CORINFO_RUNTIME_LOOKUP  *pLookup,
                                                     IRNode                **newIR);

    // Used to expand multidimensional array access intrinsics
    virtual bool           GenIR_ArrayGet(CORINFO_SIG_INFO *sig, IRNode** retVal, IRNode** newIR) = 0;
    virtual bool           GenIR_ArraySet(CORINFO_SIG_INFO *sig, IRNode** newIR) = 0;

#if !defined(NDEBUG)
    virtual void           GenIR_DBDumpFunction(void) = 0;
    virtual void           GenIR_DBPrintIRNode(IRNode *newIR) = 0;
    virtual void           GenIR_DBPrintFGNode(FlowGraphNode *fg) = 0;
    virtual void           GenIR_DBPrintEHRegion(EHRegion *eh) = 0;
    virtual DWORD          GenIR_DBGetFuncHash(void) = 0;
#endif

    static bool            rdrIsMethodVirtual(DWORD methodAttribs);

private:
    ///////////////////////////////////////////////////////////////////////
    // Last field in structure.
    char m_dummyLastBaseField;
    // Fields after this one will not be initialized in the constructor.
    ///////////////////////////////////////////////////////////////////////
};


// --------------------------------------------------------------
// Functions for reading typed values from the bytecode.
// --------------------------------------------------------------

#define MSIL_LOADTYPE(msilBytePtr__, type) \
    {                                                   \
        type retVal = *(UNALIGNED type*)msilBytePtr__;  \
        return retVal;                                  \
    }

inline signed char ReadInt8(
    BYTE* msilBytePtr
){
    MSIL_LOADTYPE(msilBytePtr,signed char);
}

inline unsigned char ReadUInt8(
    BYTE* msilBytePtr
){
    MSIL_LOADTYPE(msilBytePtr,unsigned char);
}

inline signed short ReadInt16(
    BYTE* msilBytePtr
){
    MSIL_LOADTYPE(msilBytePtr,signed short);
}

inline unsigned short ReadUInt16(
    BYTE* msilBytePtr
){
    MSIL_LOADTYPE(msilBytePtr,unsigned short);
}

inline signed int ReadInt32(
    BYTE* msilBytePtr
){
    MSIL_LOADTYPE(msilBytePtr,signed int);
}

inline unsigned int ReadUInt32(
    BYTE* msilBytePtr
){
    MSIL_LOADTYPE(msilBytePtr,unsigned int);
}

inline __int64 ReadInt64(
    BYTE* msilBytePtr
){
    MSIL_LOADTYPE(msilBytePtr,__int64);
}

inline mdToken ReadToken(
    BYTE* msilBytePtr
){
    MSIL_LOADTYPE(msilBytePtr,mdToken);
}

inline float ReadF32(
    BYTE* msilBytePtr
){
    MSIL_LOADTYPE(msilBytePtr,float);
}

inline double ReadF64(
    BYTE* msilBytePtr
){
    MSIL_LOADTYPE(msilBytePtr,double);
}

inline void* ReadPtr(
    BYTE* msilBytePtr
){
    typedef int* PtrType;
    MSIL_LOADTYPE(msilBytePtr,PtrType);
}

#endif // _READER_H
