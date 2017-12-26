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
// UtilMessageBox.cpp
//
// This module contains the message box utility code for the CLR. It is used
// by code in the CLR itself as well as other tools that build in the CLR tree.
// For message boxes inside the ExecutionEngine, EEMessageBox must be used
// instead of the these APIs.
//
//*****************************************************************************
#include "stdafx.h"                     // Standard header.
#include <utilcode.h>                   // Utility helpers.
#include <corerror.h>
#include "../dlls/mscorrc/resource.h"
#include "ex.h"
#include "ndpversion.h"

//
// This should be used for runtime dialog box, because we assume the resource is from mscorrc.dll
// For tools like ildasm or Shim which uses their own resource file, you need to define IDS_RTL in 
// their resource file and define a function like this and append the style returned from the function 
// to every calls to WszMessageBox.
//
UINT GetCLRMBRTLStyle() 
{
    WRAPPER_CONTRACT;

    UINT mbStyle = 0;
    WCHAR buff[MAX_PATH];                        
    if(SUCCEEDED(UtilLoadStringRC(IDS_RTL, buff, MAX_PATH, true))) {
        if(wcscmp(buff, L"RTL_True") == 0) {
            mbStyle = 0x00080000 |0x00100000; // MB_RIGHT || MB_RTLREADING
        }
    }
    return mbStyle;
}

BOOL ShouldDisplayMsgBoxOnCriticalFailure()
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;

#ifdef _DEBUG
    // To help find issues, we will always display dialogs for critical failures
    // under debug builds. This includes asserts and other critical issues.
   return TRUE;
#else      
    // Retrieve error mode
    UINT last = SetErrorMode(0);
    SetErrorMode(last);         //set back to previous value
                    
    // SEM_FAILCRITICALERRORS indicates that the system does not display the critical-error-handler 
    // message box. Instead, the system sends the error to the calling process.
    return !(last & SEM_FAILCRITICALERRORS);
#endif // _DEBUG
}

int UtilMessageBoxVA(
                  HWND hWnd,        // Handle to Owner Window
                  UINT uText,       // Resource Identifier for Text message
                  UINT uTitle,      // Resource Identifier for Title
                  UINT uType,       // Style of MessageBox
                  BOOL ShowFileNameInTitle, // Flag to show FileName in Caption
                  va_list args)     // Additional Arguments
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return IDCANCEL;);
    }
    CONTRACTL_END;

    SString text;
    SString title; 
    int result = IDCANCEL;
    
    EX_TRY
    {
        text.LoadResource(uText);
        title.LoadResource(uTitle);

        result = UtilMessageBoxNonLocalizedVA(hWnd, (LPWSTR)text.GetUnicode(), (LPWSTR)title.GetUnicode(), uType, ShowFileNameInTitle, args);
    }
    EX_CATCH
    {
        result = IDCANCEL;
    }
    EX_END_CATCH(SwallowAllExceptions);

    return result;            
}

int UtilMessageBoxNonLocalizedVA(
                  HWND hWnd,        // Handle to Owner Window
                  LPCWSTR lpText,   // Text message
                  LPCWSTR lpTitle,  // Title
                  UINT uType,       // Style of MessageBox
                  BOOL ShowFileNameInTitle, // Flag to show FileName in Caption
                  va_list args)     // Additional Arguments
{
    CONTRACTL
    {
        NOTHROW;
        INJECT_FAULT(return IDCANCEL;);

        // Assert if none of MB_ICON is set
        PRECONDITION((uType & MB_ICONMASK) != 0);
    }
    CONTRACTL_END;

    int result = IDCANCEL;

    EX_TRY
    {   
        StackSString formattedMessage;
        StackSString formattedTitle;
        StackSString fileName;
        BOOL fDisplayMsgBox = TRUE;
        
        // Format message string using optional parameters
        formattedMessage.VPrintf(lpText, args);
       
        // Try to get filename of Module and add it to title
        if (ShowFileNameInTitle && WszGetModuleFileName(NULL, fileName.OpenUnicodeBuffer(MAX_PATH), MAX_PATH))
        {           
            StackSString drive, dir, name, ext;

            // Close the buffer we opened before the call to WszGetModuleFileName.
            fileName.CloseBuffer();            
            
            // gcc 2.9 refuses to implicitly convert StackSString to SString
            SplitPath(SString(fileName), drive, dir, name, ext);
            formattedTitle.Printf(L"%s%s - %s", name.GetUnicode(), ext.GetUnicode(), lpTitle);
        }
        else
        {
            formattedTitle.Set(lpTitle);
        }


        if (fDisplayMsgBox)
            result = WszMessageBox(hWnd, formattedMessage, formattedTitle, uType | GetCLRMBRTLStyle());;
    }        
    EX_CATCH
    {
        result = IDCANCEL;
    }
    EX_END_CATCH(SwallowAllExceptions);

    return result;
}

