//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Macros and functions for operating on HRESULTs (error codes)
//
//-------------------------------------------------------------------------------------------------

#pragma once

// Macro to clear existing error information
#define ClearErrorInfo()    SetErrorInfo(0, NULL)

class Compiler;

// Max number of characters in an error string, after replacement
#define CCH_MAX_HRESULT_STRING    1024

//---------------------------------------------------------------------------
// @@ Generating errors to return to others.
//---------------------------------------------------------------------------

//
// Standard errors.
//

// Given an ERRID, record rich error information (IErrorInfo), set it as the
// current error info, and return a fabricated HRESULT.
HRESULT HrMake(ERRID errid);

// Given an ERRID, and optional replacement strings, record rich error
// information (IErrorInfo), set it as the current error info, and return
// a fabricated HRESULT.
HRESULT __cdecl HrMakeRepl(ERRID errid, ...);

// A version of HrMake that takes a va_list
HRESULT HrMakeReplArgs(ERRID errid, va_list ap);

//
// Errors with extra info
//

// Given an ERRID and an HRESULT, record rich error information.
HRESULT HrMakeWithError(ERRID errid, HRESULT hr);

// Given an ERRID, an HRESULT and optional replacement string, record rich
// error information.
HRESULT __cdecl HrMakeReplWithError(ERRID errid, HRESULT hr, ...);

// A version of HrMakeWithError that takes a va_list
HRESULT HrMakeReplArgsWithError(ERRID errid, HRESULT hr, va_list ap);

//
// Errors with GetLastError()
//

// Given an ERRID and the last error, record rich error information.
HRESULT HrMakeWithLast(ERRID errid);

// Given an ERRID, an HRESULT and optional replacement string, record rich
// error information.
HRESULT __cdecl HrMakeReplWithLast(ERRID errid, ...);

// A version of HrMakeWithError that takes a va_list
HRESULT HrMakeReplArgsWithLast(ERRID errid, va_list ap);

//
// Helpers
//

//---------------------------------------------------------------------------
// @@ Getting information about an error returned to you.
//---------------------------------------------------------------------------

// Structure containing information about a particular error
struct HRINFO
{
  HRESULT m_hr;              // The hresult
  BSTR  m_bstrSource;         // Source of the error.
  // Note, when E_OUTOFMEMORY, m_bstrDescription is not a bstr
  BSTR  m_bstrDescription;    // Description string, never NULL
  BSTR  m_bstrHelpFile;       // Path to help file, or NULL
  DWORD m_dwHelpContext;      // Help context ID, or 0
};

// Fill an HRINFO structure with information about a particular HRESULT.
// Call ReleaseHrInfo when you are done.
void GetHrInfo(HRESULT hr, _Out_ HRINFO* phrinfo);

// Free the contents of an HRINFO structure when you are done with it.
// This should be called for every structure returned from GetHrInfo
void ReleaseHrInfo(_In_ HRINFO* phrinfo);

void CopyHrInfo(const HRINFO& source, _Out_ HRINFO* pDest);

// Takes an HRINFO and makes it the current thread's error information.
HRESULT PropagateHrInfo(HRINFO &hrinfo);

HRESULT PropagateHrInfo(HRESULT hr, const WCHAR* description);

//---------------------------------------------------------------------------
// @@ Error formatting
//---------------------------------------------------------------------------

// Show an error in a messagebox.  Only do this if we have an error that
// won't get shown any other way.
//
bool HrShowError(HRESULT hr);
