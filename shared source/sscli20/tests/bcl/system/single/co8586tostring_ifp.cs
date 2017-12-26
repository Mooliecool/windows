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
using System.Globalization;
using System;
public class Co8586ToString_ifp
{
 public static readonly String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "Single.ToString(IFP info)";
 public static readonly String s_strTFName        = "Co8586ToString_ifp.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public virtual bool runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "Loc_0000oo_";
   String strOut = null;
   NumberFormatInfo nfi1 = new NumberFormatInfo();
   nfi1.NegativeSign = "^";  
   Single [] singTestValues = {Single.MinValue,
			       (Single)(-1000.54),
			       (Single)(-99.339),
			       (Single)(-5.45),
			       (Single)(-0.0),
			       (Single)(0.0),
			       (Single)(5.621),
			       (Single)(13.4223),
			       (Single)(101.81),
			       (Single)(1000.999),
			       Single.MaxValue,
			       (Single)(5E-5)
   };
   String [] strResultGFormat1 = {"^3.402823E+38",
				  "^1000.54",
				  "^99.339",
				  "^5.45",
				  "0",
				  "0",
				  "5.621",
				  "13.4223",
				  "101.81",
				  "1000.999",
				  "3.402823E+38",
				  "5E^05",
   };
   String [] strResultGFormat2 = {"^3.403E+38",
				  "^1001",
				  "^99.34",
				  "^5.45",
				  "0",
				  "0",
				  "5.621",
				  "13.42",
				  "101.8",
				  "1001",
				  "3.403E+38",
				  "5E^05",
   };
   try {
   strBaseLoc = "Loc_1100ds_";
   for (int i=0; i < singTestValues.Length;i++)
     {
     strLoc = strBaseLoc+ i.ToString();
     iCountTestcases++;
     strOut = singTestValues[i].ToString(nfi1);
     if(!strOut.Equals(strResultGFormat1[i]))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_293qu! , i=="+i+" strOut=="+strOut);
       }
     }
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
   Co8586ToString_ifp cbA = new Co8586ToString_ifp();
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
