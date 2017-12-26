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
// MDLog.h - Meta data logging helper.
//
//*****************************************************************************
#ifndef __MDLog_h__
#define __MDLog_h__

#if defined(_DEBUG) && !defined(DACCESS_COMPILE)
#define LOGGING
#endif

#include <log.h>

#define LOGMD LF_METADATA, LL_INFO10000
#define LOG_MDCALL(func) LOG((LF_METADATA, LL_INFO10000, "MD: %s\n", #func))

#define MDSTR(str) ((str) ? str : L"<null>")
#define MDSTRA(str) ((str) ? str : "<null>")

#endif // __MDLog_h__
