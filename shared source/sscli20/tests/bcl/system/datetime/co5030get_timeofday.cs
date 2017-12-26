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
using System.IO;
using System.Threading;
using System;
public class Co5030get_TimeOfDay
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "DateTime.GetTimeOfDay()";
 public static readonly String s_strTFName        = "Co5030get_TimeOfDay.cs";
 public static readonly String s_strTFAbbrev      = "Co5030";
 public static readonly String s_strTFPath        = "";
 public virtual Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   DateTime dt1 ;
   TimeSpan ts1;
   int inYear = 0;
   int inMonth = 0;
   int inDay = 0;
   int inHour = 0;
   int inMinute = 0;
   int inSecond = 0;
   try {
   LABEL_860_GENERAL:
   do
     {
     strLoc = "Loc_100ey";
     inYear = 2000;
     inMonth = 1;
     inDay = 14;
     inHour = 23;
     inMinute = 13;
     inSecond = 25;
     dt1 = new DateTime(inYear, inMonth, inDay, inHour, inMinute, inSecond);
     ts1 = dt1.TimeOfDay;
     iCountTestcases++;
     if(!"23:13:25".Equals(ts1.ToString()))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_128qu! , ts1=="+ts1.ToString());
       }
     strLoc = "Loc_200qu";
     inHour = 0;
     inMinute = 0;
     inSecond = 1;
     dt1 = new DateTime(inYear, inMonth, inDay, inHour, inMinute, inSecond);
     ts1 = dt1.TimeOfDay;
     iCountTestcases++;
     if(!"00:00:01".Equals(ts1.ToString()))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_947hw! , ts1="+ts1.ToString());
       }
     strLoc = "Loc_283uq";
     inHour = 0;
     inMinute = 0;
     inSecond = 0;
     dt1 = new DateTime(inYear, inMonth, inDay, inHour, inMinute, inSecond);
     ts1 = dt1.TimeOfDay;
     iCountTestcases++;
     if(!"00:00:00".Equals(ts1.ToString()))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_871he! ,ts1=="+ts1.ToString());
       }
     strLoc = "Loc_300uq";
     inHour = 23;
     inMinute = 59;
     inSecond = 59;
     dt1 = new DateTime(inYear, inMonth, inDay, inHour, inMinute, inSecond);
     ts1 = dt1.TimeOfDay;
     iCountTestcases++;
     if(!"23:59:59".Equals(ts1.ToString()))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_871he! ,ts1=="+ts1.ToString());
       }
     } while (false);
   } catch (Exception exc_general ) {
   ++iCountErrors;
   Console.WriteLine(s_strTFAbbrev +" Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general);
   }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
     return true;
     }
   else
     {
     Console.Error.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public static void Main(String[] args)
   {
   Boolean bResult = false;
   Co5030get_TimeOfDay cbA = new Co5030get_TimeOfDay();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev+ "FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main);
   }
   if (!bResult)
     {
     Console.WriteLine(s_strTFName+ s_strTFPath);
     Console.Error.WriteLine( " " );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.Error.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
