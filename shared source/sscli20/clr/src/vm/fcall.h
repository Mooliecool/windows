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
// FCall.H -
//
//
// FCall is a high-performance alternative to ECall. Unlike ECall, FCall
// methods do not necessarily create a frame.   Jitted code calls directly
// to the FCall entry point.   It is possible to do operations that need
// to have a frame within an FCall, you need to manually set up the frame
// before you do such operations.

// It is illegal to cause a GC or EH to happen in an FCALL before setting
// up a frame.  To prevent accidentally violating this rule, FCALLs turn
// on BEGINGCFORBID, which insures that these things can't happen in a 
// checked build without causing an ASSERTE.  Once you set up a frame,
// this state is turned off as long as the frame is active, and then is
// turned on again when the frame is torn down.   This mechanism should
// be sufficient to insure that the rules are followed.

// In general you set up a frame by using the following macros

//      HELPER_METHOD_FRAME_BEGIN_RET*()    // Use If the FCALL has a return value
//      HELPER_METHOD_FRAME_BEGIN*()        // Use If FCALL does not return a value
//      HELPER_METHOD_FRAME_END*()              

// These macros introduce a scope in which is protected by an HelperMethodFrame.
// in this scope you can do EH or GC.   There are rules associated with 
// their use.  In particular

//      1) These macros can only be used in the body of a FCALL (that is
//         something using the FCIMPL* or HCIMPL* macros for their decaration.

//      2) You may not perform a 'return' with this scope..

// Compile time errors occur if you try to violate either of these rules.

// The frame that is set up does NOT protect any GC variables (in particular the
// arguments of the FCALL.  THus you need to do an explicit GCPROTECT once the
// frame is established is you need to protect an argument.  There are flavors
// of HELPER_METHOD_FRAME that protect a certain number of GC variable.  For
// example

//      HELPER_METHOD_FRAME_BEGIN_RET_2(arg1, arg2)

// will protect the GC variables arg1, and arg2 as well as erect the frame.  

// Another invariant that you must be aware of is the need to poll to see if
// a GC is needed by some other thread.   Unless the FCALL is VERY short, 
// every code path through the FCALL must do such a poll.  The important 
// thing here is that a poll will cause a GC, and thus you can only do it
// when all you GC variables are protected.   To make things easier 
// HELPER_METHOD_FRAMES that protect things automatically do this poll.
// If you don't need to protect anything HELPER_METHOD_FRAME_BEGIN_0
// will also do the poll. 

// Sometimes it is convenient to do the poll a the end of the frame, you 
// can used HELPER_METHOD_FRAME_BEGIN_NOPOLL and HELPER_METHOD_FRAME_END_POLL
// to do the poll at the end.   If somewhere in the middle is the best
// place you can do that to with HELPER_METHOD_POLL()

// You don't need to erect a helper method frame to do a poll.  FC_GC_POLL
// can do this (remember all your GC refs will be trashed).  

// Finally if your method is VERY small, you can get away without a poll,
// you have to use FC_GC_POLL_NOT_NEEDED can be used to mark this.  
// Use sparingly!

// It is possible to set up the frame as the first operation in the FCALL and
// tear it down as the last operation before returning.  This works and is 
// reasonably efficient (as good as an ECall), however, if it is the case that
// you can defer the setup of the frame to an unlikely code path (exception path)
// that is much better.   

// If you defer setup of the frame, all codepaths leading to the frame setup
// must be wrapped with PERMIT_HELPER_METHOD_FRAME_BEGIN/END.  These block
// certain compiler optimizations that interfere with the delayed frame setup.
// These macros are automatically included in the HCIMPL, FCIMPL, and frame
// setup macros.


// It is common to only need to set up a frame in order to throw an exception.
// While this can be done by doing 

//      HELPER_METHOD_FRAME_BEGIN()         // Use If FCALL does not return a value
//      COMPlusThrow(execpt);
//      HELPER_METHOD_FRAME_END()           

// It is more efficient (in space) to use conviniece macro FCTHROW that does 
// this for you (sets up a frame, and does the throw).

//      FCTHROW(except)

// Since FCALLS have to conform to the EE calling conventions and not to C
// calling conventions, FCALLS, need to be declared using special macros (FCIMPL*) 
// that implement the correct calling conventions.  There are variants of these
// macros depending on the number of args, and sometimes the types of the 
// arguments. 

//------------------------------------------------------------------------
//    A very simple example:
//
//      FCIMPL2(INT32, Div, INT32 x, INT32 y)
//      {
//          if (y == 0) 
//              FCThrow(kDivideByZeroException);
//          return x/y;
//      }
//      FCIMPLEND
//
//
// *** WATCH OUT FOR THESE GOTCHAS: ***
// ------------------------------------
//  - In your FCDECL & FCIMPL protos, don't declare a param as type OBJECTREF
//    or any of its deriveds. This will break on the checked build because
//    __fastcall doesn't enregister C++ objects (which OBJECTREF is).
//    Instead, you need to do something like;
//
//      FCIMPL(.., .., Object* pObject0)
//          OBJECTREF pObject = ObjectToOBJECTREF(pObject0);
//      FCIMPL
//
//    For similar reasons, use Object* rather than OBJECTREF as a return type.  
//    Consider either using ObjectToOBJECTREF or calling VALIDATEOBJECTREF
//    to make sure your Object* is valid.
//
//  - FCThrow() must be called directly from your FCall impl function: it
//    cannot be called from a subfunction. Calling from a subfunction breaks
//    the VC code parsing hack that lets us recover the callee saved registers.
//    Fortunately, you'll get a compile error complaining about an
//    unknown variable "__me".
//
//  - If your FCall returns VOID, you must use FCThrowVoid() rather than
//    FCThrow(). This is because FCThrow() has to generate an unexecuted
//    "return" statement for the code parser's evil purposes.
//
//  - Unlike ECall, the parameters aren't gc-promoted. Of course, if you
//    cause a GC in an FCall, you're already breaking the rules.
//
//  - On 32 bit machines, the first two arguments MUST NOT be 64 bit values
//    or larger, because they are enregistered.  If you mess up here, you'll
//    likely corrupt the stack then corrupt the heap so quickly you won't 
//    be able to debug it easily.
//

// How FCall works:
// ----------------
//   An FCall target uses __fastcall or some other calling convention to
//   match the IL calling convention exactly. Thus, a call to FCall is a direct
//   call to the target w/ no intervening stub or frame.
//
//   The tricky part is when FCThrow is called. FCThrow must generate
//   a proper method frame before allocating and throwing the exception.
//   To do this, it must recover several things:
//
//      - The location of the FCIMPL's return address (since that's
//        where the frame will be based.)
//
//      - The on-entry values of the callee-saved regs; which must
//        be recorded in the frame so that GC can update them.
//        Depending on how VC compiles your FCIMPL, those values are still
//        in the original registers or saved on the stack.
//
//        To figure out which, FCThrow() generates the code:
//
//              while (NULL == __FCThrow(__me, ...)) {};
//              return 0;
//
//        The "return" statement will never execute; but its presence guarantees
//        that VC will follow the __FCThrow() call with a VC epilog
//        that restores the callee-saved registers using a pretty small
//        and predictable set of Intel opcodes. __FCThrow() parses this
//        epilog and simulates its execution to recover the callee saved
//        registers.
//
//        The while loop is to prevent the compiler from doing tail call optimizations.
//        The helper frame interpretter needs the frame to be present.
//
//      - The MethodDesc* that this FCall implements. This MethodDesc*
//        is part of the frame and ensures that the FCall will appear
//        in the exception's stack trace. To get this, FCDECL declares
//        a static local __me, initialized to point to the FC target itself.
//        This address is exactly what's stored in the ECall lookup tables;
//        so __FCThrow() simply does a reverse lookup on that table to recover
//        the MethodDesc*.
//

#ifndef __FCall_h__
#define __FCall_h__

#include "gms.h"
#include "runtimeexceptionkind.h"
#include "debugreturn.h"
#include "stackprobe.h"

