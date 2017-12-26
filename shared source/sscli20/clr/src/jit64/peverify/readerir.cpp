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
// readerir.cpp
//
// Empty implementation of required client methods.
//

#include "reader.h"
#include "readerir.h"
#include "utilcode.h"
#include "jit.h"

#ifndef NDEBUG
#define ASSERT(p) do { if (!(p)) {ReaderBase::GenIR_DebugError(__FILE__,__LINE__,#p);}} while (0)
#else
#define ASSERT(p)
#endif

/* DBPRINT/PRINT - */
static int _cdecl
dbprint_help(const char *form, va_list argptr)
{
    int num = 0;
    num = vfprintf(stdout, form, argptr);
    return num;
}


int _cdecl dbprint(const char *form, ...)
{
    va_list argptr;
    va_start(argptr, form);
    return dbprint_help(form, argptr);
}


// /////////////////////////////////////////////////////////////
// READER STACK
// /////////////////////////////////////////////////////////////

GenStack::GenStack(){
    top = NULL;
}

IRNode* GenStack::pop(void) {
    return NULL;
}

IRNode*
GenStack::getIterator(ReaderStackIterator** iterator) {
    return NULL;
}

IRNode*
GenStack::iteratorGetNext(ReaderStackIterator** iterator) {
    return NULL;
}

void
GenStack::iteratorReplace(ReaderStackIterator ** iterator, IRNode *node)
{
    return;
}


IRNode* 
GenStack::getReverseIterator(ReaderStackIterator**)
{
    return NULL;
}
    
IRNode* 
GenStack::getReverseIteratorFromDepth(ReaderStackIterator**, int depth)
{
    return NULL;
}
    
IRNode* 
GenStack::reverseIteratorGetNext(ReaderStackIterator**)
{
    return NULL;
}

void
GenStack::push(IRNode* newVal, IRNode **newIR) {

    ASSERT(newVal == NULL);
    
    return;
}

void
GenStack::clearStack(void) {
    top = NULL;
}

bool
GenStack::empty(void) {
    return (top == NULL);
}

void
GenStack::assertEmpty(void) {
    //ASSERT(top==NULL);
}

int
GenStack::depth(void) {
    return 0;
}

#if !defined(NODEBUG)
void GenStack::print(void) { }
#endif

ReaderStack*
GenStack::copy(void) {
    
    return this;
}
	

ReaderStack*    
GenIR::GenIR_CreateStack(int maxStack, ReaderBase * gen)
{
    void *buf = gen->getProcMemory(sizeof(GenStack));
    return new (buf) GenStack;
}

void
GenIR::GenIR_MethodNeedsSecurityCheck(){}

void
GenIR::GenIR_SetOptimizedCastUsed(){}



IRNode *
GenIR::GenIR_InsertIBCAnnotation(FlowGraphNode *fgnode, unsigned int count, unsigned int offset)
{
    return NULL;
}


void
GenIR::GenIR_InsertIBCAnnotations()
{
}

void
GenIR::GenIR_MethodNeedsToKeepAliveGenericsContext(bool keepGenericsCtxtAlive){}

void
GenIR::GenIR_ReaderPrePass(
    BYTE* pBuf,
    unsigned int numBytes
){

    int numAutos, numParams;
    CORINFO_SIG_INFO sig;


#ifndef NDEBUG
    if (getenv("PEV_FNAME"))
    {
        const char *modname;
        const char *methodname = getCurrentMethodName( &modname );
        
        printf("%s::%s\n", modname, methodname);
    }
#endif

    // Client specific initialization
    m_headBlock = (FlowGraphNode*)getProcMemory(sizeof(FlowGraphNode));
    m_tailBlock = (FlowGraphNode*)getProcMemory(sizeof(FlowGraphNode));
    m_headBlock->succList = m_headBlock->predList = NULL;
    m_tailBlock->succList = m_tailBlock->predList = NULL;

    m_headBlock->next = m_tailBlock;
    m_tailBlock->next = NULL;

    m_entryLabel = GenIR_MakeLabel(0, NULL);
    m_exitLabel = GenIR_MakeLabel(numBytes, NULL);

    m_headBlock->startNode = (IRNode*)getProcMemory(sizeof(IRNode));
    m_headBlock->startNode->kind = IR_block;
    m_headBlock->startNode->fgNode = m_headBlock;
    m_headBlock->startNode->isFallThrough = true;
    m_headBlock->startNode->prev = NULL;
    m_headBlock->startNode->msilOffset = 0;
    m_headBlock->startOffset = 0;
    m_headBlock->endOffset = numBytes;

    m_headBlock->startNode->next = m_entryLabel;
    m_entryLabel->prev = m_headBlock->startNode;
    

    m_tailBlock->startNode = (IRNode*)getProcMemory(sizeof(IRNode));
    m_tailBlock->startNode->kind = IR_block;
    m_tailBlock->startNode->fgNode = m_tailBlock;
    m_tailBlock->startNode->isFallThrough = true;
    m_tailBlock->startNode->prev = m_entryLabel;
    m_tailBlock->startNode->next = m_exitLabel;
    m_tailBlock->startNode->msilOffset = numBytes;
    m_tailBlock->startOffset = numBytes;
    m_tailBlock->endOffset = numBytes;

    m_exitLabel->prev = m_tailBlock->startNode;
    m_exitLabel->next = NULL;
    m_entryLabel->next = m_tailBlock->startNode;
    
    numAutos = m_jitContext->methodInfo->locals.numArgs;
    
    m_jitContext->jitInfo->getMethodSig(m_jitContext->methodInfo->ftn, &sig);
    numParams = (int)sig.totalILArgs();

    // totalILArgs does *not* include the hidden cookie param passed to varargs functions.
    if (sig.isVarArg())
    {
        numParams++;
    }

    initVerifyInfo();
    initParamsAndAutos(numParams, numAutos);

    return;
}

#ifndef NDEBUG

void PrintRegionKind(ReaderBaseNS::RegionKind kind)
{
    char *typeName;

    printf("regionType = ");

    switch(kind)
    {
    case ReaderBaseNS::RGN_UNKNOWN:
        typeName="RGN_UNKNOWN";        break;
    case ReaderBaseNS::RGN_NONE:
        typeName="RGN_NONE";           break;
    case ReaderBaseNS::RGN_ROOT:
        typeName="RGN_ROOT";           break;
    case ReaderBaseNS::RGN_TRY:
        typeName="RGN_TRY";            break;
    case ReaderBaseNS::RGN_EXCEPT:
        typeName="RGN_EXCEPT";         break;
    case ReaderBaseNS::RGN_FAULT:
        typeName="RGN_FAULT";          break;
    case ReaderBaseNS::RGN_FINALLY:
        typeName="RGN_FINALLY";        break;
    case ReaderBaseNS::RGN_FILTER:
        typeName="RGN_FILTER";         break;
    case ReaderBaseNS::RGN_DTOR: 
        typeName="RGN_DTOR";           break;
    case ReaderBaseNS::RGN_CATCH:
        typeName="RGN_CATCH";          break;
    case ReaderBaseNS::RGN_MEXCEPT:
        typeName="RGN_MEXCEPT";        break;
    case ReaderBaseNS::RGN_MCATCH:
        typeName="RGN_MCATCH";         break;
    case ReaderBaseNS::RGN_CLAUSE_NONE:
        typeName="RGN_CLAUSE_NONE";    break;
    case ReaderBaseNS::RGN_CLAUSE_FILTER:
        typeName="RGN_CLAUSE_FILTER";  break;
    case ReaderBaseNS::RGN_CLAUSE_FINALLY:
        typeName="RGN_CLAUSE_FINALLY"; break;
    case ReaderBaseNS::RGN_CLAUSE_ERROR:
        typeName="RGN_CLAUSE_ERROR";   break;
    case ReaderBaseNS::RGN_CLAUSE_FAULT:
        typeName="RGN_CLAUSE_FAULT";   break;
    default:
        typeName="<error>";
    }
    printf("%s ", typeName);
}

void PrintIRNode(IRNode *node, int indent=4)
{
    char *type;
    while (indent--)
        printf(" ");
    switch (node->kind)
    {
    case IR_label:   
        type = "label";
        break;
    case IR_branch: 
        type = "branch";
        break;
    case IR_ehFlow: 
        type = "ehflow";
        break;
    case IR_ehStart: 
        type = "ehstart";
        break;
    case IR_ehEnd: 
        type = "ehend";
        break;
    case IR_throw_rethrow: 
        type = "throw or rethrow";
        break;
    case IR_switch: 
        type = "switch";
        break;
    case IR_jmp: 
        type = "jmp";
        break;
    case IR_endfinally: 
        type = "endfinally";
        break;
    case IR_block:
        type = "block";
        break;
    default:
        type = "INVALID";
        break;
    }
    printf("0x%x: %s %s ", node->msilOffset, type, node->isFallThrough ? "":"no fallthru");
    if (node->fgNode)
    {
        printf("fgNode:%p ", node->fgNode);
    }

    if (node->kind == IR_label)
    {
        BranchList *bl = node->branchList;
        printf("branches( ");
        while (bl)
        {
            printf("%x ", bl->branch->msilOffset);
            bl = bl->next;
        }
        printf(" )");
    }

    if (node->kind == IR_ehStart || node->kind == IR_ehEnd)
    {
        PrintRegionKind(node->regionKind);
    }

    printf("\n");
    if (node->next)
    {
        if (node->next->prev != node)
            printf("BAD BAD BAD\n");

    }
}

