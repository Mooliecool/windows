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

#ifndef _READERIR_H
#define _READERIR_H

#include "reader.h"
#include "jit.h"


    #include "nopjit_datastructures.h"

extern int _cdecl dbprint(const char *form, ...);
extern inline mdToken ReadToken(BYTE* msilBytePtr);

class GenStack : public ReaderStack {
public:
    GenStack();
    GenStack(int maxStack, ReaderBase* gen);
    IRNode* pop (void);
    void push (IRNode* newVal, IRNode **newIR);
    void clearStack (void);
    bool empty(void);
    void assertEmpty(void);
    int  depth();


    // For iteration
    IRNode* getIterator(ReaderStackIterator**);
    IRNode* iteratorGetNext (ReaderStackIterator**);
    void    iteratorReplace(ReaderStackIterator**, IRNode*);
    IRNode* getReverseIterator(ReaderStackIterator**);
    IRNode* getReverseIteratorFromDepth(ReaderStackIterator**, int depth);
    IRNode* reverseIteratorGetNext(ReaderStackIterator**);

#if !defined(NODEBUG)
    virtual void print();
#endif

    ReaderStack* copy();

private:
    int                   max;
    int                   top;
    IRNode **             stack;
    ReaderBase *          gen;
};

struct ReaderStackStateStack
{
    ReaderStack *stack;
    ReaderStackStateStack *next;
};

class GenIR : public ReaderBase {

    JitContext* m_jitContext;

public:

    // ESSA

    virtual void       Ssa_LocAddressTaken(unsigned int idx);
    virtual void       Ssa_ArgAddressTaken(unsigned int idx);
    virtual void       Ssa_VirtPhiPass(FlowGraphNode* h);
    virtual void       Ssa_OptRenameInit();
    virtual void       Ssa_CreateDefMap(int * ssaParamCnt, int * ssaLocalCnt);
    virtual void       Ssa_AddDefPoint(unsigned int def, int offset, bool isLocal);
    virtual int        Ssa_HshMSILScalar(unsigned int opcode,BYTE *operPtr,FlowGraphNode *fg);
    virtual void       Ssa_LookAheadEdge(FlowGraphNode * curr, FlowGraphNode * succ, FlowGraphEdgeList * edge);
    virtual void       Ssa_OptPop(FlowGraphNode *block);
    virtual void       Ssa_PushPhis(FlowGraphNode *block);

    // KINTERF
    virtual void       Itf_RegisterNumberOfIndirs(int numIndirs);

    PCI ciPtr; // This name is fixed because it is implicit in many macros.

    // Used for fg building
    IRNode*     m_entryNode;
    IRNode*     m_exitNode;

    IRNode*     m_entryLabel;
    IRNode*     m_exitLabel;

    // Used when address taken has been removed
    bool        m_recomputeAddressTaken;
    bool        m_FuncHasSecurityCall;

    void **      m_peverifyAllocList;

    FlowGraphNode*  m_headBlock;
    FlowGraphNode*  m_tailBlock;

    GenIR(JitContext* jitContext, PCI ci) :
        ReaderBase(jitContext->jitInfo,
                   jitContext->methodInfo,
                   jitContext->flags
    ){
        m_jitContext = jitContext;
        ciPtr = ci;

    }

    void verSetPeverifyInfo(void *handler, void *pevThis)
    {
        m_peverifyErrorHandler = handler;
        m_peverifyThis = pevThis;
        m_peverifyAllocList = NULL;
    }
    ~GenIR();


    IRNode*      GenIR_getAddressOfInterfaceVtable(IRNode *tupVtable, 
                                             CORINFO_CLASS_HANDLE classHandle, IRNode **newIR);


    // There are default implementations of each of these provided in ReaderBase.
    // The PEVERIFY / NOPJIT scenario most overload these so that we can be assured that we
    // don't actually try to do any codegen (or registering of relocations/etc.).