//==============================================================================================
// These macros defeat compiler optimizations that might mix nonvolatile
// register loads and stores with other code in the function body.  This
// creates problems for the frame setup code, which assumes that any
// nonvolatiles that are saved at the point of the frame setup will be
// re-loaded when the frame is popped.
//
// Currently this is only known to be an issue on AMD64.  It's uncertain
// whether it is an issue on x86.  It is not an issue on IA64 because we don't
// permit the JIT to use s0-s3 for object references.
//==============================================================================================


#define PERMIT_HELPER_METHOD_FRAME_BEGIN()
#define PERMIT_HELPER_METHOD_FRAME_END()
#define FCALL_TRANSITION_BEGIN()
#define FCALL_TRANSITION_END()
#define CHECK_HELPER_METHOD_FRAME_PERMITTED()


//==============================================================================================
// This is where FCThrow ultimately ends up. Never call this directly.
// Use the FCThrow() macros. __FCThrowArgument is the helper to throw ArgumentExceptions
// with a resource taken from the managed resource manager.
//==============================================================================================
LPVOID __stdcall __FCThrow(LPVOID me, enum RuntimeExceptionKind reKind, UINT resID, LPCWSTR arg1, LPCWSTR arg2, LPCWSTR arg3);
LPVOID __stdcall __FCThrowArgument(LPVOID me, enum RuntimeExceptionKind reKind, LPCWSTR argumentName, LPCWSTR resourceName);

//==============================================================================================
// FDECLn: A set of macros for generating header declarations for FC targets.
// Use FIMPLn for the actual body.
//==============================================================================================

// Note: on the x86, these defs reverse all but the first two arguments
// (IL stack calling convention is reversed from __fastcall.)


// Calling convention for varargs
#define F_CALL_VA_CONV __cdecl


#ifdef _X86_

// Choose the appropriate calling convention for FCALL helpers on the basis of the JIT calling convention
#ifdef __GNUC__
#define F_CALL_CONV __attribute__((stdcall, regparm(3)))
#else
#define F_CALL_CONV __fastcall
#endif

#if defined(__GNUC__)

// GCC fastcall convention is different from MSVC fastcall convention. GCC can use up to 3 registers to
// store parameters. The registers used are EAX, EDX, ECX. Dummy parameters and reordering of the 
// actual parameters in the FCALL signature is used to make the calling convention to look like in MSVC.

#define FCDECL0(rettype, funcname) rettype F_CALL_CONV funcname()
#define FCDECL1(rettype, funcname, a1) rettype F_CALL_CONV funcname(int /* EAX */, int /* EDX */, a1)
#define FCDECL1_V(rettype, funcname, a1) rettype F_CALL_CONV funcname(int /* EAX */, int /* EDX */, int /* ECX */, a1)
#define FCDECL2(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1)
#define FCDECL2VA(rettype, funcname, a1, a2) rettype F_CALL_VA_CONV funcname(a1, a2, ...)
#define FCDECL2_VV(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(int /* EAX */, int /* EDX */, int /* ECX */, a2, a1)
#define FCDECL2_VI(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(int /* EAX */, int /* EDX */, a2, a1)
#define FCDECL2_IV(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(int /* EAX */, int /* EDX */, a1, a2)
#define FCDECL3(rettype, funcname, a1, a2, a3) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a3)
#define FCDECL3_VII(rettype, funcname, a1, a2, a3) rettype F_CALL_CONV funcname(int /* EAX */, a3, a2, a1)
#define FCDECL3_IVV(rettype, funcname, a1, a2, a3) rettype F_CALL_CONV funcname(int /* EAX */, int /* EDX */, a1, a3, a2)
#define FCDECL3_IVI(rettype, funcname, a1, a2, a3) rettype F_CALL_CONV funcname(int /* EAX */, a3, a1, a2)
#define FCDECL3_VVI(rettype, funcname, a1, a2, a3) rettype F_CALL_CONV funcname(int /* EAX */, int /* EDX */, a3, a2, a1)
#define FCDECL4(rettype, funcname, a1, a2, a3, a4) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a4, a3)
#define FCDECL5(rettype, funcname, a1, a2, a3, a4, a5) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a5, a4, a3)
#define FCDECL6(rettype, funcname, a1, a2, a3, a4, a5, a6) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a6, a5, a4, a3)
#define FCDECL7(rettype, funcname, a1, a2, a3, a4, a5, a6, a7) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a7, a6, a5, a4, a3)
#define FCDECL8(rettype, funcname, a1, a2, a3, a4, a5, a6, a7, a8) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a8, a7, a6, a5, a4, a3)
#define FCDECL9(rettype, funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a9, a8, a7, a6, a5, a4, a3)
#define FCDECL10(rettype,funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a10, a9, a8, a7, a6, a5, a4, a3)
#define FCDECL11(rettype,funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a11, a10, a9, a8, a7, a6, a5, a4, a3)
#define FCDECL12(rettype,funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a12, a11, a10, a9, a8, a7, a6, a5, a4, a3)

#define FCDECL5_IVI(rettype, funcname, a1, a2, a3, a4, a5) rettype F_CALL_CONV funcname(int /* EAX */, a3, a1, a5, a4, a2)
#define FCDECL5_VII(rettype, funcname, a1, a2, a3, a4, a5) rettype F_CALL_CONV funcname(int /* EAX */, a3, a2, a5, a4, a1)

#else // __GNUC__

#define FCDECL0(rettype, funcname) rettype F_CALL_CONV funcname()
#define FCDECL1(rettype, funcname, a1) rettype F_CALL_CONV funcname(a1)
#define FCDECL1_V(rettype, funcname, a1) rettype F_CALL_CONV funcname(a1)
#define FCDECL2(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(a1, a2)
#define FCDECL2VA(rettype, funcname, a1, a2) rettype F_CALL_VA_CONV funcname(a1, a2, ...)
#define FCDECL2_VV(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(a2, a1)
#define FCDECL2_VI(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(a2, a1)
#define FCDECL2_IV(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(a1, a2)
#define FCDECL3(rettype, funcname, a1, a2, a3) rettype F_CALL_CONV funcname(a1, a2, a3)
#define FCDECL3_VII(rettype, funcname, a1, a2, a3) rettype F_CALL_CONV funcname(a2, a3, a1)
#define FCDECL3_IVV(rettype, funcname, a1, a2, a3) rettype F_CALL_CONV funcname(a1, a3, a2)
#define FCDECL3_IVI(rettype, funcname, a1, a2, a3) rettype F_CALL_CONV funcname(a1, a3, a2)
#define FCDECL3_VVI(rettype, funcname, a1, a2, a3) rettype F_CALL_CONV funcname(a2, a1, a3)
#define FCDECL4(rettype, funcname, a1, a2, a3, a4) rettype F_CALL_CONV funcname(a1, a2, a4, a3)
#define FCDECL5(rettype, funcname, a1, a2, a3, a4, a5) rettype F_CALL_CONV funcname(a1, a2, a5, a4, a3)
#define FCDECL6(rettype, funcname, a1, a2, a3, a4, a5, a6) rettype F_CALL_CONV funcname(a1, a2, a6, a5, a4, a3)
#define FCDECL7(rettype, funcname, a1, a2, a3, a4, a5, a6, a7) rettype F_CALL_CONV funcname(a1, a2, a7, a6, a5, a4, a3)
#define FCDECL8(rettype, funcname, a1, a2, a3, a4, a5, a6, a7, a8) rettype F_CALL_CONV funcname(a1, a2, a8, a7, a6, a5, a4, a3)
#define FCDECL9(rettype, funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9) rettype F_CALL_CONV funcname(a1, a2, a9, a8, a7, a6, a5, a4, a3)
#define FCDECL10(rettype,funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) rettype F_CALL_CONV funcname(a1, a2, a10, a9, a8, a7, a6, a5, a4, a3)
#define FCDECL11(rettype,funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) rettype F_CALL_CONV funcname(a1, a2, a11, a10, a9, a8, a7, a6, a5, a4, a3)
#define FCDECL12(rettype,funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) rettype F_CALL_CONV funcname(a1, a2, a12, a11, a10, a9, a8, a7, a6, a5, a4, a3)

#define FCDECL5_IVI(rettype, funcname, a1, a2, a3, a4, a5) rettype F_CALL_CONV funcname(a1, a3, a5, a4, a2)
#define FCDECL5_VII(rettype, funcname, a1, a2, a3, a4, a5) rettype F_CALL_CONV funcname(a2, a3, a5, a4, a1)

