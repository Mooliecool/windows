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
//=============================================================================
//
// Class: __HResults
//
// Author: Automatically generated
//
// Purpose: Define HResult constants. Every exception has one of these.
//
// Date: 98/08/31 11:57:11 AM
//
//===========================================================================*/
namespace System.IO {
    using System;
    // Only static data no need to serialize
    internal static class __HResults
    {
        // These use an error code from WinError.h
        public const int COR_E_ENDOFSTREAM = unchecked((int)0x80070026);  // OS defined
        public const int COR_E_FILELOAD = unchecked((int)0x80131621);
        public const int COR_E_FILENOTFOUND = unchecked((int)0x80070002);
        public const int COR_E_DIRECTORYNOTFOUND = unchecked((int)0x80070003);
        public const int COR_E_PATHTOOLONG = unchecked((int)0x800700CE);

        public const int COR_E_IO = unchecked((int)0x80131620);
    }
}