void PrintFgNode(FlowGraphNode *node, bool printIr=false)
{
    printf("FGnode [%x:%x] %s %s ptr=%p\n", node->startOffset, node->endOffset,
           node->isVisited?"visited":"", node->isSizeZero?"zero size":"", node);
    IRNode *ir = node->startNode;
    while (ir && printIr)
    {
        PrintIRNode(ir);
        ir=ir->next;
        if (ir && ir->kind == IR_block)
            break;
    }
}

void PrintEdge(FlowGraphEdgeList *edge)
{
    if (edge->isNominal)
        printf("(nominal)");
    if (edge->isEndFinally)
        printf("(end finally)");
    if (edge->isDeletedEdge)
        printf("(deleted)");
    printf("  ");
    PrintFgNode(edge->block, false);
}

void PrintAllIR(FlowGraphNode *headBlock)
{
    FlowGraphNode *node = headBlock;
    FlowGraphEdgeList *edges;

    while (node)
    {
        PrintFgNode(node, true);

        edges = node->succList;
        while (edges)
        {
            printf("    successor ");
            PrintEdge(edges);
            edges = edges->next;
        }

        edges = node->predList;
        while (edges)
        {
            printf("    predecessor ");
            PrintEdge(edges);
            edges = edges->next;
        }
        node = node->next;
    }
}
#endif 

// Called between building the flow graph and inserting the IR
void           
GenIR::GenIR_ReaderMiddlePass(void)
{
    // no-op

#ifndef NDEBUG
    if (getenv("PEV_DUMPIR"))
    {
        PrintAllIR(m_headBlock);
    }
#endif
    
    return;
}

void
GenIR::GenIR_ReaderPostPass(
    bool fImportOnly
){
    // Cleanup from GenIR - free any dynamic memory we used
}



// Hook to notify client that we are beginning the processing of a new block.
void
GenIR::GenIR_BeginFlowGraphNode(
    FlowGraphNode* fg,
    unsigned int currOffset,
    bool isVerifyOnlye
){
    m_currentFgNode = fg;
}

void
GenIR::GenIR_EndFlowGraphNode(
    FlowGraphNode* fg,
    unsigned int currOffset,
    IRNode **newIR
){

}

GenIR::~GenIR()
{
    void **next;
    void **current = m_peverifyAllocList;
    while (current)
    {
        next = (void **) *current;
        delete current;
        current = next;
    }
}


// Get memory that will die at end of reader.
// we thread together all the allocations via the first pointer-sized chunk of 
// each allocation so we can go through and delete them at the end.  Not the most
// efficient scheme but it is simple.
void *
GenIR::getTempMemory(
    unsigned bytes
){
    // add one pointer for our bookkeeping
    bytes += sizeof(void*);
    // to round up
    bytes += sizeof(void*) - 1;
    // want this pointer-aligned
    void **mem = new void*[bytes / sizeof(void*)];

    memset(mem, 0, bytes / sizeof(void*) * sizeof(void*));

    // first pointer-sized chunk is for our bookkeeping
    *mem = m_peverifyAllocList;
    m_peverifyAllocList = mem;
    return mem+1;
}

// Get memory that will persist after the reader
// Real jit would use pools.
void *
GenIR::getProcMemory(
    unsigned bytes
){
    return getTempMemory(bytes);
}

EHRegion*      
GenIR::RgnAllocateRegion(void)
{
    return (EHRegion*)getProcMemory(sizeof(EHRegion));
}

EHRegionList*  
GenIR::RgnAllocateRegionList(void)
{
    return (EHRegionList*)getProcMemory(sizeof(EHRegionList));
}


// Build IRNode to represent Exception Object, to be pushed
// onto (empty) operand stack when processing start of funclet.
IRNode*
GenIR::GenIR_MakeExceptionObject(
    IRNode** newIR
){
    return NULL;
}

IRNode*
GenIR::GenIR_UnaryOp(
    ReaderBaseNS::UnaryOpcode opcode,
    IRNode*              arg1,
    IRNode**             newIR
){
    return NULL;
}

IRNode*
GenIR::GenIR_BinaryOp(
    ReaderBaseNS::BinaryOpcode opcode,
    IRNode*        arg1,
    IRNode*        arg2, 
    IRNode**       newIR
){
    return NULL;
}

void
GenIR::GenIR_Nop(IRNode **newIR)
{
    return;
}

bool
GenIR::GenIR_Abs(
    IRNode* arg1,
    IRNode** retVal,
    IRNode** newIR
){
    return false;
}

//
// GenIR_ArgList -
//
IRNode*
GenIR::GenIR_ArgList(
    IRNode** newIR
){
    return NULL;
}
//
// GenIR_InstParam -
//
IRNode*
GenIR::GenIR_InstParam(
    IRNode** newIR
){
    return NULL;
}

//
// GenIR_ThisObj -
//
IRNode*
GenIR::GenIR_ThisObj(
    IRNode** newIR
){
    return NULL;
}

// GenIR_Branch - 
void
GenIR::GenIR_Branch(
    IRNode**       newIR
){
    return;
}


void
GenIR::GenIR_BoolBranch(
    ReaderBaseNS::BoolBranchOpcode opcode,
    IRNode        *arg1,
    IRNode        **newIR
){
    return;
}
 
// GenIR_CondBranch - Create instruction tuples to compare the top two operands
// on the stack and do a conditional branch.
void
GenIR::GenIR_CondBranch( 
    ReaderBaseNS::CondBranchOpcode opcode,
    IRNode* arg1,
    IRNode* arg2,
    IRNode** newIR
)
{
    return;
}

// Leave target block will be lone successor block.
void
GenIR::GenIR_Leave(
    unsigned int    targetOffset,
    bool            isNonLocal,
    bool            endsWithNonLocalGoto,
    IRNode        **newIR
){
    return;
}

bool
GenIR::GenIR_Sqrt(
    IRNode      *arg1,
    IRNode     **retVal,
    IRNode     **newIR
){
    return false;
}

// Switch is passed an target count,
// and an array of the destination blocks.
void
GenIR::GenIR_Switch(
    IRNode      *opr, 
    IRNode     **newIR
){
}

void
GenIR::GenIR_Rethrow(
    IRNode** newIR
){
}

// Throw node was already created during fg build, now append src onto throw.
void
GenIR::GenIR_Throw(
    IRNode * arg1,
    IRNode** newIR
){
}

// GenIR_EndFilter - Generate a src, no dst, except tuple
void
GenIR::GenIR_EndFilter(
    IRNode* arg1,
    IRNode** newIR
){
}


// Function:  GenIR_Return
//
//    Creates a return instruction tuple if there is anything on the stack.
//
//    If the method is synchronized, then we must insert a call to 
//    monitor_exit before returning. The call to monitor_exit must occur
//    after the return value has been calculated, so, if the return type
//    is not void, assign the return value into a tmpvar, call mon_exit,
//    then return the tmpvar.
void
GenIR::GenIR_Return(
    IRNode* opr,
    bool    synchronousMethod,
    bool    needsByrefReturnCheck,
    IRNode** newIR
){
    return;
}

// GenIR_CkFinite - Places on overflow check on the floating point operand
// sitting on top of the stack.
IRNode*
GenIR::GenIR_CkFinite(
    IRNode*        opr,
    IRNode**       newIR
){
    return NULL;
}

// GenIR_Cmp - This function creates the tuples needed to implement the MSIL instructions
//   CEQ, CGT, CLT, CGT_UN, CLT_UN. This five instructions take two argruments and
//   return either 1 or 0 (int32): 1 for the comparison being true and 0 for false.
//   There are two obvious ways to implement this. Previously this was impemented
//   with an if/then/else structure. It can also be implemented easily using the question
//   operator. The question operator was ported over from UTC for this explicit purpose.
//   As noted in loQuestion, the behavior of the OPQUESTION is indetermainent if used
//   for anything besides this purpose.
IRNode* GenIR::GenIR_Cmp(
    ReaderBaseNS::CmpOpcode opcode,
    IRNode* arg1,
    IRNode* arg2,
    IRNode** newIR
)
{
    return NULL;
}


IRNode* 
GenIR::GenIR_Conv(
    ReaderBaseNS::ConvOpcode opcode,
    IRNode* arg1,
    IRNode** newIR
){
    return NULL;
}

// Accepts IRNode* opr which is operand to be duplicated.
// Returns IRNode* which is duplicate operand to be
// pushed onto stack.
void
GenIR::GenIR_Dup(
    IRNode* opr,
    IRNode** result1,
    IRNode** result2,
    IRNode** newIR
){
    *result1 = NULL;
    *result2 = NULL;
    return;
}


IRNode*
GenIR::GenIR_LoadLen(
    IRNode*        arg1,
    IRNode**       newIR
){
    return NULL;
}

bool        
GenIR::GenIR_ArrayAddress(
    CORINFO_SIG_INFO *sig, 
    IRNode **retVal, 
    IRNode **newIR
){
    return NULL;
}

IRNode*
GenIR::GenIR_LoadStringLen(
    IRNode*        addr,
    IRNode**       newIR
){
    return NULL;
}

IRNode*
GenIR::GenIR_StringGetChar(
    IRNode*        addr,
    IRNode*        index,
    IRNode**       newIR
){
    return NULL;
}

