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
public class Co3543operator_plus_decdec
{
 static String strName = "Decimal.Add";
 static String strTest = "Co3543operator_plus_decdec.cs";
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
   Decimal[] dcmlLeftValues = {Decimal.One, Decimal.MinusOne, Decimal.MaxValue, Decimal.MinValue, 
			       new Decimal(-1234.5678)};
   Decimal[] dcmlRightValues = {Decimal.One, Decimal.One, Decimal.MinValue, new Decimal(0.1), 
				new Decimal(-8765.43218)};
   Decimal[] dcmlExpValues = {new Decimal(2), Decimal.Zero, Decimal.Zero, Decimal.Parse("-79228162514264337593543950335"), 
			      new Decimal(-9999.99998)};
   Decimal[] dcmlExceptionLeftValues = {Decimal.MaxValue, Decimal.MinValue, Decimal.MaxValue, Decimal.MinValue}; 
   Decimal[] dcmlExceptionRightValues = {Decimal.MaxValue, Decimal.MinValue, new Decimal(0.50005), new Decimal(-0.5)}; 
   Decimal cncyValue;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       for (int aa = 0; aa < dcmlLeftValues.Length; aa++)
	 {
	 dcml1 = dcmlLeftValues[aa] + dcmlRightValues[aa];
	 ++iCountTestcases;
	 if ( dcml1 != dcmlExpValues[aa] )
	   {
	   ++iCountErrors;
	   Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr_" + aa + "  dcml1 ==" + dcml1  );
	   }
	 dcml1 = dcmlLeftValues[aa] + dcmlRightValues[aa];
	 }
       Decimal dcml2 = new Decimal(10);
       Decimal dcml3 = new Decimal(2);
       Decimal dcml4 = new Decimal(4);
       dcml1 = dcml4 + dcml2 / dcml3;
       ++iCountTestcases;
       if ( dcml1 != new Decimal(9) )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_374er!  dcml1 ==" + dcml1  );
	 }
       for (int aa = 0; aa < dcmlExceptionLeftValues.Length; aa++)
	 {
	 try
	   {
	   ++iCountTestcases;
	   dcml1 = dcmlExceptionLeftValues[aa] + dcmlExceptionRightValues[aa];
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
   Co3543operator_plus_decdec oCbTest = new Co3543operator_plus_decdec();
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
