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
/***************************************************************************/
/*                                 x86def.h                              */
/***************************************************************************/

/* Defines the layout of the x86 machine instructions using Cmd structures.
   Note that the code here ONLY deals with x86 specific information, no
   other instruction set (like OMNI) is involved at this point */

/* If you are not already familiar with the x86 instruction set please
   get an architecture manual first, This file can be thought of as a
   distilation of the code layout part */

/* Note that we assume that the x86 CPU is in 32bit mode */

/* Note that the macros don't check for the non-orthogonalities of the x86 addressing modes */

/*        
                                */
/***************************************************************************/


#ifndef x86def_h
#define x86def_h 1

/***************************************************************************/
/* The register numbering */

#define X86_EAX 0
#define X86_EBX 3
#define X86_ECX 1
#define X86_EDX 2
#define X86_ESP 4
#define X86_EBP 5
#define X86_ESI 6
#define X86_EDI 7

#define X86_NO_IDX_REG 4

/* segment register override Prefix*/
#define X86_FS_Prefix 0x64 /* FS segment override prefix */
/***************************************************************************/
/* Define the 'mod' and 'sib' byte that are used in other instructions */
#define x86_sib(scale, scaleReg, baseReg)                   \
    cmdByte(expOr3(expBits(scale,2,6), expBits(baseReg,3,0), expBits(scaleReg,3,3)))


#define x86_mod(mod, reg, r_m)                          \
    /*_ASSERTE(mod < 4);*/                              \
    cmdByte((mod == 0) ?                                \
        expOr2(expBits(r_m, 3, 0), expBits(reg, 3, 3)) :    \
        expOr3(expNum(mod << 6), expBits(r_m, 3, 0), expBits(reg, 3, 3)))

/* also define the 16 bit override prefix */
#define x86_16bit(cmd)                              \
    cmdBlock2(cmdByte(expNum(0x66)), cmd)


/*****************************************************************************/
/* These are the addressing modes for the x86 CPU */

#define x86_mod_disp32(reg, disp32)                         \
    cmdBlock2(                                  \
    x86_mod(0, reg, expNum(5)),                         \
    cmdDWord(disp32))


#define x86_mod_ind(reg, ptr)                           \
    x86_mod(0, reg, ptr)    /* FIX reg can't be BP! */


#define x86_mod_ind_disp8(reg, ptr, disp8)                  \
    cmdBlock2(                                  \
    x86_mod(1, reg, ptr),                           \
    cmdByte(disp8))


#define x86_mod_ind_disp32(reg, ptr, disp32)                    \
    cmdBlock2(                                  \
    x86_mod(2, reg, ptr),                           \
    cmdDWord(disp32))

/* Rotor: This macro requires that disp be an int and not a
   short. See "Type promotion and sign extension" in the
   "Writing Cross-Platform Code" document in Rotor's documentation
   for a method that can verify that all places that call
   this macro use ints. */
#define x86_mod_ind_disp(reg, ptr, disp)                                \
    if ((int) (__int8) (disp) == (int) (disp)) {                        \
        x86_mod_ind_disp8(reg, ptr, disp);                              \
    } else {                                                            \
        x86_mod_ind_disp32(reg, ptr, disp);                             \
    }                                                                   \

#define x86_mod_reg(dstReg, srcReg)                         \
    x86_mod(3, dstReg, srcReg)


#define x86_mod_base_scale(reg, baseReg, scaleReg, scale)           \
    cmdBlock2(                                  \
    x86_mod(0, reg, expNum(4)),                         \
    x86_sib(scale, scaleReg, baseReg))


#define x86_mod_base_scale_disp8(reg, baseReg, scaleReg, disp8, scale)      \
    cmdBlock3(                                  \
    x86_mod(1, reg, expNum(4)),                         \
    x86_sib(scale, scaleReg, baseReg),                  \
    cmdByte(disp8))


#define x86_mod_base_scale_disp32(reg, baseReg, scaleReg, disp32, scale)    \
    cmdBlock3(                                  \
    x86_mod(2, reg, expNum(4)),                         \
    x86_sib(scale, scaleReg, baseReg),                  \
    cmdDWord(disp32))

/* Rotor: This macro requires that disp be an int and not a
   short. See "Type promotion and sign extension" in the
   "Writing Cross-Platform Code" document in Rotor's documentation
   for a method that can verify that all places that call
   this macro use ints. */
