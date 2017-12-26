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
// File: Machine.h
// 
// ===========================================================================
#ifndef _MACHINE_H_
#define _MACHINE_H_
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifdef WIN32

#define INC_OLE2
#include <windows.h>
#ifdef INIT_GUIDS
#include <initguid.h>
#endif

#else

#include <varargs.h>

#ifndef DWORD
#define	DWORD	unsigned long
#endif

#endif // !WIN32


typedef unsigned __int64    QWORD;

#endif // ifndef _MACHINE_H_
// EOF =======================================================================
