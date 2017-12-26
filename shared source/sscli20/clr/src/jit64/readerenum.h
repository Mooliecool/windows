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

#ifndef _READERENUM_H
#define _READERENUM_H

namespace ReaderBaseNS{

    // Opcode Enumerations for communicating opcode between
    // reader and genir. Do not change the order of these
    // enumerations as client may use these as indices into
    // private arrays.

#include "openum.h"

    enum UnaryOpcode{
        NEG = 0,
        NOT,

        LAST_UNARY_OPCODE
    };

    enum BinaryOpcode{
        ADD = 0,
        ADD_OVF,
        ADD_OVF_UN,
        AND,
        DIV,
        DIV_UN,
        MUL,
        MUL_OVF,
        MUL_OVF_UN,
        OR,
        REM,
        REM_UN,
        SUB,
        SUB_OVF,
        SUB_OVF_UN,
        XOR,

        LAST_BINARY_OPCODE
    };

    enum BoolBranchOpcode{
        BR_FALSE = 0,
        BR_FALSE_S,
        BR_TRUE,
        BR_TRUE_S,
        
        LAST_BOOLBRANCH_OPCODE
    };

    enum CallOpcode{
        JMP = 0,
        CALL,
        CALLVIRT,
        CALLI,
        TAIL,
        NEWOBJ,  // Call code is used to process NEWOBJ...

        LAST_CALL_OPCODE
    };

    enum CmpOpcode{
        CEQ = 0,
        CGT,
        CGT_UN,
        CLT,
        CLT_UN,

        LAST_CMP_OPCODE
    };

    enum CondBranchOpcode{
        BEQ = 0,
        BEQ_S,
        BGE,
        BGE_S,
        BGE_UN,
        BGE_UN_S,
        BGT,
        BGT_S,
        BGT_UN,
        BGT_UN_S,
        BLE,
        BLE_S,
        BLE_UN,
        BLE_UN_S,
        BLT,
        BLT_S,
        BLT_UN,
        BLT_UN_S,
        BNE_UN,
        BNE_UN_S,

        LAST_CONDBRANCH_OPCODE
    };
        
    enum ConvOpcode{
        CONV_I1 = 0,
        CONV_I2,
        CONV_I4,
        CONV_I8,
        CONV_R4,
        CONV_R8,
        CONV_U1,
        CONV_U2,
        CONV_U4,
        CONV_U8,
        CONV_I,
        CONV_U,

        CONV_OVF_I1,
        CONV_OVF_I2,
        CONV_OVF_I4,
        CONV_OVF_I8,
        CONV_OVF_U1,
        CONV_OVF_U2,
        CONV_OVF_U4,
        CONV_OVF_U8,
        CONV_OVF_I,
        CONV_OVF_U,

        CONV_OVF_I1_UN,
        CONV_OVF_I2_UN,
        CONV_OVF_I4_UN,
        CONV_OVF_I8_UN,
        CONV_OVF_U1_UN,
        CONV_OVF_U2_UN,
        CONV_OVF_U4_UN,
        CONV_OVF_U8_UN,
        CONV_OVF_I_UN,
        CONV_OVF_U_UN,
        CONV_R_UN,
        
        LAST_CONV_OPCODE
    };

    enum ExceptOpcode{
        ENDFILTER,
        THROW,

        LAST_EXCEPT_OPCODE
    };

    enum LdElemOpcode{
        LDELEM_I1 = 0,
        LDELEM_U1,
        LDELEM_I2,
        LDELEM_U2,
        LDELEM_I4,
        LDELEM_U4,
        LDELEM_I8,
        LDELEM_I,
        LDELEM_R4,
        LDELEM_R8,
        LDELEM_REF,
        LDELEM, // (M2 Generics)
        
        LAST_LDELEM_OPCODE
    };

