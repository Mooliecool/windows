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
// Contract.h
// ---------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// CONTRACT / CONTRACT_END is a container for a set of checked declarations about a
// function.  Use of this declaration syntax allows all of the functional
// annotations to be grouped together at the start of a function.
//
// One of the primary motivations for this is that we can perform certain
// checks even in absence of a declaration - in effect we get a default
// declaration mechanism. Another benefit is that developers get more of a
// "laundry list" of checks to consider to help make checking more complete.
//
// Currently, the following annotations are legal:
//
//      THROWS          the function itself has a code path that throws an exception
//      -or- NOTHROW    the function will never allow an exception to be thrown.
//      -or-            the default is THROWS
//
//      INJECT_FAULT(statement) the function will consult the fault injection
//                              framework, and if instructed to inject, executes the
//                              specified statement.
//
//      FAULT_FORBID    this function cannot invoke an INJECT_FAULT function (which
//                      includes all allocators)
//
//
//      MODE_COOPERATIVE        the function requires Cooperative GC mode on entry
//      -or- MODE_PREEMPTIVE    the function requires Preemptive GC mode on entry
//      -or- MODE_ANY           the function can be entered in either mode
//      -or-                    the default is MODE_ANY
//
//      GC_TRIGGERS             the function has a code path which may trigger a GC
//      GC_NOTRIGGER            the function will never trigger a GC provided its
//                                  called in coop mode.
//      -or-                    the default is currently a weak condition that asserts
//                                  neither of the above
//
//      SO_INTOLERANT           the function cannot tolerat an SO at any point and must run behind an
//                              an SO probe via BEGIN_SO_INTOLERANT_XXX.  This is the default. We want most
//                              of our code to run behind an SO probe.  The only time you need to explicitly
//                              mark something as SO_INTOLERANT is if the static analysis tool incorrectly
//                              flags it as an entry point.
//      -or- SO_TOLERANT        the function can tolerate an SO.  It either does not update any global state
//                              that needs to be cleaned up should a random SO occur, or it protects those
//                              updates behind an SO probe.
//      -or- SO_NOT_MAINLINE    the function is not hardened to SO and should never run on a managed thread
//                              where we need to be hardened to SO.  You can use this for functions that run
//                              only for ngen or Win9X etc.
//
//      PRECONDITION(X) -   generic CHECK or BOOL expression which should be true
//                          on function entry
//
//      POSTCONDITION(X) -  generic CHECK or BOOL expression which should be true
//                          on function entry.  Note that variable RETVAL will be
//                          available for use in the expression.
//
//      INSTANCE_CHECK -    equivalent of:
//                          PRECONDITION(CheckPointer(this));
//                          POSTCONDITION(CheckInvariant(this));
//      INSTANCE_CHECK_NULL - equivalent of:
//                          PRECONDITION(CheckPointer(this, NULL_OK));
//                          POSTCONDITION(CheckInvariant(this, NULL_OK));
//      CONSTRUCTOR_CHECK - equivalent of:
//                          POSTCONDITION(CheckPointer(this));
//      DESTRUCTOR_CHECK -  equivalent of:
//                          PRECONDITION(CheckPointer(this));
//
// You must use the RETURN macro rather than the return keyword in the body of a contract.
// This ensures proper checking of postconditions.
//
// To include a contract declaration, but not enforce it at runtime, use the UNCHECKED
// macro around the declaration.  This might be for instance if it is too expensive to
// check.
//
// When a function does not explicitly caused a condition, use the WRAPPER macro around
// the declaration.  This implies that the function is dependent on the functions it calls
// for its behaviour, and guarantees nothing.
//
// When an assertion is supposed to be checked but is causing temporary problems, use
// the DISABLED macro around the declaration.  DISABLED implies a temporary condition
// which needs to be fixed before shipping.>
//
// SAMPLE:
//
//        int foo(int bar, void *baz)
//        {
//            CONTRACT(int)
//            {
//                WRAPPER(THROWS);  // foo() does not throw, but blap(), which foo() calls, may or may not.
//
//                INJECT_FAULT(return 483);
//
//                PRECONDITION(bar == 37);
//                PRECONDITION(baz != NULL);
//
//                POSTCONDITION(RETVAL != 0);
//
//                DISABLED(PRECONDITION(BrokenRoutineShouldSucceed(bar)));
//
//                UNCHECKED(PRECONDITION(IsAnswerToLifeUniverseEtc(bar)));
//            }
//            CONTRACT_END;
//
//            blap(bar);
//
//            RETURN 0;
//        }
//
//        void foo(int bar)
//        {
//            CONTRACT_VOID
//            {
//                NOTHROW;
//
//                PRECONDITION(bar == 37);
//            }
//            CONTRACT_END;
//
//            RETURN;
//        }
//--------------------------------------------------------------------------------

