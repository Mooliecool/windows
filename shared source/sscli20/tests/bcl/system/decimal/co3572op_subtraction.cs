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
public class Co3572operator_minus_decdec
{
 static String strName = "Decimal.-";
 static String strTest = "Co3572operator_minus_decdec.cs";
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
   Decimal[] dcmlExpValues = {Decimal.Zero, new Decimal(-2), Decimal.Zero, Decimal.Zero, 
			      new Decimal(7530.86438),
			      Decimal.Parse("-76928.344751388978446222888841", CultureInfo.InvariantCulture),
			      Decimal.Parse("12345678901234546789012345688", CultureInfo.InvariantCulture)};
   Decimal[] dcmlExceptionLeftValues = {Decimal.MaxValue, Decimal.MinValue, Decimal.MaxValue, Decimal.MinValue}; 
   Decimal[] dcmlExceptionRightValues = {Decimal.MinValue, Decimal.MaxValue, new Decimal(-0.50005), new Decimal(0.5)}; 
   Decimal dcml1;
   Decimal dcmlExpValue;
   try
     {
     do
       {
       for (int aa = 0; aa < dcmlLeftValues.Length; aa++)
	 {
	 dcml1 = dcmlLeftValues[aa] - dcmlRightValues[aa];
	 ++iCountTestcases;
	 if ( dcml1 != dcmlExpValues[aa] )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr_" + aa + "  dcml1 ==" + dcml1  );
	   }
	 }
       dcml1 = new Decimal(19) - new Decimal(3) * (new Decimal(6) - new Decimal(2))/new Decimal(4) - 15;
       dcmlExpValue = new Decimal(1);
       ++iCountTestcases;
       if ( dcml1 != dcmlExpValue )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_243wr  dcml1 ==" + dcml1  );
	 }
       for (int aa = 0; aa < dcmlExceptionLeftValues.Length; aa++)
	 {
	 try
	   {
	   ++iCountTestcases;
	   dcml1 = dcmlExceptionLeftValues[aa] - dcmlExceptionRightValues[aa];
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
   Co3572operator_minus_decdec oCbTest = new Co3572operator_minus_decdec();
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
