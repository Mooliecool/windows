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

namespace Microsoft.JScript {

    using System;
    using System.Globalization;
    using System.Reflection;
    using System.Text;

    enum Ps{Initial, Minutes, Seconds, AddOffset, SubOffset, Date, Year};
    enum Tk{Dummy, BcAd, AmPm, Zone, Day, Month};

    public class DatePrototype : DateObject{
      internal static readonly DatePrototype ob = new DatePrototype(ObjectPrototype.ob);
      internal static DateConstructor _constructor;

      internal DatePrototype(ObjectPrototype parent)
        : base(parent, 0) {
        this.noExpando = true;
      }

      //////////////////////////////Utility funcs//////////////////////////////////////////////////////

      private const double HoursPerDay = 24;
      private const double MinutesPerHour = 60;
      private const double SecondsPerMinute = 60;
      private const double msPerSecond = 1000;
      private const double msPerMinute = msPerSecond * SecondsPerMinute;
      private const double msPerHour = msPerMinute * MinutesPerHour;
      private const double msPerDay = msPerHour * HoursPerDay;
      internal const double msTo1970 = msPerDay * (365 * 1969 + 1968 / 4 - 1900 / 100 + 1600 / 400);
      internal const double ticksPerMillisecond = 10000;
      internal const double maxDate = (long)(8.64e15);
      internal const double minDate = (long)(-8.64e15);