#ifndef CONTRACT_H_
#define CONTRACT_H_

#ifdef _MSC_VER
#pragma warning(disable:4189) //local variable is initialized but not referenced
#endif


// We only enable contracts in _DEBUG builds, and only if it isn't a Rotor build.

// Also, we won't enable contracts if this is a DAC build.

// Finally, only define the implementaiton parts of contracts if this isn't a DAC build.

#include "specstrings.h"
#include "clrtypes.h"
#include "malloc.h"
#include "check.h"
#include "debugreturn.h"
#include "staticcontract.h"






#ifdef _DEBUG

// Valid parameters for CONTRACT_VIOLATION macro
enum ContractViolationBits
{
    ThrowsViolation = 0x00000001,  // suppress THROW tags in this scope
    GCViolation     = 0x00000002,  // suppress GCTRIGGER tags in this scope
    ModeViolation   = 0x00000004,  // suppress MODE_PREEMP and MODE_COOP tags in this scope
    FaultViolation  = 0x00000008,  // suppress INJECT_FAULT assertions in this scope
    FaultNotFatal   = 0x00000010,  // suppress INJECT_FAULT but not fault injection by harness
    SOToleranceViolation = 0x00000020,  // suppress SO_TOLERANCE tags in this scope
    GlobalStateViolation = 0x00000020,
    HostViolation = 0x00000030,  // supress HOSTTRIGGER tags in this scope

    //These are not violation bits. We steal some bits out of the violation mask to serve as
    // general flag bits.
    CanFreeMe       = 0x00010000,  // If this bit is ON, the ClrDebugState was allocated by
                                   // a version of utilcode that registers an Fls Callback to free
                                   // the state. If this bit is OFF, the ClrDebugState was allocated
                                   // by an old version of utilcode that doesn't. (And you can't
                                   // assume that the old utilcode used the same allocator as the new utilcode.)
                                   // (Most likely, this is because you are using an older shim with
                                   // a newer mscorwks.dll)
                                   //
                                   // The Fls callback must only attempt to free debugstates that
                                   // have this bit on.

    BadDebugState   = 0x00020000,  // If we OOM creating the ClrDebugState, we return a pointer to
                                   // a static ClrDebugState that has this bit turned on. (We don't
                                   // want to slow down contracts with null tests everywhere.)
                                   // Other than this specific bit, all other fields of the DebugState
                                   // must be considered trash. You can stomp on them and you can bit-test them
                                   // but you can't throw up any asserts based on them and you certainly
                                   // can't deref any pointers stored in the bad DebugState.

};

#endif

#define CUSTOM_CONTRACT(_contracttype, _returntype)         if (0) {  struct YouCannotUseThisHere { int x; };   // This hacky typedef allows retail use of
#define CUSTOM_CONTRACT_VOID(_contracttype)                 if (0) {  struct YouCannotUseThisHere { int x; };   // FORBIDGC_LOADER_USE_ENABLED
#define CUSTOM_CONTRACTL(_contracttype)                     if (0) {  struct YouCannotUseThisHere { int x; };   // inside contracts and asserts but nowhere else.

#define INJECT_FAULT(_statement)
#define FORBID_FAULT
#define THROWS
#define NOTHROW
#define SO_TOLERANT
#define SO_INTOLERANT
#define SO_NOT_MAINLINE
#define ENTRY_POINT

