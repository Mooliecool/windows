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
// Defines the layout of the PowerPC machine instructions using Cmd structures.
// Note that the code here ONLY deals with PowerPC specific information
//

#ifndef _PPCDEF_H_
#define _PPCDEF_H_

#ifdef _PPC_

#define DBG_CG(exp)  // Set to 'exp' to mirror the codegen to the console

/***************************************************************************/
/* The general purpose register numbering */

#define R0  0
#define R1  1
#define R2  2
#define R3  3
#define R4  4
#define R5  5
#define R6  6
#define R7  7
#define R8  8
#define R9  9
#define R10 10
#define R11 11
#define R12 12
#define R13 13
#define R14 14
#define R15 15
#define R16 16
#define R17 17
#define R18 18
#define R19 19
#define R20 20
#define R21 21
#define R22 22
#define R23 23
#define R24 24
#define R25 25
#define R26 26
#define R27 27
#define R28 28
#define R29 29
#define R30 30
#define R31 31

/* The floating point register numbering */
#define FR1 1
#define FR2 2

/***************************************************************************
  The following macros simplify access to the information encoded in a PowerPC
  opcode.
***************************************************************************/

/* Generic macro for reading/writing a multi-bit field */
#define R_VALUE(inst, s, e )     ( (((unsigned)inst) << s) >> (31-(e-s)) )
#define W_VALUE(val, s, e )      ( (((unsigned)val) << (31-s)) & ((1 << (32-e))-1) )
/* Generic macro for reading/writing a single-bit field */
#define R_BIT(inst, p)           ( (((unsigned)inst) << p) >> 31 )
#define W_BIT(bit, p)            ( (((unsigned)bit) << (31-p)) )

/* Read/write [0..5] of the instruction - opcode*/
#define R_PPC_OPCODE(inst)       ( ((unsigned)inst) >> 26 )
#define W_PPC_OPCODE(opcode)     ( ((unsigned)opcode) << 26 )
/* Read/write [6..10] of the instruction - rS or rD */
#define R_REGD(inst)             ( (((unsigned)inst) << 6) >> 27 )
#define W_REGD(reg)              ( ((unsigned)reg) << 21 )
/* Read/write [11..15] of the instruction - rA */
#define R_REGA(inst)             ( (((unsigned)inst) << 11) >> 27 )
#define W_REGA(reg)              ( ((unsigned)reg) << 16 )
/* Read/write [16..20] of the instruction - rB */
#define R_REGB(inst)             ( (((unsigned)inst) << 16) >> 27 )
#define W_REGB(reg)              ( ((unsigned)reg) << 11 )
/* Read/write bit RC */
#define R_RC_BIT(inst)           ( ((unsigned)inst) & 1 )
#define W_RC_BIT(bit)            bit

/////////////////////////////////////////////////////

#define FitsInI1(i)     ( ((__int32)(i)) == (__int32)(__int8)(i) )
#define FitsInI2(i)     ( ((__int32)(i)) == (__int32)(__int16)(i) )

#define FitsInSimm26(i) FitsInI1(((__int32)(i)) >> (26 - 8))

/////////////// Store/Load //////////////////////////

#define ppc_stb( regS, regA, offset )             {           \
DBG_CG(printf("stb R%d, %d(R%d)\t\t", regS, offset, regA  );)             \
_ASSERTE(regS < 32 && regA < 32 && offset <= 0xFFFF );       \
cmdDWord(expOr4( W_PPC_OPCODE(38), W_REGD(regS), W_REGA(regA), (offset & 0xFFFF) ) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); )}

#define ppc_sth( regS, regA, offset )          {       \
DBG_CG(printf("sth R%d, %d(R%d)\t\t", regS, offset, regA  ); )            \
_ASSERTE(regS < 32 && regA < 32 && offset <= 0xFFFF );       \
cmdDWord(expOr4( W_PPC_OPCODE(44), W_REGD(regS), W_REGA(regA), (offset & 0xFFFF) ) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); )}

#define ppc_stw( regS, regA, offset )          {       \
DBG_CG(printf("stw R%02d, %3d(R%02d)\t\t", regS,  offset, regA ); )            \
_ASSERTE(regS < 32 && regA < 32 && offset <= 0xFFFF );       \
cmdDWord(expOr4( W_PPC_OPCODE(36), W_REGD(regS), W_REGA(regA), (offset & 0xFFFF) ) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); )}

#define ppc_stwu( regS, regA, offset )      {           \
DBG_CG(printf("stwu R%2d, %3d(R%02d)\t", regS,  offset, regA );)             \
_ASSERTE(regS < 32 && regA < 32 && offset <= 0xFFFF );       \
cmdDWord(expOr4( W_PPC_OPCODE(37), W_REGD(regS), W_REGA(regA), (offset & 0xFFFF) ) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) );)}

#define ppc_stfs( regS, regA, offset )          {       \
DBG_CG(printf("stfs FR%02d, %3d(R%02d)\t\t", regS,  offset, regA ); )            \
_ASSERTE(regS < 32 && regA < 32 && offset <= 0xFFFF );       \
cmdDWord(expOr4( W_PPC_OPCODE(52), W_REGD(regS), W_REGA(regA), (offset & 0xFFFF) ) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); )}

