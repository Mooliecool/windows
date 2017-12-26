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
    using System.Threading;
    using System;
    /*=================================EraInfo==========================
    **
    ** This is the structure to store information about an era.
    **
    ============================================================================*/

    [Serializable]
    internal class EraInfo {
        internal int era;       // The value of the era.
        internal long ticks;    // The time in ticks when the era starts
        internal int yearOffset;    // The offset to Gregorian year when the era starts.
                                    // Gregorian Year = Era Year + yearOffset
                                    // Era Year = Gregorian Year - yearOffset
        internal int minEraYear;        // Min year value in this era. Generally, this value is 1, but this may
                                        // be affected by the DateTime.MinValue;
        internal int maxEraYear;       // Max year value in this era. (== the year length of the era + 1)

        internal EraInfo(int era, long ticks, int yearOffset, int minEraYear, int maxEraYear) {
            this.era = era;
            this.ticks = ticks;
            this.yearOffset = yearOffset;
            this.minEraYear = minEraYear;
            this.maxEraYear = maxEraYear;
        }
    }                

    // 
    // This class implements the Gregorian calendar. In 1582, Pope Gregory XIII made 
    // minor changes to the solar Julian or "Old Style" calendar to make it more 
    // accurate. Thus the calendar became known as the Gregorian or "New Style" 
    // calendar, and adopted in Catholic countries such as Spain and France. Later 
    // the Gregorian calendar became popular throughout Western Europe because it 
    // was accurate and convenient for international trade. Scandinavian countries 
    // adopted it in 1700, Great Britain in 1752, the American colonies in 1752 and 
    // India in 1757. China adopted the same calendar in 1911, Russia in 1918, and 
    // some Eastern European countries as late as 1940.
    // 
    // This calendar recognizes two era values:
    // 0 CurrentEra (AD) 
    // 1 BeforeCurrentEra (BC) 
    [Serializable] internal class GregorianCalendarHelper {

        // 1 tick = 100ns = 10E-7 second
        // Number of ticks per time unit
        internal const long TicksPerMillisecond   = 10000;
        internal const long TicksPerSecond        = TicksPerMillisecond * 1000;
        internal const long TicksPerMinute        = TicksPerSecond * 60;
        internal const long TicksPerHour          = TicksPerMinute * 60;
        internal const long TicksPerDay           = TicksPerHour * 24;
    
        // Number of milliseconds per time unit
        internal const int MillisPerSecond        = 1000;
        internal const int MillisPerMinute        = MillisPerSecond * 60;
        internal const int MillisPerHour          = MillisPerMinute * 60;
        internal const int MillisPerDay           = MillisPerHour * 24;    

        // Number of days in a non-leap year
        internal const int DaysPerYear            = 365;
        // Number of days in 4 years
        internal const int DaysPer4Years          = DaysPerYear * 4 + 1;
        // Number of days in 100 years
        internal const int DaysPer100Years        = DaysPer4Years * 25 - 1;
        // Number of days in 400 years
        internal const int DaysPer400Years        = DaysPer100Years * 4 + 1;
    
        // Number of days from 1/1/0001 to 1/1/10000
        internal const int DaysTo10000            = DaysPer400Years * 25 - 366;    

        internal const long MaxMillis             = (long)DaysTo10000 * MillisPerDay;

        internal const int DatePartYear = 0;
        internal const int DatePartDayOfYear = 1;
        internal const int DatePartMonth = 2;
        internal const int DatePartDay = 3;    

        //
        // This is the max Gregorian year can be represented by DateTime class.  The limitation
        // is derived from DateTime class.
        // 
        internal int MaxYear {
            get {
                return (m_maxYear);
            }
        }

        internal static readonly int[] DaysToMonth365 = 
        {
            0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365
        };
        
        internal static readonly int[] DaysToMonth366 = 
        {
            0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366
        };
        
        internal int m_maxYear = 9999;
        internal int m_minYear;
        internal Calendar m_Cal;
        internal EraInfo[] m_EraInfo;    
        internal int[] m_eras = null;

        // m_minDate is existing here just to keep the serialization compatibility. 
        // it has nothing to do with the code anymore. 
        internal DateTime m_minDate;

        /*=================================InitEraInfo==========================
        **Action: Get the era range information from calendar.nlp.
        **Returns: An array of EraInfo, which contains the information about an era.
        **Arguments:
        **      calID     the Calendar ID defined in Calendar.cs
        ============================================================================*/
        internal static EraInfo[] InitEraInfo(int calID) {
            //
            // Initialize era information.
            //

            /*
            Japaense era info example:
            eraInfo[0] = new EraInfo(4, new DateTime(1989, 1, 8).Ticks, 1988, 1, GregorianCalendar.MaxYear - 1988);    // Heisei. Most recent
            eraInfo[1] = new EraInfo(3, new DateTime(1926, 12, 25).Ticks, 1925, 1, 1989 - 1925);  // Showa
            eraInfo[2] = new EraInfo(2, new DateTime(1912, 7, 30).Ticks, 1911, 1, 1926 - 1911);   // Taisho
            eraInfo[3] = new EraInfo(1, new DateTime(1868, 9, 8).Ticks, 1867, 1, 1912 - 1867);    // Meiji            
            */
            BCLDebug.Assert(calID > 0, "[GregorianCalendarHelper.InitEraInfo] Expected calID > 0");
            int[][] eraRanges = CalendarTable.Default.SERARANGES(calID);
            EraInfo [] eraInfo = new EraInfo[eraRanges.Length];
            int maxEraYear = GregorianCalendar.MaxYear;
            for (int i = 0; i < eraRanges.Length; i++)
            {
                //
                // The eraRange arrays are each the form of "4 1989 1 8 1988 1".
                // eraRanges[i][0] is the era value.
                // eraRanges[i][1] is the year when the era starts.
                // eraRanges[i][2] is the month when the era starts.
                // eraRanges[i][3] is the day when the era starts.
                // eraRanges[i][4] is the offset to Gregorian year (1988).
                // eraRanges[i][5] is the minimum era year for this era.
                //
                BCLDebug.Assert(eraRanges[i].Length == 6, "[GregorianCalendarHelper.InitEraInfo] Expected 6 SERARANGE elements (0-5), not " + eraRanges[i].Length);
                eraInfo[i] = new EraInfo(eraRanges[i][0], new DateTime(eraRanges[i][1], eraRanges[i][2], eraRanges[i][3]).Ticks, eraRanges[i][4], eraRanges[i][5], maxEraYear - eraRanges[i][4]);
                maxEraYear= eraRanges[i][1];
            }
            return (eraInfo);
        }
        
        // Construct an instance of gregorian calendar.
        internal GregorianCalendarHelper(Calendar cal, EraInfo[] eraInfo) {
            m_Cal = cal;
            m_EraInfo = eraInfo;            
            // m_minDate is existing here just to keep the serialization compatibility. 
            // it has nothing to do with the code anymore. 
            m_minDate = m_Cal.MinSupportedDateTime;
            m_maxYear = m_EraInfo[0].maxEraYear;
            m_minYear = m_EraInfo[0].minEraYear;;
        }
        
        /*=================================GetGregorianYear==========================
        **Action: Get the Gregorian year value for the specified year in an era.
        **Returns: The Gregorian year value.
        **Arguments:
        **      year    the year value in Japanese calendar
        **      era     the Japanese emperor era value.
        **Exceptions:
        **      ArgumentOutOfRangeException if year value is invalid or era value is invalid.
        ============================================================================*/

        internal int GetGregorianYear(int year, int era) {
            if (year < 0) {
                throw new ArgumentOutOfRangeException("year",
                    Environment.GetResourceString("ArgumentOutOfRange_NeedNonNegNum")); 
            }            

            if (era == Calendar.CurrentEra) {
                era = m_Cal.CurrentEraValue;
            }
            
            for (int i = 0; i < m_EraInfo.Length; i++) {
                if (era == m_EraInfo[i].era) {
                    if (year < m_EraInfo[i].minEraYear || year > m_EraInfo[i].maxEraYear) {
                        throw new ArgumentOutOfRangeException(
                                    "year", 
                                    String.Format(
                                        CultureInfo.CurrentCulture,
                                        Environment.GetResourceString("ArgumentOutOfRange_Range"), 
                                        m_EraInfo[i].minEraYear, 
                                        m_EraInfo[i].maxEraYear));
                    }
                    return (m_EraInfo[i].yearOffset + year);
                }
            }
            throw new ArgumentOutOfRangeException("era", Environment.GetResourceString("ArgumentOutOfRange_InvalidEraValue"));
        }
        
        internal bool IsValidYear(int year, int era) {
            if (year < 0) {
                return false;
            }            

            if (era == Calendar.CurrentEra) {
                era = m_Cal.CurrentEraValue;
            }
            
            for (int i = 0; i < m_EraInfo.Length; i++) {
                if (era == m_EraInfo[i].era) {
                    if (year < m_EraInfo[i].minEraYear || year > m_EraInfo[i].maxEraYear) {
                        return false;
                    }
                    return true;
                }
            }
            return false;
        }       
        

        // Returns a given date part of this DateTime. This method is used
        // to compute the year, day-of-year, month, or day part.
        internal virtual int GetDatePart(long ticks, int part) 
        {
            CheckTicksRange(ticks);
            // n = number of days since 1/1/0001
            int n = (int)(ticks / TicksPerDay);
            // y400 = number of whole 400-year periods since 1/1/0001
            int y400 = n / DaysPer400Years;
            // n = day number within 400-year period
            n -= y400 * DaysPer400Years;
            // y100 = number of whole 100-year periods within 400-year period
            int y100 = n / DaysPer100Years;
            // Last 100-year period has an extra day, so decrement result if 4
            if (y100 == 4) y100 = 3;        
            // n = day number within 100-year period
            n -= y100 * DaysPer100Years;
            // y4 = number of whole 4-year periods within 100-year period
            int y4 = n / DaysPer4Years;
            // n = day number within 4-year period
            n -= y4 * DaysPer4Years;
            // y1 = number of whole years within 4-year period
            int y1 = n / DaysPerYear;
            // Last year has an extra day, so decrement result if 4
            if (y1 == 4) y1 = 3;        
            // If year was requested, compute and return it
            if (part == DatePartYear) 
            {
                return (y400 * 400 + y100 * 100 + y4 * 4 + y1 + 1);
            }
            // n = day number within year
            n -= y1 * DaysPerYear;
            // If day-of-year was requested, return it
            if (part == DatePartDayOfYear) 
            {
                return (n + 1);
            }
            // Leap year calculation looks different from IsLeapYear since y1, y4,
            // and y100 are relative to year 1, not year 0
            bool leapYear = (y1 == 3 && (y4 != 24 || y100 == 3));
            int[] days = leapYear? DaysToMonth366: DaysToMonth365;
            // All months have less than 32 days, so n >> 5 is a good conservative
            // estimate for the month
            int m = n >> 5 + 1;
            // m = 1-based month number
            while (n >= days[m]) m++;
            // If month was requested, return it
            if (part == DatePartMonth) return (m);
            // Return 1-based day-of-month
            return (n - days[m - 1] + 1);
        }

        /*=================================GetAbsoluteDate==========================
        **Action: Gets the absolute date for the given Gregorian date.  The absolute date means
        **       the number of days from January 1st, 1 A.D.
        **Returns:  the absolute date
        **Arguments:
        **      year    the Gregorian year
        **      month   the Gregorian month
        **      day     the day
        **Exceptions:
        **      ArgumentOutOfRangException  if year, month, day value is valid.
        **Note:
        **      This is an internal method used by DateToTicks() and the calculations of Hijri and Hebrew calendars.
        **      Number of Days in Prior Years (both common and leap years) +
        **      Number of Days in Prior Months of Current Year +
        **      Number of Days in Current Month
        **
        ============================================================================*/

        internal static long GetAbsoluteDate(int year, int month, int day) {
            if (year >= 1 && year <= 9999 && month >= 1 && month <= 12) 
            {
                int[] days = ((year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))) ? DaysToMonth366: DaysToMonth365;
                if (day >= 1 && (day <= days[month] - days[month - 1])) {
                    int y = year - 1;
                    int absoluteDate = y * 365 + y / 4 - y / 100 + y / 400 + days[month - 1] + day - 1;
                    return (absoluteDate);
                }
            }
            throw new ArgumentOutOfRangeException(null, Environment.GetResourceString("ArgumentOutOfRange_BadYearMonthDay"));
        }        

        // Returns the tick count corresponding to the given year, month, and day.
        // Will check the if the parameters are valid.
        internal static long DateToTicks(int year, int month, int day) {
            return (GetAbsoluteDate(year, month,  day)* TicksPerDay);
        }

        // Return the tick count corresponding to the given hour, minute, second.
        // Will check the if the parameters are valid.
        internal static long TimeToTicks(int hour, int minute, int second, int millisecond)
        {
            //TimeSpan.TimeToTicks is a family access function which does no error checking, so
            //we need to put some error checking out here.
            if (hour >= 0 && hour < 24 && minute >= 0 && minute < 60 && second >=0 && second < 60)
            {
                if (millisecond < 0 || millisecond >= MillisPerSecond) {
                    throw new ArgumentOutOfRangeException(
                                "millisecond", 
                                String.Format(
                                    CultureInfo.CurrentCulture,
                                    Environment.GetResourceString("ArgumentOutOfRange_Range"), 
                                    0, 
                                    MillisPerSecond - 1));
                }                    
                return (TimeSpan.TimeToTicks(hour, minute, second) + millisecond * TicksPerMillisecond);;
            }
            throw new ArgumentOutOfRangeException(null, Environment.GetResourceString("ArgumentOutOfRange_BadHourMinuteSecond"));
        }
        

        internal void CheckTicksRange(long ticks) {
            if (ticks < m_Cal.MinSupportedDateTime.Ticks || ticks > m_Cal.MaxSupportedDateTime.Ticks) {
                throw new ArgumentOutOfRangeException(
                            "time", 
                            String.Format(
                                CultureInfo.InvariantCulture, 
                                Environment.GetResourceString("ArgumentOutOfRange_CalendarRange"), 
                                m_Cal.MinSupportedDateTime,
                                m_Cal.MaxSupportedDateTime));
            }
        }

        // Returns the DateTime resulting from adding the given number of
        // months to the specified DateTime. The result is computed by incrementing
        // (or decrementing) the year and month parts of the specified DateTime by
        // value months, and, if required, adjusting the day part of the
        // resulting date downwards to the last day of the resulting month in the
        // resulting year. The time-of-day part of the result is the same as the
        // time-of-day part of the specified DateTime.
        //
        // In more precise terms, considering the specified DateTime to be of the
        // form y / m / d + t, where y is the
        // year, m is the month, d is the day, and t is the
        // time-of-day, the result is y1 / m1 / d1 + t,
        // where y1 and m1 are computed by adding value months
        // to y and m, and d1 is the largest value less than
        // or equal to d that denotes a valid day in month m1 of year
        // y1.
        //
        public DateTime AddMonths(DateTime time, int months) 
        {
            CheckTicksRange(time.Ticks);
            if (months < -120000 || months > 120000) {
                throw new ArgumentOutOfRangeException(
                            "months", 
                            String.Format(
                                CultureInfo.CurrentCulture,
                                Environment.GetResourceString("ArgumentOutOfRange_Range"), 
                                -120000, 
                                120000));
            }
            int y = GetDatePart(time.Ticks, DatePartYear);
            int m = GetDatePart(time.Ticks, DatePartMonth);
            int d = GetDatePart(time.Ticks, DatePartDay);
            int i = m - 1 + months;
            if (i >= 0) 
            {
                m = i % 12 + 1;
                y = y + i / 12;
            }
            else 
            {
                m = 12 + (i + 1) % 12;
                y = y + (i - 11) / 12;
            }
            int[] daysArray = (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0)) ? DaysToMonth366: DaysToMonth365;
            int days = (daysArray[m] - daysArray[m - 1]); 
            
            if (d > days) 
            {
                d = days;
            }
            long ticks = DateToTicks(y, m, d) + (time.Ticks % TicksPerDay);
            Calendar.CheckAddResult(ticks, m_Cal.MinSupportedDateTime, m_Cal.MaxSupportedDateTime);
            return (new DateTime(ticks));
        }
            
        // Returns the DateTime resulting from adding the given number of
        // years to the specified DateTime. The result is computed by incrementing
        // (or decrementing) the year part of the specified DateTime by value
        // years. If the month and day of the specified DateTime is 2/29, and if the
        // resulting year is not a leap year, the month and day of the resulting
        // DateTime becomes 2/28. Otherwise, the month, day, and time-of-day
        // parts of the result are the same as those of the specified DateTime.
        //
        public DateTime AddYears(DateTime time, int years) 
        {
            return (AddMonths(time, years * 12));
        }
    
        // Returns the day-of-month part of the specified DateTime. The returned
        // value is an integer between 1 and 31.
        //
        public int GetDayOfMonth(DateTime time)
        {
            return (GetDatePart(time.Ticks, DatePartDay));
        }
    
        // Returns the day-of-week part of the specified DateTime. The returned value
        // is an integer between 0 and 6, where 0 indicates Sunday, 1 indicates
        // Monday, 2 indicates Tuesday, 3 indicates Wednesday, 4 indicates
        // Thursday, 5 indicates Friday, and 6 indicates Saturday.
        //
        public DayOfWeek GetDayOfWeek(DateTime time) 
        {
            CheckTicksRange(time.Ticks);
            return ((DayOfWeek)((time.Ticks / TicksPerDay + 1) % 7));
        }
    
        // Returns the day-of-year part of the specified DateTime. The returned value
        // is an integer between 1 and 366.
        //
        public int GetDayOfYear(DateTime time)
        {
            return (GetDatePart(time.Ticks, DatePartDayOfYear));
        }

        // Returns the number of days in the month given by the year and
        // month arguments.
        //
        public int GetDaysInMonth(int year, int month, int era) {
            //
            // Convert year/era value to Gregorain year value.
            //
            year = GetGregorianYear(year, era);
            if (month < 1 || month > 12) {
                throw new ArgumentOutOfRangeException("month", Environment.GetResourceString("ArgumentOutOfRange_Month"));
            }
            int[] days = ((year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? DaysToMonth366: DaysToMonth365);
            return (days[month] - days[month - 1]);        
        }
    
        // Returns the number of days in the year given by the year argument for the current era.
        //

        public int GetDaysInYear(int year, int era)
        {
            //
            // Convert year/era value to Gregorain year value.
            //
            year = GetGregorianYear(year, era);
            return ((year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 366:365);
        }
    
        // Returns the era for the specified DateTime value.
        public int GetEra(DateTime time)
        {
            long ticks = time.Ticks;
            // The assumption here is that m_EraInfo is listed in reverse order.
            for (int i = 0; i < m_EraInfo.Length; i++) {
                if (ticks >= m_EraInfo[i].ticks) {
                    return (m_EraInfo[i].era);
                }
            }
            throw new ArgumentOutOfRangeException(Environment.GetResourceString("ArgumentOutOfRange_Era"));
        }


        public int[] Eras {
            get {
                if (m_eras == null) {
                    m_eras = new int[m_EraInfo.Length];
                    for (int i = 0; i < m_EraInfo.Length; i++) {
                        m_eras[i] = m_EraInfo[i].era;
                    }
                }
                return ((int[])m_eras.Clone());
            }
        }
            
        // Returns the month part of the specified DateTime. The returned value is an
        // integer between 1 and 12.
        //
        public int GetMonth(DateTime time) 
        {
            return (GetDatePart(time.Ticks, DatePartMonth));
        }
    
        // Returns the number of months in the specified year and era.
        public int GetMonthsInYear(int year, int era)
        {
            year = GetGregorianYear(year, era);
            return (12);
        }
                
        // Returns the year part of the specified DateTime. The returned value is an
        // integer between 1 and 9999.
        //
        public int GetYear(DateTime time) 
        {
            long ticks = time.Ticks;
            int year = GetDatePart(ticks, DatePartYear);
            for (int i = 0; i < m_EraInfo.Length; i++) {
                if (ticks >= m_EraInfo[i].ticks) {
                    return (year - m_EraInfo[i].yearOffset);
                }
            }
            throw new ArgumentException(Environment.GetResourceString("Argument_NoEra"));
        }    
    
        // Returns the year that match the specified Gregorian year. The returned value is an
        // integer between 1 and 9999.
        //
        public int GetYear(int year, DateTime time) 
        {
            long ticks = time.Ticks;
            for (int i = 0; i < m_EraInfo.Length; i++) {
                if (ticks >= m_EraInfo[i].ticks) {
                    return (year - m_EraInfo[i].yearOffset);
                }
            }
            throw new ArgumentException(Environment.GetResourceString("Argument_NoEra"));
        }    

        // Checks whether a given day in the specified era is a leap day. This method returns true if
        // the date is a leap day, or false if not.
        //
        public bool IsLeapDay(int year, int month, int day, int era)
        {
            // year/month/era checking is done in GetDaysInMonth()
            if (day < 1 || day > GetDaysInMonth(year, month, era)) {
                throw new ArgumentOutOfRangeException(
                            "day", 
                            String.Format(
                                CultureInfo.CurrentCulture,
                                Environment.GetResourceString("ArgumentOutOfRange_Range"), 
                                1, 
                                GetDaysInMonth(year, month, era)));                
            }
            
            if (!IsLeapYear(year, era)) {
                return (false);
            }
            
            if (month == 2 && day == 29) {
                return (true);
            }
            
            return (false);            
        }
    
        // Returns  the leap month in a calendar year of the specified era. This method returns 0
        // if this calendar does not have leap month, or this year is not a leap year.
        //
        public int GetLeapMonth(int year, int era)
        {
            year = GetGregorianYear(year, era);
            return (0);	
        }

        // Checks whether a given month in the specified era is a leap month. This method returns true if
        // month is a leap month, or false if not.
        //
        public bool IsLeapMonth(int year, int month, int era)
        {
            year = GetGregorianYear(year, era);
            if (month < 1 || month > 12) {
                throw new ArgumentOutOfRangeException(
                            "month", 
                            String.Format(
                                CultureInfo.CurrentCulture,
                                Environment.GetResourceString("ArgumentOutOfRange_Range"), 
                                1, 
                                12));                
            }            
            return (false);        
        }
    
        // Checks whether a given year in the specified era is a leap year. This method returns true if
        // year is a leap year, or false if not.
        //
        public bool IsLeapYear(int year, int era) {
            year = GetGregorianYear(year, era);
            return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
        }
    
        // Returns the date and time converted to a DateTime value.  Throws an exception if the n-tuple is invalid.
        //
        public DateTime ToDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, int era) {
            year = GetGregorianYear(year, era);
            long ticks = DateToTicks(year, month, day) + TimeToTicks(hour, minute, second, millisecond);
            CheckTicksRange(ticks);
            return (new DateTime(ticks));
        }

        public virtual int GetWeekOfYear(DateTime time, CalendarWeekRule rule, DayOfWeek firstDayOfWeek) {
            CheckTicksRange(time.Ticks);
            // Use GregorianCalendar to get around the problem that the implmentation in Calendar.GetWeekOfYear()
            // can call GetYear() that exceeds the supported range of the Gregorian-based calendars.
            return (GregorianCalendar.GetDefaultInstance().GetWeekOfYear(time, rule, firstDayOfWeek));
        }


        public int ToFourDigitYear(int year, int twoDigitYearMax) {
            if (year < 0) {
                throw new ArgumentOutOfRangeException("year",
                    Environment.GetResourceString("ArgumentOutOfRange_NeedPosNum"));                
            }
            
            if (year < 100) {
                int y = year % 100;
                return ((twoDigitYearMax/100 - ( y > twoDigitYearMax % 100 ? 1 : 0))*100 + y);
            }
            
            if (year < m_minYear || year > m_maxYear) {
                throw new ArgumentOutOfRangeException(
                            "year",
                            String.Format(
                                CultureInfo.CurrentCulture,
                                Environment.GetResourceString("ArgumentOutOfRange_Range"), m_minYear, m_maxYear));
            }
            // If the year value is above 100, just return the year value.  Don't have to do
            // the TwoDigitYearMax comparison.
            return (year);
        }
    }        
}

