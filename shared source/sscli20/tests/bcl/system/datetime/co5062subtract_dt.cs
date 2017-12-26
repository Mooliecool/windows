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
public class Co5062Subtract_dt
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "DateTime.Subtract(DateTime t)";
 public static readonly String s_strTFName        = "Co5062Subtract_dt.cs";
 public static readonly String s_strTFAbbrev      = "Co5062";
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
   TimeSpan ts2;
   try {
   do
     {
     strLoc = "Loc_010nw";
     iCountTestcases++;
     try {
     ts1 = TimeSpan.Parse("-00:00:00");
     } catch (Exception) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_293nq! ");
     }
     strLoc = "Loc_100nu";
     dt1 = new DateTime(1998,1,1,1,0,0);
     dt2 = new DateTime(1998,1,1,1,0,0);
     iCountTestcases++;
     ts1 = dt1.Subtract(dt2);
     ts2 = TimeSpan.Parse("00:00:00");
     if(!ts1.Equals(ts2))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_739nu!");
       }
     strLoc = "Loc_149nw";
     dt1 = new DateTime(1875,12,24,15,13,25);
     dt2 = new DateTime(1875,12,24);
     ts1 = dt1.Subtract(dt2);
     ts2 = TimeSpan.Parse("15:13:25");
     iCountTestcases++;
     if(!ts1.Equals(ts2))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_238xz!, ts1=="+ts1.ToString());
       }
     dt1 = new DateTime(2000, 3,1 ,14,1,0);
     dt2 = new DateTime(1999,2,28,14,0,0);
     ts1 = dt1.Subtract(dt2);
     ts2 = TimeSpan.Parse("367.00:01:00");
     iCountTestcases++;
     if(!ts1.Equals(ts2))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_793mw! ,ts1=="+ts1.ToString());
       }
     dt1 = new DateTime(1999,2,28,14,0,0);
     dt2 = new DateTime(2000,3,1 ,14,1,0);
     ts1 = dt1.Subtract(dt2);
     ts2 = TimeSpan.Parse("-367.00:01:00");
     iCountTestcases++;
     if(!ts1.Equals(ts2))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_218nq! ,ts1=="+ts1.ToString());
       }
     dt1 = new DateTime(1998,1,2,15,34,40);
     dt2 = new DateTime(1998,1,2,3,0,0);
     ts1 = dt1.Subtract(dt2);
     ts2 = TimeSpan.Parse("12:34:40");
     iCountTestcases++;
     if(!ts1.Equals(ts2))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_374ja! ,ts1=="+ts1.ToString());
       }
     dt1 = new DateTime(1998,1,3,3,0,0);
     dt2 = new DateTime(1998,1,3,15,34,40);
     ts1 = dt1.Subtract(dt2);
     ts2 = TimeSpan.Parse("-12:34:40");
     iCountTestcases++;
     if(!ts1.Equals(ts2))
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_893ie! ,ts1=="+ts1.ToString());
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
   Co5062Subtract_dt cbA = new Co5062Subtract_dt();
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