#ifdef _DEBUG
// This can only appear in a debug function so don't define it non-debug
#define DEBUG_ONLY STATIC_CONTRACT_DEBUG_ONLY
#endif

#define PRECONDITION_MSG(_expression, _message)     do { } while(0)
#define PRECONDITION(_expression)                   do { } while(0)
#define POSTCONDITION_MSG(_expression, _message)    do { } while(0)
#define POSTCONDITION(_expression)                  do { } while(0)
#define INSTANCE_CHECK
#define INSTANCE_CHECK_NULL
#define CONSTRUCTOR_CHECK
#define DESTRUCTOR_CHECK
#define UNCHECKED(thecheck)
#define DISABLED(thecheck)
#define WRAPPER(thecheck)
#define ENABLED(_check)
#define CONTRACT_END                                        }
#define CONTRACTL_END                                       }

#define CUSTOM_LEAF_CONTRACT(_contracttype) \
    {                                                                                       \
        /* Should add some assertion mechanism to ensure one other contract is called */    \
        STATIC_CONTRACT_LEAF;                                                            \
    }
#define CUSTOM_WRAPPER_CONTRACT(_contracttype) \
    {                                                                                       \
        /* Should add some assertion mechanism to ensure one other contract is called */    \
        STATIC_CONTRACT_WRAPPER;                                                            \
    }


#define RETURN return
#define RETURN_VOID RETURN

#define CONTRACT_THROWS()
#define CONTRACT_THROWSEX(__func, __file, __line)


#define CONTRACT(_returntype)  CUSTOM_CONTRACT(Contract, _returntype)
#define CONTRACT_VOID  CUSTOM_CONTRACT_VOID(Contract)
#define CONTRACTL  CUSTOM_CONTRACTL(Contract)
#define LEAF_CONTRACT CUSTOM_LEAF_CONTRACT(Contract)
#define WRAPPER_CONTRACT CUSTOM_WRAPPER_CONTRACT(Contract)

// GC_NOTRIGGER allowed but not currently enforced at runtime
#define GC_NOTRIGGER STATIC_CONTRACT_GC_NOTRIGGER
#define GC_TRIGGERS __error("TriggersGC not supported in utilcode contracts")


#define BEGIN_CONTRACT_VIOLATION(violationmask)
#define RESET_CONTRACT_VIOLATION()
#define END_CONTRACT_VIOLATION
#define CONTRACT_VIOLATION(violationmask)






#define FAULT_FORBID() ;
#define FAULT_FORBID_NO_ALLOC() ;
#define MAYBE_FAULT_FORBID(cond) ;
#define MAYBE_FAULT_FORBID_NO_ALLOC(cond) ;




// If you got an error about ARE_FAULTS_FORBIDDEN being undefined, it's because you tried
// to use this predicate in a free build outside of a CONTRACT or ASSERT.
//
#define ARE_FAULTS_FORBIDDEN() (sizeof(YouCannotUseThisHere) != 0)


// This allows a fault-forbid region to invoke a non-mandatory allocation, such as for the
// purpose of growing a lookaside cache (if the allocation fails, the code can abandon the
// cache growing operation without negative effect.)
//
// Although it's implemented using CONTRACT_VIOLATION(), it's not a bug to have this in the code.
//
// It *is* a bug to use this to hide a situation where an OOM is genuinely fatal but not handled.
#define FAULT_NOT_FATAL() CONTRACT_VIOLATION(FaultNotFatal)

#define CLR_TRY_MARKER()


#define SO_NOT_MAINLINE_FUNCTION STATIC_CONTRACT_SO_NOT_MAINLINE
#define ENTER_SO_NOT_MAINLINE_CODE
#define LEAVE_SO_NOT_MAINLINE_CODE

#define BEGIN_HOST_NOCALL_CODE
#define END_HOST_NOCALL_CODE

#ifdef _DEBUG
#define DEBUG_ONLY_FUNCTION STATIC_CONTRACT_DEBUG_ONLY
#define BEGIN_DEBUG_ONLY_CODE
#define END_DEBUG_ONLY_CODE
#endif

//
//
// Nothing in retail since this holder just disabled an assert.
#define SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE


#endif  // CONTRACT_H_
