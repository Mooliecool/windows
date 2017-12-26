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
using System.IO;
using System.Globalization;
using System.Text;
using System.Collections;
public class Co8792HebrewSupport
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "DateTime - testing Hebrew issues";
 public static String s_strTFName        = "Co8792HebrewSupport.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   CultureInfo culture;
   DateTime date;
   Calendar calendar;
   String result;
   String expected;
   BitArray bits;
   Hashtable table;
   DictionaryEntry entry;
   Int32 startYear;
   try
     {
     calendar = new HebrewCalendar();
     culture = new CultureInfo("he-IL");
     culture.DateTimeFormat.Calendar = calendar;
     strLoc = "Loc_0273tsdg";
     iCountTestcases++;
     date = new DateTime(2001, 10, 2);
     result = date.ToString(culture);
     expected = "\x05d8\x0022\x05d5";
     if(result.IndexOf(expected)<0){
     iCountErrors++;
     Console.WriteLine("Err_834sG! Unexpected result: Expected to contain: <{0}>, Returned: <{1}>", GetUnicodeString(expected), GetUnicodeString(result));
     }
     date = new DateTime(2001, 10, 3);
     result = date.ToString(culture);
     expected = "\x05d8\x0022\x05d6";
     if(result.IndexOf(expected)<0){
     iCountErrors++;
     Console.WriteLine("Err_3947sg! Unexpected result: Expected to contain: <{0}>, Returned: <{1}>", GetUnicodeString(expected), GetUnicodeString(result));
     }
     date = new DateTime(5370, 2, 15, calendar);
     result = date.ToString(culture);
     expected = "\x05d8\x0022\x05d5";
     if(result.IndexOf(expected)<0){
     iCountErrors++;
     Console.WriteLine("Err_3947tsg! Unexpected result: Expected to contain: <{0}>, Returned: <{1}>", GetUnicodeString(expected), GetUnicodeString(result));
     }
     date = new DateTime(5370, 2, 16, calendar);
     result = date.ToString(culture);
     expected = "\x05d8\x0022\x05d6";
     if(result.IndexOf(expected)<0){
     iCountErrors++;
     Console.WriteLine("Err_3095g! Unexpected result: Expected to contain: <{0}>, Returned: <{1}>", GetUnicodeString(expected), GetUnicodeString(result));
     }
     strLoc = "Loc_9347tsg";
     iCountTestcases++;
     bits = new BitArray(new Boolean[]{true, false, false, true, false, false, true, false, true, false, false, true, false, false, true, false, false, true, false});
     startYear = 5757;
     for(int i=startYear; i<(startYear+19);i++){
     date = new DateTime(i, 7, 1, calendar);
     result = date.ToString("M", culture);
     if(bits[i-startYear])
       expected = "\x05d0\x05d3\x05e8 \x05d1";
     else
       expected = "\x05e0\x05d9\x05e1\x05df";
     if(result.IndexOf(expected)<0){
     iCountErrors++;
     Console.WriteLine("Err_027sdg_{2}! Unexpected result: Expected to contain: <{0}>, Returned: <{1}>", GetUnicodeString(expected), GetUnicodeString(result), (i-startYear));
     }
     }
     strLoc = "Loc_9347tsg";
     iCountTestcases++;
     startYear = 5761;
     table = new Hashtable();
     table.Add(startYear, new DictionaryEntry(29, 29));
     table.Add(startYear+1, new DictionaryEntry(29, 30));
     table.Add(startYear+2, new DictionaryEntry(30, 30));
     table.Add(startYear+3, new DictionaryEntry(30, 30));
     table.Add(startYear+4, new DictionaryEntry(29, 29));
     table.Add(startYear+5, new DictionaryEntry(29, 30));
     table.Add(startYear+6, new DictionaryEntry(30, 30));
     table.Add(startYear+7, new DictionaryEntry(29, 29));
     table.Add(startYear+8, new DictionaryEntry(29, 30));
     table.Add(startYear+9, new DictionaryEntry(30, 30));
     for(int i=startYear; i<(startYear+10);i++){
     date = new DateTime(i, 3, 1, calendar);
     date = date.Subtract(new TimeSpan(TimeSpan.TicksPerDay));
     result = date.ToString(culture);
     entry = (DictionaryEntry)table[i];
     if((Int32)entry.Key==29)
       expected = "\x05db\x0022\x05d8";
     else
       expected = "\x05dc";
     if(result.IndexOf(expected)<0){
     iCountErrors++;
     Console.WriteLine("Err_347dgdzg_{2}! Unexpected result: Expected to contain: <{0}>, Returned: <{1}>", GetUnicodeString(expected), GetUnicodeString(result), (i-startYear));
     }
     date = new DateTime(i, 4, 1, calendar);
     date = date.Subtract(new TimeSpan(TimeSpan.TicksPerDay));
     result = date.ToString(culture);
     entry = (DictionaryEntry)table[i];
     if((Int32)entry.Value==29)
       expected = "\x05db\x0022\x05d8";
     else
       expected = "\x05dc";
     if(result.IndexOf(expected)<0){
     iCountErrors++;
     Console.WriteLine("Err_32408sdg_{2}! Unexpected result: Expected to contain: <{0}>, Returned: <{1}>", GetUnicodeString(expected), GetUnicodeString(result), (i-startYear));
     }
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
   Co8792HebrewSupport cbA = new Co8792HebrewSupport();
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
 static String GetUnicodeString(String str) {
 StringBuilder buffer = new StringBuilder();
 for (int i = 0; i < str.Length; i++) {
 if (str[i] < 0x20) {
 buffer.Append("\\x" + ((int)str[i]).ToString("x4"));
 } else if (str[i] < 0x7f) {
 buffer.Append(str[i]);
 } else {
 buffer.Append("\\x" + ((int)str[i]).ToString("x4"));
 }
 }
 return (buffer.ToString());
 }
}
