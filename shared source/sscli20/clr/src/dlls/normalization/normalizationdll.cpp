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

// NormalizationDLL.cpp
//
// WARNING: This .DLL is supposed to be the same in managed and native code,
// starting with the Longhorn & Whidbey releases.  Please make sure they stay in sync.
//
// This file contains the DLL main and the static wrapper methods we use
// for normalization

#include "normalizationprivate.h"

//
// Native and managed forms return different types of BOOL/bool just to be annoying
//
#define Norm_Bool  bool
#define Norm_False false
#define Norm_True  true

//
// DLL Entry Point
//
extern "C"
BOOL WINAPI DllMain( HANDLE hModule,
                     DWORD  ul_reason_for_call,
                     LPVOID lpReserved
                   )
{
    
    // We're boring
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_DETACH:
            CNormalization::CleanUp();
            break;
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE;
}

//
// Wrapper functions that we export
//

// NormalizeString
//
// Returns the length of the normalized string in the provided output buffer.  If the output buffer is NULL
// or the output buffer length is 0, then we return a guess of the string's length.
//
// The return value is the actual length of the normalized string or a guess of the length of the
// string, or if the buffer is overrun, a suggested length to try next time.  So you want to do something like:
//
// iSize = -NormalizeString(Form, strInput, -1, 0, 0);
//
// while ( iSize < 0 )
// {
//      Allocate buffer (strResult);
//      iSize = NormalizeString(form, strInput, -1, strResult, strResult.Length);
// }
//
//  If there's a problem, then iSize will be less than or equal to 0 and the value of GetLastError() will be:
//      ERROR_SUCCESS:                  (no error, really was size 0)
//      ERROR_INSUFFICIENT_BUFFER:          Need a bigger buffer.  Return value is < 0, so try buffer of -(Return Value) size
//      ERROR_NO_UNICODE_TRANSLATION:   Invalid unicode was found in string, return value is -(location of error) in input string
//      ERROR_INVALID_PARAMETER:        Input pointers were bad or normalization form was bad
int WINAPI NormalizeString(
    NORM_FORM                            NormForm,       // Normalization Form to use
    __in LPCWSTR                         lpSrcString,    // Source string
    int                                  cwSrcLength,    // Length of source string or -1 for null termination
    __out_ecount_opt(cwDstLength) LPWSTR lpDstString,    // Destination string buffer or NULL to get count estimate
    int                                  cwDstLength )   // Size of destination buffer or 0 to get count estimate.
{
    // Our input pointers could be OK
    _ASSERTE(lpSrcString != NULL);
    _ASSERTE(cwSrcLength >= -1);
    _ASSERTE(NormForm != 0);

    // Make sure we have an input buffer
    if (lpSrcString == NULL || cwSrcLength < -1 || NormForm == 0)
    {
        // Bad parameters
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    // Get our normalization object for the requested form
    CNormalization* pNormalization = CNormalization::GetNormalization(NormForm);
    if (pNormalization == NULL)
    {
        // Bad installation or not enough memory
        _ASSERTE(!"Bad install or not enough memory");

        // Couldn't load normalization form, must be invalid (or corrupted normnfc.nls files)
        // or it could be that we ran out of memory
        if (CNormalization::IsValidForm(NormForm))
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        else
            SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    // Need someplace to store our answers
    int iCount = 0;
    DWORD error = ERROR_SUCCESS;

    // We'll need our source length eventually anyway...
    if (cwSrcLength == -1)
    {
        // Calculate our source length, including the \0
        cwSrcLength = (int)(wcslen(lpSrcString) + 1);
    }

    if (cwDstLength > 0 && lpDstString != NULL)
    {
        // They want to actually do the conversion
        // lpDstString isn't allowed to be in lpSrcString
        if ((lpDstString < lpSrcString + cwSrcLength) && (lpDstString + cwDstLength > lpSrcString))
        {
            // Bad parameter
            SetLastError(ERROR_INVALID_PARAMETER);
            return 0;
        }

        // Go ahead and convert it.
        error = pNormalization->Normalize(lpSrcString, cwSrcLength, lpDstString, cwDstLength, &iCount);
    }
    else
    {
        // They just want a count of the numbers
        error = pNormalization->GuessCharCount(lpSrcString, cwSrcLength, &iCount);
    }

    // Set the last error to what we had returned
    SetLastError(error);

    // If there's an error return <= 0
    if (error == ERROR_SUCCESS)
    {
        // We were good, return iCount;
        return iCount;
    }

    // We had an error.  For ERROR_INSUFFICIENT_BUFFER iCount is the recommended new buffer size.
    // for ERROR_NO_UNICODE_TRANSLATION iCount is the location of the error.  So return -iCount;
    _ASSERTE(iCount >= 0);
    return -iCount;
} // NormalizeString

// IsNormalizedString
//
// return true if the input string is already normalized to the appropriate form
//
// If false, then SetLastError could be:
//      ERROR_SUCCESS:                   It really was false
//      ERROR_NO_UNICODE_TRANSLATION:    Invalid unicode was found in string
//      ERROR_INVALID_PARAMETER:         Input pointers were bad or bad normalization form
//
// We define ERROR_FALSE to be 0xffff so that IsNormalized can return false as well as true
Norm_Bool WINAPI IsNormalizedString(
    NORM_FORM    NormForm,           // Form to check if its normalized to
    __in LPCWSTR lpString,           // String to test
    int          cwLength )          // Length of input string, -1 if null terminated
{
    // Make sure we have an input buffer
    // Our input pointers could be OK
    _ASSERTE(lpString != NULL);
    _ASSERTE(cwLength >= -1);
    _ASSERTE(NormForm != 0);

    if (lpString == NULL || cwLength < -1 || NormForm == 0 )
    {
        // Bad parameters
        SetLastError(ERROR_INVALID_PARAMETER);
        return Norm_False;
    }

    // Get our normalization
    CNormalization* pNormalization = CNormalization::GetNormalization(NormForm);
    if (pNormalization == NULL)
    {
        // Bad installation or not enough memory
        _ASSERTE(!"Bad install or not enough memory");

        // Couldn't load normalization form, must be invalid (or corrupted normnfc.nls files)
        // or it could be that we ran out of memory
        if (CNormalization::IsValidForm(NormForm))
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        else
            SetLastError(ERROR_INVALID_PARAMETER);
        return Norm_False;
    }

    // We'll need our source length eventually anyway...
    // This is probably little slower for the -1 case, but its faster for known lengths
    if (cwLength == -1)
    {
        // Calculate our source length, including the \0
        cwLength = (int)(wcslen(lpString) + 1);
    }

    DWORD error = pNormalization->IsNormalized(lpString, cwLength);
    if (error != ERROR_FALSE)
    {
        // Most errors are ok
        SetLastError(error);
    }
    else
    {
        // False is a success version of false
        SetLastError(ERROR_SUCCESS);
    }

    // Just return true if succeeded
    if (error == ERROR_SUCCESS)
    {
        return Norm_True;
    }

    // Something happened
    return Norm_False;
}

// Initialize the appropriate normalization form
// (managed only)
extern "C"
CNormalization* WINAPI InitNormalization(NORM_FORM NormForm, Tables* pTables)
{
    return CNormalization::GetNormalization(NormForm, pTables);
}

