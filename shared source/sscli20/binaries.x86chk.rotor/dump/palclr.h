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
// File: palclr.h
//
// Various macros and constants that are necessary to make the CLR portable.
// ===========================================================================

#ifndef __PALCLR_H__
#define __PALCLR_H__

#include <windef.h>

#if !defined(_DEBUG_IMPL) && defined(_DEBUG) && !defined(DACCESS_COMPILE)
#define _DEBUG_IMPL 1
#endif

//
// CPP_ASSERT() can be used within a class definition, to perform a
// compile-time assertion involving private names within the class.
//
// MS compiler doesn't allow redefinition of the typedef within a template.
// gcc doesn't allow redefinition of the typedef within a class, though 
// it does at file scope.
#define CPP_ASSERT(n, e) typedef char __C_ASSERT__##n[(e) ? 1 : -1];


//
//
//
// 
//

#if defined(_X86_)
// Finished ports - compile-time errors
#define PORTABILITY_WARNING(message)    NEED_TO_PORT_THIS_ONE(NEED_TO_PORT_THIS_ONE)
#define PORTABILITY_ASSERT(message)     NEED_TO_PORT_THIS_ONE(NEED_TO_PORT_THIS_ONE)
#else
// Ports in progress - run-time asserts only
#define PORTABILITY_WARNING(message)
#define PORTABILITY_ASSERT(message)     _ASSERTE(false && message)
#endif


// PAL Macros
// Not all compilers support fully anonymous aggregate types, so the
// PAL provides names for those types. To allow existing definitions of
// those types to continue to work, we provide macros that should be
// used to reference fields within those types.

#ifndef DECIMAL_SCALE
#define DECIMAL_SCALE(dec)       ((dec).u.u.scale)
#endif

#ifndef DECIMAL_SIGN
#define DECIMAL_SIGN(dec)        ((dec).u.u.sign)
#endif

#ifndef DECIMAL_SIGNSCALE
#define DECIMAL_SIGNSCALE(dec)   ((dec).u.signscale)
#endif

#ifndef DECIMAL_LO32
#define DECIMAL_LO32(dec)        ((dec).v.v.Lo32)
#endif

#ifndef DECIMAL_MID32
#define DECIMAL_MID32(dec)       ((dec).v.v.Mid32)
#endif

#ifndef DECIMAL_HI32
#define DECIMAL_HI32(dec)       ((dec).Hi32)
#endif

#ifndef DECIMAL_LO64_GET
#define DECIMAL_LO64_GET(dec)       ((ULONGLONG)(((ULONGLONG)DECIMAL_MID32(dec) << 32) | DECIMAL_LO32(dec)))
#endif

#ifndef DECIMAL_LO64_SET
#define DECIMAL_LO64_SET(dec,value)   {ULONGLONG _Value = value; DECIMAL_MID32(dec) = (ULONG)(_Value >> 32); DECIMAL_LO32(dec) = (ULONG)_Value; }
#endif

#ifndef IMAGE_RELOC_FIELD
#define IMAGE_RELOC_FIELD(img, f)      ((img).u.f)
#endif

#ifndef IMAGE_IMPORT_DESC_FIELD
#define IMAGE_IMPORT_DESC_FIELD(img, f)     ((img).u.f)
#endif

#ifndef IMAGE_RDE_ID
#define IMAGE_RDE_ID(img)        ((img)->u.Id)
#endif

#ifndef IMAGE_RDE_NAME
#define IMAGE_RDE_NAME(img)      ((img)->u.Name)
#endif

#ifndef IMAGE_RDE_OFFSET
#define IMAGE_RDE_OFFSET(img)    ((img)->v.OffsetToData)
#endif

#ifndef IMAGE_RDE_NAME_FIELD
#define IMAGE_RDE_NAME_FIELD(img, f)    ((img)->u.u.f)
#endif

#ifndef IMAGE_RDE_OFFSET_FIELD
#define IMAGE_RDE_OFFSET_FIELD(img, f)  ((img)->v.v.f)
#endif

#ifndef IMAGE_FE64_FIELD
#define IMAGE_FE64_FIELD(img, f)    ((img).u.f)
#endif

#ifndef IMPORT_OBJ_HEADER_FIELD
#define IMPORT_OBJ_HEADER_FIELD(obj, f)    ((obj).u.f)
#endif

#ifndef IMAGE_COR20_HEADER_FIELD
#define IMAGE_COR20_HEADER_FIELD(obj, f)    ((obj).u.f)
#endif


// PAL Numbers
// Used to ensure cross-compiler compatibility when declaring large
// integer constants. 64-bit integer constants should be wrapped in the
// declarations listed here.
//
// Each of the #defines here is wrapped to avoid conflicts with rotor_pal.h.

#if defined(_MSC_VER)

// MSVC's way of declaring large integer constants
// If you define these in one step, without the _HELPER macros, you
// get extra whitespace when composing these with other concatenating macros.
#ifndef I64
#define I64_HELPER(x) x ## i64
#define I64(x)        I64_HELPER(x)
#endif

#ifndef UI64
#define UI64_HELPER(x) x ## ui64
#define UI64(x)        UI64_HELPER(x)
#endif

#else

// GCC's way of declaring large integer constants
// If you define these in one step, without the _HELPER macros, you
// get extra whitespace when composing these with other concatenating macros.
#ifndef I64
#define I64_HELPER(x) x ## LL
#define I64(x)        I64_HELPER(x)
#endif