    IRNode* GenIR_RuntimeLookupToNode(
        CORINFO_RUNTIME_LOOKUP_KIND kind,
        CORINFO_RUNTIME_LOOKUP  *pRuntimeLookup,
        IRNode                **newIR);

    IRNode* GenIR_GenericTokenToNode(
        mdToken                token,
        IRNode               **newIR);

    IRNode* GenIR_GetStaticFieldAddress(
        mdToken tokenField,
        IRNode** newIR);

    void GenIR_StoreIndir(
        ReaderBaseNS::StIndirOpcode opcode,
        IRNode* value,
        IRNode* address,
        ReaderAlignType alignment,
        bool fVolatile,
        IRNode** newIR);
    
    void GenIR_StoreObj(
        mdToken         token,
        IRNode*         value,
        IRNode*         addr,
        ReaderAlignType alignment,
        bool            fVolatile,
        bool            fField,
        IRNode**        newIR
    );



     // Required client functions (overload pure virtual routines in ReaderBase)

     bool           verForceVerification(void);

     void           GenIR_OpcodeDebugPrint(BYTE *pbuf, unsigned startOffset, unsigned endOffset);

     bool           GenIR_Abs(IRNode* arg1, IRNode** retVal, IRNode** newIR);
     bool           GenIR_ArrayGet(CORINFO_SIG_INFO *sig, IRNode** retVal, IRNode** newIR);
     bool           GenIR_ArraySet(CORINFO_SIG_INFO *sig, IRNode** newIR);

     IRNode*        GenIR_ArgList(IRNode** newIR);
     IRNode*        GenIR_InstParam(IRNode** newIR);
     IRNode*        GenIR_ThisObj(IRNode** newIR);

