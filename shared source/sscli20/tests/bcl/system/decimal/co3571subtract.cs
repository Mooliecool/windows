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
public class Co3571Subtract
{
 static String strName = "Decimal.Subtract";
 static String strTest = "Co3571Subtract.cs";
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
   Decimal[] dcmlLeftValues = {Decimal.One, Decimal.MinusOne, Decimal.MinValue, Decimal.MaxValue, 
			       new Decimal(-1234.5678),
			       Decimal.Parse("12345.6789012345467890123456789", CultureInfo.InvariantCulture),
			       Decimal.Parse("12345678901234546789012345689", CultureInfo.InvariantCulture)};
   Decimal[] dcmlRightValues = {Decimal.One, Decimal.One, Decimal.MinValue, Decimal.MaxValue, 
				new Decimal(-8765.43218),
				Decimal.Parse("89274.0236526235252352352345232", CultureInfo.InvariantCulture),
				Decimal.Parse("0.523456789012345467890123456789", CultureInfo.InvariantCulture)};
   Decimal[] cncyExpValues = {Decimal.Zero, new Decimal(-2), Decimal.Zero, Decimal.Zero, 
			      new Decimal(7530.86438),
			      Decimal.Parse("-76928.344751388978446222888841", CultureInfo.InvariantCulture),
			      Decimal.Parse("12345678901234546789012345688", CultureInfo.InvariantCulture)};
   Decimal[] cncyExceptionLeftValues = {Decimal.MaxValue, Decimal.MinValue, Decimal.MaxValue, Decimal.MinValue}; 
   Decimal[] cncyExceptionRightValues = {Decimal.MinValue, Decimal.MaxValue, new Decimal(-0.50005), new Decimal(0.5)}; 
   Decimal cncy1;
   try
     {
     do
       {
       for (int aa = 0; aa < dcmlLeftValues.Length; aa++)
	 {
	 cncy1 = Decimal.Subtract(dcmlLeftValues[aa], dcmlRightValues[aa]);
	 ++iCountTestcases;
	 if ( cncy1 != cncyExpValues[aa] )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr_" + aa + "  cncy1 ==" + cncy1  );
	   }
	 }
       for (int aa = 0; aa < cncyExceptionLeftValues.Length; aa++)
	 {
	 try
	   {
	   ++iCountTestcases;
	   cncy1 = Decimal.Subtract(cncyExceptionLeftValues[aa], cncyExceptionRightValues[aa]);
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_852sp_" + aa + "!  Exception not thrown"  );
	   }
	 catch (OverflowException)
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
   Co3571Subtract oCbTest = new Co3571Subtract();
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
