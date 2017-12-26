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
/******************************************************************************
	FILE : UTSEM.CPP

	Purpose: Part of the utilities library for the VIPER project

	Abstract : Implements the UTSemReadWrite class.
-------------------------------------------------------------------------------
Revision History:

*******************************************************************************/
#include "stdafx.h"


#include <utsem.h>

//TODO: this undef should not be here. We need to move this code or the callers of this
//      code to the new synchronization api
//      This function are redefined at the end of this file
#undef InitializeCriticalSection



/******************************************************************************
Function : UTSemExclusive::UTSemExclusive

Abstract: Constructor.
******************************************************************************/
CSemExclusive::CSemExclusive (DWORD ulcSpinCount)
{
    ::InitializeCriticalSection (&m_csx);

	DEBUG_STMT(m_iLocks = 0);
}


#define InitializeCriticalSection   Dont_Use_InitializeCriticalSection