     void           GenIR_BoolBranch(ReaderBaseNS::BoolBranchOpcode opcode,
                                     IRNode* arg1,IRNode** newIR);
     IRNode*        GenIR_BinaryOp(ReaderBaseNS::BinaryOpcode opcode,
                                   IRNode*        arg1,
                                   IRNode*        arg2, 
                                   IRNode**       newIR);
     void           GenIR_Branch(IRNode** newIR);
     IRNode*        GenIR_Call(ReaderBaseNS::CallOpcode opcode, mdToken token,
                                 mdToken constraintTypeRef, bool readOnlyPrefix,
                                 bool tailCallPrefix, bool isUnmarkedTailCall, IRNode** newIR);
     IRNode*        GenIR_CkFinite(IRNode* arg1,IRNode** newIR);
     IRNode*        GenIR_Cmp(ReaderBaseNS::CmpOpcode opode,
                                     IRNode* arg1,IRNode* arg2,IRNode** newIR);
     void           GenIR_CondBranch(ReaderBaseNS::CondBranchOpcode opcode,
                                     IRNode*        arg1,
                                     IRNode*        arg2,
                                     IRNode**       newIR);
     IRNode*        GenIR_Conv(ReaderBaseNS::ConvOpcode opcode,
                               IRNode* arg1,IRNode** newIR);
     void           GenIR_Dup(IRNode* opr, IRNode** result1, IRNode** result2,
                              IRNode** newIR);
     void           GenIR_EndFilter(IRNode* arg1, IRNode** newIR);
     IRNode*        GenIR_GetFieldAddress(mdToken token, IRNode* ptObj, bool fObjIsThis,
                                          bool* fNullCheckAdded, IRNode** newIR);
     void           GenIR_Jmp(ReaderBaseNS::CallOpcode opcode,mdToken token, bool hasThis,
                               bool hasVarArg, IRNode** newIR);
     void           GenIR_Leave(unsigned int targetOffset,
                                bool isNonLocal,bool endsWithNonLocalGoto,IRNode **newIR);
#ifdef READER_ESSA_RENAME
     IRNode*        GenIR_LoadArg(unsigned int argOrdinal,bool isJmp,IRNode** newIR, int ssaIdx, FlowGraphNode* block);
     IRNode*        GenIR_LoadLocal(unsigned int argOrdinal, IRNode** newIR, int ssaIdx, FlowGraphNode* block);
#else
     IRNode*        GenIR_LoadArg(unsigned int argOrdinal,bool isJmp,IRNode** newIR);
     IRNode*        GenIR_LoadLocal(unsigned int argOrdinal, IRNode** newIR);
#endif
     IRNode*        GenIR_LoadArgAddress(unsigned int argOrdinal,IRNode** newIR);
     IRNode*        GenIR_LoadLocalAddress(unsigned int locOrdinal,IRNode** newIR);
     IRNode*        GenIR_LoadConstantI4(int constant,IRNode** newIR);
     IRNode*        GenIR_LoadConstantI8(__int64 constant,IRNode** newIR);
     IRNode*        GenIR_LoadConstantI(size_t constant,IRNode** newIR);
     IRNode*        GenIR_LoadConstantR4(float value,IRNode** newIR);
     IRNode*        GenIR_LoadConstantR8(double value,IRNode** newIR);
     IRNode*        GenIR_LoadElem( ReaderBaseNS::LdElemOpcode opcode,
                                    mdToken token, IRNode* arg1, IRNode* arg2,
                                    IRNode** newIR);
     IRNode*        GenIR_LoadElemA(mdToken token, IRNode* arg1,IRNode* arg2,
                                    bool fReadOnly, IRNode** newIR);
     IRNode*        GenIR_LoadField(mdToken token,
                                    IRNode* arg1,ReaderAlignType alignment,
                                    bool fVolatile,IRNode** newIR);
     IRNode*        GenIR_LoadFuncptr(mdToken token,CORINFO_LOOKUP *pLookup,IRNode** newIR);
     IRNode*        GenIR_LoadNull(IRNode** newIR);
     IRNode*        GenIR_LocalAlloc(IRNode* arg, bool zeroInit, IRNode** newIR);
     IRNode*        GenIR_LoadFieldAddress( mdToken token,IRNode* ptObj,
                                            IRNode** newIR);
     IRNode*        GenIR_LoadLen(IRNode* arg1,IRNode** newIR);
     bool           GenIR_ArrayAddress(CORINFO_SIG_INFO *sig, IRNode **retVal, IRNode **newIR);
     IRNode*        GenIR_LoadStringLen(IRNode* arg1,IRNode** newIR);
     IRNode*        GenIR_ArrayGetDimLength(IRNode* arg1, IRNode* arg2, CORINFO_CALL_INFO *callInfo, IRNode** newIR);
     IRNode*        GenIR_LoadStaticField( mdToken token,
                                           bool fVolatile,IRNode** newIR);
     IRNode*        GenIR_LoadVirtFunc(IRNode* arg1, 
                                       mdToken token,IRNode** newIR);
     IRNode*        GenIR_LoadPrimitiveType(IRNode* addr, CorInfoType CorInfoType,
                                          ReaderAlignType alignment, bool fVolatile,
                                          bool fInterfConst, IRNode** newIR);
     IRNode*        GenIR_LoadNonPrimitiveObj(IRNode* addr, CORINFO_CLASS_HANDLE classHandle,
                                             ReaderAlignType alignment, bool fVolatile, IRNode** newIR);
     IRNode*        GenIR_MakeRefAny(mdToken token,IRNode* object,
                                     IRNode** newIR);
     IRNode*        GenIR_NewArr(mdToken token,IRNode* arg1,
                                 IRNode** newIR);
     IRNode*        GenIR_NewObj(mdToken token, mdToken loadFtnToken, IRNode** newIR);
     void           GenIR_Pop( IRNode* opr, IRNode** newIR);
     IRNode*        GenIR_RefAnyType(IRNode* arg1,IRNode** newIR);
     void           GenIR_Rethrow(IRNode** newIR); 
     void           GenIR_Return(IRNode* opr,bool synchronousMethod,bool needsByrefReturnHelper, IRNode** newIR);
     IRNode*        GenIR_Shift(ReaderBaseNS::ShiftOpcode opcode, IRNode* shiftAmount,
                                IRNode* shiftOperand,IRNode** newIR);
     IRNode*        GenIR_Sizeof(mdToken token, IRNode** newIR);
#ifdef READER_ESSA_RENAME
     void           GenIR_StoreArg(unsigned int locOrdinal,
                                   IRNode* arg1,ReaderAlignType alignment,bool fVolatile,IRNode** newIR,
                                   int ssaIdx, FlowGraphNode * block);
#else
     void           GenIR_StoreArg(unsigned int locOrdinal,
                                   IRNode* arg1,ReaderAlignType alignment,bool fVolatile,IRNode** newIR);
#endif
     void           GenIR_StoreElem(ReaderBaseNS::StElemOpcode opcode,mdToken token,
                                    IRNode* arg1, IRNode* arg2, IRNode* arg3,IRNode** newIR);
     void           GenIR_StoreField(mdToken token,IRNode* arg1,
                                     IRNode* arg2,ReaderAlignType alignment,
                                     bool fVolatile,IRNode** newIR);
#ifdef READER_ESSA_RENAME
     void           GenIR_StoreLocal(unsigned int locOrdinal,
                                     IRNode* arg1,ReaderAlignType alignment,bool fVolatile,IRNode** newIR,
                                     int ssaIdx, FlowGraphNode * block);
#else
     void           GenIR_StoreLocal(unsigned int locOrdinal,
                                     IRNode* arg1,ReaderAlignType alignment,bool fVolatile,IRNode** newIR);
#endif
     void           GenIR_StoreStaticField(mdToken token,
                                           IRNode* arg1,bool fVolatile,IRNode** newIR);
     void           GenIR_StorePrimitiveType(IRNode* value, IRNode* addr, CorInfoType CorInfoType,
                                ReaderAlignType alignment, bool fVolatile, IRNode** newIR);

