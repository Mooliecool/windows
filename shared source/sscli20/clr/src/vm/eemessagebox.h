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
// This module contains the definition for the message box utility code for use
// inside the Execution Engine. These APIs ensure the GC mode is properly
// toggled to preemptive before the dialog is displayed. 
//
//*****************************************************************************
#ifndef _H_EEMESSAGEBOX
#define _H_EEMESSAGEBOX

//========================================================================
// APIs to pop messages boxes. These should be used instead of the UtilXXX 
// versions since they ensure we properly switch to preemptive GC mode and 
// validate that the thread can tolerate GC transitions before calling 
// out.
//========================================================================

int EEMessageBoxCatastrophic(
                  UINT iText,       // Text for MessageBox
                  UINT iTitle,      // Title for MessageBox
                  ...);             // Additional Arguments

int EEMessageBox(
                  UINT uText,       // Resource Identifier for Text message
                  UINT uCaption,    // Resource Identifier for Caption
                  UINT uType,       // Style of MessageBox
                  ...);             // Additional Arguments

#ifdef _DEBUG

int EEMessageBoxNonLocalizedDebugOnly(
                  LPCWSTR lpText,    // Text message
                  LPCWSTR lpCaption, // Caption
                  UINT uType,       // Style of MessageBox
                  ... );            // Additional Arguments

#endif // _DEBUG
    
#endif /* _H_EEMESSAGEBOX */