IRNode*
GenIR::GenIR_ArrayGetDimLength(
    IRNode                  *arg1,
    IRNode                  *arg2,
    CORINFO_CALL_INFO       *callInfo,
    IRNode                 **newIR
)
{
    return NULL;
}

IRNode*
GenIR::GenIR_RuntimeLookupToNode(
    CORINFO_RUNTIME_LOOKUP_KIND kind,
    CORINFO_RUNTIME_LOOKUP  *pRuntimeLookup,
    IRNode                **newIR
){
    return NULL;
}

bool
GenIR::GenIR_ArrayGet(
    CORINFO_SIG_INFO *sig,
    IRNode          **retVal,
    IRNode          **newIR
)
{
    return false;
}

bool
GenIR::GenIR_ArraySet(
    CORINFO_SIG_INFO *sig,
    IRNode          **newIR
)
{
    return false;
}
    
IRNode*
GenIR::GenIR_GenericTokenToNode(
    mdToken                token,
    IRNode               **newIR
){
    return NULL;
}

IRNode*
GenIR::GenIR_GetStaticFieldAddress(
    mdToken tokenField,
    IRNode** newIR
){
    return NULL;
}

IRNode*
GenIR::GenIR_LoadArg(
    unsigned int    argOrdinal,
    bool            isJmp,
    IRNode        **newIR
){
    return NULL;
}

IRNode*
GenIR::GenIR_LoadStaticField(
    mdToken           token,
    bool              fVolatile,
    IRNode**          newIR
){
    return NULL;
}

IRNode*
GenIR::GenIR_LoadVirtFunc(
    IRNode* arg1,
    mdToken token,
    IRNode** newIR
){
    return NULL;
}

IRNode*
GenIR::GenIR_LocalAlloc(
    IRNode*        arg1,
    bool           zeroInit,
    IRNode**       newIR
){
    return NULL;
}


/* KMTF:  VOS Opcodes un-needed here?
IRNode*
GenIR::GenIR_GetStaticFieldAddress(
    mdToken token,
    IRNode** newIR
){
    return NULL;
}
*/

IRNode*
GenIR::GenIR_GetFieldAddress(
    mdToken  token,
    IRNode*  ptObj,
    bool     fObjIsThis,
    bool*    fNullCheckAdded,
    IRNode** newIR
){
    return NULL;
}

IRNode*
GenIR::GenIR_LoadFieldAddress(
    mdToken token,
    IRNode* ptObj,
    IRNode** newIR
){
    return NULL;
}

IRNode*
GenIR::GenIR_LoadField(
    mdToken         token,
    IRNode*         ptObj,
    ReaderAlignType alignmentPrefix,
    bool            fVolatilePrefix,
    IRNode**        newIR
){
    return NULL;
}

void
GenIR::GenIR_StoreField(
    mdToken         token,
    IRNode*         ptValue,
    IRNode*         ptObj,
    ReaderAlignType alignment,
    bool            fVolatile,
    IRNode**        newIR
){
    return;
}

void
GenIR::GenIR_StoreObj(
    mdToken         token,
    IRNode*         value,
    IRNode*         addr,
    ReaderAlignType alignment,
    bool            fVolatile,
    bool            fField,
    IRNode**        newIR
){
    return;
}

void
GenIR::GenIR_StoreIndir(
    ReaderBaseNS::StIndirOpcode opcode,
    IRNode* value,
    IRNode* address,
    ReaderAlignType alignment,
    bool fVolatile,
    IRNode** newIR
){
    return;
}

void
GenIR::GenIR_StorePrimitiveType(
    IRNode*         value,
    IRNode*         addr,
    CorInfoType     corInfoType,
    ReaderAlignType alignment,
    bool            fVolatile,
    IRNode**        newIR
){
    return;
}

IRNode*
GenIR::GenIR_LoadPrimitiveType(
    IRNode*         addr,
    CorInfoType     corInfoType,
    ReaderAlignType alignment,
    bool            fVolatile,
    bool            fInterfReadOnly,
    IRNode**        newIR
){
    return NULL;
}


IRNode*
GenIR::GenIR_LoadNonPrimitiveObj(
    IRNode*         addr,
    CORINFO_CLASS_HANDLE classHandle,
    ReaderAlignType alignment,
    bool            fVolatile,
    IRNode**        newIR
){
    return NULL;
}
IRNode*
GenIR::GenIR_LoadLocal(
    unsigned int    localOrdinal,
    IRNode        **newIR
){
    return NULL;
}


IRNode*
GenIR::GenIR_LoadArgAddress(
    unsigned int    argOrdinal,
    IRNode        **newIR
){
    return NULL;
}

IRNode*
GenIR::GenIR_LoadLocalAddress(
    unsigned int    argOrdinal,
    IRNode        **newIR
){
    return NULL;
}

IRNode*
GenIR::GenIR_LoadConstantI(
    size_t          constant,
    IRNode        **newIR)
{
    return NULL;
}

IRNode*
GenIR::GenIR_LoadConstantI4(
    int constant,
    IRNode** newIR)
{
    return NULL;
}

IRNode*
GenIR::GenIR_LoadConstantI8(
    __int64 constant,
    IRNode** newIR)
{
    return NULL;
}

IRNode*
GenIR::GenIR_LoadConstantR4(
    float constant,
    IRNode** newIR)
{
    return NULL;
}

IRNode*
GenIR::GenIR_LoadConstantR8(
    double constant,
    IRNode** newIR)
{
    return NULL;
}

IRNode*
GenIR::GenIR_LoadFuncptr(
    mdToken token,
    CORINFO_LOOKUP *pLookup,
    IRNode** newIR
){
    return NULL;
}

// GenIR_LoadNull - returns null ptr reference.
IRNode*
GenIR::GenIR_LoadNull(
    IRNode        **newIR
){
    return NULL;
}

IRNode*
GenIR::GenIR_NewArr(
    mdToken        token,
    IRNode*        arg1,
    IRNode**       newIR
){
    return NULL;
}

void
GenIR::GenIR_Pop(
    IRNode         *opr,
    IRNode        **newIR
){
}


IRNode*
GenIR::GenIR_Shift(
    ReaderBaseNS::ShiftOpcode opcode,
    IRNode* shift,
    IRNode* opr,
    IRNode** newIR
){
    return NULL;
}

IRNode*
GenIR::GenIR_Sizeof(
    mdToken         token,
    IRNode        **newIR
){
    return NULL;
}

void
GenIR::GenIR_StoreElem(
    ReaderBaseNS::StElemOpcode  opcode,
    mdToken                     token,
    IRNode                     *arg1, 
    IRNode                     *arg2, 
    IRNode                     *arg3,
    IRNode                    **newIR
){
    return;
}

void
GenIR::GenIR_StoreStaticField(
    mdToken        token,
    IRNode*        ptValue,
    bool           fIsVolatile,
    IRNode**       newIR
){
    return;
}

void
GenIR::GenIR_StoreArg(
    unsigned int    ordinal,
    IRNode*         addr,
    ReaderAlignType alignment,
    bool            fVolatile,
    IRNode**        newIR
){
    return;
}

void
GenIR::GenIR_StoreLocal(
    unsigned int    ordinal,
    IRNode*         addr,
    ReaderAlignType alignment,
    bool            fVolatile,
    IRNode**        newIR
){
    return;
}

IRNode*
GenIR::GenIR_LoadElem(
    ReaderBaseNS::LdElemOpcode  opcode,
    mdToken                     token,
    IRNode                     *arg1,
    IRNode                     *arg2,
    IRNode                    **newIR
){
    return NULL;
}

// GenIR_LoadElemA - Generate the address of an array element specified
// on the stack by an index and an address.  The size of the array elements
// comes from a class token which gives the types of the elements.
IRNode*
GenIR::GenIR_LoadElemA(
    mdToken         token,
    IRNode         *ptIndex,
    IRNode         *ptObj,
    bool            fReadOnlyPrefix,
    IRNode        **newIR
){
    return NULL;
}

IRNode*
GenIR::GenIR_RefAnyType(
    IRNode*        arg1,
    IRNode**       newIR
){
    return NULL;
}

IRNode*
GenIR::GenIR_MakeRefAny(
    mdToken        token,
    IRNode*        object,
    IRNode**       newIR
){
    return NULL;
}

IRNode*
GenIR::GenIR_NewObj(
    mdToken                  token,
    mdToken                  loadFtnToken,
    IRNode**                 newIR
){
    return NULL;
}


IRNode*
GenIR::GenIR_Call(
    ReaderBaseNS::CallOpcode opcode,
    mdToken                  token,
    mdToken                  constraintTypeRef,
    bool                     readOnlyPrefix,
    bool                     tailCallPrefix,
    bool                     isUnmarkedTailCall,
    IRNode**                 newIR
){
    return NULL;
}

// Method is called with empty stack
void
GenIR::GenIR_Jmp(
    ReaderBaseNS::CallOpcode opcode,
    mdToken                  token,
    bool                     hasThis,
    bool                     hasVarArg,
    IRNode**                 newIR
){
    return;
}

IRNode*
GenIR::GenIR_GetHelperCallAddress(
    CorInfoHelpFunc helperId,
    IRNode** newIR
){
    return NULL;
}