    enum LdIndirOpcode{
        LDIND_I1 = 0,
        LDIND_U1,
        LDIND_I2,
        LDIND_U2,
        LDIND_I4,
        LDIND_U4,
        LDIND_I8,
        LDIND_I,
        LDIND_R4,
        LDIND_R8,
        LDIND_REF,

        LAST_LDIND_OPCODE
    };

    enum StElemOpcode{
        STELEM_I = 0,
        STELEM_I1,
        STELEM_I2,
        STELEM_I4,
        STELEM_I8,
        STELEM_R4,
        STELEM_R8,
        STELEM_REF,
        STELEM,
        
        LAST_STELEM_OPCODE
    };

    enum ShiftOpcode{
        SHL = 0,
        SHR,
        SHR_UN,

        LAST_SHIFT_OPCODE
    };

    enum StIndirOpcode{
        STIND_I1 = 0,
        STIND_I2,
        STIND_I4,
        STIND_I8,
        STIND_I,
        STIND_R4,
        STIND_R8,
        STIND_REF,

        LAST_STIND_OPCODE
    };

    // Taken from rgn.h, eventually needs to go into its own file.
    typedef enum {
        RGN_UNKNOWN = 0,
        RGN_NONE,
        RGN_ROOT,
        RGN_TRY,
        RGN_EXCEPT,  // C++ except (SEH)
        RGN_FAULT,
        RGN_FINALLY,
        RGN_FILTER,
        RGN_DTOR, 
        RGN_CATCH,   // C++ catch
        RGN_MEXCEPT, // managed (CLR) except
        RGN_MCATCH,  // managed (CLR) catch

        // New region types used in common reader
        RGN_CLAUSE_NONE,
        RGN_CLAUSE_FILTER,
        RGN_CLAUSE_FINALLY,
        RGN_CLAUSE_ERROR,
        RGN_CLAUSE_FAULT,
    } RegionKind;

    // Taken from rgn.h, eventually needs to go into its own file.
    typedef enum {
        TRY_NONE = 0,
        TRY_FIN,   
        TRY_FAULT,   // try/fault
        TRY_MCATCH,     // the try has only catch handlers
        TRY_MCATCHXCPT, // the try has both catch and except handlers
        TRY_MXCPT,      // the try has only except handlers
        // for native compiler code, not used in current jit64
        TRY_XCPT,       // native SEH except
        TRY_CCATCH      // native C++ catch 
    } TryKind;

};

