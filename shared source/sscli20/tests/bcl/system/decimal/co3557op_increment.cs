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
public class Co3557operator_plusplus
{
 static String strName = "Decimal.++";
 static String strTest = "Co3557operator_plusplus.cs";
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
   Decimal[] dcmlFirstValues = {Decimal.One, Decimal.MinusOne, Decimal.Zero, 
				Decimal.MinValue, new Decimal(-1234.5678),
				new Decimal(0.0001), new Decimal(Int32.MaxValue),
				Decimal.Parse("79228162514264337593543950334", CultureInfo.InvariantCulture)};
   Decimal[] dcmlSecValues = {Decimal.One, Decimal.MinusOne, Decimal.Zero, 
			      Decimal.MinValue, new Decimal(-1234.5678),
			      new Decimal(0.0001), new Decimal(Int32.MaxValue),
			      Decimal.Parse("79228162514264337593543950334", CultureInfo.InvariantCulture)};
   Decimal[] dcmlExpValues = {new Decimal(2), Decimal.Zero, Decimal.One, 
			      Decimal.Parse("-79228162514264337593543950334"), Decimal.Parse("-1233.5678", CultureInfo.InvariantCulture),
			      new Decimal(1.0001),  Decimal.Parse("2147483648", CultureInfo.InvariantCulture),
			      Decimal.MaxValue};
   Decimal[] dcmlExceptions = {Decimal.MaxValue}; 
   try
     {
     do
       {
       for (int aa = 0; aa < dcmlFirstValues.Length; aa++)
	 {
	 dcml1 = dcmlFirstValues[aa]++;
	 ++iCountTestcases;
	 if ( dcml1 != dcmlSecValues[aa])
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr_" + aa + "  dcml1 ==" + dcml1 + "  dcmlSecValues[aa] ==" + dcmlSecValues[aa]  );
	   }
	 }
       for (int aa = 0; aa < dcmlSecValues.Length; aa++)
	 {
	 dcml1 = ++dcmlSecValues[aa];
	 ++iCountTestcases;
	 if ( dcml1 != dcmlExpValues[aa])
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_036we_" + aa + "  dcml1 ==" + dcml1  );
	   }
	 }
       for (int aa = 0; aa < dcmlExceptions.Length; aa++)
	 {
	 try
	   {
	   ++iCountTestcases;
	   dcml1 = ++dcmlExceptions[aa];
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
   Co3557operator_plusplus oCbTest = new Co3557operator_plusplus();
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
