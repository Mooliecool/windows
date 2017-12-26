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
using System.Threading;
using System.IO; 
public class Co5338Parse_str
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "StringBuilder.Replace(String, String)";
 public static String s_strTFName        = "Co5338Parse_str.cs";
 public static String s_strTFAbbrev      = "Cb5335";
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
     strLoc = "Loc_290hf";
     iCountTestcases++;
     try
       {
       ts1 = TimeSpan.Parse(null);
       iCountErrors++;
       printerr("Error_093nx! Expected ArgumentNullException , got value=="+ts1.ToString());
       }
     catch (ArgumentNullException aExc)
       {
       Console.WriteLine("Info_20hjf! Caught Expected ArgumentNullException, exc=="+aExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_0298a! Expected ArgumentNullException , got exc=="+exc.ToString());
       }
     strLoc = "Loc_209jg";
     str1 = "1.23:51:11";
     ts1 = TimeSpan.Parse(str1);
     ts2 = new TimeSpan(1, 23, 51, 11);
     iCountTestcases++;
     if(!ts1.Equals(ts2))
       {
       iCountErrors++;
       printerr("Error_209ua! expected=="+ts2+" , got value=="+ts1);
       }
     strLoc = "Loc_091us";
     str1 = "-234.12:10:33";
     ts1 = TimeSpan.Parse(str1);
     ts2 = new TimeSpan(-234, -12, -10, -33);
     iCountTestcases++;
     if(!ts1.Equals(ts2))
       {
       iCountErrors++;
       printerr("Error_2009s! Expected=="+ts2+" , got value=="+ts1);
       }
     strLoc = "Loc_308hf";
     str1 = "10675199.02:48:05.4775808";
     iCountTestcases++;
     try
       {
       ts1 = TimeSpan.Parse(str1);
       iCountErrors++;
       printerr("Error_109xn! Expected OverflowException , got value=="+ts1);
       }
     catch (OverflowException fExc)
       {
       Console.WriteLine("Info_283yc! Caught Expected OverflowException , exc=="+fExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_398sh! Expected OverflowException , got exc=="+exc.ToString());
       }
     strLoc = "Loc_290cn";
     str1 = "-10675199.02:48:05.4775809";
     iCountTestcases++;
     try
       {
       ts1 = TimeSpan.Parse(str1);
       iCountErrors++;
       printerr("Error_20hak! Expected OverflowException , got value=="+ts1);
       }
     catch (OverflowException fExc)
       {
       Console.WriteLine("Info_29hda! Caught Expected OverflowException , exc=="+fExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_099sn! Expected OverflowException , got exc=="+exc.ToString());
       }
     strLoc = "Loc_02hfn";
     str1 = "10675199.02:48:05.4775807";
     ts1 = TimeSpan.Parse(str1);
     ts2 = TimeSpan.MaxValue;
     iCountTestcases++;
     if(!ts1.Equals(ts2))
       {
       iCountErrors++;
       printerr("Error_9hj33! Expected=="+TimeSpan.MaxValue+" , got value=="+ts1.ToString());
       }
     strLoc = "Loc_8hf31";
     str1 = "122.12.12";
     iCountTestcases++;
     try
       {
       ts1 = TimeSpan.Parse(str1);
       iCountErrors++;
       printerr("Error_20jsj! Expected FormatException , got value=="+ts1.ToString());
       }
     catch (FormatException fExc)
       {
       Console.WriteLine("Info_2099u! Caught Expected FormatException , exc=="+fExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_08hf4! Expected FormatException , got exc=="+exc.ToString());
       }
     strLoc = "Loc_04kff";
     str1 = "2.13:20";
     ts1 = TimeSpan.Parse(str1);
     ts2 = new TimeSpan(2, 13, 20, 0);
     iCountTestcases++;
     if(!ts1.Equals(ts2))
       {
       iCountErrors++;
       printerr("Error_09nsa! Expected=="+ts2+" , got value=="+ts1);
       }
     strLoc = "Loc_09ht4";
     str1 = "2.25:12:12";
     iCountTestcases++;
     try
       {
       ts1 = TimeSpan.Parse(str1);
       iCountErrors++;
       printerr("Error_2090q! Expected OverflowException , got value=="+ts1);
       }
     catch (OverflowException fExc)
       {
       Console.WriteLine("Info_189sa! Caught Expected OverflowException , exc=="+fExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_9t4ha! Expected OverflowException , got exc=="+exc.ToString());
       }
     strLoc = "Loc_093hd";
     str1 = "3.23:60:1";
     iCountTestcases++;
     try
       {
       ts1 = TimeSpan.Parse(str1);
       iCountErrors++;
       printerr("Error_02hae! Expected OverflowException , got value=="+ts1.ToString());
       }
     catch (OverflowException fExc)
       {
       Console.WriteLine("Info_938hf! Caught Expected OverflowException, exc=="+fExc.Message);
       }
     catch (Exception exc)
       {
       iCountErrors++;
       printerr("Error_298gh! Expected OverflowException , got exc=="+exc.ToString());
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
   Co5338Parse_str cbA = new Co5338Parse_str();
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
