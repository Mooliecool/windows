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
// ---------------------------------------------------------------------------
// StaticContract.h
// ---------------------------------------------------------------------------

#ifndef __STATIC_CONTRACT_H_
#define __STATIC_CONTRACT_H_

#define SCAN_WIDEN2(x) L ## x
#define SCAN_WIDEN(x) SCAN_WIDEN2(x)

//
// PDB annotations for the static contract analysis tool. These are seperated
// from Contract.h to allow their inclusion in any part of the system.
//

#ifdef _X86_

//
// currently, only x86 has a static contract analysis tool, so let's not
// bloat the PDBs of all the other architectures too..
//
#define ANNOTATION_TRY_BEGIN                __annotation(L"TRY_BEGIN")
#define ANNOTATION_TRY_END                  __annotation(L"TRY_END")
#define ANNOTATION_HANDLER_BEGIN            __annotation(L"HANDLER_BEGIN")
#define ANNOTATION_HANDLER_END              __annotation(L"HANDLER_END")
#define ANNOTATION_NOTHROW                  __annotation(L"NOTHROW")
#define ANNOTATION_WRAPPER                  __annotation(L"WRAPPER")
#define ANNOTATION_FAULT                    __annotation(L"FAULT")
#define ANNOTATION_FORBID_FAULT             __annotation(L"FORBID_FAULT")
#define ANNOTATION_COOPERATIVE              __annotation(L"MODE_COOPERATIVE")
#define ANNOTATION_MODE_COOPERATIVE         __annotation(L"MODE_PREEMPTIVE")
#define ANNOTATION_MODE_ANY                 __annotation(L"MODE_ANY")
#define ANNOTATION_GC_TRIGGERS              __annotation(L"GC_TRIGGERS")
#define ANNOTATION_IGNORE_THROW             __annotation(L"THROWS", L"NOTHROW")
#define ANNOTATION_IGNORE_FAULT             __annotation(L"FAULT", L"FORBID_FAULT")
#define ANNOTATION_IGNORE_TRIGGER           __annotation(L"GC_TRIGGERS", L"GC_NOTRIGGER")
#define ANNOTATION_VIOLATION(violationmask) __annotation(L"VIOLATION(" L#violationmask L")")
#define ANNOTATION_UNCHECKED(thecheck)      __annotation(L"UNCHECKED(" L#thecheck L")")
#define ANNOTATION_FN_HOST_NOCALLS          __annotation(L"HOST_NOCALLS")
#define ANNOTATION_FN_HOST_CALLS            __annotation(L"HOST_CALLS")

#define ANNOTATION_SO_PROBE_BEGIN(probeAmount) __annotation(L"SO_PROBE_BEGIN(" L#probeAmount L")")
#define ANNOTATION_SO_PROBE_END             __annotation(L"SO_PROBE_END")

#define ANNOTATION_SO_TOLERANT              __annotation(L"GS_RO")
#define ANNOTATION_SO_INTOLERANT            __annotation(L"GS_RW")
#define ANNOTATION_SO_NOT_MAINLINE          __annotation(L"SO_NM")
#define ANNOTATION_SO_NOT_MAINLINE_BEGIN    __annotation(L"SO_NMB")
#define ANNOTATION_SO_NOT_MAINLINE_END      __annotation(L"SO_NME")
#define ANNOTATION_ENTRY_POINT              __annotation(L"SO_EP")  
//
// these annotations are all function-name qualified
//
#define ANNOTATION_FN_LEAF                  __annotation(L"LEAF " SCAN_WIDEN(__FUNCTION__))
#define ANNOTATION_FN_THROWS                __annotation(L"THROWS " SCAN_WIDEN(__FUNCTION__))
#define ANNOTATION_FN_NOTHROW               __annotation(L"NOTHROW " SCAN_WIDEN(__FUNCTION__))
#define ANNOTATION_FN_GC_TRIGGERS           __annotation(L"GC_TRIGGERS " SCAN_WIDEN(__FUNCTION__))
#define ANNOTATION_FN_GC_NOTRIGGER          __annotation(L"GC_NOTRIGGER " SCAN_WIDEN(__FUNCTION__))

#ifdef _DEBUG
#define ANNOTATION_DEBUG_ONLY               __annotation(L"DBG_ONLY")
#endif

#else // _X86_

