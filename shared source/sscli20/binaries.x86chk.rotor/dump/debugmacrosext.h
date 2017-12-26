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
//*****************************************************************************
// DebugMacrosExt.h
//
// Simple debugging macros that take no dependencies on CLR headers.
// This header can be used from outside the CLR.
//
//*****************************************************************************
#ifndef __DebugMacrosExt_h__
#define __DebugMacrosExt_h__

#if !defined(_DEBUG_IMPL) && defined(_DEBUG) && !defined(DACCESS_COMPILE)
#define _DEBUG_IMPL 1
#endif

#ifdef _DEBUG
// A macro to execute a statement only in _DEBUG.
#define DEBUG_STMT(stmt) stmt
#define INDEBUG(x)          x
#define INDEBUG_COMMA(x)    x,
#define COMMA_INDEBUG(x)    ,x
#define NOT_DEBUG(x)
#else
#define DEBUG_STMT(stmt)
#define INDEBUG(x)
#define INDEBUG_COMMA(x)
#define COMMA_INDEBUG(x)
#define NOT_DEBUG(x)        x
#endif


#ifdef _DEBUG_IMPL
#define INDEBUGIMPL(x)          x
#define INDEBUGIMPL_COMMA(x)    x,
#define COMMA_INDEBUGIMPL(x)    ,x
#else
#define INDEBUGIMPL(x)
#define INDEBUGIMPL_COMMA(x)
#define COMMA_INDEBUGIMPL(x)
#endif


#endif
