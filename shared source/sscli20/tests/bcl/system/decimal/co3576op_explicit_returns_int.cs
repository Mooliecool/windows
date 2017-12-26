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
public class Co3576int_operator_narrow
{
 static String strName = "Decimal.$narrow";
 static String strTest = "Co3576int_operator_narrow.cs";
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
   Decimal cncy1;
   Decimal[] dcmlFirstValues = {Decimal.One, Decimal.MinusOne, new Decimal(-1234.5678), new Decimal(0.0001), Decimal.Zero}; 
   Int32[] in4FirstValues = {1, -1, -1234, 0, 0}; 
   Decimal[] dcmlSecondValues = {new Decimal(1234.567895969984), new Decimal(Int32.MinValue), 
				 new Decimal(Int16.MaxValue), new Decimal(Int16.MinValue)};
   Int32[] in4SecondValues = {1234, Int32.MinValue, 
			      Int16.MaxValue, Int16.MinValue};
   Decimal[] dcmlExceptions = {Decimal.MaxValue, Decimal.MinValue, Decimal.Parse("2147483648.58062856235"), Decimal.Parse("-2147483649.580623465230523")}; 
   Int32 in4a;
   Decimal dcml1;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       for (int aa = 0; aa < dcmlFirstValues.Length; aa++)
	 {
	 in4a = (int)dcmlFirstValues[aa];
	 ++iCountTestcases;
	 if ( in4a != in4FirstValues[aa] )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr_" + aa + "  in4a ==" + in4a  );
	   }
	 }
       cncy1 = new Decimal();
       for (int aa = 0; aa < dcmlSecondValues.Length; aa++)
	 {
	 in4a = (int)dcmlSecondValues[aa];
	 ++iCountTestcases;
	 if ( in4a != in4SecondValues[aa] )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_783sf_" + aa + "  in4a ==" + in4a + " dcmlSecondValues[aa] == " + dcmlSecondValues[aa] );
	   }
	 }
       for (int aa = 0; aa < dcmlExceptions.Length; aa++)
	 {
	 try
	   {
	   ++iCountTestcases;
	   in4a = (int)dcmlExceptions[aa];
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
   Co3576int_operator_narrow oCbTest = new Co3576int_operator_narrow();
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
