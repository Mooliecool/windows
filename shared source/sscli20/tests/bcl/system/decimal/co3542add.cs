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
public class Co3542Add
{
 static String strName = "Decimal.Add";
 static String strTest = "Co3542Add.cs";
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
   Decimal[] dcmlLeftValues = {Decimal.One, Decimal.MinusOne, Decimal.MaxValue, Decimal.MinValue, 
			       new Decimal(-1234.5678),
			       Decimal.Parse("-7.9228162514264337593543950335", CultureInfo.InvariantCulture),
			       Decimal.Parse("1.4214206543486529434634231456", CultureInfo.InvariantCulture),
			       Decimal.Parse("1.42142065434865294346342314563", CultureInfo.InvariantCulture)};
   Decimal[] dcmlRightValues = {Decimal.One, Decimal.One, Decimal.MinValue, new Decimal(0.1), 
				new Decimal(-8765.43218),
				Decimal.Parse("7.9228162514264337593543950335", CultureInfo.InvariantCulture),
				Decimal.Parse("3.9228162514264337593543950335", CultureInfo.InvariantCulture),
				Decimal.Parse("3.92281625142643375935439503359", CultureInfo.InvariantCulture)};
   Decimal[] dcmlExpValues = {new Decimal(2), Decimal.Zero, Decimal.Zero, Decimal.Parse("-79228162514264337593543950335", CultureInfo.InvariantCulture), 
			      new Decimal(-9999.99998),
			      Decimal.Zero,
			      Decimal.Parse("5.3442369057750867028178181791", CultureInfo.InvariantCulture),
			      Decimal.Parse("5.3442369057750867028178181792", CultureInfo.InvariantCulture)};
   Decimal[] dcmlExceptionLeftValues = {Decimal.MaxValue, Decimal.MinValue, Decimal.MaxValue, Decimal.MinValue}; 
   Decimal[] dcmlExceptionRightValues = {Decimal.MaxValue, Decimal.MinValue, new Decimal(0.50005), new Decimal(-0.5)}; 
   try
     {
     do
       {
       for (int aa = 0; aa < dcmlLeftValues.Length; aa++)
	 {
	 dcml1 = Decimal.Add(dcmlLeftValues[aa], dcmlRightValues[aa]);
	 ++iCountTestcases;
	 if ( Decimal.Equals(dcml1, dcmlExpValues[aa]) != true )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr_" + aa + "  dcml1 ==" + dcml1  );
	   }
	 }
       for (int aa = 0; aa < dcmlExceptionLeftValues.Length; aa++)
	 {
	 try
	   {
	   ++iCountTestcases;
	   dcml1 = Decimal.Add(dcmlExceptionLeftValues[aa], dcmlExceptionRightValues[aa]);
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
   Co3542Add oCbTest = new Co3542Add();
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
