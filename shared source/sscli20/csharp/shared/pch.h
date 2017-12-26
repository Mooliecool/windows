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
// File: pch.h
//
// include all headers that are part of the PCH file
// ===========================================================================
#ifndef __pch_h__
#define __pch_h__


#define _ATL_APARTMENT_THREADED

#include <windows.h>

static const int E_EXEC_ERROR = MAKE_HRESULT(1, FACILITY_ITF, 0xF0EF);
static const int E_PARSE_ERROR = MAKE_HRESULT(1, FACILITY_ITF, 0xF0EE);
static const int E_BIND_ERROR = MAKE_HRESULT(1, FACILITY_ITF, 0xF0ED);
static const int E_EXEC_BADREF_ERROR = MAKE_HRESULT(1, FACILITY_ITF, 0xF0EC);
static const int E_GCXMDUNLOAD = MAKE_HRESULT(1, FACILITY_ITF, 0xF0EB);
static const int E_EXEC_FUNCEVALDISABLED = MAKE_HRESULT(1, FACILITY_ITF, 0xF0EA);


#ifdef DEBUG
extern void __CHECKHR(HRESULT hr, bool fromFuncEval, bool noassert, const char * file, unsigned line);
#define CHECKHR(x,fe,na) __CHECKHR(x,fe,na,__FILE__, __LINE__)
#else
#define CHECKHR(x,fe,na) 
#endif
#define RETGO(x)  {HR = (x); CHECKHR(HR, false, false); goto LERROR;}
#define FAILGO(x) {if (FAILED(HR = (x))) { CHECKHR(HR, false, false); goto LERROR;}}
#define FEFAILGO(x) {HR = (x); CHECKHR(HR, true, false); if (FAILED(HR) && HR != E_EVAL_OVERFLOW_HAS_OCCURRED) goto LERROR; }
#define CHECKMEM(x) {if (!(x)) {HR = E_OUTOFMEMORY;  CHECKHR(HR, false, false); goto LERROR;}}
#define CHECKFALSE(x,hr) {if (!(x)) {HR = (hr);  CHECKHR(HR, false, false); goto LERROR; }}
#define CHECKFAIL(x) {if (!(x)) {HR = E_FAIL;  CHECKHR(HR, false, false); goto LERROR;}}
#define CHECKFAILA(x) {if (!(x)) {ASSERT(0); HR = E_FAIL;  CHECKHR(HR, false, false); goto LERROR;}}
#define NSOKGOSF(x) {if ((HR = (x)) != S_OK) { CHECKHR(HR, false, false); HR = S_FALSE; goto LERROR; }}
#define NSOKGOEF(x) {if ((HR = (x)) != S_OK) { CHECKHR(HR, false, false); if (!FAILED(HR)) { CHECKHR(E_FAIL, false, false); HR = E_FAIL;}; goto LERROR; };}
#define NSOKGO(x) {if ((HR = (x)) != S_OK) { CHECKHR(HR, false, false); goto LERROR; }}
#define NARETGO(x)  {HR = (x); CHECKHR(HR, false, true); goto LERROR;}
#define NAFAILGO(x) {if (FAILED(HR = (x))) {  CHECKHR(HR, false, true); goto LERROR;}}
#define NACHECKFALSE(x,hr) {if (!(x)) {HR = (hr); CHECKHR(HR, false, true); goto LERROR; }}
#define NACHECKFAIL(x) {if (!(x)) {HR = E_FAIL; CHECKHR(HR, false, true); goto LERROR;}}
#define NANSOKGOSF(x) {if ((HR = (x)) != S_OK) { HR = S_FALSE; CHECKHR(HR, false, true); goto LERROR; }}
#define NAFAILGOSF(x) {if (FAILED(HR = (x))) {  CHECKHR(HR, false, true); HR = S_FALSE; goto LERROR; }}

// portable helpers for access to unaligned pointers
#ifdef _MSC_VER
// Get unaligned values from a potentially unaligned object
#define GET_UNALIGNED_16(_pObject)  (*(UINT16 UNALIGNED *)(_pObject))
#define GET_UNALIGNED_32(_pObject)  (*(UINT32 UNALIGNED *)(_pObject))
#define GET_UNALIGNED_64(_pObject)  (*(UINT64 UNALIGNED *)(_pObject))

// Set Value on an potentially unaligned object 
#define SET_UNALIGNED_16(_pObject, _Value)  (*(UNALIGNED UINT16 *)(_pObject)) = (UINT16)(_Value)
#define SET_UNALIGNED_32(_pObject, _Value)  (*(UNALIGNED UINT32 *)(_pObject)) = (UINT32)(_Value)
#define SET_UNALIGNED_64(_pObject, _Value)  (*(UNALIGNED UINT64 *)(_pObject)) = (UINT64)(_Value) 
#endif

// helpers for bigendian swapping
#if !BIGENDIAN

#define VAL16(x) x
#define VAL32(x) x
#define VAL64(x) x
#define SwapString(x)
#define SwapStringLength(x, y)
#define SwapGuid(x)

#define GET_UNALIGNED_VAL16(_pObject) VAL16(GET_UNALIGNED_16(_pObject))
#define GET_UNALIGNED_VAL32(_pObject) VAL32(GET_UNALIGNED_32(_pObject))
#define GET_UNALIGNED_VAL64(_pObject) VAL64(GET_UNALIGNED_64(_pObject))

#define SET_UNALIGNED_VAL16(_pObject, _Value) SET_UNALIGNED_16(_pObject, VAL16((UINT16)_Value))
#define SET_UNALIGNED_VAL32(_pObject, _Value) SET_UNALIGNED_32(_pObject, VAL32((UINT32)_Value))
#define SET_UNALIGNED_VAL64(_pObject, _Value) SET_UNALIGNED_64(_pObject, VAL64((UINT64)_Value))

#endif

#define ASSERT(fTest)  _ASSERTE(fTest)
#define DBGOUT(a) do {} while (0)
#include "vsassert.h"


#include <atl.h>


#include <stddef.h>
#include <unilib.h>
#include <uniapi.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif
#include <strsafe.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <cor.h>

#define USE_RWLOCKS

#define lengthof(a) (sizeof(a) / sizeof((a)[0]))

#include "cscommon.h"
#include "csiface.h"


#endif // __pch_h__

