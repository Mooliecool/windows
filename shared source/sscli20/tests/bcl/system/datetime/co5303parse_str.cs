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
using System.Text;
using Microsoft.Win32; 
using System.IO; 
using System.Globalization;
public class Co5303Parse_str
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "DateTime.Parse()";
 public static String s_strTFName        = "Co5303Parse_str.cs";
 public static String s_strTFAbbrev      = "Co5303";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public static DateTime dttmNow          = DateTime.Now;
 public static Random rand               = new Random( ( ( dttmNow.Second * 1000 ) + dttmNow.Millisecond ) );
 public Int32 GenerateI32( Int32 minVal ,Int32 maxVal )
   {
   return (Int32)rand.Next( minVal, maxVal );
   }
 public Boolean runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   try {
   do
     {
     DateTime dt1, dt2;
     DateTime date;
     String str1;
     int year, month, day, hour, minute, second;
     int iMaxCounter = 0;
     DateTimeFormatInfo info;
     String[] values;
     
     strLoc = "Loc_98hyb";
     str1 = "Sun Nov  6 08:49:37 1994"; 
     try {
     iCountTestcases++;
     dt1 = DateTime.Parse(str1);
     iCountErrors++;
     Console.WriteLine( "Error_8930u! Incorrect datetime=="+dt1.ToString("R", null));
     }catch(FormatException){
     } catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine( "Error_28nc8! Unexpected exception, exc=="+exc);
     }
     strLoc = "Loc_100aa";
     str1 = null;
     iCountTestcases++;
     try
       {
       dt1 = DateTime.Parse(str1);
       iCountErrors++;
       Console.WriteLine("Error_100bb! Expected ArgumentNullException not thrown");
       }
     catch (ArgumentNullException){
     }
     catch (Exception exc){
     iCountErrors++;
     Console.WriteLine("Error_100dd! ArgumentNullException expected, got=="+exc.ToString());
     }
     strLoc = "Loc_2598c";
     iMaxCounter = 10;
     for(int ii = 0 ; ii < iMaxCounter ; ii++)
       {
       year = GenerateI32(1, 9999);
       month = GenerateI32(1, 12);
       day = GenerateI32(1, 28);
       hour = GenerateI32(0, 23);
       minute = GenerateI32(0, 59);
       second = GenerateI32(0, 59);
       try
	 {
	 dt1 = new DateTime(year, month, day, hour, minute, second);
	 dt2 = DateTime.Parse(dt1.ToString("yyyy-MM-dd HH:mm:ss", null));
	 iCountTestcases++;
	 if(!dt2.Equals(dt1))
	   {
	   Console.WriteLine("Error_11111! dt1=="+dt1+"    , dt2=="+dt2);
	   iCountErrors++;
	   }
	 }
       catch (Exception ex){
       iCountErrors++;
       Console.WriteLine("Err_987245ds! UInexpected exception thrown, Ex: {0}, year: {1}, Month: {2}, day: {3}, hour: {4}, minute: {5}, second: {6}", ex.GetType().Name, year, month, day, hour, minute, second);
       }
       }
     strLoc = "Loc_39sds";
     iMaxCounter = 10;
     for(int ii = 0 ; ii < iMaxCounter ; ii++)
       {
       year = GenerateI32(1, 9999);
       month = GenerateI32(1, 12);
       day = GenerateI32(1, 28);
       hour = GenerateI32(0, 23);
       minute = GenerateI32(0, 59);
       second = GenerateI32(0, 59);
       try
	 {
	 dt1 = new DateTime(year, month, day, hour, minute, second);
	 dt2 = DateTime.Parse(dt1.ToString("MM-dd-yyyy HH:mm:ss", null));
	 iCountTestcases++;
	 if(!dt2.Equals(dt1)){
	 Console.WriteLine("Error_22222! dt1=="+dt1+"    , dt2=="+dt2);
	 iCountErrors++;
	 }
	 }
       catch (Exception ex){
       iCountErrors++;
       Console.WriteLine("Err_93476tsg! UInexpected exception thrown, Ex: {0}, year: {1}, Month: {2}, day: {3}, hour: {4}, minute: {5}, second: {6}", ex.GetType().Name, year, month, day, hour, minute, second);
       }
       }
     strLoc = "Loc_0392d";
     iMaxCounter = 10;
     for(int ii = 0 ; ii < iMaxCounter ; ii++)
       {
       year = GenerateI32(1, 9999);
       month = GenerateI32(1, 12);
       day = GenerateI32(1, 28);
       hour = GenerateI32(0, 23);
       minute = GenerateI32(0, 59);
       second = GenerateI32(0, 59);
       try
	 {
	 dt1 = new DateTime(year, month, day, hour, minute, second);
	 dt2 = DateTime.Parse(dt1.ToString("yyyy/MM/dd HH:mm:ss", null));
	 iCountTestcases++;
	 if(!dt2.Equals(dt1))
	   {
	   Console.WriteLine("Error_33333! dt1=="+dt1+"    , dt2=="+dt2);
	   iCountErrors++;
	   }
	 }catch(Exception exc){
	 iCountErrors++;
	 Console.WriteLine( "Error_3947tsg! Unexpected exception, exc=="+exc);
	 }
       }
     strLoc = "Loc_90382";
     iMaxCounter = 10;
     for(int ii = 0 ; ii < iMaxCounter ; ii++)
       {
       year = GenerateI32(1, 9999);
       month = GenerateI32(1, 12);
       day = GenerateI32(1, 28);
       hour = GenerateI32(0, 23);
       minute = GenerateI32(0, 59);
       second = GenerateI32(0, 59);
       try
	 {
	 dt1 = new DateTime(year, month, day, hour, minute, second);
	 dt2 = DateTime.Parse(dt1.ToString("MM/dd/yyyy HH:mm:ss", null));
	 iCountTestcases++;
	 if(!dt2.Equals(dt1))
	   {
	   Console.WriteLine("Error_44444! dt1=="+dt1+"    , dt2=="+dt2);
	   iCountErrors++;
	   }
	 }catch(Exception exc){
	 iCountErrors++;
	 Console.WriteLine( "Error_3947tsg! Unexpected exception, exc=="+exc);
	 }
       }
     strLoc = "Loc_029ds";
     iMaxCounter = 10;
     for(int ii = 0 ; ii < iMaxCounter ; ii++)
       {
       year = GenerateI32(100, 9999);
       month = GenerateI32(1, 12);
       day = GenerateI32(1, 28);
       hour = GenerateI32(0, 23);
       minute = GenerateI32(0, 59);
       second = GenerateI32(0, 59);
       str1 = year+"  /  "+month+"  / \t"+day+"\t\t "+hour+"    :"+minute+":   "+second;
       dt1 = new DateTime(year, month, day, hour, minute, second);
       dt2 = DateTime.Parse(str1);
       iCountTestcases++;
       if(!dt2.Equals(dt1))
	 {
	 Console.WriteLine("Error_55555! dt1=="+dt1+"    , dt2=="+dt2);
	 iCountErrors++;
	 }
       }
     strLoc = "Loc_9182d";
     iMaxCounter = 10;
     for(int ii = 0 ; ii < iMaxCounter ; ii++)
       {
       year = GenerateI32(100, 9999);
       month = GenerateI32(1, 12);
       day = GenerateI32(1, 28);
       hour = GenerateI32(0, 23);
       minute = GenerateI32(0, 59);
       second = GenerateI32(0, 59);
       str1 = year+"-"+month+"-"+day+" "+hour+":"+minute+":"+second;
       dt1 = new DateTime(year, month, day, hour, minute, second);
       dt2 = DateTime.Parse(str1);
       iCountTestcases++;
       if(!dt2.Equals(dt1))
	 {
	 Console.WriteLine("Error_66666! dt1=="+dt1+" , dt2=="+dt2);
	 iCountErrors++;
	 }
       }
     strLoc = "Loc_1950j";
     str1 = "   \t\t\t\t\t\t\n\n\n\r4/\t\t\r4/4004\t\t\t\n\n\n    12:\t\t\t\n\n23:   12";
     dt1 = DateTime.Parse(str1);
     iCountTestcases++;
     if(!dt1.Equals(new DateTime(4004, 4, 4, 12, 23, 12)))
       {
       iCountErrors++;
       Console.WriteLine("Error_29385! WhiteSpace not ignored like expected");
       }
     strLoc = "Loc_0912";
     iMaxCounter = 100;
     for(int ii = 0 ; ii < iMaxCounter ; ii++)
       {
       year = GenerateI32(1900, 2050);
       month = GenerateI32(1, 12);
       day = GenerateI32(1, 28);
       hour = GenerateI32(0, 23);
       minute = GenerateI32(0, 59);
       second = GenerateI32(0, 59);
       dt1 = new DateTime(year, month, day, hour, minute, second);
       dt2 = DateTime.Parse(dt1.ToShortTimeString());
       iCountTestcases++;
       if(dt2.Hour != dt1.Hour || dt2.Minute != dt1.Minute)
	 {
	 Console.WriteLine("Error_88888! dt1=="+dt1+"    , dt2=="+dt2);
	 iCountErrors++;
	 }
       }
     strLoc = "Loc_5387s";
     iMaxCounter = 100;
     for(int ii = 0 ; ii < iMaxCounter ; ii++)
       {
       year = GenerateI32(1900, 2050);
       month = GenerateI32(1, 12);
       day = GenerateI32(1, 28);
       hour = GenerateI32(0, 23);
       minute = GenerateI32(0, 59);
       second = GenerateI32(0, 59);
       dt1 = new DateTime(year, month, day, hour, minute, second);
       dt2 = DateTime.Parse(dt1.ToLongTimeString());
       iCountTestcases++;
       if(dt2.Hour != dt1.Hour || dt2.Minute != dt1.Minute || dt2.Second != dt1.Second)
	 {
	 Console.WriteLine("Error_99999! dt1=="+dt1+"    , dt2=="+dt2);
	 iCountErrors++;
	 }
       }
     iMaxCounter = 100;
     for(int ii = 0 ; ii < iMaxCounter ; ii++)
       {
       year = GenerateI32(1, 9999);
       month = GenerateI32(1, 12);
       day = GenerateI32(1, 28);
       hour = GenerateI32(0, 23);
       minute = GenerateI32(0, 59);
       second = GenerateI32(0, 59);
       dt1 = new DateTime(year, month, day, hour, minute, second);
       dt2 = DateTime.Parse(dt1.ToLongDateString());
       iCountTestcases++;
       if(dt2.Year != dt1.Year || dt2.Month != dt1.Month || dt2.Day != dt1.Day)
	 {
	 Console.WriteLine("Error_00000! dt1=="+dt1+"    , dt2=="+dt2);
	 iCountErrors++;
	 }
       }
     strLoc = "Loc_017rggfs";
     iCountTestcases++;
     info = DateTimeFormatInfo.GetInstance(CultureInfo.InvariantCulture);
     values = info.MonthNames;
     foreach(String value in values){
     try{
     date = DateTime.Parse(value);
     iCountErrors++;
     Console.WriteLine( "Error_92375rdf! Month name correctly parsed! String: {0} Date: {1}", value, date);
     }catch(FormatException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_017ef! Wrong exception thrown, " + ex.GetType().Name);
     }
     }
     strLoc = "Loc_8472dg";
     iCountTestcases++;
     info = DateTimeFormatInfo.GetInstance(CultureInfo.InvariantCulture);
     values = info.DayNames;
     foreach(String value in values){
     try{
     date = DateTime.Parse(value);
     iCountErrors++;
     Console.WriteLine( "Error_93r7dg! Month name correctly parsed! String: {0} Date: {1}", value, date);
     }catch(FormatException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_29734tsgd! Wrong exception thrown, " + ex.GetType().Name);
     }
     }
     strLoc = "Loc_3049sg";
     iCountTestcases++;
     date = new DateTime(2001, 7, 11, 15, 3, 1, 340);
     str1 = date.ToString("t");
     dt1 = DateTime.Parse(str1);
     dt2 = DateTime.Now;
     if(dt1.Hour!=date.Hour
	|| dt1.Minute!=date.Minute
	|| dt1.Year != dt2.Year
	|| dt1.Month != dt2.Month
	|| (dt1.Second != 0)
	){
     iCountErrors++;
     Console.WriteLine("Err_74598s! wrong value returned, <{0}> <{1}>", date, dt1);
     }
     values = info.GetAllDateTimePatterns('t');
     foreach(String pattern in values){
     str1 = date.ToString(pattern);
     dt1 = DateTime.Parse(str1);
     dt2 = DateTime.Now;
     if(dt1.Hour!=date.Hour
	|| dt1.Minute!=date.Minute
	|| dt1.Year != dt2.Year
	|| dt1.Month != dt2.Month
	|| (dt1.Second != 0)
	){
     iCountErrors++;
     Console.WriteLine("Err_74598s! wrong value returned, <{0}> <{1}>", date, dt1);
     }
     }
     str1 = date.ToString("T");
     dt1 = DateTime.Parse(str1);
     dt2 = DateTime.Now;
     if(dt1.Hour!=date.Hour
	|| dt1.Minute!=date.Minute
	|| dt1.Second != date.Second
	|| dt1.Year != dt2.Year
	|| dt1.Month != dt2.Month
	){
     iCountErrors++;
     Console.WriteLine("Err_39047tsdg! wrong value returned, <{0}> <{1}>", date, dt1);
     }
     values = info.GetAllDateTimePatterns('T');
     foreach(String pattern in values){
     str1 = date.ToString(pattern);
     dt1 = DateTime.Parse(str1);
     dt2 = DateTime.Now;
     if(dt1.Hour!=date.Hour
	|| dt1.Minute!=date.Minute
	|| dt1.Second != date.Second
	|| dt1.Year != dt2.Year
	|| dt1.Month != dt2.Month
	){
     iCountErrors++;
     Console.WriteLine("Err_74598s! wrong value returned, <{0}> <{1}>", date, dt1);
     }
     }
     strLoc = "Loc_32947tsfg";
     iCountTestcases++;
     str1 = "PM 10:10:10";
     dt1 = DateTime.Parse(str1);
     dt2 = DateTime.Now;
     if(dt1.Hour!=22
	|| dt1.Minute!=10
	|| dt1.Second != 10
	|| dt1.Year != dt2.Year
	|| dt1.Month != dt2.Month
	){
     iCountErrors++;
     Console.WriteLine("Err_9347tsg! wrong value returned, <{0}> <{1}>", dt2, dt1);
     }
     str1 = "AM 10:10:10";
     dt1 = DateTime.Parse(str1);
     dt2 = DateTime.Now;
     if(dt1.Hour!=10
	|| dt1.Minute!=10
	|| dt1.Second != 10
	|| dt1.Year != dt2.Year
	|| dt1.Month != dt2.Month
	){
     iCountErrors++;
     Console.WriteLine("Err_9347tsg! wrong value returned, <{0}> <{1}>", dt2, dt1);
     }
     strLoc = "Loc_32947tsfg";
     iCountTestcases++;
     str1 = @"11:00Z";
     date = DateTime.Parse(str1);
     dt2 = DateTime.Now;
     dt1 = new DateTime(dt2.Year, dt2.Month, dt2.Day, 11, 0, 0);
     dt1 = dt1.ToLocalTime();
     if(dt1!=date){				
     iCountErrors++;
     Console.WriteLine("Err_8745sg! wrong value returned, <{0}>, <{1}>", dt1, date);
     }
     str1 = @"2:25Z";
     date = DateTime.Parse(str1);
     dt2 = DateTime.Now;
     dt1 = new DateTime(dt2.Year, dt2.Month, dt2.Day, 2, 25, 0);
     dt1 = dt1.ToLocalTime();
     if(dt1!=date){
     iCountErrors++;
     Console.WriteLine("Err_027dsg! wrong value returned, <{0}>, <{1}>", dt1, date);
     }
     str1 = @"2:25:30 GMT";
     date = DateTime.Parse(str1);
     dt2 = DateTime.Now;
     dt1 = new DateTime(dt2.Year, dt2.Month, dt2.Day, 2, 25, 30);
     dt1 = dt1.ToLocalTime();
     if(dt1!=date){
     iCountErrors++;
     Console.WriteLine("Err_384tsg! wrong value returned, <{0}>, <{1}>", dt1, date);
     }
     } while (false);
   } catch (Exception exc_general ) {
   ++iCountErrors;
   Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
   Console.WriteLine(exc_general.StackTrace);
   }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs.   "+s_strTFPath +"\\"+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
     return true;
     }
   else
     {
     Console.WriteLine("FAiL!   "+s_strTFPath+"\\"+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public static void Main(String[] args) 
   {
   Boolean bResult = false;
   Co5303Parse_str cbA = new Co5303Parse_str();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main);
   }
   if (!bResult)
     {
     Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
     Console.WriteLine( " " );
     Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
