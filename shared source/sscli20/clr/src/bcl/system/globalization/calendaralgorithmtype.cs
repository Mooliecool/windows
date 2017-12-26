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

[System.Runtime.InteropServices.ComVisible(true)]
    public enum CalendarAlgorithmType {
        Unknown = 0,            // This is the default value to return in the Calendar base class.
        SolarCalendar = 1,      // Solar-base calendar, such as GregorianCalendar, jaoaneseCalendar, JulianCalendar, etc.
                                // Solar calendars are based on the solar year and seasons.
        LunarCalendar = 2,      // Lunar-based calendar, such as Hijri and UmAlQuraCalendar.
                                // Lunar calendars are based on the path of the moon.  The seasons are not accurately represented.
        LunisolarCalendar = 3

    }
}
