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
// File: parsertype.h
//
// Contains a list of the interior node parsing methods
// ===========================================================================

#if !defined(PARSERDEF)
#error Must define PARSERDEF macro before including predeftyp.h
#endif

PARSERDEF(Statement)
PARSERDEF(Block)
PARSERDEF(BreakStatement)
PARSERDEF(ConstStatement)
PARSERDEF(LabeledStatement)
PARSERDEF(DoStatement)
PARSERDEF(ForStatement)
PARSERDEF(ForEachStatement)
PARSERDEF(GotoStatement)
PARSERDEF(IfStatement)
PARSERDEF(ReturnStatement)
PARSERDEF(SwitchStatement)
PARSERDEF(ThrowStatement)
PARSERDEF(TryStatement)
PARSERDEF(WhileStatement)
PARSERDEF(YieldStatement)
PARSERDEF(Declaration)
PARSERDEF(DeclarationStatement)
PARSERDEF(ExpressionStatement)
PARSERDEF(LockStatement)
PARSERDEF(FixedStatement)
PARSERDEF(UsingStatement)
PARSERDEF(CheckedStatement)
PARSERDEF(UnsafeStatement)

#undef PARSERDEF