#define ppc_stfsu( regS, regA, offset )          {       \
DBG_CG(printf("stfsu FR%02d, %3d(R%02d)\t\t", regS,  offset, regA ); )            \
_ASSERTE(regS < 32 && regA < 32 && offset <= 0xFFFF );       \
cmdDWord(expOr4( W_PPC_OPCODE(53), W_REGD(regS), W_REGA(regA), (offset & 0xFFFF) ) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); )}

#define ppc_stfd( regS, regA, offset )          {       \
DBG_CG(printf("stfd FR%02d, %3d(R%02d)\t\t", regS,  offset, regA ); )            \
_ASSERTE(regS < 32 && regA < 32 && offset <= 0xFFFF );       \
cmdDWord(expOr4( W_PPC_OPCODE(54), W_REGD(regS), W_REGA(regA), (offset & 0xFFFF) ) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); )}

#define ppc_stfdu( regS, regA, offset )          {       \
DBG_CG(printf("stfdu FR%02d, %3d(R%02d)\t\t", regS,  offset, regA ); )            \
_ASSERTE(regS < 32 && regA < 32 && offset <= 0xFFFF );       \
cmdDWord(expOr4( W_PPC_OPCODE(55), W_REGD(regS), W_REGA(regA), (offset & 0xFFFF) ) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); )}

