/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


--*/

#ifndef __PAL_ASSERT_H__
#define __PAL_ASSERT_H__

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
//
// C_ASSERT() can be used to perform many compile-time assertions:
//            type sizes, field offsets, etc.
//
// An assertion failure results in error C2118: negative subscript.
//  or
// size of array `__C_ASSERT__' is negative
//

#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]

//
// CPP_ASSERT() can be used within a class definition, to perform a
// compile-time assertion involving private names within the class.
//
// MS compiler doesn't allow redefinition of the typedef within a template.
// gcc doesn't allow redefinition of the typedef within a class, though 
// it does at file scope.
#define CPP_ASSERT(n, e) typedef char __C_ASSERT__##n[(e) ? 1 : -1];

#endif // __cplusplus

#if defined(_DEBUG)
#define _ASSERTE(e) do {                                        \
        if (!(e)) {                                             \
            fprintf (stderr,                                    \
                     "ASSERT FAILED\n"                          \
                     "\tExpression: %s\n"                       \
                     "\tLocation:   line %d in %s\n"            \
                     "\tFunction:   %s\n"                       \
                     "\tProcess:    %d\n",                      \
                     #e, __LINE__, __FILE__, __FUNCTION__,      \
                     GetCurrentProcessId());                    \
            DebugBreak();                                       \
        }                                                       \
    }while (0)
#else // !DEBUG
#define _ASSERTE(e) ((void)0)
#endif

#ifndef assert
#define assert(e) _ASSERTE(e)
#endif  // assert

#ifdef  __cplusplus
}
#endif

#endif // __PAL_ASSERT_H__