#define x86_mod_base_scale_disp(reg, baseReg, scaleReg, disp, scale)    \
    if ((int) (__int8) (disp) == (int) (disp)) {                        \
        x86_mod_base_scale_disp8(reg, baseReg, scaleReg, disp, scale);  \
    } else {                                                            \
        x86_mod_base_scale_disp32(reg, baseReg, scaleReg, disp, scale); \
    }                                                                   \

#define x86_mod_scale_disp32(reg, scaleReg, disp32, scale)          \
    cmdBlock3(                                  \
    x86_mod(0, reg, expNum(4)),                         \
    x86_sib(scale, scaleReg, expNum(5)),                    \
    cmdDWord(disp32))


/*****************************************************************************/
/* full x86 instructions, not we only defined the ones needed for omni conversion */

#define x86Byte     0       /* possible sizes */
#define x86Big      1

#define x86NoExtend 0       /* possible 'ext' values */
#define x86Extend   1

/********************** MOV instructions ***************************/

#define x86DirFrom  0       /* possible 'dir' values */
#define x86DirTo    2

#define x86_mov_reg(dir, size, addMode)                 \
    cmdBlock2(                                          \
        /*_ASSERTE(size == x86Byte | size == x86Big),*/ \
        cmdByte(expNum(0x88 | dir | size)),             \
        addMode)

#define x86_mov_reg_imm(size, reg, imm)                 \
    cmdBlock2(                                          \
    /*_ASSERTE(size == x86Byte | size == x86Big),*/     \
    cmdByte(expOr2(expNum(0xB0 | (size << 3)), reg)),   \
    if(size) { cmdDWord(imm); } else { cmdByte(imm); } )

/* In using this instruction The destination register for addMode should be 0 */
#define x86_mov_mem_imm(size, addMode, imm)             \
     cmdBlock4(                                         \
        _ASSERTE(size == x86Byte | size == x86Big),     \
        cmdByte(expNum(0xC6 | size)),                   \
        addMode,                                        \
        if (size) { cmdDWord(imm); } else { cmdByte(imm); } )

#define x86_movsx(size, addMode)                        \
    cmdBlock3(                                          \
        /*_ASSERTE(size == x86Byte | size == x86Big),*/ \
        cmdByte(expNum(0x0F)),                          \
        cmdByte(expNum(0xBE | size)),                   \
        addMode)

#define x86_movzx(size, addMode)                        \
    cmdBlock3(                                          \
        /*_ASSERTE(size == x86Byte | size == x86Big),*/ \
        cmdByte(expNum(0x0F)),                          \
        cmdByte(expNum(0xB6 | size)),                   \
        addMode)

#define x86_lea(addMode)                        \
    cmdBlock2(                              \
    cmdByte(expNum(0x8D)),                      \
    addMode)

#define x86_mov_segment_reg(dir, size, segmentRegPrefix,offset)  \
    cmdBlock3( \
      cmdByte(segmentRegPrefix), \
      cmdByte(expNum(0xA0 | size | dir)), \
      cmdDWord(offset))
/******************** Unary Arithmetic instructions **********************/

#define x86OpNot    2       /* values for unary op */
#define x86OpNeg    3
#define x86OpMul    4
#define x86OpIMul   5
#define x86OpDiv    6
#define x86OpIDiv   7

    // we only do register ops for now
#define x86_uarith(op, size, reg)                       \
    cmdBlock2(                                          \
        /*_ASSERTE(size == x86Byte | size == x86Big),*/ \
        cmdByte(expNum(0xF6 | size)),                   \
        cmdByte(expOr2(expNum(0xC0 | (op << 3)), reg)))

#define x86_inc_dec(neg, reg)                       \
    cmdByte(expOr2(expNum(0x40 | (neg << 3)), reg))


/******************** Binary Arithmetic instructions **********************/

#define x86OpAdd    0       /* possible values for binary op */
#define x86OpOr     1
#define x86OpAdc    2
#define x86OpSbb    3
#define x86OpAnd    4
#define x86OpSub    5
#define x86OpXor    6
#define x86OpCmp    7

#define x86_barith(op, size, addMode)                   \
     cmdBlock2(                                         \
        /*_ASSERTE(size == x86Byte | size == x86Big),*/ \
        cmdByte(expNum(0x02 | (op << 3) | size)),       \
        addMode)

