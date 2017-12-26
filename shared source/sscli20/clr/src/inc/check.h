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
// Check.h
//
// Assertion checking infrastructure
// ---------------------------------------------------------------------------

#ifndef CHECK_H_
#define CHECK_H_

#ifdef _DEBUG

#ifdef _MSC_VER
// Make sure we can recurse deep enough for FORCEINLINE
#pragma inline_recursion(on)
#pragma inline_depth(16)
#pragma warning(disable:4714)
#endif // _MSC_VER


#endif  // _DEBUG

#if defined(_DEBUG) && !defined(DACCESS_COMPILE)
#define _DEBUG_IMPL 1
#endif

#ifdef _DEBUG
#define DEBUG_ARG(x)  , x
#else
#define DEBUG_ARG(x) 
#endif

#define CHECK_STRESS 1

// Static assert
//
template < bool >
class static_assert_failure;

template <>
class static_assert_failure< true > {};

template < int >
class static_assert_check {};

#define STRING_JOIN2( x, y ) x##y
#define STRING_JOIN( x, y ) STRING_JOIN2( x, y )

#define STATIC_ASSERT( cond ) \
    typedef static_assert_check< sizeof( static_assert_failure< (bool)(cond) > ) > \
        STRING_JOIN( static_assert_failure_, __LINE__ )

//--------------------------------------------------------------------------------
// A CHECK is an object which encapsulates a potential assertion
// failure.  It not only contains the result of the check, but if the check fails,
// also records information about the condition and call site.
//
// CHECK also serves as a holder to prevent recursive CHECKS. These can be 
// particularly common when putting preconditions inside predicates, especially
// routines called by an invariant.
//
// Note that using CHECK is perfectly efficient in a free build - the CHECK becomes
// a simple string constant pointer (typically either NULL or (LPCSTR)1, although some 
// check failures may include messages)
//
// NOTE: you should NEVER use the CHECK class API directly - use the macros below.
//--------------------------------------------------------------------------------

class SString;

class CHECK
{
 protected:
    // On retail, this is a pointer to a string literal, null or (LPCSTR)1.
    // On debug, this is a pointer to dynamically allocated memory - that
    // lets us have formatted strings in debug builds.
    LPCSTR  m_message;

#ifdef _DEBUG
    LPCSTR  m_condition;
    LPCSTR  m_file;
    INT     m_line;

    // Keep leakage counters.
    static  size_t s_cLeakedBytes;
    static  size_t s_cNumFailures;
#endif

    static BOOL s_neverEnforceAsserts;

 public: // !!! NOTE: Called from macros only!!!

    // If we are not in a check, return TRUE and PushCheck; otherwise return FALSE
    static BOOL EnterAssert();

    // Pops check count
    static void LeaveAssert();

    // Just return if we are in a check
    static BOOL IsInAssert();

    // Should we skip enforcing asserts
    static BOOL EnforceAssert();

    CHECK() : m_message(NULL)
#ifdef _DEBUG
              , m_condition (NULL)
              , m_file(NULL)
              , m_line(NULL)
#endif
    {}

    // Fail records the result of a condition check.  Can take either a
    // boolean value or another check result
    BOOL Fail(BOOL condition);
    BOOL Fail(const CHECK &check);

    // Setup records context info after a failure.
    void Setup(LPCSTR message DEBUG_ARG(LPCSTR condition) DEBUG_ARG(LPCSTR file) DEBUG_ARG(INT line));
    static LPCSTR FormatMessage(LPCSTR messageFormat, ...);

    // Trigger triggers the actual check failure.  The trigger may provide a reason
    // to include in the failure message.
    void Trigger(LPCSTR reason);

    // Finally, convert to a BOOL to allow just testing the result of a Check function
    operator BOOL();

    BOOL operator!();

    CHECK &operator()() { return *this; }

    static inline const CHECK OK() {
        return CHECK();
    }

    static void SetAssertEnforcement(BOOL value);

  private:
#ifdef _DEBUG
    static LPCSTR AllocateDynamicMessage(const SString &s);
#endif
};


//--------------------------------------------------------------------------------
// These CHECK macros are the correct way to propagate an assertion.  These
// routines are designed for use inside "Check" routines.  Such routines may
// be Invariants, Validate routines, or any other assertional predicates.
// 
// A Check routine should return a value of type CHECK. 
//
// It should consist of multiple CHECK or CHECK_MSG statements (along with appropritate 
// control flow) and should end with CHECK_OK() if all other checks pass.
//
// It may contain a CONTRACT_CHECK contract, but this is only appropriate if the
// check is used for non-assertional purposes (otherwise the contract will never execute).  
// Note that CONTRACT_CHECK contracts do not support postconditions.
//
// CHECK: Check the given condition, return a CHECK failure if FALSE
// CHECK_MSG: Same, but include a message paramter if the check fails
// CHECK_OK: Return a successful check value;
//--------------------------------------------------------------------------------