     IRNode*        GenIR_StringGetChar(IRNode* arg1,IRNode* arg2,IRNode** newIR);
     bool           GenIR_Sqrt(IRNode* arg1, IRNode** retVal, IRNode** newIR);
     void           GenIR_Switch(IRNode* opr, IRNode** newIR);
     void           GenIR_Throw( IRNode* arg1, IRNode** newIR);
     IRNode*        GenIR_UnaryOp(ReaderBaseNS::UnaryOpcode opcode,IRNode* arg1,IRNode** newIR);
     void           GenIR_Nop(IRNode **newIR);


    void           GenIR_InsertIBCAnnotations();
    IRNode*        GenIR_InsertIBCAnnotation(FlowGraphNode *node, unsigned int count, unsigned int offset);


    //
    // REQUIRED Client Helper Routines.
    //
     
    // Base calls to alert client it needs a security check
    void           GenIR_MethodNeedsSecurityCheck();

    // Base calls to alert client it needs keep generics context alive
    void           GenIR_MethodNeedsToKeepAliveGenericsContext(bool keepGenericsCtxtAlive);

    // Called to instantiate an empty reader stack.
    ReaderStack*    GenIR_CreateStack(int maxStack, ReaderBase * gen);

    // Called when reader begins processing method.
     void           GenIR_ReaderPrePass(BYTE * pBuf, unsigned int numBytes);

    // Called between building the flow graph and inserting the IR
     void           GenIR_ReaderMiddlePass(void);

    // Called when reader has finished processing method.
     void           GenIR_ReaderPostPass(bool fImportOnly);
     
    // Called when reader begins processing an inlinee