#endif // __GNUC__


#else // _X86_

#define F_CALL_CONV

#define FCDECL0(rettype, funcname) rettype funcname()
#define FCDECL1(rettype, funcname, a1) rettype funcname(a1)
#define FCDECL1_V(rettype, funcname, a1) rettype funcname(a1)
#define FCDECL2(rettype, funcname, a1, a2) rettype funcname(a1, a2)
#define FCDECL2VA(rettype, funcname, a1, a2) rettype funcname(a1, a2, ...)
#define FCDECL2_VV(rettype, funcname, a1, a2) rettype funcname(a1, a2)
#define FCDECL2_VI(rettype, funcname, a1, a2) rettype funcname(a1, a2)
#define FCDECL2_IV(rettype, funcname, a1, a2) rettype funcname(a1, a2)
#define FCDECL3(rettype, funcname, a1, a2, a3) rettype funcname(a1, a2, a3)
#define FCDECL3_VII(rettype, funcname, a1, a2, a3) rettype funcname(a1, a2, a3)
#define FCDECL3_IVV(rettype, funcname, a1, a2, a3) rettype funcname(a1, a2, a3)
#define FCDECL3_IVI(rettype, funcname, a1, a2, a3) rettype funcname(a1, a2, a3)
#define FCDECL3_VVI(rettype, funcname, a1, a2, a3) rettype funcname(a1, a2, a3)
#define FCDECL4(rettype, funcname, a1, a2, a3, a4) rettype funcname(a1, a2, a3, a4)
#define FCDECL5(rettype, funcname, a1, a2, a3, a4, a5) rettype funcname(a1, a2, a3, a4, a5)
#define FCDECL6(rettype, funcname, a1, a2, a3, a4, a5, a6) rettype funcname(a1, a2, a3, a4, a5, a6)
#define FCDECL7(rettype, funcname, a1, a2, a3, a4, a5, a6, a7) rettype funcname(a1, a2, a3, a4, a5, a6, a7)
#define FCDECL8(rettype, funcname, a1, a2, a3, a4, a5, a6, a7, a8) rettype funcname(a1, a2, a3, a4, a5, a6, a7, a8)
#define FCDECL9(rettype, funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9) rettype funcname(a1, a2, a3, a4, a5, a6, a7, a8, a9)
#define FCDECL10(rettype,funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) rettype funcname(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
#define FCDECL11(rettype,funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) rettype funcname(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)
#define FCDECL12(rettype,funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) rettype funcname(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)

#define FCDECL5_IVI(rettype, funcname, a1, a2, a3, a4, a5) rettype funcname(a1, a2, a3, a4, a5)
#define FCDECL5_VII(rettype, funcname, a1, a2, a3, a4, a5) rettype funcname(a1, a2, a3, a4, a5)

#endif // _X86_ 

#define HELPER_FRAME_DECL(x) FrameWithCookie<HelperMethodFrame_##x##OBJ> __helperframe

// use the capture state machinery if the architecture has one
//
// For a normal build we create a loop (see explaination on RestoreState below)
// We don't want a loop here for PREFAST since that causes 
//   warning 263: Using _alloca in a loop
// And we can't use DEBUG_OK_TO_RETURN for PREFAST because the PREFAST version 
// requires that you already be in a DEBUG_ASSURE_NO_RETURN_BEGIN scope 

#define HelperMethodFrame_0OBJ      HelperMethodFrame
#define HELPER_FRAME_ARGS(attribs)  __me, &__ms, attribs
#define FORLAZYMACHSTATE(x) x

  #define FORLAZYMACHSTATE_BEGINLOOP(x) x do
  #define FORLAZYMACHSTATE_ENDLOOP(x) while(x)
  #define FORLAZYMACHSTATE_DEBUG_OK_TO_RETURN_BEGIN DEBUG_OK_TO_RETURN_BEGIN
  #define FORLAZYMACHSTATE_DEBUG_OK_TO_RETURN_END DEBUG_OK_TO_RETURN_END

// We have to put DEBUG_OK_TO_RETURN_BEGIN around the FORLAZYMACHSTATE
// to allow the HELPER_FRAME to be installed inside an SO_INTOLERANT region
// which does not allow a return.  The return is used by FORLAZYMACHSTATE
// to capture the state, but is not an actual return, so it is ok.
#define HELPER_METHOD_FRAME_BEGIN_EX(ret, helperFrame, gcpoll,allowGC)  \
        FORLAZYMACHSTATE_BEGINLOOP(int alwaysZero = 0;)         \
        {                                                       \
            PERMIT_HELPER_METHOD_FRAME_BEGIN();                 \
            CHECK_HELPER_METHOD_FRAME_PERMITTED();              \
            FORLAZYMACHSTATE(LazyMachState __ms;)               \
            FORLAZYMACHSTATE_DEBUG_OK_TO_RETURN_BEGIN;          \
            FORLAZYMACHSTATE(CAPTURE_STATE(__ms, ret);)         \
            FORLAZYMACHSTATE_DEBUG_OK_TO_RETURN_END;            \
            helperFrame;                                        \
            DEBUG_ASSURE_NO_RETURN_BEGIN                        \
            ENDFORBIDGC();                                      \
            gcpoll;                                             \
            Thread *__pHMFThread = GetThread();                 \
            PAL_CPP_EHUNWIND_BEGIN;                             \
            TESTHOOKCALL(AppDomainCanBeUnloaded(__pHMFThread->GetDomain()->GetId().m_dwId,!allowGC));            \
            if (__pHMFThread->IsAbortRequested())               \
            {                                                   \
                __pHMFThread->HandleThreadAbort();              \
            }                                                   \
            if (__pHMFThread->HasThreadState(Thread::TS_YieldRequested))  \
            {                                                             \
                __SwitchToThread(0);                                      \
            }                                                             \
            INSTALL_UNWIND_AND_CONTINUE_HANDLER_NO_PAL_CPP_EHUNWIND;

// The while(__helperframe.RestoreState() needs a bit of explanation.
// The issue is insuring that the same machine state (which registers saved)
// exists when the machine state is probed (when the frame is created, and
// when it is actually used (when the frame is popped.  We do this by creating
// a flow of control from use to def.  Note that 'RestoreState' always returns false
// we never actually loop, but the compiler does not know that, and thus
// will be forced to make the keep the state of register spills the same at
// the two locations.
#define HELPER_METHOD_FRAME_END_EX(gcpoll,allowGC)                      \
            UNINSTALL_UNWIND_AND_CONTINUE_HANDLER_NO_PAL_CPP_EHUNWIND;      \
	    TESTHOOKCALL(AppDomainCanBeUnloaded(__pHMFThread->GetDomain()->GetId().m_dwId,!allowGC));            \
            if (__pHMFThread->HasThreadState(Thread::TS_AbortInitiated))    \
            {                                                               \
                __pHMFThread->HandleThreadAbort();                          \
            }                                                               \
            PAL_CPP_EHUNWIND_END;                                           \
            DEBUG_ASSURE_NO_RETURN_END                          \
            gcpoll;                                             \
            BEGINFORBIDGC();                                    \
            __helperframe.Pop();                                \
            FORLAZYMACHSTATE(alwaysZero =                       \
            HelperMethodFrameRestoreState(INDEBUG_COMMA(&__helperframe) \
                                          __helperframe.MachineState());) \
            PERMIT_HELPER_METHOD_FRAME_END()                    \
        } FORLAZYMACHSTATE_ENDLOOP(alwaysZero);

#define HELPER_METHOD_FRAME_BEGIN_ATTRIB(attribs)                                       \
        HELPER_METHOD_FRAME_BEGIN_EX(                                                   \
            return,                                                                     \
            HELPER_FRAME_DECL(0)(HELPER_FRAME_ARGS(attribs)),                           \
            HELPER_METHOD_POLL(),TRUE)

#define HELPER_METHOD_FRAME_BEGIN_0()                                                   \
        HELPER_METHOD_FRAME_BEGIN_ATTRIB(Frame::FRAME_ATTR_NONE)

