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
// PostErrors.cpp
//
// This module contains the error handling/posting code for the engine.  It
// is assumed that all methods may be called by a dispatch client, and therefore
// errors are always posted using IErrorInfo.
//
//*****************************************************************************
#include "stdafx.h"                     // Standard header.
#include <utilcode.h>                   // Utility helpers.
#include <corerror.h>
#include "../dlls/mscorrc/resource.h"
#include "ex.h"

#include <posterror.h>

#if !defined(lengthof)
#define lengthof(x) (sizeof(x)/sizeof(x[0]))
#endif

// Local prototypes.
HRESULT FillErrorInfo(LPCWSTR szMsg, DWORD dwHelpContext);

//For FEATURE_PAL build, resource strings are stored in satellite files and 
//use the PAL_ methods to load the values.  These methods use the HSATELLITE 
//type rather then HINSTANCE.  Thus, all references to HINSTANCE below 
//should be HSATELLITE for the FEATURE_PAL case.
#define HINSTANCE HSATELLITE 

//*****************************************************************************
// Function that we'll expose to the outside world to fire off the shutdown method
//*****************************************************************************

void GetResourceCultureCallbacks(
        FPGETTHREADUICULTURENAMES* fpGetThreadUICultureNames,
        FPGETTHREADUICULTUREID* fpGetThreadUICultureId)
{
    WRAPPER_CONTRACT;
    CCompRC::GetDefaultCallbacks(
        fpGetThreadUICultureNames, 
        fpGetThreadUICultureId
    );
}
//*****************************************************************************
// Set callbacks to get culture info
//*****************************************************************************
void SetResourceCultureCallbacks(
    FPGETTHREADUICULTURENAMES fpGetThreadUICultureNames,    
    FPGETTHREADUICULTUREID fpGetThreadUICultureId
)
{
    WRAPPER_CONTRACT;
    CCompRC::SetDefaultCallbacks(
        fpGetThreadUICultureNames,
        fpGetThreadUICultureId
    );

}

//*****************************************************************************
// Public function to load a resource string
//*****************************************************************************
STDAPI UtilLoadStringRC(
    UINT iResourceID, 
    __out_ecount(iMax) LPWSTR szBuffer, 
    int iMax, 
    int bQuiet
)
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    HRESULT retVal = E_OUTOFMEMORY;

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(retVal = COR_E_STACKOVERFLOW);

    EX_TRY
    {
        CCompRC *pResourceDLL = CCompRC::GetDefaultResourceDll();

        if (pResourceDLL != NULL)
        {
            retVal = pResourceDLL->LoadString(iResourceID, szBuffer, iMax, bQuiet);
        }
    }
    EX_CATCH
    {
        // Catch any errors and return E_OUTOFMEMORY;
        retVal = E_OUTOFMEMORY;
    }
    EX_END_CATCH(SwallowAllExceptions);

    END_SO_INTOLERANT_CODE;

    return retVal;
}

STDAPI UtilLoadStringRCEx(
    LCID lcid,
    UINT iResourceID, 
    __out_ecount(iMax) LPWSTR szBuffer, 
    int iMax, 
    int bQuiet,
    int *pcwchUsed
)
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;
        
    HRESULT retVal = E_OUTOFMEMORY;

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    EX_TRY
    {
        CCompRC *pResourceDLL = CCompRC::GetDefaultResourceDll();

        if (pResourceDLL != NULL)
        {
            retVal =  pResourceDLL->LoadString(lcid, iResourceID, szBuffer, iMax, bQuiet, pcwchUsed);
        }
    }
    EX_CATCH
    {
        // Catch any errors and return E_OUTOFMEMORY;
        retVal = E_OUTOFMEMORY;
    }
    EX_END_CATCH(SwallowAllExceptions);
    END_SO_INTOLERANT_CODE;

    return retVal;
}

//*****************************************************************************
// Format a Runtime Error message.
//*****************************************************************************
HRESULT __cdecl FormatRuntimeErrorVa(        
    __inout_ecount(cchMsg) WCHAR       *rcMsg,                 // Buffer into which to format.         
    ULONG       cchMsg,                 // Size of buffer, characters.          
    HRESULT     hrRpt,                  // The HR to report.                    
    va_list     marker)                 // Optional args.                       
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
        
    WCHAR       rcBuf[512];             // Resource string.
    HRESULT     hr;
    
    // Ensure nul termination.
    *rcMsg = L'\0';

    // If this is one of our errors or if it is simply a resource ID, then grab the error from the rc file.
    if ((HRESULT_FACILITY(hrRpt) == FACILITY_URT) || (HIWORD(hrRpt) == 0))
    {
        hr = UtilLoadStringRC(LOWORD(hrRpt), rcBuf, NumItems(rcBuf), true);
        if (hr == S_OK)
        {
            _vsnwprintf_s(rcMsg, cchMsg, _TRUNCATE, rcBuf, marker);
        }
    }
    // Otherwise it isn't one of ours, so we need to see if the system can
    // find the text for it.
    else
    {
        if (WszFormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                0, hrRpt, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                rcMsg, cchMsg, 0/*                          */))
        {
            hr = S_OK;

            // System messages contain a trailing \r\n, which we don't want normally.
            int iLen = lstrlenW(rcMsg);
            if (iLen > 3 && rcMsg[iLen - 2] == '\r' && rcMsg[iLen - 1] == '\n')
                rcMsg[iLen - 2] = '\0';
        }
        else
            hr = HRESULT_FROM_GetLastError();
    }

    // If we failed to find the message anywhere, then issue a hard coded message.
    if (FAILED(hr))
    {
        _snwprintf_s(rcMsg, cchMsg, _TRUNCATE, L"Common Language Runtime Internal error: 0x%08x", hrRpt);
        DEBUG_STMT(DbgWriteEx(rcMsg));
    }

    return hrRpt;    
}