// ---------------------------------------------------------------------
// VERIFICATION - client methods required for verification.
// ---------------------------------------------------------------------

void
GenIR::GenIR_ClearCurrentBlock(
    IRNode** newIR
){
    return ;
}


void
GenIR::GenIR_FgDeleteNodesFromBlock(
    FlowGraphNode* block
){
    return;
}



// Common code uses GenIR_CreateSym to notify client of autos and params.
void
GenIR::GenIR_CreateSym(
    int                  localNum,
    bool                 isAuto,  // auto or param
    CorInfoType          corType,
    CORINFO_CLASS_HANDLE hClass,
    bool                 fIsPinned,
    ReaderSpecialSymbolType type      /* optional */
){
    return;
}

// Given operand from stack, produce a new IRNode of legal,
// msil stack type (eg float64, int32 or int64). For example,
// if passed a node with type unsigned char, this function returns
// a node of type int32.
IRNode*
ReaderBase::GenIR_MakeStackTypeNode(
    IRNode* node
){
    return NULL;
}

// When operands are live on the operand stack at a block
// boundary then new operand stacks are created to represent
// the live values incoming to the successor blocks. The
// implementation is earlier asked to create nodes for the
// operand stack copies, and this routine is called to generate
// copies to the new nodes.
//
// Note that depending on the nodes that are used by the
// implementation, an implementation might not need to perform
// copies at this point.
void
GenIR::GenIR_AssignToSuccessorStackNode(
    FlowGraphNode* fg,
    IRNode*        dst,
    IRNode*        src,
    IRNode**       newIR,
    bool*          fmbassign
){
    return;
}

void
ReaderBase::GenIR_DebugError(
    const char* filename,
    unsigned linenumber,
    const char* s
){
#if !defined(NDEBUG)

    if(s) dbprint("%s, line %d: %s\n",filename, linenumber, s);
    RaiseException(JIT64_FATALEXCEPTION_CODE, EXCEPTION_NONCONTINUABLE, 0, NULL);

#endif
}

// Fatal error, reader cannot continue.
void
ReaderBase::GenIR_Fatal(
    int errNum
){
/*#if !defined(NODEBUG)
    if(msg)
        dbprint("%s\n",msg);
#endif*/
    RaiseException(JIT64_FATALEXCEPTION_CODE, 0, 0, NULL);
}


/* KMTF:  This has been removed from the interface!
// GenIR_CopyStackList - Returns the duplicate of a stack's list, but
// doesn't copy the container.
ReaderStackNode*
GenIR::GenIR_CopyStackList(
    ReaderStackNode* stackNode
){
    return NULL;
}
*/

// Routine called when blocks end with a non-empty operand stack. Routine
// must call handleNonEmptyStack. Opr1 and Opr2 are non-null if block end
// instruction was branch with 1 or 2 operands.
void
GenIR::GenIR_MaintainOperandStack(
    IRNode** opr1,
    IRNode** opr2,
    IRNode** newIR
){
    bool fmbassign;
    handleNonEmptyStack(m_currentFgNode,
                        newIR,
                        &fmbassign);
}

// -------------------------------------------------------------------
// Reader Entry
// -------------------------------------------------------------------

int
ReadMSIL(JitContext* jitContext,
         BYTE ** entryAddress,
         unsigned * codeSize
){
    // not supposed to get here
    return 0;
}

// ----------------------------------------------------------------------
// Everything after this point is unused in nop jit.
// ----------------------------------------------------------------------

//
// VOS support routines that must be implemented by client if VOS routines
// will be used.
//
#define REACHED_UNIMPLEMENTED_ROUTINE //(ASSERTM(UNREACHED,"ERROR - reached unimplemented routine"))

IRNode*
GenIR::GenIR_CallHelper(
    CorInfoHelpFunc helperID,
    IRNode* dst,
    IRNode** newIR,
    IRNode* arg1,     /*optional*/
    IRNode* arg2,     /*optional*/
    IRNode* arg3,     /*optional*/
    ReaderAlignType alignment, /*optional*/
    bool fVolaltile,  /*optional*/
    bool fNoCtor,
    bool fCanMoveUp
){
    return NULL;
}

// Special generics helper
IRNode *
GenIR::GenIR_CallRuntimeHandleHelper(CorInfoHelpFunc helper,
                                     IRNode* arg1, IRNode* arg2,
                                      IRNode* arg3,IRNode* arg4,IRNode* arg5,
                                      IRNode* nullCheckArg, IRNode** newIR)
{
    REACHED_UNIMPLEMENTED_ROUTINE;
    return NULL;
}

IRNode*
ReaderBase::GenIR_GetHelperCallAddress(
    CorInfoHelpFunc helperId,
    IRNode** newIR
){
    return NULL;
}

IRNode*
ReaderBase::GenIR_MakePtrDstGCOperand(
    bool isInterior
){
    return NULL;
}

IRNode*
GenIR::GenIR_AddressOfLeaf(
    IRNode* leaf,
    IRNode** newIR
){
    REACHED_UNIMPLEMENTED_ROUTINE;
    return NULL;
}

IRNode*
GenIR::GenIR_AddressOfValue(
    IRNode* leaf,
    IRNode** newIR
){
    return NULL;
}

IRNode*
GenIR::GenIR_ConvertToHelperArgumentType(
    IRNode* opr,
    unsigned int destinationSize,
    IRNode** newIR
){
    return NULL;
}

IRNode*
ReaderBase::GenIR_DerefAddress(
    IRNode*        addr,
    bool           isGc,
    IRNode**       newIR
){
    return NULL;
}

IRNode*
ReaderBase::GenIR_MakePtrNode(
    ReaderPtrType ptrType /* [default param] = READER_PTR_NOT_GC */
){
    REACHED_UNIMPLEMENTED_ROUTINE;
    return NULL;
}

IRNode*
GenIR::GenIR_MakeCallReturnNode(
    CORINFO_SIG_INFO *pSig,
    unsigned         *hiddenMBParamSize,
    PGCLAYOUT_STRUCT *ppGcLayoutStruct
){
    REACHED_UNIMPLEMENTED_ROUTINE;
    return NULL;
}

IRNode*
GenIR::GenIR_GenNullCheck(
    IRNode* tupAddr,
    IRNode** newIR
){
    REACHED_UNIMPLEMENTED_ROUTINE;
    return NULL;
}


void
GenIR::GenIR_DisableGC(
    IRNode*        threadControlBlock,
    IRNode**       newIR
){
    REACHED_UNIMPLEMENTED_ROUTINE;
    return;
}

void
GenIR::GenIR_EnableGC(
    IRNode*        threadControlBlock,
    IRNode**       newIR
){
    REACHED_UNIMPLEMENTED_ROUTINE;
    return;
}

IRNode*
GenIR::GenIR_GenCall(ReaderCallTargetData *callTargetInfo,
                        callArgTriple* argArray, unsigned int numArgs,
                        IRNode** callNode, IRNode** newIR
){
    REACHED_UNIMPLEMENTED_ROUTINE;
    return NULL;
}

void
GenIR::GenIR_OpcodeDebugPrint(BYTE *pbuf, unsigned startOffset, unsigned endOffset)
{
    //printMSIL(pbuf,startOffset,endOffset);
    return;
}


//
// Optional routines, default implementation is minimal. Client
// may wish to overload these.
//

// Client can overload to provide a way to force verification.
bool
GenIR::verForceVerification(
    void
){
    return true;
}

// removeInterference
//
// This function is necessary if the client will be placing aliasable
// references onto the operand stack. For example:
//
// ldloc_2
// ldc_1
// stloc_2
//
// The processing of ldloc_2 could produce a reference to local#2. 
// stloc_2 causes a 1 to be stored to local#2. If a reference to local#2
// were kept on the stack across the store, the stack value would change,
// which is incorrect.
//
// Function is called when we are about to emit code for an instruction
// that could alias (overwrite) elements that are live on the stack.
// Traverse the operand stack and replace anything that can be aliased
// with a temp (and generate an assignment to that temp).
//
// If the client will never place references to aliasable values onto
// the operand stack then this function can be kept empty.
void GenIR::GenIR_RemoveStackInterference(
    IRNode** storePoint
){
    return;
}

void GenIR::GenIR_RemoveStackInterferenceForLocalStore(
    unsigned int opcode,
    unsigned int ordinal,
    IRNode** storePoint
){
    return;
}

//
bool
GenIR::GenIR_IsCall(
    IRNode** newIR
){
    return false;
}

bool           GenIR::GenIR_IsRegionStartBlock(FlowGraphNode* fg)
{
    IRNode *irTemp = fg->startNode->next;
    bool    fIsRegionStart = false;

    while (irTemp && irTemp->kind != IR_block)
    {
        if (irTemp->kind == IR_ehStart)
        {
            fIsRegionStart = true;
            break;
        }
        
        irTemp = irTemp->next;
    }
    
    return fIsRegionStart;
}
 
bool           GenIR::GenIR_IsRegionEndBlock(FlowGraphNode* fg)
{
    

    return GenIR_FgBlockIsRegionEnd(fg);
}

// Function is called with the lone null placeholder
// block. Function takes this empty root block before flow graph building
// and ensures that if there are already IRNodes in the block that they
// are split into an earlier block.
FlowGraphNode*
GenIR::GenIR_FgPrePhase(
    FlowGraphNode  *block
){
    return block;
}

