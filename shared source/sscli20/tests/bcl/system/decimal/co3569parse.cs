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
public class Co3569Parse
{
 static String strName = "Decimal.Parse";
 static String strTest = "Co3569Parse.cs";
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
   String[] strValues = {"123.45", "-68.7249", "0",  "2147483647", "-2147483648",
			 "79228162514264337593543950335", "-79228162514264337593543950335"};
   Decimal[] dcmlExpValues = {new Decimal(123.45), new Decimal(-68.7249), (Decimal)0, (Decimal)Int32.MaxValue, (Decimal)Int32.MinValue,
			      Decimal.MaxValue, Decimal.MinValue};
   String[] strValues2 = {"   -134.54928       ", "0.0", "                000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000025342342424.4567000000000000000000000000000000                              "}; 
   Decimal[] dcmlExpValues2 = {new Decimal(-134.54928), Decimal.Zero, Decimal.Parse("25342342424.4567")}; 
   String[] strExceptions = {"Hello World", Single.NaN.ToString(), "", "123-4564.456"}; 
   String strValue;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       for (int aa = 0; aa < strValues.Length; aa++)
	 {
	 dcml1 = Decimal.Parse(strValues[aa]);
	 ++iCountTestcases;
	 if ( dcml1 != dcmlExpValues[aa])
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr_" + aa + "  dcml1 ==" + dcml1  );
	   }
	 }
       for (int aa = 0; aa < strValues2.Length; aa++)
	 {
	 dcml1 = Decimal.Parse(strValues2[aa]);
	 ++iCountTestcases;
	 if ( dcml1 != dcmlExpValues2[aa])
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_764ao_" + aa + "  dcml1 ==" + dcml1  );
	   }
	 }
       for (int aa = 0; aa < strExceptions.Length; aa++)
	 {
	 try
	   {
	   ++iCountTestcases;
	   dcml1 = Decimal.Parse(strExceptions[aa]);
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_852sp_" + aa + "!  Exception not thrown"  );
	   }
	 catch (FormatException ex)
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
   Co3569Parse oCbTest = new Co3569Parse();
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
