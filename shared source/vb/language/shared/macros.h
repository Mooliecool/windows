//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Useful utility macros.
//
//-------------------------------------------------------------------------------------------------

#pragma once

#if DEBUG
#define VbThrow(hr)            {VSASSERT(false,"VbThrow: About to RaiseException");    \
                                            RaiseException(hr, 0, 0, NULL);}

#define VbThrowNoAssert(hr)                 RaiseException(hr, 0, 0, NULL)

#else
#define VbThrow(hr)                         RaiseException(hr, 0, 0, NULL)
#define VbThrowNoAssert(hr)                 RaiseException(hr, 0, 0, NULL)
#endif //DEBUG

// @@ Misc macros
// The following macros are C++ helpers that may be useful:
// - DIM(ARRAY) - returns the number of elements in a fixed length array
// - WIDE(CH) - returns a wide character (prepends L)
// - WIDE(STR) - returns a wide string (prepends L)

// Evaluates to the number of elements in the given array.
#define DIM(X) (sizeof(X) / (sizeof((X)[0])))

// ArrayIndex can be used to index into any D-2 array which was allocated as a non-array.
#define ArrayIndex(Array, iSize, iRow, jColumn) (*((Array) + ((((iRow) * (iSize)) + (jColumn)))))

// Defines a wide string constant
#define WIDE(X)   L##X

#define WIDEN(x) WIDE(x)
#define __WFILE__ WIDEN(__FILE__)

// @@ COM helper macros
// Use the following macros to help you manage the lifetime of COM objects:
// - ADDREF(punk)  - only calls punk->AddRef if punk is not NULL
// - RELEASE(punk) - only calls punk->Release if punk not NULL, sets punk to NULL

// This macro releases an object (previously constructed) with its
// class's Release method and resets the object pointer to null.
#define RELEASE(pobject)                      \
    if ((pobject) != NULL) {                  \
      (pobject)->Release();                      \
      pobject = NULL;                          \
    }

// This macros addref's an object if it is not NULL.
#define ADDREF(PUNK) \
  {if (PUNK) (PUNK)->AddRef();}

// Error handling macros
//
// The following macros should be used to handle return values that are HRESULTS:
// - IfFailGo(EXPR) - hr = EXPR, goto Error if FAILED(hr)
// - IfFalseGo(EXPR, HR) - if EXPR is 0, hr = HR and goto Error
// - IfNullGo(EXPR) - if EXPR is NULL, hr = E_OUTOFMEMORY and goto Error
//
// Example usage:
//   HRESULT Foo()
//   {
//      HRESULT hr = NOERROR;
//
//      // Handle HRESULT return value
//      IfFailGo(LoadTypeLib(...));
//
//        // Handle BOOL return value
//      IfFalseGo(RegisterClass(), E_OUTOFMEMORY);
//
//        // Handle NULL return value (presumed out of memory)
//      IfNullGo(p = GlobalAlloc(1));
//
//   Error:
//      RRETURN(hr);
//   }
//
// There are additional variations on the macros which let you return immediately
// from the function (IfFailRet, IfFalseRet, IfNullRet), but this
// is not recommended since it is desireable to have a single exist point
// (for error cleanup).
//
// There are other variations on the macros that let you goto a different label
// than 'Error', although this is seldomly used.  Having more than one label is
// not desired.
//
// Resource cleanup in error handling
//
// The following is an example of how to do resource cleanup in a function that
// uses these macros.  Do not define multiple labels (e.g. Error1, Error2, ...),
// since this becomes unweildy as the function complexity grows.
//
//   HRESULT Foo()
//   {
//      HRESULT hr = NOERROR;
//      char *pch1 = NULL;
//      char *pch2 = NULL;
//
//      // Allocate first buffer
//        IfNullGo(pch1 = new char[1]);
//
//      // Allocate first buffer
//        IfNullGo(pch1 = new char[1]);
//
//      // Call a function
//      xFoo();
//
//   Error:
//      if (pch1)
//              delete pch1;
//      if (pch2)
//              delete pch2;
//      RRETURN(hr);
//   }
//
// How to return errors
//
// HRESULTs should be returned from all routines that do not use exception
// handling.  Do not return BOOL or NULL to signify errors.  HRESULTs can
// come either from an external component, or from this codebase.  See the
// section on how to define an HRESULT that is defined by this codebase.
//

// Helper functions that print out errors if stuff fails
#if DEBUG
extern HRESULT DebTraceReturn(HRESULT hr, _In_z_ const WCHAR *szFile, int iLine);
#define RRETURN(hr) return DebTraceReturn(hr, __WFILE__, __LINE__)
#else
#define RRETURN(hr) return (hr)
#endif  //DEBUG


// FAILEDHR : Same as FAILED(hr), but calls DebTraceReturn if the test failed.
#if DEBUG
#define FAILEDHR(HR) _FAILEDHR(HR, __WFILE__, __LINE__)
inline bool _FAILEDHR(HRESULT hr, _In_z_ const WCHAR* pszFile, int iLine)
  {
  if (FAILED(hr) && hr != E_PENDING)
    DebTraceReturn(hr, pszFile, iLine);
  return FAILED(hr);
  }
#else
#define FAILEDHR(HR) FAILED(HR)
#endif

// SUCCEEDEDHR : Same as SUCCEEDED(hr), but calls DebTraceReturn if the test failed.
#define SUCCEEDEDHR(HR) (!FAILEDHR(HR))

// Print a debug message if FAILED(hr).
#if DEBUG
#define CHECKHR(HR) _CHECKHR(HR, __WFILE__, __LINE__)
inline void _CHECKHR(HRESULT hr, _In_z_ const WCHAR* pszFile, int iLine)
  {
  if (FAILED(hr))
    DebTraceReturn(hr, pszFile, iLine);
  }