#define x86_barith_imm(op, size, ext, reg, imm)         \
    cmdBlock3(                                          \
        /*_ASSERTE(!(ext == x86Extend && size == x86Byte)),*/ \
        /*_ASSERTE(size == x86Byte | size == x86Big),*/ \
        cmdByte(expNum(0x80 | size | (ext << 1))),      \
        cmdByte(expOr2(expNum(0xC0 | (op << 3)), reg)), \
        if(ext == x86NoExtend && size == x86Big) { cmdDWord(imm); } else { cmdByte(imm); } )


/********************* Shift instructions ************************/

#define x86ShiftLeft        4
#define x86ShiftRight       5
#define x86ShiftArithRight  7

#define x86_shift_imm(dir, reg, imm)                    \
    cmdBlock3(                              \
    cmdByte(expNum(0xC1)),                      \
    cmdByte(expOr2(expNum(0xC0 | (dir << 3)), reg)),        \
    cmdByte(imm))


#define x86_shift_cl(dir, reg)                      \
    cmdBlock2(                              \
    cmdByte(expNum(0xD3)),                      \
    cmdByte(expOr2(expNum(0xC0 | (dir << 3)), reg)))


/********************* Stack instructions ************************/

#define x86_push(reg)                           \
    cmdByte(expOr2(expNum(0x50), reg))

#define x86_push_imm(val)                       \
    if ((unsigned) val < 256) {                 \
        cmdBlock2(                              \
            cmdByte(0x6A),                      \
            cmdByte(val));                      \
    } else {                                    \
        cmdBlock2(                              \
            cmdByte(0x68),                      \
            cmdDWord(val));                     \
    }                                           \

#define x86_push_general(addMode) \
    cmdBlock2( \
        cmdByte(expNum(0xFF)), \
        addMode)

#define x86_pop(reg)                            \
    cmdByte(expOr2(expNum(0x58), reg))

#define x86_push_immediate() \
    cmdByte(0x68)

/********************* Control  instructions ************************/

#define x86_jmp_large()                         \
    cmdByte(expNum(0xE9))

#define x86_jmp_small()                         \
    cmdByte(expNum(0xEB))


#define x86_jmp_reg(reg)                    \
    cmdBlock2(                              \
        cmdByte(expNum(0xFF)),              \
        cmdByte(expOr2(expNum(0xE0), reg)))


#define x86CondOver         0
#define x86CondNotOver      1
#define x86CondBelow        2
#define x86CondAboveEq      3
#define x86CondEq           4
#define x86CondNotEq        5
#define x86CondBelowEq      6
#define x86CondAbove        7
#define x86CondSign         8
#define x86CondNotSign      9
#define x86CondParityEven   10
#define x86CondParityOdd    11
#define x86CondLt           12
#define x86CondGtEq         13
#define x86CondLtEq         14
#define x86CondGt           15
#define x86CondAlways       -1

#define x86Neg(cond) (cond ^ 1)

#define x86_jmp_cond_small(cond)        \
    if (cond == x86CondAlways) {        \
        x86_jmp_small();                \
    } else {                            \
        cmdByte(expNum(0x70 | cond));   \
    }

#define x86_jmp_cond_large(cond, neg)   \
    if (cond == x86CondAlways) {        \
        x86_jmp_large();                \
    } else {                            \
        cmdBlock2(                      \
            cmdByte(expNum(0x0F)),      \
            cmdByte(expNum(0x80 | cond))); \
    }

#define x86_jmp_abs_address(cond, absAddress, adj)          \
    cmdBlock2(                                              \
        x86_jmp_cond_large(cond, 0),                        \
        cmdDWord(adj ? ((signed)absAddress-((signed) outPtr)-sizeof(void*)) : (signed)absAddress); )

#define x86_set_cond(reg,cond)          \
    cmdBlock3(                          \
    cmdByte(expNum(0x0F)),              \
    cmdByte(expNum(0x90 | cond)), \
    cmdByte(expNum(0xC0 | reg)))

#define x86_loop() cmdByte(expNum(0xe2))

#define x86_loope() cmdByte(expNum(0xe1))

#define x86_loopne() cmdByte(expNum(0xe0))

#define x86_call_opcode()   \
    cmdByte(expNum(0xE8))


