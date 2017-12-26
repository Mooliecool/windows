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
/*                                 fjitdef.h                              */
/***************************************************************************/

/* Defines the code generators and helpers for the fast jit in a platform
   and chip neutral way. It is also 32 and 64 bit neutral. */

/* This file implements all of the opcodes via helper calls with the exception'
   of the call/return/jmps and direct stack manipulations */

/* a chip specific file can redefine any macros directly.  For an example of
   this see the file x86fit.h */

/* The top of stack may or may not be enregistered.  The macros
   enregisterTOS and deregisterTOS dynamically move and track the
   TOS. */

/*                               
                                */
/***************************************************************************/

#ifndef FJITDEF_H
#define FJITDEF_H

#include <float.h>   // for _isnan


/***************************************************************************
  The following macros are used in the JIT but either no high level
  implementation is provided or they are redefined in terms of another macro
***************************************************************************/

/* push/load result */
#define emit_pushresult_U4() emit_pushresult_I4()
#define emit_pushresult_U8() emit_pushresult_I8()

#define emit_loadresult_U4() emit_loadresult_I4()
#define emit_loadresult_U8() emit_loadresult_I8()

/* call/return */
#define emit_ret(argsSize, restoreAddr)        ret(argsSize, restoreAddr)
#define emit_call_abs_address(absAddress, adj) call_abs_address(absAddress, adj)

/* stack operations */
#define emit_POP_R4() emit_POP_I4()
#define emit_POP_R8() emit_POP_I8()
#define emit_DUP_R4() emit_DUP_I8()     // since R4 is always promoted to R8 on the stack
#define emit_DUP_R8() emit_DUP_I8()

/* relative jumps and misc*/
#define emit_jmp_abs_address(cond, absAddress, adj) jmp_abs_address(cond, absAddress, adj)
#define emit_read_address(iStart,address, errorCode)           read_address(iStart, address, errorCode)
#define emit_patch_address(iStart, absAddress, sIP, errorCode) patch_address(iStart, absAddress, sIP, errorCode)

/***************************************************************************
  The following macros must be defined for each chip.
  These consist of call/return and direct stack manipulations
***************************************************************************/

#if defined(_X86_) // Defines for the X86 platform

/**************************************************************************
   define the calling convention for calling internal helpers
**************************************************************************/
/* macros used to implement helper calls */

#define NUMBER_ARGUMENT_REGISTERS 2
#define INTERNAL_CALL false
#define EXTERNAL_CALL true
#define HELPER_CALL __stdcall
#define CALLER_CLEANS_STACK false


/**************************************************************************
   define abstract condition codes in terms of native machine condition codes
**************************************************************************/
#define CEE_CondBelow   x86CondBelow
#define CEE_CondAboveEq x86CondAboveEq
#define CEE_CondEq      x86CondEq
#define CEE_CondNotEq   x86CondNotEq
#define CEE_CondBelowEq x86CondBelowEq
#define CEE_CondAbove   x86CondAbove
#define CEE_CondLt      x86CondLt
#define CEE_CondGtEq    x86CondGtEq
#define CEE_CondLtEq    x86CondLtEq
#define CEE_CondGt      x86CondGt
#define CEE_CondAlways  x86CondAlways
#define CondNonZero     x86CondNotEq
#define CondZero        x86CondEq

/**************************************************************************
   define abstract registers in terms of native machine registers
**************************************************************************/
#define SIZE_ARGUMENT_REGISTER 4

#define SP             X86_ESP  // Stack pointer used to access the evaluation stack
#define FP             X86_EBP  // Frame pointer used to access the local variables and arguments
// Argument registers used to pass arguments
#define ARG_1          X86_ECX
#define ARG_2          X86_EDX
#define ARG_3          X86_EDI  // Never used but needs to be defined for macro expension
#define ARG_4          X86_EDI  // Never used but needs to be defined for macro expension
#define ARG_5          X86_EDI  // Never used but needs to be defined for macro expension
// Callee saved registers that can be used to store values across calls
#define CALLEE_SAVED_1 X86_ESI
#define CALLEE_SAVED_2 X86_EDI
#define TOS_REG_1      X86_EAX
// Result registers in which the value of the function is returned
#define RESULT_1       X86_EAX  // this should be the same as TOS_REG_1 on X86
#define RESULT_2       X86_EDX  // this cannot be the same as ARG_1
#define RESULT_1_OUT   RESULT_1 // The values are returned and recieved in the same registeres on x86
#define RESULT_2_OUT   RESULT_2

// Indirect call register
#define CALLREG        X86_EAX  // register via which indirect calls should be made
#define ZEROREG        X86_EAX  // register that always reads 'zero' ( not used on x86 )

/**************************************************************************
   define macros used by high level macros to their native implementation
**************************************************************************/
#define push_register(r, slot)                 x86_push(r)
#define pop_register(r, slot)                  x86_pop(r)
#define mov_register(r1,r2)                    x86_mov_reg(x86DirTo,x86Big,x86_mod_reg(r1,r2))
#define mov_register_indirect_to(r1,r2)        load_indirect_dword_signextend(r1,r2,true)
#define mov_register_indirect_from(r1,r2)      store_indirect_32bits(r1,r2,true)
#define mov_constant(r,c)                      x86_mov_reg_imm(x86Big,r,(unsigned int)c)
#define load_indirect_byte_signextend(r1,r2)        x86_movsx(x86Byte,x86_mod_ind(r1,r2))
#define load_indirect_byte_zeroextend(r1,r2)        x86_movzx(x86Byte,x86_mod_ind(r1,r2))
#define load_indirect_word_signextend(r1,r2,align)  x86_movsx(x86Big,x86_mod_ind(r1,r2))
#define load_indirect_word_zeroextend(r1,r2,align)  x86_movzx(x86Big,x86_mod_ind(r1,r2))
#define load_indirect_dword_signextend(r1,r2,align) x86_mov_reg(x86DirTo,x86Big,x86_mod_ind(r1,r2))
#define load_indirect_dword_zeroextend(r1,r2,align) x86_mov_reg(x86DirTo,x86Big,x86_mod_ind(r1,r2))
#define load_single( reg, offset )             { _ASSERTE(false); }
#define load_double( reg, offset )             { _ASSERTE(false); }
#define store_indirect_8bits(r1,r2)            x86_mov_reg(x86DirFrom,x86Byte,x86_mod_ind(r1,r2))
#define store_indirect_16bits(r1,r2,align)     x86_16bit(x86_mov_reg(x86DirFrom,x86Big,x86_mod_ind(r1,r2)))
#define store_indirect_32bits(r1,r2,align)     x86_mov_reg(x86DirFrom,x86Big,x86_mod_ind(r1,r2))
#define store_gp_arg(a_size, num, s_size)      x86_store_gp_arg(a_size, num, s_size)
#define store_float_arg(a_size, num, g_num, s_size)   { _ASSERTE(false); num++; }
#define restore_gp_arg(a_size, num, s_size)    x86_restore_gp_arg(a_size, num, s_size)
#define restore_float_arg(a_size, num, g_num, s_size) { _ASSERTE(false); num++; }
#define move_unaligned_arg(src, dst)           { _ASSERTE(false); }
#define add_constant(r,c)                      if ((unsigned)c < 128) {x86_barith_imm(x86OpAdd,x86Big,x86Extend,r,c); } \
                                               else {x86_barith_imm(x86OpAdd,x86Big,x86NoExtend,r,c); }
#define and_constant(r,c)                      if (c < 128) {x86_barith_imm(x86OpAnd,x86Big,x86Extend,r,c); } \
                                               else {x86_barith_imm(x86OpAnd,x86Big,x86NoExtend,r,c); }
#define sub_register(r1,r2)                    x86_barith(x86OpSub, x86Big, x86_mod_reg(r1,r2))
#define compare_register(r1,r2,cond)           x86_barith(x86OpCmp,x86Big,x86_mod_reg(r1,r2))
#define and_register(r1,r2,cr)                 x86_test(x86Big, x86_mod_reg(r1, r2))
#define mult_power_of_2(r,c)                   x86_shift_imm(x86ShiftLeft,r,c)
#define div_power_of_2(r,c)                    x86_shift_imm(x86ShiftRight,r,c)
#define setup_frame()                          x86_setup_frame()
#define restore_frame(hasRetBuff,resetupFrame) x86_restore_frame()
#define emit_call_frame(n)                     grow(n, false)
#define call_frame_size(args, enreg)           (args - enreg)
#define ret(x, restoreAddr)                    x86_ret(x)
#define emit_restore_state()                   {x86_mov_reg(x86DirTo, x86Big, x86_mod_ind_disp(X86_ESI, X86_EBP, \
                                                0-sizeof(void*)));}
#define save_return_address()                  {} // call instruction always saves ret. address on stack
#define restore_return_address()               {} // ret instruction reads ret. address from the stack
#define call_register(r)                       x86_call_reg(r)
#define call_abs_address(absAddress, adj)      x86_call_abs_address(absAddress, adj)
#define mark_retbuff_callsite(size)            { _ASSERTE(false); }
#define jmp_register(r,preserveRetAddr)        x86_jmp_reg(r)
#define jmp_abs_address(cond, absAddress, adj) x86_jmp_abs_address(cond, absAddress, adj)
#define read_address(iStart,address,errorCode) x86_read_address(iStart, address)
#define patch_address(iStart, absAddress, sIP, errorCode) x86_patch_address(iStart, absAddress)
#define emit_istream_arg(arg_size,arg_ptr,reg) x86_emit_istream_arg( arg_size, arg_ptr, reg )
#define emit_break()                           x86_break()
#define nop()                                  x86_nop()
#define emit_il_nop()                          x86_cld() // we use cld for a nop since the native nop is used for seq. points
#define emit_SWITCH(limit)                     x86_SWITCH(limit)
#define emit_call_memory_indirect(c, retBuff)  x86_call_memory_indirect(c)
#define emit_conv_R4toR8                       x86_emit_conv_R4toR8
#define emit_conv_R8toR4                       x86_emit_conv_R8toR4
#define emit_narrow_R8toR4                     x86_narrow_R8toR4
#define emit_pushresult_R4()                   x86_pushresult_R4
#define emit_pushresult_R8()                   x86_pushresult_R8
#define emit_loadresult_R4()                   x86_emit_loadresult_R4()
#define emit_loadresult_R8()                   x86_emit_loadresult_R8()

#define emit_relocate_retval_filter()
#define emit_store_toc_reg(helper)
#define emit_restore_toc_reg(helper)

#include "i386/x86fjit.h"

#elif defined(_PPC_)  // defined for the PowerPC platform

/**************************************************************************
   define the calling convention for calling internal helpers
**************************************************************************/
#define NUMBER_ARGUMENT_REGISTERS 8
#define INTERNAL_CALL true
#define EXTERNAL_CALL true
#define HELPER_CALL
//#define CALLER_CLEANS_STACK false


/**************************************************************************
   define abstract condition codes in terms of native machine condition codes
**************************************************************************/
#define CEE_CondBelow   ppcCondBelow
#define CEE_CondAboveEq ppcCondAboveEq
#define CEE_CondEq      ppcCondEq
#define CEE_CondNotEq   ppcCondNotEq
#define CEE_CondBelowEq ppcCondBelowEq
#define CEE_CondAbove   ppcCondAbove
#define CEE_CondLt      ppcCondLt
#define CEE_CondGtEq    ppcCondGtEq
#define CEE_CondLtEq    ppcCondLtEq
#define CEE_CondGt      ppcCondGt
#define CEE_CondAlways  ppcCondAlways
#define CondNonZero     ppcCondNotEq
#define CondZero        ppcCondEq

/**************************************************************************
   define abstract registers in terms of native machine registers
**************************************************************************/
#define SIZE_ARGUMENT_REGISTER 4

#define SP             R1   // Stack pointer used to access the evaluation stack
#define FP             R30  // Frame pointer used to access the local variables and arguments
// Argument registers used to pass arguments
#define ARG_1          R3
#define ARG_2          R4
#define ARG_3          R5
#define ARG_4          R6
#define ARG_5          R7
// Callee saved registers that can be used to store values across calls
#define CALLEE_SAVED_1 R29
#define CALLEE_SAVED_2 R31
#define TOS_REG_1      R11
// Result registers in which the value of the function is returned
#define RESULT_1       R3
#define RESULT_2       R4  // this cannot be the same as ARG_1
#define RESULT_1_OUT   RESULT_1 // The values are returned and recieved in the same registeres on PPC
#define RESULT_2_OUT   RESULT_2

// Indirect call register
#define CALLREG        R12 // register via which indirect calls should be made
#define ZEROREG        R0  // register that always reads 'zero' ( not used on PPC )
/**************************************************************************
   define macros used by high level macros to their native implementation
**************************************************************************/

#define push_register(r, slot)                 ppc_stwu(r, R1, -4) /* stwu reg, -4(r1) */
#define pop_register(r, slot)                  { ppc_lwz(r, R1, 0); ppc_addi( R1, R1, 4 ) } /* ldw reg, 0(r1), addi r1, 4 */
#define mov_register(r1,r2)                    ppc_or( r1, r2, r2, 0 ) /* or reg1, reg2, reg2 */
#define mov_register_indirect_to(r1,r2)        load_indirect_dword_signextend(r1,r2,true)
#define mov_register_indirect_from(r1,r2)      store_indirect_32bits(r1,r2,true)
#define mov_constant(r,c)                      ppc_move_constant(r,c)
#define load_indirect_byte_signextend(r1,r2)         { ppc_lbz(r1, r2, 0); ppc_extsb(r1, r1, 0) }/* lbz reg1,0(reg2);extsb r1,r1*/
#define load_indirect_byte_zeroextend(r1,r2)         ppc_lbz(r1, r2, 0) /* lbz reg1,0(reg2) */
#define load_indirect_word_signextend(r1,r2,align)   ppc_lha(r1, r2, 0) /* lha reg1,0(reg2) */
#define load_indirect_word_zeroextend(r1,r2,align)   ppc_lhz(r1, r2, 0) /* lhz reg1,0(reg2) */
#define load_indirect_dword_signextend(r1,r2,align)  ppc_lwz(r1, r2, 0) /* ldw reg1,0(reg2) */
#define load_indirect_dword_zeroextend(r1,r2,align)  ppc_lwz(r1, r2, 0) /* ldw reg1,0(reg2) */
#define load_single( reg, offset )                   { ppc_lfs( argFloatRegistersMap[reg], R1, offset ) }
#define load_double( reg, offset )                   { ppc_lfd( argFloatRegistersMap[reg], R1, offset ) }
#define store_indirect_8bits(r1,r2)                  ppc_stb(r1, r2, 0) /* stb reg1,0(reg2) */
#define store_indirect_16bits(r1,r2,align)           ppc_sth(r1, r2, 0) /* sth reg1,0(reg2) */
#define store_indirect_32bits(r1,r2,align)           ppc_stw(r1, r2, 0) /* stw reg1, 0(reg2) */
#define store_gp_arg(a_size, num, s_size)      ppc_store_gp_arg(a_size, num, s_size)
#define store_float_arg(a_size, num, g_num, s_size)   ppc_store_float_arg(a_size, num, g_num, s_size)
#define restore_gp_arg(a_size, num, s_size)      ppc_restore_gp_arg(a_size, num, s_size)
#define restore_float_arg(a_size, num, g_num, s_size)   ppc_restore_float_arg(a_size, num, g_num, s_size)
#define move_unaligned_arg(src, dst)           { _ASSERTE(false); }
#define sub_register(r1,r2)                    ppc_subf(r1, r2, r1, 0, 0)
#define compare_register(r1,r2,cond)           ppc_compare_register(r1,r2,cond)
#define and_register(r1,r2,c)                  {if (!c) ppc_and(r1, r1, r2, 0) else ppc_and(r1, r1, r2, 1)}
#define mult_power_of_2(r,c)                   _ASSERTE(c < 0x1F && r != R0); ppc_addi( R0, R0, c & 0x1F); \
                                               ppc_slw(r, r, R0, 0)
#define div_power_of_2(r,c)                    _ASSERTE(c < 0x1F && r != R0); ppc_addi( R0, R0, c & 0x1F); \
                                               ppc_srw(r, r, R0, 0)
#define add_constant(r,c)                      ppc_add_constant(r, c)
#define and_constant(r,c)                      ppc_and_constant(r, c)
#define setup_frame()                          ppc_setup_frame()
#define restore_frame(hasRetBuff,resetupFrame) ppc_restore_frame(resetupFrame)
#define emit_call_frame(n)                     ppc_stwu(R30, R1, (((unsigned)(-(int)(n))) & 0xFFFF))
#define call_frame_size(args, enreg)           (args + sizeof(prolog_frame))
#define mark_retbuff_callsite(size)            ppc_oris(R0,R0,0)
#define ret(x, restoreAddr)                    ppc_ret(x, restoreAddr)
#define emit_restore_state()                   { if (mapInfo.savedIP) ppc_lwz(R29, R30, 0x4) }
#define save_return_address()                  { ppc_mfspr(R0, PPC_MOVE_LR); ppc_stwu(R0, R1, -4) }
#define restore_return_address()               { ppc_lwz(R12, R1, 0); ppc_addi( R1, R1, 4 ); }
#define call_register(r)                       ppc_call_register(r)
#define jmp_register(r,preserveRetAddr)        ppc_jmp_reg(r,preserveRetAddr)
#define call_abs_address(absAddress, adj)      ppc_call_abs_address(absAddress, adj)
#define jmp_abs_address(cond, absAddress, adj) ppc_jmp_abs_address(cond, absAddress, adj)
#define read_address(iStart,address, errorCode)           ppc_read_address(iStart, address, errorCode)
#define patch_address(iStart, absAddress, sIP, errorCode) ppc_patch_address(iStart, absAddress, sIP, errorCode)
#define emit_istream_arg(arg_size,arg_ptr,reg) ppc_emit_istream_arg( arg_size, arg_ptr, reg )
#define nop()                                  ppc_nop()
#define emit_break()                           ppc_break()
#define emit_il_nop()                          ppc_cld()
#define emit_SWITCH(limit)                     ppc_SWITCH(limit)
#define emit_call_memory_indirect(c, retBuff)  ppc_call_memory_indirect((unsigned)c)
#define emit_conv_R4toR8                       ppc_emit_conv_R4toR8
#define emit_conv_R8toR4                       ppc_emit_conv_R8toR4
#define emit_narrow_R8toR4(nativeOff,ilOff)    ppc_narrow_R8toR4(nativeOff,ilOff)
#define emit_pushresult_R4()                   ppc_pushresult_R4()
#define emit_pushresult_R8()                   ppc_pushresult_R8()
#define emit_loadresult_R4()                   ppc_emit_loadresult_R4()
#define emit_loadresult_R8()                   ppc_emit_loadresult_R8()

#define emit_relocate_retval_filter()
#define emit_store_toc_reg(helper)             ppc_emit_store_toc_reg(helper)
#define emit_restore_toc_reg(helper)           ppc_emit_restore_toc_reg(helper)

#include "ppc/ppcfjit.h"

#else  // could not recognize the platform
#error "Platform not supported"
#endif

#ifndef SCHAR_MAX

#define SCHAR_MAX 127.0       // Maximum signed char value
#define SCHAR_MIN -128.0      // Minimum signed char value
#define UCHAR_MAX 255.0       //Maximum unsigned char value
#define USHRT_MAX 65535.0     //Maximum unsigned short value
#define SHRT_MAX 32767.0      //Maximum (signed) short value
#define SHRT_MIN -32768       //Minimum (signed) short value
#define UINT_MAX 4294967295.0 //Maximum unsigned int value
#define INT_MAX 2147483647.0  // Maximum (signed) int value
#define INT_MIN -2147483648.0 // Minimum (unsigned) int value

#endif

/*******************************************************************************/
#ifndef emit_conv_R4toR
#define emit_conv_R4toR() { emit_conv_R4toR8() }
#endif
#ifndef emit_conv_R8toR
#define emit_conv_R8toR() { } /* nop */
#endif
#ifndef emit_conv_RtoR4
#define emit_conv_RtoR4() { emit_conv_R8toR4() }
#endif
#ifndef emit_conv_RtoR8
#define emit_conv_RtoR8() { } /* nop */
#endif

/************************************************************************************
Macros for manipulating top of stack (TOS). TOS is normally stored in a register
defined as TOS_REG_1.
*************************************************************************************/
#ifndef  deregisterTOS
#define deregisterTOS                 \
{                                     \
   if (inRegTOS)                      \
      push_register(TOS_REG_1, true); \
   inRegTOS = false;                  \
}
#endif // deregisterTOS

#ifndef  enregisterTOS
#define enregisterTOS                    \
   if (!inRegTOS)                        \
     { pop_register(TOS_REG_1, true); }  \
   inRegTOS = true;
#endif // enregisterTOS


#ifndef emit_mov_TOS_reg
#define emit_mov_TOS_reg(reg)      \
{                                  \
   if (inRegTOS) {                 \
      mov_register(reg,TOS_REG_1); \
      inRegTOS = false;            \
   }                               \
   else {                          \
      pop_register(reg, true);     \
   }                               \
}
#endif // !emit_mov_TOS_reg

#ifndef emit_mov_reg_TOS
#define emit_mov_reg_TOS(reg)            \
{                                        \
   if (inRegTOS) {                       \
      push_register(TOS_REG_1, true);    \
   }                                     \
  mov_register(TOS_REG_1, reg);          \
  inRegTOS = true;                      \
}
#endif // !emit_reg_TOS

/*
emit_save_TOS copies the TOS to a save area in the frame or register but leaves
the current value on the TOS. emit_restore_TOS pushes the saved value onto the TOS.

Used for new obj, since constructors doesn't return the constructed object. Also used for
calli since the target address needs to come off the stack while building the call frame
*/

#define emit_save_TOS()                                                \
{                                                                      \
    if (inRegTOS)                                                      \
        { mov_register(CALLEE_SAVED_1,TOS_REG_1); }                    \
    else                                                               \
        { pop_register(CALLEE_SAVED_1, true);                          \
          push_register(CALLEE_SAVED_1, true);  };                     \
}

#define emit_restore_TOS()                                             \
{                                                                      \
    deregisterTOS;                                                     \
    mov_register(TOS_REG_1,CALLEE_SAVED_1);                            \
    inRegTOS = true;                                                   \
    mov_constant(CALLEE_SAVED_1,0);                                    \
}


#ifndef emit_pushconstant_4
#define emit_pushconstant_4(val) \
{ \
   deregisterTOS; \
   mov_constant(TOS_REG_1,val); \
   inRegTOS = true; \
}
#endif // !emit_pushconstant_4

#ifndef emit_pushconstant_8
#define emit_pushconstant_8(val)                          \
{                                                         \
   int v1 = (int) ((val >> 32) & 0xffffffff);             \
   int v2 = (int) (val & 0xffffffff);                     \
   deregisterTOS;                                         \
   if ( SIZE_STACK_SLOT < 8 )                             \
   {                                                      \
     emit_pushconstant_4((BIGENDIAN_MACHINE ? v2: v1));   \
     deregisterTOS;                                       \
     emit_pushconstant_4((BIGENDIAN_MACHINE ? v1: v2));   \
     inRegTOS = true;                                     \
   }                                                      \
   else                                                   \
   {                                                      \
     grow( SIZE_STACK_SLOT, false );                      \
     mov_register(TOS_REG_1,SP);                          \
     if (STACK_BUFFER) add_constant(TOS_REG_1, (int)STACK_BUFFER); \
     mov_constant(RESULT_1,(BIGENDIAN_MACHINE ? v1: v2)); \
     mov_register_indirect_from(RESULT_1, TOS_REG_1);     \
     add_constant(TOS_REG_1, (sizeof(void *)));           \
     mov_constant(RESULT_1,(BIGENDIAN_MACHINE ? v2: v1)); \
     mov_register_indirect_from(RESULT_1, TOS_REG_1);     \
   }                                                      \
}
#endif // !emit_pushconstant_8

#ifndef emit_pushconstant_Ptr
#define emit_pushconstant_Ptr(val)\
{ \
   deregisterTOS; \
   mov_constant(TOS_REG_1,val); \
   inRegTOS = true; \
}
#endif // !emit_pushconstant_Ptr

/*************************************************************************************
        call/return macros
*************************************************************************************/


#ifndef grow
#define grow(n,zeroInitialized)                                   \
{                                                                 \
    if (zeroInitialized)                                          \
    {                                                             \
        _ASSERTE(((n) % (SIZE_STACK_SLOT/sizeof(void *))) == 0);  \
        unsigned l_slots = (n) / (SIZE_STACK_SLOT/sizeof(void *));\
        mov_constant(CALLEE_SAVED_1, (l_slots));                  \
        deregisterTOS;                                            \
        mov_constant(TOS_REG_1,0);                                \
        unsigned char* label = outPtr;                            \
        push_register(TOS_REG_1, false);                          \
        add_constant(CALLEE_SAVED_1,(unsigned)(-1));              \
        and_register(CALLEE_SAVED_1, CALLEE_SAVED_1, 1);          \
        jmp_abs_address(CondNonZero,label, true);                 \
    }                                                             \
    else                                                          \
    {                                                             \
        _ASSERTE( ((n) % SIZE_STACK_SLOT == 0) );                 \
        unsigned temp_n = n;                                      \
        int temp_p = (int)(PAGE_SIZE-SIZE_STACK_SLOT);            \
        while ( temp_n >= PAGE_SIZE)                              \
        {                                                         \
           add_constant(SP,-temp_p);                              \
           /* touch allocated page */                             \
           push_register(TOS_REG_1, true);                        \
           temp_n -= PAGE_SIZE;                                   \
        }                                                         \
        if (temp_n)                                               \
           add_constant(SP,-(int)(temp_n));                       \
    }                                                             \
}
#endif // !grow

#ifndef emit_grow
#define emit_grow(n) grow(n,false)
#endif // !emit_grow

#ifndef emit_drop
#define emit_drop(n)                          \
{                                             \
   if (n)                                     \
   {                                          \
      _ASSERTE((n) % SIZE_STACK_SLOT == 0);   \
      add_constant(SP,n);                     \
   }                                          \
}
#endif // !emit_drop

#ifndef emit_prolog
#define emit_prolog(locals)                     \
{                                               \
   setup_frame();                               \
   storeEnregisteredArguments();                \
   ON_X86_ONLY(if (locals))                     \
      grow(locals ON_PPC_ONLY(+1) ON_SPARC_ONLY(+1),true); /*zero initialized */  \
   if ( ALIGN_ARGS )                            \
      alignArguments();                         \
}
#endif // !emit_prolog

        // check to see that the stack is not corrupted only in debug code
#ifdef _DEBUG
#define emit_stack_check(localWords)                                                    \
{                                                                                       \
    _ASSERTE( !inRegTOS );                                                              \
    callInfo.reset();                                                                   \
    emit_reg_arg( SP, 3, INTERNAL_CALL); /*push_register(SP); */                        \
    emit_reg_arg( FP, 2, INTERNAL_CALL); /*push_register(FP); */                        \
    emit_arg( sizeof(prolog_data) + (localWords)*sizeof(void*) + STACK_BUFFER, 1, INTERNAL_CALL );     \
    emit_callhelper_I4I4I4(check_stack);                                                \
}
#ifdef DECLARE_HELPERS
void HELPER_CALL check_stack(int frameSize, BYTE* fp, BYTE* sp) {
        if (sp + frameSize != fp)
                _ASSERTE(!"ESP not correct on method exit.  Did you forget a leave?");
}
#endif // DECLARE_HELPERS
#else  // !_DEBUG
#define emit_stack_check(zeroCnt)
#endif // _DEBUG

#ifndef emit_return
#define emit_return(argsSize, hasRetBuff) \
{                                         \
   restore_frame(hasRetBuff,false);       \
   ret(argsSize, false);                  \
}
#endif // !emit_return

#ifndef emit_jmp_absolute
#define emit_jmp_absolute(address)   \
{                                    \
   mov_constant(CALLREG,address);    \
   jmp_register(CALLREG, true);      \
}
#endif // !emit_jmp_absolute

#ifndef emit_compute_virtaddress
#define emit_compute_virtaddress(vt_offset, sizeRetBuff) \
{  deregisterTOS; \
   mov_register_indirect_to(TOS_REG_1, \
     (ReturnBufferFirst && sizeRetBuff) ? ARG_2 : ARG_1); \
   add_constant(TOS_REG_1,vt_offset); \
   mov_register_indirect_to(TOS_REG_1,TOS_REG_1); \
   push_register(TOS_REG_1, true); \
   _ASSERTE(inRegTOS == false); \
}
#endif // !emit_compute_virtaddress

#ifndef emit_callvirt
#define emit_callvirt(vt_offset, sizeRetBuff)       \
{                                                   \
   _ASSERTE( CALLREG != TOS_REG_1 || !inRegTOS );   \
   mov_register_indirect_to(CALLREG,                \
     (ReturnBufferFirst && sizeRetBuff) ? ARG_2 : ARG_1); \
   add_constant(CALLREG,vt_offset);                 \
   mov_register_indirect_to(CALLREG,CALLREG);       \
   call_register(CALLREG);                          \
   if (M_RETBUFF_CALLSITE && sizeRetBuff )          \
     mark_retbuff_callsite(sizeRetBuff)             \
}
#endif // !emit_callvirt

#ifndef emit_check_null_reference
#define emit_check_null_reference(testTOS)            \
{                                                     \
   if (testTOS)                                       \
   {                                                  \
     enregisterTOS;                                   \
     load_indirect_byte_signextend(ARG_1,TOS_REG_1);  \
   }                                                  \
   else                                               \
     load_indirect_byte_signextend(TOS_REG_1,ARG_1);  \
}
#endif // !emit_check_this_null_reference

// The EE can demand that some calls use a helper function to find the method to call.
// The first argument to the helper function is the run-time type.
// We get this by dereferencing the object pointer.
// The second argument is the handle of the parent of the method.
// (i.e. the class of the method.) This is the argument to the macro.
// The third argument is the method descriptor for the static type
// (i.e.possibly a superclass).
#ifndef emit_ldvirtftn_helper
#define emit_ldvirtftn_helper(memberRef, parentToken) \
{\
        CORINFO_CLASS_HANDLE tokenType;\
        TokenToHandle(memberRef, tokenType); /* Push MethodHandle. */ \
        TokenToHandle(parentToken, tokenType); /* Push ClassHandle. */ \
        callInfo.reset();\
        emit_tos_arg( 1, INTERNAL_CALL ); /* ClassHandle */ \
        emit_tos_arg( 2, INTERNAL_CALL ); /* MethodHandle */ \
        emit_tos_arg( 3, INTERNAL_CALL ); /* obj * */ \
        emit_callhelper_I4I4I4_I4(ldvirtftn_helper);\
        emit_pushresult_I4();\
}
#ifdef DECLARE_HELPERS
void* HELPER_CALL ldvirtftn_helper(void* classHandle, void* methodHandle, void* obj)
{
    // reverse order of arguments and call the actual helper
    return HCALL3(void*, FJit_pHlpGenericVirtual, void*, obj, void*, classHandle, void*, methodHandle);
}
#endif // DECLARE_HELPERS
#endif // !emit_ldvirtftn_helper

#ifndef emit_calli
#define emit_calli(sizeRetBuff)    \
{                                  \
   if ( CALLREG == TOS_REG_1 )     \
     {enregisterTOS; }             \
   else                            \
     emit_mov_TOS_reg( CALLREG )   \
   call_register(CALLREG);         \
   if (M_RETBUFF_CALLSITE && sizeRetBuff )          \
     mark_retbuff_callsite(sizeRetBuff)             \
   inRegTOS = false;               \
}
#endif // !emit_calli

#ifndef emit_ldvtable_address
#define emit_ldvtable_address(hint, offset)\
{ \
   _ASSERTE(inRegTOS); \
   mov_register(ARG_1,TOS_REG_1); \
   mov_constant(TOS_REG_1,hint); \
   push_register(TOS_REG_1, true); \
   mov_register_indirect_to(TOS_REG_1,TOS_REG_1); \
   call_register(TOS_REG_1); \
   add_constant(RESULT_1,offset); \
   mov_register_indirect_to(TOS_REG_1,RESULT_1); \
   inRegTOS = true; \
}
#endif // !emit_ldvtable_address

#ifndef emit_ldvtable_address_new
#define emit_ldvtable_address_new(interface_offset, vt_offset) \
{ \
   _ASSERTE(inRegTOS); \
   mov_register_indirect_to(TOS_REG_1,TOS_REG_1); \
   add_constant(TOS_REG_1,offsetOfEEClass); \
   mov_register_indirect_to(TOS_REG_1,TOS_REG_1); \
   add_constant(TOS_REG_1,offsetOfInterfaceTable); \
   mov_register_indirect_to(TOS_REG_1,TOS_REG_1); \
   add_constant(TOS_REG_1,interface_offset); \
   mov_register_indirect_to(TOS_REG_1,TOS_REG_1); \
   add_constant(TOS_REG_1,vt_offset); \
   mov_register_indirect_to(TOS_REG_1,TOS_REG_1); \
}
#endif // !emit_ldvtable_address_new

