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
public class Co5001Parse
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Integer2.Parse()";
 public static String s_strTFName        = "Co5001Parse.cs";
 public static String s_strTFAbbrev      = "Co5001";
 public static String s_strTFPath        = "";
 public Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   short sh2a = (short)0;
   int in4a = 0;
   String str1 = null;
   try {
   do
     {
     strLoc = "Loc_111ag";
     sh2a = (short)0;
     str1 = null;
     iCountTestcases++;
     try {
     sh2a = Int16.Parse(str1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_383yq! , sh2a=="+sh2a);
     } catch (ArgumentException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_280ej! , exc=="+exc);
     }
     strLoc  = "Loc_122jf";
     in4a = (int)Int16.MaxValue+1;
     str1 = in4a.ToString();
     iCountTestcases++;
     try {
     sh2a = Int16.Parse(str1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_182fh! ,sh2a=="+sh2a);
     } catch (OverflowException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_382us! ,exc=="+exc);
     }
     strLoc = "Loc_130ds";
     in4a = 100000;
     str1 = in4a.ToString();
     iCountTestcases++;
     try {
     sh2a = Int16.Parse(str1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_833se! ,sh2a=="+sh2a);
     } catch (OverflowException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_281nw! ,exc=="+exc);
     }
     strLoc = "Loc_133uu";
     in4a = (int)Int16.MinValue-1;
     str1 = in4a.ToString();
     iCountTestcases++;
     try {
     sh2a = Int16.Parse(str1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_387! ,sh2=="+sh2a);
     } catch (OverflowException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_398jj! ,exc=="+exc);
     }
     strLoc = "Loc_144jd";
     in4a = Int16.MaxValue;
     str1 = in4a.ToString();
     iCountTestcases++;
     try {
     sh2a = Int16.Parse(str1);
     } catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_874jj! ,exc=="+exc);
     }
     strLoc = "Loc_200ee";
     in4a = (int)Int16.MinValue;
     str1 = in4a.ToString();
     iCountTestcases++;
     try {
     sh2a = Int16.Parse(str1);
     } catch (Exception) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_749ie! ,sh2=="+sh2a);
     }
     strLoc = "Loc_233ur";
     str1 = "32fg";
     iCountTestcases++;
     try {
     sh2a = Int16.Parse(str1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_233dd! ,sh2=="+sh2a);
     } catch (FormatException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_934ui! ,exc=="+exc);
     }
     strLoc = "Loc_325jd";
     str1 = "3-2";
     iCountTestcases++;
     try {
     sh2a = Int16.Parse(str1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err388! ,sh2a=="+sh2a);
     } catch (FormatException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_832fj! ,exc=="+exc);
     }
     strLoc = "Loc_400jj";
     str1 = "-1000";
     iCountTestcases++;
     sh2a = Int16.Parse(str1);
     if(sh2a != -1000) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_372eq! , sh2a=="+sh2a);
     }
     strLoc = "Loc_466ue";
     str1 = "1000";
     iCountTestcases++;
     sh2a = Int16.Parse(str1);
     if(sh2a != 1000)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_283sa! ,sh2a=="+sh2a);
       }
     iCountTestcases+= 5;
     strLoc = "Loc_500kj";
     try
       {
       str1 = "0x10000";
       iCountTestcases++;
       try {
       sh2a = Int16.Parse(str1);
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_583kj! ,sh2a=="+sh2a);
       } catch (FormatException ) {}
       catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_183sd! ,sh2a=="+exc);
       }
       strLoc = "Loc_511ee";
       str1 = "0xFFFF";
       sh2a = Int16.Parse(str1);
       iCountTestcases++;
       if(sh2a != -1)
         {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_523jd! ,sh2a=="+sh2a);
         }
       strLoc = "Loc_599fu";
       str1 = "0x7FFF";
       sh2a = Int16.Parse(str1);
       iCountTestcases++;
       if(sh2a != Int16.MaxValue)
         {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_912ty! ,sh2a=="+sh2a);
         }
       strLoc = "Loc_600ew";
       str1 = "0xffff";
       sh2a = Int16.Parse(str1);
       iCountTestcases++;
       if(sh2a != -1)
         {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_733au! ,sh2a=="+sh2a);
         }
       strLoc = "Loc_700jd";
       str1 = "0x5ffg";
       iCountTestcases++;
       try {
       sh2a = Int16.Parse(str1);
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_372do! ,sh2a=="+sh2a);
       } catch (FormatException ) {}
       catch (Exception exc) {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_128si! ,exc=="+exc);
       }
       }
     catch (FormatException ) {}
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
   Co5001Parse cbA = new Co5001Parse();
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
