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
// ================================================================================
// Standard primitive types for CLR code
//
// This header serves as a platform layer containing all of the primitive types
// which we use across CLR implementation code.
// ================================================================================

#ifndef CLRTYPES_H_
#define CLRTYPES_H_

#include "crtwrap.h"
#include "winwrap.h"
#include "staticcontract.h"

#define POINTER_BITS 32

// ================================================================================
// Integral types - use these for all integral types
// These types are in ALL_CAPS.  Each type has a _MIN and _MAX defined for it.
// ================================================================================

// --------------------------------------------------------------------------------
// Use these types for fixed size integers:
// INT8 UINT8 INT16 UINT16 INT32 UINT32 INT64 UINT64
// --------------------------------------------------------------------------------

typedef signed char           INT8;
typedef unsigned char         UINT8;
typedef short                 INT16;
typedef unsigned short        UINT16;
typedef int                   INT32;
typedef unsigned int          UINT32;
typedef __int64               INT64;
typedef unsigned __int64      UINT64;

#define INT8_MAX              ((INT8)0x7f)
#define INT8_MIN              ((INT8)0x80)

#define UINT8_MAX             ((UINT8)0xffU)
#define UINT8_MIN             ((UINT8)0U)

#define INT16_MAX             ((INT16)0x7fff)
#define INT16_MIN             ((INT16)0x8000)

#define UINT16_MAX            ((UINT16)0xffffU)
#define UINT16_MIN            ((UINT16)0U)

#define INT32_MAX             ((INT32)0x7fffffff)
#define INT32_MIN             ((INT32)0x80000000)

#define UINT32_MAX            ((UINT32)0xffffffffU)
#define UINT32_MIN            ((UINT32)0U)

#define INT64_MAX             ((INT64) I64(0x7fffffffffffffff))
#define INT64_MIN             ((INT64) I64(0x8000000000000000))

#define UINT64_MAX            ((UINT64) UI64(0xffffffffffffffff))
#define UINT64_MIN            ((UINT64) UI64(0))


// --------------------------------------------------------------------------------
// Use these types for pointer-sized integral types
// SIZE_T SSIZE_T
//
// These types are the ONLY types which can be safely cast back and forth from a
// pointer.
// --------------------------------------------------------------------------------


#if POINTER_BITS == 64

#define SIZE_T_MAX              UINT64_MAX
#define SIZE_T_MIN              UINT64_MIN

#define SSIZE_T_MAX             INT64_MAX
#define SSIZE_T_MIN             INT64_MIN

#else

#define SIZE_T_MAX              UINT32_MAX
#define SIZE_T_MIN              UINT32_MIN

#define SSIZE_T_MAX             INT32_MAX
#define SSIZE_T_MIN             INT32_MIN

#endif

// --------------------------------------------------------------------------------
// Non-pointer sized types
// COUNT_T SCOUNT_T
//
// Use these types for "large" counts or indexes which will not exceed 32 bits.  They
// may also be used for pointer differences, if you can guarantee that the pointers
// are pointing to the same region of memory. (It can NOT be used for arbitrary
// pointer subtraction.)
// --------------------------------------------------------------------------------

typedef UINT32                  COUNT_T;
typedef INT32                   SCOUNT_T;

#define COUNT_T_MAX             UINT32_MAX
#define COUNT_T_MIN             UINT32_MIN

#define SCOUNT_T_MAX            INT32_MAX
#define SCOUNT_T_MIN            INT32_MIN

// --------------------------------------------------------------------------------
// Integral types with additional semantic content
// BOOL BYTE
// --------------------------------------------------------------------------------

#if NEED_BOOL_TYPEDEF
typedef bool                    BOOL;
#endif

#define BOOL_MAX                1
#define BOOL_MIN                0

#define TRUE                    1
#define FALSE                   0

typedef UINT8                   BYTE;

#define BYTE_MAX                UINT8_MAX
#define BYTE_MIN                UINT8_MIN

// --------------------------------------------------------------------------------
// Character types
// CHAR SCHAR UCHAR WCHAR
// --------------------------------------------------------------------------------

typedef char                    CHAR;
typedef signed char             SCHAR;
typedef unsigned char           UCHAR;
typedef wchar_t                 WCHAR;

typedef CHAR                    ASCII;
typedef CHAR                    ANSI;
typedef CHAR                    UTF8;

// Standard C defines:

// CHAR_MAX
// CHAR_MIN
// SCHAR_MAX
// SCHAR_MIN
// UCHAR_MAX
// UCHAR_MIN
// WCHAR_MAX
// WCHAR_MIN

#define ASCII_MIN              ((ASCII)0)
#define ASCII_MAX              ((ASCII)127)

#define ANSI_MIN               ((ANSI)0)
#define ANSI_MAX               ((ANSI)255)

#define UTF8_MIN               ((UTF8)0)
#define UTF8_MAX               ((UTF8)255)

// ================================================================================
// Non-integral types
// These types are in ALL_CAPS.
// ================================================================================

// --------------------------------------------------------------------------------
// Floating point types
// FLOAT DOUBLE
// --------------------------------------------------------------------------------

// ================================================================================
// Runtime type definitions - these are guaranteed to be identical with the
// corresponding managed type
// ================================================================================

typedef WCHAR       CLR_CHAR;
typedef INT8        CLR_I1;
typedef UINT8       CLR_U1;
typedef INT16       CLR_I2;
typedef UINT16      CLR_U2;
typedef INT32       CLR_I4;
typedef UINT32      CLR_U4;
typedef INT64       CLR_I8;
typedef UINT64      CLR_U8;
typedef FLOAT       CLR_R4;
typedef DOUBLE      CLR_R8;
typedef SSIZE_T     CLR_I;
typedef SIZE_T      CLR_U;

