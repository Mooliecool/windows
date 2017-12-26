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
public class Co5041Parse_str
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Byte.Parse(String)";
 public static String s_strTFName        = "Co5041Parse_str.cs";
 public static String s_strTFAbbrev      = "Co5041";
 public static String s_strTFPath        = "";
 public Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   Byte byt1a = (Byte)0;
   int in4a = 0;
   String str1 = null;
   try {
   do
     {
     strLoc = "Loc_111ag";
     byt1a = (Byte)0;
     str1 = null;
     iCountTestcases++;
     try {
     byt1a = Byte.Parse(str1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_383yq! , byt1a=="+byt1a);
     } catch (ArgumentException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_280ej! , exc=="+exc);
     }
     strLoc  = "Loc_122jf";
     in4a = ((int)Byte.MaxValue)+1;
     str1 = in4a.ToString();
     iCountTestcases++;
     try {
     byt1a = Byte.Parse(str1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_182fh! ,byt1a=="+byt1a);
     } catch (OverflowException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_382us! ,exc=="+exc);
     }
     strLoc = "Loc_130ds";
     in4a = 256;
     str1 = in4a.ToString();
     iCountTestcases++;
     try {
     byt1a = Byte.Parse(str1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_833se! ,byt1a=="+byt1a);
     } catch (OverflowException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_281nw! ,exc=="+exc);
     }
     strLoc = "Loc_133uu";
     in4a = ((int)Byte.MinValue)-1;
     str1 = in4a.ToString();
     iCountTestcases++;
     try {
     byt1a = Byte.Parse(str1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_387! ,byt1a=="+byt1a);
     } catch (OverflowException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_398jj! ,exc=="+exc);
     }
     strLoc = "Loc_144jd";
     in4a = (int)Byte.MaxValue;
     str1 = in4a.ToString();
     iCountTestcases++;
     try {
     byt1a = Byte.Parse(str1);
     } catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_874jj! ,exc=="+exc);
     }
     strLoc = "Loc_200ee";
     in4a = (int)Byte.MinValue;
     str1 = in4a.ToString();
     iCountTestcases++;
     try {
     byt1a = Byte.Parse(str1);
     } catch (Exception ) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_749ie! ,sh2=="+byt1a);
     }
     strLoc = "Loc_233ur";
     str1 = "3g";
     iCountTestcases++;
     try {
     byt1a = Byte.Parse(str1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_233dd! , byt1a=="+byt1a);
     } catch (FormatException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_934ui! ,exc=="+exc);
     }
     strLoc = "Loc_325jd";
     str1 = "3-2";
     iCountTestcases++;
     try {
     byt1a = Byte.Parse(str1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err388! ,byt1a=="+byt1a);
     } catch (FormatException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_832fj! ,exc=="+exc);
     }
     strLoc = "Loc_400jj";
     str1 = "-10";
     iCountTestcases++;
     try {
     byt1a = Byte.Parse(str1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_372eq! ,byt1a=="+byt1a);
     } catch (OverflowException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_122ks! , exc=="+exc);
     }
     strLoc = "Loc_500kj";
     str1 = "0x100";
     iCountTestcases++;
     try {
     byt1a = Byte.Parse(str1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_583kj! ,byt1a=="+byt1a);
     } catch (FormatException ) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_183sd! ,byt1a=="+exc);
     }
     strLoc = "Loc_511ee";
     try
       {
       str1 = "0xFF";
       byt1a = Byte.Parse(str1);
       iCountTestcases++;
       if(byt1a != 255)
         {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_523jd! ,byt1a=="+byt1a);
         }
       strLoc = "Loc_600ew";
       str1 = "0xff";
       byt1a = Byte.Parse(str1);
       iCountTestcases++;
       if(byt1a != 255)
         {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_733au! ,byt1a=="+byt1a);
         }
       strLoc = "Loc_700jd";
       str1 = "0xfg";
       iCountTestcases++;
       try {
       byt1a = Byte.Parse(str1);
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_372do! ,byt1a=="+byt1a);
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
       Console.WriteLine( s_strTFAbbrev+ "Err_983sy! , exc=="+exc);
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
   Co5041Parse_str cbA = new Co5041Parse_str();
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