#define x86_call_abs_address(absAddress, adj) \
    cmdBlock2(                              \
        cmdByte(expNum(0xE8)),              \
        cmdDWord(adj ? (absAddress-((signed) outPtr)-sizeof(void*)) : (signed)absAddress) )

#define x86_call_reg(reg)                   \
    cmdBlock2(                              \
        cmdByte(expNum(0xFF)),              \
        cmdByte(expOr2(expNum(0xD0), reg)))

/* Rotor: This macro requires that disp be an int and not a
   short. See "Type promotion and sign extension" in the
   "Writing Cross-Platform Code" document in Rotor's documentation
   for a method that can verify that all places that call
   this macro use ints. */
#define x86_call_ind(reg, disp)             \
    cmdBlock2(                              \
    cmdByte(expNum(0xFF)),                  \
    if (disp == 0) {                        \
        x86_mod_ind(2, reg);                \
    } else {                                \
        if ((int) (__int8) (disp) == (int) (disp)) {\
            x86_mod_ind_disp8(2, reg, disp);        \
        } else {                                    \
            x86_mod_ind_disp32(2, reg, disp);       \
        }                                           \
    } )

#define x86_call_memory_indirect(adr)           \
    cmdBlock3( \
      cmdByte(expNum(0xFF)), \
      cmdByte(expNum(0x15)), \
      cmdDWord(adr))

#define x86_ret(bytes)                      \
    if (bytes) {                            \
        cmdBlock2(                          \
            cmdByte(expNum(0xC2)),          \
            cmdWord(expNum(bytes)));        \
    } else {                                \
        cmdByte(expNum(0xC3));              \
    }

/********************* FP  instructions ************************/
#define x86_FPLoad32    0
#define x86_FPLoad64    0
#define x86_FPLoad80    5
#define x86_FPStore32   2
#define x86_FPStore64   2
#define x86_FPStoreP32  3
#define x86_FPStoreP64  3
#define x86_FPStoreP80  7


#define x86_FLT32(addMode) \
    cmdBlock2(cmdByte(0xD9),addMode)

#define x86_FLT64(addMode) \
    cmdBlock2(cmdByte(0xDD),addMode)

#define x86_FLT80(addMode) \
    cmdBlock2(cmdByte(0xDB),addMode)

#define x86_FLD32(addMode) x86_FLT32(addMode)
#define x86_FLD64(addMode) x86_FLT64(addMode)
#define x86_FST32(addMode) x86_FLT32(addMode)
#define x86_FST64(addMode) x86_FLT64(addMode)

#define x86_FltAddP()  \
    cmdBlock2(cmdByte(expNum(0xde)), \
              cmdByte(expNum(0xc1)));

#define x86_FltSubP()  \
    cmdBlock2(cmdByte(expNum(0xde)), \
              cmdByte(expNum(0xe9)));

#define x86_FltMulP()  \
    cmdBlock2(cmdByte(expNum(0xde)), \
              cmdByte(expNum(0xc9)));

#define x86_FltDivP()  \
    cmdBlock2(cmdByte(expNum(0xde)), \
              cmdByte(expNum(0xf9)));

#define x86_FltComPP()  \
    cmdBlock2(cmdByte(expNum(0xde)), \
              cmdByte(expNum(0xd9)));

#define x86_FltUComPP()  \
    cmdBlock2(cmdByte(expNum(0xda)), \
              cmdByte(expNum(0xe9)));

#define x86_FltToggleSign()  \
    cmdBlock2(cmdByte(expNum(0xd9)), \
              cmdByte(expNum(0xe0)));

#define x86_FltStoreStatusWord() \
    cmdBlock2(cmdByte(expNum(0xdf)), \
              cmdByte(expNum(0xe0)));

#define x86_SAHF() cmdByte(expNum(0x9e))

/********************* Misc  instructions ************************/
#define X86_CARRY_FLAG  0x0001
#define X86_ZERO_FLAG   0x0040
#define X86 SIGN_FLAG   0x0080

#define x86_int(intNum)                         \
    cmdBlock2(                              \
    cmdByte(expNum(0xCD)),                      \
    cmdByte(expNum(intNum)))

#endif

#define x86_test(size, addMode)                 \
     cmdBlock2(                                 \
        /*_ASSERTE(size == x86Byte | size == x86Big),*/ \
        cmdByte(expNum(0x84 | size)),           \
        addMode)