    // Called at the start of block processing
     void           GenIR_BeginFlowGraphNode(FlowGraphNode* fg,
                                             unsigned int currOffset,
                                             bool isVerifyOnly);
     // Called at the end of block processing.
     void           GenIR_EndFlowGraphNode(FlowGraphNode* fg,
                                           unsigned int currOffset, IRNode **newIR);

     // Used to maintain operand stack.
     void           GenIR_MaintainOperandStack(IRNode** opr1,
                                               IRNode** opr2,IRNode** newIR);
     void           GenIR_AssignToSuccessorStackNode( FlowGraphNode*,IRNode* dst,
                                                      IRNode* src, IRNode** newIR, bool*);
     bool           GenIR_TypesCompatible(IRNode* src1, IRNode* src2);

     void           GenIR_RemoveStackInterference(IRNode** newIR);

     void           GenIR_RemoveStackInterferenceForLocalStore(unsigned int opcode, unsigned int ordinal, IRNode** newIR);

     // Remove all IRNodes from block (for verification error processing.)
     void           GenIR_ClearCurrentBlock(IRNode** newIR);

     // Called when an assert occurs (debug only)
     void           GenIR_DebugError(const char* filename, 
                                           unsigned lineNumber, const char* s);

     // Notify client of alignment problem
     void           GenIR_VerifyStaticAlignment(void* p, CorInfoType corInfoType, unsigned minClassAlign);

     // Get memory for reader lifetime
     void *         getTempMemory(unsigned bytes);

     // Get memory that will persist after the reader
     void *         getProcMemory(unsigned bytes);

     EHRegion*      RgnAllocateRegion(void);
     EHRegionList*  RgnAllocateRegionList(void);

     void           DumpFunction(void);

     //
     // REQUIRED Flow and Region Graph Manipulation Routines
     //
     FlowGraphNode* GenIR_FgPrePhase(FlowGraphNode* fg);
     void           GenIR_FgPostPhase(void);
     FlowGraphNode* GenIR_FgGetHeadBlock(void);
     FlowGraphNode* GenIR_FgGetTailBlock(void);
     unsigned       GenIR_FgGetBlockCount(void);
     FlowGraphNode* GenIR_FgNodeGetIDom(FlowGraphNode* fg);


     IRNode*        FgNodeFindStartLabel(FlowGraphNode* block);
     BranchList*    GenIR_FgGetLabelBranchList(IRNode* labelNode);

     void           GenIR_InsertHandlerAnnotation(EHRegion* handlerRegion);
     void           GenIR_InsertRegionAnnotation(IRNode* regionStartTuple,
                                                 IRNode* regionEndTuple);
     void           GenIR_FgAddLabelToBranchList(IRNode* labelNode, 
                                                 IRNode* branchNode);
     void           GenIR_FgAddArc(IRNode* branchNode, FlowGraphNode *source,
                                   FlowGraphNode *sink);
     bool           GenIR_FgBlockHasFallThrough(FlowGraphNode* block);
     bool           GenIR_FgBlockIsRegionEnd(FlowGraphNode* block);
     void           GenIR_FgDeleteBlock(FlowGraphNode* block);
     void           GenIR_FgDeleteEdge(FlowGraphEdgeList* arc);
     void           GenIR_FgDeleteNodesFromBlock(FlowGraphNode* block);

     // Returns true iff client considers the CALL/JMP recursive and wants a
     // loop back-edge rather than a forward edge to the exit label.
     bool            GenIR_FgOptRecurse(ReaderBaseNS::CallOpcode, mdToken token, bool fIsUnmarkedTailCall);

     // Returns true if node (the start of a new eh region) cannot be the start of a block.
     bool           GenIR_FgEHRegionStartRequiresBlockSplit(IRNode* node);