#ifndef UI64
#define UI64_HELPER(x) x ## ULL
#define UI64(x)        UI64_HELPER(x)
#endif

#endif


// PAL SEH
// Macros for portable exception handling. The Win32 SEH is emulated using
// these macros and setjmp/longjmp on Unix
//
// Usage notes:
//
// - The filter has to be a function taking two parameters:
// LONG MyFilter(PEXCEPTION_POINTERS *pExceptionInfo, PVOID pv)
//
// - It is not possible to directly use the local variables in the filter.
// All the local information that the filter has to need to know about should
// be passed through pv parameter
//  
// - Do not use goto to jump out of the PAL_TRY block
// (jumping out of the try block is not a good idea even on Win32, because of
// it causes stack unwind)
//
//
// Simple examples:
//
// PAL_TRY {
//   ....
// } PAL_FINALLY {
//   ....
// }
// PAL_ENDTRY
//
//
// PAL_TRY {
//   ....
// } PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER) {
//   ....
// }
// PAL_ENDTRY
// 
//
// LONG MyFilter(PEXCEPTION_POINTERS *pExceptionInfo, PVOID pv)
// {
// ...
// }
// PAL_TRY {
//   ....
// } PAL_EXCEPT_FILTER(MyFilter, NULL) {
//   ....
// }
// PAL_ENDTRY
//
//
// Complex example:
//
// struct MyParams
// {
//     ...
// } params;
//
// PAL_TRY {
//   PAL_TRY {
//       ...
//       if (error) goto Done;
//       ...
//   Done: ;
//   } PAL_EXCEPT_FILTER(OtherFilter, &params) {
//   ...
//   }
//   PAL_ENDTRY
// }
// PAL_FINALLY {
// }
// PAL_ENDTRY
//

// This is intentionally enabled for !PORTABLE_SEH && FEATURE_PAL
// to let the compiler verify that the definitions of these macros in rotor_pal.h
// are identical


#define PAL_TRY_HANDLER_DBG_BEGIN                   ANNOTATION_TRY_BEGIN;
#define PAL_TRY_HANDLER_DBG_BEGIN_DLLMAIN(_reason)  ANNOTATION_TRY_BEGIN;
#define PAL_TRY_HANDLER_DBG_END                     ANNOTATION_TRY_END;
#define PAL_ENDTRY_NAKED_DBG                                                          


#if !BIGENDIAN
// For little-endian machines, do nothing
#define VAL16(x) x
#define VAL32(x) x
#define VAL64(x) x
#define SwapString(x)
#define SwapStringLength(x, y)
#define SwapGuid(x)
#endif  // !BIGENDIAN

#ifdef _MSC_VER
// Get Unaligned values from a potentially unaligned object
#define GET_UNALIGNED_16(_pObject)  (*(UINT16 UNALIGNED *)(_pObject))
#define GET_UNALIGNED_32(_pObject)  (*(UINT32 UNALIGNED *)(_pObject))
#define GET_UNALIGNED_64(_pObject)  (*(UINT64 UNALIGNED *)(_pObject))

// Set Value on an potentially unaligned object 
#define SET_UNALIGNED_16(_pObject, _Value)  (*(UNALIGNED UINT16 *)(_pObject)) = (UINT16)(_Value)
#define SET_UNALIGNED_32(_pObject, _Value)  (*(UNALIGNED UINT32 *)(_pObject)) = (UINT32)(_Value)
#define SET_UNALIGNED_64(_pObject, _Value)  (*(UNALIGNED UINT64 *)(_pObject)) = (UINT64)(_Value) 

// Get Unaligned values from a potentially unaligned object and swap the value
#define GET_UNALIGNED_VAL16(_pObject) VAL16(GET_UNALIGNED_16(_pObject))
#define GET_UNALIGNED_VAL32(_pObject) VAL32(GET_UNALIGNED_32(_pObject))
#define GET_UNALIGNED_VAL64(_pObject) VAL64(GET_UNALIGNED_64(_pObject))

// Set a swap Value on an potentially unaligned object 
#define SET_UNALIGNED_VAL16(_pObject, _Value) SET_UNALIGNED_16(_pObject, VAL16((UINT16)_Value))
#define SET_UNALIGNED_VAL32(_pObject, _Value) SET_UNALIGNED_32(_pObject, VAL32((UINT32)_Value))
#define SET_UNALIGNED_VAL64(_pObject, _Value) SET_UNALIGNED_64(_pObject, VAL64((UINT64)_Value))
#endif

#define VALPTR(x) VAL32(x)
#define GET_UNALIGNED_PTR(x) GET_UNALIGNED_32(x)
#define GET_UNALIGNED_VALPTR(x) GET_UNALIGNED_VAL32(x)
#define SET_UNALIGNED_PTR(p,x) SET_UNALIGNED_32(p,x)
#define SET_UNALIGNED_VALPTR(p,x) SET_UNALIGNED_VAL32(p,x)

#ifndef PLATFORM_UNIX
#define MAKEDLLNAME_W(name) name L".dll"
#define MAKEDLLNAME_A(name) name  ".dll"
#endif

#ifdef UNICODE
#define MAKEDLLNAME(x) MAKEDLLNAME_W(x)
#else
#define MAKEDLLNAME(x) MAKEDLLNAME_A(x)
#endif


#endif	// __PALCLR_H__
