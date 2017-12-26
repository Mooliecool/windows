#pragma once

/*
    So as to get better assert messages for parameter validation, we are redefining
    the Verify* macros.  Each will now take a description argument that is passed to
    VSFAIL.
*/

//---------------------------------------------------------------------
// Some macros for COM Parameter validation
//---------------------------------------------------------------------
#undef VerifyParam
#define VerifyParam(param, result, description) \
    { \
        if ((param) == NULL) \
        { \
            VSFAIL(description); \
            RRETURN(result); \
        } \
    }

//---------------------------------------------------------------------
// Assert the pointer is not NULL. Return with E_POINTER if not.
//---------------------------------------------------------------------
#undef VerifyOutPtr
#define VerifyOutPtr(param, description) VerifyParam((param), E_POINTER, description);

//---------------------------------------------------------------------
// Assert the pointer is not NULL. Return with E_INVALIDARG if not.
//---------------------------------------------------------------------
#undef VerifyInPtr
#define VerifyInPtr(param, description) VerifyParam((param), E_INVALIDARG, description);

//==============================================================================
// Used to validate parameters where the return is an HRESULT and the
// conditional is not just NULL
// RRETURN assumes result is an HRESULT.
//==============================================================================
#undef VerifyParamCond
#define VerifyParamCond(paramCond, result, description ) \
    {                                       \
        if ( !(paramCond) )                 \
        {                                   \
            VSFAIL(description);    \
            RRETURN(result);                \
        }                                   \
    }

//==============================================================================
// Used to validate parameters where the return is NULL and the
// conditional is cannot be compared to NULL (gcroot<>). Named because the 
// only case needed so far is to return NULL.
//==============================================================================
#define VerifyParamCondRetNull(paramCond, description)       \
    {                                       \
        if ( !(paramCond) )                 \
        {                                   \
            VSFAIL(description);            \
            return NULL;                    \
        }                                   \
    }

//==============================================================================
// The following correspond to the above VerifyInPtr, VerifyOutPtr, but
// handle that a gcroot<> can be verified casting it, rather than by
// comparing it to null. Simpler than overloading duplicating VerifyParamCond
// to compare against nullptr.
//==============================================================================
#define VerifyInCLRPtr(x, description) VerifyParamCond(x, E_INVALIDARG, description)

//---------------------------------------------------------------------
// Assert the pointer is not NULL. Return with NULL if not.
//---------------------------------------------------------------------
#define VerifyInPtrRetNull(param, description) VerifyParamCondRetNull(param, description)
#define VerifyOutPtrRetNull(param, description) VerifyParamCondRetNull(param, description)
