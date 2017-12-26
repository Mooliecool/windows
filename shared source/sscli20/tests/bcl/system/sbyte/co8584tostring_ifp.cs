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
using System.Globalization;
public class Co8584ToString_ifp
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "SByte.ToString(IFP info)";
 public static String s_strTFName        = "Co8584ToString_ifp.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "Loc_0000oo_";
   String str1 = null;
   SByte[] sbytTestValues = {SByte.MinValue, 
			     -13,
			     -5,
			     -0,
			     0,
			     5,
			     13,
			     50,
			     101,
			     SByte.MaxValue
   };
   String[] strResultGFormat1 = {"^128", 
				 "^13",
				 "^5",
				 "0",
				 "0",
				 "5",
				 "13",
				 "50",
				 "101",
				 "127"
   };
   NumberFormatInfo nfi1 = new NumberFormatInfo();
   nfi1.NegativeSign = "^";  
   try {
   strBaseLoc = "Loc_1100ds_";
   for (int i=0; i < sbytTestValues.Length;i++)
     {
     strLoc = strBaseLoc + i;
     iCountTestcases++;
     str1 = sbytTestValues[i].ToString(nfi1);
     if(!str1.Equals(strResultGFormat1[i]))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_293qu! , i=="+i+" str1=="+str1);
       }
     }
   strBaseLoc = "Loc_1200er_";
   for (int i=0; i < sbytTestValues.Length;i++)
     {
     strLoc = strBaseLoc + i.ToString();
     iCountTestcases++;
     str1 = sbytTestValues[i].ToString(nfi1);
     if(!str1.Equals(strResultGFormat1[i]))
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_347ew! , i=="+i+" str1=="+str1);
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
   Boolean bResult = false;
   Co8584ToString_ifp cbA = new Co8584ToString_ifp();
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