#define x86_test_EAX(size, val)                 \
     cmdBlock2(                                 \
        /*_ASSERTE(size == x86Byte | size == x86Big);*/ \
        cmdByte(expNum(0xa8 | size)),           \
        cmdByte(expNum(val))

#define x86_std() cmdByte(expNum(0xFD))

#define x86_cld() cmdByte(expNum(0xFC))

#define x86_nop() cmdByte(expNum(0x90))

#define x86_break() cmdByte(expNum(0xCC))

#define x86_read_address(iStart, address)        \
address = (*(unsigned *)(((unsigned)iStart)+ ( ((*(BYTE *)iStart)==0xe8 || (*(BYTE *)iStart)==0xe9) ? 1 : 2)  ));

#define x86_patch_address(iStart, absAddress)                   \
{                                                               \
   unsigned * aStart = (unsigned *)(((unsigned)iStart)+(((*(BYTE *)iStart)==0xe8 || (*(BYTE *)iStart)==0xe9) ? 1 : 2));\
   *aStart = (unsigned)absAddress - ((unsigned) aStart + sizeof(void*));  \
}

#define x86_emit_istream_arg( arg_size, arg_ptr, reg )      \
{                                                           \
    cmdBlock2(                                  \
    cmdByte(expNum(0xE8)),                              \
        cmdDWord(arg_size) );                               \
    for(int i = 0; i < (int)arg_size; i++) {                \
        cmdByte(((BYTE *)arg_ptr)[i]);                      \
    }                                                       \
    cmdByte(expOr2(expNum(0x58), reg));                     \
}

#define x86_store_gp_arg(a_size, num, s_size)                                                \
{                                                                                            \
   _ASSERTE( num == 0 && s_size == 0 || num == 1 && s_size == sizeof(void *) );              \
   _ASSERTE( a_size == sizeof(void *) );                                                     \
   cmdByte(expOr2(expNum(0x50), (num+1)));                                                   \
   num++; s_size += sizeof(void *);                                                          \
}

#define x86_setup_frame()                                   \
{                                                           \
  cmdByte(expNum(0x55));           /* push ebp */           \
  cmdBlock2(                                    \
    cmdByte(expNum(0x8b)),     /* mov ebp,esp */        \
        cmdByte(expNum(0xec)) );                            \
  cmdByte(expNum(0x56));           /* push esi */           \
  cmdBlock2(                                    \
    cmdByte(expNum(0x33)),     /* xor esi,esi */        \
        cmdByte(expNum(0xf6)) );                            \
  cmdByte(expNum(0x56));           /* push esi */           \
}

#define x86_restore_frame()                                 \
{                                                           \
  cmdBlock3(                                    \
    cmdByte(expNum(0x8b)),     /* mov esi,[ebp-4] */    \
        cmdByte(expNum(0x75)),                              \
        cmdByte(expNum(0xfc)) );                            \
  cmdBlock2(                                    \
    cmdByte(expNum(0x8b)),     /* mov esp,ebp */        \
        cmdByte(expNum(0xe5)) );                            \
  cmdByte(expNum(0x5d));           /* pop ebp */            \
}

#define x86_restore_gp_arg(a_size, num, s_size)                                 \
{                                                                               \
  _ASSERTE( num == 0 && s_size == 0 || num == 1 && s_size == sizeof(void *) );  \
  _ASSERTE( a_size == sizeof(void *) );                                         \
  cmdBlock3(                                                        \
    cmdByte(expNum(0x8b)),/* mov ecd/edx,[ebp-4-s_size] */                  \
        cmdByte(expNum((0x45 + (0x8 << num)))),                                 \
        cmdByte(expNum((0xfc-s_size))) );                                       \
  num++; s_size += 4;                                                           \
}

#define emit_callhelper_prof1(fcn, id, arg)         \
{                                                   \
    x86_push_imm(arg);                              \
    mov_constant(CALLREG, fcn);                     \
    call_register(CALLREG);                         \
}

#define emit_callhelper_prof2(fcn, id, arg1, arg2)  \
{                                                   \
    x86_push_imm(arg2);                             \
    x86_push_imm(arg1);                             \
    mov_constant(CALLREG, fcn);                     \
    call_register(CALLREG);                         \
}


