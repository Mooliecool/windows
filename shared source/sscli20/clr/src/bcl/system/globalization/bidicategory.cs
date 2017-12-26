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
** Class: BidiCategory
**
** Purpose:
**
**
============================================================*/
namespace System.Globalization {
    [Serializable]
    internal enum BidiCategory {
        LeftToRight = 0,
        LeftToRightEmbedding = 1,
        LeftToRightOverride = 2,
        RightToLeft = 3,
        RightToLeftArabic = 4,
        RightToLeftEmbedding = 5,
        RightToLeftOverride = 6,
        PopDirectionalFormat = 7,
        EuropeanNumber = 8,
        EuropeanNumberSeparator = 9,
        EuropeanNumberTerminator = 10,
        ArabicNumber = 11,
        CommonNumberSeparator = 12,
        NonSpacingMark = 13,
        BoundaryNeutral = 14,
        ParagraphSeparator = 15,
        SegmentSeparator = 16,
        Whitespace = 17,
        OtherNeutrals = 18,
    }
}
