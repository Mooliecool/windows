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
public class Co3558GetHashCode
{
 static String strName = "Decimal.GetHashCode";
 static String strTest = "Co3558GetHashCode.cs";
 static String strPath = "";
 public Boolean runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="123_er";
   String strInfo = null;
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( strPath + strTest );
   Console.Out.WriteLine( " runTest started..." );
   Decimal dcml1;
   Decimal[] dcmlFirstValues = {Decimal.One, new Decimal(-1234.5678), Decimal.MinusOne, Decimal.MaxValue, 
				Decimal.Parse("98765.4321"), Decimal.Zero, Decimal.MaxValue,
				Decimal.MaxValue, Decimal.MinValue};
   Decimal[] dcmlSecondValues = {new Decimal(-1), new Decimal(-10), new Decimal(1), new Decimal(10), 
				 Decimal.Parse("123.4567"), new Decimal(0.0001), Decimal.Zero,
				 Decimal.Parse("-1.9999"), Decimal.One};
   Decimal dcmlValue;
   int iHashCode;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       for (int aa = 0; aa < dcmlFirstValues.Length; aa++)
	 {
	 iHashCode = dcmlFirstValues[aa].GetHashCode();
	 dcmlValue = dcmlFirstValues[aa];
	 ++iCountTestcases;
	 if ( iHashCode != dcmlValue.GetHashCode() )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr_" + aa + "  iHashCode ==" + iHashCode  );
	   }
	 }
       for (int aa = 0; aa < dcmlFirstValues.Length; aa++)
	 {
	 iHashCode = dcmlFirstValues[aa].GetHashCode();
	 dcmlValue = dcmlSecondValues[aa];
	 ++iCountTestcases;
	 if ( iHashCode == dcmlValue.GetHashCode() )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_340qo_" + aa + "  iHashCode ==" + iHashCode  );
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
   Co3558GetHashCode oCbTest = new Co3558GetHashCode();
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
