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
public class Co5105ctor
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "Double.CompareTo()";
 public static readonly String s_strTFName        = "Co5105ctor.cs";
 public static readonly String s_strTFAbbrev      = "Cb105";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc;
   Double do2a = (Double)0;
   Double do2b = (Double)0;
   Double [] doArr = {-4.34E23
		      -10.1,
		      -7.7,
		      -4.54,
		      -2.26,
		      -3.32E-34,
		      -0.0,
		      0.0,
		      3.34E-23,
		      0.34,
		      3.342,
		      6.445,
		      1.23E39,
		      Double.NegativeInfinity,
		      Double.PositiveInfinity};
   try {
   LABEL_860_GENERAL:
   do
     {
     strBaseLoc = "Loc_111oo_";
     for ( int ii = 0 ; ii < doArr.Length ; ii++)
       {
       strLoc = strBaseLoc + ii.ToString();
       iCountTestcases++;
       if( ((Double)doArr[ii]) != doArr[ii])
	 {
	 iCountErrors++;
	 Console.Error.WriteLine( s_strTFAbbrev+" Err_398au! ");
	 }
       }
     do2a = ((Double)Double.NaN);
     iCountTestcases++;
     if(! Double.IsNaN(do2a))
       {
       iCountErrors++;
       Console.Error.WriteLine( s_strTFAbbrev+" Err_111da! , do2a=="+do2a);
       }
     } while (false);
   } catch (Exception exc_general ) {
   ++iCountErrors;
   Console.Error.WriteLine(s_strTFAbbrev+" : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general);
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
   Co5105ctor cbA = new Co5105ctor();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.Error.WriteLine(s_strTFAbbrev+ "  : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main);
   }
   if (!bResult)
     {
     Console.Error.WriteLine( " " );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.Error.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