     bool           GenIR_FgIsExceptRegionStartNode(IRNode* node);
     FlowGraphNode* GenIR_FgSplitBlock(FlowGraphNode* block, IRNode* node);
     void           GenIR_FgSetBlockToRegion(FlowGraphNode* block,EHRegion* region, unsigned int lastOffset);
     IRNode*        GenIR_FgMakeBranch(IRNode* labelTup, IRNode* insertTup,
                                       unsigned int currentOffset, bool fIsConditional,
                                       bool fIsNominal);
     IRNode*        GenIR_FgMakeEndFinally(IRNode* insertTup, unsigned int currentOffset, bool fIsLexicalEnd);

     // turns an unconditional branch to the entry label into a fall-through
     // or a branch to the exit label, depending on whether it was a recursive
     // jmp or tail.call.
     void           GenIR_FgRevertRecursiveBranch(IRNode* branchTup);

     IRNode*        GenIR_FgMakeSwitch(IRNode* defaultLabel, IRNode* insert);
     IRNode*        GenIR_FgMakeThrow(IRNode* insert);
     IRNode*        GenIR_FgMakeRethrow(IRNode* insert);
     IRNode*        GenIR_FgAddCaseToCaseList(IRNode* switchNode,IRNode* labelNode,
                                              unsigned element);
     void           GenIR_InsertEHAnnotationNode(IRNode* insertionPointNode,
                                                 IRNode* insertNode);
     IRNode*        GenIR_MakeLabel(unsigned int targetOffset, EHRegion* region);
     void           GenIR_MarkAsEHLabel(IRNode* labelNode);
     IRNode*        GenIR_MakeTryEndNode(void);
     IRNode*        GenIR_MakeRegionStartNode(ReaderBaseNS::RegionKind regionType);
     IRNode*        GenIR_MakeRegionEndNode(ReaderBaseNS::RegionKind regionType);

     // Allow client to override reader's decision to optimize castclass/isinst
     bool           GenIR_DisableCastClassOptimization();
     bool           GenIR_DisableIsInstOptimization();

     bool           GenIR_IsTmpVarAndReg( IRNode *val );

    // Hook to permit client to record call information
     void           GenIR_FgCall(ReaderBaseNS::OPCODE, mdToken token, unsigned msilOffset, IRNode* block, bool fCanInline);
     
    // Given a block and offset, find the first node in the block that comes after the given offset, and
    // insert the label before that node.
     void           GenIR_InsertLabelInBlock(FlowGraphNode* block,
                                             unsigned int offset,IRNode* newLabel);
     IRNode*        GenIR_FindBlockSplitPointAfterNode(IRNode* node);
     IRNode*        GenIR_ExitLabel(void);
     IRNode*        GenIR_EntryLabel(void);
     
    // Function is passed a try region, and is expected to return the first label or instruction
    // after the region.
     IRNode*        GenIR_FindTryRegionEndOfClauses(EHRegion* tryRegion);

     bool           GenIR_IsCall(IRNode** newIR);
     bool           GenIR_IsRegionStartBlock(FlowGraphNode* fg);
     bool           GenIR_IsRegionEndBlock(FlowGraphNode* fg);

    // Create a symbol node that will be used to represent the stack-incoming exception object
    // upon entry to funclets.
     IRNode*        GenIR_MakeExceptionObject(IRNode** newIR);


    // //////////////////////////////////////////////////////////////////////////
    // OPTIONAL Client Supplied Helper Routines, required by VOS support
    // //////////////////////////////////////////////////////////////////////////

    // Asks GenIR to make operand value accessible by address, and return a node that references
    // the incoming operand by address.
     IRNode*        GenIR_AddressOfLeaf(IRNode* leaf,IRNode** newIR);
     IRNode*        GenIR_AddressOfValue(IRNode* leaf,IRNode** newIR);

     void           GenIR_EnableGC(IRNode* threadBase, IRNode** newIR);
     void           GenIR_DisableGC(IRNode* threadBase, IRNode** newIR);

    // Helper callback used by rdrCall to emit call code.
     IRNode*        GenIR_GenCall(ReaderCallTargetData *callTargetInfo,
                                    callArgTriple* argArray, unsigned int numArgs,
                                    IRNode** callNode, IRNode** newIR);

bool GenIR_CanMakeDirectCall(ReaderCallTargetData *callTargetData);