#define HELPER_METHOD_FRAME_BEGIN_ATTRIB_NOPOLL(attribs)                                \
        HELPER_METHOD_FRAME_BEGIN_EX(                                                   \
            return,                                                                     \
            HELPER_FRAME_DECL(0)(HELPER_FRAME_ARGS(attribs)),                           \
            {},FALSE)

#define HELPER_METHOD_FRAME_BEGIN_NOPOLL() HELPER_METHOD_FRAME_BEGIN_ATTRIB_NOPOLL(Frame::FRAME_ATTR_NONE)

#define HELPER_METHOD_FRAME_BEGIN_ATTRIB_1(attribs, arg1)                               \
        HELPER_METHOD_FRAME_BEGIN_EX(                                                   \
            return,                                                                     \
            HELPER_FRAME_DECL(1)(HELPER_FRAME_ARGS(attribs),                            \
                (OBJECTREF*) &arg1),                                                    \
            HELPER_METHOD_POLL(),TRUE)

#define HELPER_METHOD_FRAME_BEGIN_1(arg1)  HELPER_METHOD_FRAME_BEGIN_ATTRIB_1(Frame::FRAME_ATTR_NONE, arg1)

#define HELPER_METHOD_FRAME_BEGIN_2(arg1, arg2)                                         \
        HELPER_METHOD_FRAME_BEGIN_EX(                                                   \
            return,                                                                     \
            HELPER_FRAME_DECL(2)(HELPER_FRAME_ARGS(Frame::FRAME_ATTR_NONE),             \
                (OBJECTREF*) &arg1, (OBJECTREF*) &arg2),                                \
            HELPER_METHOD_POLL(),TRUE)

#define HELPER_METHOD_FRAME_BEGIN_PROTECT(gc)                                           \
        HELPER_METHOD_FRAME_BEGIN_EX(                                                   \
            return,                                                                     \
            HELPER_FRAME_DECL(PROTECT)(HELPER_FRAME_ARGS(Frame::FRAME_ATTR_NONE),       \
                (OBJECTREF*)&(gc), sizeof(gc)/sizeof(OBJECTREF)),                       \
            HELPER_METHOD_POLL(),TRUE)

#define HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_NOPOLL(attribs)                            \
        HELPER_METHOD_FRAME_BEGIN_EX(                                                   \
            return 0,                                                                   \
            HELPER_FRAME_DECL(0)(HELPER_FRAME_ARGS(attribs)),                           \
            {},FALSE)

#define HELPER_METHOD_FRAME_BEGIN_RET_VC_ATTRIB_NOPOLL(attribs)                         \
        HELPER_METHOD_FRAME_BEGIN_EX(                                                   \
            FC_RETURN_VC(),                                                             \
            HELPER_FRAME_DECL(0)(HELPER_FRAME_ARGS(attribs)),                           \
            {},FALSE)

#define HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB(attribs)                                   \
        HELPER_METHOD_FRAME_BEGIN_EX(                                                   \
            return 0,                                                                   \
            HELPER_FRAME_DECL(0)(HELPER_FRAME_ARGS(attribs)),                           \
            HELPER_METHOD_POLL(),TRUE)

#define HELPER_METHOD_FRAME_BEGIN_RET_0()                                               \
        HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB(Frame::FRAME_ATTR_NONE)

#define HELPER_METHOD_FRAME_BEGIN_RET_VC_0()                                            \
        HELPER_METHOD_FRAME_BEGIN_EX(                                                   \
            FC_RETURN_VC(),                                                             \
            HELPER_FRAME_DECL(0)(HELPER_FRAME_ARGS(Frame::FRAME_ATTR_NONE)),            \
            HELPER_METHOD_POLL(),TRUE)

#define HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(attribs, arg1)                           \
        HELPER_METHOD_FRAME_BEGIN_EX(                                                   \
            return 0,                                                                   \
            HELPER_FRAME_DECL(1)(HELPER_FRAME_ARGS(attribs),                            \
                (OBJECTREF*) &arg1),                                                    \
            HELPER_METHOD_POLL(),TRUE)

#define HELPER_METHOD_FRAME_BEGIN_RET_VC_ATTRIB_1(attribs, arg1)                        \
        HELPER_METHOD_FRAME_BEGIN_EX(                                                   \
            FC_RETURN_VC(),                                                             \
            HELPER_FRAME_DECL(1)(HELPER_FRAME_ARGS(attribs),                            \
                (OBJECTREF*) &arg1),                                                    \
            HELPER_METHOD_POLL(),TRUE)

#define HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_2(attribs, arg1, arg2)                     \
        HELPER_METHOD_FRAME_BEGIN_EX(                                                   \
            return 0,                                                                   \
            HELPER_FRAME_DECL(2)(HELPER_FRAME_ARGS(attribs),                            \
                (OBJECTREF*) &arg1, (OBJECTREF*) &arg2),                                \
            HELPER_METHOD_POLL(),TRUE)

#define HELPER_METHOD_FRAME_BEGIN_RET_VC_ATTRIB_2(attribs, arg1, arg2)                  \
        HELPER_METHOD_FRAME_BEGIN_EX(                                                   \
            FC_RETURN_VC(),                                                             \
            HELPER_FRAME_DECL(2)(HELPER_FRAME_ARGS(attribs),                            \
                (OBJECTREF*) &arg1, (OBJECTREF*) &arg2),                                \
            HELPER_METHOD_POLL(),TRUE)

#define HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(attribs, gc)                       \
        HELPER_METHOD_FRAME_BEGIN_EX(                                                   \
            return 0,                                                                   \
            HELPER_FRAME_DECL(PROTECT)(HELPER_FRAME_ARGS(attribs),                      \
                (OBJECTREF*)&(gc), sizeof(gc)/sizeof(OBJECTREF)),                       \
            HELPER_METHOD_POLL(),TRUE)

#define HELPER_METHOD_FRAME_BEGIN_RET_VC_NOPOLL()                                       \
        HELPER_METHOD_FRAME_BEGIN_RET_VC_ATTRIB_NOPOLL(Frame::FRAME_ATTR_NONE)

#define HELPER_METHOD_FRAME_BEGIN_RET_NOPOLL()                                          \
        HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_NOPOLL(Frame::FRAME_ATTR_NONE)

#define HELPER_METHOD_FRAME_BEGIN_RET_1(arg1)                                           \
        HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_NONE, arg1)

#define HELPER_METHOD_FRAME_BEGIN_RET_VC_1(arg1)                                        \
        HELPER_METHOD_FRAME_BEGIN_RET_VC_ATTRIB_1(Frame::FRAME_ATTR_NONE, arg1)

#define HELPER_METHOD_FRAME_BEGIN_RET_2(arg1, arg2)                                     \
        HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_2(Frame::FRAME_ATTR_NONE, arg1, arg2)

#define HELPER_METHOD_FRAME_BEGIN_RET_VC_2(arg1, arg2)                                  \
        HELPER_METHOD_FRAME_BEGIN_RET_VC_ATTRIB_2(Frame::FRAME_ATTR_NONE, arg1, arg2)

#define HELPER_METHOD_FRAME_BEGIN_RET_PROTECT(gc)                                       \
        HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_PROTECT(Frame::FRAME_ATTR_NONE, gc)


#define HELPER_METHOD_FRAME_END()        HELPER_METHOD_FRAME_END_EX({},FALSE)  
#define HELPER_METHOD_FRAME_END_POLL()   HELPER_METHOD_FRAME_END_EX(HELPER_METHOD_POLL(),TRUE)  

    // This is the fastest way to do a GC poll if you have already erected a HelperMethodFrame
//#define HELPER_METHOD_POLL()             { __helperframe.Poll(); INCONTRACT(__fCallCheck.SetDidPoll()); }

#define HELPER_METHOD_POLL()             { }

// The HelperMethodFrame knows how to get its return address.  Let other code get at it, too.
//  (Uses comma operator to call InsureInit & discard result.  InsureInit returns something
//   that may be a return address.  Or it may be a pointer to a return address.  THIS construct
//   works correctly, and returns, by golly, a return address.)
#define HELPER_METHOD_FRAME_GET_RETURN_ADDRESS()                                        \
    ( static_cast<UINT_PTR>( (__helperframe.InsureInit(false, NULL)), (*__ms.pRetAddr()) ) )



    // Very short routines, or routines that are guarenteed to force GC or EH 
    // don't need to poll the GC.  USE VERY SPARINGLY!!!
