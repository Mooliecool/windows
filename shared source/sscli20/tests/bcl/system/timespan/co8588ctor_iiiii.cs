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
public class Co8588ctor_iiiii
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "TimeSpan.ctor(int hour, int minute, int second, int millisecond)";
 public static String s_strTFName        = "Co8588ctor_iiiii.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   TimeSpan span;
   Decimal ticks;
   Int32 days, hours, minutes, seconds, millis;
   try
     {
     strLoc = "Loc_9347sg";
     iCountTestcases++;
     for(int nDay=-100; nDay <100; nDay +=9){
     for(int nHour=-100; nHour <100; nHour +=23){
     for(int nMin=-100; nMin <100; nMin +=17){
     for(int nSec=-100; nSec <100; nSec +=29){
     for(int nMil=-100; nMil <100; nMil +=23){
     span = new TimeSpan(nDay, nHour, nMin, nSec, nMil);
     ticks = new Decimal(nDay*TimeSpan.TicksPerDay) + new Decimal(nHour*TimeSpan.TicksPerHour) + new Decimal(nMin*TimeSpan.TicksPerMinute) + new Decimal(nSec*TimeSpan.TicksPerSecond) + new Decimal(nMil*TimeSpan.TicksPerMillisecond);
     days = (int)(ticks/TimeSpan.TicksPerDay);
     if(span.Days!=days){
     iCountErrors++;
     Console.WriteLine("Err_845sdg! {0} {1}, {2}", ticks, days, span.Days);
     }								
     hours = (int)((ticks/TimeSpan.TicksPerHour)%24);
     if(span.Hours!=hours){
     iCountErrors++;
     Console.WriteLine("Err_745wt! {0} {1}, {2}", ticks, hours, span.Hours);
     }
     minutes = (int)((ticks/TimeSpan.TicksPerMinute)%60);
     if(span.Minutes!=minutes){
     iCountErrors++;
     Console.WriteLine("Err_345dsg! {0} {1}, {2}", ticks, minutes, span.Minutes);
     }
     seconds = (int)((ticks/TimeSpan.TicksPerSecond)%60);
     if(span.Seconds!=seconds){
     iCountErrors++;
     Console.WriteLine("Err_89345sdg! {0} {1}, {2}", ticks, seconds, span.Seconds);
     }
     millis = (int)((ticks/TimeSpan.TicksPerMillisecond)%1000);
     if(span.Milliseconds!=millis){
     iCountErrors++;
     Console.WriteLine("Err_93247sdg! {0} {1}, {2}", ticks, millis, span.Milliseconds);
     }								
     }
     }
     }
     }
     }
     strLoc = "Loc_39764sg";
     iCountTestcases++;
     span = new TimeSpan(10675199, 2, 48, 5, 477);
     if(span.Days!=10675199){
     iCountErrors++;
     Console.WriteLine("Err_3453wg! Wrong result returned");
     }								
     if(span.Hours!=2){
     iCountErrors++;
     Console.WriteLine("Err_93475sdg! Wrong result returned");
     }								
     if(span.Minutes!=48){
     iCountErrors++;
     Console.WriteLine("Err_9347sdg! Wrong result returned");
     }								
     if(span.Seconds!=5){
     iCountErrors++;
     Console.WriteLine("Err_974325sdg! Wrong result returned");
     }								
     if(span.Milliseconds!=477){
     iCountErrors++;
     Console.WriteLine("Err_29475wdsg! Wrong result returned");
     }								
     strLoc = "Loc_3947sg";
     iCountTestcases++;
     span = new TimeSpan(-10675199, -2, -48, -5, -477);
     if(span.Days!=-10675199){
     iCountErrors++;
     Console.WriteLine("Err_3453wg! Wrong result returned");
     }								
     if(span.Hours!=-2){
     iCountErrors++;
     Console.WriteLine("Err_93475sdg! Wrong result returned");
     }								
     if(span.Minutes!=-48){
     iCountErrors++;
     Console.WriteLine("Err_9347sdg! Wrong result returned");
     }								
     if(span.Seconds!=-5){
     iCountErrors++;
     Console.WriteLine("Err_974325sdg! Wrong result returned");
     }								
     if(span.Milliseconds!=-477){
     iCountErrors++;
     Console.WriteLine("Err_29475wdsg! Wrong result returned");
     }								
     strLoc = "Loc_93746tsfg";
     iCountTestcases++;
     try{
     span = new TimeSpan(10675200, 0, 0, 0, 0);
     iCountErrors++;
     Console.WriteLine( "Err_32495dsg! No exception thrown");
     }catch(ArgumentOutOfRangeException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_32495dsg! Wrong exception thrown, " + ex.GetType().Name);
     }						
     iCountTestcases++;
     try{
     span = new TimeSpan(Int32.MaxValue, Int32.MaxValue, Int32.MaxValue, Int32.MaxValue, Int32.MaxValue);
     iCountErrors++;
     Console.WriteLine( "Err_32495dsg! No exception thrown, " + (TimeSpan.MaxValue.Ticks-span.Ticks));
     }catch(ArgumentOutOfRangeException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_32495dsg! Wrong exception thrown, " + ex.GetType().Name);
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
   Co8588ctor_iiiii cbA = new Co8588ctor_iiiii();
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
}
