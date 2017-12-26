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
// READER.CPP
//
// Common Reader Implementation
//
// This file contains all common reader code.
//
// The common reader's operation revolves around two central classes.
// ReaderBase:: the common reader class
// GenIR::      an opaque vessel for holding the client's state
//
// The GenIR class is opaque to the common reader class, all manipulations of
// GenIR are performed by client implemented code.
//
// The common reader generates code through the methods that are implemented in
// this file, and static member functions that are implemented by the client.
//

#include "reader.h"
#include "newvstate.h"
#include "imeta.h"
// --------------------------------------------------------------
//   private functions and data used by common reader.
// --------------------------------------------------------------

#define MSIL_READ_8_BYTES       8
#define MSIL_READ_4_BYTES       4
#define MSIL_READ_2_BYTES       2
#define MSIL_READ_1_BYTE        1

#ifndef UINT_MAX
#define UINT_MAX 0xffffffffu
#endif

// Data start offset for value class data.
#define VALUE_CLASS_DATA_START_OFFSET 8

#define BADCODE(msg)    (ReaderBase::verGlobalError(msg))

// Max elements per entry in labelListArray.
#define LABEL_LIST_ARRAY_STRIDE 32

// -----------------------------------------------------------------
// Debugging
// -----------------------------------------------------------------

// Macro to determine the default behavior of automatically
// detecting tail calls (without the "tail." opcode in MSIL).
// Can override by setting COMPLUS_TailCallOpt.
#define DEFAULT_TAIL_CALL_OPT          1

static UINT
doTailCallOpt()
{
    return 0;
}

#ifndef NODEBUG
static bool
checkTailCallMax()
{
    return false;
}
#endif // !NODEBUG

#define ASSERTM(p,m)   do { if (!(p)) {ReaderBase::GenIR_DebugError(__FILE__,__LINE__,m);}} while (0)
#define ASSERT(p)      ASSERTM(p,#p)
#define UNREACHED      0
#ifndef _MSC_VER
#define ASSUME(p)      __assume(p)
#else
#define ASSUME(p)
#endif

#if !defined(_DEBUG)
#define NODEBUG 1
#define RELEASE 1
#endif

#ifndef NODEBUG
#define ASSERTMNR(p,m) ASSERTM(p,m)
#define ASSERTNR(p)    ASSERTMNR(p,#p)
#define ASSERTDBG(p)   ASSERTM(p,#p)
#define TODO()         ASSERTMNR(0,"TODO\n")
#else
#define ASSERTMNR(p,m) ASSUME(p)
#define ASSERTNR(p)    ASSUME(p)
#define ASSERTDBG(p)   
#define TODO()         
#endif


// NOTE:  READER_HOST_PTR_SIZE, really should be a callback from the reader
//        into its client.  We can make that change if ever anyone cares.
//        Also, this value is only to be used in the reader do *not* change
//        it's scope.  If you are in UTC and want this info, you probably
//        should be using TY_SIZE(TyPtrType).
#define READER_HOST_PTR_SIZE    sizeof(void*)

#define CANONICAL_EXIT_INIT_VAL (-2)

// OPCODE REMAP
ReaderBaseNS::CallOpcode remapCallOpcode(ReaderBaseNS::OPCODE op)
{
    ReaderBaseNS::CallOpcode callOp = (ReaderBaseNS::CallOpcode)opcodeRemap[op];
    ASSERTNR(callOp >= 0 && callOp < ReaderBaseNS::LAST_CALL_OPCODE);
    return callOp;
}

//
// ReadNumberOfSwitchCases
//
// This higher level read method will read the number of switch cases from an operand
//  and then increment the buffer to the first case
//
static inline unsigned int
ReadNumberOfSwitchCases(BYTE **msilBytePtrPtr)
{
    unsigned __int32 val = ReadUInt32(*msilBytePtrPtr);
    (*msilBytePtrPtr) += MSIL_READ_4_BYTES;
    return val;
}

//
// ReadNumberOfSwitchCases
//
// This higher level read method will read a switch case from an operand
//  and then increment the buffer to the next case
//
static inline int
ReadSwitchCase(BYTE **msilBytePtrPtr)
{
    __int32 val = ReadInt32(*msilBytePtrPtr);
    (*msilBytePtrPtr) += MSIL_READ_4_BYTES;
    return val;
}

// LabelOffsetList - maintains a list of BYTE offsets at which branches need to 
// be inserted along with the associated labels. Used for building flow graph.
class LabelOffsetList {
    LabelOffsetList *next;
    IRNode          *label;
    unsigned int     offset;
    IRNode          *readerStack; // temp - this should be put onto the fg node
public:
    LabelOffsetList* getNext(void){ return next;}
    void             setNext(LabelOffsetList* n){ next = n;}
    IRNode*          getLabel(void){ return label;}
    void             setLabel(IRNode* l){ label = l;}
    unsigned int     getOffset(void){return offset;}
    void             setOffset(unsigned int o){offset = o;}

    IRNode*          getStack(void){return readerStack;}
    void             setStack(IRNode* rs){readerStack = rs;}
};

class ReaderBitVector {

    // Some class constants
    typedef unsigned __int8 BV_ELEM;
    static int const ELEM_SIZE = 8 * sizeof(BV_ELEM);

private:
    BV_ELEM      *bv;
    ReaderBitVector(){};

    DWORD             bvLength;
    DWORD             numBits;


public:
    void AllocateBitVector(DWORD size, ReaderBase *rdr)
    {
        DWORD length = (size + ELEM_SIZE - 1) / ELEM_SIZE; // Get array length
        bv = (BV_ELEM *)rdr->getTempMemory(sizeof(BV_ELEM)*length); // Allocate array
        memset(bv,0,(sizeof(BV_ELEM)*length)); //zero out array

        bvLength = length;
        numBits = size;
    }

    void SetBit(DWORD bitNum)
    {
        ASSERTDBG(bitNum < numBits);
        DWORD elem = bitNum / ELEM_SIZE;
        BV_ELEM mask = 1 << (bitNum % ELEM_SIZE);

        ASSERTDBG(elem < bvLength);
        bv[elem] |= mask; 
    }

    bool GetBit(DWORD bitNum)
    {
        ASSERTDBG(bitNum < numBits);
        DWORD elem = bitNum / ELEM_SIZE;
        BV_ELEM mask = 1 << (bitNum % ELEM_SIZE);

        ASSERTDBG(elem < bvLength);
        return ((bv[elem] & mask) != 0); 
    }

    void ClrBit(DWORD bitNum)
    {
        ASSERTDBG(bitNum < numBits);
        DWORD elem = bitNum / ELEM_SIZE;
        BV_ELEM mask = 1 << (bitNum % ELEM_SIZE);

        ASSERTDBG(elem < bvLength);
        bv[elem] &= ~mask;
    }
};


// Reader Arc node, used when building flow graph.
struct rdrFg;
struct rdrArc{
    unsigned branchOffset, targetOffset;
    bool nominal;
    rdrArc* next;
    rdrFg* block;

    bool isLabel(void){return (targetOffset == (unsigned)-2);}
    bool isExitLabel(void){return (isLabel() && branchOffset == (unsigned)-1);}
};



ReaderBase::ReaderBase(
    ICorJitInfo* jitInfo,
    CORINFO_METHOD_INFO* methodInfo,
    unsigned flags // jit compile flags
){
    // Zero-Initialize all class data.
    memset(&m_methodInfo, 0, ((char*)&m_dummyLastBaseField - (char*)&m_methodInfo));

    m_jitInfo = jitInfo;
    m_methodInfo = methodInfo;
    m_flags = flags;
}

// Common FlowGraphEdgeList getters/setters
FlowGraphEdgeList*
FgEdgeListGetNextSuccessorActual(
    FlowGraphEdgeList* fgEdge
){
    if (fgEdge) {
        fgEdge = FgEdgeListGetNextSuccessor(fgEdge);
    }
    while (fgEdge && FgEdgeListIsNominal(fgEdge)) {
        fgEdge = FgEdgeListGetNextSuccessor(fgEdge);    
    }
    return fgEdge;
}

FlowGraphEdgeList*
FgEdgeListGetNextPredecessorActual(
    FlowGraphEdgeList* fgEdge
){
    if (fgEdge) {
        fgEdge = FgEdgeListGetNextPredecessor(fgEdge);
    }
    while (fgEdge && FgEdgeListIsNominal(fgEdge)) {
        fgEdge = FgEdgeListGetNextPredecessor(fgEdge);
    }
    return fgEdge;
}

FlowGraphEdgeList*
FgNodeGetSuccessorListActual(
    FlowGraphNode* fg
){
    FlowGraphEdgeList* fgEdge;

    fgEdge = FgNodeGetSuccessorList (fg);

    if ((fgEdge != NULL) && FgEdgeListIsNominal(fgEdge))
        fgEdge = FgEdgeListGetNextSuccessorActual(fgEdge);
    return fgEdge;
}

FlowGraphEdgeList*
FgNodeGetPredecessorListActual(
    FlowGraphNode* fg
){
    FlowGraphEdgeList* fgEdge;

    fgEdge = FgNodeGetPredecessorList (fg);
    if (fgEdge != NULL && FgEdgeListIsNominal(fgEdge))
        fgEdge = FgEdgeListGetNextPredecessorActual(fgEdge);
    return fgEdge;
}


//
//GetMSILInstrLength - Returns the length of an instruction given an op code and a pointer
//  to the operand. It assumes the only variable length opcode is CEE_SWITCH.
//
unsigned int GetMSILInstrLength(ReaderBaseNS::OPCODE opcode, BYTE *operand)
{
    
    // Table that maps opcode enum to operand size in bytes.
    // -1 indicates either an undefined opcode, or an operand
    // with variable length, in both cases the table should
    // not be used.
    static const char OperandSizeMap[] = {
#define OPDEF_HELPER OPDEF_OPERANDSIZE
#include "ophelper.h"
#undef OPDEF_HELPER
    };

    unsigned int length;

    if (opcode == ReaderBaseNS::CEE_SWITCH) {
        ASSERTNR(NULL != operand);
        // Length of a switch is the 4 bytes + 4 bytes * the value of the first 4 bytes
        UINT numCases = ReadNumberOfSwitchCases(&operand);
        length = MSIL_READ_4_BYTES + (numCases * MSIL_READ_4_BYTES);
    } else {
        length = OperandSizeMap[opcode-ReaderBaseNS::CEE_NOP];
    }
    return length;
}


// Currently implemented with bytecode verification. This would be
// unnecessary if an earlier pass performed this (surely the fg build?)

ReaderBaseNS::OPCODE ParseMSILOpcode(
    unsigned char* curMsilPtr,
    unsigned char** operandPtrPtr,
    unsigned int* increment,
    ReaderBase *reader //for error reporting
){
    int bytesRead = 0;
    unsigned char index;
    unsigned char *operand;
    ReaderBaseNS::OPCODE opcode = ReaderBaseNS::CEE_ILLEGAL;

    // Illegal opcodes are currently marked as CEE_ILLEGAL. These should
    // cause verification errors.

    // Array which maps bytecode to WVM opcode.
    static const ReaderBaseNS::OPCODE ByteCodes[256] = {
        ReaderBaseNS::CEE_NOP,
        ReaderBaseNS::CEE_BREAK,
        ReaderBaseNS::CEE_LDARG_0,
        ReaderBaseNS::CEE_LDARG_1,
        ReaderBaseNS::CEE_LDARG_2,
        ReaderBaseNS::CEE_LDARG_3,
        ReaderBaseNS::CEE_LDLOC_0,
        ReaderBaseNS::CEE_LDLOC_1,
        ReaderBaseNS::CEE_LDLOC_2,
        ReaderBaseNS::CEE_LDLOC_3,
        ReaderBaseNS::CEE_STLOC_0,
        ReaderBaseNS::CEE_STLOC_1,    
        ReaderBaseNS::CEE_STLOC_2,
        ReaderBaseNS::CEE_STLOC_3,
        ReaderBaseNS::CEE_LDARG_S,
        ReaderBaseNS::CEE_LDARGA_S,
        ReaderBaseNS::CEE_STARG_S,
        ReaderBaseNS::CEE_LDLOC_S,
        ReaderBaseNS::CEE_LDLOCA_S,
        ReaderBaseNS::CEE_STLOC_S,
        ReaderBaseNS::CEE_LDNULL,
        ReaderBaseNS::CEE_LDC_I4_M1,
        ReaderBaseNS::CEE_LDC_I4_0,
        ReaderBaseNS::CEE_LDC_I4_1,
        ReaderBaseNS::CEE_LDC_I4_2,
        ReaderBaseNS::CEE_LDC_I4_3,
        ReaderBaseNS::CEE_LDC_I4_4,
        ReaderBaseNS::CEE_LDC_I4_5,
        ReaderBaseNS::CEE_LDC_I4_6,
        ReaderBaseNS::CEE_LDC_I4_7,
        ReaderBaseNS::CEE_LDC_I4_8,
        ReaderBaseNS::CEE_LDC_I4_S,
        ReaderBaseNS::CEE_LDC_I4,
        ReaderBaseNS::CEE_LDC_I8,
        ReaderBaseNS::CEE_LDC_R4,
        ReaderBaseNS::CEE_LDC_R8,
        ReaderBaseNS::CEE_ILLEGAL,     //ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_DUP,
        ReaderBaseNS::CEE_POP,
        ReaderBaseNS::CEE_JMP,
        ReaderBaseNS::CEE_CALL,
        ReaderBaseNS::CEE_CALLI,
        ReaderBaseNS::CEE_RET,
        ReaderBaseNS::CEE_BR_S,
        ReaderBaseNS::CEE_BRFALSE_S,
        ReaderBaseNS::CEE_BRTRUE_S,
        ReaderBaseNS::CEE_BEQ_S,
        ReaderBaseNS::CEE_BGE_S,
        ReaderBaseNS::CEE_BGT_S,
        ReaderBaseNS::CEE_BLE_S,
        ReaderBaseNS::CEE_BLT_S,
        ReaderBaseNS::CEE_BNE_UN_S,
        ReaderBaseNS::CEE_BGE_UN_S,
        ReaderBaseNS::CEE_BGT_UN_S,
        ReaderBaseNS::CEE_BLE_UN_S,
        ReaderBaseNS::CEE_BLT_UN_S,
        ReaderBaseNS::CEE_BR,
        ReaderBaseNS::CEE_BRFALSE,
        ReaderBaseNS::CEE_BRTRUE,
        ReaderBaseNS::CEE_BEQ,
        ReaderBaseNS::CEE_BGE,
        ReaderBaseNS::CEE_BGT,
        ReaderBaseNS::CEE_BLE,
        ReaderBaseNS::CEE_BLT,
        ReaderBaseNS::CEE_BNE_UN,
        ReaderBaseNS::CEE_BGE_UN,
        ReaderBaseNS::CEE_BGT_UN,
        ReaderBaseNS::CEE_BLE_UN,
        ReaderBaseNS::CEE_BLT_UN,
        ReaderBaseNS::CEE_SWITCH,
        ReaderBaseNS::CEE_LDIND_I1,
        ReaderBaseNS::CEE_LDIND_U1,
        ReaderBaseNS::CEE_LDIND_I2,
        ReaderBaseNS::CEE_LDIND_U2,
        ReaderBaseNS::CEE_LDIND_I4,
        ReaderBaseNS::CEE_LDIND_U4,
        ReaderBaseNS::CEE_LDIND_I8,
        ReaderBaseNS::CEE_LDIND_I,
        ReaderBaseNS::CEE_LDIND_R4,
        ReaderBaseNS::CEE_LDIND_R8,
        ReaderBaseNS::CEE_LDIND_REF,
        ReaderBaseNS::CEE_STIND_REF,
        ReaderBaseNS::CEE_STIND_I1,
        ReaderBaseNS::CEE_STIND_I2,
        ReaderBaseNS::CEE_STIND_I4,
        ReaderBaseNS::CEE_STIND_I8,
        ReaderBaseNS::CEE_STIND_R4,
        ReaderBaseNS::CEE_STIND_R8,
        ReaderBaseNS::CEE_ADD,
        ReaderBaseNS::CEE_SUB,
        ReaderBaseNS::CEE_MUL,
        ReaderBaseNS::CEE_DIV,
        ReaderBaseNS::CEE_DIV_UN,
        ReaderBaseNS::CEE_REM,
        ReaderBaseNS::CEE_REM_UN,
        ReaderBaseNS::CEE_AND,
        ReaderBaseNS::CEE_OR,
        ReaderBaseNS::CEE_XOR,
        ReaderBaseNS::CEE_SHL,
        ReaderBaseNS::CEE_SHR,
        ReaderBaseNS::CEE_SHR_UN,
        ReaderBaseNS::CEE_NEG,
        ReaderBaseNS::CEE_NOT,
        ReaderBaseNS::CEE_CONV_I1,
        ReaderBaseNS::CEE_CONV_I2,
        ReaderBaseNS::CEE_CONV_I4,
        ReaderBaseNS::CEE_CONV_I8,
        ReaderBaseNS::CEE_CONV_R4,
        ReaderBaseNS::CEE_CONV_R8,
        ReaderBaseNS::CEE_CONV_U4,
        ReaderBaseNS::CEE_CONV_U8,
        ReaderBaseNS::CEE_CALLVIRT,
        ReaderBaseNS::CEE_CPOBJ,
        ReaderBaseNS::CEE_LDOBJ,
        ReaderBaseNS::CEE_LDSTR,
        ReaderBaseNS::CEE_NEWOBJ,
        ReaderBaseNS::CEE_CASTCLASS,
        ReaderBaseNS::CEE_ISINST,
        ReaderBaseNS::CEE_CONV_R_UN,
        ReaderBaseNS::CEE_ILLEGAL,  // ReaderBaseNS::CEE_ANN_DATA_S,
        ReaderBaseNS::CEE_ILLEGAL,  // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_UNBOX,
        ReaderBaseNS::CEE_THROW,
        ReaderBaseNS::CEE_LDFLD,
        ReaderBaseNS::CEE_LDFLDA,
        ReaderBaseNS::CEE_STFLD,
        ReaderBaseNS::CEE_LDSFLD,
        ReaderBaseNS::CEE_LDSFLDA,
        ReaderBaseNS::CEE_STSFLD,
        ReaderBaseNS::CEE_STOBJ,
        ReaderBaseNS::CEE_CONV_OVF_I1_UN,
        ReaderBaseNS::CEE_CONV_OVF_I2_UN,
        ReaderBaseNS::CEE_CONV_OVF_I4_UN,
        ReaderBaseNS::CEE_CONV_OVF_I8_UN,
        ReaderBaseNS::CEE_CONV_OVF_U1_UN,
        ReaderBaseNS::CEE_CONV_OVF_U2_UN,
        ReaderBaseNS::CEE_CONV_OVF_U4_UN,
        ReaderBaseNS::CEE_CONV_OVF_U8_UN,
        ReaderBaseNS::CEE_CONV_OVF_I_UN,
        ReaderBaseNS::CEE_CONV_OVF_U_UN,
        ReaderBaseNS::CEE_BOX,
        ReaderBaseNS::CEE_NEWARR,
        ReaderBaseNS::CEE_LDLEN,
        ReaderBaseNS::CEE_LDELEMA,
        ReaderBaseNS::CEE_LDELEM_I1,
        ReaderBaseNS::CEE_LDELEM_U1,
        ReaderBaseNS::CEE_LDELEM_I2,
        ReaderBaseNS::CEE_LDELEM_U2,
        ReaderBaseNS::CEE_LDELEM_I4,
        ReaderBaseNS::CEE_LDELEM_U4,
        ReaderBaseNS::CEE_LDELEM_I8,
        ReaderBaseNS::CEE_LDELEM_I,
        ReaderBaseNS::CEE_LDELEM_R4,
        ReaderBaseNS::CEE_LDELEM_R8,
        ReaderBaseNS::CEE_LDELEM_REF,
        ReaderBaseNS::CEE_STELEM_I,
        ReaderBaseNS::CEE_STELEM_I1,
        ReaderBaseNS::CEE_STELEM_I2,
        ReaderBaseNS::CEE_STELEM_I4,
        ReaderBaseNS::CEE_STELEM_I8,
        ReaderBaseNS::CEE_STELEM_R4,
        ReaderBaseNS::CEE_STELEM_R8,
        ReaderBaseNS::CEE_STELEM_REF,
        ReaderBaseNS::CEE_LDELEM,     // (M2 generics)
        ReaderBaseNS::CEE_STELEM,     // (M2 generics)
        ReaderBaseNS::CEE_UNBOX_ANY,  // (M2 generics)
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_CONV_OVF_I1,
        ReaderBaseNS::CEE_CONV_OVF_U1,
        ReaderBaseNS::CEE_CONV_OVF_I2,
        ReaderBaseNS::CEE_CONV_OVF_U2,
        ReaderBaseNS::CEE_CONV_OVF_I4,
        ReaderBaseNS::CEE_CONV_OVF_U4,
        ReaderBaseNS::CEE_CONV_OVF_I8,
        ReaderBaseNS::CEE_CONV_OVF_U8,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_REFANYVAL,
        ReaderBaseNS::CEE_CKFINITE,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_MKREFANY,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_ANN_CALL,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_ANN_CATCH,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_ANN_DEAD,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_ANN_HOISTED,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_ANN_HOISTED_CALL,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_ANN_LAB,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_ANN_DEF,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_ANN_REF_S,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_ANN_PHI,
        ReaderBaseNS::CEE_LDTOKEN,
        ReaderBaseNS::CEE_CONV_U2,
        ReaderBaseNS::CEE_CONV_U1,
        ReaderBaseNS::CEE_CONV_I,
        ReaderBaseNS::CEE_CONV_OVF_I,
        ReaderBaseNS::CEE_CONV_OVF_U,
        ReaderBaseNS::CEE_ADD_OVF,
        ReaderBaseNS::CEE_ADD_OVF_UN,
        ReaderBaseNS::CEE_MUL_OVF,
        ReaderBaseNS::CEE_MUL_OVF_UN,
        ReaderBaseNS::CEE_SUB_OVF,
        ReaderBaseNS::CEE_SUB_OVF_UN,
        ReaderBaseNS::CEE_ENDFINALLY,
        ReaderBaseNS::CEE_LEAVE,
        ReaderBaseNS::CEE_LEAVE_S,
        ReaderBaseNS::CEE_STIND_I,
        ReaderBaseNS::CEE_CONV_U,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL,
        ReaderBaseNS::CEE_ILLEGAL
    };
 
    // Array which maps prefixed bytecode to WVM opcode.
    static const ReaderBaseNS::OPCODE PrefixedByteCodes[33] = {
        ReaderBaseNS::CEE_ARGLIST,
        ReaderBaseNS::CEE_CEQ,
        ReaderBaseNS::CEE_CGT,
        ReaderBaseNS::CEE_CGT_UN,
        ReaderBaseNS::CEE_CLT,
        ReaderBaseNS::CEE_CLT_UN,
        ReaderBaseNS::CEE_LDFTN,
        ReaderBaseNS::CEE_LDVIRTFTN,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_JMPI,
        ReaderBaseNS::CEE_LDARG,
        ReaderBaseNS::CEE_LDARGA,
        ReaderBaseNS::CEE_STARG,
        ReaderBaseNS::CEE_LDLOC,
        ReaderBaseNS::CEE_LDLOCA,
        ReaderBaseNS::CEE_STLOC,
        ReaderBaseNS::CEE_LOCALLOC,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_ENDFILTER,
        ReaderBaseNS::CEE_UNALIGNED,
        ReaderBaseNS::CEE_VOLATILE,
        ReaderBaseNS::CEE_TAILCALL,
        ReaderBaseNS::CEE_INITOBJ,
        ReaderBaseNS::CEE_CONSTRAINED,
        ReaderBaseNS::CEE_CPBLK,
        ReaderBaseNS::CEE_INITBLK,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_ANN_REF,
        ReaderBaseNS::CEE_RETHROW,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_SIZEOF,
        ReaderBaseNS::CEE_REFANYTYPE,
        ReaderBaseNS::CEE_READONLY,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_FIRSTOP,
        ReaderBaseNS::CEE_ILLEGAL, // ReaderBaseNS::CEE_ANN_DATA
    };


    index = *(curMsilPtr + bytesRead);
    bytesRead++;

    // If this is a prefix bytecode, then use a secondary array to map
    // from bytecode to opcode.
    if (index == 0xFE) {
        index = *(curMsilPtr + bytesRead);
        bytesRead++;
        if (index<=32) {
            opcode = PrefixedByteCodes[index];
        } else {
            // VERIFY: Bytecode error
            if (!reader)
                ReaderBase::GenIR_Fatal(CORJIT_BADCODE);
            else
                reader->verGlobalError(MVER_E_UNKNOWN_OPCODE); 

        }
    } else {
        // Use the primary array to map from bytecode to opcode.
        opcode = ByteCodes[index];
    }

    // VERIFY: Ensure that opcode isn't CEE_ILLEGAL.
    if (opcode == ReaderBaseNS::CEE_ILLEGAL) {
        if (!reader)
            ReaderBase::GenIR_Fatal(CORJIT_BADCODE);
        else
            reader->verGlobalError(MVER_E_UNKNOWN_OPCODE); 
    }
    
    operand = curMsilPtr + bytesRead;

    bytesRead += GetMSILInstrLength(opcode, operand);

    if (operandPtrPtr != NULL) {
        *operandPtrPtr = operand;  // set operand pointer
    }
    *increment += bytesRead; // set the increment

    return opcode;
}


ReaderBaseNS::OPCODE ParseMSILOpcodeSafely(
    unsigned char* buf,
    unsigned int currOffset,
    unsigned int bufSize,
    unsigned char** operand,
    unsigned int* nextOffset,
    ReaderBase *reader, // for reporting errors
    bool reportError
){
    // We need to make sure that we're not going to parse outside the buffer.
    // Note that only the opcode itself is parsed, so we can check whether
    // any operands would exceed the buffer by checking nextOffset after
    // we parse.
    // This leaves two cases:
    // 1) prefix opcode = 0xFE then actual op (no symbolic name?)
    // 2) switch opcode = CEE_SWITCH then 4-BYTE length field

    unsigned char rawOp = *(buf+currOffset);

    if (((currOffset == (bufSize-1)) && (rawOp == 0xFE)) ||
        ((currOffset >= (bufSize-4)) && (rawOp == ReaderBaseNS::CEE_SWITCH)))
    {
        // The opcode itself crosses the bounds of the IL buffer.
        // Don't parse it.
        if (reportError) {
            if (!reader)
                ReaderBase::GenIR_Fatal(CORJIT_BADCODE);
            else
                reader->verGlobalError(MVER_E_METHOD_END); //"MSIL opcode crosses IL buffer boundary"
        }
        return ReaderBaseNS::CEE_NOP;
    }

    ReaderBaseNS::OPCODE opcode = ParseMSILOpcode(buf + currOffset,operand,nextOffset, reader);

    // If the operand field is pointing off into space, die before we use it
    if ((*nextOffset) > bufSize) {
        if (reportError) {
            if (!reader)
                ReaderBase::GenIR_Fatal(CORJIT_BADCODE);
            else
                reader->verGlobalError(MVER_E_METHOD_END); //"MSIL opcode crosses IL buffer boundary"
        }
        return ReaderBaseNS::CEE_NOP;
    }

    return opcode;
}


const char* opcodeName[] = {
#define OPDEF_HELPER OPDEF_OPCODENAME
#include "ophelper.h"
#undef OPDEF_HELPER
};


#ifndef NODEBUG
void
ReaderBase::printMSIL(
    BYTE* pbuf,
    unsigned startOffset,
    unsigned endOffset
){
    unsigned char *operand;
    ReaderBaseNS::OPCODE opcode;
    unsigned offset = 0;
    unsigned __int64 operandSize;
    unsigned numBytes;


    if(startOffset >= endOffset)
        return;

    numBytes = endOffset - startOffset;

    while (offset < numBytes) {
        dbprint("0x%-4x: ", startOffset + offset);
        opcode = ParseMSILOpcode(pbuf + startOffset + offset,&operand,&offset, this);
        dbprint("%-10s ",opcodeName[opcode]);

        switch (opcode) {
        default:
            operandSize = (pbuf + startOffset + offset) - operand;
            switch (operandSize) {
            case 0:
                break;
            case 1:
                dbprint("0x%x",ReadInt8(operand));
                break;
            case 2:
                dbprint("0x%x",ReadInt16(operand));
                break;
            case 4:
                if (opcode == ReaderBaseNS::CEE_LDC_R4) {
                    dbprint("%f",ReadF32(operand));
                } else {
                    dbprint("0x%x",ReadInt32(operand));
                }
                break;
            case 8:
                if (opcode == ReaderBaseNS::CEE_LDC_R8) {
                    dbprint("%f",ReadF64(operand));
                } else {
                    dbprint("0x%I64x",ReadInt64(operand));
                }
                break;
            }
            break;

        case ReaderBaseNS::CEE_SWITCH:
            {
                unsigned int numCases = ReadNumberOfSwitchCases(&operand);
                dbprint("%-4d cases\n",numCases);
                for (unsigned i=0;i<numCases;i++) {
                    dbprint("        case %d: 0x%x\n",i,ReadSwitchCase(&operand));
                }
            }
            break;
        }
        dbprint("\n");
    }
}
#endif


// ///////////////////////////////////////////////////////////////////////
// EE Data Accessor Methods.
//
// GenIR does not have access to JitInfo or MethodInfo. It must
// ask the reader to fetch metadata.
// ///////////////////////////////////////////////////////////////////////

bool ReaderBase::isPrimitiveType(
    CORINFO_CLASS_HANDLE handle
){
    return isPrimitiveType(m_jitInfo->asCorInfoType((CORINFO_CLASS_HANDLE) handle));
}

bool ReaderBase::isPrimitiveType(
    CorInfoType corInfoType
){
    return (CORINFO_TYPE_BOOL <= corInfoType && corInfoType <= CORINFO_TYPE_DOUBLE);
}


void*
ReaderBase::getHelperDescr(
    CorInfoHelpFunc helpFuncId,
    bool *fIndirect,
    InfoAccessModule* pAccessModule
){
    void *helperHandle, *pHelperHandle;

    ASSERTNR(fIndirect != NULL);
    helperHandle = m_jitInfo->getHelperFtn(helpFuncId, &pHelperHandle, pAccessModule);
    if (helperHandle != NULL) {
        *fIndirect = false;
        return helperHandle;
    }

    ASSERTNR(pHelperHandle != NULL);
    *fIndirect = true;
    return pHelperHandle;
}

CorInfoHelpFunc
ReaderBase::getNewHelper(CORINFO_CLASS_HANDLE classHandle, unsigned classToken, CORINFO_MODULE_HANDLE tokenContext)
{
    return getNewHelper(classHandle,getCurrentMethodHandle(),classToken,tokenContext);
}

CorInfoHelpFunc
ReaderBase::getNewHelper(CORINFO_CLASS_HANDLE classHandle, CORINFO_METHOD_HANDLE context, unsigned classToken, CORINFO_MODULE_HANDLE tokenContext)
{
    return m_jitInfo->getNewHelper(classHandle,context,classToken, tokenContext);
}


void *
ReaderBase::getVarArgsHandle(CORINFO_SIG_INFO *pSig, bool *fIndirect)
{
    CORINFO_VARARGS_HANDLE *pVarCookie;
    CORINFO_VARARGS_HANDLE varCookie;

    varCookie = m_jitInfo->getVarArgsHandle(pSig, (void**)&pVarCookie);
    ASSERTNR((!varCookie) != (!pVarCookie));

    if (varCookie != NULL)
    {
        *fIndirect = false;
        return varCookie;
    }
    else
    {
        *fIndirect = true;
        return pVarCookie;
    }
}


// ///////////////////////////////////////////////////////////////////////
// Properties of current method.
// ///////////////////////////////////////////////////////////////////////

bool
ReaderBase::isZeroInitLocals(
    void
){
    return ((m_methodInfo->options & CORINFO_OPT_INIT_LOCALS) != 0);
}

unsigned int
ReaderBase::getCurrentMethodNumAutos(
    void
){
    return m_methodInfo->locals.numArgs;
}

CORINFO_CLASS_HANDLE
ReaderBase::getCurrentMethodClass(
    void
){
    return m_jitInfo->getMethodClass(getCurrentMethodHandle());
}

CORINFO_METHOD_HANDLE
ReaderBase::getCurrentMethodHandle(
    void
){
    return m_methodInfo->ftn;
}

// Returns the EE's hash code for the method being compiled.
unsigned
ReaderBase::getCurrentMethodHash(
    void
){
    return m_jitInfo->getMethodHash(getCurrentMethodHandle());
}

DWORD
ReaderBase::getCurrentMethodAttribs(
    void
){
    return m_jitInfo->getMethodAttribs(getCurrentMethodHandle(), getCurrentMethodHandle());
}

char*
ReaderBase::getCurrentMethodName(const char** moduleName)
{
    return (char *)m_jitInfo->getMethodName(getCurrentMethodHandle(),moduleName);
}

mdToken
ReaderBase::getMethodDefFromMethod(
    CORINFO_METHOD_HANDLE handle
){
    return m_jitInfo->getMethodDefFromMethod(handle);
}

void
ReaderBase::getFunctionEntryPoint(
    CORINFO_METHOD_HANDLE   handle,           
    InfoAccessType         requestedAccessType,
    CORINFO_CONST_LOOKUP *pResult,
    CORINFO_ACCESS_FLAGS    accessFlags
){
    m_jitInfo->getFunctionEntryPoint(handle, requestedAccessType, pResult, accessFlags);
}

void
ReaderBase::getFunctionFixedEntryPointInfo(
    mdToken token, 
    CORINFO_LOOKUP *pResult
){
    m_jitInfo->getFunctionFixedEntryPointInfo(getCurrentModuleHandle(), token, getCurrentMethodHandle(), pResult);
}



CORINFO_MODULE_HANDLE
ReaderBase::getCurrentModuleHandle(
    void
){
    return m_methodInfo->scope;
}


// ///////////////////////////////////////////////////////////////////////
// Properties of current jitinfo.
// These functions assume the context of the current module and method info.
// ///////////////////////////////////////////////////////////////////////

//
// class
//

CORINFO_CLASS_HANDLE
ReaderBase::getMethodClass(
    CORINFO_METHOD_HANDLE handle
){
    return m_jitInfo->getMethodClass(handle);
}

unsigned long
ReaderBase::getMethodVTableOffset(
    CORINFO_METHOD_HANDLE handle
){
    return m_jitInfo->getMethodVTableOffset(handle);
}

BOOL
ReaderBase::checkMethodModifier(
    CORINFO_METHOD_HANDLE hMethod, 
    LPCSTR                modifier, // name of the modifier to check for
    BOOL                  fOptional // true for modopt, false for modreqd
) {
    return m_jitInfo->checkMethodModifier(hMethod, modifier, fOptional);
}


const char*
ReaderBase::getClassName(
    CORINFO_CLASS_HANDLE hClass
){
    return m_jitInfo->getClassName(hClass);
}

INT 
ReaderBase::getClassCustomAttribute(
    CORINFO_CLASS_HANDLE clsHnd, 
    LPCSTR attrib, 
    const BYTE** ppVal
) {
    return m_jitInfo->getClassCustomAttribute(clsHnd, attrib, ppVal);
}

PGCLAYOUT_STRUCT
ReaderBase::getClassGCLayout(
    CORINFO_CLASS_HANDLE hClass
){
    // The actual size of the BYTE array the runtime is expecting (gcLayoutSize) is one BYTE for 
    // every sizeof(void*) slot in the valueclass.  Note that we round this computation up.
    int classSize    = m_jitInfo->getClassSize(hClass);
    int gcLayoutSize = ((classSize + READER_HOST_PTR_SIZE - 1) / READER_HOST_PTR_SIZE);

    // Our internal data strcutures prepend the number of GC pointers before the struct.  Therefore
    // we add the size of the GCLAYOUT_STRUCT to our computed size above.
    PGCLAYOUT_STRUCT pGCLayoutStruct = (PGCLAYOUT_STRUCT)getProcMemory(gcLayoutSize + sizeof(GCLAYOUT_STRUCT));
    unsigned int     numGCvars       = m_jitInfo->getClassGClayout(hClass, GCLS_GCLAYOUT(pGCLayoutStruct));

    if (numGCvars > 0)
    {
        // We cache away the number of GC vars.
        GCLS_NUMGCPTRS(pGCLayoutStruct)  = numGCvars;
    }
    else
    {
        // If we had no GC pointers, then we won't bother returning the GCLayout.  It is our convention
        // that if you have a GCLayout, then you have pointers.  This allows us to do only one check
        // when we want to know if a MB has GC pointers.
        pGCLayoutStruct = NULL;
    }

    return pGCLayoutStruct;
}

DWORD
ReaderBase::getClassAttribs(
    CORINFO_CLASS_HANDLE hClass
){
    return getClassAttribs(hClass, getCurrentMethodHandle());
}

DWORD
ReaderBase::getClassAttribs(
    CORINFO_CLASS_HANDLE hClass,
    CORINFO_METHOD_HANDLE context
){
    return m_jitInfo->getClassAttribs(hClass, context);
}

unsigned int
ReaderBase::getClassSize(
    CORINFO_CLASS_HANDLE hClass
){
    return m_jitInfo->getClassSize(hClass);
}

unsigned int
ReaderBase::getClassAlignmentRequirement(
    CORINFO_CLASS_HANDLE hClass
){
    // Class must be value (a non-primitive value class, a multibyte)
    ASSERTNR(hClass && (getClassAttribs(hClass) & CORINFO_FLG_VALUECLASS));

#if !defined(NODEBUG)
    // Make sure that classes which contain GC refs also have sufficient 
    // alignment requirements.
    if (getClassGCLayout(hClass)) {
        ASSERTNR(m_jitInfo->getClassAlignmentRequirement(hClass) >= sizeof(char*));
    }
#endif // !NODEBUG

    return m_jitInfo->getClassAlignmentRequirement(hClass);
}

ReaderAlignType
ReaderBase::getMinimumClassAlignment(
    CORINFO_CLASS_HANDLE hClass,
    ReaderAlignType      alignment
){
    ReaderAlignType alignRequired;

    alignRequired = (ReaderAlignType)getClassAlignmentRequirement(hClass);
    if (alignRequired != 0 &&
        (alignment == READER_ALIGN_NATURAL || alignment > alignRequired) )
    {
        alignment = alignRequired;
    }

    // Unaligned GC pointers are not supported by the CLR.
    // Simply ignore users that specify otherwise
    if (getClassGCLayout(hClass) != NULL) {
        alignment = READER_ALIGN_NATURAL;
    }

    return alignment;
}

CorInfoType
ReaderBase::getClassType(
    CORINFO_CLASS_HANDLE hClass
){
    return m_jitInfo->asCorInfoType(hClass);
}

unsigned __int8 *
ReaderBase::getValueClassSig(
    CORINFO_CLASS_HANDLE hClass
){
    unsigned __int8 *pSig;
    int siglen;
    mdTypeDef token;
    HRESULT hr;

    pSig = (unsigned __int8 *) getProcMemory(7);

    pSig[1] = ELEMENT_TYPE_VALUETYPE;

    hr = (HRESULT)0;
    token = mdTokenNil;

    siglen = CorSigCompressToken(token, &pSig[2]);
    if (siglen < 0) {
        *(RID *)(&pSig[2]) = RidFromToken(token);
        pSig[0] = sizeof(UCHAR) + sizeof(RID);
    } else {
        pSig[0] = sizeof(UCHAR) + (UCHAR) siglen;
    }

    return pSig;
}

// pSize and pRetSig are 0/NULL if type is non-value or primitive.
void
ReaderBase::getClassType(
    CORINFO_CLASS_HANDLE    classHandle,
    DWORD             classAttribs,
    CorInfoType*      pCorInfoType,
    unsigned int*     pSize,
    unsigned __int8** pRetSig
){
    
    if ((classAttribs & CORINFO_FLG_VALUECLASS) == 0) {
        // If non-value class then create pointer temp
        *pCorInfoType = CORINFO_TYPE_PTR;
        *pSize = 0;
    } else if (isPrimitiveType(classHandle)) {
        // If primitive value type then create temp of that type
        *pCorInfoType = m_jitInfo->asCorInfoType(classHandle);
        *pSize = 0;
    } else {
        // else class is non-primitive value class, a multibyte
        *pCorInfoType = CORINFO_TYPE_VALUECLASS;
        *pSize = getClassSize(classHandle);
        if (*pRetSig == NULL) {
            *pRetSig = getValueClassSig(classHandle);
        }
    }
}

BOOL
ReaderBase::canInlineTypeCheckWithObjectVTable(
    CORINFO_CLASS_HANDLE hClass
){
    return m_jitInfo->canInlineTypeCheckWithObjectVTable(hClass);
}

bool
ReaderBase::accessStaticFieldRequiresClassConstructor(
    CORINFO_FIELD_HANDLE fieldHandle
){
    // The class the method belongs to
    CORINFO_CLASS_HANDLE classHandleOfMethod = getCurrentMethodClass();

    // The class the field belongs to
    CORINFO_CLASS_HANDLE classHandleOfField  = getFieldClass(fieldHandle);

    // Accessing static fields belonging to the same class as this method 
    // never require the class constructor to be run.
    if (classHandleOfField != classHandleOfMethod)
    {
        DWORD fieldAttribs = getFieldAttribs(fieldHandle, CORINFO_ACCESS_ANY);
        DWORD classAttribs = getClassAttribs(classHandleOfField);
    
        bool fIsStatic = ((fieldAttribs & CORINFO_FLG_STATIC)!=0);
        ASSERT(fIsStatic);

        // For static fields: 
        // If the class
        //     (1) does not use a shared helper
        //     (2) and is not yet initialized
        //     (3) and the needs init flag is set
        //     (4) and calling jitInfo->initClass fails (eagerly initializing the class at Jit time)
        //
        // Then we return true and our caller will insert a call to the class-init helper.
        
        if ((!(fieldAttribs & CORINFO_FLG_SHARED_HELPER))  &&
            (!(classAttribs & CORINFO_FLG_INITIALIZED))    &&
            ((classAttribs & CORINFO_FLG_NEEDS_INIT))      &&
            !m_jitInfo->initClass(classHandleOfField, getCurrentMethodHandle()))
        {
            return true;
        }
    }
    return false;
}

//
// field
//

CORINFO_FIELD_HANDLE
ReaderBase::getFieldHandle(
    mdToken token
){
    return getFieldHandle(token,getCurrentMethodHandle(),getCurrentModuleHandle());
}

CORINFO_FIELD_HANDLE
ReaderBase::getFieldHandle(
    mdToken token,
    CORINFO_METHOD_HANDLE context,
    CORINFO_MODULE_HANDLE scope
){
    return m_jitInfo->findField(scope,token,context);
}

const char *
ReaderBase::getFieldName(
    CORINFO_FIELD_HANDLE hField,
    const char **moduleName

){
    return m_jitInfo->getFieldName(hField, moduleName);
}

//Returns a handle to a field that can be embedded in the JITed code
CORINFO_FIELD_HANDLE
ReaderBase::embedFieldHandle(
    CORINFO_FIELD_HANDLE hField,
    bool*   fIndirect
){
    CORINFO_FIELD_HANDLE dirFieldHandle, indFieldHandle;
    dirFieldHandle = m_jitInfo->embedFieldHandle(hField, (void**)&indFieldHandle);

    if (dirFieldHandle != NULL) {
        ASSERTNR(indFieldHandle == NULL);
        *fIndirect = false;
        return dirFieldHandle;
    } else {
        ASSERTNR(indFieldHandle != NULL);
        *fIndirect = true;
        return indFieldHandle;
    }
}

DWORD
ReaderBase::getFieldAttribs(
    CORINFO_FIELD_HANDLE hField,
    CORINFO_ACCESS_FLAGS flags
){
    return m_jitInfo->getFieldAttribs(hField, getCurrentMethodHandle(), flags);
}

CORINFO_CLASS_HANDLE
ReaderBase::getFieldClass(
    CORINFO_FIELD_HANDLE hField
){
    return m_jitInfo->getFieldClass(hField);
}

CorInfoType
ReaderBase::getFieldType(
    CORINFO_FIELD_HANDLE hField,
    CORINFO_CLASS_HANDLE* phClass,
    CORINFO_CLASS_HANDLE owner /* optional: for verification */
){
    return m_jitInfo->getFieldType(hField, phClass, owner);
}

unsigned int
ReaderBase::getClassNumInstanceFields(
    CORINFO_CLASS_HANDLE hClass
){
    return m_jitInfo->getClassNumInstanceFields(hClass);
}

CORINFO_FIELD_HANDLE
ReaderBase::getFieldInClass(
    CORINFO_CLASS_HANDLE  hClass,
    unsigned int          ordinal
){
    return m_jitInfo->getFieldInClass(hClass, ordinal);
}

CorInfoType
ReaderBase::getFieldInfo(
    CORINFO_CLASS_HANDLE  hClass,
    unsigned int          ordinal,
    unsigned int*         fieldOffset,
    CORINFO_CLASS_HANDLE* hFieldClass
){
    CORINFO_FIELD_HANDLE hField;

    hField = m_jitInfo->getFieldInClass(hClass, ordinal);
    if (fieldOffset) {
        *fieldOffset = m_jitInfo->getFieldOffset(hField);
    }
    return m_jitInfo->getFieldType(hField, hFieldClass);
}

CorInfoHelpFunc
ReaderBase::getFieldHelper(
    CORINFO_FIELD_HANDLE hField,
    CorInfoFieldAccess fieldAccess
){
    return m_jitInfo->getFieldHelper(hField, fieldAccess);
}

unsigned int
ReaderBase::getFieldOffset(
    CORINFO_FIELD_HANDLE hField
){
    return (ULONG) m_jitInfo->getFieldOffset(hField);
}

void*
ReaderBase::getStaticFieldAddress(
    CORINFO_FIELD_HANDLE fieldHandle,
    ULONG* offset,
    bool* fBoxed,
    bool* fIndirect
){
    DWORD fieldAttribs, classAttribs;
    CorInfoType fieldType;
    CORINFO_CLASS_HANDLE valClassHandle; // Used when the field is a valuetype
    CORINFO_CLASS_HANDLE classHandleOfField; // The class the field belongs to
    bool fIsStatic, fIsTLS;

    // Set the default return for offset, fBoxed and fIndirect
    *offset    = 0;
    *fBoxed    = false;
    *fIndirect = false;

    fieldAttribs = getFieldAttribs(fieldHandle, CORINFO_ACCESS_ANY);
    fieldType = getFieldType(fieldHandle, &valClassHandle);

    // Static fields of type managed byref are never allowed by the runtime
    ASSERTNR(fieldType != CORINFO_TYPE_BYREF);

    fIsTLS = ((fieldAttribs & CORINFO_FLG_TLS) != 0); 
    fIsStatic = ((fieldAttribs & CORINFO_FLG_STATIC) != 0);

    // For TLS fields, we should never get to this point.  
    // They should be taken care of by calling a runtime helper for their addresses.
    ASSERTNR(!fIsTLS && fIsStatic);

    classHandleOfField = getFieldClass(fieldHandle);
    classAttribs = getClassAttribs(classHandleOfField);

    // For a managed value type the EE will give us the handle to
    // the boxed object. We then access the unboxed object from it.
    if (fieldAttribs & CORINFO_FLG_STATIC_IN_HEAP)
    {
        *fBoxed = true;
    }

    ASSERTNR((fieldAttribs & CORINFO_FLG_HELPER) == 0);
    if ((fieldAttribs & CORINFO_FLG_SHARED_HELPER) || (fieldAttribs & CORINFO_FLG_GENERICS_STATIC))
    {
         ULONG fieldOffset = getFieldOffset(fieldHandle);
  
        // If the field is a managed object ref then we need to add 
        // an indirection since what we real have is a handle
        if (fIsTLS && ((fieldType == CORINFO_TYPE_CLASS) || *fBoxed))
        {
            *fIndirect = true;
        }
       
        *offset = fieldOffset;
        return NULL;
    }
    else
    {
        void * pFieldAddress;
        void * fieldAddress = (void*) m_jitInfo->getFieldAddress(fieldHandle, &pFieldAddress);

        ASSERTNR( ((fieldAddress != NULL) && (pFieldAddress == NULL)) ||
                  ((fieldAddress == NULL) && (pFieldAddress != NULL)) );           

        if (fieldAddress == NULL) {
            *fIndirect = true;
            return pFieldAddress;
        }
        
        return fieldAddress;
    }
}

void *
ReaderBase::getJustMyCodeHandle(
    CORINFO_METHOD_HANDLE handle,
    bool*                 fIndirect
){
    CORINFO_JUST_MY_CODE_HANDLE dbgHandle, *pDbgHandle = NULL;
    dbgHandle = m_jitInfo->getJustMyCodeHandle(handle, &pDbgHandle);
    ASSERTNR(!(dbgHandle && pDbgHandle)); // can't have both non-null

    if (dbgHandle) {
        *fIndirect = false;
        return dbgHandle;
    } else {
        *fIndirect = true;
        return pDbgHandle;
    }
}

void *
ReaderBase::getMethodSync(bool *fIndirect)
{
    void * critSect = 0, *pCritSect = 0;
    critSect = m_jitInfo->getMethodSync(getCurrentMethodHandle(), &pCritSect);
    ASSERT((!critSect) != (!pCritSect));

    if (critSect) {
        *fIndirect = false;
        return critSect;
    } else {
        *fIndirect = true;
        return pCritSect;
    }
}

void*
ReaderBase::getCookieForPInvokeCalliSig(
    CORINFO_SIG_INFO *sigTarget,
    bool* fIndirect)
{
    void *calliCookie, *pCalliCookie = NULL;
    calliCookie = m_jitInfo->GetCookieForPInvokeCalliSig(sigTarget, &pCalliCookie);
    ASSERTNR((!calliCookie) != (!pCalliCookie)); // can't have both non-null/null

    if (calliCookie) {
        *fIndirect = false;
        return calliCookie;
    } else {
        *fIndirect = true;
        return pCalliCookie;
    }
}

void*
ReaderBase::getAddressOfPInvokeFixup(
    CORINFO_METHOD_HANDLE methodHandle, 
    InfoAccessType *accessType)
{
    ASSERTNR(accessType);
    void *pAddress;
    void *address = m_jitInfo->getAddressOfPInvokeFixup(methodHandle, &pAddress);

    if (address) {
        *accessType = IAT_VALUE;
        return address;
    } else {
        ASSERTNR(pAddress);
        *accessType = IAT_PVALUE;
        return pAddress;
    }
}

void *
ReaderBase::getPInvokeUnmanagedTarget(
    CORINFO_METHOD_HANDLE methodHandle
){
    void *dummy;
    // Always retuns the entry point of the call or null.
    return m_jitInfo->getPInvokeUnmanagedTarget(methodHandle, &dummy);
}


bool
ReaderBase::pInvokeMarshalingRequired(CORINFO_METHOD_HANDLE mHandle, CORINFO_SIG_INFO *sig)
{
    return m_jitInfo->pInvokeMarshalingRequired(mHandle,sig) ? true : false;
}

mdToken
ReaderBase::getMemberParent(
    mdToken token
){
    return m_jitInfo->getMemberParent(getCurrentModuleHandle(), token);
}

mdToken
ReaderBase::getMemberParent(
    CORINFO_MODULE_HANDLE context, mdToken token 
){
    return m_jitInfo->getMemberParent(context, token);
}
//
// method
//

char*
ReaderBase::getMethodName(
    CORINFO_METHOD_HANDLE handle,const char** moduleName
){
    return (char *)m_jitInfo->getMethodName(handle,moduleName);
}

// Find the attribs of the method handle
DWORD
ReaderBase::getMethodAttribs(
    CORINFO_METHOD_HANDLE handle,
    CORINFO_METHOD_HANDLE context
){
    return m_jitInfo->getMethodAttribs(handle,context);
}

// Using context of current method (m_methodInfo) being the caller, find the attribs of
// handle.
DWORD
ReaderBase::getMethodAttribs(
    CORINFO_METHOD_HANDLE handle
){
    return getMethodAttribs(handle,getCurrentMethodHandle());
}

void
ReaderBase::setMethodAttribs(
    CORINFO_METHOD_HANDLE handle,
    CorInfoMethodRuntimeFlags flag
){
    return m_jitInfo->setMethodAttribs(handle, flag);
}



void
ReaderBase::getMethodSig(CORINFO_METHOD_HANDLE handle,CORINFO_SIG_INFO *sig)
{
    m_jitInfo->getMethodSig(handle,sig);
}

const char *
ReaderBase::getMethodRefInfo(
    CORINFO_METHOD_HANDLE  handle,
    CorInfoCallConv       *pConv,
    CorInfoType           *pCorType,
    CORINFO_CLASS_HANDLE  *pRetTypeClass,
    const char           **pModName
){
    CORINFO_SIG_INFO sig;

    // Fetch signature.
    m_jitInfo->getMethodSig(handle,&sig);

    // Get the calling convention
    *pConv = sig.getCallConv();

    // Get the return type
    *pCorType = sig.retType;
    *pRetTypeClass = sig.retTypeClass;

    // Get method and module name.
    return (char *)m_jitInfo->getMethodName(handle,pModName);
}

void
ReaderBase::getMethodSigData(
    CorInfoCallConv*      conv,
    CorInfoType*          returnType,
    CORINFO_CLASS_HANDLE* returnClass,
    int*                  totalILArgs,
    bool*                 isVarArg,
    bool*                 hasThis,
    unsigned __int8*      retSig
){
    CORINFO_SIG_INFO sig;

    m_jitInfo->getMethodSig(getCurrentMethodHandle(),&sig);
    *conv = sig.getCallConv();
    *returnType = sig.retType;
    *returnClass = sig.retTypeClass;
    *totalILArgs = (int)sig.totalILArgs();
    *isVarArg = sig.isVarArg();
    *hasThis = sig.hasThis();
    *retSig = NULL;
}

void
ReaderBase::getMethodInfo(
    CORINFO_METHOD_HANDLE handle,
    CORINFO_METHOD_INFO *info
){
    m_jitInfo->getMethodInfo(handle, info);
}

LONG ReaderBase::EEJITFilter(PEXCEPTION_POINTERS pExceptionPointers, LPVOID lpvParam)
{
   ICorJitInfo * jitInfo = (ICorJitInfo *)lpvParam;
   int answer = jitInfo->FilterException(pExceptionPointers);

   verLastError = jitInfo->GetErrorHRESULT(pExceptionPointers);
   return answer;
}


CORINFO_METHOD_HANDLE
ReaderBase::getMethodHandle(
    mdToken token
){
    return getMethodHandle(token, getCurrentMethodHandle(), getCurrentModuleHandle());
}


CORINFO_METHOD_HANDLE
ReaderBase::getMethodHandle(
    mdToken token,
    CORINFO_METHOD_HANDLE context,
    CORINFO_MODULE_HANDLE scope
){
    CORINFO_METHOD_HANDLE methodHnd = NULL;        
    PAL_TRY
    {
        methodHnd = m_jitInfo->findMethod(scope, token, context);
    }
    PAL_EXCEPT_FILTER(EEJITFilter, m_jitInfo)
    {
        VerifyOrReturn(false, MVER_E_TOKEN_RESOLVE);
    }
    PAL_ENDTRY
    return methodHnd;
}




// Finds name of MemberRef or MethodDef token
void
ReaderBase::findNameOfToken(
    mdToken token,
    __out_ecount(cchBuffer) char * szBuffer,
    size_t cchBuffer
){
    m_jitInfo->findNameOfToken(getCurrentModuleHandle(), token, szBuffer, cchBuffer);
}

CORINFO_CLASS_HANDLE
ReaderBase::getClassHandle(
    mdToken token
){
    return getClassHandle(token, getCurrentMethodHandle(),getCurrentModuleHandle());
}

CORINFO_CLASS_HANDLE
ReaderBase::getClassHandle(
    mdToken token,
    CORINFO_METHOD_HANDLE context,
    CORINFO_MODULE_HANDLE scope,
    CorInfoTokenKind      tokenKind 
){
    return m_jitInfo->findClass(scope, token, context, tokenKind);
}

CORINFO_CLASS_HANDLE
ReaderBase::getClassHandle(
    mdToken token,
    CORINFO_METHOD_HANDLE context,
    CORINFO_MODULE_HANDLE scope
){
    return m_jitInfo->findClass(scope, token, context);
}

CORINFO_MODULE_HANDLE 
ReaderBase::getClassModuleForStatics(
    CORINFO_CLASS_HANDLE classHandle
){
    return m_jitInfo->getClassModuleForStatics(classHandle);
}

//
// In general one should use embedGenericHandle via GenIR_GenericTokenToNode
//   instead of embedMethodHandle. This also holds true for embedClassHandle as
//   well. This is due to the fact that special handling has to be done in the
//   case of generics.
//
// Currently embedMethodHandle is called from four places. 
//   1 - GenIR_GenericTokenToNode
//   2 - checkCallAuthorization
//   3 - callPinvokeInlineHelper
//   4 - GenIR_InsertClassConstructor
//
// DSyme says it is also okay to use embedMethodHandle as the second argument
//  passed to the MDArrayHelper. I don't currently see this in our code.
//
CORINFO_METHOD_HANDLE
ReaderBase::embedMethodHandle(
    CORINFO_METHOD_HANDLE   methodHandle,
    bool *pfIndirect
){
    void *embMethodHandle,*indMethodHandle;

    embMethodHandle = m_jitInfo->embedMethodHandle(methodHandle, &indMethodHandle);
    if (embMethodHandle) {
        ASSERTNR(!indMethodHandle);
        *pfIndirect = false;
        return (CORINFO_METHOD_HANDLE)embMethodHandle;
    } else {
        ASSERTNR(indMethodHandle);
        *pfIndirect = true;
        return (CORINFO_METHOD_HANDLE)indMethodHandle;
    }
}


// In general one should use embedGenericHandle via GenIR_GenericTokenToNode
//   instead of embedClassHandle. This also holds true for embedMethodHandle as
//   well. This is due to the fact that special handling has to be done in the
//   case of generics.
//
// Currently embedMethodHandle is called from two places. 
//   1 - GenIR_InsertClassConstructor
//   2 - rdrCallWriteBarrierHelper (okayed by DSyme)
//
CORINFO_CLASS_HANDLE
ReaderBase::embedClassHandle(
    CORINFO_CLASS_HANDLE classHandle, 
    bool    *pfIndirect
){
    void *embClassHandle,*indClassHandle;

    embClassHandle = m_jitInfo->embedClassHandle(classHandle, &indClassHandle);
    if (embClassHandle) {
        ASSERTNR(!indClassHandle);
        *pfIndirect = false;
        return (CORINFO_CLASS_HANDLE)embClassHandle;
    } else {
        ASSERTNR(indClassHandle);
        *pfIndirect = true;
        return (CORINFO_CLASS_HANDLE)indClassHandle;
    }
}

CorInfoHelpFunc
ReaderBase::getSharedStaticBaseHelper(
    CORINFO_FIELD_HANDLE fld,
    bool                 fRuntimeLookup
){
    return m_jitInfo->getSharedStaticBaseHelper(fld,fRuntimeLookup);
}

CorInfoHelpFunc
ReaderBase::getSharedCCtorHelper(
    CORINFO_CLASS_HANDLE clsHnd
){
    return m_jitInfo->getSharedCCtorHelper(clsHnd);
}

CorInfoHelpFunc
ReaderBase::getBoxHelper(
    CORINFO_CLASS_HANDLE clsHnd
){
    return m_jitInfo->getBoxHelper(clsHnd);
}

CorInfoHelpFunc
ReaderBase::getUnBoxHelper(
    CORINFO_CLASS_HANDLE clsHnd,
    BOOL* helperCopies
){
    return m_jitInfo->getUnBoxHelper(clsHnd, helperCopies);
}

void*
ReaderBase::getAddrOfCaptureThreadGlobal(bool *pfIndirect)
{
   void * addr, *indAddr;

    addr = m_jitInfo->getAddrOfCaptureThreadGlobal(&indAddr);

    if (addr) {
        ASSERTNR(!indAddr);
        *pfIndirect = false;
        return addr;
    } else {
        ASSERTNR(indAddr);
        *pfIndirect = true;
        return indAddr;
    }
}

// NOTE: This calls getCallInfo with CORINFO_CALLINFO_KINDONLY set
void 
ReaderBase::getVirtualCallKind(
    mdToken methodToken,
    mdToken constraintToken,
    CORINFO_CALL_INFO *pResult
){
    m_jitInfo->getCallInfo(m_methodInfo->ftn,   // methodBeingCompiledHnd: this is not used since we're passing CORINFO_CALLINFO_KINDONLY
                           m_methodInfo->scope,
                           methodToken,
                           constraintToken,
                           m_methodInfo->ftn,
                           (CORINFO_CALLINFO_FLAGS)(CORINFO_CALLINFO_CALLVIRT | CORINFO_CALLINFO_KINDONLY),
                           pResult);
}

void 
ReaderBase::getCallInfo(
    mdToken methodToken,
    mdToken constraintToken,
    bool fVirtual,
    CORINFO_CALL_INFO *pResult,
    CORINFO_METHOD_HANDLE context,
    CORINFO_MODULE_HANDLE scope
){
    m_jitInfo->getCallInfo(context, scope,
                            methodToken, constraintToken,
                            context,
                            (CORINFO_CALLINFO_FLAGS)(fVirtual ? CORINFO_CALLINFO_CALLVIRT : 0),
                            pResult);
}

void 
ReaderBase::getCallInfo(
    mdToken methodToken,
    mdToken constraintToken,
    bool fVirtual,
    CORINFO_CALL_INFO *pResult
){
    getCallInfo(methodToken,constraintToken,fVirtual,pResult,getCurrentMethodHandle(),getCurrentModuleHandle());
}

void *
ReaderBase::getEmbedModuleDomainID(
    CORINFO_MODULE_HANDLE moduleHandle,
    bool    *pfIndirect
){
    size_t         dirModuleDomainID; 
    void *         indModuleDomainID;


    dirModuleDomainID = m_jitInfo->getModuleDomainID(moduleHandle, &indModuleDomainID);

    if (indModuleDomainID == NULL) {
        *pfIndirect = FALSE;
        /* Note this is an unsigned integer not a relocatable pointer */
        return (void *) dirModuleDomainID;
    } else {
        *pfIndirect = TRUE;
        return indModuleDomainID;
    }
}


void *
ReaderBase::getEmbedClassDomainID(
    CORINFO_CLASS_HANDLE classHandle,
    bool    *pfIndirect
){
    unsigned int   dirClassDomainID; 
    void *         indClassDomainID;


    dirClassDomainID = m_jitInfo->getClassDomainID(classHandle, &indClassDomainID);

    if (indClassDomainID == NULL) {
        *pfIndirect = false;
        /* Note this is an unsigned integer not a relocatable pointer */
        return (void *) (size_t) dirClassDomainID;
    } else {
        *pfIndirect = true;
        return indClassDomainID;
    }
}

InfoAccessType
ReaderBase::constructStringLiteral(
    mdToken token,
    void **ppInfo
){
    return m_jitInfo->constructStringLiteral(getCurrentModuleHandle(), token, ppInfo);
}

#if defined(CC_CALL_INTERCEPT)
CorInfoIsCallAllowedResult
ReaderBase::isCallAllowedByHost(
    CORINFO_METHOD_HANDLE callerHandle,
    CORINFO_METHOD_HANDLE handle,
    CORINFO_CALL_ALLOWED_INFO * callAllowedInfo
){
    return m_jitInfo->isCallAllowed(callerHandle, handle, callAllowedInfo);
}

void
ReaderBase::checkCallAuthorization(
    mdToken               token,
    CORINFO_METHOD_HANDLE handle,
    IRNode** newIR
){
    IRNode *callerHandle, *calleeHandle;
    void *embeddedCallerHandle, *embeddedCalleeHandle;
    CorInfoIsCallAllowedResult callAuth;

    CORINFO_CALL_ALLOWED_INFO callAllowedInfo; 

    callAuth = isCallAllowedByHost(getCurrentMethodHandle(), handle, &callAllowedInfo);

    switch(callAuth){
    case CORINFO_CALL_ALLOWED:
        // No problem
        break;
    default:
        // Decision deferred
        
        // Authorization decision deferred, emit call to clr.
        // Called function will throw if authorization is denied,
        // otherwise returns.
        // The type of helper call emitted depends on the callAllowedInfo mask
     
        bool fIndirect;
        CORINFO_METHOD_HANDLE curMethodHandle = getCurrentMethodHandle();

        GenIR_RemoveStackInterference(newIR);

        embeddedCallerHandle = embedMethodHandle(curMethodHandle, &fIndirect);
        callerHandle = GenIR_HandleToIRNode(
            getMethodDefFromMethod(curMethodHandle),
            embeddedCallerHandle,
            curMethodHandle,
            fIndirect, fIndirect, true, false, newIR);

        embeddedCalleeHandle = embedMethodHandle(handle, &fIndirect);
        calleeHandle = GenIR_HandleToIRNode(
            token,
            embeddedCalleeHandle,
            handle,
            fIndirect, fIndirect, true, true, newIR);

        // CORINFO_CALL_ALLOWED_BYSECURITY is the only supported mask
        ASSERTNR((callAllowedInfo.mask & ~CORINFO_CALL_ALLOWED_BYSECURITY) == 0);

        if (callAllowedInfo.mask & CORINFO_CALL_ALLOWED_BYSECURITY) {
             GenIR_CallHelper(CORINFO_HELP_CALL_ALLOWED_BYSECURITY, NULL, newIR, callerHandle, calleeHandle);
        }
        break;
    }
}

#endif

bool 
ReaderBase::canTailCall(
    CORINFO_METHOD_HANDLE target, 
    bool hasThis,
    bool isTailPrefix
){
    return m_jitInfo->canTailCall(getCurrentMethodHandle(), target, isTailPrefix);
}

CorInfoInline 
ReaderBase::canInline(
    CORINFO_METHOD_HANDLE  caller,
    CORINFO_METHOD_HANDLE  target,
    DWORD                 *pRestrictions
){
    return m_jitInfo->canInline(caller, target, pRestrictions);
}

CORINFO_ARG_LIST_HANDLE
ReaderBase::getArgNext(
    CORINFO_ARG_LIST_HANDLE args
)
{
    return m_jitInfo->getArgNext(args);
}

CorInfoTypeWithMod
ReaderBase::getArgType(
    CORINFO_SIG_INFO            *sig,
    CORINFO_ARG_LIST_HANDLE      args,
    CORINFO_CLASS_HANDLE        *vcTypeRet)
{
    return m_jitInfo->getArgType(sig, args, vcTypeRet);
}

CORINFO_CLASS_HANDLE
ReaderBase::getArgClass(
    CORINFO_SIG_INFO *sig,
    CORINFO_ARG_LIST_HANDLE args
)
{
    return m_jitInfo->getArgClass(sig, args);
}

CORINFO_CLASS_HANDLE
ReaderBase::getBuiltinClass(
    CorInfoClassId classId
)
{
    return m_jitInfo->getBuiltinClass(classId);
}

CorInfoType
ReaderBase::getChildType(
    CORINFO_CLASS_HANDLE  clsHnd,
    CORINFO_CLASS_HANDLE *clsRet
)
{
    return m_jitInfo->getChildType(clsHnd, clsRet);
}

BOOL
ReaderBase::isSDArray(
    CORINFO_CLASS_HANDLE  hClass
)
{
    return m_jitInfo->isSDArray(hClass);
}

unsigned
ReaderBase::getArrayRank(
    CORINFO_CLASS_HANDLE  hClass
)
{
    return m_jitInfo->getArrayRank(hClass);
}

/*++
  Function: RgnRangeIsEnclosedInRegion
    startOffset : the start offset of the MSIL EH region (handler or trybody)
    endOffset : the end offset of the MSIL EH region (handler or trybody)
    pRgn : pointer to the region that we would like to check 
    
  Description:
    return true [startOffset..endOffset-1] is enclosed in
    [REGION_START_MSIL_OFFSET(pRgn)..REGION_END_MSIL_OFFSET(pRgn))
 --*/
static int
RgnRangeIsEnclosedInRegion(
    DWORD startOffset,
    DWORD endOffset,
    EHRegion* pRgn
){
    //ASSERTNR(pRgn);
    return (RgnGetStartMSILOffset(pRgn) <= startOffset) && (endOffset - 1 < RgnGetEndMSILOffset(pRgn));
}


/*++
  Function: THX_RgnFindLowestEnclosingRegion
    pRgnTree : the root of the region subtree that we would like to search in
    startOffset : the start offset of the MSIL EH region (handler or trybody)
    endOffset : the end offset of the MSIL EH region (handler or trybody)

  Description:
    find the lowest (the region nearest to the leaf) in the region (sub)-tree
    that contains [startOffset..endOffset)

--*/
EHRegion*
RgnFindLowestEnclosingRegion(
    EHRegion* pRgnTree,
    DWORD startOffset,
    DWORD endOffset
){
    EHRegion* pTryChild;
    EHRegion* pRetval = NULL;

    // pRgnTree is a non-empty region tree
    //ASSERTNR(pRgnTree);

    // assumption (rule #3 and #4), none of the children overlap, so
    // there is at most one child that enclose the region
    
    // if (one of children of pRgnTree contain the range)
    //     return findLowestEnclosingRegion(<child of pRgnTree that contain the range>, start, end);

    EHRegionList* list;
    for (list = RgnGetChildList(pRgnTree);
         list;
         list = RgnListGetNext(list)) {

        ReaderBaseNS::RegionKind type;

        pTryChild = RgnListGetRgn(list);
        type = RgnGetRegionType(pTryChild);

        if ((type == ReaderBaseNS::RGN_TRY) &&
            RgnRangeIsEnclosedInRegion(startOffset, endOffset, pTryChild)) {
            pRetval = RgnFindLowestEnclosingRegion(pTryChild, startOffset, endOffset);
            break;
        } else {
            if (type == ReaderBaseNS::RGN_TRY) {
                // if the try body doesn't contain the range,
                // check if any of its handler (non RGN_TRY) children does
                EHRegion* pHandlerChild;
                //ASSERTNR(!RgnRangeIsEnclosedInRegion(startOffset, endOffset, pTryChild));
                //ASSERTNR(REGION_CHILDREN(pTryChild));

                EHRegionList* innerList;
                for (innerList = RgnGetChildList(pTryChild);
                     innerList;
                     innerList = RgnListGetNext(innerList))
                {
                    pHandlerChild = RgnListGetRgn(innerList);
                    // if there are children of type RGN_TRY in pTryChild, it should have been
                    // covered by pTryChild start/end offset
                    if ((RgnGetRegionType(pHandlerChild) != ReaderBaseNS::RGN_TRY) &&
                        RgnRangeIsEnclosedInRegion(startOffset, endOffset, pHandlerChild)) {
                        pRetval = RgnFindLowestEnclosingRegion(pHandlerChild, startOffset, endOffset);
                        break; 
                    }
                }
            } else {
                // not a try region, it should have been checked earlier
                continue;
            }
        }
    }

    // if there are no children, then the current node should be the lowest enclosing region
    // or if there is no child that enclose the region
    if (!pRetval) pRetval =  pRgnTree;

    //ASSERTNR(pRetval);
    return pRetval;
}


// Push a child on the parent.
void
ReaderBase::rgnPushRegionChild(
    EHRegion *parent,
    EHRegion *child
){
    EHRegionList* element;

    element = RgnAllocateRegionList();
    RgnListSetRgn(element,child);
    RgnListSetNext(element,RgnGetChildList(parent));
    RgnSetChildList(parent,element);
}


// Allocate a region structure
EHRegion*
ReaderBase::rgnMakeRegion(
    ReaderBaseNS::RegionKind   type, 
    EHRegion                  *parent,
    EHRegion                  *regionRoot,
    EHRegionList             **allRegionList
){
    EHRegionList *rgnList = RgnAllocateRegionList();
    EHRegion *res = RgnAllocateRegion();

    // Push new region onto the AllRegionList.
    RgnListSetNext(rgnList,*allRegionList);
    RgnListSetRgn(rgnList,res);
    *allRegionList = rgnList;

    // Convert from ReaderBaseNS region kind to utc region kind...
    // This can go away when the region kind is moved a more

    RgnSetRegionType(res,type);
    RgnSetHead(res,NULL);
    RgnSetLast(res,NULL);
    RgnSetIsLive(res,false);
    RgnSetParent(res,parent);
    RgnSetChildList(res,NULL);
    RgnSetHasNonLocalFlow(res,false);

    if (type == ReaderBaseNS::RGN_TRY) {
        RgnSetTryCanonicalExitOffset(res,CANONICAL_EXIT_INIT_VAL);
    }

    if (parent) {
        rgnPushRegionChild(parent, res);
    }

    return res;
}

/*

These are rules to follow when setting up the EIT:  
1. Ordering: The handlers should appear with the most nested handlers
   at the top to the outermost handlers at the bottom.  
2. Nesting: If one handler protects any region protected by another
   handler it must protect that entire handler including its try, catch,
   filter and fault blocks (that is if you protect a try block, you must also protect the catch).  
3. Nesting: A try block can not include its own filter, catch or finally blocks
4. Nesting: A single Handler must constitute a contiguous block of IL instructions which
   cannot overlap those of another handler (A try may enclose another handler completely).

*/

/*++

  Function: rgnCreateRegionTree
    pEHClauses : EIT clauses
    count : number of EIT clauses
    pRgnTree : an empty region tree, containing only RGN_ROOT node
    ciPtr : compiler instance

  Description:
    given a region tree containing one node (RGN_ROOT) and an EIT from runtime that satisfies
    the rules above, create the region tree

    REGION_HEAD and REGION_LAST won't be set here since we don't have the tuples yet
    ALL information from the EIT will be transferred to the region tree
    after this function is finished, we can discard the EIT

--*/

#ifndef NDEBUG

char *rgnTypeNames[] = {
    "RGN_UNKNOWN",
    "RGN_NONE",
    "RGN_ROOT",
    "RGN_TRY",
    "RGN_EXCEPT",
    "RGN_FAULT",
    "RGN_FINALLY",
    "RGN_FILTER",
    "RGN_DTOR",
    "RGN_CATCH",
    "RGN_MEXCEPT",
    "RGN_MCATCH"
};

void DumpRegion(EHRegion *r, int indent=0)
{
    EHRegionList *rl;

    if (indent == 0)
        printf("\n<---------------\n");
        

    for (int i=0; i<indent; i++)
        printf(" ");

    printf("Region type=%s ", rgnTypeNames[RgnGetRegionType(r)]);

    printf("msilstart=%x msilend=%x\n",
           RgnGetStartMSILOffset(r),
           RgnGetEndMSILOffset(r));

    rl = RgnGetChildList(r);
   
    while (rl)
    {
       r = RgnListGetRgn(rl);
       DumpRegion(r, indent+4);
       rl = RgnListGetNext(rl);
    }
  
    if (indent == 0)
        printf("----dump done-----\n\n");
}
#endif // NDEBUG


static bool clauseXInsideY(CORINFO_EH_CLAUSE *x, CORINFO_EH_CLAUSE *y)
{
    // is X inside Y?
    
    // note for both checks:
    // either start OR stop offset may be same, but if both are same, no nesting relationship exists
    //1. is x's try inside y's try?
    if (x->TryOffset >= y->TryOffset
        && x->TryOffset + x->TryLength <= y->TryOffset + y->TryLength
        && x->TryLength != y->TryLength 
        ) 
        return true;

    //2. is x's try inside y's handler?
    if (x->TryOffset >= y->HandlerOffset
        && x->TryOffset + x->TryLength <= y->HandlerOffset + y->HandlerLength
        && x->TryLength != y->HandlerLength)
        return true;

    return false;
}

int __cdecl clauseSortFunction(const void *c1, const void *c2)
{
    CORINFO_EH_CLAUSE *clause1 = *(CORINFO_EH_CLAUSE **) c1;
    CORINFO_EH_CLAUSE *clause2 = *(CORINFO_EH_CLAUSE **) c2;

    if (clauseXInsideY(clause1, clause2))
    {
        return -1;
    }
    if (clauseXInsideY(clause2, clause1))
    {
        return 1;
    }
    else
    {
        // IMPORTANT: if there is no nesting relationship between two,
        // we sort on the pointer to the clause itself, preserving the EIT ordering.
        // EIT order is significant in these cases and we cannot just ignore it.
        return c2 > c1 ? -1 : 1;
    }
}

void
ReaderBase::rgnCreateRegionTree(
    void
){
    CORINFO_EH_CLAUSE *pEHClauses;
    EHRegion *pRgnTree, *pRgnTreeRoot;
    EHRegionList *allRegionList;

    // Initialize all region list to NULL.
    allRegionList = NULL;

    pRgnTreeRoot = NULL;

    pEHClauses = NULL;

    if (m_methodInfo->EHcount > 0) {
        unsigned int numEHMarkers;

        numEHMarkers = 2 * m_methodInfo->EHcount;
        pEHClauses = (CORINFO_EH_CLAUSE*) getProcMemory(sizeof(CORINFO_EH_CLAUSE) * 
                                                        m_methodInfo->EHcount);
        ASSERTNR(pEHClauses);
        
        for (unsigned short i = 0; i < m_methodInfo->EHcount; i++) {
            m_jitInfo->getEHinfo(getCurrentMethodHandle(), i, &(pEHClauses[i]));
        }
    } else {
        // No EH.
        m_ehRegionTree = NULL;
        m_allRegionList = NULL;
        return;
    }

    CORINFO_EH_CLAUSE **clauseList = (CORINFO_EH_CLAUSE **) 
        alloca(sizeof (void*) * m_methodInfo->EHcount);

    // clauseList is an array of pointers into EIT
    for (unsigned j=0; j<m_methodInfo->EHcount; j++)
        clauseList[j] = &pEHClauses[j];

    // now clauselist is sorted w/ inner regions first
    qsort(clauseList, m_methodInfo->EHcount, sizeof(void*), clauseSortFunction);



    m_ehClauseInfo = pEHClauses;

    //DBEXEC(ifdb(DB_EIT_DUMP), IMetaPrintCorInfoEHClause(pEHClauses, m_methodInfo->EHcount));

    pRgnTreeRoot = rgnMakeRegion(ReaderBaseNS::RGN_ROOT,NULL,pRgnTreeRoot,&allRegionList);
    pRgnTree = pRgnTreeRoot;

    // Map the clause information into try regions for later processing
    // We need to map the EIT into the tryregion DAG as we need to 
    // maintain information as to where the END of all the constructs are
    // "Just cause its an OPLEAVE does not mean its the end of a region"

    // start from bottom to the top, insert things into the current tree
    // the current tree should be initialized to contain one node
    // (THX_RgnMakeRegion(RGN_ROOT,NULL,NULL,ciPtr))

    unsigned short i = m_methodInfo->EHcount;

    do {
        i--;
        
        CORINFO_EH_CLAUSE* pCurrentEHC = clauseList[i];

        EHRegion* pEnclosingRgn =
            RgnFindLowestEnclosingRegion(pRgnTree, pCurrentEHC->TryOffset,
                                         pCurrentEHC->TryOffset+pCurrentEHC->TryLength);
        
        EHRegion* pRgnTry = NULL;
        EHRegion* pRgnHandler = NULL;

        ASSERTNR(pCurrentEHC);

        if ((RgnGetRegionType(pEnclosingRgn) == ReaderBaseNS::RGN_TRY) &&
            (pCurrentEHC->TryOffset == RgnGetStartMSILOffset(pEnclosingRgn)) &&
            ((pCurrentEHC->TryOffset + pCurrentEHC->TryLength) == RgnGetEndMSILOffset(pEnclosingRgn))) {
            
            // pEnclosingRgn is a try region that is described by pCurrentEHC
            
            // try region already exist
            // (this is the case if there are multiple handlers for one try block)
            // try region should be the parent of the handleer region
            
            pRgnTry = pEnclosingRgn;
        } else {
            // create a new try region, make it a child of pEnclosingRgn
            pRgnTry = rgnMakeRegion(ReaderBaseNS::RGN_TRY, pEnclosingRgn, pRgnTreeRoot,&allRegionList);

            RgnSetStartMSILOffset(pRgnTry,pCurrentEHC->TryOffset);
            RgnSetEndMSILOffset(pRgnTry,pCurrentEHC->TryOffset+pCurrentEHC->TryLength);
            RgnSetEndOfClauses(pRgnTry,NULL);
            RgnSetTryBodyEnd(pRgnTry,NULL);
            RgnSetTryType(pRgnTry,ReaderBaseNS::TRY_NONE);
        }


        if (pCurrentEHC->Flags & CORINFO_EH_CLAUSE_FILTER) {
            EHRegion* pRgnFilter;
            pRgnHandler = rgnMakeRegion(ReaderBaseNS::RGN_MEXCEPT, pRgnTry, pRgnTreeRoot,&allRegionList);

            ReaderBaseNS::TryKind tryKind = RgnGetTryType(pRgnTry);
            if (tryKind == ReaderBaseNS::TRY_MCATCH) {
                RgnSetTryType(pRgnTry,ReaderBaseNS::TRY_MCATCHXCPT);
            } else {
                if (tryKind == ReaderBaseNS::TRY_NONE) {
                    RgnSetTryType(pRgnTry,ReaderBaseNS::TRY_MXCPT);
                } else {
                    ASSERTNR(tryKind == ReaderBaseNS::TRY_MXCPT || 
                             tryKind == ReaderBaseNS::TRY_MCATCHXCPT);
                }
            }

            pRgnFilter = rgnMakeRegion(ReaderBaseNS::RGN_FILTER, pRgnTry,pRgnTreeRoot, &allRegionList);

            RgnSetExceptFilterRegion(pRgnHandler,pRgnFilter);
            RgnSetExceptTryRegion(pRgnHandler,pRgnTry);
            RgnSetExceptUsesExCode(pRgnHandler,false);
            
            RgnSetFilterTryRegion(pRgnFilter,pRgnTry);
            RgnSetFilterHandlerRegion(pRgnFilter,pRgnHandler);

            RgnSetStartMSILOffset(pRgnFilter,pCurrentEHC->FilterOffset);
            // The end of the filter, is the start of its handler
            RgnSetEndMSILOffset(pRgnFilter, pCurrentEHC->HandlerOffset);
        } else {
            

            if (pCurrentEHC->Flags & CORINFO_EH_CLAUSE_FINALLY) {
                pRgnHandler = rgnMakeRegion(ReaderBaseNS::RGN_FINALLY, pRgnTry,pRgnTreeRoot,
                                            &allRegionList);

                ASSERTNR(RgnGetTryType(pRgnTry) == ReaderBaseNS::TRY_NONE);
                RgnSetTryType(pRgnTry,ReaderBaseNS::TRY_FIN);
                RgnSetFinallyTryRegion(pRgnHandler,pRgnTry);

            } else {
                if (pCurrentEHC->Flags & CORINFO_EH_CLAUSE_FAULT) {

                    ASSERTNR(RgnGetTryType(pRgnTry) == ReaderBaseNS::TRY_NONE);
                    pRgnHandler = rgnMakeRegion(ReaderBaseNS::RGN_FAULT, pRgnTry,
                                                pRgnTreeRoot,&allRegionList);
                    RgnSetTryType(pRgnTry,ReaderBaseNS::TRY_FAULT);
                    RgnSetFaultTryRegion(pRgnHandler,pRgnTry);
                } else {
                    // we need to touch the class at JIT time
                    // otherwise the classloader kicks in at exception time 
                    // (possibly stack overflow exception) in which case
                    // we are in danger of going past the stack guard
                    
                    if (pCurrentEHC->ClassToken)
                        getClassHandle(pCurrentEHC->ClassToken);

                    // this will be a catch (EH_CLAUSE_NONE)
                    // we need to keep the token somewhere            
                    pRgnHandler = rgnMakeRegion(ReaderBaseNS::RGN_MCATCH, pRgnTry,
                                                pRgnTreeRoot,&allRegionList);

                    ReaderBaseNS::TryKind tryKind;

                    tryKind = RgnGetTryType(pRgnTry);
                    if (tryKind == ReaderBaseNS::TRY_MXCPT) {
                        RgnSetTryType(pRgnTry,ReaderBaseNS::TRY_MCATCHXCPT);
                    } else {
                        if (tryKind == ReaderBaseNS::TRY_NONE) {
                            RgnSetTryType(pRgnTry,ReaderBaseNS::TRY_MCATCH);
                        } else {
                            ASSERTNR((RgnGetTryType(pRgnTry) == ReaderBaseNS::TRY_MCATCH) || 
                                     (RgnGetTryType(pRgnTry) == ReaderBaseNS::TRY_MCATCHXCPT));
                        }
                    }
                    RgnSetCatchClassToken(pRgnHandler,pCurrentEHC->ClassToken);
                    RgnSetCatchTryRegion(pRgnHandler,pRgnTry);
                }
            }
        }
        RgnSetStartMSILOffset(pRgnHandler,pCurrentEHC->HandlerOffset);
        RgnSetEndMSILOffset(pRgnHandler,pCurrentEHC->HandlerOffset+pCurrentEHC->HandlerLength);
    } while (i != 0);

    // The memory allocated for EHClause will be freed when someone called THX_freemem(T_ALLOC)
    // hopefully this happens in MSILReadProc somewhere

    m_ehRegionTree = pRgnTreeRoot;
    m_allRegionList = allRegionList;
}

//
// setupBlockForEH
//
// Called from ReaderBase::readBytesForFlowGraphNode to ensure that only exception
// object is on operand stack on entry to funclet.
//
// If GenIR needs a callback whenever a new region is entered, then this is the place
// to put it.
//
// (1) If we are entering a catch, except, or filter region for the first time,
//     we assert that the stack is empty and push a GenIR'd ExceptionObject.
// (2) Set a debug sequence point at the start of the an catch/filter/except. This is
//     required since the stack is not empty at these points.
//
void
ReaderBase::setupBlockForEH(
    IRNode** newIR
)
{
    FlowGraphNode *fg = m_currentFgNode;

    if (m_currentRegion != NULL)
    {
        RgnSetIsLive(m_currentRegion,true);

        if (GenIR_IsRegionStartBlock(fg))
        {
            ReaderBaseNS::RegionKind regionKind = RgnGetRegionType(m_currentRegion);
            switch(regionKind) {
                case ReaderBaseNS::RGN_MCATCH:
                case ReaderBaseNS::RGN_MEXCEPT:
                case ReaderBaseNS::RGN_FILTER:

                    // SEQUENCE POINTS: ensure sequence point at eh region start
                    if (GenIR_NeedSequencePoints()) {
                        GenIR_SetSequencePoint(FgNodeGetStartMSILOffset(fg),ICorDebugInfo::SOURCE_TYPE_INVALID,newIR);
                    }

                    // If we are currently entering a handler, we must reset the evaluation
                    // stack so that the only item on the stack is the exception object.
                    IRNode* exObj;

                    // Note, for example, that web prop might have dumped something here.
                    // It wouldn't get used anyway so clearing it is no harm.
                    m_readerStack->clearStack();

                    // Make the exception object and push it onto the empty stack.
                    exObj = GenIR_MakeExceptionObject(newIR);

                    // Passing NULL is okay here because the second argument is
                    //  only used to convert types. exObj is a reference (int64)
                    //  so it never needs a conversion.
                    m_readerStack->push(exObj, NULL);
                    break;

                case ReaderBaseNS::RGN_TRY:
                    // Entering a try region, the evaluation stack is required to be empty.
                    if (!m_readerStack->empty()) {
                        BADCODE(MVER_E_TRY_N_EMPTY_STACK);                    
                    }
                    break;
                default:
                    // reached
                    break;
            }
        }
    }
}


void
ReaderBase::fgAddArcs(
    FlowGraphNode* headBlock
){
    FlowGraphNode *block, *fallThruBlock;
    IRNode *nodeBr, *labelNode;
    bool hasFallThru;

    hasFallThru = false;
    fallThruBlock = NULL;

    // As a special case, localloc is incompatible with the recursive
    // tail call optimization, and any branches that we initially set up
    // for the recursive tail call (before we knew about the localloc)
    // should instead be re-pointed at the fall-through (for tail.call)
    // or the function exit (for jmp).
    if (m_hasLocAlloc) {
        // Revert all recursive branch tuples.
        // For a tail.call, this involves changing to fall-through.
        // For a jmp, this involves changing to point at the exit label.
        BranchList *branchList, *branchListNext;
        for (branchList = GenIR_FgGetLabelBranchList(GenIR_EntryLabel());
             branchList != NULL;
             branchList = branchListNext) {
            branchListNext = BranchListGetNext(branchList);
            nodeBr = BranchListGetIRNode(branchList);
            GenIR_FgRevertRecursiveBranch(nodeBr);
        }
        
    }
    
    // Process the blocks as normal, now that we have fixed any trouble
    // that localloc would cause for the edges that we are about to insert.
    for (block = headBlock;
         block != NULL;
         block = FgNodeGetNext((FlowGraphNode*)block)) {

        if (hasFallThru) {
            GenIR_FgAddArc(NULL,fallThruBlock,(FlowGraphNode*)block);
        }

        labelNode = FgNodeFindStartLabel(block);

        // Block starts with label.  Add arcs from branches to this label.
        if (labelNode) {
            BranchList *branchList;
            for (branchList = GenIR_FgGetLabelBranchList( labelNode);
                 branchList != NULL;
                 branchList = BranchListGetNext(branchList)) {

                nodeBr = BranchListGetIRNode(branchList);
                GenIR_FgAddArc( nodeBr, IRNodeGetEnclosingBlock(nodeBr),
                               (FlowGraphNode*)block);
            }
        }

        fallThruBlock = NULL;

        // Find whether block ends in a branch
        hasFallThru = GenIR_FgBlockHasFallThrough(block);
        if (hasFallThru) {
            fallThruBlock = block;
        }
    }        
}

// Builds flow graph from bytecode and initializes blocks for DFO traversal.
FlowGraphNode*
ReaderBase::buildFlowGraph(
    FlowGraphNode** ppFgTail
){
    FlowGraphNode *headBlock;

    // Build a flow graph from the BYTE codes
    headBlock = fgBuildBasicBlocksFromBytes(m_methodInfo->ILCode, m_methodInfo->ILCodeSize);

    // Add the arcs
    fgAddArcs(headBlock);

    // Return head FlowGraphNode.
    *ppFgTail = GenIR_FgGetTailBlock();
    return headBlock;
}

//

//
LabelOffsetList *
ReaderBase::fgAddLabelMSILOffset(
    IRNode **pLabel,              // A pointer to IRNode* label
    unsigned int targetOffset     // The MSIL offset of the label
){
    LabelOffsetList *elem, *prevElem, *newElem;
    unsigned index;

    // Check to see if we already have this offset
    prevElem = NULL;

    index = targetOffset / LABEL_LIST_ARRAY_STRIDE;
    elem = m_labelListArray[index];

    while ( elem ) {
        if ( elem->getOffset() == targetOffset ) {
            if (pLabel) {
                *pLabel = elem->getLabel();
            }
            return elem;
        } else if ( elem->getOffset() > targetOffset ) {
            // we must insert offsets in order, so since we've
            //  passed the offset we're looking for we're done
            break;
        }
        prevElem = elem;
        elem = elem->getNext();
    }

    // We need to create a new label
    newElem = (LabelOffsetList*)getTempMemory(sizeof(LabelOffsetList));
    newElem->setOffset(targetOffset);

    if (*pLabel == NULL) {
        *pLabel = GenIR_MakeLabel(targetOffset,fgGetRegionFromMSILOffset(targetOffset));
    }
    newElem->setLabel(*pLabel);

    // Insert the new element at the right spot
    if (prevElem) {
        newElem->setNext(elem);
        prevElem->setNext(newElem);
    } else {
        newElem->setNext(m_labelListArray[index]);
        m_labelListArray[index] = newElem;
    }

    return newElem;
}

void
ReaderBase::fgDeleteBlockAndNodes(
    FlowGraphNode* block
){
    FlowGraphEdgeList *arc, *arcNext;

    GenIR_FgDeleteNodesFromBlock( block);

    for (arc = FgNodeGetSuccessorList(block);
         arc != NULL;
         arc = arcNext) {
        arcNext = FgEdgeListGetNextSuccessor(arc);
        GenIR_FgDeleteEdge(arc);
    }
    for (arc = FgNodeGetPredecessorList(block);
        arc != NULL;
        arc = arcNext) {
        arcNext = FgEdgeListGetNextPredecessor(arc);
        GenIR_FgDeleteEdge(arc);
    }

    GenIR_FgDeleteBlock( block);
}


// removeUnusedBlocks
// - Iterate from fgHead to fgTail using FgNodeGetNext
// - fgHead and fgTail must have been visited
// - clear visited bit on remaining blocks
void
ReaderBase::fgRemoveUnusedBlocks(
    FlowGraphNode* fgHead,
    FlowGraphNode* fgTail
){
    FlowGraphNode* block;
    
    // Remove Unused Blocks
    for (block = fgHead; block != fgTail;) {
        FlowGraphNode *nextBlock;
        nextBlock = FgNodeGetNext(block);

        if ( !FgNodeIsVisited(block)) {
            // M00TODO - probably lots more cleanup checking to do.
            // Also need to issue warning when nontrivial code is removed.
            fgDeleteBlockAndNodes(block);
        } else {
            ASSERTNR(FgNodeIsVisited(block));
            FgNodeSetVisited(block,false);
        }
        block = nextBlock;
    }

    // Do some verification on the exit block
    ASSERTNR(block == fgTail);
}


// This code returns the MSIL offset of the "canonical" landing point for leaves
// from a region.  If the last instruction of a region is a leave that doesn't
// point to this point, then it is nonLocal!
unsigned int
ReaderBase::fgGetRegionCanonicalExitOffset(
    EHRegion* tr
){
    unsigned int nCanonOffset;
    EHRegion *pChildRegion, *pTryRegion = NULL, *pParentRegion;
    ReaderBaseNS::RegionKind rType;

    rType = RgnGetRegionType(tr);

    switch (rType) {
    case ReaderBaseNS::RGN_TRY:
        pTryRegion = tr;
        break;
    case ReaderBaseNS::RGN_MCATCH:
    case ReaderBaseNS::RGN_MEXCEPT:
        pTryRegion = RgnGetParent(tr);
        break;
    default:
        // Nonlocal gotos are not legal in other regions!
        //ASSERTNR(UNREACHED);
        return (unsigned int)-1;
        ;
    }

    // Short circuit, use cached result if canonical offset
    // has already been determined for this try region.
    nCanonOffset = (unsigned int)RgnGetTryCanonicalExitOffset(pTryRegion);
    if (nCanonOffset != (unsigned int)CANONICAL_EXIT_INIT_VAL) {
        return nCanonOffset;
    }

    //
    //
    EHRegionList* list;
    int *buff;
    int buffSize;
    int index;
    bool fUsedHeapAllocator = false;

    // Buff size must include an entry for 1 past the end of the buffer. This
    // will be used if the try region ends at the end of the code buffer.
    buffSize = (m_methodInfo->ILCodeSize + 1) * sizeof(unsigned int);
    
    if (buffSize < 100 * 1024) {
        buff = (int*)_alloca(buffSize);
    } else {
        buff = (int*)getTempMemory(buffSize);
        
        // Wups, out of memory!
        if (buff == NULL) {
            GenIR_Fatal(CORJIT_OUTOFMEM);
        }
        
        fUsedHeapAllocator = true;
    }
    
    memset(buff,-1,buffSize);

    // Place catch end offsets at the catch start offsets.
    for (list = RgnGetChildList(pTryRegion); list; list = RgnListGetNext(list)) {
        pChildRegion = RgnListGetRgn(list);
        buff[RgnGetStartMSILOffset(pChildRegion)] = RgnGetEndMSILOffset(pChildRegion);
    }

    // From the end of the current try region, walk to the last adjacent catch.

    index = RgnGetEndMSILOffset(pTryRegion);
    while (buff[index] != -1) {
        index = buff[index];
    }

    nCanonOffset = index;


    if (nCanonOffset != (unsigned int)-1) {
    
        pParentRegion = RgnGetParent(pTryRegion);

        while (pParentRegion) {
            if (RgnGetEndMSILOffset(pParentRegion) == nCanonOffset) {
                nCanonOffset = (unsigned int)-1;
                break;
            }
            pParentRegion = RgnGetParent(pParentRegion);
        }
    }

    // We've determined the canonical exit for this try, cache it.
    RgnSetTryCanonicalExitOffset(pTryRegion,nCanonOffset);

    return nCanonOffset;
}


// Determines if leave causes region exit. This is true if the target
//  of the leave lies in an EH region outside of the EH region that
//  contains the leave.
bool
ReaderBase::fgLeaveIsNonLocal(
    FlowGraphNode* fg,
    unsigned int leaveOffset,
    unsigned int leaveTarget,
    bool* endsWithNonLocalGoto
){
    EHRegion* currentRegion;

    *endsWithNonLocalGoto = false;

    // Technology :
    // We want to find the non-local control flow out of any of 
    // the regions that is due to a leave.  If we are on a LEAVE
    // and the next MSIL instruction is not a region delimiter in 
    // the little table used to buffer a sorted EIT then clearly this
    // is a jmp out of the middle of some region. 
    // However (there's always one of these) This example indicates 
    // an additional special case we need to check for.
    //
    //   try {
    //   }
    //   catch {                                                  
    //
    //         leave
    //         try {
    //

    // Here the next instruction after the leave is in the marker array
    // but the leave is a non-local control flow. In these edge situations
    // I need to check against the current region node.


    // If the offsets of the current TRY_REGION node contain
    // the next instruction then we may trivially deduce that
    // the leave is out of the region if the currOffset is NOT
    // pointing to the end of the current region. 
    

    // This has been refined to actually look at the destination of the
    // jump.  We did this for a couple of reasons.
    //
    // (1) There can be a leave instruction in the middle of a region which
    //     actually is staying within the region.  This is why the check below
    //     include the checks with currOffset + nDelta... if we are staying within
    //     our region, then this is not a nonLocalGoto!
    // (2) It is possible that we do indeed have a leave as the last instruction
    //     in a region, but this doesn't mean that the leave is actually going
    //     to the canonical place that we expect!  This is why we say currOffset <=
    //     REGION_END_MSIL...   If we are in the case where currOffset == REGION_END
    //     then we'll need to do additional work to see if this is actually not
    //     going to the canonical place!

    currentRegion = FgNodeGetRegion(fg);
    if ( (currentRegion)                                             && 
         (RgnGetRegionType(currentRegion) != ReaderBaseNS::RGN_ROOT) &&
         (RgnGetStartMSILOffset(currentRegion) < leaveOffset)        &&
         (leaveOffset <= RgnGetEndMSILOffset(currentRegion))         &&
         ((leaveTarget < RgnGetStartMSILOffset(currentRegion))       ||
          (leaveTarget >= RgnGetEndMSILOffset(currentRegion)) )
        ) {
        if (leaveOffset == RgnGetEndMSILOffset(currentRegion)) {
            // We need to confirm whether this leave is going to the canonical place!     
            unsigned int nCanonOffset = fgGetRegionCanonicalExitOffset(currentRegion);
            if (leaveTarget == nCanonOffset) {
                // Though this was an explicit goto which is to a nonlocal location,
                // it is canonical and correct... therefore it doesn't need any tracking!
                return false;
            }
            *endsWithNonLocalGoto = true;
        }
        // Record that this region is the source of a non-local goto.
        RgnSetHasNonLocalFlow(currentRegion,true);
        return true;
    }
    return false;
}


//
// fgSplitBlock
//
//  Common block split routine.
//   Splits a block in the flow graph and correctly updates the starting/ending offset
//   fields of the block. Additionally the the function sets the region of the block.
//   Actual flow graph manipulation is performed by GenIR_FgSplitBlock, which is not
//   common.
//
FlowGraphNode*
ReaderBase::fgSplitBlock(
    FlowGraphNode* block,
    unsigned int   currOffset,
    IRNode*        tup
){
    FlowGraphNode* newBlock;
    unsigned int   oldEndOffset;

    // Save off the ending bytes offset so that we can set it on the second block.
    oldEndOffset = FgNodeGetEndMSILOffset(block);

    // Modify the old block info
    FgNodeSetEndMSILOffset(block,currOffset);

    // Split the previous block along the given tuple using GenIR routine.
    newBlock = GenIR_FgSplitBlock( block, tup);

    // Set the correct offsets for the new block.
    FgNodeSetStartMSILOffset(newBlock,currOffset);

    FgNodeSetEndMSILOffset(newBlock,oldEndOffset);

    // Set the EH region
    FgNodeSetRegion(newBlock,fgGetRegionFromMSILOffset(currOffset));

    // Init operand stack to NULL.
    FgNodeSetOperandStack(newBlock, NULL);

    //DBEXEC(ifdb(DB_MSILFG_DUMP), DBPRINT( "Splitting block\n" ));
    //DBEXEC(ifdb(DB_MSILFG_DUMP), THX_prTupFunc(CI_Function(m_genIR->ciPtr), m_genIR->ciPtr));

    // Return the new block
    return newBlock;
}

//
// fgInsertLabel
//
//  Given a label and its offset this function searches the flow graph for
//   the basic block that this label belongs in. It then inserts the label
//   at the appropriate location and modifies the flow graph if necesary.
//
FlowGraphNode*
ReaderBase::fgInsertLabel(
    unsigned int offset,
    IRNode* newLabel,
    FlowGraphNode* startBlock
){
    FlowGraphNode* block, *nextBlock;

    if (startBlock == NULL) {
        startBlock = GenIR_FgGetHeadBlock();
    }

    // Iterate over all blocks until block that contains label offset is found.
    for (block = startBlock;
         block != NULL;
         block = nextBlock) {

        unsigned int start, end;
        
        nextBlock = FgNodeGetNext(block);

        start = FgNodeGetStartMSILOffset(block);
        end = FgNodeGetEndMSILOffset(block);

        if ((offset >= start && offset < end) ||
            (offset == start && offset == end  && !nextBlock)) {

            // Found block that contains new label offset, now find specific insertion
            // point (this is compiler dependent)

            GenIR_InsertLabelInBlock( block, offset, newLabel);

            // Labels must be at the begining of basic blocks. Thus,
            //   if this label does not start the block we just inserted
            //   it into, we must split the block along the label.
            if (offset != start) {
                fgSplitBlock(block,offset,newLabel);
            }

            break;
        }
    }

    return block;
}


int __cdecl labelSortFunction(const void *c1, const void *c2)
{
    unsigned o1, o2;

    o1 = ((LabelOffsetList*)c1)->getOffset();
    o2 = ((LabelOffsetList*)c2)->getOffset();

    if (o1 < o2) return -1;
    return (o1 > o2);
}


// Insert labels from label offset list into block stream, splitting blocks
// if necessary.
// The list is currently ordered.
void
ReaderBase::fgInsertLabels(void) {
    LabelOffsetList *list;
    FlowGraphNode* block;
    unsigned i;

    block = NULL;

    for (i = 0;i < m_labelListArraySize;i++) {
        for(list = m_labelListArray[i]; list != NULL; list = list->getNext()) {
            block = fgInsertLabel(list->getOffset(),list->getLabel(), block);
        }
    }
}


//
// fgInsertBeginRegionExceptionNode
//
//  Given an exception node (that begins a region (OPTRY, OPFILTERBEGIN, etc...))
//   and its offset this function searches the flow graph for the basic block
//   that this node belongs in. It then inserts the node at the appropriate
//   location and modifies the flow graph if necesary.
//  This function serves the secondary purpose of fixing any errors made by 
//   getRegionFromOffset(). The block that contains the node and any other non-EH
//   nodes in the block will have their region info patched up to match the region
//   info of the inserted node.
//
void
ReaderBase::fgInsertBeginRegionExceptionNode(
    unsigned int offset,        // This is the offset where you want tupInsert to be
    IRNode*      startNode      // This is our actual EH end node (OPTRY, etc.)
){
    FlowGraphNode* block;
    
    IRNodeExceptSetMSILOffset(startNode,offset);
    bool found = false;

    // Find the block that this exception node should be placed into
    for (block = GenIR_FgGetHeadBlock();
         block != NULL;
         block = FgNodeGetNext(block))
    {
        unsigned int start = FgNodeGetStartMSILOffset(block);
        unsigned int end   = FgNodeGetEndMSILOffset(block);

        IRNode* insertionPointNode;

        // If the offset is in this range we've found the correct block
        if ( offset >= start && offset < end ) {
            EHRegion* startNodeRegion;
            unsigned int lastOffset;
            bool preceedingNodeIsExceptRegionStart;



            insertionPointNode =
                IRNodeGetInsertPointAfterMSILOffset(FgNodeGetStartInsertIRNode(block),
                                                    offset);
            preceedingNodeIsExceptRegionStart = 
                GenIR_FgEHRegionStartRequiresBlockSplit(insertionPointNode);

            // Insert the EH node here
            IRNodeInsertBefore(insertionPointNode, startNode);

            // We must split the block if the insertionPointNode was already preceeded by an
            // exception node because otherwise the block would contain two adjacent
            // exception nodes. Also split if the offset is not the start of this block.

            // The first node in a block that contains execption nodes can
            //  be one of two things...
            //     (1) An exception node
            //     (2) A label
            // If the exception node does NOT have the same offset as the
            //  start of the block then we need to split the block along
            //  the exception node. If the offset of the exception node
            //  is the same as the starting offset of the block then the
            //  block only needs to be split if the block already begins
            //  with a different exception node.
            if ( (offset != start) || preceedingNodeIsExceptRegionStart ) {
                block = fgSplitBlock(block,offset, startNode);
            }

            // Now set the block to have the same region as EH node that was just
            //  inserted. This corrects the mistake from when two regions begin
            //  with the same offset.
            startNodeRegion = IRNodeGetRegion(startNode);
            lastOffset = RgnGetEndMSILOffset(startNodeRegion);
            GenIR_FgSetBlockToRegion(block,startNodeRegion,lastOffset);

            found = true;
            break;
        }
    }
    ASSERTNR(found);
}


//
// fgInsertEndRegionExceptionNode
//
//  Given an exception node that ends a region (OPTRYBODYEDN, OPCATCHEND, etc...)
//   and its offset this function searches the flow graph for the basic block
//   that this node belongs in. It then inserts the node at the appropriate
//   location and modifies the flow graph if necesary.
//
void
ReaderBase::fgInsertEndRegionExceptionNode(
    unsigned int offset,    // This is the offset where you want tupInsert to be
    IRNode*      insertNode // This is our actual EH end node (OPTRYBODYEND, etc.)
){
    FlowGraphNode *block;

    IRNodeExceptSetMSILOffset(insertNode,offset);

    for (block = GenIR_FgGetHeadBlock();
         block != NULL;
         block = FgNodeGetNext(block))
    {
        unsigned int start = FgNodeGetStartMSILOffset(block);
        unsigned int end   = FgNodeGetEndMSILOffset(block);
        IRNode* insertionPointNode;

        // Please note that we are checking for start < offset <= end !
        // For the beginning marker we were checking for start <= offset < end.
        if (start < offset && offset <= end) {




            insertionPointNode = IRNodeGetInsertPointBeforeMSILOffset(
                FgNodeGetEndInsertIRNode(block),offset);

            // Insert the EH tuple
            GenIR_InsertEHAnnotationNode(insertionPointNode, insertNode);


            if (  (offset != end)
                || GenIR_FgIsExceptRegionStartNode(IRNodeGetNext(insertNode)) )
            {
                block = fgSplitBlock(block, offset, IRNodeGetNext(insertNode));


                FgNodeSetRegion(block,IRNodeGetRegion(IRNodeGetNext(insertNode)));
            }

            // No need to keep processing blocks!
            break;
        }
    }
}


void
ReaderBase::fgEnsureEnclosingRegionBeginsWithLabel(
    IRNode* handlerStartNode
){
    EHRegion *handlerRegion, *tryRegion;
    IRNode  *handlerLabelNode, *exceptTup;

    ASSERTNR(handlerStartNode);

    handlerRegion = IRNodeGetRegion(handlerStartNode);

    ASSERTNR(RgnGetRegionType(handlerRegion) == ReaderBaseNS::RGN_FINALLY ||
             RgnGetRegionType(handlerRegion) == ReaderBaseNS::RGN_FILTER  ||
             RgnGetRegionType(handlerRegion) == ReaderBaseNS::RGN_FAULT   ||
             RgnGetRegionType(handlerRegion) == ReaderBaseNS::RGN_MCATCH  ||
             RgnGetRegionType(handlerRegion) == ReaderBaseNS::RGN_MEXCEPT);

    tryRegion = RgnGetParent(handlerRegion);
    ASSERTNR(RgnGetRegionType(tryRegion)==ReaderBaseNS::RGN_TRY);

    // Adjust region start to any labels that are located before the region
    handlerLabelNode = IRNodeGetFirstLabelOrInstrNodeInEnclosingBlock(handlerStartNode);
    ASSERTNR(handlerLabelNode); // Assert that we found something

    if (handlerLabelNode == NULL) {
        return;
    }
    
    // If this handler doesn't begin with a label make it so. 
    if ( !IRNodeIsLabel(handlerLabelNode) ) {
        exceptTup = handlerLabelNode;
        handlerLabelNode = GenIR_MakeLabel(IRNodeGetMSILOffset(handlerLabelNode), handlerRegion);
        IRNodeInsertBefore( exceptTup, handlerLabelNode );
    }
    //  Mark the label as NBREF otherwise the FG builder might kill it.
    GenIR_MarkAsEHLabel( handlerLabelNode);

    // Point the region head to the label that at the
    //  start of the handler
    RgnSetHead((EHRegion*)handlerRegion,(IRNode*)handlerLabelNode);
}


void
ReaderBase::fgInsertTryEnd(
    EHRegion *region
){
    IRNode *tryEndNode, *endOfClausesNode;

    tryEndNode = GenIR_MakeTryEndNode();
    IRNodeExceptSetMSILOffset(tryEndNode,IRNodeGetMSILOffset(RgnGetLast(region)));
    IRNodeInsertAfter(RgnGetLast(region),tryEndNode);
    IRNodeSetRegion(tryEndNode, RgnGetParent(region));
    RgnSetLast(region,tryEndNode);

    endOfClausesNode = GenIR_FindTryRegionEndOfClauses( region);
    RgnSetEndOfClauses(region,endOfClausesNode);
    GenIR_InsertRegionAnnotation(RgnGetHead(region),RgnGetLast(region));
}


//
// fgInsertEHAnnotations
//
//  - Create region start and end nodes
//  - Insert EH-flow nodes which indicate eh flow arcs.
//
//   This function works recursively to insert all of the EH tuples. 
//   Start at the root of the region tree and moves down. Along its
//   path it first inserts the start-of-region node, then the 
//   end-of-region node for its given region. It then inserts EH-Flow
//   nodes to indicate reachibility for eh funclets and handlers.
//   On the way back up the recursive path the algorithm inserts
//   TRYEND nodes and an EH-Flow edge from the TRY region start to
//   the try region end. This additional flow from try to tryend is
//   necessary for placekeeping, to prevent the reader from deleting
//   the region end node.
//
void
ReaderBase::fgInsertEHAnnotations(
    EHRegion* region
){
    unsigned long offsetStart, offsetEnd;
    IRNode *regionStartNode, *regionEndNode;
    ReaderBaseNS::RegionKind regionType;

    regionType = RgnGetRegionType(region);
    if (regionType != ReaderBaseNS::RGN_ROOT) {

        offsetStart = RgnGetStartMSILOffset(region);
        offsetEnd   = RgnGetEndMSILOffset(region);

        // If verification is turned on, make sure that all the
        //  EIT offsets are acutally instructions
        if (   m_verificationNeeded)
        {
            if (!IsOffsetInstrStart(offsetStart))
                BADCODE(regionType == ReaderBaseNS::RGN_TRY ? MVER_E_TRY_START : MVER_E_HND_START);
            if (offsetEnd != m_methodInfo->ILCodeSize && (!IsOffsetInstrStart(offsetEnd)))
                BADCODE(regionType == ReaderBaseNS::RGN_TRY ? MVER_E_TRY_START : MVER_E_HND_START);
        }

        // Add the region starting marker
        regionStartNode = GenIR_MakeRegionStartNode(regionType);
        RgnSetHead(region,regionStartNode);
        IRNodeSetRegion(regionStartNode, region);

        fgInsertBeginRegionExceptionNode(offsetStart, regionStartNode);

        // Add the region ending marker.
        regionEndNode = GenIR_MakeRegionEndNode(RgnGetRegionType(region));
        RgnSetLast(region, regionEndNode);
        IRNodeSetRegion(regionEndNode, region);
        fgInsertEndRegionExceptionNode(offsetEnd, regionEndNode);

        // Patch the REGION_TRYBODY_END field and REGION_LAST field
        if (regionType == ReaderBaseNS::RGN_TRY) {
            RgnSetTryBodyEnd(region,regionEndNode);
        }
        else if (   (RgnGetRegionType(RgnGetParent(region)) == ReaderBaseNS::RGN_TRY)
                   && ( IRNodeGetMSILOffset(regionEndNode) > 
                        IRNodeGetMSILOffset(RgnGetLast(RgnGetParent(region))))  )
        {
            RgnSetLast(RgnGetParent(region),regionEndNode);
        }

        // Notify GenIR that we've encountered a handler region (so that
        // it might insert flow annotations.)
        switch (regionType) {
        case ReaderBaseNS::RGN_FINALLY:
        case ReaderBaseNS::RGN_FILTER:
        case ReaderBaseNS::RGN_FAULT:
        case ReaderBaseNS::RGN_MCATCH:
        case ReaderBaseNS::RGN_MEXCEPT:
            fgEnsureEnclosingRegionBeginsWithLabel(regionStartNode);

            GenIR_InsertHandlerAnnotation( region);
        default:
            // reached
            break; 
        }

        // GenIR annotation of all EH regions
        GenIR_InsertRegionAnnotation(regionStartNode, regionEndNode);
    }

    EHRegionList* childList;
    for (childList = RgnGetChildList(region); childList != NULL;
         childList = RgnListGetNext(childList)) {
        fgInsertEHAnnotations(RgnListGetRgn(childList));
    }

    if (regionType == ReaderBaseNS::RGN_TRY) {
        // Insert the OPTRYEND tuples based on information gathered above
        fgInsertTryEnd(region);
    }
}


IRNode*
ReaderBase::fgAddCaseToCaseList(
    IRNode* switchNode,
    IRNode* labelNode,
    unsigned element
){
    IRNode* caseNode;

    caseNode = GenIR_FgAddCaseToCaseList( switchNode, labelNode, element);
    GenIR_FgAddLabelToBranchList( labelNode, caseNode);
    return caseNode;
}


IRNode*
ReaderBase::fgMakeBranch(
    IRNode* labelNode,
    IRNode* blockNode,
    unsigned int currOffset,
    bool fIsConditional,
    bool fIsNominal
){
    IRNode* branchNode;

    branchNode = GenIR_FgMakeBranch( labelNode, blockNode, currOffset,
                                    fIsConditional, fIsNominal);
    IRNodeSetRegion(branchNode,fgGetRegionFromMSILOffset(currOffset));
    GenIR_FgAddLabelToBranchList( labelNode, branchNode);
    return branchNode;
}

IRNode*
ReaderBase::fgMakeEndFinally(
    IRNode* blockNode,
    unsigned int currOffset,
    bool fIsLexicalEnd
){
    IRNode* endFinallyNode;

    endFinallyNode = GenIR_FgMakeEndFinally(blockNode, currOffset, fIsLexicalEnd);
    IRNodeSetRegion(endFinallyNode,fgGetRegionFromMSILOffset(currOffset));
    return endFinallyNode;
}


//
// getRegionFromOffset
//
//  Do a linear scan of the EH regions in a function looking for the
//   the smallest possible region that contains the given offset.
//  If two regions start at the same offset then we don't know which
//   one to use. We just use the inner one. This mistake is then
//   corrected when we add in the region start tuples.
//
EHRegion*
ReaderBase::fgGetRegionFromMSILOffset(unsigned int offset)
{
    EHRegionList *regionList;
    EHRegion *rgn, *candidateRegion;
    unsigned int candidateRegionSize;

    candidateRegionSize = UINT_MAX;

    for (regionList = m_allRegionList, candidateRegion = m_ehRegionTree;
         regionList != NULL;
         regionList = RgnListGetNext(regionList)) {
        unsigned startOffset, endOffset;

        rgn = RgnListGetRgn(regionList);

        if ( offset >= (startOffset = RgnGetStartMSILOffset(rgn)) &&
             offset < (endOffset = RgnGetEndMSILOffset(rgn))) {
            unsigned int rgnSize;

            rgnSize = endOffset - startOffset;
            if (rgnSize <= candidateRegionSize ) {
                candidateRegion = rgn;
                candidateRegionSize = rgnSize;
            }
        }
    }

    return candidateRegion;
}

//
//GetMSILInstrStackDelta - Returns the change in the number of items on the
// evaluation stack due to the given instruction.  This is accomplished via
// a lookup table for most operations, with special cases for CEE_CALL, 
// CEE_CALLI, CEE_CALLVIRT, CEE_NEWOBJ, CEE_RET
//
// Note that any pops occur before pushes, so for underflow detection it is
// necessary to have distinct values for pushes and pops.
//
void ReaderBase::GetMSILInstrStackDelta(ReaderBaseNS::OPCODE opcode, BYTE * operand, 
                                        unsigned short * pop, unsigned short * push)
{
    static const char StackPopMap[] = {
#define OPDEF_HELPER OPDEF_POPCOUNT
#include "ophelper.h"
#undef OPDEF_HELPER
    };

    static const char StackPushMap[] = {
#define OPDEF_HELPER OPDEF_PUSHCOUNT
#include "ophelper.h"
#undef OPDEF_HELPER
    };

    int nPop, nPush;

    nPop = 0;

    switch(opcode) {
    case ReaderBaseNS::CEE_CALLI:
        nPop++; // indirect involves an extra stack pop
        // intentional fall-through
    case ReaderBaseNS::CEE_CALL:
    case ReaderBaseNS::CEE_CALLVIRT:
    case ReaderBaseNS::CEE_NEWOBJ:
        {
            CORINFO_METHOD_HANDLE handle;
            CORINFO_SIG_INFO sig;
            bool hasThis, returnsVoid;
            mdToken token;

            token = ReadToken(operand);

            if (verIsCallToken(token) && 
                // if calli - verifier is going to kill this anyway and the
                // site sig lookup is unsafe
                (opcode != ReaderBaseNS::CEE_CALLI || !m_verificationNeeded)
                && m_jitInfo->isValidToken(getCurrentModuleHandle(), token)) 
            {
                if (opcode != ReaderBaseNS::CEE_CALLI)
                    handle = getMethodHandle(token);
                else
                    handle = NULL;

                getCallSiteSignature(handle,token,&sig,&hasThis);
                returnsVoid = (sig.retType == CORINFO_TYPE_VOID);

                nPop += (sig.numArgs + (hasThis ? 1 : 0));
                nPush = (returnsVoid ? 0 : 1);
            } else {
                // "bad token" error will show up later, global verify
                // should not complain.
                nPop  = 0;
                nPush = 0;
            }
        }
        break;
    case ReaderBaseNS::CEE_RET:
        {
            CORINFO_SIG_INFO sig;

            m_jitInfo->getMethodSig(getCurrentMethodHandle(),&sig);
            nPop = ((sig.retType == CORINFO_TYPE_VOID) ? 0 : 1);
            nPush = 0;
        }
        break;
    default:
        nPop = StackPopMap[opcode-ReaderBaseNS::CEE_NOP];
        nPush = StackPushMap[opcode-ReaderBaseNS::CEE_NOP];

        break;
    }
    
    (*pop) = nPop;
    (*push) = nPush;
}




#define CHECKTARGET(targetOffset,bufSize)             \
    { if (targetOffset < 0 || targetOffset >= bufSize) \
          ReaderBase::verGlobalError(MVER_E_BAD_BRANCH); }

// Parse bytecode to blocks.
// Incoming argument 'block' holds dummy entry block. This entry
// block may be preceeded by another block that holds IRNodes (to
// support monitored routines.)
// When this function is finished we have a flow graph with each fg node
// holding the block's start and end MSIL offset, its fg successors and
// predecessors; we also have a list of all labels in the function. These
// labels are inserted in the next pass.

void
ReaderBase::fgBuildPhase1(
    FlowGraphNode* block,
    BYTE* buf,
    unsigned bufSize
){
    IRNode *branchNode, *blockNode, *labelNode, *exitLabel;
    unsigned currOffset, branchOffset, targetOffset, nextOffset, numCases;
    EHRegion* region;
    bool fIsShortInstr, fIsConditional, fTailCall, fConstrained, fPreviousWasPrefix, fIsLexicalEnd;
    unsigned stkOfs = 0;
    ReaderBaseNS::OPCODE opcode = ReaderBaseNS::CEE_ILLEGAL;

    // If we're doing verification build up a bit vector of legal branch targets
    if (m_verificationNeeded)
    {
        // empty IL is a verification error.
        // this is the trivial case of 'do not allow control to flow off the end of a func'
        if (bufSize == 0)
        {
            BADCODE(MVER_E_CODE_SIZE_ZERO);
        }

        m_legalTargetOffsets = (ReaderBitVector *)getTempMemory(sizeof(ReaderBitVector));

        // Add 1 so that there is enough room for the offset after the last instruction
        // (asycronous flow can target this)
        m_legalTargetOffsets->AllocateBitVector(bufSize + 1, this);

        m_gvStackPush = (unsigned short *)getTempMemory(bufSize * sizeof(unsigned short));
        m_gvStackPop  = (unsigned short *)getTempMemory(bufSize * sizeof(unsigned short));
        stkOfs = 0;
    }

    m_labelListArraySize = (bufSize / LABEL_LIST_ARRAY_STRIDE) + 1;
    m_labelListArray = (LabelOffsetList**)getTempMemory(sizeof(LabelOffsetList*) * m_labelListArraySize);

    // init stuff prior to loop
    fIsShortInstr = false;
    fIsConditional = false;
    fTailCall = false;
    fConstrained = false;
    fPreviousWasPrefix = false;

    m_branchesToVerify = NULL;

    m_hasLocAlloc = false;

    nextOffset = currOffset = 0;

#ifdef READER_ESSA
    // For purposes of determining where to place phis, we want the head block
    // to be thought of as defining all parameters.
    // Ideally, we'd like the locals to be seen as being defined in the block
    // started by the entry label.  This should be revisited, but for now, it
    // seems to be adequate.
    // 
    {
        int i;
        for(i=0; i < m_ssaNoParms; i++) {
            Ssa_AddDefPoint(i, -1, false);
        }
        for(i=0; i < m_ssaNoLocals; i++) {
            Ssa_AddDefPoint(m_ssaNoParms+i, -1, true);
        }
    }
#endif

    // Keep going through the buffer of bytecodes until we get to the end.
    while(currOffset < bufSize) {
        unsigned char *operand;

        opcode = ParseMSILOpcodeSafely(buf,currOffset,bufSize,&operand,&nextOffset, this, true);

        // If we're doing verification, build up a bit vector of legal branch targets.
        // note : the instruction following a prefix is not a valid branch target.
        if (m_verificationNeeded && !fPreviousWasPrefix) {
            m_legalTargetOffsets->SetBit(currOffset);
        }

        verInstrStartOffset = currOffset;

        fPreviousWasPrefix = false;

        switch(opcode) {
        case ReaderBaseNS::CEE_BEQ_S:
        case ReaderBaseNS::CEE_BGE_S:
        case ReaderBaseNS::CEE_BGE_UN_S:
        case ReaderBaseNS::CEE_BGT_S:
        case ReaderBaseNS::CEE_BGT_UN_S:
        case ReaderBaseNS::CEE_BLE_S:
        case ReaderBaseNS::CEE_BLE_UN_S:
        case ReaderBaseNS::CEE_BLT_S:
        case ReaderBaseNS::CEE_BLT_UN_S:
        case ReaderBaseNS::CEE_BNE_UN_S:
        case ReaderBaseNS::CEE_BRFALSE_S:
        case ReaderBaseNS::CEE_BRTRUE_S:
        case ReaderBaseNS::CEE_BR_S:
        case ReaderBaseNS::CEE_LEAVE_S:
            fIsShortInstr = TRUE;
            /* Fall Through */
        case ReaderBaseNS::CEE_BEQ:
        case ReaderBaseNS::CEE_BGE:
        case ReaderBaseNS::CEE_BGE_UN:
        case ReaderBaseNS::CEE_BGT:
        case ReaderBaseNS::CEE_BLE:
        case ReaderBaseNS::CEE_BLT:
        case ReaderBaseNS::CEE_BGT_UN:
        case ReaderBaseNS::CEE_BLE_UN:
        case ReaderBaseNS::CEE_BLT_UN:
        case ReaderBaseNS::CEE_BNE_UN:
        case ReaderBaseNS::CEE_BRFALSE:
        case ReaderBaseNS::CEE_BRTRUE:
        case ReaderBaseNS::CEE_BR:
        case ReaderBaseNS::CEE_LEAVE:
            if ( opcode != ReaderBaseNS::CEE_BR && 
                 opcode != ReaderBaseNS::CEE_LEAVE &&
                 opcode != ReaderBaseNS::CEE_BR_S &&
                 opcode != ReaderBaseNS::CEE_LEAVE_S) {
                fIsConditional = true;
            }

            if (fIsShortInstr) {
                branchOffset = ReadInt8(operand);
            } else {
                branchOffset = ReadInt32(operand);
            }

            // Make the label node
            targetOffset = nextOffset + branchOffset;
            CHECKTARGET(targetOffset, bufSize);

            labelNode = NULL;
            fgAddLabelMSILOffset(&labelNode,targetOffset);

            ASSERTNR( labelNode != NULL );
            ASSERTNR( IRNodeIsLabel(labelNode));

            // Make branch node
            blockNode = FgNodeGetStartIRNode(block);
            branchNode = fgMakeBranch(labelNode, blockNode, currOffset, fIsConditional, false);

            // record a branch
            VerifyRecordBranchForVerification(branchNode, currOffset, targetOffset, 
                      opcode == ReaderBaseNS::CEE_LEAVE || opcode == ReaderBaseNS::CEE_LEAVE_S);

            // split the block
            FgNodeSetEndMSILOffset(block,nextOffset);

            block = fgSplitBlock(block, nextOffset, GenIR_FindBlockSplitPointAfterNode(branchNode));

            // Reset flags
            fIsConditional = false;
            fIsShortInstr = false;
            break;

        case ReaderBaseNS::CEE_SWITCH:
            // Get the count of cases.
            numCases = ReadNumberOfSwitchCases(&operand);
                
            // If there are no cases, we can ignore the switch statement
            if (numCases == 0) {
                break;
            }

            // Make the short-circuit target label
            blockNode = FgNodeGetStartIRNode(block);
            labelNode = NULL;
            CHECKTARGET(nextOffset, bufSize);
            fgAddLabelMSILOffset(&labelNode,nextOffset);

            // Make the switch node.
            branchNode = GenIR_FgMakeSwitch(labelNode, blockNode);
            IRNodeBranchSetMSILOffset(branchNode, currOffset);
            IRNodeSetRegion(branchNode, FgNodeGetRegion(block));

            // Create the block to hold the switch node.
            FgNodeSetEndMSILOffset(block,nextOffset);
            block = fgSplitBlock(block, nextOffset, GenIR_FindBlockSplitPointAfterNode(branchNode));

            // Set up labels for each case.
            for (int i = 0; (unsigned)i < numCases; i++ ) {
                branchOffset = ReadSwitchCase(&operand);
                targetOffset = nextOffset + branchOffset;
                CHECKTARGET(targetOffset, bufSize);

                labelNode = NULL;
                fgAddLabelMSILOffset(&labelNode,targetOffset);
                ASSERTNR(labelNode != NULL );
                ASSERTNR(IRNodeIsLabel(labelNode));
                fgAddCaseToCaseList(branchNode,labelNode,i);

                // record a branch
                VerifyRecordBranchForVerification(branchNode, currOffset, targetOffset, false);
            }
            break;

        case ReaderBaseNS::CEE_THROW:
            // throw/rethrow splits a block
            blockNode = FgNodeGetStartIRNode(block);
            branchNode = GenIR_FgMakeThrow(blockNode);
            IRNodeBranchSetMSILOffset(branchNode, currOffset);
            IRNodeSetRegion(branchNode, FgNodeGetRegion(block));

            FgNodeSetEndMSILOffset(block,nextOffset);
            block = fgSplitBlock(block, nextOffset, GenIR_FindBlockSplitPointAfterNode(branchNode));
            break;

        case ReaderBaseNS::CEE_RETHROW:
            // throw/rethrow splits a block
            blockNode = FgNodeGetStartIRNode(block);
            branchNode = GenIR_FgMakeRethrow(blockNode);
            IRNodeBranchSetMSILOffset(branchNode, currOffset);
            IRNodeSetRegion(branchNode, FgNodeGetRegion(block));

            FgNodeSetEndMSILOffset(block,nextOffset);
            block = fgSplitBlock(block, nextOffset, GenIR_FindBlockSplitPointAfterNode(branchNode));
            break;

        case ReaderBaseNS::CEE_ENDFILTER:
            // Do nothing...
            break;

        case ReaderBaseNS::CEE_ENDFINALLY:
            // Treat EndFinally as a a goto to the end of the finally.

            // if this endfinally is not in a finally don't do anything
            // verification will catch it later and insert throw
            region = fgGetRegionFromMSILOffset(currOffset);

            // note endfinally is same instruction as endfault
            if (   region == NULL
                || (   RgnGetRegionType(region) != ReaderBaseNS::RGN_FINALLY
                    && RgnGetRegionType(region) != ReaderBaseNS::RGN_FAULT))
            {
                BADCODE(MVER_E_ENDFINALLY);
            }



            fIsLexicalEnd = (nextOffset == RgnGetEndMSILOffset(region));

            // Make/insert OPENDFINALLY
            blockNode = FgNodeGetStartIRNode(block);
            branchNode = fgMakeEndFinally(blockNode, currOffset, fIsLexicalEnd);

            // And split the block
            FgNodeSetEndMSILOffset(block,nextOffset);
            block = fgSplitBlock(block, nextOffset, GenIR_FindBlockSplitPointAfterNode(branchNode));
            break;

        case ReaderBaseNS::CEE_JMP:
            // The MSIL jmp instruction will cause us to never return to the caller, so any
            //  any code after the jmp is unreachable. Thus we must break the block. In
            //  order to trick the flow graph builder into not placing a fall through
            //  arc here we place a branch tuple to the exit label. Note the fifth argument
            //  to fgMakeBranch is true, which indicates that we want to make a nominal branch.
            // We will rip out the branch when we read in the jmp instruction in the main
            //  reader loop.
            {
                bool fRecursiveTail = false;
                mdToken token = ReadToken(operand);
                if (m_jitInfo->isValidToken(getCurrentModuleHandle(), token)) {
                    fRecursiveTail = GenIR_FgOptRecurse((ReaderBaseNS::CallOpcode)opcodeRemap[opcode], token, false);
#ifdef READER_ESSA
                    if (fRecursiveTail) {
                        for(int i=0; i<m_ssaNoParms; i++)
                            Ssa_AddDefPoint(i, currOffset, false);
                    }
#endif
                }
                blockNode = FgNodeGetStartIRNode(block);
                exitLabel = (fRecursiveTail ? GenIR_EntryLabel() : GenIR_ExitLabel());
                branchNode = fgMakeBranch(exitLabel, blockNode, currOffset, false, !fRecursiveTail);
                FgNodeSetEndMSILOffset(block,nextOffset);
                block = fgSplitBlock(block, nextOffset, GenIR_FindBlockSplitPointAfterNode(branchNode));
            }
            break;

        case ReaderBaseNS::CEE_RET:

            VerifyReturnFlow(currOffset);

            // Insert a goto to the beginning of the epilog.
            blockNode = FgNodeGetStartIRNode(block);
            exitLabel = GenIR_ExitLabel();
            branchNode = fgMakeBranch(exitLabel, blockNode, currOffset, false, false);
            FgNodeSetEndMSILOffset(block,nextOffset);
            block = fgSplitBlock(block, nextOffset, GenIR_FindBlockSplitPointAfterNode(branchNode));
            break;
            
        case ReaderBaseNS::CEE_CASTCLASS:
           {
                mdToken token = ReadToken(operand);
                CorInfoHelpFunc helperId;
                if (rdrCanOptimizeCastClass(token, &helperId))
                    goto OPTIMIZE_CAST;
           }
           break;

        case ReaderBaseNS::CEE_ISINST:
            {
                mdToken token = ReadToken(operand);
                CorInfoHelpFunc helperId;
                if (rdrCanOptimizeIsInst(token, &helperId))
                    goto OPTIMIZE_CAST;
            }
            break;

        OPTIMIZE_CAST:
            // Cast will look like this:
            /*
                cmp objRef, 0
                br(eq) &$L103
                cmp [objRef], class handle
                br(eq) &$L103
                call "cast class"/"is inst"
                L103:
            */
            // In this first pass we simply create the branches and
            //  the target label

            // Create the target label
            GenIR_SetOptimizedCastUsed();
            labelNode = NULL;
            fgAddLabelMSILOffset(&labelNode,nextOffset);

            // Make the branches
            blockNode = FgNodeGetStartIRNode(block);
            branchNode = fgMakeBranch(labelNode, blockNode, currOffset, true, false);
            FgNodeSetEndMSILOffset(block, nextOffset);
            block = fgSplitBlock(block, nextOffset, GenIR_FindBlockSplitPointAfterNode(branchNode));
            FgNodeSetIBCNotReal(block);


            blockNode = FgNodeGetStartIRNode(block);
            branchNode = fgMakeBranch(labelNode, blockNode, nextOffset, true, false);
            FgNodeSetEndMSILOffset(block, nextOffset);
            block = fgSplitBlock(block, nextOffset, GenIR_FindBlockSplitPointAfterNode(branchNode));
            FgNodeSetIBCNotReal(block);
            
            FgNodeSetEndMSILOffset(block, nextOffset);
            block = fgSplitBlock(block, nextOffset, GenIR_FindBlockSplitPointAfterNode(FgNodeGetStartIRNode(block)));
            FgNodeSetIBCNotReal(block);
            break;

        case ReaderBaseNS::CEE_CALL:
        case ReaderBaseNS::CEE_CALLVIRT:
            // Optional client processing of calls. Client will not be called for
            // invalid or unavailable token. Token verification occurs during second
            // reader pass.
            //
            // If there is a RET immediately following a recursive tail call, 
            // it will be unreachable.  Same for a branch-to-return following
            // a tail call.  In either case, the fact that the recursive tail
            // call left nothing on the stack should not be a problem.
            {
                mdToken token = ReadToken(operand);

                bool fIsExplicitTailCall = fTailCall;
                bool fIsRecursiveTailCall = false;
                bool fCanInline = false;

                if (!fIsExplicitTailCall && doTailCallOpt())
                {
                    ASSERTNR(!fTailCall);
                    fTailCall = isUnmarkedTailCall(buf, bufSize, nextOffset, token);
                }

                if (fTailCall)
                {
                    ASSERTNR(!fIsRecursiveTailCall);
                    fIsRecursiveTailCall = GenIR_FgOptRecurse(remapCallOpcode(opcode), token, !fIsExplicitTailCall);
                }

                fCanInline = !fConstrained && !fIsRecursiveTailCall && !fIsExplicitTailCall;

                blockNode = FgNodeGetStartIRNode(block);

                GenIR_FgCall(opcode,token,currOffset,blockNode,fCanInline);

                if (fIsRecursiveTailCall)
                {
                    // insert an edge back to the function entry
#ifdef READER_ESSA
                    for(int i=0; i<m_ssaNoParms; i++)
                        Ssa_AddDefPoint(i, currOffset, false);
#endif
                    exitLabel = GenIR_EntryLabel();
                    branchNode = fgMakeBranch(exitLabel, blockNode, currOffset, false, true);
                    FgNodeSetEndMSILOffset(block,nextOffset);
                    block = fgSplitBlock(block, nextOffset,
                              GenIR_FindBlockSplitPointAfterNode(branchNode));
                }
            }
            break;

        case ReaderBaseNS::CEE_LOCALLOC:
            m_hasLocAlloc = TRUE;
            break;

        case ReaderBaseNS::CEE_CONSTRAINED:
            fConstrained = true;
            fPreviousWasPrefix = true;
            break;
        case ReaderBaseNS::CEE_TAILCALL:
            fTailCall = true;
            fPreviousWasPrefix = true;
            break;
        case ReaderBaseNS::CEE_VOLATILE:
        case ReaderBaseNS::CEE_UNALIGNED:
        case ReaderBaseNS::CEE_READONLY:
            fPreviousWasPrefix = true;
            break;


#ifdef READER_ESSA

        // Simple universe to map all unaliased scalar definitions in MSIL
        // <parameter0 ... parameterN><local0...localN><stack-temp0...stack-tempN>
        // Later these MSIL def points will be converted to def points in a flow
        // graph that is created for the JIT's internal representation

        case ReaderBaseNS::CEE_STARG:
            if (*((unsigned short UNALIGNED *)(operand)) == 0)
                m_thisPtrModified = true;
            Ssa_AddDefPoint(*((unsigned short UNALIGNED *)(operand)), currOffset, false);
            break;

        case ReaderBaseNS::CEE_STARG_S:
            if (*((unsigned char *)(operand)) == 0)
                m_thisPtrModified = true;
            Ssa_AddDefPoint(*((unsigned char *)(operand)), currOffset, false);
            break;

        case ReaderBaseNS::CEE_STLOC:
            Ssa_AddDefPoint(m_ssaNoParms + *((unsigned short UNALIGNED*)(operand)), currOffset, true);
            break;

        case ReaderBaseNS::CEE_STLOC_S:
            Ssa_AddDefPoint(m_ssaNoParms + *((unsigned char *)(operand)), currOffset, true);
            break;

        case ReaderBaseNS::CEE_STLOC_0:
            Ssa_AddDefPoint(m_ssaNoParms + 0, currOffset, true);
            break;

        case ReaderBaseNS::CEE_STLOC_1:
            Ssa_AddDefPoint(m_ssaNoParms + 1, currOffset, true);
            break;             
           
        case ReaderBaseNS::CEE_STLOC_2:
            Ssa_AddDefPoint(m_ssaNoParms + 2, currOffset, true);
            break;             
           
        case ReaderBaseNS::CEE_STLOC_3:
            Ssa_AddDefPoint(m_ssaNoParms + 3, currOffset, true);
            break;

        // Must detect address-taken vars in this pass.
        case ReaderBaseNS::CEE_LDLOCA:
            Ssa_LocAddressTaken(ReadUInt16(operand));
            break;
        case ReaderBaseNS::CEE_LDLOCA_S:
            Ssa_LocAddressTaken(ReadUInt8(operand));
            break;
        case ReaderBaseNS::CEE_LDARGA:
            if (ReadUInt16(operand) == 0)
                m_thisPtrModified = true;
            Ssa_ArgAddressTaken(ReadUInt16(operand));
            break;
        case ReaderBaseNS::CEE_LDARGA_S:
            if (ReadUInt8(operand) == 0)
                m_thisPtrModified = true;
            Ssa_ArgAddressTaken(ReadUInt8(operand));
            break;

#endif
        default:
            // ignore others
            break;
        }

        if (opcode != ReaderBaseNS::CEE_TAILCALL) {
            fTailCall = false;
        }
        if (opcode != ReaderBaseNS::CEE_CONSTRAINED) {
            fConstrained = false;
        }
        
        // Move the byteOffset to the next instruction
        currOffset = nextOffset;

        if (m_verificationNeeded) {    

            // compute and store the stack contributions
            // this is required for global verification

            GetMSILInstrStackDelta(
                opcode,
                operand,
                &m_gvStackPop[stkOfs],
                &m_gvStackPush[stkOfs]
                );
            stkOfs++;
            while (stkOfs < currOffset) {
                m_gvStackPop[stkOfs] = 0;
                m_gvStackPush[stkOfs] = 0;
                stkOfs++;
            }
        }

    }

    
    
    // make sure control didn't flow off the end.
    if (m_verificationNeeded)
    {
        switch (opcode)
        {
        case ReaderBaseNS::CEE_BR_S:
        case ReaderBaseNS::CEE_LEAVE_S:
        case ReaderBaseNS::CEE_ENDFILTER:
        case ReaderBaseNS::CEE_RET:
        case ReaderBaseNS::CEE_JMP:
        case ReaderBaseNS::CEE_THROW:
        case ReaderBaseNS::CEE_RETHROW:
        case ReaderBaseNS::CEE_BR:
        case ReaderBaseNS::CEE_LEAVE:
        case ReaderBaseNS::CEE_ENDFINALLY:
            break;
        default:
            // control cannot flow off the end of the function
            BADCODE(MVER_E_FALLTHRU);
        }

    }

    // Set the last blocks ending offset to where ever we stopped reading
    FgNodeSetEndMSILOffset(block,currOffset);
}

//
// When performing global verification, once the basic blocks have
// been determined we can examine the blocks to determine some values
// that will be used later on for global verification.  The stack
// contributions of each MSIL instruction have already been pre-
// computed in the first pass over the MSIL, but block boundaries
// had not yet been determined at that point.  Take the following
// example block:
//
// MSIL                               pops     pushes
// --------------------------------------------------------------
// add                                 2         1
// ldc                                 0         1
// ldc                                 0         1
// ldc                                 0         1
// call int foo(int,int)               2         1
//
// This routine scans through the contributions of the instructions
// in the block, first accounting for the pops and then for the pushes,
// tracking the net change in the number of items on the stack.  The
// minimum and maximum stack usage are tracked during this process.
//
// The sequence of stack contributions that would be examined for the
// above set of instructions is: -2, +1, 0, +1, 0, +1, 0, +1, -2, +1
//
// In computing the running sum of that sequence, the maximum value
// of the sum at any point will be +2.  The minimum value of the sum
// is -2.  The final value of the sum is +1.
//
// "maxStack" will reflect the maximum additional entries used on
// the stack beyond any entries in use upon entry to the block.
// This number is useful for validating that the actual stack usage
// meets the declared ".maxstack" value.  In this case it is +2.
//
// "minStack", if not 0, will be negative and indicates that the block
// requires values on the stack upon entry to the block.  This number
// is useful for computing underflow as well as another number discussed
// below.  In this case it is -2.
//
// "netStack" will reflect the net stack contribution of the block.
// This is useful for globally detecting underflow/overflow conditions
// as well as computing another number discussed below.  In this case
// it is +1.
//
// "nTOSTemps" is the number of stack-carried temporaries defined in
// the block that are live-out from the block.  minStack reflects
// the number of stack-carried temporaries that were live on entry.
// If there are live-out stack temporaries occupying those same stack
// slots, they must have been defined within the block.  netStack 
// reflects the increase in the number of stack-carried values on exit 
// from the block, so these two numbers together yield the number of
// "Top-Of-Stack" temporaries that are defined locally by this block.
// In the example above, nTOSTemps would be 3 = (1) - (-2).
//
// Note that the same computation works when netStack is negative, as
// minStack will always be <= netStack.  If minStack is less than
// netStack, the difference still reflects the number of items on the
// stack that are live-out and defined within the block.
//
void ReaderBase::fgAttachGlobalVerifyData(
    FlowGraphNode * headBlock,
    int ssaNoParams,
    int ssaNoLocals
){
    int unique;
    // set 'this' uninit if we need to track in a ctor
    // otherwise state is unknown
    int thisInit;

    if (!verNeedsCtorTrack())
    {
        thisInit = true;
    }
    else
    {
        thisInit = THISUNREACHED;
    }

    unique = ssaNoParams + ssaNoLocals;

    for(FlowGraphNode * b = headBlock; b; b = FgNodeGetNext(b)) {
        int min = 0;
        int max = 0;
        int cur;
        int start, end, i, nTOS;
        TOSTEMP * tos;
        GLOBALVERIFYDATA * gvData;

        //
        // compute high/low watermarks for reader stack
        // as well as net stack contribution of the block
        //
        // Note that EH funclet has the exception object
        // already placed on the stack.
        //
        cur = GenIR_IsRegionStartBlock(b) ? 1 : 0;
        min = cur;
        max = cur;
        start = FgNodeGetStartMSILOffset(b);
        end = FgNodeGetEndMSILOffset(b);
        for(i=start; i<end; i++) {
            cur -= m_gvStackPop[i];
            if(cur < min)
                min = cur;
            cur += m_gvStackPush[i];
            if(cur > max)
                max = cur;
        }

        //
        // compute the number of stack temporaries defined
        // by this block and left on stack for subsequent
        // blocks to consume
        //
        nTOS = cur - min;

        //
        // Debug dump for this data
        //

        //DBEXEC(ifdb(DB_GV_DUMP), dbprint("Global Verify Data: block=%p, min=%d, max=%d, net=%d, nTOS=%d\n",b,min,max,cur,nTOS));

        //
        // save the data into the flowgraph
        //

        gvData = (GLOBALVERIFYDATA *)getTempMemory(sizeof(GLOBALVERIFYDATA));

        gvData->minStack = min;
        gvData->maxStack = max;
        gvData->netStack = cur;
        gvData->nTOSTemps = nTOS;
        gvData->stkDepth = -1;
        gvData->tiStack = NULL;
        gvData->isOnWorklist = FALSE;
        gvData->block = b;
        gvData->thisInitialized = thisInit;

        //
        // also save an array of <block, tosIndex> structs
        // for use in SSA, and set up SSA defs for these temps
        //
        tos = (TOSTEMP *)getTempMemory(nTOS*sizeof(TOSTEMP));
        for(i=0; i<nTOS; i++) {
            // TOSINDEX struct setup
            tos[i].block = b;
            tos[i].tosIndex = i;
            tos[i].ssaIndex = unique;

/*
            // SSA def setup
            ssaDefs[ssaInstr] = unique;
            ssaDefOffsets[ssaInstr] = start; // close enough for SSA purposes
                                             // just needs to be in the block
            ssaInstr++;
*/
            unique++;

        }

        FgNodeSetGlobalVerifyData(b,gvData);
    }

/*
    (*ssaInstrOut) = ssaInstr;
*/
}

void ReaderBase::VerifyRecordBranchForVerification(IRNode *branch, unsigned srcOffset, 
                                                   unsigned targetOffset, bool isLeave)
{
    // add this to the list (m_branchesToVerify)
    if (m_verificationNeeded)
    {
        VerificationBranchInfo *branchInfo = (VerificationBranchInfo *) 
            getTempMemory(sizeof(VerificationBranchInfo));

        branchInfo->srcOffset = srcOffset;
        branchInfo->targetOffset = targetOffset;
        branchInfo->branchOp = branch;
        branchInfo->isLeave = isLeave;

        branchInfo->next = m_branchesToVerify;
        m_branchesToVerify = branchInfo;
    }
}


/////////////////////////////////////////////////////////////////////////
// FUNCTION:  void buildBasicBlocksFromBytes(...) - PRIVATE
//  RETURN VALUE: void
//  ARGUMENTS: 3
//      unsigned char *buf: the buffer containing the BYTE codes in MSIL
//      unsigned int count: the length of the buffer in bytes
//      PCI ciptr: the compiler instance
//  DESCRIPTION: This code reads through the bytes codes and builds
//    the correct basic blocks. It operates in three phases
//      PHASE 1: Read BYTE codes and create some basic blocks
//          based on branches and switches. Create the branch
//          tuples for those BYTE codes. Populate the Branch Offset
//          List with the information to generate the labels.
//      PHASE 2: Complete the flow graph by correctly inserting
//          all labels and spliting basic blocks based on those
//          insertions.
//      PHASE 3: Insert EH tuples. This includes region start/end tuples
//          type 1 and 2 OPONERRORs and OPTRY end tuples.
//    When all the phases are complete we should have a tuple
//      stream populated by block tuples, cond/uncond branch tuples, label
//      tuples, and EH tuples.
//
FlowGraphNode*
ReaderBase::fgBuildBasicBlocksFromBytes(
    BYTE *buf,
    unsigned int bufSize
){
    FlowGraphNode *block;

    // Initialize head block to root region.
    block = GenIR_FgGetHeadBlock();
    FgNodeSetRegion(block,fgGetRegionFromMSILOffset(0));

    // PRE-PHASE
    block = GenIR_FgPrePhase(block);

    // initialize SSA
    Ssa_CreateDefMap(&m_ssaNoParms,&m_ssaNoLocals);

    // PHASE 1
    // parse bytecode and construct flow graph
    // gather label information
    fgBuildPhase1(block,buf,bufSize);

    // PHASE 2:
    // insert labels that were gathered into labelOffsetList during phase 1
    // NOTE: The loop is duplicated here so that there is one version for
    //  verification and one for without. This way we don't have to check
    //  if we're verifying during each iteration
    fgInsertLabels();

    GenIR_InsertIBCAnnotations();

    // PHASE 3:
    // verify branch targets BEFORE inserting EH Annotations
    // EH annotations phase is unsafe
    if (m_verificationNeeded)
    {
        VerificationBranchInfo *branchInfo = m_branchesToVerify;
        while (branchInfo)
        {
            verInstrStartOffset = branchInfo->srcOffset;
            VerifyBranchTarget(NULL, IRNodeGetEnclosingBlock(branchInfo->branchOp), 
                               fgGetRegionFromMSILOffset(branchInfo->srcOffset),
                               branchInfo->targetOffset, branchInfo->isLeave);
            branchInfo = branchInfo->next;
        }
    }

    // PHASE 4:
    // GenIR calls to annotate IR stream with EH information
    if (m_ehRegionTree) {
        // The common reader must insert reachibility edges to indicate
        // that filter and handler blocks are reachable.

        // The client is also free to insert his own edges/code annotations.

        // This currents does the whole schmeel for utc, so isn't acceptible
        // for a common client because the interface it requires is too complex.
        fgInsertEHAnnotations(m_ehRegionTree);
    }

    // POST-PHASE - Compiler dependent flow graph cleanup
    GenIR_FgPostPhase();

    if(m_verificationNeeded) {
        // Add annotations to basic blocks to support global verification.
        // This involves analyzing the stack contributions of the various
        // blocks.
        // Has to occur after phase 3 since phase 3 can split blocks.
        block = GenIR_FgGetHeadBlock();
        fgAttachGlobalVerifyData(block,m_ssaNoParms,m_ssaNoLocals);
    }

    return GenIR_FgGetHeadBlock();
}



/******************************************************************************/
// Given information about how to do a runtime lookup, generate the tree
// for the runtime lookup.
// 
// Run-time lookup is required if the enclosing method is shared between instantiations 
// and the token refers to formal type parameters whose instantiation is not known
// at compile-time.
//
// Class Handles and Method Handles must "be restored" in case where the handle is *not*
// being passed to a helper call that will do the restore for us. Field handles
// never need to be restored.
//
IRNode*
ReaderBase::GenIR_GenericTokenToNode(
    mdToken                 token,
    IRNode                **newIR,
    bool                    mustRestoreHandle,
    CORINFO_GENERIC_HANDLE *pStaticHandle,
    bool                   *pRuntimeLookup,
    CorInfoTokenKind        tokenKind /* = CORINFO_TOKENKIND_Default */,
    bool                    needResult
){
    CORINFO_GENERICHANDLE_RESULT result;
    m_jitInfo->embedGenericHandle(getCurrentModuleHandle(),  
                                  token,
                                  getCurrentMethodHandle(), 
                                  tokenKind,
                                  &result);

    unsigned unannotatedToken = token & ~CORINFO_ANNOT_MASK;
    mdToken tokType = TypeFromToken(unannotatedToken);
    BOOL isTypeTok = (tokType == mdtTypeDef ||
                      tokType == mdtTypeRef ||
                      tokType == mdtTypeSpec);

    if (pRuntimeLookup != NULL)
        *pRuntimeLookup = result.lookup.lookupKind.needsRuntimeLookup;

    if (pStaticHandle != NULL)
        *pStaticHandle = result.compileTimeHandle;

    // Only embedGenericHandle understands this flag
    if (isTypeTok)
        token &= ~CORINFO_ANNOT_PERMITUNINSTDEFORREF;

    if (mustRestoreHandle && !result.lookup.lookupKind.needsRuntimeLookup)
    {
        if (isTypeTok)
        {
            m_jitInfo->classMustBeLoadedBeforeCodeIsRun((CORINFO_CLASS_HANDLE) result.compileTimeHandle);
        }  
        else if (tokType == mdtMethodDef || tokType == mdtMemberRef || tokType == mdtMethodSpec)
        {
            m_jitInfo->methodMustBeLoadedBeforeCodeIsRun((CORINFO_METHOD_HANDLE) result.compileTimeHandle);
        }
    }

    // No runtime lookup is required 
    if (!result.lookup.lookupKind.needsRuntimeLookup) {
        if (needResult) {
            if (result.lookup.constLookup.accessType == IAT_VALUE) {
                ASSERTNR(result.lookup.constLookup.handle != NULL);
                return GenIR_HandleToIRNode(unannotatedToken, result.lookup.constLookup.handle, result.compileTimeHandle,
                    false, false, true, false, newIR);
            } else {
                ASSERTNR(result.lookup.constLookup.accessType == IAT_PVALUE);
                return GenIR_HandleToIRNode(unannotatedToken, result.lookup.constLookup.addr, result.compileTimeHandle,
                    true, true, true, false, newIR);
            }
        }
        else
            return NULL;
    }
    else
    {
        return GenIR_RuntimeLookupToNode(result.lookup.lookupKind.runtimeLookupKind, &result.lookup.runtimeLookup, newIR);
    }
}


// Generics: Code sharing
//
// Run-time lookup is required if the enclosing method is shared between instantiations 
// and the token refers to formal type parameters whose instantiation is not known
// at compile-time
//


IRNode*
ReaderBase::GenIR_RuntimeLookupToNode(
    CORINFO_RUNTIME_LOOKUP_KIND kind,
    CORINFO_RUNTIME_LOOKUP  *pLookup,
    IRNode                **newIR
){
    bool fIndirect;
    // It's available only via the run-time helper function
    if (pLookup->indirections == CORINFO_USEHELPER)
    {
        IRNode *arg1,*arg2,*arg3,*arg4,*arg5;

        // It requires the exact method desc argument
        if (kind == CORINFO_LOOKUP_METHODPARAM) {
            arg5 = GenIR_LoadConstantI8(0,newIR);     // Don't care about vtable pointer
            arg4 = GenIR_LoadConstantI8(0,newIR);     // Null slot pointer
            arg3 = GenIR_LoadConstantI4(pLookup->token2,newIR); // Token
            arg2 = GenIR_LoadConstantI4(pLookup->token1,newIR); // Token
            // inst-param
            arg1 = GenIR_InstParam(newIR);
        } else {
            // It requires the vtable pointer
            if (kind == CORINFO_LOOKUP_CLASSPARAM) {
                // use inst-param
                arg5 = GenIR_InstParam(newIR);
            } else {
                // use this ptr
                ASSERTNR(kind == CORINFO_LOOKUP_THISOBJ);
                arg5 = GenIR_DerefAddress(GenIR_ThisObj(newIR),false,newIR);
            }
            arg4 = GenIR_LoadConstantI8(0,newIR);     // Null slot pointer
            arg3 = GenIR_LoadConstantI4(pLookup->token2,newIR); // Token
            arg2 = GenIR_LoadConstantI4(pLookup->token1,newIR); // Token
            CORINFO_METHOD_HANDLE mthdHandle = embedMethodHandle(getCurrentMethodHandle(), &fIndirect);

            arg1 = GenIR_HandleToIRNode(getMethodDefFromMethod(getCurrentMethodHandle()), mthdHandle, 0, fIndirect, fIndirect, true, false, newIR);
        }

        return GenIR_CallRuntimeHandleHelper(pLookup->helper, arg1,arg2,arg3,arg4,arg5,NULL,newIR);
    }

    IRNode *vtNode, *mdNode, *slotPtrNode, *slotPtrNodeCopy;

    // Use the method descriptor that was passed in to get at instantiation info
    if (kind == CORINFO_LOOKUP_METHODPARAM) {
        vtNode = GenIR_LoadConstantI8(0,newIR);
        mdNode = GenIR_InstParam(newIR);
        GenIR_Dup(mdNode,&slotPtrNode,&mdNode,newIR);
    } else {
        if (kind == CORINFO_LOOKUP_CLASSPARAM) {
            // Use the vtable pointer that was passed in
            vtNode = GenIR_InstParam(newIR);
        } else {
            // Use the vtable of "this" to get at instantiation info
            ASSERTNR(kind == CORINFO_LOOKUP_THISOBJ);
            vtNode = GenIR_DerefAddress(GenIR_ThisObj(newIR),false,newIR);
        }
        GenIR_Dup(vtNode,&slotPtrNode,&vtNode,newIR);
        // This could end up creating a dead-store, so only create mdNode
        //  if it is going to be used.
        if (pLookup->testForNull) {
            CORINFO_METHOD_HANDLE mthdHandle = embedMethodHandle(getCurrentMethodHandle(), &fIndirect);
            mdNode = GenIR_HandleToIRNode(getMethodDefFromMethod(getCurrentMethodHandle()), mthdHandle, 0, fIndirect, fIndirect, true, false, newIR);
        }
    }

    // Use the vtable of "this" to get at instantiation info
    
    // Apply repeated indirections
    for (WORD i = 0; i < pLookup->indirections; i++)
    {
        if (i != 0) {
            slotPtrNode = GenIR_DerefAddress(slotPtrNode,false,newIR);
        }
        if (pLookup->offsets[i] != 0) {
            slotPtrNode = GenIR_BinaryOp(ReaderBaseNS::ADD,slotPtrNode, 
                              GenIR_LoadConstantI(pLookup->offsets[i], newIR), newIR);
        }
    }

    // No null test required
    if (!pLookup->testForNull)
    {
        if (pLookup->indirections != 0)
            slotPtrNode = GenIR_DerefAddress(slotPtrNode,false,newIR);
        return slotPtrNode;
    }

    ASSERTNR(pLookup->indirections != 0);
    
    // Extract the type handle
    GenIR_Dup(slotPtrNode,&slotPtrNodeCopy,&slotPtrNode,newIR);
    IRNode *handleNode = GenIR_DerefAddress(slotPtrNodeCopy,false,newIR);
    IRNode *tokNode2 = GenIR_LoadConstantI4(pLookup->token2,newIR);
    IRNode *tokNode1 = GenIR_LoadConstantI4(pLookup->token1,newIR);

    // Call helper on null
    return GenIR_CallRuntimeHandleHelper(pLookup->helper, mdNode,tokNode1,tokNode2,slotPtrNode,vtNode,handleNode,newIR);
}

CorInfoHelpFunc
ReaderBase::getNewArrHelper(
    CORINFO_CLASS_HANDLE hndElementType)
{
    return m_jitInfo->getNewArrHelper(hndElementType, getCurrentMethodHandle()); 
}

// GenIR_InitBlk - Creates an IV_MEMCPY intrinsic.
void
ReaderBase::GenIR_CpBlk(
    IRNode*         count,        // BYTE count
    IRNode*         srcAddr,   // source address
    IRNode*         destAddr,     // dest address
    ReaderAlignType alignment,
    bool            fVolatile,
    IRNode**        newIR
){
    GenIR_CallHelper(CORINFO_HELP_MEMCPY,NULL,newIR,destAddr,srcAddr,count,alignment,fVolatile);
}

// GenIR_InitBlk - Creates an IV_MEMSET intrinsic.
void
ReaderBase::GenIR_InitBlk(
    IRNode*         count,      // BYTE count
    IRNode*         value,      // value
    IRNode*         destAddr,   // dest address
    ReaderAlignType alignment,
    bool            fVolatile,
    IRNode**        newIR
){
    GenIR_CallHelper(CORINFO_HELP_MEMSET,NULL,newIR,destAddr,value,count,alignment,fVolatile);
}

void
ReaderBase::GenIR_InitObj(
    mdToken         token,
    IRNode         *objectAddr,
    IRNode        **newIR)
{
    // GENERICS NOTE: 
    //    This routine can take in a reference, in which case we need to 
    //    zero-init it as well.  The FJIT adds special-purpose LDNULL, 
    //    STIND_REF combo.  Unless there is an extra level of indirection 
    //    that I'm missing out on, we might as well just leave it like
    //    this.  If we find, while expanding the intrinsic, that it refers
    //    to some rediculously small amount of memory, we could boil it
    //    down to an OPASSIGN or two there and catch more cases.  Since
    //    we should probably do that there regardless, doing it here too
    //    would be redundant.

    unsigned size = getClassSize(getClassHandle(token));
    IRNode *sizeNode = GenIR_LoadConstantI4(size,newIR);
    IRNode *valueNode = GenIR_LoadConstantI4(0,newIR);

    // Use init blk to initialzie the object
    GenIR_InitBlk(sizeNode,valueNode,objectAddr,READER_ALIGN_UNKNOWN,false,newIR);
}

// GenIR_Box - Default reader processing for CEE_BOX.
IRNode*
ReaderBase::GenIR_Box(
    mdToken token,
    IRNode* arg2,
    IRNode** newIR
){
    IRNode *dst, *arg1;
    CORINFO_CLASS_HANDLE clsHnd = getClassHandle(token);

    if(getClassAttribs(clsHnd) & CORINFO_FLG_VALUECLASS) {      
        // Ensure that operand from operand stack has type that is compatible with
        // box destination, then get the (possibly converted) operand's address.
        arg2 = GenIR_ConvertToHelperArgumentType(arg2, getClassSize(getClassHandle(token)),newIR);

        // Use address of value here, this helps to prevent user-syms from being aliased
        // and thrown out of ssa.
        arg2 = GenIR_AddressOfValue(arg2, newIR);

        // Pointer operand to hold result of call to box.
        dst = GenIR_MakePtrDstGCOperand(false);

        // The first arg for the helper is the class handle. Derive
        // it from the token.
        arg1 = GenIR_GenericTokenToNode(token, newIR, true);

        dst = GenIR_CallHelper(getBoxHelper(clsHnd), dst, newIR, arg1, arg2);

        return dst;
    } else {
        // GENERICS NOTE:
        //   The only major change here is that we allow box of reference types.
        //   box of reference type -> NOP
        return arg2;
    }

};

// This tells the reader base when it can optimize cast class. It is called
//  twice, once in the first pass, and once in the second. It *must* return
//  the same result both times or chaos will ensue.
//
// Optimization is disabled when
//    - verification is turned on
//    - the client disables the optimization
//    - a helper call other then CORINFO_HELP_CHKCASTCLASS is needed
//
// RETURN VALUE: Returns true if the opimization should take place
// OUTPARAM: If the function was required to get the helper id in
//  order to determain whether or not to optimize the helper id
//  is returned via the outparam. In all other cases the the
//  outparam is set to CORINFO_HELP_UNDEF. This is done in order
//  to minimize the number of calls into the runtime.
//  
bool
ReaderBase::rdrCanOptimizeCastClass(
    mdToken token, CorInfoHelpFunc *pHelperId)
{
    if (m_verificationNeeded || GenIR_DisableCastClassOptimization() ) {
        *pHelperId = CORINFO_HELP_UNDEF;
        return false;
    }

    *pHelperId = m_jitInfo->getChkCastHelper(m_methodInfo->scope, token, m_methodInfo->ftn);
    if (*pHelperId == CORINFO_HELP_CHKCASTCLASS)
    {
        *pHelperId = CORINFO_HELP_CHKCASTCLASS_SPECIAL;
        return true;
    }

    return false;
}

// This tells the reader base when it can isisnt. It is called
//  twice, once in the first pass, and once in the second. It
//  *must* return the same result both times or chaos will ensue.
//
// Optimization is disabled when
//    - verification is turned on
//    - the client disables the optimization
//    - a helper call other then CORINFO_HELP_ISINSTANCEOFCLASS is needed
//
// RETURN VALUE: Returns true if the opimization should take place
// OUTPARAM: If the function was required to get the helper id in
//  order to determain whether or not to optimize the helper id
//  is returned via the outparam. In all other cases the the
//  outparam is set to CORINFO_HELP_UNDEF. This is done in order
//  to minimize the number of calls into the runtime.
//  
bool
ReaderBase::rdrCanOptimizeIsInst(
    mdToken token, CorInfoHelpFunc *pHelperId)
{
    if (m_verificationNeeded || GenIR_DisableIsInstOptimization() ) {
        *pHelperId = CORINFO_HELP_UNDEF;
        return false;
    }

    *pHelperId = m_jitInfo->getChkCastHelper(m_methodInfo->scope, token, m_methodInfo->ftn);
    if (*pHelperId == CORINFO_HELP_ISINSTANCEOFCLASS)
    {
        *pHelperId = CORINFO_HELP_ISINSTANCEOFCLASS;
        return true;
    }

    return false;
}


// GenIR_OptimizedCast - optimized reader processing of CEE_CASTCLASS/CEE_ISINST
//  the stack should be cleared and forwarded to successor blocks before this
//  function is called.
// The return value is void because the return is *not* pushed onto the stack.
//  Instead the return value of the helper call is simply assigned to the
//  correct slot on the stack.

// Cast class/IsInst will look like this:
/*
        cmp objRef, 0
        br(eq) &$L103
        cmp [objRef], class handle
        br(eq) &$L103
        call "cast class"/"is inst"
        L103:
*/
// The branches were inserted as TBD by the first pass of the reader
//  to proceed we simply locate the branches and fill them in.

void
ReaderBase::GenIR_OptimizedCast(
    mdToken              token,
    CorInfoHelpFunc      helperId,
    IRNode              **newIR
){
    ReaderBaseNS::CondBranchOpcode ceqCondCode = ReaderBaseNS::BEQ;
    IRNode *dst, *methodTableNode;

    FlowGraphNode *classTestBlock = FgNodeGetNext(m_currentFgNode);
    FlowGraphNode *classCastBlock = FgNodeGetNext(classTestBlock);

    // First thing is load up the operand stack -- it was cleared
    //  before this was called. Then use this operand stack
    //  to find the value that needs to be caste
    ReaderStack *stack = FgNodeGetOperandStack(classTestBlock);
    ReaderStackIterator *iterator;
    IRNode *objRefNode = stack->getIterator(&iterator);

    // The class handle value must be live across several basic blocks (UTC: This removes SDSU)
    IRNode *classHandleNode = GenIR_GenericTokenToNode(token, 
                                                       newIR,
                                                       false,                    // mustRestoreHandle
                                                       NULL,                     // pStaticHandle 
                                                       NULL,                     // pRuntimeLookup 
                                                       CORINFO_TOKENKIND_Casting // tokenKind
                                                      );
    IRNode *classHandleNodeCopy = GenIR_MakeStackTypeNode(classHandleNode);
    GenIR_AssignToSuccessorStackNode(NULL,classHandleNodeCopy,classHandleNode,newIR,NULL);
    classHandleNode = classHandleNodeCopy;

    // Insert the test against zero
    ASSERTNR(IRNodeIsBranch(*newIR));
    GenIR_CondBranch(ceqCondCode, objRefNode, GenIR_LoadConstantI(0,newIR), newIR);
    
    // Insert the test against the class handle
    *newIR = FgNodeGetEndIRInsertionPoint(classTestBlock);
    ASSERTNR(IRNodeIsBranch(*newIR));
    methodTableNode = GenIR_LoadIndir(ReaderBaseNS::LDIND_I,objRefNode,READER_ALIGN_NATURAL,false,false,newIR);
    GenIR_CondBranch(ceqCondCode, methodTableNode, classHandleNode, newIR);
    
    // Forward the insertion point and insert the helper call
    *newIR = FgNodeGetEndIRInsertionPoint(classCastBlock);
    dst = GenIR_MakePtrDstGCOperand(false);
    dst = GenIR_CallHelper(helperId, dst, newIR, classHandleNode, objRefNode);

    // Assign the return value from the call back to the objRefNode
    // Don't push anything onto the stack, don't adjust newIR. This the end of a 
    //  basic block and the stack has already been forwarded.
    GenIR_AssignToSuccessorStackNode(NULL,objRefNode,dst,newIR,NULL);
}

void
ReaderBase::GenIR_CastClassOptimized(
    mdToken              token,
    IRNode              **newIR
){
    GenIR_OptimizedCast(token,CORINFO_HELP_CHKCASTCLASS_SPECIAL,newIR);
}

void
ReaderBase::GenIR_IsInstOptimized(
    mdToken              token,
    IRNode              **newIR
){
    GenIR_OptimizedCast(token,CORINFO_HELP_ISINSTANCEOFCLASS,newIR);
}

// GenIR_CastClass - Generate a simple helper call for the cast class.
IRNode*
ReaderBase::GenIR_CastClass(
    mdToken              token,
    IRNode              *objRefNode,
    IRNode              **newIR,
    CorInfoHelpFunc      helperId /* DEFAULT: CORINFO_HELP_UNDEF, means do a look up */
){
    if (helperId==CORINFO_HELP_UNDEF) {
        helperId = m_jitInfo->getChkCastHelper(m_methodInfo->scope, token, m_methodInfo->ftn);
    }

    // Generate the helper call
    IRNode *classHandleNode = GenIR_GenericTokenToNode(token, 
                                                       newIR,
                                                       false,                    // mustRestoreHandle
                                                       NULL,                     // pStaticHandle 
                                                       NULL,                     // pRuntimeLookup 
                                                       CORINFO_TOKENKIND_Casting // tokenKind
                                                      );
    IRNode *dst = GenIR_MakePtrDstGCOperand(false);
    return GenIR_CallHelper(helperId, dst, newIR, classHandleNode, objRefNode);

}

// GenIR_IsInst - Default reader processing of CEE_ISINST.
IRNode*
ReaderBase::GenIR_IsInst(
    mdToken                token,
    IRNode                *objRefNode,
    IRNode               **newIR,
    CorInfoHelpFunc        helperId /* DEFAULT: CORINFO_HELP_UNDEF, means do a look up */
){
    if (helperId==CORINFO_HELP_UNDEF) {
        helperId = m_jitInfo->getIsInstanceOfHelper(m_methodInfo->scope, token, m_methodInfo->ftn);
    }

    // Generate the helper call
    IRNode *classHandleNode = GenIR_GenericTokenToNode(token, 
                                                       newIR,
                                                       false,                    // mustRestoreHandle
                                                       NULL,                     // pStaticHandle 
                                                       NULL,                     // pRuntimeLookup 
                                                       CORINFO_TOKENKIND_Casting // tokenKind
                                                      );
    IRNode *dst = GenIR_MakePtrDstGCOperand(false);
    return GenIR_CallHelper(helperId, dst, newIR, classHandleNode, objRefNode);
}


IRNode*
ReaderBase::GenIR_LoadStr(
    mdToken        token,
    IRNode**       newIR
){
    void *stringHandle;
    InfoAccessType iat = constructStringLiteral(token, &stringHandle);
    ASSERTNR(stringHandle != NULL);

    IRNode *stringPtrNode = NULL;
    switch(iat)
    {
        case IAT_PVALUE:
        case IAT_PPVALUE:
            // Final indirection needs to generate a GC pointer so use LDIND_REF
            stringPtrNode = GenIR_HandleToIRNode(token,stringHandle,0,(iat==IAT_PPVALUE),true,true,false,newIR);
            stringPtrNode = GenIR_LoadIndir(ReaderBaseNS::LDIND_REF, stringPtrNode,
                                            READER_ALIGN_NATURAL, false, false, newIR);
            break;
        default:
            ASSERTNR(UNREACHED);
    }
    return stringPtrNode;
}


IRNode*
ReaderBase::GenIR_RefAnyVal(
    IRNode*        refany,
    mdToken        token,
    IRNode**       newIR
){
    IRNode *dst, *arg1;

    // first argument is class handle
    arg1 = GenIR_GenericTokenToNode(token, newIR);

    // Create dst operand, interior gc ptr
    dst = GenIR_MakePtrDstGCOperand(true);

    // Make the helper call
    return GenIR_CallHelper(CORINFO_HELP_GETREFANY, dst, newIR, arg1, refany);
}

void
ReaderBase::GenIR_StoreElemRefAny(
    IRNode* ptValue,
    IRNode* ptIndex,
    IRNode* ptObj,
    IRNode** newIR
){
    // Make the helper call
    GenIR_CallHelper(CORINFO_HELP_ARRADDR_ST, NULL, newIR, ptObj, ptIndex, ptValue);
}

// GenIR_StoreIndir - Creates an instruction tuple to assign the value on top
// of the stack into the memory pointed at by the operand which is 2nd from
// the top of the stack.
void
ReaderBase::GenIR_StoreIndir(
    ReaderBaseNS::StIndirOpcode opcode,
    IRNode* value,
    IRNode* address,
    ReaderAlignType alignment,
    bool fVolatile,
    IRNode** newIR
){
    static const CorInfoType map[ReaderBaseNS::LAST_STIND_OPCODE] = {
        CORINFO_TYPE_BYTE,       // STIND_I1
        CORINFO_TYPE_SHORT,      // STIND_I2
        CORINFO_TYPE_INT,        // STIND_I4
        CORINFO_TYPE_LONG,       // STIND_I8
        CORINFO_TYPE_NATIVEINT,  // STIND_I
        CORINFO_TYPE_FLOAT,      // STIND_R4
        CORINFO_TYPE_DOUBLE,     // STIND_R8
        CORINFO_TYPE_REFANY      // STIND_REF
    };

    ASSERTNR(opcode >= ReaderBaseNS::STIND_I1 &&
             opcode <  ReaderBaseNS::LAST_STIND_OPCODE);
    CorInfoType corInfoType = map[opcode];

    if (corInfoType == CORINFO_TYPE_REFANY) {
        // STIND_REF requires that our type simply be a managed pointer.
        // Pass in null for CLASS_HANDLE and TOKEN.
        rdrCallWriteBarrierHelper(address,value,alignment,fVolatile,newIR,mdTokenNil,true,false,false);
    } else {
        GenIR_StorePrimitiveType(value,address,corInfoType,
                                  alignment,fVolatile,newIR);
    }
}

// GenIR_LoadIndir - 
IRNode *
ReaderBase::GenIR_LoadIndir(
    ReaderBaseNS::LdIndirOpcode opcode,
    IRNode* address,
    ReaderAlignType alignment,
    bool fVolatile,
    bool fInterfReadOnly,
    IRNode** newIR
){
    static const CorInfoType map[ReaderBaseNS::LAST_LDIND_OPCODE] = {
        CORINFO_TYPE_BYTE,       // STIND_I1
        CORINFO_TYPE_UBYTE,      // STIND_U1
        CORINFO_TYPE_SHORT,      // STIND_I2
        CORINFO_TYPE_USHORT,     // STIND_U2
        CORINFO_TYPE_INT,        // STIND_I4
        CORINFO_TYPE_UINT,       // STIND_U4
        CORINFO_TYPE_LONG,       // STIND_I8
        CORINFO_TYPE_NATIVEINT,  // STIND_I
        CORINFO_TYPE_FLOAT,      // STIND_R4
        CORINFO_TYPE_DOUBLE,     // STIND_R8
        CORINFO_TYPE_REFANY      // STIND_REF
    };

    ASSERTNR(opcode >= ReaderBaseNS::LDIND_I1 &&
             opcode <  ReaderBaseNS::LAST_LDIND_OPCODE);
    CorInfoType corInfoType = map[opcode];

    return GenIR_LoadPrimitiveType(address,corInfoType,alignment,fVolatile,fInterfReadOnly,newIR);
}

// GenIR_StoreObj - default reader processing of CEE_STOBJ
void
ReaderBase::GenIR_StoreObj(
    mdToken         token,
    IRNode*         value,
    IRNode*         addr,
    ReaderAlignType alignment,
    bool            fVolatile,
    bool            fField,
    IRNode**        newIR
){
    CORINFO_CLASS_HANDLE classHandle;
    CorInfoType corInfoType;

    if (fField) {
        corInfoType = getFieldType(getFieldHandle(token),&classHandle);
        ASSERTNR(corInfoType == CORINFO_TYPE_VALUECLASS);
    } else {
        classHandle = getClassHandle(token);
        corInfoType = getClassType(classHandle);
    }

    if (!(getClassAttribs(classHandle) & CORINFO_FLG_VALUECLASS)) {
        GenIR_StoreIndir(ReaderBaseNS::STIND_REF,value,addr,alignment,fVolatile,newIR);
    } else if (isPrimitiveType(classHandle)) {
        GenIR_StorePrimitiveType(value,addr,corInfoType,alignment,fVolatile,newIR);
    } else {
        // Get the minimum alignment for the class
        alignment = getMinimumClassAlignment(classHandle, alignment);
        rdrCallWriteBarrierHelper(addr,value,alignment,fVolatile,newIR,token,false,false,fField);
    }
}

// GenIR_LoadObj - default reader processing of CEE_LDOBJ
IRNode*
ReaderBase::GenIR_LoadObj(
    mdToken         token,
    IRNode*         addr,
    ReaderAlignType alignment,
    bool            fVolatile,
    bool            fField,
    IRNode**        newIR
){
    CORINFO_CLASS_HANDLE classHandle;
    CorInfoType corInfoType;

    if (fField) {
        corInfoType = getFieldType(getFieldHandle(token),&classHandle);
        ASSERTNR(corInfoType == CORINFO_TYPE_VALUECLASS);
    } else {
        classHandle = getClassHandle(token);
        corInfoType = getClassType(classHandle);
    }

    if (!(getClassAttribs(classHandle) & CORINFO_FLG_VALUECLASS)) {
        return GenIR_LoadIndir(ReaderBaseNS::LDIND_REF,addr,alignment,fVolatile,false,newIR);
    } else if (isPrimitiveType(corInfoType)) {
        return GenIR_LoadPrimitiveType(addr,corInfoType,alignment,fVolatile,false,newIR);
    } else {
        return GenIR_LoadNonPrimitiveObj(addr,classHandle,alignment,fVolatile,newIR);
    }
}

// GenIR_LoadAndBox - default reader processing of constrained calls on valuetypes
// that must be boxed
IRNode*
ReaderBase::GenIR_LoadAndBox(
    mdToken         token,
    IRNode*         addr,
    ReaderAlignType alignment,
    IRNode**        newIR
){
    addr = GenIR_LoadObj(token, addr, alignment, false, false, newIR);
    return GenIR_Box(token, addr,newIR);
}


// GenIR_CpObj - default reader processing of CEE_CPOBJ
void
ReaderBase::GenIR_CpObj(
    mdToken         token,
    IRNode         *ptSrc,
    IRNode         *ptDst,
    ReaderAlignType alignment,
    bool            fVolatile,
    IRNode        **newIR
){
    IRNode *value = GenIR_LoadObj(token, ptSrc, alignment, fVolatile, false, newIR);
    GenIR_StoreObj(token, value, ptDst, alignment, fVolatile, false, newIR);
}

// GenIR_Unbox - Default reader processing for CEE_UNBOX.
IRNode*
ReaderBase::GenIR_Unbox(
    mdToken token,
    IRNode* arg2,
    IRNode** newIR
){
    // Pointer operand to hold result of call to box.
    IRNode *dst = GenIR_MakePtrDstGCOperand(true);

    // The first arg for the helper is the class handle. Derive
    // it from the token.
    IRNode *arg1 = GenIR_GenericTokenToNode(token, newIR);

    // Make the helper call
    return GenIR_CallHelper(getUnBoxHelper(getClassHandle(token), NULL), dst, newIR, arg1, arg2);
};


// GenIR_UnboxAny - Default reader processing for CEE_UNBOXANY.
IRNode*
ReaderBase::GenIR_UnboxAny(
    mdToken token,
    IRNode* arg,
    ReaderAlignType alignmentPrefix,
    bool fVolatilePrefix,
    IRNode** newIR
){
    // GENERICS NOTE: New Instruction
    //   Value Types: unbox followed by ldobj
    //   Reference Types: castclass
    if(getClassAttribs(getClassHandle(token)) & CORINFO_FLG_VALUECLASS) {
        IRNode * unboxed = GenIR_Unbox(token, arg, newIR);
        return GenIR_LoadObj(token, unboxed, alignmentPrefix, fVolatilePrefix, false, newIR);
    } else {
        return GenIR_CastClass(token, arg, newIR);
    }
}



// GenIR_Break - Default reader processing for CEE_BREAK.
void
ReaderBase::GenIR_Break(
    IRNode** newIR
){
    // Make the helper call
    GenIR_CallHelper(CORINFO_HELP_USER_BREAKPOINT, NULL, newIR);
}



// GenIR_InsertClassConstructor - Insert a call to the class constructor helper.
void
ReaderBase::GenIR_InsertClassConstructor(IRNode **newIR)
{
    CORINFO_CLASS_HANDLE classHandle = getCurrentMethodClass();
    CORINFO_METHOD_HANDLE methodHandle = getCurrentMethodHandle();
    DWORD methodAttribs = getMethodAttribs(methodHandle);
    mdToken methodToken = mdTokenNil;
    mdToken classToken = mdTokenNil;
    mdToken moduleToken = mdTokenNil;
    IRNode *methodNode, *classNode;
    bool fIndirect;
    bool fIndirect2;

#if !defined(NODEBUG)
    // We only need this for hashing the symbol, which is only
    //  really needed in a debug build.
    methodToken = getMethodDefFromMethod(methodHandle);
    classToken = getMemberParent(methodToken);
    moduleToken = getMemberParent(classToken);
#endif

    ASSERTNR(methodAttribs & CORINFO_FLG_RUN_CCTOR);

    // Generics: Code sharing
    // Special care must be taken when inserting constructors for shared classes
    CORINFO_LOOKUP_KIND kind = m_jitInfo->getLocationOfThisType(methodHandle);
    
    if (kind.needsRuntimeLookup)
    {
        switch(kind.runtimeLookupKind)
        {
        case CORINFO_LOOKUP_THISOBJ:
            // call CORINFO_HELP_INITINSTCLASS(thisobj, embedMethodHandle(M))
            methodHandle = embedMethodHandle(methodHandle, &fIndirect);
            methodNode = GenIR_HandleToIRNode(methodToken, methodHandle,0, fIndirect, fIndirect, true, false, newIR);
            classNode = GenIR_DerefAddress(GenIR_ThisObj(newIR),false,newIR);
            GenIR_CallHelper(CORINFO_HELP_INITINSTCLASS, NULL, newIR, classNode, methodNode);
            return;
        case CORINFO_LOOKUP_CLASSPARAM:
            // will only be returned when you are
            // compiling code that takes a hidden parameter P.
            // You should emit a call CORINFO_HELP_INITCLASS(P)
            classNode = GenIR_InstParam(newIR);
            GenIR_CallHelper(CORINFO_HELP_INITCLASS, NULL, newIR, classNode);
            return;
        case CORINFO_LOOKUP_METHODPARAM:
            // will only be returned when you are
            // compiling code that takes a hidden parameter P.
            // You should emit a call CORINFO_HELP_INITINSTCLASS(NULL, P)
            methodNode = GenIR_InstParam(newIR);
            classNode = GenIR_LoadConstantI8(0,newIR);
            GenIR_CallHelper(CORINFO_HELP_INITINSTCLASS, NULL, newIR, classNode, methodNode);
            return;
        default:
            ASSERTNR(!"NYI");
        }
    }
    else
    {
        // Use the shared static base helper as it is faster than InitClass
        CorInfoHelpFunc  helperId   = getSharedCCtorHelper(classHandle);

        if (helperId == CORINFO_HELP_GETGENERICS_NONGCSTATIC_BASE)
        {
            void *embHandle = embedClassHandle(classHandle, &fIndirect);

            classNode   = GenIR_HandleToIRNode(classToken, embHandle, classHandle, fIndirect, fIndirect, true, false, newIR);

            GenIR_CallHelper(helperId, NULL, newIR, classNode);
        }
        else
        {
            // Just use non-generics way of doing things.

            void *  embedModuleDomainID = getEmbedModuleDomainID(getClassModuleForStatics(classHandle),&fIndirect); 
            void *  embedClassDomainID  = getEmbedClassDomainID(classHandle,&fIndirect2);
        
            IRNode * tupArg = GenIR_HandleToIRNode(moduleToken, embedModuleDomainID, 0, fIndirect, fIndirect, fIndirect, false, newIR);
            IRNode * tupArg2 = GenIR_HandleToIRNode(classToken, embedClassDomainID, classHandle, fIndirect2, fIndirect2, fIndirect2, false, newIR);
        
            GenIR_CallHelper(helperId, NULL, newIR, tupArg, tupArg2);
        }
    }
}

// rdrGetCritSect
//  In order to get the correct lock for static generic methods we need
//  to call a helper. This method deals with this runtime quirk.
//  The method also safely handles non-static methods and non-generic
//  static methods. The return value of the method is an IRNode that
//  can be passed to any of the locking methods.
IRNode *
ReaderBase::rdrGetCritSect(IRNode **newIR)
{
    // For non-static methods, simply use the "This" pointer
    if((getCurrentMethodAttribs() & CORINFO_FLG_STATIC) == 0) {
        return GenIR_ThisObj(newIR);
    }
    
    IRNode *handleNode = NULL;
    
    CORINFO_LOOKUP_KIND kind = m_jitInfo->getLocationOfThisType(getCurrentMethodHandle());
                
    if (!kind.needsRuntimeLookup)
    {
        bool fIndirect = false;
        void * critSect = getMethodSync(&fIndirect);
        handleNode = GenIR_HandleToIRNode(mdtSyncHandle, critSect, NULL, fIndirect, fIndirect, fIndirect, false, newIR);
    }
    else 
    {
        switch (kind.runtimeLookupKind)
        {
            case CORINFO_LOOKUP_THISOBJ :
                ASSERTNR(!"Should never get this for static method.");
                break;
            case CORINFO_LOOKUP_CLASSPARAM :
                // In this case, the hidden param is the class handle. 
                handleNode = GenIR_InstParam(newIR);
                break;
            case CORINFO_LOOKUP_METHODPARAM :
                // In this case, the hidden param is the method handle.
                handleNode = GenIR_InstParam(newIR);
                // Call helper CORINFO_HELP_GETCLASSFROMMETHODPARAM to get the class handle
                // from the method handle.
                handleNode = GenIR_CallHelper(CORINFO_HELP_GETCLASSFROMMETHODPARAM, GenIR_MakePtrNode(), newIR, handleNode);
                break;
            default:
                ASSERTNR(!"Unknown LOOKUP_KIND");
                break;
        }

        ASSERTNR(handleNode);  // handleNode should now contain the CORINFO_CLASS_HANDLE for the exact class.

        // Given the class handle, get the pointer to the Monitor.
        handleNode = GenIR_CallHelper(CORINFO_HELP_GETSYNCFROMCLASSHANDLE, GenIR_MakePtrNode(), newIR, handleNode);
    }

    ASSERTNR(handleNode);
    return handleNode;
}
/*++

Function: rdrCallFieldHelper

If accessing the field requires a helper call, then call the 
appropriate helper. This can apply to loading from or storing
to the field.

For loads, the prototype is 'type ldfld(object, fieldHandle)'.
For stores, the prototype is 'void stfld(object, fieldHandle, value)'.

--*/

void
ReaderBase::rdrCallFieldHelper(
    mdToken        token,
    bool           fLoad,
    IRNode*        dst,   // dst node if this is a load, otherwise NULL
    IRNode*        tupObj,
    IRNode*        tupValue,
    ReaderAlignType alignment,
    bool           fVolatile,
    IRNode**       newIR
){
    CorInfoHelpFunc helperId;
    IRNode *tupArg1, *tupArg2, *tupArg3;
    CORINFO_FIELD_HANDLE fieldHandle = getFieldHandle(token);

#if !defined(NODEBUG)
    {
        DWORD fieldAttribs;
        ASSERTNR(tupObj != NULL);
        fieldAttribs = getFieldAttribs(fieldHandle, CORINFO_ACCESS_ANY);
        ASSERTNR(!(fieldAttribs & CORINFO_FLG_STATIC));
        ASSERTNR(fieldAttribs & CORINFO_FLG_HELPER);
    }
#endif // !NODEBUG
    
    // JIT64:119372  We need the helperId early, because we must create slightly
    // different calls for CORINFO_HELP_{GET|SET}FIELDSTRUCT, then the other field helpers!
    
    helperId = getFieldHelper(fieldHandle,fLoad ? CORINFO_GET : CORINFO_SET);

    if (fLoad) {
        ASSERTNR(tupValue == NULL);
        
        if (helperId == CORINFO_HELP_GETFIELDSTRUCT) {
            // JIT64:119372  For a GetFieldStruct, we want to create the following:
            //
            // HCIMPL3(VOID, JIT_GetFieldStruct, LPVOID retBuff, Object *obj, FieldDesc *pFD)
            // 
            // What this means, is that the helper will *not* return the value that we
            // are interested in.  Instead, it will pass a pointer to the return value as
            // the first param.
            
            tupArg1 = GenIR_AddressOfLeaf(dst, newIR);

            // Arg 2
            tupArg2 = tupObj;

            // Arg 3 is the field handle.
            tupArg3 = GenIR_GenericTokenToNode(token, newIR);

            // Make the helper call
            GenIR_CallHelper(helperId, NULL, newIR, tupArg1, tupArg2, tupArg3, alignment, fVolatile);
        } else {
            // OTHER LOAD

            // Arg2 - the field handle.
            tupArg2 = GenIR_GenericTokenToNode(token, newIR);

            // Arg1 - this pointer
            tupArg1 = tupObj;

            // Make the helper call
            GenIR_CallHelper(helperId, dst, newIR, tupArg1, tupArg2, NULL, alignment, fVolatile);
        }

    } else {
        // STORE

        if (helperId == CORINFO_HELP_SETFIELDSTRUCT) {
            // JIT64:119372  For a SetFieldStruct, we want to create the following:
            //
            // HCIMPL3(VOID, JIT_SetFieldStruct, Object *obj, FieldDesc *pFD, LPVOID valuePtr)
            //
            // The idea here is that we must pass a *pointer* to the value that we are setting,
            // rather than the value itself.  Simple enough... MSILAddressOf is your friend!
            tupArg3 = GenIR_AddressOfValue( tupValue, newIR);

            // The second argument to the helper is the field handle.
            tupArg2 = GenIR_GenericTokenToNode(token, newIR);

            // The first argument to the helper is the this pointer.        
            tupArg1 = tupObj;

            // Make the helper call
            GenIR_CallHelper(helperId, NULL, newIR, tupArg1, tupArg2, tupArg3, alignment, fVolatile);
        } else {
            // assert that the helper id is expected
            ASSERTNR(helperId==CORINFO_HELP_SETFIELD32
                || helperId==CORINFO_HELP_SETFIELD64
                || helperId==CORINFO_HELP_SETFIELDOBJ);

            tupArg3 = tupValue;

            // The second argument to the helper is the field handle.
            tupArg2 = GenIR_GenericTokenToNode(token, newIR);

            // The first argument to the helper is the this pointer.        
            tupArg1 = tupObj;
            
            // Make the helper call
            GenIR_CallHelper(helperId, NULL, newIR, tupArg1, tupArg2, tupArg3, alignment, fVolatile);
        }
    }
}

// rdrCallWriteBarrierHelper - This code adds a call to the WriteBarrier helper
// to the code stream.  What this is is a helper function which copies a value
// into a particular field of a class.  We use this helper when the runtime
// tells us to by putting the CORINFO_FLG_WRITE_BARRIER_HELPER on the fieldAttribs
// for the field.  The runtime only does this if the field we are writing to is
// a pointer to the GC heap (or is a value class which contains such a pointer).
// The helper is used in these situations so that the GC runtime can update its
// tables and know that this piece of memory has been updated.
//
// Do note that there are actually 2 slightly different versions of the helper
// that we must call.  One if we are simply writing into a field which is a pointer,
// and a different one if we are writing in a value class.
//
// Alignment is necessary in case the JIT wants to turn a struct write barrier
//  into a struct copy.
//
void
ReaderBase::rdrCallWriteBarrierHelper(
    IRNode*        arg1, // addrDst
    IRNode*        arg2, // addrSrc
    ReaderAlignType alignment,
    bool           fVolatile,
    IRNode**       newIR,
    mdToken        token,
    bool           isNotValueClass,
    bool           fValueIsPointer,
    bool           fIsFieldToken
){
    if (isNotValueClass) {
        // This is the non-value class case.  That is, we are simply writing to
        // a field in a class which happens to be a GC pointer.

        // HCIMPL2(void, JIT_CheckedWriteBarrier, Object** dest, Object * value)
        GenIR_CallHelper(CORINFO_HELP_CHECKED_ASSIGN_REF, NULL, newIR, arg1, arg2, NULL, alignment, fVolatile);
    } else {
        // This is the case in which we will be copying a value class into the
        // field of this struct.  The runtime will need to be passed the classHandle
        // of the struct so it knows which fields are of importance, w.r.t. GC.

        // HCIMPL2(void, JIT_StructWriteBarrier, void* dest, void* src, 
        //         CORINFO_CLASS_HANDLE *fieldsClassHandle) 

        if (!fValueIsPointer) {
            // Do note that in this case we want a pointer to the source, but we will actually
            // have is the struct itself, therefore we need to get its address.
            arg2 = (IRNode*)GenIR_AddressOfValue(arg2, newIR);
        }
     
        CORINFO_CLASS_HANDLE classHandle = NULL;
        if (fIsFieldToken) {
            getFieldType(getFieldHandle(token),&classHandle);
        } else {
            classHandle = getClassHandle(token);
        }

        if (getClassGCLayout(classHandle))
        {
            IRNode *arg3;
            if (fIsFieldToken) {
                bool fIndirect;
                void *embHandle = embedClassHandle(classHandle, &fIndirect);
                arg3 = GenIR_HandleToIRNode(token, embHandle, classHandle, fIndirect, fIndirect, true, false, newIR);
            } else {
                arg3 = GenIR_GenericTokenToNode(token, newIR);
            }

            GenIR_CallHelper(CORINFO_HELP_ASSIGN_STRUCT, NULL, newIR, arg1, arg2, arg3, alignment, fVolatile);
        }
        else
        {
            // If the class doesn't have a gc layout then use a memcopy
            IRNode *size = GenIR_LoadConstantI4(getClassSize(classHandle), newIR);
            GenIR_CallHelper(CORINFO_HELP_MEMCPY, NULL, newIR, arg1, arg2, size, alignment, fVolatile);
        }
    }
}

// rdrCallWriteBarrierHelperForReturnValue - This is a variant of rdrCallWriteBarrierHelper
// where we are returning a struct with gc pointers by value through a hidden parameter.

void
ReaderBase::rdrCallWriteBarrierHelperForReturnValue(
    IRNode*        arg1, // addrDst
    IRNode*        arg2, // addrSrc
    IRNode**       newIR,
    mdToken        token
){
    IRNode *arg3;
    bool fIndirect;

    CORINFO_SIG_INFO sigCurrMethod;

    getMethodSig(getCurrentMethodHandle(), &sigCurrMethod);
    void *embHandle = embedClassHandle(sigCurrMethod.retTypeClass, &fIndirect);

    arg3 = GenIR_HandleToIRNode(token,embHandle,sigCurrMethod.retTypeClass,fIndirect,fIndirect,true,false,newIR);
    GenIR_CallHelper(CORINFO_HELP_ASSIGN_STRUCT, NULL, newIR, arg1, arg2, arg3);        
}

IRNode*
ReaderBase::GenIR_GetStaticFieldAddress(
    mdToken tokenField,
    IRNode** newIR
){
    DWORD           fieldAttribs;
    IRNode         *tupAddr;
    void           *fieldAddress;
    ULONG           offset;
    bool            fBoxed;
    bool            fIndirect;
    bool            fIndirect2;
    CorInfoHelpFunc helperId;
    CORINFO_FIELD_HANDLE fieldHandle = getFieldHandle(tokenField);

    fieldAttribs = getFieldAttribs(fieldHandle, CORINFO_ACCESS_ANY);
    ASSERTNR(fieldAttribs & CORINFO_FLG_STATIC);

    bool fGenericHelper = ((fieldAttribs & CORINFO_FLG_HELPER)!=0);
    bool fGenericsStaticFieldHelper = ((fieldAttribs & CORINFO_FLG_GENERICS_STATIC)!=0);

    if (fGenericHelper) {
        // Need to generate a helper call to get the address.
        // The first argument to the helper is the field handle.
        IRNode *fieldHandleNode = GenIR_GenericTokenToNode(tokenField,newIR);

        // Get the id of the helper call and then get the func. descr.
        helperId = getFieldHelper(fieldHandle, CORINFO_ADDRESS);

        IRNode *ptrNode;
        switch(helperId)
        {
            case CORINFO_HELP_GETSTATICFIELDADDR:
            case CORINFO_HELP_GET_THREAD_FIELD_ADDR_OBJREF:
                ptrNode = GenIR_MakePtrDstGCOperand(true);
                break;
            default:
                ptrNode = GenIR_MakePtrNode();
                break;
        }
        
        // Now make the call and attach the arguments.
        return GenIR_CallHelper(helperId, ptrNode, newIR, fieldHandleNode);
    }

    if (fGenericsStaticFieldHelper || (fieldAttribs & CORINFO_FLG_SHARED_HELPER)) {
        // We access shared statics using a helper call to get the address
        // of the base of the shared statics, then we add on the offset for
        // the static field that we are accessing.

        IRNode*  tvSharedStaticsBase;
        
        bool fRuntimeLookup = FALSE;
        CORINFO_CLASS_HANDLE classHandle = NULL;
        mdToken typeRef = getMemberParent(tokenField);
        IRNode *classHandleNode = GenIR_GenericTokenToNode(typeRef,newIR, false,
                                    (CORINFO_GENERIC_HANDLE*)&classHandle, &fRuntimeLookup);

        helperId = getSharedStaticBaseHelper(fieldHandle, fRuntimeLookup);

        if (    helperId == CORINFO_HELP_GETGENERICS_GCSTATIC_BASE 
                || helperId == CORINFO_HELP_GETGENERICS_NONGCSTATIC_BASE)
        {
            if (helperId == CORINFO_HELP_GETGENERICS_GCSTATIC_BASE)
            {
                // Again, we happen to know that the results of these helper calls should be
                // interpreted as interior GC pointers.                
                tvSharedStaticsBase = GenIR_MakePtrNode(READER_PTR_GC_INTERIOR);
            } else {
                ASSERTNR(helperId == CORINFO_HELP_GETGENERICS_NONGCSTATIC_BASE);
                tvSharedStaticsBase = GenIR_MakePtrNode();
            }

            // Now make the call and attach the arguments.
            GenIR_CallHelper(helperId, tvSharedStaticsBase, newIR, classHandleNode);
        }
        else
        {
            void *  embedModuleDomainID;
            void *  embedClassDomainID;
            bool    fNoCtor = false;

            // consume this, as in this case it is unused. 
            GenIR_Pop(classHandleNode,newIR);

            // If possible, use the result of a previous sharedStaticBase call.
            embedModuleDomainID = getEmbedModuleDomainID(getClassModuleForStatics(classHandle),&fIndirect); 
            embedClassDomainID = getEmbedClassDomainID(classHandle,&fIndirect2);
            ASSERTNR(helperId == CORINFO_HELP_GETSHARED_GCSTATIC_BASE
                    || helperId == CORINFO_HELP_GETSHARED_NONGCSTATIC_BASE
                    || helperId == CORINFO_HELP_GETSHARED_GCSTATIC_BASE_NOCTOR
                    || helperId == CORINFO_HELP_GETSHARED_NONGCSTATIC_BASE_NOCTOR
                    || helperId == CORINFO_HELP_GETSHARED_GCSTATIC_BASE_DYNAMICCLASS
                    || helperId == CORINFO_HELP_GETSHARED_NONGCSTATIC_BASE_DYNAMICCLASS);

            // This will possibly switch the helperId to a NoCtor version
            // It will also set fNoCtor in the case where no static ctor is needed
            // but there's not helper call for that, just different intrinsics.
            tvSharedStaticsBase = DomInfo_DominatorDefinesSharedStaticBase(m_currentFgNode, 
                                                  helperId, embedModuleDomainID, embedClassDomainID, typeRef, &fNoCtor);
        
            if (tvSharedStaticsBase == NULL) {
                bool fCanMoveUp = false;
                IRNode *moduleDomainIDNode = GenIR_HandleToIRNode(getMemberParent(typeRef), embedModuleDomainID, 0, fIndirect, fIndirect, fIndirect, false, newIR);
                IRNode *classDomainIDNode = GenIR_HandleToIRNode(typeRef, embedClassDomainID, 0, fIndirect2, fIndirect2, fIndirect2, false, newIR);

                if (   helperId == CORINFO_HELP_GETSHARED_GCSTATIC_BASE
                       || helperId == CORINFO_HELP_GETSHARED_GCSTATIC_BASE_NOCTOR
                       || helperId == CORINFO_HELP_GETSHARED_GCSTATIC_BASE_DYNAMICCLASS) 
                {
                    // Again, we happen to know that the results of these helper calls should be
                    // interpreted as interior GC pointers.
                
                    tvSharedStaticsBase = GenIR_MakePtrNode(READER_PTR_GC_INTERIOR);
                } else {
                    tvSharedStaticsBase = GenIR_MakePtrNode();
                }

                // This intrinsic is 'moveable' if it either doesn't call the .cctor
                // or the classs is marked with the relaxed semmantics (BeforeFieldInit).
                if (fNoCtor)
                {
                    fCanMoveUp = true;
                }
                else
                {
                    // Check to see if this is a relaxed static init class, and thus the 
                    // .cctor can be CSE'd and otherwise moved around.
                    // If yes, then try and move it outside loops.
                    CORINFO_CLASS_HANDLE classHandleOfField  = getFieldClass(fieldHandle);
                    DWORD classAttribs = getClassAttribs(classHandleOfField);
                    if (classAttribs & CORINFO_FLG_BEFOREFIELDINIT)
                    {
                        fCanMoveUp = true;
                    }
                }

                // Record operand that holds shared statics base
                DomInfo_RecordSharedStaticBaseDefine(m_currentFgNode, helperId, embedModuleDomainID, 
                                                     embedClassDomainID, tvSharedStaticsBase);

                // Record that this block initialized class typeRef.
                DomInfo_RecordClassInit(m_currentFgNode, typeRef);

                GenIR_CallHelper(helperId, tvSharedStaticsBase, newIR, moduleDomainIDNode, classDomainIDNode,
                    NULL, READER_ALIGN_UNKNOWN, false, fNoCtor, fCanMoveUp);
            }
        }

        // Get the offset of the field.
        fieldAddress = getStaticFieldAddress(fieldHandle, &offset, &fBoxed, &fIndirect);
        ASSERTNR(fieldAddress == NULL); // should always be NULL for Shared_Helper cases

        // This is an offset into the shared static table, it is usually non-zero,
        // it won't do much to optimize this. FURTHERMORE: the add tells the garbage
        // collector that the base pointer is interior (even for an add of zero), so
        // this add is necessary for GC to work.
        tupAddr = GenIR_BinaryOp( ReaderBaseNS::ADD,
                                  tvSharedStaticsBase,
                                  GenIR_LoadConstantI(offset, newIR),
                                  newIR);
    
        if (fIndirect) {
            tupAddr = GenIR_DerefAddress(tupAddr, false, newIR);
        }

        // We can be have both fIndirect and fBoxed
        // This occurs for a static value classes 
        // 
        // In such a case the fieldaddress is the address of a handle 
        // The handle points at a boxed value class (which is an object reference)
        // The value class data is at offset 8 in the object
        if (fBoxed) {
            // If the field is a boxed valueclass, the address returned points to 
            // the boxed data. So the real address is at [fieldAddress]+sizeof(void*).
            IRNode* tupBoxedAddr;

            tupBoxedAddr = GenIR_DerefAddress(tupAddr, true, newIR);
            tupAddr = GenIR_BinaryOp(ReaderBaseNS::ADD, tupBoxedAddr,
                                     GenIR_LoadConstantI(VALUE_CLASS_DATA_START_OFFSET,
                                                         newIR),
                                     newIR);
        }

        return tupAddr;
    }

    // Emit a call to run the Class Constructor if necessary
    if (accessStaticFieldRequiresClassConstructor(fieldHandle)) 
    {
        mdToken typeRef = getMemberParent(tokenField);

        // If class hasn't been initialized on this path
        if (!DomInfo_DominatorHasClassInit(m_currentFgNode, typeRef))
        {
            bool fRuntimeLookup = FALSE;
            CORINFO_CLASS_HANDLE classHandle = NULL;
            IRNode *classHandleNode = GenIR_GenericTokenToNode(typeRef,newIR, false,
                                        (CORINFO_GENERIC_HANDLE*)&classHandle, &fRuntimeLookup);

            // Use the shared static base helper as it is faster than InitClass
            // getSharedCCtorHelper returns one of these three values:
            //     CORINFO_HELP_GETSHARED_NONGCSTATIC_BASE 
            //     CORINFO_HELP_GETSHARED_NONGCSTATIC_BASE_DYNAMICCLASS
            //     CORINFO_HELP_GETGENERICS_NONGCSTATIC_BASE

            helperId = fRuntimeLookup ? CORINFO_HELP_INITCLASS : getSharedCCtorHelper(classHandle);

            /* If we have a runtime lookup then we have to call the slower InitClass */
            if (helperId == CORINFO_HELP_INITCLASS
                || helperId == CORINFO_HELP_GETGENERICS_NONGCSTATIC_BASE)
            {
                // call InitClass on the class represented by tupArg
                GenIR_CallHelper(helperId, NULL, newIR, classHandleNode);
            }
            else
            {
                // consume this as in this case it went unused. 
                GenIR_Pop(classHandleNode,newIR);

                void *  embedModuleDomainID = getEmbedModuleDomainID(getClassModuleForStatics(classHandle),&fIndirect); 
                void *  embedClassDomainID  = getEmbedClassDomainID(classHandle,&fIndirect2);
                bool    fNoCtor = false;
                bool    fCanMoveUp = false;

                // This will possibly switch the helperId to a NoCtor version
                // It will also set fNoCtor in the case where no static ctor is needed
                // but there's not helper call for that, just different intrinsics.
                DomInfo_DominatorDefinesSharedStaticBase(m_currentFgNode, helperId, 
                                                         embedModuleDomainID, embedClassDomainID, 
                                                         typeRef, &fNoCtor);

                // We shouldn't have gotten here if the Ctor has already run.
                ASSERTNR(!fNoCtor);

                // Check to see if this is a relaxed static init class, and thus the 
                // .cctor can be CSE'd and otherwise moved around.
                // If yes, then try and move it outside loops.
                DWORD classAttribs = getClassAttribs(classHandle);
                if (classAttribs & CORINFO_FLG_BEFOREFIELDINIT)
                {
                    fCanMoveUp = true;
                }

                IRNode *moduleDomainIDNode = GenIR_HandleToIRNode(getMemberParent(typeRef), embedModuleDomainID, 0, fIndirect, fIndirect, fIndirect, false, newIR);
                IRNode *classDomainIDNode = GenIR_HandleToIRNode(typeRef, embedClassDomainID, classHandle, fIndirect2, fIndirect2, fIndirect2, false, newIR);
                    
                // call the faster init class helper
                GenIR_CallHelper(helperId, NULL, newIR, moduleDomainIDNode, classDomainIDNode,
                    NULL, READER_ALIGN_UNKNOWN, false, false, fCanMoveUp);
            }
                
            // Record that this block initialized class typeRef.
            DomInfo_RecordClassInit(m_currentFgNode, typeRef);
        }
    }




    fieldAddress = getStaticFieldAddress(fieldHandle, &offset, &fBoxed, &fIndirect);
    ASSERTNR(fieldAddress != NULL);
    ASSERTNR(offset == 0);

#if !defined(NODEBUG)
    CORINFO_CLASS_HANDLE hClass;
    CorInfoType corInfoType;
    unsigned minClassAlign;

    corInfoType = getFieldType(getFieldHandle(tokenField),&hClass);

    if ((corInfoType == CORINFO_TYPE_REFANY) ||
        (corInfoType == CORINFO_TYPE_VALUECLASS)) {
        if (fBoxed) {
            minClassAlign = sizeof(char*); // alignment is size of pointer
        } else {
            minClassAlign = getClassAlignmentRequirement(hClass);
        }
    } else {
        minClassAlign = 0;
    }

    GenIR_VerifyStaticAlignment(fieldAddress, corInfoType, minClassAlign);
#endif

    tupAddr = GenIR_HandleToIRNode(tokenField, fieldAddress, fieldHandle, fIndirect, fIndirect, true, false, newIR);
    
    // We can be have both fIndirect and fBoxed
    // This occurs for a static value classes 
    // 
    // In such a case the fieldaddress is the address of a handle 
    // The handle points at a boxed value class (which is an object reference)
    // The value class data is at offset 8 in the object
    //
    if (fBoxed) {
        // If the field is a boxed valueclass, the address returned points to 
        // the boxed data. So the real address is at [fieldAddress]+sizeof(void*).
        IRNode* tupBoxedAddr;

        tupBoxedAddr = GenIR_DerefAddress(tupAddr, true, newIR);
        tupAddr = GenIR_BinaryOp(ReaderBaseNS::ADD, tupBoxedAddr,
                                 GenIR_LoadConstantI(VALUE_CLASS_DATA_START_OFFSET,
                                                     newIR),
                                 newIR);
    }

    return tupAddr;
}

IRNode*
ReaderBase::rdrGetFieldAddress(
    mdToken        token,
    IRNode*        ptObj,
    bool           fBaseIsGCObj,
    bool           fBaseIsThis,
    bool*          fNullCheckAdded,
    IRNode**       newIR
){

    CORINFO_ACCESS_FLAGS accessFlags = fBaseIsThis ? CORINFO_ACCESS_THIS : CORINFO_ACCESS_ANY;
    CORINFO_FIELD_HANDLE fieldHandle = getFieldHandle(token);
    DWORD fieldAttribs = getFieldAttribs(fieldHandle, accessFlags);

    // Assume we don't need to insert an explicit null check.
    *fNullCheckAdded = false;

    if (fieldAttribs & CORINFO_FLG_STATIC) {
        return GenIR_GetStaticFieldAddress(token, newIR);
    }

    if (fieldAttribs & CORINFO_FLG_HELPER) {
        // Need to generate a helper call to get the address.
        CorInfoHelpFunc helperId;
        IRNode *tupArg1, *tupArg2, *dst;

        tupArg1 = ptObj;

        // The second argument to the helper is the field handle.
        tupArg2 = GenIR_GenericTokenToNode(token, newIR);

        // Get the id of the helper call and then get the func. descr.
        helperId = getFieldHelper(fieldHandle, CORINFO_ADDRESS);
        if (fBaseIsGCObj) {
            dst = GenIR_MakePtrDstGCOperand(true);
        } else {
            dst = GenIR_MakePtrNode();
        }

        return GenIR_CallHelper(helperId, dst, newIR, tupArg1, tupArg2);
    } else {
        // Get the offset, add it to the this pointer to calculate the
        // actual address of the field.
        IRNode *addrNode;
        unsigned int fieldOffset;

        // Get the offset of the field.
        fieldOffset = getFieldOffset(fieldHandle);
    
        // If the offset is bigger than MAX_UNCHECKED_OFFSET_FOR_NULL_OBJECT,
        //  then we need to insert an explicit null check on the object pointer.

        if (fieldOffset >= MAX_UNCHECKED_OFFSET_FOR_NULL_OBJECT) {
            ptObj = GenIR_GenNullCheck(ptObj, newIR);
            *fNullCheckAdded = true;
        }

        addrNode = GenIR_BinaryOp(ReaderBaseNS::ADD,
                                  ptObj,
                                  GenIR_LoadConstantI( fieldOffset, newIR),
                                  newIR);
        return addrNode;
    }
}

IRNode*
ReaderBase::GenIR_LoadToken(
    mdToken token,
    IRNode** newIR
){
    mdToken tokType = TypeFromToken(token);
    bool isTypeTok = (tokType == mdtTypeDef ||
                      tokType == mdtTypeRef ||
                      tokType == mdtTypeSpec);

    return GenIR_GenericTokenToNode(isTypeTok ? (token | CORINFO_ANNOT_PERMITUNINSTDEFORREF) : token,
                                       newIR, isTypeTok, NULL, NULL, CORINFO_TOKENKIND_Ldtoken);
}

void
ReaderBase::getCallSiteSignature(
    CORINFO_METHOD_HANDLE hMethod,
    mdToken               token,
    CORINFO_SIG_INFO*     pSig,
    bool*                 pHasThis
){
    getCallSiteSignature(hMethod,token,pSig,pHasThis,getCurrentMethodHandle(),getCurrentModuleHandle());
}

void
ReaderBase::getCallSiteSignature(
    CORINFO_METHOD_HANDLE hMethod,
    mdToken               token,
    CORINFO_SIG_INFO*     pSig,
    bool*                 pHasThis,
    CORINFO_METHOD_HANDLE context,
    CORINFO_MODULE_HANDLE scope
)
{
    // See if we have been given a method token or a signature token.
    if (hMethod) {
        m_jitInfo->getMethodSig(hMethod, pSig);
        *pHasThis = (pSig->hasThis() != 0);

        if (pSig->isVarArg()) 
        {
            m_jitInfo->findCallSiteSig(scope, token, context, pSig);
        }
    } else {
        // Get the signature information using the given signature token.
        m_jitInfo->findSig(scope, token, context, pSig);
        *pHasThis = (pSig->hasThis() != 0);
    }

}


// Constraint calls in generic code.  Constraint calls are operations on generic type variables,
// e.g. "x.Incr()" where "x" has type "T" in generic code and where "T" supports an interface
// (e.g. IIncr) with an "Incr" method.  They are prefixed by the "constrained."
// prefix, indicating which "T" is being invoked.
//
// Contraint calls have a very simple interpretation when generating specialized
// code: if T is a value type VC then 
//        <load-a-byref-to-T>
//        constrained.
//        callvirt I::Incr()
// becomes 
//        <load-a-byref-to-VC>
//        call VC::Incr()      <-- this is the MethodDesc that accpets a unboxed "this" pointer
// and if T is a reference type R then this becomes
//        <load-a-byref-to-R>
//        <dereference-the-byref>
//        callvirt R::Incr()
// There are some situations where a MethodDesc that accepts an unboxed "this" 
// pointer is not available, in which case the call becomes
//        <load-a-byref-to-VC>
//        <box-the-byref>
//        callvirt R::Incr()      <-- this will call is the MethodDesc that accpets a boxed "this" pointer
//
// The above interpretations make sense even when "Incr" is a generic method, or when VC or I are types
// that are shared amongst generic instantiations, e.g. VC<object> or I<object> 
// This code is lifted from the x86 JIT. The idea is that for value types the actual target of the
// constraint call is determined here.  Depending n the nature of the constraint call we may have
// to either dereference or box the "this" pointer.
//
// loadFtnToken parameter is used only to optimize the delegate constructor codepath.
//  It contains the argument to the CEE_LDFTN or CEE_LDVIRTFUNC opcode when we have a CEE_NEWOBJ opcode.
//  The CEE_LDFTN opcode must immedatiately preceed the CEE_NEWOBJ opcode and both must be in the same basic block.
//  If any of these conditions are not true then loadFtnToken will be mdTokenNil
// 
IRNode*
ReaderBase::rdrCall(
    ReaderBaseNS::CallOpcode opcode,
    mdToken                  targetMethodToken,
    mdToken                  constraintTypeRef,
    mdToken                  loadFtnToken,
    bool                     fReadonlyCallPrefix,
    bool                     fTailCallPrefix,
    bool                     fIsUnmarkedTailCall,
    IRNode**                 callNode, // out param is defined by GenIR_GenCall
    IRNode**                 newIR
){
    IRNode *returnNode;
    callArgTriple *argArray;
    unsigned int numArgs;
    unsigned int firstArgNum;
    bool hasThis;
    int i;

    // Tail call is only permitted for call, calli and callvirt.
    ASSERTNR(!fTailCallPrefix ||
             (opcode == ReaderBaseNS::CALL)  ||
             (opcode == ReaderBaseNS::CALLI) ||
             (opcode == ReaderBaseNS::CALLVIRT));

    // Constrained call is only permitted on callvirt
    ASSERTNR((constraintTypeRef == mdTokenNil) ||
             (opcode == ReaderBaseNS::CALLVIRT));

    // Readonly call prefix only for Address operation on arrays
    ASSERTNR(!fReadonlyCallPrefix ||
             (opcode == ReaderBaseNS::CALL)  ||
             (opcode == ReaderBaseNS::CALLI) ||
             (opcode == ReaderBaseNS::CALLVIRT));

    // loadFtnToken is only permitted when we are processing newobj call
    ASSERTNR((loadFtnToken == mdTokenNil) ||
             (opcode == ReaderBaseNS::NEWOBJ));

    // For certain intrinsics, we can determine that the call has no
    // side effects ...
    bool callCanSideEffect = true;

    // Create a place to hold all the call target data
    ReaderCallTargetData callTargetData(this,
                                        targetMethodToken,constraintTypeRef,
                                        loadFtnToken,
                                        fTailCallPrefix,fIsUnmarkedTailCall,fReadonlyCallPrefix,
                                        opcode);


    if (ReaderBaseNS::CALLI == opcode)
    {
        callTargetData.callTargetNode = m_readerStack->pop();
    }
    else
    {
#if defined(CC_CALL_INTERCEPT)
        checkCallAuthorization(
            callTargetData.getMethodToken(),
            callTargetData.getMethodHandle(),
            newIR);
#endif

        // If the current method calls a method which needs a security
        // check, we need to reserve a slot for the security object in
        // the current method's stack frame
        if (callTargetData.getMethodAttribs() & CORINFO_FLG_SECURITYCHECK) {
            GenIR_MethodNeedsSecurityCheck();
        }


    }

    CORINFO_SIG_INFO *pSigInfo = callTargetData.getSigInfo();

    // Get the number of arguments to this method.
    numArgs = (unsigned int) pSigInfo->numArgs;
    hasThis = callTargetData.hasThis();

    // Special case for newobj, currently the first
    // argument is handled/appended in CanonNewObj.
    // For this reason we don't need to record any
    // information about that argument.
    firstArgNum = 0;
    if (opcode == ReaderBaseNS::NEWOBJ) {
        ASSERTNR(hasThis);  // new obj better have "this"
        firstArgNum = 1;
    }

    // Create arg array and populate with stack arguments.
    //   - struct return pointer does not live on arg array,
    //     it is passed to GenIR as destination.
    //   - this pointer is also not present in array.
    
    // Note that array is populated with two loops, the first
    // traverses the ee's argument list, the second pops
    // arguments from the stack. Two loops are needed because
    // the data is stored with opposite orderings.

    // First populate argType, argClass fields.
    argArray = NULL;


    if ((hasThis + numArgs) > 0)
    {
        CORINFO_ARG_LIST_HANDLE args;
        CorInfoType corType;
        CORINFO_CLASS_HANDLE hArgType, hClass;

        argArray = (callArgTriple*)_alloca(sizeof(callArgTriple)*(numArgs+hasThis));
#if !defined(NODEBUG)
        memset(argArray,0,sizeof(callArgTriple)*(numArgs+hasThis));
#endif
        args = pSigInfo->args;
        i = 0;

        // If this call passes a this ptr, then it is first in array.
        if (hasThis) {
            argArray[i].argType = CORINFO_TYPE_BYREF;
            argArray[i].argClass = NULL;
            if (callTargetData.getMethodHandle() != NULL) {
                if ((callTargetData.getClassAttribs() & CORINFO_FLG_VALUECLASS) == 0) {
                    argArray[i].argType = CORINFO_TYPE_PTR;
                }
            } 
            i++;
        }

        // Populate remaining argument list
        for (;i<(int)(numArgs+hasThis);i++)
        {
            corType = strip(m_jitInfo->getArgType(pSigInfo, args, &hArgType));
            ASSERTNR(corType != CORINFO_TYPE_VAR); // common generics trouble

            if (corType == CORINFO_TYPE_CLASS || corType == CORINFO_TYPE_VALUECLASS) {
                hClass = m_jitInfo->getArgClass(pSigInfo, args);
            } else if (corType == CORINFO_TYPE_REFANY) {
                hClass = m_jitInfo->getBuiltinClass(CLASSID_TYPED_BYREF);
            } else {
                hClass = NULL;
            }


            argArray[i].argType = corType;
            argArray[i].argClass = hClass;
            args = m_jitInfo->getArgNext(args);
        }

        // Now pop args from argument stack (including this)
        // - populating argument list in reverse order.
        // For newobj the this pointer is not yet on the stack
        // so don't pop it!
        for (i = numArgs+hasThis-1;i>=(int)firstArgNum;i--) {
            argArray[i].argNode = (IRNode*)m_readerStack->pop();
        }

        // this-pointer specific stuff
        if (hasThis)
        {
            CORINFO_CALL_INFO *callInfo = callTargetData.getCallInfo();
            if (opcode == ReaderBaseNS::NEWOBJ)
            {
                // First argument to newobj has complete type info, but no argument node.
                argArray[0].argNode = NULL;
            }
            else if (callInfo && callInfo->thisTransform == CORINFO_DEREF_THIS)
            {
                // constraint calls on reference types dereference the byref
                //   used to specify the object
                IRNode *thisPtr = argArray[0].argNode;
                argArray[0].argNode = GenIR_LoadIndir(ReaderBaseNS::LDIND_REF,thisPtr,READER_ALIGN_NATURAL,false,false,newIR);
            }
            else if (callInfo && callInfo->thisTransform == CORINFO_BOX_THIS)
            {
                // Constraint calls on value types where there is no 
                // unboxed entry point require us to box the value. 
                // These only occur when a value type 
                // has inherited an implementation of an interface
                // method from System.Object or System.ValueType.  The EE should
                // really provide the JITs with with 
                // "boxing" stubs for these methods.

                // looks like the steps are (from x86 JIT):
                //   1. load obj from pointer
                //   2. box obj that was loaded
                //   3. set this pointer to address returned from box
            
                IRNode *thisPtr = argArray[0].argNode;
                thisPtr = GenIR_LoadAndBox(constraintTypeRef,thisPtr,READER_ALIGN_NATURAL,newIR);
                argArray[0].argNode = thisPtr;
            }
        }
    }


    //
    //
    //
    //

    if (callCanSideEffect) {
        GenIR_RemoveStackInterference(newIR);
    }

    // Get the call target
    if (ReaderBaseNS::CALLI != opcode) {
        rdrMakeCallTargetNode(
            &callTargetData,
            argArray != NULL ? &argArray[0].argNode : NULL,
            newIR);
    }

    // Ask GenIR to emit call, returns a ReturnNode.
    returnNode = GenIR_GenCall(&callTargetData,
                               argArray, numArgs+hasThis,
                               callNode, newIR);
    return returnNode;
}


bool
ReaderBase::rdrCallIsDelegateInvoke(ReaderCallTargetData *callTargetData)
{
    DWORD methodAttribs = callTargetData->getMethodAttribs();
    if ((methodAttribs & CORINFO_FLG_DELEGATE_INVOKE)!=0) {
        ASSERTNR(!(methodAttribs & CORINFO_FLG_STATIC));
        ASSERTNR(methodAttribs & CORINFO_FLG_FINAL);
        return true;
    }
    return false;
}

void
ReaderBase::rdrMakeCallTargetNode(ReaderCallTargetData *callTargetData, IRNode **thisPtr, IRNode **newIR)
{
    CORINFO_CALL_INFO *callInfo = callTargetData->getCallInfo();
    ASSERTNR(callInfo);
    IRNode *target;
    
    if (callInfo == NULL) {
        return;
    }

    // Check for Delegate Invoke optimization 
    if (rdrCallIsDelegateInvoke(callTargetData)) {
        callTargetData->callTargetNode = rdrGetDelegateInvokeTarget(callTargetData, thisPtr, newIR);
        return;
    }

    switch(callInfo->kind)
    {
        case CORINFO_CALL:
            // Direct Call
            callTargetData->fNeedsNullCheck = callInfo->nullInstanceCheck == TRUE;
            target = rdrGetDirectCallTarget(callTargetData, newIR);
            break;
        case CORINFO_CALL_CODE_POINTER:
            // Runtime lookup required (code sharing w/o using inst param)
            callTargetData->fNeedsNullCheck = callInfo->nullInstanceCheck == TRUE;
            target = rdrGetCodePointerLookupCallTarget(callTargetData, newIR);
            break;
        case CORINFO_VIRTUALCALL_STUB:
            // Virtual Call via virtual dispatch stub
            callTargetData->fNeedsNullCheck = true;
            target = rdrGetVirtualStubCallTarget(callTargetData, newIR);
            break;
        case CORINFO_VIRTUALCALL_LDVIRTFTN:
            // Virtual Call via indirect virtual call
            target = rdrGetIndirectVirtualCallTarget(callTargetData, thisPtr, newIR);
            break;
        case CORINFO_VIRTUALCALL_VTABLE:
            // Virtual call via table lookup (vtable)
            target = rdrGetVirtualTableCallTarget(callTargetData, thisPtr, newIR);
            break;
        default:
            ASSERTMNR(UNREACHED, "Unexpected call kind");
            target = NULL;
    }

    callTargetData->callTargetNode = target;
}

// Generate the target for a direct call. "Direct call" can either be
//  true direct calls if the runtime allows, or they can be indirect
//  calls through the method descriptor.
IRNode *
ReaderBase::rdrGetDirectCallTarget(ReaderCallTargetData *callTargetData, IRNode **newIR)
{
    CORINFO_CONST_LOOKUP addrInfo;
    CORINFO_METHOD_HANDLE hMethod = callTargetData->getMethodHandle();
    getFunctionEntryPoint(hMethod,IAT_VALUE,&addrInfo,
        callTargetData->fNeedsNullCheck ? CORINFO_ACCESS_NONNULL : CORINFO_ACCESS_ANY);

    IRNode *targetNode;
    if ((addrInfo.accessType==IAT_VALUE) &&
#ifdef CC_JIT_LIMITEDDIRECT
        (addrInfo.accessModule==IAM_CURRENT_MODULE) &&
#endif
        GenIR_CanMakeDirectCall(callTargetData))
    {
        targetNode = GenIR_MakeDirectCallTargetNode(hMethod,addrInfo.addr);
    }
    else
    {
        bool fIndirect = (addrInfo.accessType!=IAT_VALUE) ? true : false;
        targetNode = GenIR_HandleToIRNode(callTargetData->getMethodToken(),addrInfo.addr,0,fIndirect,fIndirect,true,fIndirect,newIR);

        if (addrInfo.accessType==IAT_PPVALUE) {
            targetNode = GenIR_DerefAddress(targetNode,false,newIR);
        }
        // "indirect" direct calls use method desc
        callTargetData->fUsesMethodDesc = true;
    }
    
    return targetNode;
}

// Generate a runtime lookup for the call target. This should only be done when
//  calling a shared method. This shared entry point actually invokes an IL
//  stub that will push the correct type arg and then call the actual target.
// This lookup is avoided if the JIT requests the true entry point. In that
//  case the runtime will return the true entry point aperrnd expect the JIT
//  to call it with the typeArg parameter directly.
IRNode *
ReaderBase::rdrGetCodePointerLookupCallTarget(ReaderCallTargetData *callTargetData, IRNode **newIR)
{
    CORINFO_CALL_INFO *callInfo = callTargetData->getCallInfo();
    // The EE has asked us to call by computing a code pointer
    //  and then doing an indirect call. This is because a
    //  runtime lookup is required to get the code entry point.
    ASSERTNR(callInfo);

    if (callInfo == NULL) {
        return NULL;
    }

    ASSERTNR(callInfo->codePointerLookup.lookupKind.needsRuntimeLookup);

    // treat as indirect call
    callTargetData->fIndirect = true;

    // These calls always follow a uniform calling convention
    ASSERTNR(!callTargetData->getSigInfo()->hasTypeArg());
    ASSERTNR(!callTargetData->getSigInfo()->isVarArg());
    
    return GenIR_RuntimeLookupToNode(callInfo->codePointerLookup.lookupKind.runtimeLookupKind,
                                     &callInfo->codePointerLookup.runtimeLookup, newIR);
}

// This is basically a runtime look up for virtual calls. A JIT helper
//  call is invoked at runtime which finds the virutal call target. The
//  return value of this helper call is then called indirectly.
IRNode *
ReaderBase::rdrGetIndirectVirtualCallTarget(ReaderCallTargetData *callTargetData, IRNode **thisPtr, IRNode **newIR)
{
    IRNode *classHandle = callTargetData->getClassHandleNode(newIR);
    IRNode *methodHandle = callTargetData->getMethodHandleNode(newIR);
    
    ASSERTMNR(!callTargetData->getSigInfo()->isVarArg(), "varargs + generics is not supported\n");
    ASSERTNR(thisPtr); // ensure we have a this pointer
    ASSERTNR(classHandle);
    ASSERTNR(methodHandle);
    
    // treat as indirect call
    callTargetData->fIndirect = true;

    // We need to make a copy because the "this"
    // pointer will be used twice:
    //     1) to look up the virtual function
    //     2) to call the method itself
    IRNode *thisPtrCopy;
    GenIR_Dup(*thisPtr,&thisPtrCopy,thisPtr,newIR);
    
    // Get the address of the target function by calling helper
    IRNode *dst = GenIR_MakePtrNode();
    return GenIR_CallHelper(CORINFO_HELP_VIRTUAL_FUNC_PTR, dst, newIR,
                                thisPtrCopy, classHandle, methodHandle);
}

// Generate the target for a virtual stub dispatch call. This is the
//  normal path for a virtual call.
IRNode *
ReaderBase::rdrGetVirtualStubCallTarget(ReaderCallTargetData *callTargetData, IRNode **newIR)
{
    CORINFO_CALL_INFO *callInfo = callTargetData->getCallInfo();
    ASSERTNR(callInfo);

    if (callInfo == NULL) {
        return NULL;
    }

    IRNode *indirectionCell, *indirectionCellCopy;
    if (callInfo->stubLookup.lookupKind.needsRuntimeLookup)
    {
        indirectionCell = GenIR_RuntimeLookupToNode(callInfo->stubLookup.lookupKind.runtimeLookupKind,
                                                        &callInfo->stubLookup.runtimeLookup,
                                                        newIR);
    }
    else
    {
        ASSERTNR(callInfo->stubLookup.constLookup.accessType == IAT_PVALUE);
        indirectionCell = GenIR_HandleToIRNode(callTargetData->getMethodToken(),
                                               callInfo->stubLookup.constLookup.addr,0,false,false,true,false,newIR);
    }

    // For Stub Dispatch we need to pass the address
    //  of the indirection cell as a secret param
    GenIR_Dup(indirectionCell,&indirectionCellCopy,&indirectionCell,newIR);
    callTargetData->setIndirectionCellNode(indirectionCellCopy);

    // One indrection leads to the target
    return GenIR_DerefAddress(indirectionCell,false,newIR);   
}

// Generate the target for a virtual call that will use the
//  virtual call table. This code path is deprecated and the
//  runtime no longer will ask the JIT to do this.
IRNode *
ReaderBase::rdrGetVirtualTableCallTarget(ReaderCallTargetData *callTargetData, IRNode **thisPtr, IRNode **newIR)
{
    // We need to make a copy because the "this" pointer
    IRNode *thisPtrCopy;
    GenIR_Dup(*thisPtr,&thisPtrCopy,thisPtr,newIR);

    // VTable call uses method desc
    callTargetData->fUsesMethodDesc = true;

    IRNode *vtableAddr = GenIR_DerefAddress(thisPtrCopy,false,newIR);

    DWORD classAttribs = callTargetData->getClassAttribs();
    if ((classAttribs & CORINFO_FLG_INTERFACE) != 0) {
        vtableAddr = GenIR_getAddressOfInterfaceVtable(vtableAddr, callTargetData->getClassHandle(), newIR);
    }

    // Get the VTable offset of the method.
    unsigned long offset = getMethodVTableOffset(callTargetData->getMethodHandle());
    IRNode *offsetNode = GenIR_LoadConstantI4(offset, newIR);

    IRNode *vtableSlot = GenIR_BinaryOp(ReaderBaseNS::ADD,vtableAddr,offsetNode,newIR);
    return GenIR_DerefAddress(vtableSlot,false,newIR);
}


// Generate the target for a delegate invoke.
IRNode *
ReaderBase::rdrGetDelegateInvokeTarget(ReaderCallTargetData *callTargetData, IRNode **thisPtr, IRNode **newIR)
{
    ASSERTNR(callTargetData->hasThis());

    IRNode *thisPtrCopy, *addressNode;
    GenIR_Dup(*thisPtr,&thisPtrCopy,thisPtr,newIR);

    // Delegate invoke uses method desc
    callTargetData->fUsesMethodDesc = true;

    CORINFO_EE_INFO eeInfo;
    m_jitInfo->getEEInfo(&eeInfo);
    unsigned instance       = eeInfo.offsetOfDelegateInstance;
    unsigned targetPtrValue = eeInfo.offsetOfDelegateFirstTarget;
    IRNode *instanceNode       = GenIR_LoadConstantI4(instance, newIR);
    IRNode *targetPtrValueNode = GenIR_LoadConstantI4(targetPtrValue, newIR);

    // Create a new this pointer
    addressNode = GenIR_BinaryOp(ReaderBaseNS::ADD,*thisPtr,instanceNode,newIR);
    *thisPtr = GenIR_DerefAddress(addressNode,true,newIR);

    // Locate the call target
    addressNode = GenIR_BinaryOp(ReaderBaseNS::ADD,thisPtrCopy,targetPtrValueNode,newIR);
    return GenIR_DerefAddress(addressNode,false,newIR);
}

bool
ReaderBase::rdrCallIsDelegateConstruct(ReaderCallTargetData *callTargetData)
{
    if (callTargetData->isNewObj())
    {
        DWORD classAttribs  = callTargetData->getClassAttribs();
        DWORD methodAttribs = callTargetData->getMethodAttribs();

        if (((classAttribs  & CORINFO_FLG_DELEGATE   ) != 0) &&
            ((methodAttribs & CORINFO_FLG_CONSTRUCTOR) != 0))
        {
            ASSERTNR(!(methodAttribs & CORINFO_FLG_STATIC)); // Implied by NewObj
            return true;
        }
    }
    return false;
}

void
ReaderBase::clearStack(
    IRNode** newIR
){
    while (!m_readerStack->empty()) {
        GenIR_Pop( m_readerStack->pop(), newIR);
    }
}

//
// Function: handleNonEmptyStack
//
// Function is called to propagate live operand stack elements across block
// boundaries. After this function has run all successors S, all the predecessors
// S' of S, and all successors of S' will be populated with copies of the same
// operand stack.
//
// For this reason, it should not be possible for this function to encounter a
// successor list that is partially populated.
// 
// Copy operand stack onto all edges in the current web.
//
//  1. If a successor is unpopulated -> no successors are populated
//     {
//  2.   Construct propagatable operand stack from current reader stack
//  3.   Push current block onto worklist.
//  4.   While worklist is non-empty
//       {
//  5.     block = worklist.pop();
//  6.     For each successor of block
//         {
//  7.       If operand stack is NULL
//           {
//  8.         push all predecessors onto worklist.
//  9.         populate operand stack.
//           }
//         }
//       }
//     }
// 10. Convert and assign reader stack operands to successor stack operands.
void
ReaderBase::handleNonEmptyStack(
    FlowGraphNode* fg,
    IRNode** newIR,
    bool *fmbassign
){
    FlowGraphEdgeList *successorList;
    FlowGraphNode* succBlock;
    ReaderStackIterator *pIter;
    IRNode *cur;

#ifndef NODEBUG
    bool sawNonNull, sawNull;
    sawNonNull = false;
    sawNull = false;
    successorList = FgNodeGetSuccessorList(fg);
    successorList = FgEdgeListGetNextSuccessorActual(successorList);
    while (successorList != NULL) {
        succBlock = FgEdgeListGetSink(successorList);
        if (FgNodeGetOperandStack(succBlock) != NULL) {
            sawNonNull = true;
        } else {
            sawNull = true;
        }
        successorList = FgEdgeListGetNextSuccessorActual(successorList);
    }

    ASSERTNR(!(sawNonNull && sawNull));
#endif // NODEBUG

    // Obtain a successor list (that isn't an EH edge).
    successorList = FgNodeGetSuccessorListActual(fg);

    // If there were no non-eh successors then we are done.
    if (successorList == NULL) {
        // UNREACHED?
        clearStack(newIR);
        return;
    }
    succBlock = FgEdgeListGetSink(successorList);

    ReaderStack* succStack = FgNodeGetOperandStack(succBlock);

    // 1. If successor is unpopulated (hence all successors are unpopulated)
    // then we must construct an operand stack using the current reader stack.
    if (succStack == NULL) {
        ReaderStackIterator *pTargetIter;

        // 2. Create stack typed stack of tmpvars.
        succStack = m_readerStack->copy();

        // Push new elements onto temp stack.
        cur = m_readerStack->getIterator(&pIter);
        succStack->getIterator(&pTargetIter);
        while (cur != NULL) {
            succStack->iteratorReplace(&pTargetIter,GenIR_MakeStackTypeNode( cur));
            cur = m_readerStack->iteratorGetNext(&pIter);
            succStack->iteratorGetNext(&pTargetIter);
        }
        

        // Copy operand stack onto all edges in the current web.

        //  3.   Push current block onto worklist.
        //  4.   While worklist is non-empty
        //  5.     block = worklist.pop();
        //  6.     For each successor of block
        //  7.       If operand stack is NULL
        //  8.         push all predecessors onto worklist.
        //  9.         populate operand stack.

        //  3. Push current block onto worklist
        FlowGraphNodeList* pWorkList, *pNewBlockList, *pDeadList;
        
        pDeadList = NULL;
        pWorkList = NULL;

        pNewBlockList = (FlowGraphNodeList*)getTempMemory(sizeof(FlowGraphNodeList));
        pNewBlockList->block = fg;
        pNewBlockList->next = pWorkList;
        pWorkList = pNewBlockList;

        // 4. While worklist is non-empty
        while (pWorkList) {
            FlowGraphNodeList* temp;
            FlowGraphNode *currBlock, *succBlk;
            FlowGraphEdgeList* succList;
                
            // 5. block = worklist.pop()
            currBlock = pWorkList->block;

            temp = pWorkList;

            pWorkList = pWorkList->next;
                
            //    push dead node onto dead list
            temp->next = pDeadList;
            pDeadList = temp;

            // 6. For each (non-EH) successor of block
            succList = FgNodeGetSuccessorListActual(currBlock);
            while (succList != NULL) {

                succBlk = FgEdgeListGetSink(succList);

                // 7. If operand stack is NULL
                // If successor block has not been populated then
                // push all of its predecessors onto the worklist,
                // and populate its operand stack. If it has already
                // been populated then its predecessors have already
                // been put onto the worklist.
                if (FgNodeGetOperandStack(succBlk) == NULL) {
                    FlowGraphEdgeList* predList;

                    // 8. Push all (non-EH) predecessors of succBlk onto worklist
                    predList = FgNodeGetPredecessorListActual(succBlk);
                    while (predList != NULL) {
                        FlowGraphNode *predBlock;

                        // Push onto worklist.
                        predBlock = FgEdgeListGetSource(predList);
                        if (predBlock != currBlock) {
                            FlowGraphEdgeList *predSuccList;
                            
                            predSuccList = FgNodeGetSuccessorListActual (predBlock);
                            if (predSuccList &&
                               (FgNodeGetOperandStack(FgEdgeListGetSink(predSuccList)) == NULL)) {
                                // Attempt to re-use node from dead list
                                if (pDeadList) {
                                    pNewBlockList = pDeadList;
                                    pDeadList = pDeadList->next;
                                } else {
                                    pNewBlockList = (FlowGraphNodeList*)getTempMemory(sizeof(FlowGraphNodeList));
                                }
                                pNewBlockList->block = predBlock;
                                pNewBlockList->next = pWorkList;
                                pWorkList = pNewBlockList;
                            }
                        }
                        predList = FgEdgeListGetNextPredecessorActual(predList);
                    }
                        
                    // 9. Populate operand stack
                    FgNodeSetOperandStack(succBlk, succStack->copy());
                }
                succList = FgEdgeListGetNextSuccessorActual(succList);
            }
        } // end worklist iteration.
    }

#ifndef NODEBUG
    // Ensure that all successors have operand stacks
    successorList = FgNodeGetSuccessorListActual(fg);
    while (successorList != NULL) {
        ReaderStack* sStack;

        succBlock = FgEdgeListGetSink(successorList);
        sStack = FgNodeGetOperandStack(succBlock);
        ASSERTMNR(sStack != NULL, "error, empty operand stack on successor.\n");
        successorList = FgEdgeListGetNextSuccessorActual(successorList);
    }
#endif // !NODEBUG

    // 10. Generate assignments from variables on current operand stack
    // to elements of the operand stack list. Note that until now we've
    // always put copies of the operand stack onto the successor blocks.
    // We've been saving the original to use as a source for nodes to
    // assign to.

    // Assign current stack elements to tmpvars on successor operand stack.
    ReaderStackIterator *pTargetIter;
    IRNode *target;

    cur = m_readerStack->getIterator(&pIter);
    target = succStack->getIterator(&pTargetIter);
    while ((cur != NULL) && (target != NULL)) {
        GenIR_AssignToSuccessorStackNode( fg, target, cur, newIR, fmbassign);
        cur = m_readerStack->iteratorGetNext(&pIter);
        target = succStack->iteratorGetNext(&pTargetIter);
    }


    // OPTIMIZATION: The reader will re-use the reader stack in the
    //  event that there are no stack-carried temporaries; this call is
    //  to makes sure that the stack that gets used will indeed be empty 
    //  if it gets re-used in this manner (It may be guaranteed anyway,
    //  but better safe than sorry).
    m_readerStack->clearStack();
}

void
ReaderBase::initParamsAndAutos(
    unsigned int    nParam,
    unsigned int    nAuto
){
    // Init verification maps
    if (m_verificationNeeded) {
        m_numVerifyParams = nParam;
        if (nParam > 0) {
            m_paramVerifyMap  = (typeInfo*)getProcMemory(nParam * sizeof(LocalDescr));
        }

        m_numVerifyAutos = nAuto;
        if (nAuto > 0) {
            m_autoVerifyMap = (typeInfo*)getProcMemory(nAuto * sizeof(LocalDescr));
        }
    } else {
        m_numVerifyParams = 0;
        m_numVerifyAutos = 0;
    }

    buildUpParams(nParam);
    buildUpAutos(nAuto);
}


//
CORINFO_ARG_LIST_HANDLE
ReaderBase::argListNext(
    CORINFO_ARG_LIST_HANDLE argListHandle,
    CORINFO_SIG_INFO *      sig,
    bool                    fIsLocalArgList,
    CorInfoType *           pCorType,   // default to NULL
    CORINFO_CLASS_HANDLE *  phClass,    // default to NULL
    bool *                  pfIsPinned  // default to NULL
){
    CORINFO_CLASS_HANDLE hClass = NULL, hArgType;
    CORINFO_ARG_LIST_HANDLE nextArg = m_jitInfo->getArgNext(argListHandle);
    CorInfoTypeWithMod corTypeWithMod = m_jitInfo->getArgType(sig, argListHandle, &hArgType);
    CorInfoType corType = strip(corTypeWithMod);

    if (corType == CORINFO_TYPE_CLASS || 
        corType == CORINFO_TYPE_VALUECLASS ||
        (fIsLocalArgList && (corType == CORINFO_TYPE_BYREF))) {
        hClass = m_jitInfo->getArgClass(sig, argListHandle);
    } else if (corType == CORINFO_TYPE_REFANY) {
        hClass = m_jitInfo->getBuiltinClass(CLASSID_TYPED_BYREF);
    }

    if (pCorType != NULL) *pCorType = corType;
    if (phClass != NULL) *phClass = hClass;
    if (pfIsPinned != NULL) *pfIsPinned = ((corTypeWithMod & CORINFO_TYPE_MOD_PINNED) != 0);

    return nextArg;
}


void
ReaderBase::buildUpAutos(
    unsigned int numAutos
){
    CORINFO_ARG_LIST_HANDLE locs;
    CorInfoType corType;
    CORINFO_CLASS_HANDLE hClass;
    bool fIsPinned;

    if (numAutos > 0) {
        locs = m_methodInfo->locals.args;

        // Get the types of all of the automatics.
        for (UINT i = 0; i < numAutos; i++) 
        {
            // don't do anything until we've verified the local
            if (m_verificationNeeded) {
                VerifyRecordLocalType(i, &(m_methodInfo->locals), locs);
            }

            locs = argListNext(locs, &(m_methodInfo->locals), true, &corType, &hClass, &fIsPinned);
            GenIR_CreateSym(i,true,corType,hClass,fIsPinned);
        }
    }
}

void
ReaderBase::buildUpParams(
    unsigned int numParams
){
    if (numParams > 0) {
        CORINFO_ARG_LIST_HANDLE nextLoc, locs;
        CORINFO_CLASS_HANDLE hClass;
        CorInfoType corType;

        locs = m_methodInfo->args.args;
        bool fIsVarArg = m_methodInfo->args.isVarArg();
        bool fHasTypeArg = m_methodInfo->args.hasTypeArg();
        unsigned int i = 0;

        // We must check to see if the first argument is a this pointer
        // in which case we have to synthesize it.
        if (m_methodInfo->args.hasThis()) {
            DWORD attribs;
            bool isValClass;
        
            // Get the handle for the class which this method is part of.
            hClass  = getCurrentMethodClass();

            // See if the current class is an valueclass
            attribs = getClassAttribs(hClass);

            if ((attribs & CORINFO_FLG_VALUECLASS) == 0) {
                isValClass = false;
                corType = CORINFO_TYPE_CLASS;
            }else{
                isValClass = true;
                corType = CORINFO_TYPE_VALUECLASS;
            }

            if (m_verificationNeeded) 
            {
                VerifyRecordParamType(i, corType, hClass, isValClass, true);
            }

            GenIR_CreateSym(i,false,isValClass?CORINFO_TYPE_BYREF:corType,NULL,false,READER_THIS_PTR);
            i++;
        }

        // For varargs, we have to synthesize the varargs cookie.  This comes after the this pointer (if any)
        // and before any fixed params.
        if (fIsVarArg) {
            // this is not a real arg.  we do not record it for verification
            GenIR_CreateSym(i, false, CORINFO_TYPE_PTR, NULL, false, READER_VARARGS_TOKEN);
            i++;
        }

        // GENERICS: Code Sharing: After varargs, before fixed params comes instParam (typeArg cookie)
        if (fHasTypeArg) {
            // this is not a real arg.  we do not record it for verification
            GenIR_CreateSym(i, false, CORINFO_TYPE_PTR, NULL, false, READER_INST_PARAM);
            i++;
        }

        // Get the types of all of the parameters.
        for (; i < numParams; i++) {

            if (m_verificationNeeded) {
                VerifyRecordParamType(i - (fIsVarArg?1:0) - (fHasTypeArg?1:0), 
                                      &(m_methodInfo->args), locs);
            }

            nextLoc = argListNext(locs, &(m_methodInfo->args), false, &corType, &hClass);
            GenIR_CreateSym(i, false, corType, hClass, false);
            locs = nextLoc;
        }
    }
}

IRNode*
ReaderBase::GetThreadControlBlock(
    IRNode **newIR
){
    // If TCB has already been obtained by a dominating block then
    // use the old value.
    IRNode *nodeTHB = DomInfo_DominatorHasTCB(m_currentFgNode);
    if (nodeTHB == NULL) {
        // Make the call to populate ThreadControlBlock.
        nodeTHB = GenIR_MakePtrNode();
        GenIR_CallHelper(CORINFO_HELP_GET_THREAD, nodeTHB, newIR);
        // Record the TCB value
        DomInfo_RecordTCB(m_currentFgNode,nodeTHB);
    }
    return nodeTHB;
}


//
//
//
struct FgData{

    // Simple unsorted list for caching sparse information.
    struct FgDataListHash{

        struct hashListNode{
            unsigned key1; // For (1), this is the helperID; for (2) this is the class typeref
            void * key2;   // For (1), this is the moduleID; for (2) this is not used and should be NULL
            void * key3;   // For (1), this is the classID; for (2) this is not used and should be NULL
            void * data;
            hashListNode *next;
        };

        hashListNode* listBase;

        void init(void){listBase = NULL;}

        void insert(ReaderBase *reader, unsigned key1, void* key2, void* key3, void* data){
            hashListNode *newNode;

            newNode = (hashListNode*)reader->getTempMemory(sizeof(hashListNode));
            newNode->key1 = key1;
            newNode->key2 = key2;
            newNode->key3 = key3;
            newNode->data = data;
            newNode->next = listBase;
            listBase = newNode;
        }

        void* get(unsigned key1, void* key2, void* key3){
            hashListNode* node;

            node = listBase;
            while (node != NULL) {
                if ((node->key1 == key1) && (node->key2 == key2) && (node->key3 == key3))
                    return node->data;
                node = node->next;
            }
            return NULL;
        }
    };

    class FgDataArrayHash{
        const static int DATAARRAY_HASH_SIZE = 63;

        FgDataListHash *pHash;

    public:
        void init(void){
            pHash = NULL;
        }

        void insert(ReaderBase *reader, unsigned key1, void* key2, void* key3, void* data){
            if (pHash == NULL) {
                pHash = (FgDataListHash*)reader->getTempMemory(sizeof(FgDataListHash) * DATAARRAY_HASH_SIZE);
            }
            pHash[key1 % DATAARRAY_HASH_SIZE].insert(reader,key1,key2,key3,data);
        }

        void* get(unsigned key1, void* key2, void* key3){
            if (pHash == NULL) {
                return NULL;
            }
            return pHash[key1 % DATAARRAY_HASH_SIZE].get(key1,key2,key3);
        }
    };

    FgDataListHash staticBaseHash, classInitHash;
    IRNode* pThreadControlBlock;

    // Init routine, since this structure will be allocated into a pool.
    // This init is not strictly necessary since the lower structures
    // only want to be initialized to zero.
    void init(void) {
        staticBaseHash.init();
        classInitHash.init();
        pThreadControlBlock = NULL;
    }

    // Getters and setters for properties tracked in FgData.

    // Getters are used via function pointer so must have prototype void* f(mdToken)

    void* getSharedStaticBase(unsigned key1_helperID, void* key2_moduleID, void* key3_classID, void* key4_typeRef){
        void * retVal = staticBaseHash.get(key1_helperID, key2_moduleID, key3_classID);
        if (retVal != NULL)
            return retVal;

        // We didn't find the getter we're looking for, so try some alternatives and gather extra
        // useful information.
        // Specifically, key4_typeRef is set to 0 if we can determine that the class .cctor
        // has already been run.

        unsigned other_helperID;
        switch (key1_helperID) {
        case CORINFO_HELP_GETSHARED_GCSTATIC_BASE:
            other_helperID = CORINFO_HELP_GETSHARED_GCSTATIC_BASE_NOCTOR;
            break;
        case CORINFO_HELP_GETSHARED_NONGCSTATIC_BASE:
            other_helperID = CORINFO_HELP_GETSHARED_NONGCSTATIC_BASE_NOCTOR;
            break;

        case CORINFO_HELP_GETSHARED_GCSTATIC_BASE_NOCTOR:
        case CORINFO_HELP_GETSHARED_NONGCSTATIC_BASE_NOCTOR:
            // The runtime only gives us these helpers if there is no
            // .cctor to run ever.  Thus we don't need to bother looking
            // for the other variant, but we do know that the .cctor never
            // needs to be run.
            *(mdToken*)key4_typeRef = 0;
            return NULL;

        default:
            // If we're still not sure about whether the .cctor has run, check
            // before returning NULL (because there are no other equivalent helpers).
            if (*(mdToken*)key4_typeRef != 0 &&
                classInitHash.get(*(mdToken*)key4_typeRef, NULL, NULL))
            {
                *(mdToken*)key4_typeRef = 0;
            }
            return NULL;
        }

        // We have an equivalent helper, look it up to see if it exists.
        retVal = staticBaseHash.get(other_helperID, key2_moduleID, key3_classID);

        // If we found a NoCtor variant or this class has already had it's
        // .cctor run, then we know the .cctor will not be run again.
        if (retVal != NULL || (*(mdToken*)key4_typeRef != 0 &&
            classInitHash.get(*(mdToken*)key4_typeRef, NULL, NULL)))
        {
            *(mdToken*)key4_typeRef = 0;
        }
        return retVal;
    }

    void* getClassInit(unsigned key1_typeRef, void* key2, void* key3, void* key4){
        ASSERTNR(key2 == NULL && key3 == NULL && key4 == NULL);
        return (classInitHash.get(key1_typeRef, NULL, NULL));
    }

    void* getThreadControlBlock(unsigned key1, void* key2, void* key3, void* key4){
        ASSERTNR(key1 == 0);
        ASSERTNR(key2 == NULL && key3 == NULL && key4 == NULL);
        return pThreadControlBlock;
    }

    void setSharedStaticBase(ReaderBase* reader, CorInfoHelpFunc helperID, void* moduleID, void* classID, IRNode* basePtr){
        staticBaseHash.insert(reader, helperID, moduleID, classID, basePtr);
    }

    void setClassInit(ReaderBase* reader, mdToken typeRef){
        classInitHash.insert(reader, typeRef, NULL, NULL, (void*)1);
    }

    void setThreadControlBlock(ReaderBase* reader, IRNode* pTCB){
        pThreadControlBlock = pTCB;
    }

};


void
ReaderBase::initBlockArray(
    unsigned blockCount
){
    m_blockArray = (FgData**)getTempMemory(blockCount * sizeof(void*));
}

// Shared routine obtains existing block data for block,
// If doCreate is true then create block data if it isn't present.
FgData*
ReaderBase::DomInfo_GetBlockData(
    FlowGraphNode *fg,
    bool           doCreate
){
    FgData* fgData;
    unsigned blockNum;

    if(m_blockArray == NULL) return NULL;

    blockNum = FgNodeGetBlockNum(fg);
    ASSERTNR(blockNum != (unsigned)-1);

    fgData = m_blockArray[blockNum];
    if(!fgData && doCreate) {
        fgData = (FgData*)getTempMemory(sizeof(FgData));
        m_blockArray[blockNum] = fgData;
    }

    return fgData;
}

// Shared routine checks current block and all dominators for first 
// non-null response from FgData method.
void*
ReaderBase::DomInfo_GetInfoFromDominator(
    FlowGraphNode* fg,
    unsigned       key1,
    void *         key2,
    void *         key3,
    void *         key4,
    bool           requireSameRegion,
    void *(FgData::*pmfn)(unsigned key1, void* key2, void* key3, void* key4)
){
    FgData*         fgData;
    void*           retVal;
    FlowGraphNode*  fgCurrent;

    retVal = NULL;
    fgCurrent = fg;

    do {
        if ( ! (requireSameRegion
                && FgNodeGetRegion(fg) != FgNodeGetRegion(fgCurrent)))
        {
            // if we get an operand back then we have succeeded,
            if ((fgData = DomInfo_GetBlockData(fgCurrent, false)) && (retVal = (fgData->*pmfn)(key1,key2,key3,key4)))
                break;
        }
        fgCurrent = GenIR_FgNodeGetIDom(fgCurrent);
    } 
    while (fgCurrent);

    return retVal;
}


// Returns node that holds previously calculated shared static base
// address, otherwise NULL.
IRNode*
ReaderBase::DomInfo_DominatorDefinesSharedStaticBase(
    FlowGraphNode*   fg,
    CorInfoHelpFunc& helperID,
    void*            moduleID,
    void*            classID,
    mdToken          typeRef,
    bool*            pfNoCtor
){
    IRNode* retVal;
    mdToken copyTypeRef = typeRef;
    ASSERTNR(typeRef != 0);
    *pfNoCtor = false;

    if (GenIR_GenerateDebugCode())
        return NULL;

    retVal = (IRNode*)DomInfo_GetInfoFromDominator(fg,helperID,moduleID,classID,&copyTypeRef,true,&FgData::getSharedStaticBase);
    if (copyTypeRef == 0) {
        *pfNoCtor = true;
        if (retVal == NULL) {
            switch (helperID) {
            case CORINFO_HELP_GETSHARED_GCSTATIC_BASE:
                helperID = CORINFO_HELP_GETSHARED_GCSTATIC_BASE_NOCTOR;
                break;
            case CORINFO_HELP_GETSHARED_NONGCSTATIC_BASE:
                helperID = CORINFO_HELP_GETSHARED_NONGCSTATIC_BASE_NOCTOR;
                break;
            default:
                break;
            }
        }
    }
    return retVal;
}


// DomInfo getters/setters

// Records that fg has calculated shared static base.
void
ReaderBase::DomInfo_RecordSharedStaticBaseDefine(
    FlowGraphNode*  fg,
    CorInfoHelpFunc helperID,
    void*           moduleID,
    void*           classID,
    IRNode*         basePtr
){
    if (GenIR_GenerateDebugCode())
        return;

    FgData* fgData = DomInfo_GetBlockData(fg,true);
    if (fgData != NULL) {
        fgData->setSharedStaticBase(
            this, helperID, moduleID, classID, basePtr);
    }
}


// Returns whether particular class has already been initialized
// by current block, or any of its dominators.
bool
ReaderBase::DomInfo_DominatorHasClassInit(
    FlowGraphNode* fg,
    mdToken        typeRef
){
    if (GenIR_GenerateDebugCode())
        return false;

    bool retVal;

    retVal = (DomInfo_GetInfoFromDominator(fg,typeRef, NULL, NULL, NULL, false, &FgData::getClassInit)!=NULL);
    return retVal;
}


// Records that current block has initialized class typeRef.
void
ReaderBase::DomInfo_RecordClassInit(
    FlowGraphNode* fg,
    mdToken        typeRef
){
    if (GenIR_GenerateDebugCode())
        return;

    FgData* fgData = DomInfo_GetBlockData(fg,true);
    if (fgData != NULL) {
        fgData->setClassInit(this, typeRef);
    }
}

// Returns whether particular class has already been initialized
// by current block, or any of its dominators.
IRNode*
ReaderBase::DomInfo_DominatorHasTCB(
    FlowGraphNode* fg
){
    if (GenIR_GenerateDebugCode())
        return NULL;

    IRNode* retVal;
    retVal = (IRNode*)DomInfo_GetInfoFromDominator(fg,0,NULL,NULL,NULL,true,&FgData::getThreadControlBlock);
    return retVal;
}

// Records that current block has initialized class typeRef.
void
ReaderBase::DomInfo_RecordTCB(
    FlowGraphNode* fg,
    IRNode*        pTCB
){
    if (GenIR_GenerateDebugCode())
        return;

    FgData* fgData = DomInfo_GetBlockData(fg,true);
    if (fgData != NULL) {
        fgData->setThreadControlBlock(this, pTCB);
    }
}


// =================================================================
// End DOMINFO
// =================================================================


// Default routine to insert verification throw.
void
ReaderBase::GenIR_InsertThrow(
    CorInfoHelpFunc throwHelper,
    unsigned int offset,
    IRNode **newIR
){
    IRNode *intConstant = GenIR_LoadConstantI4(offset, newIR);
    GenIR_CallHelper(throwHelper, NULL, newIR, intConstant);
}


// Macro used by main reader loop for distinguishing verify-only passes
#define BREAK_ON_VERIFY_ONLY if(isVerifyOnly || fLocalFault) break

LONG ObjectFilter(PEXCEPTION_POINTERS pExceptionPointers, LPVOID lpvParam)
{
    ReaderException ** pExcep = (ReaderException **)lpvParam;

    if (pExceptionPointers->ExceptionRecord->ExceptionCode == JIT64_READEREXCEPTION_CODE)
    {
        (*pExcep) = *(ReaderException **)pExceptionPointers->ExceptionRecord->ExceptionInformation;
        return EXCEPTION_EXECUTE_HANDLER;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

bool
ReaderBase::isUnmarkedTailCall(
    BYTE        *buf,
    unsigned int          bufSize,
    unsigned int          nextOffset,
    mdToken               token
)
{
    bool doTailCallOpt = false;
      
    PAL_TRY
    {
        doTailCallOpt = isUnmarkedTailCallHelper(buf, bufSize, nextOffset, token);
    }
    PAL_EXCEPT_FILTER(EEJITFilter, m_jitInfo)
    {
        doTailCallOpt = false;
    }
    PAL_ENDTRY;

    return doTailCallOpt;
}

// Determine if the current instruction is an unmarked tail call.
// We check that:
//     1) The next instruction is a return
//     2) The return type from the current function matches the
//        return type of the called function.
//         
// NOTE: Other necessary checks are performed later
bool
ReaderBase::isUnmarkedTailCallHelper(
    BYTE        *buf,
    unsigned int          bufSize,
    unsigned int          nextOffset,
    mdToken               token
)
{
    // Get the next instruction (if any)
    unsigned char* dummyOperand;
    unsigned int dummyNextOffset = nextOffset;
    ReaderBaseNS::OPCODE opcode;
    
    do {
        opcode = ParseMSILOpcodeSafely(buf, dummyNextOffset, bufSize,
                              &dummyOperand, &dummyNextOffset, this, false);
    } while (dummyNextOffset < bufSize && opcode == ReaderBaseNS::CEE_NOP);

    if (opcode == ReaderBaseNS::CEE_RET) {
        // Check the return types of the two functions
        CORINFO_METHOD_HANDLE hTargetMethod = getMethodHandle(token);
        ASSERTNR(hTargetMethod);
        CORINFO_METHOD_HANDLE hCurrMethod = getCurrentMethodHandle();
        ASSERTNR(hCurrMethod);

        CORINFO_SIG_INFO sigTarget;
        getMethodSig(hTargetMethod, &sigTarget);
        CorInfoType retTypeTarget = sigTarget.retType;
        CORINFO_CLASS_HANDLE hRetClassTarget = sigTarget.retTypeClass;

        CORINFO_SIG_INFO sigCurrMethod;
        getMethodSig(hCurrMethod, &sigCurrMethod);
        CorInfoType retTypeCurr = sigCurrMethod.retType;
        CORINFO_CLASS_HANDLE hRetClassCurr = sigCurrMethod.retTypeClass;

        if (   retTypeTarget == CORINFO_TYPE_VALUECLASS
            || retTypeCurr == CORINFO_TYPE_VALUECLASS) {

            return false;
        }

        if (   retTypeTarget != retTypeCurr
            || hRetClassTarget != hRetClassCurr) {

            return false;
        }

#ifndef NODEBUG
        if (!checkTailCallMax()) {
            return false;
        }
#endif

//        printf("****** FOUND UNMARKED TAIL CALL ******\n"));
        return true;
    }

    return false;
}

// Main reader loop, called once for each reachable block.
void
ReaderBase::readBytesForFlowGraphNode(
    FlowGraphNode*        fg,
    bool                  isVerifyOnly
){
    IRNode               *irStartNode;
    IRNode              **newIR = NULL;      // Used in a trace pr post process
    ReaderBaseNS::OPCODE  opcode = ReaderBaseNS::CEE_ILLEGAL;
    IRNode               *arg1;
    IRNode               *arg2;
    IRNode               *arg3;
    IRNode               *ResultIR;
    unsigned int          currOffset = 0, nextOffset;
    unsigned int          targetOffset;
    mdToken               token;
    ReaderAlignType       alignmentPrefix = READER_ALIGN_UNKNOWN;
    bool                  fVolatilePrefix = false;
    bool                  fTailCallPrefix = false;
    bool                  fReadOnlyPrefix = false;
    bool                  fConstrainedPrefix = false;
    mdToken               constraintTypeRef = mdTokenNil;
    mdToken               loadFtnToken = mdTokenNil;
    bool                  fLocalFault = false;
    bool                  fHasFallThrough = false;
    bool                  fVerifiedEndBlock = false;
    unsigned char        *operand;
    unsigned char        *msilBytes = NULL;

    VerificationState    *vstate = NULL;
    ReaderException      *type;

    int                   mappedValue;

    // Quick check that the array is initialized correctly.
    ASSERTNR(opcodeRemap[ReaderBaseNS::CEE_CLT_UN] == ReaderBaseNS::CLT_UN);

    // Initialize local information
    m_currentFgNode = fg;
    m_currentRegion = FgNodeGetRegion(fg);
    // Get copy of operand stack for this block.
    // OPTIMIZATION: Continue using the existing stack in the common case
    // where it is left empty at the end of the block.
    if(!isVerifyOnly) {
        ReaderStack * temp = FgNodeGetOperandStack(fg);
        if (temp)
            m_readerStack = temp->copy();
    }

    // Find the offset at which to start reading the buffer
    currOffset = FgNodeGetStartMSILOffset(fg);

    ASSERTNR(!m_verificationNeeded || 
             (currOffset == FgNodeGetEndMSILOffset(fg)) || IsOffsetInstrStart(currOffset));

    GenIR_BeginFlowGraphNode(fg, currOffset, isVerifyOnly);

    // Get IRNode insertion point for this block and
    // init newIR
    irStartNode = FgNodeGetEndIRInsertionPoint(fg);
    newIR = &irStartNode;

    if(!isVerifyOnly) {
        setupBlockForEH(newIR);
    }

    PAL_TRY
    {
        vstate = verifyInitializeBlock(fg, currOffset);

        alignmentPrefix = READER_ALIGN_NATURAL;
        fVolatilePrefix = false;
        fTailCallPrefix = false;
        fLocalFault = false;
        fHasFallThrough = true;
        fVerifiedEndBlock = false;
        
        msilBytes = m_methodInfo->ILCode;
        nextOffset = currOffset;
        
        while (currOffset < FgNodeGetEndMSILOffset(fg)) 
        {
            ReaderBaseNS::OPCODE prevOp = opcode;
            opcode = ParseMSILOpcode(msilBytes + currOffset,&operand,&nextOffset, this);

            // If we have cached a loadFtnToken from LDFTN or LDVIRTFTN
            // then clear it if the next opcode is not NEWOBJ
            if (opcode != ReaderBaseNS::CEE_NEWOBJ)
            {
                loadFtnToken = mdTokenNil;
            }

            verInstrStartOffset = currOffset;
            verInstrOpcode = opcode;

            // SEQUENCE POINTS
            if(!isVerifyOnly && GenIR_NeedSequencePoints()) {
                GenIR_SequencePoint(currOffset,prevOp,newIR);
            }

#if !defined(NODEBUG)
            GenIR_OpcodeDebugPrint(msilBytes, currOffset, nextOffset);
#endif

            mappedValue = opcodeRemap[opcode];

            // Switch on msil opcode
            switch(opcode) {
            case ReaderBaseNS::CEE_ADD_OVF_UN:
            case ReaderBaseNS::CEE_ADD:
            case ReaderBaseNS::CEE_ADD_OVF:
            case ReaderBaseNS::CEE_AND:
            case ReaderBaseNS::CEE_DIV:
            case ReaderBaseNS::CEE_DIV_UN:
            case ReaderBaseNS::CEE_MUL:
            case ReaderBaseNS::CEE_MUL_OVF:
            case ReaderBaseNS::CEE_MUL_OVF_UN:
            case ReaderBaseNS::CEE_OR:
            case ReaderBaseNS::CEE_REM:
            case ReaderBaseNS::CEE_REM_UN:
            case ReaderBaseNS::CEE_SUB:
            case ReaderBaseNS::CEE_SUB_OVF:
            case ReaderBaseNS::CEE_SUB_OVF_UN:
            case ReaderBaseNS::CEE_XOR:
                VerifyBinary(vstate, opcode);
                BREAK_ON_VERIFY_ONLY;

                arg2 = m_readerStack->pop();
                arg1 = m_readerStack->pop();
                ResultIR = GenIR_BinaryOp(
                                          (ReaderBaseNS::BinaryOpcode)mappedValue,
                                          arg1,arg2,newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_ARGLIST:
                VerifyArgList(vstate);
                BREAK_ON_VERIFY_ONLY;

                ResultIR = GenIR_ArgList(newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_BOX:
                token = ReadToken(operand);
                VerifyBox(vstate, token);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop(); // Pop the valuetype we need to box
                GenIR_RemoveStackInterference(newIR);
                ResultIR = GenIR_Box(token,arg1,newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_BEQ:
            case ReaderBaseNS::CEE_BGE:
            case ReaderBaseNS::CEE_BGE_UN:
            case ReaderBaseNS::CEE_BGT:
            case ReaderBaseNS::CEE_BGT_UN:
            case ReaderBaseNS::CEE_BLE:
            case ReaderBaseNS::CEE_BLE_UN:
            case ReaderBaseNS::CEE_BLT:
            case ReaderBaseNS::CEE_BLT_UN:
            case ReaderBaseNS::CEE_BNE_UN:
                targetOffset = nextOffset + ReadInt32(operand);
                goto GEN_COND_BRANCH;

            case ReaderBaseNS::CEE_BEQ_S:
            case ReaderBaseNS::CEE_BGE_S:
            case ReaderBaseNS::CEE_BGE_UN_S:
            case ReaderBaseNS::CEE_BGT_S:
            case ReaderBaseNS::CEE_BGT_UN_S:
            case ReaderBaseNS::CEE_BLE_S:
            case ReaderBaseNS::CEE_BLE_UN_S:
            case ReaderBaseNS::CEE_BLT_S:
            case ReaderBaseNS::CEE_BLT_UN_S:
            case ReaderBaseNS::CEE_BNE_UN_S:
                targetOffset = nextOffset + ReadInt8(operand);
                goto GEN_COND_BRANCH;

            GEN_COND_BRANCH:
                VerifyCompare(vstate, opcode);
                fVerifiedEndBlock = true;
                verifyFinishBlock(vstate, fg); // before GenIR_MaintainOperandStack
                BREAK_ON_VERIFY_ONLY;

                arg2 = m_readerStack->pop();
                arg1 = m_readerStack->pop();

                if (!m_readerStack->empty()) {
                    GenIR_MaintainOperandStack( &arg1, &arg2, newIR);
                    m_readerStack->clearStack();
                }

                // First pass create the branch and target to label
                // In JIT64 last tuple is branch and label gets you target flowgraph node.
                // So the only thing we inflict is that a label contains a pointer to an fgNode
                GenIR_CondBranch((ReaderBaseNS::CondBranchOpcode)mappedValue, arg1, arg2,newIR);
                break;

            case ReaderBaseNS::CEE_BRTRUE:
            case ReaderBaseNS::CEE_BRFALSE:
                targetOffset = nextOffset + ReadInt32(operand);
                goto GEN_BOOL_BRANCH;

            case ReaderBaseNS::CEE_BRTRUE_S:
            case ReaderBaseNS::CEE_BRFALSE_S:
                targetOffset = nextOffset + ReadInt8(operand);
                goto GEN_BOOL_BRANCH;

            GEN_BOOL_BRANCH:
                VerifyBoolBranch(vstate, nextOffset, targetOffset);
                fVerifiedEndBlock = true;
                verifyFinishBlock(vstate, fg); // before GenIR_MaintainOperandStack
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop();
                if (!m_readerStack->empty()) {
                    GenIR_MaintainOperandStack( &arg1, NULL, newIR);
                    m_readerStack->clearStack();
                }
                GenIR_BoolBranch((ReaderBaseNS::BoolBranchOpcode)mappedValue, arg1,newIR);
                break;

            case ReaderBaseNS::CEE_BR:
                targetOffset = nextOffset + ReadInt32(operand);
                goto GEN_BRANCH;

            case ReaderBaseNS::CEE_BR_S:
                targetOffset = nextOffset + ReadInt8(operand);
                goto GEN_BRANCH;

            GEN_BRANCH:
                fHasFallThrough = false;
                fVerifiedEndBlock = true;
                verifyFinishBlock(vstate, fg); // before GenIR_MaintainOperandStack
                BREAK_ON_VERIFY_ONLY;

                // Assumes first pass created branch label, here we just assist
                // any live stack operands across the block boundary.
                if (!m_readerStack->empty()) {
                    GenIR_MaintainOperandStack( NULL, NULL, newIR);
                    m_readerStack->clearStack();
                }
                GenIR_Branch(newIR);
                break;

            case ReaderBaseNS::CEE_BREAK:
                // CEE_BREAK is always verifiable
                BREAK_ON_VERIFY_ONLY;
                GenIR_Break(newIR);
                break;

            case ReaderBaseNS::CEE_CALL:
            case ReaderBaseNS::CEE_CALLI:
            case ReaderBaseNS::CEE_CALLVIRT:
                {
                    bool fIsUnmarkedTailCall = false;
                    token = ReadToken(operand);

                    VerifyCall(vstate,opcode,
                               fTailCallPrefix,fReadOnlyPrefix,fConstrainedPrefix,m_thisPtrModified,
                               constraintTypeRef,token);
                    BREAK_ON_VERIFY_ONLY;

                    if (!fTailCallPrefix &&
                        opcode != ReaderBaseNS::CEE_CALLI &&
                        doTailCallOpt() &&
                        isUnmarkedTailCall(msilBytes, m_methodInfo->ILCodeSize, nextOffset, token)) {
                        fTailCallPrefix = true;
                        fIsUnmarkedTailCall = true;
                    }
                    ResultIR = GenIR_Call((ReaderBaseNS::CallOpcode)mappedValue,token,constraintTypeRef,
                                            fReadOnlyPrefix, fTailCallPrefix, fIsUnmarkedTailCall, newIR);
                    if (ResultIR != NULL) {
                         m_readerStack->push(ResultIR, newIR);
                    }
                }
                break;

            case ReaderBaseNS::CEE_CASTCLASS:
                token = ReadToken(operand);
                VerifyCastClass(vstate,token);
                BREAK_ON_VERIFY_ONLY;

                {
                    CorInfoHelpFunc helperId;
                    if (rdrCanOptimizeCastClass(token, &helperId))
                    {
                        if (!m_readerStack->empty()) {
                            GenIR_MaintainOperandStack(NULL, NULL, newIR);
                            m_readerStack->clearStack();
                        }
                        GenIR_CastClassOptimized(token,newIR);
                    }
                    else
                    {
                        arg1 = m_readerStack->pop();
                        GenIR_RemoveStackInterference(newIR);
                        ResultIR = GenIR_CastClass(token,arg1,newIR,helperId);
                        m_readerStack->push(ResultIR, newIR);
                    }
                }
                break;
                

            case ReaderBaseNS::CEE_ISINST:
                token = ReadToken(operand);
                VerifyIsInst(vstate, token);
                BREAK_ON_VERIFY_ONLY;

                {
                    CorInfoHelpFunc helperId;
                    if (rdrCanOptimizeIsInst(token, &helperId))
                    {
                        if (!m_readerStack->empty()) {
                            GenIR_MaintainOperandStack(NULL, NULL, newIR);
                            m_readerStack->clearStack();
                        }
                        GenIR_IsInstOptimized(token,newIR);
                    }
                    else
                    {
                        arg1 = m_readerStack->pop();
                        GenIR_RemoveStackInterference(newIR);
                        ResultIR = GenIR_IsInst(token,arg1,newIR);
                        m_readerStack->push(ResultIR, newIR);
                    }
                }
                break;

            case ReaderBaseNS::CEE_CEQ:
            case ReaderBaseNS::CEE_CGT:
            case ReaderBaseNS::CEE_CGT_UN:
            case ReaderBaseNS::CEE_CLT:
            case ReaderBaseNS::CEE_CLT_UN:
                VerifyCompare(vstate, opcode);
                BREAK_ON_VERIFY_ONLY;

                arg2 = m_readerStack->pop();
                arg1 = m_readerStack->pop();
                ResultIR = GenIR_Cmp((ReaderBaseNS::CmpOpcode)mappedValue,arg1,arg2,newIR);
                 m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_CKFINITE:
                // Add overflow check for top of stack
                VerifyCkFinite(vstate);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop();
                GenIR_RemoveStackInterference(newIR);
                ResultIR = GenIR_CkFinite(arg1,newIR);
                 m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_CONV_I1:
            case ReaderBaseNS::CEE_CONV_I2:
            case ReaderBaseNS::CEE_CONV_I4:
            case ReaderBaseNS::CEE_CONV_I8:
            case ReaderBaseNS::CEE_CONV_R4:
            case ReaderBaseNS::CEE_CONV_R8:
            case ReaderBaseNS::CEE_CONV_U1:
            case ReaderBaseNS::CEE_CONV_U2:
            case ReaderBaseNS::CEE_CONV_U4:
            case ReaderBaseNS::CEE_CONV_U8:
            case ReaderBaseNS::CEE_CONV_I:
            case ReaderBaseNS::CEE_CONV_U:

            case ReaderBaseNS::CEE_CONV_OVF_I1:
            case ReaderBaseNS::CEE_CONV_OVF_I2:
            case ReaderBaseNS::CEE_CONV_OVF_I4:
            case ReaderBaseNS::CEE_CONV_OVF_I8:
            case ReaderBaseNS::CEE_CONV_OVF_U1:
            case ReaderBaseNS::CEE_CONV_OVF_U2:
            case ReaderBaseNS::CEE_CONV_OVF_U4:
            case ReaderBaseNS::CEE_CONV_OVF_U8:
            case ReaderBaseNS::CEE_CONV_OVF_I:
            case ReaderBaseNS::CEE_CONV_OVF_U:

            case ReaderBaseNS::CEE_CONV_OVF_I1_UN:
            case ReaderBaseNS::CEE_CONV_OVF_I2_UN:
            case ReaderBaseNS::CEE_CONV_OVF_I4_UN:
            case ReaderBaseNS::CEE_CONV_OVF_I8_UN:
            case ReaderBaseNS::CEE_CONV_OVF_U1_UN:
            case ReaderBaseNS::CEE_CONV_OVF_U2_UN:
            case ReaderBaseNS::CEE_CONV_OVF_U4_UN:
            case ReaderBaseNS::CEE_CONV_OVF_U8_UN:
            case ReaderBaseNS::CEE_CONV_OVF_I_UN:
            case ReaderBaseNS::CEE_CONV_OVF_U_UN:
            case ReaderBaseNS::CEE_CONV_R_UN:
                VerifyConvert(vstate, (ReaderBaseNS::ConvOpcode)mappedValue);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop(); //Operand to be converted
                ResultIR = GenIR_Conv((ReaderBaseNS::ConvOpcode)mappedValue,arg1,newIR);
                 m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_CPBLK:
                VerifyFailure(vstate);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop(); //Copy the number of bytes to copy 
                arg2 = m_readerStack->pop(); //Pop the source address
                arg3 = m_readerStack->pop(); //Pop the dest   address
                GenIR_RemoveStackInterference(newIR);
                GenIR_CpBlk(arg1,arg2,arg3,alignmentPrefix,fVolatilePrefix,newIR);
                ResultIR = NULL;
                break;

            case ReaderBaseNS::CEE_CPOBJ:
                token = ReadToken(operand);
                VerifyCpObj(vstate,token);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop(); // Source object
                arg2 = m_readerStack->pop(); // Dest   object
                GenIR_RemoveStackInterference(newIR);
                GenIR_CpObj(token,arg1,arg2,alignmentPrefix,fVolatilePrefix,newIR);
                break;

            case ReaderBaseNS::CEE_DUP:
                {
                    IRNode *result1, *result2;

                    VerifyDup(vstate, msilBytes + currOffset);
                    BREAK_ON_VERIFY_ONLY;

                    arg1 = m_readerStack->pop();
                    GenIR_Dup(arg1,&result1,&result2,newIR);
                    m_readerStack->push(result1,newIR);
                    m_readerStack->push(result2,newIR);
                }
                break;

            case ReaderBaseNS::CEE_ENDFILTER:
                VerifyEndFilter(vstate, nextOffset);
                fHasFallThrough = false;
                BREAK_ON_VERIFY_ONLY;

                // The endfilter instruction is going to turn into
                //  an OPFILTER. An OPFILTER is like an OPRET; it's
                //  source is the return value  of the filter, which
                //  must be one of the following 32-bit values:
                //
                //    EXCEPTION_EXECUTE_HANDLER    (1)
                //    EXCEPTION_CONTINUE_SEARCH    (0)
                //    EXCEPTION_CONTINUE_EXECUTION (-1, not supported in CLR currently)
                arg1 = m_readerStack->pop();   // Pop the object pointer
                GenIR_EndFilter(arg1,newIR);
                clearStack(newIR);
                break;

            case ReaderBaseNS::CEE_ENDFINALLY:
                VerifyEndFinally(vstate);
                fHasFallThrough = false;
                BREAK_ON_VERIFY_ONLY;

                // It is useful to know that the ENDFINALLY was actually reachable 
                // in this finally.  (Some clients, may for example, make cloning
                // decisions based on this.  In UTC/JIT64 we don't clone a finally
                // which has no reachable ENDFINALLY's.)
                RgnSetFinallyEndIsReachable(m_currentRegion, true);
                
                clearStack(newIR);
                break;

            case ReaderBaseNS::CEE_INITOBJ:
                token = ReadToken(operand);
                VerifyInitObj(vstate, token);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop(); // Pop address of object
                GenIR_RemoveStackInterference(newIR);
                GenIR_InitObj(token,arg1,newIR);
                break;

            case ReaderBaseNS::CEE_INITBLK:
                VerifyFailure(vstate);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop();   // Pop the number of bytes
                arg2 = m_readerStack->pop();   // Pop the value to assign to each BYTE
                arg3 = m_readerStack->pop();   // Pop the destination address
                GenIR_RemoveStackInterference(newIR);
                GenIR_InitBlk(arg1,arg2,arg3,alignmentPrefix,fVolatilePrefix,newIR);
                ResultIR = NULL;
                break;

            case ReaderBaseNS::CEE_JMP:
                {
                    CORINFO_METHOD_HANDLE handle;
                    CORINFO_SIG_INFO sig, sig2;
                    bool hasThis;
                    bool hasVarArg;

                    // check before verification otherwise we get different exceptions
                    // badcode vs verification exception depending if verifier is on
                    if (m_currentRegion != NULL 
                        && ReaderBaseNS::RGN_ROOT != RgnGetRegionType( m_currentRegion))
                    {
                        
                        BADCODE(MVER_E_TAILCALL_INSIDE_EH);
                    }

                    VerifyFailure(vstate);
                    fHasFallThrough = false;
                    BREAK_ON_VERIFY_ONLY;

                    token = ReadToken(operand);

                    // The stack must be empty when jmp is reached. If it is not
                    // empty (and verification is off), then pop stack until it
                    // is empty.
                    clearStack(newIR);


                    handle = getMethodHandle(token);
                    getCallSiteSignature(handle,token,&sig,&hasThis);
                    hasVarArg = sig.isVarArg();

                    // While we are at it, make sure that the jump prototype
                    // matches this function's prototype, otherwise it makes
                    // no sense to abandon frame and transfer control.

                    m_jitInfo->getMethodSig(getCurrentMethodHandle(), &sig2);
                    if (sig.numArgs != sig2.numArgs) {
                        //
                        // This is meant to catch illegal use of JMP
                        // While it allows some flexibility in the arguments
                        // that shouldn't really even be allowed, it serves
                        // as a basic sanity check.  It will also catch cases
                        // such as the following bad MSIL where the JMP is
                        // overspecifying the prototype given that it isn't
                        // a true callsite:
                        //
                        //   jmp varargs instance void foo(int,...,int)
                        //
                        BADCODE("signature of jump target inconsistent with current routine\n");
                    }
                
                    GenIR_Jmp((ReaderBaseNS::CallOpcode)mappedValue, token,
                                 hasThis, hasVarArg, newIR);

                    // NOTE: jmp's stack transition shows that no value is placed on the stack
                }
                break;

            case ReaderBaseNS::CEE_LDARG:
                mappedValue = ReadUInt16(operand);
                goto LOAD_ARG;
            case ReaderBaseNS::CEE_LDARG_S:
                mappedValue = ReadUInt8(operand);
                goto LOAD_ARG;
            case ReaderBaseNS::CEE_LDARG_0:
            case ReaderBaseNS::CEE_LDARG_1:
            case ReaderBaseNS::CEE_LDARG_2:
            case ReaderBaseNS::CEE_LDARG_3:
            LOAD_ARG:
                VerifyLdarg(vstate, mappedValue, opcode);
                BREAK_ON_VERIFY_ONLY;

#ifdef READER_ESSA_RENAME
                ResultIR = GenIR_LoadArg(mappedValue,false,newIR,
                                         Ssa_HshMSILScalar(opcode,operand,fg), fg);
#else
                ResultIR = GenIR_LoadArg(mappedValue,false,newIR);
#endif
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_LDLOC:
                mappedValue = ReadUInt16(operand);
                goto LOAD_LOCAL;
            case ReaderBaseNS::CEE_LDLOC_S:
                mappedValue = ReadUInt8(operand);
                goto LOAD_LOCAL;
            case ReaderBaseNS::CEE_LDLOC_0:
            case ReaderBaseNS::CEE_LDLOC_1:
            case ReaderBaseNS::CEE_LDLOC_2:
            case ReaderBaseNS::CEE_LDLOC_3:
            LOAD_LOCAL:
                VerifyLdloc(vstate, mappedValue, opcode);
                BREAK_ON_VERIFY_ONLY;

#ifdef READER_ESSA_RENAME
                ResultIR = GenIR_LoadLocal(mappedValue,newIR,
                                           Ssa_HshMSILScalar(opcode,operand,fg), fg);
#else
                ResultIR = GenIR_LoadLocal(mappedValue,newIR);
#endif

                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_LDARGA:
                {
                    unsigned short u16;
                    
                    u16 = ReadUInt16(operand);
                    VerifyLdarg(vstate, u16, opcode);
                    VerifyLoadAddr(vstate);
                    BREAK_ON_VERIFY_ONLY;

                    ResultIR = GenIR_LoadArgAddress(u16,newIR);
                    m_readerStack->push(ResultIR, newIR);
                }
                break;

            case ReaderBaseNS::CEE_LDARGA_S:
                {
                    unsigned char u8;
                    
                    u8 = ReadUInt8(operand);
                    VerifyLdarg(vstate, u8, opcode);
                    VerifyLoadAddr(vstate);
                    BREAK_ON_VERIFY_ONLY;

                    ResultIR = GenIR_LoadArgAddress(u8,newIR);
                    m_readerStack->push(ResultIR, newIR);
                }
                break;

            case ReaderBaseNS::CEE_LDLOCA:
                {
                    unsigned short u16;
                    
                    u16 = ReadUInt16(operand);
                    VerifyLdloc(vstate, u16, opcode);
                    VerifyLoadAddr(vstate);
                    BREAK_ON_VERIFY_ONLY;

                    ResultIR = GenIR_LoadLocalAddress(u16,newIR);
                    m_readerStack->push(ResultIR, newIR);
                }
                break;

            case ReaderBaseNS::CEE_LDLOCA_S:
                {
                    unsigned char u8;
                    
                    u8 = ReadUInt8(operand);
                    VerifyLdloc(vstate, u8, opcode);
                    VerifyLoadAddr(vstate);
                    BREAK_ON_VERIFY_ONLY;

                    ResultIR = GenIR_LoadLocalAddress(u8,newIR);
                    m_readerStack->push(ResultIR, newIR);
                }
                break;

            case ReaderBaseNS::CEE_LDC_I8:
                VerifyLoadConstant(vstate,opcode);
                BREAK_ON_VERIFY_ONLY;

                ResultIR = GenIR_LoadConstantI8(ReadInt64(operand),newIR);
                m_readerStack->push(ResultIR, newIR);
                break;
            case ReaderBaseNS::CEE_LDC_R4:
                VerifyLoadConstant(vstate,opcode);
                BREAK_ON_VERIFY_ONLY;

                ResultIR = GenIR_LoadConstantR4(ReadF32(operand),newIR);
                m_readerStack->push(ResultIR, newIR);
                break;
            case ReaderBaseNS::CEE_LDC_R8:
                VerifyLoadConstant(vstate,opcode);
                BREAK_ON_VERIFY_ONLY;

                ResultIR = GenIR_LoadConstantR8(ReadF64(operand),newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_LDC_I4:
                mappedValue = ReadInt32(operand);
                goto LOAD_CONSTANT;
            case ReaderBaseNS::CEE_LDC_I4_S:
                mappedValue = ReadInt8(operand);
                goto LOAD_CONSTANT;
            case ReaderBaseNS::CEE_LDC_I4_0:
            case ReaderBaseNS::CEE_LDC_I4_1:
            case ReaderBaseNS::CEE_LDC_I4_2:
            case ReaderBaseNS::CEE_LDC_I4_3:
            case ReaderBaseNS::CEE_LDC_I4_4:
            case ReaderBaseNS::CEE_LDC_I4_5:
            case ReaderBaseNS::CEE_LDC_I4_6:
            case ReaderBaseNS::CEE_LDC_I4_7:
            case ReaderBaseNS::CEE_LDC_I4_8:
            case ReaderBaseNS::CEE_LDC_I4_M1:
            LOAD_CONSTANT:
                VerifyLoadConstant(vstate,opcode);
                BREAK_ON_VERIFY_ONLY;

                ResultIR = GenIR_LoadConstantI4(mappedValue,newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_LDELEM: // (M2 Generics)
                token = ReadToken(operand);
                goto LOAD_ELEMENT;
            case ReaderBaseNS::CEE_LDELEM_I1:
            case ReaderBaseNS::CEE_LDELEM_U1:
            case ReaderBaseNS::CEE_LDELEM_I2:
            case ReaderBaseNS::CEE_LDELEM_U2:
            case ReaderBaseNS::CEE_LDELEM_I4:
            case ReaderBaseNS::CEE_LDELEM_U4:
            case ReaderBaseNS::CEE_LDELEM_I8:
            case ReaderBaseNS::CEE_LDELEM_I:
            case ReaderBaseNS::CEE_LDELEM_R4:
            case ReaderBaseNS::CEE_LDELEM_R8:
            case ReaderBaseNS::CEE_LDELEM_REF:
                token = NULL;
            LOAD_ELEMENT:
                VerifyLoadElem(vstate,opcode,token);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop();
                arg2 = m_readerStack->pop();
                ResultIR = GenIR_LoadElem((ReaderBaseNS::LdElemOpcode)mappedValue,
                                          token,arg1,arg2,newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_LDELEMA:
                token = ReadToken(operand);
                VerifyLoadElemA(vstate, fReadOnlyPrefix, token);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop();
                arg2 = m_readerStack->pop();
                ResultIR = GenIR_LoadElemA(token,arg1,arg2,fReadOnlyPrefix,newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_LDFLD:
                token = ReadToken(operand);
                VerifyFieldAccess(vstate,opcode,token);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop(); // pop load address
                ResultIR = GenIR_LoadField(token, arg1,alignmentPrefix,fVolatilePrefix,newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_LDFTN:
                {
                    CORINFO_METHOD_HANDLE handle;
                    
                    loadFtnToken = ReadToken(operand);
                    VerifyLoadFtn(vstate,opcode,loadFtnToken,msilBytes + currOffset);
                    BREAK_ON_VERIFY_ONLY;

#if defined(CC_CALL_INTERCEPT)
                    handle = getMethodHandle(loadFtnToken);
                    checkCallAuthorization(loadFtnToken, handle, newIR);
#endif
                    CORINFO_LOOKUP addrInfo;
                    getFunctionFixedEntryPointInfo(loadFtnToken,&addrInfo);
                    ResultIR = GenIR_LoadFuncptr(loadFtnToken,&addrInfo,newIR);
                    m_readerStack->push(ResultIR, newIR);
                }
                break;

            case ReaderBaseNS::CEE_LDIND_I1:
            case ReaderBaseNS::CEE_LDIND_U1:
            case ReaderBaseNS::CEE_LDIND_I2:
            case ReaderBaseNS::CEE_LDIND_U2:
            case ReaderBaseNS::CEE_LDIND_I4:
            case ReaderBaseNS::CEE_LDIND_U4:
            case ReaderBaseNS::CEE_LDIND_I8:
            case ReaderBaseNS::CEE_LDIND_I:
            case ReaderBaseNS::CEE_LDIND_R4:
            case ReaderBaseNS::CEE_LDIND_R8:
            case ReaderBaseNS::CEE_LDIND_REF:
                VerifyLoadIndirect(vstate,(ReaderBaseNS::LdIndirOpcode)mappedValue);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop();
                ResultIR = GenIR_LoadIndir((ReaderBaseNS::LdIndirOpcode)mappedValue,
                                              arg1,alignmentPrefix,
                                              fVolatilePrefix,false,newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_LDLEN:
                VerifyLoadLen(vstate);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop();
                ResultIR = GenIR_LoadLen(arg1,newIR);
                m_readerStack->push(ResultIR, newIR);
                break;
            
            case ReaderBaseNS::CEE_LDNULL:
                VerifyLoadNull(vstate);
                BREAK_ON_VERIFY_ONLY;

                ResultIR = GenIR_LoadNull(newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_LDSTR:
                token = ReadToken(operand);
                VerifyLoadStr(vstate,token);
                BREAK_ON_VERIFY_ONLY;

                ResultIR = GenIR_LoadStr(token,newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_LDSFLD:
                token = ReadToken(operand);
                VerifyFieldAccess(vstate, opcode, token);
                BREAK_ON_VERIFY_ONLY;

                ResultIR = GenIR_LoadStaticField( token, fVolatilePrefix, newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_LDSFLDA:
                token = ReadToken(operand);
                VerifyFieldAccess(vstate, opcode,token);
                BREAK_ON_VERIFY_ONLY;

                ResultIR = GenIR_GetStaticFieldAddress(token,newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_LDFLDA:
                token = ReadToken(operand);
                VerifyFieldAccess(vstate, opcode,token);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop();
                ResultIR = GenIR_LoadFieldAddress(token,arg1,newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_LDOBJ:
                token = ReadToken(operand);
                VerifyLoadObj(vstate, token);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop();
                GenIR_RemoveStackInterference(newIR);
                ResultIR = GenIR_LoadObj(token,arg1,alignmentPrefix,fVolatilePrefix,false,newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_LDTOKEN:
                token = ReadToken(operand);
                VerifyLoadToken(vstate, token);
                BREAK_ON_VERIFY_ONLY;

                ResultIR = GenIR_LoadToken(token, newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_LDVIRTFTN:
                loadFtnToken = ReadToken(operand);
                VerifyLoadFtn(vstate,opcode,loadFtnToken,msilBytes + currOffset);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop();
                ResultIR = GenIR_LoadVirtFunc(arg1, loadFtnToken, newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_LEAVE:
                targetOffset = nextOffset + ReadInt32(operand);
                goto GEN_LEAVE;

            case ReaderBaseNS::CEE_LEAVE_S:
                targetOffset = nextOffset + ReadInt8(operand);
                goto GEN_LEAVE;

            GEN_LEAVE:
                VerifyLeave(vstate);
                fHasFallThrough = false;
                BREAK_ON_VERIFY_ONLY;

                {
                    bool nonLocal, endsWithNonLocalGoto;

                    clearStack(newIR);
                    nonLocal = fgLeaveIsNonLocal(fg, nextOffset, targetOffset, &endsWithNonLocalGoto);

                    // Note here we record the nonlocal flow
                    GenIR_Leave(targetOffset,nonLocal,endsWithNonLocalGoto,newIR);
                }
                break;

            case ReaderBaseNS::CEE_LOCALLOC:
                VerifyFailure(vstate);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop();
                
                if (!m_readerStack->empty()) {
                    BADCODE("LOCALLOC requires that the evaluation stack be empty, apart from the size parameter");                    
                }
                if (m_currentRegion != NULL 
                    && ReaderBaseNS::RGN_ROOT != RgnGetRegionType( m_currentRegion)
                    && ReaderBaseNS::RGN_TRY  != RgnGetRegionType( m_currentRegion)
                    )
                {
                    BADCODE("LOCALLOC cannot occur within an exception block: filter, catch, finally, or fault");
                }


                ResultIR = GenIR_LocalAlloc(arg1,isZeroInitLocals(),newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_MKREFANY:
                token = ReadToken(operand);
                VerifyMkRefAny(vstate,token);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop();
                GenIR_RemoveStackInterference(newIR);
                ResultIR = GenIR_MakeRefAny(token,arg1,newIR);
                m_readerStack->push(ResultIR, newIR);
                break;
            
            case ReaderBaseNS::CEE_NEG:
            case ReaderBaseNS::CEE_NOT:
                VerifyUnary(vstate,(ReaderBaseNS::UnaryOpcode)mappedValue);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop();
                ResultIR = GenIR_UnaryOp((ReaderBaseNS::UnaryOpcode)mappedValue,
                                         arg1,newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_NEWARR:
                token = ReadToken(operand);
                VerifyNewArr(vstate,token);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop();
                GenIR_RemoveStackInterference(newIR);
                ResultIR = GenIR_NewArr(token, arg1, newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_NEWOBJ:
                token = ReadToken(operand);
                VerifyNewObj(vstate,opcode,fTailCallPrefix, token, msilBytes + currOffset);
                BREAK_ON_VERIFY_ONLY;

                ResultIR = GenIR_NewObj(token,loadFtnToken,newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_NOP:
                BREAK_ON_VERIFY_ONLY;
                GenIR_Nop(newIR);
                break;

            case ReaderBaseNS::CEE_POP:
                VerifyPop(vstate);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop();
                GenIR_Pop(arg1,newIR);
                break;

            case ReaderBaseNS::CEE_REFANYTYPE:
                VerifyRefAnyType(vstate);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop();
                GenIR_RemoveStackInterference(newIR);
                ResultIR = GenIR_RefAnyType( arg1,newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_REFANYVAL:
                token = ReadToken(operand);
                VerifyRefAnyVal(vstate,token);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop();
                GenIR_RemoveStackInterference(newIR);
                ResultIR = GenIR_RefAnyVal(arg1,token,newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_RETHROW:
                // We have to clear the stack here because we're leaving the handler
                clearStack(newIR);
                VerifyRethrow(vstate, fgGetRegionFromMSILOffset(currOffset));
                fHasFallThrough = false;
                BREAK_ON_VERIFY_ONLY;

                GenIR_Rethrow(newIR);
                break;

            case ReaderBaseNS::CEE_RET:
                {
                    CorInfoCallConv conv;
                    CorInfoType corType;
                    CORINFO_CLASS_HANDLE retTypeClass;
                    int numArgs;
                    bool isVarArg, hasThis;
                    unsigned __int8 retSig;
                    bool synchronizedMethod;

                    VerifyReturn(vstate, m_currentRegion);
                    fHasFallThrough = false;
                    BREAK_ON_VERIFY_ONLY;

                    // Get method return type (corType)
                    getMethodSigData(&conv,&corType,&retTypeClass,&numArgs,
                                     &isVarArg,&hasThis,&retSig);

                    synchronizedMethod = ((getCurrentMethodAttribs() & CORINFO_FLG_SYNCH) != 0);

                    // If no return type then stack must be empty
                    if (corType == CORINFO_TYPE_VOID) {
                        arg1 = NULL;
                    } else {
                        arg1 = m_readerStack->pop();
                    }
                    // Generate call to monitor helper (if synchronized)
                    // if return type is non-void, GenIR_Return performs:
                    //   - Convert return value to return type (if necessary)
                    //   - Generate return instruction
                    GenIR_Return(arg1,synchronizedMethod,fNeedsByrefReturnCheck, newIR);
                }
                break;

            case ReaderBaseNS::CEE_SHL:
            case ReaderBaseNS::CEE_SHR:
            case ReaderBaseNS::CEE_SHR_UN:
                VerifyShift(vstate);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop(); // Shift amount
                arg2 = m_readerStack->pop(); // Operand to be shifted

                // The shift opcodes operate on 32-bit or larger operands
                // if the operand was < 32 we need to insert the conversion
                // to mimic the implicit conversion done by the abstract machine
                // Also if the shift is signed and the operand is no then force it

                ResultIR = GenIR_Shift(
                                       (ReaderBaseNS::ShiftOpcode)mappedValue,
                                       arg1,arg2,newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_SIZEOF:
                token = ReadToken(operand);
                VerifySizeOf(vstate, token);
                BREAK_ON_VERIFY_ONLY;

                ResultIR = GenIR_Sizeof(token,newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_STARG:
                {
                    unsigned short u16;

                    u16 = ReadUInt16(operand);
                    VerifyStarg(vstate,u16);
                    BREAK_ON_VERIFY_ONLY;

                    arg1 = m_readerStack->pop();

                    GenIR_RemoveStackInterferenceForLocalStore(opcode, u16, newIR);

#ifdef READER_ESSA_RENAME
                    GenIR_StoreArg( u16, arg1, alignmentPrefix,
                                   fVolatilePrefix, newIR, 
                                   Ssa_HshMSILScalar(opcode,operand,fg), fg);
#else
                    GenIR_StoreArg( u16, arg1, alignmentPrefix,
                                   fVolatilePrefix, newIR);
#endif
                }
                break;

            case ReaderBaseNS::CEE_STARG_S:
                {
                    unsigned char u8;

                    u8 = ReadUInt8(operand);
                    VerifyStarg(vstate,u8);
                    BREAK_ON_VERIFY_ONLY;

                    arg1 = m_readerStack->pop();

                    GenIR_RemoveStackInterferenceForLocalStore(opcode, u8, newIR);

#ifdef READER_ESSA_RENAME
                    GenIR_StoreArg( u8, arg1, alignmentPrefix,
                                   fVolatilePrefix, newIR, 
                                   Ssa_HshMSILScalar(opcode,operand,fg), fg);
#else
                    GenIR_StoreArg(u8,arg1,alignmentPrefix,fVolatilePrefix,newIR);
#endif
                }
                break;

            case ReaderBaseNS::CEE_STELEM: // (M2 Generics)
                token = ReadToken(operand);
                goto STORE_ELEMENT;
            case ReaderBaseNS::CEE_STELEM_I:
            case ReaderBaseNS::CEE_STELEM_I1:
            case ReaderBaseNS::CEE_STELEM_I2:
            case ReaderBaseNS::CEE_STELEM_I4:
            case ReaderBaseNS::CEE_STELEM_I8:
            case ReaderBaseNS::CEE_STELEM_R4:
            case ReaderBaseNS::CEE_STELEM_R8:
            case ReaderBaseNS::CEE_STELEM_REF:
                token = NULL;
            STORE_ELEMENT:
                VerifyStoreElem(vstate,(ReaderBaseNS::StElemOpcode)mappedValue, token);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop(); //Pop the value to store 
                arg2 = m_readerStack->pop(); //Pop the array index
                arg3 = m_readerStack->pop(); //Pop the address of the array base
                GenIR_RemoveStackInterference(newIR);
                GenIR_StoreElem((ReaderBaseNS::StElemOpcode)mappedValue, token,
                                arg1,arg2,arg3,newIR);
                break;

            case ReaderBaseNS::CEE_STFLD:
                token = ReadToken(operand);
                VerifyFieldAccess(vstate,opcode,token);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop(); // Pop the value to store
                arg2 = m_readerStack->pop(); // Pop the address of the object
                GenIR_RemoveStackInterference(newIR);
                GenIR_StoreField(token, arg1,arg2,alignmentPrefix,fVolatilePrefix,
                                 newIR);
                break;

            case ReaderBaseNS::CEE_STLOC:
                mappedValue = ReadUInt16(operand);
                goto STORE_LOC;
            case ReaderBaseNS::CEE_STLOC_S:
                mappedValue = ReadUInt8(operand);
                goto STORE_LOC;
            case ReaderBaseNS::CEE_STLOC_0:
            case ReaderBaseNS::CEE_STLOC_1:
            case ReaderBaseNS::CEE_STLOC_2:
            case ReaderBaseNS::CEE_STLOC_3:
            STORE_LOC:
                VerifyStloc(vstate,mappedValue);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop();
                GenIR_RemoveStackInterferenceForLocalStore(opcode, mappedValue, newIR);

#ifdef READER_ESSA_RENAME
                GenIR_StoreLocal(mappedValue,arg1,alignmentPrefix,
                                 fVolatilePrefix,newIR,
                                 Ssa_HshMSILScalar(opcode,operand,fg), fg);
#else
                GenIR_StoreLocal(mappedValue,arg1,alignmentPrefix,
                                 fVolatilePrefix,newIR);
#endif
                break;

            case ReaderBaseNS::CEE_STIND_I1:
            case ReaderBaseNS::CEE_STIND_I2:
            case ReaderBaseNS::CEE_STIND_I4:
            case ReaderBaseNS::CEE_STIND_I8:
            case ReaderBaseNS::CEE_STIND_I:
            case ReaderBaseNS::CEE_STIND_R4:
            case ReaderBaseNS::CEE_STIND_R8:
            case ReaderBaseNS::CEE_STIND_REF:
                VerifyStoreIndir(vstate,(ReaderBaseNS::StIndirOpcode)mappedValue);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop();
                arg2 = m_readerStack->pop();
                GenIR_RemoveStackInterference(newIR);
                GenIR_StoreIndir((ReaderBaseNS::StIndirOpcode)mappedValue,
                                    arg1,arg2,alignmentPrefix,fVolatilePrefix,newIR);
                break;

            case ReaderBaseNS::CEE_STOBJ:
                token = ReadToken(operand);
                VerifyStoreObj(vstate,token);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop(); // Pop the source object 
                arg2 = m_readerStack->pop(); // Pop the destination object
                GenIR_RemoveStackInterference(newIR);
                GenIR_StoreObj(token, arg1, arg2,
                               alignmentPrefix,fVolatilePrefix,false,newIR);
                break;

            case ReaderBaseNS::CEE_STSFLD:
                token = ReadToken(operand);
                VerifyFieldAccess(vstate,opcode,token);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop(); // Pop the value to store from the stack
                GenIR_RemoveStackInterference(newIR);
                GenIR_StoreStaticField(token,arg1,fVolatilePrefix,newIR);
                break;

            case ReaderBaseNS::CEE_SWITCH:
                {
                    unsigned int numCases;

                    VerifySwitch(vstate);

                    // 1. Parse switch operands from msil.

                    // Each label is a 4 BYTE offset.
                    numCases = ReadNumberOfSwitchCases(&operand);

                    fVerifiedEndBlock = true;
                    verifyFinishBlock(vstate, fg); // before GenIR_MaintainOperandStack
                    BREAK_ON_VERIFY_ONLY;

                    // 2. Pop the top operand off the stack
                    arg1 = m_readerStack->pop();

                    // 3. If switch had cases then the flow graph builder has
                    // rigged up successor edges from the switch.
                    if (numCases != 0)
                    {
                        GenIR_Switch(arg1, newIR);

                        // 4. Maintain operand stack for all successors.
                        // If the operand stack is non-empty then it must be ushered
                        // across the block boundaries.
                        if (!m_readerStack->empty()) {
                            GenIR_MaintainOperandStack(NULL,NULL,newIR);
                        }
                    } else {
                        // consume the operand
                        GenIR_Pop(arg1, newIR);
                    }
                    m_readerStack->clearStack();
                }
                break;

            case ReaderBaseNS::CEE_TAILCALL:
                fTailCallPrefix = true;
                VerifyTail(vstate, m_currentRegion);

                BREAK_ON_VERIFY_ONLY;

                break;

            case ReaderBaseNS::CEE_CONSTRAINED:
                fConstrainedPrefix = true;
                constraintTypeRef = ReadToken(operand);
                VerifyConstrained(vstate, constraintTypeRef);

                BREAK_ON_VERIFY_ONLY;

                break;

            case ReaderBaseNS::CEE_READONLY:
                fReadOnlyPrefix = true;
                VerifyReadOnly(vstate);

                BREAK_ON_VERIFY_ONLY;

                break;

            case ReaderBaseNS::CEE_THROW:
                VerifyThrow(vstate);
                fHasFallThrough = false;
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop();
                GenIR_Throw(arg1,newIR);

                // Should this be supported or does the stack need to be null at this point?
                clearStack(newIR);
                break;

            case ReaderBaseNS::CEE_UNALIGNED:
                // this must be 1,2 or 4 (verified)
                alignmentPrefix = (ReaderAlignType)ReadUInt8(operand);
                VerifyUnaligned(vstate, alignmentPrefix);
                BREAK_ON_VERIFY_ONLY;

                break;

            case ReaderBaseNS::CEE_UNBOX:
                token = ReadToken(operand);
                VerifyUnbox(vstate, token);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop(); // Pop the object pointer
                GenIR_RemoveStackInterference(newIR);
                ResultIR = GenIR_Unbox(token,arg1,newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_UNBOX_ANY: // (M2 Generics)
                token = ReadToken(operand);
                VerifyUnboxAny(vstate, token);
                BREAK_ON_VERIFY_ONLY;

                arg1 = m_readerStack->pop(); // Pop the object pointer
                GenIR_RemoveStackInterference(newIR);
                ResultIR = GenIR_UnboxAny(token,arg1,alignmentPrefix,fVolatilePrefix, newIR);
                m_readerStack->push(ResultIR, newIR);
                break;

            case ReaderBaseNS::CEE_VOLATILE:
                fVolatilePrefix = true;
                VerifyVolatile(vstate);
                BREAK_ON_VERIFY_ONLY;

                break;

            default:
                // unknown opcode means we cannot continue
                BADCODE(MVER_E_UNKNOWN_OPCODE);
            } // opcode switch end

            // Reset prefixes
            if (opcode != ReaderBaseNS::CEE_VOLATILE && 
                opcode != ReaderBaseNS::CEE_TAILCALL &&
                opcode != ReaderBaseNS::CEE_UNALIGNED &&
                opcode != ReaderBaseNS::CEE_READONLY &&
                opcode != ReaderBaseNS::CEE_CONSTRAINED) 
            {
                fVolatilePrefix = false;
                fTailCallPrefix = false;
                alignmentPrefix = READER_ALIGN_NATURAL;
                fReadOnlyPrefix = false;
                fConstrainedPrefix = false;
                constraintTypeRef = mdTokenNil;
            }

            VerifyPrefixConsumed(vstate, opcode);


            currOffset = nextOffset;

        } 
        // Verify any fallthrough
        if (m_verificationNeeded && fHasFallThrough) {
            VerifyFallThrough(vstate, fg);
        }

        if (!fVerifiedEndBlock) {
            verifyFinishBlock(vstate, fg);
            fVerifiedEndBlock = true;
        }

    } PAL_EXCEPT_FILTER (ObjectFilter, &type) {
        CorInfoHelpFunc throwHelper = CORINFO_HELP_VERIFICATION;

        // Handle verification error, remove IRNodes from block
        // and have it filled with code to throw a verification
        // error.
        GenIR_ClearCurrentBlock(newIR);

        switch (type->type) {
        case READER_LOCAL_VERIFICATION_EXCEPTION:
            throwHelper = CORINFO_HELP_VERIFICATION;
            break;
        case READER_GLOBAL_VERIFICATION_EXCEPTION:
            GenIR_Fatal(CORJIT_BADCODE);
            break;
        default:
            ASSERTMNR(UNREACHED,"Unknown ReaderExceptionType");
            throwHelper = CORINFO_HELP_VERIFICATION;
            break;
        }
        // Free the exception object
        delete type;

        GenIR_InsertThrow(throwHelper, currOffset, newIR);

        fLocalFault = true;
        fHasFallThrough = false; // Blocks with errors can't have this verified

        // Delete all (non-EH reachability) flow edges that come from this block.
        FlowGraphEdgeList *arc, *arcNext;
        for (arc = FgNodeGetSuccessorListActual(fg);
            arc != NULL;
            arc = arcNext) {
            arcNext = FgEdgeListGetNextSuccessorActual(arc);
            GenIR_FgDeleteEdge(arc);
        }

        // Clear operand and verifier stack since block 
        // successor edges are now cut and the operands
        // have no use.
        if (!isVerifyOnly) {
            m_readerStack->clearStack();
        }

        if (m_verificationNeeded)
        {
            ASSERT(vstate);
            vstate->blockIsBad = true;
        }

        // Even though it is bad, it may need to be taken off the worklist.
        if (!fVerifiedEndBlock) {
            verifyFinishBlock(vstate, fg);
        }

    }
    PAL_ENDTRY

    if (!isVerifyOnly) {
        // Notify client JIT that end of block has been reached.
        GenIR_EndFlowGraphNode(fg, currOffset,newIR);

        // Propagate operand stack to successor blocks. Nothing to
        // do if stack is empty, or if this block caused a local
        // verification fault. Local verification error causes all
        // successor edges to be cut, so live operand stack has nowhere
        // to be propagated to.
        if (!(fLocalFault || m_readerStack->empty())) {
            GenIR_MaintainOperandStack(NULL,NULL,newIR);
        }

    }
}


FlowGraphNodeWorkList*
ReaderBase::fgAppendUnvisitedSuccToWorklist(
    FlowGraphNodeWorkList* worklist,
    FlowGraphNode*         currBlock
){
    FlowGraphNode* succ;

    for (FlowGraphEdgeList *fgEdge = FgNodeGetSuccessorList(currBlock);
         fgEdge != NULL;
         fgEdge = FgEdgeListGetNextSuccessor(fgEdge)) {

        succ = FgEdgeListGetSink(fgEdge);

#ifdef READER_ESSA_RENAME
        Ssa_LookAheadEdge(currBlock, succ, fgEdge);
#endif

        if (!FgNodeIsVisited(succ)) {
#ifndef NODEBUG
            // Ensure that no block is on the worklist twice.
            FlowGraphNodeWorkList* dbTemp;

            dbTemp = worklist;
            while (dbTemp != NULL) {
                if (dbTemp->block == succ) {
                    ASSERTNR(UNREACHED);
                }
                dbTemp = dbTemp->next;
            }
#endif

            FlowGraphNodeWorkList* pNewBlockList = 
                (FlowGraphNodeWorkList*)getProcMemory(sizeof(FlowGraphNodeWorkList));
            
            // Mark the block as added to the list
            FgNodeSetVisited(succ,true);
            
            // Add the new blockList element to the head of the list.
            pNewBlockList->block = succ;
            pNewBlockList->next = worklist;
            pNewBlockList->parent = currBlock;
            worklist = pNewBlockList;
        }
    }

    return worklist;
}


//
// MSILToIR - main reader function translates MSIL to IR using calls
// to GenIR object.
//
void
ReaderBase::MSILToIR(
    void
){
#ifdef READER_ESSA
    FlowGraphNodeList *pathlist = NULL;
    int                pBlockPathIndx = -1;
#endif

    FlowGraphNodeWorkList *worklist;
    FlowGraphNodeList *tmp;
    FlowGraphNode *pFgHead, *pFgTail;

    //
    // Compiler dependent pre-pass
    //
    GenIR_ReaderPrePass(m_methodInfo->ILCode,m_methodInfo->ILCodeSize);

    // SEQUENCE POINTS: Query the runtime before we
    //  do anything to check for special debugger driven
    //  sequence points
    if (GenIR_NeedSequencePoints()) {
        getCustomequencePoints();
    }

    // we have block offsets now, eit can be verified
    VerifyEIT();

    // Build the region graph and allocate the filter offset array
    rgnCreateRegionTree();
    
    // Notify GenIR of eh info.
    GenIR_SetEHInfo(m_ehRegionTree,m_allRegionList);

    // Notify client if generics context must be kept alive
    // This triggers the client to store the generics context on
    //  the stack and report its location to the EE. The JIT must
    //  also be responsible for extending its GC lifetime (if it
    //  is a gc tracked pointer).
    bool keepGenericsCtxtAlive = ((m_methodInfo->options & CORINFO_GENERICS_CTXT_KEEP_ALIVE)!=0);
    GenIR_MethodNeedsToKeepAliveGenericsContext(keepGenericsCtxtAlive);

    // Build flow graph
    pFgHead = buildFlowGraph(&pFgTail);

    if(m_verificationNeeded) {
        PGLOBALVERIFYDATA gvData = FgNodeGetGlobalVerifyData(pFgHead);
        ASSERTNR(gvData);
        gvData->tiStack = NULL;
        gvData->stkDepth = 0;
        gvData->blockIsBad = false;
        m_gvWorklistHead = NULL;
        m_gvWorklistTail = NULL;
    }

    m_areInlining = true;

    // Notify GenIR we've finsihed building the flow graph
    GenIR_ReaderMiddlePass();

#ifdef READER_ESSA
    Ssa_VirtPhiPass(pFgHead);

#ifdef READER_ESSA_RENAME
    Ssa_OptRenameInit();
#endif

    initBlockArray(GenIR_FgGetBlockCount());
#endif

    // Iterate over flow graph in depth-first preorder.

    worklist = (FlowGraphNodeWorkList*)getProcMemory(sizeof(FlowGraphNodeWorkList));
    worklist->block = pFgHead;
    worklist->next = NULL;
    worklist->parent = NULL;
    FgNodeSetVisited(pFgHead,true);

    // fake up edges to unreachable code for peverify
    // (so we can report errors in unreachable code)
    FlowGraphNode *blk;
    for (blk = pFgHead; blk != pFgTail;) 
    {
        FlowGraphNode *nextBlock;
        nextBlock = FgNodeGetNext(blk);

        if (!FgNodeGetPredecessorList(blk)
            && blk != pFgHead
            && (FgNodeGetStartMSILOffset(blk) != FgNodeGetEndMSILOffset(blk)))
        {
            GenIR_FgAddArc(NULL, pFgHead, blk);
            FlowGraphEdgeList *edgeList = FgNodeGetSuccessorList(pFgHead);
            while (edgeList)
            {
                FlowGraphNode *succBlock = FgEdgeListGetSink(edgeList);
                if (succBlock == blk)
                {
                    FgEdgeListMakeFake(edgeList);
                }
                edgeList = FgEdgeListGetNextSuccessor(edgeList);
            }
        }
        blk = nextBlock;
    }
    GenIR_ReaderMiddlePass();

    // Set up the initial stack
    m_readerStack = GenIR_CreateStack(m_methodInfo->maxStack,this);
    ASSERTNR(m_readerStack);
    FgNodeSetOperandStack(pFgHead, m_readerStack);

    bool fImportOnly = (m_flags & CORJIT_FLG_IMPORT_ONLY) != 0;

    while (worklist != NULL) {
        FlowGraphNode *block, *parent;

        // Pop top block
        block = worklist->block;
        worklist = worklist->next;

#ifdef READER_ESSA_RENAME
        Ssa_PushPhis(block);
#endif

        readBytesForFlowGraphNode(block, fImportOnly);

        // Append unvisited successors to worklist
        worklist = fgAppendUnvisitedSuccToWorklist(worklist,block);

#ifdef READER_ESSA
        pBlockPathIndx += 1;

        if (pathlist) {
            tmp = (FlowGraphNodeList*)getProcMemory(sizeof(FlowGraphNodeList));
            tmp->block = block;
            tmp->next = pathlist;
            pathlist = tmp;
        }
        else {
            pathlist = (FlowGraphNodeList*)getProcMemory(sizeof(FlowGraphNodeList));
            pathlist->block = block;
            pathlist->next = NULL;    
        }

        // This loop will not be enetered if we are descending preorder.
        // Otherwise we are popping back up a path in the graph which may 
        // only be one block.

        // This is where I want SESE threading to eventually play in.

        // This could be sped up by using ssa stack markers rather than
        // traversing the tuples but for now it's a good consistency check
        // that going backup we parse what we think we created going down.
        parent = (worklist ? worklist->parent : NULL);

        while (pathlist && (pathlist->block != parent)) {
#ifdef READER_ESSA_RENAME
            Ssa_OptPop(pathlist->block);
#endif
            tmp = pathlist;
            pathlist = pathlist->next;

            pBlockPathIndx -= 1;
        }
#endif
    }

#ifdef READER_ESSA
    ASSERTNR(pBlockPathIndx == -1);
#endif

    // global verification dataflow
    if(m_verificationNeeded) {
        // iteration portion of global verification, as needed to
        // revisit node for which incoming edge has new confluence data
#if !defined(NODEBUG)
        // This paranoia counter will help us detect infinite loops
        int GlobalVerificationParanoiaCounter = 0;
#endif
        while (m_gvWorklistHead) {
            ASSERTNR(m_gvWorklistHead->block);
            ASSERTNR(!m_gvWorklistHead->blockIsBad);
            readBytesForFlowGraphNode(m_gvWorklistHead->block,TRUE);

#if !defined(NODEBUG)
            GlobalVerificationParanoiaCounter++;
            ASSERTNR(GlobalVerificationParanoiaCounter < 2000);
#endif
        }
    }

    // Remove blocks that weren't marked as visited.
    fgRemoveUnusedBlocks(pFgHead,pFgTail);

    // Report result of verification to the VM
    if (fImportOnly)
    {
        ASSERTNR(m_verificationNeeded);

        CorInfoMethodRuntimeFlags verFlags;
        // Do not inline this function if we see it again.  
        verFlags = m_isVerifiableCode ? (CORINFO_FLG_VERIFIABLE) :
               CorInfoMethodRuntimeFlags(CORINFO_FLG_UNVERIFIABLE | CORINFO_FLG_BAD_INLINEE);
        m_jitInfo->setMethodAttribs(getCurrentMethodHandle(), verFlags);
    }

    //
    // Client post-pass
    //
    GenIR_ReaderPostPass(fImportOnly);
}



//
// Checks to see if a given offset is the start of an instruction. If the offset is not the
//  start of an instruction the whole program must be discarded as it's global flow may not
//  be able to be verified.
// The method should be called at the beginning of each basic block and for each branch target. This
//  prevents us from ever reading the middle of an instr and allows us to bail ASAP if the flow graph
//  is invalid.
//
bool ReaderBase::IsOffsetInstrStart(unsigned int targetOffset)
{
    return (targetOffset < m_methodInfo->ILCodeSize) && m_legalTargetOffsets->GetBit(targetOffset);
}


//
// __RuntimeFilter allows the JIT to catch exceptions that may be thrown
// by the runtime using a runtime-supplied filter.
//
int
ReaderBase::__RuntimeFilter(struct _EXCEPTION_POINTERS *pExceptionPointers)
{
    return m_jitInfo->FilterException(pExceptionPointers);
}



//
// SEQUENCE POINTS
//


// JIT generated sequence points are located at stack empty
//  locations, EH boundary regions, and after nops and calls.
//  Calls include call/calli/callvirt/jmp/newobj instructions.
// Debugger generated sequece points are placed based on the
//  values revieved from the runtime via getSequencePoints
// 
//
void
ReaderBase::GenIR_SequencePoint(int offset, ReaderBaseNS::OPCODE prevOp, IRNode **newIR)
{
    ASSERTNR(GenIR_NeedSequencePoints());

    DWORD typeFlags = ICorDebugInfo::SOURCE_TYPE_INVALID;
    bool genSeqPoint = false;

    if(m_readerStack->empty()) {
        typeFlags |= ICorDebugInfo::STACK_EMPTY;
        genSeqPoint = true;
    }

    switch(prevOp)
    {
        case ReaderBaseNS::CEE_NOP:
            genSeqPoint = true;
            break;
        case ReaderBaseNS::CEE_CALL:
        case ReaderBaseNS::CEE_CALLI:
        case ReaderBaseNS::CEE_CALLVIRT:
        case ReaderBaseNS::CEE_JMP:
        case ReaderBaseNS::CEE_NEWOBJ:
            typeFlags |= ICorDebugInfo::CALL_SITE;
            genSeqPoint = true;
            break;
        default:
            break;
    }

    // Check for debugger (pdb) generated sequence points
    if (m_customSequencePoints->GetBit(offset)) {
        typeFlags |= ICorDebugInfo::SEQUENCE_POINT;
        genSeqPoint = true;
    }

    if (genSeqPoint) {
        GenIR_SetSequencePoint(offset,(ICorDebugInfo::SourceTypes)typeFlags,newIR);
    }
}


void
ReaderBase::getCustomequencePoints()
{
    unsigned cILOffsets;
    DWORD *pILOffsets;
    ICorDebugInfo::BoundaryTypes implicitBoundaries;
    m_customSequencePoints = (ReaderBitVector*)getTempMemory(sizeof(ReaderBitVector));
    CORINFO_METHOD_HANDLE methodHandle = getCurrentMethodHandle();

    ASSERTNR(GenIR_NeedSequencePoints());

    m_jitInfo->getBoundaries(methodHandle,&cILOffsets,&pILOffsets,&implicitBoundaries);
    m_customSequencePoints->AllocateBitVector(m_methodInfo->ILCodeSize,this);
    for(unsigned i = 0; i < cILOffsets; i++)
    {
        m_customSequencePoints->SetBit(pILOffsets[i]);
    }
    m_jitInfo->freeArray(pILOffsets); // free the array
}


bool
ReaderBase::rdrIsMethodVirtual(DWORD methodAttribs)
{
  // final methods aren’t virutal
  if  ((methodAttribs & CORINFO_FLG_FINAL) !=0)
     return false;

   // static methods aren’t virtual
   if ((methodAttribs & CORINFO_FLG_STATIC) !=0) 
     return false;

   // methods not explicitly marked as virtual are not virutal 
   if ((methodAttribs & CORINFO_FLG_VIRTUAL) == 0)
     return false;

  // assume all other methods are virtual
  return true;
}


/////////////////////////////////////////////////////////////////////////////////
//
// ReaderCallTargetData Object
//
// This object encapsulates information about a call
//  target. Some of this information is hidden from
//  the client but can be accessed by the common
//  reader.
// The class acts like a hash, generating information
//  the first time it is requested, and then using
//  the cached information for later requests.
// 


DWORD
ReaderCallTargetData::getClassAttribs()
{
    if (!fClassAttribsValid) {
        targetClassAttribs = reader->getClassAttribs(getClassHandle());
        fClassAttribsValid = true;
    }
    return targetClassAttribs;
}

CORINFO_CLASS_HANDLE
ReaderCallTargetData::getClassHandle()
{
    if (!targetClassHandle) {
        targetClassHandle = reader->getMethodClass(getMethodHandle());
    }
    return targetClassHandle;
}

IRNode*
ReaderCallTargetData::getMethodHandleNode(IRNode **newIR)
{
    ASSERTNR(!isIndirect()); // This makes no sense for indirect calls
    if (!targetMethodHandleNode) {
        targetMethodHandleNode = 
          reader->GenIR_GenericTokenToNode(targetMethodToken,newIR,true);
    }
    return targetMethodHandleNode;
}

IRNode*
ReaderCallTargetData::getClassHandleNode(IRNode **newIR)
{
    if (!targetClassHandleNode) {
        mdToken typeRef = reader->getMemberParent(targetMethodToken);
        targetClassHandleNode = 
            reader->GenIR_GenericTokenToNode(typeRef,newIR,true);
    }
    return targetClassHandleNode;
}

IRNode*
ReaderCallTargetData::getTypeContextNode(IRNode **newIR)
{
    if (sigInfo.hasTypeArg()) {
        if (TypeFromToken(getMethodToken()) == mdtMethodSpec) {           
            // Instantiated generic method
            return getMethodHandleNode(newIR);
        } else if ((getClassAttribs() & CORINFO_FLG_ARRAY) && fReadonlyCall) {
            return reader->GenIR_LoadConstantI(0,newIR);
        } else {
            // otherwise must be an instance method in a generic struct,
            //  a static method in a generic type, or a runtime-generated array method
            return getClassHandleNode(newIR);
        }
    }
    return NULL;
}

void
ReaderCallTargetData::setOptimizedDelegateCtor(CORINFO_METHOD_HANDLE newTargetMethodHandle)
{
    // Leave the original class handle info around for canonNewObjCall

    this->targetMethodHandle = newTargetMethodHandle;
    this->targetMethodHandleNode = NULL;
    this->fOptimizedDelegateCtor = true;

    this->targetMethodAttribs = reader->getMethodAttribs(targetMethodHandle);

    reader->getCallSiteSignature(newTargetMethodHandle,
                                 mdTokenNil, /* We don't have the token */
                                 &this->sigInfo,&this->fThisPtr);
}



// Private constructor: only called by friend class ReaderBase
ReaderCallTargetData::ReaderCallTargetData(ReaderBase *reader, 
                                           mdToken targetToken, mdToken constraintToken, 
                                           mdToken loadFtnToken,
                                           bool fTailCall, bool fUnmarkedTailCall, bool fReadonlyCall,
                                           ReaderBaseNS::CallOpcode opcode)
{
    this->reader = reader;

    this->targetMethodToken = targetToken;
    this->constraintToken   = constraintToken;
    this->loadFtnToken      = loadFtnToken;

    this->fCallVirt = (opcode==ReaderBaseNS::CALLVIRT);
    this->fIndirect = (opcode==ReaderBaseNS::CALLI);
    this->fNewObj   = (opcode==ReaderBaseNS::NEWOBJ);
    this->fJmp      = (opcode==ReaderBaseNS::JMP);
    this->fTailCall = fTailCall;

    // Check for recursive tail call
    if (fTailCall) {
        this->fRecursiveTailCall = 
            reader->GenIR_FgOptRecurse(opcode, targetToken, fUnmarkedTailCall);
    } else {
        this->fRecursiveTailCall = false;
    }
    this->fUnmarkedTailCall = fUnmarkedTailCall;
    this->fReadonlyCall = fReadonlyCall;

    // NULL out attribs, class handle, and nodes
    this->fClassAttribsValid = false;
    this->fCallInfoValid = false;
    this->fNeedsNullCheck = false;
    this->corIntrinsicId = CORINFO_INTRINSIC_Count;
    this->fUsesMethodDesc = false;
    this->fOptimizedDelegateCtor = false;
    this->fThisInSecretRegister = false;
    this->pCtorArgs = NULL;
    this->targetClassHandle = NULL;
    this->targetClassHandleNode = NULL;
    this->targetMethodHandleNode = NULL;
    this->indirectionCellNode = NULL;
    this->callTargetNode = NULL;
    
    // fill CALL_INFO, SIG_INFO, METHOD_HANDLE, METHOD_ATTRIBS
    fillTargetInfo();
}

// fill CALL_INFO, SIG_INFO, METHOD_HANDLE, and METHOD_ATTRIBS
void
ReaderCallTargetData::fillTargetInfo()
{
    targetMethodHandle = NULL;
    targetMethodAttribs = 0;
    if (!fIndirect) {
        reader->getCallInfo(targetMethodToken, constraintToken, fCallVirt, &callInfo);
        fCallInfoValid = true;
        targetMethodHandle = callInfo.targetMethodHandle;
        targetMethodAttribs = reader->getMethodAttribs(targetMethodHandle);
    }

    reader->getCallSiteSignature(targetMethodHandle,targetMethodToken,&sigInfo,&fThisPtr);

#if !defined(NODEBUG)
    // DEBUG: Attach the name of the target to the CallTargetData struct
    // NOTE: This guy seems to throw up "Token out of range" assert in some ilstubs
    reader->findNameOfToken(targetMethodToken, targetName, COUNTOF(targetName));
#endif
}
