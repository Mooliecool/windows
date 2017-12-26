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
//
// The enumeration constants used in NumberFormatInfo.DigitSubstitution.
//
namespace System.Globalization {    


    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum DigitShapes : int {

        Context         = 0x0000,   // The shape depends on the previous text in the same output.

        None            = 0x0001,   // Gives full Unicode compatibility.

        NativeNational  = 0x0002,   // National shapes determined by LOCALE_SNATIVEDIGITS
    }
}