#ifdef _DEBUG
#define DEBUG_ONLY_MESSAGE(msg)     msg
#else
// On retail, we don't want to add a bunch of string literals to the image, 
// so we just use the same one everywhere.
#define DEBUG_ONLY_MESSAGE(msg)     ((LPCSTR)1)
#endif

#define CHECK_MSG_EX(_condition, _message, _RESULT)                 \
do                                                                  \
{                                                                   \
    CHECK _check;                                                   \
    if (_check.Fail(_condition))                                    \
    {                                                               \
        _check.Setup(DEBUG_ONLY_MESSAGE(_message)                   \
            DEBUG_ARG(#_condition)                                  \
            DEBUG_ARG(__FILE__)                                     \
            DEBUG_ARG(__LINE__));                                   \
        _RESULT(_check);                                            \
    }                                                               \
} while (0)

#define RETURN_RESULT(r) return r

#define CHECK_MSG(_condition, _message)                             \
    CHECK_MSG_EX(_condition, _message, RETURN_RESULT)

#define CHECK(_condition)                                           \
    CHECK_MSG(_condition, "")

#define CHECK_MSGF(_condition, _args)                               \
    CHECK_MSG(_condition, CHECK::FormatMessage _args)

#define CHECK_FAIL(_message)                                        \
    CHECK_MSG(FALSE, _message); UNREACHABLE()

#define CHECK_FAILF(_args)                                          \
    CHECK_MSGF(FALSE, _args); UNREACHABLE()

#define CHECK_OK                                                    \
    return CHECK::OK()

//--------------------------------------------------------------------------------
// ASSERT_CHECK is the proper way to trigger a check result.  If the CHECK 
// has failed, the diagnostic assertion routines will fire with appropriate 
// context information.
//
// Note that the condition may either be a raw boolean expression or a CHECK result
// returned from a Check routine.
//
// Recursion note: ASSERT_CHECKs are only performed if there is no current check in 
// progress.
//--------------------------------------------------------------------------------

class CHECK_AssertHolder
{
    BOOL m_bEntered;
    public:
        BOOL Enter()
        {
            m_bEntered=CHECK::EnterAssert();
            return m_bEntered;
        }
        ~CHECK_AssertHolder()
        {
            if(m_bEntered)
                CHECK::LeaveAssert();                                                   
        }
};

#define ASSERT_CHECK(_condition, _message, _reason)                             \
do                                                                              \
{                                                                               \
    CHECK _check;                                                               \
    CHECK_AssertHolder checkassert;                                         \
    if (checkassert.Enter())                                                   \
    {                                                                           \
        if (_check.Fail(_condition))                                            \
        {                                                                       \
            _check.Setup(_message                                               \
                DEBUG_ARG(#_condition)                                          \
                DEBUG_ARG(__FILE__)                                             \
                DEBUG_ARG(__LINE__));                                           \
            _check.Trigger(_reason);                                            \
        }                                                                       \
    }                                                                           \
} while (0)

// ex: ASSERT_CHECKF(1+2==4, "my reason", ("Woah %d", 1+3));
// note that the double parenthesis, the 'args' param below will include one pair of parens.
#define ASSERT_CHECKF(_condition, _reason, _args)                               \
    ASSERT_CHECK(_condition, CHECK::FormatMessage _args, _reason) 

//--------------------------------------------------------------------------------
// INVARIANTS are descriptions of conditions which are always true at well defined
// points of execution.  Invariants may be checked by the caller or callee at any
// time as paranoia requires.  
//
// There are really two flavors of invariant.  The "public invariant" describes
// to the caller invariant behavior about the abstraction which is visible from 
// the public API (and of course it should be expressible in that public API).  
// 
// The "internal invariant" (or representation invariant), on the other hand, is 
// a description of the private implementation of the abstraction, which may examine 
// internal state of the abstraction or use private entry points.
//
// Classes with invariants should introduce methods called 
// void Invariant();
// and 
// void InternalInvariant();
// to allow invariant checks.
//--------------------------------------------------------------------------------

#if CHECK_INVARIANTS

template <typename TYPENAME>
CHECK CheckInvariant(TYPENAME &obj)
{
    __if_exists(TYPENAME::Invariant)
    {
        CHECK(obj.Invariant());
    }
    __if_exists(TYPENAME::InternalInvariant)
    {
        CHECK(obj.InternalInvariant());
    }

    CHECK_OK;
}

#define CHECK_INVARIANT(o) \
    ASSERT_CHECK(CheckInvariant(o), NULL, "Invariant failure")

#else

#define CHECK_INVARIANT(o)  do { } while (0)

#endif

//--------------------------------------------------------------------------------
// VALIDATE is a check to be made on an object type which identifies a pointer as 
// a valid instance of the object, by calling CheckPointer on it.  Normally a null 
// pointer is treated as an error; VALIDATE_NULL (or CheckPointer(o, NULL_OK)) 
// may be used when a null pointer is acceptible.  
// 
// In addition to the null/non-null check, a type may provide a specific Check method 
// for more sophisticated identification. In general, the Check method 
// should answer the question
// "Is this a valid instance of its declared compile-time type?". For instance, if 
// runtype type identification were supported for the type, it should be invoked here.
// 
// Note that CheckPointer will also check the invariant(s) if appropriate, so the 
// invariants should NOT be explicitly invoked from the Check method.
//--------------------------------------------------------------------------------

enum IsNullOK
{
    NULL_NOT_OK = 0,
    NULL_OK = 1
};

#if CHECK_INVARIANTS
template <typename TYPENAME>
CHECK CheckPointer(TYPENAME *o, IsNullOK ok = NULL_NOT_OK)
{
    if (o == NULL)
    {
        CHECK_MSG(ok, "Illegal null pointer");
    }
    else
    {
        __if_exists(TYPENAME::Check)
        {
            CHECK(o->Check());
        }
    }

    CHECK_OK;
}

template <typename TYPENAME>
CHECK CheckValue(TYPENAME &val)
{
    __if_exists(TYPENAME::Check)
    {
        CHECK(val.Check());
    }

    CHECK(CheckInvariant(val));

    CHECK_OK;
}
#else // CHECK_INVARIANTS

#ifdef _DEBUG_IMPL
// Don't defined these functions to be nops for the non-debug
// build as it may hide important checks
template <typename TYPENAME>
CHECK CheckPointer(TYPENAME *o, IsNullOK ok = NULL_NOT_OK)
{
    CHECK_OK;
}

template <typename TYPENAME>
CHECK CheckValue(TYPENAME &val)
{
    CHECK_OK;
}
#endif

#endif  // CHECK_INVARIANTS

#if VALIDATE_OBJECTS

#define VALIDATE(o) \
    ASSERT_CHECK(CheckPointer(o), "Validation failure")    
#define VALIDATE_NULL(o) \
    ASSERT_CHECK(CheckPointer(o, NULL_OK), "Validation failure")

#else

#define VALIDATE(o)         do { } while (0)
#define VALIDATE_NULL(o)    do { } while (0)

#endif

//--------------------------------------------------------------------------------
// CONSISTENCY_CHECKS are ad-hoc assertions about the expected state of the program
// at a given time.  A failure in one of these indicates a bug in the code.
//
// Note that the condition may either be a raw boolean expression or a CHECK result
// returned from a Check routine.
//--------------------------------------------------------------------------------

#define CONSISTENCY_CHECK(_condition) \
    CONSISTENCY_CHECK_MSG(_condition, "")

#ifdef _DEBUG_IMPL

#define CONSISTENCY_CHECK_MSG(_condition, _message) \
    ASSERT_CHECK(_condition, _message, "Consistency check failed")

#define CONSISTENCY_CHECK_MSGF(_condition, args) \
    ASSERT_CHECKF(_condition, "Consistency check failed", args)

#else

#define CONSISTENCY_CHECK_MSG(_condition, _message) do { } while (0)
#define CONSISTENCY_CHECK_MSGF(_condition, args) do { } while (0)

#endif

//--------------------------------------------------------------------------------
// SIMPLIFYING_ASSUMPTIONS are HACKS which are placed in the code to allow progress
// to be made in the case of difficult corner cases.  These should NOT be left in the
// code; they are really just markers of things which need to be fixed.
//
// Note that the condition may either be a raw boolean expression or a CHECK result
// returned from a Check routine.
//--------------------------------------------------------------------------------

// Ex usage:
// SIMPLIFYING_ASSUMPTION(SUCCEEDED(hr));
#define SIMPLIFYING_ASSUMPTION(_condition) \
    SIMPLIFYING_ASSUMPTION_MSG(_condition, "")

#ifdef _DEBUG_IMPL

// Ex usage:
// SIMPLIFYING_ASSUMPTION_MSG(SUCCEEDED(hr), "It failed!");
#define SIMPLIFYING_ASSUMPTION_MSG(_condition, _message) \
    ASSERT_CHECK(_condition, _message, "Unhandled special case detected")

// use a formatted string. Ex usage:
// SIMPLIFYING_ASSUMPTION_MSGF(SUCCEEDED(hr), ("Woah it failed! 0x%08x", hr));
#define SIMPLIFYING_ASSUMPTION_MSGF(_condition, args) \
    ASSERT_CHECKF(_condition, "Unhandled special case detected", args)

#else   // !_DEBUG_IMPL

#define SIMPLIFYING_ASSUMPTION_MSG(_condition, _message)    do { } while (0)
#define SIMPLIFYING_ASSUMPTION_MSGF(_condition, args)       do { } while (0)

#endif  // !_DEBUG_IMPL

//--------------------------------------------------------------------------------
// COMPILER_ASSUME_MSG is a statement that tells the compiler to assume the 
// condition is true.  In a checked build these turn into asserts; 
// in a free build they are passed through to the compiler to use in optimization.
//--------------------------------------------------------------------------------


#if defined(DACCESS_COMPILE)
#define COMPILER_ASSUME_MSG(_condition, _message) do { } while (0)
#define COMPILER_ASSUME_MSGF(_condition, args)    do { } while (0)
#else

#if defined(_DEBUG)
#define COMPILER_ASSUME_MSG(_condition, _message) \
    ASSERT_CHECK(_condition, _message, "Compiler optimization assumption invalid")
#define COMPILER_ASSUME_MSGF(_condition, args) \
    ASSERT_CHECKF(_condition, "Compiler optimization assumption invalid", args)
#else
#define COMPILER_ASSUME_MSG(_condition, _message) __assume(_condition)
#define COMPILER_ASSUME_MSGF(_condition, args) __assume(_condition)
#endif // _DEBUG

#endif // DACCESS_COMPILE



#define COMPILER_ASSUME(_condition) \
    COMPILER_ASSUME_MSG(_condition, "")


//--------------------------------------------------------------------------------
// PREFIX_ASSUME_MSG is another name for COMPILER_ASSUME_MSG
// In a checked build these turn into asserts; in a free build
// they are passed through to the compiler to use in optimization;
//  via an __assume(_condition) optimization hint.
//--------------------------------------------------------------------------------

#define PREFIX_ASSUME_MSG(_condition, _message) \
    COMPILER_ASSUME_MSG(_condition, _message)

#define PREFIX_ASSUME_MSGF(_condition, args) \
    COMPILER_ASSUME_MSGF(_condition, args)

#define PREFIX_ASSUME(_condition) \
    COMPILER_ASSUME_MSG(_condition, "")

//--------------------------------------------------------------------------------
// UNREACHABLE points are locations in the code which should not be able to be 
// reached under any circumstances (e.g. a default in a switch which is supposed to 
// cover all cases.).  This macro tells the compiler this, and also embeds a check
// to make sure it is always true.
//--------------------------------------------------------------------------------

#define UNREACHABLE() \
    UNREACHABLE_MSG("")

// Bug PREFast_:510  v4.51 does not support __assume(0)
#if (defined(_MSC_VER) && !defined(_PREFAST_)) || defined(_PREFIX_)
# define __UNREACHABLE() __assume(0)
#else
#define __UNREACHABLE()  do { } while(true)
#endif

#ifdef _DEBUG_IMPL

#define UNREACHABLE_MSG(_message)                                               \
{                                                                               \
    CHECK _check;                                                               \
    _check.Setup(_message, "<unreachable>", __FILE__, __LINE__);                \
    _check.Trigger("Reached the \"unreachable\"");                              \
} __UNREACHABLE()

#else

#define UNREACHABLE_MSG(_message) __UNREACHABLE()

#endif


//
//

#if CHECK_STRESS

#define STRESS_CHECK(_condition, _message) \
    ASSERT_CHECK(_condition, _message, "Stress Assertion Failure")

#else

#define STRESS_CHECK(_condition, _message)  do { } while (0)

#endif

//--------------------------------------------------------------------------------
// COMPILE_TIME_ASSERT represents a check which should be made at compile time.  It
// can only be done on a constant expression.
//
// Note that this assert can even be placed in a declaration!
//--------------------------------------------------------------------------------

#define COMPILE_TIME_ASSERT(_condition) \
    C_ASSERT(_condition)

#define COMPILE_TIME_ASSERT_MSG(_condition, _message) \
    C_ASSERT(_condition)


//--------------------------------------------------------------------------------
// CONTRACT_CHECK is used to put contracts on Check function.  Note that it does
// not support postconditions.
//--------------------------------------------------------------------------------

#define CONTRACT_CHECK      CONTRACTL
#define CONTRACT_CHECK_END  CONTRACTL_END


#ifdef _DEBUG

#define CCHECK_START                                                            \
    {                                                                           \
        BOOL ___exception = FALSE;                                              \
        BOOL ___transient = FALSE;                                              \
        CHECK ___result = CHECK::OK();                                          \
        EX_TRY {

#define CCHECK_END                                                              \
        } EX_CATCH {                                                            \
            if (CHECK::IsInAssert())                                            \
            {                                                                   \
                ___exception = TRUE;                                            \
                ___transient = GET_EXCEPTION()->IsTransient();                  \
            }                                                                   \
            else                                                                \
                EX_RETHROW;                                                     \
        } EX_END_CATCH(RethrowTerminalExceptions);                              \
                                                                                \
        if (___exception)                                                       \
        {                                                                       \
            if (___transient)                                                   \
                CHECK_OK;                                                       \
            else                                                                \
                CHECK_FAIL("Nontransient exception occurred during check");     \
        }                                                                       \
        CHECK(___result);                                                       \
    }

#define CRETURN_RESULT(r) ___result =  r

#define CCHECK_MSG(_condition, _message)                             \
    CHECK_MSG_EX(_condition, _message, CRETURN_RESULT)

#define CCHECK(_condition)                                           \
    CCHECK_MSG(_condition, "")

#define CCHECK_MSGF(_condition, _args)                               \
    CCHECK_MSG(_condition, CHECK::FormatMessage _args)

#define CCHECK_FAIL(_message)                                        \
    CCHECK_MSG(FALSE, _message); UNREACHABLE()

#define CCHECK_FAILF(_args)                                          \
    CCHECK_MSGF(FALSE, _args); UNREACHABLE()

#else // _DEBUG

#define CCHECK_START
#define CCHECK_END

#define CCHECK          CHECK
#define CCHECK_MSG      CHECK_MSG
#define CCHECK_MSGF     CHECK_MSGF
#define CCHECK_FAIL     CHECK_FAIL
#define CCHECK_FAILF    CHECK_FAILF

#endif



//--------------------------------------------------------------------------------
// Common base level checks
//--------------------------------------------------------------------------------

CHECK CheckAlignment(UINT alignment);

CHECK CheckAligned(UINT value, UINT alignment);
#if defined(_MSC_VER)
CHECK CheckAligned(ULONG value, UINT alignment);
#endif
CHECK CheckAligned(UINT64 value, UINT alignment);
CHECK CheckAligned(const void *address, UINT alignment);

CHECK CheckOverflow(UINT value1, UINT value2);
#if defined(_MSC_VER)
CHECK CheckOverflow(ULONG value1, ULONG value2);
#endif
CHECK CheckOverflow(UINT64 value1, UINT64 value2);
CHECK CheckOverflow(const void *address, UINT offset);
#if defined(_MSC_VER)
CHECK CheckOverflow(const void *address, ULONG offset);
#endif
CHECK CheckOverflow(const void *address, UINT64 offset);

CHECK CheckUnderflow(UINT value1, UINT value2);
#if defined(_MSC_VER)
CHECK CheckUnderflow(ULONG value1, ULONG value2);
#endif
CHECK CheckUnderflow(UINT64 value1, UINT64 value2);
CHECK CheckUnderflow(const void *address, UINT offset);
#if defined(_MSC_VER)
CHECK CheckUnderflow(const void *address, ULONG offset);
#endif
CHECK CheckUnderflow(const void *address, UINT64 offset);
CHECK CheckUnderflow(const void *address, void *address2);

CHECK CheckZeroedMemory(const void *memory, SIZE_T size);

// These include overflow checks 
CHECK CheckBounds(const void *rangeBase, UINT32 rangeSize, UINT32 offset);
CHECK CheckBounds(const void *rangeBase, UINT32 rangeSize, UINT32 offset, UINT32 size);


// ================================================================================
// Inline definitions
// ================================================================================

#include "check.inl"

#endif // CHECK_H_