#define ppc_lbz( regD, regA, offset )         {         \
DBG_CG(printf("lbz R%d, %d(R%d)\t\t", regD, offset, regA );)             \
_ASSERTE(regD < 32 && regA < 32 && offset <= 0xFFFF );       \
cmdDWord(expOr4( W_PPC_OPCODE(34), W_REGD(regD), W_REGA(regA), (offset & 0xFFFF) ) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

#define ppc_lha( regD, regA, offset )        {          \
DBG_CG(printf("lha R%d, %d(R%d)\t\t", regD, offset, regA );)             \
_ASSERTE(regD < 32 && regA < 32 && offset <= 0xFFFF );       \
cmdDWord(expOr4( W_PPC_OPCODE(42), W_REGD(regD), W_REGA(regA), (offset & 0xFFFF) ) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

#define ppc_lhz( regD, regA, offset )         {         \
DBG_CG(printf("lhz R%d, %d(R%d)\t\t", regD, offset, regA ); )            \
_ASSERTE(regD < 32 && regA < 32 && offset <= 0xFFFF );       \
cmdDWord(expOr4( W_PPC_OPCODE(40), W_REGD(regD), W_REGA(regA), (offset & 0xFFFF) ) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

#define ppc_lwz( regD, regA, offset )         {         \
DBG_CG(printf("lwz R%d, %d(R%d)\t\t", regD, offset, regA );)             \
_ASSERTE(regD < 32 && regA < 32 && offset <= 0xFFFF );       \
cmdDWord(expOr4( W_PPC_OPCODE(32), W_REGD(regD), W_REGA(regA), (offset & 0xFFFF) ) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

#define ppc_lfs( regD, regA, offset )         {         \
DBG_CG(printf("lfs FR%d, %d(R%d)\t\t", regD, offset, regA );)             \
_ASSERTE(regD < 32 && regA < 32 && offset <= 0xFFFF );       \
cmdDWord(expOr4( W_PPC_OPCODE(48), W_REGD(regD), W_REGA(regA), (offset & 0xFFFF) ) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

#define ppc_lfd( regD, regA, offset )         {         \
DBG_CG(printf("lfd FR%d, %d(R%d)\t\t", regD, offset, regA );)             \
_ASSERTE(regD < 32 && regA < 32 && offset <= 0xFFFF );       \
cmdDWord(expOr4( W_PPC_OPCODE(50), W_REGD(regD), W_REGA(regA), (offset & 0xFFFF) ) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

///////////// Arithmetic Operations  ////////////////////
#define ppc_add( regA, regD, regB, Rc )       {          \
DBG_CG(printf("add R%d, R%d, R%d (%d)\t\t", regA, regD, regB, Rc );)             \
_ASSERTE(regD < 32 && regA < 32 && regB < 32 && (Rc == 0 || Rc == 1) );       \
cmdDWord(expOr4( W_PPC_OPCODE(31), W_REGD(regD), W_REGA(regA), W_REGB(regB) ) | 532 | W_RC_BIT(Rc) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

#define ppc_addi( regD, regA, immd )       {            \
DBG_CG(printf("addi R%d, R%d, %d\t\t", regD, regA, immd );)            \
_ASSERTE(regD < 32 && regA < 32 && (immd) <= 0xFFFF );       \
cmdDWord(expOr4( W_PPC_OPCODE(14), W_REGD(regD), W_REGA(regA), (immd & 0xFFFF) ) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

#define ppc_addis( regD, regA, immd )        {           \
DBG_CG(printf("addis R%d, R%d, %d\t\t", regD, regA, immd );)            \
_ASSERTE(regD < 32 && regA < 32 && immd <= 0xFFFF );       \
cmdDWord(expOr4( W_PPC_OPCODE(15), W_REGD(regD), W_REGA(regA), (immd & 0xFFFF) ) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

#define ppc_subf( regD, regA, regB, OE, Rc )      {           \
DBG_CG(printf("subf R%d, R%d, R%d\t\t", regD, regA, regB );)             \
_ASSERTE(regD < 32 && regA < 32 && (OE == 0 || OE == 1) && (Rc == 0 || Rc == 1) );       \
cmdDWord(expOr4( W_PPC_OPCODE(31), W_REGD(regD), W_REGA(regA), W_REGB(regB) ) | W_BIT(OE,21) | 80 | W_RC_BIT(Rc) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }
//////////// Logical Operations and Shifts ////////////

#define ppc_andi( regA, regS, immd )          {         \
DBG_CG(printf("addi R%d, R%d, %d\t\t", regA, regS, immd );)            \
_ASSERTE(regS < 32 && regA < 32 && (immd) <= 0xFFFF );       \
cmdDWord(expOr4( W_PPC_OPCODE(28), W_REGD(regS), W_REGA(regA), (immd & 0xFFFF) ) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

#define ppc_andis( regA, regS, immd )         {          \
DBG_CG(printf("addis R%d, R%d, %d\t\t", regA, regS, immd );)            \
_ASSERTE(regS < 32 && regA < 32 && (immd) <= 0xFFFF );       \
cmdDWord(expOr4( W_PPC_OPCODE(29), W_REGD(regS), W_REGA(regA), (immd & 0xFFFF) ) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

#define ppc_and( regA, regS, regB, Rc )       {          \
DBG_CG(printf("and R%d, R%d, R%d (%d)\t\t", regA, regS, regB, Rc );)             \
_ASSERTE(regS < 32 && regA < 32 && regB < 32 && (Rc == 0 || Rc == 1) );       \
cmdDWord(expOr4( W_PPC_OPCODE(31), W_REGD(regS), W_REGA(regA), W_REGB(regB) ) | 56 | W_RC_BIT(Rc) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

#define ppc_or( regA, regD, regB, Rc )        {         \
DBG_CG(printf("or R%d, R%d, R%d\t\t", regA, regD, regB );)             \
_ASSERTE(regD < 32 && regA < 32 && regB < 32 && (Rc == 0 || Rc == 1) );       \
cmdDWord(expOr4( W_PPC_OPCODE(31), W_REGD(regD), W_REGA(regA), W_REGB(regB) ) | 888 | W_RC_BIT(Rc) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

#define ppc_ori(  regA, regD, immd )         {           \
DBG_CG(printf("ori R%d, R%d, %d\t\t", regA, regD, immd );)            \
_ASSERTE(regD < 32 && regA < 32 && (immd) <= 0xFFFF );       \
cmdDWord(expOr4( W_PPC_OPCODE(24), W_REGD(regD), W_REGA(regA), (immd & 0xFFFF) ) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

#define ppc_oris(  regA, regD, immd )         {           \
DBG_CG(printf("oris R%d, R%d, %d\t\t", regA, regD, immd );)            \
_ASSERTE(regD < 32 && regA < 32 && (immd) <= 0xFFFF );       \
cmdDWord(expOr4( W_PPC_OPCODE(25), W_REGD(regD), W_REGA(regA), (immd & 0xFFFF) ) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

#define ppc_slw( regA, regS, regB, Rc )        {         \
DBG_CG(printf("slw R%d, R%d, R%d\t\t", regA, regS, regB );)             \
_ASSERTE(regS < 32 && regA < 32 && regB < 32 && (Rc == 0 || Rc == 1) );       \
cmdDWord(expOr4( W_PPC_OPCODE(31), W_REGD(regS), W_REGA(regA), W_REGB(regB) ) | 48 | W_RC_BIT(Rc) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

#define ppc_srw( regA, regS, regB, Rc )        {         \
DBG_CG(printf("srw R%d, R%d, R%d\t\t", regA, regS, regB ); )            \
_ASSERTE(regS < 32 && regA < 32 && regB < 32 && (Rc == 0 || Rc == 1) );       \
cmdDWord(expOr4( W_PPC_OPCODE(31), W_REGD(regS), W_REGA(regA), W_REGB(regB) ) | 1072 | W_RC_BIT(Rc) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

/////////////// Branching operations ///////////////////

// Fields of the condition register

#define ppc_FALSE_CR        (unsigned int)0x10
#define ppc_TRUE_CR         (unsigned int)0x00
#define ppc_SIGNED_CR       (unsigned int)0x00
#define ppc_UNSIGNED_CR     (unsigned int)0x20
#define ppc_CR_LT           (unsigned int)0x00
#define ppc_CR_GT           (unsigned int)0x01
#define ppc_CR_EQ           (unsigned int)0x02
#define ppc_CR_NOTUSED      (unsigned int)0x0F
#define ppcCondBelow        ( ppc_UNSIGNED_CR | ppc_TRUE_CR  | ppc_CR_LT )
#define ppcCondAboveEq      ( ppc_UNSIGNED_CR | ppc_FALSE_CR | ppc_CR_LT )
#define ppcCondBelowEq      ( ppc_UNSIGNED_CR | ppc_FALSE_CR | ppc_CR_GT )
#define ppcCondAbove        ( ppc_UNSIGNED_CR | ppc_TRUE_CR  | ppc_CR_GT )
#define ppcCondEq           ( ppc_SIGNED_CR | ppc_TRUE_CR  | ppc_CR_EQ )
#define ppcCondNotEq        ( ppc_SIGNED_CR | ppc_FALSE_CR | ppc_CR_EQ )
#define ppcCondLt           ( ppc_SIGNED_CR | ppc_TRUE_CR  | ppc_CR_LT )
#define ppcCondGtEq         ( ppc_SIGNED_CR | ppc_FALSE_CR | ppc_CR_LT )
#define ppcCondLtEq         ( ppc_SIGNED_CR | ppc_FALSE_CR | ppc_CR_GT )
#define ppcCondGt           ( ppc_SIGNED_CR | ppc_TRUE_CR  | ppc_CR_GT )
#define ppcCondAlways       ( ppc_SIGNED_CR | ppc_FALSE_CR | ppc_CR_NOTUSED )

// Different arguments for conditional branches
#define PPC_PREDICT_TAKEN 1
#define PPC_PREDICT_NOT_TAKEN 0
#define BO_0 0  // Decrement counter, branch if counter is not 0 and condition is false
#define BO_1 2  // Decrement counter, branch if counter is 0 and condition is false
#define BO_2 4  // Branch if condition is false
#define BO_3 8  // Decrement counter, branch if counter is not 0 and condition is true
#define BO_4 10 // Decrement counter, branch if counter is 0 and condition is true
#define BO_5 12 // Branch if condition is true
#define BO_6 16 // Decrement counter, branch if counter is not 0
#define BO_7 18 // Decrement counter, branch if counter is 0
#define BO_8 20 // Branch always

// Link register options
#define SAVE_RET_ADDR      1
#define DONT_SAVE_RET_ADDR 0

#define ppc_b( LI, AA, LK )       {          \
DBG_CG(printf("b %d, AA(%d), LK(%d) \t\t", LI, AA, LK ); )           \
_ASSERTE( LI < 0xFFFFFFF && (LK == 0 || LK == 1 ) && (AA == 0 || AA == 1 ));\
cmdDWord(expOr4( W_PPC_OPCODE(18), W_VALUE(LI, 29, 6 ), W_BIT(AA,30), LK ) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); )  }

#define ppc_bc( BO, BI, BD, AA, LK, TAKEN )   {     \
DBG_CG(printf("bc %d, %d, %d, %d \t\t", BO, BI, BD, LK );)            \
_ASSERTE( BO <=20 && BI < 32 && BD <= 0x3FFF && (TAKEN == 0 || TAKEN == 1 ));\
cmdDWord(expOr4( W_PPC_OPCODE(16), W_REGD((BO & (TAKEN | 30))), W_REGA(BI), W_VALUE(BD, 29, 16 )) | W_BIT(AA,30) | LK );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

#define ppc_bclr( BO, BI, LK, TAKEN )    {   \
DBG_CG(printf("bclr %d, %d, LK(%d) \t\t", BO, BI, LK ); )           \
_ASSERTE( BO <=20 && BI < 32 && (TAKEN == 0 || TAKEN == 1 ) && (LK == 0 || LK == 1 ));\
cmdDWord(expOr4( W_PPC_OPCODE(19), W_REGD((BO & (TAKEN | 30))), W_REGA(BI), 32) | LK  );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

#define ppc_bcctr( BO, BI, LK, TAKEN )   {  \
DBG_CG(printf("bcctr %d, %d, LK(%d) \t\t", BO, BI, LK ); )           \
_ASSERTE( BO <=20 && BI < 32 && (TAKEN == 0 || TAKEN == 1 ) && (LK == 0 || LK == 1 ));\
cmdDWord(expOr4( W_PPC_OPCODE(19), W_REGD((BO & (TAKEN | 30))), W_REGA(BI), 1056) | LK );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

/////////////// Sign extension, Compare ////////////////////////

#define ppc_extsb( regS, regA, Rc )       {          \
DBG_CG(printf("extsb R%d, R%d, Rc(%d)\t\t", regS, regA, Rc );)             \
_ASSERTE(regS < 32 && regA < 32 && (Rc == 0 || Rc == 1) );       \
cmdDWord(expOr4( W_PPC_OPCODE(31), W_REGD(regS), W_REGA(regA), W_REGB(0) ) | 1908 | W_RC_BIT(Rc) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

#define ppc_frsp( regD, regB, Rc )           {          \
DBG_CG(printf("frsp R%d, R%d Rc(%d)\t\t", regB, regD, Rc );)             \
_ASSERTE(regB < 32 && regD < 32 && (Rc == 0 || Rc == 1));       \
cmdDWord(expOr4( W_PPC_OPCODE(63), W_REGD(regD), W_REGA(0), W_REGB(regB) ) | 24 | W_RC_BIT(Rc) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

#define ppc_cmp( crfD, rA, rB )      {\
DBG_CG(printf("cmp R%d, R%d, %d \t\t", rA, rB, crfD ); )            \
_ASSERTE( crfD <= 8 && rA < 32 && rB < 32);\
cmdDWord(expOr4( W_PPC_OPCODE(31), W_REGD((crfD << 2)), W_REGA(rA), W_REGB(rB) ) );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

#define ppc_cmpl( crfD, rA, rB )      {\
DBG_CG(printf("cmpl R%d, R%d, %d \t\t", rA, rB, crfD ); )            \
_ASSERTE( crfD <= 8 && rA < 32 && rB < 32);\
cmdDWord(expOr4( W_PPC_OPCODE(31), W_REGD((crfD << 2)), W_REGA(rA), W_REGB(rB) ) | 64 );\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

/////////////// Moves ////////////////////////

// Arguments for moving from/to special register accessible from user mode
#define PPC_MOVE_XER 1
#define PPC_MOVE_LR  8
#define PPC_MOVE_CTR 9

#define ppc_mfspr( regD, SPR )          {       \
DBG_CG(printf("mfspr R%d, SPR(%d)\t\t", regD, SPR ); )            \
_ASSERTE(regD < 32 && (SPR == 1 || SPR == 8 || SPR == 9) );       \
cmdDWord(expOr4( W_PPC_OPCODE(31), W_REGD(regD), W_VALUE( SPR, 15, 11 ), 678 ));\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

#define ppc_mtspr( regS, SPR )       {          \
DBG_CG(printf("mtspr R%d, SPR(%d)\t\t", regS, SPR );)             \
_ASSERTE(regS < 32 && (SPR == 1 || SPR == 8 || SPR == 9) );       \
cmdDWord(expOr4( W_PPC_OPCODE(31), W_REGD(regS), W_VALUE( SPR, 15, 11 ), 934 ));\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

#define ppc_mcrf( crfD, crfS )       {          \
DBG_CG(printf("mcrf cr%d, cr%d\t\t",  crfD, crfS  );)             \
_ASSERTE(((unsigned)(crfS)) < 8 && ((unsigned)(crfD)) < 8);       \
cmdDWord(expOr3( W_PPC_OPCODE(19), W_VALUE( ((crfD) << 2), 10, 6 ), W_VALUE( ((crfS) << 2), 15, 11 ) ) & 0xFFFF0000);\
DBG_CG(printf(": %x \n", *(((unsigned *)outPtr)-1) ); ) }

/////////////// Low level macros required by fjitdef.h //////////////////////////

#define ppc_move_constant(r, c)    \
{                                  \
  if ( !FitsInI2((unsigned)c) )                   \
  {                                \
    ppc_addis( r, R0, (((unsigned)c) >> 16));  \
    ppc_ori( r, r,((unsigned)c&0xFFFF));       \
  }                                \
  else                             \
    ppc_addi(r, R0, ((unsigned)c&0xFFFF));             \
}

#define ppc_add_constant(r, c)     \
{                                  \
  if ( !FitsInI2((unsigned)c) )                   \
  {                                \
    ppc_addis(r, r, (((unsigned)c) >> 16));    \
    ppc_addi(r, r, (((unsigned)c)&0xFFFF));  \
  }                                \
  else                             \
    { ppc_addi(r, r, (((unsigned)c)&0xFFFF));   }          \
}

#define ppc_and_constant(r, c)     \
{                                  \
  if ( ((unsigned)c) >> 16 ) \
  {                                \
    ppc_andis(r, r, (((unsigned)c) >> 16));    \
    ppc_andi(r, r, (((unsigned)c) & 0xFFFF));  \
  }                                \
  else                             \
    ppc_andi(r, r, ((unsigned)c));             \
}

#define ppc_compare_register(r1, r2, cond)  \
{                                           \
  if ( cond & ppc_UNSIGNED_CR )             \
       ppc_cmpl(0, r1, r2)                  \
  else                                      \
       ppc_cmp(0, r1, r2)                   \
}

#define ppc_ret(x, restoreAddr)    \
{                                  \
  if (x)                           \
    { ppc_add_constant(R1,x) }     \
  if ( restoreAddr )               \
  {                                \
     ppc_lwz(R12, R1, 0); ppc_addi( R1, R1, 4 );\
  }                                \
  ppc_mtspr(R12, PPC_MOVE_LR);     \
  ppc_bclr(BO_8, 0, DONT_SAVE_RET_ADDR, PPC_PREDICT_TAKEN); \
}

#define ppc_call_register(r)                            \
{                                                       \
   ppc_mtspr(r, PPC_MOVE_LR);                           \
   ppc_bclr(BO_8, 0, SAVE_RET_ADDR, PPC_PREDICT_TAKEN); \
}


#define ppc_jmp_reg(r, preserveRetAddr)                        \
{                                                              \
   if (!preserveRetAddr)                                       \
   {                                                           \
     ppc_mtspr(r, PPC_MOVE_LR);                                \
     ppc_bclr(BO_8, 0, DONT_SAVE_RET_ADDR, PPC_PREDICT_TAKEN); \
   }                                                           \
   else                                                        \
   {                                                           \
     ppc_mtspr(r, PPC_MOVE_CTR);                               \
     ppc_bcctr(BO_8, 0, DONT_SAVE_RET_ADDR, PPC_PREDICT_TAKEN); \
   }                                                           \
}


#define ppc_call_abs_address(absAddress, adj)                \
{                                                            \
  unsigned ppc_address;                                      \
  if (adj)                                                   \
    ppc_address = ((unsigned)absAddress - (unsigned)outPtr) >> 2;                 \
  else                                                       \
    ppc_address = ((unsigned)absAddress);                    \
  ppc_b(ppc_address, 0, SAVE_RET_ADDR);                       \
}

#define ppc_jmp_abs_address(cond, absAddress, adj)                                          \
{                                                                                           \
  unsigned BO = 0, BI;                                                                      \
  bool largeBranch = false;                                                                 \
  /*printf("In address: %x, Adj %d OutPtr %x , D %x\n", absAddress, adj, outPtr, ((__int32)absAddress - (__int32)outPtr) );*/                                \
  __int32 ppc_address;                                                                      \
  if (adj)                                                                                  \
  {                                                                                         \
    ppc_address = ( (__int32)((__int32)absAddress - (__int32)outPtr) >> 2);                 \
    if ( !FitsInI2((ppc_address << 2)) && ( cond & 0xF ) != ppc_CR_NOTUSED )                \
      { ppc_address = ((__int32)((__int32)absAddress - (__int32)storedStartIP)); largeBranch = true; }\
  }                                                                                         \
  else                                                                                      \
  {                                                                                         \
    ppc_address = ((__int32)absAddress);                                                    \
    largeBranch = (mapInfo.savedIP || !FitsInI2((ppc_address << 2)) )&& (cond & 0xF) != ppc_CR_NOTUSED; \
  }                                                                                         \
  /*printf("Out address: %x, Adj %d\n", ppc_address, adj );            */                   \
                                                                                            \
  /* Select the appropriate condition code or emit an uncondional branch */                 \
  if ( ( cond & 0xF ) == ppc_CR_NOTUSED )                                                   \
  {                                                                                         \
      if ( !FitsInSimm26((ppc_address << 2)) )                                              \
        { codeGenState = FJIT_INTERNALERROR; } /* Set internal error flag */                \
      ppc_b( (ppc_address & 0xFFFFFF), 0, DONT_SAVE_RET_ADDR);                              \
  }                                                                                         \
  else                                                                                      \
  {                                                                                         \
    if ( cond & ppc_FALSE_CR )                                                              \
       BO = BO_2;                                                                           \
    else                                                                                    \
       BO = BO_5;                                                                           \
    /* Figure out which bit in CR contains given condion */                                 \
    BI = ( cond & 0x0F );                                                                   \
                                                                                            \
    /* Depending on the function IL length choose code gen */                               \
    if ( largeBranch )                                                                      \
    {                                                                                       \
       if (!mapInfo.savedIP) codeGenState = FJIT_JITAGAIN;                                  \
       ppc_addis( R0, R0, ((ppc_address) >> 16) );                                          \
       ppc_ori( R0, R0, ((ppc_address) & 0xFFFF));                                          \
       ppc_add( R0, R0, R28, 0 );                                                           \
       ppc_mtspr(R0, PPC_MOVE_LR);                                                          \
       ppc_bclr(BO, BI, DONT_SAVE_RET_ADDR, PPC_PREDICT_TAKEN);                             \
    }                                                                                       \
    else                                                                                    \
       ppc_bc(BO, BI, (ppc_address & 0x3FFF), 0, DONT_SAVE_RET_ADDR, PPC_PREDICT_TAKEN);    \
  }                                                                                         \
}



#define  ppc_read_address(iStart, address, errorCode)                                       \
{                                                                                           \
  if ( R_PPC_OPCODE(*iStart)  == 16 )                                                       \
    address = (unsigned)( R_VALUE(*iStart, 16, 29 ));                                       \
  else if ( R_PPC_OPCODE(*iStart)  == 18 )                                                  \
    address = (unsigned)( R_VALUE(*iStart, 6, 29 ) );                                       \
  else if ( R_PPC_OPCODE(*iStart)  == 15 )                                                  \
    address = (unsigned)((*((unsigned *)iStart) << 16) | ( *((unsigned *)iStart + 1) & 0xFFFF) );  \
 else                                                                                       \
    {_ASSERTE(false && "Couldn't read the address");  address = 0; errorCode = FJIT_INTERNALERROR; }     \
}

#define ppc_patch_address(iStart, absAddress, storedStartIP, errorCode)                     \
{                                                                                           \
  unsigned ppc_address = ((unsigned)absAddress - (unsigned)iStart) >> 2;                    \
  unsigned * ppc_iStart  = (unsigned *)iStart;                                              \
  if (R_PPC_OPCODE(*ppc_iStart) == 16 && !FitsInI2((ppc_address << 2)))                     \
     errorCode = FJIT_JITAGAIN;                                                             \
  if (R_PPC_OPCODE(*ppc_iStart) == 18 && !FitsInSimm26((ppc_address << 2)))                 \
     errorCode = FJIT_INTERNALERROR;                                                        \
  if ( R_PPC_OPCODE(*ppc_iStart) == 16 )                                                    \
    *ppc_iStart = ( ((*ppc_iStart) & 0xFFFF0000) | W_VALUE(ppc_address, 29, 16 ) | ((*ppc_iStart) & 3) );   \
  else if ( R_PPC_OPCODE(*ppc_iStart)  == 18 )                                              \
    *ppc_iStart = ( ((*ppc_iStart) & 0xF8000000) | W_VALUE(ppc_address, 29, 6 ) | ((*ppc_iStart) & 3) );    \
 else if ( R_PPC_OPCODE(*ppc_iStart)  == 15 )                                               \
  {                                                                                         \
    _ASSERTE( storedStartIP != 0 );                                                         \
    ppc_address = ((unsigned)absAddress - (unsigned)storedStartIP);                         \
    *(ppc_iStart)   = *(ppc_iStart) & 0xFFFF0000 | ( ppc_address >> 16);                    \
    *(ppc_iStart+1) = *(ppc_iStart+1) & 0xFFFF0000 | ( ppc_address & 0xFFFF);               \
  }                                                                                         \
  else                                                                                      \
    {_ASSERTE(false && "Couldn't write the address"); errorCode = FJIT_INTERNALERROR;}      \
}

#define ppc_emit_istream_arg( arg_size, arg_ptr, reg )           \
{                                                                \
    ppc_b((((arg_size+3)>>2)+1), 0, SAVE_RET_ADDR);	         \
    for(int i = 0; i < (int)((arg_size+3)& 0xFFFFFFFC); i++) {   \
       if ( i < (int)arg_size )                                  \
        { cmdByte(((BYTE *)arg_ptr)[i]); }                       \
       else                                                      \
        { _ASSERTE( i - (int)arg_size < 3); cmdByte(0x00);}      \
    }                                                            \
    ppc_mfspr(reg, PPC_MOVE_LR);                                 \
}

#define ppc_break()                                         \
{                                                           \
    cmdDWord(0x7ef00008);                                   \
}

#define ppc_nop()                                           \
{                                                           \
    ppc_ori(R0,R0,0)                                        \
}

#define ppc_cld()                                           \
{                                                           \
    ppc_or(R0,R0,R0, 0)                                     \
}

#ifdef DECLARE_HELPERS
// this helper is implemented in ppc/ppcfjitasm.asm
extern "C" void __stdcall SWITCH_helper(void);
#endif

#define ppc_SWITCH(limit)                                   \
{                                                           \
   if (inRegTOS) { ppc_stwu(R11, R1, -4);inRegTOS = false; }\
   ppc_move_constant(R12, (HASTOCREG ? (size_t)*(void**)SWITCH_helper : (size_t)(void*)SWITCH_helper)); \
   ppc_mtspr(R12, PPC_MOVE_LR);                             \
   ppc_bclr(BO_8, 0, SAVE_RET_ADDR, PPC_PREDICT_TAKEN);     \
}

#define ppc_call_memory_indirect(c)                         \
{                                                           \
  mov_constant(R12,c);                                      \
  ppc_mtspr(R12, PPC_MOVE_LR);                              \
  ppc_bclr(BO_8, 0, SAVE_RET_ADDR, PPC_PREDICT_TAKEN);      \
}

#define ppc_setup_frame()                                   \
{                                                           \
  ppc_mfspr(R0, PPC_MOVE_LR);                               \
  ppc_stw(R0, R1, 0x8);                                     \
  if (mapInfo.savedIP )                                     \
  {                                                         \
    ppc_stw(R28, R1, 0x4);                                  \
    ppc_b( 1, 0, SAVE_RET_ADDR);                            \
    storedStartIP = outPtr;                                 \
    ppc_mfspr(R28, PPC_MOVE_LR);                            \
  }                                                         \
  ppc_stw(R30, R1, ((unsigned)(-4)&0xFFFF));                \
  ppc_or( R30, R1, R1, 0 );                                 \
  ppc_stwu(R29, R1,((unsigned)(-8)&0xFFFF));                \
}

#define ppc_restore_frame(resetupFrame)                     \
{                                                           \
  ppc_lwz(R29, R30, ((unsigned)(-8)&0xFFFF));               \
  ppc_or( R1, R30, R30, 0 );                                \
  ppc_lwz(R30, R1, ((unsigned)(-4)&0xFFFF));                \
  if (mapInfo.savedIP)                                      \
    ppc_lwz(R28, R1, 4)                                     \
  ppc_lwz(R12, R1, 8);                                      \
  if (resetupFrame)                                         \
    ppc_mtspr(R12, PPC_MOVE_LR);                            \
}

#define ppc_store_gp_arg(a_size, num, s_size)               \
{                                                           \
  _ASSERTE( s_size <= 40 && num < 8 && num >= 0);           \
  for (;a_size >= 4 && num < MAX_GP_ARG_REGISTER; a_size -= 4 ) \
  {                                                         \
     ppc_stw(argRegistersMap[num], R30, 24 + s_size);       \
     num++; s_size += 4;                                    \
  }                                                         \
  s_size += a_size;                                         \
}

#define ppc_store_float_arg(a_size, num, g_num, s_size)     \
{                                                           \
  _ASSERTE( s_size >= 0 && num < 13 && num >= 0 );          \
  _ASSERTE( a_size == 4 || a_size == 8 );                   \
  if ( a_size == 4 )                                        \
     ppc_stfs(argFloatRegistersMap[num], R30, 24 + s_size)  \
  else                                                      \
     ppc_stfd(argFloatRegistersMap[num], R30, 24 + s_size)  \
  s_size += a_size;                                         \
  num++;                                                    \
  g_num += a_size/4;                                        \
}

#define ppc_restore_gp_arg(a_size, num, s_size)             \
{                                                           \
  _ASSERTE( s_size <= 40 && num < 8 && num >= 0);           \
  for (;a_size >= 4 && num < MAX_GP_ARG_REGISTER; a_size -= 4 ) \
  {                                                         \
     ppc_lwz(argRegistersMap[num], R30, 24 + s_size);       \
     num++; s_size += 4;                                    \
  }                                                         \
  s_size += a_size;                                         \
}

#define ppc_restore_float_arg(a_size, num, g_num, s_size)   \
{                                                           \
  _ASSERTE( s_size >= 0 && num < 13 && num >= 0 );          \
  _ASSERTE( a_size == 4 || a_size == 8 );                   \
  if ( a_size == 4 )                                        \
     ppc_lfs(argFloatRegistersMap[num], R30, 24 + s_size)   \
  else                                                      \
     ppc_lfd(argFloatRegistersMap[num], R30, 24 + s_size)   \
  s_size += a_size;                                         \
  num++;                                                    \
  g_num += a_size/4;                                        \
}

#define ppc_emit_conv_R4toR8()                              \
{                                                           \
  if (inRegTOS) { ppc_stwu(R11, R1, -4);inRegTOS = false; } \
  ppc_lfs(FR1, R1, 0);                                      \
  ppc_add_constant(R1, -4);                                 \
  ppc_stfd(FR1, R1, 0 );                                    \
}

#define ppc_emit_conv_R8toR4()                              \
{                                                           \
  if (inRegTOS) { ppc_stwu(R11, R1, -4);inRegTOS = false; } \
  ppc_lfd(FR1, R1, 0);                                      \
  ppc_frsp(FR1, FR1, 0);                                    \
  ppc_add_constant(R1, 4);                                  \
  ppc_stfs(FR1, R1, 0 );                                    \
}

// Used to convert R4 value that were stored as R8 on the evaluation stack back to R4 to be passed as parameter
#define ppc_narrow_R8toR4(nativeOff,ilOff)                  \
{                                                           \
  ppc_lfd(FR1, R1, ilOff);                                  \
  ppc_frsp(FR1, FR1, 0);                                    \
  ppc_stfs(FR1, R1, nativeOff );                            \
}

// Used to move the return value from the result register (FR1) onto the evaluation stack of the caller
#define ppc_pushresult_R4()                                 \
{                                                           \
  if (inRegTOS) { ppc_stwu(R11, R1, -4);inRegTOS = false; } \
  ppc_stfdu(FR1, R1, -8 );                                  \
}
#define ppc_pushresult_R8()                                 \
{                                                           \
  if (inRegTOS) { ppc_stwu(R11, R1, -4);inRegTOS = false; } \
  ppc_stfdu(FR1, R1, -8 );                                  \
}

// Used to move the return value from the the evaluation stack of the callee into the result register (FR1)
#define ppc_emit_loadresult_R4()                            \
{                                                           \
  if (inRegTOS) { ppc_stwu(R11, R1, -4);inRegTOS = false; } \
  ppc_lfd(FR1, R1, 0);                                      \
  ppc_frsp(FR1, FR1, 0);                                    \
  ppc_add_constant(R1, 8); /* R4 is stored as R8 */         \
}
#define ppc_emit_loadresult_R8()                            \
{                                                           \
  if (inRegTOS) { ppc_stwu(R11, R1, -4);inRegTOS = false; } \
  ppc_lfd(FR1, R1, 0);                                      \
  ppc_add_constant(R1, 8);                                  \
}

#define ppc_emit_store_toc_reg(helper)                      \
{                                                           \
    if (HASTOCREG) {                                        \
        /* nothing to do for EE helpers - TOC is correct for them */ \
        if (*((void**)(helper)+1) != *((void**)FJit_pHlpInternalThrowFromHelper+1)) { \
            /* should be FJIT helper */                     \
            _ASSERTE(*((void**)(helper)+1) == *((void**)SWITCH_helper+1)); \
            ppc_stw(R2, R1, 20);                            \
            ppc_move_constant(R2, (*((void**)(helper)+1))); \
        }                                                   \
    }                                                       \
}

#define ppc_emit_restore_toc_reg(helper)                    \
{                                                           \
    if (HASTOCREG) {                                        \
        if (*((void**)(helper)+1) == *((void**)SWITCH_helper+1)) { \
            ppc_lwz(R2, R1, 20);                            \
        }                                                   \
    }                                                       \
}



#endif // _PPC_
#endif // _PPCDEF_H_
