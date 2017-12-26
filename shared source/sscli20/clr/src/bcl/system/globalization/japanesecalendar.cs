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

    /*=================================JapaneseCalendar==========================
    **
    ** JapaneseCalendar is based on Gregorian calendar.  The month and day values are the same as
    ** Gregorian calendar.  However, the year value is an offset to the Gregorian
    ** year based on the era.
    **
    ** This system is adopted by Emperor Meiji in 1868. The year value is counted based on the reign of an emperor,
    ** and the era begins on the day an emperor ascends the throne and continues until his death.
    ** The era changes at 12:00AM.
    **
    ** For example, the current era is Heisei.  It started on 1989/1/8 A.D.  Therefore, Gregorian year 1989 is also Heisei 1st.
    ** 1989/1/8 A.D. is also Heisei 1st 1/8.
    **
    ** Any date in the year during which era is changed can be reckoned in either era.  For example,
    ** 1989/1/1 can be 1/1 Heisei 1st year or 1/1 Showa 64th year.
    **
    ** Note:
    **  The DateTime can be represented by the JapaneseCalendar are limited to two factors:
    **      1. The min value and max value of DateTime class.
    **      2. The available era information.
    **
    **  Calendar support range:
    **      Calendar    Minimum     Maximum
    **      ==========  ==========  ==========
    **      Gregorian   1868/09/08  9999/12/31
    **      Japanese    Meiji 01/01 Heisei 8011/12/31
    ============================================================================*/


    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public class JapaneseCalendar : Calendar
    {
        internal static readonly DateTime calendarMinValue = new DateTime(1868, 9, 8);


        [System.Runtime.InteropServices.ComVisible(false)]
        public override DateTime MinSupportedDateTime
        {
            get
            {
                return (calendarMinValue);
            }
        }

        [System.Runtime.InteropServices.ComVisible(false)]
        public override DateTime MaxSupportedDateTime
        {
            get
            {
                return (DateTime.MaxValue);
            }
        }

        // Return the type of the Japanese calendar.
        //

        [System.Runtime.InteropServices.ComVisible(false)]
        public override CalendarAlgorithmType AlgorithmType
        {
            get
            {
                return CalendarAlgorithmType.SolarCalendar;
            }
        }

        // m_EraInfo must be listed in reverse chronological order.  The most recent era
        // should be the first element.
        // That is, m_EraInfo[0] contains the most recent era.
        static internal EraInfo[] m_EraInfo = GregorianCalendarHelper.InitEraInfo(Calendar.CAL_JAPAN);
        // The era value of the current era.

        internal static Calendar m_defaultInstance;
        internal GregorianCalendarHelper helper;

        /*=================================GetDefaultInstance==========================
        **Action: Internal method to provide a default intance of JapaneseCalendar.  Used by NLS+ implementation
        **       and other calendars.
        **Returns:
        **Arguments:
        **Exceptions:
        ============================================================================*/

        internal static Calendar GetDefaultInstance() {
            if (m_defaultInstance == null) {
                m_defaultInstance = new JapaneseCalendar();
            }
            return (m_defaultInstance);
        }


        public JapaneseCalendar() {
            helper = new GregorianCalendarHelper(this, m_EraInfo);
        }

        internal override int ID {
            get {
                return (CAL_JAPAN);
            }
        }


        public override DateTime AddMonths(DateTime time, int months) {
            return (helper.AddMonths(time, months));
        }


        public override DateTime AddYears(DateTime time, int years) {
            return (helper.AddYears(time, years));
        }

        /*=================================GetDaysInMonth==========================
        **Action: Returns the number of days in the month given by the year and month arguments.
        **Returns: The number of days in the given month.
        **Arguments:
        **      year The year in Japanese calendar.
        **      month The month
        **      era     The Japanese era value.
        **Exceptions
        **  ArgumentException  If month is less than 1 or greater * than 12.
        ============================================================================*/


        public override int GetDaysInMonth(int year, int month, int era) {
            return (helper.GetDaysInMonth(year, month, era));
        }


        public override int GetDaysInYear(int year, int era) {
            return (helper.GetDaysInYear(year, era));
        }


        public override int GetDayOfMonth(DateTime time) {
            return (helper.GetDayOfMonth(time));
        }


        public override DayOfWeek GetDayOfWeek(DateTime time)  {
            return (helper.GetDayOfWeek(time));
        }


        public override int GetDayOfYear(DateTime time)
        {
            return (helper.GetDayOfYear(time));
        }


        public override int GetMonthsInYear(int year, int era)
        {
            return (helper.GetMonthsInYear(year, era));
        }


        [System.Runtime.InteropServices.ComVisible(false)]
        public override int GetWeekOfYear(DateTime time, CalendarWeekRule rule, DayOfWeek firstDayOfWeek)
        {
            return (helper.GetWeekOfYear(time, rule, firstDayOfWeek));
        }

        /*=================================GetEra==========================
        **Action: Get the era value of the specified time.
        **Returns: The era value for the specified time.
        **Arguments:
        **      time the specified date time.
        **Exceptions: ArgumentOutOfRangeException if time is out of the valid era ranges.
        ============================================================================*/


        public override int GetEra(DateTime time) {
            return (helper.GetEra(time));
        }


        public override int GetMonth(DateTime time) {
            return (helper.GetMonth(time));
            }


        public override int GetYear(DateTime time) {
            return (helper.GetYear(time));
        }


        public override bool IsLeapDay(int year, int month, int day, int era)
        {
            return (helper.IsLeapDay(year, month, day, era));
        }


        public override bool IsLeapYear(int year, int era) {
            return (helper.IsLeapYear(year, era));
        }

        // Returns  the leap month in a calendar year of the specified era. This method returns 0
        // if this calendar does not have leap month, or this year is not a leap year.
        //

        [System.Runtime.InteropServices.ComVisible(false)]
        public override int GetLeapMonth(int year, int era)
        {
            return (helper.GetLeapMonth(year, era));
        }


        public override bool IsLeapMonth(int year, int month, int era) {
            return (helper.IsLeapMonth(year, month, era));
        }


        public override DateTime ToDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, int era) {
            return (helper.ToDateTime(year, month, day, hour, minute, second, millisecond, era));
        }

        // For Japanese calendar, four digit year is not used.  Few emperors will live for more than one hundred years.
        // Therefore, for any two digit number, we just return the original number.

        public override int ToFourDigitYear(int year) {
            if (year <= 0) {
                throw new ArgumentOutOfRangeException("year",
                    Environment.GetResourceString("ArgumentOutOfRange_NeedPosNum"));
            }

            if (year > helper.MaxYear) {
                throw new ArgumentOutOfRangeException(
                            "year",
                            String.Format(
                                CultureInfo.CurrentCulture,
                                Environment.GetResourceString("ArgumentOutOfRange_Range"),
                                1,
                                helper.MaxYear));
            }
            return (year);
        }


        public override int[] Eras {
            get {
                return (helper.Eras);
            }
        }

        private const int DEFAULT_TWO_DIGIT_YEAR_MAX = 99;
        
        internal override bool IsValidYear(int year, int era) {
            return helper.IsValidYear(year, era);
        }       

        public override int TwoDigitYearMax {
            get {
                if (twoDigitYearMax == -1) {
                    twoDigitYearMax = GetSystemTwoDigitYearSetting(ID, DEFAULT_TWO_DIGIT_YEAR_MAX);
                }
                return (twoDigitYearMax);
            }

            set {
                VerifyWritable();
                if (value < 99|| value > helper.MaxYear) {
                    throw new ArgumentOutOfRangeException(
                                "year",
                                String.Format(
                                    CultureInfo.CurrentCulture,
                                    Environment.GetResourceString("ArgumentOutOfRange_Range"),
                                    99,
                                    helper.MaxYear));
                }
                twoDigitYearMax = value;
            }
        }
    }
}
