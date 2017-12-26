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
public class Co3680ToDouble
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Decimal.ToDouble()";
 public static String s_strTFName        = "Co3680ToDouble.cs";
 public static String s_strTFAbbrev      = "Co3680";
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
			   Decimal.MaxValue, Decimal.MinValue};
   double[] dblValues = {(double)1,  127d, 124.13, 0d, 1.23456, 
			 (double)Int16.MinValue, -1.0, (double)(-124.13),
			 (double)Decimal.MaxValue, (double)Decimal.MinValue};
   double dbl1;
   Decimal[] dcmlExceptions = {}; 
   double da;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       for (int aa = 0; aa < dcmlValues.Length; aa++)
	 {
	 dbl1 = Decimal.ToDouble(dcmlValues[aa]);
	 ++iCountTestcases;
	 if (dbl1 != dblValues[aa])
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_972qr_" + aa + "  dbl1 ==" + dbl1 + "  dblValues[aa] ==" + dblValues[aa]  );
	   }
	 }
       for (int aa = 0; aa < dcmlExceptions.Length; aa++)
	 {
	 try
	   {
	   ++iCountTestcases;
	   dbl1 = Decimal.ToDouble(dcmlExceptions[aa]);
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
   Co3680ToDouble oCbTest = new Co3680ToDouble();
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
