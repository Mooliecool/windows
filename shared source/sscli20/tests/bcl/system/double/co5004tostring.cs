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
public class Co5004ToString
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "double.ToString()";
 public static readonly String s_strTFName        = "Co5004ToString.cs";
 public static readonly String s_strTFAbbrev      = "Co5004";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   Double do1a = (Double)0;
   Double do1b = (Double)0;
   String str1 = null;
   Double [] doArr = {-3.35E28,
		      -5.33E22,
		      -3.5E-19,
		      -3.39E-5,
		      -10.1,
		      -7.7,
		      -4.54,
		      -2.26,
		      -0.0,
		      0.0,
		      0.1E-34,
		      5.4E-14,
		      0.34,
		      3.342,
		      6.445,
		      4.28E038,
		      5.34E101,
		      Double.NegativeInfinity,
		      Double.PositiveInfinity,
		      Double.NaN
   };
   String [] resultArr = {"-3.35E+28",
			  "-5.33E+22",
			  "-3.5E-19",
			  "-3.39E-05",
			  "-10.1",
			  "-7.7",
			  "-4.54",
			  "-2.26",
			  "0",
			  "0",
			  "1E-35",
			  "5.4E-14",
			  "0.34",
			  "3.342",
			  "6.445",
			  "4.28E+38",
			  "5.34E+101",
			  "-Infinity",
			  "Infinity",
			  "NaN"
   };
   try {
   LABEL_860_GENERAL:
   do
     {
     for(int i=0; i < doArr.Length;i++)
       {
       iCountTestcases++;
       if(!(resultArr[i].Equals(doArr[i].ToString())))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_823ys! ,i=="+i+" str1=="+doArr[i].ToString());
	 }
       }
     for(int i=0; i < doArr.Length;i++)
       {
       iCountTestcases++;
       if(!(resultArr[i].Equals(doArr[i].ToString( (IFormatProvider)null ))))
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_823ys! ,i=="+i+" str1=="+doArr[i].ToString());
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
   bool bResult = false;
   Co5004ToString cbA = new Co5004ToString();
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
