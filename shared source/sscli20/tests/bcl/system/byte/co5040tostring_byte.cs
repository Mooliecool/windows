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
public class Co5040ToString_SByte
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Byte.ToString(Byte b)";
 public static String s_strTFName        = "Co5040ToString_SByte.cs";
 public static String s_strTFAbbrev      = "Co5040";
 public static String s_strTFPath        = "";
 public Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "";
   Byte byt1a = (Byte)0;
   Byte byt1b = (Byte)0;
   String str1 = null;
   Byte[] bytTestValues = {Byte.MinValue, 
			   (Byte)5,
			   (Byte)27,
			   (Byte)50,
			   (Byte)100,
			   (Byte)127,
			   (Byte)200,
			   Byte.MaxValue
   };
   String[] strResultValues = {"0", 
			       "5",
			       "27",
			       "50",
			       "100",
			       "127",
			       "200",
			       "255"
   };
   try {
   LABEL_860_GENERAL:
   do
     {
     strBaseLoc = "Loc_1100eu_";
     for (int ii = 0 ; ii < bytTestValues.Length ; ii++)
       {
       strLoc = strBaseLoc + ii.ToString();
       iCountTestcases++;
       if(!strResultValues[ii].Equals(bytTestValues[ii].ToString((IFormatProvider) null)))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_792jq! ,return=="+bytTestValues[ii].ToString());
	 }
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
   Co5040ToString_SByte cbA = new Co5040ToString_SByte();
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
