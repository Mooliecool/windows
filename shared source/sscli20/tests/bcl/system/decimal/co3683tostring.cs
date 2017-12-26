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
public class Co3683ToString
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Decimal.ToToString()";
 public static String s_strTFName        = "Co3683ToString.cs";
 public static String s_strTFAbbrev      = "Co3683";
 public static String s_strTFPath        = "";
 public Boolean runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="123_er";
   String strInfo = null;
   Console.Out.Write( s_strClassMethod );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strTFPath + s_strTFName );
   Console.Out.Write( ": " );
   Console.Out.Write( s_strDtTmVer );
   Console.Out.WriteLine( " runTest started..." );
   Decimal dcml1;
   Decimal[] dcmlValues = {Decimal.One, 127, (Decimal)124.13, Decimal.Zero, (Decimal)1.23456, 
			   (Decimal)Int16.MinValue, Decimal.MinusOne, (Decimal)(-124.13),
			   Decimal.MaxValue, Decimal.MinValue,
			   Decimal.Parse("1.234567890123456789012345678")};
   String[] strValues = {"1",  "127", "124.13", "0", "1.23456", 
			 "-32768", "-1", "-124.13",
			 "79228162514264337593543950335", "-79228162514264337593543950335",
			 "1.234567890123456789012345678"};
   String str1;
   Decimal[] dcmlExceptions = {}; 
   double da;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       for (int aa = 0; aa < dcmlValues.Length; aa++)
	 {
	 str1 = dcmlValues[aa].ToString();
	 ++iCountTestcases;
	 if (String.Equals(str1, strValues[aa]) != true)
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_972qr_" + aa + "  str1 ==" + str1 + "  strValues[aa] ==" + strValues[aa]  );
	   }
	 }
       for (int aa = 0; aa < dcmlExceptions.Length; aa++)
	 {
	 try
	   {
	   ++iCountTestcases;
	   str1 = dcmlExceptions[aa].ToString();
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_852sp_" + aa + "!  Exception not thrown"  );
	   }
	 catch (OverflowException ex)
	   {
	   }
	 catch (Exception ex)
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err259pl_" + aa + "!  , Wrong Exception thrown == " + ex  );
	   }
	 }
       } while ( false );
     }
   catch (Exception exc_general)
     {
     ++iCountErrors;
     Console.WriteLine( s_strTFAbbrev +"Error Err_8888yyy!  strLoc=="+ strLoc +" ,exc_general=="+ exc_general );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountTestcases=="+ iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   "+ s_strTFPath +" "+ s_strTFName +"  ,iCountErrors=="+ iCountErrors +" ,BugNums?: "+ s_strActiveBugNums );
     return false;
     }
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false;	
   Co3683ToString oCbTest = new Co3683ToString();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.WriteLine( s_strTFAbbrev +"FAiL!  Error Err_9999zzz!  Uncaught Exception caught in main(), exc_main=="+ exc_main );
     }
   if ( ! bResult )
     {
     Console.WriteLine( s_strTFName +s_strTFPath );
     Console.Error.WriteLine( " " );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
     Console.Error.WriteLine( " " );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