FlowGraphNode* 
GenIR::GenIR_FgGetHeadBlock(void)
{
    return m_headBlock;
}
     

FlowGraphNode* 
GenIR::GenIR_FgGetTailBlock(void)
{
    return m_tailBlock;
}


void
GenIR::GenIR_FgPostPhase(
    void
){
    // make a zero size entry block so we can wire all the unreachable
    // code up to it
    fgSplitBlock(m_headBlock, 0, m_headBlock->startNode->next);
}

#if !defined(NDEBUG)
DWORD
GenIR::GenIR_DBGetFuncHash(
    void
){
    return 0;
}

void
GenIR::GenIR_DBPrintEHRegion(EHRegion * region)
{
    return;
}

void
GenIR::GenIR_DBPrintFGNode(FlowGraphNode * fgNode)
{
    return;
}

void
GenIR::GenIR_DBPrintIRNode(IRNode * irNode)
{
    PrintIRNode(irNode);
    return;
}

void
GenIR::GenIR_DBDumpFunction(void)
{
    return;
}
#endif //!defined(NDEBUG)

void GenIR::Itf_RegisterNumberOfIndirs(int numIndirs)
{
    return;
}

void
GenIR::GenIR_SetSequencePoint(unsigned nLineNumber,ICorDebugInfo::SourceTypes, IRNode **newIR)
{
    return;
}

bool
GenIR::GenIR_NeedSequencePoints()
{
    return false;
}

void
GenIR::GenIR_SetEHInfo(EHRegion* ehRegionTree, EHRegionList* ehRegionList)
{
    return;
}

IRNode*
GenIR::GenIR_DerefAddress(IRNode* address, bool fDstIsGCPtr, IRNode** newIR)
{
    return NULL;
}

IRNode*
GenIR::GenIR_HandleToIRNode(mdToken token, void* embedHandle, void* realHandle, bool fIndirect, bool fReadOnly, bool fRelocatable, bool fCallTarget, IRNode** newIR, bool fIsFrozenObject)
{
    return NULL;
}


IRNode*
GenIR::GenIR_MakePtrDstGCOperand( bool fInteriorGC)
{
    return NULL;
}

IRNode*
GenIR::GenIR_MakePtrNode(ReaderPtrType ptrType /* [default param] = READER_PTR_NOT_GC */)
{
    return NULL;
}

IRNode*
GenIR::GenIR_MakeStackTypeNode(IRNode* node)
{
    return NULL;
}

// Hook to permit client to record call information
void
GenIR::GenIR_FgCall(ReaderBaseNS::OPCODE, mdToken token, unsigned msilOffset, IRNode* block, bool fCanInline)
{
}

bool
GenIR::GenIR_DisableCastClassOptimization()
{
    // Peverify doesn't ever want this to happen -- it would
    //  just end up complicating the flow graph for no reason.
    return true;
}

bool
GenIR::GenIR_DisableIsInstOptimization()
{
    // Peverify doesn't ever want this to happen -- it would
    //  just end up complicating the flow graph for no reason.
    return true;
}

// Given a block and offset, find the first node in the block that comes after the given offset, and
// insert the label before that node.
void           GenIR::GenIR_InsertLabelInBlock(FlowGraphNode* block,
                                                    unsigned int offset,IRNode* newLabel)
{
    IRNode* irTemp = block->startNode->next;

    while (   irTemp->msilOffset < offset
           && irTemp->kind != IR_block)
    {
        irTemp = irTemp->next;
    }
    
    //ASSERTNR(irTemp->prev != NULL)

    // irTemp is now the IRNode immediately after where we want to be inserted.
    newLabel->prev = irTemp->prev;
    irTemp->prev = newLabel;
    newLabel->next = irTemp;
    newLabel->prev->next = newLabel;

    return;
}

IRNode*        GenIR::GenIR_FindBlockSplitPointAfterNode(IRNode* node)
{
    return node->next;
}

IRNode*        GenIR::GenIR_ExitLabel(void)
{
    return m_exitLabel;
}

IRNode*        GenIR::GenIR_EntryLabel(void)
{
    return m_entryLabel;
}

// Function is passed a try region, and is expected to return the first label or instruction
// after the region.
IRNode*        GenIR::GenIR_FindTryRegionEndOfClauses(EHRegion* tryRegion)
{
    return NULL;
}

void           GenIR::GenIR_InsertEHAnnotationNode(IRNode* insertionPointNode,
                                                 IRNode* insertNode)
{
    // Let's put insertNode after insertionPointNode;
    insertNode->next = insertionPointNode->next;
    insertionPointNode->next = insertNode;
    insertNode->prev = insertionPointNode;
    insertNode->next->prev = insertNode;
    
    return;
}

IRNode*        GenIR::GenIR_MakeLabel(unsigned int targetOffset, EHRegion* region)
{
    IRNode  *newIR = (IRNode*)getProcMemory(sizeof(IRNode));

    newIR->kind = IR_label;
    newIR->isFallThrough = true;
    newIR->msilOffset = targetOffset;
    newIR->region = region;
    newIR->branchList = NULL;

    return newIR;
}

void           GenIR::GenIR_MarkAsEHLabel(IRNode* labelNode)
{
    // no-op
    return;
}

IRNode*        GenIR::GenIR_MakeTryEndNode(void)
{
    return GenIR_MakeRegionEndNode(ReaderBaseNS::RGN_TRY);
}

IRNode*        GenIR::GenIR_MakeRegionStartNode(ReaderBaseNS::RegionKind regionType)
{
    IRNode* newIR = (IRNode*)getProcMemory(sizeof(IRNode));

    newIR->kind = IR_ehStart;
    newIR->isFallThrough = true;
    newIR->regionKind = regionType;


    return newIR;
}

IRNode*        GenIR::GenIR_MakeRegionEndNode(ReaderBaseNS::RegionKind regionType)
{
    IRNode* newIR = (IRNode*)getProcMemory(sizeof(IRNode));

    newIR->kind = IR_ehEnd;
    newIR->isFallThrough = false;
    newIR->regionKind = regionType;

    return newIR;
}

// Flowgraph Routine Implementations
// Interface to GenIR defined Flow Graph structures.
// Implementation Supplied by Jit Client
FlowGraphNode*       FgNodeGetNext(FlowGraphNode* fgNode)
{
    return fgNode->next;
}

bool                 FgNodeIsVisited(FlowGraphNode* fgNode)
{
    return fgNode->isVisited;    
}

void                 FgNodeSetVisited(FlowGraphNode* fgNode,bool visited)
{
    fgNode->isVisited = visited;
}

EHRegion*            FgNodeGetRegion(FlowGraphNode* fgNode)
{
    return fgNode->rgn;
}

void                 FgNodeSetRegion(FlowGraphNode* fgNode,EHRegion* ehRegion)
{
    fgNode->rgn = ehRegion;
}


FlowGraphEdgeList*   FgNodeGetFirstNotDeleted(FlowGraphEdgeList *edge)
{
    while (edge && edge->isDeletedEdge)
    {
        edge = edge->next;
    }
    return edge;
}


FlowGraphEdgeList*   FgNodeGetSuccessorList(FlowGraphNode* fgNode)
{
    return FgNodeGetFirstNotDeleted(fgNode->succList);
}

FlowGraphEdgeList*   FgNodeGetPredecessorList(FlowGraphNode* fgNode)
{
    return FgNodeGetFirstNotDeleted(fgNode->predList);
}

// Get the special block-start placekeeping node
IRNode*              FgNodeGetStartIRNode(FlowGraphNode* fgNode)
{
    return fgNode->startNode;
}

// Get the first non-placekeeping node in block
IRNode*              FgNodeGetStartInsertIRNode(FlowGraphNode* fgNode)
{
    return fgNode->startNode->next;
}

// Get the special block-end placekeeping node
IRNode*              FgNodeGetEndIRNode(FlowGraphNode* fgNode)
{
    IRNode* ir = fgNode->startNode->next;

    while (ir->kind != IR_block)
    {
        if (ir->next == NULL)
            return ir;
        
        ir = ir->next;
    }
    
    return ir->prev;
}

// Get the last non-placekeeping node in block
IRNode*              FgNodeGetEndInsertIRNode(FlowGraphNode* fgNode)
{
    return FgNodeGetEndIRNode(fgNode);
}

IRNode*              FgNodeGetEndIRInsertionPoint(FlowGraphNode* fgNode)
{
    return FgNodeGetEndIRNode(fgNode);
}

unsigned int         FgNodeGetStartMSILOffset(FlowGraphNode* fg)
{
    return fg->startOffset;
}

void FgNodeSetIBCNotReal(FlowGraphNode* fg)
{
}

void                 FgNodeSetStartMSILOffset(FlowGraphNode* fg, unsigned int offset)
{
    fg->startOffset = offset;
    IRNode *sNode = fg->startNode;
    if (sNode->kind == IR_block)
        sNode->msilOffset = offset;
#ifdef DEBUG
    else
    {
        printf("startnode must be block\n");
        DebugBreak();
    }
#endif
}

unsigned int         FgNodeGetEndMSILOffset(FlowGraphNode* fg)
{
    return fg->endOffset;
}

void                 FgNodeSetEndMSILOffset(FlowGraphNode* fgNode, unsigned int offset)
{
    fgNode->endOffset = offset;
}