#define ANNOTATION_TRY_BEGIN                { }
#define ANNOTATION_TRY_END                  { }
#define ANNOTATION_HANDLER_BEGIN            { }
#define ANNOTATION_HANDLER_END              { }
#define ANNOTATION_NOTHROW                  { }
#define ANNOTATION_WRAPPER                  { }
#define ANNOTATION_FAULT                    { }
#define ANNOTATION_FORBID_FAULT             { }
#define ANNOTATION_COOPERATIVE              { }
#define ANNOTATION_MODE_COOPERATIVE         { }
#define ANNOTATION_MODE_ANY                 { }
#define ANNOTATION_GC_TRIGGERS              { }
#define ANNOTATION_IGNORE_THROW             { }
#define ANNOTATION_IGNORE_FAULT             { }
#define ANNOTATION_IGNORE_TRIGGER           { }
#define ANNOTATION_VIOLATION(violationmask) { }
#define ANNOTATION_UNCHECKED(thecheck)      { }
#define ANNOTATION_FN_HOST_NOCALLS          { }
#define ANNOTATION_FN_HOST_CALLS            { }

#define ANNOTATION_FN_LEAF                  { }
#define ANNOTATION_FN_THROWS                { }
#define ANNOTATION_FN_NOTHROW               { }
#define ANNOTATION_FN_GC_TRIGGERS           { }
#define ANNOTATION_FN_GC_NOTRIGGER          { }

#define ANNOTATION_SO_PROBE_BEGIN(probeAmount) { }
#define ANNOTATION_SO_PROBE_END             { }

#define ANNOTATION_SO_TOLERANT              { }
#define ANNOTATION_SO_INTOLERANT            { }
#define ANNOTATION_SO_NOT_MAINLINE          { }
#define ANNOTATION_SO_NOT_MAINLINE_BEGIN    { }
#define ANNOTATION_SO_NOT_MAINLINE_END      { }
#define ANNOTATION_ENTRY_POINT              { }
#ifdef _DEBUG
#define ANNOTATION_DEBUG_ONLY               { }
#endif

#endif // _X86_

#define STATIC_CONTRACT_THROWS              ANNOTATION_FN_THROWS
#define STATIC_CONTRACT_NOTHROW             ANNOTATION_FN_NOTHROW
#define STATIC_CONTRACT_FAULT               ANNOTATION_FAULT
#define STATIC_CONTRACT_FORBID_FAULT        ANNOTATION_FORBID_FAULT
#define STATIC_CONTRACT_GC_TRIGGERS         ANNOTATION_FN_GC_TRIGGERS
#define STATIC_CONTRACT_GC_NOTRIGGER        ANNOTATION_FN_GC_NOTRIGGER
#define STATIC_CONTRACT_HOST_NOCALLS        ANNOTATION_FN_HOST_NOCALLS
#define STATIC_CONTRACT_HOST_CALLS          ANNOTATION_FN_HOST_CALLS 

#define STATIC_CONTRACT_MODE_COOPERATIVE    ANNOTATION_COOPERATIVE
#define STATIC_CONTRACT_MODE_PREEMPTIVE     ANNOTATION_MODE_COOPERATIVE
#define STATIC_CONTRACT_MODE_ANY            ANNOTATION_MODE_ANY
#define STATIC_CONTRACT_LEAF                ANNOTATION_FN_LEAF
#define STATIC_CONTRACT_WRAPPER             ANNOTATION_WRAPPER


#define STATIC_CONTRACT_SO_INTOLERANT       ANNOTATION_SO_INTOLERANT                                   
#define STATIC_CONTRACT_SO_TOLERANT         ANNOTATION_SO_TOLERANT                                   
#define STATIC_CONTRACT_SO_NOT_MAINLINE     ANNOTATION_SO_NOT_MAINLINE
#define STATIC_CONTRACT_SO_NOT_MAINLINE_BEGIN ANNOTATION_SO_NOT_MAINLINE_BEGIN
#define STATIC_CONTRACT_SO_NOT_MAINLINE_END ANNOTATION_SO_NOT_MAINLINE_END

#define STATIC_CONTRACT_ENTRY_POINT         ANNOTATION_ENTRY_POINT
#ifdef _DEBUG
#define STATIC_CONTRACT_DEBUG_ONLY          ANNOTATION_DEBUG_ONLY
#endif

#define EnsureSOIntolerantOK(x,y,z)


#ifdef _MSC_VER
#define SCAN_IGNORE_THROW                   ANNOTATION_IGNORE_THROW
#define SCAN_IGNORE_FAULT                   ANNOTATION_IGNORE_FAULT
#define SCAN_IGNORE_TRIGGER                 ANNOTATION_IGNORE_TRIGGER
#else
#define SCAN_IGNORE_THROW
#define SCAN_IGNORE_FAULT
#define SCAN_IGNORE_TRIGGER
#endif

#define CANNOT_HAVE_CONTRACT                __annotation(L"NO_CONTRACT")

#endif // __STATIC_CONTRACT_H_