      private static readonly int[] daysToMonthEnd     = new int[] {31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
      private static readonly int[] leapDaysToMonthEnd = new int[] {31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};

      private static double Day(double time){
        return Math.Floor(time / msPerDay);
      }

      private static double TimeWithinDay(double time){
        double result = time % msPerDay;
        if (result < 0) result += msPerDay;
        return result;
      }

      private static int DaysInYear(double year){
        if (year %   4 != 0.0) return 365;
        if (year % 100 != 0.0) return 366;
        if (year % 400 != 0.0) return 365;
        return 366;
      }

      private static double DayFromYear(double year){
        return 365*(year-1970) + Math.Floor((year-1969)/4) - Math.Floor((year-1901)/100) + Math.Floor((year-1601)/400);
      }

      private static double TimeFromYear(double year){
        return msPerDay * DayFromYear(year);
      }

      private static double YearFromTime(double time){
        double day = Math.Floor(time / msPerDay);
        double year = 1970 + Math.Floor((400 * day + 398) / 146097); //gives us the right year, or right year + 1
        double dfy = DayFromYear(year);
        if (day < dfy){ //The first day of year is beyond day, so we have right year + 1
          year -= 1;
        }
        return year;
      }

      private static bool InLeapYear(double year){
        if (year %   4 != 0.0) return false;
        if (year % 100 != 0.0) return true;
        if (year % 400 != 0.0) return false;
        return true;
      }

      private static int MonthFromTime(double time){
        int month = 0;
        int day = DayWithinYear(time)+1;
        if (InLeapYear(YearFromTime(time)))
          while (day > leapDaysToMonthEnd[month]) month++;
        else
          while (day > daysToMonthEnd[month]) month++;
        return month;
      }

      private static int DayWithinYear(double time){
        return (int)(Day(time) - DayFromYear(YearFromTime(time)));
      }

      private static int DateFromTime(double time){
        int month = 0;
        int day = DayWithinYear(time)+1;
        if (day <= 31)
          return day;
        if (InLeapYear(YearFromTime(time))){
          while (day > leapDaysToMonthEnd[month]) month++;
          return day - leapDaysToMonthEnd[month-1];
        }else{
          while (day > daysToMonthEnd[month]) month++;
          return day - daysToMonthEnd[month-1];
        }
      }

      private static int WeekDay(double time){
        double result = (Day(time) + 4) % 7;
        if (result < 0) result += 7;
        return (int)result;
      }

      private static bool DaylightSavingsTime(double localTime) {
        if (!useDST)
          return false;
        double t = (localTime + msTo1970) * ticksPerMillisecond;
        if ((double)Int64.MinValue <= t && t <= (double)Int64.MaxValue)
          try {
            DateTime dt = new DateTime((long)t);
            return System.TimeZone.CurrentTimeZone.IsDaylightSavingTime(dt);
          }catch (System.ArgumentOutOfRangeException) { }

        int month = MonthFromTime(localTime);
        if (month < 3 || month > 9) return false;
        if (month > 3 && month < 9) return true;
        int date = DateFromTime(localTime);
        if (month == 3) {
          if (date > 7) return true;
          int weekDay = WeekDay(localTime);
          if (weekDay > 0)
            return date > weekDay;
          return HourFromTime(localTime) > 1;
        } else {
          if (date < 25) return true;
          int weekDay = WeekDay(localTime);
          if (weekDay > 0)
            return date - weekDay < 25;
          return HourFromTime(localTime) < 1;
        }
      }

      private static readonly double localStandardTZA;
      private static readonly double localDaylightTZA;
      private static readonly bool   useDST;

      static DatePrototype(){
        DateTime jan1ThisYear = new DateTime(DateTime.Now.Year, 1, 1);
        double jantimeoffset =  (jan1ThisYear.Ticks - jan1ThisYear.ToUniversalTime().Ticks) / ticksPerMillisecond;
        DateTime july1ThisYear = new DateTime(DateTime.Now.Year, 7, 1);
        double julytimeoffset = (july1ThisYear.Ticks - july1ThisYear.ToUniversalTime().Ticks) / ticksPerMillisecond;
        if (jantimeoffset < julytimeoffset){
          localStandardTZA = jantimeoffset;
          localDaylightTZA = julytimeoffset;
        } else{
          localStandardTZA = julytimeoffset;
          localDaylightTZA = jantimeoffset;
        }
        useDST = localStandardTZA != localDaylightTZA;
      }

      private static double LocalTime(double utcTime){
        return utcTime + (DaylightSavingsTime(utcTime + localStandardTZA)
          ? localDaylightTZA : localStandardTZA);
      }

      internal static double UTC(double localTime){
        return localTime - (DaylightSavingsTime(localTime)
          ? localDaylightTZA : localStandardTZA);
      }

      private static int HourFromTime(double time){
        double result = Math.Floor(time / msPerHour) % HoursPerDay;
        if (result < 0) result += HoursPerDay;
        return (int)result;
      }

      private static int MinFromTime(double time){
        double result = Math.Floor(time / msPerMinute) % MinutesPerHour;
        if (result < 0) result += MinutesPerHour;
        return (int)result;
      }

      private static int SecFromTime(double time){
        double result = Math.Floor(time / msPerSecond) % SecondsPerMinute;
        if (result < 0) result += SecondsPerMinute;
        return (int)result;
      }

      private static int msFromTime(double time){
        double result = time % msPerSecond;
        if (result < 0) result += msPerSecond;
        return (int)result;
      }

      internal static double MakeTime(double hour, double min, double sec, double ms){
        if (Double.IsInfinity(hour) || Double.IsInfinity(min) || Double.IsInfinity(sec) || Double.IsInfinity(ms) ||
            hour != hour || min != min || sec != sec || ms != ms)
          return Double.NaN;
        hour = (double)(int)Runtime.DoubleToInt64(hour);
        min = (double)(int)Runtime.DoubleToInt64(min);
        sec = (double)(int)Runtime.DoubleToInt64(sec);
        ms = (double)(int)Runtime.DoubleToInt64(ms);
        return hour*msPerHour + min*msPerMinute + sec*msPerSecond + ms;
      }

      internal static double MakeDay(double year, double month, double date){
        if (Double.IsInfinity(year) || Double.IsInfinity(month) || Double.IsInfinity(date) || year != year || month != month || date != date)
          return Double.NaN;
        year = (double)(int)Runtime.DoubleToInt64(year);
        month = (double)(int)Runtime.DoubleToInt64(month);
        date = (double)(int)Runtime.DoubleToInt64(date);
        year += Math.Floor(month/12);
        month = month % 12;
        if (month < 0) month += 12;
        double daysToMonthStart = 0;
        if (month > 0)
          if (InLeapYear((int)Runtime.DoubleToInt64(year)))
            daysToMonthStart = leapDaysToMonthEnd[(int)(month-1)];
          else
            daysToMonthStart = daysToMonthEnd[(int)(month-1)];
        return DayFromYear(year)-1 + daysToMonthStart + date;
      }

      internal static double MakeDate(double day, double time){
        if (Double.IsInfinity(day) || Double.IsInfinity(time))
          return Double.NaN;
        return day*msPerDay + time;
      }

      internal static double TimeClip(double time){
        if (Double.IsInfinity(time))
          return Double.NaN;
        if (minDate <= time && time <= maxDate)
          return (double)(long)time;
        else
          return Double.NaN;
      }

      internal static String DateToLocaleDateString(double time){
        if (Double.IsNaN(time)) return "NaN";
        StringBuilder result = new StringBuilder();
        int month = MonthFromTime(time)+1;
        if (month < 10)
          result.Append("0");
        result.Append(month);
        result.Append("/");
        int date = DateFromTime(time);
        if (date < 10)
          result.Append("0");
        result.Append(date);
        result.Append("/");
        result.Append(YearString(time));
        return result.ToString();
      }

      internal static String DateToLocaleString(double time){
        if (Double.IsNaN(time)) return "NaN";
        StringBuilder result = new StringBuilder();
        int month = MonthFromTime(time)+1;
        if (month < 10)
          result.Append("0");
        result.Append(month);
        result.Append("/");
        int date = DateFromTime(time);
        if (date < 10)
          result.Append("0");
        result.Append(date);
        result.Append("/");
        result.Append(YearString(time));
        result.Append(" ");
        AppendTime(time, result);
        return result.ToString();
      }

      internal static String DateToLocaleTimeString(double time){
        if (Double.IsNaN(time)) return "NaN";
        StringBuilder result = new StringBuilder();
        AppendTime(time, result);
        return result.ToString();
      }

      static void AppendTime(double time, StringBuilder sb){
        int hour = HourFromTime(time);
        if (hour < 10)
          sb.Append("0");
        sb.Append(hour);
        sb.Append(":");
        int minute = MinFromTime(time);
        if (minute < 10)
          sb.Append("0");
        sb.Append(minute);
        sb.Append(":");
        int second = SecFromTime(time);
        if (second < 10)
          sb.Append("0");
        sb.Append(second);
      }

      static String TimeZoneID(double utcTime) {
        int tz = (int)(localStandardTZA / msPerHour);
        if (DaylightSavingsTime(utcTime + localStandardTZA))
          switch (tz){
            case -5 : return "EDT";
            case -6 : return "CDT";
            case -7 : return "MDT";
            case -8 : return "PDT";
          }
        else
          switch (tz){
            case -5 : return "EST";
            case -6 : return "CST";
            case -7 : return "MST";
            case -8 : return "PST";
          }
        return (tz >= 0 ? "UTC+" : "UTC") + tz.ToString(CultureInfo.InvariantCulture);
      }

      static String YearString(double time){
        double year = YearFromTime(time);
        if (year > 0)
          return year.ToString(CultureInfo.InvariantCulture);
        else{
          return (1-year).ToString(CultureInfo.InvariantCulture) + " B.C.";
        }
      }


      static readonly String[] WeekDayName = new String[]{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
      static readonly String[] MonthName = new String[]{"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

      internal static String DateToDateString(double utcTime){
        if (Double.IsNaN(utcTime)) return "NaN";
        StringBuilder result = new StringBuilder();
        double localTime = LocalTime(utcTime);
        result.Append(WeekDayName[WeekDay(localTime)]);
        result.Append(" ");
        int month = MonthFromTime(localTime);
        result.Append(MonthName[month]);
        result.Append(" ");
        result.Append(DateFromTime(localTime));
        result.Append(" ");
        result.Append(YearString(localTime));
        return result.ToString();
      }

      internal static String DateToString(double utcTime){
        if (Double.IsNaN(utcTime)) return "NaN";
        StringBuilder result = new StringBuilder();
        double localTime = LocalTime(utcTime);
        result.Append(WeekDayName[WeekDay(localTime)]);
        result.Append(" ");
        int month = MonthFromTime(localTime);
        result.Append(MonthName[month]);
        result.Append(" ");
        result.Append(DateFromTime(localTime));
        result.Append(" ");
        AppendTime(localTime, result);
        result.Append(" ");
        result.Append(TimeZoneID(utcTime));
        result.Append(" ");
        result.Append(YearString(localTime));
        return result.ToString();
      }

      internal static String DateToTimeString(double utcTime){
        if (Double.IsNaN(utcTime)) return "NaN";
        StringBuilder result = new StringBuilder();
        double localTime = LocalTime(utcTime);
        AppendTime(localTime, result);
        result.Append(" ");
        result.Append(TimeZoneID(utcTime));
        return result.ToString();
      }

      internal static String UTCDateToString(double utcTime){
        if (Double.IsNaN(utcTime)) return "NaN";
        StringBuilder result = new StringBuilder();
        result.Append(WeekDayName[WeekDay(utcTime)]);
        result.Append(", ");
        result.Append(DateFromTime(utcTime));
        result.Append(" ");
        result.Append(MonthName[MonthFromTime(utcTime)]);
        result.Append(" ");
        result.Append(YearString(utcTime));
        result.Append(" ");
        AppendTime(utcTime, result);
        result.Append(" UTC");
        return result.ToString();
      }

      static bool NotSpecified(Object value){
        return value == null || value is Missing;
      }

      static bool isASCII(char ch){
        return ((int)ch) < 128;
      }

      static bool isalpha(char ch){
        return 'A' <= ch && ch <= 'Z' || 'a' <= ch && ch <= 'z';
      }

      static bool isdigit(char ch){
        return '0' <= ch && ch <= '9';
      }

      static readonly String[] Strings =
        new String[]{"bc", "b.c", "ad", "a.d", "am", "a.m", "pm", "p.m",
                     "est", "edt", "cst", "cdt", "mst", "mdt", "pst", "pdt", "gmt", "utc",
                     "sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday",
                     "january", "february", "march", "april", "may", "june", "july", "august", "september", "october", "november", "december"};

      static readonly Tk[] Tokens =
        new Tk[]{Tk.BcAd, Tk.BcAd, Tk.BcAd, Tk.BcAd, Tk.AmPm, Tk.AmPm, Tk.AmPm, Tk.AmPm,
                 Tk.Zone, Tk.Zone, Tk.Zone, Tk.Zone, Tk.Zone, Tk.Zone, Tk.Zone, Tk.Zone, Tk.Zone, Tk.Zone,
                 Tk.Day, Tk.Day, Tk.Day, Tk.Day, Tk.Day, Tk.Day, Tk.Day,
                 Tk.Month,  Tk.Month, Tk.Month, Tk.Month, Tk.Month, Tk.Month, Tk.Month, Tk.Month, Tk.Month, Tk.Month, Tk.Month, Tk.Month};

      static readonly int[] Values =
        new int[]{-1, -1, +1, +1, -1, -1, +1, +1,
                  -5 * 60,  -4 * 60, -6 * 60, -5 * 60, -7 * 60, -6 * 60, -8 * 60,  -7 * 60, 0, 0,
                  0, 1, 2, 3, 4, 5, 6,
                  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      internal static double ParseDate(String str){
        long Nil = 0x80000000;

        int AmPm = 0;
        int BcAd = 0;
        Ps ps = Ps.Initial;
        long Year = Nil;
        long Month = Nil;
        long Date = Nil;
        long Time = Nil;
        long Zone = Nil;
        long Offset = Nil;

        str = str.ToLowerInvariant();

        int i = 0, n = str.Length;
        while (i < n){
          char ch = str[i++];
          if (ch <= ' ')
            continue;
          switch (ch){
            case '(':
              // skip stuff in parens
              for (int depth = 1; i < n; ){
                ch = str[i++];
                if (ch == '(')
                  depth++;
                else if (ch == ')' && --depth <= 0)
                  break;
              }
              continue;
            case ',':
              continue;
            case ':':
              continue;
            case '/':
              continue;
            case '+':
              if (Nil != Year)
                ps = Ps.AddOffset;
              continue;
            case '-':
              if (Nil != Year)
                ps = Ps.SubOffset;
              continue;
            default:
              break;
          }

          if (isalpha(ch)){  //scan an alpha token
            int j = i-1; //remember start of token
            while (i < n){
              ch = str[i++];
              if (isalpha(ch) || '.' == ch) continue;
              break;
            }

            int cch = i - j - (i < n ? 1 : 0);
            if ('.' == str[i-(i < n ? 2 : 1)]) //Exclude a trailing dot from the token
              cch--;

            // skip over any spaces
            while (ch == ' ' && i < n) ch = str[i++];

            // have an alphabetic token - look it up

            if (1 == cch){
              // military version of time zone
              // z = GMT
              // j isn't used
              // a to m are -1 to -12
              // n to y are 1 to 12
              if (Nil != Zone)
                return Double.NaN;
              char chj = str[j];
              if (chj <= 'm'){
                if (chj == 'j' || chj < 'a')
                  return Double.NaN;
                Zone = -(long)(chj - 'a' + (chj < 'j'?1:0)) * 60;
              }else if (chj <= 'y')
                Zone = (long)(chj - 'm') * 60;
              else if (chj == 'z')
                Zone = 0;
              else
                return Double.NaN;

              // look for a time zone offset
              if ('+' == ch){
                ps = Ps.AddOffset;
              }else if ('-' == ch){
                ps = Ps.SubOffset;
              }else
                ps = Ps.Initial;

              continue;
            }

            for (int kk = Strings.Length-1; kk >= 0; kk--){
              String tkstr = Strings[kk];
              if (tkstr.Length < cch) continue;
              if (0 != String.CompareOrdinal(str, j, tkstr, 0, cch)){
                if (kk == 0) return Double.NaN; //The current token does not match anything. Date string cannot be parsed by this routine.
              }else{
                switch (Tokens[kk]){
                  case Tk.BcAd :
                    if (0 != BcAd) return Double.NaN;
                    BcAd = Values[kk];
                    break;
                  case Tk.AmPm :
                    if (0 != AmPm) return Double.NaN;
                    AmPm = Values[kk];
                    break;
                  case Tk.Month :
                    if (Nil != Month) return Double.NaN;
                    Month = Values[kk];
                    break;
                  case Tk.Zone :
                    if (Nil != Zone) return Double.NaN;
                    Zone = Values[kk];

                    // look for a time zone offset
                    if ('+' == ch){
                      ps = Ps.AddOffset; i++;
                    }else if ('-' == ch){
                      ps = Ps.SubOffset; i++;
                    }else
                      ps = Ps.Initial;
                    break;
                }
                break; //out of the loop matching the current token with known tokens
              }
            }
            if (i < n) i--;
            continue; //with overall loop, looking for next bit of date
          }

          if (!isdigit(ch))
            return Double.NaN;

          int T = 0, k = i;
          do{
            T = T * 10 + ch - '0';
            if (i >= n) break;
            ch = str[i++];
          }while (isdigit(ch));

          // to avoid overflow
          if (i - k > 6)
            return Double.NaN;

          // skip over any spaces
          while (ch == ' ' && i < n) ch = str[i++];

          switch (ps){
            case Ps.AddOffset:
              if (Nil != Offset)
                return Double.NaN;
              Offset = T < 24 ? T * 60 : (T % 100) + (T / 100) * 60; //> 24 implies hhmm
              ps = Ps.Initial;
              if (i < n) i--;
              break;

            case Ps.SubOffset:
              if (Nil != Offset)
                return Double.NaN;
              Offset = T < 24 ? -T * 60 : -((T % 100) + (T / 100) * 60); //> 24 implies hhmm
              ps = Ps.Initial;
              if (i < n) i--;
              break;

            case Ps.Minutes:
              if (T >= 60)
                return Double.NaN;
              Time += T * 60;
              if (ch == ':'){
                ps = Ps.Seconds;
              }else{
                ps = Ps.Initial;
                if (i < n) i--;
              }
              break;

            case Ps.Seconds:
              if (T >= 60)
                return Double.NaN;
              Time += T;
              ps = Ps.Initial;
              if (i < n) i--;
              break;

            case Ps.Date:
              if (Nil != Date)
                return Double.NaN;
              Date = T;
              if ('/' == ch || '-' == ch){
                ps = Ps.Year;
              }else{
                ps = Ps.Initial;
                if (i < n) i--;
              }
              break;

            case Ps.Year:
              if (Nil != Year)
                return Double.NaN;
              Year = T;
              ps = Ps.Initial;
              if (i < n) i--;
              break;

            default:
              //Assert(ps == Ps.Initial);
              if (T >= 70){
                // assume it's a year
                if (Nil != Year)
                  return Double.NaN;
                Year = T;
                if (i < n) i--;
                break;
              }

              switch (ch){
                case ':':
                  // hour
                  if (Nil != Time)
                    return Double.NaN;
                  if (T >= 24)
                    return Double.NaN;
                  Time = T * 3600;
                  ps = Ps.Minutes;
                  break;

                case '/': goto case '-';

                case '-':
                  // month
                  if (Nil != Month)
                    return Double.NaN;
                  Month = T - 1;
                  ps = Ps.Date;
                  break;

                default:
                  // date
                  if (Nil != Date)
                    return Double.NaN;
                  Date = T;
                  if (i < n) i--;
                  break;
              }
            break;
          }
        }

        if (Nil == Year || Nil == Month || Nil == Date)
          return Double.NaN;

        if (0 != BcAd){
          if (BcAd < 0)
            // BC. Note that 1 BC is year 0 and 2 BC is year -1.
            Year = -Year + 1;
        }else if (Year < 100)
          Year += 1900;

        if (0 != AmPm){
          if (Nil == Time)
            return Double.NaN;
          if (Time >= 12 * 3600 && Time < 13 * 3600){
            // In the 12:00 hour. AM means subtract 12 hours and PM means
            // do nothing.
            if (AmPm < 0)
              Time -= 12 * 3600;
          }else{
            // Not in the 12:00 hour. AM means do nothing and PM means
            // add 12 hours.
            if (AmPm > 0){
              if (Time >= 12 * 3600)
                return Double.NaN;
              Time += 12 * 3600;
            }
          }
        }else if (Nil == Time)
          Time = 0;

        bool Utc = false;
        if (Nil != Zone){
          Time -= Zone * 60;
          Utc = true;
        }
        if (Nil != Offset)
          Time -= Offset * 60;

        // Rebuild time.
        double result = MakeDate(MakeDay(Year, Month, Date), Time*1000);
        if (!Utc)
          result = UTC(result);
        return result;
      }

      ////////////////////////////////public funcs visible from scripts//////////////////////////////////

      public static DateConstructor constructor{
        get{
          return DatePrototype._constructor;
        }
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_getDate)]
      public static double getDate(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        if (t != t) return t;
        return DateFromTime(LocalTime(t));
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_getDay)]
      public static double getDay(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        if (t != t) return t;
        return WeekDay(LocalTime(t));
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_getFullYear)]
      public static double getFullYear(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        if (t != t) return t;
        return YearFromTime(LocalTime(t));
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_getHours)]
      public static double getHours(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        if (t != t) return t;
        return HourFromTime(LocalTime(t));
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_getMilliseconds)]
      public static double getMilliseconds(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        if (t != t) return t;
        return msFromTime(LocalTime(t));
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_getMinutes)]
      public static double getMinutes(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        if (t != t) return t;
        return MinFromTime(LocalTime(t));
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_getMonth)]
      public static double getMonth(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        if (t != t) return t;
        return MonthFromTime(LocalTime(t));
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_getSeconds)]
      public static double getSeconds(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        if (t != t) return t;
        return SecFromTime(LocalTime(t));
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_getTime)]
      public static double getTime(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        return ((DateObject)thisob).value;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_getTimezoneOffset)]
      public static double getTimezoneOffset(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        if (t != t) return t;
        return (t-LocalTime(t)) / msPerMinute;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_getUTCDate)]
      public static double getUTCDate(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        if (t != t) return t;
        return DateFromTime(t);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_getUTCDay)]
      public static double getUTCDay(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        if (t != t) return t;
        return WeekDay(t);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_getUTCFullYear)]
      public static double getUTCFullYear(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        if (t != t) return t;
        return YearFromTime(t);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_getUTCHours)]
      public static double getUTCHours(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        if (t != t) return t;
        return HourFromTime(t);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_getUTCMilliseconds)]
      public static double getUTCMilliseconds(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        if (t != t) return t;
        return msFromTime(t);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_getUTCMinutes)]
      public static double getUTCMinutes(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        if (t != t) return t;
        return MinFromTime(t);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_getUTCMonth)]
      public static double getUTCMonth(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        if (t != t) return t;
        return MonthFromTime(t);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_getUTCSeconds)]
      public static double getUTCSeconds(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        if (t != t) return t;
        return SecFromTime(t);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_getVarDate)]
      public static Object getVarDate(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        if (t != t)
          return null;
        long it;
        checked {
          try {
            it = (long)(LocalTime(t) + msTo1970) * (long)ticksPerMillisecond;
          } catch (System.OverflowException) {
            return null;
          }
        }
        if (it < DateTime.MinValue.Ticks || it > DateTime.MaxValue.Ticks)
          return null;
        DateTime dt;
        try {
          dt = new DateTime(it);
        } catch (System.ArgumentOutOfRangeException) {
          return null;
        }
        return dt;
      }

      [NotRecommended ("getYear")]
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_getYear)]
      public static double getYear(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        if (t != t) return t;
        double year = YearFromTime(LocalTime(t));
        if (1900 <= year && year <= 1999)
          return year - 1900;
        else
          return year;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_setDate)]
      public static double setDate(Object thisob, double ddate){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = LocalTime(((DateObject)thisob).value);
        double dday = MakeDay(YearFromTime(t), MonthFromTime(t), ddate);
        t = TimeClip(UTC(MakeDate(dday, TimeWithinDay(t))));
        ((DateObject)thisob).value = t;
        return t;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_setFullYear)]
      public static double setFullYear(Object thisob, double dyear, Object month, Object date){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = LocalTime(((DateObject)thisob).value);
        double dmonth = NotSpecified(month) ? MonthFromTime(t) : Convert.ToNumber(month);
        double ddate = NotSpecified(date) ? DateFromTime(t) : Convert.ToNumber(date);
        double day = MakeDay(dyear, dmonth, ddate);
        t = TimeClip(UTC(MakeDate(day, TimeWithinDay(t))));
        ((DateObject)thisob).value = t;
        return t;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_setHours)]
      public static double setHours(Object thisob, double dhour, Object min, Object sec, Object msec){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = LocalTime(((DateObject)thisob).value);
        double dmin = NotSpecified(min) ? MinFromTime(t) : Convert.ToNumber(min);
        double dsec = NotSpecified(sec) ? SecFromTime(t) : Convert.ToNumber(sec);
        double dmsec = NotSpecified(msec) ? msFromTime(t) : Convert.ToNumber(msec);
        t = TimeClip(UTC(MakeDate(Day(t), MakeTime(dhour, dmin, dsec, dmsec))));
        ((DateObject)thisob).value = t;
        return t;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_setMinutes)]
      public static double setMinutes(Object thisob, double dmin, Object sec, Object msec){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = LocalTime(((DateObject)thisob).value);
        double dsec = NotSpecified(sec) ? SecFromTime(t) : Convert.ToNumber(sec);
        double dmsec = NotSpecified(msec) ? msFromTime(t) : Convert.ToNumber(msec);
        t = TimeClip(UTC(MakeDate(Day(t), MakeTime(HourFromTime(t), dmin, dsec, dmsec))));
        ((DateObject)thisob).value = t;
        return t;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_setMilliseconds)]
      public static double setMilliseconds(Object thisob, double dmsec){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = LocalTime(((DateObject)thisob).value);
        t = TimeClip(UTC(MakeDate(Day(t), MakeTime(HourFromTime(t), MinFromTime(t), SecFromTime(t), dmsec))));
        ((DateObject)thisob).value = t;
        return t;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_setMonth)]
      public static double setMonth(Object thisob, double dmonth, Object date){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = LocalTime(((DateObject)thisob).value);
        double ddate = NotSpecified(date) ? DateFromTime(t) : Convert.ToNumber(date);
        double day = MakeDay(YearFromTime(t), dmonth, ddate);
        t = TimeClip(UTC(MakeDate(day, TimeWithinDay(t))));
        ((DateObject)thisob).value = t;
        return t;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_setSeconds)]
      public static double setSeconds(Object thisob, double dsec, Object msec){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = LocalTime(((DateObject)thisob).value);
        double dmsec = NotSpecified(msec) ? msFromTime(t) : Convert.ToNumber(msec);
        t = TimeClip(UTC(MakeDate(Day(t), MakeTime(HourFromTime(t), MinFromTime(t), dsec, dmsec))));
        ((DateObject)thisob).value = t;
        return t;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_setTime)]
      public static double setTime(Object thisob, double time){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        time = TimeClip(time);
        ((DateObject)thisob).value = time;
        return time;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_setUTCDate)]
      public static double setUTCDate(Object thisob, double ddate){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        double day = MakeDay(YearFromTime(t), MonthFromTime(t), ddate);
        t = TimeClip(MakeDate(day, TimeWithinDay(t)));
        ((DateObject)thisob).value = t;
        return t;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_setUTCFullYear)]
      public static double setUTCFullYear(Object thisob, double dyear, Object month, Object date){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        double dmonth = NotSpecified(month) ? MonthFromTime(t) : Convert.ToNumber(month);
        double ddate = NotSpecified(date) ? DateFromTime(t) : Convert.ToNumber(date);
        double day = MakeDay(dyear, dmonth, ddate);
        t = TimeClip(MakeDate(day, TimeWithinDay(t)));
        ((DateObject)thisob).value = t;
        return t;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_setUTCHours)]
      public static double setUTCHours(Object thisob, double dhour, Object min, Object sec, Object msec){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        double dmin = NotSpecified(min) ? MinFromTime(t) : Convert.ToNumber(min);
        double dsec = NotSpecified(sec) ? SecFromTime(t) : Convert.ToNumber(sec);
        double dmsec = NotSpecified(msec) ? msFromTime(t) : Convert.ToNumber(msec);
        t = TimeClip(MakeDate(Day(t), MakeTime(dhour, dmin, dsec, dmsec)));
        ((DateObject)thisob).value = t;
        return t;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_setUTCMinutes)]
      public static double setUTCMinutes(Object thisob, double dmin, Object sec, Object msec){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        double dsec = NotSpecified(sec) ? SecFromTime(t) : Convert.ToNumber(sec);
        double dmsec = NotSpecified(msec) ? msFromTime(t) : Convert.ToNumber(msec);
        t = TimeClip(MakeDate(Day(t), MakeTime(HourFromTime(t), dmin, dsec, dmsec)));
        ((DateObject)thisob).value = t;
        return t;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_setUTCMilliseconds)]
      public static double setUTCMilliseconds(Object thisob, double dmsec){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        t = TimeClip(MakeDate(Day(t), MakeTime(HourFromTime(t), MinFromTime(t), SecFromTime(t), dmsec)));
        ((DateObject)thisob).value = t;
        return t;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_setUTCMonth)]
      public static double setUTCMonth(Object thisob, double dmonth, Object date){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        double ddate = NotSpecified(date) ? DateFromTime(t) : Convert.ToNumber(date);
        double day = MakeDay(YearFromTime(t), dmonth, ddate);
        t = TimeClip(MakeDate(day, TimeWithinDay(t)));
        ((DateObject)thisob).value = t;
        return t;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_setUTCSeconds)]
      public static double setUTCSeconds(Object thisob, double dsec, Object msec){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        double dmsec = NotSpecified(msec) ? msFromTime(t) : Convert.ToNumber(msec);
        t = TimeClip(MakeDate(Day(t), MakeTime(HourFromTime(t), MinFromTime(t), dsec, dmsec)));
        ((DateObject)thisob).value = t;
        return t;
      }

      [NotRecommended("setYear")]
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_setYear)]
      public static double setYear(Object thisob, double dyear){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = LocalTime(((DateObject)thisob).value);
        if (Double.IsNaN(dyear)){
          ((DateObject)thisob).value = dyear;
          return dyear;
        }
        dyear = Convert.ToInteger(dyear);
        if (0 <= dyear && dyear <= 99)
          dyear += 1900;
        double day = MakeDay(dyear, MonthFromTime(t), DateFromTime(t));
        t = TimeClip(UTC(MakeDate(day, TimeWithinDay(t))));
        ((DateObject)thisob).value = t;
        return t;
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_toDateString)]
      public static String toDateString(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        return DateToDateString(t);
      }

      [NotRecommended ("toGMTString")]
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_toGMTString)]
      public static String toGMTString(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        return UTCDateToString(t);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_toLocaleDateString)]
      public static String toLocaleDateString(Object thisob){
        Object dt = getVarDate(thisob);
        return dt == null
          ? DateToDateString(((DateObject)thisob).value)
          : ((DateTime)dt).ToLongDateString();
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_toLocaleString)]
      public static String toLocaleString(Object thisob){
        Object dt = getVarDate(thisob);
        return dt == null
          ? DateToString(((DateObject)thisob).value)
          : ((DateTime)dt).ToLongDateString() + " " + ((DateTime)dt).ToLongTimeString();
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_toLocaleTimeString)]
      public static String toLocaleTimeString(Object thisob){
        Object dt = getVarDate(thisob);
        return dt == null
          ? DateToTimeString(((DateObject)thisob).value)
          : ((DateTime)dt).ToLongTimeString();
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_toString)]
      public static String toString(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        return DateToString(t);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_toTimeString)]
      public static String toTimeString(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        return DateToTimeString(t);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_toUTCString)]
      public static String toUTCString(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        double t = ((DateObject)thisob).value;
        return UTCDateToString(t);
      }

      [JSFunctionAttribute(JSFunctionAttributeEnum.HasThisObject, JSBuiltin.Date_valueOf)]
      public static double valueOf(Object thisob){
        if (!(thisob is DateObject))
          throw new JScriptException(JSError.DateExpected);
        return ((DateObject)thisob).value;
      }

    }
}