#ifndef emit_callinterface
#define emit_callinterface(vt_offset,hint, sizeRetBuff)\
{ \
   mov_constant(TOS_REG_1,hint); \
   push_register(TOS_REG_1, true); \
   inRegTOS = false; \
   mov_register_indirect_to(CALLREG, CALLREG); \
   call_register(CALLREG); \
   add_constant(RESULT_1,vt_offset); \
   mov_register_indirect_to(CALLREG,RESULT_1); \
   call_register(CALLREG); \
   if (M_RETBUFF_CALLSITE && sizeRetBuff )          \
     mark_retbuff_callsite(sizeRetBuff)             \
}
#endif // !emit_callinterface

#ifndef emit_compute_interface_new
#define emit_compute_interface_new(interface_offset, vt_offset, sizeRetBuff) \
{ \
   mov_register_indirect_to(TOS_REG_1, \
     (ReturnBufferFirst && sizeRetBuff) ? ARG_2 : ARG_1); \
   add_constant(TOS_REG_1,offsetOfEEClass); \
   mov_register_indirect_to(TOS_REG_1,TOS_REG_1); \
   add_constant(TOS_REG_1,offsetOfInterfaceTable); \
   mov_register_indirect_to(TOS_REG_1,TOS_REG_1); \
   add_constant(TOS_REG_1,interface_offset); \
   mov_register_indirect_to(TOS_REG_1,TOS_REG_1); \
   add_constant(TOS_REG_1,vt_offset); \
   mov_register_indirect_to(TOS_REG_1,TOS_REG_1); \
   push_register(TOS_REG_1, true); \
}
#endif // !emit_computed_interface_new

#ifndef emit_callinterface_new
#define emit_callinterface_new(interface_offset, vt_offset, sizeRetBuff) \
{                                                                           \
   _ASSERTE( CALLREG != TOS_REG_1 || !inRegTOS );                           \
   mov_register_indirect_to(CALLREG, \
        (ReturnBufferFirst && sizeRetBuff) ? ARG_2 : ARG_1); \
   add_constant(CALLREG,offsetOfEEClass); \
   mov_register_indirect_to(CALLREG,CALLREG); \
   add_constant(CALLREG,offsetOfInterfaceTable); \
   mov_register_indirect_to(CALLREG,CALLREG); \
   add_constant(CALLREG,interface_offset); \
   mov_register_indirect_to(CALLREG,CALLREG); \
   add_constant(CALLREG,vt_offset); \
   mov_register_indirect_to(CALLREG,CALLREG); \
   call_register(CALLREG); \
   if (M_RETBUFF_CALLSITE && sizeRetBuff )          \
     mark_retbuff_callsite(sizeRetBuff)             \
}
#endif // !emit_callinterface_new

#ifndef  emit_callnonvirt
#define emit_callnonvirt(ftnptr, sizeRetBuff, indirect) \
{                                                \
   _ASSERTE( CALLREG != TOS_REG_1 || !inRegTOS );\
   mov_constant(CALLREG,ftnptr);                 \
   if (indirect)                                 \
     { mov_register_indirect_to(CALLREG,CALLREG); } \
   call_register(CALLREG);                       \
   if (M_RETBUFF_CALLSITE && sizeRetBuff )       \
     mark_retbuff_callsite(sizeRetBuff)          \
}
#endif // !emit_callnonvirt

#ifndef emit_tail_call
#define emit_tail_call(sizeCaller, sizeTarget, Flags ) {                   \
    emit_LDC_I(sizeCaller);                                                \
    emit_LDC_I(sizeTarget);                                                \
    emit_LDC_I(Flags);                                                     \
    }
#endif // !emit_tail_call


#ifndef emit_call_stub
#define emit_call_stub( stubAddress )                                      \
{                                                                          \
    _ASSERTE(!inRegTOS); /* TOS_REG is used for custom call conv */        \
    if ( !NUMBER_ARGUMENT_REGISTERS )                                      \
      { mov_register_indirect_to(ARG_1, SP); }                             \
    mov_register_indirect_to(TOS_REG_1, ARG_1);                            \
    mov_constant(CALLREG, stubAddress);                                    \
    call_register(CALLREG);                                                \
}
#endif // !emit_call_stub

#ifndef emit_set_zero
#define emit_set_zero(offset)                          \
{                                                      \
    _ASSERTE(!inRegTOS); /* I trash EAX */             \
    mov_register(TOS_REG_1,SP);                        \
    add_constant(TOS_REG_1,offset);                    \
    if (!HASZEROREG)                                   \
    {                                                  \
        /* since this is going to be trashed*/         \
        push_register(ARG_1, true);                    \
        mov_constant(ARG_1,0);                         \
        mov_register_indirect_from(ARG_1,TOS_REG_1);   \
        pop_register(ARG_1, true); /* restore */       \
    }                                                  \
    else                                               \
    {                                                  \
        mov_register_indirect_from(ZEROREG,TOS_REG_1); \
    }                                                  \
}
#endif // !emit_set_zero

#ifndef emit_getSP
#define emit_getSP(n)\
{ \
   deregisterTOS; \
   mov_register(TOS_REG_1,SP); \
   add_constant(TOS_REG_1,n); \
   inRegTOS = true; \
}
#endif // !emit_getSP

#ifndef emit_storeTOS_in_JitGenerated_local
#define emit_storeTOS_in_JitGenerated_local(nestingLevel,isFilter)                            \
    save_return_address();                                                                    \
    mov_register(ARG_1,FP);                                                                   \
    add_constant(ARG_1,((int)prolog_bias+(int)sizeof(void*)*(JIT_GENERATED_LOCAL_NESTING_COUNTER-1))) ; \
    mov_constant(ARG_2, nestingLevel);                                                        \
    mov_register_indirect_from(ARG_2,ARG_1);                                                  \
    mult_power_of_2(ARG_2, emit_WIN32(2) emit_WIN64(3) );                                     \
    sub_register(ARG_1,ARG_2);                                                                \
    if (isFilter) {                                                                           \
        mov_register(ARG_2,SP);                                                               \
        add_constant(ARG_2, ((int)STACK_BUFFER +1));                                          \
        mov_register_indirect_from(ARG_2,ARG_1);                                              \
    }                                                                                         \
    else {                                                                                    \
        if (STACK_BUFFER)                                                                     \
        {                                                                                     \
           mov_register(ARG_2,SP);                                                            \
           add_constant(ARG_2, (int)STACK_BUFFER);                                            \
           mov_register_indirect_from(ARG_2,ARG_1);                                           \
        } else {                                                                              \
           mov_register_indirect_from(SP,ARG_1);                                              \
        }                                                                                     \
    }
#endif

#ifndef emit_reset_storedTOS_in_JitGenerated_local
#define emit_reset_storedTOS_in_JitGenerated_local(ret_val)                                   \
    if (ret_val && ARG_1 == RESULT_1_OUT)                                                     \
      push_register(ARG_1, true);                                                             \
    mov_register(ARG_1,FP);                                                                   \
    add_constant(ARG_1,((int)prolog_bias+(int)sizeof(void*)*(JIT_GENERATED_LOCAL_NESTING_COUNTER-1))) ; \
    mov_register_indirect_to(ARG_2,ARG_1);                                                    \
    add_constant(ARG_2,-1);                                                                   \
    mov_register_indirect_from(ARG_2,ARG_1);                                                  \
    mult_power_of_2(ARG_2, emit_WIN32(2) emit_WIN64(3) );                                     \
    sub_register(ARG_1,ARG_2);                                                                \
    mov_constant(ARG_2,0) ;                                                                   \
    add_constant(ARG_1,-(int)sizeof(void*));                                                  \
    mov_register_indirect_from(ARG_2,ARG_1);                                                  \
    if (ret_val && ARG_1 == RESULT_1_OUT)                                                     \
      pop_register(ARG_1, true);                                                              \
    restore_return_address();
#endif

#ifndef emit_LOCALLOC
#define emit_LOCALLOC(initialized,EHcount)                                                          \
{                                                                                                   \
    /* Get the size of the localloc into TOS_REG_1 */                                               \
    enregisterTOS;                                                                                  \
    and_register(TOS_REG_1,TOS_REG_1, 1);                                                           \
    /* If the size is zero jump over the following loop */                                          \
    BYTE* label_1 = outPtr;                                                                         \
    jmp_abs_address(CondZero, 0, false);                                                            \
    /* Calculate the size in stack slots */                                                         \
    add_constant(TOS_REG_1, (SIZE_STACK_SLOT - 1) );                                                \
    mov_register(ARG_1,TOS_REG_1);                                                                  \
    div_power_of_2(ARG_1, ( SIZE_STACK_SLOT == 4 ? 2 : 3 ) );                                       \
    /* Loop for sizeInStackSlots pushing '0' */                                                     \
    mov_constant(TOS_REG_1,0);                                                                      \
    unsigned char* label = outPtr;                                                                  \
    push_register(TOS_REG_1, false);                                                                \
    add_constant(ARG_1,-1);                                                                         \
    and_register(ARG_1, ARG_1, 1);                                                                  \
    jmp_abs_address(CondNonZero,label, true);                                                       \
    /* Patch the jump address, since the loop size is now known */                                  \
    FJitResult PatchResult = FJIT_OK;                                                               \
    patch_address( label_1, outPtr, storedStartIP, PatchResult );                                            \
    _ASSERTE( PatchResult == FJIT_OK );                                                             \
    /* also store the esp in the appropriate JitGenerated local slot, to support GC reporting */    \
    mov_register(TOS_REG_1,FP);                                                                     \
    add_constant(TOS_REG_1,((int)prolog_bias+(int)sizeof(void*)*(JIT_GENERATED_LOCAL_LOCALLOC_OFFSET-1))) ;   \
    if (STACK_BUFFER)                                                                               \
    {                                                                                               \
      mov_register(ARG_2,SP);                                                                       \
      add_constant(ARG_2, (int)STACK_BUFFER);                                                       \
      mov_register_indirect_from(ARG_2,TOS_REG_1);                                                  \
      mov_register(TOS_REG_1, ARG_2);                                                               \
    } else {                                                                                        \
      mov_register_indirect_from(SP,TOS_REG_1);                                                     \
      /* Put the pointer to the allocated register into TOS */                                      \
      mov_register(TOS_REG_1,SP) ;                                                                  \
    }                                                                                               \
    _ASSERTE( inRegTOS );                                                                           \
}
#endif


#ifndef emit_init_bytes
#define emit_init_bytes(num_of_bytes)                 \
{                                                     \
    callInfo.reset();                                 \
    emit_arg(num_of_bytes, 1, INTERNAL_CALL);         \
    emit_tos_arg(2, INTERNAL_CALL);                   \
    emit_callhelper_I4I4(InitBytes_helper);           \
}

#ifdef DECLARE_HELPERS
void HELPER_CALL InitBytes_helper(const unsigned __int32 size, __int8 * dest)
{
    if(dest == NULL) {
        THROW_FROM_HELPER(CORINFO_NullReferenceException);
    }
    memset(dest,0,size);
}
#endif // DECLARE_HELPERS
#endif // !emit_init_bytes

#ifndef emit_copy_bytes
#define emit_copy_bytes(num_of_bytes,gcLayoutSize,gcLayout,unaligned) \
{                                                                     \
    callInfo.reset();                                                 \
    emit_tos_arg(4, INTERNAL_CALL);                                   \
    emit_tos_arg(5, INTERNAL_CALL);                                   \
    emit_arg(((int)unaligned), 3, INTERNAL_CALL);                     \
    emit_arg(num_of_bytes, 2, INTERNAL_CALL);                         \
    deregisterTOS;                                                    \
    emit_istream_arg(gcLayoutSize, gcLayout, ARG_1);                  \
    emit_reg_to_arg(1, ARG_1, INTERNAL_CALL);                         \
    emit_callhelper_I4I4I4I4I4(CopyBytes_helper);                     \
}
#endif // !emit_copy_bytes

// Helper macro to allow the interpter to unwind the stack
#ifndef INTEPRETER_ESCAPE
#define INTEPRETER_ESCAPE()                       \
                              volatile int x = 0; \
                              if(x != 0) return;
#endif


#ifdef DECLARE_HELPERS
void HELPER_CALL CopyBytes_helper(const unsigned char* gcLayout, unsigned __int32 size, __int32 unaligned, __int32 * src, __int32 * dest)
{
    unsigned char mask = 1;
    if((unsigned) dest <= sizeof(void*)) {
        THROW_FROM_HELPER(CORINFO_NullReferenceException);
    }
    unsigned i;
    for (i = 0; i < size/sizeof(void*); i++) {
        if (gcLayout[i/8] & mask) {
            __int32 val = unaligned == 0 ? *src : GET_UNALIGNED_32(src);
            HCALL2(void, FJit_pHlpAssign_Ref, CORINFO_Object**, (CORINFO_Object**)dest, CORINFO_Object*, (CORINFO_Object*)val);
            INTEPRETER_ESCAPE();
            src++;
            dest++;
        }
        else {
        if ( unaligned == 0 )
              *dest++ = *src++;
            else
        {
          SET_UNALIGNED_32(dest, GET_UNALIGNED_32(src));
              src++;
              dest++;
        }
        }
        if (mask == 0x80)
            mask = 1;
        else
            mask <<= 1;
    }
        // all of the bits left in the byte should be zero (this insures we aren't pointing at trash).
    _ASSERTE(mask == 1 || ((-mask) & gcLayout[i/8]) == 0);

    // now copy any leftover bytes
    {
        unsigned char* _dest = (unsigned char*)dest;
        unsigned char* _src = (unsigned char*)src;
        unsigned int numBytes = size & 3;
        for (i=0; i<numBytes;i++)
            *_dest++ = *_src++;
    }
}
#endif // DECLARE_HELPERS

#ifndef emit_DUP_PTR
#define emit_DUP_PTR()\
{ \
   emit_WIN32(emit_DUP_I4()); \
   emit_WIN64(emit_DUP_I8()); \
}
#endif // !emit_DUP_PTR


#ifndef emit_CPBLK
#define emit_CPBLK()                       \
{                                          \
    callInfo.reset();                      \
    emit_tos_arg( 1, INTERNAL_CALL );      \
    emit_tos_arg( 2, INTERNAL_CALL );      \
    emit_tos_arg( 3, INTERNAL_CALL );      \
    emit_callhelper_I4I4I4(CPBLK_helper);  \
}
#ifdef DECLARE_HELPERS
void HELPER_CALL CPBLK_helper(int size, __in_ecount(size) char* pSrc, __in_ecount(size) char* pDst) {
     memcpy(pDst, pSrc, size);
}
#endif
#endif

#ifndef emit_INITBLK
#define emit_INITBLK()                     \
{                                          \
    callInfo.reset();                      \
    emit_tos_arg( 1, INTERNAL_CALL );      \
    emit_tos_arg( 2, INTERNAL_CALL );      \
    emit_tos_arg( 3, INTERNAL_CALL );      \
    emit_callhelper_I4I4I4(INITBLK_helper);\
}
#ifdef DECLARE_HELPERS
void HELPER_CALL INITBLK_helper(int size, char val, __in_ecount(size) char* pDst) {
    memset(pDst, val, size);
}
#endif
#endif

/*************************************************************************************
       Macros for passing arguments to helpers
*************************************************************************************/

/* emit_arg(val, argNumber, useReg) - emit code to place value into the register if
   the current calling convention has enough registers. If successful increment
   callInfo.enregSize by sizeof(void *), if there are not enough registers push 'val'
   onto the stack. (!useReg - temporarily makes registers unavailable) */