GLOBALVERIFYDATA *   FgNodeGetGlobalVerifyData(FlowGraphNode* fg)
{
    return fg->globVerData;
}

void                 FgNodeSetGlobalVerifyData(FlowGraphNode* fg, GLOBALVERIFYDATA * gvData)
{
    fg->globVerData = gvData;
}

void                 FgNodeSetOperandStack(FlowGraphNode* fg, ReaderStack* stack)
{
    fg->stack = stack;
}

ReaderStack*         FgNodeGetOperandStack(FlowGraphNode* fg)
{
    return fg->stack;
}

FlowGraphNode*       FgNodeGetIDom(FlowGraphNode* fg)
{
    // This routine is used for the dominator based CSE in the reader for SharedStaticBase initialization,
    // ClassInit, and TCB Init.  If we pass back NULL for an IDOM, then the logic is shut off [unless
    // one of these inits has occured in the same basic block]...
    
    return NULL;
}

unsigned             FgNodeGetBlockNum(FlowGraphNode* fg)
{
    return fg->nBlockNum;
}

FlowGraphEdgeList*   FgEdgeListGetNextSuccessor(FlowGraphEdgeList* fgEdge)
{
    return FgNodeGetFirstNotDeleted(fgEdge->next);
}

FlowGraphEdgeList*   FgEdgeListGetNextPredecessor(FlowGraphEdgeList* fgEdge)
{
    return FgNodeGetFirstNotDeleted(fgEdge->next);
}

FlowGraphNode*       FgEdgeListGetSource(FlowGraphEdgeList* fgEdge)
{
    return fgEdge->block;
}

FlowGraphNode*       FgEdgeListGetSink(FlowGraphEdgeList* fgEdge)
{
    return fgEdge->block;
}

bool                 FgEdgeListIsNominal(FlowGraphEdgeList* fgEdge)
{
    return fgEdge->isNominal;
}

bool                 FgEdgeListIsFake(FlowGraphEdgeList* fgEdge)
{
    return fgEdge->isFakeForUnreachable;
}

void                 FgEdgeListMakeFake(FlowGraphEdgeList* fgEdge)
{
    fgEdge->isFakeForUnreachable = 1;
}




// Region Interface
// Interface to GenIR defined EHRegion structure
// Implementation Supplied by Jit Client
EHRegion*            RgnAllocateRegion(GenIR* genIR)
{
    EHRegion* rgn = (EHRegion*)genIR->getProcMemory(sizeof(EHRegion));
    
    
    return rgn;
}

EHRegionList*        RgnAllocateRegionList(GenIR* genIR)
{
    EHRegionList* rgnList = (EHRegionList*)genIR->getProcMemory(sizeof(EHRegionList*));


    return rgnList;
}

EHRegionList*        RgnListGetNext(EHRegionList* ehRegionList)
{
    return ehRegionList->next;
}

void                 RgnListSetNext(EHRegionList* ehRegionList,EHRegionList* next)
{
    ehRegionList->next = next;
}

EHRegion*            RgnListGetRgn(EHRegionList* ehRegionList)
{
    return ehRegionList->region;
}

void                 RgnListSetRgn(EHRegionList* ehRegionList,EHRegion* rgn)
{
    ehRegionList->region = rgn;
}

ReaderBaseNS::RegionKind RgnGetRegionType(EHRegion* ehRegion)
{
    return ehRegion->kind;
}

void                 RgnSetRegionType(EHRegion* ehRegion,ReaderBaseNS::RegionKind type)
{
    ehRegion->kind = type;    
}

DWORD                RgnGetStartMSILOffset(EHRegion* ehRegion)
{
    return ehRegion->extentStart;
}

void                 RgnSetStartMSILOffset(EHRegion* ehRegion,DWORD offset)
{
    ehRegion->extentStart = offset;
}

DWORD                RgnGetEndMSILOffset(EHRegion* ehRegion)
{
    return ehRegion->extentEnd;
}

void                 RgnSetEndMSILOffset(EHRegion* ehRegion,DWORD offset)
{
    ehRegion->extentEnd = offset;
}

IRNode*              RgnGetHead(EHRegion* ehRegion)
{
    return ehRegion->headNode;
}

void                 RgnSetHead(EHRegion* ehRegion, IRNode* head)
{
    ehRegion->headNode = head;
}

IRNode*              RgnGetLast(EHRegion* ehRegion)
{
    return ehRegion->lastNode;
}

void                 RgnSetLast(EHRegion* ehRegion, IRNode* last)
{
    ehRegion->lastNode = last;
}

bool                 RgnGetIsLive(EHRegion *ehRegion)
{
    return ehRegion->isLive;
}

void                 RgnSetIsLive(EHRegion *ehRegion, bool live)
{
    ehRegion->isLive = live;
}

void                 RgnSetParent(EHRegion *ehRegion,EHRegion* parent)
{
    ehRegion->parent = parent;
}

EHRegion*            RgnGetParent(EHRegion *ehRegion)
{
    return ehRegion->parent;
}

void                 RgnSetChildList(EHRegion *ehRegion,EHRegionList* children)
{
    ehRegion->childList = children;
}

EHRegionList*        RgnGetChildList(EHRegion *ehRegion)
{
    return ehRegion->childList;
}

bool                 RgnGetHasNonLocalFlow(EHRegion *ehRegion)
{
    return ehRegion->hasNonLocalFlow;
}

void                 RgnSetHasNonLocalFlow(EHRegion *ehRegion,bool nonLocalFlow)
{
    ehRegion->hasNonLocalFlow = nonLocalFlow;
}

IRNode*              RgnGetEndOfClauses(EHRegion* ehRegion)
{
    return NULL;
}

void                 RgnSetEndOfClauses(EHRegion* ehRegion,IRNode* node)
{
    // no-op
    return;
}

IRNode*              RgnGetTryBodyEnd(EHRegion* ehRegion)
{
    return NULL;
}

void                 RgnSetTryBodyEnd(EHRegion* ehRegion,IRNode* node)
{
    // no-op
    return;
}

ReaderBaseNS::TryKind RgnGetTryType(EHRegion* ehRegion)
{
    return ehRegion->tryType;
}

void                 RgnSetTryType(EHRegion* ehRegion,ReaderBaseNS::TryKind type)
{
    ehRegion->tryType = type;
}

int                  RgnGetTryCanonicalExitOffset(EHRegion* tryRegion)
{
    return tryRegion->canonicalExitOffset;
}

void                 RgnSetTryCanonicalExitOffset(EHRegion* tryRegion, int offset)
{
    tryRegion->canonicalExitOffset = offset;
}

EHRegion*            RgnGetExceptFilterRegion(EHRegion* ehRegion)
{
    return ehRegion->filterRegion;
}

void                 RgnSetExceptFilterRegion(EHRegion* ehRegion,EHRegion* filterRegion)
{
    ehRegion->filterRegion = filterRegion;
}

EHRegion*            RgnGetExceptTryRegion(EHRegion* ehRegion)
{
    return ehRegion->tryRegion;
}

void                 RgnSetExceptTryRegion(EHRegion* ehRegion, EHRegion* tryRegion)
{
    ehRegion->tryRegion = tryRegion;
}

bool                 RgnGetExceptUsesExCode(EHRegion* ehRegion)
{
    return ehRegion->usesExCode;
}

void                 RgnSetExceptUsesExCode(EHRegion* ehRegion, bool usesExceptionCode)
{
    ehRegion->usesExCode = usesExceptionCode;
}

EHRegion*            RgnGetFilterTryRegion(EHRegion* ehRegion)
{
    return ehRegion->tryRegion;
}

void                 RgnSetFilterTryRegion(EHRegion* ehRegion, EHRegion* tryRegion)
{
    ehRegion->tryRegion = tryRegion;
}

EHRegion*            RgnGetFilterHandlerRegion(EHRegion* ehRegion)
{
    return ehRegion->handlerRegion;
}

void                 RgnSetFilterHandlerRegion(EHRegion* ehRegion, EHRegion* handler)
{
    ehRegion->handlerRegion = handler;
}

EHRegion*            RgnGetFinallyTryRegion(EHRegion* finallyRegion)
{
    return finallyRegion->tryRegion;
}

void                 RgnSetFinallyTryRegion(EHRegion* finallyRegion, EHRegion* tryRegion)
{
    finallyRegion->tryRegion = tryRegion;
}

bool RgnGetFinallyEndIsReachable(EHRegion* finallyRegion)
{
    return finallyRegion->endFinallyIsReachable;
}

void RgnSetFinallyEndIsReachable(EHRegion* finallyRegion, bool isReachable)
{
    finallyRegion->endFinallyIsReachable = isReachable;
}

EHRegion*            RgnGetFaultTryRegion(EHRegion* faultRegion)
{
    return faultRegion->tryRegion;
}

void                 RgnSetFaultTryRegion(EHRegion* faultRegion,EHRegion* tryRegion)
{
    faultRegion->tryRegion = tryRegion;
}

EHRegion*            RgnGetCatchTryRegion(EHRegion* catchRegion)
{
    return catchRegion->tryRegion;
}

void                 RgnSetCatchTryRegion(EHRegion* catchRegion,EHRegion* tryRegion)
{
    catchRegion->tryRegion = tryRegion;
}

mdToken              RgnGetCatchClassToken(EHRegion* catchRegion)
{
    return catchRegion->catchClassToken;
}

