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
/*============================================================
**
** Class: UnicodeCategory
**
**
** Purpose: 
**
**
============================================================*/
namespace System.Globalization {

    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum UnicodeCategory {

        UppercaseLetter = 0,

        LowercaseLetter = 1,

        TitlecaseLetter = 2,

        ModifierLetter = 3,

        OtherLetter = 4,

        NonSpacingMark = 5,

        SpacingCombiningMark = 6,

        EnclosingMark = 7,

        DecimalDigitNumber = 8,

        LetterNumber = 9,

        OtherNumber = 10,

        SpaceSeparator = 11,

        LineSeparator = 12,

        ParagraphSeparator = 13,

        Control = 14,

        Format = 15,

        Surrogate = 16,

        PrivateUse = 17,

        ConnectorPunctuation = 18,

        DashPunctuation = 19,

        OpenPunctuation = 20,

        ClosePunctuation = 21,

        InitialQuotePunctuation = 22,

        FinalQuotePunctuation = 23,

        OtherPunctuation = 24,

        MathSymbol = 25,

        CurrencySymbol = 26,

        ModifierSymbol = 27,

        OtherSymbol = 28,

        OtherNotAssigned = 29,
    }
}
