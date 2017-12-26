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
// EEMessageBox.h
//
// This module contains the implementation for the message box utility code for 
// use inside the Execution Engine. These APIs ensure the GC mode is properly
// toggled to preemptive before the dialog is displayed. 
//
//*****************************************************************************
#include "common.h"
#include "eemessagebox.h"

// Undef these so we can call them from the EE versions.
#undef UtilMessageBoxCatastrophicVA
#undef UtilMessageBoxVA
#undef UtilMessageBoxNonLocalizedVA

int EEMessageBoxCatastrophic(
                  UINT uText,       // Text for MessageBox
                  UINT uTitle,      // Title for MessageBox
                  ...)              // Additional Arguments
{
    CONTRACTL
    {
        MODE_ANY;
        GC_NOTRIGGER;
        NOTHROW;
    }
    CONTRACTL_END;

    va_list marker;
    va_start(marker, uTitle);

    int result = UtilMessageBoxCatastrophicVA(uText, uTitle, MB_OK | MB_ICONERROR, TRUE, marker);
    va_end( marker );

    return result;
}

int EEMessageBox(
                  UINT uText,       // Resource Identifier for Text message
                  UINT uTitle,      // Resource Identifier for Title
                  UINT uType,       // Style of MessageBox
                  ...)              // Additional Arguments
{
    CONTRACTL
    {
        MODE_ANY;
        GC_TRIGGERS;
        NOTHROW;
    }
    CONTRACTL_END;

    GCX_PREEMP();

    va_list marker;
    va_start(marker, uType);

    int result = UtilMessageBoxVA(NULL, uText, uTitle, uType, TRUE, marker);
    va_end( marker );

    return result;
}

#ifdef _DEBUG

int EEMessageBoxNonLocalizedDebugOnly(
                  LPCWSTR lpText,   // Text message
                  LPCWSTR lpTitle,  // Caption
                  UINT uType,       // Style of MessageBox
                  ... )             // Additional Arguments
{
    CONTRACTL
    {
        MODE_ANY;
        GC_TRIGGERS;
        NOTHROW;
    }
    CONTRACTL_END;

    GCX_PREEMP();

    va_list marker;
    va_start(marker, uType);

    int result = UtilMessageBoxNonLocalizedVA(NULL, lpText, lpTitle, uType, TRUE, marker);
    va_end( marker );

    return result;
}

#endif // _DEBUG

// Redefine these to errors just in case code is added after this point in the file.
#define UtilMessageBoxCatastrophicVA __error("Use one of the EEMessageBox APIs (defined in eemessagebox.h) from inside the EE")
#define UtilMessageBoxVA __error("Use one of the EEMessageBox APIs (defined in eemessagebox.h) from inside the EE")
#define UtilMessageBoxNonLocalizedVA __error("Use one of the EEMessageBox APIs (defined in eemessagebox.h) from inside the EE")

