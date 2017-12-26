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
public class Co3545ctor_float
{
 static String strName = "Decimal.Decimal";
 static String strTest = "Co3545ctor_float.cs";
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
   float[] fltValues = { (float)Int64.MinValue,  0f, 12.5f, 
			 (float)Int64.MaxValue};
   float[] fltExceptions = {Single.MinValue, Single.MaxValue, Single.NaN, 
			    Single.PositiveInfinity, Single.NegativeInfinity};
   float fltValue;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       for (int aa = 0; aa < fltValues.Length; aa++)
	 {
	 dcml1 = new Decimal(fltValues[aa]);
	 ++iCountTestcases;
	 if ( (float)dcml1 != fltValues[aa] )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr_" + aa + "  dcml1 ==" + dcml1 + "  fltValues[aa] ==" + fltValues[aa]  );
	   }
	 }
       fltValue = 1234.4567f;
       dcml1 = new Decimal(fltValue);
       ++iCountTestcases;
       if ( (float)dcml1 < (fltValue - 0.1) || (float)dcml1 > (fltValue + 0.1))
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_479tq dcml1 ==" + dcml1 + "  fltValue ==" + fltValue  );
	 }
       fltValue = Single.Epsilon;
       dcml1 = new Decimal(fltValue);
       ++iCountTestcases;
       if ( Decimal.ToSingle(dcml1) != 0f )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_436sf!  dcml1 ==" + dcml1  );
	 }
       fltValue = 123.34358f;
       dcml1 = new Decimal(fltValue);
       ++iCountTestcases;
       if ( Decimal.ToSingle(dcml1) != 123.3436f )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_346ae!  dcml1 ==" + dcml1  );
	 }
       for (int aa = 0; aa < fltExceptions.Length; aa++)
	 {
	 try
	   {
	   ++iCountTestcases;
	   dcml1 = new Decimal(fltExceptions[aa]);
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_852sp_" + aa + "!  Exception not thrown"  );
	   }
	 catch (OverflowException ex)
	   {
	   }
	 catch (Exception ex)
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_259pl_" + aa + "!  , Wrong Exception thrown == " + ex.ToString()  );
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
   Co3545ctor_float oCbTest = new Co3545ctor_float();
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
