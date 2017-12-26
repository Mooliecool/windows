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
// File: exprkind.h
//
// Types of nodes in the bound expression tree
// ===========================================================================

#if !defined(EXPRDEF)
#error Must define EXPRDEF macro before including exprkind.h
#endif

//      EK_xxx value      | asXXX operator

// Statements come first
EXPRDEF(BLOCK)
EXPRDEF(STMTAS)
EXPRDEF(RETURN)
EXPRDEF(DECL)
EXPRDEF(LABEL)
EXPRDEF(GOTO)
EXPRDEF(GOTOIF)
EXPRDEF(SWITCH)
EXPRDEF(SWITCHLABEL)
EXPRDEF(TRY)
EXPRDEF(HANDLER)
EXPRDEF(THROW)
EXPRDEF(NOOP)
EXPRDEF(DEBUGNOOP)
EXPRDEF(DELIM)

// Now expressions. Keep BINOP first!
EXPRDEF(BINOP)
EXPRDEF(CALL)
EXPRDEF(EVENT)
EXPRDEF(FIELD)
EXPRDEF(LOCAL)
EXPRDEF(CONSTANT)
EXPRDEF(CLASS)
EXPRDEF(NSPACE)
EXPRDEF(ERROR)
EXPRDEF(FUNCPTR)
EXPRDEF(PROP)
EXPRDEF(MULTI)
EXPRDEF(MULTIGET)
EXPRDEF(STTMP)
EXPRDEF(LDTMP)
EXPRDEF(FREETMP)
EXPRDEF(WRAP)
EXPRDEF(CONCAT)
EXPRDEF(ARRINIT)
EXPRDEF(CAST)
EXPRDEF(TYPEOF)
EXPRDEF(SIZEOF)
EXPRDEF(ZEROINIT)
EXPRDEF(USERLOGOP)
EXPRDEF(MEMGRP)
EXPRDEF(ANONMETH)
EXPRDEF(DBLQMARK)

#undef EXPRDEF

/***************************************************************************************************
    Ones below here are not used to create actual expr types, only EK_ values.
***************************************************************************************************/

#ifndef EXPRKINDDEF
#define EXPRKINDDEF(ek)
#endif // !EXPRKINDDEF

#ifndef EXPRKIND_EXTRA
#define EXPRKIND_EXTRA(ek)
#endif // !EXPRKIND_EXTRA

EXPRKINDDEF(COUNT)

EXPRKINDDEF(LIST)
EXPRKINDDEF(ASSG)

EXPRKINDDEF(MAKERA)
EXPRKINDDEF(VALUERA)
EXPRKINDDEF(TYPERA)

EXPRKINDDEF(ARGS)

EXPRKINDDEF(EQUALS)        // this is only used as a parameter, no actual exprs are constructed with it
EXPRKIND_EXTRA(EK_FIRSTOP = EK_EQUALS)
EXPRKINDDEF(COMPARE)       // this is only used as a parameter, no actual exprs are constructed with it

EXPRKINDDEF(TRUE)
EXPRKINDDEF(FALSE)

EXPRKINDDEF(INC)       // this is only used as a parameter, no actual exprs are constructed with it
EXPRKINDDEF(DEC)       // this is only used as a parameter, no actual exprs are constructed with it

EXPRKINDDEF(LOGNOT)

// keep EK_EQ to EK_GE in the same sequence (ILGENREC::genCondBranch)
EXPRKINDDEF(EQ)
EXPRKIND_EXTRA(EK_RELATIONAL_MIN = EK_EQ)
EXPRKINDDEF(NE)
EXPRKINDDEF(LT)
EXPRKINDDEF(LE)
EXPRKINDDEF(GT)
EXPRKINDDEF(GE)
EXPRKIND_EXTRA(EK_RELATIONAL_MAX = EK_GE)

// keep EK_ADD to EK_ARRLEN in the same sequence (ILGENREC::genBinopExpr)
EXPRKINDDEF(ADD)
EXPRKIND_EXTRA(EK_ARITH_MIN = EK_ADD)
EXPRKINDDEF(SUB)
EXPRKINDDEF(MUL)
EXPRKINDDEF(DIV)
EXPRKINDDEF(MOD)
EXPRKINDDEF(NEG)
EXPRKINDDEF(UPLUS)
EXPRKIND_EXTRA(EK_ARITH_MAX = EK_UPLUS)

EXPRKINDDEF(BITAND)
EXPRKIND_EXTRA(EK_BIT_MIN = EK_BITAND)
EXPRKINDDEF(BITOR)
EXPRKINDDEF(BITXOR)
EXPRKINDDEF(BITNOT)
EXPRKIND_EXTRA(EK_BIT_MAX = EK_BITNOT)

EXPRKINDDEF(LSHIFT)
EXPRKINDDEF(RSHIFT)
EXPRKINDDEF(ARRLEN)
// keep EK_ADD to EK_ARRLEN in the same sequence (ILGENREC::genBinopExpr)

EXPRKINDDEF(LOGAND)
EXPRKINDDEF(LOGOR)

EXPRKINDDEF(IS)
EXPRKINDDEF(AS)
EXPRKINDDEF(ARRINDEX)
EXPRKINDDEF(NEWARRAY)
EXPRKINDDEF(QMARK)
EXPRKINDDEF(SEQUENCE)    // p1 is side effects, p2 is values
EXPRKINDDEF(SEQREV)      // p1 is values, p2 is side effects
EXPRKINDDEF(SAVE)        // p1 is expr, p2 is wrap to be saved into...
EXPRKINDDEF(SWAP)

EXPRKINDDEF(ARGLIST)

EXPRKINDDEF(INDIR)
EXPRKINDDEF(ADDR)
EXPRKINDDEF(LOCALLOC)

#undef EXPRKINDDEF
#undef EXPRKIND_EXTRA
