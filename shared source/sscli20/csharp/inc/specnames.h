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
// File: specnames.h
//
// Contains a list of the special strings used by the parser. This include
// semi-reserved words: identifiers that mean something special in certain
// contexts. These are not reserved words.
// ===========================================================================

#if !defined(SPECNAME)
#error Must define SPECNAME macro before including specnames.h
#endif

//               id             text    )
SPECNAME(SN_MISSING, L"?")
SPECNAME(SN_GET, L"get")
SPECNAME(SN_SET, L"set")
SPECNAME(SN_ADD, L"add")
SPECNAME(SN_REMOVE, L"remove")
SPECNAME(SN_WHERE, L"where")
SPECNAME(SN_PARTIAL, L"partial")
SPECNAME(SN_GLOBAL, L"global")
SPECNAME(SN_YIELD, L"yield")
SPECNAME(SN_ALIAS, L"alias")

#undef SPECNAME
