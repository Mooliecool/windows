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
/*============================================================
**
** Header:  SecurityRuntime.h
**       
**
** Purpose:
**
** Date:  March 21, 1998
**
===========================================================*/
#ifndef __ComSecurityRuntime_h__
#define __ComSecurityRuntime_h__

#include "common.h"

#include "object.h"
#include "util.hpp"

// Forward declarations to avoid pulling in too many headers.
class Frame;
enum StackWalkAction;

//-----------------------------------------------------------
// The SecurityRuntime implements all the native methods
// for the interpreted System/Security/SecurityRuntime.
//-----------------------------------------------------------
class SecurityRuntime
{
friend class Security;
public:
    // private helper for getting a security object
    static FCDECL2(Object*, GetSecurityObjectForFrame, StackCrawlMark* stackMark, CLR_BOOL create);
protected:

    static void CheckBeforeAllocConsole(AppDomain* pDomain, Assembly* pAssembly);
};

#endif /* __ComSecurityRuntime_h__ */