#else
#define CHECKHR(HR) HR
#endif

// HRESULT value error handling

#define IfFailedLeave(EXPR) \
{ hr = (EXPR); if (FAILEDHR(hr)) {__leave;}}

#define IfFailGoto(EXPR, LABEL) \
{ hr = (EXPR); if(FAILEDHR(hr)) { goto LABEL;} }

#define IfFailRet(EXPR) \
    { hr = (EXPR); if(FAILED(hr)) RRETURN(hr); }

#define IfFailGo(EXPR) IfFailGoto(EXPR, Error)

#define IfFailThrow(EXPR) { HRESULT __hr = (EXPR); if(FAILEDHR(__hr)) { VbThrow(__hr);} }

#define IfFailContinue(EXPR) { if (FAILEDHR(EXPR)) { continue; } }
#define IfFailBreak(EXPR) { if (FAILEDHR(EXPR)) { break; } }

// Boolean value error handling

#define IfFalseGoto(EXPR, HR, LABEL) \
    { if(!(EXPR)) { hr = (HR); goto LABEL; } }

#define IfFalseRet(EXPR, HR) \
    { if(!(EXPR)) RRETURN(HR); }

#define IfFalseRetLast(EXPR) IfFalseRet(EXPR, GetLastHResultError())

#define IfFalseGo(EXPR, HR) IfFalseGoto(EXPR, HR, Error)

#define IfFalseGoLast(EXPR) IfFalseGo(EXPR, GetLastHResultError())

#define IfFalseThrow(EXPR) { if (!(EXPR)) { VbThrow(GetLastHResultError()); } }

#define IfTrueGo(EXPR, HR) IfFalseGo(!(EXPR), HR)

#define IfTrueRet(EXPR, HR) { if(EXPR) RRETURN(HR); }

#define IfFalseAssertRet(EXPR, HR, STR) \
    { if(!(EXPR)) { VSFAIL(STR); RRETURN(HR); } }

#define IfTrueAssertRet(EXPR, HR, STR) IfFalseAssertRet(!(EXPR), HR, STR)

#define IfFailAssertRet(EXPR, STR) \
    { hr = (EXPR); if(FAILEDHR(hr)) { VSFAIL(STR); RRETURN(hr); } }

#define IfTrueGoLast(EXPR) IfFalseGoLast(!(EXPR))

#define IfTrueThrow(EXPR) { if ((EXPR)) { VbThrow(GetLastHResultError()); } }

// Used only in the core compilation routines.
#define IfTrueAbort(EXPR)  { if (fAborted = EXPR) goto Abort; }

// NULL value error handling

#define IfNullGo(EXPR) IfFalseGoto(EXPR, GetLastHResultError(), Error)

#define IfNullGoto(EXPR, LABEL) IfFalseGoto(EXPR, GetLastHResultError(), LABEL)

#define IfNullRet(EXPR) IfFalseRet(EXPR, GetLastHResultError())

#define IfNullThrow(EXPR) { if (!(EXPR)) { VbThrow(GetLastHResultError()); } }

#define IfNullLeave(EXPR) { if (!(EXPR)) { hr = GetLastHResultError(); __leave; } }

//---------------------------------------------------------------------
// Constants for OA VT_BOOL type & COM Boolean type
//---------------------------------------------------------------------
#define BASIC_TRUE    (-1)
#define BASIC_FALSE   0

#define COMPLUS_FALSE 0
#define COMPLUS_TRUE  1

//---------------------------------------------------------------------
// String id; hungarian = strid
//---------------------------------------------------------------------
typedef UINT STRID;

//---------------------------------------------------------------------
// Some macros for COM Parameter validation
//---------------------------------------------------------------------
#define VerifyParam(param, result) \
    { \
        if ((param) == NULL) \
        { \
            VSFAIL("Invalid NULL Pointer"); \
            RRETURN(result); \
        } \
    }

//---------------------------------------------------------------------
// Assert the pointer is not NULL. Return with E_POINTER if not.
//---------------------------------------------------------------------
#define VerifyOutPtr(param) VerifyParam((param), E_POINTER);

//---------------------------------------------------------------------
// Assert the pointer is not NULL. Return with E_INVALIDARG if not.
//---------------------------------------------------------------------
#define VerifyInPtr(param) VerifyParam((param), E_INVALIDARG);

//---------------------------------------------------------------------
// Call the IsNotZombie function and return E_FAIL if zombied.
// May return E_PENDING if the user cancelled a WaitForState
//---------------------------------------------------------------------
#define VerifyNotZombie() { HRESULT __hr = IsNotZombie(); if(FAILED(__hr)) RRETURN(__hr); }

//==============================================================================
// Used to validate parameters where the return is not an HRESULT and the
// conditional is not just NULL
//==============================================================================
#define VerifyParamCond(paramCond, result ) \
    {                                       \
        if ( !(paramCond) )                 \
        {                                   \
            VSFAIL("Invalid Parameter");    \
            RRETURN(result);                \
        }                                   \
    }


//---------------------------------------------------------------------
// Simplify Assertions
//---------------------------------------------------------------------

#define AssertIfNull(param) VSASSERT((param) != NULL, "Invalid NULL Pointer");

#define AssertIfTrue(param) VSASSERT(!(param), "Condition shouldn't be true.");

#define AssertIfFalse(param) VSASSERT((param), "Condition shouldn't be false");

#define BoolToInt(x) ((x) ? 1 : 0)

#define CompareValues(x, y)  ( ((x) > (y)) ? 1 : (((x) < (y)) ?  -1 : 0) )

#define VPASSERT(cond, message) VSASSERT(!DebVPAssert() || (cond), message);
