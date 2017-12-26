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
public class Co3566operator_multiply
{
 static String strName = "Decimal.*";
 static String strTest = "Co3566operator_multiply.cs";
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
   Decimal[] dcmlLeftValues = {Decimal.One, Decimal.Parse("7922816251426433759354395033.5", CultureInfo.InvariantCulture), 
			       Decimal.Parse(".2352523523423422342354395033", CultureInfo.InvariantCulture),
			       Decimal.Parse("46161363632634613634.093453337", CultureInfo.InvariantCulture),
			       Decimal.Parse(".0000000000000345435353453563", CultureInfo.InvariantCulture)};
   Decimal[] dcmlRightValues = {Decimal.One, new Decimal(10), 
				Decimal.Parse("56033525474612414574574757495", CultureInfo.InvariantCulture),
				Decimal.Parse("461613636.32634613634083453337", CultureInfo.InvariantCulture),
				Decimal.Parse(".0000000000000023525235234234", CultureInfo.InvariantCulture)};
   Decimal[] dcmlExpValues = {Decimal.One, Decimal.MaxValue, 
			      Decimal.Parse("13182018677937129120135020796", CultureInfo.InvariantCulture),
			      Decimal.Parse("21308714924243214928823669051", CultureInfo.InvariantCulture),
			      Decimal.Parse(".0000000000000000000000000001", CultureInfo.InvariantCulture)};
   Decimal[] dcmlOverflowExceptionLeftValues = {Decimal.MaxValue, 
						Decimal.MinValue};
   Decimal[] dcmlOverflowExceptionRightValues = {Decimal.MinValue, 
						 Decimal.Parse("1.1", CultureInfo.InvariantCulture)};
   Decimal dcml1;
   Decimal dcmlExpValue;
   try
     {
     do
       {
       for (int aa = 0; aa < dcmlLeftValues.Length; aa++)
	 {
	 dcml1 = dcmlLeftValues[aa]*dcmlRightValues[aa];
	 ++iCountTestcases;
	 if ( dcml1 != dcmlExpValues[aa] )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr_" + aa + "  dcml1 ==" + dcml1  );
	   }
	 }
       dcml1 = (Decimal.Parse("5") + Decimal.Parse("10")) / (Decimal.Parse("2") - Decimal.Parse("5")) * (Decimal.Parse("3") + Decimal.Parse("18"))/Decimal.Parse("7");
       dcmlExpValue = Decimal.Parse("-15");
       ++iCountTestcases;
       if ( dcml1 != dcmlExpValue )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_346ew!  dcml1 == " + dcml1  );
	 }
       for (int aa = 0; aa < dcmlOverflowExceptionLeftValues.Length; aa++)
	 {
	 try
	   {
	   ++iCountTestcases;
	   dcml1 = dcmlOverflowExceptionLeftValues[aa] * dcmlOverflowExceptionRightValues[aa];
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_304sd_" + aa + "!  Exception not thrown"  );
	   }
	 catch (OverflowException)
	   {
	   }
	 catch (Exception ex)
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_306se_" + aa + "!  , Wrong Exception thrown == " + ex.ToString()  );
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
   Co3566operator_multiply oCbTest = new Co3566operator_multiply();
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
