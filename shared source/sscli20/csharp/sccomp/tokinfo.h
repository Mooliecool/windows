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
// File: tokinfo.h
//
// ===========================================================================

#ifndef __tokinfo_h__
#define __tokinfo_h__

#include "enum.h"
#include "posdata.h"
#include "tokdata.h"

class CParser;
struct STATEMENTNODE;
struct BASENODE;

////////////////////////////////////////////////////////////////////////////////
// TOKINFO

struct TOKINFO
{
    PCWSTR      pszText;            // token text
    // Keep the pointers first for better packing on 64-bits
    DWORD       dwFlags;            // token flags
    BYTE        iLen;               // token length
    BYTE        iStmtParser;        // Parser fn
    BYTE        iPredefType;        // predefined type represented by token
    BYTE        iBinaryOp;          // Binary operator
    BYTE        iUnaryOp;           // Unary operator
    BYTE        iSelfOp;            // Self operator (like true, false, this...)
};

////////////////////////////////////////////////////////////////////////////////
// OPINFO

struct OPINFO
{
    BYTE        iToken:8;           // Token ID
    BYTE        iPrecedence:7;      // Operator precedence
    BYTE        fRightAssoc:1;      // Associativity
};

struct NAME;
class COMPILER;

#endif //__tokinfo_h__
