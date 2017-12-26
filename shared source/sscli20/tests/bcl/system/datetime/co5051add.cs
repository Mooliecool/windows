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
public class Co5051Add
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "DateTime.Add()";
 public static readonly String s_strTFName        = "Co5051Add.cs";
 public static readonly String s_strTFAbbrev      = "Co5051";
 public static readonly String s_strTFPath        = "";
 public virtual Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   DateTime dt1 ;
   DateTime dt2 ;
   DateTime dt3 ;
   TimeSpan ts1 ;
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
     strLoc = "Loc_128wi";
     inYear = 9999;
     inMonth = 12;
     inDay = 31;
     inHour = 23;
     inMinute = 59;
     inSecond = 59;
     dt1 = new DateTime( inYear, inMonth, inDay, inHour, inMinute, inSecond );
     ts1 = new TimeSpan(0,0,1);
     iCountTestcases++;
     try {
     dt1 = dt1.Add(ts1);
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_820uu! , dt1=="+dt1);
     } catch (ArgumentException aExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_192jh1 ,exc=="+exc);
     }
     strLoc = "Loc_128wi";
     inYear = 1999;
     inMonth = 12;
     inDay = 31;
     inHour = 23;
     inMinute = 20;
     inSecond = 15;
     dt1 = new DateTime( inYear, inMonth, inDay, inHour, inMinute, inSecond );
     dt2 = new DateTime( inYear, inMonth, inDay, inHour, inMinute, inSecond );
     dt2 = dt2.AddHours(23);
     dt2 = dt2.AddMinutes(1);
     dt2 = dt2.AddSeconds(54);
     ts1 = new TimeSpan(23,01,54);
     dt1 = dt1.Add(ts1);
     iCountTestcases++;
     if(!dt1.Equals(dt2))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_129qu! , dt1=="+dt1.ToString()+" , dt2=="+dt2.ToString());
       }
     strLoc = "Loc_128wi";
     inYear = 1996;
     inMonth = 2;
     inDay = 28;
     inHour = 22;
     inMinute = 20;
     inSecond = 15;
     dt1 = new DateTime( inYear, inMonth, inDay, inHour, inMinute, inSecond );
     dt2 = new DateTime( inYear, inMonth, inDay, inHour, inMinute, inSecond );
     dt2 = dt2.AddHours(5);
     dt2 = dt2.AddMinutes(21);
     dt2 = dt2.AddSeconds(3);
     ts1 = new TimeSpan(05,21,03);
     dt1 = dt1.Add(ts1);
     iCountTestcases++;
     if(!dt1.Equals(dt2))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_452qu! , dt1=="+dt1.ToString()+" , dt2=="+dt2.ToString());
       }
     strLoc = "Loc_873as";
     dt1 = new DateTime(1999, 1, 1);
     dt2 = new DateTime(1999, 1, 31, 12, 15, 34);
     ts1 = new TimeSpan(30,12,15,34);
     dt1 = dt1.Add(ts1);
     iCountTestcases++;
     if(!dt1.Equals(dt2))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_471nl! ,dt1=="+dt1);
       }
     strLoc = "Loc 934nn";
     dt1 = new DateTime(1999, 1, 31, 12, 15, 34);
     dt2 = new DateTime(1999, 1, 2);
     dt3 = new DateTime(1999, 2, 1); 
     ts1 = new TimeSpan(-29,-12,-15,-34);
     dt1 = dt1.Add(ts1);
     iCountTestcases++;
     if(!dt1.Equals(dt2) && !dt1.Equals(dt3))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_148ij! ,dt1=="+dt1);
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
   Co5051Add cbA = new Co5051Add();
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
