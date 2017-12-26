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
namespace System.Globalization {
    using System;


    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum CalendarWeekRule
    {

        FirstDay = 0,           // Week 1 begins on the first day of the year

        FirstFullWeek = 1,      // Week 1 begins on first FirstDayOfWeek not before the first day of the year

        FirstFourDayWeek = 2    // Week 1 begins on first FirstDayOfWeek such that FirstDayOfWeek+3 is not before the first day of the year        
    };
}