#define CLR_CHAR_MAX    WCHAR_MAX
#define CLR_CHAR_MIN    WCHAR_MIN

#define CLR_I1_MAX      INT8_MAX
#define CLR_I1_MIN      INT8_MIN

#define CLR_U1_MAX      UINT8_MAX
#define CLR_U1_MIN      UINT8_MIN

#define CLR_I2_MAX      INT16_MAX
#define CLR_I2_MIN      INT16_MIN

#define CLR_U2_MAX      UINT16_MAX
#define CLR_U2_MIN      UINT16_MIN

#define CLR_I4_MAX      INT32_MAX
#define CLR_I4_MIN      INT32_MIN

#define CLR_U4_MAX      UINT32_MAX
#define CLR_U4_MIN      UINT32_MIN

#define CLR_I8_MAX      INT64_MAX
#define CLR_I8_MIN      INT64_MIN

#define CLR_U8_MAX      UINT64_MAX
#define CLR_U8_MIN      UINT64_MIN

#define CLR_I_MAX       SSIZE_T_MAX
#define CLR_I_MIN       SSIZE_T_MIN

#define CLR_U_MAX       SIZE_T_MAX
#define CLR_U_MIN       SIZE_T_MIN

typedef BYTE            CLR_BOOL;
C_ASSERT(sizeof(CLR_BOOL) == 1);

#define CLR_BOOL_MAX    BOOL_MAX
#define CLR_BOOL_MIN    BOOL_MIN

#define CLR_NAN_32 0xFFC00000
#define CLR_NAN_64 I64(0xFFF8000000000000)

// ================================================================================
// Simple utility functions
// ================================================================================

// Note that these routines are in terms of UINT, ULONG, and ULONG64, since those are
// the unsigned integral types the compiler overloads based on.

// --------------------------------------------------------------------------------
// Min/Max
// --------------------------------------------------------------------------------

template <typename T>
T Min(T v1, T v2)
{
    STATIC_CONTRACT_LEAF;
    STATIC_CONTRACT_SO_TOLERANT;
    return v1 < v2 ? v1 : v2;
}

template <typename T>
T Max(T v1, T v2)
{
    STATIC_CONTRACT_LEAF;
    STATIC_CONTRACT_SO_TOLERANT;
    return v1 > v2 ? v1 : v2;
}

// --------------------------------------------------------------------------------
// Alignment bit twiddling macros - "alignment" must be power of 2
// 
// AlignUp - align value to given increment, rounding up
// AlignmentPad - amount adjusted by AlignUp
//       AlignUp(value, x) == value + AlignmentPad(value, x)
//
// AlignDown - align value to given increment, rounding down
// AlignmentTrim - amount adjusted by AlignDown
//       AlignDown(value, x) == value - AlignmentTrim(value, x)
// --------------------------------------------------------------------------------

inline UINT AlignUp(UINT value, UINT alignment)
{
    STATIC_CONTRACT_LEAF;
    STATIC_CONTRACT_SO_TOLERANT;
    return (value+alignment-1)&~(alignment-1);
}

#if defined(_MSC_VER)
inline ULONG AlignUp(ULONG value, UINT alignment)
{
    STATIC_CONTRACT_LEAF;
    STATIC_CONTRACT_SO_TOLERANT;
    return (value+alignment-1)&~(alignment-1);
}
#endif

inline UINT64 AlignUp(UINT64 value, UINT alignment)
{
    STATIC_CONTRACT_LEAF;
    STATIC_CONTRACT_SO_TOLERANT;
    return (value+alignment-1)&~(UINT64)(alignment-1);
}

inline UINT AlignDown(UINT value, UINT alignment)
{
    STATIC_CONTRACT_LEAF;
    STATIC_CONTRACT_SO_TOLERANT;
    return (value&~(alignment-1));
}

#if defined(_MSC_VER)
inline ULONG AlignDown(ULONG value, UINT alignment)
{
    STATIC_CONTRACT_LEAF;
    STATIC_CONTRACT_SO_TOLERANT;
    return (value&~(ULONG)(alignment-1));
}
#endif

inline UINT64 AlignDown(UINT64 value, UINT alignment)
{
    STATIC_CONTRACT_LEAF;
    STATIC_CONTRACT_SO_TOLERANT;
    return (value&~(UINT64)(alignment-1));
}

inline UINT AlignmentPad(UINT value, UINT alignment)
{
    STATIC_CONTRACT_WRAPPER;
    return AlignUp(value, alignment) - value;
}

#if defined(_MSC_VER)
inline UINT AlignmentPad(ULONG value, UINT alignment)
{
    STATIC_CONTRACT_WRAPPER;
    return AlignUp(value, alignment) - value;
}
#endif

inline UINT AlignmentPad(UINT64 value, UINT alignment)
{
    STATIC_CONTRACT_WRAPPER;
    return (UINT) (AlignUp(value, alignment) - value);
}

inline UINT AlignmentTrim(UINT value, UINT alignment)
{
    STATIC_CONTRACT_LEAF;
    STATIC_CONTRACT_SO_TOLERANT;
    return value&(alignment-1);
}

#ifndef PLATFORM_UNIX
inline UINT AlignmentTrim(ULONG value, UINT alignment)
{
    STATIC_CONTRACT_LEAF;
    STATIC_CONTRACT_SO_TOLERANT;
    return value&(alignment-1);
}
#endif

inline UINT AlignmentTrim(UINT64 value, UINT alignment)
{
    STATIC_CONTRACT_LEAF;
    STATIC_CONTRACT_SO_TOLERANT;
    return ((UINT)value)&(alignment-1);
}

#endif  // CLRTYPES_H_