#define FC_GC_POLL_NOT_NEEDED()    INCONTRACT(__fCallCheck.SetNotNeeded()) 

Object* FC_GCPoll(void* me, Object* objToProtect = NULL);

#define FC_GC_POLL_EX(pThread, ret)               {     \
    INCONTRACT(Thread::TriggersGC(pThread);)               \
    INCONTRACT(__fCallCheck.SetDidPoll();)                 \
    if (pThread->CatchAtSafePoint())                    \
        if (FC_GCPoll(__me))                            \
            return ret;                                 \
    }

#define FC_GC_POLL()        FC_GC_POLL_EX(GetThread(), ;)
#define FC_GC_POLL_RET()    FC_GC_POLL_EX(GetThread(), 0)

#define FC_GC_POLL_AND_RETURN_OBJREF(obj)   {           \
    INCONTRACT(__fCallCheck.SetDidPoll();)                 \
    Object* __temp = obj;                               \
    if (GetThread()->CatchAtSafePoint())                \
        return(FC_GCPoll(__me, __temp));                \
    return(__temp);                                     \
    }

#define FC_COMMON_PROLOG(target, assertFn) FCALL_TRANSITION_BEGIN()

// Macros that allows fcall to be split into two function to avoid the helper frame overhead on common fast codepaths.

#define FC_INNER_PROLOG(outterfuncname)                       \
    LPVOID __me;                                              \
    __me = GetEEFuncEntryPointMacro(outterfuncname);          \
    ENDFORBIDGC();                                             \
    INCONTRACT(FCallCheck __fCallCheck(__FILE__, __LINE__));

#define FC_INNER_EPILOG()                                      \
    BEGINFORBIDGC();                                           \
    return (LPVOID)1;

#define FC_INNER_RET LPVOID

#define FC_CALL_INNER(callinner)                               \
    while (NULL == callinner) { } // Dummy loop to ensure that helper frame interpreter will find its way out.

//==============================================================================================
// FIMPLn: A set of macros for generating the proto for the actual
// implementation (use FDECLN for header protos.)
//
// The hidden "__me" variable lets us recover the original MethodDesc*
// so any thrown exceptions will have the correct stack trace. FCThrow()
// passes this along to __FCThrowInternal(). 
//==============================================================================================

#define GetEEFuncEntryPointMacro(func)  ((LPVOID)(func))

#define FCIMPL_PROLOG(funcname)  \
    LPVOID __me; \
    __me = GetEEFuncEntryPointMacro(funcname); \
    FC_COMMON_PROLOG(__me, FCallAssert)


#ifdef _DEBUG

// Build the list of all fcalls signatures. It is used in binder.cpp to verify 
// compatibility of managed and unmanaged fcall signatures. The check is currently done 
// for x86 only.

struct FCSigCheck {
public:
    FCSigCheck(void* fnc, char* sig)
    {
        func = fnc;
        signature = sig;
        next = g_pFCSigCheck;
        g_pFCSigCheck = this;
    }

    FCSigCheck* next;
    void* func;
    char* signature;

    static FCSigCheck* g_pFCSigCheck;
};

#define FCSIGCHECK(funcname, signature) \
    static FCSigCheck UNIQUE_LABEL(FCSigCheck)(GetEEFuncEntryPointMacro(funcname), signature);

#else

#define FCSIGCHECK(funcname, signature)

#endif


#ifdef _X86_

#if defined(__GNUC__)

#define FCIMPL0(rettype, funcname) rettype F_CALL_CONV funcname() { FCIMPL_PROLOG(funcname)
#define FCIMPL1(rettype, funcname, a1) rettype F_CALL_CONV funcname(int /* EAX */, int /* EDX */, a1) { FCIMPL_PROLOG(funcname)
#define FCIMPL1_V(rettype, funcname, a1) rettype F_CALL_CONV funcname(int /* EAX */, int /* EDX */, int /* ECX */, a1) { FCIMPL_PROLOG(funcname)
#define FCIMPL2(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1) { FCIMPL_PROLOG(funcname)
#define FCIMPL2_VV(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(int /* EAX */, int /* EDX */, int /* ECX */, a2, a1) { FCIMPL_PROLOG(funcname)
#define FCIMPL2_VI(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(int /* EAX */, int /* EDX */, a2, a1) { FCIMPL_PROLOG(funcname)
#define FCIMPL2_IV(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(int /* EAX */, int /* EDX */, a1, a2) { FCIMPL_PROLOG(funcname)
#define FCIMPL3(rettype, funcname, a1, a2, a3) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a3) { FCIMPL_PROLOG(funcname)
#define FCIMPL3_VII(rettype, funcname, a1, a2, a3) rettype F_CALL_CONV funcname(int /* EAX */, a3, a2, a1) { FCIMPL_PROLOG(funcname)
#define FCIMPL3_IVV(rettype, funcname, a1, a2, a3) rettype F_CALL_CONV funcname(int /* EAX */, int /* EDX */, a1, a3, a2) { FCIMPL_PROLOG(funcname)
#define FCIMPL3_IVI(rettype, funcname, a1, a2, a3) rettype F_CALL_CONV funcname(int /* EAX */, a3, a1, a2) { FCIMPL_PROLOG(funcname)
#define FCIMPL3_VVI(rettype, funcname, a1, a2, a3) rettype F_CALL_CONV funcname(int /* EAX */, int /* EDX */, a3, a2, a1) {  FCIMPL_PROLOG(funcname)
#define FCIMPL4(rettype, funcname, a1, a2, a3, a4) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a4, a3) { FCIMPL_PROLOG(funcname)
#define FCIMPL5(rettype, funcname, a1, a2, a3, a4, a5) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a5, a4, a3) { FCIMPL_PROLOG(funcname)
#define FCIMPL6(rettype, funcname, a1, a2, a3, a4, a5, a6) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a6, a5, a4, a3) { FCIMPL_PROLOG(funcname)
#define FCIMPL7(rettype, funcname, a1, a2, a3, a4, a5, a6, a7) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a7, a6, a5, a4, a3) { FCIMPL_PROLOG(funcname)
#define FCIMPL8(rettype, funcname, a1, a2, a3, a4, a5, a6, a7, a8) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a8, a7, a6, a5, a4, a3) { FCIMPL_PROLOG(funcname)
#define FCIMPL9(rettype, funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a9, a8, a7, a6, a5, a4, a3) { FCIMPL_PROLOG(funcname)
#define FCIMPL10(rettype,funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a10, a9, a8, a7, a6, a5, a4, a3) { FCIMPL_PROLOG(funcname)
#define FCIMPL11(rettype,funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a11, a10, a9, a8, a7, a6, a5, a4, a3) { FCIMPL_PROLOG(funcname)
#define FCIMPL12(rettype,funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a12, a11, a10, a9, a8, a7, a6, a5, a4, a3) { FCIMPL_PROLOG(funcname)

#define FCIMPL5_IVI(rettype, funcname, a1, a2, a3, a4, a5) rettype F_CALL_CONV funcname(int /* EAX */, a3, a1, a5, a4, a2) { FCIMPL_PROLOG(funcname)
#define FCIMPL5_VII(rettype, funcname, a1, a2, a3, a4, a5) rettype F_CALL_CONV funcname(int /* EAX */, a3, a2, a5, a4, a1) { FCIMPL_PROLOG(funcname)

#else // __GNUC__

