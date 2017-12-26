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
using System.Threading;
using System.IO; 
public class Co5339FromDays
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "TimeSpan.FromHours(double)";
 public static String s_strTFName        = "Co5339FromDays.cs";
 public static String s_strTFAbbrev      = "Co5339";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public Boolean runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "";
   try {
   LABEL_860_GENERAL:
   do
     {
     TimeSpan ts1, ts2;
     String str1, str2, strRec, strExp;
     Double dbl1, dbl2;
     strLoc = "Loc_093fj";
     iCountTestcases++;
     try
       {
       ts1 = TimeSpan.FromDays(Double.MaxValue);
       iCountErrors++;
       printerr("Error_209sh! Expected OverflowException , got value=="+ts1);
       }
     catch (OverflowException oExc)
       {
       Console.WriteLine("Info_28hgb! Caught Expected OverflowException , exc=="+oExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_20h8f! Expected OverflowException , got exc=="+exc.ToString());
       }
     strLoc = "Loc_3089f";
     iCountTestcases++;
     try
       {
       ts1 = TimeSpan.FromDays(Double.MinValue);
       iCountErrors++;
       printerr("Error_f38ee! Expected OverflowException , got value=="+ts1);
       }
     catch (OverflowException oExc)
       {
       Console.WriteLine("Info_38fhs! Caught Expected OverflowException , exc=="+oExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_398jf! Expected OverflowException , got exc=="+exc.ToString());
       }
     strLoc = "Loc_3009j";
     iCountTestcases++;
     try
       {
       ts1 = TimeSpan.FromDays(Double.NaN);
       iCountErrors++;
       printerr("Error_9j4jf! Expected OverflowException , got value=="+ts1);
       }
     catch (ArgumentException oExc)
       {
       Console.WriteLine("Info_2908d! Caught Expected OverflowException , exc=="+oExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_111i3! Expected OverflowException , got exc=="+exc.ToString());
       }
     strLoc = "Loc_20sje";
     iCountTestcases++;
     try
       {
       ts1 = TimeSpan.FromDays(10675200);
       iCountErrors++;
       printerr("Error_398ys! Expected OverflowException , got value=="+ts1);
       }
     catch (OverflowException oExc)
       {
       Console.WriteLine("Info_3099u! Caught Expected OverflowException , exc=="+oExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_38udf! Expected OverflowException , got exc=="+exc.ToString());
       }
     strLoc = "Loc_3r8jd";
     iCountTestcases++;
     try
       {
       ts1 = TimeSpan.FromDays(-10675200);
       iCountErrors++;
       printerr("Error_398ye! Expected OverflowException , got value=="+ts1);
       }
     catch (OverflowException oExc)
       {
       Console.WriteLine("Info_389sw! Caught Expected OverflowException , exc=="+oExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_1289f! Expected OverflowException , got exc=="+exc.ToString());
       }
     strLoc = "Loc_298fh";
     Console.WriteLine(TimeSpan.MaxValue);
     dbl1 = 10675199;
     ts1 = TimeSpan.FromDays(dbl1);
     ts2 = TimeSpan.Parse("10675199");
     iCountTestcases++;
     if(!ts1.Equals(ts2))
       {
       iCountErrors++;
       printerr("Error_209us! Expected=="+ts2+" , got value=="+ts1);
       }
     strLoc = "Loc_2890h";
     dbl1 = -10675199;
     ts1 = TimeSpan.FromDays(dbl1);
     ts2 = TimeSpan.Parse("-10675199");
     iCountTestcases++;
     if(!ts1.Equals(ts2))
       {
       iCountErrors++;
       printerr("Error_89hjc! Expected=="+ts2+" , got value=="+ts1);
       }
     strLoc = "Loc_09ngn";
     dbl1 = 1.5;
     ts1 = TimeSpan.FromDays(dbl1);
     ts2 = TimeSpan.Parse("1.12:00:00");
     iCountTestcases++;
     if(!ts1.Equals(ts2))
       {
       iCountErrors++;
       printerr("Error_9028n! Expected=="+ts2+" , got value=="+ts1);
       }
     dbl1 = 1.15740740740740740740740740740e-8;
     ts1 = TimeSpan.FromDays(dbl1);
     iCountTestcases++;
     if(!ts1.ToString().Equals("00:00:00.0010000"))
       {
       iCountErrors++;
       printerr("Error_39u98! Expected==00:00:00.0010000 , got value=="+ts1.ToString());
       }
     dbl1 = -1.15e-8;
     ts1 = TimeSpan.FromDays(dbl1);
     iCountTestcases++;
     Console.WriteLine(ts1.ToString());
     if(!ts1.ToString().Equals("-00:00:00.0010000"))
       {
       iCountErrors++;
       printerr("Error_2901d! Expected==-00:00:00.0010000 , got value=="+ts1.ToString());
       }
     dbl1 = 1.15740740740740740740740740740e-5;
     ts1 = TimeSpan.FromDays(dbl1);
     iCountTestcases++;
     Console.WriteLine(ts1.ToString());
     if(!ts1.ToString().Equals("00:00:01"))
       {
       iCountErrors++;
       printerr("Error_918hd! Expected==00:00:01, got value=="+ts1.ToString());
       }
     dbl1 = -1.15740740740740740740740740740e-5;
     ts1 = TimeSpan.FromDays(dbl1);
     iCountTestcases++;
     if(!ts1.ToString().Equals("-00:00:01"))
       {
       iCountErrors++;
       printerr("Error_0tu3s! Expected==-00:00:01 , got value=="+ts1.ToString());
       }
     strLoc = "Loc_208ud";
     dbl1 = 6.94444444444444444444444444444e-4;
     ts1 = TimeSpan.FromDays(dbl1);
     iCountTestcases++;
     if(!ts1.ToString().Equals("00:01:00"))
       {
       iCountErrors++;
       printerr("Error_83hgd! Expected==00:01:00, got value=="+ts1.ToString());
       }
     strLoc = "Loc_9283h";
     dbl1 = -6.94444444444444444444444444444e-4;
     ts1 = TimeSpan.FromDays(dbl1);
     iCountTestcases++;
     if(!ts1.ToString().Equals("-00:01:00"))
       {
       iCountErrors++;
       printerr("Error_392fh! Expected==-00:01:00 , got value=="+ts1.ToString());
       }
     strLoc = "Loc_28yhg";
     dbl1 = 4.16666666666666666666666666666e-2;
     ts1 = TimeSpan.FromDays(dbl1);
     iCountTestcases++;
     if(!ts1.ToString().Equals("01:00:00"))
       {
       iCountErrors++;
       printerr("Error_2910x! Expected==01:00:00 , got value=="+ts1.ToString());
       }
     strLoc = "Loc_87tha";
     dbl1 = -4.16666666666666666666666666666e-2;
     ts1 = TimeSpan.FromDays(dbl1);
     iCountTestcases++;
     if(!ts1.ToString().Equals("-01:00:00"))
       {
       iCountErrors++;
       printerr("Error_92gha! Expected==-01:00:00 , got value=="+ts1.ToString());
       }
     } while (false);
   } catch (Exception exc_general ) {
   ++iCountErrors;
   Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
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
 public void printerr ( String err )
   {
   Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
   }
 public void printinfo ( String info )
   {
   Console.WriteLine ("EXTENDEDINFO: ("+ s_strTFAbbrev + ") "+ info);
   }
 public static void Main(String[] args) 
   {
   Boolean bResult = false;
   Co5339FromDays cbA = new Co5339FromDays();
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
