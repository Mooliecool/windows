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
public class Co3573operator_minusminus
{
 static String strName = "Decimal.--";
 static String strTest = "Co3573operator_minusminus.cs";
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
   Decimal[] dcmlFirstValues = {Decimal.One, Decimal.MinusOne, Decimal.Zero, 
				Decimal.MaxValue, new Decimal(-1234.5678),
				new Decimal(0.0001), new Decimal(Int32.MaxValue),
				Decimal.Parse("-79228162514264337593543950334", CultureInfo.InvariantCulture)};
   Decimal[] dcmlSecValues = {Decimal.One, Decimal.MinusOne, Decimal.Zero, 
			      Decimal.MaxValue, new Decimal(-1234.5678),
			      new Decimal(0.0001), new Decimal(Int32.MaxValue),
			      Decimal.Parse("-79228162514264337593543950334")};
   Decimal[] dcmlExpValues = {Decimal.Zero, new Decimal(-2), Decimal.MinusOne, 
			      Decimal.Parse("79228162514264337593543950334"), Decimal.Parse("-1235.5678", CultureInfo.InvariantCulture),
			      new Decimal(-.9999),  Decimal.Parse("2147483646", CultureInfo.InvariantCulture),
			      Decimal.MinValue};
   Decimal[] dcmlExceptions = {Decimal.MinValue}; 
   Decimal dcml1;
   Decimal dcml2;
   Decimal dcmlExpValue;
   try
     {
     do
       {
       for (int aa = 0; aa < dcmlFirstValues.Length; aa++)
	 {
	 dcml1 = dcmlFirstValues[aa]--;
	 ++iCountTestcases;
	 if ( dcml1 != dcmlSecValues[aa])
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr_" + aa + "  dcml1 ==" + dcml1 + "  dcmlSecValues[aa] ==" + dcmlSecValues[aa]  );
	   }
	 }
       for (int aa = 0; aa < dcmlSecValues.Length; aa++)
	 {
	 dcml1 = --dcmlSecValues[aa];
	 ++iCountTestcases;
	 if ( dcml1 != dcmlExpValues[aa])
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_036we_" + aa + "  dcml1 ==" + dcml1  );
	   }
	 }
       dcml2 = new Decimal(3);
       dcml1 = new Decimal(19) - --dcml2 * (new Decimal(6) - new Decimal(2))/new Decimal(4) - 15;
       dcmlExpValue = new Decimal(2);
       ++iCountTestcases;
       if ( dcml1 != dcmlExpValue )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_243wr  dcml1 ==" + dcml1  );
	 }
       for (int aa = 0; aa < dcmlExceptions.Length; aa++)
	 {
	 try
	   {
	   ++iCountTestcases;
	   dcml1 = --dcmlExceptions[aa];
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
   Co3573operator_minusminus oCbTest = new Co3573operator_minusminus();
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
