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
public class Co5002Parse
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer    = "";
 public static String s_strClassMethod   = "Integer8.CompareTo()";
 public static String s_strTFName        = "Co5002Parse.cs";
 public static String s_strTFAbbrev      = "Co5002";
 public static String s_strTFPath        = "";
 public Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   Int64 in8a = (Int64)0;
   long lo2a = (long)0;
   String str1 = null;
   try {
   do
     {
     strLoc = "Loc_111ag";
     in8a = (Int64)0;
     lo2a = (long)0;
     str1 = null;
     iCountTestcases++;
     try {
     lo2a = Int64.Parse(str1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_383yq! , lo2a=="+lo2a);
     } catch (ArgumentException) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_280ej! , exc=="+exc);
     }
     strLoc  = "Loc_122jf";
     str1 = "0x10000000000000000";
     iCountTestcases++;
     try {
     lo2a = Int64.Parse(str1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_182fh! ,lo2a=="+lo2a);
     } catch (FormatException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_382us! ,exc=="+exc);
     }
     strLoc = "Loc_122ui";
     str1 = "9223372036854775808";
     iCountTestcases++;
     try {
     lo2a = Int64.Parse(str1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_733sy! ,lo2a=="+lo2a);
     } catch (OverflowException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_281hs!, exc=="+exc);
     }
     strLoc = "Loc_130jd";
     str1 = "-9223372036854775809";
     iCountTestcases++;
     try {
     lo2a = Int64.Parse(str1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_398dh! ,lo2a=="+lo2a);
     } catch (OverflowException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_982sy! ,exc=="+exc);
     }
     strLoc = "Loc_144jd";
     in8a = Int64.MaxValue;
     str1 = in8a.ToString();
     iCountTestcases++;
     lo2a = Int64.Parse(str1);
     if(lo2a != Int64.MaxValue) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_874jj! ,lo2a=="+lo2a);
     }
     strLoc = "Loc_200ee";
     in8a = Int64.MinValue;
     str1 = in8a.ToString();
     Console.Error.WriteLine(str1);
     iCountTestcases++;
     lo2a = Int64.Parse(str1);
     if(lo2a != Int64.MinValue) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_749ie! ,lo2a=="+lo2a);
     }
     strLoc = "Loc_233ur";
     str1 = "32fg";
     iCountTestcases++;
     try {
     lo2a = Int64.Parse(str1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_233dd! ,lo2=="+lo2a);
     } catch (FormatException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_934ui! ,exc=="+exc);
     }
     strLoc = "Loc_325jd";
     str1 = "3-2";
     iCountTestcases++;
     try {
     lo2a = Int64.Parse(str1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err388! ,lo2a=="+lo2a);
     } catch (FormatException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_832fj! ,exc=="+exc);
     }
     strLoc = "Loc_400jj";
     str1 = "-1000";
     iCountTestcases++;
     lo2a = Int64.Parse(str1);
     if(lo2a != -1000) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_372eq! , lo2a=="+lo2a);
     }
     strLoc = "Loc_466ue";
     str1 = "1000";
     iCountTestcases++;
     lo2a = Int64.Parse(str1);
     if(lo2a != 1000)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_283sa! ,lo2a=="+lo2a);
       }
     try
       {
       iCountTestcases += 4;
       strLoc = "Loc_500kj";
       str1 = "0xFFfFF";
       iCountTestcases++;
       lo2a = Int64.Parse(str1);
       if(lo2a != 1048575) {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_583kj! ,lo2a=="+lo2a);
       }
       strLoc = "Loc_511ee";
       str1 = "0xFFFFffffFFFFffff";
       lo2a = Int64.Parse(str1);
       iCountTestcases++;
       if(lo2a != -1)
         {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_523jd! ,lo2a=="+lo2a);
         }
       strLoc = "Loc_599fu";
       str1 = "0x7FFFffffFFFFffff";
       lo2a = Int64.Parse(str1);
       iCountTestcases++;
       if(lo2a != Int64.MaxValue)
         {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_912ty! ,lo2a=="+lo2a);
         }
       strLoc = "Loc_700jd";
       str1 = "0x5ffg";
       iCountTestcases++;
       try {
       lo2a = Int64.Parse(str1);
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_372do! ,lo2a=="+lo2a);
       } catch (FormatException ) {}
       catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_128si! ,exc=="+exc);
       }
       } catch (FormatException ) {}
       catch (Exception exc)
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_847ys! , exc=="+exc);
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
   Co5002Parse cbA = new Co5002Parse();
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
