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
//*****************************************************************************
// File: stdafx.h
//
//*****************************************************************************
#include <stdio.h>

#define USE_COM_CONTEXT_DEF

#include <windows.h>

#include <winwrap.h>

#include <cor.h>
#include <dacprivate.h>

#include <common.h>
#include <codeman.h>
#include <debugger.h>
#include <eedbginterfaceimpl.h>
#include <methoditer.h>

#include "dacimpl.h"

#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>
#undef _ftcscat
#undef _ftcscpy

#define __uuidof(x)         IID_ ## x

// XXX drewb - Figure out exception mess.  The standard
// ex.h has the macros we want, but then common.h includes
// clrex.h which redefines them with extra EE stuff we don't
// want.

#undef EX_THROW
#undef EX_TRY
#undef EX_TRY_CUSTOM
#undef EX_TRY_FOR_FINALLY_CUSTOM
#undef EX_CATCH
#undef EX_END_CATCH
#undef EX_END_CATCH_UNREACHABLE
#undef EX_RETHROW
#undef EX_FINALLY
#undef EX_END_FINALLY
#undef GET_EXCEPTION
#undef RethrowTerminalExceptions
#define RethrowTerminalExceptions                                       \
    if (GET_EXCEPTION()->IsTerminal())                                  \
    {                                                                   \
        EX_RETHROW;                                                     \
    }                                                                   \

#define RethrowTransientExceptions                                      \
    if (GET_EXCEPTION()->IsTransient())                                 \
    {                                                                   \
        EX_RETHROW;                                                     \
    }                                                                   \

#define SwallowAllExceptions ;


#define EX_THROW(_type, _args)                                                          \
    {                                                                                   \
        _type * ___pExForExThrow =  new _type _args ;                                   \
        PAL_CPP_THROW(_type, ___pExForExThrow);                         \
    }

#define EX_TRY   EX_TRY_CUSTOM(Exception::HandlerState, SEHException)

#define EX_TRY_CUSTOM(STATETYPE, DEFAULT_EXCEPTION_TYPE)                                \
    {                                                                                   \
        Exception*              __pExceptionPtr = NULL;                                 \
        STATETYPE               __state;                                                \
        DEFAULT_EXCEPTION_TYPE  __defaultException;                                     \
        bool                    __fCaughtCxx = false;                                   \
        PAL_CPP_TRY                                                     \
        {                                                                               \
            PAL_CPP_TRY                                                 \
            {                                                                           \
                CAutoTryCleanup<STATETYPE> __autoCleanupTry(__state);


#define EX_CATCH                                                                        \
                ;                                                                       \
            }                                                                           \
            PAL_CPP_CATCH_EXCEPTION (__pExceptionRaw)                                   \
            {                                                                           \
                __fCaughtCxx = true;                                                    \
                __pExceptionPtr = __pExceptionRaw;                                         \
                PAL_CPP_RETHROW;                                                        \
            }                                                                           \
            PAL_CPP_ENDTRY                                                              \
        }                                                                               \
        PAL_CPP_CATCH_ALL                                                               \
        {                                                                               \
            ExceptionHolder __pException(__pExceptionPtr);                           \
            /* work around unreachable code warning */                                  \
            if (true) { DEBUG_ASSURE_NO_RETURN_BEGIN                                    \
            __state.SetupCatch(__fCaughtCxx);


#define EX_END_CATCH_UNREACHABLE                                                        \
            DEBUG_ASSURE_NO_RETURN_END                                                  \
        } UNREACHABLE(); }                                                              \
        PAL_CPP_ENDTRY                                                                  \
    }

#define EX_END_CATCH(terminalexceptionpolicy)                                           \
            terminalexceptionpolicy;                                                    \
            __state.SucceedCatch(__fCaughtCxx);                                         \
            DEBUG_ASSURE_NO_RETURN_END                                                  \
        } }                                                                             \
        PAL_CPP_ENDTRY                                                                  \
    }

#define EX_TRY_FOR_FINALLY_CUSTOM(STATETYPE)                                            \
    {                                                                                   \
        STATETYPE __state;                                                              \
        PAL_TRY                                                                         \
        {                                                                               \
            /* this is necessary for Rotor exception handling to work */                \
            DEBUG_ASSURE_NO_RETURN_BEGIN                                                \

#define EX_FINALLY                                                                      \
            DEBUG_ASSURE_NO_RETURN_END                                                  \
        }                                                                               \
        PAL_FINALLY                                                                     \
        {                                                                               \
            __state.CleanupTry();

#define EX_END_FINALLY                                                                  \
        PAL_ENDTRY                                                                      \
    }


#define EX_RETHROW                                                                      \
        {                                                                               \
            __pException.SuppressRelease();                                             \
            PAL_CPP_RETHROW;                                            \
        }

#define GET_EXCEPTION() (__pException.IsNull() ? &__defaultException : __pException.GetValue())