    // Generate call to helper
     IRNode*        GenIR_CallHelper(CorInfoHelpFunc helperID,IRNode* dst, IRNode** newIR, 
                                    IRNode* arg1 = NULL /* optional: args to helper (up to 3) */,
                                    IRNode* arg2 = NULL /* optional: args to helper (up to 3) */,
                                    IRNode* arg3 = NULL /* optional: args to helper (up to 3) */,
                                    ReaderAlignType alignment = READER_ALIGN_UNKNOWN /* optional: alignment */,
                                    bool fVolatile = false /* optional: volatile */,
                                    bool fNoCtor = false /* optional: is this known not to invoke the static .cctor */,
                                    bool fCanMoveUp = false /* optional: is this a relaxed intrinsic that can move up */ );

    // Special generics helper
     IRNode*        GenIR_CallRuntimeHandleHelper(CorInfoHelpFunc helper,
                                      IRNode* arg1, IRNode* arg2,
                                      IRNode* arg3,IRNode* arg4,IRNode* arg5,
                                      IRNode* nullCheckArg, IRNode** newIR);


     IRNode*        GenIR_ConvertToHelperArgumentType(IRNode* opr,
                                                      unsigned int destinationSize,
                                                      IRNode** newIR);

     IRNode*        GenIR_GenNullCheck(IRNode* node,
                                       IRNode** newIR);

     void           GenIR_CreateSym(int num, bool isAuto, CorInfoType corType,
                                    CORINFO_CLASS_HANDLE hClass, bool fIsPinned,
                         ReaderSpecialSymbolType type = READER_NOT_SPECIAL_SYMBOL /* optional */);



     IRNode*        GenIR_DerefAddress(IRNode* address, bool fDstIsGCPtr,
                                       IRNode** newIR);

     IRNode*        GenIR_GetHelperCallAddress(CorInfoHelpFunc helperId,
                                               IRNode** newIR);

     IRNode*        GenIR_HandleToIRNode(mdToken token, void* embedHandle, void* realHandle,
                                bool fIndirect, bool fReadOnlyConst, bool fRelocatable, bool fCallTarget,
                                IRNode** newIR,
                                bool fIsFrozenObject = false /*default value: not a frozen object*/
                                   );

    // Create an operand that will be used to hold a pointer.
     IRNode*        GenIR_MakePtrDstGCOperand(bool fInteriorGC);
     IRNode*        GenIR_MakePtrNode(ReaderPtrType ptrType = READER_PTR_NOT_GC);
     IRNode*        GenIR_MakeStackTypeNode(IRNode* node);
     IRNode*        GenIR_MakeCallReturnNode(CORINFO_SIG_INFO* sig,
                                              unsigned* pHiddenMBParamSize,
                                              PGCLAYOUT_STRUCT *ppGcLayoutStruct);

    IRNode* GenIR_MakeDirectCallTargetNode(CORINFO_METHOD_HANDLE hMethod, void *codeAddr);

    // Called once region tree has been built.
     void           GenIR_SetEHInfo(EHRegion* ehRegionTree, EHRegionList* ehRegionList);

    // Line number info
    void            GenIR_SetSequencePoint(unsigned int,ICorDebugInfo::SourceTypes, IRNode **newIR);
    void            GenIR_SetOptimizedCastUsed();
    bool            GenIR_NeedSequencePoints();

#if !defined(NDEBUG)
    void GenIR_DBDumpFunction( void );
    void GenIR_DBPrintIRNode(IRNode *newIR);
    void GenIR_DBPrintFGNode(FlowGraphNode *fg);
    void GenIR_DBPrintEHRegion(EHRegion *eh);
    DWORD GenIR_DBGetFuncHash(void);
#endif




};
#endif // _READERIR_H