// Used to map read opcodes to function-specific opcode enumerations.
// Uses the same ordering as openum.h.
static const char opcodeRemap[ReaderBaseNS::CEE_MACRO_END-ReaderBaseNS::CEE_NOP] = {
    -1, // CEE_NOP,
    -1, // CEE_BREAK,
        0, // CEE_LDARG_0,
        1, // CEE_LDARG_1,
        2, // CEE_LDARG_2,
        3, // CEE_LDARG_3,
        0, // CEE_LDLOC_0,
        1, // CEE_LDLOC_1,
        2, // CEE_LDLOC_2,
        3, // CEE_LDLOC_3,
        0, // CEE_STLOC_0,
        1, // CEE_STLOC_1,    
        2, // CEE_STLOC_2,
        3, // CEE_STLOC_3,
    -1, // CEE_LDARG_S,
    -1, // CEE_LDARGA_S,
    -1, // CEE_STARG_S,
    -1, // CEE_LDLOC_S,
    -1, // CEE_LDLOCA_S,
    -1, // CEE_STLOC_S,
    -1, // CEE_LDNULL,
    -1, // CEE_LDC_I4_M1,
        0, // CEE_LDC_I4_0,
        1, // CEE_LDC_I4_1,
        2, // CEE_LDC_I4_2,
        3, // CEE_LDC_I4_3,
        4, // CEE_LDC_I4_4,
        5, // CEE_LDC_I4_5,
        6, // CEE_LDC_I4_6,
        7, // CEE_LDC_I4_7,
        8, // CEE_LDC_I4_8,
    -1, // CEE_LDC_I4_S,
    -1, // CEE_LDC_I4,
    -1, // CEE_LDC_I8,
    -1, // CEE_LDC_R4,
    -1, // CEE_LDC_R8,
    -1, // CEE_UNUSED49,
    -1, // CEE_DUP,
    -1, // CEE_POP,
    ReaderBaseNS::JMP,   // CEE_JMP,
    ReaderBaseNS::CALL,  // CEE_CALL,
    ReaderBaseNS::CALLI, // CEE_CALLI,
    -1, // CEE_RET,
    -1, // CEE_BR_S,
    ReaderBaseNS::BR_FALSE_S, // CEE_BRFALSE_S,
    ReaderBaseNS::BR_TRUE_S,  // CEE_BRTRUE_S,
    ReaderBaseNS::BEQ_S,
    ReaderBaseNS::BGE_S,
    ReaderBaseNS::BGT_S,
    ReaderBaseNS::BLE_S,
    ReaderBaseNS::BLT_S,
    ReaderBaseNS::BNE_UN_S,
    ReaderBaseNS::BGE_UN_S,
    ReaderBaseNS::BGT_UN_S,
    ReaderBaseNS::BLE_UN_S,
    ReaderBaseNS::BLT_UN_S,
    -1, // CEE_BR,
    ReaderBaseNS::BR_FALSE, // CEE_BRFALSE,
    ReaderBaseNS::BR_TRUE,  // CEE_BRTRUE,
    ReaderBaseNS::BEQ,
    ReaderBaseNS::BGE,
    ReaderBaseNS::BGT,
    ReaderBaseNS::BLE,
    ReaderBaseNS::BLT,
    ReaderBaseNS::BNE_UN,
    ReaderBaseNS::BGE_UN,
    ReaderBaseNS::BGT_UN,
    ReaderBaseNS::BLE_UN,
    ReaderBaseNS::BLT_UN,
    -1, // CEE_SWITCH,
    ReaderBaseNS::LDIND_I1,  // CEE_LDIND_I1,
    ReaderBaseNS::LDIND_U1,  // CEE_LDIND_U1,
    ReaderBaseNS::LDIND_I2,  // CEE_LDIND_I2,
    ReaderBaseNS::LDIND_U2,  // CEE_LDIND_U2,
    ReaderBaseNS::LDIND_I4,  // CEE_LDIND_I4,
    ReaderBaseNS::LDIND_U4,  // CEE_LDIND_U4,
    ReaderBaseNS::LDIND_I8,  // CEE_LDIND_I8,
    ReaderBaseNS::LDIND_I,   // CEE_LDIND_I,
    ReaderBaseNS::LDIND_R4,  // CEE_LDIND_R4,
    ReaderBaseNS::LDIND_R8,  // CEE_LDIND_R8,
    ReaderBaseNS::LDIND_REF, // CEE_LDIND_REF,
    ReaderBaseNS::STIND_REF, // CEE_STIND_REF,
    ReaderBaseNS::STIND_I1,  // CEE_STIND_I1,
    ReaderBaseNS::STIND_I2,  // CEE_STIND_I2,
    ReaderBaseNS::STIND_I4,  // CEE_STIND_I4,
    ReaderBaseNS::STIND_I8,  // CEE_STIND_I8,
    ReaderBaseNS::STIND_R4,  // CEE_STIND_R4,
    ReaderBaseNS::STIND_R8,  // CEE_STIND_R8,
    ReaderBaseNS::ADD,     // CEE_ADD,
    ReaderBaseNS::SUB,     // CEE_SUB,
    ReaderBaseNS::MUL,     // CEE_MUL,
    ReaderBaseNS::DIV,     // CEE_DIV,
    ReaderBaseNS::DIV_UN,  // CEE_DIV_UN,
    ReaderBaseNS::REM,     // CEE_REM,
    ReaderBaseNS::REM_UN,  // CEE_REM_UN,
    ReaderBaseNS::AND,     // CEE_AND,
    ReaderBaseNS::OR,      // CEE_OR,
    ReaderBaseNS::XOR,     // CEE_XOR,
    ReaderBaseNS::SHL,     // CEE_SHL,
    ReaderBaseNS::SHR,     // CEE_SHR,
    ReaderBaseNS::SHR_UN,  // CEE_SHR_UN,
    ReaderBaseNS::NEG,     // CEE_NEG,
    ReaderBaseNS::NOT,     // CEE_NOT,
    ReaderBaseNS::CONV_I1, // CEE_CONV_I1,
    ReaderBaseNS::CONV_I2, // CEE_CONV_I2,
    ReaderBaseNS::CONV_I4, // CEE_CONV_I4,
    ReaderBaseNS::CONV_I8, // CEE_CONV_I8,
    ReaderBaseNS::CONV_R4, // CEE_CONV_R4,
    ReaderBaseNS::CONV_R8, // CEE_CONV_R8,
    ReaderBaseNS::CONV_U4, // CEE_CONV_U4,
    ReaderBaseNS::CONV_U8, // CEE_CONV_U8,
    ReaderBaseNS::CALLVIRT, // CEE_CALLVIRT,
    -1, // CEE_CPOBJ,
    -1, // CEE_LDOBJ,
    -1, // CEE_LDSTR,
    ReaderBaseNS::NEWOBJ,   // CEE_NEWOBJ,
    -1, // CEE_CASTCLASS,
    -1, // CEE_ISINST,
    ReaderBaseNS::CONV_R_UN, // CEE_CONV_R_UN,
    -1, // CEE_UNUSED58,
    -1, // CEE_UNUSED1,
    -1, // CEE_UNBOX,
    ReaderBaseNS::THROW, // CEE_THROW,
    -1, // CEE_LDFLD,
    -1, // CEE_LDFLDA,
    -1, // CEE_STFLD,
    -1, // CEE_LDSFLD,
    -1, // CEE_LDSFLDA,
    -1, // CEE_STSFLD,
    -1, // CEE_STOBJ,
    ReaderBaseNS::CONV_OVF_I1_UN, // CEE_CONV_OVF_I1_UN,
    ReaderBaseNS::CONV_OVF_I2_UN, // CEE_CONV_OVF_I2_UN,
    ReaderBaseNS::CONV_OVF_I4_UN, // CEE_CONV_OVF_I4_UN,
    ReaderBaseNS::CONV_OVF_I8_UN, // CEE_CONV_OVF_I8_UN,
    ReaderBaseNS::CONV_OVF_U1_UN, // CEE_CONV_OVF_U1_UN,
    ReaderBaseNS::CONV_OVF_U2_UN, // CEE_CONV_OVF_U2_UN,
    ReaderBaseNS::CONV_OVF_U4_UN, // CEE_CONV_OVF_U4_UN,
    ReaderBaseNS::CONV_OVF_U8_UN, // CEE_CONV_OVF_U8_UN,
    ReaderBaseNS::CONV_OVF_I_UN,  // CEE_CONV_OVF_I_UN,
    ReaderBaseNS::CONV_OVF_U_UN,  // CEE_CONV_OVF_U_UN,
    -1, // CEE_BOX,
    -1, // CEE_NEWARR,
    -1, // CEE_LDLEN,
    -1, // CEE_LDELEMA,
    ReaderBaseNS::LDELEM_I1,   // CEE_LDELEM_I1,
    ReaderBaseNS::LDELEM_U1,   // CEE_LDELEM_U1,
    ReaderBaseNS::LDELEM_I2,   // CEE_LDELEM_I2,
    ReaderBaseNS::LDELEM_U2,   // CEE_LDELEM_U2,
    ReaderBaseNS::LDELEM_I4,   // CEE_LDELEM_I4,
    ReaderBaseNS::LDELEM_U4,   // CEE_LDELEM_U4,
    ReaderBaseNS::LDELEM_I8,   // CEE_LDELEM_I8,
    ReaderBaseNS::LDELEM_I,    // CEE_LDELEM_I,
    ReaderBaseNS::LDELEM_R4,   // CEE_LDELEM_R4,
    ReaderBaseNS::LDELEM_R8,   // CEE_LDELEM_R8,
    ReaderBaseNS::LDELEM_REF,  // CEE_LDELEM_REF,
    ReaderBaseNS::STELEM_I,    // CEE_STELEM_I,
    ReaderBaseNS::STELEM_I1,   // CEE_STELEM_I1,
    ReaderBaseNS::STELEM_I2,   // CEE_STELEM_I2,
    ReaderBaseNS::STELEM_I4,   // CEE_STELEM_I4,
    ReaderBaseNS::STELEM_I8,   // CEE_STELEM_I8,
    ReaderBaseNS::STELEM_R4,   // CEE_STELEM_R4,
    ReaderBaseNS::STELEM_R8,   // CEE_STELEM_R8,
    ReaderBaseNS::STELEM_REF,  // CEE_STELEM_REF,
    ReaderBaseNS::LDELEM,      // CEE_LDELEM (M2 Generics),
    ReaderBaseNS::STELEM,      // CEE_STELEM (M2 Generics),
    -1, // UNBOX_ANY (M2 Generics),
    -1, // CEE_UNUSED5
    -1, // CEE_UNUSED6
    -1, // CEE_UNUSED7
    -1, // CEE_UNUSED8
    -1, // CEE_UNUSED9
    -1, // CEE_UNUSED10
    -1, // CEE_UNUSED11
    -1, // CEE_UNUSED12
    -1, // CEE_UNUSED13
    -1, // CEE_UNUSED14
    -1, // CEE_UNUSED15
    -1, // CEE_UNUSED16
    -1, // CEE_UNUSED17
    ReaderBaseNS::CONV_OVF_I1, // CEE_CONV_OVF_I1,
    ReaderBaseNS::CONV_OVF_U1, // CEE_CONV_OVF_U1,
    ReaderBaseNS::CONV_OVF_I2, // CEE_CONV_OVF_I2,
    ReaderBaseNS::CONV_OVF_U2, // CEE_CONV_OVF_U2,
    ReaderBaseNS::CONV_OVF_I4, // CEE_CONV_OVF_I4,
    ReaderBaseNS::CONV_OVF_U4, // CEE_CONV_OVF_U4,
    ReaderBaseNS::CONV_OVF_I8, // CEE_CONV_OVF_I8,
    ReaderBaseNS::CONV_OVF_U8, // CEE_CONV_OVF_U8,
    -1, // CEE_UNUSED50,
    -1, // CEE_UNUSED18,
    -1, // CEE_UNUSED19,
    -1, // CEE_UNUSED20,
    -1, // CEE_UNUSED21,
    -1, // CEE_UNUSED22,
    -1, // CEE_UNUSED23,
    -1, // CEE_REFANYVAL,
    -1, // CEE_CKFINITE,
    -1, // CEE_UNUSED24,
    -1, // CEE_UNUSED25,
    -1, // CEE_MKREFANY,
    -1, // CEE_UNUSED59,
    -1, // CEE_UNUSED60,
    -1, // CEE_UNUSED61,
    -1, // CEE_UNUSED62,
    -1, // CEE_UNUSED63,
    -1, // CEE_UNUSED64,
    -1, // CEE_UNUSED65,
    -1, // CEE_UNUSED66,
    -1, // CEE_UNUSED67,
    -1, // CEE_LDTOKEN,
    ReaderBaseNS::CONV_U2,    // CEE_CONV_U2,
    ReaderBaseNS::CONV_U1,    // CEE_CONV_U1,
    ReaderBaseNS::CONV_I,     // CEE_CONV_I,
    ReaderBaseNS::CONV_OVF_I, // CEE_CONV_OVF_I,
    ReaderBaseNS::CONV_OVF_U, // CEE_CONV_OVF_U,
    ReaderBaseNS::ADD_OVF,    // CEE_ADD_OVF,
    ReaderBaseNS::ADD_OVF_UN, // CEE_ADD_OVF_UN,
    ReaderBaseNS::MUL_OVF,    // CEE_MUL_OVF,
    ReaderBaseNS::MUL_OVF_UN, // CEE_MUL_OVF_UN,
    ReaderBaseNS::SUB_OVF,    // CEE_SUB_OVF,
    ReaderBaseNS::SUB_OVF_UN, // CEE_SUB_OVF_UN,
    -1, // CEE_ENDFINALLY,
    -1, // CEE_LEAVE,
    -1, // CEE_LEAVE_S,
    ReaderBaseNS::STIND_I,    // CEE_STIND_I,
    ReaderBaseNS::CONV_U,     // CEE_CONV_U,
    -1, // CEE_UNUSED26,
    -1, // CEE_UNUSED27,
    -1, // CEE_UNUSED28,
    -1, // CEE_UNUSED29,
    -1, // CEE_UNUSED30,
    -1, // CEE_UNUSED31,
    -1, // CEE_UNUSED32,
    -1, // CEE_UNUSED33,
    -1, // CEE_UNUSED34,
    -1, // CEE_UNUSED35,
    -1, // CEE_UNUSED36,
    -1, // CEE_UNUSED37,
    -1, // CEE_UNUSED38,
    -1, // CEE_UNUSED39,
    -1, // CEE_UNUSED40,
    -1, // CEE_UNUSED41,
    -1, // CEE_UNUSED42,
    -1, // CEE_UNUSED43,
    -1, // CEE_UNUSED44,
    -1, // CEE_UNUSED45,
    -1, // CEE_UNUSED46,
    -1, // CEE_UNUSED47,
    -1, // CEE_UNUSED48,
    -1, // CEE_PREFIX7,
    -1, // CEE_PREFIX6,
    -1, // CEE_PREFIX5,
    -1, // CEE_PREFIX4,
    -1, // CEE_PREFIX3,
    -1, // CEE_PREFIX2,
    -1, // CEE_PREFIX1,
    -1, // CEE_PREFIXREF,

    -1, // CEE_ARGLIST,
    ReaderBaseNS::CEQ,        // CEE_CEQ,
    ReaderBaseNS::CGT,        // CEE_CGT,
    ReaderBaseNS::CGT_UN,     // CEE_CGT_UN,
    ReaderBaseNS::CLT,        // CEE_CLT,
    ReaderBaseNS::CLT_UN,     // CEE_CLT_UN,
    -1, // CEE_LDFTN,
    -1, // CEE_LDVIRTFTN,
    -1, // CEE_UNUSED56,
    -1, // CEE_LDARG,
    -1, // CEE_LDARGA,
    -1, // CEE_STARG,
    -1, // CEE_LDLOC,
    -1, // CEE_LDLOCA,
    -1, // CEE_STLOC,
    -1, // CEE_LOCALLOC,
    -1, // CEE_UNUSED57,
    ReaderBaseNS::ENDFILTER, // CEE_ENDFILTER,
    -1, // CEE_UNALIGNED,
    -1, // CEE_VOLATILE,
    -1, // CEE_TAILCALL,
    -1, // CEE_INITOBJ,
    -1, // CEE_CONSTRAINED,
    -1, // CEE_CPBLK,
    -1, // CEE_INITBLK,
    -1, // CEE_UNUSED69,
    -1, // CEE_RETHROW,
    -1, // CEE_UNUSED51,
    -1, // CEE_SIZEOF,
    -1, // CEE_REFANYTYPE,
    -1, // CEE_READONLY,
    -1, // CEE_UNUSED53,
    -1, // CEE_UNUSED54,
    -1, // CEE_UNUSED55,
    -1, // CEE_UNUSED70,
    -1, // CEE_ILLEGAL,
};

#endif