#define FCIMPL0(rettype, funcname) FCSIGCHECK(funcname, #rettype) \
    rettype F_CALL_CONV funcname() { FCIMPL_PROLOG(funcname)
#define FCIMPL1(rettype, funcname, a1) FCSIGCHECK(funcname, #rettype "," #a1) \
    rettype F_CALL_CONV funcname(a1) { FCIMPL_PROLOG(funcname)
#define FCIMPL1_V(rettype, funcname, a1) FCSIGCHECK(funcname, #rettype "," "V" #a1) \
    rettype F_CALL_CONV funcname(a1) { FCIMPL_PROLOG(funcname)
#define FCIMPL2(rettype, funcname, a1, a2) FCSIGCHECK(funcname, #rettype "," #a1 "," #a2) \
    rettype F_CALL_CONV funcname(a1, a2) { FCIMPL_PROLOG(funcname)
#define FCIMPL2VA(rettype, funcname, a1, a2) FCSIGCHECK(funcname, #rettype "," #a1 "," #a2 "," "...") \
    rettype F_CALL_VA_CONV funcname(a1, a2, ...) { FCIMPL_PROLOG(funcname)
#define FCIMPL2_VV(rettype, funcname, a1, a2) FCSIGCHECK(funcname, #rettype "," "V" #a1 "," "V" #a2) \
    rettype F_CALL_CONV funcname(a2, a1) { FCIMPL_PROLOG(funcname)
#define FCIMPL2_VI(rettype, funcname, a1, a2) FCSIGCHECK(funcname, #rettype "," "V" #a1 "," #a2) \
    rettype F_CALL_CONV funcname(a2, a1) { FCIMPL_PROLOG(funcname)
#define FCIMPL2_IV(rettype, funcname, a1, a2) FCSIGCHECK(funcname, #rettype "," #a1 "," "V" #a2) \
    rettype F_CALL_CONV funcname(a1, a2) { FCIMPL_PROLOG(funcname)
#define FCIMPL3(rettype, funcname, a1, a2, a3) FCSIGCHECK(funcname, #rettype "," #a1 "," #a2 "," #a3) \
    rettype F_CALL_CONV funcname(a1, a2, a3) { FCIMPL_PROLOG(funcname)
#define FCIMPL3_VII(rettype, funcname, a1, a2, a3) FCSIGCHECK(funcname, #rettype "," "V" #a1 "," #a2 "," #a3) \
    rettype F_CALL_CONV funcname(a2, a3, a1) { FCIMPL_PROLOG(funcname) 
#define FCIMPL3_IVV(rettype, funcname, a1, a2, a3) FCSIGCHECK(funcname, #rettype "," #a1 "," "V" #a2 "," "V" #a3) \
    rettype F_CALL_CONV funcname(a1, a3, a2) { FCIMPL_PROLOG(funcname)
#define FCIMPL3_IVI(rettype, funcname, a1, a2, a3) FCSIGCHECK(funcname, #rettype "," #a1 "," "V" #a2 "," #a3) \
    rettype F_CALL_CONV funcname(a1, a3, a2) { FCIMPL_PROLOG(funcname)
#define FCIMPL3_VVI(rettype, funcname, a1, a2, a3) FCSIGCHECK(funcname, #rettype "," "V" #a1 "," "V" #a2 "," #a3) \
    rettype F_CALL_CONV funcname(a2, a1, a3) { FCIMPL_PROLOG(funcname)
#define FCIMPL4(rettype, funcname, a1, a2, a3, a4) FCSIGCHECK(funcname, #rettype "," #a1 "," #a2 "," #a3 "," #a4) \
    rettype F_CALL_CONV funcname(a1, a2, a4, a3) { FCIMPL_PROLOG(funcname)
#define FCIMPL5(rettype, funcname, a1, a2, a3, a4, a5) FCSIGCHECK(funcname, #rettype "," #a1 "," #a2 "," #a3 "," #a4 "," #a5) \
    rettype F_CALL_CONV funcname(a1, a2, a5, a4, a3) { FCIMPL_PROLOG(funcname)
#define FCIMPL6(rettype, funcname, a1, a2, a3, a4, a5, a6) FCSIGCHECK(funcname, #rettype "," #a1 "," #a2 "," #a3 "," #a4 "," #a5 "," #a6) \
    rettype F_CALL_CONV funcname(a1, a2, a6, a5, a4, a3) { FCIMPL_PROLOG(funcname)
#define FCIMPL7(rettype, funcname, a1, a2, a3, a4, a5, a6, a7) FCSIGCHECK(funcname, #rettype "," #a1 "," #a2 "," #a3 "," #a4 "," #a5 "," #a6 "," #a7) \
    rettype F_CALL_CONV funcname(a1, a2, a7, a6, a5, a4, a3) { FCIMPL_PROLOG(funcname)
#define FCIMPL8(rettype, funcname, a1, a2, a3, a4, a5, a6, a7, a8) FCSIGCHECK(funcname, #rettype "," #a1 "," #a2 "," #a3 "," #a4 "," #a5 "," #a6 "," #a7 "," #a8) \
    rettype F_CALL_CONV funcname(a1, a2, a8, a7, a6, a5, a4, a3) { FCIMPL_PROLOG(funcname)
#define FCIMPL9(rettype, funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9) FCSIGCHECK(funcname, #rettype "," #a1 "," #a2 "," #a3 "," #a4 "," #a5 "," #a6 "," #a7 "," #a8 "," #a9) \
    rettype F_CALL_CONV funcname(a1, a2, a9, a8, a7, a6, a5, a4, a3) { FCIMPL_PROLOG(funcname)
#define FCIMPL10(rettype,funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) FCSIGCHECK(funcname, #rettype "," #a1 "," #a2 "," #a3 "," #a4 "," #a5 "," #a6 "," #a7 "," #a8 "," #a9 "," #a10) \
    rettype F_CALL_CONV funcname(a1, a2, a10, a9, a8, a7, a6, a5, a4, a3) { FCIMPL_PROLOG(funcname)
#define FCIMPL11(rettype,funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) FCSIGCHECK(funcname, #rettype "," #a1 "," #a2 "," #a3 "," #a4 "," #a5 "," #a6 "," #a7 "," #a8 "," #a9 "," #a10 "," #a11) \
    rettype F_CALL_CONV funcname(a1, a2, a11, a10, a9, a8, a7, a6, a5, a4, a3) { FCIMPL_PROLOG(funcname)
#define FCIMPL12(rettype,funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) FCSIGCHECK(funcname, #rettype "," #a1 "," #a2 "," #a3 "," #a4 "," #a5 "," #a6 "," #a7 "," #a8 "," #a9 "," #a10 "," #a11 "," #a12) \
    rettype F_CALL_CONV funcname(a1, a2, a12, a11, a10, a9, a8, a7, a6, a5, a4, a3) { FCIMPL_PROLOG(funcname)

#define FCIMPL5_IVI(rettype, funcname, a1, a2, a3, a4, a5) FCSIGCHECK(funcname, #rettype "," #a1 "," "V" #a2 "," #a3 "," #a4 "," #a5) \
    rettype F_CALL_CONV funcname(a1, a3, a5, a4, a2) { FCIMPL_PROLOG(funcname)
#define FCIMPL5_VII(rettype, funcname, a1, a2, a3, a4, a5) FCSIGCHECK(funcname, #rettype "," "V" #a1 "," #a2 "," #a3 "," #a4 "," #a5) \
    rettype F_CALL_CONV funcname(a2, a3, a5, a4, a1) { FCIMPL_PROLOG(funcname)

#endif // __GNUC__

#else
//
// non-x86 platforms don't have messed-up calling convention swizzling 
//

#define FCIMPL0(rettype, funcname) rettype funcname() { FCIMPL_PROLOG(funcname)
#define FCIMPL1(rettype, funcname, a1) rettype funcname(a1) {  FCIMPL_PROLOG(funcname)
#define FCIMPL1_V(rettype, funcname, a1) rettype funcname(a1) {  FCIMPL_PROLOG(funcname)
#define FCIMPL2(rettype, funcname, a1, a2) rettype funcname(a1, a2) {  FCIMPL_PROLOG(funcname)
#define FCIMPL2VA(rettype, funcname, a1, a2) rettype funcname(a1, a2, ...) {  FCIMPL_PROLOG(funcname)
#define FCIMPL2_VV(rettype, funcname, a1, a2) rettype funcname(a1, a2) {  FCIMPL_PROLOG(funcname)
#define FCIMPL2_VI(rettype, funcname, a1, a2) rettype funcname(a1, a2) {  FCIMPL_PROLOG(funcname)
#define FCIMPL2_IV(rettype, funcname, a1, a2) rettype funcname(a1, a2) {  FCIMPL_PROLOG(funcname)
#define FCIMPL3(rettype, funcname, a1, a2, a3) rettype funcname(a1, a2, a3) {  FCIMPL_PROLOG(funcname)
#define FCIMPL3_IVV(rettype, funcname, a1, a2, a3) rettype funcname(a1, a2, a3) {  FCIMPL_PROLOG(funcname)
#define FCIMPL3_VII(rettype, funcname, a1, a2, a3) rettype funcname(a1, a2, a3) {  FCIMPL_PROLOG(funcname)
#define FCIMPL3_IVI(rettype, funcname, a1, a2, a3) rettype funcname(a1, a2, a3) {  FCIMPL_PROLOG(funcname)
#define FCIMPL3_VVI(rettype, funcname, a1, a2, a3) rettype funcname(a1, a2, a3) {  FCIMPL_PROLOG(funcname)
#define FCIMPL4(rettype, funcname, a1, a2, a3, a4) rettype funcname(a1, a2, a3, a4) {  FCIMPL_PROLOG(funcname)
#define FCIMPL5(rettype, funcname, a1, a2, a3, a4, a5) rettype funcname(a1, a2, a3, a4, a5) {  FCIMPL_PROLOG(funcname)
#define FCIMPL6(rettype, funcname, a1, a2, a3, a4, a5, a6) rettype funcname(a1, a2, a3, a4, a5, a6) {  FCIMPL_PROLOG(funcname)
#define FCIMPL7(rettype, funcname, a1, a2, a3, a4, a5, a6, a7) rettype funcname(a1, a2, a3, a4, a5, a6, a7) {  FCIMPL_PROLOG(funcname)
#define FCIMPL8(rettype, funcname, a1, a2, a3, a4, a5, a6, a7, a8) rettype funcname(a1, a2, a3, a4, a5, a6, a7, a8) {  FCIMPL_PROLOG(funcname)
#define FCIMPL9(rettype, funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9) rettype funcname(a1, a2, a3, a4, a5, a6, a7, a8, a9) {  FCIMPL_PROLOG(funcname)
#define FCIMPL10(rettype,funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) rettype funcname(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) {  FCIMPL_PROLOG(funcname)
#define FCIMPL11(rettype,funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) rettype funcname(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) {  FCIMPL_PROLOG(funcname)
#define FCIMPL12(rettype,funcname, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) rettype funcname(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) {  FCIMPL_PROLOG(funcname)

#define FCIMPL5_IVI(rettype, funcname, a1, a2, a3, a4, a5) rettype __fastcall funcname(a1, a2, a3, a4, a5) { FCIMPL_PROLOG(funcname)
#define FCIMPL5_VII(rettype, funcname, a1, a2, a3, a4, a5) rettype __fastcall funcname(a1, a2, a3, a4, a5) { FCIMPL_PROLOG(funcname)

#endif

//==============================================================================================
// Use this to terminte an FCIMPLEND.
//==============================================================================================

#define FCIMPL_EPILOG()   FCALL_TRANSITION_END()

#define FCIMPLEND   FCIMPL_EPILOG(); }

#define HCIMPL_PROLOG(funcname) LPVOID __me; __me = 0; FC_COMMON_PROLOG(funcname, HCallAssert)

    // HCIMPL macros are just like their FCIMPL counterparts, however
    // they do not remember the function they come from. Thus they will not
    // show up in a stack trace.  This is what you want for JIT helpers and the like

#ifdef _X86_

#if defined(__GNUC__)

#define HCIMPL0(rettype, funcname) rettype F_CALL_CONV funcname() { HCIMPL_PROLOG(funcname)
#define HCIMPL1(rettype, funcname, a1) rettype F_CALL_CONV funcname(int /* EAX */, int /* EDX */, a1) { HCIMPL_PROLOG(funcname)
#define HCIMPL1_RAW(rettype, funcname, a1) rettype F_CALL_CONV funcname(int /* EAX */, int /* EDX */, a1) {
#define HCIMPL1_V(rettype, funcname, a1) rettype F_CALL_CONV funcname(int /* EAX */, int /* EDX */, int /* ECX */, a1) { HCIMPL_PROLOG(funcname)
#define HCIMPL2(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1) { HCIMPL_PROLOG(funcname)
#define HCIMPL2_RAW(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1) {
#define HCIMPL2_VV(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(int /* EAX */, int /* EDX */, int /* ECX */, a2, a1) { HCIMPL_PROLOG(funcname)
#define HCIMPL2_IV(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(int /* EAX */, int /* EDX */, a1, a2) { HCIMPL_PROLOG(funcname)
#define HCIMPL2VA(rettype, funcname, a1, a2) rettype F_CALL_VA_CONV funcname(a1, a2, ...) { HCIMPL_PROLOG(funcname)
#define HCIMPL3(rettype, funcname, a1, a2, a3) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a3) { HCIMPL_PROLOG(funcname)
#define HCIMPL4(rettype, funcname, a1, a2, a3, a4) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a4, a3) { HCIMPL_PROLOG(funcname)
#define HCIMPL5(rettype, funcname, a1, a2, a3, a4, a5) rettype F_CALL_CONV funcname(int /* EAX */, a2, a1, a5, a4, a3) { HCIMPL_PROLOG(funcname)

#define HCCALL1(funcname, a1)           funcname(0, 0, a1)
#define HCCALL1_V(funcname, a1)         funcname(0, 0, 0, a1)
#define HCCALL2(funcname, a1, a2)       funcname(0, a2, a1)
#define HCCALL3(funcname, a1, a2, a3)   funcname(0, a2, a1, a3)
#define HCCALL4(funcname, a1, a2, a3, a4)       funcname(0, a2, a1, a4, a3)
#define HCCALL5(funcname, a1, a2, a3, a4, a5)   funcname(0, a2, a1, a5, a4, a3)
#define HCCALL1_PTR(rettype, funcptr, a1)        rettype (F_CALL_CONV * funcptr)(int /* EAX */, int /* EDX */, a1)
#define HCCALL2_PTR(rettype, funcptr, a1, a2)    rettype (F_CALL_CONV * funcptr)(int /* EAX */, a2, a1)
#else

#define HCIMPL0(rettype, funcname) rettype F_CALL_CONV funcname() { HCIMPL_PROLOG(funcname) 
#define HCIMPL1(rettype, funcname, a1) rettype F_CALL_CONV funcname(a1) { HCIMPL_PROLOG(funcname)
#define HCIMPL1_RAW(rettype, funcname, a1) rettype F_CALL_CONV funcname(a1) {
#define HCIMPL1_V(rettype, funcname, a1) rettype F_CALL_CONV funcname(a1) { HCIMPL_PROLOG(funcname)
#define HCIMPL2(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(a1, a2) { HCIMPL_PROLOG(funcname)
#define HCIMPL2_RAW(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(a1, a2) {
#define HCIMPL2_VV(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(a2, a1) { HCIMPL_PROLOG(funcname)
#define HCIMPL2_IV(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(a1, a2) { HCIMPL_PROLOG(funcname)
#define HCIMPL2VA(rettype, funcname, a1, a2) rettype F_CALL_VA_CONV funcname(a1, a2, ...) { HCIMPL_PROLOG(funcname)
#define HCIMPL3(rettype, funcname, a1, a2, a3) rettype F_CALL_CONV funcname(a1, a2, a3) { HCIMPL_PROLOG(funcname)
#define HCIMPL4(rettype, funcname, a1, a2, a3, a4) rettype F_CALL_CONV funcname(a1, a2, a4, a3) { HCIMPL_PROLOG(funcname)
#define HCIMPL5(rettype, funcname, a1, a2, a3, a4, a5) rettype F_CALL_CONV funcname(a1, a2, a5, a4, a3) { HCIMPL_PROLOG(funcname)

#define HCCALL1(funcname, a1)           funcname(a1)
#define HCCALL1_V(funcname, a1)         funcname(a1)
#define HCCALL2(funcname, a1, a2)       funcname(a1, a2)
#define HCCALL3(funcname, a1, a2, a3)   funcname(a1, a2, a3)
#define HCCALL4(funcname, a1, a2, a3, a4)       funcname(a1, a2, a4, a3)
#define HCCALL5(funcname, a1, a2, a3, a4, a5)   funcname(a1, a2, a5, a4, a3)
#define HCCALL1_PTR(rettype, funcptr, a1)        rettype (F_CALL_CONV * funcptr)(a1)
#define HCCALL2_PTR(rettype, funcptr, a1, a2)    rettype (F_CALL_CONV * funcptr)(a1, a2)

#endif

#else
//
// non-x86 platforms don't have messed-up calling convention swizzling 
//

#define HCIMPL0(rettype, funcname) rettype F_CALL_CONV funcname() { HCIMPL_PROLOG(funcname) 
#define HCIMPL1(rettype, funcname, a1) rettype F_CALL_CONV funcname(a1) { HCIMPL_PROLOG(funcname)
#define HCIMPL1_RAW(rettype, funcname, a1) rettype F_CALL_CONV funcname(a1) {
#define HCIMPL1_V(rettype, funcname, a1) rettype F_CALL_CONV funcname(a1) { HCIMPL_PROLOG(funcname)
#define HCIMPL2(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(a1, a2) { HCIMPL_PROLOG(funcname)
#define HCIMPL2_RAW(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(a1, a2) {

// For historic reasons, the 64-bit JIT expects the arguments in reverse order for these
#define HCIMPL2_VV(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(a2, a1) { HCIMPL_PROLOG(funcname)

#define HCIMPL2_IV(rettype, funcname, a1, a2) rettype F_CALL_CONV funcname(a1, a2) { HCIMPL_PROLOG(funcname)
#define HCIMPL2VA(rettype, funcname, a1, a2) rettype F_CALL_VA_CONV funcname(a1, a2, ...) { HCIMPL_PROLOG(funcname)
#define HCIMPL3(rettype, funcname, a1, a2, a3) rettype F_CALL_CONV funcname(a1, a2, a3) { HCIMPL_PROLOG(funcname)
#define HCIMPL4(rettype, funcname, a1, a2, a3, a4) rettype F_CALL_CONV funcname(a1, a2, a3, a4) { HCIMPL_PROLOG(funcname)
#define HCIMPL5(rettype, funcname, a1, a2, a3, a4, a5) rettype F_CALL_CONV funcname(a1, a2, a3, a4, a5) { HCIMPL_PROLOG(funcname)

#define HCCALL1(funcname, a1)           funcname(a1)
#define HCCALL1_V(funcname, a1)         funcname(a1)
#define HCCALL2(funcname, a1, a2)       funcname(a1, a2)
#define HCCALL3(funcname, a1, a2, a3)   funcname(a1, a2, a3)
#define HCCALL4(funcname, a1, a2, a3, a4)       funcname(a1, a2, a3, a4)
#define HCCALL5(funcname, a1, a2, a3, a4, a5)   funcname(a1, a2, a3, a4, a5)
#define HCCALL1_PTR(rettype, funcptr, a1)        rettype (F_CALL_CONV * funcptr)(a1)
#define HCCALL2_PTR(rettype, funcptr, a1, a2)    rettype (F_CALL_CONV * funcptr)(a1, a2)

#endif

#define HCIMPLEND_RAW   }
#define HCIMPLEND       FCALL_TRANSITION_END(); }


//==============================================================================================
// Throws an exception from an FCall. See rexcep.h for a list of valid
// exception codes.
//==============================================================================================
#define FCThrow(reKind) FCThrowEx(reKind, 0, 0, 0, 0)

//==============================================================================================
// This version lets you attach a message with inserts (similar to
// COMPlusThrow()).
//==============================================================================================
#define FCThrowEx(reKind, resID, arg1, arg2, arg3)              \
    {                                                           \
        while (NULL ==                                          \
            __FCThrow(__me, reKind, resID, arg1, arg2, arg3)) {}; \
        return 0;                                               \
    }

//==============================================================================================
// Like FCThrow but can be used for a VOID-returning FCall. The only
// difference is in the "return" statement.
//==============================================================================================
#define FCThrowVoid(reKind) FCThrowExVoid(reKind, 0, 0, 0, 0)

//==============================================================================================
// This version lets you attach a message with inserts (similar to
// COMPlusThrow()).
//==============================================================================================
#define FCThrowExVoid(reKind, resID, arg1, arg2, arg3)          \
    {                                                           \
        while (NULL ==                                          \
            __FCThrow(__me, reKind, resID, arg1, arg2, arg3)) {}; \
        return;                                                 \
    }

// Use FCThrowRes to throw an exception with a localized error message from the
// ResourceManager in managed code.
#define FCThrowRes(reKind, resourceName) FCThrowArgumentEx(reKind, NULL, resourceName)
#define FCThrowArgumentNull(argName) FCThrowArgumentEx(kArgumentNullException, argName, NULL)
#define FCThrowArgumentOutOfRange(argName, message) FCThrowArgumentEx(kArgumentOutOfRangeException, argName, message)
#define FCThrowArgument(argName, message) FCThrowArgumentEx(kArgumentException, argName, message)

#define FCThrowArgumentEx(reKind, argName, resourceName)        \
    {                                                           \
        while (NULL ==                                                  \
            __FCThrowArgument(__me, reKind, argName, resourceName)) {}; \
        return 0;                                               \
    }

// Use FCThrowRes to throw an exception with a localized error message from the
// ResourceManager in managed code.
#define FCThrowResVoid(reKind, resourceName) FCThrowArgumentVoidEx(reKind, NULL, resourceName)
#define FCThrowArgumentNullVoid(argName) FCThrowArgumentVoidEx(kArgumentNullException, argName, NULL)
#define FCThrowArgumentOutOfRangeVoid(argName, message) FCThrowArgumentVoidEx(kArgumentOutOfRangeException, argName, message)
#define FCThrowArgumentVoid(argName, message) FCThrowArgumentVoidEx(kArgumentException, argName, message)

#define FCThrowArgumentVoidEx(reKind, argName, resourceName)    \
    {                                                           \
        while (NULL ==                                                  \
            __FCThrowArgument(__me, reKind, argName, resourceName)) {};  \
        return;                                                 \
    }



// The x86 JIT calling convention expects returned small types (e.g. bool) to be
// widened on return. The C/C++ calling convention does not guarantee returned
// small types to be widened. The small types has to be artifically widened on return
// to fit x86 JIT calling convention. Thus fcalls returning small types has to
// use the FC_XXX_RET types to force C/C++ compiler to do the widening.
//
// The most common small return type of FCALLs is bool. The widening of bool is
// especially tricky since the value has to be also normalized. FC_BOOL_RET and 
// FC_RETURN_BOOL macros are provided to make it fool-proof. FCALLs returning bool
// should be implemented using following pattern:

// FCIMPL0(FC_BOOL_RET, Foo)    // the return type should be FC_BOOL_RET
//      BOOL ret;
//
//      FC_RETURN_BOOL(ret);    // return statements should be FC_RETURN_BOOL
// FCIMPLEND

// This rules are verified in binder.cpp if COMPlus_ConsistencyCheck is set.


#if defined(_X86_) || defined(_AMD64_)
// The return value is artifically widened on x86 and amd64
typedef INT32 FC_BOOL_RET;
#else
typedef CLR_BOOL FC_BOOL_RET;
#endif

#define FC_RETURN_BOOL(x)   do { return !!(x); } while(0)



#if defined(_X86_) || defined(_AMD64_)
// The return value is artifically widened on x86 and amd64
typedef UINT32 FC_CHAR_RET;
typedef INT32 FC_INT8_RET;
typedef UINT32 FC_UINT8_RET;
typedef INT32 FC_INT16_RET;
typedef UINT32 FC_UINT16_RET;
#else
typedef CLR_CHAR FC_CHAR_RET;
typedef INT8 FC_INT8_RET;
typedef UINT8 FC_UINT8_RET;
typedef INT16 FC_INT16_RET;
typedef UINT16 FC_UINT16_RET;
#endif




// The fcall entrypoints has to be at unique addresses. Use this helper macro to make 
// the code of the fcalls unique if you get assert in ecall.cpp that mentions it.
// The parameter of the FCUnique macro is an arbitrary 32-bit random number.
#define FCUnique(unique) { static volatile int u = (unique); while (u == 0) { }; }



#endif //__FCall_h__
