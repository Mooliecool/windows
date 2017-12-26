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
public class Co3682ToSingle
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Decimal.ToSingle()";
 public static String s_strTFName        = "Co3682ToSingle.cs";
 public static String s_strTFAbbrev      = "Co3682";
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
   float[] fltValues = {(float)1,  127f, 124.13f, 0f, 1.23456f, 
			(float)Int16.MinValue, -1.0f, (float)(-124.13),
			(float)Decimal.MaxValue, (float)Decimal.MinValue};
   float flt1;
   Decimal[] dcmlExceptions = {}; 
   double da;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       for (int aa = 0; aa < dcmlValues.Length; aa++)
	 {
	 flt1 = Decimal.ToSingle(dcmlValues[aa]);
	 ++iCountTestcases;
	 if ((double)flt1 != (double)fltValues[aa])
	   {
	   ++iCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_972qr_" + aa + "  flt1 ==" + flt1 + "  fltValues[aa] ==" + fltValues[aa]  );
	   }
	 }
       for (int aa = 0; aa < dcmlExceptions.Length; aa++)
	 {
	 try
	   {
	   ++iCountTestcases;
	   flt1 = Decimal.ToSingle(dcmlExceptions[aa]);
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
   Co3682ToSingle oCbTest = new Co3682ToSingle();
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