#ifndef emit_arg
#define emit_arg(value, argNum, useReg )            \
{                                                    \
   /* First check if 'argNum' can be enregistered */ \
   if (argNum <= NUMBER_ARGUMENT_REGISTERS && useReg) \
   {                                                 \
      callInfo.EnregSize+=SIZE_ARGUMENT_REGISTER;    \
      mov_constant(ARG_##argNum, value );            \
   }                                                 \
   else /* This argument is passed on the stack */   \
   {                                                 \
      _ASSERTE(SIZE_STACK_SLOT == sizeof(void *) );  \
      deregisterTOS;                                 \
      emit_LDC_I( value );                           \
   }                                                 \
}
#endif // !emit_arg

/* emit_reg_arg(val, argNumber, useReg) - emit code to move the reg into the register if
   the current calling convention has enough registers (!useReg - temporarily makes registers unavailable).
   If successful increment callInfo.enregSize by sizeof(void *), if there are not enough registers push 'val'
   onto the stack */

#ifndef emit_reg_arg
#define emit_reg_arg(reg, argNum, useReg )           \
{                                                    \
   /* First check if 'argNum' can be enregistered */ \
   if (argNum <= NUMBER_ARGUMENT_REGISTERS && useReg) \
   {                                                 \
      callInfo.EnregSize+=SIZE_ARGUMENT_REGISTER;    \
      mov_register(ARG_##argNum, reg );              \
   }                                                 \
   else /* This argument is passed on the stack */   \
   {                                                 \
      _ASSERTE(SIZE_STACK_SLOT == sizeof(void *) );  \
      deregisterTOS;                                 \
      push_register( reg, true );                    \
   }                                                 \
}
#endif // !emit_reg_arg

/* emit_tos_arg(argNumber, useReg) - emit code to move the TOS into a register if
   the current calling convention has enough registers. If successful increment
   enregSize by sizeof(void *), if there are not enough registers do nothing.
   (!useReg - temporarily makes registers unavailable) */

#ifndef emit_tos_arg
#define emit_tos_arg(argNum, useReg)                 \
{                                                    \
  if (useReg)                                        \
  {                                                  \
   /* First check if 'argNum' can be enregistered */ \
   if (argNum <= NUMBER_ARGUMENT_REGISTERS)           \
   {                                                 \
      callInfo.EnregSize+=SIZE_ARGUMENT_REGISTER;    \
      if (inRegTOS)                                  \
      {                                              \
         mov_register(ARG_##argNum, TOS_REG_1);      \
         inRegTOS = false;                           \
      }                                              \
      else                                           \
      {                                              \
         pop_register(ARG_##argNum, true);           \
      }                                              \
   }                                                 \
   else /* This argument is passed on the stack */   \
   {                                                 \
      _ASSERTE(SIZE_STACK_SLOT == sizeof(void *) );  \
      deregisterTOS;                                 \
   }                                                 \
  }                                                  \
}
#endif // !emit_tos__arg

/*  This routine assumes that the argument is padded to next DWORD at the end */
#ifndef emit_tos__fixedsize_arg
#define emit_tos_fixedsize_arg(argNum, sizeInBytes, numRegsUsed, useReg)                \
{                                                                                       \
  if (useReg)                                                                           \
  {                                                                                     \
   if ( SIZE_STACK_SLOT == 8 ) { deregisterTOS;   }                                     \
   int copySize = (int)sizeInBytes; int usedRegs = 0;                                   \
   _ASSERTE( (copySize % SIZE_STACK_SLOT) == 0 || copySize < SIZE_STACK_SLOT);          \
   _ASSERTE( (NUMBER_ARGUMENT_REGISTERS - argNum)*SIZE_ARGUMENT_REGISTER >= copySize ); \
   while ( argNum + usedRegs <= NUMBER_ARGUMENT_REGISTERS && copySize > 0 )             \
   {                                                                                    \
     if ( SIZE_STACK_SLOT == SIZE_ARGUMENT_REGISTER || sizeInBytes == SIZE_ARGUMENT_REGISTER) \
     {                                                                                  \
        emit_tos_arg( argNum + usedRegs, true )                                         \
        usedRegs++;                                                                     \
        copySize -= SIZE_ARGUMENT_REGISTER;                                             \
     }                                                                                  \
     else                                                                               \
     {                                                                                  \
        _ASSERTE( SIZE_STACK_SLOT == 8 && inRegTOS == false );                          \
        mov_register(TOS_REG_1, SP);                                                    \
        if (STACK_BUFFER)                                                               \
           add_constant(TOS_REG_1, STACK_BUFFER);                                       \
        emit_tos_indirect_to_arg( argNum + usedRegs);                                   \
        add_constant(TOS_REG_1, (sizeof(void *)));                                      \
        emit_tos_indirect_to_arg( argNum + usedRegs+1);                                 \
        emit_drop(SIZE_STACK_SLOT);                                                     \
        usedRegs += 2;                                                                  \
        copySize -= SIZE_STACK_SLOT;                                                    \
     }                                                                                  \
   }                                                                                    \
   numRegsUsed += usedRegs;                                                             \
  }                                                                                     \
}
#endif // !emit_tos__arg


#ifndef emit_tos_indirect_to_arg
#define emit_tos_indirect_to_arg(argNum)              \
{                                                     \
  if (argNum <= NUMBER_ARGUMENT_REGISTERS)            \
  {                                                   \
    callInfo.EnregSize += SIZE_ARGUMENT_REGISTER;     \
    mov_register_indirect_to(ARG_##argNum, TOS_REG_1);\
  }                                                   \
}
#endif // !emit_tos_indirect_to_arg

/* emit_reg_to_arg(argNumber, reg, useReg) - emits code to move a value from 'reg' into an
   argument register if the current calling convention has enough registers. If successful increment
   enregSize by sizeof(void *), if there are not enough registers push the value from 'reg' onto
   the stack. (!useReg - temporarily makes registers unavailable) */
#ifndef emit_reg_to_arg
#define emit_reg_to_arg(argNum, reg, useReg)          \
{                                                     \
     if (useReg && argNum <= NUMBER_ARGUMENT_REGISTERS)\
     {                                                \
       callInfo.EnregSize += SIZE_ARGUMENT_REGISTER;  \
       if ( reg != ARG_##argNum )                     \
        { mov_register(ARG_##argNum, reg);}           \
     }                                                \
     else                                             \
     {                                                \
       _ASSERTE(SIZE_STACK_SLOT == sizeof(void *) );  \
       deregisterTOS;                                 \
       push_register(reg, true);                      \
     }                                                \
}
#endif // !emit_reg_to_arg

/*                                                                                                 */
#ifndef emit_mov_arg_reg
#define emit_mov_arg_reg(offset, reg)\
{ \
   _ASSERTE(reg < MAX_GP_ARG_REGISTER ); \
   _ASSERTE(!inRegTOS); \
   mov_register(argRegistersMap[reg],SP); \
   add_constant(argRegistersMap[reg],offset); \
   mov_register_indirect_to(argRegistersMap[reg],argRegistersMap[reg]); \
}
#endif // !emit_mov_arg_reg

#ifndef emit_mov_arg_floatreg
#define emit_mov_arg_floatreg(offset, reg, type)     \
{                                                    \
   if ( type == typeR4 )                             \
   {                                                 \
     emit_narrow_R8toR4( offset, offset )            \
     load_single( reg, offset );                     \
   }                                                 \
   else                                              \
     load_double( reg, offset )                      \
}
#endif // !emit_mov_arg_reg

/*                                                                                                */
#ifndef emit_mov_TOS_arg
#define emit_mov_TOS_arg(reg)\
{ \
   _ASSERTE(reg < MAX_GP_ARG_REGISTER ); \
   if (inRegTOS) { \
      mov_register(argRegistersMap[reg],TOS_REG_1); \
      inRegTOS = false; \
   }\
   else { \
      pop_register(argRegistersMap[reg], true); \
  }\
}
#endif // !emit_mov_TOS_arg

#ifndef emit_mov_arg_stack_pointer
#define emit_mov_arg_stack_pointer( ilOffset, reg, nativeOffset, cls, isTailCall)             \
{ /* The code below only works on architectures which save CALLEE_SAVED_2 automatically, otherwise it is trashed*/ \
   _ASSERTE(isTailCall ? inRegTOS : !inRegTOS);                         \
   unsigned int EndCorr =                                               \
         cls ? bigEndianOffset( typeSizeInBytes(jitInfo, cls) ) : 0;    \
   if ( nativeOffset == 0 )                                             \
   {                                                                    \
     _ASSERTE(reg < MAX_GP_ARG_REGISTER );                              \
     if(isTailCall) {                                                   \
       /* old struct ptr in TOS */                                      \
       mov_register(argRegistersMap[reg], TOS_REG_1);                   \
     } else {                                                           \
       /* calculate src pointer */                                      \
       mov_register(argRegistersMap[reg], SP);                          \
       add_constant(argRegistersMap[reg], ilOffset + EndCorr);          \
     }                                                                  \
   }                                                                    \
   else                                                                 \
   {                                                                    \
     if(!isTailCall)                                                    \
     { /* TOS_REG_1 already has old struct ptr */                       \
       mov_register(TOS_REG_1, SP);                                     \
       add_constant(TOS_REG_1, ilOffset + EndCorr);                     \
     }                                                                  \
     mov_register(CALLEE_SAVED_2, SP);                                  \
     add_constant(CALLEE_SAVED_2, nativeOffset);                        \
     mov_register_indirect_from(TOS_REG_1,CALLEE_SAVED_2);              \
   }                                                                    \
   inRegTOS = false; /* done with TOS */                                \
}

#endif // !emit_mov_arg_stack_pointer

#ifndef emit_copybytes_nooverlap
#define emit_copybytes_nooverlap(ilOffset,sizeArg)  \
{\
    _ASSERTE(inRegTOS); /*dest pointer must be in the TOS */     \
    _ASSERTE(sizeArg >= 4);                                      \
    _ASSERTE(sizeArg % sizeof(void*) == 0);                      \
    push_register(TOS_REG_1, true);                              \
    push_register(CALLEE_SAVED_1,true);                          \
    push_register(CALLEE_SAVED_2,true);                          \
    mov_register(CALLEE_SAVED_1, SP);                            \
    add_constant(CALLEE_SAVED_1, ilOffset + (3*SIZE_STACK_SLOT));\
    int emcpno_size = sizeArg;                                   \
    while(true)                                                  \
    {                                                            \
        mov_register_indirect_to(CALLEE_SAVED_2,CALLEE_SAVED_1); \
        mov_register_indirect_from(CALLEE_SAVED_2,TOS_REG_1);    \
        if(emcpno_size == sizeof(void*)) break;                  \
        add_constant(CALLEE_SAVED_1, (int) sizeof(void*));       \
        add_constant(TOS_REG_1, (int) sizeof(void*));            \
        emcpno_size -= sizeof(void*);                            \
    }                                                            \
    pop_register(CALLEE_SAVED_2,true);                           \
    pop_register(CALLEE_SAVED_1,true);                           \
    pop_register(TOS_REG_1,true);                                \
}
#endif

#ifndef emit_mov_arg_stack
#define emit_mov_arg_stack(dest,src, size)\
{  ON_SPARC_ONLY(_ASSERTE(false);)\
   _ASSERTE(!inRegTOS); \
   _ASSERTE(size >= 4 );\
   if (dest > src) \
   { \
       push_register(CALLEE_SAVED_1,true); \
       int emitter_scratch_i4 = size; \
       mov_register(CALLEE_SAVED_1,SP); \
       push_register(CALLEE_SAVED_2, true); \
       mov_register(CALLEE_SAVED_2,CALLEE_SAVED_1); \
       add_constant(CALLEE_SAVED_1,src+emitter_scratch_i4); \
       add_constant(CALLEE_SAVED_2,dest+emitter_scratch_i4); \
       _ASSERTE(emitter_scratch_i4 > 0); \
       while (true) \
       { \
          mov_register_indirect_to(TOS_REG_1,CALLEE_SAVED_1); \
          mov_register_indirect_from(TOS_REG_1,CALLEE_SAVED_2); \
          if (emitter_scratch_i4 == sizeof(void*)) \
            break;\
          add_constant(CALLEE_SAVED_1,-(int) sizeof(void*)); \
          add_constant(CALLEE_SAVED_2,-(int) sizeof(void*)); \
          emitter_scratch_i4 -= sizeof(void*); \
       }\
       pop_register(CALLEE_SAVED_2, true); \
       pop_register(CALLEE_SAVED_1, true); \
   } \
   else \
   { \
       /*_ASSERTE(!""); */\
       push_register(CALLEE_SAVED_1, true); \
       unsigned int emitter_scratch_i4 = sizeof(void*); \
       mov_register(CALLEE_SAVED_1,SP); \
       push_register(CALLEE_SAVED_2, true); \
       mov_register(CALLEE_SAVED_2,CALLEE_SAVED_1); \
       add_constant(CALLEE_SAVED_1,src+emitter_scratch_i4); \
       add_constant(CALLEE_SAVED_2,dest+emitter_scratch_i4); \
       _ASSERTE(emitter_scratch_i4 <= size); \
       while (true) \
       { \
          mov_register_indirect_to(TOS_REG_1,CALLEE_SAVED_1); \
          mov_register_indirect_from(TOS_REG_1,CALLEE_SAVED_2); \
          if (emitter_scratch_i4 == size) \
            break;\
          add_constant(CALLEE_SAVED_1,(int) sizeof(void*)); \
          add_constant(CALLEE_SAVED_2,(int) sizeof(void*)); \
          emitter_scratch_i4 += sizeof(void*); \
       }\
       pop_register(CALLEE_SAVED_2, true); \
       pop_register(CALLEE_SAVED_1, true); \
   } \
}
#endif // !emit_mov_arg_stack

#ifndef emit_set_return_buffer

#define emit_set_return_buffer(retBufReg, retValBase, isTailCall )         \
{                                                              \
  _ASSERTE(!inRegTOS);                                         \
  if(isTailCall)                                               \
  {                                                            \
    emit_loadretbuf(); /* use the old return buffer */         \
  } else {                                                     \
    emit_getSP(retValBase);  /* get pointer to return buffer*/ \
  }                                                            \
  if (EnregReturnBuffer)                                       \
  {                                                            \
     emit_mov_TOS_arg((ReturnBufferFirst ? 0 : retBufReg));    \
  }                                                            \
  else                                                         \
  {  /* The code below only works on architectures which save CALLEE_SAVED_2 automatically, otherwise it is trashed*/ \
     mov_register(CALLEE_SAVED_2,SP);                          \
     add_constant(CALLEE_SAVED_2, RETURN_BUFF_OFFSET );        \
     mov_register_indirect_from(TOS_REG_1,CALLEE_SAVED_2);     \
     inRegTOS = false;                                         \
  }                                                            \
}
#endif //!emit_set_return_buffer

// This function is used for initializing the vararg cookie. There should be no enregistered
// arguments at the time of the call on non-x86 platforms
#ifndef emit_set_vararg_cookie
#define emit_set_vararg_cookie(cookie, varArgCookieOffset, enreg )    \
{                                                                     \
   _ASSERTE(!inRegTOS);                                               \
  if (!PARAMETER_SPACE)                                               \
  {                                                                   \
     emit_LDC_I(cookie);                                              \
     deregisterTOS;                                                   \
  }                                                                   \
  else                                                                \
  {                                                                   \
     if (!(enreg))                                                    \
     {                                                                \
         emit_getSP(varArgCookieOffset); /*get pointer to stack slot*/\
         mov_constant(ARG_1, cookie ); /* load the cookie */          \
         mov_register_indirect_from(ARG_1, TOS_REG_1);                \
         inRegTOS = false;                                            \
     } else {                                                         \
         mov_constant( ARG_1 + (varArgCookieOffset - sizeof(prolog_frame))/sizeof(void *), cookie ); \
     }                                                                \
  }                                                                   \
}
#endif //!emit_set_vararg_cookie

// This function is used to store enregistered this pointer or return buff back in its home location. It is only used on
// non-x86 platforms for calls to functions with variable number of parameters
#ifndef emit_set_arg_pointer
#define emit_set_arg_pointer(secondArg, offset, toStack)              \
{                                                                     \
  _ASSERTE(!inRegTOS);                                                \
  emit_getSP(offset); /*get pointer to stack slot*/                   \
  if (toStack)                                                        \
    {mov_register_indirect_from((secondArg ? ARG_2 : ARG_1), TOS_REG_1);} \
  else                                                                \
    {mov_register_indirect_to((secondArg ? ARG_2 : ARG_1), TOS_REG_1);} \
  inRegTOS = false;                                                   \
}
#endif //!emit_set_arg_pointer

/*************************************************************************************
Macros for making calls to helpers. The macros attempt to abstract the register
size from the rest of the system.
*************************************************************************************/

#define emit_callhelper_il(helper)  \
    deregisterTOS;                  \
    emit_store_toc_reg(helper);     \
    mov_constant(CALLREG, (HASTOCREG ? (size_t)*(void**)(helper) : (size_t)(void*)(helper))); \
    call_register(CALLREG);         \
    emit_restore_toc_reg(helper);   \

/* emit_callhelper( helper, argSize ) - at the time when this macro is called
   it is assumed that there is (argSize - CallInfo.enregSize) bytes of arguments on the top of the stack and
   CallInfo.enregSize bytes in the appropriate registers. This macro will construct an appropriate
   stack structure for the call, call 'helper' and it will remove the arguments from the stack
   if necessary after the call. It will not (and can't) trash any argument registers prior to the call. */
#define emit_callhelper(helper,argsize)                                                           \
{                                                                                                 \
    deregisterTOS;                                                                                \
    if ( PARAMETER_SPACE && call_frame_size(argsize, callInfo.EnregSize) )                        \
       { emit_call_frame(call_frame_size(argsize, callInfo.EnregSize)); }                         \
    emit_callhelper_il(helper);                                                                   \
    if ( CALLER_CLEANS_STACK && call_frame_size(argsize, callInfo.EnregSize))                     \
       { add_constant(SP, (call_frame_size(argsize, callInfo.EnregSize)) ); }                     \
}


// Define helpers that operate on abstract types D (dword) and Q (qword)
// in terms of the above helpers

#define emit_callhelper_Q(helper)     emit_callhelper(helper,2*sizeof(void*))
#define emit_callhelper_DQ(helper)    emit_callhelper(helper,3*sizeof(void*))
#define emit_callhelper_QD(helper)    emit_callhelper(helper,3*sizeof(void*))
#define emit_callhelper_D_Q(helper)   emit_callhelper(helper,sizeof(void*))
#define emit_callhelper_Q_D(helper)   emit_callhelper(helper,2*sizeof(void*))
#define emit_callhelper_Q_Q(helper)   emit_callhelper(helper,2*sizeof(void*))
#define emit_callhelper_DD_Q(helper)  emit_callhelper(helper,2*sizeof(void*))
#define emit_callhelper_DQ_D(helper)  emit_callhelper(helper,3*sizeof(void*))
#define emit_callhelper_DQ_Q(helper)  emit_callhelper(helper,3*sizeof(void*))
#define emit_callhelper_QQ_D(helper)  emit_callhelper(helper,4*sizeof(void*))
#define emit_callhelper_QQ_Q(helper)  emit_callhelper(helper,4*sizeof(void*))
#define emit_callhelper_DQD(helper)   emit_callhelper(helper,4*sizeof(void*))
#define emit_callhelper_QDD(helper)   emit_callhelper(helper,4*sizeof(void*))
#define emit_callhelper_DDQ_D(helper) emit_callhelper(helper,4*sizeof(void*))
#define emit_callhelper_DDD_Q(helper) emit_callhelper(helper,3*sizeof(void*))
#define emit_callhelper_DDQD(helper)  emit_callhelper(helper,5*sizeof(void*))


// Define helpers that are the same in Win32 and Win64
#define emit_callhelper_(helper)      emit_callhelper(helper,0)
#define emit_callhelper_D(helper)     emit_callhelper(helper,sizeof(void*))
#define emit_callhelper_DD(helper)    emit_callhelper(helper,2*sizeof(void*))
#define emit_callhelper_DDD(helper)   emit_callhelper(helper,3*sizeof(void*))
#define emit_callhelper_DDDD(helper)  emit_callhelper(helper,4*sizeof(void*))
#define emit_callhelper_DDDDD(helper) emit_callhelper(helper,5*sizeof(void*))

#define emit_callhelper_D_D(helper)    emit_callhelper(helper,sizeof(void*))
#define emit_callhelper_DD_D(helper)   emit_callhelper(helper,2*sizeof(void*))
#define emit_callhelper_DDD_D(helper)  emit_callhelper(helper,3*sizeof(void*))
#define emit_callhelper_DDDD_D(helper) emit_callhelper(helper,4*sizeof(void*))

// Define type specific helpers in terms of D (dword) and Q (qword)
#define emit_callhelper_I4 emit_callhelper_D
#define emit_callhelper_I8 emit_callhelper_Q
#define emit_callhelper_R4 emit_callhelper_D
#define emit_callhelper_R8 emit_callhelper_Q
#define emit_callhelper_I4I4 emit_callhelper_DD
#define emit_callhelper_I4I8 emit_callhelper_DQ
#define emit_callhelper_I4R4 emit_callhelper_DD
#define emit_callhelper_I4R8 emit_callhelper_DQ
#define emit_callhelper_I8I4 emit_callhelper_QD
#define emit_callhelper_R4I4 emit_callhelper_DD
#define emit_callhelper_R8I4 emit_callhelper_QD
#define emit_callhelper_I4I4I4 emit_callhelper_DDD
#define emit_callhelper_I4I8I4 emit_callhelper_DQD
#define emit_callhelper_I4R4I4 emit_callhelper_DDD
#define emit_callhelper_I4R8I4 emit_callhelper_DQD
#define emit_callhelper_I8I4I4 emit_callhelper_QDD
#define emit_callhelper_R4I4I4 emit_callhelper_DDD
#define emit_callhelper_R8I4I4 emit_callhelper_QDD
#define emit_callhelper_I4I4I4I4 emit_callhelper_DDDD
#define emit_callhelper_I4I4I8I4  emit_callhelper_DDQD
#define emit_callhelper_I4I4I4I4I4 emit_callhelper_DDDDD


#define emit_callhelper_I4_I4 emit_callhelper_D_D
#define emit_callhelper_I4_I8 emit_callhelper_D_Q
#define emit_callhelper_I8_I4 emit_callhelper_Q_D
#define emit_callhelper_I8_I8 emit_callhelper_Q_Q
#define emit_callhelper_R4_I4 emit_callhelper_D_D
#define emit_callhelper_R8_I4 emit_callhelper_Q_D
#define emit_callhelper_R4_I8 emit_callhelper_D_Q
#define emit_callhelper_R8_I8 emit_callhelper_Q_Q
#define emit_callhelper_I4I4_I4 emit_callhelper_DD_D
#define emit_callhelper_I4I4_I8 emit_callhelper_DD_Q
#define emit_callhelper_I4I8_I4 emit_callhelper_DQ_D
#define emit_callhelper_I4I8_I8 emit_callhelper_DQ_Q
#define emit_callhelper_I8I8_I4 emit_callhelper_QQ_D
#define emit_callhelper_I8I8_I8 emit_callhelper_QQ_Q
#define emit_callhelper_R4R4_I4 emit_callhelper_DD_D
#define emit_callhelper_R8I8_I8 emit_callhelper_QQ_Q
#define emit_callhelper_R8R8_I4 emit_callhelper_QQ_D
#define emit_callhelper_R8R8_I8 emit_callhelper_QQ_Q
#define emit_callhelper_R8R8_R8 emit_callhelper_QQ_Q
#define emit_callhelper_I4I4I4_I4 emit_callhelper_DDD_D
#define emit_callhelper_I4I4I4_I8 emit_callhelper_DDD_Q
#define emit_callhelper_I4I4I8_I4 emit_callhelper_DDQ_D
#define emit_callhelper_I4I4I4I4_I4 emit_callhelper_DDDD_D

/*************************************************************************************
Macros for moving the value between between the platfom defined result registers
and the top of stack (TOS). Used for RET and to access the return value of a function
inside the caller.
*************************************************************************************/

#ifndef emit_loadresult_U1
#define emit_loadresult_U1()                    \
{                                               \
  callInfo.reset();                             \
  emit_tos_arg(1, INTERNAL_CALL);               \
  emit_callhelper_I4_I4(load_result_U1_helper); \
  if ( RESULT_1_OUT != RESULT_1 )               \
     {mov_register(RESULT_1_OUT,RESULT_1);}     \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL load_result_U1_helper(int x) {return (unsigned int)((unsigned char) x);}
#endif
#endif // !emit_loadresult_U1

#ifndef emit_loadresult_I1
#define emit_loadresult_I1()                    \
{                                               \
  callInfo.reset();                             \
  emit_tos_arg(1, INTERNAL_CALL);               \
  emit_callhelper_I4_I4(load_result_I1_helper); \
  if ( RESULT_1_OUT != RESULT_1 )               \
     {mov_register(RESULT_1_OUT,RESULT_1);}     \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL load_result_I1_helper(int x) {return (int) ((signed char) x);}
#endif
#endif // !emit_loadresult_I1

#ifndef emit_loadresult_U2
#define emit_loadresult_U2()                    \
{                                               \
  callInfo.reset();                             \
  emit_tos_arg(1, INTERNAL_CALL);               \
  emit_callhelper_I4_I4(load_result_U2_helper); \
  if ( RESULT_1_OUT != RESULT_1 )               \
     {mov_register(RESULT_1_OUT,RESULT_1);}     \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL load_result_U2_helper(int x) {return (unsigned int) ((unsigned short) x);}
#endif
#endif // !emit_loadresult_U2

#ifndef emit_loadresult_I2
#define emit_loadresult_I2()                    \
{                                               \
  callInfo.reset();                             \
  emit_tos_arg(1, INTERNAL_CALL);               \
  emit_callhelper_I4_I4(load_result_I2_helper); \
  if ( RESULT_1_OUT != RESULT_1 )               \
     {mov_register(RESULT_1_OUT,RESULT_1);}     \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL load_result_I2_helper(int x) {return (int) ((short) x);}
#endif
#endif  // !emit_loadresult_I2

#ifndef emit_loadresult_I4
#define emit_loadresult_I4()                     \
{                                                \
   if ( RESULT_1_OUT == TOS_REG_1 )              \
        {enregisterTOS;}                         \
   else                                          \
   {                                             \
     if (inRegTOS)                               \
        {mov_register(RESULT_1_OUT,TOS_REG_1);}  \
     else                                        \
        pop_register(RESULT_1_OUT, true);        \
   }                                             \
   inRegTOS = false;                             \
}
#endif // !emit_loadresult_I4

#ifndef emit_loadresult_I8
#define emit_loadresult_I8()                              \
{                                                         \
   if ( SIZE_STACK_SLOT < 8 )                             \
   {                                                      \
       emit_loadresult_I4();                              \
       pop_register(RESULT_2_OUT, true);                  \
       inRegTOS = false;                                  \
   }                                                      \
   else                                                   \
   {                                                      \
       _ASSERTE(!inRegTOS);                               \
       mov_register(TOS_REG_1,SP);                        \
       if (STACK_BUFFER)                                  \
           add_constant(TOS_REG_1, STACK_BUFFER);         \
       mov_register_indirect_to(RESULT_1_OUT, TOS_REG_1); \
       add_constant(TOS_REG_1, (sizeof(void *)));         \
       mov_register_indirect_to(RESULT_2_OUT, TOS_REG_1); \
       emit_drop( SIZE_STACK_SLOT );                      \
   }                                                      \
}
#endif // !emit_loadresult_I8

#define emit_loadresult_Ptr()           \
    emit_WIN32(emit_loadresult_U4())    \
    emit_WIN64(emit_loadresult_U8())

#define emit_pushresult_U1()                              \
{                                                         \
        and_constant(RESULT_1,0xff);                      \
        inRegTOS = true;                                  \
        if ( TOS_REG_1 != RESULT_1 )                      \
          { mov_register( TOS_REG_1, RESULT_1);   }       \
}

#define emit_pushresult_U2()                              \
{                                                         \
        and_constant(RESULT_1,0xffff);                    \
        inRegTOS = true;                                  \
        if ( TOS_REG_1 != RESULT_1 )                      \
          { mov_register( TOS_REG_1, RESULT_1);   }       \
}

#ifndef emit_pushresult_I1
#define emit_pushresult_I1()                           \
{                                                      \
        callInfo.reset();                              \
        if (TOS_REG_1 == RESULT_1)                     \
           inRegTOS = true;                            \
        else if ( ARG_1 == RESULT_1 )                  \
        {                                              \
           _ASSERTE( inRegTOS == false );              \
           callInfo.EnregSize+=SIZE_ARGUMENT_REGISTER; \
        }                                              \
        emit_callhelper_I4_I4(CONV_TOI4_I1_helper);    \
        emit_pushresult_I4()                           \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_TOI4_I1_helper(__int8 val) {
    return (__int32) val;
}
#endif
#endif

#ifndef emit_pushresult_I2
#define emit_pushresult_I2()                           \
{                                                      \
        callInfo.reset();                              \
        if (TOS_REG_1 == RESULT_1)                     \
           inRegTOS = true;                            \
        else if ( ARG_1 == RESULT_1 )                  \
        {                                              \
           _ASSERTE( inRegTOS == false );              \
           callInfo.EnregSize+=SIZE_ARGUMENT_REGISTER; \
        }                                              \
        emit_callhelper_I4_I4(CONV_TOI4_I2_helper);    \
        emit_pushresult_I4()                           \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_TOI4_I2_helper(__int16 val) {
    return (__int32) val;
}
#endif
#endif

#ifndef emit_pushresult_I4
#define emit_pushresult_I4()                           \
{                                                      \
        /*                                            */\
        inRegTOS = true;                               \
        if ( TOS_REG_1 != RESULT_1 )                   \
           { mov_register( TOS_REG_1, RESULT_1);  }    \
}
#endif // !emit_pushresult_I4

#ifndef emit_pushresult_I8
#define emit_pushresult_I8()                              \
{                                                         \
   if ( SIZE_STACK_SLOT < 8 )                             \
   {                                                      \
        push_register(RESULT_2, true);                    \
        emit_pushresult_I4();                             \
   }                                                      \
   else                                                   \
   {                                                      \
       _ASSERTE(!inRegTOS);                               \
       grow( SIZE_STACK_SLOT, false );                    \
       mov_register(TOS_REG_1, SP );                      \
       if (STACK_BUFFER)                                  \
           add_constant(TOS_REG_1, STACK_BUFFER);         \
       mov_register_indirect_from(RESULT_1, TOS_REG_1);   \
       add_constant(TOS_REG_1, (sizeof(void *)));         \
       mov_register_indirect_from(RESULT_2, TOS_REG_1);   \
   }                                                      \
}
#endif // !emit_pushresult_I8

#define emit_pushresult_Ptr()           \
    emit_WIN32(emit_pushresult_U4())    \
    emit_WIN64(emit_pushresult_U8())

/***************************************************************************************
 * High level macros for IL opcodes loading/storing local variables and arguments:
 *   LDLOC.*, STLOC.*, STLOC.*, STARG.*, LDARGA.*, LDLOCA.*
 *
 * For functions with variable number of arguments the following two macros are used to
 * load/store arguments:
 *    emit_VARARG_LDARGA(offset, offsetVarArgToken);
 *    emit_STIND_REV_*
 * Typically need to be redefined for each platform in order to obtain acceptable
 * performance
 ****************************************************************************************/

#ifndef emit_LDVARA
#define emit_LDVARA(offset, size)                                 \
{                                                                 \
   deregisterTOS;                                                 \
   mov_register(TOS_REG_1,FP);                                    \
   add_constant(TOS_REG_1,(int)(offset + bigEndianOffset(size))); \
   inRegTOS = true;                                               \
}
#endif // !emit_LDVARA

#define emit_LDNULL()          \
    emit_WIN32(emit_LDC_I4(0)) \
    emit_WIN64(emit_LDC_I8(0))

#ifndef emit_VARARG_LDARGA
#define emit_VARARG_LDARGA(offset, offset_vararg_token) {                  \
    emit_LDVARA(offset_vararg_token, sizeof(void *)); /* get the vararg handle */ \
    emit_LDC_I4(offset);                                                   \
    emit_LDC_I4(offset_vararg_token);                                      \
    emit_callhelper_I4I4I4(VARARG_LDARGA_helper);                          \
    emit_WIN32(emit_pushresult_I4())                                       \
    emit_WIN64(emit_pushresult_I8());                                      \
    }
#ifdef DECLARE_HELPERS
void* HELPER_CALL VARARG_LDARGA_helper(int tokenOffset, int argOffset, CORINFO_VarArgInfo** varArgHandle) {

    CORINFO_VarArgInfo* argInfo = *varArgHandle;
    char* argPtr = (char*) varArgHandle;

    argPtr += argInfo->argBytes+argOffset;

    return(argPtr);
}
#endif // DECLARE_HELPERS
#endif // !emit_VARARG_LDARGA

#ifndef emit_LDVAR_U1
#define emit_LDVAR_U1(offset)                           \
    emit_LDVARA( offset, 1 );                           \
    load_indirect_byte_zeroextend(TOS_REG_1,TOS_REG_1); \
    _ASSERTE(inRegTOS);
#endif // !emit_LDVAR_U1

#ifndef emit_LDVAR_U2
#define emit_LDVAR_U2(offset)                                 \
    emit_LDVARA(offset, 2);                                   \
    load_indirect_word_zeroextend(TOS_REG_1,TOS_REG_1, true); \
    _ASSERTE(inRegTOS);
#endif // !emit_LDVAR_U2

#ifndef emit_LDVAR_I1
#define emit_LDVAR_I1(offset)                           \
    emit_LDVARA( offset, 1 );                           \
    load_indirect_byte_signextend(TOS_REG_1,TOS_REG_1); \
    _ASSERTE(inRegTOS);
#endif // !emit_LDVAR_I1

#ifndef emit_LDVAR_I2
#define emit_LDVAR_I2(offset)                                 \
    emit_LDVARA( offset, 2 );                                 \
    load_indirect_word_signextend(TOS_REG_1,TOS_REG_1, true); \
    _ASSERTE(inRegTOS);
#endif // !emit_LDVAR_I2

#ifndef emit_LDVAR_I4
#define emit_LDVAR_I4(offset)                      \
{                                                  \
    emit_LDVARA(offset, 4);                        \
    mov_register_indirect_to(TOS_REG_1,TOS_REG_1); \
    inRegTOS = true;                               \
}
#endif // !emit_LDVAR_I4

#ifndef emit_LDVAR_I8
#define emit_LDVAR_I8(offset)                      \
    emit_LDVARA(offset, 8);                        \
    emit_LDIND_I8(false);
#endif // !emit_LDVAR_I8

#ifndef emit_LDVAR_R4
#define emit_LDVAR_R4(offset)                      \
    emit_LDVARA(offset, 4);                        \
    emit_LDIND_R4(false);
#endif // !emit_LDVAR_R4

#ifndef emit_LDVAR_R8
#define emit_LDVAR_R8(offset)                      \
    emit_LDVARA(offset, 8);                        \
    emit_LDIND_R8(false);
#endif // !emit_LDVAR_R8

#ifndef emit_STVAR_U1
#define emit_STVAR_U1(offset) emit_STVAR_I1(offset)
#endif

#ifndef emit_STVAR_U2
#define emit_STVAR_U2(offset) emit_STVAR_I2(offset)
#endif

#ifndef emit_STVAR_I1
#define emit_STVAR_I1(offset)                                                     \
{                                                                                 \
    emit_LDVARA(offset, 1);                                                       \
    enregisterTOS;                                                                \
    pop_register(ARG_1, true);                                                    \
    store_indirect_8bits(ARG_1,TOS_REG_1);                                        \
    inRegTOS = false;                                                             \
}
#endif // !emit_STVAR_I4

#ifndef emit_STVAR_I2
#define emit_STVAR_I2(offset)                                                     \
{                                                                                 \
    emit_LDVARA( offset, 2);                                                      \
    enregisterTOS;                                                                \
    pop_register(ARG_1, true);                                                    \
    store_indirect_16bits(ARG_1,TOS_REG_1, true);                                 \
    inRegTOS = false;                                                             \
}
#endif // !emit_STVAR_I4

#ifndef emit_STVAR_I4
#define emit_STVAR_I4(offset)                \
    emit_LDVARA(offset, 4);                  \
    enregisterTOS; \
    pop_register(ARG_1, true); \
    mov_register_indirect_from(ARG_1,TOS_REG_1); \
    inRegTOS = false
#endif // !emit_STVAR_I4

#ifndef emit_STVAR_I8
#define emit_STVAR_I8(offset)     \
    emit_LDVARA(offset, 8);       \
    emit_STIND_REV_I8(false);
#endif // !emit_STVAR_I8

#ifndef emit_STVAR_R4
#define emit_STVAR_R4(offset)     \
    emit_LDVARA(offset, 4);       \
    emit_STIND_REV_R4(false)
#endif // !emit_STVAR_R4

#ifndef emit_STVAR_R8
#define emit_STVAR_R8(offset)     \
    emit_LDVARA(offset, 8);       \
    emit_STIND_REV_R8(false)
#endif // !emit_STVAR_R8

/***************************************************************************************
 * High level macros for manipulating value classes
 ****************************************************************************************/

#define emit_LDVAR_VC(offset, valClassHnd)                                                \
{                                                                                         \
    emit_LDVARA(offset, typeSizeInBytes(jitInfo, valClassHnd));                           \
    if ( PASS_VALUETYPE_BYREF && offset > 0)                                              \
       {mov_register_indirect_to(TOS_REG_1,TOS_REG_1);       }                            \
    emit_valClassLoad(valClassHnd, false);                                                \
}

#define emit_STVAR_VC(offset, valClassHnd)                                                \
{                                                                                         \
    emit_LDVARA(offset, typeSizeInBytes(jitInfo, valClassHnd));                           \
    if ( PASS_VALUETYPE_BYREF && offset > 0 )                                             \
       {mov_register_indirect_to(TOS_REG_1,TOS_REG_1);     }                              \
    emit_valClassStore(valClassHnd, false);                                               \
}

    // This is needed for varargs support
#define emit_LDIND_VC(unaligned, valClassHnd)                                             \
{                                                                                         \
    emit_valClassLoad(valClassHnd, unaligned);                                            \
}

    // This is needed for varargs support
#define emit_STIND_REV_VC( unaligned, valClassHnd )                                       \
{                                                                                         \
    emit_valClassStore(valClassHnd, false);                                               \
}

#define emit_DUP_VC(dummy, valClassHnd)                                                   \
{   /* get pointer to current struct */                                                   \
    emit_getSP((STACK_BUFFER + bigEndianOffset( typeSizeInBytes(jitInfo, valClassHnd)))); \
    emit_valClassLoad(valClassHnd, false);                                                \
}

#define emit_POP_VC(dummy, valClassHnd)                                                   \
{                                                                                         \
    emit_drop(BYTE_ALIGNED(typeSizeInSlots(jitInfo, valClassHnd) * sizeof(void*)));       \
}

#define emit_loadretbuf() \
{                                                                                         \
    if ( EnregReturnBuffer )                                                              \
    {                                                                                     \
        unsigned retBufReg = ReturnBufferFirst ? 0 : methodInfo->args.hasThis();          \
        emit_WIN32(emit_LDVAR_I4(offsetOfRegister(retBufReg))) emit_WIN64(emit_LDVAR_I8(offsetOfRegister(retBufReg))); \
    }                                                                                     \
    else                                                                                  \
    {                                                                                     \
        emit_LDVAR_I4(RETURN_BUFF_OFFSET);                                                \
    }                                                                                     \
}                                                                                         \

#define emit_pushresult_VC(dummy, valClassHnd)          {}  /* result already where it belongs */

#define emit_loadresult_VC(dummy, valClassHnd)                                            \
{                                                                                         \
    emit_loadretbuf();                                                                    \
    emit_valClassStore(valClassHnd, false);                                               \
}

#define emit_copy_VC(dest, src, valClassHnd)                                              \
{                                                                                         \
 emit_getSP(dest+ bigEndianOffset( typeSizeInBytes(jitInfo, valClassHnd)));               \
 emit_getSP(src+SIZE_STACK_SLOT);                                                         \
 emit_valClassCopy( valClassHnd, false );                                                 \
}

/***************************************************************************************
 * High level macros for IL opcodes loading/storing local variables and arguments:
 *   LDIND.*, STIND.*
 *
 * There are two sets of macros for STIND.* operations, that expect two different
 * argument orders. One is used to implement the STIND.* IL opcode the other for
 * doing indirect stores for other macros.
 *
 * Typically need to be redefined for each platform in order to obtain acceptable
 * performance
 ****************************************************************************************/

#ifndef emit_LDIND_U1
#define emit_LDIND_U1(unaligned)                                      \
    enregisterTOS;                                                    \
    load_indirect_byte_zeroextend(TOS_REG_1,TOS_REG_1)
#endif // !emit_LDIND_U1

#ifndef emit_LDIND_U2
#define emit_LDIND_U2(unaligned)                                      \
    enregisterTOS;                                                    \
    load_indirect_word_zeroextend(TOS_REG_1,TOS_REG_1, (!unaligned) )
#endif // !emit_LDIND_U2

#ifndef emit_LDIND_I1
#define emit_LDIND_I1(unaligned)                                      \
    enregisterTOS;                                                    \
    load_indirect_byte_signextend(TOS_REG_1,TOS_REG_1)
#endif // !emit_LDIND_I1

#ifndef emit_LDIND_I2
#define emit_LDIND_I2(unaligned)                                      \
    enregisterTOS;                                                    \
    load_indirect_word_signextend(TOS_REG_1,TOS_REG_1, (!unaligned))
#endif // !emit_LDIND_I2

#ifndef emit_LDIND_I4
#define emit_LDIND_I4(unaligned)                                      \
    enregisterTOS;                                                    \
    load_indirect_dword_signextend(TOS_REG_1,TOS_REG_1, (!unaligned))
#endif // !emit_LDIND_I4

#ifndef emit_LDIND_U4
#define emit_LDIND_U4(unaligned)                                      \
    enregisterTOS;                                                    \
    load_indirect_dword_zeroextend(TOS_REG_1,TOS_REG_1, (!unaligned))
#endif // !emit_LDIND_U4

#define emit_LDIND_I(unaligned)                                       \
    emit_WIN32(emit_LDIND_I4(unaligned))                              \
    emit_WIN64(emit_LDIND_I8(unaligned))

#define emit_LDIND_PTR(unaligned)                                     \
    emit_LDIND_I(unaligned)

#ifndef emit_LDIND_I8
#define emit_LDIND_I8(unaligned)                                            \
{                                                                           \
  enregisterTOS;   /* get the point into TOS_REG */                         \
  if (SIZE_STACK_SLOT < 8 )                                                 \
  {                                                                         \
    mov_register( ARG_1, TOS_REG_1 );                                       \
    add_constant( TOS_REG_1, sizeof(void*));                                \
    load_indirect_dword_zeroextend( TOS_REG_1, TOS_REG_1, (!unaligned) );   \
    push_register( TOS_REG_1, true );                                       \
    mov_register( TOS_REG_1, ARG_1);                                        \
    load_indirect_dword_zeroextend( TOS_REG_1, TOS_REG_1, (!unaligned) );   \
    inRegTOS = true;                                                        \
  }                                                                         \
  else                                                                      \
  {                                                                         \
    load_indirect_dword_zeroextend( RESULT_1_OUT, TOS_REG_1, (!unaligned) );\
    add_constant( TOS_REG_1, sizeof(void*));                                \
    load_indirect_dword_zeroextend( RESULT_2_OUT, TOS_REG_1, (!unaligned) );\
    grow(SIZE_STACK_SLOT, false);                                           \
    mov_register( TOS_REG_1, SP);                                           \
    if (STACK_BUFFER) add_constant(TOS_REG_1, (int)STACK_BUFFER);           \
    mov_register_indirect_from( RESULT_1_OUT, TOS_REG_1 );                  \
    add_constant( TOS_REG_1, sizeof(void*));                                \
    mov_register_indirect_from( RESULT_2_OUT, TOS_REG_1 );                  \
    inRegTOS = false;                                                       \
  }                                                                         \
}
#endif // !emit_LDIND_I8


#ifndef emit_LDIND_R4
#define emit_LDIND_R4(unaligned) {                                          \
   emit_LDIND_I4(unaligned);                                                \
   emit_conv_R4toR();                                                       \
   }
#endif // !emit_LDIND_R4

#ifndef emit_LDIND_R8
#define emit_LDIND_R8(unaligned) emit_LDIND_I8(unaligned)   /* this should really load a 80bit float*/
#endif // !emit_LDIND_R8

// Note: the STIND op codes do not use the shared helpers since the
//       args on the stack are reversed
#ifndef emit_STIND_I1
#define emit_STIND_I1(unaligned)                                            \
    enregisterTOS;                                                          \
    pop_register(ARG_1, true);                                              \
    inRegTOS = false;                                                       \
    store_indirect_8bits(TOS_REG_1,ARG_1);
#endif

#ifndef emit_STIND_I2
#define emit_STIND_I2(unaligned)                                            \
    enregisterTOS;                                                          \
    pop_register(ARG_1, true);                                              \
    inRegTOS = false;                                                       \
    store_indirect_16bits(TOS_REG_1,ARG_1, (!unaligned));
#endif // !emit_STIND_I2

#ifndef emit_STIND_I
#define emit_STIND_I(unaligned)                                             \
    emit_WIN32(emit_STIND_I4(unaligned)) emit_WIN64(emit_STIND_I8(unaligned));
#endif

#ifndef emit_STIND_I4
#define emit_STIND_I4(unaligned)                                            \
    enregisterTOS;                                                          \
    pop_register(ARG_1, true);                                              \
    inRegTOS = false;                                                       \
    store_indirect_32bits(TOS_REG_1,ARG_1, (!unaligned));
#endif // !emit_STIND_I4

#ifndef emit_STIND_I8
#define emit_STIND_I8(unaligned)                                   \
{                                                                  \
  if (SIZE_STACK_SLOT < 8 )                                        \
  {                                                                \
    enregisterTOS; /*val lo*/                                      \
    pop_register(ARG_1, true);  /* val hi*/                        \
    pop_register(ARG_2, true); /*adr*/                             \
    store_indirect_32bits(TOS_REG_1, ARG_2,(!unaligned));          \
    add_constant(ARG_2,sizeof(void*));                             \
    store_indirect_32bits(ARG_1,ARG_2, (!unaligned));              \
    inRegTOS = false;                                              \
  }                                                                \
  else                                                             \
  {                                                                \
    _ASSERTE( inRegTOS == false );                                 \
    mov_register( TOS_REG_1, SP);                                  \
    if (STACK_BUFFER) add_constant(TOS_REG_1, (int)STACK_BUFFER);  \
    mov_register_indirect_to( RESULT_1_OUT, TOS_REG_1 );           \
    add_constant( TOS_REG_1, sizeof(void*));                       \
    mov_register_indirect_to( RESULT_2_OUT, TOS_REG_1 );           \
    emit_drop(SIZE_STACK_SLOT);                                    \
    pop_register( TOS_REG_1, true);                                \
    store_indirect_32bits( RESULT_1_OUT, TOS_REG_1,(!unaligned) ); \
    add_constant( TOS_REG_1, sizeof(void*));                       \
    store_indirect_32bits( RESULT_2_OUT, TOS_REG_1,(!unaligned) ); \
    inRegTOS = false;                                              \
  }                                                                \
}
#endif

#ifndef emit_STIND_R4
#define emit_STIND_R4(unaligned) {                                 \
        emit_conv_R8toR4();                                        \
        emit_STIND_I4(unaligned);                                  \
        }
#endif

#ifndef emit_STIND_R8
#define emit_STIND_R8(unaligned) emit_STIND_I8(unaligned)
#endif

#ifndef emit_STIND_REF
#define emit_STIND_REF(unaligned)          \
{                                          \
    LABELSTACK((outPtr-outBuff),2);        \
    callInfo.reset();                      \
    emit_tos_arg( 1, INTERNAL_CALL );      \
    emit_tos_arg( 2, INTERNAL_CALL );      \
    emit_callhelper_I4I4(STIND_REF_helper);\
}
#ifdef DECLARE_HELPERS
void HELPER_CALL STIND_REF_helper(CORINFO_Object* val, CORINFO_Object** ptr_) {
    HCALL2(void, FJit_pHlpAssign_Ref, CORINFO_Object**, ptr_, CORINFO_Object*, val);
}
#endif  // DECLARE_HELPERS
#endif  // emit_STIND_REF

#ifndef emit_STIND_REV_U1
#define emit_STIND_REV_U1(unaligned)    emit_STIND_REV_I1(unaligned)
#endif // !emit_LDIND_U1

#ifndef emit_STIND_REV_U2
#define emit_STIND_REV_U2(unaligned)    emit_STIND_REV_I2(unaligned)
#endif // !emit_LDIND_U2

#ifndef emit_STIND_REV_I1
#define emit_STIND_REV_I1(unaligned)        \
    enregisterTOS;                          \
    pop_register(ARG_1, true);              \
    inRegTOS = false;                       \
    store_indirect_8bits(ARG_1,TOS_REG_1);
#endif

#ifndef emit_STIND_REV_I2
#define emit_STIND_REV_I2(unaligned)        \
    enregisterTOS;                          \
    pop_register(ARG_1, true);              \
    inRegTOS = false;                       \
    store_indirect_16bits(ARG_1,TOS_REG_1,!(unaligned));
#endif // !emit_STIND_REV_I2

#ifndef emit_STIND_REV_I4
#define emit_STIND_REV_I4(unaligned)       \
    enregisterTOS;                         \
    pop_register(ARG_1, true);             \
    inRegTOS = false;                      \
    store_indirect_32bits(ARG_1,TOS_REG_1,!(unaligned));
#endif // !emit_STIND_REV_I4

#ifndef emit_STIND_REV_I8
#define emit_STIND_REV_I8(unaligned)                               \
{                                                                  \
  enregisterTOS; /* adr */                                         \
  if (SIZE_STACK_SLOT < 8 )                                        \
  {                                                                \
    pop_register(ARG_1, true);  /* val lo */                       \
    pop_register(ARG_2, true); /* vali hi */                       \
    store_indirect_32bits(ARG_1, TOS_REG_1,!(unaligned));          \
    add_constant(TOS_REG_1,sizeof(void*));                         \
    store_indirect_32bits(ARG_2, TOS_REG_1,!(unaligned));          \
  }                                                                \
  else                                                             \
  {                                                                \
    mov_register( RESULT_2, SP);                                   \
    if (STACK_BUFFER) add_constant(RESULT_2, STACK_BUFFER);        \
    mov_register_indirect_to( RESULT_1, RESULT_2 );                \
    store_indirect_32bits( RESULT_1, TOS_REG_1, !(unaligned) );    \
    add_constant( TOS_REG_1, sizeof(void*));                       \
    add_constant( RESULT_2, sizeof(void*));                        \
    mov_register_indirect_to( RESULT_1, RESULT_2 );                \
    store_indirect_32bits( RESULT_1, TOS_REG_1, !(unaligned) );    \
    emit_drop(SIZE_STACK_SLOT);                                    \
  }                                                                \
  inRegTOS = false;                                                \
}
#endif

#ifndef emit_STIND_REV_R4
#define emit_STIND_REV_R4(unaligned) {                   \
    enregisterTOS;                                       \
    inRegTOS = false;                                    \
    emit_conv_RtoR4()                                    \
    pop_register(ARG_1, true);                           \
    store_indirect_32bits(ARG_1,TOS_REG_1,!(unaligned)); \
}
#endif

#ifndef emit_STIND_REV_R8
#define emit_STIND_REV_R8(unaligned) emit_STIND_REV_I8(unaligned)
#endif

#ifndef emit_STIND_REV_Ref
#define emit_STIND_REV_Ref(unaligned)      \
{                                          \
    LABELSTACK((outPtr-outBuff), 1);       \
    callInfo.reset();                      \
    emit_tos_arg( 1, INTERNAL_CALL );      \
    emit_tos_arg( 2, INTERNAL_CALL );      \
    emit_callhelper_I4I4(STIND_REV_REF_helper)\
}
#ifdef DECLARE_HELPERS
void HELPER_CALL STIND_REV_REF_helper(CORINFO_Object** ptr_, CORINFO_Object* val) {
    HCALL2(void, FJit_pHlpAssign_Ref, CORINFO_Object**, ptr_, CORINFO_Object*, val);
}
#endif
#endif // !emit_STIND_REV_REF

/***************************************************************************************
 * High level macros for IL opcodes loading constants onto the stack:
 *   LDC.*
 * Typically need to be redefined for each platform in order to obtain acceptable
 * performance
 ****************************************************************************************/

#define emit_LDC_I(val) emit_WIN32(emit_LDC_I4(val)) ;\
                        emit_WIN64(emit_LDC_I8(val))

#ifndef emit_LDC_I4
#define emit_LDC_I4(val)    \
    emit_pushconstant_4(val)
#endif

#ifndef emit_LDC_I8
#define emit_LDC_I8(val)    \
    emit_pushconstant_8(val)
#endif

#ifndef emit_LDC_R4
#define emit_LDC_R4(val)  {          \
    emit_pushconstant_4(val); \
        emit_conv_R4toR();        \
}
#endif

#ifndef emit_LDC_R8
#define emit_LDC_R8(val)            \
    emit_pushconstant_8(val)
#endif

/***************************************************************************************
 * High level macros for stack manipulation IL opcodes:
 *   POP, DUP
 * Typically need to be redefined for each platform in order to obtain acceptable
 * performance
 ****************************************************************************************/

#ifndef emit_POP_I4
#define emit_POP_I4() \
{ \
   enregisterTOS; \
   inRegTOS = false; \
}
#endif // !emit_POP_I4

#ifndef emit_POP_I8
#define emit_POP_I8()      \
{                          \
  if (SIZE_STACK_SLOT < 8) \
  {                        \
      emit_POP_I4();       \
      emit_POP_I4();       \
  }                        \
  else /* I8 takes just one slot */ \
  {                        \
      emit_POP_I4();       \
  }                        \
}
#endif // !emit_POP_I8

#define emit_POP_PTR()          \
    emit_WIN32(emit_POP_I4())   \
    emit_WIN64(emit_POP_I8())

#ifndef emit_DUP_I4
#define emit_DUP_I4() \
{ \
   enregisterTOS; \
   push_register(TOS_REG_1, true); \
}
#endif // !emit_DUP_I4

#ifndef emit_DUP_I8
#define emit_DUP_I8()                                      \
{                                                          \
  if (SIZE_STACK_SLOT < 8) \
  {                        \
    emit_DUP_I4() ; \
    mov_register(RESULT_2,SP); \
    add_constant(RESULT_2, SIZE_STACK_SLOT + STACK_BUFFER); \
    mov_register_indirect_to(RESULT_2,RESULT_2); \
    push_register(RESULT_2, true); \
  }                                                         \
  else                                                      \
  {                                                         \
    _ASSERTE( inRegTOS == false );                          \
    emit_getSP(STACK_BUFFER);                               \
    emit_LDIND_I8(false);                                   \
  }                                                         \
}

#endif // !emit_DUP_I8

/***************************************************************************************
 * High level macros for arithmetic IL opcodes:
 *   ADD, SUB, DIV, MUL, REM
 * Typically need to be redefined for each platform in order to obtain acceptable
 * performance
 ****************************************************************************************/

/************* ADD ************************/
#ifndef emit_ADD_I4
#define emit_ADD_I4()                      \
{                                          \
    callInfo.reset();                      \
    emit_tos_arg( 1, INTERNAL_CALL );      \
    emit_tos_arg( 2, INTERNAL_CALL );      \
    emit_callhelper_I4I4_I4(ADD_I4_helper);\
    emit_pushresult_I4()                   \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL ADD_I4_helper(int i, int j) {
    return j + i;
}
#endif
#endif

#ifndef emit_ADD_I8
#define emit_ADD_I8()                                                        \
{                                                                            \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I8(ADD_I8_helper);                                  \
    emit_pushresult_I8();                                                    \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL ADD_I8_helper(__int64 i, __int64 j) {
    return j + i;
}
#endif
#endif

#ifndef emit_ADD_R4
#define emit_ADD_R4()                      \
{                                          \
    callInfo.reset();                      \
    emit_tos_arg( 1, INTERNAL_CALL );      \
    emit_tos_arg( 2, INTERNAL_CALL );      \
    emit_callhelper_I4I4_I4(ADD_R4_helper);\
    emit_pushresult_I4();                  \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL ADD_R4_helper(int i, int j) {
    float result = (*(float *)&j) + (*(float *)&i);
    return *(unsigned int*)&result;
}
#endif
#endif

#ifndef emit_ADD_R8
#define emit_ADD_R8()                                                        \
{                                                                            \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I8(ADD_R8_helper);                                  \
    emit_pushresult_I8();                                                    \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL ADD_R8_helper(__int64 i, __int64 j) {
    double result = (*(double *)&j) + (*(double *)&i);
    return *(unsigned __int64*)&result;
}
#endif
#endif

/************* ADD_OVF ************************/

#ifndef emit_ADD_OVF_I1
#define emit_ADD_OVF_I1()                       \
{                                               \
    LABELSTACK((outPtr-outBuff),2);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_callhelper_I4I4_I4(ADD_OVF_I1_helper); \
    emit_pushresult_I4();                       \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL ADD_OVF_I1_helper(int i, int j) {
    int i4 = j + i;
    if ((int)((signed char) i4) != i4) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return i4;
}
#endif
#endif

#ifndef emit_ADD_OVF_I2
#define emit_ADD_OVF_I2()                       \
{                                               \
    LABELSTACK((outPtr-outBuff),2);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_callhelper_I4I4_I4(ADD_OVF_I2_helper); \
    emit_pushresult_I4();                       \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL ADD_OVF_I2_helper(int i, int j) {
    int i4 = j + i;
    if ((int)((signed short) i4) != i4) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return i4;
}
#endif
#endif

#ifndef emit_ADD_OVF_I4
#define emit_ADD_OVF_I4()                       \
{                                               \
    LABELSTACK((outPtr-outBuff),2);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_callhelper_I4I4_I4(ADD_OVF_I4_helper); \
    emit_pushresult_I4();                       \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL ADD_OVF_I4_helper(int i, int j) {
    int i4 = j + i;
    // if the signs of i and j are different, then we can never overflow
    // if the signs of i and j are the same, then the result must have the same sign
    if ((j ^ i) >= 0) {
        // i and j have the same sign (the sign bit of j^i is not set)
        // ensure that the result has the same sign
        if ((i4 ^ j) < 0) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
        }
    }
    return i4;
}
#endif
#endif

#ifndef emit_ADD_OVF_I8
#define emit_ADD_OVF_I8()                                                    \
{                                                                            \
    LABELSTACK((outPtr-outBuff),2);                                          \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_R8I8_I8(ADD_OVF_I8_helper);                              \
    emit_pushresult_I8();                                                    \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL ADD_OVF_I8_helper(__int64 i, __int64 j) {
    __int64 i8 = j + i;
    // if the signs of i and j are different, then we can never overflow
    // if the signs of i and j are the same, then the result must have the same sign
    if ((j>=0) == (i>=0)) {
        // ensure that the result has the same sign
        if ((i8>=0) != (j>=0)) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
        }
    }
    return i8;
}
#endif
#endif

#ifndef emit_ADD_OVF_U1
#define emit_ADD_OVF_U1()                       \
{                                               \
    LABELSTACK((outPtr-outBuff),2);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_callhelper_I4I4_I4(ADD_OVF_U1_helper); \
    emit_pushresult_U4();                       \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL ADD_OVF_U1_helper(unsigned int i, unsigned int j) {
    unsigned int u4 = j + i;
    if (u4 > 0xff) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return u4;
}
#endif
#endif

#ifndef emit_ADD_OVF_U2
#define emit_ADD_OVF_U2()                       \
{                                               \
    LABELSTACK((outPtr-outBuff),2);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_callhelper_I4I4_I4(ADD_OVF_U2_helper); \
    emit_pushresult_U4();                       \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL ADD_OVF_U2_helper(unsigned int i, unsigned int j) {
    unsigned int u4 = j + i;
    if (u4 > 0xffff) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return u4;
}
#endif
#endif

#ifndef emit_ADD_OVF_U4
#define emit_ADD_OVF_U4()                       \
{                                               \
    LABELSTACK((outPtr-outBuff),2);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_callhelper_I4I4_I4(ADD_OVF_U4_helper); \
    emit_pushresult_U4();                       \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL ADD_OVF_U4_helper(unsigned int i, unsigned int j) {
    unsigned int u4 = j + i;
    if (u4 < j) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return u4;
}
#endif
#endif

#ifndef emit_ADD_OVF_U8
#define emit_ADD_OVF_U8()                                                    \
{                                                                            \
    LABELSTACK((outPtr-outBuff),2);                                          \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I8(ADD_OVF_U8_helper);                              \
    emit_pushresult_U8();                                                    \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL ADD_OVF_U8_helper(unsigned __int64 i, unsigned __int64 j) {
    unsigned __int64 u8 = j + i;
    if (u8 < j) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return u8;
}
#endif
#endif

/************* SUB ************************/

#ifndef emit_SUB_I4
#define emit_SUB_I4()                       \
{                                           \
    callInfo.reset();                       \
    emit_tos_arg( 1, INTERNAL_CALL );       \
    emit_tos_arg( 2, INTERNAL_CALL );       \
    emit_callhelper_I4I4_I4(SUB_I4_helper); \
    emit_pushresult_I4()                    \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL SUB_I4_helper(int i, int j) {
    return j - i;
}
#endif
#endif

#ifndef emit_SUB_I8
#define emit_SUB_I8()   \
{                                                                            \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I8(SUB_I8_helper);                                  \
    emit_pushresult_I8();                                                    \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL SUB_I8_helper(__int64 i, __int64 j) {
    return j - i;
}
#endif
#endif

#ifndef emit_SUB_R4
#define emit_SUB_R4()   \
{                                           \
    callInfo.reset();                       \
    emit_tos_arg( 1, INTERNAL_CALL );       \
    emit_tos_arg( 2, INTERNAL_CALL );       \
    emit_callhelper_I4I4_I4(SUB_R4_helper); \
    emit_pushresult_I4();                   \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL SUB_R4_helper(int i, int j) {
    float result = (*(float *)&j) - (*(float *)&i);
    return *(unsigned int*)&result;
}
#endif
#endif

#ifndef emit_SUB_R8
#define emit_SUB_R8()   \
{                                                                            \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I8(SUB_R8_helper);                                  \
    emit_pushresult_I8();                                                    \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL SUB_R8_helper(__int64 i, __int64 j) {
    double result = (*(double *)&j) - (*(double *)&i);
    return *(unsigned __int64*)&result;
}
#endif
#endif

/************* SUB_OVF ******************/

#ifndef emit_SUB_OVF_I1
#define emit_SUB_OVF_I1()                       \
{                                               \
    LABELSTACK((outPtr-outBuff),2);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_callhelper_I4I4_I4(SUB_OVF_I1_helper); \
    emit_pushresult_I4();                       \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL SUB_OVF_I1_helper(int i, int j) {
    int i4 = j - i;
    if ((int)((signed char) i4) != i4) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return i4;
}
#endif
#endif

#ifndef emit_SUB_OVF_I2
#define emit_SUB_OVF_I2()                       \
{                                               \
    LABELSTACK((outPtr-outBuff),2);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_callhelper_I4I4_I4(SUB_OVF_I2_helper); \
    emit_pushresult_I4();                       \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL SUB_OVF_I2_helper(int i, int j) {
    int i4 = j - i;
    if ((int)((signed short) i4) != i4) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return i4;
}
#endif
#endif

#ifndef emit_SUB_OVF_I4
#define emit_SUB_OVF_I4()                       \
{                                               \
    LABELSTACK((outPtr-outBuff),2);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_callhelper_I4I4_I4(SUB_OVF_I4_helper); \
    emit_pushresult_I4();                       \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL SUB_OVF_I4_helper(int i, int j) {
    int i4 = j - i;
    // if the signs of i and j are the same, then we can never overflow
    // if the signs of i and j are different, then the result must have the same sign as j
    if ((j ^ i) < 0) {
        // i and j have different sign (the sign bit of j^i is set)
        // ensure that the result has the same sign as j
        if ((i4 ^ j) < 0) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
        }
    }
    return i4;
}
#endif
#endif

#ifndef emit_SUB_OVF_I8
#define emit_SUB_OVF_I8()                                                    \
{                                                                            \
    LABELSTACK((outPtr-outBuff),2);                                          \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I8(SUB_OVF_I8_helper);                              \
    emit_pushresult_I8();                                                    \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL SUB_OVF_I8_helper(__int64 i, __int64 j) {
    __int64 i8 = j - i;
    // if the signs of i and j are the same, then we can never overflow
    // if the signs of i and j are different, then the result must have the same sign as j
    if ((j>=0) != (i>=0)) {
        // ensure that the result has the same sign as j
        if ((i8>=0) != (j>=0)) {
            THROW_FROM_HELPER_RET(CORINFO_OverflowException);
        }
    }
    return i8;
}
#endif
#endif

#ifndef emit_SUB_OVF_U1
#define emit_SUB_OVF_U1()                       \
{                                               \
    LABELSTACK((outPtr-outBuff),2);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_callhelper_I4I4_I4(SUB_OVF_U1_helper); \
    emit_pushresult_U4();                       \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL SUB_OVF_U1_helper(unsigned int i, unsigned int j) {
    unsigned int u4 = j - i;
    if (u4 > 0xff) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return u4;
}
#endif
#endif

#ifndef emit_SUB_OVF_U2
#define emit_SUB_OVF_U2()                       \
{                                               \
    LABELSTACK((outPtr-outBuff),2);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_callhelper_I4I4_I4(SUB_OVF_U2_helper); \
    emit_pushresult_U4();                       \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL SUB_OVF_U2_helper(unsigned int i, unsigned int j) {
    unsigned int u4 = j - i;
    if (u4 > 0xffff) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return u4;
}
#endif
#endif

#ifndef emit_SUB_OVF_U4
#define emit_SUB_OVF_U4()                       \
{                                               \
    LABELSTACK((outPtr-outBuff),2);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_callhelper_I4I4_I4(SUB_OVF_U4_helper); \
    emit_pushresult_U4();                       \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL SUB_OVF_U4_helper(unsigned int i, unsigned int j) {
    unsigned int u4 = j - i;
    if (u4 > j) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return u4;
}
#endif
#endif

#ifndef emit_SUB_OVF_U8
#define emit_SUB_OVF_U8()                                                    \
{                                                                            \
    LABELSTACK((outPtr-outBuff),2);                                          \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I8(SUB_OVF_U8_helper);                              \
    emit_pushresult_U8();                                                    \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL SUB_OVF_U8_helper(unsigned __int64 i, unsigned __int64 j) {
    unsigned __int64 u8 = j - i;
    if (u8 > j) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return u8;
}
#endif
#endif

/************* MUL ************************/

#ifndef emit_MUL_I4
#define emit_MUL_I4()                       \
{                                           \
    callInfo.reset();                       \
    emit_tos_arg( 1, INTERNAL_CALL );       \
    emit_tos_arg( 2, INTERNAL_CALL );       \
    emit_callhelper_I4I4_I4(MUL_I4_helper); \
    emit_pushresult_I4();                   \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL MUL_I4_helper(int i, int j) {
    return j * i;
}
#endif
#endif

#ifndef emit_MUL_I8
#define emit_MUL_I8()   \
{                                                                            \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I8(MUL_I8_helper);                                  \
    emit_pushresult_I8();                                                    \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL MUL_I8_helper(__int64 i, __int64 j) {
    return j * i;
}
#endif
#endif

#ifndef emit_MUL_R4
#define emit_MUL_R4()   \
{                                           \
    callInfo.reset();                       \
    emit_tos_arg( 1, INTERNAL_CALL );       \
    emit_tos_arg( 2, INTERNAL_CALL );       \
    emit_callhelper_I4I4_I4(MUL_R4_helper); \
    emit_pushresult_I4();                   \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL MUL_R4_helper(int i, int j) {
    float result = (*(float *)&j) * (*(float *)&i);
    return *(unsigned int*)&result;
}
#endif
#endif

#ifndef emit_MUL_R8
#define emit_MUL_R8()                                                        \
{                                                                            \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_R8R8_R8(MUL_R8_helper);                                  \
    emit_pushresult_I8();                                                    \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL MUL_R8_helper(__int64 i, __int64 j) {
    double result = (*(double *)&j) * (*(double *)&i);
    return *(unsigned __int64*)&result;
}
#endif
#endif

/************* MUL_OVF ************************/

#ifndef emit_MUL_OVF_I1
#define emit_MUL_OVF_I1()                               \
{                                                       \
    LABELSTACK((outPtr-outBuff),0);                     \
    callInfo.reset();                                   \
    emit_tos_arg( 1, INTERNAL_CALL );                   \
    emit_tos_arg( 2, INTERNAL_CALL );                   \
    emit_callhelper_I4I4_I4(MUL_OVF_I1_helper);         \
    emit_pushresult_I4();                               \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL MUL_OVF_I1_helper(int i, int j) {
    int i4 = j * i;
    if((int)(signed char) i4 != i4) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return i4;
}
#endif
#endif

#ifndef emit_MUL_OVF_I2
#define emit_MUL_OVF_I2()                               \
{                                                       \
    LABELSTACK((outPtr-outBuff),0);                     \
    callInfo.reset();                                   \
    emit_tos_arg( 1, INTERNAL_CALL );                   \
    emit_tos_arg( 2, INTERNAL_CALL );                   \
    emit_callhelper_I4I4_I4(MUL_OVF_I2_helper);         \
    emit_pushresult_I4();                               \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL MUL_OVF_I2_helper(int i, int j) {
    int i4 = j * i;
    if((int)(signed short) i4 != i4) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return i4;
}
#endif
#endif

#ifndef emit_MUL_OVF_I4
#define emit_MUL_OVF_I4()                               \
{                                                       \
    LABELSTACK((outPtr-outBuff),0);                     \
    callInfo.reset();                                   \
    emit_tos_arg( 1, INTERNAL_CALL );                   \
    emit_tos_arg( 2, INTERNAL_CALL );                   \
    emit_callhelper_I4I4_I4(MUL_OVF_I4_helper);         \
    emit_pushresult_I4();                               \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL MUL_OVF_I4_helper(int i, int j) {
    __int64 i8 = (__int64) j * (__int64) i;
    if((__int64)(int) i8 != i8) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (int) i8;
}
#endif
#endif

#ifndef emit_MUL_OVF_I8
#define emit_MUL_OVF_I8()                           \
{                                                                            \
    LABELSTACK((outPtr-outBuff),0);                                          \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I8(FJit_pHlpLMulOvf);                               \
    emit_pushresult_I8();                                                    \
}
#endif

#ifndef emit_MUL_OVF_U1
#define emit_MUL_OVF_U1()                               \
{                                                       \
    LABELSTACK((outPtr-outBuff),0);                     \
    callInfo.reset();                                   \
    emit_tos_arg( 1, INTERNAL_CALL );                   \
    emit_tos_arg( 2, INTERNAL_CALL );                   \
    emit_callhelper_I4I4_I4(MUL_OVF_U1_helper);         \
    emit_pushresult_U4();                               \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL MUL_OVF_U1_helper(unsigned int i, unsigned int j) {
    unsigned int u4 = j * i;
    if(u4 > 0xff) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return u4;
}
#endif
#endif

#ifndef emit_MUL_OVF_U2
#define emit_MUL_OVF_U2()                               \
{                                                       \
    LABELSTACK((outPtr-outBuff),0);                     \
    callInfo.reset();                                   \
    emit_tos_arg( 1, INTERNAL_CALL );                   \
    emit_tos_arg( 2, INTERNAL_CALL );                   \
    emit_callhelper_I4I4_I4(MUL_OVF_U2_helper);         \
    emit_pushresult_U4();                               \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL MUL_OVF_U2_helper(unsigned int i, unsigned int j) {
    unsigned int u4 = j * i;
    if(u4 > 0xffff) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return u4;
}
#endif
#endif

#ifndef emit_MUL_OVF_U4
#define emit_MUL_OVF_U4()                               \
{                                                       \
    LABELSTACK((outPtr-outBuff),0);                     \
    callInfo.reset();                                   \
    emit_tos_arg( 1, INTERNAL_CALL );                   \
    emit_tos_arg( 2, INTERNAL_CALL );                   \
    emit_callhelper_I4I4_I4(MUL_OVF_U4_helper);         \
    emit_pushresult_U4();                               \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL MUL_OVF_U4_helper(unsigned int i, unsigned int j) {
    unsigned __int64 u8 = (unsigned __int64) j * (unsigned __int64) i;
    if(u8 > 0xffffffff) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (unsigned int) u8;
}
#endif
#endif

#ifndef emit_MUL_OVF_U8
#define emit_MUL_OVF_U8()                           \
{                                                                            \
    LABELSTACK((outPtr-outBuff),0);                                          \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I8(MUL_OVF_U8_helper);                              \
    emit_pushresult_U8();                                                    \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL MUL_OVF_U8_helper(unsigned __int64 i, unsigned __int64 j) {
    unsigned __int64 u8 = 0;
    while (i > 0) {
        if (i & 1) {
            if (u8 + j < u8) {
                THROW_FROM_HELPER_RET(CORINFO_OverflowException);
            }
            u8 += j;
        }
        i >>= 1;
        if (i > 0 && (j & I64(0x8000000000000000)))
        {
            THROW_FROM_HELPER_RET(CORINFO_OverflowException);
        }
        j <<= 1;
    }
    return u8;
}
#endif
#endif

/************* DIV ************************/

#ifndef emit_DIV_I4
#define emit_DIV_I4()                             \
{                                                 \
    LABELSTACK((outPtr-outBuff),2);               \
    callInfo.reset();                             \
    emit_tos_arg( 1, INTERNAL_CALL );             \
    emit_tos_arg( 2, INTERNAL_CALL );             \
    emit_callhelper_I4I4_I4(DIV_I4_helper);       \
    emit_pushresult_I4();                         \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL DIV_I4_helper(int i, int j) {

    // check for divisor == 0 and divisor == -1 cases at the same time
    if (((unsigned int) -i) <= 1) {
        if(i == 0) {
            THROW_FROM_HELPER_RET(CORINFO_DivideByZeroException)
        }
        else if (j == (int)0x80000000 ) {
            //divisor == -1, dividend == MIN_INT
            THROW_FROM_HELPER_RET(CORINFO_OverflowException);
        }
    }
    return j / i;
}
#endif
#endif

#ifndef emit_DIV_I8
#define emit_DIV_I8()                                                        \
{                                                                            \
    LABELSTACK((outPtr-outBuff),2);                                          \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I8(DIV_I8_helper);                                  \
    emit_pushresult_I8();                                                    \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL DIV_I8_helper(__int64 i, __int64 j) {
    // check for divisor == 0 and divisor == -1 cases at the same time
    if (((unsigned __int64) -i) <= 1) {
        if(i == 0) {
            THROW_FROM_HELPER_RET(CORINFO_DivideByZeroException);
        }
        else if ((unsigned __int64) j == UI64(0x8000000000000000) ) {
            //divisor == -1, dividend == MIN_INT
            THROW_FROM_HELPER_RET(CORINFO_OverflowException);
        }
    }
    return j / i;
}
#endif
#endif

#ifndef emit_DIV_UN_U4
#define emit_DIV_UN_U4()                          \
{                                                 \
    LABELSTACK((outPtr-outBuff),2);               \
    callInfo.reset();                             \
    emit_tos_arg( 1, INTERNAL_CALL );             \
    emit_tos_arg( 2, INTERNAL_CALL );             \
    emit_callhelper_I4I4_I4(DIV_UN_U4_helper);    \
    emit_pushresult_U4();                         \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL DIV_UN_U4_helper(unsigned int i, unsigned int j) {
    if (i == 0) {
        THROW_FROM_HELPER_RET(CORINFO_DivideByZeroException);
    }
    return j / i;
}
#endif
#endif

#ifndef emit_DIV_UN_U8
#define emit_DIV_UN_U8()                                                     \
{                                                                            \
    LABELSTACK((outPtr-outBuff),2);                                          \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I8(DIV_UN_U8_helper);                               \
    emit_pushresult_U8();                                                    \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL DIV_UN_U8_helper(unsigned __int64 i, unsigned __int64 j) {
    if(i == 0) {
        THROW_FROM_HELPER_RET(CORINFO_DivideByZeroException);
    }
    return j / i;
}
#endif
#endif

#ifndef emit_DIV_R4
#define emit_DIV_R4()                             \
{                                                 \
    LABELSTACK((outPtr-outBuff),2);               \
    callInfo.reset();                             \
    emit_tos_arg( 1, INTERNAL_CALL );             \
    emit_tos_arg( 2, INTERNAL_CALL );             \
    emit_callhelper_I4I4_I4(DIV_R4_helper);       \
    emit_pushresult_I4();                         \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL DIV_R4_helper(int i, int j) {
    float result = (*(float *)&j) / (*(float *)&i);
    return *(unsigned int*)&result;
}
#endif
#endif

#ifndef emit_DIV_R8
#define emit_DIV_R8()                                                        \
{                                                                            \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I8(DIV_R8_helper);                                  \
    emit_pushresult_I8();                                                    \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL DIV_R8_helper(__int64 i, __int64 j) {
    double result = (*(double *)&j) / (*(double *)&i);
    return *(unsigned __int64*)&result;
}
#endif
#endif

/************* REM ************************/

#ifndef emit_REM_I4
#define emit_REM_I4()                             \
{                                                 \
    LABELSTACK((outPtr-outBuff),2);               \
    callInfo.reset();                             \
    emit_tos_arg( 1, INTERNAL_CALL );             \
    emit_tos_arg( 2, INTERNAL_CALL );             \
    emit_callhelper_I4I4_I4(REM_I4_helper);       \
    emit_pushresult_I4();                         \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL REM_I4_helper(int i, int j) {
    // check for divisor == 0 and divisor == -1 cases at the same time
    if (((unsigned int) -i) <= 1) {
        if(i == 0) {
            THROW_FROM_HELPER_RET(CORINFO_DivideByZeroException);
        }
        else if (j == (int) (0x80000000) ) {
            //divisor == -1, dividend == MIN_INT
            THROW_FROM_HELPER_RET(CORINFO_OverflowException);
        }
    }
    return j % i;
}
#endif
#endif

#ifndef emit_REM_I8
#define emit_REM_I8()                                                        \
{                                                                            \
    LABELSTACK((outPtr-outBuff),2);                                          \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I8(REM_I8_helper);                                  \
    emit_pushresult_I8();                                                    \
}

#ifdef DECLARE_HELPERS
__int64 HELPER_CALL REM_I8_helper(__int64 i, __int64 j) {
    // check for divisor == 0 and divisor == -1 cases at the same time
    if (((unsigned __int64) -i) <= 1) {
        if(i == 0) {
            THROW_FROM_HELPER_RET(CORINFO_DivideByZeroException);
        }
        else if ((unsigned __int64) j == UI64(0x8000000000000000) ) {
            //divisor == -1, dividend == MIN_INT
            THROW_FROM_HELPER_RET(CORINFO_OverflowException);
        }
    }
    return j % i;
}
#endif
#endif

#ifndef emit_REM_R8
#define emit_REM_R8()                                                        \
{                                                                            \
    LABELSTACK((outPtr-outBuff),2);                                          \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I8(REM_R8_helper);                                  \
    emit_pushresult_I8();                                                    \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL REM_R8_helper(__int64 i, __int64 j) {
    double result =
        ((double (__stdcall *) (double divisor, double dividend))(FJit_pHlpDblRem))
            ( (*(double *)&i), (*(double *)&j) );
    return *(unsigned __int64*)&result;
}
#endif
#endif

#ifndef emit_REM_UN_U4
#define emit_REM_UN_U4()                          \
{                                                 \
    LABELSTACK((outPtr-outBuff),2);               \
    callInfo.reset();                             \
    emit_tos_arg( 1, INTERNAL_CALL );             \
    emit_tos_arg( 2, INTERNAL_CALL );             \
    emit_callhelper_I4I4_I4(REM_UN_U4_helper);    \
    emit_pushresult_U4();                         \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL REM_UN_U4_helper(unsigned int i, unsigned int j) {
    if (i == 0) {
        THROW_FROM_HELPER_RET(CORINFO_DivideByZeroException);
    }
    return j % i;
}
#endif
#endif

#ifndef emit_REM_UN_U8
#define emit_REM_UN_U8()                           \
{                                                                            \
    LABELSTACK((outPtr-outBuff),2);                                          \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I8(REM_UN_U8_helper);                               \
    emit_pushresult_U8();                                                    \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL REM_UN_U8_helper(unsigned __int64 i, unsigned __int64 j) {
    if(i == 0) {
        THROW_FROM_HELPER_RET(CORINFO_DivideByZeroException);
    }
    return j % i;
}
#endif
#endif

/************* CKFINITE ******************/
#ifndef emit_CKFINITE_R
#define emit_CKFINITE_R()                                      \
{                                                              \
    emit_conv_RtoR8();                                         \
    LABELSTACK((outPtr-outBuff), 1);                           \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I8(CKFINITE_R8_helper);                 \
    emit_pushresult_U8();                                      \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL CKFINITE_R8_helper(__int64 i) {
    if (!_finite((*(double *)&i))) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return ((unsigned __int64)i);
}
#endif
#endif

/****************************************************************************************
 * High level macros for IL opcodes performing logical operations:
 *   OR, AND, XOR, NOT, NEG, SHL, SHR, SHR.UN
 * Typically need to be redefined for each platform in order to obtain acceptable
 * performance
 ***************************************************************************************/

/************* OR ********************/
#ifndef emit_OR_U4
#define emit_OR_U4()                        \
{                                           \
    callInfo.reset();                       \
    emit_tos_arg( 1, INTERNAL_CALL );       \
    emit_tos_arg( 2, INTERNAL_CALL );       \
    emit_callhelper_I4I4_I4(OR_U4_helper);  \
    emit_pushresult_U4();                   \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL OR_U4_helper(unsigned int i, unsigned int j) {
    return j | i;
}
#endif
#endif

#ifndef emit_OR_U8
#define emit_OR_U8()                                                         \
{                                                                            \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I8(OR_U8_helper);                                   \
    emit_pushresult_U8();                                                    \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL OR_U8_helper(unsigned __int64 i, unsigned __int64 j) {
    return j | i;
}
#endif
#endif

/************* AND ********************/
#ifndef emit_AND_U4
#define emit_AND_U4()                       \
{                                           \
    callInfo.reset();                       \
    emit_tos_arg( 1, INTERNAL_CALL );       \
    emit_tos_arg( 2, INTERNAL_CALL );       \
    emit_callhelper_I4I4_I4(AND_U4_helper); \
    emit_pushresult_U4();                   \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL AND_U4_helper(unsigned int i, unsigned int j) {
    return j & i;
}
#endif
#endif

#ifndef emit_AND_U8
#define emit_AND_U8()                                                        \
{                                                                            \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I8(AND_U8_helper);                                  \
    emit_pushresult_U8();                                                    \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL AND_U8_helper(unsigned __int64 i, unsigned __int64 j) {
    return j & i;
}
#endif
#endif

/************* XOR ********************/
#ifndef emit_XOR_U4
#define emit_XOR_U4()                       \
{                                           \
    callInfo.reset();                       \
    emit_tos_arg( 1, INTERNAL_CALL );       \
    emit_tos_arg( 2, INTERNAL_CALL );       \
    emit_callhelper_I4I4_I4(XOR_U4_helper); \
    emit_pushresult_U4();                   \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL XOR_U4_helper(unsigned int i, unsigned int j) {
    return j ^ i;
}
#endif
#endif

#ifndef emit_XOR_U8
#define emit_XOR_U8()                                                        \
{                                                                            \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I8(XOR_U8_helper);                                  \
    emit_pushresult_U8();                                                    \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL XOR_U8_helper(unsigned __int64 i, unsigned __int64 j) {
    return j ^ i;
}
#endif
#endif

/************* NOT ********************/
#ifndef emit_NOT_U4
#define emit_NOT_U4()                       \
{                                           \
    callInfo.reset();                       \
    emit_tos_arg( 1, INTERNAL_CALL );       \
    emit_callhelper_I4_I4(NOT_U4_helper);   \
    emit_pushresult_U4();                   \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL NOT_U4_helper(int val) {
    return ~val;
}
#endif
#endif

#ifndef emit_NOT_U8
#define emit_NOT_U8()                                           \
{                                                               \
    callInfo.reset();                                           \
    int NumRegUsed = 0;                                         \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8_I8(NOT_U8_helper);                       \
    emit_pushresult_U8();                                       \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL NOT_U8_helper(__int64 val) {
    return ~val;
}
#endif
#endif

/************* NEG ********************/
#ifndef emit_NEG_I4
#define emit_NEG_I4()                       \
{                                           \
    callInfo.reset();                       \
    emit_tos_arg( 1, INTERNAL_CALL );       \
    emit_callhelper_I4_I4(Neg_I4_helper);   \
    emit_pushresult_I4();                   \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL Neg_I4_helper(int val) {
    return -val;
}
#endif
#endif

#ifndef emit_NEG_I8
#define emit_NEG_I8()                                           \
{                                                               \
    callInfo.reset();                                           \
    int NumRegUsed = 0;                                         \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8_I8(Neg_I8_helper);                       \
    emit_pushresult_I8();                                       \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL Neg_I8_helper(__int64 val) {
    return -val;
}
#endif
#endif

#ifndef emit_NEG_R4
#define emit_NEG_R4()                       \
{                                           \
    callInfo.reset();                       \
    emit_tos_arg( 1, INTERNAL_CALL );       \
    emit_callhelper_I4_I4(Neg_R4_helper);   \
    emit_pushresult_I4();                   \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL Neg_R4_helper(int val) {
    float result = -(*(float *)&val);
    return *(int*)&result;
}
#endif
#endif

#ifndef emit_NEG_R8
#define emit_NEG_R8()                                           \
{                                                               \
    callInfo.reset();                                           \
    int NumRegUsed = 0;                                         \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8_I8(Neg_R8_helper);                       \
    emit_pushresult_I8();                                       \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL Neg_R8_helper(__int64 val) {
    double result = -(*(double *)&val);
    return *(__int64*)&result;
}
#endif
#endif

/************* SHR_UN ********************/
#ifndef emit_SHR_S_U4
#define emit_SHR_S_U4()                     \
{                                           \
    callInfo.reset();                       \
    emit_tos_arg( 1, INTERNAL_CALL );       \
    emit_tos_arg( 2, INTERNAL_CALL );       \
    emit_callhelper_I4I4_I4(SHR_I4_helper); \
    emit_pushresult_I4();                   \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL SHR_I4_helper(unsigned int cnt, int val) {
    return val>>cnt;
}
#endif
#endif

#ifndef emit_SHR_S_U8
#define emit_SHR_S_U8()                                         \
{                                                               \
    callInfo.reset();                                           \
    int NumRegUsed = 0;                                         \
    emit_tos_arg( 1, INTERNAL_CALL );                           \
    emit_tos_fixedsize_arg( 2, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I4I8_I8(SHR_I8_helper);                     \
    emit_pushresult_I8();                                       \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL SHR_I8_helper(unsigned int cnt, __int64 val) {
    return val>>cnt;
}
#endif
#endif

/************* SHR_UN ********************/
#ifndef emit_SHR_U4
#define emit_SHR_U4()                       \
{                                           \
    callInfo.reset();                       \
    emit_tos_arg( 1, INTERNAL_CALL );       \
    emit_tos_arg( 2, INTERNAL_CALL );       \
    emit_callhelper_I4I4_I4(SHR_U4_helper); \
    emit_pushresult_U4();                   \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL SHR_U4_helper(unsigned int cnt, unsigned int val) {
    return val>>cnt;
}
#endif
#endif

#ifndef emit_SHR_U8
#define emit_SHR_U8()                                           \
{                                                               \
    callInfo.reset();                                           \
    int NumRegUsed = 0;                                         \
    emit_tos_arg( 1, INTERNAL_CALL );                           \
    emit_tos_fixedsize_arg( 2, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I4I8_I8(SHR_U8_helper);                     \
    emit_pushresult_U8();                                       \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL SHR_U8_helper(unsigned int cnt, unsigned __int64 val) {
    return val>>cnt;
}
#endif
#endif


/************* SHL ********************/
#ifndef emit_SHL_U4
#define emit_SHL_U4()                       \
{                                           \
    callInfo.reset();                       \
    emit_tos_arg( 1, INTERNAL_CALL );       \
    emit_tos_arg( 2, INTERNAL_CALL );       \
    emit_callhelper_I4I4_I4(SHL_U4_helper); \
    emit_pushresult_U4();                   \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL SHL_U4_helper(unsigned int cnt, unsigned int val) {
    return val<<cnt;
}
#endif
#endif

#ifndef emit_SHL_U8
#define emit_SHL_U8()                                           \
{                                                               \
    callInfo.reset();                                           \
    int NumRegUsed = 0;                                         \
    emit_tos_arg( 1, INTERNAL_CALL );                           \
    emit_tos_fixedsize_arg( 2, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I4I8_I8(SHL_U8_helper);                     \
    emit_pushresult_U8();                                       \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL SHL_U8_helper(unsigned int cnt, unsigned __int64 val) {
    return val<<cnt;
}
#endif
#endif

/****************************************************************************************
 * High level macros for branching IL opcodes:
 *   CEQ, CGT, CGE, CLT, CLE
 * Typically need to be redefined for each platform in order to obtain acceptable
 * performance
 ***************************************************************************************/

/************* CEQ ********************/
#ifndef emit_CEQ_I4
#define emit_CEQ_I4()                             \
{                                                 \
    callInfo.reset();                             \
    emit_tos_arg( 1, INTERNAL_CALL );             \
    emit_tos_arg( 2, INTERNAL_CALL );             \
    emit_callhelper_I4I4_I4(CEQ_I4_helper);       \
    emit_pushresult_I4();                         \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CEQ_I4_helper(int i, int j) {
    return (j == i);
}
#endif
#endif


#ifndef emit_CEQ_I8
#define emit_CEQ_I8()                                                        \
{                                                                            \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I4(CEQ_I8_helper);                                  \
    emit_pushresult_I4();                                                    \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CEQ_I8_helper(__int64 i, __int64 j) {
    return (j == i);
}
#endif
#endif

#ifndef emit_CEQ_R4
#define emit_CEQ_R4()                             \
{                                                 \
    callInfo.reset();                             \
    emit_tos_arg( 1, INTERNAL_CALL );             \
    emit_tos_arg( 2, INTERNAL_CALL );             \
    emit_callhelper_I4I4_I4(CEQ_R4_helper);       \
    emit_pushresult_I4();                         \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CEQ_R4_helper(int i, int j) {
    if (_isnan((*(float *)&i)) || _isnan((*(float *)&j)))
    {
        return FALSE;
    }
    return ((*(float *)&j) == (*(float *)&i));
}
#endif
#endif

#ifndef emit_CEQ_R8
#define emit_CEQ_R8()                                                        \
{                                                                            \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I4(CEQ_R8_helper);                                  \
    emit_pushresult_I4();                                                    \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CEQ_R8_helper(__int64 i, __int64 j) {
    if (_isnan((*(double *)&j)) || _isnan((*(double *)&i)))
    {
        return FALSE;
    }
    return ((*(double *)&j) == (*(double *)&i));
}
#endif
#endif

/************* CGT ********************/

#ifndef emit_CGT_I4
#define emit_CGT_I4()                             \
{                                                 \
    callInfo.reset();                             \
    emit_tos_arg( 1, INTERNAL_CALL );             \
    emit_tos_arg( 2, INTERNAL_CALL );             \
    emit_callhelper_I4I4_I4(CGT_I4_helper);       \
    emit_pushresult_I4();                         \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CGT_I4_helper(int i, int j) {
    return (j > i);
}
#endif
#endif

#ifndef emit_CGT_I8
#define emit_CGT_I8()                                                        \
{                                                                            \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I4(CGT_I8_helper);                                  \
    emit_pushresult_I4();                                                    \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CGT_I8_helper(__int64 i, __int64 j) {
    return (j > i);
}
#endif
#endif


#ifndef emit_CGT_R4
#define emit_CGT_R4()                             \
{                                                 \
    callInfo.reset();                             \
    emit_tos_arg( 1, INTERNAL_CALL );             \
    emit_tos_arg( 2, INTERNAL_CALL );             \
    emit_callhelper_I4I4_I4(CGT_R4_helper);       \
    emit_pushresult_I4();                         \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CGT_R4_helper(int i, int j) {
    // if either number is NaN return FALSE
    if (_isnan((*(float *)&j)) || _isnan((*(float *)&i)))
    {
        return FALSE;
    }
    return (*(float *)&j) > (*(float *)&i);
}
#endif
#endif

#ifndef emit_CGT_R8
#define emit_CGT_R8()                                                        \
{                                                                            \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I4(CGT_R8_helper);                                  \
    emit_pushresult_I4();                                                    \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CGT_R8_helper(__int64 i, __int64 j) {
    // if either number is NaN return FALSE
    if (_isnan((*(double *)&j)) || _isnan((*(double *)&i)))
    {
        return FALSE;
    }
    return (*(double *)&j) > (*(double *)&i);
}
#endif
#endif

/************* CGT_UN ********************/

#ifndef emit_CGT_UN_I4
#define emit_CGT_UN_I4()                          \
{                                                 \
    callInfo.reset();                             \
    emit_tos_arg( 1, INTERNAL_CALL );             \
    emit_tos_arg( 2, INTERNAL_CALL );             \
    emit_callhelper_I4I4_I4(CGT_U4_helper);       \
    emit_pushresult_I4();                         \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CGT_U4_helper(unsigned int i, unsigned int j) {
    return (j > i);
}
#endif
#endif

#ifndef emit_CGT_UN_I8
#define emit_CGT_UN_I8()                                                     \
{                                                                            \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I4(CGT_U8_helper);                                  \
    emit_pushresult_I4();                                                    \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CGT_U8_helper(unsigned __int64 i, unsigned __int64 j) {
    return (j > i);
}
#endif
#endif

#ifndef emit_CGT_UN_R4
#define emit_CGT_UN_R4()                          \
{                                                 \
    callInfo.reset();                             \
    emit_tos_arg( 1, INTERNAL_CALL );             \
    emit_tos_arg( 2, INTERNAL_CALL );             \
    emit_callhelper_I4I4_I4(CGT_UN_R4_helper);    \
    emit_pushresult_I4();                         \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CGT_UN_R4_helper(int i, int j) {
    // if either number is NaN return TRUE
    if (_isnan((*(float *)&j)) || _isnan((*(float *)&i)))
    {
        return TRUE;
    }
    return (*(float *)&j) > (*(float *)&i);
}
#endif
#endif

#ifndef emit_CGT_UN_R8
#define emit_CGT_UN_R8()                                                     \
{                                                                            \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_R8R8_I4(CGT_UN_R8_helper);                               \
    emit_pushresult_I4();                                                    \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CGT_UN_R8_helper(__int64 i, __int64 j) {
    // if either number is NaN return TRUE
    if (_isnan((*(double *)&j)) || _isnan((*(double *)&i)))
    {
        return TRUE;
    }
    return (*(double *)&j) > (*(double *)&i);
}
#endif
#endif

/************* CLT ********************/

#ifndef emit_CLT_I4
#define emit_CLT_I4()                             \
{                                                 \
    callInfo.reset();                             \
    emit_tos_arg( 1, INTERNAL_CALL );             \
    emit_tos_arg( 2, INTERNAL_CALL );             \
    emit_callhelper_I4I4_I4(CLT_I4_helper);       \
    emit_pushresult_I4();                         \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CLT_I4_helper(int i, int j) {
    return (j < i);
}
#endif
#endif

#ifndef emit_CLT_I8
#define emit_CLT_I8()                                                        \
{                                                                            \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I4(CLT_I8_helper);                                  \
    emit_pushresult_I4();                                                    \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CLT_I8_helper(__int64 i, __int64 j) {
    return (j < i);
}
#endif
#endif

#ifndef emit_CLT_R4
#define emit_CLT_R4()                             \
{                                                 \
    callInfo.reset();                             \
    emit_tos_arg( 1, INTERNAL_CALL );             \
    emit_tos_arg( 2, INTERNAL_CALL );             \
    emit_callhelper_I4I4_I4(CLT_R4_helper);       \
    emit_pushresult_I4();                         \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CLT_R4_helper(int i, int j) {
    // if either number is NaN return FALSE
    if (_isnan((*(float *)&j)) || _isnan((*(float *)&i)))
    {
        return FALSE;
    }
    return (*(float *)&j) < (*(float *)&i);
}
#endif
#endif

#ifndef emit_CLT_R8
#define emit_CLT_R8()                                                        \
{                                                                            \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_R8R8_I4(CLT_R8_helper);                                  \
    emit_pushresult_I4();                                                    \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CLT_R8_helper(__int64 i, __int64 j) {
    // if either number is NaN return FALSE
    if (_isnan((*(double *)&j)) || _isnan((*(double *)&i)))
    {
        return FALSE;
    }
    return (*(double *)&j) < (*(double *)&i);
}
#endif
#endif

/************* CLT_UN ********************/

#ifndef emit_CLT_UN_I4
#define emit_CLT_UN_I4()                          \
{                                                 \
    callInfo.reset();                             \
    emit_tos_arg( 1, INTERNAL_CALL );             \
    emit_tos_arg( 2, INTERNAL_CALL );             \
    emit_callhelper_I4I4_I4(CLT_U4_helper);       \
    emit_pushresult_I4();                         \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CLT_U4_helper(unsigned int i, unsigned int j) {
    return (j < i);
}
#endif
#endif

#ifndef emit_CLT_UN_I8
#define emit_CLT_UN_I8()                                                     \
{                                                                            \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_I8I8_I4(CLT_U8_helper);                                  \
    emit_pushresult_I4();                                                    \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CLT_U8_helper(unsigned __int64 i, unsigned __int64 j) {
    return (j < i);
}
#endif
#endif

#ifndef emit_CLT_UN_R4
#define emit_CLT_UN_R4()                          \
{                                                 \
    callInfo.reset();                             \
    emit_tos_arg( 1, INTERNAL_CALL );             \
    emit_tos_arg( 2, INTERNAL_CALL );             \
    emit_callhelper_I4I4_I4(CLT_UN_R4_helper);    \
    emit_pushresult_I4();                         \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CLT_UN_R4_helper(int i, int j) {
    // if either number is NaN return TRUE
    if (_isnan((*(float *)&j)) || _isnan((*(float *)&i)))
    {
        return TRUE;
    }
    return (*(float *)&j) < (*(float *)&i);
}
#endif
#endif

#ifndef emit_CLT_UN_R8
#define emit_CLT_UN_R8()                                                     \
{                                                                            \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_tos_fixedsize_arg( 1 + NumRegUsed, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_callhelper_R8R8_I4(CLT_UN_R8_helper);                               \
    emit_pushresult_I4();                                                    \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CLT_UN_R8_helper(__int64 i, __int64 j) {
    // if either number is NaN return TRUE
    if (_isnan((*(double *)&j)) || _isnan((*(double *)&i)))
    {
        return TRUE;
    }
    return (*(double *)&j) < (*(double *)&i);
}
#endif
#endif

/****************************************************************************************
 * High level macros for branching IL opcodes:
 *   BEQ, BGE, BGT, BGT.UN, BLE, BLE.UN, BLT, BLT.UN, BNE.UN, BR, BRTRUE, BRFALSE
 * These macros are routers that directly use the macros defined for CEQ, CGT, CGE, CLT, CLE
 *
 * Typically need to be redefined for each platform in order to obtain acceptable
 * performance
 ***************************************************************************************/
// The following four macros are all the same, they have been
// separated out, so we can overwrite some of them with more
// efficient, less portable ones.
#ifndef emit_BR_Common
#define emit_BR_Common(Ctest,Cjmp,Bjmp,JmpOp) \
    Ctest(); \
    emit_testTOS(); \
    JmpOp = Cjmp;
#endif // !emit_BR_Common

#define emit_BR_I4(Ctest,Cjmp,Bjmp,JmpOp) \
    enregisterTOS; \
    pop_register(ARG_1, true); \
    compare_register(ARG_1,TOS_REG_1, Bjmp); \
    inRegTOS = false; \
    JmpOp = Bjmp;

#ifndef emit_BR_I4
#define emit_BR_I4(Ctest,Cjmp,Bjmp,JmpOp) \
    emit_BR_Common(Ctest,Cjmp,Bjmp,JmpOp)
#endif // !emit_BR_I4

#ifndef emit_BR_I8
#define emit_BR_I8(Ctest,Cjmp,Bjmp,JmpOp) \
    emit_BR_Common(Ctest,Cjmp,Bjmp,JmpOp)
#endif // !emit_BR_I8

#ifndef emit_BR_R4
#define emit_BR_R4(Ctest,Cjmp,Bjmp,JmpOp) \
    emit_BR_Common(Ctest,Cjmp,Bjmp,JmpOp)
#endif // !emit_BR_R4

#ifndef emit_BR_R8
#define emit_BR_R8(Ctest,Cjmp,Bjmp,JmpOp) \
    emit_BR_Common(Ctest,Cjmp,Bjmp,JmpOp)
#endif // !emit_BR_R8

#ifndef emit_testTOS        /* Used for BRTRUE and BRFALSE */
#define emit_testTOS()                                 \
    enregisterTOS;                                     \
    inRegTOS = false;                                  \
    and_register(TOS_REG_1,TOS_REG_1, 1);
#endif // !emit_testTOS

#ifndef emit_testTOS_I8     /* Used for BRTRUE and BRFALSE */
#define emit_testTOS_I8()                              \
    callInfo.reset();                                  \
    int NumRegUsed = 0;                                \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(BoolI8ToI4_helper)           \
    and_register(RESULT_1,RESULT_1, 1);
#ifdef DECLARE_HELPERS
int HELPER_CALL BoolI8ToI4_helper(__int64 val) {
        return (val ? 1 : 0);
}
#endif
#endif // !emit_testTOS

/****************************************************************************************
 * High level macros for conversion IL opcodes:
 *   CONV.*, CONV.OVF.*, CONV.OVF.UN.*
 * Typically need to be redefined for each platform in order to obtain acceptable
 * performance
 ***************************************************************************************/

/*****signed convert overflow (CONV_OVF) to I1 ******/
#ifndef emit_CONV_OVF_TOI1_I4
#define emit_CONV_OVF_TOI1_I4()                      \
{                                                    \
    LABELSTACK((outPtr-outBuff), 1);                 \
    callInfo.reset();                                \
    emit_tos_arg( 1, INTERNAL_CALL );                \
    emit_callhelper_I4_I4(CONV_OVF_TOI1_I4_helper);  \
    emit_pushresult_I4();                            \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_OVF_TOI1_I4_helper(int val) {
    char i1 = val ;
    if (val != (int) i1) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (int) i1;
}
#endif
#endif

#ifndef emit_CONV_OVF_TOI1_I8
#define emit_CONV_OVF_TOI1_I8()                                              \
{                                                                            \
    LABELSTACK((outPtr-outBuff), 1);                                         \
    callInfo.reset();                                                        \
    int NumRegUsed = 0;                                                      \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );               \
    emit_callhelper_I8_I4(CONV_OVF_TOI1_I8_helper);                          \
    emit_pushresult_I4();                                                    \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_OVF_TOI1_I8_helper(__int64 val) {
    __int8 x = (__int8) val;
    if ( (__int64) x != val) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (int) x;
}
#endif
#endif

#ifndef emit_CONV_OVF_TOI1_R4
#define emit_CONV_OVF_TOI1_R4()                      \
{                                                    \
    LABELSTACK((outPtr-outBuff), 1);                 \
    callInfo.reset();                                \
    emit_tos_arg( 1, INTERNAL_CALL );                \
    emit_callhelper_I4_I4(CONV_OVF_TOI1_R4_helper);  \
    emit_pushresult_I4();                            \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_OVF_TOI1_R4_helper(int val_i) {
    float val = (*(float *)&val_i);
    if ( _isnan(val) || val >= SCHAR_MAX + 1.0 || val <= SCHAR_MIN - 1.0)
    {   // have we overflowed
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (__int8) val;
}
#endif
#endif

#ifndef emit_CONV_OVF_TOI1_R8
#define emit_CONV_OVF_TOI1_R8()                                \
{                                                              \
    LABELSTACK((outPtr-outBuff), 1);                           \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_OVF_TOI1_R8_helper);            \
    emit_pushresult_I4();                                      \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_OVF_TOI1_R8_helper(__int64 val_i) {
    double val = (*(double *)&val_i);
    if ( _isnan(val) || val >= SCHAR_MAX + 1.0 || val <= SCHAR_MIN - 1.0)
    {   // have we overflowed
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (__int8) val;
}
#endif
#endif

/*****signed convert overflow (CONV_OVF) to I2 ******/

#ifndef emit_CONV_OVF_TOI2_I4
#define emit_CONV_OVF_TOI2_I4()                      \
{                                                    \
    LABELSTACK((outPtr-outBuff), 1);                 \
    callInfo.reset();                                \
    emit_tos_arg( 1, INTERNAL_CALL );                \
    emit_callhelper_I4_I4(CONV_OVF_TOI2_I4_helper);  \
    emit_pushresult_I4();                            \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_OVF_TOI2_I4_helper(int val) {
    signed short i2 = val;
    if (val != (int) i2) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return i2;
}
#endif
#endif

#ifndef emit_CONV_OVF_TOI2_I8
#define emit_CONV_OVF_TOI2_I8()                                \
{                                                              \
    LABELSTACK((outPtr-outBuff), 1);                           \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_OVF_TOI2_I8_helper);            \
    emit_pushresult_I4();                                      \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_OVF_TOI2_I8_helper(__int64 val) {
    __int16 x = (__int16) val;
    if (((__int64) x ) != val) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (int) x;
}
#endif
#endif

#ifndef emit_CONV_OVF_TOI2_R4
#define emit_CONV_OVF_TOI2_R4()                      \
{                                                    \
    LABELSTACK((outPtr-outBuff), 1);                 \
    callInfo.reset();                                \
    emit_tos_arg( 1, INTERNAL_CALL );                \
    emit_callhelper_I4_I4(CONV_OVF_TOI2_R4_helper);  \
    emit_pushresult_I4();                            \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_OVF_TOI2_R4_helper(int val_i) {
    float val = (*(float *)&val_i);
    if ( _isnan(val) || val >= SHRT_MAX + 1.0 || val <= SHRT_MIN - 1.0)
    {   // have we overflowed
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (__int16) val;
}
#endif
#endif

#ifndef emit_CONV_OVF_TOI2_R8
#define emit_CONV_OVF_TOI2_R8()                                \
{                                                              \
    LABELSTACK((outPtr-outBuff), 1);                           \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_OVF_TOI2_R8_helper);            \
    emit_pushresult_I4();                                      \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_OVF_TOI2_R8_helper(__int64 val_i) {
    double val = (*(double *)&val_i);
    if ( _isnan(val) || val >= SHRT_MAX + 1.0 || val <= SHRT_MIN - 1.0)
    {   // have we overflowed
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (__int16) val;
}
#endif
#endif

/*****signed convert overflow (CONV_OVF) to I4 ******/
#define emit_CONV_OVF_TOI4_I4() /* do nothing */

#ifndef emit_CONV_OVF_TOI4_I8
#define emit_CONV_OVF_TOI4_I8()                                \
{                                                              \
    LABELSTACK((outPtr-outBuff), 1);                           \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_OVF_TOI4_I8_helper);            \
    emit_pushresult_I4();                                      \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_OVF_TOI4_I8_helper(signed __int64 val) {
    int i4 = (int) val;
    if (val != (signed __int64) i4) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return i4;
}
#endif
#endif

#ifndef emit_CONV_OVF_TOI4_R4
#define emit_CONV_OVF_TOI4_R4()                      \
{                                                    \
    LABELSTACK((outPtr-outBuff), 1);                 \
    callInfo.reset();                                \
    emit_tos_arg( 1, INTERNAL_CALL );                \
    emit_callhelper_I4_I4(CONV_OVF_TOI4_R4_helper);  \
    emit_pushresult_I4();                            \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_OVF_TOI4_R4_helper(int val_i) {
    float val = (*(float *)&val_i);
    if ( _isnan(val) || val >= INT_MAX + 1.0 || val <= INT_MIN - 1.0)
    {   // have we overflowed
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (__int32) val;
}
#endif
#endif

#ifndef emit_CONV_OVF_TOI4_R8
#define emit_CONV_OVF_TOI4_R8()                                \
{                                                              \
    LABELSTACK((outPtr-outBuff), 1);                           \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_OVF_TOI4_R8_helper);            \
    emit_pushresult_I4();                                      \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_OVF_TOI4_R8_helper(__int64 val_i) {
    double val = (*(double *)&val_i);
    if ( _isnan(val) || val >= INT_MAX + 1.0 || val <= INT_MIN - 1.0)
        {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (__int32) val;
}
#endif
#endif

/*****signed convert overflow (CONV_OVF) to I8 ******/
#define emit_CONV_OVF_TOI8_I4() emit_CONV_TOI8_I4()

#define emit_CONV_OVF_TOI8_I8() /* do nothing */

#ifndef emit_CONV_OVF_TOI8_R4
#define emit_CONV_OVF_TOI8_R4()                      \
{                                                    \
    LABELSTACK((outPtr-outBuff), 1);                 \
    callInfo.reset();                                \
    emit_tos_arg( 1, INTERNAL_CALL );                \
    emit_callhelper_I4_I8(CONV_OVF_TOI8_R4_helper);  \
    emit_pushresult_I8();                            \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL CONV_OVF_TOI8_R4_helper(int val_i) {
    float val = (*(float *)&val_i);
    if ( _isnan(val)) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    else {
        // this code is shared by fncbind.cpp
        if (val < I64(-0x7000000000000000)) {
            double tmp = val - I64(-0x7000000000000000);
            if (tmp > I64(-0x7000000000000000) &&
                (__int64)tmp > I64(-0x1000000000000001)) {
                goto success;
            }
        }
        else {
            if (val > I64(0x7000000000000000)) {
                double tmp = val - I64(0x7000000000000000);
                if (tmp < I64(0x7000000000000000) &&
                    (__int64)tmp < I64(0x1000000000000000)) {
                    goto success;
                }
            }
            else {
                goto success;
            }
        }
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
 success:
    return (__int64) val;
}
#endif
#endif

#ifndef emit_CONV_OVF_TOI8_R8
#define emit_CONV_OVF_TOI8_R8()                                \
{                                                              \
    LABELSTACK((outPtr-outBuff), 1);                           \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I8(CONV_OVF_TOI8_R8_helper);            \
    emit_pushresult_I8();                                      \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL CONV_OVF_TOI8_R8_helper(__int64 val_i) {
    double val = (*(double *)&val_i);
    if ( _isnan(val)) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    else {
        // this code is shared by fncbind.cpp
        if (val < I64(-0x7000000000000000)) {
            double tmp = val - I64(-0x7000000000000000);
            if (tmp > I64(-0x7000000000000000) &&
                (__int64)tmp > I64(-0x1000000000000001)) {
                goto success;
            }
        }
        else {
            if (val > I64(0x7000000000000000)) {
                double tmp = val - I64(0x7000000000000000);
                if (tmp < I64(0x7000000000000000) &&
                    (__int64)tmp < I64(0x1000000000000000)) {
                    goto success;
                }
            }
            else {
                goto success;
            }
        }
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
 success:
    return (__int64) val;
}
#endif
#endif

/*****signed convert overflow (CONV_OVF) to U1 ******/
#ifndef emit_CONV_OVF_TOU1_I4
#define emit_CONV_OVF_TOU1_I4()         \
{                                                    \
    LABELSTACK((outPtr-outBuff), 1);                 \
    callInfo.reset();                                \
    emit_tos_arg( 1, INTERNAL_CALL );                \
    emit_callhelper_I4_I4(CONV_OVF_TOU1_U4_helper);  \
    emit_pushresult_U4();                            \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_OVF_TOU1_U4_helper(unsigned int val) {
    if (val > 0xff) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return val;
}
#endif
#endif

#ifndef emit_CONV_OVF_TOU1_I8
#define emit_CONV_OVF_TOU1_I8()                                \
{                                                              \
    LABELSTACK((outPtr-outBuff), 1);                           \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_OVF_TOU1_I8_helper);            \
    emit_pushresult_I4();                                      \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_OVF_TOU1_I8_helper(__int64 val) {
    unsigned __int8 x = (unsigned __int8) val;
    if ((__int64) x != val) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (int) x;
}
#endif
#endif

#ifndef emit_CONV_OVF_TOU1_R4
#define emit_CONV_OVF_TOU1_R4()         \
{                                                    \
    LABELSTACK((outPtr-outBuff), 1);                 \
    callInfo.reset();                                \
    emit_tos_arg( 1, INTERNAL_CALL );                \
    emit_callhelper_I4_I4(CONV_OVF_TOU1_R4_helper);  \
    emit_pushresult_U4();                            \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_OVF_TOU1_R4_helper(int val_i) {
    float val = (*(float *)&val_i);
    if ( _isnan(val) || val <= -1.0 || val >= UCHAR_MAX + 1.0)
    {   // have we overflowed
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (__int8) val;
}
#endif
#endif

#ifndef emit_CONV_OVF_TOU1_R8
#define emit_CONV_OVF_TOU1_R8()                                \
{                                                              \
    LABELSTACK((outPtr-outBuff), 1);                           \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_OVF_TOU1_R8_helper);            \
    emit_pushresult_U4();                                      \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_OVF_TOU1_R8_helper(__int64 val_i) {
    double val = (*(double *)&val_i);
    if ( _isnan(val) || val <= -1.0 || val >= UCHAR_MAX + 1.0 )
    {   // have we overflowed
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (__int8) val;
}
#endif
#endif

/*****signed convert overflow (CONV_OVF) to U2 ******/
#ifndef emit_CONV_OVF_TOU2_I4
#define emit_CONV_OVF_TOU2_I4()                      \
{                                                    \
    LABELSTACK((outPtr-outBuff), 1);                 \
    callInfo.reset();                                \
    emit_tos_arg( 1, INTERNAL_CALL );                \
    emit_callhelper_I4_I4(CONV_OVF_TOU2_U4_helper);  \
    emit_pushresult_U4();                            \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_OVF_TOU2_U4_helper(unsigned int val) {
    if (val > 0xffff) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return val;
}
#endif
#endif

#ifndef emit_CONV_OVF_TOU2_I8
#define emit_CONV_OVF_TOU2_I8()                                \
{                                                              \
    LABELSTACK((outPtr-outBuff), 1);                           \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_OVF_TOU2_I8_helper);            \
    emit_pushresult_I4();                                      \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_OVF_TOU2_I8_helper(__int64 val) {
    unsigned __int16 x = (unsigned __int16) val;
    if ((__int64) x != val) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (int) x;
}
#endif
#endif

#ifndef emit_CONV_OVF_TOU2_R4
#define emit_CONV_OVF_TOU2_R4()                      \
{                                                    \
    LABELSTACK((outPtr-outBuff), 1);                 \
    callInfo.reset();                                \
    emit_tos_arg( 1, INTERNAL_CALL );                \
    emit_callhelper_I4_I4(CONV_OVF_TOU2_R4_helper);  \
    emit_pushresult_U4();                            \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_OVF_TOU2_R4_helper(int val_i) {
    float val = (*(float *)&val_i);
    if ( _isnan(val) || val <= -1.0 || val >= USHRT_MAX + 1.0)
    {   // have we overflowed
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (__int16) val;
}
#endif
#endif

#ifndef emit_CONV_OVF_TOU2_R8
#define emit_CONV_OVF_TOU2_R8()                                \
{                                                              \
    LABELSTACK((outPtr-outBuff), 1);                           \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_OVF_TOU2_R8_helper);            \
    emit_pushresult_U4();                                      \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_OVF_TOU2_R8_helper(__int64 val_i) {
    double val = (*(double *)&val_i);
    if ( _isnan(val) || val <= -1.0 || val >= USHRT_MAX + 1.0 )
    {   // have we overflowed
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (__int16) val;
}
#endif
#endif

/*****signed convert overflow (CONV_OVF) to U4 ******/
#ifndef emit_CONV_OVF_TOU4_I4
#define emit_CONV_OVF_TOU4_I4()                      \
{                                                    \
    LABELSTACK((outPtr-outBuff), 1);                 \
    callInfo.reset();                                \
    emit_tos_arg( 1, INTERNAL_CALL );                \
    emit_callhelper_I4_I4(CONV_OVF_TOU4_I4_helper);  \
    emit_pushresult_U4();                            \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_OVF_TOU4_I4_helper(int val) {
    if (val < 0) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return val;
}
#endif
#endif

#ifndef emit_CONV_OVF_TOU4_I8
#define emit_CONV_OVF_TOU4_I8()                                \
{                                                              \
    LABELSTACK((outPtr-outBuff), 1);                           \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_OVF_TOU4_U8_helper);            \
    emit_pushresult_U4();                                      \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_OVF_TOU4_U8_helper(unsigned __int64 val) {
    if (val > 0xffffffff) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (unsigned __int32) val;
}
#endif
#endif

#ifndef emit_CONV_OVF_TOU4_R4
#define emit_CONV_OVF_TOU4_R4()                      \
{                                                    \
    LABELSTACK((outPtr-outBuff), 1);                 \
    callInfo.reset();                                \
    emit_tos_arg( 1, INTERNAL_CALL );                \
    emit_callhelper_I4_I4(CONV_OVF_TOU4_R4_helper);  \
    emit_pushresult_U4();                            \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_OVF_TOU4_R4_helper(int val_i) {
    float val = (*(float *)&val_i);
    if ( _isnan(val) || val <= -1.0 || val >= UINT_MAX + 1.0)
    {   // have we overflowed
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
        __int64 valI8 = (__int64) val;
    return (unsigned __int32) valI8;
}
#endif
#endif

#ifndef emit_CONV_OVF_TOU4_R8
#define emit_CONV_OVF_TOU4_R8()                                \
{                                                              \
    LABELSTACK((outPtr-outBuff), 1);                           \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_R8_I4(CONV_OVF_TOU4_R8_helper);            \
    emit_pushresult_U4();                                      \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_OVF_TOU4_R8_helper(__int64 val_i) {
    double val = (*(double *)&val_i);
    if ( _isnan(val) || val <= -1.0 || val >= UINT_MAX + 1.0)
        {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
        __int64 valI8 = (__int64) val;
    return (unsigned __int32) valI8;
}
#endif
#endif

/*****signed convert overflow (CONV_OVF) to U8 ******/
#ifndef emit_CONV_OVF_TOU8_I4
#define emit_CONV_OVF_TOU8_I4()                      \
{                                                    \
    LABELSTACK((outPtr-outBuff), 1);                 \
    callInfo.reset();                                \
    emit_tos_arg( 1, INTERNAL_CALL );                \
    emit_callhelper_I4_I8(CONV_OVF_TOU8_I4_helper);  \
    emit_pushresult_U8();                            \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL CONV_OVF_TOU8_I4_helper(signed __int32 val) {
    if (val < 0) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return val;
}
#endif
#endif

#ifndef emit_CONV_OVF_TOU8_I8
#define emit_CONV_OVF_TOU8_I8()                                \
{                                                              \
    LABELSTACK((outPtr-outBuff), 1);                           \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I8(CONV_OVF_TOU8_I8_helper);            \
    emit_pushresult_U8();                                      \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL CONV_OVF_TOU8_I8_helper(signed __int64 val) {
    if (val < 0) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return val;
}
#endif
#endif

#ifndef emit_CONV_OVF_TOU8_R4
#define emit_CONV_OVF_TOU8_R4()                      \
{                                                    \
    LABELSTACK((outPtr-outBuff), 1);                 \
    callInfo.reset();                                \
    emit_tos_arg( 1, INTERNAL_CALL );                \
    emit_callhelper_I4_I8(CONV_OVF_TOU8_R4_helper);  \
    emit_pushresult_U8();                            \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL CONV_OVF_TOU8_R4_helper(int val_i) {
    float val = (*(float *)&val_i);
    if (_isnan(val)) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    else {
        // this code is shared by fncbind.cpp
        if (val < UI64(0xF000000000000000)) {
            if (val > -1) {
                goto success;
            }
        }
        else {
            double tmp = val - UI64(0xF000000000000000);
            if (tmp < I64(0x7000000000000000) &&
                (__int64)tmp < I64(0x1000000000000000)) {
                goto success;
            }
        }
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
 success:
    if (val <= INT64_MAX)
        return (unsigned __int64) val;

    // subtract 0x8000000000000000, do the convert then add it back again
    val = (val - (float) (2147483648.0 * 4294967296.0));
    return(((unsigned __int64) val) + I64(0x8000000000000000));
}
#endif
#endif

#ifndef emit_CONV_OVF_TOU8_R8
#define emit_CONV_OVF_TOU8_R8()                                \
{                                                              \
    LABELSTACK((outPtr-outBuff), 1);                           \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I8(CONV_OVF_TOU8_R8_helper);            \
    emit_pushresult_I8();                                      \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL CONV_OVF_TOU8_R8_helper(__int64 val_i) {
    double val = (*(double *)&val_i);
    if (_isnan(val)) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    else {
        // this code is shared by fncbind.cpp
        if (val < UI64(0xF000000000000000)) {
            if (val > -1) {
                goto success;
            }
        }
        else {
            double tmp = val - UI64(0xF000000000000000);
            if (tmp < I64(0x7000000000000000) &&
                (__int64)tmp < I64(0x1000000000000000)) {
                goto success;
            }
        }
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
 success:

    if (val <= INT64_MAX)
        return (unsigned __int64) val;

    // subtract 0x8000000000000000, do the convert then add it back again
    val = (val - (2147483648.0 * 4294967296.0));
    return(((unsigned __int64) val) + I64(0x8000000000000000));
}
#endif
#endif

/****************************************************************
// Support for Precise R
*****************************************************************/
#ifndef emit_CONV_OVF_TOI1_R
#define emit_CONV_OVF_TOI1_R()         \
    emit_conv_RtoR4(); \
    emit_CONV_OVF_TOI1_R4();
#endif

#ifndef emit_CONV_OVF_TOI2_R
#define emit_CONV_OVF_TOI2_R()         \
    emit_conv_RtoR4(); \
    emit_CONV_OVF_TOI2_R4();
#endif

#ifndef emit_CONV_OVF_TOI4_R
#define emit_CONV_OVF_TOI4_R()         \
    emit_conv_RtoR4(); \
    emit_CONV_OVF_TOI4_R4();
#endif

#ifndef emit_CONV_OVF_TOI8_R
#define emit_CONV_OVF_TOI8_R()         \
    emit_conv_RtoR8(); \
    emit_CONV_OVF_TOI8_R8();
#endif

#ifndef emit_CONV_OVF_TOU1_R
#define emit_CONV_OVF_TOU1_R()         \
    emit_conv_RtoR4(); \
    emit_CONV_OVF_TOU1_R4();
#endif

#ifndef emit_CONV_OVF_TOU2_R
#define emit_CONV_OVF_TOU2_R()         \
    emit_conv_RtoR4(); \
    emit_CONV_OVF_TOU2_R4();
#endif

#ifndef emit_CONV_OVF_TOU4_R
#define emit_CONV_OVF_TOU4_R()         \
    emit_conv_RtoR4(); \
    emit_CONV_OVF_TOU4_R4();
#endif

#ifndef emit_CONV_OVF_TOU8_R
#define emit_CONV_OVF_TOU8_R()         \
    emit_conv_RtoR8(); \
    emit_CONV_OVF_TOU8_R8();
#endif

#ifndef emit_CONV_TOI1_R
#define emit_CONV_TOI1_R()                 \
    emit_conv_RtoR8(); \
    emit_callhelper_R8_I4(CONV_TOI1_R8_helper);  \
    emit_pushresult_I4()
#endif

#ifndef emit_CONV_TOI2_R
#define emit_CONV_TOI2_R()                 \
    emit_conv_RtoR8(); \
    emit_callhelper_R8_I4(CONV_TOI2_R8_helper);  \
    emit_pushresult_I4()
#endif

#ifndef emit_CONV_TOI4_R
#define emit_CONV_TOI4_R()                 \
    emit_conv_RtoR8() ; \
    emit_callhelper_R8_I4(CONV_TOI4_R8_helper);  \
    emit_pushresult_I4()
#endif

#ifndef emit_CONV_TOI8_R
#define emit_CONV_TOI8_R()                 \
    emit_conv_RtoR8(); \
    emit_callhelper_R8_I8(CONV_TOI8_R8_helper);  \
    emit_pushresult_I8()
#endif


#ifndef emit_CONV_TOU1_R
#define emit_CONV_TOU1_R()                 \
    emit_conv_RtoR8(); \
    emit_callhelper_R8_I4(CONV_TOU1_R8_helper);  \
    emit_pushresult_I4()
#endif

#ifndef emit_CONV_TOU2_R
#define emit_CONV_TOU2_R()                 \
    emit_conv_RtoR8(); \
    emit_callhelper_R8_I4(CONV_TOU2_R8_helper);  \
    emit_pushresult_I4()
#endif

/*****unsigned convert overflow (CONV_OVF_UN) to U1 ******/
#ifndef emit_CONV_OVF_UN_TOI1_I4
#define emit_CONV_OVF_UN_TOI1_I4()                      \
{                                                       \
    LABELSTACK((outPtr-outBuff), 1);                    \
    callInfo.reset();                                   \
    emit_tos_arg( 1, INTERNAL_CALL );                   \
    emit_callhelper_I4_I4(CONV_OVF_UN_TOI1_I4_helper);  \
    emit_pushresult_I4();                               \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_OVF_UN_TOI1_I4_helper(unsigned int val) {
    if (val > 0x7F) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (__int8) val;
}
#endif
#endif

#ifndef emit_CONV_OVF_UN_TOI1_I8
#define emit_CONV_OVF_UN_TOI1_I8()                             \
{                                                              \
    LABELSTACK((outPtr-outBuff), 1);                           \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_OVF_UN_TOI1_I8_helper);         \
    emit_pushresult_I4();                                      \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_OVF_UN_TOI1_I8_helper(unsigned __int64 val) {
    if (val > 0x7F) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (__int8) val;
}
#endif
#endif

#define emit_CONV_OVF_UN_TOI1_R4() emit_CONV_OVF_TOI1_R4()
#define emit_CONV_OVF_UN_TOI1_R8() emit_CONV_OVF_TOI1_R8()
#define emit_CONV_OVF_UN_TOI1_R() emit_CONV_OVF_TOI1_R()

/*****unsigned convert overflow (CONV_OVF_UN) to U2 ******/
#ifndef emit_CONV_OVF_UN_TOI2_I4
#define emit_CONV_OVF_UN_TOI2_I4()                      \
{                                                       \
    LABELSTACK((outPtr-outBuff), 1);                    \
    callInfo.reset();                                   \
    emit_tos_arg( 1, INTERNAL_CALL );                   \
    emit_callhelper_I4_I4(CONV_OVF_UN_TOI2_I4_helper);  \
    emit_pushresult_I4();                               \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_OVF_UN_TOI2_I4_helper(unsigned int val) {
    if (val > 0x7FFF) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (__int16) val;
}
#endif
#endif

#ifndef emit_CONV_OVF_UN_TOI2_I8
#define emit_CONV_OVF_UN_TOI2_I8()                             \
{                                                              \
    LABELSTACK((outPtr-outBuff), 1);                           \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_OVF_UN_TOI2_I8_helper);         \
    emit_pushresult_I4();                                      \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_OVF_UN_TOI2_I8_helper(unsigned __int64 val) {
    if (val > 0x7FFF) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (__int16) val;
}
#endif
#endif

#define emit_CONV_OVF_UN_TOI2_R4() emit_CONV_OVF_TOI2_R4()
#define emit_CONV_OVF_UN_TOI2_R8() emit_CONV_OVF_TOI2_R8()
#define emit_CONV_OVF_UN_TOI2_R()  emit_CONV_OVF_TOI2_R()

/*****unsigned convert overflow (CONV_OVF_UN) to U4 ******/
#ifndef emit_CONV_OVF_UN_TOI4_I4
#define emit_CONV_OVF_UN_TOI4_I4()                      \
{                                                       \
    LABELSTACK((outPtr-outBuff), 1);                    \
    callInfo.reset();                                   \
    emit_tos_arg( 1, INTERNAL_CALL );                   \
    emit_callhelper_I4_I4(CONV_OVF_UN_TOI4_I4_helper);  \
    emit_pushresult_I4();                               \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_OVF_UN_TOI4_I4_helper(unsigned int val) {
    if (val > 0x7FFFFFFF) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (__int32) val;
}
#endif
#endif

#ifndef emit_CONV_OVF_UN_TOI4_I8
#define emit_CONV_OVF_UN_TOI4_I8()                             \
{                                                              \
    LABELSTACK((outPtr-outBuff), 1);                           \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_OVF_UN_TOI4_I8_helper);         \
    emit_pushresult_I4();                                      \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_OVF_UN_TOI4_I8_helper(unsigned __int64 val) {
    if (val > 0x7FFFFFFF) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (__int32) val;
}
#endif
#endif

#define emit_CONV_OVF_UN_TOI4_R4() emit_CONV_OVF_TOI4_R4()
#define emit_CONV_OVF_UN_TOI4_R8() emit_CONV_OVF_TOI4_R8()
#define emit_CONV_OVF_UN_TOI4_R() emit_CONV_OVF_TOI4_R()

/*****unsigned convert overflow (CONV_OVF_UN) to I8 ******/
#define emit_CONV_OVF_UN_TOI8_I4() emit_CONV_TOU8_I4()

#ifndef emit_CONV_OVF_UN_TOI8_I8
#define emit_CONV_OVF_UN_TOI8_I8()                             \
{                                                              \
    LABELSTACK((outPtr-outBuff), 1);                           \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I8(CONV_OVF_UN_TOI8_I8_helper);         \
    emit_pushresult_I8();                                      \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL CONV_OVF_UN_TOI8_I8_helper(signed __int64 val) {     /* note SIGNED value */
    if (val < 0) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return val;
}
#endif
#endif

#define emit_CONV_OVF_UN_TOI8_R4() emit_CONV_OVF_TOI8_R4()
#define emit_CONV_OVF_UN_TOI8_R8() emit_CONV_OVF_TOI8_R8()
#define emit_CONV_OVF_UN_TOI8_R() emit_CONV_OVF_TOI8_R()

/*****unsigned convert overflow (CONV_OVF_UN) to U1 ******/
#ifndef emit_CONV_OVF_UN_TOU1_I4
#define emit_CONV_OVF_UN_TOU1_I4()                      \
{                                                       \
    LABELSTACK((outPtr-outBuff), 1);                    \
    callInfo.reset();                                   \
    emit_tos_arg( 1, INTERNAL_CALL );                   \
    emit_callhelper_I4_I4(CONV_OVF_UN_TOU1_U4_helper);  \
    emit_pushresult_U4();                               \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_OVF_UN_TOU1_U4_helper(unsigned int val) {
    if (val > 0xff) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (unsigned __int8) val;
}
#endif
#endif

#ifndef emit_CONV_OVF_UN_TOU1_I8
#define emit_CONV_OVF_UN_TOU1_I8()                             \
{                                                              \
    LABELSTACK((outPtr-outBuff), 1);                           \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_OVF_UN_TOU1_I8_helper);         \
    emit_pushresult_I4();                                      \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_OVF_UN_TOU1_I8_helper(unsigned __int64 val) {
    if (val > 0xff) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (unsigned __int8) val;
}
#endif
#endif

#define emit_CONV_OVF_UN_TOU1_R4() emit_CONV_OVF_TOU1_R4()
#define emit_CONV_OVF_UN_TOU1_R8() emit_CONV_OVF_TOU1_R8()
#define emit_CONV_OVF_UN_TOU1_R() emit_CONV_OVF_TOU1_R()

/*****unsigned convert overflow (CONV_OVF_UN) to U2 ******/
#ifndef emit_CONV_OVF_UN_TOU2_I4
#define emit_CONV_OVF_UN_TOU2_I4()                      \
{                                                       \
    LABELSTACK((outPtr-outBuff), 1);                    \
    callInfo.reset();                                   \
    emit_tos_arg( 1, INTERNAL_CALL );                   \
    emit_callhelper_I4_I4(CONV_OVF_UN_TOU2_I4_helper);  \
    emit_pushresult_U4();                               \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_OVF_UN_TOU2_I4_helper(unsigned int val) {
    if (val > 0xffff) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (unsigned __int16) val;
}
#endif
#endif

#ifndef emit_CONV_OVF_UN_TOU2_I8
#define emit_CONV_OVF_UN_TOU2_I8()                             \
{                                                              \
    LABELSTACK((outPtr-outBuff), 1);                           \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_OVF_UN_TOU2_I8_helper);         \
    emit_pushresult_I4();                                      \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_OVF_UN_TOU2_I8_helper(unsigned __int64 val) {
    if (val > 0xffff) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (unsigned __int16) val;
}
#endif
#endif

#define emit_CONV_OVF_UN_TOU2_R4() emit_CONV_OVF_TOU2_R4()
#define emit_CONV_OVF_UN_TOU2_R8() emit_CONV_OVF_TOU2_R8()
#define emit_CONV_OVF_UN_TOU2_R() emit_CONV_OVF_TOU2_R()

/*****unsigned convert overflow (CONV_OVF_UN) to U4 ******/

#define emit_CONV_OVF_UN_TOU4_I4() /* do nothing */

#ifndef emit_CONV_OVF_UN_TOU4_I8
#define emit_CONV_OVF_UN_TOU4_I8()                             \
{                                                              \
    LABELSTACK((outPtr-outBuff), 1);                           \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_OVF_UN_TOU4_U8_helper);         \
    emit_pushresult_U4();                                      \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_OVF_UN_TOU4_U8_helper(unsigned __int64 val) {
    if (val > 0xffffffff) {
        THROW_FROM_HELPER_RET(CORINFO_OverflowException);
    }
    return (unsigned __int32) val;
}
#endif
#endif

#define emit_CONV_OVF_UN_TOU4_R4() emit_CONV_OVF_TOU4_R4()
#define emit_CONV_OVF_UN_TOU4_R8() emit_CONV_OVF_TOU4_R8()
#define emit_CONV_OVF_UN_TOU4_R() emit_CONV_OVF_TOU4_R()

/*****unsigned convert overflow (CONV_OVF_UN) to U8 ******/
#define emit_CONV_OVF_UN_TOU8_I4() emit_CONV_TOU8_I4()
#define emit_CONV_OVF_UN_TOU8_I8()      /* do nothing */

#define emit_CONV_OVF_UN_TOU8_R4() emit_CONV_OVF_TOU8_R4()
#define emit_CONV_OVF_UN_TOU8_R8() emit_CONV_OVF_TOU8_R8()
#define emit_CONV_OVF_UN_TOU8_R() emit_CONV_OVF_TOU8_R()

/******************************************************************/
/*****************************************************************/

/***** convert  (no overflow) (CONV) to I1 ******/
#ifndef emit_CONV_TOI1_I4
#define emit_CONV_TOI1_I4()                      \
{                                                \
    callInfo.reset();                            \
    emit_tos_arg( 1, INTERNAL_CALL );            \
    emit_callhelper_I4_I4(CONV_TOI1_I4_helper);  \
    emit_pushresult_I4();                        \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_TOI1_I4_helper(int val) {
    return (int) ((char) val);
}
#endif
#endif

#ifndef emit_CONV_TOI1_I8
#define emit_CONV_TOI1_I8()                                    \
{                                                              \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_TOI1_I8_helper);                \
    emit_pushresult_I4();                                      \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_TOI1_I8_helper(__int64 val) {
    return (int) ((char) val);
}
#endif
#endif

#ifndef emit_CONV_TOI1_R4
#define emit_CONV_TOI1_R4()                      \
{                                                \
    callInfo.reset();                            \
    emit_tos_arg( 1, INTERNAL_CALL );            \
    emit_callhelper_I4_I4(CONV_TOI1_R4_helper);  \
    emit_pushresult_I4();                        \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_TOI1_R4_helper(int val) {
    int x = (int)(*(float *)&val);
    return (int) ((char) x);
}
#endif
#endif

#ifndef emit_CONV_TOI1_R8
#define emit_CONV_TOI1_R8()                                    \
{                                                              \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_TOI1_R8_helper);                \
    emit_pushresult_I4();                                      \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_TOI1_R8_helper(__int64 val) {
    int x = (int)(*(double *)&val);
    return (int) ((char) x);
}
#endif
#endif

/***** convert  (no overflow) (CONV) to I2 ******/
#ifndef emit_CONV_TOI2_I4
#define emit_CONV_TOI2_I4()                      \
{                                                \
    callInfo.reset();                            \
    emit_tos_arg( 1, INTERNAL_CALL );            \
    emit_callhelper_I4_I4(CONV_TOI2_I4_helper);  \
    emit_pushresult_I4();                        \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_TOI2_I4_helper(int val) {
    return (int) ((short) val);
}
#endif
#endif

#ifndef emit_CONV_TOI2_I8
#define emit_CONV_TOI2_I8()                                    \
{                                                              \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_TOI2_I8_helper);                \
    emit_pushresult_I4();                                      \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_TOI2_I8_helper(__int64 val) {
    return (int) ((short) val);
}
#endif
#endif

#ifndef emit_CONV_TOI2_R4
#define emit_CONV_TOI2_R4()                      \
{                                                \
    callInfo.reset();                            \
    emit_tos_arg( 1, INTERNAL_CALL );            \
    emit_callhelper_I4_I4(CONV_TOI2_R4_helper);  \
    emit_pushresult_I4();                        \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_TOI2_R4_helper(int val) {
    int x = (int)(*(float *)&val);
    return (int) ((short) x);
}
#endif
#endif

#ifndef emit_CONV_TOI2_R8
#define emit_CONV_TOI2_R8()                                    \
{                                                              \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_TOI2_R8_helper);                \
    emit_pushresult_I4();                                      \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_TOI2_R8_helper(__int64 val) {
    int x = (int)(*(double *)&val);
    return (int) ((short) x);
}
#endif
#endif

/***** convert  (no overflow) (CONV) to I4 ******/
#define emit_CONV_TOI4_I4()     /* do nothing */

#ifndef emit_CONV_TOI4_I8
#define emit_CONV_TOI4_I8()                                    \
{                                                              \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_TOI4_I8_helper);                \
    emit_pushresult_I4();                                      \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_TOI4_I8_helper(__int64 val) {
    return (__int32) val;
}
#endif
#endif

#ifndef emit_CONV_TOI4_R4
#define emit_CONV_TOI4_R4()                      \
{                                                \
    callInfo.reset();                            \
    emit_tos_arg( 1, INTERNAL_CALL );            \
    emit_callhelper_I4_I4(CONV_TOI4_R4_helper);  \
    emit_pushresult_I4();                        \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_TOI4_R4_helper(int val) {
    return (__int32)(*(float *)&val);
}
#endif
#endif

#ifndef emit_CONV_TOI4_R8
#define emit_CONV_TOI4_R8()                                    \
{                                                              \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_TOI4_R8_helper);                \
    emit_pushresult_I4();                                      \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL CONV_TOI4_R8_helper(__int64 val) {
    return (__int32)(*(double *)&val);
}
#endif
#endif

/***** convert (no overflow) (CONV) to I8 ******/
#ifndef emit_CONV_TOI8_I4
#define emit_CONV_TOI8_I4()                      \
{                                                \
    callInfo.reset();                            \
    emit_tos_arg( 1, INTERNAL_CALL );            \
    emit_callhelper_I4_I8(CONV_TOI8_I4_helper);  \
    emit_pushresult_I8();                        \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL CONV_TOI8_I4_helper(int val) {
    return val;
}
#endif
#endif

#define emit_CONV_TOI8_I8()     /* do nothing */

#ifndef emit_CONV_TOI8_R4
#define emit_CONV_TOI8_R4()                      \
{                                                \
    callInfo.reset();                            \
    emit_tos_arg( 1, INTERNAL_CALL );            \
    emit_callhelper_I4_I8(CONV_TOI8_R4_helper);  \
    emit_pushresult_I8();                        \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL CONV_TOI8_R4_helper(int val) {
    return (__int64)(*(float *)&val);
}
#endif
#endif

#ifndef emit_CONV_TOI8_R8
#define emit_CONV_TOI8_R8()                                    \
{                                                              \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I8(CONV_TOI8_R8_helper);                \
    emit_pushresult_I8();                                      \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL CONV_TOI8_R8_helper(__int64 val) {
    return (__int64)(*(double *)&val);
}
#endif
#endif

/***** convert (no overflow) (CONV) to R4 ******/
#ifdef DECLARE_HELPERS
/* this routine insures that a float is truncated to float
   precision.  We do this by forcing the memory spill  */
float truncateToFloat(float f) {
        return(f);
}

/* Assign this function to a global to prevent the compiler from
   optimizing it away  */
float (*g_truncateToFloat)(float f) = truncateToFloat;
#endif

#ifndef emit_CONV_TOR4_I4
#define emit_CONV_TOR4_I4()                      \
{                                                \
    callInfo.reset();                            \
    emit_tos_arg( 1, INTERNAL_CALL );            \
    emit_callhelper_I4_I8(CONV_TOR4_I4_helper);  \
    emit_pushresult_I8();                        \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL CONV_TOR4_I4_helper(int val) {
    double result = (double) g_truncateToFloat((float) val);
    return *(__int64*)&result;
}
#endif
#endif

#ifndef emit_CONV_TOR4_I8
#define emit_CONV_TOR4_I8()                                    \
{                                                              \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_TOR4_I8_helper);                \
    emit_pushresult_I8();                                      \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL CONV_TOR4_I8_helper(__int64 val) {
    double result = (double) g_truncateToFloat((float) val);
    return *(__int64*)&result;
}
#endif
#endif

//#define emit_CONV_TOR4_R4()     /* do nothing */

#define emit_CONV_TOR4_R8() {\
        emit_conv_R8toR4(); \
        emit_conv_R4toR8(); \
}


#ifndef emit_CONV_TOR4_R8
#define emit_CONV_TOR4_R8()                                    \
{                                                              \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I8(CONV_TOR4_R8_helper);                \
    emit_pushresult_I8();                                      \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL CONV_TOR4_R8_helper(__int64 val) {
    double result = (double)g_truncateToFloat((float) (*(double *)&val));
    return *(__int64*)&result;
}
#endif
#endif

/***** convert (no overflow) (CONV) to R8 ******/
#ifndef emit_CONV_TOR8_I4
#define emit_CONV_TOR8_I4()                      \
{                                                \
    callInfo.reset();                            \
    emit_tos_arg( 1, INTERNAL_CALL );            \
    emit_callhelper_I4_I8(CONV_TOR8_I4_helper);  \
    emit_pushresult_I8();                        \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL CONV_TOR8_I4_helper(int val) {
    double result = (double) val;
    return *(__int64*)&result;
}
#endif
#endif

#ifndef emit_CONV_TOR8_I8
#define emit_CONV_TOR8_I8()                                    \
{                                                              \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I8(CONV_TOR8_I8_helper);                \
    emit_pushresult_I8();                                      \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL CONV_TOR8_I8_helper(__int64 val) {
    double result = (double) val;
    return *(__int64*)&result;
}
#endif
#endif

#ifndef emit_CONV_TOR8_R4
#define emit_CONV_TOR8_R4()                      \
{                                                \
    callInfo.reset();                            \
    emit_tos_arg( 1, INTERNAL_CALL );            \
    emit_callhelper_I4_I8(CONV_TOR8_R4_helper);  \
    emit_pushresult_I8();                        \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL CONV_TOR8_R4_helper(int val) {
    double result = (*(float *)&val);
    return *(__int64*)&result;
}
#endif
#endif

#define emit_CONV_TOR8_R8()     /* do nothing */

/***** convert (no overflow) (CONV.R.UN) to R ******/
#ifndef emit_CONV_UN_TOR_I4
#define emit_CONV_UN_TOR_I4()                    \
{                                                \
    callInfo.reset();                            \
    emit_tos_arg( 1, INTERNAL_CALL );            \
    emit_callhelper_I4_I8(CONV_UN_TOR_I4_helper);\
    emit_pushresult_I8();                        \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL CONV_UN_TOR_I4_helper(unsigned int val) {
    double result = (double) val;
    return *(__int64*)&result;
}
#endif
#endif

#ifndef emit_CONV_UN_TOR_I8
#define emit_CONV_UN_TOR_I8()                                  \
{                                                              \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I8(CONV_UN_TOR_I8_helper);              \
    emit_pushresult_I8();                                      \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL CONV_UN_TOR_I8_helper(unsigned __int64 val) {
    double result = (double)I64(0x100000000) * (double)(unsigned __int32)(val >> 32)
        + (double)(unsigned __int32)val;
    return *(__int64*)&result;
}
#endif
#endif


#define emit_CONV_UN_TOR_R4()     /* do nothing */
#define emit_CONV_UN_TOR_R8()     /* do nothing */

/***** convert (no overflow) (CONV) to U1 ******/
#ifndef emit_CONV_TOU1_I4
#define emit_CONV_TOU1_I4()                      \
{                                                \
    callInfo.reset();                            \
    emit_tos_arg( 1, INTERNAL_CALL );            \
    emit_callhelper_I4_I4(CONV_TOU1_I4_helper);  \
    emit_pushresult_I4();                        \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_TOU1_I4_helper(int val) {
    return (int) (val & 0xff);
}
#endif
#endif

#ifndef emit_CONV_TOU1_I8
#define emit_CONV_TOU1_I8()                                    \
{                                                              \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_TOU1_I8_helper);                \
    emit_pushresult_I4();                                      \
}
#ifdef DECLARE_HELPERS
unsigned  HELPER_CALL CONV_TOU1_I8_helper(__int64 val) {
    return (unsigned) (val & 0xff);
}
#endif
#endif

#ifndef emit_CONV_TOU1_R4
#define emit_CONV_TOU1_R4()                      \
{                                                \
    callInfo.reset();                            \
    emit_tos_arg( 1, INTERNAL_CALL );            \
    emit_callhelper_I4_I4(CONV_TOU1_R4_helper);  \
    emit_pushresult_I4();                        \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_TOU1_R4_helper(int val) {
    __int32 valI4 = (__int32)(*(float *)&val);
    return (unsigned int) ((unsigned char) (valI4));
}
#endif
#endif

#ifndef emit_CONV_TOU1_R8
#define emit_CONV_TOU1_R8()                                    \
{                                                              \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_TOU1_R8_helper);                \
    emit_pushresult_I4();                                      \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_TOU1_R8_helper(__int64 val) {
    __int32 valI4 = (__int32)(*(double *)&val);
    return (unsigned int) ((unsigned char) (valI4));
}
#endif
#endif

/***** convert (no overflow) (CONV) to U2 ******/
#ifndef emit_CONV_TOU2_I4
#define emit_CONV_TOU2_I4()                      \
{                                                \
    callInfo.reset();                            \
    emit_tos_arg( 1, INTERNAL_CALL );            \
    emit_callhelper_I4_I4(CONV_TOU2_I4_helper);  \
    emit_pushresult_I4();                        \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_TOU2_I4_helper(int val) {
    return (unsigned int) (val & 0xffff);
}
#endif
#endif

#ifndef emit_CONV_TOU2_I8
#define emit_CONV_TOU2_I8()                                    \
{                                                              \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_TOU2_I8_helper);                \
    emit_pushresult_I4();                                      \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_TOU2_I8_helper(__int64 val) {
    return (unsigned int) (val & 0xffff);
}
#endif
#endif

#ifndef emit_CONV_TOU2_R4
#define emit_CONV_TOU2_R4()                      \
{                                                \
    callInfo.reset();                            \
    emit_tos_arg( 1, INTERNAL_CALL );            \
    emit_callhelper_I4_I4(CONV_TOU2_R4_helper);  \
    emit_pushresult_I4();                        \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_TOU2_R4_helper(int val) {
    __int32 valI4 = (__int32)(*(float *)&val);
    return (unsigned int) ((unsigned short) (valI4));
}
#endif
#endif

#ifndef emit_CONV_TOU2_R8
#define emit_CONV_TOU2_R8()                                    \
{                                                              \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_TOU2_R8_helper);                \
    emit_pushresult_I4();                                      \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_TOU2_R8_helper(__int64 val) {
    __int32 valI4 = (__int32)(*(double *)&val);
    return (unsigned int) (unsigned short)(valI4);
}
#endif
#endif

/***** convert (no overflow) (CONV) to U4 ******/
#define emit_CONV_TOU4_I4()     /* do nothing */

#define emit_CONV_TOU4_I8()     emit_CONV_TOI4_I8()

#ifndef emit_CONV_TOU4_R4
#define emit_CONV_TOU4_R4()                                    \
{                                                              \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_arg( 1, INTERNAL_CALL ); \
    emit_callhelper_I4_I4(CONV_TOU4_R4_helper);                \
    emit_pushresult_U4();                                      \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_TOU4_R4_helper(__int32 val) {
    __int64 valI8 = (__int64)(*(float *)&val);
    return (unsigned __int32)(valI8);
}
#endif
#endif


#ifndef emit_CONV_TOU4_R8
#define emit_CONV_TOU4_R8()                                    \
{                                                              \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed,  INTERNAL_CALL ); \
    emit_callhelper_I8_I4(CONV_TOU4_R8_helper);                \
    emit_pushresult_U4();                                      \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL CONV_TOU4_R8_helper(__int64 val) {
     __int64 valI8 = (__int64)(*(double *)&val);
    return (unsigned __int32)(valI8);
}
#endif
#endif

#ifndef emit_CONV_TOU4_R
#define emit_CONV_TOU4_R()                 \
    emit_conv_RtoR8() ; \
    emit_callhelper_R8_I4(CONV_TOU4_R8_helper);  \
    emit_pushresult_U4()
#endif



/***** convert (no overflow) (CONV) to U8 ******/
#ifndef emit_CONV_TOU8_I4
#define emit_CONV_TOU8_I4()                      \
{                                                \
    callInfo.reset();                            \
    emit_tos_arg( 1, INTERNAL_CALL );            \
    emit_callhelper_I4_I8(CONV_TOU8_U4_helper);  \
    emit_pushresult_U8();                        \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL CONV_TOU8_U4_helper(unsigned int val) {
    return val;
}
#endif
#endif

#define emit_CONV_TOU8_I8()     /* do nothing */

#ifndef emit_CONV_TOU8_R4
#define emit_CONV_TOU8_R4()                                    \
{                                                              \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_arg( 1, INTERNAL_CALL ); \
    emit_callhelper_I4_I8(CONV_TOU8_R4_helper);                \
    emit_pushresult_U8();                                      \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL CONV_TOU8_R4_helper(__int64 val_i) {
    float val = (*(float *)&val_i);
    if (val <= INT64_MAX)
                return (unsigned __int64) val;

    // subtract 0x8000000000000000, do the convert then add it back again
    val = (val - (float) (2147483648.0 * 4294967296.0));
    return(((unsigned __int64) val) + I64(0x8000000000000000));
}
#endif
#endif

#ifndef emit_CONV_TOU8_R8
#define emit_CONV_TOU8_R8()                                    \
{                                                              \
    callInfo.reset();                                          \
    int NumRegUsed = 0;                                        \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed,  INTERNAL_CALL ); \
    emit_callhelper_I8_I8(CONV_TOU8_R8_helper);                \
    emit_pushresult_U8();                                      \
}
#ifdef DECLARE_HELPERS
unsigned __int64 HELPER_CALL CONV_TOU8_R8_helper(__int64 val_i) {
    double val = (*(double *)&val_i);
    if (val <= INT64_MAX)
                return (unsigned __int64) val;

    // subtract 0x8000000000000000, do the convert then add it back again
    val = (val - (2147483648.0 * 4294967296.0));
    return(((unsigned __int64) val) + I64(0x8000000000000000));
}
#endif
#endif

#ifndef emit_CONV_TOU8_R
#define emit_CONV_TOU8_R()                 \
    emit_conv_RtoR8() ; \
    emit_callhelper_R8_I8(CONV_TOU8_R8_helper);  \
    emit_pushresult_U8()
#endif


/****************************************************************************************
 * High level macros for IL opcodes accessing the elements of an array:
 *   LDELEM.*, LDELEMA, STELEM.*
 * Typically need to be redefined for each platform in order to obtain acceptable
 * performance
 ***************************************************************************************/

/************* LDELEMA ********************/
#ifndef emit_LDELEMA
#define emit_LDELEMA_0(elemSize,isReadOnly)                 \
{                                                \
    LABELSTACK((outPtr-outBuff),2);              \
    callInfo.reset();                            \
    emit_tos_arg( 3, INTERNAL_CALL );            \
    emit_tos_arg( 4, INTERNAL_CALL );            \
    emit_arg(elemSize, 2, INTERNAL_CALL);        \
    emit_arg(0, 1, INTERNAL_CALL);               \
    if(isReadOnly) {                                        \
       emit_callhelper_I4I4I4I4_I4(READONLY_LDELEMA_helper);\
    } else {                                                \
       emit_callhelper_I4I4I4I4_I4(LDELEMA_helper);         \
    }                                                       \
    emit_pushresult_Ptr();                       \
}

#define emit_LDELEMA(elemSize, clshnd, isReadOnly)          \
{                                                 \
    LABELSTACK((outPtr-outBuff),2);               \
    callInfo.reset();                             \
    emit_tos_arg( 3, INTERNAL_CALL );             \
    emit_tos_arg( 4, INTERNAL_CALL );             \
    emit_arg(elemSize, 2, INTERNAL_CALL);         \
    emit_arg(clshnd, 1, INTERNAL_CALL);           \
    if(isReadOnly) {                              \
       emit_callhelper_I4I4I4I4_I4(READONLY_LDELEMA_helper);\
    } else {                                                \
       emit_callhelper_I4I4I4I4_I4(LDELEMA_helper);         \
    }                                                       \
    emit_pushresult_Ptr();                        \
}

#ifdef DECLARE_HELPERS
void* HELPER_CALL LDELEMA_helper(void* clshnd, unsigned int elemSize, unsigned int index, CORINFO_Array* or_arr) {
    if (or_arr == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }
    if (index >= or_arr->length) {
        THROW_FROM_HELPER_RET(CORINFO_IndexOutOfRangeException);
    }
    unsigned fixup = 0;
    if (clshnd != 0) {
        CORINFO_CLASS_HANDLE elemType = *((CORINFO_CLASS_HANDLE*) &or_arr->i1Elems);
        if (elemType != clshnd)
            THROW_FROM_HELPER_RET(CORINFO_ArrayTypeMismatchException);
        fixup = sizeof(CORINFO_CLASS_HANDLE);
    }
    else
    {
#if !defined(_WIN64) && (SIZE_STACK_SLOT > 4)
        fixup = (elemSize >= 8) ? 4 : 0;
#endif
    }
    return &or_arr->i1Elems[index*elemSize + fixup];
}

//
// version of LDELEMA that does no runtime-check of the class Handle
//
void* HELPER_CALL READONLY_LDELEMA_helper(void * clshnd, unsigned int elemSize, unsigned int index, CORINFO_Array* or_arr) {
    if (or_arr == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }
    if (index >= or_arr->length) {
        THROW_FROM_HELPER_RET(CORINFO_IndexOutOfRangeException);
    }
    unsigned fixup = 0;
    if (clshnd != 0)
    {
        fixup = sizeof(CORINFO_CLASS_HANDLE);
    }
    else
    {
#if !defined(_WIN64) && (SIZE_STACK_SLOT > 4)
    fixup = (elemSize >= 8) ? 4 : 0;
#endif
    }
    return &or_arr->i1Elems[index*elemSize + fixup];
}
#endif

#endif

/************* LDELEM ********************/
#ifndef emit_LDELEM_I1
#define emit_LDELEM_I1()                        \
{                                               \
    LABELSTACK((outPtr-outBuff),2);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_callhelper_I4I4_I4(LDELEM_I1_helper);  \
    emit_pushresult_I4();                       \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL LDELEM_I1_helper(unsigned int index, CORINFO_Array* or_arr) {
    int i4;
    if (or_arr == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }
    if (index >= or_arr->length) {
        THROW_FROM_HELPER_RET(CORINFO_IndexOutOfRangeException);
    }
    i4 = or_arr->i1Elems[index];
    return i4;
}
#endif
#endif

#ifndef emit_LDELEM_I2
#define emit_LDELEM_I2()                        \
{                                               \
    LABELSTACK((outPtr-outBuff),2);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_callhelper_I4I4_I4(LDELEM_I2_helper);  \
    emit_pushresult_I4();                       \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL LDELEM_I2_helper(unsigned int index, CORINFO_Array* or_arr) {
    int i4;
    if (or_arr == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }
    if (index >= or_arr->length) {
        THROW_FROM_HELPER_RET(CORINFO_IndexOutOfRangeException);
    }
    i4 = or_arr->i2Elems[index];
    return i4;
}
#endif
#endif

#ifndef emit_LDELEM_I4
#define emit_LDELEM_I4()                        \
{                                               \
    LABELSTACK((outPtr-outBuff),2);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_callhelper_I4I4_I4(LDELEM_I4_helper);  \
    emit_pushresult_I4();                       \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL LDELEM_I4_helper(unsigned int index, CORINFO_Array* or_arr) {
    int i4;
    if (or_arr == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }
    if (index >= or_arr->length) {
        THROW_FROM_HELPER_RET(CORINFO_IndexOutOfRangeException);
    }
    i4 = or_arr->i4Elems[index];
    return i4;
}
#endif
#endif

#ifndef emit_LDELEM_U4
#define emit_LDELEM_U4()                        \
{                                               \
    LABELSTACK((outPtr-outBuff),2);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_callhelper_I4I4_I4(LDELEM_U4_helper);  \
    emit_pushresult_U4();                       \
}
#ifdef DECLARE_HELPERS
unsigned HELPER_CALL LDELEM_U4_helper(unsigned int index, CORINFO_Array* or_arr) {
    int u4;
    if (or_arr == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }
    if (index >= or_arr->length) {
        THROW_FROM_HELPER_RET(CORINFO_IndexOutOfRangeException);
    }
    u4 = or_arr->u4Elems[index];
    return u4;
}
#endif
#endif

#ifndef emit_LDELEM_I8
#define emit_LDELEM_I8()                        \
{                                               \
    LABELSTACK((outPtr-outBuff),2);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_callhelper_I4I4_I8(LDELEM_I8_helper);  \
    emit_pushresult_I8();                       \
}
#ifdef DECLARE_HELPERS
signed __int64 HELPER_CALL LDELEM_I8_helper(unsigned int index, CORINFO_Array8* or_arr) {
    signed __int64 i8;
    if (or_arr == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }
    if (index >= or_arr->length) {
        THROW_FROM_HELPER_RET(CORINFO_IndexOutOfRangeException);
    }
    i8 = or_arr->i8Elems[index];
    return i8;
}
#endif
#endif

#ifndef emit_LDELEM_I
#define emit_LDELEM_I()                        \
emit_WIN32(emit_LDELEM_I4()) emit_WIN64(emit_LDELEM_I8());
#endif

#ifndef emit_LDELEM_U1
#define emit_LDELEM_U1()                        \
{                                               \
    LABELSTACK((outPtr-outBuff),2);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_callhelper_I4I4_I4(LDELEM_U1_helper);  \
    emit_pushresult_I4();                       \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL LDELEM_U1_helper(unsigned int index, CORINFO_Array* or_arr) {
    unsigned int u4;
    if (or_arr == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }
    if (index >= or_arr->length) {
        THROW_FROM_HELPER_RET(CORINFO_IndexOutOfRangeException);
    }
    u4 = or_arr->u1Elems[index];
    return u4;
}
#endif
#endif

#ifndef emit_LDELEM_U2
#define emit_LDELEM_U2()                        \
{                                               \
    LABELSTACK((outPtr-outBuff),2);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_callhelper_I4I4_I4(LDELEM_U2_helper);  \
    emit_pushresult_I4();                       \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL LDELEM_U2_helper(unsigned int index, CORINFO_Array* or_arr) {
    unsigned int u4;
    if (or_arr == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }
    if (index >= or_arr->length) {
        THROW_FROM_HELPER_RET(CORINFO_IndexOutOfRangeException);
    }
    u4 = or_arr->u2Elems[index];
    return u4;
}
#endif
#endif


#ifndef emit_LDELEM_R4
#define emit_LDELEM_R4()                        \
{                                               \
    LABELSTACK((outPtr-outBuff),2);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_callhelper_I4I4_I4(LDELEM_R4_helper);  \
    emit_pushresult_I8();                       \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL LDELEM_R4_helper(unsigned int index, CORINFO_Array* or_arr) {
    if (or_arr == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }
    if (index >= or_arr->length) {
        THROW_FROM_HELPER_RET(CORINFO_IndexOutOfRangeException);
    }
    double r8;
    r8 = (double) or_arr->r4Elems[index];
    return *(__int64*)&r8;
}
#endif
#endif

#ifndef emit_LDELEM_R8
#define emit_LDELEM_R8()                        \
{                                               \
    LABELSTACK((outPtr-outBuff),2);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_callhelper_I4I4_I8(LDELEM_R8_helper);  \
    emit_pushresult_I8();                       \
    emit_conv_R8toR();                          \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL LDELEM_R8_helper(unsigned int index, CORINFO_Array8* or_arr) {
    double r8;
    if (or_arr == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }
    if (index >= or_arr->length) {
        THROW_FROM_HELPER_RET(CORINFO_IndexOutOfRangeException);
    }
    r8 = or_arr->r8Elems[index];
    return *(__int64*)&r8;
}
#endif
#endif

#ifndef emit_LDELEM_REF
#define emit_LDELEM_REF()                       \
{                                               \
    LABELSTACK((outPtr-outBuff),2);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_callhelper_I4I4_I4(LDELEM_REF_helper); \
    emit_pushresult_Ptr();                      \
}
#ifdef DECLARE_HELPERS
unsigned HELPER_CALL LDELEM_REF_helper(unsigned int index, CORINFO_RefArray* or_arr) {
    CORINFO_Object* result;
    if (or_arr == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }
    if (index >= or_arr->length) {
        THROW_FROM_HELPER_RET(CORINFO_IndexOutOfRangeException);
    }
    result = or_arr->refElems[index];
    return (unsigned) result;
}
#endif
#endif

/************* STELEM ********************/
#ifndef emit_STELEM_I1
#define emit_STELEM_I1()                       \
{                                              \
    LABELSTACK((outPtr-outBuff),3);            \
    callInfo.reset();                          \
    emit_tos_arg( 1, INTERNAL_CALL );          \
    emit_tos_arg( 2, INTERNAL_CALL );          \
    emit_tos_arg( 3, INTERNAL_CALL );          \
    emit_callhelper_I4I4I4(STELEM_I1_helper);  \
}
#ifdef DECLARE_HELPERS
void HELPER_CALL STELEM_I1_helper(signed char i1, unsigned int index, CORINFO_Array* or_arr) {
    if (or_arr == NULL) {
        THROW_FROM_HELPER(CORINFO_NullReferenceException);
    }
    if (index >= or_arr->length) {
        THROW_FROM_HELPER(CORINFO_IndexOutOfRangeException);
    }
    or_arr->i1Elems[index] = i1;
}
#endif
#endif

#ifndef emit_STELEM_I2
#define emit_STELEM_I2()                       \
{                                              \
    LABELSTACK((outPtr-outBuff),3);            \
    callInfo.reset();                          \
    emit_tos_arg( 1, INTERNAL_CALL );          \
    emit_tos_arg( 2, INTERNAL_CALL );          \
    emit_tos_arg( 3, INTERNAL_CALL );          \
    emit_callhelper_I4I4I4(STELEM_I2_helper);  \
}
#ifdef DECLARE_HELPERS
void HELPER_CALL STELEM_I2_helper(signed short i2, unsigned int index, CORINFO_Array* or_arr) {
    if (or_arr == NULL) {
        THROW_FROM_HELPER(CORINFO_NullReferenceException);
    }
    if (index >= or_arr->length) {
        THROW_FROM_HELPER(CORINFO_IndexOutOfRangeException);
    }
    or_arr->i2Elems[index] = i2;
}
#endif
#endif

#ifndef emit_STELEM_I
#define emit_STELEM_I() \
    emit_WIN64(emit_STELEM_I8()) emit_WIN32(emit_STELEM_I4());
#endif

#ifndef emit_STELEM_I4
#define emit_STELEM_I4()                        \
{                                               \
    LABELSTACK((outPtr-outBuff),3);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_tos_arg( 3, INTERNAL_CALL );           \
    emit_callhelper_I4I4I4(STELEM_I4_helper);   \
}
#ifdef DECLARE_HELPERS
void HELPER_CALL STELEM_I4_helper(signed int i4, unsigned int index, CORINFO_Array* or_arr) {
    if (or_arr == NULL) {
        THROW_FROM_HELPER(CORINFO_NullReferenceException);
    }
    if (index >= or_arr->length) {
        THROW_FROM_HELPER(CORINFO_IndexOutOfRangeException);
    }
    or_arr->i4Elems[index] = i4;
}
#endif
#endif

#ifndef emit_STELEM_I8
#define emit_STELEM_I8()                                        \
{                                                               \
    LABELSTACK((outPtr-outBuff),3);                             \
    callInfo.reset();                                           \
    int NumRegUsed = 0;                                         \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_tos_arg( 1 + NumRegUsed, INTERNAL_CALL );              \
    emit_tos_arg( 2 + NumRegUsed, INTERNAL_CALL );              \
    emit_callhelper_I8I4I4(STELEM_I8_helper);                   \
}

#ifdef DECLARE_HELPERS
void HELPER_CALL STELEM_I8_helper(signed __int64 i8, unsigned int index, CORINFO_Array8* or_arr) {
    if (or_arr == NULL) {
        THROW_FROM_HELPER(CORINFO_NullReferenceException);
    }
    if (index >= or_arr->length) {
        THROW_FROM_HELPER(CORINFO_IndexOutOfRangeException);
    }
    or_arr->i8Elems[index] = i8;
}
#endif
#endif

#ifndef emit_STELEM_U1
#define emit_STELEM_U1()                        \
{                                               \
    LABELSTACK((outPtr-outBuff),3);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_tos_arg( 3, INTERNAL_CALL );           \
    emit_callhelper_I4I4I4(STELEM_U1_helper);   \
}
#ifdef DECLARE_HELPERS
void HELPER_CALL STELEM_U1_helper(unsigned char u1, unsigned int index, CORINFO_Array* or_arr) {
    if (or_arr == NULL) {
        THROW_FROM_HELPER(CORINFO_NullReferenceException);
    }
    if (index >= or_arr->length) {
        THROW_FROM_HELPER(CORINFO_IndexOutOfRangeException);
    }
    or_arr->u1Elems[index] = u1;
}
#endif
#endif

#ifndef emit_STELEM_U2
#define emit_STELEM_U2()                        \
{                                               \
    LABELSTACK((outPtr-outBuff),3);             \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_tos_arg( 3, INTERNAL_CALL );           \
    emit_callhelper_I4I4I4(STELEM_U2_helper);   \
}
#ifdef DECLARE_HELPERS
void HELPER_CALL STELEM_U2_helper(unsigned short u2, unsigned int index, CORINFO_Array* or_arr) {
    if (or_arr == NULL) {
        THROW_FROM_HELPER(CORINFO_NullReferenceException);
    }
    if (index >= or_arr->length) {
        THROW_FROM_HELPER(CORINFO_IndexOutOfRangeException);
    }
    or_arr->u2Elems[index] = u2;
}
#endif
#endif

#ifndef emit_STELEM_R4
#define emit_STELEM_R4()                        \
{                                               \
    LABELSTACK((outPtr-outBuff),3);             \
    emit_conv_RtoR4();                          \
    callInfo.reset();                           \
    emit_tos_arg( 1, INTERNAL_CALL );           \
    emit_tos_arg( 2, INTERNAL_CALL );           \
    emit_tos_arg( 3, INTERNAL_CALL );           \
    emit_callhelper_I4I4I4(STELEM_R4_helper);   \
}
#ifdef DECLARE_HELPERS
void HELPER_CALL STELEM_R4_helper(int r4, unsigned int index, CORINFO_Array* or_arr) {
    if (or_arr == NULL) {
        THROW_FROM_HELPER(CORINFO_NullReferenceException);
    }
    if (index >= or_arr->length) {
        THROW_FROM_HELPER(CORINFO_IndexOutOfRangeException);
    }
    or_arr->r4Elems[index] = (*(float *)&r4);
}
#endif
#endif

#ifndef emit_STELEM_R8
#define emit_STELEM_R8()                                        \
{                                                               \
    LABELSTACK((outPtr-outBuff),3);                             \
    emit_conv_RtoR8();                                          \
    callInfo.reset();                                           \
    int NumRegUsed = 0;                                         \
    emit_tos_fixedsize_arg( 1, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_tos_arg( 1 + NumRegUsed, INTERNAL_CALL );              \
    emit_tos_arg( 2 + NumRegUsed, INTERNAL_CALL );              \
    emit_callhelper_I8I4I4(STELEM_R8_helper);                   \
}
#ifdef DECLARE_HELPERS
void HELPER_CALL STELEM_R8_helper(__int64 r8, unsigned int index, CORINFO_Array8* or_arr) {
    if (or_arr == NULL) {
        THROW_FROM_HELPER(CORINFO_NullReferenceException);
    }
    if (index >= or_arr->length) {
        THROW_FROM_HELPER(CORINFO_IndexOutOfRangeException);
    }
    or_arr->r8Elems[index] = (*(double *)&r8);
}
#endif
#endif

#ifndef emit_STELEM_REF
#define emit_STELEM_REF()                       \
{                                               \
    LABELSTACK((outPtr-outBuff), 3);            \
    callInfo.reset();                           \
    enregisterTOS;  /* array */                 \
    pop_register(ARG_2, true);   /* index */    \
    pop_register(ARG_1, true);   /* ref   */    \
    emit_tos_arg( 3, EXTERNAL_CALL );           \
    emit_reg_to_arg( 2, ARG_2, EXTERNAL_CALL ); \
    emit_reg_to_arg( 1, ARG_1, EXTERNAL_CALL ); \
    emit_callhelper_I4I4I4(FJit_pHlpArrAddr_St);\
}
#endif


/****************************************************************************************
 * High level macros for IL opcodes accessing the fields of an object:
 *   LDFLD.*(LDSFLD.*), LDFLDA(LDSFLDA), STFLD.*(STSFLD)
 * Typically need to be redefined for each platform in order to obtain acceptable
 * performance
 ***************************************************************************************/

/************* LDFLD ******************/
#ifndef emit_LDFLD_I1
#define emit_LDFLD_I1(isStatic, unaligned)                    \
{                                                             \
    if (!isStatic)                                            \
    {                                                         \
       LABELSTACK((outPtr-outBuff),0);                        \
       callInfo.reset();                                      \
       emit_tos_arg( 1, INTERNAL_CALL );                      \
       emit_tos_arg( 2, INTERNAL_CALL );                      \
       emit_callhelper_I4I4_I4(LDFLD_I1_helper);              \
       emit_pushresult_I4();                                  \
    }                                                         \
    else                                                      \
      { load_indirect_byte_signextend(TOS_REG_1,TOS_REG_1); } \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL LDFLD_I1_helper(unsigned int offset, CORINFO_Object* or_obj) {
    if (or_obj == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }
    return *((signed char*) ((char*)(or_obj)+offset));
}
#endif
#endif

#ifndef emit_LDFLD_I2
#define emit_LDFLD_I2(isStatic, unaligned)                    \
{                                                             \
    if (!isStatic)                                            \
    {                                                         \
       LABELSTACK((outPtr-outBuff),0);                        \
       callInfo.reset();                                      \
       emit_tos_arg( 2, INTERNAL_CALL );                      \
       emit_tos_arg( 3, INTERNAL_CALL );                      \
       emit_arg(((int)unaligned), 1, INTERNAL_CALL);          \
       emit_callhelper_I4I4I4_I4(LDFLD_I2_helper);            \
       emit_pushresult_I4();                                  \
    }                                                         \
    else                                                      \
       {load_indirect_word_signextend(TOS_REG_1,TOS_REG_1, (!unaligned)); inRegTOS = true;}  \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL LDFLD_I2_helper(unsigned int unaligned, unsigned int offset, CORINFO_Object* or_obj) {
    if (or_obj == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }

    if ( unaligned == 0 )
        return *((signed short*) ((char*)(or_obj)+offset));

    signed short temp_val = GET_UNALIGNED_16(((signed short*) ((char*)(or_obj)+offset)));
    return (temp_val);
}
#endif
#endif

#ifndef emit_LDFLD_I4
#define emit_LDFLD_I4(isStatic, unaligned)                    \
{                                                             \
    if (!isStatic)                                            \
    {                                                         \
       LABELSTACK((outPtr-outBuff),0);                        \
       callInfo.reset();                                      \
       emit_tos_arg( 2, INTERNAL_CALL );                      \
       emit_tos_arg( 3, INTERNAL_CALL );                      \
       emit_arg(((int)unaligned), 1, INTERNAL_CALL);          \
       emit_callhelper_I4I4I4_I4(LDFLD_I4_helper);            \
       emit_pushresult_I4();                                  \
    }                                                         \
    else {load_indirect_dword_signextend(TOS_REG_1,TOS_REG_1, (!unaligned)); inRegTOS = true; }     \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL LDFLD_I4_helper(unsigned int unaligned, unsigned int offset, CORINFO_Object* or_obj) {
    if (or_obj == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }
    if ( unaligned == 0 )
        return *((int*) ((char*)(or_obj)+offset));

    return GET_UNALIGNED_32(((int*) ((char*)(or_obj)+offset)));
}
#endif
#endif

#ifndef emit_LDFLD_R4
#define emit_LDFLD_R4(isStatic, unaligned)                    \
{                                                             \
    if (!isStatic)                                            \
    {                                                         \
       LABELSTACK((outPtr-outBuff),0);                        \
       callInfo.reset();                                      \
       emit_tos_arg( 2, INTERNAL_CALL );                      \
       emit_tos_arg( 3, INTERNAL_CALL );                      \
       emit_arg(((int)unaligned), 1, INTERNAL_CALL);          \
       emit_callhelper_I4I4I4_I8(LDFLD_R4_helper);            \
       emit_pushresult_I8();                                  \
    }                                                         \
    else                                                      \
       {emit_LDIND_R4(unaligned);}                            \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL LDFLD_R4_helper(unsigned int unaligned, unsigned int offset, CORINFO_Object* or_obj) {
    if (or_obj == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }

    if ( unaligned == 0 )
    {
        double f = (double) (*((float*) ((char*)(or_obj)+offset)));
        return *(__int64*) (&f);
    }

    int i = GET_UNALIGNED_32(((int*) ((char*)(or_obj)+offset)));
    double d = (double)*(float *)(&i);
    return *(__int64*) (&d);
}
#endif
#endif

#ifndef emit_LDFLD_I8
#define emit_LDFLD_I8(isStatic, unaligned)                    \
{                                                             \
    if (!isStatic)                                            \
    {                                                         \
       LABELSTACK((outPtr-outBuff),0);                        \
       callInfo.reset();                                      \
       emit_tos_arg( 2, INTERNAL_CALL );                      \
       emit_tos_arg( 3, INTERNAL_CALL );                      \
       emit_arg(((int)unaligned), 1, INTERNAL_CALL);          \
       emit_callhelper_I4I4I4_I8(LDFLD_I8_helper);            \
       emit_pushresult_I8();                                  \
    }                                                         \
    else                                                      \
       {emit_LDIND_I8(unaligned);}                            \
}
#ifdef DECLARE_HELPERS
__int64 HELPER_CALL LDFLD_I8_helper(unsigned int unaligned, unsigned int offset, CORINFO_Object* or_obj) {
    if (or_obj == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }

    if ( unaligned == 0 )
        return *(__int64*) ((char*)(or_obj)+offset);

    return GET_UNALIGNED_64(((char*)(or_obj)+offset));
}
#endif
#endif

#ifndef emit_LDFLD_R8
#define emit_LDFLD_R8(isStatic, unaligned)                    \
{                                                             \
    if (!isStatic)                                            \
    {                                                         \
       LABELSTACK((outPtr-outBuff),0);                        \
       callInfo.reset();                                      \
       emit_tos_arg( 2, INTERNAL_CALL );                      \
       emit_tos_arg( 3, INTERNAL_CALL );                      \
       emit_arg(((int)unaligned), 1, INTERNAL_CALL);          \
       emit_callhelper_I4I4I4_I8(LDFLD_R8_helper);            \
       emit_pushresult_I8();                                  \
    }                                                         \
    else                                                      \
       {emit_LDIND_R8(unaligned);}                            \
    emit_conv_R8toR();                                        \
}

#ifdef DECLARE_HELPERS
__int64 HELPER_CALL LDFLD_R8_helper(unsigned int unaligned, unsigned int offset, CORINFO_Object* or_obj) {
    if (or_obj == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }

    if ( unaligned == 0 )
        return *((__int64*) ((char*)(or_obj)+offset));

    return GET_UNALIGNED_64(((char*)(or_obj)+offset));
}
#endif
#endif

#ifndef emit_LDFLD_U1
#define emit_LDFLD_U1(isStatic, unaligned)                    \
{                                                             \
    if (!isStatic)                                            \
    {                                                         \
       LABELSTACK((outPtr-outBuff),0);                        \
       callInfo.reset();                                      \
       emit_tos_arg( 1, INTERNAL_CALL );                      \
       emit_tos_arg( 2, INTERNAL_CALL );                      \
       emit_callhelper_I4I4_I4(LDFLD_U1_helper);              \
       emit_pushresult_I4();                                  \
    }                                                         \
    else                                                      \
       {load_indirect_byte_zeroextend(TOS_REG_1,TOS_REG_1); inRegTOS = true;}  \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL LDFLD_U1_helper(unsigned int offset, CORINFO_Object* or_obj) {
    if (or_obj == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }
    return *((unsigned char*) ((char*)(or_obj)+offset));
}
#endif
#endif

#ifndef emit_LDFLD_U2
#define emit_LDFLD_U2(isStatic, unaligned)                    \
{                                                             \
    if (!isStatic)                                            \
    {                                                         \
       LABELSTACK((outPtr-outBuff),0);                        \
       callInfo.reset();                                      \
       emit_tos_arg( 2, INTERNAL_CALL );                      \
       emit_tos_arg( 3, INTERNAL_CALL );                      \
       emit_arg(((int)unaligned), 1, INTERNAL_CALL);          \
       emit_callhelper_I4I4I4_I4(LDFLD_U2_helper);            \
       emit_pushresult_I4();                                  \
    }                                                         \
    else                                                      \
       {load_indirect_word_zeroextend(TOS_REG_1,TOS_REG_1, !(unaligned)); inRegTOS = true;}  \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL LDFLD_U2_helper(unsigned int unaligned, unsigned int offset, CORINFO_Object* or_obj) {
    if (or_obj == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }

    if ( unaligned == 0 )
        return *((unsigned short*) ((char*)(or_obj)+offset));

    unsigned short temp_val = GET_UNALIGNED_16(((signed short*) ((char*)(or_obj)+offset)));
    return (temp_val);
}
#endif
#endif
#ifndef emit_LDFLD_U4
#define emit_LDFLD_U4(isStatic, unaligned)                    \
{                                                             \
    if (!isStatic)                                            \
    {                                                         \
       LABELSTACK((outPtr-outBuff),0);                        \
       callInfo.reset();                                      \
       emit_tos_arg( 2, INTERNAL_CALL );                      \
       emit_tos_arg( 3, INTERNAL_CALL );                      \
       emit_arg(((int)unaligned), 1, INTERNAL_CALL);          \
       emit_callhelper_I4I4I4_I4(LDFLD_U4_helper);            \
       emit_pushresult_U4();                                  \
    }                                                         \
    else                                                      \
       {load_indirect_dword_zeroextend(TOS_REG_1,TOS_REG_1, !(unaligned)); inRegTOS = true;}       \
}
#ifdef DECLARE_HELPERS
int HELPER_CALL LDFLD_U4_helper(unsigned int unaligned, unsigned int offset, CORINFO_Object* or_obj) {
    if (or_obj == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }

    if ( unaligned == 0 )
        return *((int*) ((char*)(or_obj)+offset));

    return GET_UNALIGNED_32(((int*) ((char*)(or_obj)+offset)));
}
#endif
#endif

#ifndef emit_LDFLD_REF
#define emit_LDFLD_REF(isStatic, unaligned)                   \
{                                                             \
    if (!isStatic)                                            \
    {                                                         \
       LABELSTACK((outPtr-outBuff),0);                        \
       callInfo.reset();                                      \
       emit_tos_arg( 2, INTERNAL_CALL );                      \
       emit_tos_arg( 3, INTERNAL_CALL );                      \
       emit_arg(((int)unaligned), 1, INTERNAL_CALL);          \
       emit_callhelper_I4I4I4_I4(LDFLD_REF_helper);           \
       emit_pushresult_Ptr();                                 \
    }                                                         \
    else                                                      \
      {load_indirect_dword_zeroextend(TOS_REG_1,TOS_REG_1,!(unaligned)); inRegTOS = true;}  \
}
#ifdef DECLARE_HELPERS
unsigned HELPER_CALL LDFLD_REF_helper(unsigned int unaligned, unsigned int offset, CORINFO_Object* or_obj) {
    if (or_obj == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }

    if ( unaligned == 0 )
        return *((unsigned*) ((char*)(or_obj)+offset));

    return GET_UNALIGNED_32(((unsigned*) ((char*)(or_obj)+offset)));
}
#endif
#endif

#ifndef emit_LDFLD_helper
#define emit_LDFLD_helper(helper,fieldDesc)        \
{                                                  \
    callInfo.reset();                              \
    emit_mov_TOS_reg(ARG_1);                       \
    emit_arg( fieldDesc, 2, EXTERNAL_CALL );       \
    emit_reg_to_arg( 1, ARG_1, EXTERNAL_CALL );    \
    emit_callhelper_I4I4_I4(helper);               \
}
#endif


/************* STFLD ******************/
#ifndef emit_STFLD_I1
#define emit_STFLD_I1(isStatic, unaligned)                    \
{                                                             \
    if (!isStatic)                                            \
    {                                                         \
       LABELSTACK((outPtr-outBuff),0);                        \
       callInfo.reset();                                      \
       emit_tos_arg( 1, INTERNAL_CALL );                      \
       emit_tos_arg( 2, INTERNAL_CALL );                      \
       emit_tos_arg( 3, INTERNAL_CALL );                      \
       emit_callhelper_I4I4I4(STFLD_I1_helper);               \
    }                                                         \
    else                                                      \
       {emit_STIND_REV_I1(false);}                            \
}
#ifdef DECLARE_HELPERS
void HELPER_CALL STFLD_I1_helper(unsigned int offset, signed char val, CORINFO_Object* or_obj) {
    if (or_obj == NULL) {
        THROW_FROM_HELPER(CORINFO_NullReferenceException);
    }
    *((signed char*) ((char*)(or_obj)+offset)) = val;
}
#endif
#endif

#ifndef emit_STFLD_I2
#define emit_STFLD_I2(isStatic, unaligned)                    \
{                                                             \
    if (!isStatic)                                            \
    {                                                         \
       LABELSTACK((outPtr-outBuff),0);                        \
       callInfo.reset();                                      \
       emit_tos_arg( 2, INTERNAL_CALL );                      \
       emit_tos_arg( 3, INTERNAL_CALL );                      \
       emit_tos_arg( 4, INTERNAL_CALL );                      \
       emit_arg(((int)unaligned), 1, INTERNAL_CALL);          \
       emit_callhelper_I4I4I4I4(STFLD_I2_helper);             \
    }                                                         \
    else                                                      \
      {emit_STIND_REV_I2(unaligned);}                         \
}
#ifdef DECLARE_HELPERS
void HELPER_CALL STFLD_I2_helper(unsigned int unaligned, unsigned int offset, signed short val, CORINFO_Object* or_obj) {
    if (or_obj == NULL) {
        THROW_FROM_HELPER(CORINFO_NullReferenceException);
    }

    if ( unaligned == 0 )
    {
        *((signed short*) ((char*)(or_obj)+offset)) = val;
    }
    else
    {
        signed short* ptr = ((signed short*) ((char*)(or_obj)+offset));
        SET_UNALIGNED_16(ptr, val);
    }
}
#endif
#endif

#ifndef emit_STFLD_I4
#define emit_STFLD_I4(isStatic, unaligned)                    \
{                                                             \
    if (!isStatic)                                            \
    {                                                         \
       LABELSTACK((outPtr-outBuff),0);                        \
       callInfo.reset();                                      \
       emit_tos_arg( 2, INTERNAL_CALL );                      \
       emit_tos_arg( 3, INTERNAL_CALL );                      \
       emit_tos_arg( 4, INTERNAL_CALL );                      \
       emit_arg(((int)unaligned), 1, INTERNAL_CALL);          \
       emit_callhelper_I4I4I4I4(STFLD_I4_helper);             \
    }                                                         \
    else                                                      \
    {                                                         \
       enregisterTOS;                                         \
       pop_register(ARG_1, true);                             \
       store_indirect_32bits(ARG_1,TOS_REG_1,!(unaligned));   \
       inRegTOS = false;                                      \
    }                                                         \
}
#ifdef DECLARE_HELPERS
void HELPER_CALL STFLD_I4_helper(unsigned int unaligned, unsigned int offset, int val, CORINFO_Object* or_obj) {
    if (or_obj == NULL) {
        THROW_FROM_HELPER(CORINFO_NullReferenceException);
    }

    if ( unaligned == 0 )
       *((int*) ((char*)(or_obj)+offset)) = val;
    else
       SET_UNALIGNED_32(((int*) ((char*)(or_obj)+offset)), val);
}
#endif
#endif

#ifndef emit_STFLD_R4
#define emit_STFLD_R4(isStatic, unaligned)                    \
{                                                             \
    if (!isStatic)                                            \
    {                                                         \
       LABELSTACK((outPtr-outBuff),0);                        \
       callInfo.reset();                                      \
       emit_tos_arg( 2, INTERNAL_CALL );                      \
       emit_tos_arg( 3, INTERNAL_CALL );                      \
       emit_tos_arg( 4, INTERNAL_CALL );                      \
       emit_arg(((int)unaligned), 1, INTERNAL_CALL);          \
       /*emit_conv_RtoR4(); - hoisted out because of tls support  */ \
       emit_callhelper_I4I4I4I4(STFLD_R4_helper);             \
    }                                                         \
    else                                                      \
       {emit_STIND_REV_I4(unaligned); /* since we have already converted the R to R4 */}\
}
#ifdef DECLARE_HELPERS
void HELPER_CALL STFLD_R4_helper(unsigned int unaligned, unsigned int offset, int val, CORINFO_Object* or_obj) {
    if (or_obj == NULL) {
        THROW_FROM_HELPER(CORINFO_NullReferenceException);
    }
    if ( unaligned == 0 )
       *((float*) ((char*)(or_obj)+offset)) = (float)(*(float *)&val);
    else
       SET_UNALIGNED_32(((int*) ((char*)(or_obj)+offset)), val);
}
#endif
#endif

#ifndef emit_STFLD_I8
#define emit_STFLD_I8(isStatic, unaligned)                      \
{                                                               \
  if (!isStatic)                                                \
  {                                                             \
    LABELSTACK((outPtr-outBuff),0);                             \
   /*emit_conv_RtoR8(); - hoisted out because of tls support */ \
    callInfo.reset();                                           \
    int NumRegUsed = 0;                                         \
    emit_tos_arg( 2, INTERNAL_CALL );                           \
    emit_tos_fixedsize_arg( 3, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_tos_arg( 3 + NumRegUsed, INTERNAL_CALL );              \
    emit_arg(((int)unaligned), 1, INTERNAL_CALL);               \
    emit_callhelper_I4I4I8I4(STFLD_I8_helper);                  \
  }                                                             \
  else                                                          \
    {emit_STIND_REV_I8(unaligned);}                             \
}
#ifdef DECLARE_HELPERS
void HELPER_CALL STFLD_I8_helper(unsigned int unaligned, unsigned int offset, __int64 val, CORINFO_Object* or_obj) {
    if (or_obj == NULL) {
        THROW_FROM_HELPER(CORINFO_NullReferenceException);
    }
    if ( unaligned == 0 )
       *((__int64*) ((char*)(or_obj)+offset)) = val;
    else
       SET_UNALIGNED_64(((__int64*) ((char*)(or_obj)+offset)), val);
}
#endif
#endif

#ifndef emit_STFLD_R8
#define emit_STFLD_R8(isStatic, unaligned)                      \
{                                                               \
  if (!isStatic)                                                \
  {                                                             \
    LABELSTACK((outPtr-outBuff),0);                             \
   /*emit_conv_RtoR8(); - hoisted out because of tls support */ \
    callInfo.reset();                                           \
    int NumRegUsed = 0;                                         \
    emit_tos_arg( 2, INTERNAL_CALL );                           \
    emit_tos_fixedsize_arg( 3, 8, NumRegUsed, INTERNAL_CALL );  \
    emit_tos_arg( 3 + NumRegUsed, INTERNAL_CALL );              \
    emit_arg(((int)unaligned), 1, INTERNAL_CALL);               \
    emit_callhelper_I4I4I8I4(STFLD_R8_helper);                  \
   }                                                            \
   else                                                         \
    {emit_STIND_REV_R8(unaligned);}                             \
}
#ifdef DECLARE_HELPERS
void HELPER_CALL STFLD_R8_helper(unsigned int unaligned, unsigned int offset, __int64 val, CORINFO_Object* or_obj) {
    if (or_obj == NULL) {
        THROW_FROM_HELPER(CORINFO_NullReferenceException);
    }
    if ( unaligned == 0 )
       *((__int64*) ((char*)(or_obj)+offset)) = val;
    else
       SET_UNALIGNED_64(((__int64*) ((char*)(or_obj)+offset)), val);
}
#endif
#endif

#ifndef emit_STFLD_REF
#define emit_STFLD_REF(isStatic, unaligned)                   \
{                                                             \
    callInfo.reset();                                         \
    emit_tos_arg( 1, INTERNAL_CALL );                         \
    emit_tos_arg( 2, INTERNAL_CALL );                         \
    if (!isStatic)                                            \
    {                                                         \
       LABELSTACK((outPtr-outBuff),0);                        \
       emit_tos_arg( 3, INTERNAL_CALL );                      \
       emit_callhelper_I4I4I4(STFLD_REF_helper);              \
    }                                                         \
    else                                                      \
       {emit_callhelper_I4I4(STSFLD_REF_helper);}             \
}
#ifdef DECLARE_HELPERS
void HELPER_CALL STFLD_REF_helper(unsigned int offset, CORINFO_Object* val, CORINFO_Object* or_obj) {
    if(or_obj == NULL) {
        THROW_FROM_HELPER(CORINFO_NullReferenceException);
    }
    HCALL2(void, FJit_pHlpAssign_Ref, CORINFO_Object**, (CORINFO_Object**)((char*)(or_obj)+offset), CORINFO_Object*, val);
}
void HELPER_CALL STSFLD_REF_helper(CORINFO_Object** pObj, CORINFO_Object* val)
{
    HCALL2(void, FJit_pHlpAssign_Ref, CORINFO_Object**, pObj, CORINFO_Object*, val);
}
#endif  // DECLARE_HELPERS
#endif  // emit_STFLD_REF

/*
Description:

Possible values for 'helper':
HCIMPL3(VOID, JIT_SetField32, Object *obj, FieldDesc *pFD, __int32 value)
HCIMPL3(VOID, JIT_SetField64, Object *obj, FieldDesc *pFD, __int64 value)
HCIMPL3(VOID, JIT_SetField32Obj, Object *obj, FieldDesc *pFD, Object *value)

On entry to this macro it is assumed that the 'value' is on the top
of the stack and the 'obj' is bellow it. The value can be either 4 or 8 bytes, its size
is determined from 'sizeInBytes' argument.
*/
#ifndef emit_STFLD_NonStatic_field_helper
#define emit_STFLD_NonStatic_field_helper(fieldDesc,sizeInBytes,helper)   \
{                                                                         \
    emit_getSP((BYTE_ALIGNED(sizeInBytes) + STACK_BUFFER));               \
    emit_LDIND_PTR(false);                                                \
    callInfo.reset();                                                     \
    int NumRegUsed = 0;                                                   \
    emit_mov_TOS_reg(ARG_1);                                              \
    emit_tos_fixedsize_arg( 3, sizeInBytes, NumRegUsed, EXTERNAL_CALL ON_X86_ONLY( && false) );  \
    emit_arg( fieldDesc, 2, EXTERNAL_CALL );                              \
    emit_reg_to_arg( 1, ARG_1, EXTERNAL_CALL );                           \
    if (sizeInBytes == sizeof(void*))                                     \
         { emit_callhelper_I4I4I4_I4(helper);}                            \
    else                                                                  \
         { emit_callhelper_I4I4I8_I4(helper);}                            \
    emit_POP_PTR();                                                       \
}
#endif

#ifndef emit_LDLEN
#define emit_LDLEN()                                          \
{                                                             \
    LABELSTACK((outPtr-outBuff), 1);                          \
    callInfo.reset();                                         \
    emit_tos_arg( 1, INTERNAL_CALL );                         \
    emit_callhelper_I4_I4(LDLEN_helper);                      \
    emit_pushresult_U4();                                     \
}
#ifdef DECLARE_HELPERS
unsigned int HELPER_CALL LDLEN_helper(CORINFO_Array* or_obj) {
    if (or_obj == NULL) {
        THROW_FROM_HELPER_RET(CORINFO_NullReferenceException);
    }
    return or_obj->length;
}
#endif
#endif

#ifndef emit_verification_throw
#define emit_verification_throw(ptr)                          \
{                                                             \
    callInfo.reset();                                         \
    emit_arg( ptr, 1, EXTERNAL_CALL );                        \
    emit_callhelper_I4(FJit_pHlpVerification);                \
}
#endif

/*********************************************************************************************
    opcodes implemented by inline calls to the standard JIT helpers

    Note: An extra call layer is defined here in the cases the JIT helper calling convention
          differs from the FJIT helper calling convention on a particular chip.
          If they are the same, then these could be redefined in the chip specific macro file
          to remove the extra call layer, if desired.

**********************************************************************************************/

/* HCIMPL3(VOID, JIT_SetFieldStruct, Object *obj, FieldDesc *pFD, LPVOID valuePtr) */
#ifndef emit_SetFieldStruct
#define emit_SetFieldStruct(slots, targetField, helperFunc, valClassHnd) \
{                                                                   \
          callInfo.reset();                                         \
          emit_mov_TOS_reg(ARG_1);            /* obj == arg 1 => reg 1 */\
          /* arg 3 == pointer to value class => stack*/             \
          emit_getSP(STACK_BUFFER + bigEndianOffset( typeSizeInBytes(jitInfo, valClassHnd) ));  \
          emit_tos_arg( 3, EXTERNAL_CALL );   /* arg 3 => argument reg if necessary */          \
          emit_arg( targetField, 2, EXTERNAL_CALL );  /* fieldDesc == arg 2 => stack or arg. reg. */                     \
          emit_reg_to_arg(1, ARG_1, EXTERNAL_CALL );                       \
          emit_callhelper_I4I4I4(helperFunc);                              \
          emit_drop((slots) * sizeof(void*)); /* pop of the value class*/  \
}
#endif

#ifndef emit_GetFieldStruct
#define emit_GetFieldStruct(slots, targetField, helperFunc, valClassHnd) \
{                                                             \
     callInfo.reset();                                        \
     emit_mov_TOS_reg(ARG_2);                                 \
       while (slots > 0) {                                    \
        if ( SIZE_STACK_SLOT == 4 )                           \
        {                                                     \
            emit_LDC_I4(0);                                   \
            --slots;                                          \
        } else if ( SIZE_STACK_SLOT == 8 )                    \
        {                                                     \
            emit_LDC_I8(I64(0));                              \
            slots = slots - 2;                                \
    } else                                                \
      _ASSERTE(false && "NYI - emit_GetFieldStruct");     \
     }                                                        \
     emit_getSP(STACK_BUFFER + bigEndianOffset( typeSizeInBytes(jitInfo, valClassHnd) )); \
     emit_mov_TOS_reg(ARG_1);        /* retBuff => arg reg 1*/\
     emit_arg( targetField, 3, EXTERNAL_CALL );  /* fieldDesc on the stack*/\
     emit_reg_to_arg(2, ARG_2, EXTERNAL_CALL );               \
     emit_reg_to_arg(1, ARG_1, EXTERNAL_CALL );               \
     LABELSTACK((outPtr-outBuff),0);                          \
     emit_callhelper_I4I4I4_I4(helperFunc);                   \
}
#endif

#ifndef emit_FIELDADDRHelper
#define emit_FIELDADDRHelper(jitHelper, field)    \
{                                                 \
     callInfo.reset();                            \
     emit_arg( field, 1, EXTERNAL_CALL );         \
     emit_callhelper_I4_I4(jitHelper)             \
     emit_pushresult_I4();                        \
}
#endif

#ifndef emit_initclass
#define emit_initclass(cls)                       \
{                                                 \
     LABELSTACK((outPtr-outBuff), 0);             \
     callInfo.reset();                            \
     emit_arg( cls, 1, EXTERNAL_CALL );           \
     emit_callhelper_I4(FJit_pHlpInitClass);      \
}
#endif

#ifndef emit_trap_gc
#define emit_trap_gc()                            \
{                                                 \
    LABELSTACK((outPtr-outBuff), 0);              \
    callInfo.reset();                             \
    emit_callhelper_(FJit_pHlpPoll_GC);           \
}
#endif

#ifndef emit_NEWOARR
#define emit_NEWOARR(comType)                      \
{                                                  \
     LABELSTACK((outPtr-outBuff), 0);              \
     callInfo.reset();                             \
     emit_tos_arg( 2, EXTERNAL_CALL );             \
     emit_arg( comType, 1, EXTERNAL_CALL );        \
     emit_callhelper_I4I4_I4(FJit_pHlpNewArr_1_Direct);\
     emit_pushresult_Ptr();                        \
}
#endif

#ifndef emit_NEWOBJ
#define emit_NEWOBJ(targetClass,jit_helper)        \
{                                                  \
     LABELSTACK((outPtr-outBuff), 0);              \
     callInfo.reset();                             \
     emit_arg( targetClass, 1, EXTERNAL_CALL );    \
     emit_callhelper_I4(jit_helper);               \
     emit_pushresult_Ptr();                        \
}
#endif

#ifndef emit_NEWOBJ_array
#define emit_NEWOBJ_array(targetClass, constrMethod, constructorArgBytes)        \
    LABELSTACK((outPtr-outBuff),0);                                 \
    if (PARAMETER_SPACE)                                            \
    {                                                               \
        deregisterTOS;                                              \
        unsigned callFrameSize = BYTE_ALIGNED( call_frame_size(     \
            max( constructorArgBytes + 2 * sizeof(void*),           \
                MAX_GP_ARG_REGISTER * sizeof(void*) ),              \
            (MAX_GP_ARG_REGISTER * sizeof(void*) ) ) );             \
        if ( callFrameSize )                                        \
            { emit_call_frame(callFrameSize); }                     \
        int numArgs = (constructorArgBytes/sizeof(void*)) + 2;      \
        int numRegArgs = min(MAX_GP_ARG_REGISTER, numArgs);         \
        int temp_i;                                                 \
        for( temp_i = 2; temp_i < numRegArgs; temp_i++ )            \
            { emit_mov_arg_reg(                                     \
                (callFrameSize+(numArgs-temp_i-1)*SIZE_STACK_SLOT+STACK_BUFFER), \
                temp_i); }                                          \
        for( temp_i = numRegArgs; temp_i < numArgs; temp_i++ )      \
            { emit_mov_arg_stack(sizeof(prolog_frame)+sizeof(void*)*temp_i, \
                (callFrameSize+(numArgs-temp_i-1)*SIZE_STACK_SLOT+STACK_BUFFER), \
                sizeof(void*) ); }                                  \
        callInfo.reset();                                           \
        emit_arg( constrMethod, 2, EXTERNAL_CALL);                  \
        emit_arg( targetClass, 1, EXTERNAL_CALL);                   \
        emit_callhelper_il(FJit_pHlpNewObj);                        \
        if ( CALLER_CLEANS_STACK && callFrameSize )                 \
           { add_constant(SP, callFrameSize); }                     \
    }                                                               \
    else                                                            \
    {                                                               \
        callInfo.reset();                                           \
        emit_arg( constrMethod, 2, EXTERNAL_CALL ON_X86_ONLY(&&false));\
        emit_arg( targetClass, 1, EXTERNAL_CALL ON_X86_ONLY(&&false));\
        emit_callhelper_I4I4_I4(FJit_pHlpNewObj);                   \
        ON_X86_ONLY(if (!CALLER_CLEANS_STACK) emit_drop(8);)        \
    }                                                               \
    emit_drop((constructorArgBytes/sizeof(void*)) * SIZE_STACK_SLOT); \
    emit_pushresult_Ptr()
#endif

#ifndef emit_MKREFANY
#define emit_MKREFANY(targetClass)      \
    if ( !PASS_VALUETYPE_BYREF)         \
    {                                   \
      emit_save_TOS();                    \
      inRegTOS = false;                   \
      emit_pushconstant_4(targetClass);   \
      emit_restore_TOS();                 \
    }                                     \
    else                                  \
    {                                     \
      deregisterTOS;                      \
      emit_getSP(STACK_BUFFER+4);                    \
      mov_constant( ARG_1, targetClass);             \
      mov_register_indirect_from(ARG_1, TOS_REG_1);  \
      inRegTOS = false;                              \
    }
#endif

#ifndef emit_REFANYVAL
#define emit_REFANYVAL(targetClass)                              \
{                                                                \
    callInfo.reset();                                            \
    int NumRegUsed = 0;                                          \
    if ( !PASS_VALUETYPE_BYREF)                                  \
       emit_tos_fixedsize_arg( 2, 8, NumRegUsed, INTERNAL_CALL ) \
    else                                                         \
    {                                                            \
       emit_getSP(STACK_BUFFER);                                 \
       emit_tos_arg( 2, EXTERNAL_CALL );                         \
    }                                                            \
    emit_arg( targetClass, 1, EXTERNAL_CALL );                   \
    LABELSTACK((outPtr-outBuff),0);                              \
    if ( !PASS_VALUETYPE_BYREF)                                  \
       emit_callhelper_I4I8_I4(FJit_pHlpGetRefAny)               \
    else                                                         \
    {                                                            \
       emit_callhelper_I4I4_I4(FJit_pHlpGetRefAny);              \
       emit_drop(8);                                             \
    }                                                            \
    emit_pushresult_Ptr();                                       \
}
#endif

#ifndef emit_THROW
#define emit_THROW()                               \
{                                                  \
     callInfo.reset();                             \
     emit_tos_arg( 1, EXTERNAL_CALL );             \
     emit_callhelper_I4(FJit_pHlpThrow);           \
}
#endif

#ifndef emit_RETHROW
#define emit_RETHROW()                             \
{                                                  \
     callInfo.reset();                             \
     emit_callhelper_(FJit_pHlpRethrow);           \
}
#endif

#ifndef emit_ENDCATCH
#define emit_ENDCATCH()                            \
{                                                  \
     callInfo.reset();                             \
     emit_callhelper_(FJit_pHlpEndCatch);          \
}
#endif

#ifndef emit_ENTER_CRIT
//monitor object is <this>, i.e. arg #0
#define emit_ENTER_CRIT()                          \
{                                                  \
     callInfo.reset();                             \
     emit_tos_arg( 1, EXTERNAL_CALL );             \
     LABELSTACK((outPtr-outBuff),0);               \
     emit_callhelper_I4(FJit_pHlpMonEnter);        \
}
#endif

#ifndef emit_EXIT_CRIT
//monitor object is <this>, i.e. arg #0
#define emit_EXIT_CRIT()                           \
{                                                  \
     callInfo.reset();                             \
     emit_tos_arg( 1, EXTERNAL_CALL );             \
     LABELSTACK((outPtr-outBuff),0);               \
     emit_callhelper_I4(FJit_pHlpMonExit);         \
}
#endif

#ifndef emit_ENTER_CRIT_STATIC
#define emit_ENTER_CRIT_STATIC(methodHandle)       \
{                                                  \
     callInfo.reset();                             \
     emit_arg( methodHandle, 1, EXTERNAL_CALL );   \
     LABELSTACK((outPtr-outBuff),0);               \
     emit_callhelper_I4(FJit_pHlpMonEnterStatic);  \
}
#endif

#ifndef emit_EXIT_CRIT_STATIC
#define emit_EXIT_CRIT_STATIC(methodHandle) \
{                                                  \
     callInfo.reset();                             \
     emit_arg( methodHandle, 1, EXTERNAL_CALL );   \
     LABELSTACK((outPtr-outBuff),0);               \
     emit_callhelper_I4(FJit_pHlpMonExitStatic);   \
}
#endif

#ifndef emit_CASTCLASS
#define emit_CASTCLASS(targetClass, jit_helper)    \
{                                                  \
     callInfo.reset();                             \
     emit_tos_arg( 2, EXTERNAL_CALL );             \
     emit_arg( targetClass, 1, EXTERNAL_CALL );    \
     LABELSTACK((outPtr-outBuff),0);               \
     emit_callhelper_I4I4(jit_helper);             \
     emit_pushresult_Ptr();                        \
}
#endif

#ifndef emit_ISINST
#define emit_ISINST(targetClass, jit_helper)       \
{                                                  \
     callInfo.reset();                             \
     emit_tos_arg( 2, EXTERNAL_CALL );             \
     emit_arg( targetClass, 1, EXTERNAL_CALL );    \
     LABELSTACK((outPtr-outBuff),0);               \
     emit_callhelper_I4I4(jit_helper);             \
     emit_pushresult_I4();                         \
}
#endif

#ifndef emit_BOXVAL
#define emit_BOXVAL(cls, clsSize, helper_ftn)      \
{                                                  \
   deregisterTOS;                                  \
   callInfo.reset();                               \
   if (!STACK_BUFFER)                              \
      { emit_reg_to_arg(2, SP, EXTERNAL_CALL); }   \
   else                                            \
   {                                               \
      emit_getSP(STACK_BUFFER);                    \
      emit_reg_to_arg(2, TOS_REG_1, EXTERNAL_CALL);\
      inRegTOS = false;                            \
   }                                               \
   if (bigEndianOffset(clsSize))                   \
   {                                               \
      add_constant(ARG_2, bigEndianOffset(clsSize));\
   }                                               \
   emit_arg( cls, 1, EXTERNAL_CALL );              \
   LABELSTACK((outPtr-outBuff),0);                 \
   emit_callhelper_I4I4_I4(helper_ftn);            \
   emit_drop(BYTE_ALIGNED(clsSize));               \
   emit_pushresult_Ptr();                          \
}
#endif

#ifndef emit_UNBOX
#define emit_UNBOX(cls, helper_ftn)                \
{                                                  \
     callInfo.reset();                             \
     emit_tos_arg( 2, EXTERNAL_CALL );             \
     emit_arg( cls, 1, EXTERNAL_CALL );            \
     LABELSTACK((outPtr-outBuff),0);               \
     emit_callhelper_I4I4_I4(helper_ftn);          \
     emit_pushresult_Ptr();                        \
}
#endif

#ifndef emit_sequence_point_marker
#define emit_sequence_point_marker() nop()
#endif




/**************************************************************************
   debugging/profiling and logging macros
**************************************************************************/

#ifndef emit_justmycode_callback
#define emit_justmycode_callback( dbgHandle, pDbgHandle )                                           \
{   /* Depending on what value is provided dereference either once or twice */                      \
    if (dbgHandle)                                                                                  \
    {                                                                                               \
        emit_WIN32(emit_LDC_I4(dbgHandle)) emit_WIN64(emit_LDC_I8(dbgHandle)) ;                     \
    }                                                                                               \
    else                                                                                            \
    {                                                                                               \
        emit_WIN32(emit_LDC_I4(pDbgHandle)) emit_WIN64(emit_LDC_I8(pDbgHandle)) ;                   \
        emit_LDIND_PTR(false);                                                                      \
    }                                                                                               \
    emit_LDIND_PTR(false);                                                                          \
    /* Test if the value is NULL */                                                                 \
    emit_testTOS();                                                                                 \
    BYTE * label_1 = outPtr;                                                                        \
    jmp_abs_address(CondZero, 0, false);                                                            \
    callInfo.reset();                                                                               \
    emit_callhelper_(FJit_pDbgIsJustMyCode);                                                        \
    /* Patch the jump address, since the loop size is now known */                                  \
    FJitResult PatchResult;                                                                         \
    PatchResult = FJIT_OK;                                                                          \
    patch_address( label_1, outPtr, storedStartIP, PatchResult );                                            \
    _ASSERTE( PatchResult == FJIT_OK );                                                             \
}
#endif

#ifndef emit_callhelper_prof3
#define emit_callhelper_prof3(fcn, id, arg1, arg2, arg3)  \
{   /* On x86 custom call. conv  is used to match V1 interface */ \
    ON_X86_ONLY(_ASSERTE(false);)                   \
    callInfo.reset();                               \
    emit_arg(arg3, 3, INTERNAL_CALL);               \
    emit_arg(arg2, 2, INTERNAL_CALL);               \
    emit_arg(arg1, 1, INTERNAL_CALL);               \
    emit_callhelper_I4I4I4(fcn);                    \
}
#endif

#ifndef emit_callhelper_prof4
#define emit_callhelper_prof4(fcn, id, arg1, arg2, arg3, arg4)  \
{   /* On x86 custom call. conv  is used to match V1 interface */ \
    ON_X86_ONLY(_ASSERTE(false);)                   \
    callInfo.reset();                               \
    emit_arg(arg4, 4, INTERNAL_CALL);               \
    emit_arg(arg3, 3, INTERNAL_CALL);               \
    emit_arg(arg2, 2, INTERNAL_CALL);               \
    emit_arg(arg1, 1, INTERNAL_CALL);               \
    emit_callhelper_I4I4I4I4(fcn);                  \
}
#endif

#ifdef LOGGING

extern ICorJitInfo* logCallback;                // where to send the logging mesages

#define emit_log_opcode(il, opcode, TOSstate)   \
{                                               \
    deregisterTOS;                              \
    callInfo.reset();                           \
    emit_reg_arg(SP, 5, INTERNAL_CALL );        \
    emit_reg_arg(FP, 4, INTERNAL_CALL );        \
    emit_arg( il, 3, INTERNAL_CALL );           \
    emit_arg( opcode, 2, INTERNAL_CALL );       \
    emit_arg( TOSstate, 1, INTERNAL_CALL );     \
    emit_callhelper_I4I4I4I4I4(log_opcode_helper);\
    if (TOSstate) {                             \
        enregisterTOS;                          \
    }                                           \
}

#ifdef DECLARE_HELPERS
void HELPER_CALL log_opcode_helper(bool TOSstate, unsigned short opcode, unsigned short il, unsigned framePtr, unsigned* stackPtr) {
    logMsg(logCallback, LL_INFO100, "ESP:%1s%8x[%8x:%8x:%8x] EBP:%8x IL:%4x %s \n",
        (TOSstate? "+" :""),
        (unsigned) stackPtr,
        stackPtr[0], stackPtr[1], stackPtr[2],
        framePtr, il, opname[opcode]
        );
}
#endif // DECLARE_HELPERS

#define emit_log_entry(szDebugClassName, szDebugMethodName)     \
{                                                               \
    inRegTOS = false;                                           \
    callInfo.reset();                                           \
    emit_arg( szDebugClassName, 2, INTERNAL_CALL );             \
    emit_arg( szDebugMethodName, 1, INTERNAL_CALL );            \
    emit_callhelper_I4I4(log_entry_helper);                     \
}
#ifdef DECLARE_HELPERS
void HELPER_CALL log_entry_helper(const char * szDebugMethodName, const char * szDebugClassName) {
    logMsg(logCallback, LL_INFO10, "{ entering %s::%s\n", szDebugClassName, szDebugMethodName);
}
#endif // DECLARE_HELPERS

#define emit_log_exit(szDebugClassName, szDebugMethodName)      \
{                                                               \
    inRegTOS = false;                                           \
    callInfo.reset();                                           \
    emit_arg( szDebugClassName, 2, INTERNAL_CALL );             \
    emit_arg( szDebugMethodName, 1, INTERNAL_CALL );            \
    emit_callhelper_I4I4(log_exit_helper);                      \
}
#ifdef DECLARE_HELPERS
void HELPER_CALL log_exit_helper(const char * szDebugMethodName, const char * szDebugClassName) {
    logMsg(logCallback, LL_INFO10, "} leaving %s::%s \n", szDebugClassName, szDebugMethodName);
}
#endif // DECLARE_HELPERS

#endif // LOGGING

#endif // FJITDEF_H

