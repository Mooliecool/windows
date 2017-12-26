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
using System;
using System.Collections;
using System.Globalization;
using System.IO;
using System.Text;
public class Co8571Parse_ifp_dts
{
 private Boolean runAllDates=false;
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "DateTime.Parse(String s, IFormatProvider provider, DateTimeStyle style)";
 public static String s_strTFName        = "Co8571Parse_ifp_dts.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   DateTime date;
   DateTime dateReturned;
   DateTime dateExpected;
   DateTime dateExpectedFromPattern;
   CultureInfo[] cultures;
   DateTimeStyles style;
   String s;
   Random random;
   Hashtable table;
   Char[] splittableChars = {' ', '/', ':'};
   Char ch1;
   String[] formats = {"d", "D", "f", "F", "g", "G", "m", "M", "r", "R", "s", "t", "T", "u", "U", "y", "Y"};
   Boolean fDoneAtLeastOnce;
   String[] dateInfoCustomFormats;
   String parseToBeString;
   TimeZone zone;
   TimeSpan span;
   int year,month,day,hour,minute,second,millisecond;
   DateTime tempDate;
   Int32 iNumOfDates;
   Calendar calendar = new GregorianCalendar();
   GregorianCalendar greg;
   Calendar[] calendars = {
     new GregorianCalendar(GregorianCalendarTypes.Arabic),
     new GregorianCalendar(GregorianCalendarTypes.Localized),
     new GregorianCalendar(GregorianCalendarTypes.MiddleEastFrench),
     new GregorianCalendar(GregorianCalendarTypes.TransliteratedEnglish),
     new GregorianCalendar(GregorianCalendarTypes.TransliteratedFrench),
     new GregorianCalendar(GregorianCalendarTypes.USEnglish),
     new HijriCalendar(),
     new TaiwanCalendar(),
     new JapaneseCalendar(),
     new KoreanCalendar(),
     new ThaiBuddhistCalendar(),
   };
   try
     {
     random = new Random();
     for(int iCalendarCount=0; iCalendarCount<calendars.Length; iCalendarCount++){
     calendar = calendars[iCalendarCount];
     strLoc = "Loc_384sdg";
     date=DateTime.MinValue;
     if(runAllDates)
       {
       iNumOfDates = 80*365+90;
       date = GetStartDate(calendar);
       }
     else
       iNumOfDates = 100;
     for(int noDates=0; noDates<iNumOfDates;noDates++)
       {
       strLoc = "Loc_451dsg";
       if(runAllDates)
	 {
	 date = calendar.AddDays(date,1);
	 }
       else
	 {
	 year = GetRandomYear(calendar, random);
	 month = random.Next(1, 12);
	 day = random.Next(1, 28);
	 hour = random.Next(0, 23);
	 minute = random.Next(0, 59);
	 second = random.Next(0, 59);
	 millisecond=random.Next(0, 999);
	 date = calendar.ToDateTime(year, month, day, hour, minute, second, millisecond);
	 }
       strLoc = "Loc_347tsg";
       zone = TimeZone.CurrentTimeZone;
       span = zone.GetUtcOffset(date);
       table = new Hashtable();
       table.Add("d", new DateTime(date.Year, date.Month, date.Day));
       table.Add("D", new DateTime(date.Year, date.Month, date.Day));
       table.Add("f", new DateTime(date.Year, date.Month, date.Day, date.Hour, date.Minute, 0));
       table.Add("F", new DateTime(date.Year, date.Month, date.Day, date.Hour, date.Minute, date.Second));
       table.Add("g", new DateTime(date.Year, date.Month, date.Day, date.Hour, date.Minute, 0));
       table.Add("G", new DateTime(date.Year, date.Month, date.Day, date.Hour, date.Minute, date.Second));
       table.Add("m", new DateTime(date.Year, date.Month, date.Day));
       table.Add("M", new DateTime(date.Year, date.Month, date.Day));
       tempDate = date.ToLocalTime();
       table.Add("r", new DateTime(tempDate.Year, tempDate.Month, tempDate.Day, tempDate.Hour, tempDate.Minute, tempDate.Second));
       table.Add("R", new DateTime(tempDate.Year, tempDate.Month, tempDate.Day, tempDate.Hour, tempDate.Minute, tempDate.Second));
       table.Add("s", new DateTime(date.Year, date.Month, date.Day, date.Hour, date.Minute, date.Second));
       table.Add("t", new DateTime(date.Year, date.Month, date.Day, date.Hour, date.Minute, 0));
       table.Add("T", new DateTime(date.Year, date.Month, date.Day, date.Hour, date.Minute, date.Second));
       table.Add("u", new DateTime(tempDate.Year, tempDate.Month, tempDate.Day, tempDate.Hour, tempDate.Minute, tempDate.Second));
       tempDate = date.ToUniversalTime();
       table.Add("U", new DateTime(tempDate.Year, tempDate.Month, tempDate.Day, tempDate.Hour, tempDate.Minute, tempDate.Second));
       table.Add("y", new DateTime(date.Year, date.Month, date.Day));
       table.Add("Y", new DateTime(date.Year, date.Month, date.Day));
       strLoc = "Loc_3497sdg";
       cultures = CultureInfo.GetCultures(CultureTypes.AllCultures);
       foreach(CultureInfo culture in cultures)
	 {
	 strLoc = "Loc_37tsdg";
	 if(culture.IsNeutralCulture)
	   continue;
	 try{
	 culture.DateTimeFormat.Calendar = calendar;
	 }catch{
	 continue;
	 }
	 if((calendar is GregorianCalendar)){
	 greg = (GregorianCalendar)calendar;
	 if(greg.CalendarType==GregorianCalendarTypes.TransliteratedFrench){
	 if(calendar.GetDayOfWeek(date)==DayOfWeek.Thursday)
	   continue;
	 }
	 }
	 if(culture.Name.Equals("div-MV"))
	   continue;
	 iCountTestcases++;
	 foreach(String format in formats)
	   {
	   if((calendar is GregorianCalendar)){
	   greg = (GregorianCalendar)calendar;
	   if(greg.CalendarType==GregorianCalendarTypes.TransliteratedFrench){
	   if(calendar.GetDayOfWeek(date.ToUniversalTime())==DayOfWeek.Thursday)
	     continue;
	   }
	   }
	   if(!(calendar is GregorianCalendar) && (format.ToUpper().Equals("U") || format.ToUpper().Equals("R")))
	     continue;
	   if(format.ToUpper().Equals("M")){
	   if(calendar.GetDayOfMonth(date) > calendar.GetDaysInMonth(calendar.GetYear(DateTime.Now), calendar.GetMonth(date)))
	     continue;
	   }
	   if((culture.Name.Equals("ka-GE")
	       || culture.Name.Equals("af-ZA"))
	      && format.Equals("u")
	      )
	     continue;
	   if(culture.Name.Equals("fa-IR")
	      && format.ToUpper().Equals("Y")
	      )
	     continue;
	   if((culture.Name.Equals("af-ZA")
	       || culture.Name.Equals("th-TH"))
	      && format.ToUpper().Equals("R")
	      )
	     continue;
	   if((culture.Name.Equals("sq-AL")
	       || culture.Name.Equals("mn-MN")
	       )
	      && format.ToUpper().Equals("Y")
	      )
	     continue;
	   strLoc = "Loc_386tsg";
	   s = date.ToString(format, culture);
	   strLoc = "Loc_3947tsg";
	   dateExpected = (DateTime)table[format];
	   dateInfoCustomFormats = null;
	   try
	     {
	     dateInfoCustomFormats = DateTimeFormatInfo.GetInstance(culture).GetAllDateTimePatterns(format[0]);
	     }
	   catch(Exception)
	     {
	     Console.WriteLine("Err_3498wsg");
	     }
	   try
	     {
	     strLoc = "Loc_9374tsdg!";
	     style = DateTimeStyles.None;
	     dateReturned = DateTime.Parse(s, culture, style);
	     if(!ResultCorrect(dateReturned, dateExpected, format, culture, calendar))
	       {
	       iCountErrors++;
	       Console.WriteLine("Err_93745sdg! Culture: <{4}>, Format: {2}, String: <{3}>, Expected: {0}, Returned: {1}", dateExpected, dateReturned, format, s, culture);
	       }								
	     strLoc = "Loc_93456tfg!";
	     if( !format.Equals("U") 
		 && (culture.Name.Equals("en-US") || culture.Name.Equals("ja-JP") || culture.Name.Equals("de-DE"))
		 )
	       {
	       if(culture.Name.Equals("ja-JP") && (format.Equals("r")||format.Equals("R")))
		 continue;
	       if(culture.Name.Equals("de-DE") && (format.Equals("f")||format.Equals("F")||format.Equals("G")||format.Equals("g")||format.Equals("t")||format.Equals("T")))
		 continue;
	       foreach(String patternFormat in dateInfoCustomFormats)
		 {
		 if(GetPatternedDate(table, patternFormat, format, out dateExpectedFromPattern))
		   {
		   parseToBeString = date.ToString(patternFormat, culture);
		   dateReturned = DateTime.Parse(parseToBeString, culture, style);
		   if(!ResultCorrect(dateReturned, dateExpectedFromPattern, format, culture, calendar))
		     {
		     iCountErrors++;
		     Console.WriteLine("Err_347tsdg! Culture: <{4}>, Format: {2}, BaseFormat: {5}, String: <{3}>, Expected: {0}, Returned: {1}", dateExpectedFromPattern, dateReturned, patternFormat, parseToBeString, culture, format);
		     }
		   }
		 }
	       }
	     strLoc = "Loc_2837sdg!";
	     style = DateTimeStyles.NoCurrentDateDefault;
	     dateReturned = DateTime.Parse(s, culture, style);
	     if(!ResultCorrect(dateReturned, dateExpected, format, culture, calendar))
	       {
	       iCountErrors++;
	       Console.WriteLine("Err_3745dsgdg! Culture: <{4}>, Format: {2}, String: <{3}>, Expected: {0}, Returned: {1}", dateExpected, dateReturned, format, s, culture);
	       }
	     strLoc = "Loc_486tsg";
	     s = s.PadLeft(random.Next(50));
	     style = DateTimeStyles.AllowLeadingWhite;
	     dateReturned = DateTime.Parse(s, culture, style);
	     if(!ResultCorrect(dateReturned, dateExpected, format, culture, calendar))
	       {
	       iCountErrors++;
	       Console.WriteLine("Err_74985wgdsg! Culture: <{4}>, Format: {2}, String: <{3}>, Expected: {0}, Returned: {1}", dateExpected, dateReturned, format, s, culture);
	       }
	     strLoc = "Loc_23947tsdg!";
	     s = s.TrimStart();
	     s = s.PadRight(random.Next(50));
	     style = DateTimeStyles.AllowTrailingWhite;
	     dateReturned = DateTime.Parse(s, culture, style);
	     if(!ResultCorrect(dateReturned, dateExpected, format, culture, calendar))
	       {
	       iCountErrors++;
	       Console.WriteLine("Err_9374sdg! Culture: <{4}>, Format: {2}, String: <{3}>, Expected: {0}, Returned: {1}", dateExpected, dateReturned, format, GetUnicodeString(s), culture);
	       }
	     strLoc = "Loc_234967sgd!";
	     s = s.TrimEnd();
	     fDoneAtLeastOnce = false;
	     for(int i=0; i<10; i++)
	       {
	       ch1 = splittableChars[random.Next(splittableChars.Length)];
	       if(s.IndexOf(ch1) >0)
		 {
		 s = s.Insert(s.IndexOf(ch1), new String(' ', random.Next(2, 20)));
		 if(!fDoneAtLeastOnce)
		   fDoneAtLeastOnce=true;
		 }
	       }
	     style = DateTimeStyles.AllowInnerWhite;
	     dateReturned = DateTime.Parse(s, culture, style);
	     if(!ResultCorrect(dateReturned, dateExpected, format, culture, calendar))
	       {
	       iCountErrors++;
	       Console.WriteLine("Err_43275sdg! Culture: <{4}>, Format: {2}, String: <{3}>, Expected: {0}, Returned: {1}", dateExpected, dateReturned, format, s, culture);
	       }    					
	     strLoc = "Loc_4329076tsfg!";
	     s = s.PadLeft(random.Next(50));
	     s = s.PadRight(random.Next(50));
	     style = DateTimeStyles.AllowWhiteSpaces;
	     dateReturned = DateTime.Parse(s, culture, style);
	     if(!ResultCorrect(dateReturned, dateExpected, format, culture, calendar))
	       {
	       iCountErrors++;
	       Console.WriteLine("Err_3497gsg! Culture: <{4}>, Format: {2}, String: <{3}>, Expected: {0}, Returned: {1}", dateExpected, dateReturned, format, s, culture);
	       } 
	     }
	   catch(Exception ex)
	     {
	     if(culture.Name.Equals("vi-VN") && Char.ToUpper(format[0]).Equals('Y'))
	       ex=null;
	     else
	       {
	       iCountErrors++;
	       Console.WriteLine("Err_75639rge! Unexpected exception thrown {0}, Loc: {3}, format: {4}, Culture: {1}, s:<{2}>, Date: {5}", ex.GetType().Name, culture, s, strLoc, format, date);
	       }
	     }
	   }
	 }
       }
     }
     } 
   catch (Exception exc_general ) 
     {
     ++iCountErrors;
     Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
     }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs. "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
     return true;
     }
   else
     {
     Console.WriteLine("FAiL! "+s_strTFName+" ,inCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 private Boolean ResultCorrect(DateTime returned, DateTime expected, String format, CultureInfo culture, Calendar cal)
   {
   switch(format)
     {
     case "m":
     case "M":
       if(cal is HijriCalendar)
	 {
	 if(cal.GetYear(DateTime.Now) != cal.GetYear(returned))
	   return false;
	 return (returned.ToString("m", culture)==expected.ToString("m", culture));
	 }
       else
	 return ((returned.Month==expected.Month) && (returned.Day==expected.Day));
     case "t":
       return ((returned.Hour==expected.Hour) && (returned.Minute==expected.Minute));
     case "T":
       return ((returned.Hour==expected.Hour) && (returned.Minute==expected.Minute) && (returned.Second==expected.Second));
     case "u":
       return (returned==expected);				
     case "y":
     case "Y":
       if(cal is HijriCalendar)
	 {
	 return (returned.ToString("y", culture.DateTimeFormat)==expected.ToString("y", culture.DateTimeFormat));
	 }
       else
	 return ((returned.Month==expected.Month) && (returned.Year==expected.Year));
     default:
       return (returned==expected);
     }
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co8571Parse_ifp_dts cbA = new Co8571Parse_ifp_dts();
   try 
     {
     bResult = cbA.runTest();
     } 
   catch (Exception exc_main)
     {
     bResult = false;
     Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
     }
   if (!bResult)
     {
     Console.WriteLine ("Path: "+s_strTFPath+"\\"+s_strTFName);
     Console.WriteLine( " " );
     Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
 static String GetUnicodeString(String str) 
   {
   StringBuilder buffer = new StringBuilder();
   for (int i = 0; i < str.Length; i++) 
     {
     if (str[i] < 0x20) 
       {
       buffer.Append("\\x" + ((int)str[i]).ToString("x4"));
       } 
     else if (str[i] < 0x7f) 
       {
       buffer.Append(str[i]);
       } 
     else 
       {
       buffer.Append("\\x" + ((int)str[i]).ToString("x4"));
       }
     }
   return (buffer.ToString());
   }
 private Boolean GetPatternedDate(Hashtable table, String pattern, String format, out DateTime date)
   {
   date = (DateTime)table[format];
   if(pattern.StartsWith("yy"))
     return false;
   switch(format)
     {
     case "F":
     case "G":
     case "U":
       if(pattern.IndexOf("ss")<=0
	  && pattern.IndexOf(" s")<=0				
	  )
	 date = (DateTime)table["f"];					
       break;			
     case "T":
       if(pattern.IndexOf("ss")<=0
	  && pattern.IndexOf(" s")<=0
	  )
	 date = (DateTime)table["t"];
       break;			
     }
   return true;
   }
 private DateTime GetStartDate(Calendar calendar)
   {
   Int32 year = calendar.TwoDigitYearMax-90;		
   DateTime date = calendar.ToDateTime(year, 1, 1, 1, 1, 1, 980);
   return date;
   }    
 private Int32 GetRandomYear(Calendar calendar, Random rand)
   {
   Int32 year = calendar.TwoDigitYearMax-90;
   Int32 value = rand.Next(year, year+80);
   return value;
   }    
}
