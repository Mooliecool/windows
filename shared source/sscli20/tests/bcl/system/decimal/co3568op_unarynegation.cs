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
public class Co3568operator_minus_dec
{
 static String strName = "Decimal.-";
 static String strTest = "Co3568operator_minus_dec.cs";
 static String strPath = Environment.CurrentDirectory;
 public Boolean runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="123_er";
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( strPath + strTest );
   Console.Out.WriteLine( " runTest started..." );
   Decimal dcml1;
   Decimal[] dcmlValues = {Decimal.One, Decimal.MinusOne, Decimal.MaxValue, Decimal.MinValue, Decimal.Zero, 
			   new Decimal(-0), Decimal.Parse("123456787901.1234567890123456789", CultureInfo.InvariantCulture),
			   Decimal.Parse("-123456787901.1234567890123456789", CultureInfo.InvariantCulture), Decimal.Parse("0.000000000000000000000000000001", CultureInfo.InvariantCulture)};
   Decimal[] dcmlExpValues = {Decimal.MinusOne, Decimal.One, Decimal.MinValue, Decimal.MaxValue, Decimal.Zero, 
			      Decimal.Zero, Decimal.Parse("-123456787901.1234567890123456789", CultureInfo.InvariantCulture),
			      Decimal.Parse("123456787901.1234567890123456789", CultureInfo.InvariantCulture), Decimal.Parse("-0.000000000000000000000000000001", CultureInfo.InvariantCulture)};
   try
     {
     do
       {
       for (int aa = 0; aa < dcmlValues.Length; aa++)
	 {
	 dcml1 = -dcmlValues[aa];
	 ++iCountTestcases;
	 if ( dcml1 != dcmlExpValues[aa])
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr_" + aa + "  dcml1 ==" + dcml1  );
	   }
	 dcml1 = -             dcmlValues[aa];
	 if ( dcml1 != dcmlExpValues[aa])
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_365df_" + aa + "  dcml1 ==" + dcml1  );
	   }
	 dcml1 =-dcmlValues[aa];
	 if ( dcml1 != dcmlExpValues[aa])
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_346se_" + aa + "  dcml1 ==" + dcml1  );
	   }
	 }
       } while ( false );
     }
   catch (Exception exc_general)
     {
     ++iCountErrors;
     Console.Error.WriteLine ( "POINTTOBREAK: Error Err_103! strLoc=="+ strLoc + " ,exc_general=="+exc_general  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs. " + strPath + strTest + "  iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL! " + strPath + strTest + "   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false;	
   Co3568operator_minus_dec oCbTest = new Co3568operator_minus_dec();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (" + strTest + ") Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     Console.Error.WriteLine(  "PATHTOSOURCE:  " + strPath + strTest + "   FAiL!"  );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