int UtilMessageBox(
                  HWND hWnd,        // Handle to Owner Window
                  UINT uText,       // Resource Identifier for Text message
                  UINT uTitle,      // Resource Identifier for Title
                  UINT uType,       // Style of MessageBox
                  BOOL ShowFileNameInTitle, // Flag to show FileName in Caption
                  ...)              // Additional Arguments
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;

    va_list marker;
    va_start(marker, ShowFileNameInTitle);

    int result = UtilMessageBoxVA(hWnd, uText, uTitle, uType, ShowFileNameInTitle, marker);
    va_end( marker );

    return result;    
}

int UtilMessageBoxNonLocalized(
                  HWND hWnd,        // Handle to Owner Window
                  LPCWSTR lpText,   // Text message
                  LPCWSTR lpTitle,  // Title message
                  UINT uType,       // Style of MessageBox
                  BOOL ShowFileNameInTitle, // Flag to show FileName in Caption
                  ... )             // Additional Arguments
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;

    va_list marker;
    va_start(marker, ShowFileNameInTitle);

    int result = UtilMessageBoxNonLocalizedVA(hWnd, lpText, lpTitle, uType, ShowFileNameInTitle, marker);
    va_end( marker );

    return result;
}

int UtilMessageBoxCatastrophic(
                  UINT uText,       // Text for MessageBox
                  UINT uTitle,      // Title for MessageBox
                  UINT uType,       // Style of MessageBox
                  BOOL ShowFileNameInTitle, // Flag to show FileName in Caption
                  ...)
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;

    va_list marker;
    va_start(marker, ShowFileNameInTitle);

    int result = UtilMessageBoxCatastrophicVA(uText, uTitle, uType, ShowFileNameInTitle, marker);
    va_end( marker );

    return result;
}

int UtilMessageBoxCatastrophicNonLocalized(
                  LPCWSTR lpText,    // Text for MessageBox
                  LPCWSTR lpTitle,   // Title for MessageBox
                  UINT uType,       // Style of MessageBox
                  BOOL ShowFileNameInTitle, // Flag to show FileName in Caption
                  ...)
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;

    va_list marker;
    va_start(marker, ShowFileNameInTitle);

    int result = UtilMessageBoxCatastrophicNonLocalizedVA(lpText, lpTitle, uType, ShowFileNameInTitle, marker);
    va_end( marker );

    return result;
}

int UtilMessageBoxCatastrophicVA(
                  UINT uText,       // Text for MessageBox
                  UINT uTitle,      // Title for MessageBox
                  UINT uType,       // Style of MessageBox
                  BOOL ShowFileNameInTitle, // Flag to show FileName in Caption
                  va_list args)     // Additional Arguments
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;

    HWND hwnd = NULL;

    // We are already in a catastrophic situation so we can tolerate faults as well as SO & GC mode violations to keep going. 
    CONTRACT_VIOLATION(FaultNotFatal | GCViolation | ModeViolation | SOToleranceViolation);

    if (!ShouldDisplayMsgBoxOnCriticalFailure())
        return IDABORT;

    // GetDesktopWindow is not supported under ROTOR.

    // Add the MB_SYSTEMMODAL style and the MB_SERVICE_NOTIFICATION style if required.
    uType |= MB_SYSTEMMODAL;
    uType |= RunningInteractive() ? 0 : MB_SERVICE_NOTIFICATION;

    return UtilMessageBoxVA(hwnd, uText, uTitle, uType, ShowFileNameInTitle, args);
}

int UtilMessageBoxCatastrophicNonLocalizedVA(
                  LPCWSTR lpText,   // Text for MessageBox
                  LPCWSTR lpTitle,  // Title for MessageBox
                  UINT uType,       // Style of MessageBox
                  BOOL ShowFileNameInTitle, // Flag to show FileName in Caption
                  va_list args)     // Additional Arguments
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;

    HWND hwnd = NULL;

    // We are already in a catastrophic situation so we can tolerate faults as well as SO & GC mode violations to keep going. 
    CONTRACT_VIOLATION(FaultNotFatal | GCViolation | ModeViolation | SOToleranceViolation);

    if (!ShouldDisplayMsgBoxOnCriticalFailure())
        return IDABORT;

    // GetDesktopWindow is not supported under ROTOR.

    // Add the MB_SYSTEMMODAL style and the MB_SERVICE_NOTIFICATION style if required.
    uType |= MB_SYSTEMMODAL;
    uType |= RunningInteractive() ? 0 : MB_SERVICE_NOTIFICATION;

    return UtilMessageBoxNonLocalizedVA(hwnd, lpText, lpTitle, uType, ShowFileNameInTitle, args);
}

