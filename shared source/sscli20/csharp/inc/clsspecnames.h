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
// File: clsspecnames.h
//
// This header contains the mapping of C# names to CLS names.
// To pull out significant pieces of each entry, define CLSSPECNAMEDEF appropriately before including this file.
//
//               C# Name              Metadata Name                 is cast     operator type
// ===========================================================================

// CLS method names for user defined operators
CLSSPECNAMEDEF(L"operator +",       L"op_UnaryPlus",                false, OP_UPLUS)
CLSSPECNAMEDEF(L"operator -",       L"op_UnaryNegation",            false, OP_NEG)
CLSSPECNAMEDEF(L"operator ~",       L"op_OnesComplement",           false, OP_BITNOT)
CLSSPECNAMEDEF(L"operator ++",      L"op_Increment",                false, OP_PREINC)
CLSSPECNAMEDEF(L"operator --",      L"op_Decrement",                false, OP_PREDEC)
CLSSPECNAMEDEF(L"operator +",       L"op_Addition",                 false, OP_ADD)
CLSSPECNAMEDEF(L"operator -",       L"op_Subtraction",              false, OP_SUB)
CLSSPECNAMEDEF(L"operator *",       L"op_Multiply",                 false, OP_MUL)
CLSSPECNAMEDEF(L"operator /",       L"op_Division",                 false, OP_DIV)
CLSSPECNAMEDEF(L"operator %",       L"op_Modulus",                  false, OP_MOD)
CLSSPECNAMEDEF(L"operator ^",       L"op_ExclusiveOr",              false, OP_BITXOR)
CLSSPECNAMEDEF(L"operator &",       L"op_BitwiseAnd",               false, OP_BITAND)
CLSSPECNAMEDEF(L"operator |",       L"op_BitwiseOr",                false, OP_BITOR)
CLSSPECNAMEDEF(L"operator <<",      L"op_LeftShift",                false, OP_LSHIFT)
CLSSPECNAMEDEF(L"operator >>",      L"op_RightShift",               false, OP_RSHIFT)
//CLSSPECNAMEDEF(PN_OPEQUALS,       L"op_Equals",                   false)
//CLSSPECNAMEDEF(PN_OPCOMPARE,      L"op_Compare",                  false)

CLSSPECNAMEDEF(L"operator ==",      L"op_Equality",                 false, OP_EQ)
CLSSPECNAMEDEF(L"operator !=",      L"op_Inequality",               false, OP_NEQ)
CLSSPECNAMEDEF(L"operator >",       L"op_GreaterThan",              false, OP_GT)
CLSSPECNAMEDEF(L"operator <",       L"op_LessThan",                 false, OP_LT)
CLSSPECNAMEDEF(L"operator >=",      L"op_GreaterThanOrEqual",       false, OP_GE)
CLSSPECNAMEDEF(L"operator <=",      L"op_LessThanOrEqual",          false, OP_LE)
CLSSPECNAMEDEF(L"operator true",    L"op_True",                     false, OP_TRUE)
CLSSPECNAMEDEF(L"operator false",   L"op_False",                    false, OP_FALSE)
CLSSPECNAMEDEF(L"operator !",       L"op_LogicalNot",               false, OP_LOGNOT)
CLSSPECNAMEDEF(L"operator implicit",    L"op_Implicit",             true,  OP_IMPLICIT)
CLSSPECNAMEDEF(L"operator explicit",    L"op_Explicit",             true,  OP_EXPLICIT)

#undef  CLSSPECNAMEDEF