void                 RgnSetCatchClassToken(EHRegion* catchRegion, mdToken token)
{
    catchRegion->catchClassToken = token;
}

IRNode*              IRNodeGetNext(IRNode* node)
{
    return (node != NULL) ? node->next : NULL;
}

bool IRNodeIsBranch(IRNode* node)
{
    return (node->kind == IR_branch || node->kind == IR_jmp);
}

IRNode*              IRNodeGetInsertPointAfterMSILOffset(IRNode* node, unsigned int offset)
{
    while (node->kind != IR_block 
           && node->kind != IR_switch
           && node->kind != IR_branch
           && node->kind != IR_jmp
           && node->kind != IR_throw_rethrow
           && node->msilOffset <= offset)
    {
        node = node->next;
    }

    return node;
}

IRNode*              IRNodeGetInsertPointBeforeMSILOffset(IRNode* node, unsigned int offset)
{
    while (node->kind != IR_block && node->msilOffset >= offset)
    {
        node = node->prev;
    }

    return node;
}

IRNode*              IRNodeGetFirstLabelOrInstrNodeInEnclosingBlock(IRNode* handlerStartNode)
{
    FlowGraphNode *curFgNode = IRNodeGetEnclosingBlock(handlerStartNode);

    IRNode *firstIRNode = handlerStartNode;

    // find first label or instr
    firstIRNode = curFgNode->startNode;
    ASSERT(firstIRNode->kind == IR_block);
    firstIRNode = firstIRNode->next;
    while (firstIRNode)
    {
        if (firstIRNode->kind != IR_ehStart)
            return firstIRNode;
        firstIRNode = firstIRNode->next;
    }
    return NULL;
}

unsigned int         IRNodeGetMSILOffset(IRNode* node)
{
    return node->msilOffset;
}

void                 IRNodeLabelSetMSILOffset(IRNode* node, unsigned int labelMSILOffset)
{
    node->msilOffset = labelMSILOffset;
}

void                 IRNodeBranchSetMSILOffset(IRNode* branchNode, unsigned int offset)
{
    branchNode->msilOffset = offset;
}

void                 IRNodeExceptSetMSILOffset(IRNode* branchNode, unsigned int offset)
{
    branchNode->msilOffset = offset;
}

void                 IRNodeInsertBefore(IRNode* insertionPointTuple, IRNode* newNode)
{
    IRNode  *prev = insertionPointTuple->prev;

    //ASSERTNR(insertionPointTuple == prev->next);

    prev->next = newNode;
    newNode->prev = prev;

    newNode->next = insertionPointTuple;
    insertionPointTuple->prev = newNode;
}

void                 IRNodeInsertAfter(IRNode* insertionPointTuple, IRNode* newNode)
{
    IRNode  *next = insertionPointTuple->next;

    insertionPointTuple->next = newNode;
    newNode->prev = insertionPointTuple;
    
    newNode->next = next;
    if (next != NULL)
    {
        //ASSERTNR(next->prev == insertionPointTuple);
        next->prev = newNode;
    }
}

void                 IRNodeSetRegion(IRNode* node, EHRegion* region)
{
    node->region = region;
}

EHRegion*            IRNodeGetRegion(IRNode* node)
{
    return node->region;
}

FlowGraphNode*       IRNodeGetEnclosingBlock(IRNode* node)
{
    while (node->kind != IR_block)
    {
        node = node->prev;
    }

    return node->fgNode;
}

bool                 IRNodeIsLabel(IRNode* node)
{
    return (node->kind == IR_label);
}

bool                 IRNodeIsEHFlowAnnotation(IRNode* node)
{
    return (node->kind == IR_ehFlow);
}

// Implementation Supplied by Jit Client.
BranchList*          BranchListGetNext(BranchList* branchList)
{
    return branchList->next;
}

IRNode*              BranchListGetIRNode(BranchList* branchList)
{
    return branchList->branch;
}


//
// REQUIRED Flow and Region Graph Manipulation Routines
//
IRNode*        GenIR::FgNodeFindStartLabel(FlowGraphNode* block)
{
    IRNode  *candIRLabel = block->startNode;

    while (candIRLabel && candIRLabel->kind != IR_label)
    {
        candIRLabel = candIRLabel->next;
        if (candIRLabel->kind == IR_block)
            return NULL;
    }


    return candIRLabel;
}


BranchList*    GenIR::GenIR_FgGetLabelBranchList(IRNode* labelNode)
{
    return labelNode->branchList;
}

void fgHelperInsertNodeAtEndOfBlock(IRNode* newNode, IRNode* insertionNode)
{
    IRNode *irTemp = insertionNode;

    while (irTemp->next && irTemp->next->kind != IR_block)
    {
        irTemp = irTemp->next;
    }
    
    newNode->next = irTemp->next;
    irTemp->next = newNode;
    if (newNode->next)
        newNode->next->prev = newNode;
    newNode->prev = irTemp;
}

void           GenIR::GenIR_InsertHandlerAnnotation(EHRegion* handlerRegion)
{
    FlowGraphNode *fgnode = m_headBlock;
    EHRegion *tryRegion = RgnGetParent(handlerRegion);

    // edges 
    //    from : all blocks of try
    //      to : handler region head
    while (fgnode)
    {
        if (fgnode->rgn == tryRegion)
        {
            IRNode *branch = (IRNode*)getProcMemory(sizeof(IRNode));

            branch->kind = IR_ehFlow;
            branch->msilOffset = fgnode->startOffset;
            branch->isFallThrough = true;  
    
            //fgHelperInsertNodeAtEndOfBlock(branch, fgnode->startNode);
            IRNodeInsertAfter(fgnode->startNode, branch);   
            
            IRNode *irBlock = handlerRegion->headNode;
            while (irBlock->kind != IR_block) {
                irBlock = irBlock->prev;
            }
            GenIR_FgAddArc(branch, fgnode, irBlock->fgNode);
        }
        fgnode = fgnode->next;
    }
    return;
}

void           GenIR::GenIR_InsertRegionAnnotation(IRNode* regionStartTuple,
                                         IRNode* regionEndTuple)
{
    return;
}

void           GenIR::GenIR_FgAddLabelToBranchList(IRNode* labelNode, 
                                         IRNode* branchNode)
{
    //ASSERTNR(labelNode != NULL);
    //ASSERTNR(branchNode != NULL);
    
    // This is putting the BRANCHing instruction on the BranchList associated with the label.
    BranchList  *newBranchListElem;

    newBranchListElem = (BranchList*)getProcMemory(sizeof(BranchList));

    newBranchListElem->branch = branchNode;
    newBranchListElem->next = labelNode->branchList;

    labelNode->branchList = newBranchListElem;


    // And this is putting the LABEL on the BranchList associated with the BRANCHing instruction.

    newBranchListElem = (BranchList*)getProcMemory(sizeof(BranchList));

    newBranchListElem->branch = labelNode;
    newBranchListElem->next = branchNode->branchList;

    branchNode->branchList = newBranchListElem;
}

void           GenIR::GenIR_FgAddArc(IRNode* branchNode, FlowGraphNode *source,
                           FlowGraphNode *sink)
{
    FlowGraphEdgeList *edge1, *edge2;

    edge1 = (FlowGraphEdgeList*)getProcMemory(sizeof(FlowGraphEdgeList));
    edge1->block = sink;
    edge1->next = source->succList;
    edge1->isNominal = false;
    edge1->isFakeForUnreachable = false;
    source->succList = edge1;


    edge2 = (FlowGraphEdgeList*)getProcMemory(sizeof(FlowGraphEdgeList));
    edge2->block = source;
    edge2->next = sink->predList;
    edge2->isNominal = false;
    edge1->isFakeForUnreachable = false;
    sink->predList = edge2;

    if (branchNode != NULL)
    {
        if (   IRNodeIsEHFlowAnnotation(branchNode)
            )
        {
            edge1->isNominal = edge2->isNominal = true;
        }
    }
}

bool           GenIR::GenIR_FgBlockIsRegionEnd(FlowGraphNode* block)
{
    IRNode *irTemp = block->startNode->next;
    bool    fIsRegionEnd = false;

    while (irTemp && irTemp->kind != IR_block)
    {
        if (irTemp->kind == IR_ehEnd)
        {
            fIsRegionEnd = true;
            break;
        }
        
        irTemp = irTemp->next;
    }
    
    return fIsRegionEnd;
}

void           GenIR::GenIR_FgDeleteBlock(FlowGraphNode* block)
{
}

void           GenIR::GenIR_FgDeleteEdge(FlowGraphEdgeList* arc)
{
    arc->isDeletedEdge = 1;
}


IRNode*        GenIR::GenIR_FgMakeSwitch(IRNode* defaultLabel, IRNode* insert)
{
    IRNode  *newIR = (IRNode*)getProcMemory(sizeof(IRNode));

    newIR->kind = IR_switch;
    newIR->isFallThrough = true;

    fgHelperInsertNodeAtEndOfBlock(newIR, insert);

    return newIR;
}

IRNode*        GenIR::GenIR_FgMakeThrow(IRNode* insert)
{
    IRNode  *newIR = (IRNode*)getProcMemory(sizeof(IRNode));

    newIR->kind = IR_throw_rethrow;
    newIR->isFallThrough = false;

    fgHelperInsertNodeAtEndOfBlock(newIR, insert);

    return newIR;
}

