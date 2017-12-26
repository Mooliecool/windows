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
using System;
public class Co5061Subtract_ts
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "DateTime.Subtract(TimeSpan t)";
 public static readonly String s_strTFName        = "Co5061Subtract_ts.cs";
 public static readonly String s_strTFAbbrev      = "Co5061";
 public static readonly String s_strTFPath        = Environment.CurrentDirectory;
 public virtual Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   DateTime dt1 ;
   DateTime dt2 ;
   TimeSpan ts1;
   try {
   do
     {
     strLoc = "Loc_100nu";
     ts1 = TimeSpan.Parse("00:00:00");
     dt1 = new DateTime(1999,1,1);
     dt2 = new DateTime(1999,1,1);
     iCountTestcases++;
     dt1 = dt1.Subtract(ts1);
     if(!dt1.Equals(dt2))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_372ne! ,dt1=="+dt1);
       }
     strLoc = "Loc_149nw";
     ts1 = TimeSpan.Parse("15:13:25");
     dt1 = new DateTime(1875,12,24,15,13,25);
     dt2 = new DateTime(1875,12,24);
     iCountTestcases++;
     dt1 = dt1.Subtract(ts1);
     if(!dt1.Equals(dt2))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_238xz!, dt1=="+dt1);
       }
     strLoc = "Loc_120je";
     ts1 = TimeSpan.Parse("015:21:22");
     dt1 = new DateTime(1999,1,1,0,0,2);
     dt2 = new DateTime(1998,12,31,8,38,40);
     dt1 = dt1.Subtract(ts1);
     iCountTestcases++;
     if(!dt1.Equals(dt2))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_820qj! , dt1=="+dt1);
       }
     strLoc = "Loc_130je";
     ts1 = TimeSpan.Parse("00:30:01");
     dt1 = new DateTime(2000, 3,1,0,0,2);
     dt2 = new DateTime(2000,2,29,23,30,1);
     dt1 = dt1.Subtract(ts1);
     iCountTestcases++;
     if(!dt1.Equals(dt2))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_982ew! , dt1=="+dt1);
       }
     strLoc = "Loc_140je";
     ts1 = TimeSpan.Parse("00:30:01");
     dt1 = new DateTime(1999, 3,1,0,0,2);
     dt2 = new DateTime(1999,2,28,23,30,1);
     dt1 = dt1.Subtract(ts1);
     iCountTestcases++;
     if(!dt1.Equals(dt2))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_283ji! , dt1=="+dt1);
       }
     strLoc = "Loc_150uw";
     ts1 = TimeSpan.Parse("365.00:00:01");
     dt1 = new DateTime(1998,2,1, 0, 0, 0);
     dt2 = new DateTime(1997,1,31,23,59,59);
     dt1 = dt1.Subtract(ts1);
     iCountTestcases++;
     if(!dt1.Equals(dt2))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_148ej!, dt1=="+dt1);
       }
     strLoc = "Loc_169hu";
     ts1 = TimeSpan.Parse("-365.00:00:01");
     dt1 = new DateTime(1998,2,1);
     dt2 = new DateTime(1999,2,1,0,0,1);
     dt1 = dt1.Subtract(ts1);
     iCountTestcases++;
     if(!dt1.Equals(dt2))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_834ji! ,dt1=="+dt1);
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
   Co5061Subtract_ts cbA = new Co5061Subtract_ts();
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
