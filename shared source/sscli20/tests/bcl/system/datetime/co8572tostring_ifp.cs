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
using System.Threading;
public class Co8572ToString_ifp
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "DateTime.ToString(IFormatProvider)";
 public static String s_strTFName        = "Co8572ToString_ifp.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   DateTimeFormatInfo info;
   DateTime date;
   String returnedValue;
   String expectedValue;
   CultureInfo culture;
   String strJapaneseCalRetValue = "\x5e73\x6210 13/4/18 16:17:26";
   String strJapaneseCalRetValueWhenRunInJpnMachine = "\x5e73\x6210 13/4/18 \x5348\x5f8c 04:17:26";
   StringBuilder builder;
   try
     {
     strLoc = "Loc_97563wgs";
     iCountTestcases++;
     info = CultureInfo.InvariantCulture.DateTimeFormat;
     date = new DateTime(2001, 4, 18, 16, 17, 26, 980);
     expectedValue = "04/18/2001 16:17:26";
     returnedValue = date.ToString(info);
     if(!returnedValue.Equals(expectedValue)){
     iCountErrors++;
     Console.WriteLine("Err_3249dsg! Unexpcted value returned, " + returnedValue);
     }
     strLoc = "Loc_8346zdg";
     iCountTestcases++;
     date = new DateTime(2001, 4, 18, 9, 17, 26, 980);
     info = new DateTimeFormatInfo();
     info.AMDesignator = "NowOrNever";
     expectedValue = "04/18/2001 09:17:26";
     returnedValue = date.ToString(info);
     if(!returnedValue.Equals(expectedValue)){
     iCountErrors++;
     Console.WriteLine("Err_834sgd! Unexpcted value returned, " + returnedValue);
     }
     strLoc = "Loc_8346zdg";
     iCountTestcases++;
     date = new DateTime(2001, 4, 18, 9, 17, 26, 980);
     info = new DateTimeFormatInfo();
     info.DateSeparator = "NowOrNever";
     expectedValue = "04NowOrNever18NowOrNever2001 09:17:26";
     returnedValue = date.ToString(info);
     if(!returnedValue.Equals(expectedValue)){
     iCountErrors++;
     Console.WriteLine("Err_834sdg! Unexpcted value returned, " + returnedValue);
     }
     builder = new StringBuilder("Simple");
     try{
     for(int i=0; i<50000; i++)
       builder.Append('a');
     }catch(Exception){
     Console.WriteLine("Loc_7439wedwg! Test case not succedding");
     }
     try{
     info.DateSeparator = builder.ToString();
     returnedValue = date.ToString(info);
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_384sdg! Unexpcted Exception thrown, " + ex.GetType().Name);
     }
     strLoc = "Loc_8346zdg";
     iCountTestcases++;
     date = new DateTime(2001, 4, 18, 16, 17, 26, 980);
     culture = new CultureInfo("ja-JP");
     info = culture.DateTimeFormat;
     info.Calendar = new JapaneseCalendar();
     returnedValue = date.ToString(info);
     if(!returnedValue.Equals(strJapaneseCalRetValue))
       {
       iCountErrors++;
       Console.WriteLine("Err_87435sdg! Unexpcted value returned, <{0}><{1}>", GetUnicodeString(returnedValue), GetUnicodeString(strJapaneseCalRetValue));
       }
     strLoc = "Loc_98376wdgs";
     iCountTestcases++;
     date = new DateTime(2001, 4, 18, 9, 17, 26, 980);
     info = new DateTimeFormatInfo();
     info.ShortDatePattern = "yyyyyyy";
     info.LongTimePattern = "ss";
     expectedValue = "0002001 26";
     returnedValue = date.ToString(info);
     if(!returnedValue.Equals(expectedValue)){
     iCountErrors++;
     Console.WriteLine("Err_93475sdg! Unexpcted value returned, " + returnedValue);
     }
     strLoc = "Loc_398745dsg";
     iCountTestcases++;
     date = new DateTime(2001, 4, 18, 9, 17, 26, 980);
     info = new DateTimeFormatInfo();
     info.LongDatePattern = "yyyyyyy";
     info.ShortTimePattern = "ss";
     expectedValue = "04/18/2001 09:17:26";
     returnedValue = date.ToString(info);
     if(!returnedValue.Equals(expectedValue)){
     iCountErrors++;
     Console.WriteLine("Err_93475sdg! Unexpcted value returned, " + returnedValue);
     }
     strLoc = "Loc_398745dsg";
     iCountTestcases++;
     date = new DateTime(2001, 4, 18, 9, 17, 26, 980);
     info = new DateTimeFormatInfo();
     info.TimeSeparator = "yipppetyDooo";
     expectedValue = "04/18/2001 09yipppetyDooo17yipppetyDooo26";
     returnedValue = date.ToString(info);
     if(!returnedValue.Equals(expectedValue)){
     iCountErrors++;
     Console.WriteLine("Err_93475sdg! Unexpcted value returned, " + returnedValue);
     }
     strLoc = "Loc_398745dsg";
     iCountTestcases++;
     date = new DateTime(2001, 4, 18, 9, 17, 26, 980);
     info = new DateTimeFormatInfo();
     info.YearMonthPattern = "yyyy-mmmm";
     expectedValue = "04/18/2001 09:17:26";
     returnedValue = date.ToString(info);
     if(!returnedValue.Equals(expectedValue)){
     iCountErrors++;
     Console.WriteLine("Err_93475sdg! Unexpcted value returned, " + returnedValue);
     }
     } catch (Exception exc_general ) {
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
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co8572ToString_ifp cbA = new Co8572ToString_ifp();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
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
}
