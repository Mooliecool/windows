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
public class Co5053IsLeapYear
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "DateTime.IsLeapYear()";
 public static readonly String s_strTFName        = "Co5053IsLeapYear.cs";
 public static readonly String s_strTFAbbrev      = "CB5053";
 public static readonly String s_strTFPath        = "";
 public virtual Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   DateTime dt1 ;
   DateTime dt2 ;
   String strOut;
   //int inYear = 0;
   try {
   LABEL_860_GENERAL:
		do
		{
			strLoc = "Loc_237uq";
			for(int inYear = 1; inYear <= 5000 ; inYear++)
			{
				iCountTestcases++;
				if((inYear % 4) == 0 && (inYear % 100 != 0 || inYear % 400 == 0))
				{
					if(!DateTime.IsLeapYear(inYear))
					{
						iCountErrors++;
						Console.WriteLine( s_strTFAbbrev+ "Err_923jq , inYear=="+inYear);
					}
				}
				else
				{
					if(DateTime.IsLeapYear(inYear))
					{
						iCountErrors++;
						Console.WriteLine( s_strTFAbbrev+ "Err_871ji , inYear=="+inYear);
					}
				}
			} 
			strLoc = "Loc_579fr";

			int[] years = {Int32.MinValue, -100, -1, 0, 10000, 10001, Int32.MaxValue};
			foreach (int inYear in years)
			{
				iCountTestcases++;
				try
				{
					bool bRet = DateTime.IsLeapYear(inYear);
					iCountErrors++;
					Console.WriteLine( s_strTFAbbrev+ "Err_767hrs, inYear=={0}, expected ArgumentOutOfRangeException, got {0}", inYear, bRet);
				}
				catch (ArgumentOutOfRangeException e)
				{
					// expected
				}
				catch (Exception e)
				{
					iCountErrors++;
					Console.WriteLine( s_strTFAbbrev+ "Err_601ptl, inYear=={0}, got unexpected exception", inYear);
					Console.WriteLine(e.ToString());
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
   Co5053IsLeapYear cbA = new Co5053IsLeapYear();
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