IRNode*        GenIR::GenIR_FgMakeRethrow(IRNode* insert)
{
    IRNode  *newIR = (IRNode*)getProcMemory(sizeof(IRNode));

    newIR->kind = IR_throw_rethrow;
    newIR->isFallThrough = false;

    fgHelperInsertNodeAtEndOfBlock(newIR, insert);

    return newIR;
}

IRNode*        GenIR::GenIR_FgAddCaseToCaseList(IRNode* switchNode,IRNode* labelNode,
                                      unsigned element)
{
    // This is some hackiness here.  What I'm doing is I'm using the branchList field
    // on a switch to be effectively a caseList.  That is, we use the BranchList*, and make
    // the IRNode* called "branch" simply point to the labels that we are interested in.
    //
    // The caller of FgAddCaseToCaseList wants to know what to point the actual BranchList
    // on the labelNode to.  That's why we return switchNode.... we're saying, simply wire up
    // the label to be pointing to the switch itself.
    
    BranchList  *caseList = (BranchList*)getProcMemory(sizeof(BranchList));

    caseList->next = switchNode->branchList;
    caseList->branch = labelNode;
    switchNode->branchList = caseList;

    return switchNode;
}

IRNode*        GenIR::GenIR_FgMakeBranch(IRNode* labelNode, IRNode* insertNode,
                               unsigned int currentOffset, bool fIsConditional,
                               bool fIsNominal)
{
    // Can't have a nominal conditional branch
    //ASSERTNR(!(fIsConditional && fIsNominal));

    IRNode *branch = (IRNode*)getProcMemory(sizeof(IRNode));

    if (fIsNominal)
    {
        branch->kind = IR_jmp;   
    }
    else
    {
        branch->kind = IR_branch;
    }

    branch->msilOffset = currentOffset;
    branch->isFallThrough = fIsConditional;

    fgHelperInsertNodeAtEndOfBlock(branch, insertNode);

    return branch;
}

IRNode* GenIR::GenIR_FgMakeEndFinally(IRNode* insertTup, unsigned int currentOffset, bool fIsLexicalEnd)
{
    IRNode *node = (IRNode*)getProcMemory(sizeof(IRNode));

    node->kind = IR_endfinally;

    node->msilOffset = currentOffset;
    node->isFallThrough = false;

    fgHelperInsertNodeAtEndOfBlock(node, insertTup);

    return node;
}




void
GenIR::GenIR_FgSetBlockToRegion(
    FlowGraphNode* block,
    EHRegion*      region,
    unsigned int   lastOffset
){
    IRNode *node, *lastNode;

    FgNodeSetRegion(block,region);
    node = FgNodeGetStartInsertIRNode(block);
    lastNode = FgNodeGetEndIRNode(block);
    while (node != lastNode) {
        if (IRNodeGetMSILOffset(node) >= lastOffset) {
            // we need to split the block here because of region boundary
            FlowGraphNode * newBlock = GenIR_FgSplitBlock(block,node);
            FgNodeSetEndMSILOffset(newBlock,FgNodeGetEndMSILOffset(block));
            FgNodeSetEndMSILOffset(block,lastOffset);
            FgNodeSetStartMSILOffset(newBlock,lastOffset);
            FgNodeSetRegion(newBlock,IRNodeGetRegion(node));
            break;
        }
        if (!GenIR_FgIsExceptRegionStartNode(node)) {
            IRNodeSetRegion(node,region);
        }
        node = IRNodeGetNext(node);
        //ASSERTNR(NULL != node);
    }
}

// create a new block starting before node
FlowGraphNode* GenIR::GenIR_FgSplitBlock(FlowGraphNode* block, IRNode* node)
{

    FlowGraphNode *newBlock;

    newBlock = (FlowGraphNode*)getProcMemory(sizeof(FlowGraphNode));
    newBlock->succList = newBlock->predList = NULL;

    newBlock->next = block->next;
    block->next = newBlock;

    // create new block irnode
    newBlock->startNode = (IRNode*)getProcMemory(sizeof(IRNode));
    newBlock->startNode->kind = IR_block;
    newBlock->startNode->msilOffset = node->msilOffset;
    newBlock->startNode->fgNode = newBlock;
    newBlock->startNode->prev = node->prev;
    newBlock->startNode->isFallThrough = true;
    node->prev->next = newBlock->startNode;
    newBlock->startNode->next = node;

    node->prev = newBlock->startNode;

    return newBlock;
}

bool           GenIR::GenIR_FgIsExceptRegionStartNode(IRNode* node)
{
    return (node->kind == IR_ehStart);
}

bool           GenIR::GenIR_FgBlockHasFallThrough(class FlowGraphNode * block)
{
    IRNode *cursor = FgNodeGetEndIRNode(block);
    // falls through if every irnode in the block falls through.
    while (cursor->kind != IR_block)
    {
        if (cursor->isFallThrough == false)
            return false;
        cursor = cursor->prev;
    }
    return true;
}

unsigned       GenIR::GenIR_FgGetBlockCount(void)
{
    // This code is only called on the ESSA path.  In fact, it should be conditionally compiled, IMHO
    
    // ASSERTNR(UNREACHED)
    return 0;
}

FlowGraphNode* GenIR::GenIR_FgNodeGetIDom(FlowGraphNode* fg)
{
    return NULL;
}


//
//
void       GenIR::Ssa_LocAddressTaken(unsigned int idx)
{
    
    // ASSERTNR(UNREACHED)
    return;
}

void       GenIR::Ssa_ArgAddressTaken(unsigned int idx)
{
    
    // ASSERTNR(UNREACHED)
    return;
}

void       GenIR::Ssa_VirtPhiPass(FlowGraphNode* h)
{
    
    // ASSERTNR(UNREACHED)
    return;
}

void       GenIR::Ssa_OptRenameInit(void)
{
    
    // ASSERTNR(UNREACHED)
    return;
}

void       GenIR::Ssa_CreateDefMap(int * ssaParamCnt, int * ssaLocalCnt)
{
    
    // ASSERTNR(UNREACHED)
#ifdef READER_ESSA
    *ssaParamCnt = 0;
    *ssaLocalCnt = 0;
#endif
    return;
}

void       GenIR::Ssa_AddDefPoint(unsigned int def, int offset, bool isLocal)
{
    
    // ASSERTNR(UNREACHED)
    return;
}

int        GenIR::Ssa_HshMSILScalar(unsigned int opcode,BYTE *operPtr,FlowGraphNode *fg)
{
    
    // ASSERTNR(UNREACHED)
    return 0;
}

void       GenIR::Ssa_LookAheadEdge(FlowGraphNode * curr, FlowGraphNode * succ, FlowGraphEdgeList * edge)
{
    
    // ASSERTNR(UNREACHED)
    return;
}

void       GenIR::Ssa_OptPop(FlowGraphNode *block)
{
    
    // ASSERTNR(UNREACHED)
    return;
}

void       GenIR::Ssa_PushPhis(FlowGraphNode *block)
{
    
    // ASSERTNR(UNREACHED)
    return;
}
//


bool GenIR::GenIR_TypesCompatible(IRNode* src1, IRNode* src2)
{
    // The no-op JIT does not put anything on the reader stack so we can just ignrore this side-effect
    // of web propagation...
    return true;
}

void
GenIR::GenIR_VerifyStaticAlignment(
    void*       fieldAddress,
    CorInfoType corInfoType,
    unsigned    minClassAlign
){
    return;
}

bool
GenIR::GenIR_FgOptRecurse(
    ReaderBaseNS::CallOpcode opcode,
    mdToken token,
    bool    fIsUnmarkedTailCall
){
    // We have no need to do a recursive tail call optimization in the no-op JIT.
    return false;
}

void
GenIR::GenIR_FgRevertRecursiveBranch(
    IRNode*      branchTup
){
    //ASSERTNR(TU_OPCODE(branchTup) == OPGOTO);
    //ASSERTNR(TU_IS_MSIL_JMP(branchTup));

    // reverts to nominal edge
    //THX_TupReplaceBranchTarget(branchTup, GenIR_ExitLabel(), FALSE, ciPtr);

    // The code in JIT64 does what you see above.  I'm hoping that this is unnecessary,
    // since we always return false in GenIR_FgOptRecurse().

    // ASSERTNR(UNREACHED);

    return;
}

bool
GenIR::GenIR_FgEHRegionStartRequiresBlockSplit(IRNode* node)
{
    bool    fReturn = false;
    IRNode *irPrev;

    irPrev = node->prev;

    if (   irPrev != NULL
        &&
           (   irPrev->kind == IR_ehFlow
            || irPrev->kind == IR_ehStart
            || irPrev->kind == IR_ehEnd
           ))
    {
        fReturn = true;
    }

    return fReturn;
}

IRNode *
GenIR::GenIR_getAddressOfInterfaceVtable(IRNode *tupVtable, 
                    CORINFO_CLASS_HANDLE classHandle, IRNode **newIR)
{
    return NULL;
}

IRNode* 
GenIR::GenIR_MakeDirectCallTargetNode(
    CORINFO_METHOD_HANDLE hMethod,
    void *codeAddr
    )
{
    return NULL;
}
bool
GenIR::GenIR_CanMakeDirectCall(ReaderCallTargetData *callTargetData)
{
    return false;
}

bool
GenIR::GenIR_IsTmpVarAndReg( IRNode *val )
{
    return false;
}


