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
// Desription:
//
// 

#ifndef _PPCFJIT_H_
#define _PPCFJIT_H_

#ifdef _PPC_

// List of macros re-defined for performance
#define emit_mov_arg_reg(offset, reg)  ppc_mov_arg_reg(offset, reg)
#define emit_LDVARA(offset, size)      ppc_LDVARA(offset, size)
#define emit_LDVAR_I4(offset)          ppc_LDVAR_I4(offset) 
#define emit_STVAR_I4(offset)          ppc_STVAR_I4(offset)

/****************************************************************************/
// Performance macros 
#define ppc_LDVAR_I4(offset)                                \
{                                                           \
   deregisterTOS;                                           \
   if ( !FitsInI2((unsigned)offset) )                       \
   {                                                        \
     mov_register(TOS_REG_1,FP);                            \
     add_constant(TOS_REG_1,(int)offset);                   \
     ppc_lwz( TOS_REG_1, TOS_REG_1, 0);                     \
   }                                                        \
   else                                                     \
   {                                                        \
     ppc_lwz( TOS_REG_1, FP, (offset&0xFFFF));              \
   }                                                        \
   inRegTOS = true;                                         \
}


#define ppc_STVAR_I4(offset)                                \
{                                                           \
   enregisterTOS;                                           \
   if ( !FitsInI2((unsigned)offset) )                       \
   {                                                        \
     mov_register(ARG_1,FP);                                \
     add_constant(ARG_1,(int)offset);                       \
     ppc_stw( TOS_REG_1, ARG_1, 0);                         \
   }                                                        \
   else                                                     \
   {                                                        \
     ppc_stw( TOS_REG_1, FP, (offset&0xFFFF));              \
   }                                                        \
   inRegTOS = false;                                        \
}


#define ppc_LDVARA(offset, size)                       \
{                                                      \
   unsigned offset_i = offset + bigEndianOffset(size); \
   deregisterTOS;                                      \
   if ( !FitsInI2((unsigned)offset_i) )                \
   {                                                   \
     mov_register(TOS_REG_1,FP);                       \
     add_constant(TOS_REG_1,(int)offset_i);            \
   }                                                   \
   else                                                \
   {                                                   \
     ppc_addi(TOS_REG_1, FP,(offset_i & 0xFFFF));      \
   }                                                   \
   inRegTOS = true;                                    \
} 

#define ppc_mov_arg_reg(offset,reg)                         \
{                                                           \
   _ASSERTE( inRegTOS == false);                            \
   if ( !FitsInI2((unsigned)offset) )                       \
   {                                                        \
     mov_register(TOS_REG_1,SP);                            \
     add_constant(TOS_REG_1,(int)offset);                   \
     ppc_lwz( argRegistersMap[reg], TOS_REG_1, 0);          \
   }                                                        \
   else                                                     \
   {                                                        \
     ppc_lwz( argRegistersMap[reg], SP, (offset&0xFFFF));   \
   }                                                        \
}


#endif //_PPC_

#endif //_PPCFJIT_H_