//*****************************************************************************
// Format a Runtime Error message, varargs.
//*****************************************************************************
HRESULT __cdecl FormatRuntimeError(
    __out_ecount(cchMsg) WCHAR       *rcMsg,                 // Buffer into which to format.
    ULONG       cchMsg,                 // Size of buffer, characters.
    HRESULT     hrRpt,                  // The HR to report.
    ...)                                // Optional args.
{
    WRAPPER_CONTRACT;
    va_list     marker;                 // User text.
    va_start(marker, hrRpt);
    hrRpt = FormatRuntimeErrorVa(rcMsg, cchMsg, hrRpt, marker);
    va_end(marker);
    return hrRpt;
}

//*****************************************************************************
// This function will post an error for the client.  If the LOWORD(hrRpt) can
// be found as a valid error message, then it is loaded and formatted with
// the arguments passed in.  If it cannot be found, then the error is checked
// against FormatMessage to see if it is a system error.  System errors are
// not formatted so no add'l parameters are required.  If any errors in this
// process occur, hrRpt is returned for the client with no error posted.
//*****************************************************************************
extern "C"
HRESULT __cdecl PostErrorVA(                      // Returned error.
    HRESULT     hrRpt,                  // Reported error.
    va_list     marker)                  // Error arguments.
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        ENTRY_POINT;
    }
    CONTRACTL_END;
        
    WCHAR       rcMsg[4096];             // Error message.
    HRESULT     hr;

    BEGIN_ENTRYPOINT_NOTHROW;

    // Return warnings without text.
    if (!FAILED(hrRpt))
        goto ErrExit;        

    // If we are already out of memory or out of stack or the thread is in some bad state,
    // we don't want throw gasoline on the fire by calling ErrorInfo stuff below (which can
    // trigger a delayload of oleaut32.dll). We don't need to embellish transient errors
    // so just return this without text.
    if (Exception::IsTransient(hrRpt))
    {
        goto ErrExit;
    }

    // Format the error.
    FormatRuntimeErrorVa(rcMsg, lengthof(rcMsg), hrRpt, marker);

    // Turn the error into a posted error message.  If this fails, we still
    // return the original error message since a message caused by our error
    // handling system isn't going to give you a clue about the original error.
    hr = FillErrorInfo(rcMsg, LOWORD(hrRpt));
    _ASSERTE(hr == S_OK);

ErrExit:

    END_ENTRYPOINT_NOTHROW;

    return (hrRpt);
}

//*****************************************************************************
// This function will post an error for the client.  If the LOWORD(hrRpt) can
// be found as a valid error message, then it is loaded and formatted with
// the arguments passed in.  If it cannot be found, then the error is checked
// against FormatMessage to see if it is a system error.  System errors are
// not formatted so no add'l parameters are required.  If any errors in this
// process occur, hrRpt is returned for the client with no error posted.
//*****************************************************************************
extern "C"
HRESULT __cdecl PostError(                      // Returned error.
    HRESULT     hrRpt,                  // Reported error.
    ...)                                // Error arguments.
{
    WRAPPER_CONTRACT;
    va_list     marker;                 // User text.
    va_start(marker, hrRpt);
    hrRpt = PostErrorVA(hrRpt, marker);
    va_end(marker);
    return hrRpt;
}

//*****************************************************************************
// Create, fill out and set an error info object.  Note that this does not fill
// out the IID for the error object; that is done elsewhere.
//*****************************************************************************
HRESULT FillErrorInfo(                  // Return status.
    LPCWSTR     szMsg,                  // Error message.
    DWORD       dwHelpContext)          // Help context.
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
        
    ICreateErrorInfo *pICreateErr;      // Error info creation Iface pointer.
    IErrorInfo *pIErrInfo = NULL;       // The IErrorInfo interface.
    HRESULT     hr;                     // Return status.

    // Get the ICreateErrorInfo pointer.
    hr = S_OK;
    EX_TRY
    {
        hr = CreateErrorInfo(&pICreateErr);
    }
    EX_CATCH
    {
        hr = GET_EXCEPTION()->GetHR();
    }
    EX_END_CATCH(SwallowAllExceptions);

    if (FAILED(hr))
        return (hr);

    // Set message text description.
    if (FAILED(hr = pICreateErr->SetDescription((LPWSTR) szMsg)))
        goto Exit1;

    // Set the help file and help context.
    if (FAILED(hr = pICreateErr->SetHelpFile(L"complib.hlp")) ||
        FAILED(hr = pICreateErr->SetHelpContext(dwHelpContext)))
        goto Exit1;

    // Get the IErrorInfo pointer.
    if (FAILED(hr = pICreateErr->QueryInterface(IID_IErrorInfo, (PVOID *) &pIErrInfo)))
        goto Exit1;

    // Save the error and release our local pointers.
    {
        // If we get here, we have loaded oleaut32.dll.
        CONTRACT_VIOLATION(ThrowsViolation);
        SetErrorInfo(0L, pIErrInfo);
    }

Exit1:
    pICreateErr->Release();
    if (pIErrInfo) {
        pIErrInfo->Release();
    }
    return hr;
}

